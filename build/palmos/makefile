## --------------------------------------------------------------------------
#  Palm OS Generic Protein Makefile for Eclipse v1.0.0
#
# Fill in this file to specify your project and the source that you want
# to build, and the settings involved in the build. The makefile-engine.mk
# will then do the hard work of the makefile and dependency handling.
#
# After starting a new project, please remember the following steps...
#	1. Add all sources and resources in SOURCES and RESOURCES
#	2. Review the other settings as needed.
#
## --------------------------------------------------------------------------

SHELL = /bin/sh
## conditionally include an auto-generated.mk for dynamic definitions
-include auto-generated.mk

## --------------------------------------------------------------------------
# Set up the artifact name.
# The database name and other file names are based on the application name
## --------------------------------------------------------------------------

ARTIFACT_NAME = wxWidgets
EMPTY =
SPACE =$(EMPTY) $(EMPTY)
ESCAPED_ARTIFACT_NAME = $(subst $(SPACE),\ ,$(ARTIFACT_NAME))

PROJECT_NAME = wxWidgets
PRC_NAME = wxWidgets.prc

## --------------------------------------------------------------------------
# Sources and Resources and Definition files
# List all the sources (.c/.cpp), resources (.xrd), and definition file (.sld)
# in your project.  Use project relative path names with forward slashes 
# (src/code.cpp).  Please do not use spaces in directory or file names.
#
# A note about XRD resource files:  If you have existing .rsrc or .rcp files, 
# refer to the documentation for the GenerateXRD tool to convert them into 
# XRD files for use with all Palm OS SDKs.
## --------------------------------------------------------------------------

# TODO: Update all sources and resources

BASE_CMN_SRC = \
../../src/common/appbase.cpp \
../../src/common/archive.cpp \
../../src/common/clntdata.cpp \
../../src/common/cmdline.cpp \
../../src/common/config.cpp \
../../src/common/datetime.cpp \
../../src/common/datstrm.cpp \
../../src/common/dircmn.cpp \
../../src/common/dynarray.cpp \
../../src/common/dynlib.cpp \
../../src/common/dynload.cpp \
../../src/common/encconv.cpp \
../../src/common/extended.c \
../../src/common/ffile.cpp \
../../src/common/file.cpp \
../../src/common/fileconf.cpp \
../../src/common/filefn.cpp \
../../src/common/filename.cpp \
../../src/common/filesys.cpp \
../../src/common/fmapbase.cpp \
../../src/common/fs_zip.cpp \
../../src/common/hash.cpp \
../../src/common/hashmap.cpp \
../../src/common/init.cpp \
../../src/common/intl.cpp \
../../src/common/ipcbase.cpp \
../../src/common/list.cpp \
../../src/common/log.cpp \
../../src/common/longlong.cpp \
../../src/common/memory.cpp \
../../src/common/mimecmn.cpp \
../../src/common/module.cpp \
../../src/common/mstream.cpp \
../../src/common/object.cpp \
../../src/common/platinfo.cpp \
../../src/common/process.cpp \
../../src/common/regex.cpp \
../../src/common/stdpbase.cpp \
../../src/common/sstream.cpp \
../../src/common/stopwatch.cpp \
../../src/common/strconv.cpp \
../../src/common/stream.cpp \
../../src/common/string.cpp \
../../src/common/sysopt.cpp \
../../src/common/textbuf.cpp \
../../src/common/textfile.cpp \
../../src/common/tokenzr.cpp \
../../src/common/txtstrm.cpp \
../../src/common/uri.cpp \
../../src/common/variant.cpp \
../../src/common/wfstream.cpp \
../../src/common/wxchar.cpp \
../../src/common/xti.cpp \
../../src/common/xtistrm.cpp \
../../src/common/zipstrm.cpp \
../../src/common/zstream.cpp

BASE_PALMOS_SRC = \
../../src/palmos/base.cpp \
../../src/palmos/dir.cpp \
../../src/palmos/prefconf.cpp \
../../src/palmos/snglinst.cpp \
../../src/palmos/thread.cpp \
../../src/palmos/utils.cpp \
../../src/palmos/utilsexc.cpp

BASE_AND_GUI_CMN_SRC = \
../../src/common/event.cpp \
../../src/common/fs_mem.cpp \
../../src/common/msgout.cpp \
../../src/common/utilscmn.cpp

BASE_AND_GUI_PALMOS_SRC = \
../../src/palmos/main.cpp \
../../src/palmos/volume.cpp

NET_CMN_SRC = \
../../src/common/fs_inet.cpp \
../../src/common/ftp.cpp \
../../src/common/http.cpp \
../../src/common/protocol.cpp \
../../src/common/sckaddr.cpp \
../../src/common/sckfile.cpp \
../../src/common/sckipc.cpp \
../../src/common/sckstrm.cpp \
../../src/common/socket.cpp \
../../src/common/url.cpp

PALMOS_LOWLEVEL_SRC = \
../../src/palmos/app.cpp \
../../src/palmos/bitmap.cpp \
../../src/palmos/brush.cpp \
../../src/palmos/caret.cpp \
../../src/palmos/clipbrd.cpp \
../../src/generic/colour.cpp \
../../src/palmos/cursor.cpp \
../../src/palmos/data.cpp \
../../src/palmos/dc.cpp \
../../src/palmos/dcclient.cpp \
../../src/palmos/dcmemory.cpp \
../../src/palmos/dcprint.cpp \
../../src/palmos/dcscreen.cpp \
../../src/palmos/dib.cpp \
../../src/palmos/enhmeta.cpp \
../../src/palmos/evtloop.cpp \
../../src/palmos/font.cpp \
../../src/palmos/fontenum.cpp \
../../src/palmos/fontutil.cpp \
../../src/palmos/gdiimage.cpp \
../../src/generic/icon.cpp \
../../src/palmos/minifram.cpp \
../../src/palmos/palette.cpp \
../../src/palmos/pen.cpp \
../../src/palmos/popupwin.cpp \
../../src/palmos/printdlg.cpp \
../../src/palmos/printpalm.cpp \
../../src/palmos/region.cpp \
../../src/palmos/settings.cpp \
../../src/palmos/timer.cpp \
../../src/palmos/tooltip.cpp \
../../src/palmos/toplevel.cpp \
../../src/palmos/utilsgui.cpp \
../../src/palmos/window.cpp 

PALMOS_DESKTOP_LOWLEVEL_SRC = \
../../src/palmos/help.cpp

PALMOS_SRC = \
../../src/palmos/accel.cpp \
../../src/palmos/bmpbuttn.cpp \
../../src/palmos/button.cpp \
../../src/palmos/checkbox.cpp \
../../src/palmos/checklst.cpp \
../../src/palmos/choice.cpp \
../../src/palmos/colordlg.cpp \
../../src/palmos/combobox.cpp \
../../src/palmos/control.cpp \
../../src/palmos/dialog.cpp \
../../src/palmos/dirdlg.cpp \
../../src/palmos/dragimag.cpp \
../../src/palmos/fdrepdlg.cpp \
../../src/palmos/filedlg.cpp \
../../src/palmos/fontdlg.cpp \
../../src/palmos/frame.cpp \
../../src/palmos/gauge.cpp \
../../src/palmos/imaglist.cpp \
../../src/palmos/listbox.cpp \
../../src/palmos/listctrl.cpp \
../../src/palmos/mdi.cpp \
../../src/palmos/menu.cpp \
../../src/palmos/menuitem.cpp \
../../src/palmos/metafile.cpp \
../../src/palmos/msgdlg.cpp \
../../src/palmos/nativdlg.cpp \
../../src/palmos/notebook.cpp \
../../src/palmos/ownerdrw.cpp \
../../src/palmos/radiobox.cpp \
../../src/palmos/radiobut.cpp \
../../src/palmos/scrolbar.cpp \
../../src/palmos/slider.cpp \
../../src/palmos/spinbutt.cpp \
../../src/palmos/spinctrl.cpp \
../../src/palmos/statbmp.cpp \
../../src/palmos/statbox.cpp \
../../src/palmos/statbrpalm.cpp \
../../src/palmos/statline.cpp \
../../src/palmos/stattext.cpp \
../../src/palmos/toolbar.cpp \
../../src/palmos/textctrl.cpp \
../../src/palmos/tglbtn.cpp \
../../src/palmos/treectrl.cpp

GUI_CMN_SRC = \
../../src/common/accesscmn.cpp \
../../src/common/appcmn.cpp \
../../src/common/artprov.cpp \
../../src/common/artstd.cpp \
../../src/common/bmpbase.cpp \
../../src/common/bookctrl.cpp \
../../src/common/choiccmn.cpp \
../../src/common/clipcmn.cpp \
../../src/common/colourcmn.cpp \
../../src/common/cmdproc.cpp \
../../src/common/cmndata.cpp \
../../src/common/containr.cpp \
../../src/common/cshelp.cpp \
../../src/common/ctrlcmn.cpp \
../../src/common/ctrlsub.cpp \
../../src/common/datacmn.cpp \
../../src/common/dcbase.cpp \
../../src/common/dlgcmn.cpp \
../../src/common/dndcmn.cpp \
../../src/common/dobjcmn.cpp \
../../src/common/docmdi.cpp \
../../src/common/docview.cpp \
../../src/common/dpycmn.cpp \
../../src/common/dseldlg.cpp \
../../src/common/effects.cpp \
../../src/common/evtloopcmn.cpp \
../../src/common/fddlgcmn.cpp \
../../src/common/fldlgcmn.cpp \
../../src/common/fontcmn.cpp \
../../src/common/fontenumcmn.cpp \
../../src/common/fontmap.cpp \
../../src/common/framecmn.cpp \
../../src/common/gaugecmn.cpp \
../../src/common/gbsizer.cpp \
../../src/common/gdicmn.cpp \
../../src/common/geometry.cpp \
../../src/common/gifdecod.cpp \
../../src/common/helpbase.cpp \
../../src/common/iconbndl.cpp \
../../src/common/imagall.cpp \
../../src/common/imagbmp.cpp \
../../src/common/image.cpp \
../../src/common/imagfill.cpp \
../../src/common/imaggif.cpp \
../../src/common/imagiff.cpp \
../../src/common/imagjpeg.cpp \
../../src/common/imagpcx.cpp \
../../src/common/imagpng.cpp \
../../src/common/imagpnm.cpp \
../../src/common/imagtiff.cpp \
../../src/common/imagxpm.cpp \
../../src/common/layout.cpp \
../../src/common/lboxcmn.cpp \
../../src/common/matrix.cpp \
../../src/common/menucmn.cpp \
../../src/common/nbkbase.cpp \
../../src/common/paper.cpp \
../../src/common/popupcmn.cpp \
../../src/common/prntbase.cpp \
../../src/common/quantize.cpp \
../../src/common/radiocmn.cpp \
../../src/common/rendcmn.cpp \
../../src/common/rgncmn.cpp \
../../src/common/settcmn.cpp \
../../src/common/sizer.cpp \
../../src/common/statbar.cpp \
../../src/common/stockitem.cpp \
../../src/common/tbarbase.cpp \
../../src/common/textcmn.cpp \
../../src/common/timercmn.cpp \
../../src/common/toplvcmn.cpp \
../../src/common/treebase.cpp \
../../src/common/valgen.cpp \
../../src/common/validate.cpp \
../../src/common/valtext.cpp \
../../src/common/wincmn.cpp \
../../src/common/xpmdecod.cpp \
../../src/generic/busyinfo.cpp \
../../src/generic/choicdgg.cpp \
../../src/generic/choicbkg.cpp \
../../src/generic/dcpsg.cpp \
../../src/generic/dirctrlg.cpp \
../../src/generic/dragimgg.cpp \
../../src/generic/listbkg.cpp \
../../src/generic/logg.cpp \
../../src/generic/numdlgg.cpp \
../../src/generic/panelg.cpp \
../../src/generic/printps.cpp \
../../src/generic/renderg.cpp \
../../src/generic/scrlwing.cpp \
../../src/generic/selstore.cpp \
../../src/generic/spinctlg.cpp \
../../src/generic/splitter.cpp \
../../src/generic/textdlgg.cpp \
../../src/generic/tipwin.cpp \
../../src/generic/treectlg.cpp \
../../src/generic/vlbox.cpp \
../../src/generic/vscroll.cpp

ADVANCED_CMN_SRC = \
../../src/generic/calctrl.cpp \
../../src/generic/datectlg.cpp \
../../src/generic/grid.cpp \
../../src/generic/gridctrl.cpp \
../../src/generic/gridsel.cpp \
../../src/generic/helpext.cpp \
../../src/generic/laywin.cpp \
../../src/generic/sashwin.cpp \
../../src/generic/splash.cpp \
../../src/generic/tipdlg.cpp \
../../src/generic/wizard.cpp

ADVANCED_PALMOS_SRC = \
../../src/common/taskbarcmn.cpp \
../../src/palmos/sound.cpp \
../../src/palmos/taskbar.cpp

ADVANCED_PALMOS_DESKTOP_SRC = \
../../src/palmos/joystick.cpp

ADVANCED_PALMOS_NATIVE_SRC = \
../../src/palmos/datectrl.cpp

MEDIA_CMN_SRC = \
../../src/common/mediactrlcmn.cpp

ADVANCED_PLATFORM_SRC = $(ADVANCED_PALMOS_SRC) $(ADVANCED_PALMOS_DESKTOP_SRC)

ADVANCED_PLATFORM_NATIVE_SRC = $(ADVANCED_PALMOS_NATIVE_SRC)

LOWLEVEL_SRC = $(PALMOS_LOWLEVEL_SRC)

GUI_SRC = $(PALMOS_SRC) $(PALMOS_DESKTOP_SRC)

BASE_PLATFORM_SRC = $(BASE_PALMOS_SRC)

BASE_AND_GUI_PLATFORM_SRC = $(BASE_AND_GUI_PALMOS_SRC)

BASE_SRC = $(BASE_CMN_SRC) $(BASE_PLATFORM_SRC)

BASE_AND_GUI_SRC = $(BASE_AND_GUI_CMN_SRC) $(BASE_AND_GUI_PLATFORM_SRC)

NET_SRC = $(NET_CMN_SRC)

CORE_SRC = $(LOWLEVEL_SRC) $(GUI_SRC) $(GUI_CMN_SRC)

ADVANCED_SRC = $(ADVANCED_CMN_SRC) $(ADVANCED_PLATFORM_SRC) $(ADVANCED_PLATFORM_NATIVE_SRC)

MEDIA_SRC = $(MEDIA_CMN_SRC)

HTML_SRC = \
../../src/html/helpctrl.cpp \
../../src/html/helpdata.cpp \
../../src/html/helpfrm.cpp \
../../src/html/htmlcell.cpp \
../../src/html/htmlfilt.cpp \
../../src/html/htmlpars.cpp \
../../src/html/htmltag.cpp \
../../src/html/htmlwin.cpp \
../../src/html/htmprint.cpp \
../../src/html/m_dflist.cpp \
../../src/html/m_fonts.cpp \
../../src/html/m_hline.cpp \
../../src/html/m_image.cpp \
../../src/html/m_layout.cpp \
../../src/html/m_links.cpp \
../../src/html/m_list.cpp \
../../src/html/m_pre.cpp \
../../src/html/m_style.cpp \
../../src/html/m_tables.cpp \
../../src/html/winpars.cpp \
../../src/generic/htmllbox.cpp

ODBC_SRC = \
../../src/common/db.cpp \
../../src/common/dbtable.cpp

DBGRID_SRC = \
../../src/common/dbgrid.cpp

XRC_SRC = \
../../src/xrc/xh_bmpbt.cpp \
../../src/xrc/xh_bmp.cpp \
../../src/xrc/xh_bttn.cpp \
../../src/xrc/xh_cald.cpp \
../../src/xrc/xh_chckb.cpp \
../../src/xrc/xh_chckl.cpp \
../../src/xrc/xh_choic.cpp \
../../src/xrc/xh_choicbk.cpp \
../../src/xrc/xh_combo.cpp \
../../src/xrc/xh_datectrl.cpp \
../../src/xrc/xh_dlg.cpp \
../../src/xrc/xh_frame.cpp \
../../src/xrc/xh_gauge.cpp \
../../src/xrc/xh_gdctl.cpp \
../../src/xrc/xh_html.cpp \
../../src/xrc/xh_listb.cpp \
../../src/xrc/xh_listbk.cpp \
../../src/xrc/xh_listc.cpp \
../../src/xrc/xh_menu.cpp \
../../src/xrc/xh_notbk.cpp \
../../src/xrc/xh_panel.cpp \
../../src/xrc/xh_radbt.cpp \
../../src/xrc/xh_radbx.cpp \
../../src/xrc/xh_scrol.cpp \
../../src/xrc/xh_scwin.cpp \
../../src/xrc/xh_sizer.cpp \
../../src/xrc/xh_slidr.cpp \
../../src/xrc/xh_spin.cpp \
../../src/xrc/xh_split.cpp \
../../src/xrc/xh_statbar.cpp \
../../src/xrc/xh_stbmp.cpp \
../../src/xrc/xh_stbox.cpp \
../../src/xrc/xh_stlin.cpp \
../../src/xrc/xh_sttxt.cpp \
../../src/xrc/xh_text.cpp \
../../src/xrc/xh_tglbtn.cpp \
../../src/xrc/xh_toolb.cpp \
../../src/xrc/xh_tree.cpp \
../../src/xrc/xh_unkwn.cpp \
../../src/xrc/xh_wizrd.cpp \
../../src/xrc/xmlres.cpp \
../../src/xrc/xmlrsall.cpp

XML_SRC = \
../../src/xml/xml.cpp \
../../src/common/xtixml.cpp

OPENGL_SRC = \
../../src/palmos/glcanvas.cpp

SOURCES =  ../../samples/minimal/minimal.cpp \
$(BASE_SRC) \
$(BASE_AND_GUI_SRC) \
$(NET_SRC) \
$(CORE_SRC) \
$(ADVANCED_SRC) \
$(MEDIA_SRC) \
$(HTML_SRC) \
$(ODBC_SRC) \
$(DBGRID_SRC) \
$(XRC_SRC) \
$(XML_SRC) \
$(OPENGL_SRC) \
../../src/palmos/progdlg.cpp

RESOURCES = ../../include/wx/palmos/wxWidgets.xrd ../../include/wx/palmos/Version.xrd
DEFS_FILE = ../../include/wx/palmos/wxWidgets.def
SLD_FILE = ../../include/wx/palmos/wxWidgets.sld

## --------------------------------------------------------------------------
# Review Database information
# Register Creator ID at: http://dev.palmos.com/creatorid/
## --------------------------------------------------------------------------

CREATOR_ID = wx06
DB_TYPE = appl

DATABASE_RESET =
DATABASE_BACKUP = -b
DATABASE_HIDDEN =
DATABASE_PROTECT =
DATABASE_VERSION = 1

LOCALE = enUS
DATABASE_NAME = $(APP_NAME)

## --------------------------------------------------------------------------
# Choose SDK
# Supported values:
#	sdk-6
## --------------------------------------------------------------------------

SDK_VERSION = sdk-6

ifeq ($(SDK_LOCATION),)
SDK_LOCATION=../../
endif
## --------------------------------------------------------------------------
# Build Settings
# Review the following for your needs.
# The default settings build with debug information and no optimization and
# a target platform of an actual device.
## --------------------------------------------------------------------------

#
# Set Debug or Release configuration for this project via the project 
# properties (right-click on the project and select Properties).
#
DEBUG_OR_RELEASE=Debug

#
# Set the target platform for the build; either Device or Simulator
# Use Device as a target for emulator builds.
#
TARGET_PLATFORM=Simulator

# Specify the level of optimization that you want:
# NONE, SOME, FULL, INTRAPROCEDURAL, INTERPROCEDURAL, INLINING.
# Leave blank to select FULL for release builds, and NONE for
# debug builds.
# INTRAPROCEDURAL, INTERPROCEDURAL, INLINING are applicable to 
# device builds only; simulator builds will translate those levels
# to FULL.
OPTIMIZE_LEVEL = NONE

# Specify warning level
# NONE = suppress all warnings
# ALL = enable all warnings
# <undefined> = default warnings
WARNING_LEVEL = 

# Specify exception handling support
# true = enable it
# false = don't enable it
ENABLE_EXCEPTION_HANDLING=true

# List additional libraries to link with
# (Full or project relative path)
ADDITIONAL_LINK_LIBRARIES =
ADDITIONAL_SIM_LINK_LIBRARIES =

# Additional paths to look for #include "header"
# (Source file directories are automatically included)
# Please note that both local and system include paths should 
# either use "cygwin" syntax or if in Windows syntax the should
# be quoted.  ie: ../MyLibrary/headers or "C:\Source\MyLibrary\headers"
# Additionally, you must explicly specify the "-I" prior to each
# path included in this variable.  Spaces are used to separate
# each path from each other.
LOCAL_INCLUDE_PATHS = -Irsc -I../../lib/gcc_lib/palmos -I../../include
LOCAL_SIM_INCLUDE_PATHS = $(LOCAL_INCLUDE_PATHS)

# Additional paths to look for #include <header>
# (Palm OS SDK directories are automatically included)
# Additionally, you must explicly specify the "-I" prior to each
# path included in this variable.  Spaces are used to separate
# each path from each other.
SYSTEM_INCLUDE_PATHS =
SYSTEM_SIM_INCLUDE_PATHS =

# Specify any needed preprocessor symbols.
# If you set DEFINES to "ASSERTLEVEL=1", the compiler will see "-DASSERTLEVEL=1"
# (separate individual entries with spaces)
DEFINES =
SIM_DEFINES =

# Specify additional compiler flags for all files
ADDITIONAL_COMPILER_FLAGS = 
ADDITIONAL_SIM_COMPILER_FLAGS =

# Specify additional linker flags
ADDITIONAL_LINKER_FLAGS =
ADDITIONAL_SIM_LINKER_FLAGS =

# Specify additional archival flags (for static libraries)
ADDITIONAL_AR_FLAGS =
ADDITIONAL_SIM_AR_FLAGS =


# Tools Directory
# Normally you wouldn't want to override this since the necessary tools
# are in the PATH.  But if you experimenting with other tools, then 
# reset the directory here
# If overridden, end with a trailing '/'

ifeq ($(TOOLS_DIR), )
TOOLS_DIR = ../../../PalmOSTools/
endif

## --------------------------------------------------------------------------
# Resource Settings
## --------------------------------------------------------------------------

# Specify the target text encoding
# LATIN, JAPANESE, SIMPLIFIED_CHINESE
TEXTENCODING = LATIN

# Specify error levels for locale and text encoding
# NOCHECK
# STRICT
# <default>
LOCALE_CHECKING =

# Specific if encoding should be checked
# FALSE, TRUE
STRICT_ENCODING = FALSE

## --------------------------------------------------------------------------
# Output Settings
## --------------------------------------------------------------------------

# Modify if you want object and output files to be in different locations
# (However, please note that all three object directories must be different
# for the makefile engine to properly operate.)
# OUTPUT_DIR is the "main" output directory, where all linked and binary objects
# will be put.

DEBUG_DEVICE_OUTPUT_DIR := DebugDevice
RELEASE_DEVICE_OUTPUT_DIR := ReleaseDevice
DEBUG_SIMULATOR_OUTPUT_DIR := DebugSim
RELEASE_SIMULATOR_OUTPUT_DIR := ReleaseSim
RSC_OBJ_DIR := ResourceObjs

## --------------------------------------------------------------------------
# Makefiles
## --------------------------------------------------------------------------


## include the makefile engine
include makefile-engine.mk

## conditionally include your own custom.mk for your specific overrides or definitions
## this is useful if you don't want to use the auto-generated.mk values
## for some reason
-include custom.mk
