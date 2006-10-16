@echo off
rem   Builds a zip containing stuff needed to link with the wxWindows DLL
rem   shipped with wxPython.  This should allow other developers to create apps
rem   or extensions that can share the same DLL.

setlocal
set BASE=_distrib_zip\wxPython-%1

rem **** Make a directory to build up a distribution tree
mkdir _distrib_zip
mkdir %BASE%


rem *** copy files
copy distrib\README.devel.txt %BASE%\README.txt


mkdir %BASE%\include
mkdir %BASE%\include\wx
copy /s %WXWIN%\include\wx\*                 %BASE%\include\wx

copy /s %WXWIN%\contrib\include\wx\gizmos    %BASE%\include\wx\gizmos
copy /s %WXWIN%\contrib\include\wx\ogl       %BASE%\include\wx\ogl
copy /s %WXWIN%\contrib\include\wx\stc       %BASE%\include\wx\stc

copy /s %WXWIN%\wxPython\include\wx\*        %BASE%\include\wx
mkdir %BASE%\include\wx\wxPython\i_files
copy  %WXWIN%\wxPython\src\*.i               %BASE%\include\wx\wxPython\i_files
copy  %WXWIN%\wxPython\src\*.py              %BASE%\include\wx\wxPython\i_files

mkdir %BASE%\lib
mkdir %BASE%\lib\vc_dll
mkdir %BASE%\lib\vc_dll\mswh
mkdir %BASE%\lib\vc_dll\mswuh

copy /s %WXWIN%\lib\vc_dll\mswh\*            %BASE%\lib\vc_dll\mswh
copy /s %WXWIN%\lib\vc_dll\mswuh\*           %BASE%\lib\vc_dll\mswuh
copy %WXWIN%\lib\vc_dll\*                    %BASE%\lib\vc_dll


rem *** remove unneeded files
cd _distrib_zip

ffind /SB wx*d_*.*		>  del-files
ffind /SB wx*d.*		>> del-files
ffind /SB .#*			>> del-files
ffind /SB .cvsignore		>> del-files

for %f in (@del-files) do 	rm -fv %f
rem del /y @del-files


ffind /SB /A:D	CVS			>  del-dirs
echo  wxPython-%1\include\wx\cocoa 	>> del-dirs
echo  wxPython-%1\include\wx\gtk 	>> del-dirs
echo  wxPython-%1\include\wx\mac 	>> del-dirs
echo  wxPython-%1\include\wx\mgl 	>> del-dirs
echo  wxPython-%1\include\wx\motif 	>> del-dirs
echo  wxPython-%1\include\wx\os2 	>> del-dirs
echo  wxPython-%1\include\wx\univ 	>> del-dirs
echo  wxPython-%1\include\wx\unix 	>> del-dirs
echo  wxPython-%1\include\wx\x11 	>> del-dirs

for %d in (@del-dirs) do 	rm -rfv %d
rem del /sxzy @del-dirs

rm del-files
rm del-dirs

rem *** bundle it all up  TODO: don't hard-code the 2.6
tar cvf ../dist/wxPython2.7-win32-devel-%1.tar wxPython-%1
bzip2 -9 ../dist/wxPython2.7-win32-devel-%1.tar

rem *** cleanup
cd ..
rm -rf _distrib_zip

endlocal



