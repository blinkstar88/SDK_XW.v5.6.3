#!/sbin/cgi
<?
include("lib/settings.inc");
include("lib/system.inc");
include("lib/link.inc");

if (strlen($uniband_name) > 0 && strlen($uniband_key) > 0) {
	$action = "-u";
	if ($radio1_caps & $radio_cap_new_grant_certified) {
		if ($uniband_status == "false") {
			$action = "-l";
		}

		if ((($uniband_status == "true") && ($board_fcc_unii_lock_state == $lock_state_new_grant)) ||
			(($uniband_status == "false") && ($board_fcc_unii_lock_state != $lock_state_new_grant))) {
			exit;
		}
	}

	$cmd = $cmd_uniband + " " + $action + " " + EscapeShellCmd($uniband_name)+" "+EscapeShellCmd(ereg_replace("-", "", $uniband_key));
	exec($cmd, $lines, $res);
	if ($res != 0) {
		echo "-1";
	} else {
		bgexec(0, $cmd_softrestart+"force");
		echo "0";
	}
}
exit;
>
