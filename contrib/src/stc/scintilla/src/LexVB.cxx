// Scintilla source code edit control
/** @file LexVB.cxx
 ** Lexer for Visual Basic and VBScript.
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

static int classifyWordVB(unsigned int start, unsigned int end, WordList &keywords, Accessor &styler) {

	char s[100];
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.') ||
		(styler[start] == '&' && tolower(styler[start+1]) == 'h');
	unsigned int i;
	for (i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = static_cast<char>(tolower(styler[start + i]));
	}
	s[i] = '\0';
	char chAttr = SCE_C_DEFAULT;
	if (wordIsNumber)
		chAttr = SCE_C_NUMBER;
	else {
		if (strcmp(s, "rem") == 0)
			chAttr = SCE_C_COMMENTLINE;
		else if (keywords.InList(s))
			chAttr = SCE_C_WORD;
	}
	styler.ColourTo(end, chAttr);
	if (chAttr == SCE_C_COMMENTLINE)
		return SCE_C_COMMENTLINE;
	else
		return SCE_C_DEFAULT;
}

static bool IsVBComment(Accessor &styler, int pos, int len) {
	return len>0 && styler[pos]=='\'';
}

static void ColouriseVBDoc(unsigned int startPos, int length, int initStyle,
                           WordList *keywordlists[], Accessor &styler) {

	WordList &keywords = *keywordlists[0];

	styler.StartAt(startPos);

	int visibleChars = 0;
	int state = initStyle;
	char chNext = styler[startPos];
	styler.StartSegment(startPos);
	int lengthDoc = startPos + length;
	for (int i = startPos; i < lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if (styler.IsLeadByte(ch)) {
			chNext = styler.SafeGetCharAt(i + 2);
			i += 1;
			continue;
		}

		if (ch == '\r' || ch == '\n') {
			// End of line
			if (state == SCE_C_COMMENTLINE || state == SCE_C_PREPROCESSOR) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_DEFAULT;
			}
			visibleChars = 0;
		}
		if (!isspacechar(ch))
			visibleChars++;

		if (state == SCE_C_DEFAULT) {
			if (iswordstart(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_WORD;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_STRING;
			} else if (ch == '#' && visibleChars == 1) {
				// Preprocessor commands are alone on their line
				styler.ColourTo(i - 1, state);
				state = SCE_C_PREPROCESSOR;
			} else if (ch == '&' && tolower(chNext) == 'h') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_WORD;
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_C_OPERATOR);
			}
		} else if (state == SCE_C_WORD) {
			if (!iswordchar(ch)) {
				state = classifyWordVB(styler.GetStartSegment(), i - 1, keywords, styler);
				if (state == SCE_C_DEFAULT) {
					if (ch == '\'') {
						state = SCE_C_COMMENTLINE;
					} else if (ch == '\"') {
						state = SCE_C_STRING;
					} else if (isoperator(ch)) {
						styler.ColourTo(i - 1, state);
						styler.ColourTo(i, SCE_C_OPERATOR);
					}
				}
			}
		} else {
			if (state == SCE_C_STRING) {
				// VB doubles quotes to preserve them
				if (ch == '\"') {
					styler.ColourTo(i, state);
					state = SCE_C_DEFAULT;
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			}
			if (state == SCE_C_DEFAULT) {    // One of the above succeeded
				if (ch == '\'') {
					state = SCE_C_COMMENTLINE;
				} else if (ch == '\"') {
					state = SCE_C_STRING;
				} else if (iswordstart(ch)) {
					state = SCE_C_WORD;
				}
			}
		}
	}
	styler.ColourTo(lengthDoc, state);
}

static void FoldVBDoc(unsigned int startPos, int length, int initStyle,
						   WordList *[], Accessor &styler) {
	int lengthDoc = startPos + length;

	// Backtrack to previous line in case need to fix its fold status
	int lineCurrent = styler.GetLine(startPos);
	if (startPos > 0) {
		if (lineCurrent > 0) {
			lineCurrent--;
			startPos = styler.LineStart(lineCurrent);
			if (startPos == 0)
				initStyle = SCE_P_DEFAULT;
			else
				initStyle = styler.StyleAt(startPos-1);
		}
	}
	int state = initStyle & 31;
	int spaceFlags = 0;
	int indentCurrent = styler.IndentAmount(lineCurrent, &spaceFlags, IsVBComment);
	if ((state == SCE_P_TRIPLE) || (state == SCE_P_TRIPLEDOUBLE))
		indentCurrent |= SC_FOLDLEVELWHITEFLAG;
	char chNext = styler[startPos];
	for (int i = startPos; i < lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		int style = styler.StyleAt(i) & 31;

		if ((ch == '\r' && chNext != '\n') || (ch == '\n') || (i == lengthDoc)) {
			int lev = indentCurrent;
			int indentNext = styler.IndentAmount(lineCurrent + 1, &spaceFlags, IsVBComment);
			if ((style == SCE_P_TRIPLE) || (style== SCE_P_TRIPLEDOUBLE))
				indentNext |= SC_FOLDLEVELWHITEFLAG;
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

LexerModule lmVB(SCLEX_VB, ColouriseVBDoc, "vb", FoldVBDoc);
