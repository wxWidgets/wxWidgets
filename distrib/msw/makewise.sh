#! /bin/sh
# Make a WISE Installer distribution list, where each file is represented by
# a section like this:
# 
# item: Install File
#   Source=d:\wx2\thefile.txt
#   Destination=%MAINDIR%\thefile.txt
#   Flags=0000000000000010
# end

wxdir=`cygpath -u $WXWIN`
tempdir=`cygpath -u $TEMP`

# Generate a list of all files in the distribution.
# We pass the output through sed in order to remove the preceding "./"
cd $wxdir/deliver/wx
find . -print | sed -e "s/\.\\///g" > $tempdir/files1.tmp

# Now we iterate through the list of files, writing out the middle section of
# the file.
# We have to remove the first part of the path,
# by truncating the start by the size of the current directory.

rm -f $tempdir/files2.tmp

# First add system files
cat $wxdir/distrib/msw/wisesys.txt > $tempdir/files2.tmp

for line in `cat $tempdir/files1.tmp` ; do

    # If not a directory, add to file
    if [ ! -d $line ] ; then
        # The relative path
        line2=`cygpath -w $line`
        # The absolute path
        line1=$WXWIN"\\deliver\\wx\\"$line2

        echo "item: Install File " >> $tempdir/files2.tmp
        echo "  Source=$line1 " >> $tempdir/files2.tmp
        echo "  Destination=%MAINDIR%\\"$line2 >> $tempdir/files2.tmp
        echo "  Flags=0000000000000010" >> $tempdir/files2.tmp
        echo "end" >> $tempdir/files2.tmp
    fi
done

# Concatenate the 3 sections
cat $wxdir/distrib/msw/wisetop.txt $tempdir/files2.tmp $wxdir/distrib/msw/wisebott.txt > $wxdir/distrib/msw/wxwin2.wse

rm -f $tempdir/files1.tmp


