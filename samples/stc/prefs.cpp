//////////////////////////////////////////////////////////////////////////////
// File:        contrib/samples/stc/prefs.cpp
// Purpose:     STC test Preferences initialization
// Maintainer:  Wyo
// Created:     2003-09-01
// RCS-ID:      $Id$
// Copyright:   (c) wxGuide
// Licence:     wxWindows licence
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all 'standard' wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//! wxWidgets headers

//! wxWidgets/contrib headers

//! application headers
#include "defsext.h"     // Additional definitions
#include "prefs.h"       // Preferences


//============================================================================
// declarations
//============================================================================

//----------------------------------------------------------------------------
//! language types
const CommonInfo g_CommonPrefs = {
    // editor functionality prefs
    true,  // syntaxEnable
    true,  // foldEnable
    true,  // indentEnable
    // display defaults prefs
    false, // overTypeInitial
    false, // readOnlyInitial
    false,  // wrapModeInitial
    false, // displayEOLEnable
    false, // IndentGuideEnable
    true,  // lineNumberEnable
    false, // longLineOnEnable
    false, // whiteSpaceEnable
};

//----------------------------------------------------------------------------
// keywordlists
// C++
const char* CppWordlist1 =
    "asm auto bool break case catch char class const const_cast "
    "continue default delete do double dynamic_cast else enum explicit "
    "export extern false float for friend goto if inline int long "
    "mutable namespace new operator private protected public register "
    "reinterpret_cast return short signed sizeof static static_cast "
    "struct switch template this throw true try typedef typeid "
    "typename union unsigned using virtual void volatile wchar_t "
    "while";
const char* CppWordlist2 =
    "file";
const char* CppWordlist3 =
    "a addindex addtogroup anchor arg attention author b brief bug c "
    "class code date def defgroup deprecated dontinclude e em endcode "
    "endhtmlonly endif endlatexonly endlink endverbatim enum example "
    "exception f$ f[ f] file fn hideinitializer htmlinclude "
    "htmlonly if image include ingroup internal invariant interface "
    "latexonly li line link mainpage name namespace nosubgrouping note "
    "overload p page par param post pre ref relates remarks return "
    "retval sa section see showinitializer since skip skipline struct "
    "subsection test throw todo typedef union until var verbatim "
    "verbinclude version warning weakgroup $ @ \"\" & < > # { }";

// Python
const char* PythonWordlist1 =
    "and assert break class continue def del elif else except exec "
    "finally for from global if import in is lambda None not or pass "
    "print raise return try while yield";
const char* PythonWordlist2 =
    "ACCELERATORS ALT AUTO3STATE AUTOCHECKBOX AUTORADIOBUTTON BEGIN "
    "BITMAP BLOCK BUTTON CAPTION CHARACTERISTICS CHECKBOX CLASS "
    "COMBOBOX CONTROL CTEXT CURSOR DEFPUSHBUTTON DIALOG DIALOGEX "
    "DISCARDABLE EDITTEXT END EXSTYLE FONT GROUPBOX ICON LANGUAGE "
    "LISTBOX LTEXT MENU MENUEX MENUITEM MESSAGETABLE POPUP PUSHBUTTON "
    "RADIOBUTTON RCDATA RTEXT SCROLLBAR SEPARATOR SHIFT STATE3 "
    "STRINGTABLE STYLE TEXTINCLUDE VALUE VERSION VERSIONINFO VIRTKEY";


//----------------------------------------------------------------------------
//! languages
const LanguageInfo g_LanguagePrefs [] = {
    // C++
    {"C++",
     "*.c;*.cc;*.cpp;*.cxx;*.cs;*.h;*.hh;*.hpp;*.hxx;*.sma",
     wxSTC_LEX_CPP,
     {{mySTC_TYPE_DEFAULT, NULL},
      {mySTC_TYPE_COMMENT, NULL},
      {mySTC_TYPE_COMMENT_LINE, NULL},
      {mySTC_TYPE_COMMENT_DOC, NULL},
      {mySTC_TYPE_NUMBER, NULL},
      {mySTC_TYPE_WORD1, CppWordlist1}, // KEYWORDS
      {mySTC_TYPE_STRING, NULL},
      {mySTC_TYPE_CHARACTER, NULL},
      {mySTC_TYPE_UUID, NULL},
      {mySTC_TYPE_PREPROCESSOR, NULL},
      {mySTC_TYPE_OPERATOR, NULL},
      {mySTC_TYPE_IDENTIFIER, NULL},
      {mySTC_TYPE_STRING_EOL, NULL},
      {mySTC_TYPE_DEFAULT, NULL}, // VERBATIM
      {mySTC_TYPE_REGEX, NULL},
      {mySTC_TYPE_COMMENT_SPECIAL, NULL}, // DOXY
      {mySTC_TYPE_WORD2, CppWordlist2}, // EXTRA WORDS
      {mySTC_TYPE_WORD3, CppWordlist3}, // DOXY KEYWORDS
      {mySTC_TYPE_ERROR, NULL}, // KEYWORDS ERROR
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL}},
     mySTC_FOLD_COMMENT | mySTC_FOLD_COMPACT | mySTC_FOLD_PREPROC},
    // Python
    {"Python",
     "*.py;*.pyw",
     wxSTC_LEX_PYTHON,
     {{mySTC_TYPE_DEFAULT, NULL},
      {mySTC_TYPE_COMMENT_LINE, NULL},
      {mySTC_TYPE_NUMBER, NULL},
      {mySTC_TYPE_STRING, NULL},
      {mySTC_TYPE_CHARACTER, NULL},
      {mySTC_TYPE_WORD1, PythonWordlist1}, // KEYWORDS
      {mySTC_TYPE_DEFAULT, NULL}, // TRIPLE
      {mySTC_TYPE_DEFAULT, NULL}, // TRIPLEDOUBLE
      {mySTC_TYPE_DEFAULT, NULL}, // CLASSNAME
      {mySTC_TYPE_DEFAULT, PythonWordlist2}, // DEFNAME
      {mySTC_TYPE_OPERATOR, NULL},
      {mySTC_TYPE_IDENTIFIER, NULL},
      {mySTC_TYPE_DEFAULT, NULL}, // COMMENT_BLOCK
      {mySTC_TYPE_STRING_EOL, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL}},
     mySTC_FOLD_COMMENTPY | mySTC_FOLD_QUOTESPY},
    // * (any)
    {wxTRANSLATE(DEFAULT_LANGUAGE),
     "*.*",
     wxSTC_LEX_PROPERTIES,
     {{mySTC_TYPE_DEFAULT, NULL},
      {mySTC_TYPE_DEFAULT, NULL},
      {mySTC_TYPE_DEFAULT, NULL},
      {mySTC_TYPE_DEFAULT, NULL},
      {mySTC_TYPE_DEFAULT, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL}},
     0},
    };

const int g_LanguagePrefsSize = WXSIZEOF(g_LanguagePrefs);

//----------------------------------------------------------------------------
//! style types
const StyleInfo g_StylePrefs [] = {
    // mySTC_TYPE_DEFAULT
    {wxT("Default"),
     wxT("BLACK"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_WORD1
    {wxT("Keyword1"),
     wxT("BLUE"), wxT("WHITE"),
     wxT(""), 10, mySTC_STYLE_BOLD, 0},

    // mySTC_TYPE_WORD2
    {wxT("Keyword2"),
     wxT("MIDNIGHT BLUE"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_WORD3
    {wxT("Keyword3"),
     wxT("CORNFLOWER BLUE"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_WORD4
    {wxT("Keyword4"),
     wxT("CYAN"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_WORD5
    {wxT("Keyword5"),
     wxT("DARK GREY"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_WORD6
    {wxT("Keyword6"),
     wxT("GREY"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_COMMENT
    {wxT("Comment"),
     wxT("FOREST GREEN"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_COMMENT_DOC
    {wxT("Comment (Doc)"),
     wxT("FOREST GREEN"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_COMMENT_LINE
    {wxT("Comment line"),
     wxT("FOREST GREEN"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_COMMENT_SPECIAL
    {wxT("Special comment"),
     wxT("FOREST GREEN"), wxT("WHITE"),
     wxT(""), 10, mySTC_STYLE_ITALIC, 0},

    // mySTC_TYPE_CHARACTER
    {wxT("Character"),
     wxT("KHAKI"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_CHARACTER_EOL
    {wxT("Character (EOL)"),
     wxT("KHAKI"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_STRING
    {wxT("String"),
     wxT("BROWN"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_STRING_EOL
    {wxT("String (EOL)"),
     wxT("BROWN"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_DELIMITER
    {wxT("Delimiter"),
     wxT("ORANGE"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_PUNCTUATION
    {wxT("Punctuation"),
     wxT("ORANGE"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_OPERATOR
    {wxT("Operator"),
     wxT("BLACK"), wxT("WHITE"),
     wxT(""), 10, mySTC_STYLE_BOLD, 0},

    // mySTC_TYPE_BRACE
    {wxT("Label"),
     wxT("VIOLET"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_COMMAND
    {wxT("Command"),
     wxT("BLUE"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_IDENTIFIER
    {wxT("Identifier"),
     wxT("BLACK"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_LABEL
    {wxT("Label"),
     wxT("VIOLET"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_NUMBER
    {wxT("Number"),
     wxT("SIENNA"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_PARAMETER
    {wxT("Parameter"),
     wxT("VIOLET"), wxT("WHITE"),
     wxT(""), 10, mySTC_STYLE_ITALIC, 0},

    // mySTC_TYPE_REGEX
    {wxT("Regular expression"),
     wxT("ORCHID"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_UUID
    {wxT("UUID"),
     wxT("ORCHID"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_VALUE
    {wxT("Value"),
     wxT("ORCHID"), wxT("WHITE"),
     wxT(""), 10, mySTC_STYLE_ITALIC, 0},

    // mySTC_TYPE_PREPROCESSOR
    {wxT("Preprocessor"),
     wxT("GREY"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_SCRIPT
    {wxT("Script"),
     wxT("DARK GREY"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_ERROR
    {wxT("Error"),
     wxT("RED"), wxT("WHITE"),
     wxT(""), 10, 0, 0},

    // mySTC_TYPE_UNDEFINED
    {wxT("Undefined"),
     wxT("ORANGE"), wxT("WHITE"),
     wxT(""), 10, 0, 0}

    };

const int g_StylePrefsSize = WXSIZEOF(g_StylePrefs);
