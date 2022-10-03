<?
$firmware_file = "/tmp/fwupdate.bin";
$upload_dir = "/tmp/upload";

Function fw_get_version (
	$version = @file("/etc/version");
	if ($version == "-1" || count($version) == 0)
	{
		$version = "unknown";
	}
	else
	{
		$version = $version[0];
	}
    return $version;
);

Function fw_get_build (
	$version = @file("/usr/lib/version");
	if ($version == "-1" || count($version) == 0)
	{
		$version = "unknown";
	}
	else
	{
		$version = $version[0];
	}
	if (ereg("[.]([[:digit:]]+)[.][[:digit:]]{6}[.][[:digit:]]{4}", $version, $regs))
	{
		$build = $regs[1];
	}
	else
	{
		$build = "unknown";
	}
    return $build;
);

Function getRetVal $ret_val
(
	$res = (($ret_val & 65280) / 256);
	if ($res > 127)
	{
		$res -= 256;
	}
	return $res;
);

Function fw_validate $file (
	global $firmware_file;
	$result = "";
	if (fileinode($file) == -1)
	{
		$result = dict_translate("err_no_memory1|Not enough free memory to upload Firmware image. Please reboot device and try again.");
	}
	else
	{
		if ($file != $firmware_file && rename($file, $firmware_file) != 0)
		{
			$result = dict_translate("err_no_memory2|Not enough free memory to check Firmware image. Please reboot device and try again.");
		}
		else
		{
			exec("/sbin/fwupdate -c 2>&1", $lines, $res);
			if ($res != 0) {
				$ret_val = getRetVal($res);
				switch ($ret_val)
				{
					case -4;
						$result = dict_translate("Invalid Firmware file is uploaded. Please upload the correct file and try again.");
						break;
					case -5;
						$result = dict_translate("Invalid or corrupted Firmware file is uploaded. Please upload the correct file and try again.");
						break;
					case -6;
						$result = dict_translate("Wrong Firmware version is uploaded. Please upload the correct version and try again.");
						break;
					case -7;
					case -8;
					case -20;
						$result = dict_translate("Corrupted Firmware file is uploaded. Please upload the correct file and try again.")
							+ " (" + getRetVal($res) + ")";
						break;
					default;
						$result = dict_translate("Firmware image check failed. Error code") + ": " + $ret_val;
					break;									
				}
			}
		}
	}
	return $result;
);

	Function check_uploaded_file $file_name, $file_size, $file_description, $max_size (
		$error_msg = "";
		if (strlen($file_name) > 0) {
			if ($file_size > $max_size) {
				$error_msg = sprintf(dict_translate("msg_upl_file_too_big|Uploaded %s file is too big. Maximum allowed size is %d bytes."), $file_description, $max_size);
			} elseif ($file_size == 0) {
				$error_msg = sprintf(dict_translate("msg_upl_file_empty|Uploaded %s file is empty!"), $file_description);
			}
		}
		return $error_msg;
	);

	Function check_cfg_version $oldcfg, $newcfg (
		$ret = 0;
                $old_ver = cfg_get_def($oldcfg, "system.cfg.version", 0);
                $new_ver = cfg_get_def($newcfg, "system.cfg.version", 0);
                if ($new_ver < $old_ver) { $ret = -1; }
                elseif ($new_ver > $old_ver) { $ret = 1; }
		return $ret;
	);

  Function fw_extract_version $file (
    $fp = @fopen($file, "r");
    if ($fp != -1) {
	    $head = fgets($fp, 5);
	    $version = fgets($fp, 128);
	    fclose($fp);
    }
    if (ereg("^([^\.]*)\.([^\.]*)\.([^\.]*)\.([^\.]*)\.(.*)$", $version, $regs)) {
	if (strchr($version, '-')) {
		return "$regs[1].$regs[3].$regs[4].$regs[5]";
	} else {
                $len = strlen($regs[5]);
	        if ($len > 17) {
                        return "$regs[1].$regs[3].$regs[4]." + substr($regs[5], 0, $len - 17);
                }
		return "$regs[1].$regs[3].$regs[4]";
	}
    } else {
        return $version;
    }
  );

Function fw_is_thirdparty $file (
	$fp = @fopen($file, "r");
	if ($fp == -1)
	{
		$result = 1;
	}
	else
	{
		$head = fgets($fp, 5);
		fclose($fp);
		if ($head == "OPEN")
		{
			$result = 1;
		}
		else
		{
			$result = 0;
		}
	}
	return $result;
);

Function fw_is_short_passwd $fwversion, $passwd (
	$res = 0;
        if (substr($passwd, 0, 5) == "\$1\$") { /* NOTE: \ has to be counted into length */
	        if (substr($fwversion, 0, 4) == "XM.v") {
        	        $d = doubleval(substr($fwversion, 4, strlen($fwversion) - 4));
                	if ($d < 5.6) {
                        	$res = 1;
	                }
        	}
	        if (substr($fwversion, 0, 4) == "XW.v") {
                	if ((substr($fwversion, 0, 9) == "XW.v5.5.5") ||
                            (substr($fwversion, 0, 15) == "XW.v5.5.6-beta.")) {
                        	$res = 1;
                        }
                }
        }
	return $res;
);

Function fw_is_downgrade $n_fwversion, $c_fwversion (
	$res = 0;
        $n_p = strstr($n_fwversion, ".v");
        $c_p = strstr($c_fwversion, ".v");
	if ((strlen($n_p) > 6) && (strlen($n_p) > 6)) {
             	$n = doubleval(substr($n_p, 2, strlen($n_p) - 2));
             	$c = doubleval(substr($c_p, 2, strlen($c_p) - 2));
                if ($n < $c) {
                	$res = 1;
	        } elseif ($n == $c) {
	             	$n = doubleval(substr($n_p, 6, strlen($n_p) - 6));
        	     	$c = doubleval(substr($c_p, 6, strlen($c_p) - 6));
                        if (($n < 64) && ($c < 64) && ($n < $c)) {
                                $res = 1;
                        } elseif (($n > 64) && ($c < 64)) {
                                $res = 1;
                        }
                }
        }
	return $res;
);

Function cleanup_dir $d (
	if (fileinode($d) != -1)
	{
		@openDir($d);
		$f = readDir();
		while (strlen($f) != 0) {
			if ($f != "." && $f != "..") {
				@unlink("$d/$f");
			}
			$f = readDir();
		}
		@closeDir();
        }
);

Function do_update_check $cfg (
	$update_status = cfg_get_def($cfg, "update.check.status", "enabled");
	if ($update_status != "disabled") {
		return "true";
	}
	else {
		return "false";
	}
);

Function gen_update_check $cfg (
	echo "var fw_check = {\n";
	echo "\tenabled: " + do_update_check($cfg) + ",\n";
	echo "\tfw_str: \"" + dict_translate("fw_ver_str|Firmware update {1} is available.") + "\",\n";
	echo "\tno_str: \"" + dict_translate("fw_no_update|You have latest available Firmware version.") + "\",\n";
	echo "\tno_conn_str: \"" + dict_translate("fw_no_conn_update|No connection to www.ubnt.com from device.") + "\",\n";
	echo "\tdl_str: '" + dict_translate("ubnt_dl_str|Go to Ubiquiti Networks <a href=\"http://www.ubnt.com/support/downloads\" target=\"_blank\"> downloads</a> page.") + "'\n";
	echo "};\n";
);

Function is_config_changed (
	global $cfg_file, $cfg_file_bak;

	$cfg_sorted = $cfg_file + ".tmp";
	$cfg_running_sorted = $cfg_file_bak + ".tmp";
	
	exec("sort $cfg_file | tr -d \"\\r\" > $cfg_sorted");
	exec("sort $cfg_file_bak | tr -d \"\\r\" > $cfg_running_sorted");
	exec("diff $cfg_sorted $cfg_running_sorted", $output, $res);
	
	Unlink($cfg_sorted);
	Unlink($cfg_running_sorted);
	
	return $res;
);

Function show_emerg (
	global $emerg_file;
	global $emerg_crashlog_file;

	if ((fileinode($emerg_file) != -1) || fileinode($emerg_crashlog_file) != -1)
	{
		$show = 1;
	}
	else
	{
		$show = 0;
	}

	return $show;
);

Function get_emerg_reason (
	global $emerg_file;
	global $emerg_crashlog_file;

	$reason = 0;
	if (fileinode($emerg_crashlog_file) != -1)
	{
		$reason = 100;
	}
	else
	{
		$fp = fopen($emerg_file, "r");
		if ($fp) {
			fgets($fp, 255);
			if (!feof($fp)) {
				$reason = IntVal(fgets($fp, 255));
			}
			fclose($fp);
		}
	}
	
	return $reason;
);

>
