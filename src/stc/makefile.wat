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
    ExternalLexer.obj		&
    Indicator.obj	        &
    KeyMap.obj		        &
    KeyWords.obj	        &
	LexAVE.obj		        &
	LexAda.obj		        &
	LexAsm.obj		        &
	LexBaan.obj		        &
	LexBullant.obj		    &
	LexMatlab.obj	    	&
	LexCPP.obj		        &
	LexConf.obj		        &
	LexCrontab.obj		    &
	LexCSS.obj		    &
	LexEiffel.obj		    &
	LexEScript.obj		    &
	LexFortran.obj  	    &
	LexHTML.obj		        &
	LexLisp.obj		        &
	LexLout.obj			&
	LexLua.obj		        &
	LexOthers.obj		    &
	LexPOV.obj		    &
	LexPascal.obj		    &
	LexPerl.obj		        &
	LexPython.obj		    &
	LexRuby.obj		        &
	LexSQL.obj		        &
	LexVB.obj		        &
        LexMMIXAL.obj		        &
        LexNsis.obj		        &
        LexPS.obj		        &
        LexScriptol.obj		        &
    LineMarker.obj	    	&
    PropSet.obj	            &
	RESearch.obj		    &
    ScintillaBase.obj	    &
    Style.obj		        &
	StyleContext.obj	    &
	UniConversion.obj	    &
    ViewStyle.obj		    &
	WindowAccessor.obj	    &
	XPM.obj			    &
	PlatWX.obj              &
	ScintillaWX.obj         &
	stc.obj

all: $(STCLIB) .SYMBOLIC

$(STCLIB): $(OBJECTS)
	*wlib /b /c /n /P=256 $(STCLIB) $(OBJECTS)

clean:   .SYMBOLIC
    -erase *.obj
    -erase *.bak
    -erase *.err
    -erase *.pch
    -erase $(STCLIB)
    -erase *.lbc

.EXTENSIONS: .cxx
.cxx: $(S)

.cxx.obj:
    $(CXX) $[*.cxx $(CXXFLAGS) $(STCEXTRACPPFLAGS)


