#makefile for Digital Mars / Symantec compilers

WXDIR=..

all:
    cd access
    -make -f $(WXDIR)\samples\access\makefile.sc
    -make -f $(WXDIR)\samples\artprov\makefile.sc 
    -make -f $(WXDIR)\samples\calendar\makefile.sc $@
    -make -f $(WXDIR)\samples\caret\makefile.sc $@
    -make -f $(WXDIR)\samples\checklst\makefile.sc $@
    -make -f $(WXDIR)\samples\config\makefile.sc $@
    -make -f $(WXDIR)\samples\console\makefile.sc $@
    -make -f $(WXDIR)\samples\controls\makefile.sc $@
    -make -f $(WXDIR)\samples\db\makefile.sc $@
    -make -f $(WXDIR)\samples\dialogs\makefile.sc $@
    -make -f $(WXDIR)\samples\dialup\makefile.sc $@
    -make -f $(WXDIR)\samples\display\makefile.sc $@
    -make -f $(WXDIR)\samples\dnd\makefile.sc $@
    -make -f $(WXDIR)\samples\docview\makefile.sc $@
    -make -f $(WXDIR)\samples\docvwmdi\makefile.sc $@
    -make -f $(WXDIR)\samples\dragimag\makefile.sc $@
    -make -f $(WXDIR)\samples\drawing\makefile.sc $@
    -make -f $(WXDIR)\samples\dynamic\makefile.sc $@
    -make -f $(WXDIR)\samples\erase\makefile.sc $@
    -make -f $(WXDIR)\samples\event\makefile.sc $@
    -make -f $(WXDIR)\samples\exec\makefile.sc $@
    -make -f $(WXDIR)\samples\font\makefile.sc $@
    -make -f $(WXDIR)\samples\grid\makefile.sc $@
    -make -f $(WXDIR)\samples\help\makefile.sc $@
    -make -f $(WXDIR)\samples\htlbox\makefile.sc $@
    -make -f $(WXDIR)\samples\html\makefile.sc $@
    -make -f $(WXDIR)\samples\image\makefile.sc $@
    -make -f $(WXDIR)\samples\internat\makefile.sc $@
    -make -f $(WXDIR)\samples\ipc\makefile.sc $@
    -make -f $(WXDIR)\samples\joytest\makefile.sc $@
    -make -f $(WXDIR)\samples\keyboard\makefile.sc $@
    -make -f $(WXDIR)\samples\layout\makefile.sc $@
    -make -f $(WXDIR)\samples\listbox\makefile.sc $@
    -make -f $(WXDIR)\samples\listctrl\makefile.sc $@
    -make -f $(WXDIR)\samples\mdi\makefile.sc $@
    -make -f $(WXDIR)\samples\memcheck\makefile.sc $@
    -make -f $(WXDIR)\samples\menu\makefile.sc $@
    -make -f $(WXDIR)\samples\mfc\makefile.sc $@
    -make -f $(WXDIR)\samples\minifram\makefile.sc $@
    -make -f $(WXDIR)\samples\minimal\makefile.sc $@
    -make -f $(WXDIR)\samples\mobile\makefile.sc $@
    -make -f $(WXDIR)\samples\multimon\makefile.sc $@
    -make -f $(WXDIR)\samples\nativdlg\makefile.sc $@
    -make -f $(WXDIR)\samples\newgrid\makefile.sc $@
    -make -f $(WXDIR)\samples\notebook\makefile.sc $@
    -make -f $(WXDIR)\samples\oleauto\makefile.sc $@
    -make -f $(WXDIR)\samples\opengl\makefile.sc $@
    -make -f $(WXDIR)\samples\ownerdrw\makefile.sc $@
    -make -f $(WXDIR)\samples\png\makefile.sc $@
    -make -f $(WXDIR)\samples\printing\makefile.sc $@
    -make -f $(WXDIR)\samples\propsize\makefile.sc $@
    -make -f $(WXDIR)\samples\regtest\makefile.sc $@
    -make -f $(WXDIR)\samples\resource\makefile.sc $@
    -make -f $(WXDIR)\samples\richedit\makefile.sc $@
    -make -f $(WXDIR)\samples\rotate\makefile.sc $@
    -make -f $(WXDIR)\samples\sashtest\makefile.sc $@
    -make -f $(WXDIR)\samples\scroll\makefile.sc $@
    -make -f $(WXDIR)\samples\scrollsub\makefile.sc $@
    -make -f $(WXDIR)\samples\shaped\makefile.sc $@
    -make -f $(WXDIR)\samples\sockets\makefile.sc $@
    -make -f $(WXDIR)\samples\splitter\makefile.sc $@
    -make -f $(WXDIR)\samples\statbar\makefile.sc $@
    -make -f $(WXDIR)\samples\tab\makefile.sc $@
    -make -f $(WXDIR)\samples\taskbar\makefile.sc $@
    -make -f $(WXDIR)\samples\text\makefile.sc $@
    -make -f $(WXDIR)\samples\thread\makefile.sc $@
    -make -f $(WXDIR)\samples\toolbar\makefile.sc $@
    -make -f $(WXDIR)\samples\treectrl\makefile.sc $@
    -make -f $(WXDIR)\samples\typetest\makefile.sc $@
    -make -f $(WXDIR)\samples\validate\makefile.sc $@
    -make -f $(WXDIR)\samples\vscroll\makefile.sc $@
    -make -f $(WXDIR)\samples\widgets\makefile.sc $@
    -make -f $(WXDIR)\samples\wizard\makefile.sc $@
    -make -f $(WXDIR)\samples\bytes\makefile.sc $@



