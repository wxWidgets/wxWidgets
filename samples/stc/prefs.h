//////////////////////////////////////////////////////////////////////////////
// File:        prefs.h
// Purpose:     STC test Preferences initialization
// Maintainer:  Wyo
// Created:     2003-09-01
// RCS-ID:      $Id$
// Copyright:   (c) wxGuide
// Licence:     wxWindows licence
//////////////////////////////////////////////////////////////////////////////

#ifndef _PREFS_H_
#define _PREFS_H_

//----------------------------------------------------------------------------
// informations
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------

//! wxWidgets headers

//! wxWidgets/contrib headers
#include "wx/stc/stc.h"  // styled text control

//! application headers


//============================================================================
// declarations
//============================================================================

//! general style types
#define mySTC_TYPE_DEFAULT 0

#define mySTC_TYPE_WORD1 1
#define mySTC_TYPE_WORD2 2
#define mySTC_TYPE_WORD3 3
#define mySTC_TYPE_WORD4 4
#define mySTC_TYPE_WORD5 5
#define mySTC_TYPE_WORD6 6

#define mySTC_TYPE_COMMENT 7
#define mySTC_TYPE_COMMENT_DOC 8
#define mySTC_TYPE_COMMENT_LINE 9
#define mySTC_TYPE_COMMENT_SPECIAL 10

#define mySTC_TYPE_CHARACTER 11
#define mySTC_TYPE_CHARACTER_EOL 12
#define mySTC_TYPE_STRING 13
#define mySTC_TYPE_STRING_EOL 14

#define mySTC_TYPE_DELIMITER 15

#define mySTC_TYPE_PUNCTUATION 16

#define mySTC_TYPE_OPERATOR 17

#define mySTC_TYPE_BRACE 18

#define mySTC_TYPE_COMMAND 19
#define mySTC_TYPE_IDENTIFIER 20
#define mySTC_TYPE_LABEL 21
#define mySTC_TYPE_NUMBER 22
#define mySTC_TYPE_PARAMETER 23
#define mySTC_TYPE_REGEX 24
#define mySTC_TYPE_UUID 25
#define mySTC_TYPE_VALUE 26

#define mySTC_TYPE_PREPROCESSOR 27
#define mySTC_TYPE_SCRIPT 28

#define mySTC_TYPE_ERROR 29

//----------------------------------------------------------------------------
//! style bits types
#define mySTC_STYLE_BOLD 1
#define mySTC_STYLE_ITALIC 2
#define mySTC_STYLE_UNDERL 4
#define mySTC_STYLE_HIDDEN 8

//----------------------------------------------------------------------------
//! general folding types
#define mySTC_FOLD_COMMENT 1
#define mySTC_FOLD_COMPACT 2
#define mySTC_FOLD_PREPROC 4

#define mySTC_FOLD_HTML 16
#define mySTC_FOLD_HTMLPREP 32

#define mySTC_FOLD_COMMENTPY 64
#define mySTC_FOLD_QUOTESPY 128

//----------------------------------------------------------------------------
//! flags
#define mySTC_FLAG_WRAPMODE 16

//----------------------------------------------------------------------------
// CommonInfo

struct CommonInfo {
    // editor functionality prefs
    bool syntaxEnable;
    bool foldEnable;
    bool indentEnable;
    // display defaults prefs
    bool readOnlyInitial;
    bool overTypeInitial;
    bool wrapModeInitial;
    bool displayEOLEnable;
    bool indentGuideEnable;
    bool lineNumberEnable;
    bool longLineOnEnable;
    bool whiteSpaceEnable;
};
extern const CommonInfo g_CommonPrefs;

//----------------------------------------------------------------------------
// LanguageInfo

struct LanguageInfo {
    const wxChar *name;
    const wxChar *filepattern;
    int lexer;
    struct {
        int type;
        const wxChar *words;
    } styles [STYLE_TYPES_COUNT];
    int folds;
};

extern const LanguageInfo g_LanguagePrefs[];
extern const int g_LanguagePrefsSize;

//----------------------------------------------------------------------------
// StyleInfo
struct StyleInfo {
    wxChar *name;
    wxChar *foreground;
    wxChar *background;
    wxChar *fontname;
    int fontsize;
    int fontstyle;
    int lettercase;
};

extern const StyleInfo g_StylePrefs[];
extern const int g_StylePrefsSize;

#endif // _PREFS_H_
