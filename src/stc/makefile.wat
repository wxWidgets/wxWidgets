# STC makefile for Watcom C++

WXDIR = ..\..\..

SCINTILLA=$(WXDIR)\contrib\src\stc\scintilla
S=$(SCINTILLA)\src
STCEXTRACPPFLAGS=-D__WX__ -DSCI_LEXER -DLINK_LEXERS -I$(SCINTILLA)\include -I$(S)

EXTRACPPFLAGS = -I$(WXDIR)\contrib\include $(STCEXTRACPPFLAGS)

!include $(WXDIR)\src\makewat.env

STCLIB = $(WXDIR)\lib\stc_w.lib
THISDIR = $(WXDIR)\contrib\src\stc
OUTPUTDIR = $(THISDIR)\


NAME = stc
LNK = $(name).lnk

OBJECTS = &
    AutoComplete.obj	    &
    CallTip.obj	            &
    CellBuffer.obj	    	&
    ContractionState.obj  	&
    Document.obj	        &
	DocumentAccessor.obj	&
    Editor.obj		        &
    Indicator.obj	        &
    KeyMap.obj		        &
    KeyWords.obj	        &
	LexAVE.obj		        &
	LexAda.obj		        &
	LexBaan.obj		        &
	LexBullant.obj		    &
	LexMatlab.obj	    	&
	LexCPP.obj		        &
	LexConf.obj		        &
	LexCrontab.obj		    &
	LexEiffel.obj		    &
	LexHTML.obj		        &
	LexLisp.obj		        &
	LexLua.obj		        &
	LexOthers.obj		    &
	LexPascal.obj		    &
	LexPerl.obj		        &
	LexPython.obj		    &
	LexRuby.obj		        &
	LexSQL.obj		        &
	LexVB.obj		        &
    LineMarker.obj	    	&
    PropSet.obj	            &
	RESearch.obj		    &
    ScintillaBase.obj	    &
    Style.obj		        &
	StyleContext.obj	    &
	UniConversion.obj	    &
    ViewStyle.obj		    &
	WindowAccessor.obj	    &
	PlatWX.obj              &
	ScintillaWX.obj         &
	stc.obj     

all: $(STCLIB) .SYMBOLIC

$(STCLIB): $(OBJECTS)
	*wlib /b /c /n /P=256 $(STCLIB) $(OBJECTS)

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch $(STCLIB) *.lbc

.EXTENSIONS: .cxx
.cxx: $(S)

.cxx.obj:
    $(CXX) $[*.cxx $(CXXFLAGS) $(STCEXTRACPPFLAGS) 


