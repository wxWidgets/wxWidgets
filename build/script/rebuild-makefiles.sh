#!/bin/sh

CURDATE=`date -I`
WORKDIR=/home/bake/bkl-cronjob
FTPDIR=/home/ftp/pub

update_from_cvs()
{
    (
    cd ${WORKDIR}/wxWidgets &&  cvs -z3 update -P -d
    )
}


regenerate_makefiles()
{
    (cd ${WORKDIR}/wxWidgets/build/bakefiles && nice python -O ${WORKDIR}/bakefile/bin/bakefile_gen)
}


do_package()
{
    archtype=$1
    format=$2
    shift ; shift

    rm -f ${WORKDIR}/archives/wx-mk-${format}-*

    cd ${WORKDIR}/wxWidgets

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
    rm -f ${WORKDIR}/archives/wx-cvs-*
    cd ${WORKDIR}/
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
find ${FTPDIR}/CVS_HEAD/files -type f -name wx-cvs\*.tar.bz2 -mtime +6 | xargs rm -rf
cp  ${WORKDIR}/archives/wx-cvs-* ${FTPDIR}/CVS_HEAD/files

rm ${FTPDIR}/CVS_HEAD/wx-cvs.tar.bz2
ln -s  ${FTPDIR}/CVS_HEAD/files/wx-cvs-${CURDATE}.tar.bz2  ${FTPDIR}/CVS_HEAD/wx-cvs.tar.bz2
## make sure updated at is really last
sleep 10
echo cvs checkout done at  `date -u` > ${FTPDIR}/CVS_HEAD/updated_at.txt

## Makefiles
find ${FTPDIR}/CVS_Makefiles/files -type f -name wx-mk\* -mtime +3 | xargs rm -rf
cp  ${WORKDIR}/archives/wx-mk-* ${FTPDIR}/CVS_Makefiles/files

rm ${FTPDIR}/CVS_Makefiles/wx*
##there musrt be an easier way of doing these links...
for f in `find ${FTPDIR}/CVS_Makefiles/files -type f -name wx-mk\* -mmin -601` ; do
       ln -s $f `echo $f | sed -e "s/-${CURDATE}//" | sed -e "s|/files||" `
#      echo $f      
#      echo $f | sed -e "s/-${CURDATE}//" | sed -e "s|/files||"
done
sleep 10
echo CVS Makefiles generated from bakefiles last updated at `date -u` > ${FTPDIR}/CVS_Makefiles/updated_at.txt
}



update_from_cvs
regenerate_makefiles
package_makefiles
package_cvs

copy_files


