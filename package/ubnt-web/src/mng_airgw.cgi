<?
include("lib/settings.inc");
include("lib/remote.inc");
include("lib/link.inc");

$airgw_inc = "/etc/airgw.inc";

Function create_pfwd $wan, $fwd_to, $fwd_dport
(
	$rm_pfwd_file = "/tmp/.rm_airgw_pfwd";
	if (fileSize($rm_pfwd_file) > 0) {
		exec("/bin/sh $rm_pfwd_file", $lines, $res);
	}
	$fd = fopen($rm_pfwd_file, "w");
	if ($fd != -1) {
		fputs($fd, "iptables -t nat -D PREROUTING -i $wan -p TCP --dport $fwd_dport -j DNAT --to $fwd_to > /dev/null 2>&1\n");
		fclose($fd);
	}
	exec("iptables -t nat -A PREROUTING -i $wan -p TCP --dport $fwd_dport -j DNAT --to $fwd_to > /dev/null 2>&1", $lines, $res);
);

$cfg = @cfg_load($cfg_file);
if ($cfg == -1) {
         include("lib/busy.tmpl");
         exit;
}

if (fileSize($airgw_inc) > 0) {
	include($airgw_inc);

	if ($https == "true") {
		$scheme = "https";
	}
	else {
		$scheme = "http";
	}
	$fwd_dport = "81";
	$fwd_to = "$ip:$httpport";
	$wan_idx = get_role_index($cfg, "wan");
	$wan = cfg_get_def($cfg, "netconf.$wan_idx.devname", "ath0");

	$ppp_status = cfg_get_def($cfg, "ppp.status", "disabled");
	if ($ppp_status == "enabled") {
		$idx = 1;
		while ($idx <= 32) {
			$ppp_idx_status = cfg_get_def($cfg, "ppp.$idx.status", "disabled");
			if ($ppp_idx_status == "enabled") {
				$ppp_parent = cfg_get_def($cfg, "ppp.$idx.devname", "");
				if ($ppp_parent == $wan) {
					$wan = "ppp+";
					$idx = 32;
				}
			}
			$idx++;
		}
	}

	create_pfwd($wan, $fwd_to, $fwd_dport);

	$err = fetchCookies($ip, $httpport);
	if ($err) {
		include("lib/busy.tmpl");
		exit;
	}
	$cookie = buildCookieStr();
	$cfgfile = "/tmp/.trigger.txt";
	$user = cfg_get_def($cfg, "provisioning.username", "ubnt");
	$passw = cfg_get_def($cfg, "provisioning.password", "ubnt");
	writeConfig($cfgfile, $user, $passw);
	$cmd = "$cmd_trigger $cookie -c $cfgfile";
	$url = "$base_url/login.cgi";
	$full_cmd = "$cmd $url";
	exec($full_cmd, $lines, $res);
	unlink($cfgfile);

	SetCookie($session_key, $session_value, 0, "/", $HTTP_HOST, 1);
	$redir_url = "$scheme://$HTTP_HOST:$fwd_dport/index.cgi";
	Header("Location: " + $redir_url);
}
>
