echo                               --------
echo    This script will copy the wxMSW release files into \wxmsw_dist
echo                               --------
echo                    

md \wxmsw_dist
md \wxmsw_dist\wxMSW

echo Base dir..

cd ..\..

echo Lib..

cd lib
md \wxmsw_dist\wxMSW\lib
copy dummy \wxmsw_dist\wxMSW\lib
cd ..

echo Locale..

cd locale
md \wxmsw_dist\wxMSW\locale
copy *.?? \wxmsw_dist\wxMSW\locale
cd ..

echo Docs..

cd docs\wine
copy COPYING.LIB \wxmsw_dist\wxMSW
copy licence.txt \wxmsw_dist\wxMSW\LICENCE.txt
cd ..\msw
copy install.txt \wxmsw_dist\wxMSW\INSTALL.txt
copy readme.txt \wxmsw_dist\wxMSW\README.txt
copy todo.txt \wxmsw_dist\wxMSW\TODO.txt
cd ..
copy symbols.txt \wxmsw_dist\wxMSW\SYMBOLS.txt
cd ..
echo Include dir..

md \wxmsw_dist\wxMSW\include
cd include
copy wx_*.* \wxmsw_dist\wxMSW\include

md \wxmsw_dist\wxMSW\include\wx
cd wx
copy *.h \wxmsw_dist\wxMSW\include\wx
copy *.cpp \wxmsw_dist\wxMSW\include\wx

md \wxmsw_dist\wxMSW\include\wx\generic
cd generic
copy *.h \wxmsw_dist\wxMSW\include\wx\generic
cd ..

md \wxmsw_dist\wxMSW\include\wx\html
cd html
copy *.h \wxmsw_dist\wxMSW\include\wx\html
md \wxmsw_dist\wxMSW\include\wx\html\msw
cd msw
copy *.* \wxmsw_dist\wxMSW\include\wx\html\msw
cd ..\..


md \wxmsw_dist\wxMSW\include\wx\msw
cd msw
copy *.* \wxmsw_dist\wxMSW\include\wx\msw
rem del \wxmsw_dist\wxMSW\include\wx\msw\setup.h
del \wxmsw_dist\wxMSW\include\wx\msw\Makefile.am
del \wxmsw_dist\wxMSW\include\wx\msw\Makefile.in
md \wxmsw_dist\wxMSW\include\wx\msw\gnuwin32
cd gnuwin32
copy *.h \wxmsw_dist\wxMSW\include\wx\msw\gnuwin32
cd ..
md \wxmsw_dist\wxMSW\include\wx\msw\ole
cd ole
copy *.h \wxmsw_dist\wxMSW\include\wx\msw\ole
cd ..
cd ctl3d
copy *.h \wxmsw_dist\wxMSW\include\wx\msw\ctl3d
cd ..
cd ..

md \wxmsw_dist\wxMSW\include\wx\protocol
cd protocol
copy *.h \wxmsw_dist\wxMSW\include\wx\protocol
cd ..

cd ..\..

echo Src dir..

cd src
md \wxmsw_dist\wxMSW\src
copy *.* \wxmsw_dist\wxMSW\src
del Makefile.am \wxmsw_dist\wxMSW\src\Makefile.am
del Makefile.in \wxmsw_dist\wxMSW\src\Makefile.in

cd msw
md \wxmsw_dist\wxMSW\src\msw
copy *.cpp \wxmsw_dist\wxMSW\src\msw
copy *.c \wxmsw_dist\wxMSW\src\msw
copy *.def \wxmsw_dist\wxMSW\src\msw
copy makefile.* \wxmsw_dist\wxMSW\src\msw
del Makefile.am \wxmsw_dist\wxMSW\src\msw\Makefile.am
del Makefile.in \wxmsw_dist\wxMSW\src\msw\Makefile.in
cd ..

cd common
md \wxmsw_dist\wxMSW\src\common
copy glob.inc \wxmsw_dist\wxMSW\src\common
copy lexer.l \wxmsw_dist\wxMSW\src\common
copy parser.y \wxmsw_dist\wxMSW\src\common
copy *.c \wxmsw_dist\wxMSW\src\common
copy *.h \wxmsw_dist\wxMSW\src\common
copy *.cpp \wxmsw_dist\wxMSW\src\common
cd ..

cd html
md \wxgtk_dist\wxGTK\src\html
copy *.cpp \wxgtk_dist\wxGTK\src\html
copy *.h \wxgtk_dist\wxGTK\src\html
cd bitmaps
md \wxgtk_dist\wxGTK\src\html\bitmaps
copy *.xpm \wxgtk_dist\wxGTK\src\html\bitmaps
cd ..\..

cd generic
md \wxmsw_dist\wxMSW\src\generic
copy *.cpp \wxmsw_dist\wxMSW\src\generic
cd ..

cd zlib
md \wxmsw_dist\wxMSW\src\zlib
copy *.* \wxmsw_dist\wxMSW\src\zlib
cd ..

cd png
md \wxmsw_dist\wxMSW\src\png
copy *.* \wxmsw_dist\wxMSW\src\png
cd ..

cd jpeg
md \wxmsw_dist\wxMSW\src\jpeg
copy *.* \wxmsw_dist\wxMSW\src\jpeg
cd ..

cd xpm
md \wxmsw_dist\wxMSW\src\xpm
copy *.* \wxmsw_dist\wxMSW\src\xpm
cd ..

cd ..

echo Utils dir..

cd utils
md \wxmsw_dist\wxMSW\utils

echo wxGLCanvas..

cd glcanvas
md \wxmsw_dist\wxMSW\utils\glcanvas
copy .\docs\notes.txt \wxmsw_dist\wxMSW\utils\glcanvas\NOTES.txt

md \wxmsw_dist\wxMSW\utils\glcanvas\win
copy .\win\glcanvas.cpp \wxmsw_dist\wxMSW\utils\glcanvas\win
copy .\win\glcanvas.h \wxmsw_dist\wxMSW\utils\glcanvas\win
copy .\win\makefile.* \wxmsw_dist\wxMSW\utils\glcanvas\win

md \wxmsw_dist\wxMSW\utils\glcanvas\samples
md \wxmsw_dist\wxMSW\utils\glcanvas\samples\cube
md \wxmsw_dist\wxMSW\utils\glcanvas\samples\isosurf
md \wxmsw_dist\wxMSW\utils\glcanvas\samples\penguin

cd samples\cube
copy Makefile.* \wxmsw_dist\wxMSW\utils\glcanvas\samples\cube
copy cube.rc \wxmsw_dist\wxMSW\utils\glcanvas\samples\cube
copy cube.h \wxmsw_dist\wxMSW\utils\glcanvas\samples\cube
copy cube.cpp \wxmsw_dist\wxMSW\utils\glcanvas\samples\cube
copy mondrian.ico \wxmsw_dist\wxMSW\utils\glcanvas\samples\cube
cd ..

cd isosurf
copy Makefile.* \wxmsw_dist\wxMSW\utils\glcanvas\samples\isosurf
copy isosurf.h \wxmsw_dist\wxMSW\utils\glcanvas\samples\isosurf
copy isosurf.rc \wxmsw_dist\wxMSW\utils\glcanvas\samples\isosurf
copy isosurf.cpp \wxmsw_dist\wxMSW\utils\glcanvas\samples\isosurf
copy isosurf.dat.gz \wxmsw_dist\wxMSW\utils\glcanvas\samples\isosurf
copy mondrian.ico \wxmsw_dist\wxMSW\utils\glcanvas\samples\isosurf
cd ..

cd penguin
copy Makefile.* \wxmsw_dist\wxMSW\utils\glcanvas\samples\penguin
copy penguin.rc \wxmsw_dist\wxMSW\utils\glcanvas\samples\penguin
copy penguin.h \wxmsw_dist\wxMSW\utils\glcanvas\samples\penguin
copy penguin.cpp \wxmsw_dist\wxMSW\utils\glcanvas\samples\penguin
copy trackball.h \wxmsw_dist\wxMSW\utils\glcanvas\samples\penguin
copy trackball.c \wxmsw_dist\wxMSW\utils\glcanvas\samples\penguin
copy lw.h \wxmsw_dist\wxMSW\utils\glcanvas\samples\penguin
copy lw.cpp \wxmsw_dist\wxMSW\utils\glcanvas\samples\penguin
copy penguin.lwo \wxmsw_dist\wxMSW\utils\glcanvas\samples\penguin
copy mondrian.ico \wxmsw_dist\wxMSW\utils\glcanvas\samples\penguin
cd ..\..\..

echo DialogEd..

cd dialoged\src
md \wxmsw_dist\wxMSW\utils\dialoged
md \wxmsw_dist\wxMSW\utils\dialoged\src
copy Makefile.* \wxmsw_dist\wxMSW\utils\dialoged\src
copy dialoged.def \wxmsw_dist\wxMSW\utils\dialoged\src
copy dialoged.rc \wxmsw_dist\wxMSW\utils\dialoged\src
copy dialoged.ico \wxmsw_dist\wxMSW\utils\dialoged\src
copy dialoged.cpp \wxmsw_dist\wxMSW\utils\dialoged\src
copy dlghndlr.cpp \wxmsw_dist\wxMSW\utils\dialoged\src
copy edlist.cpp \wxmsw_dist\wxMSW\utils\dialoged\src
copy edtree.cpp \wxmsw_dist\wxMSW\utils\dialoged\src
copy reseditr.cpp \wxmsw_dist\wxMSW\utils\dialoged\src
copy reswrite.cpp \wxmsw_dist\wxMSW\utils\dialoged\src
copy symbtabl.cpp \wxmsw_dist\wxMSW\utils\dialoged\src
copy winprop.cpp \wxmsw_dist\wxMSW\utils\dialoged\src
copy winstyle.cpp \wxmsw_dist\wxMSW\utils\dialoged\src
copy dialoged.h \wxmsw_dist\wxMSW\utils\dialoged\src
copy dlghndlr.h \wxmsw_dist\wxMSW\utils\dialoged\src
copy edlist.h \wxmsw_dist\wxMSW\utils\dialoged\src
copy edtree.h \wxmsw_dist\wxMSW\utils\dialoged\src
copy reseditr.h \wxmsw_dist\wxMSW\utils\dialoged\src
copy symbtabl.h \wxmsw_dist\wxMSW\utils\dialoged\src
copy winprop.h \wxmsw_dist\wxMSW\utils\dialoged\src
copy winstyle.h \wxmsw_dist\wxMSW\utils\dialoged\src

cd bitmaps
md \wxmsw_dist\wxMSW\utils\dialoged\src\bitmaps
copy *.bmp \wxmsw_dist\wxMSW\utils\dialoged\src\bitmaps
copy *.ico \wxmsw_dist\wxMSW\utils\dialoged\src\bitmaps
cd ..\..\..


cd ..

echo Samples dir..

cd samples
md \wxmsw_dist\wxMSW\samples
copy makefile.* \wxmsw_dist\wxMSW\samples

echo Minimal sample..

cd minimal
md \wxmsw_dist\wxMSW\samples\minimal
copy Makefile.* \wxmsw_dist\wxMSW\samples\minimal
copy *.def \wxmsw_dist\wxMSW\samples\minimal
copy *.rc \wxmsw_dist\wxMSW\samples\minimal
copy minimal.cpp \wxmsw_dist\wxMSW\samples\minimal
copy mondrian.ico \wxmsw_dist\wxMSW\samples\minimal
cd ..


echo Bombs sample..

cd bombs
md \wxmsw_dist\wxMSW\samples\bombs
copy Makefile.* \wxmsw_dist\wxMSW\samples\bombs
copy *.cpp \wxmsw_dist\wxMSW\samples\bombs
copy *.h \wxmsw_dist\wxMSW\samples\bombs
copy *.ico \wxmsw_dist\wxMSW\samples\bombs
copy *.def \wxmsw_dist\wxMSW\samples\bombs
copy *.rc \wxmsw_dist\wxMSW\samples\bombs
cd ..

echo Caret sample..

cd caret
md \wxmsw_dist\wxMSW\samples\caret
copy Makefile.* \wxmsw_dist\wxMSW\samples\caret
copy *.cpp \wxmsw_dist\wxMSW\samples\caret
copy *.h \wxmsw_dist\wxMSW\samples\caret
copy *.ico \wxmsw_dist\wxMSW\samples\caret
copy *.def \wxmsw_dist\wxMSW\samples\caret
copy *.rc \wxmsw_dist\wxMSW\samples\caret
cd ..

echo Checklst sample..

cd checklst
md \wxmsw_dist\wxMSW\samples\checklst
copy Makefile.* \wxmsw_dist\wxMSW\samples\checklst
copy *.h \wxmsw_dist\wxMSW\samples\checklst
copy *.cpp \wxmsw_dist\wxMSW\samples\checklst
copy *.ico \wxmsw_dist\wxMSW\samples\checklst
copy *.def \wxmsw_dist\wxMSW\samples\checklst
copy *.rc \wxmsw_dist\wxMSW\samples\checklst
cd ..

echo Config sample..

cd config
md \wxmsw_dist\wxMSW\samples\config
copy Makefile.* \wxmsw_dist\wxMSW\samples\config
copy *.h \wxmsw_dist\wxMSW\samples\config
copy *.cpp \wxmsw_dist\wxMSW\samples\config
copy *.ico \wxmsw_dist\wxMSW\samples\config
copy *.def \wxmsw_dist\wxMSW\samples\config
copy *.rc \wxmsw_dist\wxMSW\samples\config
cd ..

echo Controls sample..

cd controls
md \wxmsw_dist\wxMSW\samples\controls
copy Makefile.* \wxmsw_dist\wxMSW\samples\controls
copy *.cpp \wxmsw_dist\wxMSW\samples\controls
copy *.h \wxmsw_dist\wxMSW\samples\controls
copy *.rc \wxmsw_dist\wxMSW\samples\controls
copy *.def \wxmsw_dist\wxMSW\samples\controls
copy *.ico \wxmsw_dist\wxMSW\samples\controls
cd icons
md \wxmsw_dist\wxMSW\samples\controls\icons
copy *.* \wxmsw_dist\wxMSW\samples\controls\icons
cd ..\..

echo Dialogs sample..

cd dialogs
md \wxmsw_dist\wxMSW\samples\dialogs
copy Makefile.* \wxmsw_dist\wxMSW\samples\dialogs
copy *.cpp \wxmsw_dist\wxMSW\samples\dialogs
copy *.h \wxmsw_dist\wxMSW\samples\dialogs
copy *.def \wxmsw_dist\wxMSW\samples\dialogs
copy *.rc \wxmsw_dist\wxMSW\samples\dialogs
copy *.ico \wxmsw_dist\wxMSW\samples\dialogs
cd ..

echo DnD sample..

cd dnd
md \wxmsw_dist\wxMSW\samples\dnd
copy Makefile.* \wxmsw_dist\wxMSW\samples\dnd
copy *.cpp \wxmsw_dist\wxMSW\samples\dnd
copy *.h \wxmsw_dist\wxMSW\samples\dnd
copy *.def \wxmsw_dist\wxMSW\samples\dnd
copy *.rc \wxmsw_dist\wxMSW\samples\dnd
copy *.ico \wxmsw_dist\wxMSW\samples\dnd
cd ..

echo Docview sample..

cd docview
md \wxmsw_dist\wxMSW\samples\docview
copy Makefile.* \wxmsw_dist\wxMSW\samples\docview
copy *.cpp \wxmsw_dist\wxMSW\samples\docview
copy *.h \wxmsw_dist\wxMSW\samples\docview
copy *.def \wxmsw_dist\wxMSW\samples\docview
copy *.rc \wxmsw_dist\wxMSW\samples\docview
copy *.ico \wxmsw_dist\wxMSW\samples\docview
cd ..

echo DocvwMDI sample..

cd docvwmdi
md \wxmsw_dist\wxMSW\samples\docvwmdi
copy Makefile.* \wxmsw_dist\wxMSW\samples\docvwmdi
copy *.cpp \wxmsw_dist\wxMSW\samples\docvwmdi
copy *.h \wxmsw_dist\wxMSW\samples\docvwmdi
copy *.def \wxmsw_dist\wxMSW\samples\docvwmdi
copy *.rc \wxmsw_dist\wxMSW\samples\docvwmdi
copy *.ico \wxmsw_dist\wxMSW\samples\docvwmdi
cd ..

echo Dynamic sample..

cd dynamic
md \wxmsw_dist\wxMSW\samples\dynamic
copy Makefile.* \wxmsw_dist\wxMSW\samples\dynamic
copy *.cpp \wxmsw_dist\wxMSW\samples\dynamic
copy *.h \wxmsw_dist\wxMSW\samples\dynamic
copy *.def \wxmsw_dist\wxMSW\samples\dynamic
copy *.rc \wxmsw_dist\wxMSW\samples\dynamic
copy *.ico \wxmsw_dist\wxMSW\samples\dynamic
cd ..

echo Drawing sample..

cd drawing
md \wxmsw_dist\wxMSW\samples\drawing
copy Makefile.* \wxmsw_dist\wxMSW\samples\drawing
copy *.cpp \wxmsw_dist\wxMSW\samples\drawing
copy *.ico \wxmsw_dist\wxMSW\samples\drawing
copy *.h \wxmsw_dist\wxMSW\samples\drawing
copy *.rc \wxmsw_dist\wxMSW\samples\drawing
copy *.def \wxmsw_dist\wxMSW\samples\drawing
cd ..

echo Forty sample..

cd forty
md \wxmsw_dist\wxMSW\samples\forty
copy Makefile.* \wxmsw_dist\wxMSW\samples\forty
copy *.cpp \wxmsw_dist\wxMSW\samples\forty
copy *.h \wxmsw_dist\wxMSW\samples\forty
copy *.def \wxmsw_dist\wxMSW\samples\forty
copy *.rc \wxmsw_dist\wxMSW\samples\forty
copy *.ico \wxmsw_dist\wxMSW\samples\forty
copy *.bmp \wxmsw_dist\wxMSW\samples\forty
copy *.xpm \wxmsw_dist\wxMSW\samples\forty
copy *.xbm \wxmsw_dist\wxMSW\samples\forty
cd ..

echo Fractal sample..

cd fractal
md \wxmsw_dist\wxMSW\samples\fractal
copy Makefile.* \wxmsw_dist\wxMSW\samples\fractal
copy *.cpp \wxmsw_dist\wxMSW\samples\fractal
copy *.h \wxmsw_dist\wxMSW\samples\fractal
copy *.def \wxmsw_dist\wxMSW\samples\fractal
copy *.rc \wxmsw_dist\wxMSW\samples\fractal
copy *.ico \wxmsw_dist\wxMSW\samples\fractal
cd ..

echo Grid sample..

cd grid
md \wxmsw_dist\wxMSW\samples\grid
copy Makefile.* \wxmsw_dist\wxMSW\samples\grid
copy *.cpp \wxmsw_dist\wxMSW\samples\grid
copy *.h \wxmsw_dist\wxMSW\samples\grid
copy *.def \wxmsw_dist\wxMSW\samples\grid
copy *.rc \wxmsw_dist\wxMSW\samples\grid
copy *.ico \wxmsw_dist\wxMSW\samples\grid
cd ..

echo Help sample..

cd help
md \wxmsw_dist\wxMSW\samples\help
copy Makefile.* \wxmsw_dist\wxMSW\samples\help
copy *.cpp \wxmsw_dist\wxMSW\samples\help
copy *.def \wxmsw_dist\wxMSW\samples\help
copy *.rc \wxmsw_dist\wxMSW\samples\help
copy *.ico \wxmsw_dist\wxMSW\samples\help
copy *.xpm \wxmsw_dist\wxMSW\samples\help
cd doc
md \wxmsw_dist\wxMSW\samples\help\doc
copy *.* \wxmsw_dist\wxMSW\samples\help\doc
cd ..\..

echo Image sample..

cd image
md \wxmsw_dist\wxMSW\samples\image
copy Makefile.* \wxmsw_dist\wxMSW\samples\image
copy *.cpp \wxmsw_dist\wxMSW\samples\image
copy *.def \wxmsw_dist\wxMSW\samples\image
copy *.rc \wxmsw_dist\wxMSW\samples\image
copy *.ico \wxmsw_dist\wxMSW\samples\image
copy horse.png \wxmsw_dist\wxMSW\samples\image
copy horse.gif \wxmsw_dist\wxMSW\samples\image
copy horse.jpg \wxmsw_dist\wxMSW\samples\image
cd ..

echo Internat sample..

cd internat
md \wxmsw_dist\wxMSW\samples\internat
copy Makefile.* \wxmsw_dist\wxMSW\samples\internat
copy *.cpp \wxmsw_dist\wxMSW\samples\internat
copy *.def \wxmsw_dist\wxMSW\samples\internat
copy *.rc \wxmsw_dist\wxMSW\samples\internat
copy *.ico \wxmsw_dist\wxMSW\samples\internat
copy readme.txt \wxmsw_dist\wxMSW\samples\internat
copy wxstd.po \wxmsw_dist\wxMSW\samples\internat
cd fr
md \wxmsw_dist\wxMSW\samples\internat\fr
copy *.?o \wxmsw_dist\wxMSW\samples\internat\fr
cd ..\..

echo Layout sample..

cd layout
md \wxmsw_dist\wxMSW\samples\layout
copy Makefile.* \wxmsw_dist\wxMSW\samples\layout
copy *.cpp \wxmsw_dist\wxMSW\samples\layout
copy *.ico \wxmsw_dist\wxMSW\samples\layout
copy *.def \wxmsw_dist\wxMSW\samples\layout
copy *.rc \wxmsw_dist\wxMSW\samples\layout
copy *.h \wxmsw_dist\wxMSW\samples\layout
cd ..

echo Listctrl sample..

cd listctrl
md \wxmsw_dist\wxMSW\samples\listctrl
copy Makefile.* \wxmsw_dist\wxMSW\samples\listctrl
copy *.cpp \wxmsw_dist\wxMSW\samples\listctrl
copy *.h \wxmsw_dist\wxMSW\samples\listctrl
copy *.def \wxmsw_dist\wxMSW\samples\listctrl
copy *.rc \wxmsw_dist\wxMSW\samples\listctrl
copy *.ico \wxmsw_dist\wxMSW\samples\listctrl
cd bitmaps
md \wxmsw_dist\wxMSW\samples\listctrl\bitmaps
copy *.* \wxmsw_dist\wxMSW\samples\listctrl\bitmaps
cd ..\..

echo MDI sample..

cd mdi
md \wxmsw_dist\wxMSW\samples\mdi
copy Makefile.* \wxmsw_dist\wxMSW\samples\mdi
copy *.cpp \wxmsw_dist\wxMSW\samples\mdi
copy *.rc \wxmsw_dist\wxMSW\samples\mdi
copy *.def \wxmsw_dist\wxMSW\samples\mdi
copy *.h \wxmsw_dist\wxMSW\samples\mdi
copy *.ico \wxmsw_dist\wxMSW\samples\mdi
cd bitmaps
md \wxmsw_dist\wxMSW\samples\mdi\bitmaps
copy *.* \wxmsw_dist\wxMSW\samples\mdi\bitmaps
cd ..\..

echo Memcheck sample..

cd memcheck
md \wxmsw_dist\wxMSW\samples\memcheck
copy Makefile.* \wxmsw_dist\wxMSW\samples\memcheck
copy *.cpp \wxmsw_dist\wxMSW\samples\memcheck
copy *.ico \wxmsw_dist\wxMSW\samples\memcheck
copy *.rc \wxmsw_dist\wxMSW\samples\memcheck
copy *.def \wxmsw_dist\wxMSW\samples\memcheck
copy *.xpm \wxmsw_dist\wxMSW\samples\memcheck
cd ..

echo Minifram sample..

cd minifram
md \wxmsw_dist\wxMSW\samples\minifram
copy Makefile.am \wxmsw_dist\wxMSW\samples\minifram
copy Makefile.in \wxmsw_dist\wxMSW\samples\minifram
copy *.cpp \wxmsw_dist\wxMSW\samples\minifram
copy *.xpm \wxmsw_dist\wxMSW\samples\minifram
copy *.ico \wxmsw_dist\wxMSW\samples\minifram
copy *.def \wxmsw_dist\wxMSW\samples\minifram
copy *.rc \wxmsw_dist\wxMSW\samples\minifram
cd bitmaps
md \wxmsw_dist\wxMSW\samples\minifram\bitmaps
copy *.* \wxmsw_dist\wxMSW\samples\minifram\bitmaps
cd ..\..

echo Notebook sample..

cd notebook
md \wxmsw_dist\wxMSW\samples\notebook
copy Makefile.* \wxmsw_dist\wxMSW\samples\notebook
copy *.cpp \wxmsw_dist\wxMSW\samples\notebook
copy *.ico \wxmsw_dist\wxMSW\samples\notebook
copy *.def \wxmsw_dist\wxMSW\samples\notebook
copy *.rc \wxmsw_dist\wxMSW\samples\notebook
copy *.h \wxmsw_dist\wxMSW\samples\notebook
cd ..

echo Printing sample..

cd printing
md \wxmsw_dist\wxMSW\samples\printing
copy Makefile.* \wxmsw_dist\wxMSW\samples\printing
copy *.cpp \wxmsw_dist\wxMSW\samples\printing
copy *.h \wxmsw_dist\wxMSW\samples\printing
copy *.rc \wxmsw_dist\wxMSW\samples\printing
copy *.ico \wxmsw_dist\wxMSW\samples\printing
copy *.def \wxmsw_dist\wxMSW\samples\printing
copy *.xpm \wxmsw_dist\wxMSW\samples\printing
cd ..

echo Proplist sample..

cd proplist
md \wxmsw_dist\wxMSW\samples\proplist
copy Makefile.* \wxmsw_dist\wxMSW\samples\proplist
copy *.rc \wxmsw_dist\wxMSW\samples\printing
copy *.def \wxmsw_dist\wxMSW\samples\printing
copy *.cpp \wxmsw_dist\wxMSW\samples\proplist
copy *.ico \wxmsw_dist\wxMSW\samples\printing
copy *.h \wxmsw_dist\wxMSW\samples\proplist
cd ..

echo Resource sample..

cd resource
md \wxmsw_dist\wxMSW\samples\resource
copy Makefile.* \wxmsw_dist\wxMSW\samples\resource
copy *.ico \wxmsw_dist\wxMSW\samples\resource
copy *.rc \wxmsw_dist\wxMSW\samples\resource
copy *.def \wxmsw_dist\wxMSW\samples\resource
copy *.cpp \wxmsw_dist\wxMSW\samples\resource
copy *.h \wxmsw_dist\wxMSW\samples\resource
copy *.wxr \wxmsw_dist\wxMSW\samples\resource
cd ..

echo Sashtest sample..

cd sashtest
md \wxmsw_dist\wxMSW\samples\sashtest
copy Makefile.* \wxmsw_dist\wxMSW\samples\sashtest
copy *.cpp \wxmsw_dist\wxMSW\samples\sashtest
copy *.rc \wxmsw_dist\wxMSW\samples\sashtest
copy *.h \wxmsw_dist\wxMSW\samples\sashtest
copy *.def \wxmsw_dist\wxMSW\samples\sashtest
copy *.ico \wxmsw_dist\wxMSW\samples\sashtest
cd ..

echo Scroll sample..

cd sashtest
md \wxmsw_dist\wxMSW\samples\scroll
copy Makefile.* \wxmsw_dist\wxMSW\samples\scroll
copy *.cpp \wxmsw_dist\wxMSW\samples\scroll
copy *.def \wxmsw_dist\wxMSW\samples\scroll
copy *.rc \wxmsw_dist\wxMSW\samples\scroll
copy *.ico \wxmsw_dist\wxMSW\samples\scroll
copy *.h \wxmsw_dist\wxMSW\samples\scroll
cd ..

echo Splitter sample..

cd splitter
md \wxmsw_dist\wxMSW\samples\splitter
copy Makefile.* \wxmsw_dist\wxMSW\samples\splitter
copy *.cpp \wxmsw_dist\wxMSW\samples\splitter
copy *.def \wxmsw_dist\wxMSW\samples\splitter
copy *.rc \wxmsw_dist\wxMSW\samples\splitter
copy *.ico \wxmsw_dist\wxMSW\samples\splitter
cd ..

echo Text sample..

cd text
md \wxmsw_dist\wxMSW\samples\text
copy Makefile.* \wxmsw_dist\wxMSW\samples\text
copy *.cpp \wxmsw_dist\wxMSW\samples\text
copy *.def \wxmsw_dist\wxMSW\samples\text
copy *.rc \wxmsw_dist\wxMSW\samples\text
copy *.ico \wxmsw_dist\wxMSW\samples\text
cd ..

echo Thread sample..

cd thread
md \wxmsw_dist\wxMSW\samples\thread
copy Makefile.* \wxmsw_dist\wxMSW\samples\thread
copy *.cpp \wxmsw_dist\wxMSW\samples\thread
copy *.def \wxmsw_dist\wxMSW\samples\thread
copy *.rc \wxmsw_dist\wxMSW\samples\thread
copy *.ico \wxmsw_dist\wxMSW\samples\thread
cd ..

echo Toolbar sample..

cd toolbar
md \wxmsw_dist\wxMSW\samples\toolbar
copy Makefile.am \wxmsw_dist\wxMSW\samples\toolbar
copy Makefile.in \wxmsw_dist\wxMSW\samples\toolbar
copy *.cpp \wxmsw_dist\wxMSW\samples\toolbar
copy *.h \wxmsw_dist\wxMSW\samples\toolbar
copy *.rc \wxmsw_dist\wxMSW\samples\toolbar
copy *.def \wxmsw_dist\wxMSW\samples\toolbar
copy *.xpm \wxmsw_dist\wxMSW\samples\toolbar
copy *.ico \wxmsw_dist\wxMSW\samples\toolbar
cd bitmaps
md \wxmsw_dist\wxMSW\samples\toolbar\bitmaps
copy *.* \wxmsw_dist\wxMSW\samples\toolbar\bitmaps
cd ..\..

echo TreeCtrl sample..

cd treectrl
md \wxmsw_dist\wxMSW\samples\treectrl
copy Makefile.* \wxmsw_dist\wxMSW\samples\treectrl
copy *.rc \wxmsw_dist\wxMSW\samples\toolbar
copy *.def \wxmsw_dist\wxMSW\samples\toolbar
copy *.cpp \wxmsw_dist\wxMSW\samples\treectrl
copy *.h \wxmsw_dist\wxMSW\samples\treectrl
copy *.xpm \wxmsw_dist\wxMSW\samples\treectrl
copy *.ico \wxmsw_dist\wxMSW\samples\treectrl
cd ..

echo typetest sample..

cd typetest
md \wxmsw_dist\wxMSW\samples\typetest
copy Makefile.* \wxmsw_dist\wxMSW\samples\typetest
copy *.cpp \wxmsw_dist\wxMSW\samples\typetest
copy *.h \wxmsw_dist\wxMSW\samples\typetest
copy *.ico \wxmsw_dist\wxMSW\samples\typetest
copy *.def \wxmsw_dist\wxMSW\samples\typetest
copy *.rc \wxmsw_dist\wxMSW\samples\typetest
cd ..

echo Validate sample..

cd validate
md \wxmsw_dist\wxMSW\samples\validate
copy Makefile.* \wxmsw_dist\wxMSW\samples\validate
copy *.cpp \wxmsw_dist\wxMSW\samples\validate
copy *.h \wxmsw_dist\wxMSW\samples\validate
copy *.xpm \wxmsw_dist\wxMSW\samples\validate
copy *.rc \wxmsw_dist\wxMSW\samples\validate
copy *.def \wxmsw_dist\wxMSW\samples\validate
cd ..

echo wxPoem sample..

cd wxpoem
md \wxmsw_dist\wxMSW\samples\wxpoem
copy Makefile.* \wxmsw_dist\wxMSW\samples\wxpoem
copy *.cpp \wxmsw_dist\wxMSW\samples\wxpoem
copy *.h \wxmsw_dist\wxMSW\samples\wxpoem
copy *.xpm \wxmsw_dist\wxMSW\samples\wxpoem
copy *.def \wxmsw_dist\wxMSW\samples\wxpoem
copy *.rc \wxmsw_dist\wxMSW\samples\wxpoem
copy wxpoem.dat \wxmsw_dist\wxMSW\samples\wxpoem
copy wxpoem.txt \wxmsw_dist\wxMSW\samples\wxpoem
copy wxpoem.idx \wxmsw_dist\wxMSW\samples\wxpoem
cd ..

echo wxSocket sample..

cd wxsocket
md \wxmsw_dist\wxMSW\samples\wxsocket
copy Makefile.* \wxmsw_dist\wxMSW\samples\wxsocket
copy *.cpp \wxmsw_dist\wxMSW\samples\wxsocket
copy *.h \wxmsw_dist\wxMSW\samples\wxsocket
copy *.rc \wxmsw_dist\wxMSW\samples\wxsocket
copy *.def \wxmsw_dist\wxMSW\samples\wxsocket
copy *.xpm \wxmsw_dist\wxMSW\samples\wxsocket
cd ..\..

