#############################################################################
# Name:        build/cmake/lib/stc/scintilla.cmake
# Purpose:     CMake file for Scintilla library
# Author:      Maarten Bent
# Created:     2022-12-28
# Copyright:   (c) 2022 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)
set(SCINTILLA_SRC_DIR src/stc/scintilla)

set(SCINTILLA_DEFINES)

set(SCINTILLA_SOURCES
    ${SCINTILLA_SRC_DIR}/include/ILexer.h
    ${SCINTILLA_SRC_DIR}/include/ILoader.h
    ${SCINTILLA_SRC_DIR}/include/Platform.h
    ${SCINTILLA_SRC_DIR}/include/Sci_Position.h
    ${SCINTILLA_SRC_DIR}/include/Scintilla.h
    ${SCINTILLA_SRC_DIR}/include/ScintillaWidget.h
    ${SCINTILLA_SRC_DIR}/src/AutoComplete.cxx
    ${SCINTILLA_SRC_DIR}/src/AutoComplete.h
    ${SCINTILLA_SRC_DIR}/src/CallTip.cxx
    ${SCINTILLA_SRC_DIR}/src/CallTip.h
    ${SCINTILLA_SRC_DIR}/src/CaseConvert.cxx
    ${SCINTILLA_SRC_DIR}/src/CaseConvert.h
    ${SCINTILLA_SRC_DIR}/src/CaseFolder.cxx
    ${SCINTILLA_SRC_DIR}/src/CaseFolder.h
    ${SCINTILLA_SRC_DIR}/src/CellBuffer.cxx
    ${SCINTILLA_SRC_DIR}/src/CellBuffer.h
    ${SCINTILLA_SRC_DIR}/src/CharClassify.cxx
    ${SCINTILLA_SRC_DIR}/src/CharClassify.h
    ${SCINTILLA_SRC_DIR}/src/CharacterCategory.cxx
    ${SCINTILLA_SRC_DIR}/src/CharacterCategory.h
    ${SCINTILLA_SRC_DIR}/src/CharacterSet.cxx
    ${SCINTILLA_SRC_DIR}/src/CharacterSet.h
    ${SCINTILLA_SRC_DIR}/src/ContractionState.cxx
    ${SCINTILLA_SRC_DIR}/src/ContractionState.h
    ${SCINTILLA_SRC_DIR}/src/DBCS.cxx
    ${SCINTILLA_SRC_DIR}/src/DBCS.h
    ${SCINTILLA_SRC_DIR}/src/Decoration.cxx
    ${SCINTILLA_SRC_DIR}/src/Decoration.h
    ${SCINTILLA_SRC_DIR}/src/Document.cxx
    ${SCINTILLA_SRC_DIR}/src/Document.h
    ${SCINTILLA_SRC_DIR}/src/EditModel.cxx
    ${SCINTILLA_SRC_DIR}/src/EditModel.h
    ${SCINTILLA_SRC_DIR}/src/EditView.cxx
    ${SCINTILLA_SRC_DIR}/src/EditView.h
    ${SCINTILLA_SRC_DIR}/src/Editor.cxx
    ${SCINTILLA_SRC_DIR}/src/Editor.h
    ${SCINTILLA_SRC_DIR}/src/ElapsedPeriod.h
    ${SCINTILLA_SRC_DIR}/src/FontQuality.h
    ${SCINTILLA_SRC_DIR}/src/Indicator.cxx
    ${SCINTILLA_SRC_DIR}/src/Indicator.h
    ${SCINTILLA_SRC_DIR}/src/IntegerRectangle.h
    ${SCINTILLA_SRC_DIR}/src/KeyMap.cxx
    ${SCINTILLA_SRC_DIR}/src/KeyMap.h
    ${SCINTILLA_SRC_DIR}/src/LineMarker.cxx
    ${SCINTILLA_SRC_DIR}/src/LineMarker.h
    ${SCINTILLA_SRC_DIR}/src/MarginView.cxx
    ${SCINTILLA_SRC_DIR}/src/MarginView.h
    ${SCINTILLA_SRC_DIR}/src/Partitioning.h
    ${SCINTILLA_SRC_DIR}/src/PerLine.cxx
    ${SCINTILLA_SRC_DIR}/src/PerLine.h
    ${SCINTILLA_SRC_DIR}/src/Position.h
    ${SCINTILLA_SRC_DIR}/src/PositionCache.cxx
    ${SCINTILLA_SRC_DIR}/src/PositionCache.h
    ${SCINTILLA_SRC_DIR}/src/RESearch.cxx
    ${SCINTILLA_SRC_DIR}/src/RESearch.h
    ${SCINTILLA_SRC_DIR}/src/RunStyles.cxx
    ${SCINTILLA_SRC_DIR}/src/RunStyles.h
    ${SCINTILLA_SRC_DIR}/src/ScintillaBase.cxx
    ${SCINTILLA_SRC_DIR}/src/ScintillaBase.h
    ${SCINTILLA_SRC_DIR}/src/Selection.cxx
    ${SCINTILLA_SRC_DIR}/src/Selection.h
    ${SCINTILLA_SRC_DIR}/src/SparseVector.h
    ${SCINTILLA_SRC_DIR}/src/SplitVector.h
    ${SCINTILLA_SRC_DIR}/src/Style.cxx
    ${SCINTILLA_SRC_DIR}/src/Style.h
    ${SCINTILLA_SRC_DIR}/src/UniConversion.cxx
    ${SCINTILLA_SRC_DIR}/src/UniConversion.h
    ${SCINTILLA_SRC_DIR}/src/UniqueString.cxx
    ${SCINTILLA_SRC_DIR}/src/UniqueString.h
    ${SCINTILLA_SRC_DIR}/src/ViewStyle.cxx
    ${SCINTILLA_SRC_DIR}/src/ViewStyle.h
    ${SCINTILLA_SRC_DIR}/src/XPM.cxx
    ${SCINTILLA_SRC_DIR}/src/XPM.h
)
if(WXMSW)
    set(SCINTILLA_SOURCES
        ${SCINTILLA_SOURCES}
        ${SCINTILLA_SRC_DIR}/win32/HanjaDic.cxx
        ${SCINTILLA_SRC_DIR}/win32/HanjaDic.h
        ${SCINTILLA_SRC_DIR}/win32/PlatWin.cxx
        ${SCINTILLA_SRC_DIR}/win32/PlatWin.h
        ${SCINTILLA_SRC_DIR}/win32/ScintillaDLL.cxx
        ${SCINTILLA_SRC_DIR}/win32/ScintillaWin.cxx
        ${SCINTILLA_SRC_DIR}/win32/ScintillaWin.h
    )
elseif(WXGTK)
    set(SCINTILLA_DEFINES GTK)
    set(SCINTILLA_SOURCES
        ${SCINTILLA_SOURCES}
        ${SCINTILLA_SRC_DIR}/gtk/Converter.h
        ${SCINTILLA_SRC_DIR}/gtk/PlatGTK.cxx
        ${SCINTILLA_SRC_DIR}/gtk/ScintillaGTK.cxx
        ${SCINTILLA_SRC_DIR}/gtk/ScintillaGTK.h
        ${SCINTILLA_SRC_DIR}/gtk/ScintillaGTKAccessible.cxx
        ${SCINTILLA_SRC_DIR}/gtk/ScintillaGTKAccessible.h
        ${SCINTILLA_SRC_DIR}/gtk/scintilla-marshal.c
        ${SCINTILLA_SRC_DIR}/gtk/scintilla-marshal.h
    )
elseif(WXOSX_COCOA)
    set(SCINTILLA_SOURCES
        ${SCINTILLA_SOURCES}
        ${SCINTILLA_SRC_DIR}/cocoa/InfoBar.h
        ${SCINTILLA_SRC_DIR}/cocoa/InfoBar.mm
        ${SCINTILLA_SRC_DIR}/cocoa/InfoBarCommunicator.h
        ${SCINTILLA_SRC_DIR}/cocoa/PlatCocoa.h
        ${SCINTILLA_SRC_DIR}/cocoa/PlatCocoa.mm
        ${SCINTILLA_SRC_DIR}/cocoa/QuartzTextLayout.h
        ${SCINTILLA_SRC_DIR}/cocoa/QuartzTextStyle.h
        ${SCINTILLA_SRC_DIR}/cocoa/QuartzTextStyleAttribute.h
        ${SCINTILLA_SRC_DIR}/cocoa/ScintillaCocoa.h
        ${SCINTILLA_SRC_DIR}/cocoa/ScintillaCocoa.mm
        ${SCINTILLA_SRC_DIR}/cocoa/ScintillaView.h
        ${SCINTILLA_SRC_DIR}/cocoa/ScintillaView.mm
    )
endif()

wx_add_builtin_library(wxscintilla ${SCINTILLA_SOURCES})

target_include_directories(wxscintilla PRIVATE
    ${wxSOURCE_DIR}/${SCINTILLA_SRC_DIR}/include
    ${wxSOURCE_DIR}/${SCINTILLA_SRC_DIR}/src
    ${wxTOOLKIT_INCLUDE_DIRS}
)

target_compile_definitions(wxscintilla PRIVATE
    ${SCINTILLA_DEFINES}
)

if(WIN32)
    target_link_libraries(wxscintilla PRIVATE imm32)
endif()

wx_target_enable_precomp(wxscintilla
    "${wxSOURCE_DIR}/${SCINTILLA_SRC_DIR}/include/Scintilla.h"
)
