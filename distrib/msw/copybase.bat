@echo off

rem VZ: this is quick and _very_ dirty, to be replaced by a script directly
rem     parsing the files.txt and include/wx/version.h...

set VER=2.3.4
set DEST=t:\wxBase-%VER%

mkdir %DEST%
mkdir %DEST%\include
mkdir %DEST%\include\wx
mkdir %DEST%\include\wx\msw
mkdir %DEST%\include\wx\protocol
mkdir %DEST%\include\wx\unix
mkdir %DEST%\locale
mkdir %DEST%\samples
mkdir %DEST%\samples\console
mkdir %DEST%\src
mkdir %DEST%\src\common
mkdir %DEST%\src\msw
mkdir %DEST%\src\regex
mkdir %DEST%\src\unix
mkdir %DEST%\src\zlib
mkdir %DEST%\lib

chdir %WXWIN%

rem Copy the files to the root directory

copy /q configure.in %DEST%
copy /q configure %DEST%
copy /q wxwin.m4 %DEST%
copy /q aclocal.m4 %DEST%
copy /q config.sub %DEST%
copy /q config.guess %DEST%
copy /q install-sh %DEST%
copy /q mkinstalldirs %DEST%
copy /q wx-config.in %DEST%
copy /q setup.h.in %DEST%
copy /q Makefile.in %DEST%
copy /q wxBase.spec %DEST%
copy /q docs\changes.txt %DEST%\CHANGES.txt
copy /q docs\licence.txt %DEST%\LICENCE.txt
copy /q docs\base\readme.txt %DEST%\README.txt

rem Copy the project/make files

copy /q src\wxBase.dsp %DEST%\src\wxBase.dsp
copy /q src\wxBase.dsw %DEST%\src\wxBase.dsw
copy /q include\wx\msw\setup0.h %DEST%\include\wx\msw\setup.h
copy /q src\makeb32.env %DEST%\src\makeb32.env
copy /q src\makelib.b32 %DEST%\src\makelib.b32
copy /q src\makeprog.b32 %DEST%\src\makeprog.b32
copy /q src\msw\makebase.b32 %DEST%\src\msw\makebase.b32
copy /q src\*.in %DEST%\src

rem Copy the sample

copy /q samples\console\console.cpp %DEST%\samples\console\console.cpp
copy /q samples\console\console.dsp %DEST%\samples\console\console.dsp
copy /q samples\console\makefile.b32 %DEST%\samples\console\makefile.b32
copy /q samples\console\makefile.unx %DEST%\samples\console
copy /q samples\console\Makefile.in %DEST%\samples\console
copy /q samples\console\testdata.fc %DEST%\samples\console\testdata.fc

rem Copy regex and zlib files

copy /q src\regex\*.* %DEST%\src\regex
copy /q src\zlib\*.* %DEST%\src\zlib

rem The files not in src/files.lst
copy /q src\msw\dummy.cpp %DEST%\src\msw\dummy.cpp
copy /q src\msw\dummydll.cpp %DEST%\src\msw\dummydll.cpp

copy /q src\common\execcmn.cpp %DEST%\src\common\execcmn.cpp
copy /q src\common\unictabl.inc %DEST%\src\common\unictabl.inc
copy /q src\common\unzip.h %DEST%\src\common\unzip.h

copy /q include\wx\msw\gsockmsw.h %DEST%\include\wx\msw\gsockmsw.h
copy /q include\wx\msw\missing.h %DEST%\include\wx\msw\missing.h
copy /q include\wx\msw\mslu.h %DEST%\include\wx\msw\mslu.h
copy /q include\wx\msw\msvcrt.h %DEST%\include\wx\msw\msvcrt.h
copy /q include\wx\msw\private.h %DEST%\include\wx\msw\private.h
copy /q include\wx\msw\regconf.h %DEST%\include\wx\msw\regconf.h
copy /q include\wx\msw\registry.h %DEST%\include\wx\msw\registry.h

rem The rest is generated from src/files.lst by pasting the ALL_SOURCES and
rem ALL_HEADERS here and doing
rem
rem .,$s@^\s*\(.*\)/\([^ ]*\)\( \\\)\?$@copy /q src\\\1\\\2 %DEST%\\src\\\1\\\2
rem
rem and
rem
rem .,$s@/@\\@|exe "normal "|.,$s@^\s*\([^ ]*\)\( \\\)\?$@copy /q include\\wx\\\1 %DEST%\\include\\wx\\\1
rem 
copy /q src\common\init.cpp %DEST%\src\common\init.cpp
copy /q src\common\appcmn.cpp %DEST%\src\common\appcmn.cpp
copy /q src\common\clntdata.cpp %DEST%\src\common\clntdata.cpp
copy /q src\common\cmdline.cpp %DEST%\src\common\cmdline.cpp
copy /q src\common\config.cpp %DEST%\src\common\config.cpp
copy /q src\common\datetime.cpp %DEST%\src\common\datetime.cpp
copy /q src\common\datstrm.cpp %DEST%\src\common\datstrm.cpp
copy /q src\common\db.cpp %DEST%\src\common\db.cpp
copy /q src\common\dbtable.cpp %DEST%\src\common\dbtable.cpp
copy /q src\common\dircmn.cpp %DEST%\src\common\dircmn.cpp
copy /q src\common\dynarray.cpp %DEST%\src\common\dynarray.cpp
copy /q src\common\dynlib.cpp %DEST%\src\common\dynlib.cpp
copy /q src\common\dynload.cpp %DEST%\src\common\dynload.cpp
copy /q src\common\encconv.cpp %DEST%\src\common\encconv.cpp
copy /q src\common\event.cpp %DEST%\src\common\event.cpp
copy /q src\common\extended.c %DEST%\src\common\extended.c
copy /q src\common\ffile.cpp %DEST%\src\common\ffile.cpp
copy /q src\common\file.cpp %DEST%\src\common\file.cpp
copy /q src\common\fileconf.cpp %DEST%\src\common\fileconf.cpp
copy /q src\common\filefn.cpp %DEST%\src\common\filefn.cpp
copy /q src\common\filename.cpp %DEST%\src\common\filename.cpp
copy /q src\common\filesys.cpp %DEST%\src\common\filesys.cpp
copy /q src\common\fontmap.cpp %DEST%\src\common\fontmap.cpp
copy /q src\common\fs_inet.cpp %DEST%\src\common\fs_inet.cpp
copy /q src\common\fs_mem.cpp %DEST%\src\common\fs_mem.cpp
copy /q src\common\fs_zip.cpp %DEST%\src\common\fs_zip.cpp
copy /q src\common\ftp.cpp %DEST%\src\common\ftp.cpp
copy /q src\common\hash.cpp %DEST%\src\common\hash.cpp
copy /q src\common\hashmap.cpp %DEST%\src\common\hashmap.cpp
copy /q src\common\http.cpp %DEST%\src\common\http.cpp
copy /q src\common\intl.cpp %DEST%\src\common\intl.cpp
copy /q src\common\ipcbase.cpp %DEST%\src\common\ipcbase.cpp
copy /q src\common\list.cpp %DEST%\src\common\list.cpp
copy /q src\common\log.cpp %DEST%\src\common\log.cpp
copy /q src\common\longlong.cpp %DEST%\src\common\longlong.cpp
copy /q src\common\memory.cpp %DEST%\src\common\memory.cpp
copy /q src\common\mimecmn.cpp %DEST%\src\common\mimecmn.cpp
copy /q src\common\module.cpp %DEST%\src\common\module.cpp
copy /q src\common\msgout.cpp %DEST%\src\common\msgout.cpp
copy /q src\common\mstream.cpp %DEST%\src\common\mstream.cpp
copy /q src\common\object.cpp %DEST%\src\common\object.cpp
copy /q src\common\process.cpp %DEST%\src\common\process.cpp
copy /q src\common\protocol.cpp %DEST%\src\common\protocol.cpp
copy /q src\common\regex.cpp %DEST%\src\common\regex.cpp
copy /q src\common\sckaddr.cpp %DEST%\src\common\sckaddr.cpp
copy /q src\common\sckfile.cpp %DEST%\src\common\sckfile.cpp
copy /q src\common\sckipc.cpp %DEST%\src\common\sckipc.cpp
copy /q src\common\sckstrm.cpp %DEST%\src\common\sckstrm.cpp
copy /q src\common\socket.cpp %DEST%\src\common\socket.cpp
copy /q src\common\strconv.cpp %DEST%\src\common\strconv.cpp
copy /q src\common\stream.cpp %DEST%\src\common\stream.cpp
copy /q src\common\string.cpp %DEST%\src\common\string.cpp
copy /q src\common\sysopt.cpp %DEST%\src\common\sysopt.cpp
copy /q src\common\textbuf.cpp %DEST%\src\common\textbuf.cpp
copy /q src\common\textfile.cpp %DEST%\src\common\textfile.cpp
copy /q src\common\timercmn.cpp %DEST%\src\common\timercmn.cpp
copy /q src\common\tokenzr.cpp %DEST%\src\common\tokenzr.cpp
copy /q src\common\txtstrm.cpp %DEST%\src\common\txtstrm.cpp
copy /q src\common\unzip.c %DEST%\src\common\unzip.c
copy /q src\common\url.cpp %DEST%\src\common\url.cpp
copy /q src\common\utilscmn.cpp %DEST%\src\common\utilscmn.cpp
copy /q src\common\variant.cpp %DEST%\src\common\variant.cpp
copy /q src\common\wfstream.cpp %DEST%\src\common\wfstream.cpp
copy /q src\common\wxchar.cpp %DEST%\src\common\wxchar.cpp
copy /q src\common\zipstrm.cpp %DEST%\src\common\zipstrm.cpp
copy /q src\common\zstream.cpp %DEST%\src\common\zstream.cpp
copy /q src\unix\dir.cpp %DEST%\src\unix\dir.cpp
copy /q src\unix\gsocket.c %DEST%\src\unix\gsocket.c
copy /q src\unix\mimetype.cpp %DEST%\src\unix\mimetype.cpp
copy /q src\unix\snglinst.cpp %DEST%\src\unix\snglinst.cpp
copy /q src\unix\threadpsx.cpp %DEST%\src\unix\threadpsx.cpp
copy /q src\unix\utilsunx.cpp %DEST%\src\unix\utilsunx.cpp
copy /q src\msw\dde.cpp %DEST%\src\msw\dde.cpp
copy /q src\msw\dir.cpp %DEST%\src\msw\dir.cpp
copy /q src\msw\gsocket.c %DEST%\src\msw\gsocket.c
copy /q src\msw\gsockmsw.c %DEST%\src\msw\gsockmsw.c
copy /q src\msw\main.cpp %DEST%\src\msw\main.cpp
copy /q src\msw\mimetype.cpp %DEST%\src\msw\mimetype.cpp
copy /q src\msw\regconf.cpp %DEST%\src\msw\regconf.cpp
copy /q src\msw\registry.cpp %DEST%\src\msw\registry.cpp
copy /q src\msw\snglinst.cpp %DEST%\src\msw\snglinst.cpp
copy /q src\msw\thread.cpp %DEST%\src\msw\thread.cpp
copy /q src\msw\utils.cpp %DEST%\src\msw\utils.cpp
copy /q src\msw\utilsexc.cpp %DEST%\src\msw\utilsexc.cpp
copy /q src\msw\volume.cpp %DEST%\src\msw\volume.cpp

copy /q include\wx\app.h %DEST%\include\wx\app.h
copy /q include\wx\arrimpl.cpp %DEST%\include\wx\arrimpl.cpp
copy /q include\wx\buffer.h %DEST%\include\wx\buffer.h
copy /q include\wx\build.h %DEST%\include\wx\build.h
copy /q include\wx\chkconf.h %DEST%\include\wx\chkconf.h
copy /q include\wx\clntdata.h %DEST%\include\wx\clntdata.h
copy /q include\wx\cmdline.h %DEST%\include\wx\cmdline.h
copy /q include\wx\confbase.h %DEST%\include\wx\confbase.h
copy /q include\wx\config.h %DEST%\include\wx\config.h
copy /q include\wx\date.h %DEST%\include\wx\date.h
copy /q include\wx\datetime.h %DEST%\include\wx\datetime.h
copy /q include\wx\datetime.inl %DEST%\include\wx\datetime.inl
copy /q include\wx\datstrm.h %DEST%\include\wx\datstrm.h
copy /q include\wx\db.h %DEST%\include\wx\db.h
copy /q include\wx\dbkeyg.h %DEST%\include\wx\dbkeyg.h
copy /q include\wx\dbtable.h %DEST%\include\wx\dbtable.h
copy /q include\wx\dde.h %DEST%\include\wx\dde.h
copy /q include\wx\debug.h %DEST%\include\wx\debug.h
copy /q include\wx\defs.h %DEST%\include\wx\defs.h
copy /q include\wx\dir.h %DEST%\include\wx\dir.h
copy /q include\wx\dynarray.h %DEST%\include\wx\dynarray.h
copy /q include\wx\dynlib.h %DEST%\include\wx\dynlib.h
copy /q include\wx\dynload.h %DEST%\include\wx\dynload.h
copy /q include\wx\encconv.h %DEST%\include\wx\encconv.h
copy /q include\wx\event.h %DEST%\include\wx\event.h
copy /q include\wx\features.h %DEST%\include\wx\features.h
copy /q include\wx\ffile.h %DEST%\include\wx\ffile.h
copy /q include\wx\file.h %DEST%\include\wx\file.h
copy /q include\wx\fileconf.h %DEST%\include\wx\fileconf.h
copy /q include\wx\filefn.h %DEST%\include\wx\filefn.h
copy /q include\wx\filename.h %DEST%\include\wx\filename.h
copy /q include\wx\filesys.h %DEST%\include\wx\filesys.h
copy /q include\wx\fontenc.h %DEST%\include\wx\fontenc.h
copy /q include\wx\fontmap.h %DEST%\include\wx\fontmap.h
copy /q include\wx\fs_inet.h %DEST%\include\wx\fs_inet.h
copy /q include\wx\fs_mem.h %DEST%\include\wx\fs_mem.h
copy /q include\wx\fs_zip.h %DEST%\include\wx\fs_zip.h
copy /q include\wx\gsocket.h %DEST%\include\wx\gsocket.h
copy /q include\wx\hash.h %DEST%\include\wx\hash.h
copy /q include\wx\hashmap.h %DEST%\include\wx\hashmap.h
copy /q include\wx\intl.h %DEST%\include\wx\intl.h
copy /q include\wx\ioswrap.h %DEST%\include\wx\ioswrap.h
copy /q include\wx\ipc.h %DEST%\include\wx\ipc.h
copy /q include\wx\ipcbase.h %DEST%\include\wx\ipcbase.h
copy /q include\wx\isql.h %DEST%\include\wx\isql.h
copy /q include\wx\isqlext.h %DEST%\include\wx\isqlext.h
copy /q include\wx\list.h %DEST%\include\wx\list.h
copy /q include\wx\listimpl.cpp %DEST%\include\wx\listimpl.cpp
copy /q include\wx\log.h %DEST%\include\wx\log.h
copy /q include\wx\longlong.h %DEST%\include\wx\longlong.h
copy /q include\wx\memconf.h %DEST%\include\wx\memconf.h
copy /q include\wx\memory.h %DEST%\include\wx\memory.h
copy /q include\wx\memtext.h %DEST%\include\wx\memtext.h
copy /q include\wx\mimetype.h %DEST%\include\wx\mimetype.h
copy /q include\wx\module.h %DEST%\include\wx\module.h
copy /q include\wx\msgout.h %DEST%\include\wx\msgout.h
copy /q include\wx\mstream.h %DEST%\include\wx\mstream.h
copy /q include\wx\object.h %DEST%\include\wx\object.h
copy /q include\wx\platform.h %DEST%\include\wx\platform.h
copy /q include\wx\process.h %DEST%\include\wx\process.h
copy /q include\wx\regex.h %DEST%\include\wx\regex.h
copy /q include\wx\sckaddr.h %DEST%\include\wx\sckaddr.h
copy /q include\wx\sckipc.h %DEST%\include\wx\sckipc.h
copy /q include\wx\sckstrm.h %DEST%\include\wx\sckstrm.h
copy /q include\wx\snglinst.h %DEST%\include\wx\snglinst.h
copy /q include\wx\socket.h %DEST%\include\wx\socket.h
copy /q include\wx\strconv.h %DEST%\include\wx\strconv.h
copy /q include\wx\stream.h %DEST%\include\wx\stream.h
copy /q include\wx\string.h %DEST%\include\wx\string.h
copy /q include\wx\sysopt.h %DEST%\include\wx\sysopt.h
copy /q include\wx\textbuf.h %DEST%\include\wx\textbuf.h
copy /q include\wx\textfile.h %DEST%\include\wx\textfile.h
copy /q include\wx\thread.h %DEST%\include\wx\thread.h
copy /q include\wx\thrimpl.cpp %DEST%\include\wx\thrimpl.cpp
copy /q include\wx\time.h %DEST%\include\wx\time.h
copy /q include\wx\timer.h %DEST%\include\wx\timer.h
copy /q include\wx\tokenzr.h %DEST%\include\wx\tokenzr.h
copy /q include\wx\txtstrm.h %DEST%\include\wx\txtstrm.h
copy /q include\wx\url.h %DEST%\include\wx\url.h
copy /q include\wx\utils.h %DEST%\include\wx\utils.h
copy /q include\wx\variant.h %DEST%\include\wx\variant.h
copy /q include\wx\vector.h %DEST%\include\wx\vector.h
copy /q include\wx\version.h %DEST%\include\wx\version.h
copy /q include\wx\volume.h %DEST%\include\wx\volume.h
copy /q include\wx\wfstream.h %DEST%\include\wx\wfstream.h
copy /q include\wx\wx.h %DEST%\include\wx\wx.h
copy /q include\wx\wxchar.h %DEST%\include\wx\wxchar.h
copy /q include\wx\wxprec.h %DEST%\include\wx\wxprec.h
copy /q include\wx\zipstrm.h %DEST%\include\wx\zipstrm.h
copy /q include\wx\zstream.h %DEST%\include\wx\zstream.h
copy /q include\wx\unix\gsockunx.h %DEST%\include\wx\unix\gsockunx.h
copy /q include\wx\unix\mimetype.h %DEST%\include\wx\unix\mimetype.h
copy /q include\wx\msw\dde.h %DEST%\include\wx\msw\dde.h
copy /q include\wx\msw\gccpriv.h %DEST%\include\wx\msw\gccpriv.h
copy /q include\wx\msw\mimetype.h %DEST%\include\wx\msw\mimetype.h
copy /q include\wx\msw\winundef.h %DEST%\include\wx\msw\winundef.h
copy /q include\wx\protocol\file.h %DEST%\include\wx\protocol\file.h
copy /q include\wx\protocol\ftp.h %DEST%\include\wx\protocol\ftp.h
copy /q include\wx\protocol\http.h %DEST%\include\wx\protocol\http.h
copy /q include\wx\protocol\protocol.h %DEST%\include\wx\protocol\protocol.h
