#!/sbin/cgi
<?
include("lib/settings.inc");
$cfg = @cfg_load($cfg_file);
include("lib/l10n.inc");
include("lib/system.inc");
include("lib/help.inc");

$SCRIPT_DIR = "/etc/persistent/";
$SCRIPT_DIR_TMP = "/tmp/persistent/";
$SAVE_CMD = "/bin/cfgmtd -w -p /etc/";
$close_window = 0;
$scripts_modified = 0;

Function remove_script $rm_scr
(
	global $SCRIPT_DIR_TMP;
	@openDir($SCRIPT_DIR_TMP);
	$f = readDir();
	while (strlen($f) != 0) {
		if (strstr($f, $rm_scr)) {
			@unlink($SCRIPT_DIR_TMP + $f);
		}
		$f = readDir();
	}
	@closeDir();
);

Function remove_scripts_from $SCRIPT_DIR
(
	@openDir($SCRIPT_DIR);
	$f = readDir();
	while (strlen($f) != 0) {
		if ($f != "." && $f != "..") {
			if (strstr($f, "rc.pre") || strstr($f, "rc.post")) {
				@unlink($SCRIPT_DIR + $f);
			}
		}
		$f = readDir();
	}
	@closeDir();
);

Function copy_scripts $FROM, $TO
(
	@openDir($FROM);
	$f = readDir();
	while (strlen($f) != 0) {
		if ($f != "." && $f != "..") {
			if (strstr($f, "rc.pre") || strstr($f, "rc.post")) {
				@link($FROM + $f, $TO + $f);
				$i++;
			}
		}
		$f = readDir();
	}
	@closeDir();
);

Function save_changes
(
	global $SCRIPT_DIR_TMP;
	global $SCRIPT_DIR;
	global $SAVE_CMD;
	global $cmd_softrestart;

	if (fileType($SCRIPT_DIR_TMP) == "dir") {
		remove_scripts_from($SCRIPT_DIR);
		copy_scripts($SCRIPT_DIR_TMP, $SCRIPT_DIR);
		remove_scripts_from($SCRIPT_DIR_TMP);
		RmDir($SCRIPT_DIR_TMP);
		exec($SAVE_CMD);
	}
	bgexec(0, $cmd_softrestart+"force");
);

Function remove_tmp
(
	global $SCRIPT_DIR_TMP;

	if (fileType($SCRIPT_DIR_TMP) == "dir") {
		remove_scripts_from($SCRIPT_DIR_TMP);
		RmDir($SCRIPT_DIR_TMP);
	}
);

if ($REQUEST_METHOD=="POST")
{
	if ($action == "save") {
		save_changes();
		$close_window = 1;
	} elseif ($action == "no_changes") {
		remove_tmp();
		$close_window = 1;
	}

	if (strlen($rm_scr) > 0) { #remove script
		$close_window = 0;
		$scripts_modified = 1;
		remove_script($rm_scr);
	} elseif (strlen($scr_name) > 0) { #change status
		$close_window = 0;
		$scripts_modified = 1;
		if ($scr_enabled == "true") {
			if (@fopen($SCRIPT_DIR_TMP + "rc." + $scr_name + "_disabled", "r") != "-1") {
				rename($SCRIPT_DIR_TMP + "rc." + $scr_name + "_disabled", $SCRIPT_DIR_TMP + "rc." + $scr_name);
			}
		} else {
			if (@fopen($SCRIPT_DIR_TMP + "rc." + $scr_name, "r") != "-1") {
				rename($SCRIPT_DIR_TMP + "rc." + $scr_name, $SCRIPT_DIR_TMP + "rc." + $scr_name  + "_disabled");
			}
		}
	}
}

include("lib/custom_scripts_head.tmpl");

if (fileinode($SCRIPT_DIR) != -1)
{
	$is_ro = 0;
	if (ereg("([^:]+):([^:]+):([^:]+)",$REMOTE_USER,$res)) {
		$is_ro = $res[3];
	}

	if (GetEnv("REQUEST_METHOD") == "GET") {
		if (fileType($SCRIPT_DIR_TMP) != "dir") {
			MkDir($SCRIPT_DIR_TMP,0755);
		} else {
			remove_scripts_from($SCRIPT_DIR_TMP);
			@closeDir();
		}
		copy_scripts($SCRIPT_DIR, $SCRIPT_DIR_TMP);
	}

	if (fileType($SCRIPT_DIR_TMP) == "dir") {
		@openDir($SCRIPT_DIR_TMP);
		$f = readDir();
		$arr = "[";
		while (strlen($f) != 0) {
			if ($f != "." && $f != "..") {
				if (strstr($f, "rc.pre") || strstr($f, "rc.post")) {
					if (strstr($f, "_disabled")) {
						$status =  "disabled";
						$typename =  ereg_replace("_disabled", "", $f);
					} else {
						$status = "enabled";
						$typename = "$f";
					}
					$typename =  ereg_replace("rc.", "", $typename);
					$arr = $arr + "{" + "\"filename\": " + "\"" +  $typename + "\"" + ",\"status\": " + "\"" +  $status + "\"" + "},";
				}
			}
			$f = readDir();
		}
		$arr = $arr +  "]";
		@closeDir();
		echo "<script> push_list('$arr'); </script>";
	}
}
include("lib/custom_scripts_tail.tmpl");
