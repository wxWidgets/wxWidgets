WXDIR = ..\..\..

SCINTILLA=.\scintilla
S=$(SCINTILLA)\src

EXTRASTCCPPFLAGS = -D__WX__ -DSCI_LEXER -DLINK_LEXERS -I$(SCINTILLA)/include -I$(S)
EXTRACPPFLAGS=$(EXTRACPPFLAGS) $(EXTRASTCCPPFLAGS)
STCCPPFLAGS=$(DLL_FLAGS) $(EXTRACPPFLAGS)

include $(WXDIR)\src\makesc.env
LIBTARGET=$(WXDIR)\lib\stc$(SC_SUFFIX).lib
THISDIR=$(WXDIR)\contrib\src\stc




# variables
OBJECTS = \
    $(S)\AutoComplete.obj		\
    $(S)\CallTip.obj	        	\
    $(S)\CellBuffer.obj	    	\
    $(S)\ContractionState.obj  	\
    $(S)\Document.obj	        \
	$(S)\DocumentAccessor.obj	\
    $(S)\Editor.obj			\
    $(S)\ExternalLexer.obj   \
    $(S)\Indicator.obj       \
    $(S)\KeyMap.obj			\
    $(S)\KeyWords.obj	    \
	$(S)\LexAVE.obj			\
	$(S)\LexAda.obj			\
	$(S)\LexAsm.obj			\
	$(S)\LexBaan.obj			\
	$(S)\LexBullant.obj		\
	$(S)\LexMatlab.obj		\
	$(S)\LexCPP.obj			\
	$(S)\LexConf.obj			\
	$(S)\LexCrontab.obj		\
	$(S)\LexCSS.obj			\
	$(S)\LexEiffel.obj		\
	$(S)\LexEScript.obj  \
	$(S)\LexFortran.obj		\
	$(S)\LexHTML.obj			\
	$(S)\LexLisp.obj			\
	$(S)\LexLua.obj			\
	$(S)\LexLout.obj        \
	$(S)\LexOthers.obj		\
	$(S)\LexPOV.obj			\
	$(S)\LexPascal.obj		\
	$(S)\LexPerl.obj			\
	$(S)\LexPython.obj		\
	$(S)\LexRuby.obj			\
	$(S)\LexSQL.obj			\
	$(S)\LexVB.obj			\
        $(S)\LexMMIXAL.obj			\
        $(S)\LexNsis.obj			\
        $(S)\LexPS.obj			\
        $(S)\LexScriptol.obj			\
    $(S)\LineMarker.obj	    \
    $(S)\PropSet.obj	        \
	$(S)\RESearch.obj		\
    $(S)\ScintillaBase.obj	\
    $(S)\Style.obj			\
	$(S)\StyleContext.obj	\
	$(S)\UniConversion.obj	\
    $(S)\ViewStyle.obj		\
	$(S)\WindowAccessor.obj	\
	$(S)\XPM.obj  			\
                            		\
	PlatWX.obj          \
	ScintillaWX.obj     \
	stc.obj             \


include $(WXDIR)\src\makelib.sc


.cxx.obj:
	*$(CC) -c $(CFLAGS) $(STCCPPFLAGS) $(INCLUDE) $(OPTIONS) $< -o$@


