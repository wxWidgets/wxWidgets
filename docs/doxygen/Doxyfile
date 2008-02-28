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
DISTRIBUTE_GROUP_DOC   = NO
SUBGROUPING            = YES

#---------------------------------------------------------------------------
# Aliases
#---------------------------------------------------------------------------

# This is the most important part of the wxWidgets Doxyfile; aliases allow
# us to keep the headers readable and "implement" wxWidgets-specific commands.

# event aliases
ALIASES = beginEventTable="\section class_events Events\n"
ALIASES += event{1}="\li \b <tt>\1</tt>"
ALIASES += event{2}="\li \b <tt>\1, \2</tt>"
ALIASES += endEventTable="\n"

# style aliases
ALIASES += beginStyleTable="\section class_styles Styles\n"
ALIASES += style{1}="\li \b \c \1"
ALIASES += endStyleTable="\n"

# extra style aliases
ALIASES += beginExtraStyleTable="\section class_extrastyles Extra Styles\n"
ALIASES += endExtraStyleTable="\n"

# misc aliases
ALIASES += library{1}="\section class_lib Library\n \ref page_libs_\1"
ALIASES += nolibrary="\section class_lib Library\n None; this class implementation is entirely header-based."
ALIASES += stdobjects="\section class_stdobj Predefined objects\n \b"

# FIXME: we could also introduce additional tags for adding wx-specific infos
#        to the documented classes/functions; some ideas are:
#          @category{GUI classes}
#          @nativeimpl{wxgtk,wxmsw}
#          @genericimpl{wxuniv,wxmac}
#          @onlyfor{wxmsw}
#          @appeareance{button.png}


# IMPORTANT: the default \sa command does not generate a \section!
ALIASES += seealso="\section class_sa See also\n"

# IMPORTANT: the \headerfile command does not work everywhere we'd need it,
#            e.g. in the constants page we need to specify an header file
#            even if they're not inside a \class, \function or \struct command....
ALIASES += header{1}="Include file:\n \verbatim #include <\1> @endverbatim"

# some formatting aliases
# NOTE: using <b> and <tt> tags is different from using \c \b because in the last case
#       \c gets applied to the first word following and \b to the second word following;
#       we want both bold and typetext attributes to be applied to the same word!
ALIASES += true="<b><tt>true</tt></b>"
ALIASES += false="<b><tt>false</tt></b>"
ALIASES += NULL="<b><tt>NULL</tt></b>"

# FIXME: we could put aliases also for e.g. @wxDefaultSize, @wxDefaultPosition, @wxID_ANY, 
#       @wxEmptyString, @wxNullIcon, etc etc

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
ALIASES += itemdef{2}="<tr><td>\b \1</td> <td>\2</td></tr>"
ALIASES += itemdef{3}="<tr><td>\b \1</td> <td>\2,\3</td></tr>"
ALIASES += itemdef{4}="<tr><td>\b \1</td> <td>\2,\3,\4</td></tr>"
ALIASES += itemdef{5}="<tr><td>\b \1</td> <td>\2,\3,\4,\5</td></tr>"
ALIASES += itemdef{6}="<tr><td>\b \1</td> <td>\2,\3,\4,\5,\6</td></tr>"
ALIASES += itemdef{7}="<tr><td>\b \1</td> <td>\2,\3,\4,\5,\6,\7</td></tr>"
ALIASES += itemdef{8}="<tr><td>\b \1</td> <td>\2,\3,\4,\5,\6,\7,\8</td></tr>"
ALIASES += itemdef{9}="<tr><td>\b \1</td> <td>\2,\3,\4,\5,\6,\7,\8,\9</td></tr>"
ALIASES += itemdef{10}="<tr><td>\b \1</td> <td>\2,\3,\4,\5,\6,\7,\8,\9,\10</td></tr>"
ALIASES += endDefList="</table>"

# NOTE: these are important as you can't write in the docs
#       just #ifdef since the # character is used by Doxygen
#       for explicit linking; use the following aliases instead!
#       However to avoid overwriting e.g. \if Doxygen command the _ character is postfixed
ALIASES += ifdef_="<b><tt>\#ifdef</tt></b>"
ALIASES += ifndef_="<b><tt>\#ifndef</tt></b>"
ALIASES += if_="<b><tt>\#if</tt></b>"
ALIASES += endif_="<b><tt>\#endif</tt></b>"

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
HIDE_UNDOC_MEMBERS     = YES
HIDE_UNDOC_CLASSES     = YES
HIDE_FRIEND_COMPOUNDS  = NO
HIDE_IN_BODY_DOCS      = NO
INTERNAL_DOCS          = NO
CASE_SENSE_NAMES       = NO
HIDE_SCOPE_NAMES       = NO
SHOW_INCLUDE_FILES     = YES
INLINE_INFO            = YES
SORT_MEMBER_DOCS       = YES
SORT_BRIEF_DOCS        = YES
SORT_BY_SCOPE_NAME     = NO
GENERATE_TODOLIST      = YES
GENERATE_TESTLIST      = YES
GENERATE_BUGLIST       = YES
GENERATE_DEPRECATEDLIST= YES

# can contain WXPERL_MANUAL, WXPYTHON_MANUAL keywords
ENABLED_SECTIONS       = 

MAX_INITIALIZER_LINES  = 30
SHOW_USED_FILES        = YES
SHOW_DIRECTORIES       = NO
FILE_VERSION_FILTER    = 

#---------------------------------------------------------------------------
# configuration options related to warning and progress messages
#---------------------------------------------------------------------------
QUIET                  = NO
WARNINGS               = YES
WARN_IF_UNDOCUMENTED   = NO
WARN_IF_DOC_ERROR      = YES
WARN_NO_PARAMDOC       = NO
WARN_FORMAT            = "$file:$line: $text   "
WARN_LOGFILE           = doxygen.log

#---------------------------------------------------------------------------
# configuration options related to the input files
#---------------------------------------------------------------------------
INPUT                  = ./mainpages ./overviews # ../../interface
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
LATEX_HEADER           = 
PDF_HYPERLINKS         = NO
USE_PDFLATEX           = NO
LATEX_BATCHMODE        = NO
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
PERLMOD_LATEX          = NO
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
