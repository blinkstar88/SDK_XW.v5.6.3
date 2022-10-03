#!/sbin/cgi
<?
include("lib/settings.inc");
$cfg = @cfg_load($cfg_file);

include("lib/misc.inc");
include("lib/password.inc");

Function writeStatus $rc, $message
(
	header("Content-Type: application/json");
	echo "{ \"rc\" : $rc, \"message\" : \"$message\" }";
	exit;
);

$result = validatePassword($OldPassword, $NewPassword, $NewPassword2);
if ($result != 0) {
	$err_msg = passChangeErr($result);
	writeStatus($result, $err_msg);
}

$newPass = changePassword($NewPassword);
cfg_save($cfg, $cfg_file);
cfg_set_modified($cfg_file);

writeStatus(0, $newPass);

>
