// Scintilla source code edit control
// @file LexPB.cxx
// Lexer for PowerBasic by Roland Walter, roland@rowalt.de (for PowerBasic see www.powerbasic.com)
//
// Changes:
// 17.10.2003 Toggling of subs/functions now until next sub/function - this gives better results
// 29.10.2003 1. Bug: Toggling didn't work for subs/functions added in editor
//            2. Own colors for PB constants and Inline Assembler SCE_B_CONSTANT and SCE_B_ASM
//            3. Several smaller syntax coloring improvements and speed optimizations
//
// Copyright for Scintilla: 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#include "Platform.h"

#include "PropSet.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "KeyWords.h"
#include "Scintilla.h"
#include "SciLexer.h"

static inline bool IsTypeCharacter(const int ch)
{
    return ch == '%' || ch == '&' || ch == '@' || ch == '!' || ch == '#' || ch == '$' || ch == '?';
}

static inline bool IsAWordChar(const int ch)
{
    return (ch < 0x80) && (isalnum(ch) || ch == '.' || ch == '_');
}

static inline bool IsAWordStart(const int ch)
{
    return (ch < 0x80) && (isalnum(ch) || ch == '_');
}

bool MatchUpperCase(Accessor &styler, int pos, const char *s)   //Same as styler.Match() but uppercase comparison (a-z,A-Z and space only)
{
    char ch;
    for (int i=0; *s; i++)
    {
        ch=styler.SafeGetCharAt(pos+i);
        if (ch > 0x60) ch -= '\x20';
        if (*s != ch) return false;
        s++;
    }
    return true;
}

static void ColourisePBDoc(unsigned int startPos, int length, int initStyle,WordList *keywordlists[],Accessor &styler) {

    WordList &keywords = *keywordlists[0];

    styler.StartAt(startPos);

    StyleContext sc(startPos, length, initStyle, styler);

    for (; sc.More(); sc.Forward()) {
        switch (sc.state)
        {
            case SCE_B_OPERATOR:
            {
                sc.SetState(SCE_B_DEFAULT);
                break;
            }
            case SCE_B_KEYWORD:
            {
                if (!IsAWordChar(sc.ch))
                {
                    if (!IsTypeCharacter(sc.ch))
                    {
                        char s[100];
                        sc.GetCurrentLowered(s, sizeof(s));
                        if (keywords.InList(s))
                        {
                            if (strcmp(s, "rem") == 0)
                            {
                                sc.ChangeState(SCE_B_COMMENT);
                                if (sc.atLineEnd) {sc.SetState(SCE_B_DEFAULT);}
                            }
                            else if (strcmp(s, "asm") == 0)
                            {
                                sc.ChangeState(SCE_B_ASM);
                                if (sc.atLineEnd) {sc.SetState(SCE_B_DEFAULT);}
                            }
                            else
                            {
                                sc.SetState(SCE_B_DEFAULT);
                            }
                        }
                        else
                        {
                            sc.ChangeState(SCE_B_IDENTIFIER);
                            sc.SetState(SCE_B_DEFAULT);
                        }
                    }
                }
                break;
            }
            case SCE_B_NUMBER:
            {
                if (!IsAWordChar(sc.ch)) {sc.SetState(SCE_B_DEFAULT);}
                break;
            }
            case SCE_B_STRING:
            {
                if (sc.ch == '\"'){sc.ForwardSetState(SCE_B_DEFAULT);}
                break;
            }
            case SCE_B_CONSTANT:
            {
                if (!IsAWordChar(sc.ch)) {sc.SetState(SCE_B_DEFAULT);}
                break;
            }
            case SCE_B_COMMENT:
            {
                if (sc.atLineEnd) {sc.SetState(SCE_B_DEFAULT);}
                break;
            }
            case SCE_B_ASM:
            {
                if (sc.atLineEnd) {sc.SetState(SCE_B_DEFAULT);}
                break;
            }
        }  //switch (sc.state)

        // Determine if a new state should be entered:
        if (sc.state == SCE_B_DEFAULT)
        {
            if (sc.ch == '\'') {sc.SetState(SCE_B_COMMENT);}
            else if (sc.ch == '\"') {sc.SetState(SCE_B_STRING);}
            else if (sc.ch == '&' && tolower(sc.chNext) == 'h') {sc.SetState(SCE_B_NUMBER);}
            else if (sc.ch == '&' && tolower(sc.chNext) == 'b') {sc.SetState(SCE_B_NUMBER);}
            else if (sc.ch == '&' && tolower(sc.chNext) == 'o') {sc.SetState(SCE_B_NUMBER);}
            else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {sc.SetState(SCE_B_NUMBER);}
            else if (IsAWordStart(sc.ch)) {sc.SetState(SCE_B_KEYWORD);}
            else if (sc.ch == '%') {sc.SetState(SCE_B_CONSTANT);}
            else if (sc.ch == '$') {sc.SetState(SCE_B_CONSTANT);}
            else if (sc.ch == '#') {sc.SetState(SCE_B_KEYWORD);}
            else if (sc.ch == '!') {sc.SetState(SCE_B_ASM);}
            else if (isoperator(static_cast<char>(sc.ch)) || (sc.ch == '\\')) {sc.SetState(SCE_B_OPERATOR);}
        }
    }      //for (; sc.More(); sc.Forward())
    sc.Complete();
}

//The folding routine for PowerBasic toggles SUBs and FUNCTIONs only. This was exactly what I wanted,
//nothing more. I had worked with this kind of toggling for several years when I used the great good old
//GFA Basic which is dead now. After testing the feature of toggling FOR-NEXT loops, WHILE-WEND loops
//and so on too I found this is more disturbing then helping (for me). So if You think in another way
//you can (or must) write Your own toggling routine ;-)
static void FoldPBDoc(unsigned int startPos, int length, int, WordList *[], Accessor &styler)
{
    // No folding enabled, no reason to continue...
    if( styler.GetPropertyInt("fold") == 0 )
        return;

    unsigned int endPos = startPos + length;
    int lineCurrent = styler.GetLine(startPos);
    int levelCurrent = SC_FOLDLEVELBASE;
    if (lineCurrent > 0)
        levelCurrent = styler.LevelAt(lineCurrent-1) >> 16;
    int levelNext = levelCurrent;
    char chNext = styler[startPos];

    bool fNewLine=true;
    for (unsigned int i = startPos; i < endPos; i++)
    {
        char ch = chNext;
        chNext = styler.SafeGetCharAt(i + 1);

        if (fNewLine)            //Begin of a new line (The Sub/Function/Macro keywords may occur at begin of line only)
        {
            fNewLine=false;

            switch (ch)
            {
            case ' ':      //Most lines start with space - so check this first
                {
                    int levelUse = levelCurrent;
                    int lev = levelUse | levelNext << 16;
                    styler.SetLevel(lineCurrent, lev);
                    break;
                }
            case 'F':
            case 'S':
            case 'C':
            case 'f':
            case 's':
            case 'c':
                {
                    if( MatchUpperCase(styler,i,"FUNCTION") )
                    {
                        styler.SetLevel(lineCurrent, (SC_FOLDLEVELBASE << 16) | SC_FOLDLEVELHEADERFLAG);
                        levelNext=SC_FOLDLEVELBASE+1;
                    }
                else if( MatchUpperCase(styler,i,"SUB") )
                    {
                        styler.SetLevel(lineCurrent, (SC_FOLDLEVELBASE << 16) | SC_FOLDLEVELHEADERFLAG);
                        levelNext=SC_FOLDLEVELBASE+1;
                    }
                else if( MatchUpperCase(styler,i,"CALLBACK FUNCTION") )
                    {
                        styler.SetLevel(lineCurrent, (SC_FOLDLEVELBASE << 16) | SC_FOLDLEVELHEADERFLAG);
                        levelNext=SC_FOLDLEVELBASE+1;
                    }
                else if( MatchUpperCase(styler,i,"STATIC FUNCTION") )
                    {
                        styler.SetLevel(lineCurrent, (SC_FOLDLEVELBASE << 16) | SC_FOLDLEVELHEADERFLAG);
                        levelNext=SC_FOLDLEVELBASE+1;
                    }
                else if( MatchUpperCase(styler,i,"STATIC SUB") )
                    {
                        styler.SetLevel(lineCurrent, (SC_FOLDLEVELBASE << 16) | SC_FOLDLEVELHEADERFLAG);
                        levelNext=SC_FOLDLEVELBASE+1;
                    }
                break;
                }
            default:
                {
                    int levelUse = levelCurrent;
                    int lev = levelUse | levelNext << 16;
                    styler.SetLevel(lineCurrent, lev);
                    break;
                }
            }  //switch (ch)
        }  //if( fNewLine )

        switch (ch)
        {
            case '\n':
            {
                lineCurrent++;
                levelCurrent = levelNext;
                fNewLine=true;
                break;
            }
            case '\r':
            {
                if (chNext != '\n')
                {
                    lineCurrent++;
                    levelCurrent = levelNext;
                    fNewLine=true;
                }
                break;
            }
        }  //switch (ch)
    }  //for (unsigned int i = startPos; i < endPos; i++)
}

static const char * const pbWordListDesc[] = {
    "Keywords",
    0
};

LexerModule lmPB(SCLEX_POWERBASIC, ColourisePBDoc, "powerbasic", FoldPBDoc, pbWordListDesc);
