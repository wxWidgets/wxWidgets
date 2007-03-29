#!/bin/sh

CURDATE=`date -I`
WORKDIR=/home/bake/bkl-cronjob
WINSRCDIR=/mnt/daily
FTPDIR=/home/ftp/pub
LD_LIBRARY_PATH=/usr/local/lib

update_from_cvs()
{
    (
    cd ${WORKDIR}/wxWidgets 
    cvs -z3 update -P -d
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
        zip -l -q -9 ../archives/wx-mk-${format}-${CURDATE}.zip $files
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
    do_package zip evcprj    '*.vcp' '*.vcw'
  (cd ${WORKDIR}/wxWidgets/build/bakefiles && nice python -O /usr/local/bin/bakefile_gen -f watcom -d ../os2/Bakefiles.os2.bkgen)
  do_package zip watcom_os2    makefile.wat config.wat
}

copy_files ()
{
##delete old files and then copy new ones, add a symlink

## Makefiles
find ${FTPDIR}/CVS_Makefiles/files -type f -name wx-mk\* -mtime +3 | xargs rm -rf
cp  ${WORKDIR}/archives/wx-mk-* ${FTPDIR}/CVS_Makefiles/files

rm -f ${FTPDIR}/CVS_Makefiles/wx* ${FTPDIR}/CVS_Makefiles/MD5SUM
##there must be an easier way of doing these links...
for f in `find ${FTPDIR}/CVS_Makefiles/files -type f -name wx-mk\* -mmin -601` ; do
       ln -s $f `echo $f | sed -e "s/-${CURDATE}//" | sed -e "s|/files||" `
done
md5sum ${FTPDIR}/CVS_Makefiles/wx* > ${FTPDIR}/CVS_Makefiles/MD5SUM
sleep 10
echo CVS Makefiles generated from bakefiles last updated at `date -u` > ${FTPDIR}/CVS_Makefiles/updated_at.txt

## Setup.exe
find ${FTPDIR}/CVS_HEAD/v1/files -type f -name wx\* -mtime +3 | xargs rm -rf
cp  ${WORKDIR}/archives/win/*.exe ${FTPDIR}/CVS_HEAD/v1/files

rm -f ${FTPDIR}/CVS_HEAD/v1/*.exe ${FTPDIR}/CVS_HEAD/v1/MD5SUM
##there must be an easier way of doing these links...
for f in `find ${FTPDIR}/CVS_HEAD/v1/files -type f -name wx\*.exe -mmin -601` ; do
       ln -s $f `echo $f | sed -e "s/-${CURDATE}//" | sed -e "s|/files||" `
done
md5sum ${FTPDIR}/CVS_HEAD/v1/wx* > ${FTPDIR}/CVS_HEAD/v1/MD5SUM
sleep 10
echo CVS HEAD  last updated at `date -u` > ${FTPDIR}/CVS_HEAD/v1/updated_at.txt


## Docs...
find ${FTPDIR}/CVS_Docs/files -type f -name wx\* -mtime +3 | xargs rm -rf
cp  ${WORKDIR}/archives/wx-doc* ${FTPDIR}/CVS_Docs/files
cp  ${WORKDIR}/archives/win/wxW* ${FTPDIR}/CVS_Docs/files

rm -f ${FTPDIR}/CVS_Docs/wx* ${FTPDIR}/CVS_Docs/MD5SUM
##there must be an easier way of doing these links...
for f in `find ${FTPDIR}/CVS_Docs/files -type f -name wx\* -mmin -601` ; do
       ln -s $f `echo $f | sed -e "s/-${CURDATE}//" | sed -e "s|/files||" `
done
md5sum ${FTPDIR}/CVS_Docs/wx* > ${FTPDIR}/CVS_Docs/MD5SUM
sleep 10
echo CVS Documentation generated from bakefiles last updated at `date -u` > ${FTPDIR}/CVS_Docs/updated_at.txt
}

do_texrtf ()
{

##parameters : subdir_of_tex index.tex  dir_under_wxWidgets
# need this first time only
if [ ! -e ${WORKDIR}/archives/html/$1 ] ; then
  mkdir ${WORKDIR}/archives/html/$1
fi  

rm -f ${WORKDIR}/archives/html/$1/*
/usr/local/bin/tex2rtf ${WORKDIR}/wxWidgets/$3/docs/latex/$1/$2  ${WORKDIR}/archives/html/$1/$1 -twice -html -macros ${WORKDIR}/wxWidgets/docs/latex/wx/tex2rtf_css.ini
cp ${WORKDIR}/wxWidgets/$3/docs/latex/$1/*.gif ${WORKDIR}/archives/html/$1
cp ${WORKDIR}/wxWidgets/$3/docs/latex/$1/*.css ${WORKDIR}/archives/html/$1
cd ${WORKDIR}/archives/html/$1
zip -l -q -9 ${WORKDIR}/archives/htb/$1.htb *.html  *.css wx.hhc wx.hhp wx.hhk
zip -q -9 ${WORKDIR}/archives/htb/$1.htb  *.gif 
}

##this one for tex2rtf as its latex docs aren't in latex...
do_util_texrtf ()
{
##parameters : subdir_of_tex index.tex 
# need this first time only
if [ ! -e ${WORKDIR}/archives/html/$1 ] ; then
  mkdir ${WORKDIR}/archives/html/$1
fi
  
rm -f ${WORKDIR}/archives/html/$1/*
/usr/local/bin/tex2rtf ${WORKDIR}/wxWidgets/utils/$1/docs/$2  ${WORKDIR}/archives/html/$1/$1 -twice -html -macros ${WORKDIR}/wxWidgets/docs/latex/wx/tex2rtf_css.ini
cp ${WORKDIR}/wxWidgets/utils/$1/docs/*.gif ${WORKDIR}/archives/html/$1
cp ${WORKDIR}/wxWidgets/utils/$1/docs/*.css ${WORKDIR}/archives/html/$1
cd ${WORKDIR}/archives/html/$1
zip -l -q -9 ${WORKDIR}/archives/htb/$1.htb *.html  *.css wx.hhc wx.hhp wx.hhk
zip -q -9 ${WORKDIR}/archives/htb/$1.htb  *.gif 
}

do_docs ()
{
##remove files, then regenerate
rm ${WORKDIR}/archives/wx-do*
rm ${WORKDIR}/archives/win/wx*
rm ${WORKDIR}/archives/htb/*.*

######### dir index.tex rootdir
do_texrtf wx manual.tex
#do_texrtf book book.tex
#do_texrtf svg dcsvg.tex contrib
#do_texrtf ogl ogl.tex contrib
#do_texrtf mmedia mmedia.tex contrib
#do_texrtf gizmos manual.tex contrib
#do_texrtf fl fl.tex contrib
do_util_texrtf tex2rtf tex2rtf.tex 

cd ${WORKDIR}/archives/
tar zcvf ${WORKDIR}/archives/wx-docs-html-${CURDATE}.tar.gz `find . -name '*.gif' -print -o -name '*.html' -print` html/wx/*.css

tar zcvf ${WORKDIR}/archives/wx-docs-htb-${CURDATE}.tar.gz  htb/*
zip -q -9 ${WORKDIR}/archives/wx-docs-htb-${CURDATE}.zip htb/*

##remove .con files
rm ${WORKDIR}/*.con

}


add_win_files ()
{
### starts with wx***.zip

for f in `find ${WINSRCDIR}/ -maxdepth 0 -name wx\*.zip ` ; do       
       cp $f ${WORKDIR}/archives/win/`basename $f | sed -e "s/.zip//"`-${CURDATE}.zip
       done

for f in `find ${WINSRCDIR}/ -maxdepth 0 -name wx\*.exe ` ; do       
       cp $f ${WORKDIR}/archives/win/`basename $f | sed -e "s/.exe//"`-${CURDATE}.exe
       done

}

update_from_cvs
regenerate_makefiles
package_makefiles

do_docs
add_win_files

copy_files

exit 0
