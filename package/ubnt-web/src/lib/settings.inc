<?
$br_iface = "br0";
$wlan_iface = "ath0";
$eth0_iface = "eth0";
$eth1_iface = "eth1";

$lan_iface = $eth0_iface;
$wan_iface = $eth1_iface;

$cfg_file = "/tmp/system.cfg";
$cfg_file_bak = "/tmp/running.cfg";
$fwup_lock = "/tmp/fwup-started";
$test_lock_file = "/var/run/testmode";
$test_mode_time = 180;
$dfs_test_mode_time = (10 * 60);
$proc_modules="/proc/modules";
$cmd_cfgsave = "/sbin/cfgmtd -p /etc/ -w";
$cmd_cfgrestore = "/sbin/cfgmtd -p /etc/ -r -f " + $cfg_file;
$def_cfg_file = "/etc/default.cfg";
$cmd_cfgdef = "/sbin/cfgmtd -w -f " + $def_cfg_file;
$cmd_cfgfix = "/sbin/ubntconf -f";
$cmd_reboot = "/sbin/reboot";
$cmd_ebtables = "/sbin/ebtables ";
$cmd_iptables = "/sbin/iptables ";
$cmd_ip6tables = "/sbin/ip6tables ";
$cmd_regdomain = "regdomain ";
$cmd_scanparser = "/bin/scanparser ";
$cmd_brctl = "brctl ";
$cmd_log = "/bin/cat /var/log/messages";
$cmd_webping = "/bin/webping";
$cmd_pppstats = "/bin/pppstats -a -r ";
$cmd_wstalist = "/bin/wstalist";
$cmd_iwpriv = "/sbin/iwpriv";
$cmd_discover = "/usr/bin/discover";
$cmd_softrestart = "/usr/etc/rc.d/rc.softrestart ";
$cmd_bgnd = "/usr/bin/bgnd";
$cmd_ccode = "/bin/ccode.sh";
$cmd_uniband = "/bin/ubntbox dfs-unlock";
$cmd_brmacs = "/usr/bin/brmacs";
$cmd_trigger  = "/usr/bin/trigger_url";
$dss_priv_filename = "/etc/persistent/dropbear_dss_host_key";
$rsa_priv_filename = "/etc/persistent/dropbear_rsa_host_key";
$dss_rsa_gen_time = 70;
$simpinok_file = "/var/run/simpinok";
$gps_info_file = "/proc/sys/dev/ubnt_poll/gps_info";
$emerg_file = "/etc/persistent/emerg";
$emerg_crashlog_file = "/tmp/crashlog";
$progress_file = "/tmp/.dl-fw-progress";
$eula_url = "http://www.ubnt.com/eula/eula.html";
$eula_html = "/tmp/eula.html";
$update_info = "/tmp/.update-info.json";
$cmd_wget = "/usr/bin/wget";
$check_url = "http://www.ubnt.com/update/check.php";
$check_interval = 86400; # 24 hours
$cmd_md5sum = "/usr/bin/md5sum";
$firmware_file = "/tmp/fwupdate.bin";

$airview_tcp_port = 18888;

$soft_reboot_time=20;
$defip = "192.168.1.20";

$def_rate_module = "ath_rate_atheros";
$def_dfs = "enabled";

$db_sessions = "/tmp/.sessions.tdb";
$db_tickets = "/tmp/.tickets.tdb";

$netmode_bridge = 1;
$netmode_router = 2;
$netmode_soho = 4;
$netmode_3g = 8;
$radio1_legacy = 0;
$radio1_ccode_locked = 0;
$radio1_ccode_fixed = 0;
$feature_obey_rd = 1;
$radio1_antennas = 0;
$radio1_ant_builtin[0] = 0;
$feature_diversity = 0;
$feature_super_comp = 0;
$feature_channel_width = 1;
$feature_macclone = 0;
$feature_ratemodule = 0;
$feature_ap_scan = 1;
$feature_noiseim = 0;
$feature_qos = 0;
$feature_mcastrate = 0;
$feature_datarate = 1;
$feature_sta_channel = 0;
$feature_logo = 0;
$feature_ccq = 1;
$feature_ap = 1;
$feature_airview = 1;
$radio_cap_airmax_only = 2;    /*0x0000002*/
$radio_cap_11n_no_ht40 = 1024; /*0x0000400*/
$radio_cap_fast_clock = 8192;  /*0x0002000*/
$radio_cap_dfs_certified = 524288;  /*0x0080000*/
$radio_cap_new_grant_certified = 268435456; /*0x10000000*/
$radio1_feature_sta = 1;
$radio1_feature_ap = 1;

$lock_state_new_grant = 2;
$ipaliases_max = 8;
$default_max_mtu = 1524;

$radio1_name = "unknown";
$radio1_shortname = "unknown";
$oem_copyright = "txt_copyright|&copy; Copyright 2006-2015 Ubiquiti Networks, Inc.";
$logo_url = "http://www.ubnt.com";
$logo_pic = "airos_logo.png";

$radio["index"] = 1;
$radio["devdomain"] = 0;

include("/etc/board.inc");
include("lib/oem.inc");
>
