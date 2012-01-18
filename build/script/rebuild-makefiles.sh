#!/bin/sh

CURDATE=`date -I`
WORKDIR=/home/bake/bkl-cronjob
WINSRCDIR=/mnt/daily
FTPDIR=/home/ftp/pub
LD_LIBRARY_PATH=/usr/local/lib

update_from_svn()
{
    (
    cd ${WORKDIR}/wxWidgets 
    svn up
    find . -name \.#\* | xargs rm -rf
    )

}


regenerate_makefiles()
{
    (cd ${WORKDIR}/wxWidgets/build/bakefiles && nice python -O /usr/local/bin/bakefile_gen)
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
###        zip -l -q -9 ../archives/wx-mk-${format}-${CURDATE}.zip $files
        zip -q -9 ../archives/wx-mk-${format}-${CURDATE}.zip $files
    fi
    
    cd ..
}




package_makefiles()
{
    do_package tar autoconf  Makefile.in autoconf_inc.m4
    do_package zip gnu       makefile.unx 
    do_package tar gnu       makefile.unx 
    do_package zip mingw     makefile.gcc config.gcc
    do_package zip dmars     makefile.dmc config.dmc makefile.dms config.dms
    do_package zip watcom_msw    makefile.wat config.wat
    do_package zip msvc      makefile.vc config.vc
    do_package zip msvc6prj  '*.dsp' '*.dsw'
    do_package zip msvc7-8prj  '*.vcproj' '*.sln'    
    do_package zip evcprj    '*.vcp' '*.vcw'
  (cd ${WORKDIR}/wxWidgets/build/bakefiles && nice python -O /usr/local/bin/bakefile_gen -f watcom -d ../os2/Bakefiles.os2.bkgen)
  do_package zip watcom_os2    makefile.wat config.wat
}

copy_files ()
{
##delete old files and then copy new ones, add a symlink

## Makefiles
find ${FTPDIR}/Daily_Makefiles/files -type f -name wx-mk\* -mtime +3 | xargs rm -rf
cp  ${WORKDIR}/archives/wx-mk-* ${FTPDIR}/Daily_Makefiles/files

rm -f ${FTPDIR}/Daily_Makefiles/wx* ${FTPDIR}/Daily_Makefiles/MD5SUM
##there must be an easier way of doing these links...
for f in `find ${FTPDIR}/Daily_Makefiles/files -type f -name wx-mk\* -mmin -601` ; do
       ln -s $f `echo $f | sed -e "s/-${CURDATE}//" | sed -e "s|/files||" `
done
md5sum ${FTPDIR}/Daily_Makefiles/wx* > ${FTPDIR}/Daily_Makefiles/MD5SUM
sleep 10
echo CVS Makefiles generated from bakefiles last updated at `date -u` > ${FTPDIR}/Daily_Makefiles/updated_at.txt

## Setup.exe
find ${FTPDIR}/Daily_HEAD/files -type f -name wx\* -mtime +3 | xargs rm -rf
cp  ${WORKDIR}/archives/win/*.exe ${FTPDIR}/Daily_HEAD/files

rm -f ${FTPDIR}/Daily_HEAD/*.exe ${FTPDIR}/Daily_HEAD/MD5SUM
##there must be an easier way of doing these links...
for f in `find ${FTPDIR}/Daily_HEAD/files -type f -name wx\*.exe -mmin -601` ; do
       ln -s $f `echo $f | sed -e "s/-${CURDATE}//" | sed -e "s|/files||" `
done
md5sum ${FTPDIR}/Daily_HEAD/wx* > ${FTPDIR}/Daily_HEAD/MD5SUM
sleep 10
echo CVS HEAD  last updated at `date -u` > ${FTPDIR}/Daily_HEAD/updated_at.txt



}


add_win_files ()
{
### starts with wx***.zip

for f in `find ${WINSRCDIR}/ -maxdepth 1 -name wx\*.zip ` ; do       
       cp $f ${WORKDIR}/archives/win/`basename $f | sed -e "s/.zip//"`-${CURDATE}.zip
       done

for f in `find ${WINSRCDIR}/ -maxdepth 1 -name wx\*.exe ` ; do       
       cp $f ${WORKDIR}/archives/win/`basename $f | sed -e "s/.exe//"`-${CURDATE}.exe
       done

}

update_from_svn
regenerate_makefiles
package_makefiles


add_win_files

copy_files

exit 0
