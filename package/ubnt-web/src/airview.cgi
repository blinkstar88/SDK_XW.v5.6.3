<?
include("lib/settings.inc");
include("lib/l10n.inc");
$cfg = @cfg_load($cfg_file);

if (isset($start) && $start == 1) {
	header("Content-Type: application/json");
	/* check whether airview server is already running */
	exec("/sbin/airview status", $out, $retval);
	if ($retval != 0) {
		bgexec(2, "/sbin/airview web_start 1>/dev/null 2>/dev/null");
	}
	header("Location: airview.jnlp");
	exit;
}
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
	<title><? echo get_title($cfg, dict_translate("airView")); ?></title>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
	<meta http-equiv="Pragma" content="no-cache">
	<meta http-equiv="Expires" content="0">
	<meta http-equiv="Cache-Control" content="no-cache">
	<link rel="shortcut icon" href="FULL_VERSION_LINK/images/airview.ico" >
	<link href="FULL_VERSION_LINK/style.css" rel="stylesheet" type="text/css">
	<link href="FULL_VERSION_LINK/jquery-ui.css" rel="stylesheet" type="text/css">
	<script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/js/jquery.js"></script>
	<script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/js/jquery.cookie.js"></script>
</head>
<body class="popup">
	<br />
	<form action="#">
		<table cellspacing="0" cellpadding="0" align="center" style="width: 490px;" class="popup">
			<tr>
				<th><? echo dict_translate("airView"); ?> <? echo dict_translate("Spectrum Analyzer"); ?></th>
			</tr>
			<tr>
				<td>&nbsp;</td>
			</tr>
			<tr>
				<td align="center">
					<div class="ui-state-error ui-corner-all errors" style="display: block;"><a target="_blank" href="https://java.com/en/download/">Java Runtime Environment 1.6</a> (or above) is required on your client machine to use airView.</div>
				</td>
			</tr>
			<tr>
				<td align="center">
					<? echo dict_translate("msg_airview_warning|WARNING: Launching airView Spectrum Analyzer <br> <span style=\"color: red\">WILL TERMINATE</span> <br> all wireless connections on the device!"); ?><br /><br />
				</td>
			</tr>
			<tr>
				<td align="center">
					<br />
					<a href="airview.cgi?start=1"><img class="middle" height="32" width="32" src="FULL_VERSION_LINK/images/airview_32.png" border="0" /></a>
					<span style="font-size: 200%; vertical-align: middle;"><a href="airview.cgi?start=1"><? echo dict_translate("Launch airView"); ?></a> <br/></span>
				</td>
			</tr>
			<tr>
				<td align="center">
					<br /><br />
					<input type="button" value="<? echo dict_translate("Close"); ?>" onclick="window.close();">
				</td>
			</tr>
		</table>
	</form>
</body>
</html>
