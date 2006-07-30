@echo off

echo starting >  %WXWIN%\distrib\msw\proc_bcb.log
if "%WXWIN%=="" goto nowxdir
if not exist %WXWIN%\distrib\msw\process_sample_bcb.bat goto nocommand
cd %WXWIN%\samples\calendar

call  %WXWIN%\distrib\msw\process_sample_bcb calendar
cd %WXWIN%\samples\caret
call  %WXWIN%\distrib\msw\process_sample_bcb caret
cd %WXWIN%\samples\checklst
call  %WXWIN%\distrib\msw\process_sample_bcb checklst
cd %WXWIN%\samples\config
call  %WXWIN%\distrib\msw\process_sample_bcb conftest
cd %WXWIN%\samples\console
call  %WXWIN%\distrib\msw\process_sample_bcb console
cd %WXWIN%\samples\controls
call  %WXWIN%\distrib\msw\process_sample_bcb controls
cd %WXWIN%\samples\db
call  %WXWIN%\distrib\msw\process_sample_bcb dbtest
cd %WXWIN%\samples\dialogs
call  %WXWIN%\distrib\msw\process_sample_bcb dialogs
cd %WXWIN%\samples\dialup
rem call  %WXWIN%\distrib\msw\process_sample_bcb nettest
rem uncooment line above to do dialup
cd %WXWIN%\samples\dnd
call  %WXWIN%\distrib\msw\process_sample_bcb dnd
cd %WXWIN%\samples\docview
call  %WXWIN%\distrib\msw\process_sample_bcb docview doc view
cd %WXWIN%\samples\docvwmdi
call  %WXWIN%\distrib\msw\process_sample_bcb docview doc view
cd %WXWIN%\samples\dragimag
call  %WXWIN%\distrib\msw\process_sample_bcb dragimag
cd %WXWIN%\samples\drawing
call  %WXWIN%\distrib\msw\process_sample_bcb drawing
cd %WXWIN%\samples\dynamic
call  %WXWIN%\distrib\msw\process_sample_bcb dynamic
cd %WXWIN%\samples\erase
call  %WXWIN%\distrib\msw\process_sample_bcb erase
cd %WXWIN%\samples\exec
call  %WXWIN%\distrib\msw\process_sample_bcb exec
cd %WXWIN%\samples\event
call  %WXWIN%\distrib\msw\process_sample_bcb event
cd %WXWIN%\samples\font
call  %WXWIN%\distrib\msw\process_sample_bcb font
cd %WXWIN%\samples\grid
call  %WXWIN%\distrib\msw\process_sample_bcb grid
cd %WXWIN%\samples\help
call  %WXWIN%\distrib\msw\process_sample_bcb demo

        cd %WXWIN%\samples\html\about
call %WXWIN%\distrib\msw\process_sample_bcb about
        cd %WXWIN%\samples\html\help
call %WXWIN%\distrib\msw\process_sample_bcb help
        cd %WXWIN%\samples\html\helpview
call %WXWIN%\distrib\msw\process_sample_bcb helpview
        cd %WXWIN%\samples\html\printing
call %WXWIN%\distrib\msw\process_sample_bcb printing
        cd %WXWIN%\samples\html\test
call %WXWIN%\distrib\msw\process_sample_bcb test
        cd %WXWIN%\samples\html\virtual
call %WXWIN%\distrib\msw\process_sample_bcb virtual
        cd %WXWIN%\samples\html\widget
call %WXWIN%\distrib\msw\process_sample_bcb widget
        cd %WXWIN%\samples\html\zip
call %WXWIN%\distrib\msw\process_sample_bcb zip


cd %WXWIN%\samples\image
call  %WXWIN%\distrib\msw\process_sample_bcb image
cd %WXWIN%\samples\internat
call  %WXWIN%\distrib\msw\process_sample_bcb internat
cd %WXWIN%\samples\ipc
call  %WXWIN%\distrib\msw\process_sample_bcb server
call  %WXWIN%\distrib\msw\process_sample_bcb client
cd %WXWIN%\samples\joytest
call  %WXWIN%\distrib\msw\process_sample_bcb joytest
cd %WXWIN%\samples\listbox
call  %WXWIN%\distrib\msw\process_sample_bcb lboxtest
cd %WXWIN%\samples\listctrl
call  %WXWIN%\distrib\msw\process_sample_bcb listtest
cd %WXWIN%\samples\mdi
call  %WXWIN%\distrib\msw\process_sample_bcb mdi
cd %WXWIN%\samples\memcheck
call  %WXWIN%\distrib\msw\process_sample_bcb memcheck
cd %WXWIN%\samples\menu
call  %WXWIN%\distrib\msw\process_sample_bcb menu
cd %WXWIN%\samples\mfc
call  %WXWIN%\distrib\msw\process_sample_bcb mfctest
cd %WXWIN%\samples\minifram
call  %WXWIN%\distrib\msw\process_sample_bcb minifram
cd %WXWIN%\samples\minimal
call  %WXWIN%\distrib\msw\process_sample_bcb minimal
cd %WXWIN%\samples\nativdlg
call  %WXWIN%\distrib\msw\process_sample_bcb nativdlg
cd %WXWIN%\samples\newgrid
call  %WXWIN%\distrib\msw\process_sample_bcb griddemo
cd %WXWIN%\samples\notebook
call  %WXWIN%\distrib\msw\process_sample_bcb notebook
cd %WXWIN%\samples\oleauto
call  %WXWIN%\distrib\msw\process_sample_bcb oleauto
cd %WXWIN%\samples\opengl\cube
call  %WXWIN%\distrib\msw\process_sample_bcb cube
cd %WXWIN%\samples\opengl\isosurf
call  %WXWIN%\distrib\msw\process_sample_bcb isosurf
cd %WXWIN%\samples\opengl\penguin
call  %WXWIN%\distrib\msw\process_sample_bcb penguin lw trackball

cd %WXWIN%\samples\ownerdrw
call  %WXWIN%\distrib\msw\process_sample_bcb ownerdrw

cd %WXWIN%\samples\png
call  %WXWIN%\distrib\msw\process_sample_bcb pngdemo
cd %WXWIN%\samples\printing
call  %WXWIN%\distrib\msw\process_sample_bcb printing
cd %WXWIN%\samples\proplist
call  %WXWIN%\distrib\msw\process_sample_bcb proplist
cd %WXWIN%\samples\propsize
call  %WXWIN%\distrib\msw\process_sample_bcb propsize
cd %WXWIN%\samples\regtest
call  %WXWIN%\distrib\msw\process_sample_bcb regtest
cd %WXWIN%\samples\resource
call  %WXWIN%\distrib\msw\process_sample_bcb resource
cd %WXWIN%\samples\richedit
call  %WXWIN%\distrib\msw\process_sample_bcb wxLayout kbList wxllist wxlparser  wxlwindow
cd %WXWIN%\samples\rotate
call  %WXWIN%\distrib\msw\process_sample_bcb rotate
cd %WXWIN%\samples\sashtest
call  %WXWIN%\distrib\msw\process_sample_bcb sashtest
cd %WXWIN%\samples\scroll
call  %WXWIN%\distrib\msw\process_sample_bcb scroll
cd %WXWIN%\samples\scrollsub
call  %WXWIN%\distrib\msw\process_sample_bcb scrollsub
cd %WXWIN%\samples\sockets
call  %WXWIN%\distrib\msw\process_sample_bcb server
call  %WXWIN%\distrib\msw\process_sample_bcb client
cd %WXWIN%\samples\splitter
call  %WXWIN%\distrib\msw\process_sample_bcb splitter
cd %WXWIN%\samples\statbar
call  %WXWIN%\distrib\msw\process_sample_bcb statbar
cd %WXWIN%\samples\tab
call  %WXWIN%\distrib\msw\process_sample_bcb tab
cd %WXWIN%\samples\taskbar
call  %WXWIN%\distrib\msw\process_sample_bcb tbtest
cd %WXWIN%\samples\text
call  %WXWIN%\distrib\msw\process_sample_bcb text
cd %WXWIN%\samples\thread
call  %WXWIN%\distrib\msw\process_sample_bcb thread
cd %WXWIN%\samples\toolbar
call  %WXWIN%\distrib\msw\process_sample_bcb toolbar
cd %WXWIN%\samples\treectrl
call  %WXWIN%\distrib\msw\process_sample_bcb treectrl
cd %WXWIN%\samples\treelay
call  %WXWIN%\distrib\msw\process_sample_bcb treelay
cd %WXWIN%\samples\typetest
call  %WXWIN%\distrib\msw\process_sample_bcb typetest
cd %WXWIN%\samples\validate
call  %WXWIN%\distrib\msw\process_sample_bcb validate
cd %WXWIN%\samples\wizard
call  %WXWIN%\distrib\msw\process_sample_bcb wizard
cd %WXWIN%\samples\

echo Processing ended

goto end 

:nowxdir
echo Please use the MSDOS command SET WXWIN=DRV:PATH_TO_WX before trying this batch file
echo e.g. SET WXWIN=c:\wx
goto end

:nocommand
echo There appears to be no command to modify files in %WXWIN%\distrib\msw\
echo There sould be a file process_sample_bcb.bat  
echo in that directory; please reinstall wxWindows
goto end


:end
