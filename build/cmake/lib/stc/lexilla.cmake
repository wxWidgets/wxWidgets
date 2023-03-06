#############################################################################
# Name:        build/cmake/lib/stc/lexilla.cmake
# Purpose:     CMake file for Lexilla library
# Author:      Maarten Bent
# Created:     2022-12-28
# Copyright:   (c) 2022 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)
set(LEXILLA_SRC_DIR src/stc/lexilla)

wx_add_builtin_library(wxlexilla
    ${LEXILLA_SRC_DIR}/access/LexillaAccess.cxx
    ${LEXILLA_SRC_DIR}/access/LexillaAccess.h
    ${LEXILLA_SRC_DIR}/include/Lexilla.h
    ${LEXILLA_SRC_DIR}/include/SciLexer.h
    ${LEXILLA_SRC_DIR}/lexers/LexA68k.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexAPDL.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexASY.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexAU3.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexAVE.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexAVS.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexAbaqus.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexAda.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexAsm.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexAsn1.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexBaan.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexBash.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexBasic.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexBatch.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexBibTeX.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexBullant.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexCIL.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexCLW.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexCOBOL.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexCPP.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexCSS.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexCaml.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexCmake.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexCoffeeScript.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexConf.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexCrontab.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexCsound.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexD.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexDMAP.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexDMIS.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexDataflex.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexDiff.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexECL.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexEDIFACT.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexEScript.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexEiffel.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexErlang.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexErrorList.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexFSharp.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexFlagship.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexForth.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexFortran.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexGAP.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexGui4Cli.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexHTML.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexHaskell.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexHex.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexHollywood.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexIndent.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexInno.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexJSON.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexKVIrc.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexKix.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexLaTeX.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexLisp.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexLout.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexLua.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexMMIXAL.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexMPT.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexMSSQL.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexMagik.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexMake.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexMarkdown.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexMatlab.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexMaxima.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexMetapost.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexModula.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexMySQL.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexNim.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexNimrod.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexNsis.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexNull.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexOScript.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexOpal.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexPB.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexPLM.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexPO.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexPOV.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexPS.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexPascal.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexPerl.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexPowerPro.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexPowerShell.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexProgress.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexProps.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexPython.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexR.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexRaku.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexRebol.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexRegistry.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexRuby.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexRust.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexSAS.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexSML.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexSQL.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexSTTXT.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexScriptol.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexSmalltalk.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexSorcus.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexSpecman.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexSpice.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexStata.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexTACL.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexTADS3.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexTAL.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexTCL.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexTCMD.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexTeX.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexTxt2tags.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexVB.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexVHDL.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexVerilog.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexVisualProlog.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexX12.cxx
    ${LEXILLA_SRC_DIR}/lexers/LexYAML.cxx
    ${LEXILLA_SRC_DIR}/lexlib/Accessor.cxx
    ${LEXILLA_SRC_DIR}/lexlib/Accessor.h
    ${LEXILLA_SRC_DIR}/lexlib/CatalogueModules.h
    # ${LEXILLA_SRC_DIR}/lexlib/CharacterCategory.cxx
    # ${LEXILLA_SRC_DIR}/lexlib/CharacterCategory.h
    # ${LEXILLA_SRC_DIR}/lexlib/CharacterSet.cxx
    # ${LEXILLA_SRC_DIR}/lexlib/CharacterSet.h
    ${LEXILLA_SRC_DIR}/lexlib/DefaultLexer.cxx
    ${LEXILLA_SRC_DIR}/lexlib/DefaultLexer.h
    ${LEXILLA_SRC_DIR}/lexlib/LexAccessor.h
    ${LEXILLA_SRC_DIR}/lexlib/LexerBase.cxx
    ${LEXILLA_SRC_DIR}/lexlib/LexerBase.h
    ${LEXILLA_SRC_DIR}/lexlib/LexerModule.cxx
    ${LEXILLA_SRC_DIR}/lexlib/LexerModule.h
    ${LEXILLA_SRC_DIR}/lexlib/LexerNoExceptions.cxx
    ${LEXILLA_SRC_DIR}/lexlib/LexerNoExceptions.h
    ${LEXILLA_SRC_DIR}/lexlib/LexerSimple.cxx
    ${LEXILLA_SRC_DIR}/lexlib/LexerSimple.h
    ${LEXILLA_SRC_DIR}/lexlib/OptionSet.h
    ${LEXILLA_SRC_DIR}/lexlib/PropSetSimple.cxx
    ${LEXILLA_SRC_DIR}/lexlib/PropSetSimple.h
    ${LEXILLA_SRC_DIR}/lexlib/SparseState.h
    ${LEXILLA_SRC_DIR}/lexlib/StringCopy.h
    ${LEXILLA_SRC_DIR}/lexlib/StyleContext.cxx
    ${LEXILLA_SRC_DIR}/lexlib/StyleContext.h
    ${LEXILLA_SRC_DIR}/lexlib/SubStyles.h
    ${LEXILLA_SRC_DIR}/lexlib/WordList.cxx
    ${LEXILLA_SRC_DIR}/lexlib/WordList.h
    ${LEXILLA_SRC_DIR}/src/Lexilla.cxx
)

get_target_property(SCINTILLA_INCLUDE wxscintilla INCLUDE_DIRECTORIES)

target_include_directories(wxlexilla PRIVATE
    ${wxSOURCE_DIR}/${LEXILLA_SRC_DIR}/access
    ${wxSOURCE_DIR}/${LEXILLA_SRC_DIR}/include
    ${wxSOURCE_DIR}/${LEXILLA_SRC_DIR}/lexlib
    ${SCINTILLA_INCLUDE}
)


wx_target_enable_precomp(wxlexilla
    "${wxSOURCE_DIR}/${SCINTILLA_SRC_DIR}/include/Scintilla.h"
    "${wxSOURCE_DIR}/${LEXILLA_SRC_DIR}/include/Lexilla.h"
)
