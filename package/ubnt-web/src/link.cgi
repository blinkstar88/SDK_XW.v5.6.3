#!/sbin/cgi
<?
  SecureVar("cmd*");
  SecureVar("lines");
  include("lib/settings.inc");
  $cfg = @cfg_load($cfg_file);
  include("lib/l10n.inc");
  include("lib/link.inc");
  include("lib/misc.inc");
  include("lib/system.inc");
  include("/etc/board.inc");

  if ($cfg == -1) {
	  include("lib/busy.tmpl");
	  exit;
  }

  if ($radio_count < 1) {
	include("lib/linknoradio.tmpl");
	exit;
  }

  $netmode = cfg_get_def($cfg, "netmode", "bridge");

  $curr_iface = $wlan_iface;
  $curr_ifidx = get_wlan_index($curr_iface); 
  init_board_inc($curr_iface);
  
  $polling = cfg_get_def($cfg, "radio.$curr_ifidx.polling", "disabled");
  $polling_fh = cfg_get_def($cfg, "radio.$curr_ifidx.polling_fh", "0");

  if (strlen($wep_key_type) == 0) {
	$wep_key_type = 1;
  }

  $cfgwmode = cfg_get_wmode($cfg, $curr_iface);
  if (strlen($cfgwmode) == 0) {
	  $cfgwmode = "sta";
  }

  if (strlen($wmode) == 0) {
	  $wmode = $cfgwmode;
  }

	if (strlen($oldwmode) == 0) {
		$oldwmode = $wmode;
	}

	if (strlen($antenna_info) == 0) {
		$antenna_info = get_active_antenna($cfg, $curr_ifidx);
	} else {
		$antenna_info = get_antenna_index($antenna_info);
	}

	if (strstr($antenna_info,"|")) {
		$tok = strtok($antenna_info,"|");
		$antenna_idx = $tok;
		$tok = strtok("|");
		$antenna_gain = $tok;
	}

	if ($radio["ant_builtin_$antenna_idx"] == 1) {
		$cable_loss = 0;
	}
	if (strlen($cable_loss) == 0) {
		$cable_loss = get_cable_loss($cfg, $curr_ifidx, $cable_loss);;
	}

	if ($radio["ant_chains_$antenna_idx"] > 0) {
		$radio_chains = $radio["ant_chains_$antenna_idx"];
	}
	else {
		$radio_chains = $radio["chains"];
	}

  if (strlen($country) == 0)
  {
  	if (strlen($old_country) == 0) {
		$country = cfg_get_country($cfg, $curr_iface, $country);
		$old_country = $country;
        } else {
		$country = $old_country;
	}
  }

	$showHideIndoor = 0;
	if ($radio["ieee_mode_a"]) {
		if (requiresCE($country) || $country == 756) {
			$showHideIndoor = 1;
		}
	}

  $eirp_status = cfg_get_def($cfg, "system.eirp.status", "");
  $eirp_first_run = 0;
  if (strlen($eirp_status) == 0) {
  	$eirp_first_run = 1;
	if (has_builtin_antenna() != 1) {
       		$eirp_status = "enabled";
  	} else {
       		$eirp_status = "disabled";
        }
  }

  $force_obey_regulatory = force_obey_regulatory($country);

	if ($REQUEST_METHOD == "POST")
	{
		if (isset($rate_module)) {
			cfg_set($cfg, "radio.rate_module", $rate_module);
		}
		
		if ($wmode == "ap" || $wmode == "aprepeater")
		{
                	if (!isset($mac_acl_policy)) {
                        	$mac_acl_policy = cfg_get_mac_acl_policy($cfg, $curr_ifidx, $mac_acl_policy);
                        }
		}

		if ($radio["legacy"] != 1)
		{
			if (strlen($ieee_mode) == 6)
			{
				if (40 == $chanbw)
				{
					$ieee_mode += "40";
					if (strstr($wmode, "ap") > 0)
					{
						$ieee_mode += $extchannel;
					}
				}
				else
				{
					$ieee_mode += "20";
				}
			}
		}

		if ($cc != "changed")
		{
			$netwarning = 0;
			if ($netmode == "router" &&
				  (get_wmode_type($cfgwmode) != get_wmode_type($wmode)))
			{
				$netwarning = 1;
			}

			/* common variables */
			set_wmode($cfg, $curr_iface, $wmode, $wds_chkbox, $chan_freq, $macclone);

			set_essid($cfg, $curr_ifidx, $essid);
			set_country($cfg, $curr_ifidx, $country, $radio["subsystemid"]);

			if (strlen($obey_regulatory_status)) {
				$obey_regulatory_status = "enabled";
			} else {
				$obey_regulatory_status = "disabled";
			}
			set_obey($cfg, $curr_ifidx, $obey_regulatory_status);
		        cfg_set($cfg, "system.eirp.status", $eirp_status);
                        $eirp_first_run = 0;

			$rtxpower = intval($txpower);
			if ($rtxpower < $radio["txpower_min"]) {
				$rtxpower = $radio["txpower_min"];
			}
			set_txpower($cfg, $curr_ifidx, $rtxpower);

			if ($radio["legacy"] != 1)
			{
				if (strstr($ieee_mode, "11ng") > 0)
				{
					$forbiasauto = 1;
				}
				else
				{
					$forbiasauto = 0;
				}
				if (strstr($ieee_mode, "ht40") > 0)
				{
					if (strstr($wmode, "sta") > 0)
					{
						$cwm_mode = 1;
					}
					else
					{
						$cwm_mode = 2;
					}
				}
				else
				{
					$cwm_mode = 0;
				}
				$cwm_enable = 0;
				cfg_set($cfg, "radio.$curr_ifidx.cwm.mode", $cwm_mode);
				cfg_set($cfg, "radio.$curr_ifidx.forbiasauto", $forbiasauto)
				cfg_set($cfg, "radio.$curr_ifidx.cwm.enable", $cwm_enable);
			}
			set_ieee_mode($cfg, $curr_ifidx, $ieee_mode);
			if ($radio["ieee_mode_a"]) {
				if (!requiresCE($country) && $country != 40 && $country != 756) {
					$dfs_status = get_status($dfs);
				}
				else {
					$dfs_status = "enabled";
				}
				cfg_set($cfg, "radio.$curr_ifidx.dfs.status", $dfs_status);
			}
			set_ack_distance($cfg, $curr_iface, $ackdistance, $ieee_mode);
			set_rate($cfg, $curr_ifidx, $rate, $rate_auto);
			cfg_set($cfg, "radio.$curr_ifidx.mcastrate", $mcast_rate);
			cfg_set($cfg, "radio.$curr_ifidx.chanbw", $chanbw);
			set_authtype($cfg, $curr_ifidx, $authtype);

			set_antenna($cfg, $curr_ifidx, $antenna_idx);
			if (strlen($antenna_gain) && strlen($cable_loss)) {
				set_antenna_gain($cfg, $curr_ifidx, $antenna_gain, $cable_loss);
			}

			set_channel_scan_list($cfg, $curr_ifidx, $channel_scan_list);
			if (IsSet($scan_channels)) {
				set_scan_channels($cfg, $curr_ifidx, $scan_channels);
			}
			set_hide_indoor($cfg, $hideindoor);

			/* mode specific */
			if ($wmode == "sta")
			{
				set_apmac($cfg, $curr_ifidx, $apmac);
				set_hide_ssid($cfg, $curr_ifidx, "");
				
			}
			else
			{
				$apmac = "";
				set_apmac($cfg, $curr_ifidx, $apmac);
				set_hide_ssid($cfg, $curr_ifidx, $hidessid);
				if ($wmode == "aprepeater")
				{
					set_wds_info($cfg, $curr_ifidx, $wds_auto, $peer1, $peer2, $peer3, $peer4, $peer5, $peer6);
				}
				else {
					set_wds_info($cfg, $curr_ifidx, "disabled", "", "", "", "", "", "");
				}
				set_mac_acl($cfg, $curr_ifidx, $mac_acl_status);
				set_mac_acl_policy($cfg, $curr_ifidx, $mac_acl_policy);
			}

			set_security($cfg, $curr_iface, $curr_ifidx, $security, $wep_key_length, $wmode);
			if ($security == "wep")
			{
				set_def_wep_key_id($cfg, $curr_ifidx, $wep_key_id);
				set_wep_key($cfg, $curr_ifidx, $wep_key_id, $wep_key, $wep_key_type);
			}
			elseif (substr($security, 0, 3) == "wpa")
			{
				if ($wmode == "ap" || $wmode == "aprepeater")
				{
					set_wpa_ap($cfg, $curr_ifidx, $wpa_auth, $wpa_key,
						$radius_auth_ip, $radius_auth_port, $radius_auth_secret, 
                                                $radius_acct_ip, $radius_acct_port, $radius_acct_secret, 
                                                $radius_acct_status);
				}
				else
				{
					set_wpa_sta($cfg, $curr_ifidx, $wpa_auth, $wpa_key, $wpa_eap,
						$wpa_inner, $wpa_ident, $wpa_user, $wpa_passwd, $apmac);
				}
			}
                        elseif ($wmode == "ap" || $wmode == "aprepeater")
                        {
				set_radius_macacl($cfg, $curr_iface, $curr_ifidx, $radius_mac_acl_status,
						$radius_auth_ip, $radius_auth_port, $radius_auth_secret, 
                                                $radius_acct_ip, $radius_acct_port, $radius_acct_secret, 
                                                $radius_acct_status, $radius_mac_acl_format, $radius_mac_acl_passwd);
                        }

			cfg_save($cfg, $cfg_file);
			cfg_set_modified($cfg_file);
			$message = dict_translate("Configuration saved");
		}
		else
		{
			$secwarningwpa = 0;
			$secwarningwep = 0;
			if ($wmode == "aprepeater" &&
				substr($security, 0, 3) == "wpa")
			{
				$security = "none";
				$secwarningwpa = 1;
			} elseif (($wmode == "sta" || $wmode == "ap") &&
				substr($security, 0, 3) == "wep" &&
				($oldwmode == "aprepeater"))
			{
				$secwarningwep = 1;
			}
			$oldwmode = $wmode;

			$txpower = cfg_get_txpower($cfg, $curr_ifidx, $txpower);
			if (strlen($obey_regulatory_status)) {
				$obey_regulatory_status = "enabled";
			} else {
				$obey_regulatory_status = "disabled";
			}
			if ($old_country != $country)
			{
				if ($country == 511) {
					$obey_regulatory_status = "disabled";
				}
			}
			if ($radio["ieee_mode_a"]) {
				$dfs = get_status($dfs);
			}

			$channel_scan_list = cfg_get_channel_scan_list($cfg, $curr_ifidx, $channel_scan_list);
			if (!IsSet($scan_channels)) {
				$scan_channels = cfg_get_scan_channels($cfg, $curr_ifidx, $scan_channels);
			}
			if (!IsSet($wds_chkbox)) {
				$wds_chkbox = cfg_get_def($cfg, "wireless.$curr_ifidx.wds.status", "disabled");
			}
                        
			$essid = htmlspecialchars($essid);
			$wpa_key = htmlspecialchars($wpa_key);
			$wpa_ident = htmlspecialchars($wpa_ident);
			$wpa_user = htmlspecialchars($wpa_user);
			$wpa_passwd = htmlspecialchars($wpa_passwd);
			$radius_auth_secret = htmlspecialchars($radius_auth_secret);

			if ($wmode == "ap" || $wmode == "aprepeater")
			{
				$include_page="ap";
				include("lib/linkap.tmpl");
				exit;
			}
			if ($wmode == "sta")
			{
				$include_page="sta";
				include("lib/linksta.tmpl");
				exit;
			}
		}
	}

  /* retrieve common variables */
  $essid = cfg_get_essid($cfg, $curr_ifidx, $essid);
  $hidessid = cfg_get_hide_ssid($cfg, $curr_ifidx, $hidessid);
  $hideindoor = cfg_get_hide_indoor($cfg, $hideindoor);
  $ieee_mode = cfg_get_ieee_mode($cfg, $curr_iface, $ieee_mode);
  if ($radio["ieee_mode_a"]) {
	  $dfs = cfg_get_dfs($cfg, $curr_iface);
  }

	if ($radio["ant_builtin_$antenna_idx"] != 1) {
		$cable_loss = get_cable_loss($cfg, $curr_ifidx, $cable_loss);
	} else {
		$cable_loss = 0;
	}

  $country = cfg_get_country($cfg, $curr_iface, $country);
  $old_country = $country;
  $txpower = cfg_get_txpower($cfg, $curr_ifidx, $txpower);
  $rtxpower = intval($txpower);
  if ($rtxpower <= $radio["txpower_max"]) {
	  $txpower = $rtxpower;
  } else {
	  $txpower = $radio["txpower_max"];
  }
  $obey_default = cfg_get_obey_default($cfg, $curr_iface);
  $obey_regulatory_status = cfg_get_obey($cfg, $curr_iface, $obey_default);
  if (($eirp_first_run != 0) && (has_builtin_antenna() != 1)) {
  	$obey_regulatory_status = "disabled";
  }
  if ($force_obey_regulatory == 1) {
  	$obey_regulatory_status = "enabled";
  }
  $rate_auto = cfg_get_def($cfg, "radio.$curr_ifidx.rate.auto", "enabled");
  $rate = cfg_get_rate($cfg, $curr_iface, $rate);
  $mcast_rate = cfg_get_def($cfg, "radio.$curr_ifidx.mcastrate", $mcast_rate);
  $chanbw = cfg_get_def_ne($cfg, "radio.$curr_ifidx.chanbw", 0);
  if ($chanbw == 0) {
      exec("/bin/iwpriv wifi0 get_chanbw | sed -e 's/^.*://' 2>&1", $ret);
	  $chanbw = $ret;
      if ($wmode == "sta" && 20 == $ret) {
	      $chanbw = 40;
      }
  }
  $timings = get_timings($ieee_mode, $chanbw, $radio["caps"] & $radio_cap_fast_clock);
  $sltconst = $timings[0];
  $ackdistance = cfg_get_ackdistance($cfg, $curr_iface, $sltconst);

  $authtype = cfg_get_authtype($cfg, $curr_ifidx, $authtype);
  $wep_key_id = cfg_get_def_wep_id($cfg, $curr_ifidx, $wep_key_id);
  $wep_key = cfg_get_wep_key($cfg, $curr_ifidx, $wep_key_id, $wep_key);
  $wep_key_length = cfg_get_wep_key_length($cfg, $curr_ifidx, $wep_key_length);
  $wep_key_type = 1;
  if (strlen($wep_key) > 2 && "s:" == substr($wep_key, 0, 2)) {
        $wep_key_type = 2;
        $wep_key = substr($wep_key, 2, strlen($wep_key) - 2);
  }

  $essid = htmlspecialchars($essid);
  $wpa_key = htmlspecialchars(cfg_get_wpa_key($cfg, $curr_ifidx, $wmode, $wpa_key));

  $wpa_auth = cfg_get_wpa_auth($cfg, $curr_ifidx, $wmode, $wpa_auth);
  $wpa_eap = cfg_get_wpa_eap($cfg, $curr_ifidx, $wmode, $wpa_eap);
  $wpa_inner = cfg_get_wpa_inner($cfg, $curr_ifidx, $wpa_inner);
  $wpa_ident = htmlspecialchars(cfg_get_wpa_ident($cfg, $curr_ifidx, $wpa_ident));
  $wpa_user = htmlspecialchars(cfg_get_wpa_user($cfg, $curr_ifidx, $wpa_user));
  $wpa_passwd = htmlspecialchars(cfg_get_wpa_passwd($cfg, $curr_ifidx, $wpa_passwd));

  $radius_auth_ip = cfg_get_radius_ip($cfg, $curr_ifidx, "auth", $radius_auth_ip);
  $radius_auth_port = cfg_get_radius_port($cfg, $curr_ifidx, "auth", "1812");
  $radius_auth_secret = htmlspecialchars(cfg_get_radius_secret($cfg, $curr_ifidx, "auth", "secret"));
  $radius_acct_ip = cfg_get_radius_ip($cfg, $curr_ifidx, "acct", $radius_acct_ip);
  $radius_acct_port = cfg_get_radius_port($cfg, $curr_ifidx, "acct", "1813");
  $radius_acct_secret = htmlspecialchars(cfg_get_radius_secret($cfg, $curr_ifidx, "acct", "secret"));
  $radius_acct_status = cfg_get_def($cfg, "aaa."+$curr_ifidx+".radius.acct.1.status", $radius_acct_status);
  $radius_mac_acl_status = cfg_get_def($cfg, "aaa."+$curr_ifidx+".radius.macacl.status", $radius_mac_acl_status);
  $radius_mac_acl_format = htmlspecialchars(cfg_get_def($cfg, "aaa."+$curr_ifidx+".radius.macacl.format", $radius_mac_acl_format));
  $radius_mac_acl_passwd = cfg_get_def($cfg, "aaa."+$curr_ifidx+".radius.macacl.password.status", $radius_mac_acl_passwd);
  if (strlen($radius_mac_acl_passwd) == 0) {
  	$radius_mac_acl_passwd = "enabled";
  }

  $security = cfg_get_security($cfg, $curr_iface, $security, $wmode);

  $channel_scan_list = cfg_get_channel_scan_list($cfg, $curr_ifidx, $channel_scan_list);
  $scan_channels = cfg_get_scan_channels($cfg, $curr_ifidx, $scan_channels);
  $wds_chkbox = cfg_get_def($cfg, "wireless.$curr_ifidx.wds.status", "disabled");
  
  $rate_module = cfg_get_def($cfg, "radio.rate_module", "atheros");

  if ($wmode == "sta" || $feature_ap == 0 || $radio1_station_only == 1) {
	  $apmac = cfg_get_apmac($cfg, $curr_ifidx, $apmac);
	  $macclone = cfg_get_def($cfg, "wireless.$curr_ifidx.macclone", $macclone);
	  $include_page="sta";
	  include("lib/linksta.tmpl");
  } else {
		if ($wmode == "aprepeater") {
			if (strstr($security, "wpa") > 0) {
				$wmode = "ap";
			} else {
				$info = get_wds_info($cfg, $curr_ifidx);
				$wds_auto = $info[0];
				$peer1 = $info[1];
				$peer2 = $info[2];
				$peer3 = $info[3];
				$peer4 = $info[4];
				$peer5 = $info[5];
				$peer6 = $info[6];
			}
		}
		$chan_freq = cfg_get_def($cfg, "radio.$curr_ifidx.freq", $chan_freq);
		$mac_acl_status = cfg_get_mac_acl($cfg, $curr_ifidx, $mac_acl_status);
		$mac_acl_policy = cfg_get_mac_acl_policy($cfg, $curr_ifidx, $mac_acl_policy);
		$include_page="ap";
		include("lib/linkap.tmpl");
  }
>
