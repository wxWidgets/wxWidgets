// SciTE - Scintilla based Text Editor
// LexCPP.cxx - lexer for C++, C, Java, and Javascript
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
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

static bool classifyWordCpp(unsigned int start, unsigned int end, WordList &keywords, StylingContext &styler) {
	char s[100];
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.');
	bool wordIsUUID = false;
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = styler[start + i];
		s[i + 1] = '\0';
	}
	char chAttr = SCE_C_IDENTIFIER;
	if (wordIsNumber)
		chAttr = SCE_C_NUMBER;
	else {
		if (keywords.InList(s)) {
			chAttr = SCE_C_WORD;
			wordIsUUID = strcmp(s, "uuid") == 0; 
		}
	}
	styler.ColourTo(end, chAttr);
	return wordIsUUID;
}

static void ColouriseCppDoc(unsigned int startPos, int length, int initStyle, WordList *keywordlists[], 
	StylingContext &styler) {
	
	WordList &keywords = *keywordlists[0];
	
	styler.StartAt(startPos);
	
	bool fold = styler.GetPropSet().GetInt("fold");
	int lineCurrent = styler.GetLine(startPos);
	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;

	int state = initStyle;
	char chPrev = ' ';
	char chNext = styler[startPos];
	unsigned int lengthDoc = startPos + length;
	int visChars = 0;
	styler.StartSegment(startPos);
	bool lastWordWasUUID = false;
	for (unsigned int i = startPos; i <= lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if ((fold) && ((ch == '\r' && chNext != '\n') || (ch == '\n'))) {
			int lev = levelPrev;
			if (visChars == 0)
				lev |= SC_FOLDLEVELWHITEFLAG;
			if ((levelCurrent > levelPrev) && (visChars > 0))
				lev |= SC_FOLDLEVELHEADERFLAG;
			styler.SetLevel(lineCurrent, lev);
			lineCurrent++;
			visChars = 0;
			levelPrev = levelCurrent;
		}
		if (!isspace(ch))
			visChars++;

		if (styler.IsLeadByte(ch)) {
			chNext = styler.SafeGetCharAt(i + 2);
			chPrev = ' ';
			i += 1;
			continue;
		}

		if (state == SCE_C_STRINGEOL) {
			if (ch != '\r' && ch != '\n') {
				styler.ColourTo(i-1, state);
				state = SCE_C_DEFAULT;
			}
		}
		if (state == SCE_C_DEFAULT) {
			if (iswordstart(ch)) {
				styler.ColourTo(i-1, state);
				if (lastWordWasUUID) {
					state = SCE_C_UUID;
					lastWordWasUUID = false;
				} else {
					state = SCE_C_WORD;
				}
			} else if (ch == '/' && chNext == '*') {
				styler.ColourTo(i-1, state);
				if (styler.SafeGetCharAt(i + 2) == '*')
					state = SCE_C_COMMENTDOC;
				else
					state = SCE_C_COMMENT;
			} else if (ch == '/' && chNext == '/') {
				styler.ColourTo(i-1, state);
				state = SCE_C_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i-1, state);
				state = SCE_C_STRING;
			} else if (ch == '\'') {
				styler.ColourTo(i-1, state);
				state = SCE_C_CHARACTER;
			} else if (ch == '#') {
				styler.ColourTo(i-1, state);
				state = SCE_C_PREPROCESSOR;
			} else if (isoperator(ch)) {
				styler.ColourTo(i-1, state);
				styler.ColourTo(i, SCE_C_OPERATOR);
				if ((ch == '{') || (ch == '}')) {
					levelCurrent += (ch == '{') ? 1 : -1;
				}
			}
		} else if (state == SCE_C_WORD) {
			if (!iswordchar(ch)) {
				lastWordWasUUID = classifyWordCpp(styler.GetStartSegment(), i - 1, keywords, styler);
				state = SCE_C_DEFAULT;
				if (ch == '/' && chNext == '*') {
					if (styler.SafeGetCharAt(i + 2) == '*')
						state = SCE_C_COMMENTDOC;
					else
						state = SCE_C_COMMENT;
				} else if (ch == '/' && chNext == '/') {
					state = SCE_C_COMMENTLINE;
				} else if (ch == '\"') {
					state = SCE_C_STRING;
				} else if (ch == '\'') {
					state = SCE_C_CHARACTER;
				} else if (ch == '#') {
					state = SCE_C_PREPROCESSOR;
				} else if (isoperator(ch)) {
					styler.ColourTo(i, SCE_C_OPERATOR);
					if ((ch == '{') || (ch == '}')) {
						levelCurrent += (ch == '{') ? 1 : -1;
					}
				}
			}
		} else {
			if (state == SCE_C_PREPROCESSOR) {
				if ((ch == '\r' || ch == '\n') && (chPrev != '\\')) {
					styler.ColourTo(i-1, state);
					state = SCE_C_DEFAULT;
				}
			} else if (state == SCE_C_COMMENT) {
				if (ch == '/' && chPrev == '*') {
					if (((i > styler.GetStartSegment() + 2) || (
						(initStyle == SCE_C_COMMENT) && 
						(styler.GetStartSegment() == static_cast<unsigned int>(startPos))))) {
						styler.ColourTo(i, state);
						state = SCE_C_DEFAULT;
					}
				}
			} else if (state == SCE_C_COMMENTDOC) {
				if (ch == '/' && chPrev == '*') {
					if (((i > styler.GetStartSegment() + 3) || (
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
				if ((ch == '\r' || ch == '\n') && (chPrev != '\\')) {
					styler.ColourTo(i-1, state);
					state = SCE_C_STRINGEOL;
				} else if (ch == '\\') {
					if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\"') {
					styler.ColourTo(i, state);
					state = SCE_C_DEFAULT;
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (state == SCE_C_CHARACTER) {
				if ((ch == '\r' || ch == '\n') && (chPrev != '\\')) {
					styler.ColourTo(i-1, state);
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
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (state == SCE_C_UUID) {
				if (ch == '\r' || ch == '\n' || ch == ')') {
					styler.ColourTo(i-1, state);
					state = SCE_C_DEFAULT;
				}
			}
			if (state == SCE_C_DEFAULT) {    // One of the above succeeded
				if (ch == '/' && chNext == '*') {
					if (styler.SafeGetCharAt(i + 2) == '*')
						state = SCE_C_COMMENTDOC;
					else
						state = SCE_C_COMMENT;
				} else if (ch == '/' && chNext == '/') {
					state = SCE_C_COMMENTLINE;
				} else if (ch == '\"') {
					state = SCE_C_STRING;
				} else if (ch == '\'') {
					state = SCE_C_CHARACTER;
				} else if (ch == '#') {
					state = SCE_C_PREPROCESSOR;
				} else if (iswordstart(ch)) {
					state = SCE_C_WORD;
				} else if (isoperator(ch)) {
					styler.ColourTo(i, SCE_C_OPERATOR);
					if ((ch == '{') || (ch == '}')) {
						levelCurrent += (ch == '{') ? 1 : -1;
					}
				}
			}
		}
		chPrev = ch;
	}
	styler.ColourTo(lengthDoc - 1, state);

	// Fill in the real level of the next line, keeping the current flags as they will be filled in later
	if (fold) {
		int flagsNext = styler.LevelAt(lineCurrent) & ~SC_FOLDLEVELNUMBERMASK;
		//styler.SetLevel(lineCurrent, levelCurrent | flagsNext);
		styler.SetLevel(lineCurrent, levelPrev | flagsNext);
		
	}
}

LexerModule lmCPP(SCLEX_CPP, ColouriseCppDoc);
