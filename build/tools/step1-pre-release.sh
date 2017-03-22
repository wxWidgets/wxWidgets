# build a list of English locales
declare -a locale_list=("en_GB" "en_US")
# get available en locales (including .utf8 and .UTF-8 ones)
declare -a en_locales=(`locale -a | grep '^C\|^POSIX\|^en_'`)
locale_list+=("${en_locales[@]}")

found=0

# set an English locale to get the month name
for i in "${locale_list[@]}"
do
    LC_ALL="$i"

	# check that setting the locale succeeded
	# if it failed then LC_TIME would be empty
	my_locale=`locale | grep LC_TIME | sed 's/.*"\(.*\)".*/\1/'`
	if [ "$my_locale" = "$i" ]
	then
	    echo Locale set to $i
	    found=1
		break
	fi
done

if [ $found = 0 ]
then
    echo Could not set an appropriate locale
	exit 1
fi


# get the month name and the year, see $(date +'%B %Y')
# then use it in the relevant line (one before last)
sed -i "/^The wxWidgets Team, / s/.*/The wxWidgets Team, $(date +'%B %Y')/" ./../../docs/readme.txt
if [ $? != 0 ]; then exit $?; fi
echo Updated date in docs/readme.txt

# reset checksums to a string of 40 0-s, see $(printf '%.40d' 0)
sed -i -E "/^\s*[0-9a-f]{40}\s{1,2}wx/ s/(\s*)[0-9a-f]{40}(\s{1,2}wx)/\1$(printf '%.40d' 0)\2/" ./../../docs/release.md
if [ $? != 0 ]; then exit $?; fi
echo Reset checksums in docs/release.md

# get current date, see $(date +'%Y-%m-%d')
# then use it in the line containing "X.X.X: (not released yet)"
sed -i "/^[3-9]\.[0-9]\.[0-9]: (not released yet)$/ s/not released yet/released $(date +'%Y-%m-%d')/" ./../../docs/changes.txt
if [ $? != 0 ]; then exit $?; fi
echo Updated release date in docs/changes.txt

# get current date, see $(date +'%B %d, %Y')
# then use it in the line starting with @date
sed -i "/^@date / s/.*/@date $(date +'%B %d, %Y')/" ./../../docs/doxygen/mainpages/manual.h
if [ $? != 0 ]; then exit $?; fi
echo Updated date in docs/doxygen/mainpages/manual.h

# get current date, see $(date +'%B %d, %Y')
# then use it in the first line
sed -i "1s/^.* -- /$(date +'%B %d, %Y') -- /" ./../../docs/publicity/announce.txt
if [ $? != 0 ]; then exit $?; fi
echo Updated date in docs/publicity/announce.txt
