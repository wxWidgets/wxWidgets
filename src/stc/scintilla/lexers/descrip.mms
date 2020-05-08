#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 4 March 2020                                                        *
#                                                                            *
#*****************************************************************************

.first
	define wx [----.include.wx]

.suffixes : .cpp

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WX__=1,__WXMOTIF__=1,__USE_STD_IOSTREAM=1,WXBUILDING=1)/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)\
	   /include=([],[-.src],[-.include],[-.lexlib])\
	   /warn=disable=(INTSIGNCHANGE)
CC_DEFINE = /define=(__WX__=1,__WXMOTIF__=1,WXBUILDING=1)/name=(as_is,short)\
	/include=([],[-.src],[-.include],[-.lexlib])
.else
.ifdef __WXGTK__
CXX_DEFINE = /define=(__WX__=1,__WXGTK__=1,__USE_STD_IOSTREAM=1,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)\
	   /include=([],[-.src],[-.include],[-.lexlib])\
	   /warn=disable=(INTSIGNCHANGE)
CC_DEFINE = /define=(__WX__=1,__WXGTK__=1,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	/include=([],[-.src],[-.include],[-.lexlib])
.else
.ifdef __WXGTK2__
CXX_DEFINE = /define=(__WX__=1,__WXGTK__=1,VMS_GTK2=1,__USE_STD_IOSTREAM=1,WXBUILDING=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)\
	   /include=([],[-.src],[-.include],[-.lexlib])\
	   /warn=disable=(INTSIGNCHANGE)
CC_DEFINE = /define=(__WX__=1,__WXGTK__=1,VMS_GTK2=1,WXBUILDING=1)/float=ieee/name=(as_is,short)\
	/ieee=denorm/include=([],[-.src],[-.include],[-.lexlib])
.else
.ifdef __WXX11__
CXX_DEFINE = /define=(__WX__=1,__WXX11__=1,__WXUNIVERSAL__==1,__USE_STD_IOSTREAM=1,WXBUILDING=1)/float=ieee\
	/name=(as_is,short)/assume=(nostdnew,noglobal_array_new)\
	/include=([],[-.src],[-.include],[-.lexlib])\
	/warn=disable=(INTSIGNCHANGE)
CC_DEFINE = /define=(__WX__=1,__WXX11__=1,__WXUNIVERSAL__==1,WXBUILDING=1)/float=ieee\
	/name=(as_is,short)/include=([],[-.src],[-.include],[-.lexlib])
.else
CXX_DEFINE =
CC_DEFINE =
.endif
.endif
.endif
.endif

.cxx.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cxx
.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp
.c.obj :
	cc $(CFLAGS)$(CC_DEFINE) $(MMS$TARGET_NAME).c

OBJECTS=LexA68k.obj,LexAbaqus.obj,LexAda.obj,LexAPDL.obj,LexAsm.obj,\
	LexAsn1.obj,LexASY.obj,LexAU3.obj,LexAVE.obj,LexAVS.obj,LexBaan.obj,\
	LexBash.obj,LexBasic.obj,LexBullant.obj,LexCaml.obj,LexCLW.obj,\
	LexCmake.obj,LexCOBOL.obj,LexCoffeeScript.obj,LexConf.obj,\
	LexCPP.obj,LexCrontab.obj,LexCsound.obj,LexCSS.obj,LexD.obj,\
	LexECL.obj,LexEiffel.obj,LexErlang.obj,LexEScript.obj,LexFlagship.obj,\
	LexForth.obj,LexFortran.obj,LexGAP.obj,LexGui4Cli.obj,LexHaskell.obj,\
	LexHTML.obj,LexInno.obj,LexKix.obj,LexKVIrc.obj,LexLaTeX.obj,LexLisp.obj,\
	LexLout.obj,LexLua.obj,LexMagik.obj,LexMarkdown.obj,LexMatlab.obj,\
        LexMetapost.obj

OBJECTS1=LexMMIXAL.obj,LexModula.obj,LexMPT.obj,LexMSSQL.obj,LexMySQL.obj,\
	LexNimrod.obj,LexNsis.obj,LexOpal.obj,LexOScript.obj,\
	LexPascal.obj,LexPB.obj,LexPerl.obj,LexPLM.obj,LexPO.obj,LexPOV.obj,\
	LexPowerPro.obj,LexPowerShell.obj,LexProgress.obj,LexPS.obj,\
	LexPython.obj,LexR.obj,LexRebol.obj,LexRuby.obj,LexRust.obj,\
	LexScriptol.obj,LexSmalltalk.obj,LexSML.obj,LexSorcus.obj,LexSpecman.obj,\
	LexSpice.obj,LexSQL.obj,LexSTTXT.obj,LexTACL.obj,LexTADS3.obj,LexTAL.obj,\
	LexTCL.obj,LexTCMD.obj,LexTeX.obj,LexTxt2tags.obj,LexVB.obj,\
	LexVerilog.obj,LexVHDL.obj,LexVisualProlog.obj,LexYAML.obj,\
	LexDMAP.obj,LexHex.obj,LexBibTeX.obj,LexDMIS.obj,LexRegistry.obj,\
	LexBatch.obj,LexDiff.obj,LexErrorList.obj,LexMake.obj,LexNull.obj,\
	LexProps.obj,LexJSON.obj,LexEDIFACT.obj

SOURCES=LexA68k.cxx,LexAbaqus.cxx,LexAda.cxx,LexAPDL.cxx,LexAsm.cxx,\
	LexAsn1.cxx,LexASY.cxx,LexAU3.cxx,LexAVE.cxx,LexAVS.cxx,LexBaan.cxx,\
	LexBash.cxx,LexBasic.cxx,LexBullant.cxx,LexCaml.cxx,LexCLW.cxx,\
	LexCmake.cxx,LexCOBOL.cxx,LexCoffeeScript.cxx,LexConf.cxx,\
	LexCPP.cxx,LexCrontab.cxx,LexCsound.cxx,LexCSS.cxx,LexD.cxx,LexDMAP.cxx,\
	LexECL.cxx,LexEiffel.cxx,LexErlang.cxx,LexEScript.cxx,LexFlagship.cxx,\
	LexForth.cxx,LexFortran.cxx,LexGAP.cxx,LexGui4Cli.cxx,LexHaskell.cxx,\
	LexHTML.cxx,LexInno.cxx,LexKix.cxx,LexKVIrc.cxx,LexLaTeX.cxx,LexLisp.cxx,\
	LexLout.cxx,LexLua.cxx,LexMagik.cxx,LexMarkdown.cxx,LexMatlab.cxx,\
        LexMetapost.cxx,\
	LexMMIXAL.cxx,LexModula.cxx,LexMPT.cxx,LexMSSQL.cxx,LexMySQL.cxx,\
	LexNimrod.cxx,LexNsis.cxx,LexOpal.cxx,LexOScript.cxx,\
	LexPascal.cxx,LexPB.cxx,LexPerl.cxx,LexPLM.cxx,LexPO.cxx,LexPOV.cxx,\
	LexPowerPro.cxx,LexPowerShell.cxx,LexProgress.cxx,LexPS.cxx,\
	LexPython.cxx,LexR.cxx,LexRebol.cxx,LexRuby.cxx,LexRust.cxx,\
	LexScriptol.cxx,LexSmalltalk.cxx,LexSML.cxx,LexSorcus.cxx,LexSpecman.cxx,\
	LexSpice.cxx,LexSQL.cxx,LexSTTXT.cxx,LexTACL.cxx,LexTADS3.cxx,LexTAL.cxx,\
	LexTCL.cxx,LexTCMD.cxx,LexTeX.cxx,LexTxt2tags.cxx,LexVB.cxx,\
	LexVerilog.cxx,LexVHDL.cxx,LexVisualProlog.cxx,LexYAML.cxx,\
	LexDMAP.cxx LexHex.cxx LexBibTeX.cxx LexDMIS.cxx LexRegistry.cxx \
	LexBatch.cxx LexDiff.cxx LexErrorList.cxx LexMake.cxx LexNull.cxx \
	LexProps.cxx LexJSON.cxx LexEDIFACT.cxx

all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS1)
.ifdef __WXMOTIF__
	library [----.lib]libwx_motif.olb $(OBJECTS)
	library [----.lib]libwx_motif.olb $(OBJECTS1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [----.lib]libwx_motif.olb [.CXX_REPOSITORY]*.obj
.else
.ifdef __WXGTK__
	library [----.lib]libwx_gtk.olb $(OBJECTS)
	library [----.lib]libwx_gtk.olb $(OBJECTS1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [----.lib]libwx_gtk.olb [.CXX_REPOSITORY]*.obj
.else
.ifdef __WXGTK2__
	library [----.lib]libwx_gtk2.olb $(OBJECTS)
	library [----.lib]libwx_gtk2.olb $(OBJECTS1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [----.lib]libwx_gtk2.olb [.CXX_REPOSITORY]*.obj
.else
.ifdef __WXX11__
	library [----.lib]libwx_x11_univ.olb $(OBJECTS)
	library [----.lib]libwx_x11_univ.olb $(OBJECTS1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [----.lib]libwx_x11_univ.olb [.CXX_REPOSITORY]*.obj
.endif
.endif
.endif
.endif

$(OBJECTS) : [----.include.wx]setup.h
$(OBJECTS1) : [----.include.wx]setup.h

LexA68k.obj : LexA68k.cxx
LexAbaqus.obj : LexAbaqus.cxx
LexAda.obj : LexAda.cxx
LexAPDL.obj : LexAPDL.cxx
LexAsm.obj : LexAsm.cxx
LexAsn1.obj : LexAsn1.cxx
LexASY.obj : LexASY.cxx
LexAU3.obj : LexAU3.cxx
LexAVE.obj : LexAVE.cxx
LexAVS.obj : LexAVS.cxx
LexBaan.obj : LexBaan.cxx
LexBash.obj : LexBash.cxx
LexBasic.obj : LexBasic.cxx
LexBullant.obj : LexBullant.cxx
LexCaml.obj : LexCaml.cxx
LexCLW.obj : LexCLW.cxx
LexCmake.obj : LexCmake.cxx
LexCOBOL.obj : LexCOBOL.cxx
LexCoffeeScript.obj : LexCoffeeScript.cxx
LexConf.obj : LexConf.cxx
LexCPP.obj : LexCPP.cxx
LexCrontab.obj : LexCrontab.cxx
LexCsound.obj : LexCsound.cxx
LexCSS.obj : LexCSS.cxx
LexD.obj : LexD.cxx
LexDMAP.obj : LexDMAP.cxx
LexECL.obj : LexECL.cxx
LexEiffel.obj : LexEiffel.cxx
LexErlang.obj : LexErlang.cxx
LexEScript.obj : LexEScript.cxx
LexFlagship.obj : LexFlagship.cxx
LexForth.obj : LexForth.cxx
LexFortran.obj : LexFortran.cxx
LexGAP.obj : LexGAP.cxx
LexGui4Cli.obj : LexGui4Cli.cxx
LexHaskell.obj : LexHaskell.cxx
LexHTML.obj : LexHTML.cxx
LexInno.obj : LexInno.cxx
LexKix.obj : LexKix.cxx
LexKVIrc.obj : LexKVIrc.cxx
LexLaTeX.obj : LexLaTeX.cxx
LexLisp.obj : LexLisp.cxx
LexLout.obj : LexLout.cxx
LexLua.obj : LexLua.cxx
LexMagik.obj : LexMagik.cxx
LexMarkdown.obj : LexMarkdown.cxx
LexMatlab.obj : LexMatlab.cxx
LexMetapost.obj : LexMetapost.cxx
LexMMIXAL.obj : LexMMIXAL.cxx
LexModula.obj : LexModula.cxx
LexMPT.obj : LexMPT.cxx
LexMSSQL.obj : LexMSSQL.cxx
LexMySQL.obj : LexMySQL.cxx
LexNimrod.obj : LexNimrod.cxx
LexNsis.obj : LexNsis.cxx
LexOpal.obj : LexOpal.cxx
LexOScript.obj : LexOScript.cxx
LexPascal.obj : LexPascal.cxx
LexPB.obj : LexPB.cxx
LexPerl.obj : LexPerl.cxx
LexPLM.obj : LexPLM.cxx
LexPO.obj : LexPO.cxx
LexPOV.obj : LexPOV.cxx
LexPowerPro.obj : LexPowerPro.cxx
LexPowerShell.obj : LexPowerShell.cxx
LexProgress.obj : LexProgress.cxx
LexPS.obj : LexPS.cxx
LexPython.obj : LexPython.cxx
LexR.obj : LexR.cxx
LexRebol.obj : LexRebol.cxx
LexRuby.obj : LexRuby.cxx
LexRust.obj : LexRust.cxx
LexScriptol.obj : LexScriptol.cxx
LexSmalltalk.obj : LexSmalltalk.cxx
LexSML.obj : LexSML.cxx
LexSorcus.obj : LexSorcus.cxx
LexSpecman.obj : LexSpecman.cxx
LexSpice.obj : LexSpice.cxx
LexSQL.obj : LexSQL.cxx
LexSTTXT.obj : LexSTTXT.cxx
LexTACL.obj : LexTACL.cxx
LexTADS3.obj : LexTADS3.cxx
LexTAL.obj : LexTAL.cxx
LexTCL.obj : LexTCL.cxx
LexTCMD.obj : LexTCMD.cxx
LexTeX.obj : LexTeX.cxx
LexTxt2tags.obj : LexTxt2tags.cxx
LexVB.obj : LexVB.cxx
LexVerilog.obj : LexVerilog.cxx
LexVHDL.obj : LexVHDL.cxx
LexVisualProlog.obj : LexVisualProlog.cxx
LexYAML.obj : LexYAML.cxx
LexDMAP.obj : LexDMAP.cxx
LexHex.obj : LexHex.cxx
LexBibTeX.obj : LexBibTeX.cxx
LexDMIS.obj : LexDMIS.cxx
LexRegistry.obj : LexRegistry.cxx
LexBatch.obj : LexBatch.cxx
LexDiff.obj : LexDiff.cxx
LexErrorList.obj : LexErrorList.cxx
LexMake.obj : LexMake.cxx
LexNull.obj : LexNull.cxx
LexProps.obj : LexProps.cxx
LexJSON.obj : LexJSON.cxx
LexEDIFACT.obj : LexEDIFACT.cxx
