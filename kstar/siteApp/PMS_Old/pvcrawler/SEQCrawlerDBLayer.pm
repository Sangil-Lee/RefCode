
package SEQCrawlerDBLayer;

# SEQCrawlerDBLayer.pm
# 
#   This package provides routines for writing Sequence file name, IOC, and PV occurance
#   data to a database. 
#
# Author: Jan. 1. 2006 Ron MacKenzie (ronm@slac.stanford.edu).
#
# Modifications:
#
#   10/23/07  Mariana Varotto (mvarotto@aps.anl.gov)
#
#             Fixed segment of code that clears current_load flag
#             Added code to check whether a (potential) record discovered in a seq program 
#              does exist

use DBI qw(:sql_types);
require Exporter;
use POSIX;
use LogUtil;
use Blowfish_PP;
use File::stat;

our @ISA = qw(Exporter);
our @EXPORT_OK = qw();
our $VERSION = 1.00;

our $ALH = 2;       # for writing to rec_client.rec_client_type_id
our $SEQ = 4;       # Mike Zelazny reserved this (SEQ=4) with Claude.

my $test_mode = 0;  # if 1, do no db access
my $home_dir = "."; # dir from which parent crawler is run
my $db_properties_path = "db.properties";
my $dbh;
my $db_vendor; # established on first db_connect
my $decryption_key; # optional 16 byte blowfish key

#***********
# Public sub
#***********
sub set_test_mode {
    $test_mode = $_[0];
}

# ***********
# Public sub
# ***********
# Sets the directory where db.properties file can be found. 
sub set_home_dir {
    $home_dir = $_[0];
    $db_properties_path = $home_dir . "/db.properties";
    log('info',"SEQ_Cr Set Sequence properties file to  $db_properties_path");
}

# **********
# Public sub
# **********
# Sets the 16 char blowfish decryption key. If this is defined, this module
# will assume the db.properties file is encrypted with this key.
sub set_decryption_key {
    $decryption_key = $_[0];
}

#***********
# Public sub
#***********
# Store data in the database.
sub update_db_tables {
    my ($ioc_nm) = @_;

    LogUtil::log('debug',"SEQCrawlerDBLayer: update_db_tables entered");

    if ($test_mode) {
       return;
    }

    if (!$dbh) {
        $dbh = db_connect();
        if (!$dbh) {
            LogUtil::log('info',"SEQCrawlerDBLayer: Cannot connect to DB");
            exit;
        }
    }

    # This subr is called once per IOC.
    #
    # The data to be stored (as a result of the current crawl of the IOC) is in 
    # the @seq_hits array.  Before we store it in the database, let's set 
    # rec_client::current_load to zero for any previous crawls of this IOC
    # by calling mark_seq_records_as_old().

    mark_seq_records_as_old($ioc_nm);    

    add_new_seq_records();

    $dbh->commit; 

    LogUtil::log('info',"SEQCrawlerDBLayer: update_db_tables finished.");
    return;
}

#************
# Private sub 
#
# Mark sequence records in rec_client as old by setting rec_client::current_load=0
# ***********
sub mark_seq_records_as_old {
    my ($ioc_name) = @_;

    if ($test_mode) {
        return;
    }
    if (!$dbh) {
        $dbh = db_connect();
    }

    LogUtil::log('debug',"Entering SEQCrawlerDBLayer::mark_records_as_old for ioc  $ioc_name  ");

    # Set rec_client::current_load = 0 for any records associated with the
    # current IOC.  To do that, look into vuri_rel for rows for the current IOC with the 
    # same pointer to vuri (child_vuri_id) as the rec_client has (in vuri_id).
    my $update_rec_client_sql;
    my $select_rec_client_sql = 'select rec_client_id from rec_client, vuri_rel '
             ."where rec_client_type_id = $SEQ and rel_info like '$ioc_name %' and vuri_id = child_vuri_id and current_load='1'";
    my $select_rec_client_st;
    $select_rec_client_st = $dbh->prepare($select_rec_client_sql);
    $select_rec_client_st->execute();
    if ($select_rec_client_st){
        while (my $rec_client_row = $select_rec_client_st->fetchrow_hashref) {
            my $rec_client_id_old = $rec_client_row->{rec_client_id};
            # LogUtil::log('debug',"SEQCrawlerDBLayer: mark_seq_records_as_old(): marking rec_client_id $rec_client_id_old as old");
            if ($rec_client_id_old) {
                $update_rec_client_sql = 'update rec_client set current_load=0 '
                   ."where rec_client_id = $rec_client_id_old";
                my $update_rec_client_st;
                $update_rec_client_st = $dbh->prepare($update_rec_client_sql);
                $update_rec_client_st->execute();
           
                if ($@) {
                    $dbh->rollback;
                    LogUtil::log('error',"ERROR: mark_seq_records_as_old():$@");
                    return;
                };
            };
        }; 
    };
 
    LogUtil::log('debug',"SEQCrawlerDBLayer::mark_seq_records_as_old finished... success");
    return;
};


#***********************************
# Private subr:  add_new_seq_records
#***********************************
sub add_new_seq_records {
    my ($ioc_name) = @_;

    my $seq_hits_ref = SEQCrawlerParser::get_seq_hits;
    my $ioc_nm = '';
    if (defined $$seq_hits_ref[0][2]) {
      $ioc_nm = $$seq_hits_ref[0][2];
    }

    if ($ioc_nm eq '') {
        LogUtil::log('info', "No sequences found for this IOC"); 
	LogUtil::log('debug',"SEQCrawlerDBLayer::add_new_seq_records exiting due to no seq_hits");
        return;
    }
    else {
      LogUtil::log('debug',"Entering SEQCrawlerDBLayer::add_new_seq_records for ioc  $ioc_nm  ");
    }

    if (!$dbh) {
        $dbh = db_connect();
    }

    LogUtil::log('debug',"SEQCr Adding uri, and  rec_client records\n");

    my $uri_id  = -1;
    my $vuri_id = -1;

    # ********************************************************************************
    # Loop over array items, writing to DB tables: uri, vuri, vuri_rel, and rec_client
    # ********************************************************************************

    foreach $row (@$seq_hits_ref) {
        
        # rows in 2D array seq_hits were added using following line
        #     push (@seq_hits, [$dot_oh_file, \@seq_pv_list, $rel_info]);

        my $filename    = $row->[0];
        my $pv_list_ref = $row->[1];
        my $rel_info    = $row->[2];

        #my ($ioc_nm) = split(/ /,$rel_info);
        #
        # print "SEQa YYY ARRAY FILENAME IS **************** = $filename \n";
        # print "SEQa YYY IOC NAME IS ********************** = $ioc_nm \n";
                       # two different ways to use pointer
                       # $_->[1] is the array pointer.  
                       #   which is dereferenced by ->[0] for array element.
        # print "SEQa YYY PV 0 is ************************** =  $pv_list_ref->[0] \n";
        # print "SEQa YYY PV 1 is ************************** =  $row->[1]->[1] \n";

        # my $last_ix = @$pv_list_ref;    # @ works differently with a reference.
        # print ("SEQb last_ix is $last_ix \n");

        # for ($ii=0; $ii<$last_ix; $ii++) {
	#   print "SEQb YYY PV number $ii is $pv_list_ref->[$ii] \n";
	# }

        eval {                                      # try/catch.

            # *************************
            # Write a row to  URI table
            # *************************

            $uri_id = pre_insert_get_id($dbh, "uri");

            # SQL for writing out a row of the URI table
            my $insert_uri_sql = 'insert into uri ' .
                    '(uri_id, uri, uri_modified_date, modified_date, modified_by) ' .
                    'values (?, ?, ?, ' . get_current_date_time() . ', \'seqcrawler\')';
            my $insert_uri_st = $dbh->prepare($insert_uri_sql);
    
            # bind and execute to write out uri row
            if ($uri_id) { $insert_uri_st->bind_param(1, $uri_id, SQL_INTEGER); }
            $insert_uri_st->bind_param(2, $filename, SQL_VARCHAR);
	    eval { $mtime = stat($filename)->mtime; };
            if ($@) {
                 $insert_uri_st->bind_param(3, get_null_timestamp(), SQL_TIMESTAMP);
            } else {
                 $insert_uri_st->bind_param(3, format_date($mtime), SQL_TIMESTAMP);
            }
            $insert_uri_st->execute();
    
            $uri_id = post_insert_get_id($dbh, "uri");

            $prevFilename = $filename;

            LogUtil::log('debug',"SEQCr URI table insert: uri_id = $uri_id, filename = $filename "); 

            # ***********************************************
            # WRITE A ROW INTO VURI TABLE
            # VURI is just a pointer record pointing to URI 
            #      with one-to-one correspondance            
            # ***********************************************

            $vuri_id = pre_insert_get_id($dbh, "vuri");

            # SQL for writing out a row of the vuri table
            my $insert_vuri_sql = 'insert into vuri ' .
                             '(vuri_id, uri_id) ' .
                             'values (?, ?)';
            my $insert_vuri_st = $dbh->prepare($insert_vuri_sql);
    
            # bind and execute to write out vuri row
            if ($vuri_id) { $insert_vuri_st->bind_param(1, $vuri_id, SQL_INTEGER); }
            $insert_vuri_st->bind_param(2, $uri_id, SQL_INTEGER);
            $insert_vuri_st->execute();
   
            $vuri_id = post_insert_get_id($dbh, "vuri");

            LogUtil::log('debug',"SEQCr VURI table insert: vuri_id = $vuri_id uri_id = $uri_id"); 

            # *************************
            # WRITE A ROW INTO VURI_REL
            # *************************
            # 
            # This subr is entered once for each startup file.  Therefore once for each IOC.
            # But, we want to write a vuri_rel row for each uri row.  That way, we know
            #   the ioc associated with each sequence file.
            # So, we will have lots of duplicate IOC names in vuri_rel but that will
            #     make cleanup easier in some cases.

            my $vuri_rel_id = pre_insert_get_id($dbh, "vuri_rel");

            # SQL for writing out a row of the vuri_rel table
            my $insert_vuri_rel_sql = 'insert into vuri_rel ' .
                    '(vuri_rel_id, parent_vuri_id, child_vuri_id, rel_info) ' .
                    'values (?, null, ?, ?)';
            my $insert_vuri_rel_st = $dbh->prepare($insert_vuri_rel_sql);

            log('debug',"SEQCr Writing to vuri_rel: rel_info = $rel_info");

            # bind and execute to write out vuri_rel row
            if ($vuri_rel_id) { $insert_vuri_rel_st->bind_param(1, $vuri_rel_id, SQL_INTEGER); }
            $insert_vuri_rel_st->bind_param(2, $vuri_id, SQL_INTEGER);
            $insert_vuri_rel_st->bind_param(3, $rel_info, SQL_VARCHAR);  # "IOC Name" space "SEQ Command"
            $insert_vuri_rel_st->execute();

            $vuri_rel_id = post_insert_get_id($dbh, "vuri_rel");
            LogUtil::log('debug',"SEQCr VURI_REL table insert: vuri_rel_id = $vuri_rel_id, vuri_id = $vuri_id, rel_info = $rel_info "); 

            if (scalar(@$pv_list_ref)) { 
                
                # ***********************************************
                # WRITE A ROW INTO REC_CLIENT
                # REC_CLIENT holds PV name and points to VURI.
                #       There  is one REC_CLIENT per PV name.  
                # ***********************************************

                my $last_ix = @$pv_list_ref;    # @ works differently with a reference.
                # print ("SEQb last_ix is $last_ix \n");

                for ($ii=0; $ii<$last_ix; $ii++) {
	            # print "SEQb PV number $ii is $pv_list_ref->[$ii] \n";
	            $pv_name =  $pv_list_ref->[$ii];
	            next if $pv_name eq '';
                    # print "SEQc YYY PV number $ii is  $pv_name \n";
                    my ($rec_name,$fld_name) = split (/\./,$pv_name,2); # dot seperates PV and FIELD
                    unless (defined $fld_name) {$fld_name = "VAL";}
                    #
                    my $rec_client_id = pre_insert_get_id($dbh, "rec_client");
                    #
		    # CHECK NOW IF REC EXIST
		    
		    my $select_rec_nm_sql = "select rec_nm from rec where rec_nm = '$rec_name'";
                    my $select_rec_nm_st;
                    $select_rec_nm_st = $dbh->prepare($select_rec_nm_sql);
                    $select_rec_nm_st->execute();
                    if ($select_rec_nm_st){
                        my $rec_nm_row = $select_rec_nm_st->fetchrow_hashref; # rec exists -- ASSUMING THERE'S ONLY ONE
                        my $rec_nm = $rec_nm_row->{rec_nm};
                        # LogUtil::log('debug',"SEQCr rec_nm found in db = $rec_nm"); 
                        # LogUtil::log('debug',"SEQCr rec_nm from seq = $rec_name"); 
			if ($rec_nm) {    		                     # Insert only if rec exists
                            if ($rec_nm eq $rec_name)
			    {
                                my $rec_client_id = pre_insert_get_id($dbh, "rec_client");
                                #
                                # SQL for writing out a row of the rec_client table
                                my $insert_rec_client_sql = 'insert into rec_client ' .
                                      '(rec_client_id, rec_client_type_id, rec_nm, fld_type, '.
                                      'vuri_id, current_load) ';
                                $insert_rec_client_sql = $insert_rec_client_sql .
                                                 "values (?, $SEQ, ?, ?, ?, 1)";
                                my $insert_rec_client_st = $dbh->prepare($insert_rec_client_sql);
                                #
                                # bind and execute to write out rec_client row
                                if ($rec_client_id) { $insert_rec_client_st->bind_param(1, $rec_client_id, SQL_INTEGER); }
                                $insert_rec_client_st->bind_param(2, $rec_name , SQL_VARCHAR);
                                $insert_rec_client_st->bind_param(3, $fld_name, SQL_VARCHAR);
                                $insert_rec_client_st->bind_param(4, $vuri_id, SQL_INTEGER);
                                $insert_rec_client_st->execute();        
                            }
			    else {  
			        LogUtil::log('warn',"SEQ: Record $rec_name not found. $filename $rel_info");
			    }
			}
                    }
	        }   # end for
            }        # end if scalar  
        };           # end eval (try/catch...)
        if ($@) {
            $dbh->rollback;
            LogUtil::log('error',"SEQCr ERROR update_DB():$@");
            exit;
        }
        else {
          LogUtil::log('debug',"SEQCr Database written successfully. Commit not done yet."); 
	}

    }              # end for each row.
    return;
}


#************
# Private sub
#************
# Takes seconds after epoch and converts to string with "YYYY-MM-DD HH:MM:SS"
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

#************
# Private sub
#************
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

# *************************
# Very odd perl autoloading ...
# **************************
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
# JROCK: FIND THIS IN @INC INSTEAD
# require "$db_functions_module.pm";

    my $subroutine = "$db_functions_module"."::".$call_signature;
    goto &$subroutine;
}
1;
