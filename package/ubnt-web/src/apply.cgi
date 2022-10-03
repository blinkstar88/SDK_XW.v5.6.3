#!/sbin/cgi
<?
include("lib/settings.inc");
$cfg = @cfg_load($cfg_file);
include("lib/l10n.inc");
$pagetitle = dict_translate("Applying...");
$message = dict_translate("msg_conf_applied|Configuration is being applied. Please stand by...");
$duration = $soft_reboot_time;
include("lib/link.inc");
include("lib/ipcheck.inc");

function get_dfs_list_test_time $cfg
(
	global $dfs_test_mode_time;
        
	$list_status = cfg_get_def($cfg, "wireless.1.scan_list.status", "disabled");
        if ($list_status == "enabled") {
		$freq_list = cfg_get_def($cfg, "wireless.1.scan_list.channels", "");
                if (strlen($freq_list) > 3) {
                	$tok = strtok($freq_list,",");
			while($tok);
		  		$freq = intVal($tok);
                                if ($freq >= 5600 && $freq <= 5650) {
                       			return $dfs_test_mode_time;
				} else {
                                	if ($freq >= 5250 && $freq <= 5725) { /*Right for most cases except Korea*/
                                        	return 60;
                                        }
                                }
		  		$tok = strtok(",");
			endwhile;
                }
        }
        return 0;
);

function get_dfs_test_time $cfg
(
	global $dfs_test_mode_time;
        
	$dfs = cfg_get_def($cfg, "radio.1.dfs.status", "disabled");
	if ($dfs == "enabled") {
        	$freq = cfg_get_def($cfg, "radio.1.freq", 0);
               	$freq = intVal($freq);
                if ($freq >= 5600 && $freq <= 5650) {
                       	return $dfs_test_mode_time;
        	} else {
                	
                	return get_dfs_list_test_time($cfg);
                }
        }
	return 0;
);

$added_time = 0;
if ($cfg != -1) {
       	$sshd_state = cfg_get_def($cfg, "sshd.status", "disabled");
       	if ($sshd_state == "enabled") {
       		if ((fileinode($dss_priv_filename) == -1) ||
       			(fileinode($rsa_priv_filename) == -1)) {
       				$added_time = $dss_rsa_gen_time;
       				$duration = $duration + $added_time;
       		}
       	}
}

$sr_delay = 1;
if (strlen($testmode) != 0) {
	$fp = @fopen($test_lock_file, "w");
	if ($fp != -1) {
        	$test_time = $test_mode_time + get_dfs_test_time($cfg);
		@fputs($fp, $test_time);
		@fclose($fp);
		chmod($test_lock_file, 755);
		bgexec($sr_delay, $cmd_softrestart+"test");
        }
} else {
	chmod($cfg_file, "644");
	$fp = @fopen($test_lock_file, "r");
	if ($fp != -1) {
		@fclose($fp);
        	@unlink($test_lock_file);
		exec("/usr/bin/sort $cfg_file_bak > $cfg_file");
		bgexec(0, $cmd_cfgsave);
	}
	else {
		bgexec($sr_delay, $cmd_softrestart+"save");
	}
}
exit;
>
