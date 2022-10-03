#!/sbin/cgi
<?
include("lib/settings.inc");
include("lib/misc.inc");
$cfg = @cfg_load($cfg_file);
$ipv6_enabled = get_ipv6_status($cfg);
include("lib/l10n.inc");
$route_regexp="([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)$";
$route6_regexp="([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([^[:space:]]+)$";
$page_title=dict_translate("Routing Table");
include("lib/ptable_head.tmpl");
>
<tr>
<th colspan="4"><? echo dict_translate("IPv4 Routes"); ></th>
</tr>
<tr>
<th><? echo dict_translate("Destination"); ></th><th><? echo dict_translate("Gateway IP"); ></th><th><? echo dict_translate("Netmask"); ></th><th><? echo dict_translate("Interface"); ></th>
</tr>
<?
flush();

Exec("/sbin/route -n", $lines, $result);

if ($result == 0) {
	$i = 2;
	$size = count($lines);
	while ($i < $size) {
		if (ereg($route_regexp,$lines[$i],$res)) {
			$f = devname2ifname($res[8]);
			echo "<tr><td class=\"str\">" + $res[1] + "</td><td class=\"str\">" + $res[2] + "</td>";
			echo "<td class=\"str\">" + $res[3] + "</td>";
			echo "<td>" + $f + "</td></tr>\n";
		}
		$i++;
	}
}

>
<?if ($ipv6_enabled) {>
</table>
</br>
<table cellspacing="0" cellpadding="0" class="listhead sortable" id="survey">
<tr>
<th colspan="3"><? echo dict_translate("IPv6 Routes"); ></th>
</tr>
<tr>
<th><? echo dict_translate("Destination"); ></th><th><? echo dict_translate("Gateway IP"); ></th><th><? echo dict_translate("Interface"); ></th>
</tr>
<?
UnSet($lines);
Exec("/sbin/route -A inet6 -n", $lines, $result);

if ($result == 0) {
	$i = 2;
	$size = count($lines);
	while ($i < $size) {
		if (ereg($route6_regexp,$lines[$i],$res)) {
			if ($res[7] != "lo") {
				$f = devname2ifname($res[7]);
				echo "<tr><td class=\"str\">" + $res[1] + "</td><td class=\"str\">" + $res[2] + "</td>";
				echo "<td>" + $f + "</td></tr>\n";
			}
		}
		$i++;
	}
}
>
<?}>
<?
include("lib/arp_tail.tmpl");
>
