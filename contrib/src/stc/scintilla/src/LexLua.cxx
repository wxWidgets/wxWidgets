// Scintilla source code edit control
/** @file LexLua.cxx
 ** Lexer for Lua language.
 **
 ** Written by Paul Winwood.
 ** Folder by Alexey Yutkin.
 ** Modified by Marcos E. Wurzius & Philippe Lhoste
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
#include "StyleContext.h"
#include "KeyWords.h"
#include "Scintilla.h"
#include "SciLexer.h"

static inline bool IsAWordChar(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '_' || ch == '.');
}

static inline bool IsAWordStart(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '_');
}

static inline bool IsANumberChar(const int ch) {
	// Not exactly following number definition (several dots are seen as OK, etc.)
	// but probably enough in most cases.
	return (ch < 0x80) &&
	        (isdigit(ch) || toupper(ch) == 'E' ||
             ch == '.' || ch == '-' || ch == '+');
}

static inline bool IsLuaOperator(int ch) {
	if (ch >= 0x80 || isalnum(ch)) {
		return false;
	}
	// '.' left out as it is used to make up numbers
	if (ch == '*' || ch == '/' || ch == '-' || ch == '+' ||
		ch == '(' || ch == ')' || ch == '=' ||
		ch == '{' || ch == '}' || ch == '~' ||
		ch == '[' || ch == ']' || ch == ';' ||
		ch == '<' || ch == '>' || ch == ',' ||
		ch == '.' || ch == '^' || ch == '%' || ch == ':') {
		return true;
	}
	return false;
}

static void ColouriseLuaDoc(
	unsigned int startPos,
	int length,
	int initStyle,
	WordList *keywordlists[],
	Accessor &styler) {

	WordList &keywords = *keywordlists[0];
	WordList &keywords2 = *keywordlists[1];
	WordList &keywords3 = *keywordlists[2];
	WordList &keywords4 = *keywordlists[3];
	WordList &keywords5 = *keywordlists[4];
	WordList &keywords6 = *keywordlists[5];
	WordList &keywords7 = *keywordlists[6];
	WordList &keywords8 = *keywordlists[7];

	int currentLine = styler.GetLine(startPos);
	// Initialize the literal string [[ ... ]] nesting level, if we are inside such a string.
	int literalStringLevel = 0;
	if (initStyle == SCE_LUA_LITERALSTRING) {
		literalStringLevel = styler.GetLineState(currentLine - 1);
	}
	// Initialize the block comment --[[ ... ]] nesting level, if we are inside such a comment
	int blockCommentLevel = 0;
	if (initStyle == SCE_LUA_COMMENT) {
		blockCommentLevel = styler.GetLineState(currentLine - 1);
	}

	// Do not leak onto next line
	if (initStyle == SCE_LUA_STRINGEOL) {
		initStyle = SCE_LUA_DEFAULT;
	}

	StyleContext sc(startPos, length, initStyle, styler);
	if (startPos == 0 && sc.ch == '#') {
		// shbang line: # is a comment only if first char of the script
		sc.SetState(SCE_LUA_COMMENTLINE);
	}
	for (; sc.More(); sc.Forward()) {
		if (sc.atLineEnd) {
			// Update the line state, so it can be seen by next line
			currentLine = styler.GetLine(sc.currentPos);
			switch (sc.state) {
			case SCE_LUA_LITERALSTRING:
				// Inside a literal string, we set the line state
				styler.SetLineState(currentLine, literalStringLevel);
				break;
			case SCE_LUA_COMMENT: 	// Block comment
				// Inside a block comment, we set the line state
				styler.SetLineState(currentLine, blockCommentLevel);
				break;
			default:
				// Reset the line state
				styler.SetLineState(currentLine, 0);
				break;
			}
		}
		if (sc.atLineStart && (sc.state == SCE_LUA_STRING)) {
			// Prevent SCE_LUA_STRINGEOL from leaking back to previous line
			sc.SetState(SCE_LUA_STRING);
		}

		// Handle string line continuation
		if ((sc.state == SCE_LUA_STRING || sc.state == SCE_LUA_CHARACTER) &&
		        sc.ch == '\\') {
			if (sc.chNext == '\n' || sc.chNext == '\r') {
				sc.Forward();
				if (sc.ch == '\r' && sc.chNext == '\n') {
					sc.Forward();
				}
				continue;
			}
		}

		// Determine if the current state should terminate.
		if (sc.state == SCE_LUA_OPERATOR) {
			sc.SetState(SCE_LUA_DEFAULT);
		} else if (sc.state == SCE_LUA_NUMBER) {
			// We stop the number definition on non-numerical non-dot non-eE non-sign char
			if (!IsANumberChar(sc.ch)) {
				sc.SetState(SCE_LUA_DEFAULT);
			}
		} else if (sc.state == SCE_LUA_IDENTIFIER) {
			if (!IsAWordChar(sc.ch) || sc.Match('.', '.')) {
				char s[100];
				sc.GetCurrent(s, sizeof(s));
				if (keywords.InList(s)) {
					sc.ChangeState(SCE_LUA_WORD);
				} else if (keywords2.InList(s)) {
					sc.ChangeState(SCE_LUA_WORD2);
				} else if (keywords3.InList(s)) {
					sc.ChangeState(SCE_LUA_WORD3);
				} else if (keywords4.InList(s)) {
					sc.ChangeState(SCE_LUA_WORD4);
				} else if (keywords5.InList(s)) {
					sc.ChangeState(SCE_LUA_WORD5);
				} else if (keywords6.InList(s)) {
					sc.ChangeState(SCE_LUA_WORD6);
				} else if (keywords6.InList(s)) {
					sc.ChangeState(SCE_LUA_WORD6);
				} else if (keywords7.InList(s)) {
					sc.ChangeState(SCE_LUA_WORD7);
				} else if (keywords8.InList(s)) {
					sc.ChangeState(SCE_LUA_WORD8);
				}
				sc.SetState(SCE_LUA_DEFAULT);
			}
		} else if (sc.state == SCE_LUA_COMMENTLINE ) {
			if (sc.atLineEnd) {
				sc.SetState(SCE_LUA_DEFAULT);
			}
		} else if (sc.state == SCE_LUA_PREPROCESSOR ) {
			if (sc.atLineEnd) {
				sc.SetState(SCE_LUA_DEFAULT);
			}
		} else if (sc.state == SCE_LUA_STRING) {
			if (sc.ch == '\\') {
				if (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\') {
					sc.Forward();
				}
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_LUA_DEFAULT);
			} else if (sc.atLineEnd) {
				sc.ChangeState(SCE_LUA_STRINGEOL);
				sc.ForwardSetState(SCE_LUA_DEFAULT);
			}
		} else if (sc.state == SCE_LUA_CHARACTER) {
			if (sc.ch == '\\') {
				if (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\') {
					sc.Forward();
				}
			} else if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_LUA_DEFAULT);
			} else if (sc.atLineEnd) {
				sc.ChangeState(SCE_LUA_STRINGEOL);
				sc.ForwardSetState(SCE_LUA_DEFAULT);
			}
		} else if (sc.state == SCE_LUA_LITERALSTRING) {
			if (sc.Match('[', '[')) {
				literalStringLevel++;
				sc.Forward();
				sc.SetState(SCE_LUA_LITERALSTRING);
			} else if (sc.Match(']', ']') && literalStringLevel > 0) {
				literalStringLevel--;
				sc.Forward();
				if (literalStringLevel == 0) {
					sc.ForwardSetState(SCE_LUA_DEFAULT);
				}
			}
		} else if (sc.state == SCE_LUA_COMMENT) {	// Lua 5.0's block comment
			if (sc.Match('[', '[')) {
				blockCommentLevel++;
				sc.Forward();
			} else if (sc.Match(']', ']') && blockCommentLevel > 0) {
				blockCommentLevel--;
				sc.Forward();
				if (blockCommentLevel == 0) {
					sc.ForwardSetState(SCE_LUA_DEFAULT);
				}
			}
		}

		// Determine if a new state should be entered.
		if (sc.state == SCE_LUA_DEFAULT) {
			if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_LUA_NUMBER);
			} else if (IsAWordStart(sc.ch)) {
				sc.SetState(SCE_LUA_IDENTIFIER);
			} else if (sc.Match('\"')) {
				sc.SetState(SCE_LUA_STRING);
			} else if (sc.Match('\'')) {
				sc.SetState(SCE_LUA_CHARACTER);
			} else if (sc.Match('[', '[')) {
				literalStringLevel = 1;
				sc.SetState(SCE_LUA_LITERALSTRING);
				sc.Forward();
			} else if (sc.Match("--[[")) {	// Lua 5.0's block comment
				blockCommentLevel = 1;
				sc.SetState(SCE_LUA_COMMENT);
				sc.Forward(3);
			} else if (sc.Match('-', '-')) {
				sc.SetState(SCE_LUA_COMMENTLINE);
				sc.Forward();
			} else if (sc.atLineStart && sc.Match('$')) {
				sc.SetState(SCE_LUA_PREPROCESSOR);	// Obsolete since Lua 4.0, but still in old code
			} else if (IsLuaOperator(static_cast<char>(sc.ch))) {
				sc.SetState(SCE_LUA_OPERATOR);
			}
		}
	}
	sc.Complete();
}

static void FoldLuaDoc(unsigned int startPos, int length, int /* initStyle */, WordList *[],
                       Accessor &styler) {
	unsigned int lengthDoc = startPos + length;
	int visibleChars = 0;
	int lineCurrent = styler.GetLine(startPos);
	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;
	char chNext = styler[startPos];
	bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;
	int styleNext = styler.StyleAt(startPos);
	char s[10];

	for (unsigned int i = startPos; i < lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		int style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		if (style == SCE_LUA_WORD) {
			if (ch == 'i' || ch == 'd' || ch == 'f' || ch == 'e') {
				for (unsigned int j = 0; j < 8; j++) {
					if (!iswordchar(styler[i + j])) {
						break;
					}
					s[j] = styler[i + j];
					s[j + 1] = '\0';
				}

				if ((strcmp(s, "if") == 0) || (strcmp(s, "do") == 0) || (strcmp(s, "function") == 0)) {
					levelCurrent++;
				}
				if ((strcmp(s, "end") == 0) || (strcmp(s, "elseif") == 0)) {
					levelCurrent--;
				}
			}
		} else if (style == SCE_LUA_OPERATOR) {
			if (ch == '{' || ch == '(') {
				levelCurrent++;
			} else if (ch == '}' || ch == ')') {
				levelCurrent--;
			}
		}

		if (atEOL) {
			int lev = levelPrev;
			if (visibleChars == 0 && foldCompact) {
				lev |= SC_FOLDLEVELWHITEFLAG;
			}
			if ((levelCurrent > levelPrev) && (visibleChars > 0)) {
				lev |= SC_FOLDLEVELHEADERFLAG;
			}
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}
			lineCurrent++;
			levelPrev = levelCurrent;
			visibleChars = 0;
		}
		if (!isspacechar(ch)) {
			visibleChars++;
		}
	}
	// Fill in the real level of the next line, keeping the current flags as they will be filled in later

	int flagsNext = styler.LevelAt(lineCurrent) & ~SC_FOLDLEVELNUMBERMASK;
	styler.SetLevel(lineCurrent, levelPrev | flagsNext);
}

static const char * const luaWordListDesc[] = {
	"Keywords",
	"Basic functions",
	"String, (table) & math functions",
	"(coroutines), I/O & system facilities",
	"XXX",
	"XXX",
	0
};

LexerModule lmLua(SCLEX_LUA, ColouriseLuaDoc, "lua", FoldLuaDoc, luaWordListDesc);
