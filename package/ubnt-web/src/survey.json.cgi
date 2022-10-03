#!/sbin/cgi
<?
include("lib/settings.inc");

if (strlen($iface) == 0) {
	$iface = $wlan_iface;
}
PassThru("iwlist " + EscapeShellCmd($iface) + " scan " + $update + " | " + $cmd_scanparser);
>
