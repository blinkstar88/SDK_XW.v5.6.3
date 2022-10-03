#!/sbin/cgi
<?
include("lib/system.inc");
header("Content-type: text/javascript");
$build = fw_get_build();

if (isset($HTTP_IF_NONE_MATCH) && $HTTP_IF_NONE_MATCH == $build) {
	header("HTTP/1.1 304 Not Modified");
	exit;
} else {
	header("ETag: $build");
}

include("lib/settings.inc");
$cfg = @cfg_load($cfg_file);
include("lib/l10n.inc");
>
l10n_start = "<? echo dict_translate("Start"); >";
l10n_stop = "<? echo dict_translate("Stop"); >";

jsval_l10n_err_form = "<? echo dict_translate("msg_missing_values|Please enter/select values for the following fields"); >:";
jsval_l10n_err_select = '<? echo sprintf(dict_translate("msg_invalid_select|Please select a valid \"%s\""), "%FIELDNAME%"); >';
jsval_l10n_err_enter = '<? echo sprintf(dict_translate("msg_invalid_enter|Please enter a valid \"%s\""), "%FIELDNAME%"); >';

pingtest_l10n_timeout = "<? echo dict_translate("timeout"); >";

system_l10n_change = "<? echo dict_translate("Change"); >";
system_l10n_upload = "<? echo dict_translate("Upload"); >";

traceroute_l10n_msg_unable_initialize = "<? echo dict_translate("Unable to initialize request"); >";
traceroute_l10n_fail = "<? echo dict_translate("Failed!"); >";

var js_translations = <? dict_dump_jsmap();>;

function jsTranslate(word)
{
	if (js_translations[word])
	{
		return js_translations[word];
	}
	var i = word.indexOf('|');
	if (i > 0) {
		key=word.slice(0,i);
		if (js_translations[key])
		{
			return js_translations[key];
		} else {
			return word.slice(i + 1, word.length);
		}
	}
	return word;
}
