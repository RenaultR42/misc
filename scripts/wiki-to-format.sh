#!/bin/sh

# Script to convert a wiki page to others format to post on forum, Linuxfr.org and blog.
# Usefull to port Fedora release announces for example.

FORMAT=$1
FILE=$2
FILE_DST=$(basename $FILE .wiki)
FILE_DST="$FILE_DST.$FORMAT"

if [ -z $FORMAT ]; then
	echo "You must provide a format name: markedown, blog or forum"
	exit
fi

if [ -z $FILE ]; then
	echo "You must provide a filename"
	exit
fi

cp $FILE $FILE_DST

if [ "$FORMAT" == "markdown" ]; then
	# Bold
	sed -i "s/'''\(.*\)'''/**\1**/g" $FILE_DST

	# Italic
	sed -i "s/''\(.*\)''/_\1_/g" $FILE_DST

	# Titles
	sed -i "s/==\(.*\)==/#\1/ #g" $FILE_DST
	sed -i "s/===\(.*\)===/##\1 ##/g" $FILE_DST
	sed -i "s/====\(.*\)====/###\1 ###/g" $FILE_DST

	# Links
	sed -i "s/\[\([^ ]*\) \(.*\)\]/[\2](\1)/g" $FILE_DST

	# Code
	sed -i "s/^ \(.*\)/\`\`\`shell\\n\1\n\`\`\`/g" $FILE_DST
	# To merge consecutive lines of code
	sed -i ":start s/\`\`\`\n\`\`\`shell//g; {N; b start}" $FILE_DST
elif [ "$FORMAT" == "blog" ]; then
	# Bold
	sed -i "s/'''\(.*\)'''/__\1__/g" $FILE_DST

	# Italic
	sed -i "s/''\(.*\)''/\'\'\1\'\'/g" $FILE_DST

	# Titles
	sed -i "s/==\(.*\)==/!!!\1/g" $FILE_DST
	sed -i "s/===\(.*\)===/!!\1/g" $FILE_DST
	sed -i "s/====\(.*\)====/!\1/g" $FILE_DST

	# Links
	sed -i "s/\[\([^ ]*\) \(.*\)\]/[\2|\1|fr]/g" $FILE_DST

	# Code
	sed -i "s|^ \(.*\)|///\n\1\n///|g" $FILE_DST
	# To merge consecutive lines of code
	sed -i ":start s|///\n///||g; {N; b start}" $FILE_DST
else
	# Links to be the first regex here
	# Because conflicts between other balises (forum format) and links detection (in wiki format)
	sed -i "s/\[\([^ ]*\) \(.*\)\]/[url=\1]\2[\/url]/g" $FILE_DST

	# Bold
	sed -i "s/'''\(.*\)'''/[b]\1[\/b]/g" $FILE_DST

	# Italic
	sed -i "s/''\(.*\)''/[i]\1[\/i]/g" $FILE_DST

	# Titles
	sed -i "s/==\(.*\)==/[h]\1[\/h]/g" $FILE_DST
	sed -i "s/===\(.*\)===/[h]\1[\/h]/g" $FILE_DST
	sed -i "s/====\(.*\)====/[h]\1[\/h]/g" $FILE_DST

	# Code
	sed -i "s|^ \(.*\)|[code]\n\1\n[/code]|g" $FILE_DST
	# To merge consecutive lines of code
	sed -i ":start s|\[/code\]\n\[code\]||g; {N; b start}" $FILE_DST

	# List
	sed -i "s|^* \(.*\)|[list]\n[*]\1[/*]\n[/list]|g" $FILE_DST
	# To merge consecutive lists
	sed -i ":start s|\[/list\]\n\[list\]||g; {N; b start}" $FILE_DST
fi
