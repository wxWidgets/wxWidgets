// Scintilla source code edit control
/** @file LexLua.cxx
 ** Lexer for Lua language.
 **
 ** Written by Paul Winwood.
 ** Folder by Alexey Yutkin.
 **/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <fcntl.h>

#include "Platform.h"

#include "PropSet.h"
#include "Accessor.h"
#include "KeyWords.h"
#include "Scintilla.h"
#include "SciLexer.h"

inline bool isLuaOperator(char ch) {
	if (isalnum(ch))
		return false;
	// '.' left out as it is used to make up numbers
	if (ch == '*' || ch == '/' || ch == '-' || ch == '+' ||
	        ch == '(' || ch == ')' || ch == '=' ||
	        ch == '{' || ch == '}' || ch == '~' ||
	        ch == '[' || ch == ']' || ch == ';' ||
	        ch == '<' || ch == '>' || ch == ',' ||
	        ch == '.' || ch == '^' || ch == '%' || ch == ':')
		return true;
	return false;
}

static void classifyWordLua(unsigned int start,
                            unsigned int end,
                            WordList	&keywords,
                            Accessor	&styler) {
	char s[100];
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.');

	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = styler[start + i];
		s[i + 1] = '\0';
	}

	char chAttr = SCE_LUA_IDENTIFIER;

	if (wordIsNumber)
		chAttr = SCE_LUA_NUMBER;
	else {
		if (keywords.InList(s)) {
			chAttr = SCE_LUA_WORD;
		}
	}
	styler.ColourTo(end, chAttr);
}

static void ColouriseLuaDoc(unsigned int startPos,
                            int length,
                            int initStyle,
                            WordList *keywordlists[],
                            Accessor &styler) {

	WordList &keywords = *keywordlists[0];

	styler.StartAt(startPos);
	styler.GetLine(startPos);

	int state = initStyle;
	char chPrev = ' ';
	char chNext = styler[startPos];
	unsigned int lengthDoc = startPos + length;
	bool firstChar = true;

	/* Must initialize the literalString level, if we are inside such a string.
	 * Note: this isn't enough, because literal strings can be nested,
	 * we should go back to see at what level we are...
	 */
	int literalString = (initStyle == SCE_LUA_LITERALSTRING) ? 1 : 0;

	styler.StartSegment(startPos);
	for (unsigned int i = startPos; i <= lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if (styler.IsLeadByte(ch)) {
			chNext = styler.SafeGetCharAt(i + 2);
			chPrev = ' ';
			i += 1;
			continue;
		}

		if (state == SCE_LUA_STRINGEOL) {
			if (ch != '\r' && ch != '\n') {
				styler.ColourTo(i - 1, state);
				state = SCE_LUA_DEFAULT;
			}
		}

		if (state == SCE_LUA_LITERALSTRING && ch == '[' && chNext == '[') {
			literalString++;
		} else if (state == SCE_LUA_DEFAULT) {
			if (ch == '-' && chNext == '-') {
				styler.ColourTo(i - 1, state);
				state = SCE_LUA_COMMENTLINE;
			} else if (ch == '[' && chNext == '[') {
				state = SCE_LUA_LITERALSTRING;
				literalString = 1;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				state = SCE_LUA_STRING;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, state);
				state = SCE_LUA_CHARACTER;
			} else if (ch == '$' && firstChar) {
				styler.ColourTo(i - 1, state);
				state = SCE_LUA_PREPROCESSOR;
			} else if (ch == '#' && firstChar)	// Should be only on the first line of the file! Cannot be tested here
			{
				styler.ColourTo(i - 1, state);
				state = SCE_LUA_COMMENTLINE;
			} else if (isLuaOperator(ch)) {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_LUA_OPERATOR);
			} else if (iswordstart(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_LUA_WORD;
			}
		} else if (state == SCE_LUA_WORD) {
			if (!iswordchar(ch)) {
				classifyWordLua(styler.GetStartSegment(), i - 1, keywords, styler);
				state = SCE_LUA_DEFAULT;
				if (ch == '[' && chNext == '[') {
					literalString = 1;
					state = SCE_LUA_LITERALSTRING;
				} else if (ch == '-' && chNext == '-') {
					state = SCE_LUA_COMMENTLINE;
				} else if (ch == '\"') {
					state = SCE_LUA_STRING;
				} else if (ch == '\'') {
					state = SCE_LUA_CHARACTER;
				} else if (ch == '$' && firstChar) {
					state = SCE_LUA_PREPROCESSOR;
				} else if (isLuaOperator(ch)) {
					styler.ColourTo(i, SCE_LUA_OPERATOR);
				}
			} else if (ch == '.' && chNext == '.') {
				classifyWordLua(styler.GetStartSegment(), i - 1, keywords, styler);
				styler.ColourTo(i, SCE_LUA_OPERATOR);
				state = SCE_LUA_DEFAULT;
			}
		} else {
			if (state == SCE_LUA_LITERALSTRING) {
				if (ch == ']' && (chPrev == ']') && (--literalString == 0)) {
					styler.ColourTo(i, state);
					state = SCE_LUA_DEFAULT;
				}
			} else if (state == SCE_LUA_PREPROCESSOR) {
				if ((ch == '\r' || ch == '\n') && (chPrev != '\\')) {
					styler.ColourTo(i - 1, state);
					state = SCE_LUA_DEFAULT;
				}
			} else if (state == SCE_LUA_COMMENTLINE) {
				if (ch == '\r' || ch == '\n') {
					styler.ColourTo(i - 1, state);
					state = SCE_LUA_DEFAULT;
				}
			} else if (state == SCE_LUA_STRING) {
				if ((ch == '\r' || ch == '\n') && (chPrev != '\\')) {
					styler.ColourTo(i - 1, state);
					state = SCE_LUA_STRINGEOL;
				} else if (ch == '\\') {
					if (chNext == '\"' || chNext == '\\') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\"') {
					styler.ColourTo(i, state);
					state = SCE_LUA_DEFAULT;
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (state == SCE_LUA_CHARACTER) {
				if ((ch == '\r' || ch == '\n') && (chPrev != '\\')) {
					styler.ColourTo(i - 1, state);
					state = SCE_LUA_STRINGEOL;
				} else if (ch == '\\') {
					if (chNext == '\'' || chNext == '\\') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\'') {
					styler.ColourTo(i, state);
					state = SCE_LUA_DEFAULT;
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			}

			if (state == SCE_LUA_DEFAULT) {
				if (ch == '-' && chNext == '-') {
					state = SCE_LUA_COMMENTLINE;
				} else if (ch == '\"') {
					state = SCE_LUA_STRING;
				} else if (ch == '\'') {
					state = SCE_LUA_CHARACTER;
				} else if (ch == '$' && firstChar) {
					state = SCE_LUA_PREPROCESSOR;
				} else if (iswordstart(ch)) {
					state = SCE_LUA_WORD;
				} else if (isLuaOperator(ch)) {
					styler.ColourTo(i, SCE_LUA_OPERATOR);
				}
			}
		}
		chPrev = ch;
		firstChar = (ch == '\r' || ch == '\n');
	}
	styler.ColourTo(lengthDoc - 1, state);
}

static void FoldLuaDoc(unsigned int startPos, int length, int /* initStyle */, WordList *[],
                       Accessor &styler) {
	unsigned int lengthDoc = startPos + length;
	int visibleChars = 0;
	int lineCurrent = styler.GetLine(startPos);
	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;
	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	char s[10];
	for (unsigned int i = startPos; i < lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		int style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		if (style == SCE_LUA_WORD)
			if ( ch == 'e' || ch == 't' || ch == 'd' || ch == 'f') {
				for (unsigned int j = 0; j < 8; j++) {
					if (!iswordchar(styler[i + j])) break;
					s[j] = styler[i + j];
					s[j + 1] = '\0';
				}

				if ((strcmp(s, "then") == 0) || (strcmp(s, "do") == 0)
				        || (strcmp(s, "function") == 0))
					levelCurrent++;
				if (strcmp(s, "end") == 0) levelCurrent--;
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

LexerModule lmLua(SCLEX_LUA, ColouriseLuaDoc, "lua", FoldLuaDoc);
