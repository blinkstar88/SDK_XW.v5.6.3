#!/usr/bin/python
# -*- coding: utf-8 -*-
from __future__ import print_function

import sys
import os
import subprocess
import re
import codecs
import locale

class AirOsDict:
	def __init__(self):
		self.rg = re.compile(u'^(.*?)\s?=>\s?(.*)$')
		self.d = {
			u'Main'                : { u'EN' : u'Main' },
			u'Secondary'           : { u'EN' : u'Secondary' },
			u'Adaptive'            : { u'EN' : u'Adaptive' },
			u'External'            : { u'EN' : u'External' },
			u'Internal'            : { u'EN' : u'Internal' },
			u'External + Internal' : { u'EN' : u'External + Internal' },
			u'Antenna 1'           : { u'EN' : u'Antenna 1' },
			u'Antenna 2'           : { u'EN' : u'Antenna 2' },
			u'Diversity'           : { u'EN' : u'Diversity' },
			u'Not specified'       : { u'EN' : u'Not specified' },
			u'Feed only'           : { u'EN' : u'Feed only' }
		}

	def add_key(self, lang, key, val):
		if key not in self.d:
			self.d[key] = {}
			self.d[key][lang] = val
		else:
			print(u'duplicate: %s' % key, file = sys.stderr)

	def load_en(self):
		cmd = subprocess.Popen('./dict.sh', shell=True, stdout=subprocess.PIPE)
		for line in cmd.stdout:
			m = re.match(self.rg, line)
			if m:
				self.add_key(u'EN', unicode(m.group(1).decode('utf-8').strip()), unicode(m.group(2).decode('utf-8').strip()))
			else:
				print(u'Invalid translation: %s' % line, file = sys.stderr)
		del self.d[u'$radio[ant_name_$i]']

	def get_item(self, key):
		if key in self.d:
			return self.d[key]

		# key might be replaced with shorter version
		for k, v in self.d.iteritems():
			if v[u'EN'] == key:
				return v

		return None

	def get_translations(self):
		lang_root = './lib/lang'

		trans = {}
		for root, subfolders, files in os.walk(lang_root):
			for subfolder in subfolders:
				folder = lang_root + '/' + subfolder
				tran = subfolder[-2:]
				for r, s, f in os.walk(folder):
					for file in f:
						trans[tran] = folder + '/' + file
		return trans

	def add_translation(self, lang, key, val):
		item = self.get_item(key)
		if item:
			item[lang] = val
		elif key[-1] == ':':
			new_key = key[:-1]
			item = self.get_item(new_key)
			if item:
				item[lang] = val[:-1].strip()

	def load_translation(self, lang, fpath):
		with codecs.open(fpath, 'r', encoding='utf-8') as f:
			for line in f:
				m = re.match(self.rg, line)
				if m:
					self.add_translation(lang, unicode(m.group(1).strip()), unicode(m.group(2).strip()))
				else:
					print(u'Invalid translation: %s' % line, file = sys.stderr)

	def write_translation(self, fpath, fending, l):
		fname = os.path.basename(fpath);
		with codecs.open(fname + fending, 'w', encoding='utf-8') as f:
			f.write(u'\n'.join(map(lambda x: x, l)))

	def save_translation(self, lang, fpath):
		u = []
		m = []
		r = []
		for k, v in self.d.items():
			if lang in v:
				u.append(u'{} => {}'.format(k, v[lang]))
				if v[lang] == v[u'EN']:
					r.append(u'{} => {}'.format(k, v[lang]))
			else:
				m.append(u'{} => {}'.format(k, v[u'EN']))

		# sort at the end (complete lines) to match sorting from shell
		locale.setlocale(locale.LC_ALL, "C")
		u.sort(cmp=locale.strcoll)
		m.sort(cmp=locale.strcoll)
		r.sort(cmp=locale.strcoll)

		self.write_translation(fpath, '.updated', u);
		self.write_translation(fpath, '.missing', m);
		self.write_translation(fpath, '.revise', r);

        @staticmethod
        def escape_lang_str(s):
            return s.replace('\\', '\\\\').replace('"', '\\"')

        def save_lang_strings(self, lang, translation):
            s = []
            for k, v in self.d.items():
                key = k

                if lang in v:
                    val = v[lang]
                else:
                    val = ''

                key = AirOsDict.escape_lang_str(key)
                val = AirOsDict.escape_lang_str(val)
                s.append(u'"{}" = "{}";'.format(key, val))

                locale.setlocale(locale.LC_ALL, "C")
                s.sort(cmp=locale.strcoll)

		self.write_translation(translation, '.strings', s);

def main():
	dc = AirOsDict()
	dc.load_en()

        dc.save_lang_strings(u'EN', 'English')
	tr = dc.get_translations()
	for lang, translation in tr.iteritems():
		dc.load_translation(lang, translation)
#		dc.save_translation(lang, translation)
                dc.save_lang_strings(lang, translation)

if __name__ == "__main__":
	main()
