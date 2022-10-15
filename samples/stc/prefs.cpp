//////////////////////////////////////////////////////////////////////////////
// File:        contrib/samples/stc/prefs.cpp
// Purpose:     STC test Preferences initialization
// Maintainer:  Wyo
// Created:     2003-09-01
// Copyright:   (c) wxGuide
// Licence:     wxWindows licence
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


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
     {{mySTC_TYPE_DEFAULT, nullptr},
      {mySTC_TYPE_COMMENT, nullptr},
      {mySTC_TYPE_COMMENT_LINE, nullptr},
      {mySTC_TYPE_COMMENT_DOC, nullptr},
      {mySTC_TYPE_NUMBER, nullptr},
      {mySTC_TYPE_WORD1, CppWordlist1}, // KEYWORDS
      {mySTC_TYPE_STRING, nullptr},
      {mySTC_TYPE_CHARACTER, nullptr},
      {mySTC_TYPE_UUID, nullptr},
      {mySTC_TYPE_PREPROCESSOR, nullptr},
      {mySTC_TYPE_OPERATOR, nullptr},
      {mySTC_TYPE_IDENTIFIER, nullptr},
      {mySTC_TYPE_STRING_EOL, nullptr},
      {mySTC_TYPE_DEFAULT, nullptr}, // VERBATIM
      {mySTC_TYPE_REGEX, nullptr},
      {mySTC_TYPE_COMMENT_SPECIAL, nullptr}, // DOXY
      {mySTC_TYPE_WORD2, CppWordlist2}, // EXTRA WORDS
      {mySTC_TYPE_WORD3, CppWordlist3}, // DOXY KEYWORDS
      {mySTC_TYPE_ERROR, nullptr}, // KEYWORDS ERROR
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr}},
     mySTC_FOLD_COMMENT | mySTC_FOLD_COMPACT | mySTC_FOLD_PREPROC},
    // Python
    {"Python",
     "*.py;*.pyw",
     wxSTC_LEX_PYTHON,
     {{mySTC_TYPE_DEFAULT, nullptr},
      {mySTC_TYPE_COMMENT_LINE, nullptr},
      {mySTC_TYPE_NUMBER, nullptr},
      {mySTC_TYPE_STRING, nullptr},
      {mySTC_TYPE_CHARACTER, nullptr},
      {mySTC_TYPE_WORD1, PythonWordlist1}, // KEYWORDS
      {mySTC_TYPE_DEFAULT, nullptr}, // TRIPLE
      {mySTC_TYPE_DEFAULT, nullptr}, // TRIPLEDOUBLE
      {mySTC_TYPE_DEFAULT, nullptr}, // CLASSNAME
      {mySTC_TYPE_DEFAULT, PythonWordlist2}, // DEFNAME
      {mySTC_TYPE_OPERATOR, nullptr},
      {mySTC_TYPE_IDENTIFIER, nullptr},
      {mySTC_TYPE_DEFAULT, nullptr}, // COMMENT_BLOCK
      {mySTC_TYPE_STRING_EOL, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr}},
     mySTC_FOLD_COMMENTPY | mySTC_FOLD_QUOTESPY},
    // * (any)
    {wxTRANSLATE(DEFAULT_LANGUAGE),
     "*.*",
     wxSTC_LEX_PROPERTIES,
     {{mySTC_TYPE_DEFAULT, nullptr},
      {mySTC_TYPE_DEFAULT, nullptr},
      {mySTC_TYPE_DEFAULT, nullptr},
      {mySTC_TYPE_DEFAULT, nullptr},
      {mySTC_TYPE_DEFAULT, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr},
      {-1, nullptr}},
     0},
    };

const int g_LanguagePrefsSize = WXSIZEOF(g_LanguagePrefs);

//----------------------------------------------------------------------------
//! style types
const StyleInfo g_StylePrefs [] = {
    // mySTC_TYPE_DEFAULT
    {"Default",
     "BLACK", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_WORD1
    {"Keyword1",
     "BLUE", "WHITE",
     "", 10, mySTC_STYLE_BOLD, 0},

    // mySTC_TYPE_WORD2
    {"Keyword2",
     "MIDNIGHT BLUE", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_WORD3
    {"Keyword3",
     "CORNFLOWER BLUE", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_WORD4
    {"Keyword4",
     "CYAN", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_WORD5
    {"Keyword5",
     "DARK GREY", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_WORD6
    {"Keyword6",
     "GREY", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_COMMENT
    {"Comment",
     "FOREST GREEN", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_COMMENT_DOC
    {"Comment (Doc)",
     "FOREST GREEN", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_COMMENT_LINE
    {"Comment line",
     "FOREST GREEN", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_COMMENT_SPECIAL
    {"Special comment",
     "FOREST GREEN", "WHITE",
     "", 10, mySTC_STYLE_ITALIC, 0},

    // mySTC_TYPE_CHARACTER
    {"Character",
     "KHAKI", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_CHARACTER_EOL
    {"Character (EOL)",
     "KHAKI", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_STRING
    {"String",
     "BROWN", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_STRING_EOL
    {"String (EOL)",
     "BROWN", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_DELIMITER
    {"Delimiter",
     "ORANGE", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_PUNCTUATION
    {"Punctuation",
     "ORANGE", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_OPERATOR
    {"Operator",
     "BLACK", "WHITE",
     "", 10, mySTC_STYLE_BOLD, 0},

    // mySTC_TYPE_BRACE
    {"Label",
     "VIOLET", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_COMMAND
    {"Command",
     "BLUE", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_IDENTIFIER
    {"Identifier",
     "BLACK", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_LABEL
    {"Label",
     "VIOLET", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_NUMBER
    {"Number",
     "SIENNA", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_PARAMETER
    {"Parameter",
     "VIOLET", "WHITE",
     "", 10, mySTC_STYLE_ITALIC, 0},

    // mySTC_TYPE_REGEX
    {"Regular expression",
     "ORCHID", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_UUID
    {"UUID",
     "ORCHID", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_VALUE
    {"Value",
     "ORCHID", "WHITE",
     "", 10, mySTC_STYLE_ITALIC, 0},

    // mySTC_TYPE_PREPROCESSOR
    {"Preprocessor",
     "GREY", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_SCRIPT
    {"Script",
     "DARK GREY", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_ERROR
    {"Error",
     "RED", "WHITE",
     "", 10, 0, 0},

    // mySTC_TYPE_UNDEFINED
    {"Undefined",
     "ORANGE", "WHITE",
     "", 10, 0, 0}

    };

const int g_StylePrefsSize = WXSIZEOF(g_StylePrefs);
