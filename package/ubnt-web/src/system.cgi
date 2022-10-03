#!/sbin/cgi
<?
include("lib/settings.inc");
$cfg = @cfg_load($cfg_file);
include("lib/l10n.inc");
include("lib/system.inc");
include("lib/link.inc");
include("lib/misc.inc");
include("lib/help.inc");
include("lib/password.inc");
include("/etc/board.inc");

$fwversion = fw_get_version();
$fwbuild= fw_get_build();
if ($cfg == -1)
{
	include("lib/busy.tmpl");
	exit;
}
$message = "";
$modified = 0;
$cfg_mismatch = 0;

$is_new_fw_uploaded = 0;
$country = cfg_get_country($cfg, $wlan_iface, "");

$curr_iface = $wlan_iface;
$curr_ifidx = get_wlan_index($curr_iface); 
init_board_inc($curr_iface);

$cfgwmode = cfg_get_wmode($cfg, $curr_iface);
if (strlen($cfgwmode) == 0) {
	$cfgwmode = "sta";
}

$country = cfg_get_country($cfg, $curr_iface, $country);
$ieee_mode = cfg_get_ieee_mode($cfg, $curr_iface, $ieee_mode);
$chanbw = cfg_get_def_ne($cfg, "radio.$curr_ifidx.chanbw", 0);
$chan_freq = cfg_get_def($cfg, "radio.$curr_ifidx.freq", $chan_freq);

$channel_scan_list = cfg_get_channel_scan_list($cfg, $curr_ifidx, $channel_scan_list);
$scan_channels = cfg_get_scan_channels($cfg, $curr_ifidx, $scan_channels);
  
if ($REQUEST_METHOD=="POST")
{
	if (strlen($fwfile) > 0) {
        if (strlen($fwupload) == 0) {
            @unlink($firmware_file);
            @unlink($eula_html);
            cleanup_dir($upload_dir);
        } else {
		    $is_new_fw_uploaded = 1;
        }
	}

	if (strlen($update_discard) > 1) {
		@unlink($firmware_file);
                @unlink($eula_html);
		cleanup_dir($upload_dir);
	}
	
	if (strlen($cfgupload) > 0)
	{
		if (strlen($cfgfile) > 0)
		{
			$error_msg = check_uploaded_file($cfgfile, $cfgfile_size,
				dict_translate("configuration"), 65535);
			if (strlen($error_msg) == 0)
			{
				$tcfg = @cfg_load($cfgfile);
				if ($tcfg == -1)
				{
					$error_msg = dict_translate("msg_invalid_conf_file|Invalid configuration file");
				}
				else
				{
					if (cfg_get($tcfg, "radio.status") != "enabled" || cfg_get($tcfg, "radio.1.status") != "enabled")
					{
						$error_msg = dict_translate("msg_invalid_conf_file_struct|Invalid configuration file structure");
					} elseif ($radio["ccode_locked"] == 1 || $radio["ccode_fixed"] == 1) {
						$ccode = cfg_get_def($tcfg, "radio.countrycode", $radio["ccode"]);
						$ccode1 = cfg_get_def($tcfg, "radio.1.countrycode", $ccode);
						if ($ccode1 != $radio["ccode"]) {
							$error_msg = dict_translate("msg_invalid_conf_file_ccode|Invalid country code in configuration file");
						}
					} elseif ($radio["ccode_locked"] == 2) {
						$ccode = cfg_get_def($tcfg, "radio.countrycode", $radio["ccode"]);
						$ccode1 = cfg_get_def($tcfg, "radio.1.countrycode", $ccode);
						$cmd = $cmd_regdomain + "-L -u -D" + $radio["devdomain"] + " " + $radio["ccode"];
						$cmd += " | cut -f 2 -d ' ' | grep -q ";
						$cmd += EscapeShellCmd($ccode1);
						exec($cmd, $lines, $res);
						if ($res != 0) {
							$error_msg = dict_translate("msg_invalid_conf_file_ccode|Invalid country code in configuration file");
						}
					}
					$wmode = strtolower(cfg_get_def($tcfg, "radio.1.mode", "managed"));
					if ( (($radio1_feature_ap == 0) && ($wmode == "master")) ||
						(($radio1_feature_sta == 0) && ($wmode == "managed")) )
					{
						$error_msg = dict_translate("msg_invalid_conf_file_wmode|Invalid wireless mode in configuration file");
					}
				}
			}
			$cfg_mismatch = check_cfg_version($cfg, $tcfg);

			if (strlen($error_msg) > 0)
			{
				@unlink($cfgfile);
			} else {
				cfg_save($tcfg, $cfg_file);
				cfg_set_modified($cfg_file);
				$modified = 1;
				if ($cfg_mismatch < 0) { 
					exec($cmd_cfgfix);
				}
				$cfg = @cfg_load($cfg_file);
				check_default_passwd($cfg);
			}
		}
	}
	elseif ($feature_logo == 1 && strlen($chlogo) > 0)
	{
		if ($logoStatus == "on")
		{
			$logoStatus = "enabled";
		}
		else
		{
			$logoStatus = "disabled";
		}

		if (strlen($logo_file) > 0)
		{
			$error_msg = check_uploaded_file($logo_file,
				$logo_file_size, dict_translate("logo"), 51200);
			if (strlen($error_msg) > 0)
			{
				@unlink($logo_file);
				include("lib/system.tmpl");
				exit;
			}

			@mkdir("/etc/persistent/www/", 0755);
			@rename($logo_file, "/etc/persistent/www/logo.gif");
		}
		elseif ($logoStatus == "enabled")
		{
			if (fileinode("/etc/persistent/www/logo.gif") == -1)
			{
				@mkdir("/etc/persistent/www/", 0755);
				exec("cp /usr/www/images/ulogo.gif /etc/persistent/www/logo.gif");
			}
		}
		if ($logoStatus == "enabled") {
			cfg_set($cfg, "ls_logo.url", $logoURL);
		}
		cfg_set($cfg, "ls_logo.status", $logoStatus);
		cfg_save($cfg, $cfg_file);
		cfg_set_modified($cfg_file);
		$logo_status = $logoStatus;
		$logo_url = $logoURL;
	}
	elseif(strlen($change))
	{
		if ((strlen($OldPassword) != 0) || (strlen($NewPassword) != 0) || (strlen($NewPassword2) != 0))
		{
			$result = validatePassword($OldPassword, $NewPassword, $NewPassword2);
			if ($result == 0)
			{
				changePassword($NewPassword);
			}
			else
			{
				$error_msg = passChangeErr($result);
				$pwd_change_error = 1;
			}
		}

		if (strlen($error_msg) == 0)
		{
			$old_username = cfg_get_def($cfg, "users.1.name", "ubnt");
			cfg_set($cfg, "gui.language", $active_language);
			if (strlen($adminname) == 0)
			{
				$adminname = $old_username;
			}
			cfg_set($cfg, "users.1.name", $adminname);
			if ($ro_status == "enabled")
			{
				cfg_set($cfg, "users.2.status", "enabled");
				$crypted = cfg_get_def($cfg, "users.2.password", "");
				if (strlen($hasRoPassword)) {
					if (strlen($roPassword)) {
						$crypted = crypt($roPassword);
					} else {
						$crypted = "";
					}
				}
				cfg_set($cfg, "users.2.password", $crypted);
				$old_username = cfg_get_def($cfg, "users.2.name", "guest");
				if (strlen($rousername) == 0)
				{
					$rousername = $old_username;
				}
				cfg_set($cfg, "users.2.name", $rousername);
				cfg_set($cfg, "users.2.gid", 100);
				cfg_set($cfg, "users.2.uid", 100);
				cfg_set($cfg, "users.2.shell", "/bin/false");
			}
			else
			{
				cfg_set($cfg, "users.2.status", "disabled");
			}
			cfg_set($cfg, "resolv.host.1.status", "enabled");
			cfg_set($cfg, "resolv.host.1.name", $hostname);
			if ($date_status != "enabled") {
				$date_status = "disabled";
			}
			cfg_set($cfg, "system.date.status", $date_status);
			if (isset($systemdate)) {
				cfg_set($cfg, "system.date.timestamp", $systemdate);
			}
			cfg_set($cfg, "system.timezone", $timezone);
			if ($resetb_status != "enabled") {
				$resetb_status = "disabled";
			}
			cfg_set($cfg, "system.button.reset", $resetb_status);
			if ($update_status != "enabled") {
				$update_status = "disabled";
			}
			cfg_set($cfg, "update.check.status", $update_status);
			if ($radio_outdoor == 0) {
				if ($advmode_status != "enabled") {
					$advmode_status = "disabled";
				}
				cfg_set($cfg, "system.advanced.mode", $advmode_status);
			}

			cfg_set($cfg, "system.latitude", $latitude);
			cfg_set($cfg, "system.longitude", $longitude);
			cfg_save($cfg, $cfg_file);
			cfg_set_modified($cfg_file);
			$modified = 1;
		}
	}
}

if ($modified || !isset($hostname))
{
	$def_host_name = $board_name;
        if (strlen($board_subtype)) { $def_host_name += " " + $board_subtype; }
	$hostname = cfg_get_def($cfg, "resolv.host.1.name", $def_host_name);
}
if ($modified || !isset($adminname))
{
	$adminname = cfg_get_def($cfg, "users.1.name", $adminname);
}
if ($modified || !isset($rousername))
{
	$rousername = cfg_get_def($cfg, "users.2.name", $rousername);
}
if ($modified || !isset($ro_status))
{
	$ro_status = cfg_get_def($cfg, "users.2.status", $ro_status);
}

if ($modified || !isset($date_status))
{
	$date_status = cfg_get_def($cfg, "system.date.status", "disabled");
}

if ($modified || !isset($systemdate))
{
	$systemdate = cfg_get_def($cfg, "system.date.timestamp", $systemdate);
}

if ($modified || !isset($timezone))
{
	$timezone = cfg_get_def($cfg, "system.timezone", "GMT");
}

if ($modified || !isset($resetb_status)) {
	$resetb_status = cfg_get_def($cfg, "system.button.reset", "enabled");
}

if ($modified || !isset($update_status)) {
	$update_status = cfg_get_def($cfg, "update.check.status", "enabled");
}

if ($radio_outdoor == 0) {
	if ($modified || !isset($advmode_status)) {
		$advmode_status = cfg_get_def($cfg, "system.advanced.mode", "disabled");
	}
}

if ($modified)
{
	$OldPassword = "";
	$NewPassword = "";
	$NewPassword2 = "";
	$roPassword = "";
}

if ($feature_logo == 1)
{
	$logo_url = cfg_get_def($cfg, "ls_logo.url", "http://");
	$logo_status = cfg_get_def($cfg, "ls_logo.status", "disabled");
}

$hostname = htmlspecialchars($hostname);
$adminname = htmlspecialchars($adminname);

$latitude = cfg_get_def($cfg, "system.latitude", $latitude);
$longitude = cfg_get_def($cfg, "system.longitude", $longitude);
if ($feature_gps == 1)
{
	$fd_gps_info = @fopen($gps_info_file, "r");
	if ($fd_gps_info != -1)
	{
		$gps_info = fgets($fd_gps_info, 255);
		$field = strtok($gps_info, ","); /** UTC */
		$field = strtok(","); /** utc_date */
		$field = strtok(","); /** validity */
		$field = strtok(","); /** num_satellites */
		$field = strtok(","); /** hdop */
		$field = strtok(","); /** dimensions */
		$field = strtok(","); /** latitude */
		if ($field)
		{
			$latitude = $field;
		}
		$field = strtok(","); /** longitude */
		if ($field)
		{
			$longitude = $field;
			$location_readonly = 1;
		}
		fclose($fd_gps_info);
	}
}

if (fileinode($firmware_file) != -1) {
	$is_fw_exist = 1;
} else {
	$is_fw_exist = 0;
}

if ($is_fw_exist || $is_new_fw_uploaded) {
	if ($is_new_fw_uploaded) {
		@unlink($firmware_file);
                @unlink($eula_html);
	} else {
		$fwfile = $firmware_file;
	}

	$res = fw_validate($fwfile);
	if (strlen($res) > 0) {
		if (fileinode($progress_file) != -1) {
         		$error_msg = dict_translate("Please Wait. Uploading Firmware...");
		} else {
			$error_msg = $res;
			@unlink($fwfile);
			@unlink($firmware_file);
                        @unlink($eula_html);
                }
	} else {
		$newfwversion = fw_extract_version($firmware_file);
		if (fw_is_thirdparty($firmware_file)) {
			$firmware_third_party_warning = dict_translate("warn_third_party_firmware|WARNING: Uploaded Firmware is from third-party, make sure you're familiar with the recovery procedure!");
		}
		if (fw_is_short_passwd($newfwversion, cfg_get_def($cfg, "users.1.password", "\$1\$"))) {
                	if (strlen($firmware_third_party_warning) > 0) {
                        	$firmware_third_party_warning += "<BR>";
                        }
			$firmware_third_party_warning += dict_translate("warn_short_passwd_firmware|WARNING: Uploaded Firmware does not support long passwords. Please make sure the current password is up to 8 symbols in length!");
		}
		if (fw_is_downgrade($newfwversion, $fwversion)) {
                	if (strlen($firmware_third_party_warning) > 0) {
                        	$firmware_third_party_warning += "<BR>";
                        }
			$firmware_third_party_warning += dict_translate("warn_downgrade_firmware|NOTE: It's your own responsibility to keep your network secure after airOS downgrade.");
		}
	}
}

include("lib/system.tmpl");
>
