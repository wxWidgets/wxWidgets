// Scintilla source code edit control
/** @file LexLisp.cxx
 ** Lexer for Lisp.
 ** Written by Alexey Yutkin.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

#if 1  // Bricsys change : adjusted for AutoLISP syntax

#include "wx/bricsys/LexLisp.hxx"
#include "Catalogue.h"

// AutoLISP : Colourise + Fold

static inline bool isLispOperator(const char ch)
{
    return ((ch == '(' || ch == ')' || ch == '\''));
}
static inline bool isLispWordstart(const char ch)
{
    return ch != ';'  && !isspacechar(ch) && !isLispOperator(ch) &&  ch != '\"';
}
static inline bool isLispWordEnd(const char ch)
{
    return (isspacechar(ch) || isLispOperator(ch) || ch == '\"' || ch == ';'); // 'isspacechar()' includes 0xD and oxA and TAB
}
static inline bool isNewLine(const char ch)
{
    return (ch == 0xD || ch == 0xA);
}
static inline bool isEndOfLine(const char ch)
{
    return (ch == 0xD || ch == 0xA || ch == 0x0);
}
static char gotoEndOfLine(Accessor &styler, unsigned int& pos)
{
    char ch = styler.SafeGetCharAt(++pos);
    for (; !isEndOfLine(ch); )
    {
        ch = styler.SafeGetCharAt(++pos);
    }
    return ch;
}

static int parseKeyFunctionLisp(Accessor &styler, int& pos, int lengthDoc)
{
    static const char* s_defun     = "defun";          static const size_t l_defun(5);
    static const char* s_setvar    = "setvar";         static const size_t l_setvar(6);
    static const char* s_loadlsp   = "load";           static const size_t l_loadlsp(4);
    static const char* s_loaddcl   = "load_dialog";    static const size_t l_loaddcl(11);

    static const char* s_cmdcall   = "command";        static const size_t l_cmdcall(7);
    static const char* s_cmdcalls  = "command-s";      static const size_t l_cmdcalls(9);
    static const char* s_vlcmdf    = "vl-cmdf";        static const size_t l_vlcmdf(7);

    static const char* s_vl_prefix = "vl-";            static const size_t l_vl_prefix(3);
    static const char* s_vlxdocexp = "vl-doc-export";  static const size_t l_vlxdocexp(13);
    static const char* s_vlxdocimp = "vl-doc-import";  static const size_t l_vlxdocimp(13);
    static const char* s_vlxdocset = "vl-doc-set";     static const size_t l_vlxdocset(10);
    static const char* s_vlxdocref = "vl-doc-ref";     static const size_t l_vlxdocref(10);
    static const char* s_vlxarximp = "vl-arx-import";  static const size_t l_vlxarximp(13);

    static const char* s_vlbbset   = "vl-bb-set";      static const size_t l_vlbbset(9);
    static const char* s_vlbbref   = "vl-bb-ref";      static const size_t l_vlbbref(9);

    static const size_t l_min(4);
    static const size_t l_max(13);
    static char  word[l_max + 2];

    int initPos(++pos);
    bool hasNewLines(false);

    // skip leading '(' + skip white spaces
    char ch = styler.SafeGetCharAt(pos);
    for (; isspacechar(ch) && (pos < lengthDoc); ) // 'isspacechar()' includes 0xD and oxA and TAB
    {
        if (!hasNewLines)
            hasNewLines = isNewLine(ch);
        ch = styler.SafeGetCharAt(++pos);
    }
    ch = tolower(ch);
    if ((ch != 'd') && (ch != 'l') && (ch != 's') && (ch != 'c') && (ch != 'v'))
    {
        if (hasNewLines)
            pos = initPos;
        return stateNoState;
    }

    // parse till next whitespace or ( or ) or ' or \n or \r
    // extract the word - check against key functions
    *word = 0;
    size_t l_word(0);
    for (; !isLispWordEnd(ch) && (l_word < l_max) && (pos < lengthDoc); ++l_word)
    {
        word[l_word] = tolower(ch);
        ch = styler.SafeGetCharAt(++pos);
    }
    if (hasNewLines)
        pos = initPos;
    if ((l_word < l_min) || (l_word > l_max))
    {
        return stateNoState;
    }

    word[l_word] = 0;

    if (*word == 'd')
    {
        if ((l_word == l_defun)     && (strcmp(word, s_defun)    == 0))
            return stateIdDefun;
        return stateNoState;
    }
    if (*word == 's')
    {
        if ((l_word == l_setvar)    && (strcmp(word, s_setvar)   == 0))
            return stateLspSetvar;
        return stateNoState;
    }
    if (*word == 'l')
    {
        if ((l_word == l_loadlsp)   && (strcmp(word, s_loadlsp)  == 0))
            return stateLspLoad;
        if ((l_word == l_loaddcl)   && (strcmp(word, s_loaddcl)  == 0))
            return stateDclLoad;
        return stateNoState;
    }
    if (*word == 'c')
    {
        if ((l_word == l_cmdcall)   && (strcmp(word, s_cmdcall)  == 0))
            return stateLspCmdCall;
        if ((l_word == l_cmdcalls)  && (strcmp(word, s_cmdcalls) == 0))
            return stateLspCmdCall;
        return stateNoState;
    }
    if (*word == 'v')
    {
        if ((l_word == l_vlcmdf)    && (strcmp(word, s_vlcmdf)   == 0))
            return stateLspCmdCall;
    
        if (strncmp(word, s_vl_prefix, l_vl_prefix) == 0)
        {
            if ((l_word == l_vlxdocexp) && (strcmp(word, s_vlxdocexp) == 0))
                return stateLspVlx;
            if ((l_word == l_vlxdocimp) && (strcmp(word, s_vlxdocimp) == 0))
                return stateLspVlx;
            if ((l_word == l_vlxdocset) && (strcmp(word, s_vlxdocset) == 0))
                return stateLspVlx;
            if ((l_word == l_vlxdocref) && (strcmp(word, s_vlxdocref) == 0))
                return stateLspVlx;
            if ((l_word == l_vlxarximp) && (strcmp(word, s_vlxarximp) == 0))
                return stateLspVlx;
            if ((l_word == l_vlbbset) && (strcmp(word, s_vlbbset) == 0))
                return stateLspBB;
            if ((l_word == l_vlbbref) && (strcmp(word, s_vlbbref) == 0))
                return stateLspBB;
        }
        return stateNoState;
    }
    return stateNoState;
}

static void classifyWordLisp(unsigned int start, unsigned int end,
                             const WordList &keywords, const WordList &keywords_kw,
                             const WordList &keywords_us,
                             Accessor &styler)
{
    assert(end >= start);
    static const size_t s_maxChars(1024);
    static char  str[s_maxChars];

    int len = end - start + 1;
    if (len >= s_maxChars)
        len  = s_maxChars - 1;
    else
    if (len < 0)
        len = 0;

    *str = '\0';

    bool digit_flag = true;
    int ch = 0, numSigns = 0, numExps = 0, numDots = 0, ePos = -1;
    --len;
	for (int i = 0, pos = (int)start; i <= len; ++pos, ++i)
    {
		str[i] = ch = tolower(styler[pos]);
        if (digit_flag)
        {
            if ((ch == '+') || (ch == '-')) ++numSigns;
            else
            if (ch == 'e') ++numExps, ePos = i;
            else
            if (ch == '.') ++numDots;
            else
            if (!isdigit(ch)) digit_flag = false;
        }
	}
    str[len + 1] = '\0';

    if (digit_flag)
    {
        if (numExps == 1) --numSigns;  // allow 1 more '-/+' if 'e' is present

        if ((numExps > 1) || (numSigns > 1) || (numDots > 1)) digit_flag = false;
        else
        if (*str == '.') digit_flag = false;  // no leading '.' allowed
        else
        if ((len <= 0) && (numExps || numSigns || numDots)) digit_flag = false;
        else
        if (ePos == 0) digit_flag = false;
    }

    char chAttr = SCE_LISP_IDENTIFIER;
	if (digit_flag)
    {
        chAttr = SCE_LISP_NUMBER;
    }
    else
	if (keywords.InList(str))
    {
		chAttr = SCE_LISP_KEYWORD;
	}
    else
    if (keywords_kw.InList(str))
    {
		chAttr = SCE_LISP_KEYWORD_KW;
	}
    else
    if (keywords_us.InList(str))
    {
		chAttr = SCE_LISP_SPECIAL;
	}

    styler.ColourTo(end, chAttr);
}

// start + end positions need to be outside block comment area ...
static void AdjustStartEndPositions(unsigned int& startPos, int& length, Accessor& styler)
{
    static const int s_insideComments = (stateInBlockComment | stateInComment);

    int sPos  = (int)startPos;
    int ePos  = sPos + length;
	int sLine = styler.GetLine(sPos);
	int eLine = styler.GetLine(ePos);

    // adjust start position
    int oldState = styler.GetLineState(sLine);
    while ((sLine > 0) && ((oldState & s_insideComments) != 0))
    {
        oldState = styler.GetLineState(--sLine);
    }
    if (sLine < 0) sLine = 0;

    // adjust end position
    const int maxLine = styler.GetLine(styler.Length());
    int line(eLine);
    oldState = styler.GetLineState(line);
    while ((line < maxLine) && ((oldState & s_insideComments) != 0))
    {
        oldState = styler.GetLineState(++line);
    }
    eLine = line; //eLine = --line; ?
    if (eLine > maxLine) eLine = maxLine;

	sPos = styler.LineStart(sLine);
    if (sPos < 0) sPos = 0;

    ePos = styler.LineStart(eLine); // if at last line, use last position //ePos = styler.LineStart(eLine + 1); ??

    if (eLine >= maxLine) ePos = styler.Length();
    if (ePos < ((int)startPos + length))
        ePos = ((int)startPos + length);

    startPos = sPos;
    length   = ePos - sPos;
}

static void ColouriseLispDoc(unsigned int startPos, int length, int initStyle,
                             WordList *keywordlists[], Accessor &styler)
{
	static const int stylingBitsMask = 0x1F; // from Document::Document() CTor

    const WordList& keywords    = *keywordlists[0];
    const WordList& keywords_kw = *keywordlists[1];
    const WordList& keywords_us = *keywordlists[2];

    bool atEOL = false, insideString = false;
    int radix = -1;

    AdjustStartEndPositions(startPos, length, styler);

	int lineCurrent = styler.GetLine((int)startPos);
    int state = (startPos > 0) ? styler.StyleAt(startPos - 1) : 0;
    state &= stylingBitsMask;

    int lineState = styler.GetLineState(lineCurrent);
    lineState    &= (~stateInComment) & (~stateInBlockComment);

    bool isMBCS = (styler.Encoding() == encDBCS);

    char ch       = 0;
    char chNext   = styler[startPos];
    int lengthDoc = startPos + length;

    styler.StartAt(startPos);
    styler.StartSegment(startPos);

    for (int i = (int)startPos; i < lengthDoc; ++i)
    {
        ch = chNext;
        chNext = styler.SafeGetCharAt(i + 1);

        atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

        if (isMBCS && styler.IsLeadByte(ch))
        {
            chNext = styler.SafeGetCharAt(i + 2);
            ++i;
            continue;
        }

        if (state == SCE_LISP_DEFAULT)
        {
            if (isLispWordstart(ch))
            {
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, state);
                state = SCE_LISP_IDENTIFIER;
            }
            else
            if (ch == ';')
            {
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, state);
				state = SCE_LISP_COMMENT;
                if (chNext == '|')
                {
                    state = SCE_LISP_MULTI_COMMENT;
                    styler.ColourTo(i, SCE_LISP_COMMENT);
                }
			}
			else
            if (isLispOperator(ch))
            {
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_LISP_OPERATOR);
			}
			else
            if (ch == '\"')
            {
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, state);
				state = SCE_LISP_STRING;
			}
		}
        else
        if (state == SCE_LISP_IDENTIFIER || state == SCE_LISP_SYMBOL)
        {
			if (!isLispWordstart(ch))
            {
				if (state == SCE_LISP_IDENTIFIER)
                {
                    if ((i - 1) >= 0)
                        classifyWordLisp(styler.GetStartSegment(), i - 1, keywords, keywords_kw, keywords_us, styler);
				}
                else
                {
                    if ((i - 1) >= 0)
                        styler.ColourTo(i - 1, state);
				}
				state = SCE_LISP_DEFAULT;
                if (ch == ';')
                {
                    if ((i - 1) >= 0)
                        styler.ColourTo(i - 1, state);
    				state = SCE_LISP_COMMENT;
                    if (chNext == '|')
                    {
                        state = SCE_LISP_MULTI_COMMENT;
                        styler.ColourTo(i, SCE_LISP_COMMENT);
                    }
    			}
                else
                if (ch == '\"')
                {
                    state = SCE_LISP_STRING;
                }
            } /*else*/
			if (isLispOperator(ch))
            {
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_LISP_OPERATOR);
			}
		}
        else
        if (state == SCE_LISP_SPECIAL)
        {
			if (!isLispWordstart(ch) || (radix != -1 && !IsADigit(ch, radix)))
            {
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, state);
				state = SCE_LISP_DEFAULT;
			}
			if (isLispOperator(ch))
            {
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_LISP_OPERATOR);
			}
		}
        else
		if (state == SCE_LISP_COMMENT)
        {
			if (atEOL)
            {
                lineState |= stateInComment;
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, SCE_LISP_COMMENT);
			    state = SCE_LISP_DEFAULT;
			}
		}
        else
        if (state == SCE_LISP_MULTI_COMMENT)
        {
			if (ch == '|' && chNext == ';')
            {
                lineState |= stateInBlockComment;
                styler.ColourTo(++i, SCE_LISP_COMMENT);
                state  = SCE_LISP_DEFAULT;
                chNext = styler.SafeGetCharAt(i + 1);
			}
		}
        else
        if (state == SCE_LISP_STRING)
        {
			if (ch == '\\')
            {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\')
                {
					++i;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			}
            else
            if (ch == '\"')
            {
				styler.ColourTo(i, state);
				state = SCE_LISP_DEFAULT;
			}
		}

		if (atEOL)
        {
            if (state == SCE_LISP_MULTI_COMMENT)
            {
                lineState |= stateInBlockComment;
                styler.ColourTo(i - 1, SCE_LISP_COMMENT);
            }

            if (lineState != styler.GetLineState(lineCurrent))
                styler.SetLineState(lineCurrent, lineState);

            lineState = styler.GetLineState(++lineCurrent);
            lineState &= (~stateInComment) & (~stateInBlockComment);
        }
	}

    if (lineState != styler.GetLineState(lineCurrent))
        styler.SetLineState(lineCurrent, lineState);

    styler.ColourTo(lengthDoc - 1, state);
}

static void FoldLispDoc(unsigned int startPos, int length, int /* initStyle */, WordList *[],
                        Accessor &styler)
{
    int lengthDoc    = startPos + length;
    int lineCurrent  = styler.GetLine(startPos);
    int levelPrev    = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
    int levelCurrent = levelPrev;
	int lev          = 0;

    int lineState    = styler.GetLineState(lineCurrent);
    lineState       &= (~stateLineMask); // clear old function line states

    char ch    = 0, chNext = styler.SafeGetCharAt(startPos);
    int  style = 0;

    for (int pos = (int)startPos; pos < lengthDoc; ++pos)
    {
        ch     = chNext;
        chNext = styler.SafeGetCharAt(pos + 1);
        style  = styler.StyleAt(pos);

        if (style == SCE_LISP_OPERATOR)
        {
			if (ch == '(')
            {
				++levelCurrent;
                lineState |= parseKeyFunctionLisp(styler, pos, lengthDoc);
                // adjust pos, chNext
                chNext = styler.SafeGetCharAt(pos--);
            }
            else
            if (ch == ')')
            {
				--levelCurrent;
			}
		}
        else
        if ((ch == '\r' && chNext != '\n') || (ch == '\n')) // at EOL
        {
			lev = levelPrev;

            if (levelCurrent > levelPrev)
				lev |= SC_FOLDLEVELHEADERFLAG;

            if (lev != styler.LevelAt(lineCurrent))
                styler.SetLevel(lineCurrent, lev);

            levelPrev  = levelCurrent;

            if (lineState != styler.GetLineState(lineCurrent))
                styler.SetLineState(lineCurrent, lineState);

            lineState  = styler.GetLineState(++lineCurrent);
            lineState &= (~stateLineMask); // clear old function line states
        }
	}

    if (lineState != styler.GetLineState(lineCurrent))
        styler.SetLineState(lineCurrent, lineState);

    // Fill in the real level of the next line, keeping the current flags as they will be filled in later
	int flagsNext = styler.LevelAt(lineCurrent) & ~SC_FOLDLEVELNUMBERMASK;
    int newLevel = levelPrev | flagsNext;
    styler.SetLevel(lineCurrent, newLevel);
}

// DCL : Colourise + Fold

static inline bool isDclOperator(const char ch)
{
    return ((ch == '=' || ch == '{' || ch == '}' || ch == ':' || ch == ';'));
}

static inline bool isDclWordstart(const char ch)
{
	return ch != '/'  && !isspacechar(ch) && !isDclOperator(ch) &&  ch != '\"';
}

static bool parseDialog(Accessor &styler, unsigned int pos, unsigned int lengthDoc)
{
    static const char* s_dialog = "dialog";
    static char s_str[10] = {0};

    char ch = 0;

    // skip white spaces
    for (; pos < lengthDoc; ++pos)
    {
        ch = styler.SafeGetCharAt(pos);
        if (wxIsgraph(ch))
            break;
    }

    s_str[0] = tolower(ch);
    if (s_str[0] != 'd')
        return false;

    for (unsigned int i = 1; i < 6; ++i)
    {
        ch = styler.SafeGetCharAt(++pos);
        s_str[i] = tolower(ch);
    }
    s_str[6] = 0;

    if (strcmp(s_str, s_dialog) != 0)
        return false;

    // check next character after 'dialog'
    ch = styler.SafeGetCharAt(++pos);
    if (!wxIsspace(ch) && (ch != wxT('{')))
        return false;

    return true;
}

static void classifyWordDcl(unsigned int start, unsigned int end,
                            const WordList &keywords, const WordList &keywords_kw,
                            Accessor &styler)
{
	assert(end >= start);
	static char s[8192];

    int len = end - start + 1;
    if (len >= 8192) --len;

    bool digit_flag = true;
	for (int i = 0, pos = (int)start; i < len; ++pos)
    {
		s[i] = tolower(styler[pos]);
		if (!isdigit(s[i]) && (s[i] != '.')) digit_flag = false;
		s[++i] = '\0';
	}

    char chAttr = SCE_DCL_IDENTIFIER;
	if (digit_flag)
    {
        chAttr = SCE_DCL_NUMBER;
    }
    else
	if (keywords.InList(s))
    {
		chAttr = SCE_DCL_KEYWORD;
	}
    else
    if (keywords_kw.InList(s))
    {
		chAttr = SCE_DCL_SPECIAL;
	}

    styler.ColourTo(end, chAttr);
	return;
}

static void ColouriseDclDoc(unsigned int startPos, int length, int initStyle,
                            WordList *keywordlists[], Accessor &styler)
{
	const WordList &keywords    = *keywordlists[0];
	const WordList &keywords_kw = *keywordlists[1];

	int lineCurrent = styler.GetLine(startPos);
    bool atEOL = false, insideString = false;

    int state = initStyle, radix = -1;
    if ((styler.GetLineState(lineCurrent) & stateInBlockComment) != 0)
        state = SCE_DCL_COMMENT;

    int lineState = styler.GetLineState(lineCurrent);
    lineState    &= (~stateLineMask); // clear old function line states

    char ch       = 0;
    char chNext   = styler[startPos];
	int lengthDoc = startPos + length;

    styler.StartAt(startPos);
    styler.StartSegment(startPos);

    for (int i = (int)startPos; i < lengthDoc; ++i)
    {
		ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (styler.IsLeadByte(ch))
        {
			chNext = styler.SafeGetCharAt(i + 2);
			++i;
			continue;
		}

		if (state == SCE_DCL_DEFAULT)
        {
            if (isDclWordstart(ch))
            {
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, state);
				state = SCE_DCL_IDENTIFIER;
			}
			else
            if (ch == '/') // comment
            {
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, state);

                if ((chNext == '/') || (chNext == '*'))
                {
                    state = SCE_DCL_COMMENT;
                    if (chNext == '*') state = SCE_DCL_MULTI_COMMENT;
                }
			}
			else
            if (isDclOperator(ch))
            {
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_DCL_OPERATOR);
				if (ch == '\'' && isDclWordstart(chNext))
                {
					state = SCE_DCL_SYMBOL;
				}
			}
			else
            if (ch == '\"')
            {
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, state);
				state = SCE_DCL_STRING;
			}
		}
        else
        if (state == SCE_DCL_IDENTIFIER || state == SCE_DCL_SYMBOL)
        {
			if (!isDclWordstart(ch))
            {
				if (state == SCE_DCL_IDENTIFIER)
                {
                    if ((i - 1) >= 0)
                        classifyWordDcl(styler.GetStartSegment(), i - 1, keywords, keywords_kw, styler);
				}
                else
                {
                    if ((i - 1) >= 0)
                        styler.ColourTo(i - 1, state);
				}
				state = SCE_DCL_DEFAULT;
                if (ch == '\"')
                {
                    state = SCE_DCL_STRING;
                }
			} /*else*/
			if (isDclOperator(ch))
            {
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_DCL_OPERATOR);
				if (ch == '\'' && isDclWordstart(chNext)) {
					state = SCE_DCL_SYMBOL;
				}
			}
		}
        else
        if (state == SCE_DCL_SPECIAL)
        {
			if (!isDclWordstart(ch) || (radix != -1 && !IsADigit(ch, radix)))
            {
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, state);
				state = SCE_DCL_DEFAULT;
			}
			if (isDclOperator(ch))
            {
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_DCL_OPERATOR);
				if (ch == '\'' && isDclWordstart(chNext))
                {
					state = SCE_DCL_SYMBOL;
				}
			}
		}
        else
		if (state == SCE_DCL_COMMENT)
        {
			if (atEOL)
            {
                lineState |= stateInComment;
                if ((i - 1) >= 0)
                    styler.ColourTo(i - 1, SCE_DCL_COMMENT);
			    state = SCE_DCL_DEFAULT;
			}
		}
        else
        if (state == SCE_DCL_MULTI_COMMENT)
        {
			if (ch == '*' && chNext == '/')
            {
                lineState |= stateInBlockComment;
                styler.ColourTo(++i, SCE_DCL_COMMENT);
                state = SCE_DCL_DEFAULT;
                chNext = styler.SafeGetCharAt(++i);
			}
		}
        else
        if (state == SCE_DCL_STRING)
        {
			if (ch == '\\')
            {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\')
                {
					++i;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			}
            else
            if (ch == '\"')
            {
				styler.ColourTo(i, state);
				state = SCE_DCL_DEFAULT;
			}
		}

		if (atEOL)
        {
            if (state == SCE_DCL_MULTI_COMMENT)
                lineState |= stateInBlockComment;

            if (lineState != styler.GetLineState(lineCurrent))
                styler.SetLineState(lineCurrent, lineState);

            lineState  = styler.GetLineState(++lineCurrent);
            lineState &= (~stateLineMask); // clear old function line states
        }
    }

    if (lineState != styler.GetLineState(lineCurrent))
        styler.SetLineState(lineCurrent, lineState);

    styler.ColourTo(lengthDoc - 1, state);
}

static void FoldDclDoc(unsigned int startPos, int length, int /* initStyle */, WordList *[],
                       Accessor &styler)
{
    int lengthDoc = (int)(startPos + length);
    int lineCurrent = styler.GetLine(startPos);
    int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
    int levelCurrent = levelPrev;

    char ch     = 0, chNext = styler[startPos];
    int  style  = 0;
    bool atEOL  = false;

    int lineState = styler.GetLineState(lineCurrent);
    lineState    &= (~stateLineMask); // clear old function line states

    for (int i = (int)startPos; i < lengthDoc; ++i)
    {
        ch     = chNext;
        chNext = styler.SafeGetCharAt(i + 1);
        style  = styler.StyleAt(i);

        atEOL  = (ch == '\r' && chNext != '\n') || (ch == '\n');

        if (style == SCE_DCL_OPERATOR)
        {
            if (ch == ':')  // check for next word is 'dialog'
                lineState |= parseDialog(styler, i + 1, lengthDoc) ? stateIdDialog : 0;
            else
			if (ch == '{')
				++levelCurrent;
            else
            if (ch == '}')
				--levelCurrent;
		}
        else
		if (atEOL)
        {
			int lev = levelPrev;

            if (levelCurrent > levelPrev)
				lev |= SC_FOLDLEVELHEADERFLAG;

            if (lev != styler.LevelAt(lineCurrent))
				styler.SetLevel(lineCurrent, lev);

			levelPrev = levelCurrent;

            if (lineState != styler.GetLineState(lineCurrent))
                styler.SetLineState(lineCurrent, lineState);

            lineState  = styler.GetLineState(++lineCurrent);
            lineState &= (~stateLineMask); // clear old function line states
        }
	}

    if (lineState != styler.GetLineState(lineCurrent))
        styler.SetLineState(lineCurrent, lineState);

    // Fill in the real level of the next line, keeping the current flags as they will be filled in later
	int flagsNext = styler.LevelAt(lineCurrent) & ~SC_FOLDLEVELNUMBERMASK;
	styler.SetLevel(lineCurrent, levelPrev | flagsNext);
}

static const char * const dclWordListDesc[] = {
	"Functions and special operators",
	"Keywords",
	0
};

static bool registerDclLexer()
{
    static LexerModule lmDCL(SCLEX_DCL, ColouriseDclDoc, "DCL", FoldDclDoc, dclWordListDesc);
    Catalogue::AddLexerModule(&lmDCL);
    return true;
}
static const bool dummy = registerDclLexer();


#else

#define SCE_LISP_CHARACTER 29
#define SCE_LISP_MACRO 30
#define SCE_LISP_MACRO_DISPATCH 31

static inline bool isLispoperator(char ch) {
	if (IsASCII(ch) && isalnum(ch))
		return false;
	if (ch == '\'' || ch == '`' || ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}')
		return true;
	return false;
}

static inline bool isLispwordstart(char ch) {
	return IsASCII(ch) && ch != ';'  && !isspacechar(ch) && !isLispoperator(ch) &&
		ch != '\n' && ch != '\r' &&  ch != '\"';
}


static void classifyWordLisp(Sci_PositionU start, Sci_PositionU end, WordList &keywords, WordList &keywords_kw, Accessor &styler) {
	assert(end >= start);
	char s[100];
	Sci_PositionU i;
	bool digit_flag = true;
	for (i = 0; (i < end - start + 1) && (i < 99); i++) {
		s[i] = styler[start + i];
		s[i + 1] = '\0';
		if (!isdigit(s[i]) && (s[i] != '.')) digit_flag = false;
	}
	char chAttr = SCE_LISP_IDENTIFIER;

	if(digit_flag) chAttr = SCE_LISP_NUMBER;
	else {
		if (keywords.InList(s)) {
			chAttr = SCE_LISP_KEYWORD;
		} else if (keywords_kw.InList(s)) {
			chAttr = SCE_LISP_KEYWORD_KW;
		} else if ((s[0] == '*' && s[i-1] == '*') ||
			   (s[0] == '+' && s[i-1] == '+')) {
			chAttr = SCE_LISP_SPECIAL;
		}
	}
	styler.ColourTo(end, chAttr);
	return;
}


static void ColouriseLispDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, WordList *keywordlists[],
                            Accessor &styler) {

	WordList &keywords = *keywordlists[0];
	WordList &keywords_kw = *keywordlists[1];

	styler.StartAt(startPos);

	int state = initStyle, radix = -1;
	char chNext = styler[startPos];
	Sci_PositionU lengthDoc = startPos + length;
	styler.StartSegment(startPos);
	for (Sci_PositionU int i = startPos; i < lengthDoc; ++i)
    {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (styler.IsLeadByte(ch))
        {
			chNext = styler.SafeGetCharAt(i + 2);
			i += 1;
			continue;
		}

		if (state == SCE_LISP_DEFAULT) {
			if (ch == '#') {
				styler.ColourTo(i - 1, state);
				radix = -1;
				state = SCE_LISP_MACRO_DISPATCH;
			}
            else
            if (ch == ':' && isLispwordstart(chNext)) {
				styler.ColourTo(i - 1, state);
				state = SCE_LISP_SYMBOL;
			}
            else
            if (isLispwordstart(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_LISP_IDENTIFIER;
			}
			else
            if (ch == ';') {
				styler.ColourTo(i - 1, state);
				state = SCE_LISP_COMMENT;
			}
			else
            if (isLispoperator(ch)) // || ch=='\'') {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_LISP_OPERATOR);
if (ch=='\'' && isLispwordstart(chNext))
					state = SCE_LISP_SYMBOL;
			}
			else
            if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				state = SCE_LISP_STRING;
			}
		}
        else
        if (state == SCE_LISP_IDENTIFIER || state == SCE_LISP_SYMBOL)
        {
			if (!isLispwordstart(ch))
            {
				if (state == SCE_LISP_IDENTIFIER)
                {
					classifyWordLisp(styler.GetStartSegment(), i - 1, keywords, keywords_kw, styler);
				}
                else
                {
					styler.ColourTo(i - 1, state);
				}
				state = SCE_LISP_DEFAULT;
			} /*else*/
			if (isLispoperator(ch)) // || ch=='\'')
            {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_LISP_OPERATOR);
if (ch=='\'' && isLispwordstart(chNext))
					state = SCE_LISP_SYMBOL;
			}
		}
        else
        if (state == SCE_LISP_MACRO_DISPATCH)
        {
			if (!(IsASCII(ch) && isdigit(ch)))
            {
				if (ch != 'r' && ch != 'R' && (i - styler.GetStartSegment()) > 1) {
					state = SCE_LISP_DEFAULT;
				} else {
					switch (ch) {
						case '|': state = SCE_LISP_MULTI_COMMENT; break;
						case 'o':
						case 'O': radix = 8; state = SCE_LISP_MACRO; break;
						case 'x':
						case 'X': radix = 16; state = SCE_LISP_MACRO; break;
						case 'b':
						case 'B': radix = 2; state = SCE_LISP_MACRO; break;
						case '\\': state = SCE_LISP_CHARACTER; break;
						case ':':
						case '-':
						case '+': state = SCE_LISP_MACRO; break;
						case '\'': if (isLispwordstart(chNext)) {
								   state = SCE_LISP_SPECIAL;
							   } else {
								   styler.ColourTo(i - 1, SCE_LISP_DEFAULT);
								   styler.ColourTo(i, SCE_LISP_OPERATOR);
								   state = SCE_LISP_DEFAULT;
							   }
							   break;
						default: if (isLispoperator(ch)) {
								 styler.ColourTo(i - 1, SCE_LISP_DEFAULT);
								 styler.ColourTo(i, SCE_LISP_OPERATOR);
							 }
							 state = SCE_LISP_DEFAULT;
							 break;
					}
				}
			}
		}
        else
        if (state == SCE_LISP_MACRO)
        {
			if (isLispwordstart(ch) && (radix == -1 || IsADigit(ch, radix))) {
				state = SCE_LISP_SPECIAL;
			} else {
				state = SCE_LISP_DEFAULT;
			}
		}
        else
        if (state == SCE_LISP_CHARACTER)
        {
			if (isLispoperator(ch)) {
				styler.ColourTo(i, SCE_LISP_SPECIAL);
				state = SCE_LISP_DEFAULT;
			} else if (isLispwordstart(ch)) {
				styler.ColourTo(i, SCE_LISP_SPECIAL);
				state = SCE_LISP_SPECIAL;
			} else {
				state = SCE_LISP_DEFAULT;
			}
		}
        else
        if (state == SCE_LISP_SPECIAL)
        {
			if (!isLispwordstart(ch) || (radix != -1 && !IsADigit(ch, radix))) {
				styler.ColourTo(i - 1, state);
				state = SCE_LISP_DEFAULT;
			}
			if (isLispoperator(ch)) // || ch=='\'') {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_LISP_OPERATOR);
if (ch=='\'' && isLispwordstart(chNext))
					state = SCE_LISP_SYMBOL;
			}
		}
        else
        {
			if (state == SCE_LISP_COMMENT)
            {
				if (atEOL) {
					styler.ColourTo(i - 1, state);
					state = SCE_LISP_DEFAULT;
				}
			}
            else
            if (state == SCE_LISP_MULTI_COMMENT)
            {
				if (ch == '|' && chNext == '#') {
					++i;
					chNext = styler.SafeGetCharAt(i + 1);
					styler.ColourTo(i, state);
					state = SCE_LISP_DEFAULT;
				}
			}
            else if (state == SCE_LISP_STRING)
            {
				if (ch == '\\') {
					if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
						i++;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\"') {
					styler.ColourTo(i, state);
					state = SCE_LISP_DEFAULT;
				}
			}
		}

	}
	styler.ColourTo(lengthDoc - 1, state);
}

static void FoldLispDoc(Sci_PositionU startPos, Sci_Position length, int /* initStyle */, WordList *[],
                            Accessor &styler) {
	Sci_PositionU lengthDoc = startPos + length;
	int visibleChars = 0;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;
	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	for (Sci_PositionU i = startPos; i < lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		int style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		if (style == SCE_LISP_OPERATOR) {
			if (ch == '(' || ch == '[' || ch == '{') {
				levelCurrent++;
			} else if (ch == ')' || ch == ']' || ch == '}') {
				levelCurrent--;
			}
		}
		if (atEOL) {
			int lev = levelPrev;
			if (visibleChars == 0)
				lev |= SC_FOLDLEVELWHITEFLAG;
			if ((levelCurrent > levelPrev) && (visibleChars > 0))
				lev |= SC_FOLDLEVELHEADERFLAG;
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}
			lineCurrent++;
			levelPrev = levelCurrent;
			visibleChars = 0;
		}
		if (!isspacechar(ch))
			visibleChars++;
	}
	// Fill in the real level of the next line, keeping the current flags as they will be filled in later
	int flagsNext = styler.LevelAt(lineCurrent) & ~SC_FOLDLEVELNUMBERMASK;
	styler.SetLevel(lineCurrent, levelPrev | flagsNext);
}

#endif

static const char * const lispWordListDesc[] = {
	"Functions and special operators",
	"Keywords",
	"User Functions",
	0
};

LexerModule lmLISP(SCLEX_LISP, ColouriseLispDoc, "lisp", FoldLispDoc, lispWordListDesc);
