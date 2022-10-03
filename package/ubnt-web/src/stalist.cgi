#!/sbin/cgi
<?
include("lib/settings.inc");
$cfg = @cfg_load($cfg_file_bak);
include("lib/l10n.inc");
include("lib/misc.inc");
include("lib/link.inc");

$global_ack = 0;
$global_distance = 0;

$wmode = cfg_get_wmode($cfg, $wlan_iface);
$polling = cfg_get_def($cfg, "radio.1.polling", "disabled");

if (($polling == "enabled") && ($wmode == "ap" || $wmode == "aprepeater")) {
    $airmax_on = "true";
} else {
    $airmax_on = "false";
}


$autoack = cfg_get_def($cfg, "radio.1.ack.auto", "disabled");
if ($autoack == "enabled") {
	$noack = cfg_get_def($cfg, "radio.1.pollingnoack", "0");
	$airsync = cfg_get_def($cfg, "radio.1.airsync.status", "disabled");
	if ($polling == "enabled" &&
		($wmode == "ap" || $wmode == "aprepeater") &&
		($noack == "1" || $airsync == "enabled")) {
		$autoack = "disabled";
	}
	else {
		$global_ack = get_current_ack();
		$global_distance = get_current_distance();
	}
}
>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title><? echo dict_translate("Associated Stations"); ></title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Pragma" content="no-cache">
<link rel="shortcut icon" href="FULL_VERSION_LINK/favicon.ico" >
<link href="FULL_VERSION_LINK/style.css" rel="stylesheet" type="text/css">
</head>

<body class="popup">
<script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.js"></script>
<script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.utils.js"></script>
<script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.dataTables.js"></script>
<script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.ui.js"></script>
<script type="text/javascript" src="FULL_VERSION_LINK/util.js"></script>
<script type="text/javascript" language="javascript">

var ab5BeamAngles = [
    '<img src="FULL_VERSION_LINK/images/ab5-p39.png" title="+39 <? echo dict_translate("degrees");>">',
    '<img src="FULL_VERSION_LINK/images/ab5-p26.png" title="+26 <? echo dict_translate("degrees");>">',
    '<img src="FULL_VERSION_LINK/images/ab5-p13.png" title="+13 <? echo dict_translate("degrees");>">',
    '<img src="FULL_VERSION_LINK/images/ab5-0.png" title="0 <? echo dict_translate("degrees");>">',
    '<img src="FULL_VERSION_LINK/images/ab5-m13.png" title="-13 <? echo dict_translate("degrees");>">',
    '<img src="FULL_VERSION_LINK/images/ab5-m26.png" title="-26 <? echo dict_translate("degrees");>">',
    '<img src="FULL_VERSION_LINK/images/ab5-m39.png" title="-39 <? echo dict_translate("degrees");>">',
    '<img src="FULL_VERSION_LINK/images/ab5-bcast.png" title="bcast">',
];

var sl_global = {
	'wlan_iface' : '<? echo $wlan_iface; >',
	'autoack' : ('<? echo $autoack; >' == 'enabled'),
	'ack' : '<? echo $global_ack; >',
	'distance' : '<? echo $global_distance; >',
	'airmax_on' : <? echo $airmax_on; >,
	'phased_array' : ('<? echo $feature_phased_array; >' == '1'),
	'beam_angles' : ab5BeamAngles, /* Currently we only have Airbeam-5, need selectivty in future */
	'_': '_'
};

</script>
<script type="text/javascript" src="FULL_VERSION_LINK/stalist.js"></script>

<br>
<form action="<?echo $PHP_SELF;>" method="GET">
<table cellspacing="0" cellpadding="0" align="center">
	<tr>
		<td class="change">
			<input type="button" id="_refresh" value="<? echo dict_translate("Refresh"); >">
		</td>
	</tr>
	<tr>
		<td>
			<table id="sta_list" class="listhead dataTables_head" cellspacing="0" cellpadding="0">
				<thead>
					<tr>
						<th><? echo dict_translate("Station MAC"); >&nbsp;&nbsp;&nbsp;</th>
						<th><? echo dict_translate("Device Name"); >&nbsp;&nbsp;&nbsp;</th>
						<th class="disable_sort" title="<? echo dict_translate("Remote Signal Strength"); >" >
							<span id="tx_signal_sort">
								<? echo dict_translate("TX Signal"); >,&nbsp;dBm
							</span>
							<br/>
							<a id="tx_details" href="#"><? echo dict_translate("Combined"); ></a>
						</th>
						<th class="disable_sort" title="<? echo dict_translate("Local Signal Strength"); >" >
							<span id="rx_signal_sort">
								<? echo dict_translate("RX Signal"); >,&nbsp;dBm
							</span>
							<br/>
							<a id="rx_details" href="#"><? echo dict_translate("Combined"); ></a>
						</th>
						</th>
						<th><? echo dict_translate("Noise"); >,&nbsp;&nbsp;<br>dBm</th>
						<th class="initial_hide"><? echo dict_translate("Latency"); >,&nbsp;&nbsp;<br>ms</th>
						<th class="initial_hide"><? echo dict_translate("Beam"); >&nbsp;&nbsp;&nbsp;</th>
						<th class="disable_sort initial_hide">
							<span id="distance_sort">
								<? echo dict_translate("Distance"); >,
							</span>
							<br/>
							<a id="distance_label" href="#"><? echo dict_translate("miles"); ></a>
						</th>
						<th><? echo dict_translate("TX/RX"); >,&nbsp;&nbsp;<BR>Mbps</th>
						<th><? echo dict_translate("CCQ"); >,&nbsp;&nbsp;<BR>%</th>
						<th><? echo dict_translate("Connection"); >&nbsp;&nbsp;<BR><? echo dict_translate("Time"); ></th>
						<th><? echo dict_translate("Last IP"); >&nbsp;&nbsp;&nbsp;</th>
						<th><? echo dict_translate("Action"); >&nbsp;</th>
						<th>&nbsp;</th>
						<th>&nbsp;</th>
					</tr>
				</thead>
				<tbody>
				</tbody>
			</table>
		</td>
	</tr>
</table>
</form>
</body>
</html>
