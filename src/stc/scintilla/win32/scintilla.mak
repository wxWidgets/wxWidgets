# Make file for Scintilla on Windows Visual C++ version
# Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
# The License.txt file describes the conditions under which this software may be distributed.
# This makefile is for using Visual C++ with nmake.
# Usage for Microsoft:
#     nmake -f scintilla.mak
# For debug versions define DEBUG on the command line:
#     nmake DEBUG=1 -f scintilla.mak
# The main makefile uses mingw32 gcc and may be more current than this file.

.SUFFIXES: .cxx

DIR_O=.
DIR_BIN=..\bin

COMPONENT=$(DIR_BIN)\Scintilla.dll
LIBSCI=$(DIR_BIN)\libscintilla.lib

LD=link

!IFDEF SUPPORT_XP
ADD_DEFINE=-D_USING_V110_SDK71_
# Different subsystems for 32-bit and 64-bit Windows XP so detect based on Platform
# environment vairable set by vcvars*.bat to be either x86 or x64
!IF "$(PLATFORM)" == "x64"
SUBSYSTEM=-SUBSYSTEM:WINDOWS,5.02
!ELSE
SUBSYSTEM=-SUBSYSTEM:WINDOWS,5.01
!ENDIF
!ELSEIFDEF ARM64
ADD_DEFINE=-D_ARM64_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE=1
SUBSYSTEM=-SUBSYSTEM:WINDOWS,10.00
!ENDIF

CRTFLAGS=-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1 -D_CRT_SECURE_NO_DEPRECATE=1 -D_SCL_SECURE_NO_WARNINGS=1 $(ADD_DEFINE)
CXXFLAGS=-Zi -TP -MP -W4 -EHsc -std:c++17 $(CRTFLAGS)
CXXDEBUG=-Od -MTd -DDEBUG
CXXNDEBUG=-O1 -MT -DNDEBUG -GL
NAME=-Fo
LDFLAGS=-OPT:REF -LTCG -IGNORE:4197 -DEBUG $(SUBSYSTEM)
LDDEBUG=
LIBS=KERNEL32.lib USER32.lib GDI32.lib IMM32.lib OLE32.lib OLEAUT32.lib MSIMG32.lib
NOLOGO=-nologo

!IFDEF QUIET
CXX=@$(CXX)
CXXFLAGS=$(CXXFLAGS) $(NOLOGO)
LDFLAGS=$(LDFLAGS) $(NOLOGO)
!ENDIF

!IFDEF NO_CXX11_REGEX
CXXFLAGS=$(CXXFLAGS) -DNO_CXX11_REGEX
!ENDIF

!IFDEF DEBUG
CXXFLAGS=$(CXXFLAGS) $(CXXDEBUG)
LDFLAGS=$(LDDEBUG) $(LDFLAGS)
!ELSE
CXXFLAGS=$(CXXFLAGS) $(CXXNDEBUG)
!ENDIF

INCLUDES=-I../include -I../src -I../lexlib
CXXFLAGS=$(CXXFLAGS) $(INCLUDES)

all:	$(COMPONENT) $(LIBSCI)

clean:
	-del /q $(DIR_O)\*.obj $(DIR_O)\*.pdb $(DIR_O)\*.asm $(COMPONENT) \
	$(DIR_O)\*.res $(DIR_BIN)\*.map $(DIR_BIN)\*.exp $(DIR_BIN)\*.pdb \
	$(DIR_BIN)\Scintilla.lib $(DIR_BIN)\SciLexer.lib $(LIBSCI)

depend:
	pyw DepGen.py

# Required for base Scintilla
SRC_OBJS=\
	$(DIR_O)\AutoComplete.obj \
	$(DIR_O)\CallTip.obj \
	$(DIR_O)\CaseConvert.obj \
	$(DIR_O)\CaseFolder.obj \
	$(DIR_O)\CellBuffer.obj \
	$(DIR_O)\CharacterCategory.obj \
	$(DIR_O)\CharacterSet.obj \
	$(DIR_O)\CharClassify.obj \
	$(DIR_O)\ContractionState.obj \
	$(DIR_O)\DBCS.obj \
	$(DIR_O)\Decoration.obj \
	$(DIR_O)\Document.obj \
	$(DIR_O)\EditModel.obj \
	$(DIR_O)\Editor.obj \
	$(DIR_O)\EditView.obj \
	$(DIR_O)\Indicator.obj \
	$(DIR_O)\KeyMap.obj \
	$(DIR_O)\LineMarker.obj \
	$(DIR_O)\MarginView.obj \
	$(DIR_O)\PerLine.obj \
	$(DIR_O)\PositionCache.obj \
	$(DIR_O)\RESearch.obj \
	$(DIR_O)\RunStyles.obj \
	$(DIR_O)\Selection.obj \
	$(DIR_O)\Style.obj \
	$(DIR_O)\UniConversion.obj \
	$(DIR_O)\UniqueString.obj \
	$(DIR_O)\ViewStyle.obj \
	$(DIR_O)\XPM.obj

# Required by lexers
LEXLIB_OBJS = \
	$(DIR_O)\Accessor.obj \
	$(DIR_O)\CatalogueL.obj \
	$(DIR_O)\ExternalLexer.obj \
	$(DIR_O)\DefaultLexer.obj \
	$(DIR_O)\LexerBase.obj \
	$(DIR_O)\LexerModule.obj \
	$(DIR_O)\LexerSimple.obj \
	$(DIR_O)\StyleContext.obj \
	$(DIR_O)\WordList.obj

# Required by libraries and DLLs that include lexing
SCILEX_OBJS = \
	$(SRC_OBJS) \
	$(LEXLIB_OBJS) \
	$(LEX_OBJS) \
	$(DIR_O)\HanjaDic.obj \
	$(DIR_O)\PlatWin.obj \
	$(DIR_O)\ScintillaBaseL.obj \
	$(DIR_O)\ScintillaWin.obj

COMPONENT_OBJS = \
	$(DIR_O)\HanjaDic.obj \
	$(DIR_O)\PlatWin.obj \
	$(DIR_O)\ScintillaBase.obj \
	$(DIR_O)\ScintillaDLL.obj \
	$(DIR_O)\ScintillaWin.obj \
	$(SRC_OBJS)

$(DIR_O)\ScintRes.res : ScintRes.rc
	$(RC) -fo$@ $**

$(COMPONENT): $(COMPONENT_OBJS) $(DIR_O)\ScintRes.res
	$(LD) $(LDFLAGS) -DEF:Scintilla.def -DLL -OUT:$@ $** $(LIBS)

$(LIBSCI): $(COMPONENT_OBJS)
	LIB /OUT:$@ $**

# Define how to build all the objects and what they depend on

{..\src}.cxx{$(DIR_O)}.obj::
	$(CXX) $(CXXFLAGS) -c $(NAME)$(DIR_O)\ $<
{..\lexlib}.cxx{$(DIR_O)}.obj::
	$(CXX) $(CXXFLAGS) -c $(NAME)$(DIR_O)\ $<
{..\lexers}.cxx{$(DIR_O)}.obj::
	$(CXX) $(CXXFLAGS) -c $(NAME)$(DIR_O)\ $<
{.}.cxx{$(DIR_O)}.obj::
	$(CXX) $(CXXFLAGS) -c $(NAME)$(DIR_O)\ $<

# Some source files are compiled into more than one object because of different conditional compilation
$(DIR_O)\ScintillaBaseL.obj: ..\src\ScintillaBase.cxx
	$(CXX) $(CXXFLAGS) -DSCI_LEXER -c $(NAME)$@ ..\src\ScintillaBase.cxx

$(DIR_O)\CatalogueL.obj: ..\src\Catalogue.cxx
	$(CXX) $(CXXFLAGS) -DSCI_LEXER -c $(NAME)$@ ..\src\Catalogue.cxx

$(DIR_O)\Catalogue.obj: ..\src\Catalogue.cxx
	$(CXX) $(CXXFLAGS) -DSCI_LEXER -DSCI_EMPTYCATALOGUE -c $(NAME)$@ ..\src\Catalogue.cxx

# Dependencies

!IF EXISTS(nmdeps.mak)

# Protect with !IF EXISTS to handle accidental deletion - just 'nmake -f scintilla.mak deps'

!INCLUDE nmdeps.mak

!ENDIF

