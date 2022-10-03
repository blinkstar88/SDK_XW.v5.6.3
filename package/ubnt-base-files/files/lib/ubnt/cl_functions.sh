#!/bin/sh

# Various common function for Cloud Agent

SLEEP_TIME=5
DSS_RSA_GEN_TIME=70
BOARD_INFO_FILE="/etc/board.info"
BOARD_INC_FILE="/etc/board.inc"
CFG_FILE="/tmp/running.cfg"
ERR_LOCK=200


cl_escape_json() {
	printf %s "$1" | tr '\b\f\n\r\v\t' ' ' | sed -r 's/(["\\\/])/\\\1/g'
}

cl_escape_base64() {
	printf %s "$1" | tr -d '\b\f\n\r\v\t'
}

cl_format_error() {
	printf '{"rc":%d,"rc_msg":"%s"}' $1 "$(cl_escape_json "$2")"
}

cl_md5sum() {
	md5sum "$1" | cut -d ' ' -f 1
}

cl_get_file_val() {
	local FILE=$1
	local KEY=$2
	local ESC=$3
	local FIND=${ESC}${KEY}
	
	local line=$(grep "^${FIND}=" ${FILE})
	local val=${line//${FIND}=/}		
	val=${val//[\";\n]/}
				
	printf %s ${val}
}

cl_get_file_int() {

	local val=$(cl_get_file_val $1 $2 $3)

	val=${val//[!0-9]/}
	
	if [ -z $val ]; then
		val=0
	fi
		
	printf %d ${val}
}

cl_get_board_val() {
	cl_get_file_val $BOARD_INFO_FILE $1
}

cl_get_board_int() {
	cl_get_file_int $BOARD_INFO_FILE $1
}

cl_get_board_inc_val() {
	cl_get_file_val $BOARD_INC_FILE $1 '$'
}

cl_get_board_inc_int() {
	cl_get_file_int $BOARD_INC_FILE $1 '$'
}

cl_get_cfg_val() {
	cl_get_file_val $CFG_FILE $1
}

cl_get_cfg_int() {
	cl_get_file_int $CFG_FILE $1
}

cl_close_pipes() {
	exec 0<&-
	exec 1>&-
	exec 2>&-
}                                                                                                                                                                                             

cl_lock() {
	lockdir="$1"
	pidfile="$1/lock.pid"

	if mkdir "$lockdir" 2>/dev/null; then
		# lock succeeded, install signal handlers to remove lock on exit
		trap 'rm -rf "$lockdir"' 0 1 2 3 15

		# store current pid
		echo "$$" >"$pidfile"

	else
		# lock failed, check if locking PID is alive
		lockingpid=$(cat "$pidfile")
				 
		# if cat wasn't able to read the file anymore, another instance probably is
		# about to remove the lock -- exit, we're *still* locked

		if [ $? != 0 ]; then
			cl_format_error "$ERR_LOCK" "lock failed, another instance is running"
			exit 0

		elif ! kill -0 $lockingpid 2>/dev/null; then
			# lock is stale, remove it and restart
			rm -rf "$lockdir"
			cl_lock "$lockdir"

		else
			# lock is valid
			cl_format_error "$ERR_LOCK" "lock failed, another instance with PID $lockingpid is running"
			exit 0

		fi

	fi

}
