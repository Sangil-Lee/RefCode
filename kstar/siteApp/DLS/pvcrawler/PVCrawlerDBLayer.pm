
package PVCrawlerDBLayer;

# This package provides subroutines for writing out crawler data to
# a relational database. Vendor specific code is factored out into
# perl modules XXXDBFunctions.pm, so this module should remain
# vendor agnostic.

use File::Basename; 
use DBI qw(:sql_types);
require Exporter;
use POSIX;
use LogUtil;
use Blowfish_PP;

our @ISA = qw(Exporter);
our @EXPORT = qw(all_save ioc_boot_save ioc_boot_record_load ioc_find find_ioc_resource_and_uri ioc_copy set_test_mode set_home_dir set_decryption_key);
our @EXPORT_OK = qw();
our $VERSION = 1.00;

my $test_mode = 0;  # if 1, do no db access
my $home_dir = ".";  # home dir from which parent crawler is run
my $db_properties_path = "db.properties";
my $dbh;
my $db_vendor; # established on first db_connect
my $decryption_key; # optional 16 byte blowfish key

# Public
sub set_test_mode {
    $test_mode = $_[0];
}

# Public
# Sets the directory where db.properties file can be found. 
sub set_home_dir {
    $home_dir = $_[0];

    $db_properties_path = $home_dir . "/db.properties";
}

# Public
# Sets the 16 char blowfish decryption key. If this is defined, this module
# will assume the db.properties file is encrypted with this key.
sub set_decryption_key {
    $decryption_key = $_[0];
}

# Public
sub all_save {
    #
    # arguments to subroutine
    #

    # ioc name
    my $ioc_boot_info = $_[0];

    # results of st.cmd file parse
    my $parse_err = $_[1];

    # flag indicating whether an ioc resource has changed
    my $ioc_resource_changed = $_[2];

    # array reference to rnam_fldn
    my $rnam_fldn = $_[3];

    # array ref to file_list (array of hashes, each with id, path, subst keys)
    my $file_list = $_[4];

    # array ref to dbd
    my $dbd = $_[5];

    # array ref to dev_sup (array of hashes)
    my $dev_sup = $_[6];

    if ($test_mode) {
        return;
    }

    #
    # locals
    #
    LogUtil::log('debug',"PVCrawlerDBLayer: begin all_save");

    if (!$dbh) {
        $dbh = db_connect();
    }

    $ioc_boot_info->{ioc_boot_id} = 
        ioc_boot_save($ioc_boot_info, $parse_err, $ioc_resource_changed);

    # if a dbd or db file has changed, write out info on resources
    if ($ioc_resource_changed && !$parse_err) {

        # ioc_resource_ids array indexed by file_index in dbd
        my @ioc_resource_ids = 
            ioc_resource_and_uri_save($dbh, $ioc_boot_info, $file_list);


        # if no parse errors detected, write pv data out
        if (!$parse_err) {
            
            LogUtil::log('debug',"PVCrawlerDBLayer: save all pv data");
            # rec_type_ids hash keyed by rec_typ
            my %rec_type_ids = 
                rec_type_save($dbh, $ioc_boot_info, \@ioc_resource_ids, $dbd);
            
            fld_type_save($dbh, \%rec_type_ids, $dbd);
            
            rec_and_fld_save($dbh, $dbd, $ioc_boot_info, 
                             \@ioc_resource_ids, \%rec_type_ids, $rnam_fldn);

		# shbaek commented out next line for irmis3. [2009.12.09]
            dev_sup_save($dbh, $dev_sup, \%rec_type_ids);
        } 

    }

    LogUtil::log('debug',"PVCrawlerDBLayer: commit changes");
    $dbh->commit;
    return;

}

# Load all active ioc's from ioc table. Returns array of ioc_nm.
# Public
sub ioc_find {

    my @ioc_names = ();

    LogUtil::log('debug',"PVCrawlerDBLayer: begin ioc_find");

    if (!$dbh) {
        $dbh = db_connect();
    }

    my $select_ioc_sql = 'select ioc_nm from ioc where active=1';
    my $select_ioc_st;    
    eval {
        $select_ioc_st = $dbh->prepare($select_ioc_sql);
        $select_ioc_st->execute();
    };
    if ($@) {
        LogUtil::log('error',"ioc_find():$@");
        exit;
    }

    my $ioc_row;
    while ($ioc_row = $select_ioc_st->fetchrow_hashref) {
        $ioc_nm = $ioc_row->{ioc_nm};
        push @ioc_names, "$ioc_nm";
    }     

    return \@ioc_names;
}

# Load a single ioc record given the ioc name. Returns ioc_id.
# Private
sub ioc_load_by_name {
    my $ioc_nm = $_[0];

    my $ioc_id = 0;

    LogUtil::log('debug',"PVCrawlerDBLayer: begin ioc_load_by_name -> $ioc_nm");

    if (!$dbh) {
        $dbh = db_connect();
    }

    my $select_ioc_sql = 'select ioc_id from ioc where ioc_nm = ?';
    my $select_ioc_st;    
    eval {
        $select_ioc_st = $dbh->prepare($select_ioc_sql);
        $select_ioc_st->bind_param(1,$ioc_nm,SQL_VARCHAR);
        $select_ioc_st->execute();
    };
    if ($@) {
        LogUtil::log('error',"ioc_load_by_name():$@");
        exit;
    }

    if (my $ioc_row = $select_ioc_st->fetchrow_hashref) {
        $ioc_id = $ioc_row->{ioc_id};
    } 

    return $ioc_id;
}

# Public
sub ioc_boot_save {
    my $ioc_boot_info = $_[0];
    my $parse_err = $_[1];
    my $ioc_resource_changed = $_[2];

    my $ioc_nm = $ioc_boot_info->{ioc_nm};
    my $st_cmd_dir = $ioc_boot_info->{st_cmd_dir};
    my $st_cmd_file = $ioc_boot_info->{st_cmd_file};
    my $boot_date = $ioc_boot_info->{boot_date};
    my $ioc_booted = $ioc_boot_info->{booted};

    my $ioc_boot_id;
    my $current_boot = 1;
    my $current_load = 1;

    if ($test_mode) {
        return;
    }

    LogUtil::log('debug',"PVCrawlerDBLayer: begin ioc_boot_save");

    if (!$dbh) {
        $dbh = db_connect();
    }

    # We still want to log ioc boot info, even if we aren't saving all
    #   the pv info. But don't do it repeatedly when the same error
    #   condition has occurred.
    if (!$ioc_booted || !$ioc_resource_changed || $parse_err) {
        $current_load = 0; # we aren't doing a pv load

        # Check our last record of boot from db
        my $last_boot_record = ioc_boot_record_load($ioc_boot_info,0);
        if ($last_boot_record->{ioc_boot_id} != 0) {
            # If we have a prior boot record, and it's error num matches
            #   our current error condition, don't insert another boot record.
            if (!$ioc_booted && ($last_boot_record->{ioc_error_num} == 3)) {
                return;
            }
            if ($parse_err && ($parse_err == $last_boot_record->{ioc_error_num})) {
                return;
            }
        }
    }

    # Get ioc_id from ioc table.
    my $ioc_id = ioc_load_by_name($ioc_nm);

    eval {
        # Update any previos ioc_boot record(s), marking them "past-tense"
        my $update_ioc_boot_sql;
        if ($current_load) {
            $update_ioc_boot_sql = 'update ioc_boot set current_boot=0, ' .
                'current_load=0, ioc_boot_date=ioc_boot_date where ioc_id=?';
        } else {
            $update_ioc_boot_sql = 'update ioc_boot set current_boot=0, ' .
                'ioc_boot_date=ioc_boot_date where ioc_id=?';
        }
        my $update_ioc_boot_st;
        $update_ioc_boot_st = $dbh->prepare($update_ioc_boot_sql);
        if ($ioc_id) { $update_ioc_boot_st->bind_param(1, $ioc_id, SQL_INTEGER); }
        $update_ioc_boot_st->execute();
        
        # Insert new row and get id
        $ioc_boot_id = pre_insert_get_id($dbh, "ioc_boot");

        my $insert_ioc_boot_sql = 'insert into ioc_boot ' .
            '(ioc_boot_id, ioc_id, sys_boot_line, ioc_boot_date, ' .
            'current_load, current_boot, modified_date, modified_by) ' .
            'values (?, ?, ?, ?, ?, ?, '. get_current_date_time() . ', \'pvcrawler\')';

        my $insert_ioc_boot_st;
        log('debug',"insert new ioc_boot record");
        $insert_ioc_boot_st = $dbh->prepare($insert_ioc_boot_sql);
        if ($ioc_boot_id) { $insert_ioc_boot_st->bind_param(1, $ioc_boot_id, SQL_INTEGER); }
        $insert_ioc_boot_st->bind_param(2, $ioc_id, SQL_INTEGER);
        if ($ioc_booted) {
            $insert_ioc_boot_st->bind_param(3, $st_cmd_dir . "/" . $st_cmd_file, SQL_VARCHAR);
        } else {
            $insert_ioc_boot_st->bind_param(3, "", SQL_VARCHAR);
        }
        $insert_ioc_boot_st->bind_param(4, format_date($boot_date), SQL_TIMESTAMP);
        $insert_ioc_boot_st->bind_param(5, $current_load, SQL_INTEGER);
        $insert_ioc_boot_st->bind_param(6, $current_boot, SQL_INTEGER);

        $insert_ioc_boot_st->execute();
    };
    if ($@) {
        $dbh->rollback;
        LogUtil::log('error',"ioc_boot_save():$@");
        exit;
    }

    $ioc_boot_id = post_insert_get_id($dbh, 'ioc_boot');

    log('debug',"inserted new ioc_boot record with id $ioc_boot_id");

    # Check to see if we need to also insert an ioc_error record
    my $error_num = 0;
    if ($parse_err) {
        $error_num = $parse_err;
    } elsif (!$ioc_booted) {
        $error_num = 3;
    }
    if ($error_num) {
        ioc_error_save($ioc_boot_id, $error_num);
    }

    $dbh->commit;

    return $ioc_boot_id;
}

# Save a record to ioc_error table
# Private
sub ioc_error_save {
    my $ioc_boot_id = $_[0];
    my $ioc_error_num = $_[1];

    LogUtil::log('debug',"PVCrawlerDBLayer: begin ioc_error_save");

    if (!$dbh) {
        $dbh = db_connect();
    }
    # Insert new row
    my $ioc_error_id = pre_insert_get_id($dbh, "ioc_error");

    my $insert_ioc_error_sql = 'insert into ioc_error ' .
        '(ioc_error_id, ioc_boot_id, ioc_error_num) ' .
        'values (?, ?, ?)';

    my $insert_ioc_error_st;
    eval {
        $insert_ioc_error_st = $dbh->prepare($insert_ioc_error_sql);
        if ($ioc_error_id) { $insert_ioc_error_st->bind_param(1, $ioc_error_id, SQL_INTEGER); }
        $insert_ioc_error_st->bind_param(2, $ioc_boot_id, SQL_INTEGER);
        $insert_ioc_error_st->bind_param(3, $ioc_error_num, SQL_INTEGER);
        $insert_ioc_error_st->execute();
    };
    if ($@) {
        $dbh->rollback;
        LogUtil::log('error',"ioc_error_save():$@");
        exit;
    }
    return;
}

# Find latest ioc_boot record in db for given ioc_nm. Also gets any
# possible error code from the ioc_error companion table. The second
# argument specifies whether to look for the latest boot record where
# current_boot = 1, or whether to look for current_load = 1
# Public
sub ioc_boot_record_load {
    my $ioc_boot_info = $_[0];
    my $current_load_flag = $_[1];

    my $ioc_nm = $ioc_boot_info->{ioc_nm};

    my $ioc_boot_id = 0;
    my $ioc_boot_date = 0;
    my $load_performed = 0;

    LogUtil::log('debug',"PVCrawlerDBLayer: begin ioc_boot_record_load");

    if (!$dbh) {
        $dbh = db_connect();
    }

    # Get latest boot record for this ioc (if any)
    my $select_ioc_boot_sql;
    my $db_vendor_format_date = get_format_date_sql('ioc_boot.ioc_boot_date');
    if ($current_load_flag) {
        $select_ioc_boot_sql = 'select ioc_boot.ioc_boot_id, ' .
            $db_vendor_format_date . ' ioc_boot_date, ' .
            'ioc_boot.current_load ' .
            'from ioc_boot, ioc where ioc.ioc_id = ioc_boot.ioc_id and ' .
            'ioc.ioc_nm = ? and ioc_boot.current_load = 1';
    } else {
        $select_ioc_boot_sql = 'select ioc_boot.ioc_boot_id, ' .
            $db_vendor_format_date . ' ioc_boot_date, ' .
            'ioc_boot.current_load ' .
            'from ioc_boot, ioc where ioc.ioc_id = ioc_boot.ioc_id and ' .
            'ioc.ioc_nm = ? and ioc_boot.current_boot = 1';
    }

    my $select_ioc_boot_st;
    eval {
        $select_ioc_boot_st = $dbh->prepare($select_ioc_boot_sql);
        $select_ioc_boot_st->bind_param(1, $ioc_nm, SQL_VARCHAR);
        $select_ioc_boot_st->execute();
    };
    if ($@) {
        LogUtil::log('error',"ioc_boot_record_load():$@");
        exit;
    }
    
    if (my $ioc_boot_row = $select_ioc_boot_st->fetchrow_hashref) {
        $ioc_boot_id = $ioc_boot_row->{ioc_boot_id};
        $ioc_boot_date = $ioc_boot_row->{ioc_boot_date};
        $load_performed = $ioc_boot_row->{current_load};
    }

    # Look for any ioc_error record
    my $select_ioc_error_sql = 'select ioc_error_num from ioc_error where ' .
        'ioc_boot_id = ?';

    my $select_ioc_error_st;
    eval {
        $select_ioc_error_st = $dbh->prepare($select_ioc_error_sql);
        $select_ioc_error_st->bind_param(1, $ioc_boot_id, SQL_INTEGER);
        $select_ioc_error_st->execute();
    };
    if ($@) {
        LogUtil::log('error',"ioc_boot_record_load():$@");
        exit;
    }
    my $ioc_error_num = 0;
    if (my $ioc_error_row = $select_ioc_error_st->fetchrow_hashref) {
        $ioc_error_num = $ioc_error_row->{ioc_error_num};
    }

    my %ioc_boot_record = (ioc_boot_id=>"$ioc_boot_id", ioc_boot_date=>"$ioc_boot_date",
                           load_performed=>"$load_performed", ioc_error_num=>$ioc_error_num);

    return \%ioc_boot_record;
}

# Private
sub ioc_resource_and_uri_save {
    my $dbh = $_[0];
    my $ioc_boot_info = $_[1];
    my $file_list = $_[2];

    my $ioc_boot_id = $ioc_boot_info->{ioc_boot_id};

    my @ioc_resource_ids = ();  # hold onto assigned ioc_resource_id

    LogUtil::log('debug',"PVCrawlerDBLayer: begin ioc_resource_and_uri_save");

    eval {
        # SQL for writing out a row of the URI table
        my $insert_uri_sql = 'insert into uri ' .
            '(uri_id, uri, uri_modified_date, modified_date, ' .
            'modified_by) ' .
            'values (?, ?, ?, ' . get_current_date_time() . ', \'pvcrawler\')';
        my $insert_uri_st = $dbh->prepare($insert_uri_sql);
        
        # SQL for writing out a row of the ioc_resource table
        my $insert_ioc_resource_sql = 'insert into ioc_resource ' .
            '(ioc_resource_id, ioc_boot_id, text_line, load_order, ' .
            'uri_id, unreachable, subst_str, ioc_resource_type_id) ' .
            'values (?, ?, ?, ?, ?, ?, ?, ?)';
        my $insert_ioc_resource_st = $dbh->prepare($insert_ioc_resource_sql);
        
        # Loop over files, writing out URI and ioc_resource row
        my $count = 0;
        my $last_uri_id;
        my $last_ioc_resource_id;
        foreach $file (@$file_list) {
            # bind and execute to write out uri row
            $last_uri_id = pre_insert_get_id($dbh, "uri");
            if ($last_uri_id) { $insert_uri_st->bind_param(1,  $last_uri_id, SQL_INTEGER); }
            $insert_uri_st->bind_param(2, $file->{path}, SQL_VARCHAR);
            if ($file->{mtime}) {
                $insert_uri_st->bind_param(3, format_date($file->{mtime}), SQL_TIMESTAMP);
            } else {
                $insert_uri_st->bind_param(3, get_null_timestamp(), SQL_TIMESTAMP);
            }
            $insert_uri_st->execute();
            
            # need to get the id given by the insert
            $last_uri_id = post_insert_get_id($dbh, 'uri');
            
            # bind and execute to write out ioc_resource row
            $last_ioc_resource_id = pre_insert_get_id($dbh, "ioc_resource");
            if ($last_ioc_resource_id) { 
                $insert_ioc_resource_st->bind_param(1, $last_ioc_resource_id, SQL_INTEGER);
            }
            $insert_ioc_resource_st->bind_param(2, $ioc_boot_id , SQL_INTEGER);
            $insert_ioc_resource_st->bind_param(3, "", SQL_VARCHAR);
            $insert_ioc_resource_st->bind_param(4, $count, SQL_INTEGER);
            $insert_ioc_resource_st->bind_param(5, $last_uri_id, SQL_INTEGER);
            if ($file->{mtime}) {
                $insert_ioc_resource_st->bind_param(6, 0, SQL_INTEGER);
            } else {
                $insert_ioc_resource_st->bind_param(6, 1, SQL_INTEGER);
            }
            $insert_ioc_resource_st->bind_param(7, $file->{subst}, SQL_VARCHAR);
            if ($file->{type} eq 'dbd') {
                $insert_ioc_resource_st->bind_param(8, 3, SQL_INTEGER);
            } elsif ($file->{type} eq 'db') {
                $insert_ioc_resource_st->bind_param(8, 1, SQL_INTEGER);
            } else {
                $insert_ioc_resource_st->bind_param(8, 2, SQL_INTEGER);
            }
            $insert_ioc_resource_st->execute();
            $last_ioc_resource_id = post_insert_get_id($dbh, 'ioc_resource');
            $ioc_resource_ids[$count] = $last_ioc_resource_id;
            $count++;
        }
    };
    if ($@) {
        $dbh->rollback;
        LogUtil::log('error',"ioc_resource_and_uri_save():$@");
        exit;
    }
    
    return @ioc_resource_ids;
}

# Public
sub find_ioc_resource_and_uri {
    my $ioc_boot_info = $_[0];

    LogUtil::log('debug',"PVCrawlerDBLayer: begin find_ioc_resource_and_uri");

    if (!$dbh) {
        $dbh = db_connect();
    }

    # Get the right ioc_boot record (must have current_load=1)
    my $ioc_boot_record = ioc_boot_record_load($ioc_boot_info,1);
    if ($ioc_boot_record->{ioc_boot_id} == 0) {  # not found
        return;
    }
    my $ioc_boot_id = $ioc_boot_record->{ioc_boot_id};

    my $db_vendor_format_date = get_format_date_sql('uri.uri_modified_date');
    my $select_ioc_resource_sql = 'select unreachable,uri, ' . $db_vendor_format_date . 
        ' uri_modified_date from ' .
        'ioc_resource,uri where ioc_resource.uri_id=uri.uri_id ' .
        'and ioc_resource.ioc_boot_id = ?';
    my $select_ioc_resource_st;    
    eval {
        $select_ioc_resource_st = $dbh->prepare($select_ioc_resource_sql);
        $select_ioc_resource_st->bind_param(1,$ioc_boot_id,SQL_INTEGER);
        $select_ioc_resource_st->execute();
    };
    if ($@) {
        LogUtil::log('error',"find_ioc_resource_and_uri():$@");
        exit;
    }

    my $ioc_resource_row;
    my @ioc_resources = ();
    my %ioc_resource;
    while ($ioc_resource_row = $select_ioc_resource_st->fetchrow_hashref) {
        $unreachable = $ioc_resource_row->{unreachable};
        $uri = $ioc_resource_row->{uri};
        $uri_modified_date = $ioc_resource_row->{uri_modified_date};
        $ioc_resource = {unreachable=>"$unreachable",uri=>"$uri",
                         uri_modified_date=>"$uri_modified_date"};
        push @ioc_resources, $ioc_resource;
    } 
    
    return \@ioc_resources;
}


# Writes out rec_type rows, returning array of assigned id's
# Private
sub rec_type_save {
    
    my $dbh = $_[0];
    my $ioc_boot_info = $_[1];
    my $ioc_resource_ids = $_[2];
    my $dbd = $_[3];

    my $ioc_boot_id = $ioc_boot_info->{ioc_boot_id};

    my %rec_type_ids = ();

    eval {
        # SQL for writing out a row of the rec_type table
        my $insert_rec_type_sql = 'insert into rec_type ' .
            '(rec_type_id, ioc_boot_id, rec_type, ioc_resource_id) ' .
            'values (?, ?, ?, ?)';
        my $insert_rec_type_st = $dbh->prepare($insert_rec_type_sql);    
        
        my $count = 0;
        my $last_rec_type_id;
        for $rec_typ (keys %$dbd) {
            $fld = @{$dbd->{$rec_typ}}[0]; # grab any field, say zero-th
            $ioc_resource_id = $$ioc_resource_ids[$fld->{file_index} - 1];

            $last_rec_type_id = pre_insert_get_id($dbh, "rec_type");
            if ($last_rec_type_id) { $insert_rec_type_st->bind_param(1, $last_rec_type_id, SQL_INTEGER); }
            $insert_rec_type_st->bind_param(2, $ioc_boot_id, SQL_INTEGER );
            $insert_rec_type_st->bind_param(3, $rec_typ, SQL_VARCHAR);
            $insert_rec_type_st->bind_param(4, $ioc_resource_id, SQL_INTEGER);
            $insert_rec_type_st->execute();
            
            # need to get id assigned by insert
            $last_rec_type_id = post_insert_get_id($dbh, 'rec_type');
            
            $rec_type_ids{$rec_typ} = $last_rec_type_id;
            $count++;
        }
    };
    if ($@) {
        $dbh->rollback;
        LogUtil::log('error',"rec_type_save():$@");
        exit;
    }
    return %rec_type_ids;
}

# Writes out fld_type rows (id's stored back in dbd hash)
# Private
sub fld_type_save {
    my $dbh = $_[0];
    my $rec_type_ids = $_[1];
    my $dbd = $_[2];

    eval {
        # SQL for writing out a row of the fld_type table
        my $insert_fld_type_sql = 'insert into fld_type ' .
            '(fld_type_id, rec_type_id, fld_type, dbd_type, def_fld_val) ' .
            'values (?, ?, ?, ?, ?)';
        my $insert_fld_type_st = $dbh->prepare($insert_fld_type_sql);    
        
        my $count = 0;
        my $last_fld_type_id;
        for $rec_typ (keys %$dbd) {
            $rec_type_fk_id = $rec_type_ids->{$rec_typ};        
            for $i (0 .. $#{$dbd->{$rec_typ}}) {
                $fld = @{$dbd->{$rec_typ}}[$i];
                $fld_type = $fld->{fldn};
                $dbd_type = $fld->{type};
                $def_fld_val = $fld->{def_val};

                $last_fld_type_id = pre_insert_get_id($dbh, "fld_type");                
                if ($last_fld_type_id) { $insert_fld_type_st->bind_param(1, $last_fld_type_id, SQL_INTEGER); }
                $insert_fld_type_st->bind_param(2, $rec_type_fk_id, SQL_INTEGER);
                $insert_fld_type_st->bind_param(3, $fld_type, SQL_VARCHAR);
                $insert_fld_type_st->bind_param(4, $dbd_type, SQL_VARCHAR);
                $insert_fld_type_st->bind_param(5, $def_fld_val, SQL_VARCHAR);
                $insert_fld_type_st->execute();
                
                # need to get id assigned by insert
                $last_fld_type_id = post_insert_get_id($dbh, 'fld_type');

                # note: need to set $fld->{id} here for use by rec_and_fld_save
                $fld->{id} = $last_fld_type_id;
                $count++;
            }
        }
    };
    if ($@) {
        $dbh->rollback;
        LogUtil::log('error',"fld_type_save():$@");
        exit;
    }
    return;
}

# Private
sub rec_and_fld_save {
    my $dbh = $_[0];
    my $dbd = $_[1];
    my $ioc_boot_info = $_[2];
    my $ioc_resource_ids = $_[3];
    my $rec_type_ids = $_[4];
    my $rnam_fldn = $_[5];

    my $ioc_boot_id = $ioc_boot_info->{ioc_boot_id};

    eval {
        # SQL for writing out a row of the rec table
        my $insert_rec_sql = 'insert into rec ' .
            '(rec_id, ioc_boot_id, rec_nm, rec_type_id) ' .
            'values (?, ?, ?, ?)';
        my $insert_rec_st = $dbh->prepare($insert_rec_sql);    
        
        # SQL for writing out a row of the fld table
        my $insert_fld_sql = 'insert into fld ' .
            '(fld_id, rec_id, fld_type_id, fld_val, ioc_resource_id) ' .
            'values (?, ?, ?, ?, ?)';
        my $insert_fld_st = $dbh->prepare($insert_fld_sql);    
        
        my $last_rec_nm = ""; 
        my $current_rec_nm = "";
        my $last_rec_id = 0;
        my $rec_count = 0;
        for $fld (@$rnam_fldn) {
            $current_rec_nm = $fld->{rec_nm};
            log('verbose',"fld: $fld->{rec_nm} $fld->{fld_nm}");

            # derive some fk id's
            
            # get dbd info for this instance's rec type (ie. array of fields)
            $dbd_flds = $dbd->{$fld->{rec_typ}}; # get dbd info for this instance's type
            
            # iterate over fairly small list to find matching field name
            $fld_type_fk_id = 0; # this will stay zero if no fields were defined for record
            for $dbd_fld (@{$dbd_flds}) {
                if ($dbd_fld->{fldn} eq $fld->{fld_nm}) {
                    $fld_type_fk_id = $dbd_fld->{id};
                }
            }
            
            # get actual db id from prior inserts using relative file_index
            $ioc_resource_fk_id = $$ioc_resource_ids[$fld->{file_index}];
            
            # If we hit another field in same record, just write out fld
            if ($current_rec_nm eq $last_rec_nm) {

                if ($fld_type_fk_id != 0) {
                    my $last_fld_id = pre_insert_get_id($dbh, "fld");
                    if ($last_fld_id) { $insert_fld_st->bind_param(1, $last_fld_id, SQL_INTEGER); }
                    $insert_fld_st->bind_param(2, $last_rec_id, SQL_INTEGER);
                    $insert_fld_st->bind_param(3, $fld_type_fk_id, SQL_INTEGER);
                    $insert_fld_st->bind_param(4, $fld->{fld_val}, SQL_VARCHAR);
                    $insert_fld_st->bind_param(5, $ioc_resource_fk_id, SQL_INTEGER);
                    log('verbose',"insert fld $last_rec_id $rec_count $fld_type_fk_id $ioc_resource_fk_id");
                    $insert_fld_st->execute();
                } else {
                    log('warn',"pv $fld->{rec_nm} field $fld->{fld_nm} referenced in db, but not part of rec def");
                }
                
            } else { # write out rec and fld
                
                $rec_type_fk_id = $rec_type_ids->{$fld->{rec_typ}};
                if (!$rec_type_fk_id) {
                    LogUtil::log('error',"rec_type_fk_id not found for rec_typ $fld->{rec_typ}")
                }
                $last_rec_id = pre_insert_get_id($dbh, "rec");
                if ($last_rec_id) { $insert_rec_st->bind_param(1, $last_rec_id, SQL_INTEGER); }
                $insert_rec_st->bind_param(2, $ioc_boot_id, SQL_INTEGER);
                $insert_rec_st->bind_param(3, $fld->{rec_nm}, SQL_VARCHAR);
                $insert_rec_st->bind_param(4, $rec_type_fk_id, SQL_INTEGER);
                LogUtil::log('verbose',"insert rec $fld->{rec_nm} rec_type_id $rec_type_fk_id");
                $insert_rec_st->execute();
                
                $last_rec_id = post_insert_get_id($dbh, 'rec');
                
                # if the fk_id here is zero, means we have rec def in db file, but no fields
                if ($fld_type_fk_id != 0) {
                    my $last_fld_id = pre_insert_get_id($dbh, "fld");
                    if ($last_fld_id) { $insert_fld_st->bind_param(1, $last_fld_id, SQL_INTEGER); }
                    $insert_fld_st->bind_param(2, $last_rec_id, SQL_INTEGER);
                    $insert_fld_st->bind_param(3, $fld_type_fk_id, SQL_INTEGER);
                    $insert_fld_st->bind_param(4, $fld->{fld_val}, SQL_VARCHAR);
                    $insert_fld_st->bind_param(5, $ioc_resource_fk_id, SQL_INTEGER);
                    log('verbose',"insert fld $last_rec_id $rec_count $fld_type_fk_id $ioc_resource_fk_id");
                    $insert_fld_st->execute();
                }
                if (($rec_count % 1000) == 0) {
                    log('debug',"writing pv $rec_count to db");
                }
                $rec_count++;
            }
            $last_rec_nm = $current_rec_nm;
        }
    };
    if ($@) {
        $dbh->rollback;
        LogUtil::log('error',"rec_and_fld_save():$@");
        exit;
    }
    return;
}


# Private
sub dev_sup_save {
    my $dbh = $_[0];
    my $dev_sup = $_[1];
    my $rec_type_ids = $_[2];

    LogUtil::log('debug',"PVCrawlerDBLayer: begin dev_sup_save");

    eval {
        # SQL for writing out a row of the URI table
        my $insert_rec_type_dev_sup_sql = 'insert into rec_type_dev_sup ' .
            '(rec_type_dev_sup_id, rec_type_id, dtyp_str, dev_sup_dset, ' .
            'dev_sup_io_type) ' .
            'values (?, ?, ?, ?, ?)';
        my $insert_rec_type_dev_sup_st = $dbh->prepare($insert_rec_type_dev_sup_sql);
        
        # Loop over dev_sup
        my $count = 0;
        my $last_rec_type_dev_sup_id;
        foreach $dev_entry (@$dev_sup) {
            my $rec_type_id = $rec_type_ids->{$dev_entry->{rec_typ}};
            if ($rec_type_id) {
                # bind and execute to write out uri row
                $last_rec_type_dev_sup_id = pre_insert_get_id($dbh, "rec_type_dev_sup");
                if ($last_rec_type_dev_sup_id) {
                    $insert_rec_type_dev_sup_st->bind_param(1,$last_rec_type_dev_sup_id, SQL_INTEGER);
                }
                $insert_rec_type_dev_sup_st->bind_param(2, $rec_type_id, SQL_INTEGER);
                $insert_rec_type_dev_sup_st->bind_param(3, $dev_entry->{dtyp_str}, SQL_VARCHAR);
                $insert_rec_type_dev_sup_st->bind_param(4, $dev_entry->{dev_sup_dset_nm}, SQL_VARCHAR);
                $insert_rec_type_dev_sup_st->bind_param(5, $dev_entry->{io_type}, SQL_VARCHAR);
                $insert_rec_type_dev_sup_st->execute();
                
                # need to get the id given by the insert
                $last_rec_type_dev_sup_id = post_insert_get_id($dbh, 'rec_type_dev_sup');
                
                $count++;

            } else {
                LogUtil::log('warn',"unresolvable rec_typ $dev_entry->{rec_typ} in device declaration");
            }
        }
    };
    if ($@) {
        $dbh->rollback;
        LogUtil::log('error',"dev_sup_save():$@");
        exit;
    }
    
    return;
}

# Takes seconds after epoch and converts to string with "YYYY-MM-DD HH:MM:SS"
# Private
sub format_date {
    my $secsAfter1970 = $_[0];

    if (!$secsAfter1970 || $secsAfter1970 == 0) {
        return get_null_timestamp();
    }

    my @tmArr = localtime($secsAfter1970);
    my $formatStr = '%Y-%m-%d %H:%M:%S';

    my $formatDate = POSIX::strftime($formatStr, $tmArr[0],$tmArr[1],$tmArr[2],
                                     $tmArr[3], $tmArr[4], $tmArr[5]);

    return $formatDate;
}

# Private
sub db_connect {

    #connect to database
    my $dbh;

    # Look up connection parameters in external db.properties file.
    # If a decryption key has been supplied, use blowfish algorithm
    # to decrypt the contents of the file.
    
    my $db_host;
    my $db_database;
    my $db_user;
    my $db_password;

    if (!-e $db_properties_path) {
        LogUtil::log('error',"Unable to find db.properties file");
        LogUtil::log('error',"Exiting...");
        exit;
    }

    # Read in contents of db.properties (decrypting if key defined)
    my $blowfish_object;
    if ($decryption_key) {
        my $packed_key = pack("a16",$decryption_key);
        $blowfish_object = Crypt::Blowfish_PP->new($packed_key);
        if (!$blowfish_object) {
            LogUtil::log('error',"The given blowfish key appears to be invalid");
            LogUtil::log('error',"Exiting...");
            exit;
        }
    }
    open(DBPROPS,$db_properties_path);
    my $inbuf;
    my $input_text;
    while (my $read_len = read(DBPROPS, $inbuf, 8)) {
        if ($blowfish_object) {
            my $decrypted_text = $blowfish_object->decrypt($inbuf);
            $input_text = $input_text . $decrypted_text;
        } else {
            $input_text = $input_text . $inbuf;
        }
    }
    close(DBPROPS);

    # Parse up the db.properties text
    my $props_line;
    while ($input_text =~ /^([^\n]*)\n(.*)/s) {
        $props_line = $1;
        $input_text = $2;
        if ($props_line =~ /(.*)=(.*)/) {
            $prop_name = $1;
            $prop_val = $2;
            if ($prop_name eq "db.host") {
                $db_host = $prop_val;
            } elsif ($prop_name eq "db.database") {
                $db_database = $prop_val;
            } elsif ($prop_name eq "db.user") {
                $db_user = $prop_val;
            } elsif ($prop_name eq "db.password") {
                $db_password = $prop_val;
            } elsif ($prop_name eq "db.vendor") {
                # db_vendor is package global
                $db_vendor = $prop_val;
            }
        }
    }
    if (!$db_host || !$db_database || !$db_user || !$db_password ||!$db_vendor) {
        LogUtil::log('error',"Unable to read db connection parameters from db.properties");
        LogUtil::log('error',"Exiting...");
        exit;
    }


    # Get db vendor-specific DBI connect string
    my $dsn = get_dbi_dsn($db_database, $db_host);

    eval {
        $dbh = DBI->connect($dsn,$db_user,$db_password,
                            {AutoCommit=>0, RaiseError=>1, PrintError=>0, 
                             FetchHashKeyName=>'NAME_lc'});
    };
    if ($@) {
        LogUtil::log('error',"Unable to connect to db $@");
        LogUtil::log('error',"Exiting...");
        exit;
    }
    # Do any db vendor-specific db session initialization
    initialize_session($dbh);

    return $dbh;
}

# Utility to copy ioc names from ioc_table to ioc
# Public
sub ioc_copy {

    if (!$dbh) {
        $dbh = db_connect();
    }

    # SQL to get set of known ioc names from ioc_table
    my $select_ioc_sql = 'select ioc_name from ioc_table';
    my $select_ioc_st;    
    eval {
        $select_ioc_st = $dbh->prepare($select_ioc_sql);
        $select_ioc_st->execute();
    };
    if ($@) {
        LogUtil::log('error',"ioc_copy():$@");
        exit;
    }

    # SQL for inserting row in ioc
    my $insert_ioc_sql = 'insert into ioc ' .
        '(ioc_id, ioc_nm, modified_date, modified_by) ' .
        'values (null, ?, ' . get_current_date_time() . ', \'pvcrawler\')';
    my $insert_ioc_st;

    eval {
        $insert_ioc_st = $dbh->prepare($insert_ioc_sql);    
        
        my $ioc_row;
        while ($ioc_row = $select_ioc_st->fetchrow_hashref) {
            $ioc_name = $ioc_row->{ioc_name};
            
            # insert into ioc table
            $insert_ioc_st->bind_param(1, $ioc_name, SQL_VARCHAR);
            $insert_ioc_st->execute();
        }
    };
    if ($@) {
        LogUtil::log('error',"ioc_copy():$@");
        exit;
    }
    $dbh->commit;
    return;
}

# Very odd perl autoloading ...
sub AUTOLOAD {

    # Get the core function being called
    my $call_signature;
    if ($AUTOLOAD =~ /.*::(.*)/) {
        $call_signature = $1;
    } else {
        $call_signature = $AUTOLOAD;
    }

    if ($call_signature ne 'pre_insert_get_id' &&
        $call_signature ne 'post_insert_get_id' &&
        $call_signature ne 'get_dbi_dsn' &&
        $call_signature ne 'get_null_timestamp' &&
        $call_signature ne 'get_current_date_time' &&
        $call_signature ne 'initialize_session' &&
        $call_signature ne 'get_format_date_sql') {
        warn "Attempt to call $call_signature, which is not defined!\n";
        return;
    }

    # Determine which vendor DBFunctions.pm module we need to load
    my $db_functions_module = $db_vendor . "DBFunctions";
    require "$home_dir/$db_functions_module.pm";

    my $subroutine = "$db_functions_module"."::".$call_signature;
    goto &$subroutine;
}

1;
