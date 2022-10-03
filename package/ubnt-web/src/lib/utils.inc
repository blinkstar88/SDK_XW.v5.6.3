<?

Function get_session_id $id, $old_id, $agent (
	if (IsSet($id) && strlen($id) == 32) {
		return $id;
	}

	if ($agent == "Speedtest/0.1" && IsSet($old_id) && strlen($old_id) == 32) {
		return $old_id;
	}

	return "";
)

?>
