#############################################################################
## Name:        Doxyfile.inc
## Purpose:     Main configuration file for the wxWidgets manual
## Author:      Francesco Montorsi
## RCS-ID:      $Id$
## Licence:     wxWindows license
#############################################################################

#---------------------------------------------------------------------------
# Project related configuration options
#---------------------------------------------------------------------------

DOXYFILE_ENCODING      = UTF-8
PROJECT_NAME           = wxWidgets
PROJECT_NUMBER         = 2.9.0
OUTPUT_DIRECTORY       = out
CREATE_SUBDIRS         = NO
OUTPUT_LANGUAGE        = English
BRIEF_MEMBER_DESC      = YES
REPEAT_BRIEF           = YES
ABBREVIATE_BRIEF       =
ALWAYS_DETAILED_SEC    = YES
INLINE_INHERITED_MEMB  = NO
FULL_PATH_NAMES        = NO
STRIP_FROM_PATH        =
STRIP_FROM_INC_PATH    =
SHORT_NAMES            = NO
JAVADOC_AUTOBRIEF      = NO
QT_AUTOBRIEF           = NO
MULTILINE_CPP_IS_BRIEF = NO
DETAILS_AT_TOP         = NO
INHERIT_DOCS           = YES
SEPARATE_MEMBER_PAGES  = NO
TAB_SIZE               = 4
OPTIMIZE_OUTPUT_FOR_C  = NO
OPTIMIZE_OUTPUT_JAVA   = NO
BUILTIN_STL_SUPPORT    = NO
CPP_CLI_SUPPORT        = NO
DISTRIBUTE_GROUP_DOC   = YES
SUBGROUPING            = YES

#---------------------------------------------------------------------------
# Aliases
#---------------------------------------------------------------------------

# This is the most important part of the wxWidgets Doxyfile; aliases allow
# us to keep the headers readable and "implement" wxWidgets-specific commands.

# event aliases
ALIASES = beginEventTable="\n<span class='events'>Events:</span>"
ALIASES += event{1}="\li <span class='event'>\1</span>"
ALIASES += event{2}="\li <span class='event'>\1, \2</span>"
ALIASES += event{3}="\li <span class='event'>\1, \2, \3</span>"
ALIASES += event{4}="\li <span class='event'>\1, \2, \3, \4</span>"
ALIASES += endEventTable="\n"

# style aliases
ALIASES += beginStyleTable="\n<span class='styles'>Styles:</span>"
ALIASES += style{1}="\li <span class='style'>\1</span>"
ALIASES += endStyleTable="\n"

# extra style aliases
ALIASES += beginExtraStyleTable="<div><span class='styles'>Extra styles:</span>"
ALIASES += endExtraStyleTable="</div>"

# aliases for additional wx-specific infos
ALIASES += category{1}="<div><span class='category'>Category:</span>&nbsp;&nbsp;<span class='category_text'>\ref page_class_cat_\1</span></div>"
ALIASES += library{1}="<div><span class='lib'>Library:</span>&nbsp;&nbsp;<span class='lib_text'>\ref page_libs_\1</span></div>"
ALIASES += nolibrary="<div><span class='lib'>Library:</span>&nbsp;&nbsp;<span class='lib_text'>None; this class implementation is entirely header-based.</span></div>"
ALIASES += stdobjects="<span class='stdobj'>Predefined objects:</span>"

# native implementation infos:
ALIASES += nativeimpl{1}="<div><span class='impl'>Implementations:</span>&nbsp;&nbsp;native under \ref page_port_\1 port; a generic implementation is used elsewhere.</div>"
ALIASES += nativeimpl{2}="<div><span class='impl'>Implementations:</span>&nbsp;&nbsp;native under \ref page_port_\1, \ref page_port_\2 ports; a generic implementation is used elsewhere.</div>"
ALIASES += nativeimpl{3}="<div><span class='impl'>Implementations:</span>&nbsp;&nbsp;native under \ref page_port_\1, \ref page_port_\2, \ref page_port_\3 ports; a generic implementation is used elsewhere.</div>"
ALIASES += nativeimpl{4}="<div><span class='impl'>Implementations:</span>&nbsp;&nbsp;native under \ref page_port_\1, \ref page_port_\2, \ref page_port_\3, \ref page_port_\4 ports; a generic implementation is used elsewhere.</div>"

# onlyfor infos:
ALIASES += onlyfor{1}="<div><span class='avail'>Availability:</span>&nbsp;&nbsp;only available for the \ref page_port_\1 port.</div>"
ALIASES += onlyfor{2}="<div><span class='avail'>Availability:</span>&nbsp;&nbsp;only available for the \ref page_port_\1, \ref page_port_\2 ports.</div>"
ALIASES += onlyfor{3}="<div><span class='avail'>Availability:</span>&nbsp;&nbsp;only available for the \ref page_port_\1, \ref page_port_\2, \ref page_port_\3 ports.</div>"
ALIASES += onlyfor{4}="<div><span class='avail'>Availability:</span>&nbsp;&nbsp;only available for the \ref page_port_\1, \ref page_port_\2, \ref page_port_\3, \ref page_port_\4 ports.</div>"

# creates appearance section: this should be used for all main GUI controls
ALIASES += appearance{1}="@htmlonly <div><span class='appearance'>Appearance:</span><br><table class='appearance'><tr><td><img src='wxmsw/\1' alt='wxMSW appearance' class='appearance'></td><td><img src='wxgtk/\1' alt='wxGTK appearance' class='appearance'></td><td><img src='wxmac/\1' alt='wxMac appearance' class='appearance'></td></tr><tr><td>wxMSW appearance</td><td>wxGTK appearance</td><td>wxMac appearance</td></tr></table></div> @endhtmlonly"

# IMPORTANT: the \headerfile command does not work everywhere we'd need it,
#            e.g. in the constants page we need to specify an header file
#            even if they're not inside a \class, \function or \struct command:
ALIASES += header{1}="Include file:\n \verbatim #include <\1> \endverbatim"

# IMPORTANT: inside a \class, we want to specify the header file only once
#            and not twice as the \headerfile command requires:
ALIASES += wxheader{1}="\headerfile \1 wx/\1"

# the following alias avoids to repeat lots of times the same statement:
ALIASES += wxsince{1}="\since This function is new since wxWidgets version \1."

# some formatting aliases
ALIASES += true="<span class='literal'>true</span>"
ALIASES += false="<span class='literal'>false</span>"
ALIASES += NULL="<span class='literal'>NULL</span>"

# NOTE: these are important as you can't write in the docs
#       just #ifdef since the # character is used by Doxygen
#       for explicit linking; use the following aliases instead!
#       However to avoid overwriting e.g. \if Doxygen command the _
#       character is postfixed
ALIASES += ifdef_="<span class='literal'>\#ifdef</span>"
ALIASES += ifndef_="<span class='literal'>\#ifndef</span>"
ALIASES += if_="<span class='literal'>\#if</span>"
ALIASES += endif_="<span class='literal'>\#endif</span>"

# table aliases for 2 and 3 column tables
ALIASES += beginTable="<center><table class='doctable' border='1' cellspacing='0' cellpadding='3'>"
ALIASES += beginInvisibleTable="<center><table class='doctable' border='0' cellspacing='0' cellpadding='4'>"
ALIASES += row2col{2}="<tr><td>\1</td> <td>\2</td></tr>"
ALIASES += row2col{3}="<tr><td>\1</td> <td>\2,\3</td></tr>"
ALIASES += row2col{4}="<tr><td>\1</td> <td>\2,\3,\4</td></tr>"
ALIASES += row2col{5}="<tr><td>\1</td> <td>\2,\3,\4,\5</td></tr>"
ALIASES += row2col{6}="<tr><td>\1</td> <td>\2,\3,\4,\5,\6</td></tr>"
ALIASES += row2col{7}="<tr><td>\1</td> <td>\2,\3,\4,\5,\6,\7</td></tr>"
ALIASES += row2col{8}="<tr><td>\1</td> <td>\2,\3,\4,\5,\6,\7,\8</td></tr>"
ALIASES += row2col{9}="<tr><td>\1</td> <td>\2,\3,\4,\5,\6,\7,\8,\9</td></tr>"
ALIASES += row2col{10}="<tr><td>\1</td> <td>\2,\3,\4,\5,\6,\7,\8,\9,\10</td></tr>"
ALIASES += row3col{3}="<tr><td>\1</td> <td>\2</td> <td>\3</td></tr>"
ALIASES += row3col{4}="<tr><td>\1</td> <td>\2</td> <td>\3,\4</td></tr>"
ALIASES += row3col{5}="<tr><td>\1</td> <td>\2</td> <td>\3,\4,\5</td></tr>"
ALIASES += row3col{6}="<tr><td>\1</td> <td>\2</td> <td>\3,\4,\5,\6</td></tr>"
ALIASES += row3col{7}="<tr><td>\1</td> <td>\2</td> <td>\3,\4,\5,\6,\7</td></tr>"
ALIASES += row3col{8}="<tr><td>\1</td> <td>\2</td> <td>\3,\4,\5,\6,\7,\8</td></tr>"
ALIASES += row3col{9}="<tr><td>\1</td> <td>\2</td> <td>\3,\4,\5,\6,\7,\8,\9</td></tr>"
ALIASES += row3col{10}="<tr><td>\1</td> <td>\2</td> <td>\3,\4,\5,\6,\7,\8,\9,\10</td></tr>"
ALIASES += endTable="</table></center>"

# definition list aliases
# a definition list currently is rendered as 2-column table but it may use as
# well the <dl>, <dt> and <dd> HTML tags in future.
# In any case a definition list differs from a table because of its semantic
# nature and because it is always the association of a "title" with a "description"
# (in contrast a table may have more than 2 columns of data)...
ALIASES += beginDefList="<table class='doclist'>"
ALIASES += itemdef{2}="<tr><td><span class='itemdef'>\1</span></td> <td>\2</td></tr>"
ALIASES += itemdef{3}="<tr><td><span class='itemdef'>\1</span></td> <td>\2,\3</td></tr>"
ALIASES += itemdef{4}="<tr><td><span class='itemdef'>\1</span></td> <td>\2,\3,\4</td></tr>"
ALIASES += itemdef{5}="<tr><td><span class='itemdef'>\1</span></td> <td>\2,\3,\4,\5</td></tr>"
ALIASES += itemdef{6}="<tr><td><span class='itemdef'>\1</span></td> <td>\2,\3,\4,\5,\6</td></tr>"
ALIASES += itemdef{7}="<tr><td><span class='itemdef'>\1</span></td> <td>\2,\3,\4,\5,\6,\7</td></tr>"
ALIASES += itemdef{8}="<tr><td><span class='itemdef'>\1</span></td> <td>\2,\3,\4,\5,\6,\7,\8</td></tr>"
ALIASES += itemdef{9}="<tr><td><span class='itemdef'>\1</span></td> <td>\2,\3,\4,\5,\6,\7,\8,\9</td></tr>"
ALIASES += itemdef{10}="<tr><td><span class='itemdef'>\1</span></td> <td>\2,\3,\4,\5,\6,\7,\8,\9,\10</td></tr>"
ALIASES += endDefList="</table>"

# See ENABLED_SECTIONS configuration key for more info about these:
ALIASES += beginWxPerlOnly="\if WXPERL_MANUAL \n <b>wxPerl note</b>\n\n"
ALIASES += endWxPerlOnly="\endif"
ALIASES += beginWxPythonOnly="\if WXPYTHON_MANUAL \n <b>wxPython note</b>\n\n"
ALIASES += endWxPythonOnly="\endif"

# This is to get around a bug in Doxygen that prevents use of '{' or '}'
# inside of any aliased command in parameters.
ALIASES += leftCurly="{"
ALIASES += rightCurly="}"


#---------------------------------------------------------------------------
# Build related configuration options
#---------------------------------------------------------------------------
EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = NO
EXTRACT_STATIC         = YES
EXTRACT_LOCAL_CLASSES  = YES
EXTRACT_LOCAL_METHODS  = NO
EXTRACT_ANON_NSPACES   = YES
HIDE_UNDOC_MEMBERS     = NO
HIDE_UNDOC_CLASSES     = NO
HIDE_FRIEND_COMPOUNDS  = NO
HIDE_IN_BODY_DOCS      = NO
INTERNAL_DOCS          = NO
CASE_SENSE_NAMES       = NO
HIDE_SCOPE_NAMES       = NO
SHOW_INCLUDE_FILES     = YES
INLINE_INFO            = YES
SORT_MEMBER_DOCS       = YES
SORT_BRIEF_DOCS        = NO     # don't set it to YES! see http://bugzilla.gnome.org/show_bug.cgi?id=312655
SORT_BY_SCOPE_NAME     = NO
GENERATE_TODOLIST      = YES
GENERATE_TESTLIST      = YES
GENERATE_BUGLIST       = YES
GENERATE_DEPRECATEDLIST= YES

# can contain WXPERL_MANUAL, WXPYTHON_MANUAL keywords
ENABLED_SECTIONS       = 

MAX_INITIALIZER_LINES  = 30
SHOW_USED_FILES        = YES
SHOW_DIRECTORIES       = YES
FILE_VERSION_FILTER    = 

#---------------------------------------------------------------------------
# configuration options related to warning and progress messages
#---------------------------------------------------------------------------
QUIET                  = NO
WARNINGS               = YES
WARN_IF_UNDOCUMENTED   = YES
WARN_IF_DOC_ERROR      = YES
WARN_NO_PARAMDOC       = YES
WARN_FORMAT            = "$file:$line: $text   "
WARN_LOGFILE           = doxygen.log

#---------------------------------------------------------------------------
# configuration options related to the input files
#---------------------------------------------------------------------------
INPUT                  = ./mainpages ./groups ./overviews ../../interface
INPUT_ENCODING         = UTF-8
FILE_PATTERNS          = *.h *.txt
RECURSIVE              = YES
EXCLUDE                = 
EXCLUDE_SYMLINKS       = NO
EXCLUDE_PATTERNS       = 
EXCLUDE_SYMBOLS        = 
EXAMPLE_PATH           = 
EXAMPLE_PATTERNS       = *
EXAMPLE_RECURSIVE      = YES
IMAGE_PATH             = images
INPUT_FILTER           = 
FILTER_PATTERNS        = 
FILTER_SOURCE_FILES    = NO

#---------------------------------------------------------------------------
# configuration options related to source browsing
#---------------------------------------------------------------------------
SOURCE_BROWSER         = NO
INLINE_SOURCES         = NO
STRIP_CODE_COMMENTS    = YES
REFERENCED_BY_RELATION = NO
REFERENCES_RELATION    = NO
REFERENCES_LINK_SOURCE = YES
USE_HTAGS              = NO
VERBATIM_HEADERS       = NO

#---------------------------------------------------------------------------
# configuration options related to the alphabetical class index
#---------------------------------------------------------------------------
ALPHABETICAL_INDEX     = YES
COLS_IN_ALPHA_INDEX    = 5
IGNORE_PREFIX          = wx

#---------------------------------------------------------------------------
# configuration options related to the HTML output
#---------------------------------------------------------------------------
HTML_OUTPUT            = html
HTML_FILE_EXTENSION    = .html
HTML_HEADER            = htmlheader.inc
HTML_FOOTER            = htmlfooter.inc
HTML_STYLESHEET        = 
HTML_ALIGN_MEMBERS     = YES
HTML_DYNAMIC_SECTIONS  = NO
CHM_FILE               = wx.chm
GENERATE_CHI           = NO
BINARY_TOC             = NO
TOC_EXPAND             = NO
DISABLE_INDEX          = NO
ENUM_VALUES_PER_LINE   = 4
GENERATE_TREEVIEW      = NO
TREEVIEW_WIDTH         = 250

#---------------------------------------------------------------------------
# configuration options related to the LaTeX output
#---------------------------------------------------------------------------
LATEX_OUTPUT           = latex
LATEX_CMD_NAME         = latex
MAKEINDEX_CMD_NAME     = makeindex
COMPACT_LATEX          = NO
PAPER_TYPE             = a4wide
EXTRA_PACKAGES         = 
LATEX_HEADER           = latexdocstart.inc
PDF_HYPERLINKS         = YES
USE_PDFLATEX           = YES
LATEX_BATCHMODE        = YES
LATEX_HIDE_INDICES     = NO

#---------------------------------------------------------------------------
# configuration options related to the RTF output
#---------------------------------------------------------------------------
RTF_OUTPUT             = rtf
COMPACT_RTF            = NO
RTF_HYPERLINKS         = NO
RTF_STYLESHEET_FILE    = 
RTF_EXTENSIONS_FILE    = 

#---------------------------------------------------------------------------
# configuration options related to the man page output
#---------------------------------------------------------------------------
MAN_OUTPUT             = man
MAN_EXTENSION          = .3
MAN_LINKS              = NO

#---------------------------------------------------------------------------
# configuration options related to the XML output
#---------------------------------------------------------------------------
XML_OUTPUT             = xml
XML_SCHEMA             = 
XML_DTD                = 
XML_PROGRAMLISTING     = YES

#---------------------------------------------------------------------------
# configuration options for the AutoGen Definitions output
#---------------------------------------------------------------------------


#---------------------------------------------------------------------------
# configuration options related to the Perl module output
#---------------------------------------------------------------------------
GENERATE_PERLMOD       = NO
PERLMOD_LATEX          = YES
PERLMOD_PRETTY         = YES
PERLMOD_MAKEVAR_PREFIX =

#---------------------------------------------------------------------------
# Configuration options related to the preprocessor
#---------------------------------------------------------------------------
ENABLE_PREPROCESSING   = YES
MACRO_EXPANSION        = NO
EXPAND_ONLY_PREDEF     = NO
SEARCH_INCLUDES        = YES
INCLUDE_PATH           = 
INCLUDE_FILE_PATTERNS  = 
PREDEFINED             = 
EXPAND_AS_DEFINED      = 
SKIP_FUNCTION_MACROS   = YES

#---------------------------------------------------------------------------
# Configuration::additions related to external references   
#---------------------------------------------------------------------------
TAGFILES               = 
ALLEXTERNALS           = NO
EXTERNAL_GROUPS        = YES
PERL_PATH              = /usr/bin/perl

#---------------------------------------------------------------------------
# Configuration options related to the dot tool
#---------------------------------------------------------------------------
CLASS_DIAGRAMS         = YES
MSCGEN_PATH            =
HIDE_UNDOC_RELATIONS   = YES
HAVE_DOT               = YES
CLASS_GRAPH            = NO
COLLABORATION_GRAPH    = NO
GROUP_GRAPHS           = NO
UML_LOOK               = NO
TEMPLATE_RELATIONS     = NO
INCLUDE_GRAPH          = NO
INCLUDED_BY_GRAPH      = NO
CALL_GRAPH             = NO
CALLER_GRAPH           = NO
GRAPHICAL_HIERARCHY    = NO
DIRECTORY_GRAPH        = NO
DOT_IMAGE_FORMAT       = png
DOT_PATH               = 
DOTFILE_DIRS           = 
DOT_GRAPH_MAX_NODES    = 50
MAX_DOT_GRAPH_DEPTH    = 1000
DOT_TRANSPARENT        = NO
DOT_MULTI_TARGETS      = NO
GENERATE_LEGEND        = YES
DOT_CLEANUP            = YES

#---------------------------------------------------------------------------
# Configuration::additions related to the search engine
#---------------------------------------------------------------------------
SEARCHENGINE           = NO
