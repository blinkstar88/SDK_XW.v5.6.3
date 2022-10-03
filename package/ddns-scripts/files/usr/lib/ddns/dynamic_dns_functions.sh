# /usr/lib/dynamic_dns/dynamic_dns_functions.sh
#
# Written by Eric Paul Bishop, Janary 2008
# Distributed under the terms of the GNU General Public License (GPL) version 2.0
#
# This script is (loosely) based on the one posted by exobyte in the forums here:
# http://forum.openwrt.org/viewtopic.php?id=14040


CFG_FILE=/etc/ddns.conf


config_get_sections()
{
	SECTIONS=`awk '($1=="config" && $2=="service") { print $3 }' $CFG_FILE`
}


config_get_options()
{
	section_id=$1

	eval `awk '{OFS="="} { if ($1=="config") { if ($3 ~ /'$section_id'/) { triggered=1; } else { triggered=0; } }
							if (triggered) {
								if ($1 == "option") {
									print $2, $3;
									print "ALL_OPTION_VARIABLES", "\"""$ALL_OPTION_VARIABLES " $2"\"";
								}
							}
						 }' $CFG_FILE`

}


get_registered_ip()
{
	echo $( nslookup "$domain" 2>/dev/null |
			awk '{ if ($1 ~ /^Name/) { if ($2 ~ /'$domain'/) { triggered=1; } else { triggered=0; } }
					if (triggered) {
						if ($1 ~ /^Address/) {
							print $0;
						}
					}
				}' |
			grep -m1 -o "$ip_regex")
}


get_current_ip()
{

	#if ip source is not defined, assume we want to get ip from wan 
	if [ "$ip_source" != "interface" ] && [ "$ip_source" != "web" ] && [ "$ip_source" != "script" ] && [ "$ip_source" != "none" ]
	then
		ip_source="network"
	fi

	if [ "$ip_source" = "network" ]
	then
		if [ -z "$ip_network" ]
		then
			ip_network="wan"
		fi
	fi

	current_ip='';
	if [ "$ip_source" = "none" ]
	then
		# do nothing
		current_ip=''
	elif [ "$ip_source" = "network" ]
	then
		network_get_ipaddr current_ip "$ip_network" || return
	elif [ "$ip_source" = "interface" ]
	then
		current_ip=$(ifconfig $ip_interface | grep -o 'inet addr:[0-9.]*' | grep -o "$ip_regex")
	elif [ "$ip_source" = "script" ]
	then
		# get ip from script
		current_ip=$($ip_script)
	else
		# get ip from web
		# we check each url in order in ip_url variable, and if no ips are found we use dyndns ip checker
		# ip is set to FIRST expression in page that matches the ip_regex regular expression
		for addr in $ip_url
		do
			if [ -z "$current_ip" ]
			then
				current_ip=$(echo $( wget -O - $addr 2>/dev/null) | grep -o "$ip_regex")
			fi
		done

		#here we hard-code the dyndns checkip url in case no url was specified
		if [ -z "$current_ip" ]
		then
			current_ip=$(echo $( wget -O - http://checkip.dyndns.org 2>/dev/null) | grep -o "$ip_regex")
		fi
	fi

	echo "$current_ip"
}


verbose_echo()
{
	if [ "$verbose_mode" = 1 ]
	then
		echo $1
	fi
}


syslog_echo()
{
	if [ "$use_syslog" = 1 ]
	then
		echo $1|logger -t ddns-scripts-$service_id
	fi
}


start_daemon_for_all_ddns_sections()
{
	SECTIONS=""
	config_get_sections

	for section in $SECTIONS
	do
		/usr/lib/ddns/dynamic_dns_updater.sh $section 0 > /dev/null 2>&1 &
	done
}


monotonic_time()
{
	local uptime
	read uptime < /proc/uptime
	echo "${uptime%%.*}"
}
