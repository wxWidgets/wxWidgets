// Scintilla source code edit control
/** @file LexCPP.cxx
 ** Lexer for C++, C, Java, and Javascript.
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
#include "StyleContext.h"
#include "KeyWords.h"
#include "Scintilla.h"
#include "SciLexer.h"

static bool IsOKBeforeRE(const int ch) {
	return (ch == '(') || (ch == '=') || (ch == ',');
}

inline bool IsAWordChar(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '.' || ch == '_');
}

inline bool IsAWordStart(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '_');
}

inline bool IsADoxygenChar(const int ch) {
	return (islower(ch) || ch == '$' || ch == '@' ||
		    ch == '\\' || ch == '&' || ch == '<' ||
			ch == '>' || ch == '#' || ch == '{' ||
			ch == '}' || ch == '[' || ch == ']');
}

inline bool IsStateComment(const int state) {
	return ((state == SCE_C_COMMENT) ||
		      (state == SCE_C_COMMENTLINE) ||
		      (state == SCE_C_COMMENTDOC) ||
		      (state == SCE_C_COMMENTDOCKEYWORD) ||
		      (state == SCE_C_COMMENTDOCKEYWORDERROR));
}

inline bool IsStateString(const int state) {
	return ((state == SCE_C_STRING) || (state == SCE_C_VERBATIM));
}

static void ColouriseCppDoc(unsigned int startPos, int length, int initStyle, WordList *keywordlists[],
                            Accessor &styler) {

	WordList &keywords = *keywordlists[0];
	WordList &keywords2 = *keywordlists[1];
	WordList &keywords3 = *keywordlists[2];

	bool stylingWithinPreprocessor = styler.GetPropertyInt("styling.within.preprocessor");

	// Do not leak onto next line
	if (initStyle == SCE_C_STRINGEOL)
		initStyle = SCE_C_DEFAULT;

	int chPrevNonWhite = ' ';
	int visibleChars = 0;
	int noDocChars = 0;
	bool lastWordWasUUID = false;

	StyleContext sc(startPos, length, initStyle, styler);

	for (; sc.More(); sc.Forward()) {
	
		// Handle line continuation generically.
		if (sc.ch == '\\') {
			if (sc.Match("\\\n")) {
				sc.Forward();
				sc.Forward();
				continue;
			}
			if (sc.Match("\\\r\n")) {
				sc.Forward();
				sc.Forward();
				sc.Forward();
				continue;
			}
		}

		// Determine if the current state should terminate.
		if (sc.state == SCE_C_OPERATOR) {
			sc.SetState(SCE_C_DEFAULT);
		} else if (sc.state == SCE_C_NUMBER) {
			if (!IsAWordChar(sc.ch)) {
				sc.SetState(SCE_C_DEFAULT);
			}
		} else if (sc.state == SCE_C_IDENTIFIER) {
			if (!IsAWordChar(sc.ch) || (sc.ch == '.')) {
				char s[100];
				sc.GetCurrent(s, sizeof(s));
				if (keywords.InList(s)) {
					lastWordWasUUID = strcmp(s, "uuid") == 0;
					sc.ChangeState(SCE_C_WORD);
				} else if (keywords2.InList(s)) {
					sc.ChangeState(SCE_C_WORD2);
				}
				sc.SetState(SCE_C_DEFAULT);
			}
		} else if (sc.state == SCE_C_PREPROCESSOR) {
			if (stylingWithinPreprocessor) {
				if (IsASpace(sc.ch)) {
					sc.SetState(SCE_C_DEFAULT);
				}
			} else {
				if (sc.atLineEnd) {
					sc.SetState(SCE_C_DEFAULT);
				}
			}
		} else if (sc.state == SCE_C_COMMENT) {
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_C_DEFAULT);
			}
		} else if (sc.state == SCE_C_COMMENTDOC) {
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_C_DEFAULT);
			} else if ((sc.ch == '@' || sc.ch == '\\') && (noDocChars == 0)) {
				sc.SetState(SCE_C_COMMENTDOCKEYWORD);
			} else if (sc.atLineEnd) {
				noDocChars = 0;
			} else if (!isspace(sc.ch) && (sc.ch != '*')) {
				noDocChars++;
			}
		} else if (sc.state == SCE_C_COMMENTLINE || sc.state == SCE_C_COMMENTLINEDOC) {
			if (sc.atLineEnd) {
				sc.SetState(SCE_C_DEFAULT);
				visibleChars = 0;
			}
		} else if (sc.state == SCE_C_COMMENTDOCKEYWORD) {
			if (sc.Match('*', '/')) {
				sc.ChangeState(SCE_C_COMMENTDOCKEYWORDERROR);
				sc.Forward();
				sc.ForwardSetState(SCE_C_DEFAULT);
			} else if (!IsADoxygenChar(sc.ch)) {
				char s[100];
				sc.GetCurrent(s, sizeof(s));
				if (!isspace(sc.ch) || !keywords3.InList(s+1)) {
					sc.ChangeState(SCE_C_COMMENTDOCKEYWORDERROR);
				}
				sc.SetState(SCE_C_COMMENTDOC);
			}
		} else if (sc.state == SCE_C_STRING) {
			if (sc.ch == '\\') {
				if (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\') {
					sc.Forward();
				}
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_C_DEFAULT);
			} else if (sc.atLineEnd) {
				sc.ChangeState(SCE_C_STRINGEOL);
				sc.ForwardSetState(SCE_C_DEFAULT);
				visibleChars = 0;
			}
		} else if (sc.state == SCE_C_CHARACTER) {
			if (sc.atLineEnd) {
				sc.ChangeState(SCE_C_STRINGEOL);
				sc.ForwardSetState(SCE_C_DEFAULT);
				visibleChars = 0;
			} else if (sc.ch == '\\') {
				if (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\') {
					sc.Forward();
				}
			} else if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_C_DEFAULT);
			}
		} else if (sc.state == SCE_C_REGEX) {
			if (sc.ch == '\r' || sc.ch == '\n' || sc.ch == '/') {
				sc.ForwardSetState(SCE_C_DEFAULT);
			} else if (sc.ch == '\\') {
				// Gobble up the quoted character
				if (sc.chNext == '\\' || sc.chNext == '/') {
					sc.Forward();
				}
			}
		} else if (sc.state == SCE_C_VERBATIM) {
			if (sc.ch == '\"') {
				if (sc.chNext == '\"') {
					sc.Forward();
				} else {
					sc.ForwardSetState(SCE_C_DEFAULT);
				}
			}
		} else if (sc.state == SCE_C_UUID) {
			if (sc.ch == '\r' || sc.ch == '\n' || sc.ch == ')') {
				sc.SetState(SCE_C_DEFAULT);
			}
		}

		// Determine if a new state should be entered.
		if (sc.state == SCE_C_DEFAULT) {
			if (sc.Match('@', '\"')) {
				sc.SetState(SCE_C_VERBATIM);
				sc.Forward();
			} else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				if (lastWordWasUUID) {
					sc.SetState(SCE_C_UUID);
					lastWordWasUUID = false;
				} else {
					sc.SetState(SCE_C_NUMBER);
				}
			} else if (IsAWordStart(sc.ch) || (sc.ch == '@')) {
				if (lastWordWasUUID) {
					sc.SetState(SCE_C_UUID);
					lastWordWasUUID = false;
				} else {
					sc.SetState(SCE_C_IDENTIFIER);
				}
			} else if (sc.Match('/', '*')) {
				if (sc.Match("/**") || sc.Match("/*!")) {	// Support of Qt/Doxygen doc. style
					noDocChars = 0;
					sc.SetState(SCE_C_COMMENTDOC);
				} else {
					sc.SetState(SCE_C_COMMENT);
				}
				sc.Forward();	// Eat the * so it isn't used for the end of the comment
			} else if (sc.Match('/', '/')) {
				if (sc.Match("///") || sc.Match("//!"))	// Support of Qt/Doxygen doc. style
					sc.SetState(SCE_C_COMMENTLINEDOC);
				else
					sc.SetState(SCE_C_COMMENTLINE);
			} else if (sc.ch == '/' && IsOKBeforeRE(chPrevNonWhite)) {
				sc.SetState(SCE_C_REGEX);
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_C_STRING);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_C_CHARACTER);
			} else if (sc.ch == '#' && visibleChars == 0) {
				// Preprocessor commands are alone on their line
				sc.SetState(SCE_C_PREPROCESSOR);
				// Skip whitespace between # and preprocessor word
				do {
					sc.Forward();
				} while ((sc.ch == ' ') && (sc.ch == '\t') && sc.More());
				if (sc.atLineEnd) {
					sc.SetState(SCE_C_DEFAULT);
				}
			} else if (isoperator(static_cast<char>(sc.ch))) {
				sc.SetState(SCE_C_OPERATOR);
			}
		}
		
		if (sc.atLineEnd) {
			// Reset states to begining of colourise so no surprises 
			// if different sets of lines lexed.
			chPrevNonWhite = ' ';
			visibleChars = 0;
			lastWordWasUUID = false;
		}
		if (!IsASpace(sc.ch)) {
			chPrevNonWhite = sc.ch;
			visibleChars++;
		}
	}
	sc.Complete();
}

static void FoldCppDoc(unsigned int startPos, int length, int initStyle, WordList *[],
                            Accessor &styler) {
	bool foldComment = styler.GetPropertyInt("fold.comment");
	bool foldCompact = styler.GetPropertyInt("fold.compact", 1);
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
		if (foldComment &&
			(style == SCE_C_COMMENT || style == SCE_C_COMMENTDOC)) {
			if (style != stylePrev) {
				levelCurrent++;
			} else if ((style != styleNext) && !atEOL) {
				// Comments don't end at end of line and the next character may be unstyled.
				levelCurrent--;
			}
		}
		if (style == SCE_C_OPERATOR) {
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

LexerModule lmCPP(SCLEX_CPP, ColouriseCppDoc, "cpp", FoldCppDoc);
LexerModule lmTCL(SCLEX_TCL, ColouriseCppDoc, "tcl", FoldCppDoc);
