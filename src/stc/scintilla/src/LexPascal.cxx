// Scintilla source code edit control
/** @file LexPascal.cxx
 ** Lexer for Pascal.
 ** Written by Laurent le Tynevez
 **/

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


static int classifyWordPascal(unsigned int start, unsigned int end, WordList &keywords, Accessor &styler) {
	char s[100];
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = static_cast<char>(tolower(styler[start + i]));
		s[i + 1] = '\0';
	}
	int lev= 0;
	char chAttr = SCE_C_IDENTIFIER;
	if (isdigit(s[0]) || (s[0] == '.')){
		chAttr = SCE_C_NUMBER;
	}
	else {
		if (keywords.InList(s)) {
			chAttr = SCE_C_WORD;
			if (strcmp(s, "begin") == 0 || strcmp(s, "object") == 0)
				lev=1;
			else if (strcmp(s, "end") == 0)
				lev=-1;
		}
	}
	styler.ColourTo(end, chAttr);
	return lev;
}

static void ColourisePascalDoc(unsigned int startPos, int length, int initStyle, WordList *keywordlists[],
	Accessor &styler) {
	WordList &keywords = *keywordlists[0];

	styler.StartAt(startPos);

	bool fold = styler.GetPropertyInt("fold");
	int lineCurrent = styler.GetLine(startPos);
	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;

	int state = initStyle;
	if (state == SCE_C_STRINGEOL)	// Does not leak onto next line
		state = SCE_C_DEFAULT;
	char chPrev = ' ';
	char chNext = styler[startPos];
	unsigned int lengthDoc = startPos + length;
	int visibleChars = 0;
	styler.StartSegment(startPos);
	for (unsigned int i = startPos; i < lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if ((ch == '\r' && chNext != '\n') || (ch == '\n')) {
			// Trigger on CR only (Mac style) or either on LF from CR+LF (Dos/Win) or on LF alone (Unix)
			// Avoid triggering two times on Dos/Win
			// End of line
			if (state == SCE_C_STRINGEOL) {
				styler.ColourTo(i, state);
				state = SCE_C_DEFAULT;
			}
			if (fold) {
				int lev = levelPrev;
				if (visibleChars == 0)
					lev |= SC_FOLDLEVELWHITEFLAG;
				if ((levelCurrent > levelPrev) && (visibleChars > 0))
					lev |= SC_FOLDLEVELHEADERFLAG;
				styler.SetLevel(lineCurrent, lev);
				lineCurrent++;
				levelPrev = levelCurrent;
			}
			visibleChars = 0;
		}
		if (!isspacechar(ch))
			visibleChars++;

		if (styler.IsLeadByte(ch)) {
			chNext = styler.SafeGetCharAt(i + 2);
			chPrev = ' ';
			i += 1;
			continue;
		}

		if (state == SCE_C_DEFAULT) {
			if (iswordstart(ch) || (ch == '@')) {
				styler.ColourTo(i-1, state);
				state = SCE_C_IDENTIFIER;
			} else if (ch == '{' && chNext != '$' && chNext != '&') {
				styler.ColourTo(i-1, state);
				state = SCE_C_COMMENT;
			} else if (ch == '(' && chNext == '*'
						&& styler.SafeGetCharAt(i + 2) != '$'
						&& styler.SafeGetCharAt(i + 2) != '&') {
				styler.ColourTo(i-1, state);
				state = SCE_C_COMMENTDOC;
			} else if (ch == '/' && chNext == '/') {
				styler.ColourTo(i-1, state);
				state = SCE_C_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i-1, state);
				state = SCE_C_STRING;
			} else if (ch == '\'') {
				styler.ColourTo(i-1, state);
				state = SCE_C_CHARACTER;
			} else if (ch == '{' && (chNext == '$' || chNext=='&') && visibleChars == 1) {
				styler.ColourTo(i-1, state);
				state = SCE_C_PREPROCESSOR;
			} else if (isoperator(ch)) {
				styler.ColourTo(i-1, state);
				styler.ColourTo(i, SCE_C_OPERATOR);

			}
		} else if (state == SCE_C_IDENTIFIER) {
			if (!iswordchar(ch)) {
				int levelChange = classifyWordPascal(styler.GetStartSegment(), i - 1, keywords, styler);
				state = SCE_C_DEFAULT;
				chNext = styler.SafeGetCharAt(i + 1);
				if (ch == '{' && chNext != '$' && chNext != '&') {
					state = SCE_C_COMMENT;
				} else if (ch == '(' && chNext == '*'
						&& styler.SafeGetCharAt(i + 2) != '$'
						&& styler.SafeGetCharAt(i + 2) != '&') {
					styler.ColourTo(i-1, state);
					state = SCE_C_COMMENTDOC;
				} else if (ch == '/' && chNext == '/') {
					state = SCE_C_COMMENTLINE;
				} else if (ch == '\"') {
					state = SCE_C_STRING;
				} else if (ch == '\'') {
					state = SCE_C_CHARACTER;
				} else if (isoperator(ch)) {
					styler.ColourTo(i, SCE_C_OPERATOR);
				}
				levelCurrent+=levelChange;
			}
		} else {
			if (state == SCE_C_PREPROCESSOR) {
				if (ch=='}'){
					styler.ColourTo(i, state);
					state = SCE_C_DEFAULT;
				} else {
					if ((ch == '\r' || ch == '\n') && !(chPrev == '\\' || chPrev == '\r')) {
						styler.ColourTo(i-1, state);
						state = SCE_C_DEFAULT;
					}
				}
			} else if (state == SCE_C_COMMENT) {
				if (ch == '}' ) {
					styler.ColourTo(i, state);
					state = SCE_C_DEFAULT;
				}
			} else if (state == SCE_C_COMMENTDOC) {
				if (ch == ')' && chPrev == '*') {
					if (((i > styler.GetStartSegment() + 2) || (
						(initStyle == SCE_C_COMMENTDOC) &&
						(styler.GetStartSegment() == static_cast<unsigned int>(startPos))))) {
							styler.ColourTo(i, state);
							state = SCE_C_DEFAULT;
					}
				}
			} else if (state == SCE_C_COMMENTLINE) {
				if (ch == '\r' || ch == '\n') {
					styler.ColourTo(i-1, state);
					state = SCE_C_DEFAULT;
				}
			} else if (state == SCE_C_STRING) {
				if (ch == '\\') {
					if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\"') {
					styler.ColourTo(i, state);
					state = SCE_C_DEFAULT;
				} else if (chNext == '\r' || chNext == '\n') {
					styler.ColourTo(i-1, SCE_C_STRINGEOL);
					state = SCE_C_STRINGEOL;
				}
			} else if (state == SCE_C_CHARACTER) {
				if ((ch == '\r' || ch == '\n') && (chPrev != '\\')) {
					styler.ColourTo(i-1, SCE_C_STRINGEOL);
					state = SCE_C_STRINGEOL;
				} else if (ch == '\\') {
					if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\'') {
					styler.ColourTo(i, state);
					state = SCE_C_DEFAULT;
				}
			}
		}
		chPrev = ch;
	}
	styler.ColourTo(lengthDoc - 1, state);

	// Fill in the real level of the next line, keeping the current flags as they will be filled in later
	if (fold) {
		int flagsNext = styler.LevelAt(lineCurrent) & ~SC_FOLDLEVELNUMBERMASK;
		styler.SetLevel(lineCurrent, levelPrev | flagsNext);
	}
}

LexerModule lmPascal(SCLEX_PASCAL, ColourisePascalDoc, "pascal");
