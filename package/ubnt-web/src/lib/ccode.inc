<?
	$ccode_include_fail = 0;
	if (fileSize("/etc/ccodes.inc") < 1) {
		global $cmd_regdomain;
		$def_cntry_output = "<option value=\"840\">United States</option>";

		$modestring = " -j -L";
		$modestring += " -D " + $radio["devdomain"];
		if ($radio["ccode_locked"] != 0) {
			$modestring += " -u";
		}
		if ($radio["ccode_fixed"] == 1 || $radio["ccode_locked"] != 0) {
			$modestring += " "+$radio["ccode"];
		} else {
			$modestring += " "+$country;
		}
		$fd_in = popen ($cmd_regdomain + $modestring, "r");
		if ($fd_in == -1) {
			echo $def_cntry_output;
			$ccode_include_fail = 1;
		}
		$fd_out = fopen("/etc/ccodes.inc", "w");
		if ($fd_out == -1) {
			echo $def_cntry_output;
			$ccode_include_fail = 1;
		}
		$cont = 1;
		$size = 0;
		while ($cont > 0) {
			$buf = fgets($fd_in, 1024);
			if (strlen(strstr($buf, "Serbia and Montenegro")) <= 0) {
				fputs($fd_out, $buf);
				$len = strlen($buf);
				$size += $len;
				if (!$len) { $cont = 0; }
			}
		}
		if ($size == 0) {
			echo $def_cntry_output;
			$ccode_include_fail = 1;
		}

		fputs($fd_out, "<option value=511>Compliance Test</option>");

		fclose($fd_out);
		pclose($fd_in);
	}
	if (!$ccode_include_fail) {
		include("/etc/ccodes.inc"); }
>
