#!/sbin/cgi
<?
include("lib/settings.inc");
include("lib/l10n.inc");
include("lib/help.inc");

if ($warn == 1) {
	$warning_msg = "<font color=\"red\">" + dict_translate("Warning") + ": </font>" + dict_translate("no_unii2_freq|UNII-2 band frequencies will be disabled in AP Repeater mode after revised UNII rules will be activated. Please change operating channel first.");
} elseif ($warn == 2) {
	$warning_msg = "<font color=\"red\">" + dict_translate("Warning") + ": </font>" + dict_translate("no_unii1_2_freq|UNII-1 and UNII-2 bands frequencies will be disabled after revised UNII rules will be deactivated. Please change operating channel first.");
} elseif ($warn == 3) {
	$warning_msg = "<font color=\"red\">" + dict_translate("Warning") + ": </font>" + dict_translate("err_incorrect_chanbw|Unsupported channel width. Please change operating channel width first.");
}

><!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"
 "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title><? echo dict_translate("Activate/Deactivate"); ></title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Pragma" content="no-cache">
<link rel="shortcut icon" href="FULL_VERSION_LINK/favicon.ico" >
<link href="FULL_VERSION_LINK/jquery-ui.css" rel="stylesheet" type="text/css">
<link href="FULL_VERSION_LINK/style.css" rel="stylesheet" type="text/css">
<script type="text/javascript" language="javascript" src="jsl10n.cgi?l=<? echo $ui_language; >&v=FULL_VERSION_LINK"></script>
<script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/jsval.js"></script>
<script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/util.js"></script>
<script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/system.js"></script>
<script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.js"></script>
<script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.ui.js"></script>
<script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.cookie.js"></script>
<script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.utils.js"></script>
<script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.blockUI.js"></script>
<script type="text/javascript">
$(document).ready(function(){
<? if ($action == "Activate") {>
                $('#info').load("<? echo localize_help("unii.html");>");
<? else {>
		$('#info').html("<h3><? echo dict_translate("Revised UNII Rules");></h3>"+
                "<p><? echo dict_translate("Please enter valid company name and key to deactivate revised UNII rules.");></p>");
<? }>
             });
</script>
</head>
<body class="popup">
	<form id="unii" name="unii" enctype="multipart/form-data" action="<? echo $PHP_SELF;>" method="POST">
	<table cellspacing="0" cellpadding="0" align="center" class="popup">
        <tr>
        <td class="wraptext" colspan="2">
		<div name="info" id="info" class="wraptext"></div>
                <div name="unii2" id="unii2" class="wraptext">
                <? echo $warning_msg;>
                </div>
	</td>
        </tr>
        <tr>
        <td colspan="2">
        <?include("lib/error.tmpl");>
        </td>
        </tr>
<? if ((($radio1_caps & $radio_cap_new_grant_certified)) && ($country == 840 || $country == 630) && ($board_fcc_unii_factory_enabled != 1)) { >
	<tr class="uniband_entry <? if (!($radio1_caps & $radio_cap_new_grant_certified)) > initial_hide <? } >">
		<td class="f"><? echo dict_translate("Company Name"); >:</td>
		<td><input type="text" name="uniband_name" id="uniband_name" <? if ($warn != 0) { echo " disabled=\"disabled\" "; }> ></td>
	</tr>
	<tr class="uniband_entry <? if (!($radio1_caps & $radio_cap_new_grant_certified)) > initial_hide <? } >">
		<td class="f"><? echo dict_translate("Key"); >:</td>
		<td><input type="text" name="uniband_key" id="uniband_key" <? if ($warn != 0) { echo " disabled=\"disabled\" "; }> ></td>
	</tr>
	<tr class="uniband_entry <? if (!($radio1_caps & $radio_cap_new_grant_certified)) > initial_hide <? } >">
		<td align="center" colspan="2"><input type="button" id="unii_state" <? if ($warn != 0) { echo " disabled=\"disabled\" "; }> onClick="change_unii_state(<? 
                $state = "false"; if ($action == "Activate") {$state = "true";} echo "$soft_reboot_time,$state"; 
                >);" value="<? echo dict_translate($action); >" />
        <input type="button" id="btn_close" value="<? echo dict_translate("Close"); >" onClick="window.close()"></td>
	</tr>
<? } >
</table>

</form>
</body>
</html>
