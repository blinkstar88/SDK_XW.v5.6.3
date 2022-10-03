#!/usr/bin/python
# -*- coding: utf-8 -*-
# run this script with param 'off' to change placeholders back to original values

from __future__ import print_function

import re
import codecs
import locale
import subprocess
import glob
from sets import Set
import collections
import sys

class AirOsStringsFormat:
	def __init__(self):
		self.placeholdersOn = True
		self.rg = re.compile(u'^\"(.*?)\"\s?=\s?\"(.*)\";$')
		self.sInvalidValues = Set([
			u'msg_agree_with_terms',
			u'msg_to_change_country',
			u'msg_change_in_system',
			u'msg_invalid_entry_at_line',
			u'msg_invalid_remote_port',
			u'eirp_off_obey_off',
			u'err_freq_list_status',
			u'fw_ver_str',
			u'license_agreement2',
			u'msg_airview_startup',
			u'msg_airview_warning',
			u'msg_airview_window_autoclose',
			u'msg_firmware_upgrading',
			u'msg_not_compatible_speedtest',
			u'msg_request_timed_out',
			u'ubnt_dl_str',
			u'warn_emergency_info',
			u'warn_emergency_info_flash',
			u'warn_link_settings_changed',
			u'msg_upl_file_too_big',
			u'warn_device_role_changed'
		])
		self.dValuesToReplace = {
			u'{1}'										: { u'%{1}' : True },
			u'&#8482;'									: { u'%{tm}' : True },
			u'%d'										: { u'%{int}' : True },
			u'%s'										: { u'%{string}' : True },
			u'&nbsp;'									: { u'%{nb_space}' : True },
			u'<br>'										: { u'%{linebreak}' : True },
			u'<br/>' 									: { u'%{linebreak}' : False },
			u'<br />' 									: { u'%{linebreak}' : False },
			u'<br / >' 									: { u'%{linebreak}' : False },
			u'<span id=\\"counter\\">'							: { u'%{span_counter}' : True },
			u'<span style=\\"color: red\\">'						: { u'%{span_red}' : True },
			u'</span>'									: { u'%{span_end}' : True },
			u'</ span>'									: { u'%{span_end}': False },
			u'<a href=\\"http://www.ubnt.com/\\">'						: { u'%{url_ubnt_com}' : True },
			u'<a href=\\"link.cgi\\">'							: { u'%{url_wireless}' : False },
			u"<a href='/link.cgi'>"								: { u'%{url_wireless}' : True },
			u'<a href=\\"ubnt.cgi\\">'							: { u'%{url_ubnt}' : True },
			u'<a href=\\"network.cgi\\">'							: { u'%{url_network}' : True },
			u'<a href=\\"/system.cgi\\">'							: { u'%{url_system}' : True },
			u'<a href=\\"http://www.ubnt.com/support/downloads\\" target=\\"_blank\\">'	: { u'%{url_downloads}' : True },
			u'<a href=\\"#\\" onclick=\\"openPage(\'support.cgi\'); return false;\\">'	: { u'%{url_support}' : True },
			u'<a class=\\"jnlp\\" href=\\"#\\">'						: { u'%{url_jnlp}' : True },
			u'</a>'										: { u'%{url_end}' : True },
			u'</ a>'									: { u'%{url_end}' : False }

		}

        def change_placeholders(self, value):
                new_val = value
		for k, v in self.dValuesToReplace.items():
			for kv, vb in v.items():
				if self.placeholdersOn:
					new_val = new_val.replace(k, kv)
				elif v[kv]:
					new_val = new_val.replace(kv, k)
                return new_val

        def load_translation(self, fname):
                translation = {}
		with codecs.open(fname, 'r', encoding='utf-8') as f:
			for line in f:
                            m = re.match(self.rg, line)
                            if m:
                                key = unicode(m.group(1).strip())
                                val = unicode(m.group(2).strip())
                                translation[key] = {}
                                translation[key]["orig"] = val

                                if key in self.sInvalidValues:
                                    if val: # only non-empty strings
                                        translation[key]["new"] = self.change_placeholders(val)
                                        translation[key]["pcount"] = translation[key]["new"].count('%')
                return translation

        def write_translation(self, base, translation, fname):
            s = []
            for k, v in translation.iteritems():
                if not v["orig"]:
                    continue
                value = v["orig"]
                if "new" in v and v["new"]:
                    if k in base and "new" in base[k]:
                        if base[k]["pcount"] == v["pcount"]:
                            value = v["new"]

                s.append(u'"{}" = "{}";'.format(k, value))

            locale.setlocale(locale.LC_ALL, "C")
            s.sort(cmp=locale.strcoll)

            with codecs.open(fname, 'w', encoding='utf-8') as f:
                    f.write(u'\n'.join(map(lambda x: x, s)))

def main():
	sf = AirOsStringsFormat()
	
	if (len(sys.argv) == 2):
		placeholders = sys.argv[1]
		if ('off' == placeholders):
			print(placeholders)
			sf.placeholdersOn = False

        eng = sf.load_translation('English.strings')
	for file in glob.glob("*.strings"):
#            if file != 'English.strings':
		print(file)
	        tr = sf.load_translation(file)
                sf.write_translation(eng, tr, file)
	
if __name__ == "__main__":
	main()
