#!/bin/sh

CURDATE=`date -I`
WORKDIR=/home/bake/bkl-cronjob
FTPDIR=/home/ftp/pub
LD_LIBRARY_PATH=/usr/local/lib

update_from_cvs()
{
    (
    cd ${WORKDIR}/wxWidgets &&  cvs -z3 update -P -d
    )
    (
    cd ${WORKDIR}/wxGTK &&  cvs -z3 update -P 
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
        zip -l -q -9 ../archives/wx-mk-${format}-${CURDATE}.zip $files
    fi
    
    cd ..
}

package_cvs()
{
    rm -f ${WORKDIR}/archives/wx-cvs-$1*
    cd ${WORKDIR}/
    ##tar --exclude=*.ds* -jcf ./archives/test.tar.bz2 ./wxGTK
    tar --exclude=*.ds* -jcf ./archives/wx-cvs-$1-${CURDATE}.tar.bz2 ./$2
}


package_makefiles()
{
    do_package tar autoconf  Makefile.in autoconf_inc.m4
    do_package zip borland   makefile.bcc config.bcc
    do_package zip mingw     makefile.gcc config.gcc
    do_package zip dmars     makefile.dmc config.dmc makefile.dms config.dms
    do_package zip watcom    makefile.wat config.wat
    do_package zip msvc      makefile.vc config.vc
    do_package zip msvc6prj  '*.dsp' '*.dsw'
    do_package zip evcprj    '*.vcp' '*.vcw'
}

copy_files ()
{
##delete old files and then copy new ones, add a symlink
## CVS
find ${FTPDIR}/CVS_HEAD/files -type f -name wx-cvs\* -mtime +6 | xargs rm -rf
cp  ${WORKDIR}/archives/wx-cvs-* ${FTPDIR}/CVS_HEAD/files

rm ${FTPDIR}/CVS_HEAD/wx* ${FTPDIR}/CVS_HEAD/MD5SUM
for f in `find ${FTPDIR}/CVS_HEAD/files -type f -name wx-cvs\* -mmin -601` ; do
       ln -s $f `echo $f | sed -e "s/-${CURDATE}//" | sed -e "s|/files||" `
done
md5sum ${FTPDIR}/CVS_HEAD/wx* > ${FTPDIR}/CVS_HEAD/MD5SUM
## make sure updated at is really last
sleep 10
echo cvs checkout done at  `date -u` > ${FTPDIR}/CVS_HEAD/updated_at.txt

## Makefiles
find ${FTPDIR}/CVS_Makefiles/files -type f -name wx-mk\* -mtime +3 | xargs rm -rf
cp  ${WORKDIR}/archives/wx-mk-* ${FTPDIR}/CVS_Makefiles/files

rm ${FTPDIR}/CVS_Makefiles/wx* ${FTPDIR}/CVS_Makefiles/MD5SUM
##there must be an easier way of doing these links...
for f in `find ${FTPDIR}/CVS_Makefiles/files -type f -name wx-mk\* -mmin -601` ; do
       ln -s $f `echo $f | sed -e "s/-${CURDATE}//" | sed -e "s|/files||" `
done
md5sum ${FTPDIR}/CVS_Makefiles/wx* > ${FTPDIR}/CVS_Makefiles/MD5SUM
sleep 10
echo CVS Makefiles generated from bakefiles last updated at `date -u` > ${FTPDIR}/CVS_Makefiles/updated_at.txt


## Docs...
find ${FTPDIR}/CVS_Docs/files -type f -name wxWidgets-Do\* -mtime +3 | xargs rm -rf
cp  ${WORKDIR}/archives/wxWidgets-Docs-* ${FTPDIR}/CVS_Docs/files

rm ${FTPDIR}/CVS_Docs/wx* ${FTPDIR}/CVS_Docs/MD5SUM
##there must be an easier way of doing these links...
for f in `find ${FTPDIR}/CVS_Docs/files -type f -name wxWidgets-Do\* -mmin -601` ; do
       ln -s $f `echo $f | sed -e "s/-${CURDATE}//" | sed -e "s|/files||" `
done
md5sum ${FTPDIR}/CVS_Docs/wx* > ${FTPDIR}/CVS_Docs/MD5SUM
sleep 10
echo CVS Documentation generated from bakefiles last updated at `date -u` > ${FTPDIR}/CVS_Docs/updated_at.txt
}

do_texrtf ()
{

##parameters : subdir_of_tex index.tex  dir_under_wxWidgets
rm ${WORKDIR}/archives/html/$1/*
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
rm ${WORKDIR}/archives/html/$1/*
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
rm ${WORKDIR}/archives/wxWidgets-Docs*
rm ${WORKDIR}/archives/htb/*.*

######### dir index.tex rootdir
do_texrtf wx manual.tex
do_texrtf book book.tex
do_texrtf svg dcsvg.tex contrib
do_texrtf ogl ogl.tex contrib
do_texrtf mmedia mmedia.tex contrib
do_texrtf gizmos manual.tex contrib
do_texrtf fl fl.tex contrib
do_util_texrtf tex2rtf tex2rtf.tex 

cd ${WORKDIR}/archives/
tar zcvf ${WORKDIR}/archives/wxWidgets-Docs-HTML-${CURDATE}.tar.gz  html/wx/*.html html/wx/*.gif html/wx/*.css
# TODO: include the extra HTML docs into the above archive.
# TODO: we need to add the version number to the doc archives
# tar zcvf ${WORKDIR}/archives/wxWidgets-Docs-Extra-HTML-${CURDATE}.tar.gz `find . -name 'wx' -prune -o -name '*.html' -print`

tar zcvf ${WORKDIR}/archives/wxWidgets-Docs-HTB-${CURDATE}.tar.gz  htb/*.htb
# tar zcvf ${WORKDIR}/archives/wxWidgets-Docs-Extra-HTB-${CURDATE}.tar.gz --exclude wx.htb htb/*.htb

##remove .con files
rm ${WORKDIR}/*.con

}


add_win_files ()
{
### starts with wxWidgets-Docs-xxx.zip
for f in `find ${WORKDIR}/archives/win/ -name wx\*.zip ` ; do       
       mv $f ${WORKDIR}/archives/`basename $f | tr -d ".zip"`-${CURDATE}.zip
       done

rm ${WORKDIR}/archives/*.exe
for f in `find ${WORKDIR}/archives/win/ -name wx\*.exe ` ; do       
       mv $f ${WORKDIR}/archives/`basename $f | tr -d ".exe"`-${CURDATE}.exe
       done

}

update_from_cvs
regenerate_makefiles
package_makefiles
package_cvs All wxWidgets
package_cvs Gtk wxGTK

do_docs
add_win_files

copy_files

exit 0
