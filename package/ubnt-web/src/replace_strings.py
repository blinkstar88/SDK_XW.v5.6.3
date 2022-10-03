#!/usr/bin/python
# -*- coding: utf-8 -*-
from __future__ import print_function

import re
import codecs
import locale
import subprocess
import glob
import collections
import sys
import os

class AirOsReplaceStrings:
	def __init__(self):
		self.rg = re.compile(u'^\"(.*?)\"\s?=\s?\"(.*)\";$')
		self.dOldStringsFile = {}
		self.dNewStringsFile = {}
		self.dDifferences = {}
		self.dChangedKeys = {}
		self.sFilesToModify = set()
		self.dir_path = str(os.path.dirname(os.path.realpath(sys.argv[0])))
		self.dLang = {
			u'Česky' 		: 'cz_CZ',
			u'Deutsch' 		: 'de_DE',
			u'Français'		: 'fr_FR', 
			u'Italiano' 	: 'it_IT',
			u'Polski' 		: 'pl_PL',
			u'Português' 	: 'pt_PT',
			u'Español' 		: 'sp_SP',
			u'Türk' 		: 'tr_TR',
			u'中文（简体）' 	: 'zh_CN'
			}

	@staticmethod
	def add_lang_str(s):
		return s.replace('\\\\', '\\').replace('\\"', '"')

	@staticmethod
	def escape_lang_str(s):
		return s.replace('\\', '\\\\').replace('"', '\\"').replace("\\\\n", "\\\\n")

	def load_file(self, fname, d):
		if os.path.isfile(fname):
			with codecs.open(fname, 'r', encoding='utf-8') as f:
				for line in f:
					m = re.match(self.rg, line)
					if line.startswith('/*'):
						continue
					if m:
						key = unicode(m.group(1).strip())
						val = unicode(m.group(2).strip())

						key = AirOsReplaceStrings.add_lang_str(key)
						val = AirOsReplaceStrings.add_lang_str(val)

						d[key] = val

	def rgrep(self, pattern):
		rg = "(?:jsTranslate|dict_translate)\([\"'](?:(?:(?P<id>\w+)\|)?(?P<msg>" + re.escape(pattern) + "))[\"']\)"
		rg = rg.replace('\\"', '\\\\\\"')
		r = re.compile(rg)
		for root, subfolders, files in os.walk(self.dir_path):
			for fname in files:
				fname = os.path.join(root, fname)
				if os.path.isfile(fname) and not fname.endswith('py') and not fname.endswith('strings'):
					with open(fname) as f:
						for line in f:
							if r.search(line):
								self.sFilesToModify.add(fname)

	def save_modified_file(self, fname):
		with codecs.open(fname, 'r', encoding='utf-8') as fr, codecs.open(fname + '.new', 'w+', encoding='utf-8') as fw:
			for line in fr:
				for key, val in self.dDifferences.items():
					for ot, v in val.items():
						if ot == "new":
							nVal = v
						elif ot == "old":
							oVal = v
					
					nVal = AirOsReplaceStrings.escape_lang_str(nVal)

					rg = "(?:jsTranslate|dict_translate)\([\"'](?:(?:(?P<id>\w+)\|)?(?P<msg>" + re.escape(oVal) + "))[\"']\)"
					rg = rg.replace('\\"', '\\\\\\"')
					r = re.compile(rg)

					if r.search(line):
						#print(u' line: {}'.format(line))
						#print(u' nVal: {}'.format(repr(nVal)))
						line = re.sub(re.escape(oVal).replace('\\"', '\\\\\\"'), nVal, line)
						#print(u'file: {} \ncline: {}'.format(fname, line))
						#print('---')

				fw.write(line)

		os.remove(fname)
		os.rename(fname + '.new', fname)

	def collect_differences(self):
		for oKey, oVal in self.dOldStringsFile.items():
			for nKey, nVal in self.dNewStringsFile.items():
				if oKey == nKey:
					if oVal != nVal:
						self.dDifferences[nKey] = {}
						self.dDifferences[nKey]["old"] = oVal
						self.dDifferences[nKey]["new"] = nVal

		print('no_of_diff: {}'.format(len(self.dDifferences)))
		
	def collect_files(self):
		for key, val in self.dDifferences.items():
			for ot, v in val.items():
				if ot == 'old':
					self.rgrep(v)
		print('no_of_files: {}'.format(len(self.sFilesToModify)))

	def collect_changed_keys(self):
		for key, val in self.dNewStringsFile.items():
			if key not in self.dOldStringsFile:
				self.dChangedKeys[key] = val

		print('no_of_keys_diff: {}'.format(len(self.dChangedKeys)))
	#	for key, val in self.dChangedKeys.items():
	#		print(u'k:{} v:{}'.format(key, val))

	def replace_English(self, oldFname, newFname):

		self.load_file(oldFname, self.dOldStringsFile)
		self.load_file(newFname, self.dNewStringsFile)
		#print(u'old_file_size: {}'.format(len(self.dOldStringsFile)))
		#print(u'new_file_size: {}'.format(len(self.dNewStringsFile)))
		

		self.collect_differences()
		self.collect_files()

		if (len(self.sFilesToModify) > 0):
			for key in self.sFilesToModify:
				print(u'{}'.format(key))
				self.save_modified_file(key)

	def replace_other_languages(self):
		
		lang_curr = os.getcwd()
		lang_root = './lib/lang'

		for lang, fold in self.dLang.items():
			fname = 'new_' + lang + '.strings'
			#print(u'fname: {}'.format(fname))

			dTransaltions = {}
			self.load_file(fname, dTransaltions)

			path = lang_root + '/' + fold
			os.chdir(path)
			with codecs.open(lang + '.new', 'w+', encoding='utf-8') as fw:
				for key, value in sorted(dTransaltions.items()):
					if key in self.dChangedKeys:
						fw.write(self.dChangedKeys[key] + ' => ' + value + '\n')
					else:
						fw.write(key + ' => ' + value + '\n')

			os.remove(lang)
			os.rename(lang + '.new', lang)

			dTransaltions.clear()
			os.chdir(lang_curr)

		print('language files updated.')


def main():
	sf = AirOsReplaceStrings()

	sf.replace_English('English.strings', 'new_English.strings')
	sf.collect_changed_keys()

	sf.replace_other_languages()


if __name__ == "__main__":
	main()