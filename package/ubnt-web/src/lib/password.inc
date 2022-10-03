<?
Function passChangeErr $err
(
	switch($err) {
		case -1;
			$msg = dict_translate("msg_passwords_dont_match|New passwords do not match!");
			break;
		case -2;
			$msg = dict_translate("msg_password_empty|New password cannot be empty!");
			break;
		case -3;
			$msg = dict_translate("msg_curr_passwd_wrong|Current password is wrong.");
			break;
		case -4;
			$msg = dict_translate("msg_all_passw_empty|Please enter current and new passwords.");
			break;
		default;
			$msg = dict_translate("Undefined error.");
			break;
	}

	return $msg;
);

Function validatePassword $OldPassword, $NewPassword, $NewPassword2
(
	global $cfg;

	if ((strlen($OldPassword) != 0) || (strlen($NewPassword) != 0) ||
		(strlen($NewPassword2) != 0))
	{
		if ($NewPassword != $NewPassword2)
		{
			return -1;
		}
		elseif ((strlen($NewPassword) == 0) || (strlen($NewPassword2) == 0)) {
			return -2;
		}
		else
		{
			$passwd = cfg_get($cfg, "users.1.password");
			if ($passwd == "")
			{
				$passwd = "$1$CCtKtXoV$t3YJh1/OXd0qiuIDLsxKT0";
			}

			$crypted = crypt($OldPassword, $passwd);
			if ($passwd != $crypted)
			{
				return -3;
			}
			else
			{
				return 0;
			}
		}
	}
	return -4;
);

Function changePassword $NewPassword
(
	global $cfg;

	$crypted_new = crypt($NewPassword);

	cfg_set($cfg, "users.1.password", $crypted_new);
	cfg_set($cfg, "users.1.status", "enabled");

	check_default_passwd($cfg);	

	return $crypted_new;

);

>
