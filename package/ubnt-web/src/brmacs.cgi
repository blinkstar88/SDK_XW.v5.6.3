#!/sbin/cgi
<?
include("lib/settings.inc");
include("lib/l10n.inc");

if (IsSet($brmacs) && $brmacs == "y") {
	PassThru($cmd_brmacs);
	exit;
}
>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
      <head>
	    <title><? echo get_title($cfg, dict_translate("Bridge Table")); ></title>
	    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
	    <meta http-equiv="Pragma" content="no-cache">
	    <link rel="shortcut icon" href="FULL_VERSION_LINK/favicon.ico" >
	    <link href="FULL_VERSION_LINK/style.css" rel="stylesheet" type="text/css">
      </head>

      <body class="popup">
		<script type="text/javascript" language="javascript" src="jsl10n.cgi?l=<? echo $ui_language; >&v=FULL_VERSION_LINK"></script>
	    <script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.js"></script>
	    <script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.dataTables.js"></script>
	    <script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.ui.js"></script>
	    <script type="text/javascript" src="FULL_VERSION_LINK/common.js"></script>
	    <script type="text/javascript" src="FULL_VERSION_LINK/brmacs.js"></script>
	    <br>
	    <form action="<?echo $PHP_SELF;>" method="GET">
	    <table cellspacing="0" cellpadding="0" align="center">
		  <tr><td>
			      <table  id="brmacs_tbl" cellspacing="0" cellpadding="0" class="listhead dataTables_head">
				    <thead>
				    <tr> 
					  <th><? echo dict_translate("Bridge"); ></th>
					  <th><? echo dict_translate("MAC Address"); ></th>
					  <th><? echo dict_translate("Interface"); ></th>
					  <th><? echo dict_translate("Aging Timer"); ></th>
				    </tr>
				    </thead>
			      </table>
		  </td></tr>
		  <tr>
			<td class="change">
			      <input type="button" id="_refresh" value=" <? echo dict_translate("Refresh"); > ">
			</td>
		  </tr>
	    </table>
	    </form>
      </body>
</html>
