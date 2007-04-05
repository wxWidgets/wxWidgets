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
wxChar* CppWordlist1 =
    _T("asm auto bool break case catch char class const const_cast ")
    _T("continue default delete do double dynamic_cast else enum explicit ")
    _T("export extern false float for friend goto if inline int long ")
    _T("mutable namespace new operator private protected public register ")
    _T("reinterpret_cast return short signed sizeof static static_cast ")
    _T("struct switch template this throw true try typedef typeid ")
    _T("typename union unsigned using virtual void volatile wchar_t ")
    _T("while");
wxChar* CppWordlist2 =
    _T("file");
wxChar* CppWordlist3 =
    _T("a addindex addtogroup anchor arg attention author b brief bug c ")
    _T("class code date def defgroup deprecated dontinclude e em endcode ")
    _T("endhtmlonly endif endlatexonly endlink endverbatim enum example ")
    _T("exception f$ f[ f] file fn hideinitializer htmlinclude ")
    _T("htmlonly if image include ingroup internal invariant interface ")
    _T("latexonly li line link mainpage name namespace nosubgrouping note ")
    _T("overload p page par param post pre ref relates remarks return ")
    _T("retval sa section see showinitializer since skip skipline struct ")
    _T("subsection test throw todo typedef union until var verbatim ")
    _T("verbinclude version warning weakgroup $ @ \"\" & < > # { }");

// Python
wxChar* PythonWordlist1 =
    _T("and assert break class continue def del elif else except exec ")
    _T("finally for from global if import in is lambda None not or pass ")
    _T("print raise return try while yield");
wxChar* PythonWordlist2 =
    _T("ACCELERATORS ALT AUTO3STATE AUTOCHECKBOX AUTORADIOBUTTON BEGIN ")
    _T("BITMAP BLOCK BUTTON CAPTION CHARACTERISTICS CHECKBOX CLASS ")
    _T("COMBOBOX CONTROL CTEXT CURSOR DEFPUSHBUTTON DIALOG DIALOGEX ")
    _T("DISCARDABLE EDITTEXT END EXSTYLE FONT GROUPBOX ICON LANGUAGE ")
    _T("LISTBOX LTEXT MENU MENUEX MENUITEM MESSAGETABLE POPUP PUSHBUTTON ")
    _T("RADIOBUTTON RCDATA RTEXT SCROLLBAR SEPARATOR SHIFT STATE3 ")
    _T("STRINGTABLE STYLE TEXTINCLUDE VALUE VERSION VERSIONINFO VIRTKEY");


//----------------------------------------------------------------------------
//! languages
const LanguageInfo g_LanguagePrefs [] = {
    // C++
    {_T("C++"),
     _T("*.c;*.cc;*.cpp;*.cxx;*.cs;*.h;*.hh;*.hpp;*.hxx;*.sma"),
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
    {_T("Python"),
     _T("*.py;*.pyw"),
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
    {(const wxChar *)DEFAULT_LANGUAGE,
     _T("*.*"),
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
    {_T("Default"),
     _T("BLACK"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_WORD1
    {_T("Keyword1"),
     _T("BLUE"), _T("WHITE"),
     _T(""), 10, mySTC_STYLE_BOLD, 0},

    // mySTC_TYPE_WORD2
    {_T("Keyword2"),
     _T("DARK BLUE"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_WORD3
    {_T("Keyword3"),
     _T("CORNFLOWER BLUE"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_WORD4
    {_T("Keyword4"),
     _T("CYAN"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_WORD5
    {_T("Keyword5"),
     _T("DARK GREY"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_WORD6
    {_T("Keyword6"),
     _T("GREY"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_COMMENT
    {_T("Comment"),
     _T("FOREST GREEN"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_COMMENT_DOC
    {_T("Comment (Doc)"),
     _T("FOREST GREEN"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_COMMENT_LINE
    {_T("Comment line"),
     _T("FOREST GREEN"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_COMMENT_SPECIAL
    {_T("Special comment"),
     _T("FOREST GREEN"), _T("WHITE"),
     _T(""), 10, mySTC_STYLE_ITALIC, 0},

    // mySTC_TYPE_CHARACTER
    {_T("Character"),
     _T("KHAKI"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_CHARACTER_EOL
    {_T("Character (EOL)"),
     _T("KHAKI"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_STRING
    {_T("String"),
     _T("BROWN"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_STRING_EOL
    {_T("String (EOL)"),
     _T("BROWN"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_DELIMITER
    {_T("Delimiter"),
     _T("ORANGE"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_PUNCTUATION
    {_T("Punctuation"),
     _T("ORANGE"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_OPERATOR
    {_T("Operator"),
     _T("BLACK"), _T("WHITE"),
     _T(""), 10, mySTC_STYLE_BOLD, 0},

    // mySTC_TYPE_BRACE
    {_T("Label"),
     _T("VIOLET"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_COMMAND
    {_T("Command"),
     _T("BLUE"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_IDENTIFIER
    {_T("Identifier"),
     _T("BLACK"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_LABEL
    {_T("Label"),
     _T("VIOLET"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_NUMBER
    {_T("Number"),
     _T("SIENNA"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_PARAMETER
    {_T("Parameter"),
     _T("VIOLET"), _T("WHITE"),
     _T(""), 10, mySTC_STYLE_ITALIC, 0},

    // mySTC_TYPE_REGEX
    {_T("Regular expression"),
     _T("ORCHID"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_UUID
    {_T("UUID"),
     _T("ORCHID"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_VALUE
    {_T("Value"),
     _T("ORCHID"), _T("WHITE"),
     _T(""), 10, mySTC_STYLE_ITALIC, 0},

    // mySTC_TYPE_PREPROCESSOR
    {_T("Preprocessor"),
     _T("GREY"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_SCRIPT
    {_T("Script"),
     _T("DARK GREY"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_ERROR
    {_T("Error"),
     _T("RED"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_UNDEFINED
    {_T("Undefined"),
     _T("ORANGE"), _T("WHITE"),
     _T(""), 10, 0, 0}

    };

const int g_StylePrefsSize = WXSIZEOF(g_StylePrefs);
