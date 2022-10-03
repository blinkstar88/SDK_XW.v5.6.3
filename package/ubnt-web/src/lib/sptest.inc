<?

Function loadModule
(
	$command = "/usr/bin/lsmod | grep spdtst";
	exec($command, $lines, $res);
	if (count($lines) == 0) {
		exec("/usr/bin/insmod /lib/modules/`uname -r`/spdtst.ko", $lines, $res);
		exec($command, $lines, $res);
		if (count($lines) == 0) {
			writeStatus(-1, dict_translate("Failed to load speedtest module."));
			exit;
		}
	}
);

Function getSessionId
(
	exec("cat /proc/net/spdtst/stctl | grep Session", $lines, $ret);
	if ($ret != 0) {
		return 0;
	}

	if (ereg("Session ID: ([[:digit:]]+), State: ([[:digit:]]+), Flags:", $lines[0], $regs)) {
		return $regs[1];
	}

	return 0;
);

Function actionStart
(
	global $ticket, $target, $duration, $direction;
	loadModule();

	# check if other speedtest is not running
	$old = getSessionId();
	
	if ($old == $ticket) {
		actionStatus();
	} elseif ($old > 0) {
		actionStop($old);
	}

	# init local
	exec("echo " + $ticket + " init " + $target + " > /proc/net/spdtst/stctl", $lines, $res);
	$res = getRetVal($res);
	if ($res != 0) {
		writeStatus(-1, dict_translate("Failed to initialize speedtest module."));
		exit;
	}

	if (IsSet($duration) == 1) {
		exec("echo " + $ticket + " duration " + $duration + " > /proc/net/spdtst/stctl", $lines, $res);
		$res = getRetVal($res);
		if ($res != 0) {
			writeStatus(-1, dict_translate("Failed to set test duration."));
			exit;
		}

		exec("echo " + $ticket + " direction " + $direction + " > /proc/net/spdtst/stctl", $lines, $res);
		$res = getRetVal($res);
		if ($res != 0) {
			writeStatus(-1, dict_translate("Failed to set test direction."));
			exit;
		}
	}

	# start test
	# Delay execution for 1 second, to send response to browser
	bgexec(1, "echo " + $ticket + " start " + " > /proc/net/spdtst/stctl");

	actionStatus();
#no return here
);

Function actionSlave
(
	global $ticket;

	loadModule();
	$old = getSessionId();
	if ($old > 0) {
		actionStop($old);
	}

	exec("echo " + $ticket + " slave > /proc/net/spdtst/stctl", $lines, $res);
	$res = getRetVal($res);
	if ($res != 0) {
		writeStatus(-1, dict_translate("Failed to initialize remote host."));
		exit;
	}

	return 0;
);

Function actionStop $ticket
(
	exec("echo " + $ticket + " stop > /proc/net/spdtst/stctl", $lines, $res);
	$res = getRetVal($res);
	return $res;
);

Function actionRemote
(
	global $ticket, $target, $port, $login, $passwd;
	doRequest($target, $port, "/sptest_action.cgi?action=slave&ticket=" + $ticket, $login, $passwd);
);

Function actionStatus
(
	$session = -1;
	$state = -1;
	$duration = 0;
	$flags = 0;
	$tx = 0;
	$rx = 0;

	$session_regex = "Session ID: ([[:digit:]]+), State: ([[:digit:]]+), Flags: 0x([[:digit:]]{4}), Out Dev: (.*), In Dev: (.*)";
	$duration_regex = "Duration: ([[:digit:]]+), Data size:";
	$tx_regex = "[[:digit:]]+pps \(([[:digit:]]+)bps\) - time : [[:digit:]]+us";
	$rx_regex = "[[:digit:]]+pps \(([[:digit:]]+)bps\) - time : [[:digit:]]+us";

	$lines = @file("/proc/net/spdtst/stctl");
	$count = count($lines);
	$in = "ath0";
	$out = "ath0";

	if ($count > 10)
	{
		if (ereg($session_regex, $lines[1], $regs))
		{
			$session = $regs[1];
			$state = $regs[2];
			$flags = HexDec($regs[3]);
			$out = $regs[4];
			$in = $regs[5];
		}

		if ($state == 10) { # completed
			if (ereg($duration_regex, $lines[2], $regs)) {
				$duration = $regs[1];
			}
			if (ereg($tx_regex, $lines[7], $regs)) {
				$tx = doubleval($regs[1]) / 1048576.0;
			}
			if (ereg($rx_regex, $lines[10], $regs)) {
				$rx = doubleval($regs[1]) / 1048576.0;
			}
		}
		else {
			# Get rx/tx from system
			$lines = @file("/proc/net/dev");
			$count = count($lines);
			$in_regex = ".*$in: *([[:digit:]]+) *([[:digit:]]+) *([[:digit:]]+) *([[:digit:]]+) *([[:digit:]]+) *([[:digit:]]+) *([[:digit:]]+) *([[:digit:]]+) *([[:digit:]]+)";
			$out_regex = ".*$out: *([[:digit:]]+) *([[:digit:]]+) *([[:digit:]]+) *([[:digit:]]+) *([[:digit:]]+) *([[:digit:]]+) *([[:digit:]]+) *([[:digit:]]+) *([[:digit:]]+)";
			while($count > 0) {
				$count--;
				if ($in == $out) {
					if (strlen(strstr($lines[$count], $out)) > 0) {
						if (ereg($out_regex, $lines[$count], $regs)) {
							$tx = doubleval($regs[9]) / 131072.0;
							$rx = doubleval($regs[1]) / 131072.0;
							$count = 0;
						}
					}
				}
				else {
					if (strlen(strstr($lines[$count], $out)) > 0) {
						if (ereg($out_regex, $lines[$count], $regs)) {
							$tx = doubleval($regs[9]) / 131072.0;
						}
					}
					else {
						if (strlen(strstr($lines[$count], $in)) > 0) {
							if (ereg($in_regex, $lines[$count], $regs)) {
								$rx = doubleval($regs[1]) / 131072.0;
							}
						}
					}
				}
			}
		}
	}

	header("Content-Type: application/json");
	echo "{ \n";
	echo "\t\"status\" : 0,\n";
	echo "\t\"message\" : \"Success.\",\n";
	echo "\t\"session\" : $session,\n";
	echo "\t\"state\" : $state,\n";
	echo "\t\"flags\" : $flags,\n";
	echo "\t\"duration\" : $duration,\n";
	echo "\t\"tx\" : $tx,\n";
	echo "\t\"rx\" : $rx,\n";
	echo "\t\"microtime\" : \"" + Microtime() + "\"\n";
	echo "}\n";
	exit;
);

Function exitErr $err
(
	switch($err) {
		case -1;
			writeStatus($err, dict_translate("msg_not_compatible_speedtest|Specified device is not<br>compatible for speedtest."));
			break;
		case -2;
			writeStatus($err, dict_translate("msg_request_timed_out|Request timed out—<br>remote IP is not accessible."));
			break;
		case -3;
			writeStatus($err, dict_translate("msg_invalid_remote_port|Invalid remote port or<br>web server is not running."));
			break;
		case -4;
			writeStatus($err, dict_translate("Remote connection failed ($res)."));
			break;
		case -5;
			writeStatus($err, dict_translate("Login credentials are invalid."));
			break;
		case -6;
			writeStatus($err, dict_translate("Invalid remote port specified."));
			break;
		default;
			writeStatus($err, dict_translate("Remote connection failed ($res)."));
			break;
	}
	exit;
);

>
