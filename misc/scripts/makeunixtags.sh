##############################################################################
# Name:       misc/scripts/makeunixtags.sh
# Purpose:    create tags file for a wxWidgets port under a Unix system
# Created:    2007-05-05
# RCS-ID:     $Id$
# Copyright:  (c) 2007 Vadim Zeitlin <vadim@wxwindows.org>
# Licence:    wxWindows licence
##############################################################################

# this function should be called with a single parameter containing addition
# port-specific directories to scan
function create_tags()
{
ctags --totals --c++-kinds=+px --language-force=c++ \
	-I WXDLLEXPORT_DATA+ -I WXDLLEXPORT \
	-I WXDLLIMPEXP_DATA_ADV+ -I WXDLLIMPEXP_ADV \
	-I WXDLLIMPEXP_DATA_AUI+ -I WXDLLIMPEXP_AUI \
	-I WXDLLIMPEXP_DATA_BASE+ -I WXDLLIMPEXP_BASE \
	-I WXDLLIMPEXP_DATA_CORE+ -I WXDLLIMPEXP_CORE \
	-I WXDLLIMPEXP_DATA_DBGRID+ -I WXDLLIMPEXP_DBGRID \
	-I WXDLLIMPEXP_DATA_HTML+ -I WXDLLIMPEXP_HTML \
	-I WXDLLIMPEXP_DATA_GL+ -I WXDLLIMPEXP_GL \
	-I WXDLLIMPEXP_DATA_MEDIA+ -I WXDLLIMPEXP_MEDIA \
	-I WXDLLIMPEXP_DATA_NET+ -I WXDLLIMPEXP_NET \
	-I WXDLLIMPEXP_DATA_ODBC+ -I WXDLLIMPEXP_ODBC \
	-I WXDLLIMPEXP_DATA_QA+ -I WXDLLIMPEXP_QA \
	-I WXDLLIMPEXP_DATA_RICHTEXT+ -I WXDLLIMPEXP_RICHTEXT \
	-I WXDLLIMPEXP_DATA_STC+ -I WXDLLIMPEXP_STC \
	-I WXDLLIMPEXP_DATA_XML+ -I WXDLLIMPEXP_XML \
	-I WXDLLIMPEXP_DATA_XRC+ -I WXDLLIMPEXP_XRC \
	include/wx/*.h \
	include/wx/aui/*.h \
	include/wx/generic/*.h \
	include/wx/$1/*.h \
	include/wx/$1/private/*.h \
	include/wx/html/*.h \
	include/wx/protocol/*.h \
	include/wx/richtext/*.h \
	include/wx/xml/*.h \
	include/wx/xrc/*.h \
	include/wx/unix/*.h \
	src/aui/*.cpp \
	src/common/*.cpp \
	src/generic/*.cpp \
	src/$1/*.cpp \
	src/html/*.cpp \
	src/richtext/*.cpp \
	src/unix/*.cpp \
	src/xml/*.cpp \
	src/xrc/*.cpp
}
