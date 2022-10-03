#!/sbin/cgi
<?
include("/etc/board.inc");
include("lib/utils.inc");

$session = get_session_id($$session_id, $AIROS_SESSIONID, $HTTP_USER_AGENT);
if (isset($session) && strlen($session) == 32) {
	Exec("/bin/ma-deauth /tmp/.sessions.tdb " + $session);
	if (!isset($redirect) || $redirect != "false") {
		Header("Location: /login.cgi");
	}
}
>
