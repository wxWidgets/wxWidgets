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
INSTALL=install
INSTALL_DATA=${INSTALL} -m 644
INSTALL_DIR=${INSTALL} -d
TOPDIR=../..
INCDIR_WXPALM=$(TOPDIR)/lib/gcc_lib/palmos
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
$(TOPDIR)/src/common/arrstr.cpp \
$(TOPDIR)/src/common/appbase.cpp \
$(TOPDIR)/src/common/archive.cpp \
$(TOPDIR)/src/common/btncmn.cpp \
$(TOPDIR)/src/common/clntdata.cpp \
$(TOPDIR)/src/common/cmdline.cpp \
$(TOPDIR)/src/common/config.cpp \
$(TOPDIR)/src/common/convauto.cpp \
$(TOPDIR)/src/common/datetime.cpp \
$(TOPDIR)/src/common/datstrm.cpp \
$(TOPDIR)/src/common/dcbufcmn.cpp \
$(TOPDIR)/src/common/dircmn.cpp \
$(TOPDIR)/src/common/dynarray.cpp \
$(TOPDIR)/src/common/dynlib.cpp \
$(TOPDIR)/src/common/dynload.cpp \
$(TOPDIR)/src/common/encconv.cpp \
$(TOPDIR)/src/common/extended.c \
$(TOPDIR)/src/common/ffile.cpp \
$(TOPDIR)/src/common/file.cpp \
$(TOPDIR)/src/common/fileconf.cpp \
$(TOPDIR)/src/common/filefn.cpp \
$(TOPDIR)/src/common/filename.cpp \
$(TOPDIR)/src/common/filesys.cpp \
$(TOPDIR)/src/common/fmapbase.cpp \
$(TOPDIR)/src/common/hash.cpp \
$(TOPDIR)/src/common/hashmap.cpp \
$(TOPDIR)/src/common/init.cpp \
$(TOPDIR)/src/common/intl.cpp \
$(TOPDIR)/src/common/ipcbase.cpp \
$(TOPDIR)/src/common/list.cpp \
$(TOPDIR)/src/common/log.cpp \
$(TOPDIR)/src/common/longlong.cpp \
$(TOPDIR)/src/common/memory.cpp \
$(TOPDIR)/src/common/mimecmn.cpp \
$(TOPDIR)/src/common/module.cpp \
$(TOPDIR)/src/common/mstream.cpp \
$(TOPDIR)/src/common/object.cpp \
$(TOPDIR)/src/common/platinfo.cpp \
$(TOPDIR)/src/common/process.cpp \
$(TOPDIR)/src/common/regex.cpp \
$(TOPDIR)/src/common/stdpbase.cpp \
$(TOPDIR)/src/common/sstream.cpp \
$(TOPDIR)/src/common/stopwatch.cpp \
$(TOPDIR)/src/common/strconv.cpp \
$(TOPDIR)/src/common/stream.cpp \
$(TOPDIR)/src/common/string.cpp \
$(TOPDIR)/src/common/stringimpl.cpp \
$(TOPDIR)/src/common/stringops.cpp \
$(TOPDIR)/src/common/strvararg.cpp \
$(TOPDIR)/src/common/sysopt.cpp \
$(TOPDIR)/src/common/textbuf.cpp \
$(TOPDIR)/src/common/textfile.cpp \
$(TOPDIR)/src/common/tokenzr.cpp \
$(TOPDIR)/src/common/txtstrm.cpp \
$(TOPDIR)/src/common/unichar.cpp \
$(TOPDIR)/src/common/uri.cpp \
$(TOPDIR)/src/common/variant.cpp \
$(TOPDIR)/src/common/wfstream.cpp \
$(TOPDIR)/src/common/wxcrt.cpp \
$(TOPDIR)/src/common/wxprintf.cpp \
$(TOPDIR)/src/common/xti.cpp \
$(TOPDIR)/src/common/xtistrm.cpp \
$(TOPDIR)/src/common/zipstrm.cpp \
$(TOPDIR)/src/common/zstream.cpp

BASE_PALMOS_SRC = \
$(TOPDIR)/src/palmos/base.cpp \
$(TOPDIR)/src/palmos/dir.cpp \
$(TOPDIR)/src/palmos/prefconf.cpp \
$(TOPDIR)/src/palmos/snglinst.cpp \
$(TOPDIR)/src/palmos/thread.cpp \
$(TOPDIR)/src/palmos/utils.cpp \
$(TOPDIR)/src/palmos/utilsexc.cpp

BASE_AND_GUI_CMN_SRC = \
$(TOPDIR)/src/common/event.cpp \
$(TOPDIR)/src/common/fs_mem.cpp \
$(TOPDIR)/src/common/msgout.cpp \
$(TOPDIR)/src/common/utilscmn.cpp \
$(TOPDIR)/src/common/windowid.cpp

BASE_AND_GUI_PALMOS_SRC = \
$(TOPDIR)/src/palmos/main.cpp \
$(TOPDIR)/src/palmos/volume.cpp

NET_CMN_SRC = \
$(TOPDIR)/src/common/fs_inet.cpp \
$(TOPDIR)/src/common/ftp.cpp \
$(TOPDIR)/src/common/http.cpp \
$(TOPDIR)/src/common/protocol.cpp \
$(TOPDIR)/src/common/sckaddr.cpp \
$(TOPDIR)/src/common/sckfile.cpp \
$(TOPDIR)/src/common/sckipc.cpp \
$(TOPDIR)/src/common/sckstrm.cpp \
$(TOPDIR)/src/common/socket.cpp \
$(TOPDIR)/src/common/url.cpp

PALMOS_LOWLEVEL_SRC = \
$(TOPDIR)/src/palmos/stdall.c \
$(TOPDIR)/src/palmos/pflib/pfpvfs.c \
$(TOPDIR)/src/palmos/app.cpp \
$(TOPDIR)/src/palmos/bitmap.cpp \
$(TOPDIR)/src/palmos/brush.cpp \
$(TOPDIR)/src/palmos/caret.cpp \
$(TOPDIR)/src/palmos/clipbrd.cpp \
$(TOPDIR)/src/palmos/cursor.cpp \
$(TOPDIR)/src/palmos/data.cpp \
$(TOPDIR)/src/palmos/dc.cpp \
$(TOPDIR)/src/palmos/dcclient.cpp \
$(TOPDIR)/src/palmos/dcmemory.cpp \
$(TOPDIR)/src/palmos/dcprint.cpp \
$(TOPDIR)/src/palmos/dcscreen.cpp \
$(TOPDIR)/src/palmos/dib.cpp \
$(TOPDIR)/src/palmos/enhmeta.cpp \
$(TOPDIR)/src/palmos/evtloop.cpp \
$(TOPDIR)/src/palmos/font.cpp \
$(TOPDIR)/src/palmos/fontenum.cpp \
$(TOPDIR)/src/palmos/fontutil.cpp \
$(TOPDIR)/src/palmos/gdiimage.cpp \
$(TOPDIR)/src/palmos/minifram.cpp \
$(TOPDIR)/src/palmos/palette.cpp \
$(TOPDIR)/src/palmos/pen.cpp \
$(TOPDIR)/src/palmos/popupwin.cpp \
$(TOPDIR)/src/palmos/printdlg.cpp \
$(TOPDIR)/src/palmos/printpalm.cpp \
$(TOPDIR)/src/palmos/region.cpp \
$(TOPDIR)/src/palmos/settings.cpp \
$(TOPDIR)/src/palmos/timer.cpp \
$(TOPDIR)/src/palmos/tooltip.cpp \
$(TOPDIR)/src/palmos/toplevel.cpp \
$(TOPDIR)/src/palmos/utilsgui.cpp \
$(TOPDIR)/src/palmos/window.cpp 

PALMOS_DESKTOP_LOWLEVEL_SRC = \
$(TOPDIR)/src/palmos/help.cpp

PALMOS_SRC = \
$(TOPDIR)/src/palmos/accel.cpp \
$(TOPDIR)/src/palmos/bmpbuttn.cpp \
$(TOPDIR)/src/palmos/button.cpp \
$(TOPDIR)/src/palmos/checkbox.cpp \
$(TOPDIR)/src/palmos/checklst.cpp \
$(TOPDIR)/src/palmos/choice.cpp \
$(TOPDIR)/src/palmos/colordlg.cpp \
$(TOPDIR)/src/palmos/combobox.cpp \
$(TOPDIR)/src/palmos/control.cpp \
$(TOPDIR)/src/palmos/dialog.cpp \
$(TOPDIR)/src/generic/dirdlgg.cpp \
$(TOPDIR)/src/palmos/dragimag.cpp \
$(TOPDIR)/src/palmos/fdrepdlg.cpp \
$(TOPDIR)/src/generic/filedlgg.cpp \
$(TOPDIR)/src/palmos/fontdlg.cpp \
$(TOPDIR)/src/palmos/frame.cpp \
$(TOPDIR)/src/palmos/gauge.cpp \
$(TOPDIR)/src/palmos/imaglist.cpp \
$(TOPDIR)/src/palmos/listbox.cpp \
$(TOPDIR)/src/generic/listctrl.cpp \
$(TOPDIR)/src/palmos/mdi.cpp \
$(TOPDIR)/src/palmos/menu.cpp \
$(TOPDIR)/src/palmos/menuitem.cpp \
$(TOPDIR)/src/palmos/metafile.cpp \
$(TOPDIR)/src/palmos/msgdlg.cpp \
$(TOPDIR)/src/palmos/nativdlg.cpp \
$(TOPDIR)/src/palmos/notebook.cpp \
$(TOPDIR)/src/palmos/ownerdrw.cpp \
$(TOPDIR)/src/palmos/progdlg.cpp \
$(TOPDIR)/src/palmos/radiobox.cpp \
$(TOPDIR)/src/palmos/radiobut.cpp \
$(TOPDIR)/src/palmos/scrolbar.cpp \
$(TOPDIR)/src/palmos/slider.cpp \
$(TOPDIR)/src/palmos/spinbutt.cpp \
$(TOPDIR)/src/palmos/spinctrl.cpp \
$(TOPDIR)/src/palmos/statbmp.cpp \
$(TOPDIR)/src/palmos/statbox.cpp \
$(TOPDIR)/src/palmos/statbrpalm.cpp \
$(TOPDIR)/src/palmos/statline.cpp \
$(TOPDIR)/src/palmos/stattext.cpp \
$(TOPDIR)/src/palmos/toolbar.cpp \
$(TOPDIR)/src/palmos/textctrl.cpp \
$(TOPDIR)/src/palmos/tglbtn.cpp \
$(TOPDIR)/src/generic/treectlg.cpp \

GUI_CMN_SRC = \
$(TOPDIR)/src/common/accesscmn.cpp \
$(TOPDIR)/src/common/appcmn.cpp \
$(TOPDIR)/src/common/artprov.cpp \
$(TOPDIR)/src/common/artstd.cpp \
$(TOPDIR)/src/common/bmpbase.cpp \
$(TOPDIR)/src/common/bookctrl.cpp \
$(TOPDIR)/src/common/choiccmn.cpp \
$(TOPDIR)/src/common/clipcmn.cpp \
$(TOPDIR)/src/common/colourcmn.cpp \
$(TOPDIR)/src/common/cmdproc.cpp \
$(TOPDIR)/src/common/cmndata.cpp \
$(TOPDIR)/src/common/containr.cpp \
$(TOPDIR)/src/common/cshelp.cpp \
$(TOPDIR)/src/common/ctrlcmn.cpp \
$(TOPDIR)/src/common/ctrlsub.cpp \
$(TOPDIR)/src/common/datacmn.cpp \
$(TOPDIR)/src/common/dcbase.cpp \
$(TOPDIR)/src/common/dlgcmn.cpp \
$(TOPDIR)/src/common/dndcmn.cpp \
$(TOPDIR)/src/common/dobjcmn.cpp \
$(TOPDIR)/src/common/docmdi.cpp \
$(TOPDIR)/src/common/docview.cpp \
$(TOPDIR)/src/common/dpycmn.cpp \
$(TOPDIR)/src/common/dseldlg.cpp \
$(TOPDIR)/src/common/effects.cpp \
$(TOPDIR)/src/common/evtloopcmn.cpp \
$(TOPDIR)/src/common/fddlgcmn.cpp \
$(TOPDIR)/src/common/filectrlcmn.cpp \
$(TOPDIR)/src/common/fldlgcmn.cpp \
$(TOPDIR)/src/common/fontcmn.cpp \
$(TOPDIR)/src/common/fontenumcmn.cpp \
$(TOPDIR)/src/common/fontmap.cpp \
$(TOPDIR)/src/common/framecmn.cpp \
$(TOPDIR)/src/common/gaugecmn.cpp \
$(TOPDIR)/src/common/gbsizer.cpp \
$(TOPDIR)/src/common/gdicmn.cpp \
$(TOPDIR)/src/common/geometry.cpp \
$(TOPDIR)/src/common/gifdecod.cpp \
$(TOPDIR)/src/common/helpbase.cpp \
$(TOPDIR)/src/common/iconbndl.cpp \
$(TOPDIR)/src/common/imagall.cpp \
$(TOPDIR)/src/common/imagbmp.cpp \
$(TOPDIR)/src/common/image.cpp \
$(TOPDIR)/src/common/imagfill.cpp \
$(TOPDIR)/src/common/imaggif.cpp \
$(TOPDIR)/src/common/imagiff.cpp \
$(TOPDIR)/src/common/imagjpeg.cpp \
$(TOPDIR)/src/common/imagpcx.cpp \
$(TOPDIR)/src/common/imagpng.cpp \
$(TOPDIR)/src/common/imagpnm.cpp \
$(TOPDIR)/src/common/imagtiff.cpp \
$(TOPDIR)/src/common/imagxpm.cpp \
$(TOPDIR)/src/common/layout.cpp \
$(TOPDIR)/src/common/lboxcmn.cpp \
$(TOPDIR)/src/common/listctrlcmn.cpp \
$(TOPDIR)/src/common/matrix.cpp \
$(TOPDIR)/src/common/menucmn.cpp \
$(TOPDIR)/src/common/nbkbase.cpp \
$(TOPDIR)/src/common/paper.cpp \
$(TOPDIR)/src/common/popupcmn.cpp \
$(TOPDIR)/src/common/prntbase.cpp \
$(TOPDIR)/src/common/quantize.cpp \
$(TOPDIR)/src/common/radiocmn.cpp \
$(TOPDIR)/src/common/rendcmn.cpp \
$(TOPDIR)/src/common/rgncmn.cpp \
$(TOPDIR)/src/common/settcmn.cpp \
$(TOPDIR)/src/common/sizer.cpp \
$(TOPDIR)/src/common/statbar.cpp \
$(TOPDIR)/src/common/stattextcmn.cpp \
$(TOPDIR)/src/common/stockitem.cpp \
$(TOPDIR)/src/common/taskbarcmn.cpp \
$(TOPDIR)/src/common/tbarbase.cpp \
$(TOPDIR)/src/common/textcmn.cpp \
$(TOPDIR)/src/common/textentrycmn.cpp \
$(TOPDIR)/src/common/timercmn.cpp \
$(TOPDIR)/src/common/timerimpl.cpp \
$(TOPDIR)/src/common/toplvcmn.cpp \
$(TOPDIR)/src/common/treebase.cpp \
$(TOPDIR)/src/common/valgen.cpp \
$(TOPDIR)/src/common/validate.cpp \
$(TOPDIR)/src/common/valtext.cpp \
$(TOPDIR)/src/common/wincmn.cpp \
$(TOPDIR)/src/common/xpmdecod.cpp \
$(TOPDIR)/src/generic/busyinfo.cpp \
$(TOPDIR)/src/generic/choicdgg.cpp \
$(TOPDIR)/src/generic/choicbkg.cpp \
$(TOPDIR)/src/generic/colour.cpp \
$(TOPDIR)/src/generic/dcpsg.cpp \
$(TOPDIR)/src/generic/dirctrlg.cpp \
$(TOPDIR)/src/generic/dragimgg.cpp \
$(TOPDIR)/src/generic/filectrlg.cpp \
$(TOPDIR)/src/generic/icon.cpp \
$(TOPDIR)/src/generic/listbkg.cpp \
$(TOPDIR)/src/generic/logg.cpp \
$(TOPDIR)/src/generic/numdlgg.cpp \
$(TOPDIR)/src/generic/panelg.cpp \
$(TOPDIR)/src/generic/printps.cpp \
$(TOPDIR)/src/generic/renderg.cpp \
$(TOPDIR)/src/generic/scrlwing.cpp \
$(TOPDIR)/src/generic/selstore.cpp \
$(TOPDIR)/src/generic/spinctlg.cpp \
$(TOPDIR)/src/generic/splitter.cpp \
$(TOPDIR)/src/generic/textdlgg.cpp \
$(TOPDIR)/src/generic/tipwin.cpp \
$(TOPDIR)/src/generic/vlbox.cpp \
$(TOPDIR)/src/generic/vscroll.cpp

ADVANCED_CMN_SRC = \
$(TOPDIR)/src/generic/calctrl.cpp \
$(TOPDIR)/src/generic/datectlg.cpp \
$(TOPDIR)/src/generic/grid.cpp \
$(TOPDIR)/src/generic/gridctrl.cpp \
$(TOPDIR)/src/generic/gridsel.cpp \
$(TOPDIR)/src/generic/helpext.cpp \
$(TOPDIR)/src/generic/laywin.cpp \
$(TOPDIR)/src/generic/sashwin.cpp \
$(TOPDIR)/src/generic/splash.cpp \
$(TOPDIR)/src/generic/tipdlg.cpp \
$(TOPDIR)/src/generic/wizard.cpp

ADVANCED_PALMOS_SRC = \
$(TOPDIR)/src/palmos/sound.cpp \
$(TOPDIR)/src/palmos/taskbar.cpp

ADVANCED_PALMOS_DESKTOP_SRC = \
$(TOPDIR)/src/palmos/joystick.cpp

ADVANCED_PALMOS_NATIVE_SRC = \
$(TOPDIR)/src/palmos/datectrl.cpp

MEDIA_CMN_SRC = \
$(TOPDIR)/src/common/mediactrlcmn.cpp

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
$(TOPDIR)/src/html/helpctrl.cpp \
$(TOPDIR)/src/html/helpdata.cpp \
$(TOPDIR)/src/html/helpfrm.cpp \
$(TOPDIR)/src/html/htmlcell.cpp \
$(TOPDIR)/src/html/htmlfilt.cpp \
$(TOPDIR)/src/html/htmlpars.cpp \
$(TOPDIR)/src/html/htmltag.cpp \
$(TOPDIR)/src/html/htmlwin.cpp \
$(TOPDIR)/src/html/htmprint.cpp \
$(TOPDIR)/src/html/m_dflist.cpp \
$(TOPDIR)/src/html/m_fonts.cpp \
$(TOPDIR)/src/html/m_hline.cpp \
$(TOPDIR)/src/html/m_image.cpp \
$(TOPDIR)/src/html/m_layout.cpp \
$(TOPDIR)/src/html/m_links.cpp \
$(TOPDIR)/src/html/m_list.cpp \
$(TOPDIR)/src/html/m_pre.cpp \
$(TOPDIR)/src/html/m_style.cpp \
$(TOPDIR)/src/html/m_tables.cpp \
$(TOPDIR)/src/html/winpars.cpp \
$(TOPDIR)/src/generic/htmllbox.cpp

XRC_SRC = \
$(TOPDIR)/src/xrc/xh_bmpbt.cpp \
$(TOPDIR)/src/xrc/xh_bmp.cpp \
$(TOPDIR)/src/xrc/xh_bttn.cpp \
$(TOPDIR)/src/xrc/xh_cald.cpp \
$(TOPDIR)/src/xrc/xh_chckb.cpp \
$(TOPDIR)/src/xrc/xh_chckl.cpp \
$(TOPDIR)/src/xrc/xh_choic.cpp \
$(TOPDIR)/src/xrc/xh_choicbk.cpp \
$(TOPDIR)/src/xrc/xh_combo.cpp \
$(TOPDIR)/src/xrc/xh_datectrl.cpp \
$(TOPDIR)/src/xrc/xh_dlg.cpp \
$(TOPDIR)/src/xrc/xh_frame.cpp \
$(TOPDIR)/src/xrc/xh_gauge.cpp \
$(TOPDIR)/src/xrc/xh_gdctl.cpp \
$(TOPDIR)/src/xrc/xh_html.cpp \
$(TOPDIR)/src/xrc/xh_listb.cpp \
$(TOPDIR)/src/xrc/xh_listbk.cpp \
$(TOPDIR)/src/xrc/xh_listc.cpp \
$(TOPDIR)/src/xrc/xh_menu.cpp \
$(TOPDIR)/src/xrc/xh_notbk.cpp \
$(TOPDIR)/src/xrc/xh_panel.cpp \
$(TOPDIR)/src/xrc/xh_radbt.cpp \
$(TOPDIR)/src/xrc/xh_radbx.cpp \
$(TOPDIR)/src/xrc/xh_scrol.cpp \
$(TOPDIR)/src/xrc/xh_scwin.cpp \
$(TOPDIR)/src/xrc/xh_sizer.cpp \
$(TOPDIR)/src/xrc/xh_slidr.cpp \
$(TOPDIR)/src/xrc/xh_spin.cpp \
$(TOPDIR)/src/xrc/xh_split.cpp \
$(TOPDIR)/src/xrc/xh_statbar.cpp \
$(TOPDIR)/src/xrc/xh_stbmp.cpp \
$(TOPDIR)/src/xrc/xh_stbox.cpp \
$(TOPDIR)/src/xrc/xh_stlin.cpp \
$(TOPDIR)/src/xrc/xh_sttxt.cpp \
$(TOPDIR)/src/xrc/xh_text.cpp \
$(TOPDIR)/src/xrc/xh_tglbtn.cpp \
$(TOPDIR)/src/xrc/xh_toolb.cpp \
$(TOPDIR)/src/xrc/xh_tree.cpp \
$(TOPDIR)/src/xrc/xh_unkwn.cpp \
$(TOPDIR)/src/xrc/xh_wizrd.cpp \
$(TOPDIR)/src/xrc/xmlres.cpp \
$(TOPDIR)/src/xrc/xmlrsall.cpp

XML_SRC = \
$(TOPDIR)/src/xml/xml.cpp \
$(TOPDIR)/src/common/xtixml.cpp

OPENGL_SRC = \
$(TOPDIR)/src/palmos/glcanvas.cpp

-include tstinc

#TST_SRC += ../../samples/palmtst/palmtst.cpp
#TST_SRC += ../../samples/minimal/minimal.cpp

SOURCES = \
$(TST_SRC) \
$(BASE_SRC) \
$(BASE_AND_GUI_SRC) \
$(NET_SRC) \
$(CORE_SRC) \
$(ADVANCED_SRC) \
$(MEDIA_SRC) \
$(HTML_SRC) \
$(XRC_SRC) \
$(XML_SRC) \
$(OPENGL_SRC) \

#include othersrc.mak
#SOURCES += $(OTHER_SRC)

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
SDK_LOCATION=$(TOPDIR)
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
#TARGET_PLATFORM=Simulator
TARGET_PLATFORM=Device

#TARGET_FORMAT=PalmOS4

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
INCDIR_PALMOS=-I$(INCDIR_WXPALM) -I$(TOPDIR)/src/palmos/pflib
LOCAL_INCLUDE_PATHS = -Irsc $(INCDIR_PALMOS) -I$(TOPDIR)/include
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
TOOLS_DIR = $(TOPDIR)/PalmOSTools/
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

makefile: $(INCDIR_WXPALM)/wx/setup.h

$(INCDIR_WXPALM)/wx/setup.h: $(INCDIR_WXPALM)/wx/ $(TOPDIR)/include/wx/palmos/setup0.h
	$(INSTALL_DATA) $(TOPDIR)/include/wx/palmos/setup0.h $(INCDIR_WXPALM)/wx/setup.h

$(INCDIR_WXPALM)/wx/:
	$(INSTALL_DIR) $(INCDIR_WXPALM)/wx/

