#!/sbin/cgi
<?
# parameters:
# action - start, slave, stop, remote, status
# ticket - speed test session id
# target - destination (slave) ip address
# port   - remote port
# login  - username
# passwd - password

include("lib/settings.inc");
include("lib/remote.inc");
include("lib/sptest.inc");
include("lib/l10n.inc");

if ($action == "start")
{
	actionStart();
}
elseif ($action == "slave")
{
	actionSlave();
}
elseif ($action == "stop")
{
	actionStop($ticket);
}
elseif ($action == "remote")
{
	actionRemote();
}
elseif ($action == "status")
{
	actionStatus();
}

writeStatus(0, "Success.");
>
