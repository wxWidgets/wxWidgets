#!/bin/sh

CURDATE=`date -I`

update_from_cvs()
{
    (
    cd /home/bake/bkl-cronjob/wxWidgets &&  cvs -z3 update -P -d
    )
}


regenerate_makefiles()
{
    BAKEFILE_GEN="python -O /home/bake/bkl-cronjob/bakefile/bin/bakefile_gen"
    (cd /home/bake/bkl-cronjob/wxWidgets/build/bakefiles && nice $BAKEFILE_GEN)
}


do_package()
{
    archtype=$1
    format=$2
    shift ; shift

    rm -f /home/bake/bkl-cronjob/archives/wx-mk-${format}-*

    cd /home/bake/bkl-cronjob/wxWidgets

    files=""
    for i in $* ; do
        files="$files `find -name "$i"`"
    done
    
    if test $archtype = tar ; then
        tar czf ../archives/wx-mk-${format}-${CURDATE}.tar.gz $files
    elif test $archtype = zip ; then
        zip -q -9 ../archives/wx-mk-${format}-${CURDATE}.zip $files
    fi
    
    cd ..
}

package_cvs()
{
    rm -f /home/bake/bkl-cronjob/archives/wx-cvs-*
    cd /home/bake/bkl-cronjob/
    tar jcf ./archives/wx-cvs-${CURDATE}.tar.bz2 ./wxWidgets
}


package_makefiles()
{
    do_package tar autoconf  Makefile.in autoconf_inc.m4
    do_package zip borland   makefile.bcc config.bcc
    do_package zip mingw     makefile.gcc config.gcc
    do_package zip dmars     makefile.dmc config.dmc
    do_package zip watcom    makefile.wat config.wat
    do_package zip msvc      makefile.vc config.vc
    do_package zip msvc6prj  '*.dsp' '*.dsw'
    do_package zip evcprj    '*.vcp' '*.vcw'
}

copy_files ()
{
##delete old files and then copy new ones, add a symlink
## CVS
find /home/ftp/pub/CVS_HEAD/files -type f -name wx-cvs\*.tar.bz2 -mtime +6 | xargs rm -rf
cp  /home/bake/bkl-cronjob/archives/wx-cvs-* /home/ftp/pub/CVS_HEAD/files

rm /home/ftp/pub/CVS_HEAD/wx-cvs.tar.bz2
ln -s  /home/ftp/pub/CVS_HEAD/files/wx-cvs-${CURDATE}.tar.bz2  /home/ftp/pub/CVS_HEAD/wx-cvs.tar.bz2

echo cvs checkout done at  `date` > /home/ftp/pub/CVS_HEAD/updated_at.txt

## Makefiles
find /home/ftp/pub/CVS_Makefiles/files -type f -name wx-mk\* -mtime +3 | xargs rm -rf
cp  /home/bake/bkl-cronjob/archives/wx-mk-* /home/ftp/pub/CVS_Makefiles/files

rm /home/ftp/pub/CVS_Makefiles/wx*
##there musrt be an easier way of doing these links...
for f in `find /home/ftp/pub/CVS_Makefiles/files -type f -name wx-mk\* -mmin -601` ; do
       ln -s $f `echo $f | sed -e "s/-${CURDATE}//" | sed -e "s|/files||" `
#      echo $f      
#      echo $f | sed -e "s/-${CURDATE}//" | sed -e "s|/files||"
done

echo CVS Makefiles generated from bakefiles last updated at `date` > /home/ftp/pub/CVS_Makefiles/updated_at.txt
}



update_from_cvs
regenerate_makefiles
package_makefiles
package_cvs

copy_files


