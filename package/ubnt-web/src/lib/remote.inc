<?

$base_url = "";
UnSet($session_key);
UnSet($session_value);
UnSet($session_str);

Function writeStatus $status, $message
(
	header("Content-Type: application/json");
	echo "{ \"status\" : $status, \"message\" : \"$message\" }";
);

Function getRetVal $ret_val
(
	$res = (($ret_val & 65280) / 256);
	if ($res > 127) {
		$res -= 256;
	}
	return $res;
);

Function buildUrl $ip, $port, $force_ssl
(
	$scheme = "http";
	if ($port == 443 || $force_ssl) {
		$scheme += "s";
	}

	$url = "$scheme://$ip";
	if ($port != 80 && $port != 443) {
		$url += ":$port";
	}

	return $url;
);

Function writeConfig $fname, $login, $passwd
(
	$fd = fopen($fname, "w");
	if ($fd == -1) {
		writeStatus(-1, "Internal error.");
		exit;
	}
	fputs($fd, "-a=username=$login\n");
	fputs($fd, "-a=password=$passwd\n");
	fclose($fd);
);

Function parseHeaders $headers
(
	global $session_key, $session_value;
	$cookie_regex = "^Set-Cookie: ([AIROS_[:print:]]+)=([[:alnum:]]{32});";

	$i = 0;
	while ($i < count($headers)) {
		if (ereg($cookie_regex, $headers[$i], $regs)) {
			$session_key = $regs[1];
			$session_value = $regs[2];
			return 0;
		}
		$i++;
	}
	return $i;
);

# try to connect to http(s) server and extract session cookies
Function fetchCookies $ip, $port
(
	global $cmd_trigger, $base_url, $session_key;
	UnSet($session_key);

	$base_url = buildUrl($ip, $port, 0);
	$url = EscapeShellCmd($base_url + "/login.cgi");
	$full_cmd = "$cmd_trigger -p $url";

	exec($full_cmd, $lines, $res);
	$res = getRetVal($res);

	if ($res != 0) {
		$retry = 0;
		if ($res == -11) {
			# got redirect, will have to try new one (if that's login.cgi)
			$lcount = count($lines);
			if ($lcount > 0) {
				$new_url = $lines[$lcount - 1];
			}
			$rg_login = "(https?://$ip(:[[:digit:]]+)?)/login.cgi";
			if (IsSet($new_url) && ereg($rg_login, $new_url, $regs)) {
				$retry = 1;
				$base_url = $regs[1];
				$url = $new_url;
			}
		}
		elseif ($res == -6) {
			# need to retry with https
			$retry = 1;
			$base_url = buildUrl($ip, $port, 1);
			$url = "$base_url/login.cgi";
		}

		if ($retry != 0) {
			$full_cmd = "$cmd_trigger -p $url";
			exec($full_cmd, $lines, $res);
			$res = getRetVal($res);
		}
	}

	if ($res == 0) {
		# all good, grab cookies and get out
		parseHeaders($lines);
		if (!IsSet($session_key)) {
			return -1;
		}
	}
	elseif ((-28 <= $res) && ($res <= -21)) {
		return -2;
	}
	elseif ($res == -3) {
		return -3;
	}
	else {
		# "generic" (=unknown) error
		return -4;
	}

	return 0;
);

Function buildCookieStr
(
	global $session_key, $session_value;
	return " -d 'Cookie: $session_key=$session_value'";
);

Function doLogin $ip, $port, $login, $passwd
(
	global $cmd_trigger, $base_url, $session_str;

	$err = fetchCookies($ip, $port);
	if ($err) {
		return $err;
	}
	$session_str = buildCookieStr();

	$cfgfile = "/tmp/.trigger.txt";
	writeConfig($cfgfile, $login, $passwd);

	$cmd = $cmd_trigger + $session_str;
	$cmd += " -c $cfgfile";

	$url = EscapeShellCmd($base_url + "/login.cgi");
	$full_cmd = "$cmd $url";

	exec($full_cmd, $lines, $res);
	$res = getRetVal($res);

	unlink($cfgfile);
	if ($res == -11 && $lines[0] == "/index.cgi") {
		return 0;
	}

	if ($res == 0) {
		return -5;
	}

	if ((-28 <= $res) && ($res <= -21)) {
		return -2;
	}

	if ($res == -3) {
		return -3;
	}

	if ($res != -11 && $lines[0] != "/index.cgi") {
		return -1;
	}

	if ($lines[0] != "/index.cgi") {
		if (ereg("https?://$ip:?[[:digit:]]*/login.cgi", $lines[0], $regs)) {
			return -6;
		}
		else {
			return -1;
		}
	}

	return -1;
);

Function doLogout
(
	global $cmd_trigger, $session_str, $base_url;
	$url = EscapeShellCmd($base_url + "/logout.cgi");
	$cmd = "$cmd_trigger $session_str $url";
	exec($cmd, $lines, $res);
);

Function doRequest $ip, $port, $path, $login, $passwd
(
	global $cmd_trigger, $session_str, $base_url;

	$err = doLogin($ip, $port, $login, $passwd);
	if ($err != 0) {
		exitErr($err);
	}

	$url = $base_url + $path;
	$cmd = $cmd_trigger + $session_str;
	$cmd += " '" + $url + "'";
	exec($cmd, $lines, $res);
	$res = getRetVal($res);

	doLogout();
	if ($res == 0) {
		$i = 0;
		while ($i < count($lines)) {
			echo $lines[$i] + "\n";
			$i++;
		}
		exit;
	}
	else {
		return -1;
	}
);

>
