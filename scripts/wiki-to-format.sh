#!/bin/sh

# Script to convert a wiki page to others format to post on forum, Linuxfr.org, Developpez.net, Programmez.com and blog.
# Useful to port Fedora release announces for example.

FORMAT=$1
FILE=$2
FILE_DST=$(basename $FILE .wiki)
FILE_DST="$FILE_DST.$FORMAT"

REGEX_BOLD="'''\(.*\)'''"
REGEX_ITALIC="''\(.*\)''"

REGEX_TITLE_1="==\(.*\)=="
REGEX_TITLE_2="===\(.*\)==="
REGEX_TITLE_3="====\(.*\)===="

REGEX_CODE="^ \(.*\)"
REGEX_LIST="^\* \(.*\)"
#REGEX_URL="\[\(?!url\)\([^ \[]*\) \(.*\)\]"
REGEX_URL="\[\([^ ]*\) \(.*\)\]"

if [ -z $FORMAT ]; then
	echo "You must provide a format name: markedown, blog, developpez, programmez or forum"
	exit
fi

if [ -z $FILE ]; then
	echo "You must provide a filename"
	exit
fi

cp $FILE $FILE_DST

if [ "$FORMAT" == "markdown" ]; then
	# Bold
	grep -q $REGEX_BOLD $FILE_DST
	while [ $? -eq 0 ]
	do
		sed -i "s/$REGEX_BOLD/**\1**/g" $FILE_DST
		grep -q $REGEX_BOLD $FILE_DST
	done

	# Italic
	grep -q $REGEX_ITALIC $FILE_DST
	while [ $? -eq 0 ]
	do
		sed -i "s/$REGEX_ITALIC/_\1_/g" $FILE_DST
		grep -q $REGEX_ITALIC $FILE_DST
	done

	# Titles
	sed -i "s/$REGEX_TITLE_3/###\1 ###/g" $FILE_DST
	sed -i "s/$REGEX_TITLE_2/##\1 ##/g" $FILE_DST
	sed -i "s/$REGEX_TITLE_1/#\1/ #/g" $FILE_DST

	# Links
	sed -i "s/$REGEX_URL/[\2](\1)/g" $FILE_DST

	# Code
	sed -i "s/$REGEX_CODE/\`\`\`shell\\n\1\n\`\`\`/g" $FILE_DST
	# To merge consecutive lines of code
	sed -i ":start s/\`\`\`\n\`\`\`shell//g; {N; b start}" $FILE_DST
elif [ "$FORMAT" == "blog" ]; then
	# Bold
	grep -q $REGEX_BOLD $FILE_DST
	while [ $? -eq 0 ]
	do
		sed -i "s/$REGEX_BOLD/__\1__/g" $FILE_DST
		grep -q $REGEX_BOLD $FILE_DST
	done

	# Titles
	sed -i "s/$REGEX_TITLE_3/!\1/g" $FILE_DST
	sed -i "s/$REGEX_TITLE_2/!!\1/g" $FILE_DST
	sed -i "s/$REGEX_TITLE_1/!!!\1/g" $FILE_DST

	# Links
	sed -i "s/$REGEX_URL/[\2|\1|fr]/g" $FILE_DST

	# Code
	sed -i "s|$REGEX_CODE|///\n\1\n///|g" $FILE_DST
	# To merge consecutive lines of code
	sed -i ":start s|///\n///||g; {N; b start}" $FILE_DST
elif [ "$FORMAT" == "programmez" ]; then
	# Bold
	grep -q $REGEX_BOLD $FILE_DST
	while [ $? -eq 0 ]
	do
		sed -i "s|$REGEX_BOLD|<strong>\1</strong>|g" $FILE_DST
		grep -q $REGEX_BOLD $FILE_DST
	done

	# Italic
	grep -q $REGEX_ITALIC $FILE_DST
	while [ $? -eq 0 ]
	do
		sed -i "s|$REGEX_ITALIC|<em>\1</em>|g" $FILE_DST
		grep -q $REGEX_ITALIC $FILE_DST
	done

	# Titles
	sed -i "s|$REGEX_TITLE_3|</p>\n<p><strong><em>\1</em></strong></p>\n<p>|g" $FILE_DST
	sed -i "s|$REGEX_TITLE_2|</p>\n<p><strong><em>\1</em></strong></p>\n<p>|g" $FILE_DST
	sed -i "s|$REGEX_TITLE_1|</p>\n<p><strong><em>\1</em></strong></p>\n<p>|g" $FILE_DST

	# Links
	sed -i "s|$REGEX_URL|<a href=\"\1\">\2</a>|g" $FILE_DST

	# Code
	sed -i "s|$REGEX_CODE|<blockquote>\1</blockquote>|g" $FILE_DST
	# To merge consecutive lines of code
	sed -i ":start s|</blockquote>\n<blockquote>||g; {N; b start}" $FILE_DST

	# List
	sed -i "s|$REGEX_LIST|<ul>\n<li>\1</li>\n</ul>|g" $FILE_DST
	# To merge consecutive lists
	sed -i ":start s|</ul>\n<ul>||g; {N; b start}" $FILE_DST
elif [ "$FORMAT" == "developpez" ]; then
	# Links to be the first regex here
	# Because conflicts between other balises (forum format) and links detection (in wiki format)
	sed -i "s|$REGEX_URL|[URL=\"\1\"]\2[/URL]|g" $FILE_DST

	# Bold
	grep -q $REGEX_BOLD $FILE_DST
	while [ $? -eq 0 ]
	do
		sed -i "s|$REGEX_BOLD|[B]\1[/B]|g" $FILE_DST
		grep -q $REGEX_BOLD $FILE_DST
	done

	# Italic
	grep -q $REGEX_ITALIC $FILE_DST
	while [ $? -eq 0 ]
	do
		sed -i "s|$REGEX_ITALIC|[I]\1[/I]|g" $FILE_DST
		grep -q $REGEX_ITALIC $FILE_DST
	done

	# Titles
	sed -i "s|$REGEX_TITLE_3|[SIZE=3]\1[/SIZE]|g" $FILE_DST
	sed -i "s|$REGEX_TITLE_2|[SIZE=4]\1[/SIZE]|g" $FILE_DST
	sed -i "s|$REGEX_TITLE_1|[SIZE=5]\1[/SIZE]|g" $FILE_DST

	# Code
	sed -i "s|$REGEX_CODE|[CODE]\1[/CODE]|g" $FILE_DST
	# To merge consecutive lines of code
	sed -i ":start s|[/CODE]\n[CODE]||g; {N; b start}" $FILE_DST

	# List
	sed -i "s|$REGEX_LIST|[LIST]\n[*]\1\n[/LIST]|g" $FILE_DST
	# To merge consecutive lists
	sed -i ":start s|\[/LIST\]\n\[LIST\]||g; {N; b start}" $FILE_DST
else
	# Links to be the first regex here
	# Because conflicts between other balises (forum format) and links detection (in wiki format)
	sed -i "s/$REGEX_URL/[url=\1]\2[\/url]/g" $FILE_DST

	# Bold
	grep -q $REGEX_BOLD $FILE_DST
	while [ $? -eq 0 ]
	do
		sed -i "s/$REGEX_BOLD/[b]\1[\/b]/g" $FILE_DST
		grep -q $REGEX_BOLD $FILE_DST
	done

	# Italic
	grep -q $REGEX_ITALIC $FILE_DST
	while [ $? -eq 0 ]
	do
		sed -i "s/$REGEX_ITALIC/[i]\1[\/i]/g" $FILE_DST
		grep -q $REGEX_ITALIC $FILE_DST
	done

	# Titles
	sed -i "s/$REGEX_TITLE_3/[h]\1[\/h]/g" $FILE_DST
	sed -i "s/$REGEX_TITLE_2/[h]\1[\/h]/g" $FILE_DST
	sed -i "s/$REGEX_TITLE_1/[h]\1[\/h]/g" $FILE_DST

	# Code
	sed -i "s|$REGEX_CODE|[code]\n\1\n[/code]|g" $FILE_DST
	# To merge consecutive lines of code
	sed -i ":start s|\[/code\]\n\[code\]||g; {N; b start}" $FILE_DST

	# List
	sed -i "s|$REGEX_LIST|[list]\n[*]\1[/*]\n[/list]|g" $FILE_DST
	# To merge consecutive lists
	sed -i ":start s|\[/list\]\n\[list\]||g; {N; b start}" $FILE_DST
fi
