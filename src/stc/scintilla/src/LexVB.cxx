// Scintilla source code edit control
/** @file LexVB.cxx
 ** Lexer for Visual Basic and VBScript.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
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

static bool IsVBComment(Accessor &styler, int pos, int len) {
	return len>0 && styler[pos]=='\'';
}

static inline bool IsTypeCharacter(int ch) {
	return ch == '%' || ch == '&' || ch == '@' || ch == '!' || ch == '#' || ch == '$';
}

// Extended to accept accented characters
static inline bool IsAWordChar(int ch) {
	return ch >= 0x80 ||
	       (isalnum(ch) || ch == '.' || ch == '_');
}

static inline bool IsAWordStart(int ch) {
	return ch >= 0x80 ||
	       (isalnum(ch) || ch == '_');
}

static inline bool IsADateCharacter(const int ch) {
	return (ch < 0x80) &&
		(isalnum(ch) || ch == '|' || ch == '-' || ch == '/' || ch == ':' || ch == ' ' || ch == '\t');
}

static void ColouriseVBDoc(unsigned int startPos, int length, int initStyle,
                           WordList *keywordlists[], Accessor &styler, bool vbScriptSyntax) {

	WordList &keywords = *keywordlists[0];
	WordList &keywords2 = *keywordlists[1];
	WordList &keywords3 = *keywordlists[2];
	WordList &keywords4 = *keywordlists[3];

	styler.StartAt(startPos);

	int visibleChars = 0;

	StyleContext sc(startPos, length, initStyle, styler);

	for (; sc.More(); sc.Forward()) {

		if (sc.state == SCE_B_OPERATOR) {
			sc.SetState(SCE_B_DEFAULT);
		} else if (sc.state == SCE_B_IDENTIFIER) {
			if (!IsAWordChar(sc.ch)) {
				// In Basic (except VBScript), a variable name or a function name
				// can end with a special character indicating the type of the value
				// held or returned.
				bool skipType = false;
				if (!vbScriptSyntax && IsTypeCharacter(sc.ch)) {
					sc.Forward();	// Skip it
					skipType = true;
				}
				if (sc.ch == ']') {
					sc.Forward();
				}
				char s[100];
				sc.GetCurrentLowered(s, sizeof(s));
				if (skipType) {
					s[strlen(s) - 1] = '\0';
				}
				if (strcmp(s, "rem") == 0) {
					sc.ChangeState(SCE_B_COMMENT);
				} else {
					if (keywords.InList(s)) {
						sc.ChangeState(SCE_B_KEYWORD);
					} else if (keywords2.InList(s)) {
						sc.ChangeState(SCE_B_KEYWORD2);
					} else if (keywords3.InList(s)) {
						sc.ChangeState(SCE_B_KEYWORD3);
					} else if (keywords4.InList(s)) {
						sc.ChangeState(SCE_B_KEYWORD4);
					}	// Else, it is really an identifier...
					sc.SetState(SCE_B_DEFAULT);
				}
			}
		} else if (sc.state == SCE_B_NUMBER) {
			if (!IsAWordChar(sc.ch)) {
				sc.SetState(SCE_B_DEFAULT);
			}
		} else if (sc.state == SCE_B_STRING) {
			// VB doubles quotes to preserve them, so just end this string
			// state now as a following quote will start again
			if (sc.ch == '\"') {
				if (tolower(sc.chNext) == 'c') {
					sc.Forward();
				}
				sc.ForwardSetState(SCE_B_DEFAULT);
			} else if (sc.atLineEnd) {
				sc.ChangeState(SCE_B_STRINGEOL);
				sc.ForwardSetState(SCE_B_DEFAULT);
			}
		} else if (sc.state == SCE_B_COMMENT) {
			if (sc.atLineEnd) {
				sc.SetState(SCE_B_DEFAULT);
			}
		} else if (sc.state == SCE_B_PREPROCESSOR) {
			if (sc.atLineEnd) {
				sc.SetState(SCE_B_DEFAULT);
			}
		} else if (sc.state == SCE_B_DATE) {
			if (sc.ch == '#' || !IsADateCharacter(sc.chNext)) {
				sc.ForwardSetState(SCE_B_DEFAULT);
			}
		}

		if (sc.state == SCE_B_DEFAULT) {
			if (sc.ch == '\'') {
				sc.SetState(SCE_B_COMMENT);
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_B_STRING);
			} else if (sc.ch == '#' && visibleChars == 0) {
				// Preprocessor commands are alone on their line
				sc.SetState(SCE_B_PREPROCESSOR);
			} else if (sc.ch == '#') {
				int n = 1;
				int chSeek = ' ';
				while ((n < 100) && (chSeek == ' ' || chSeek == '\t')) {
					chSeek = sc.GetRelative(n);
					n++;
				}
				if (IsADigit(chSeek)) {
					sc.SetState(SCE_B_DATE);
				} else {
					sc.SetState(SCE_B_OPERATOR);
				}
			} else if (sc.ch == '&' && tolower(sc.chNext) == 'h') {
				sc.SetState(SCE_B_NUMBER);
			} else if (sc.ch == '&' && tolower(sc.chNext) == 'o') {
				sc.SetState(SCE_B_NUMBER);
			} else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_B_NUMBER);
			} else if (IsAWordStart(sc.ch) || (sc.ch == '[')) {
				sc.SetState(SCE_B_IDENTIFIER);
			} else if (isoperator(static_cast<char>(sc.ch)) || (sc.ch == '\\')) {
				sc.SetState(SCE_B_OPERATOR);
			}
		}

		if (sc.atLineEnd) {
			visibleChars = 0;
		}
		if (!IsASpace(sc.ch)) {
			visibleChars++;
		}
	}
	sc.Complete();
}

static void FoldVBDoc(unsigned int startPos, int length, int,
						   WordList *[], Accessor &styler) {
	int endPos = startPos + length;

	// Backtrack to previous line in case need to fix its fold status
	int lineCurrent = styler.GetLine(startPos);
	if (startPos > 0) {
		if (lineCurrent > 0) {
			lineCurrent--;
			startPos = styler.LineStart(lineCurrent);
		}
	}
	int spaceFlags = 0;
	int indentCurrent = styler.IndentAmount(lineCurrent, &spaceFlags, IsVBComment);
	char chNext = styler[startPos];
	for (int i = startPos; i < endPos; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if ((ch == '\r' && chNext != '\n') || (ch == '\n') || (i == endPos)) {
			int lev = indentCurrent;
			int indentNext = styler.IndentAmount(lineCurrent + 1, &spaceFlags, IsVBComment);
			if (!(indentCurrent & SC_FOLDLEVELWHITEFLAG)) {
				// Only non whitespace lines can be headers
				if ((indentCurrent & SC_FOLDLEVELNUMBERMASK) < (indentNext & SC_FOLDLEVELNUMBERMASK)) {
					lev |= SC_FOLDLEVELHEADERFLAG;
				} else if (indentNext & SC_FOLDLEVELWHITEFLAG) {
					// Line after is blank so check the next - maybe should continue further?
					int spaceFlags2 = 0;
					int indentNext2 = styler.IndentAmount(lineCurrent + 2, &spaceFlags2, IsVBComment);
					if ((indentCurrent & SC_FOLDLEVELNUMBERMASK) < (indentNext2 & SC_FOLDLEVELNUMBERMASK)) {
						lev |= SC_FOLDLEVELHEADERFLAG;
					}
				}
			}
			indentCurrent = indentNext;
			styler.SetLevel(lineCurrent, lev);
			lineCurrent++;
		}
	}
}

static void ColouriseVBNetDoc(unsigned int startPos, int length, int initStyle,
                           WordList *keywordlists[], Accessor &styler) {
	ColouriseVBDoc(startPos, length, initStyle, keywordlists, styler, false);
}

static void ColouriseVBScriptDoc(unsigned int startPos, int length, int initStyle,
                           WordList *keywordlists[], Accessor &styler) {
	ColouriseVBDoc(startPos, length, initStyle, keywordlists, styler, true);
}

static const char * const vbWordListDesc[] = {
	"Keywords",
	"user1",
	"user2",
	"user3",
	0
};

LexerModule lmVB(SCLEX_VB, ColouriseVBNetDoc, "vb", FoldVBDoc, vbWordListDesc);
LexerModule lmVBScript(SCLEX_VBSCRIPT, ColouriseVBScriptDoc, "vbscript", FoldVBDoc, vbWordListDesc);

