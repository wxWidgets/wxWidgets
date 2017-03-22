# This will be a RC, unless "final" is given as the first argument!

# go to new release branch
has_new_release_branch=`git branch | grep -E '^\*?\s*new_release' | wc -l`
create_branch=
if [ "$has_new_release_branch" = 0 ]
then create_branch="-b"
fi

git checkout $create_branch new_release --quiet

# determine if there are changes
changes_count=`git diff-index HEAD | wc -l`
if [ $changes_count -gt 0 ]
then
    # silently commit changes to new release branch, if any
    git commit -a -m "changes for a new release" &> /dev/null
    if [ $? != 0 ]; then exit $?; fi
	echo Committed pending changes!
fi

# use sign key or not
#git config user.signingkey <key>
sign_option=
keyCount=`gpg --list-keys | grep '^pub  ' | wc -l`
if [ $keyCount -gt 0 ]
then sign_option="-s"
fi

# get the version
# the primary source of information is include/wx/version.h
ver_string=`grep '#define wxVERSION_STRING ' ./../../include/wx/version.h | sed 's/^.*"wxWidgets \(.*\)")/\1/'`

# do a RC or final release
rc_string=
if [[ $# = 0 ]] || [ "$1" != "final" ]
then
	# get the next RC index for this release
	rc_count=`git tag | grep v"$ver_string"-rc | wc -l`
	((rc_count++))
	rc_string=-rc"$rc_count"
fi

# create the new tag
git tag "$sign_option" -m "Tag $ver_string$rc_string release" v"$ver_string$rc_string"

if [ $? = 0 ]
then echo Tag v"$ver_string$rc_string" created
else
    echo Tag not created.
    exit $?
fi
