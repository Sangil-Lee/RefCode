#!/usr/bin/perl -w

#
#  Copyright (c) 2004-2005 The University of Chicago, as Operator
#  of Argonne National Laboratory.
#

# pv_crawler.pl
#   Reads ioc st.cmd files, dbd files, db files and 
#   writes resulting data to db. 
#
# Authors: Don Dohan (dohan@aps.anl.gov) 
#          Claude Saunders (saunders@aps.anl.gov)
#
# Modifications:
#   1/10/06.  Ron MacKenzie (ronm@slac.stanford.edu).
#     Production changes:
#     Add additional parameters to call to parse_startup_command().
#     Add these new calls: 
#       SEQCrawlerDBLayer::set_home_dir(getcwd()); 
#       SEQCrawlerDBLayer::set_decryption_key($decryption_key);
#       SEQCrawlerDBLayer::set_test_mode(1);
#       SEQCrawlerDBLayer::update_db_tables();  (this writes to database)
#     Add command option: --no-sequence-scan to turn off sequence file scanning.
#
#   2005.    Judy Rock (jrock@slac.stanford.edu).
#     Added changes.  Search for "JROCK" to see them.
#
#JROCK: these 2 things:
#use lib "/afs/slac/package/epics/tools/irmis/db/src/util/perl";
#use Oracle10;

use FileHandle;
use File::Basename; 
use Cwd;
use File::Compare;
use File::Path;
use File::stat;

use LogUtil;
use PVCrawlerDBLayer;
use PVCrawlerParser;
use SEQCrawlerDBLayer;

##########################################################
#  GET SCRIPT ARGUMENTS
##########################################################
$go = 0; # don't run script unless explicitly enabled
$force = 0; # crawl ioc's even if boot date is same as last time
$debug_level = "info"; # default
$sequence_scan = 1;  # default is to run sequence file scan
$boot_scan_module = "";
if (!@ARGV) {
    print_usage();
    print "You must at a minimum use --go and --boot-scan command line options to run the script!\n";
    exit;
}
while (@ARGV) {
    $command = shift @ARGV;
    if ($command =~ /-help/) {
        print_usage();
        exit;
    } elsif ($command =~ /--boot-scan=(.*)/) {
        $boot_scan_module = $1;
    } elsif ($command =~ /--debug-level=(.*)/) {
        $debug_level = $1;
    } elsif ($command =~ /--go/) {
        $go = 1; 
    } elsif ($command =~ /--key=(.*)/) {
        $decryption_key = $1;
    } elsif ($command =~ /--no-sequence-scan/) {
        $sequence_scan = 0;
    } elsif ($command =~ /--test/) {
        PVCrawlerDBLayer::set_test_mode(1);
        SEQCrawlerDBLayer::set_test_mode(1);
    } elsif ($command =~ /--force/) {
        $force = 1;
    } else {
        print_usage();
        exit;
    }
}
if (!$go) {
    print "You must at a minimum use --go and --boot-scan command line options to run the script!\n";
    exit;
}
if (!$boot_scan_module || (!-e $boot_scan_module.".pm")) {
    print "You must specify a valid boot scan module (minus the .pm extension)\n";
    print "Example command line option: --boot-scan=APSBootScan\n";
    exit;
}

##########################################################
#  SETUP
##########################################################
# Can be 'verbose', 'debug', 'info', 'warn', or 'error'
LogUtil::set_log_level($debug_level);

# skipping LogUtil prefix for brevity from now on
log('info',"pv crawler beginning");

# Tell db layer where the home dir is before we do any chdir.
PVCrawlerDBLayer::set_home_dir(getcwd());
SEQCrawlerDBLayer::set_home_dir(getcwd()); # RONM ADDED

# Optional 16 byte blowfish decryption key. If given (non null),
# it is assumed the db.properties file is encrypted.
PVCrawlerDBLayer::set_decryption_key($decryption_key);
SEQCrawlerDBLayer::set_decryption_key($decryption_key);  #RONM ADDED

# Read site-specific file path substitutions (if any)
$path_substs = get_file_path_substs();

# Read site-specific env vars (if any)
# JROCK new
$env_vars = get_env_vars();

# Directory where temporary msioutput file can be written/read
$temp_file_dir       = "/tmp";

# default values for the different DBF_XXX field data types
$dbd_def = create_dbd_default_lookup_hash();

# verify that msi tool is accessible in user's path
if (system('msi -h') == -1) {
    log('error','Cannot find the required EPICS utility "msi" in your path.');
    log('error','Exiting...');
    exit;
}

##########################################################
#  IDENTIFY IOC's to be examined
##########################################################

@ioc_boot_infos = find_boot_iocs(); # implemented in *BootScan package

##########################################################
#  ITERATE OVER IOC's
##########################################################
foreach $ioc_boot_info (@ioc_boot_infos) {

    $ioc_nm = $ioc_boot_info->{ioc_nm};
    $st_cmd_dir = $ioc_boot_info->{st_cmd_dir};
    $st_cmd_file = $ioc_boot_info->{st_cmd_file};
    $boot_date = format_date($ioc_boot_info->{boot_date});
    $ioc_booted = $ioc_boot_info->{booted}; # can't even find/open boot params

    $st_cmd_path = $st_cmd_dir . "/" . $st_cmd_file;

    log('info',"Processing ioc $ioc_nm, boot date: $boot_date");

    if (!$ioc_booted) {

        log('warn',"Ioc appears not to be booted. Unable to find and/or open boot info.");
        PVCrawlerDBLayer::ioc_boot_save($ioc_boot_info, 0, 0);

    } else {

        # Check our last record of boot from db
        $last_boot_record = PVCrawlerDBLayer::ioc_boot_record_load($ioc_boot_info,0);

        # If there is no previous record, or boot date differs, or force enabled,
        #   parse the st.cmd and save data to db
        if ( $last_boot_record->{ioc_boot_id} == 0 ||
            $last_boot_record->{ioc_boot_date} ne $boot_date ||
            $force) {
            
            log('info',"Parsing $st_cmd_path");
            
            #############################################################
            # PARSE UP ST.CMD FILE AND RETRIEVE RESULTANT DATA STRUCTURES
            #############################################################
            
            initialize_parser($temp_file_dir, $st_cmd_dir, $dbd_def, $ioc_nm, $path_substs, $env_vars);
            $parse_err = parse_startup_command($st_cmd_path, $st_cmd_dir, $ioc_nm, $sequence_scan);

            # record/field instance defs from db and template files
            # (array of hashes, each hash with rec_nm, fld_nm, rec_typ, 
            #  fld_val, file_index keys)
            $rnam_fldn = get_rnam_fldn();

            # db and dbd file list 
            # (array of hashes, each hash with id, path, subst, mtime keys)
            $file_list = get_file_list();

            # record type defs from dbd files
            # (hash of arrays, keyed by record type. 
            #  array elems are each hash with fldn, file_index, def_val, type keys)
            $dbd = get_dbd();

            # device support info gleaned from dbd file
            # (array of hashes, each hash with rec_typ, io_type, dev_sup_dset_nm, dtyp_str keys)
            $dev_sup = get_device_support();

            # Sort the rnam_fldn results by pv name concat with fld name
            @$rnam_fldn = sort { $a->{rec_nm}." ".$a->{fld_nm} cmp $b->{rec_nm}." ".$b->{fld_nm} } @$rnam_fldn;

            # Remove duplicates from rnam_fldn (rule: last field def wins)
            @dupl_rnam_fldn = ();
            filter_duplicate_records($rnam_fldn, \@dupl_rnam_fldn);

            ##########################################################
            #  COMPARE CURRENT file_list WITH LAST DB LOAD INFO
            ##########################################################
            $ioc_resource_changed = 
                check_for_file_mods($file_list, $ioc_boot_info);
            log('info',"IOC Resources changed: $ioc_resource_changed, Parse error: $parse_err");
            if ($force) {
                log('info',"Forcing write to db regardless.");
                $ioc_resource_changed = 1;
            }
            
            ##########################################################
            #  WRITE DATA TO DB (boot record and pv data (if changed))
            ##########################################################    

            log('info',"Write data to DB");
            PVCrawlerDBLayer::all_save($ioc_boot_info, $parse_err, 
                                       $ioc_resource_changed,
                                       $rnam_fldn, $file_list, $dbd, $dev_sup);

            if ($sequence_scan) {
              SEQCrawlerDBLayer::update_db_tables($ioc_nm);
            }

            # recover memory from possibly rather large arrays
            undef(@$rnam_fldn);
            undef(@$file_list);
            undef(%$dbd);
            undef(@$dev_sup);
            undef(@dupl_rnam_fldn);
            
        } else {
            log('info',"Skipping since boot dates are the same");
        }

    } # end if (!booted) else
    
}    # end for each ioc in the bootparams directory


log('info',"pv crawler done");
exit;


#############################################################
# SUBROUTINES
#############################################################

sub create_dbd_default_lookup_hash {
    my $dbd_def = {
                   'DBF_STRING'   => '',
                   'DBF_CHAR'     => '',
                   'DBF_UCHAR'    => '',
                   'DBF_SHORT'    => '0',
                   'DBF_USHORT'   => '',
                   'DBF_LONG'     => '0',
                   'DBF_ULONG'    => '0',
                   'DBF_FLOAT'    => '0',
                   'DBF_DOUBLE'   => '0',
                   'DBF_ENUM'     => '',
                   'DBF_MENU'     => '',
                   'DBF_DEVICE'   => '',
                   'DBF_INLINK'   => '',
                   'DBF_OUTLINK'  => '',
                   'DBF_FWDLINK'  => '',
                   'DBF_NOACCESS' => ''
                   };

    return $dbd_def;
}

# Return mtime of given file in YYYYMMDDHHMMSS format
sub get_file_mod_date {
    my $file_name = $_[0];    # full file path
    
    if ( !( -e $file_name ) ) {
        return;
	} else {
        $mod_time = stat($file_name)->mtime;
        if ($mod_time) {
            return format_date($mod_time);
        } else {
            return;
        }
    }
}

sub filter_duplicate_records {

    #
    # arguments to subroutine
    #

    # array reference to rnam_fldn (assumes it is sorted already)
    my $rnam_fldn = $_[0];

    # array ref to destination array for duplicates found
    my $dupl_rnam_fldn = $_[1];

    #
    # Find duplicate pv definitions in rnam_fldn. Afterwards, @s contains the
    # final clean set of pv's, and @d contains duplicates.
    #
    my $pvdlength   = @$rnam_fldn;         #length of the pv array with duplicates
    # candidate for duplicate
    my $candidate = $$rnam_fldn[0];
    my $id        = 0;                     #index for duplicate array
    my $is        = 0;                     #index for filtered (single) array

    my @d = ();  # duplicates go here
    my @s = ();  # uniques go here

    # Note: this can be done within a single loop, since rnam_fldn is sorted.

    # the length of $id + length of $is should equal length of rnam_fldnam
    #  when the rnfn changes, then the previous (i.e.candidate) rnfn is put 
    #  in the $s.. All other identical rnfn have been put in $d

    my $test = "";
    my $new_record = 1;
    my $accept_null_fld = 1;
    for ( my $ix = 1 ; $ix < $pvdlength ; $ix++ ) {
        $test = $$rnam_fldn[$ix];
        my $test_fw = $test->{rec_nm} . $test->{fld_nm};
        my $candidate_fw = $candidate->{rec_nm} . $candidate->{fld_nm};

        # This is some fairly complex logic to handle the possibility of
        # multiple null field names appearing while processing a record.
        # Generally the last field def wins, but not if the fld_nm is null.
        if ($new_record) {
            $accept_null_fld = 1;

        } elsif ($candidate->{fld_nm}) {
            $accept_null_fld = 0;
        }

        if ($test_fw eq $candidate_fw) {
            $new_record = 0;
            $d[ $id++ ] = $candidate;
            #Not an error, duplicates ok, last one wins
            #log('warn',"duplicate pv/fld ignored $candidate->{rec_nm} $candidate->{fld_nm}");
            $candidate = $test; 

        } else {
            if ($candidate->{rec_nm} eq $test->{rec_nm}) {
                $new_record = 0;
            } else {
                $new_record = 1;
            }
            if ($candidate->{fld_nm} || ($accept_null_fld && $new_record)) {
                $s[ $is++ ] = $candidate;
                $candidate = $test;
                $accept_null_fld = 0;
            } else {
                $candidate = $test;
            }
        }
    } 
    # at this point, $candidate contains the last field.  It must go into the 
    #  last entry of the (s) array;
    $s[ $is++ ] = $candidate;

    @$dupl_rnam_fldn = @d;
    @$rnam_fldn = @s;
}

# Compare file_list just scanned from ioc startup against last ioc boot records.
# If any file mod dates have changed, or any new files have been
# introduced or removed, return 1. Otherwise return 0.
#
sub check_for_file_mods {
    my $file_list = $_[0];
    my $ioc_boot_info = $_[1];

    my $ioc_resources = PVCrawlerDBLayer::find_ioc_resource_and_uri($ioc_boot_info);
    if (!$ioc_resources) {
        log('debug',"check_for_file_mods(): no record found in db, so return 1");
        return 1; # no prior record, so everything is "new"
    }
    
    # if they are different size, then something must have changed
    my $file_list_size = @$file_list;
    my $ioc_resource_size = @$ioc_resources;
    log('debug',"file_list_size $file_list_size irs $ioc_resource_size");
    if ($file_list_size != $ioc_resource_size) {
        log('debug',"check_for_file_mods(): resource array sizes differ, so return 1");
        return 1;
    }

    # now check to see if all file names and mod dates match
    my $all_match = 1;
    for $file (@$file_list) {
        my $found = 0;
        my $mod_dates_match = 0;
        # compare file paths and mod dates
        for $resource (@$ioc_resources) {
            if ($file->{path} eq $resource->{uri}) {
                $found = 1;
                if (format_date($file->{mtime}) eq $resource->{uri_modified_date}) {
                    $mod_dates_match = 1;
                }
            }
        }
        if (!$found || !$mod_dates_match) {
            log('debug',"check_for_file_mods(): mismatch found: $found mod_dates_match $mod_dates_match, so return 1");
            return 1;
        }
    }

    return 0;

}

# Look for a file called path.properties which contains a set of possible
# path substitutions to try when a file path cannot be found. This is to
# help in odd cases where some systems use non-standard mount points.
# File path.properties is a series of lines, each with:
#   /existing/sub-path=/new/sub-path/to/try
#
sub get_file_path_substs {

    my %substs = ();

    my $substs_path = getcwd() . "/path.properties";

    if (-e $substs_path) {
        open(PATHPROPS,$substs_path);
        while (my $props_line = <PATHPROPS>) {
            chomp($props_line);
            if ($props_line =~ /(.*)=(.*)/) {
                $existing_path = $1;
                $new_path = $2;
                $substs{$existing_path} = $new_path;
            }
        }
    }
    return \%substs;
}

#JROCK new
# Look for a file called env.properties which contains a set of 
# "env" variables to set.
# One of these, IRMIS_SITE, is used to run site-specific code snippets.
# File env.properties is a series of lines, each with:
#   global_var_name=global_var_value
#  e.g. 
#   IRMIS_SITE=SLAC
#
sub get_env_vars {

    my %vars = ();

    my $vars_path = $ENV{ENV_PROP_FILE};
    if ($vars_path) {
        if (-e $vars_path) {
            open(VARS,$vars_path);
            while (my $vars_line = <VARS>) {
                chomp($vars_line);
                if ($vars_line =~ /(.*)=(.*)/) {
                    $var_name = $1;
                    $var_value = $2;
                    $vars{$var_name} = $var_value;
                }
            }
        }
    }
    return \%vars;
}

# Takes seconds after epoch and converts to string of format "YYYYMMDDHHMMSS"
sub format_date {
    my $secsAfter1970 = $_[0];

    if (!$secsAfter1970) {
        return "0000-00-00";
    }

    my @tmArr = localtime($secsAfter1970);

    my $formatStr;
    $formatStr = '%Y%m%d%H%M%S';

    my $formatDate = POSIX::strftime($formatStr, $tmArr[0],$tmArr[1],$tmArr[2],
                                     $tmArr[3], $tmArr[4], $tmArr[5]);

    return $formatDate;
}

# Print out application usage text
sub print_usage {
    print "usage: pv_crawler --go --boot-scan=<module-name> [--no-sequence-scan] [--key=<16 char key>] [--test] [--force] [--help] [--debug-level=[verbose|debug|info]]\n\n";
    print "The pv_crawler scans a list of ioc startup command files, parsing\n";
    print "what data it can into data structures representing db and dbd files\n";
    print "and their content (record and field definitions). These data\n";
    print "structures are then inserted into a database using perl DBI.\n";
    print "\n";
    print "The pv_crawler also scans sequence files while it scans ioc startup\n";
    print "files.  It inserts pv, ioc, and sequence filename information from those\n";
    print "files into the database.  By default, sequence files are scanned.  To turn\n";
    print "off the scanning of sequence files, use the --no-sequence-scan option.\n";
    print "\n";
    print "Log output goes to standard out, and can be adjusted from the default\n";
    print "\"info\" level. Watch out, since verbose is VERY verbose.\n\n";
    print "You will need to establish the expected schema by running the\n";
    print "create_pv_tables.sql and alter_pv_tables.sql scripts in the\n";
    print "ddl subdirectory. Then edit db.properties to set your local database\n";
    print "connection parameters (ie. user/password, db vendor, etc.).\n";
    print "If a 16 char decryption key is given, crawler assumes the\n";
    print "db.properties file is blowfish encrypted with the given key.\n\n";
    print "Lastly, the list of iocs to scan is determined by the *BootScan\n";
    print "module you are using. You specify which one you want to use with\n";
    print "the --boot-scan option. Right now, APSBootScan and SNSBootScan are\n";
    print "available. Each EPICS site generally has a very different ioc boot\n";
    print "methodology, so you will likely need to create your own *BootScan\n";
    print "perl module.\n\n";
    print "The --test option runs the crawler, but does no db writes.\n";
    print "The --force option forces a re-crawling of all iocs.\n";
}

# Very odd perl autoloading ...
sub AUTOLOAD {
    if ($AUTOLOAD !~ /find_boot_iocs/) {
        warn "Attempt to call $AUTOLOAD, which is not defined!\n";
        return;
    }
    require "$boot_scan_module.pm";
    my $subroutine = "$boot_scan_module"."::find_boot_iocs";
    goto &$subroutine;
}
