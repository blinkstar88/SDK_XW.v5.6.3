#!/sbin/cgi
<?
include("lib/settings.inc");
$cmd = $cmd_wstalist;
PassThru(EscapeShellCmd($cmd));
>
