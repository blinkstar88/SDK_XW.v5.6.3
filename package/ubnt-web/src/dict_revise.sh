#!/bin/bash
IFS=$'\n'
lang_file=`find lib/lang/ -type f | grep -v ".svn"`
for f in ${lang_file}; do
	lang=`echo ${f} | cut -f 3 -d "/"`
	echo -n "Processing ${lang}:"
	[ -f "Revise_${lang}.txt" ] && rm Revise_${lang}.txt
	for i in `cat ${f}`; do
		orig=`echo $i | cut -f 1 -d "=" | sed 's/^[ \t]*//;s/[ \t]*$//'`
		tran=`echo $i | cut -f 2 -d ">" | sed 's/^[ \t]*//;s/[ \t]*$//'`
		if [ "$orig" == "$tran" ]; then
			echo $i >> Revise_${lang}.txt
		fi
	done
	echo " `wc -l Revise_${lang}.txt | cut -f 1 -d " "` untranslated lines"
done
