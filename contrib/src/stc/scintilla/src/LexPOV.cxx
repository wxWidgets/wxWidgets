// Scintilla source code edit control
/** @file LexPOV.cxx
 ** Lexer for POV-Ray, based on lexer for C++.
 **/
// Copyright 2003 by Steven te Brinke <steven.t.b@zonnet.nl>
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

#define KEYWORD_BOXHEADER 1
#define KEYWORD_FOLDCONTRACTED 2

static inline bool IsAWordChar(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '.' || ch == '_');
}

static inline bool IsAWordStart(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '_');
}

static inline bool IsStateComment(const int state) {
	return ((state == SCE_POV_COMMENT) ||
	        (state == SCE_POV_COMMENTLINE) ||
	        (state == SCE_POV_COMMENTDOC));
}

static inline bool IsStateString(const int state) {
	return ((state == SCE_POV_STRING));
}

static void ColourisePOVDoc(unsigned int startPos, int length, int initStyle, WordList *keywordlists[],
                            Accessor &styler) {

	WordList &keywords = *keywordlists[0];
	WordList &keywords2 = *keywordlists[1];

	// Do not leak onto next line
	/*if (initStyle == SCE_POV_STRINGEOL)
		initStyle = SCE_POV_DEFAULT;*/

	StyleContext sc(startPos, length, initStyle, styler);

	bool caseSensitive = styler.GetPropertyInt("pov.case.sensitive", 1) != 0;

	for (; sc.More(); sc.Forward()) {

		/*if (sc.atLineStart && (sc.state == SCE_POV_STRING)) {
			// Prevent SCE_POV_STRINGEOL from leaking back to previous line
			sc.SetState(SCE_POV_STRING);
		}*/

		// Handle line continuation generically.
		if (sc.ch == '\\') {
			if (sc.chNext == '\n' || sc.chNext == '\r') {
				sc.Forward();
				if (sc.ch == '\r' && sc.chNext == '\n') {
					sc.Forward();
				}
				continue;
			}
		}

		// Determine if the current state should terminate.
		if (sc.state == SCE_POV_OPERATOR || sc.state == SCE_POV_BRACE) {
			sc.SetState(SCE_POV_DEFAULT);
		} else if (sc.state == SCE_POV_NUMBER) {
			if (!IsADigit(sc.ch) || sc.ch != '.') {
				sc.SetState(SCE_POV_DEFAULT);
			}
		} else if (sc.state == SCE_POV_IDENTIFIER) {
			if (!IsAWordChar(sc.ch) || (sc.ch == '.')) {
				char s[100];
				if (caseSensitive) {
					sc.GetCurrent(s, sizeof(s));
				} else {
					sc.GetCurrentLowered(s, sizeof(s));
				}
				if (keywords.InList(s)) {
					sc.ChangeState(SCE_POV_WORD);
				} else if (keywords2.InList(s)) {
					sc.ChangeState(SCE_POV_WORD2);
				}
				sc.SetState(SCE_POV_DEFAULT);
			}
		} else if (sc.state == SCE_POV_COMMENT) {
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_POV_DEFAULT);
			}
		} else if (sc.state == SCE_POV_COMMENTDOC) {
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_POV_DEFAULT);
			}
		} else if (sc.state == SCE_POV_COMMENTLINE) {
			if (sc.atLineEnd) {
				sc.SetState(SCE_POV_DEFAULT);
			}
		} else if (sc.state == SCE_POV_STRING) {
			if (sc.ch == '\\') {
				if (sc.chNext == '\"' || sc.chNext == '\\') {
					sc.Forward();
				}
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_POV_DEFAULT);
			}
		}

		// Determine if a new state should be entered.
		if (sc.state == SCE_POV_DEFAULT) {
			if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_POV_NUMBER);
			} else if (IsAWordStart(sc.ch) || (sc.ch == '#')) {
				sc.SetState(SCE_POV_IDENTIFIER);
			} else if (sc.Match('/', '*')) {
				sc.SetState(SCE_POV_COMMENT);
				sc.Forward();	// Eat the * so it isn't used for the end of the comment
			} else if (sc.Match('/', '/')) {
				sc.SetState(SCE_POV_COMMENTLINE);
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_POV_STRING);
				//} else if (isoperator(static_cast<char>(sc.ch))) {
			} else if (sc.ch == '+' || sc.ch == '-' || sc.ch == '*' || sc.ch == '/' || sc.ch == '=' || sc.ch == '<' || sc.ch == '>' || sc.ch == '&' || sc.ch == '|' || sc.ch == '!' || sc.ch == '?' || sc.ch == ':') {
				sc.SetState(SCE_POV_OPERATOR);
			} else if (sc.ch == '{' || sc.ch == '}') {
				sc.SetState(SCE_POV_BRACE);
			}
		}

	}
	sc.Complete();
}

static bool IsStreamCommentStyle(int style) {
	return style == SCE_POV_COMMENT ||
	       style == SCE_POV_COMMENTDOC;
}

static void FoldNoBoxPOVDoc(unsigned int startPos, int length, int initStyle,
                            Accessor &styler) {
	bool foldComment = styler.GetPropertyInt("fold.comment", 1) != 0;
	bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;
	unsigned int endPos = startPos + length;
	int visibleChars = 0;
	int lineCurrent = styler.GetLine(startPos);
	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;
	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;
	for (unsigned int i = startPos; i < endPos; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		if (foldComment && IsStreamCommentStyle(style)) {
			if (!IsStreamCommentStyle(stylePrev)) {
				levelCurrent++;
			} else if (!IsStreamCommentStyle(styleNext) && !atEOL) {
				// Comments don't end at end of line and the next character may be unstyled.
				levelCurrent--;
			}
		}
		if (style == SCE_POV_BRACE) {
			if (ch == '{') {
				levelCurrent++;
			} else if (ch == '}') {
				levelCurrent--;
			}
		}
		if (atEOL) {
			int lev = levelPrev;
			if (visibleChars == 0 && foldCompact)
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

static void FoldPOVDoc(unsigned int startPos, int length, int initStyle, WordList *[], Accessor &styler) {
	FoldNoBoxPOVDoc(startPos, length, initStyle, styler);
}

static const char * const POVWordLists[] = {
            "Primary keywords and identifiers",
            "Secondary keywords and identifiers",
            0,
        };

static void ColourisePOVDocSensitive(unsigned int startPos, int length, int initStyle, WordList *keywordlists[],
                                     Accessor &styler) {
	ColourisePOVDoc(startPos, length, initStyle, keywordlists, styler);
}

LexerModule lmPOV(SCLEX_POV, ColourisePOVDocSensitive, "pov", FoldPOVDoc, POVWordLists);
