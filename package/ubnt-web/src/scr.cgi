#!/sbin/cgi
<?
include("lib/settings.inc");
include("lib/system.inc");

$filename = $fname + ".sh";
$file = $fname + $status;
header("Content-Type: application/force-download");
header("Content-Disposition: attachment; filename=" + $filename);
passthru("cat /tmp/persistent/$file");
exit;
>
