// Scintilla source code edit control
/** @file LexLisp.cxx
 ** Lexer for Lisp.
 ** Written by Alexey Yutkin.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#include "Platform.h"

#include "PropSet.h"
#include "Accessor.h"
#include "KeyWords.h"
#include "Scintilla.h"
#include "SciLexer.h"


static inline bool isLispoperator(char ch) {
	if (isascii(ch) && isalnum(ch))
		return false;
	if (ch == '\'' || ch == '(' || ch == ')' )
		return true;
	return false;
}

static inline bool isLispwordstart(char ch) {
	return isascii(ch) && ch != ';'  && !isspacechar(ch) && !isLispoperator(ch) &&
		ch != '\n' && ch != '\r' &&  ch != '\"';
}


static void classifyWordLisp(unsigned int start, unsigned int end, WordList &keywords, Accessor &styler) {
	PLATFORM_ASSERT(end >= start);
	char s[100];
	unsigned int i;
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
		}
	}
	styler.ColourTo(end, chAttr);
	return;
}


static void ColouriseLispDoc(unsigned int startPos, int length, int initStyle, WordList *keywordlists[],
                            Accessor &styler) {

	WordList &keywords = *keywordlists[0];

	styler.StartAt(startPos);

	int state = initStyle;
	if (state == SCE_LISP_STRINGEOL)	// Does not leak onto next line
		state = SCE_LISP_DEFAULT;
	char chPrev = ' ';
	char chNext = styler[startPos];
	unsigned int lengthDoc = startPos + length;
	styler.StartSegment(startPos);
	for (unsigned int i = startPos; i < lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		if (atEOL) {
			// Trigger on CR only (Mac style) or either on LF from CR+LF (Dos/Win) or on LF alone (Unix)
			// Avoid triggering two times on Dos/Win
			// End of line
			if (state == SCE_LISP_STRINGEOL) {
				styler.ColourTo(i, state);
				state = SCE_LISP_DEFAULT;
			}
		}

		if (styler.IsLeadByte(ch)) {
			chNext = styler.SafeGetCharAt(i + 2);
			chPrev = ' ';
			i += 1;
			continue;
		}

		if (state == SCE_LISP_DEFAULT) {
			if (isLispwordstart(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_LISP_IDENTIFIER;
			}
			else if (ch == ';') {
				styler.ColourTo(i - 1, state);
				state = SCE_LISP_COMMENT;
			}
			else if (isLispoperator(ch) || ch=='\'') {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_LISP_OPERATOR);
			}

			else if (ch == '\"') {
					state = SCE_LISP_STRING;
			}
		} else if (state == SCE_LISP_IDENTIFIER) {
			if (!isLispwordstart(ch)) {
				classifyWordLisp(styler.GetStartSegment(), i - 1, keywords, styler);
				state = SCE_LISP_DEFAULT;
			} /*else*/
			if (isLispoperator(ch) || ch=='\'') {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_LISP_OPERATOR);
			}

		} else {
			if (state == SCE_LISP_COMMENT) {
				if (atEOL) {
					styler.ColourTo(i - 1, state);
					state = SCE_LISP_DEFAULT;
				}
			} else if (state == SCE_LISP_STRING) {
				if (ch == '\\') {
					if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\"') {
					styler.ColourTo(i, state);
					state = SCE_LISP_DEFAULT;
				} else if ((chNext == '\r' || chNext == '\n') && (chPrev != '\\')) {
					styler.ColourTo(i - 1, SCE_LISP_STRINGEOL);
					state = SCE_LISP_STRINGEOL;
				}
			}
		}

		chPrev = ch;
	}
	styler.ColourTo(lengthDoc - 1, state);
}

static void FoldLispDoc(unsigned int startPos, int length, int /* initStyle */, WordList *[],
                            Accessor &styler) {
	unsigned int lengthDoc = startPos + length;
	int visibleChars = 0;
	int lineCurrent = styler.GetLine(startPos);
	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;
	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	for (unsigned int i = startPos; i < lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		int style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		if (style == SCE_LISP_OPERATOR) {
			if (ch == '(') {
				levelCurrent++;
			} else if (ch == ')') {
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

static const char * const lispWordListDesc[] = {
	"Keywords",
	0
};

LexerModule lmLISP(SCLEX_LISP, ColouriseLispDoc, "lisp", FoldLispDoc, lispWordListDesc);
