#!/sbin/cgi
<?
include("lib/settings.inc");
$cfg = @cfg_load($cfg_file);

if (!isset($dlgAdminname))
	{
		$dlgAdminname = cfg_get_def($cfg, "users.1.name", $dlgAdminname);
	}
>
<script type="text/javascript" language="javascript">

function update_controls() {
	var p1 = $('#dlgNewPassword').val();
	var p2 = $('#dlgNewPassword2').val();
	var verdict = $('#dlgPasswordVerdict > span').text();

	var valid = true;
	if (p1 == p2 && p1 == 'ubnt')
		valid = false;
	if (verdict == 'Too short')	
		valid = false;

	var change_btn = $(".ui-dialog-buttonset > :button:contains(Change)");
	change_btn.enable(valid);
	if (valid)
		change_btn.removeClass("ui-state-disabled");
	else
		change_btn.addClass("ui-state-disabled");
}

$(document).ready(function() {
	var ps_opts = {
			'shortPass' : '<span style="color:DarkRed "><? echo dict_translate("Too short");?>',
			'badPass' : '<span style="color:Red"><? echo dict_translate("Weak");?>',
			'goodPass' : '<span style="color:DarkOrange "><? echo dict_translate("Normal");?>',
			'strongPass' : '<span style="color:Green"><? echo dict_translate("Strong");?>'
	};
	$('#dlgNewPassword').pStrength($('#dlgAdminname'), $('#dlgPasswordVerdict'), ps_opts);
	$('#dlgNewPassword').keyup(update_controls);
	$('#dlgNewPassword2').keyup(update_controls);
});
</script>

<table border="0" cellpadding="0" cellspacing="0" align="center" width="350" class="fixed">
<col width="35%" />
<col width="45%" />
<col width="20%" />
	<tr id="errortbl_dlg" class="initial_hide">
		<td colspan="3">
			<div id="errorpad" class="err-msg round-corner">
				<table cellspacing="0" cellpadding="0" id="errortbl_dlg" class="error-msg">
					<tr>
						<td valign="middle">
							<div id="error_msg"></div>
						</td>
					</tr>
				</table>
			</div>
		</td>
	</tr>
	<tr>
		<td colspan="3">&nbsp;</td>
	</tr>
	<tr>
		<td colspan="3"><font color="red"><label><? echo dict_translate("Default Password must be changed to apply configuration changes!"); ></label></td>
	</tr>
	<tr>
		<td colspan="3"><input type="hidden" name="adminname" id="dlgAdminname" value="<?echo $dlgAdminname>"></td>
	</tr>
	<tr>
		<td colspan="3">&nbsp;</td>
	</tr>
	<tr class="passwdchange">
		<td><? echo dict_translate("Current Password"); >:</td>
		<td><input type="password" autocomplete="off" class="config i_adminpasswd" name="OldPassword" id="dlgOldPassword" maxlength="63" value="<? echo $dlgOldPassword;>" realname="<? echo dict_translate("Current Password"); >">
		<td colspan="3">&nbsp;</td>
	</tr>
	<tr>
		<td colspan="3">&nbsp;</td>
	</tr>
	<tr class="passwdchange">
		<td>
			<label for="NewPassword"><? echo dict_translate("New Password"); >:</label>
		</td>
		<td>
			<input type="password" class="config i_adminpasswd" name="NewPassword" id="dlgNewPassword" maxlength="63" value="<? echo $dlgNewPassword;>" realname="<? echo dict_translate("New Password"); >">
		</td>
		<td>
			&nbsp;<span id="dlgPasswordVerdict" name="dlgPasswordVerdict"></span>
		</td>
	</tr>
	<tr>
		<td colspan="3">&nbsp;</td>
	</tr>
	<tr class="passwdchange">
		<td>
			<label for="NewPassword2"><? echo dict_translate("Verify New Password"); >:</label>
		</td>
		<td>
			<input type="password" autocomplete="off" class="config i_adminpasswd" name="NewPassword2" id="dlgNewPassword2" maxlength="63" value="<? echo $dlgNewPassword2;>" equals="dlgNewPassword" realname="<? echo dict_translate("msg_passwd_verify|New password for verification"); >">
		</td>
		<td colspan="3">&nbsp;</td>
	</tr>
	<tr>
</table>
