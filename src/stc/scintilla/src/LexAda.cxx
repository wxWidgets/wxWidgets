// SciTE - Scintilla based Text Editor
// LexAda.cxx - lexer for Ada95
// by Tahir Karaca <tahir@bigfoot.de>
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

inline void classifyWordAda(unsigned int start, unsigned int end,
	WordList &keywords, Accessor &styler) {

	static const unsigned KEWORD_LEN_MAX = 30;

	char wordLower[KEWORD_LEN_MAX + 1];
	unsigned i;
	for(i = 0; ( i < KEWORD_LEN_MAX ) && ( i < end - start + 1 ); i++) {
		wordLower[i] = static_cast<char>(tolower(styler[start + i]));		
	}
	wordLower[i] = '\0';
		
//	int levelChange = 0;
	char chAttr = SCE_ADA_IDENTIFIER;
	if (keywords.InList(wordLower)) {
		chAttr = SCE_ADA_WORD;

// Folding doesn't work this way since the semantics of some keywords depends
// on the current context.
// E.g. - "cond1 and THEN cond2" <-> "if ... THEN ..."		
//      - "procedure X IS ... end X;" <-> "procedure X IS new Y;"
//		if (strcmp(wordLower, "is") == 0 || strcmp(wordLower, "then") == 0)
//			levelChange=1;
//		else if (strcmp(wordLower, "end") == 0)
//			levelChange=-1;
	}
	styler.ColourTo(end, chAttr);
	
//	return levelChange;
}


inline bool isAdaOperator(char ch) {
	
	if (ch == '&' || ch == '\'' || ch == '(' || ch == ')' ||
	        ch == '*' || ch == '+' || ch == ',' || ch == '-' ||
	        ch == '.' || ch == '/' || ch == ':' || ch == ';' ||
	        ch == '<' || ch == '=' || ch == '>')
		return true;
	return false;
}


inline void styleTokenBegin(char beginChar, unsigned int pos, int &state,
	Accessor &styler) {
		
	if (isalpha(beginChar)) {
		styler.ColourTo(pos-1, state);
		state = SCE_ADA_IDENTIFIER;
	} else if (isdigit(beginChar)) {
		styler.ColourTo(pos-1, state);
		state = SCE_ADA_NUMBER;
	} else if (beginChar == '-' && styler.SafeGetCharAt(pos + 1) == '-') {
		styler.ColourTo(pos-1, state);
		state = SCE_ADA_COMMENT;
	} else if (beginChar == '\"') {
		styler.ColourTo(pos-1, state);
		state = SCE_ADA_STRING;
	} else if (beginChar == '\'' && styler.SafeGetCharAt(pos + 2) == '\'') {
		styler.ColourTo(pos-1, state);
		state = SCE_ADA_CHARACTER;
	} else if (isAdaOperator(beginChar)) {
		styler.ColourTo(pos-1, state);
		styler.ColourTo(pos, SCE_ADA_OPERATOR);
	}
}


static void ColouriseAdaDoc(unsigned int startPos, int length, int initStyle,
	WordList *keywordlists[], Accessor &styler) {
	
	WordList &keywords = *keywordlists[0];
	
	styler.StartAt(startPos);
	
//	bool fold = styler.GetPropertyInt("fold");
//	int lineCurrent = styler.GetLine(startPos);
//	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
//	int levelCurrent = levelPrev;

	int state = initStyle;
	if (state == SCE_ADA_STRINGEOL)	// Does not leak onto next line
		state = SCE_ADA_DEFAULT;
	char chNext = styler[startPos];
	const unsigned int lengthDoc = startPos + length;
	//int visibleChars = 0;
	styler.StartSegment(startPos);
	for (unsigned int i = startPos; i < lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if ((ch == '\r' && chNext != '\n') || (ch == '\n')) {
			// Trigger on CR only (Mac style) or either on LF from CR+LF (Dos/Win) or on LF alone (Unix)
			// Avoid triggering two times on Dos/Win
			if (state == SCE_ADA_STRINGEOL) {
				styler.ColourTo(i, state);
				state = SCE_ADA_DEFAULT;
			}
//			if (fold) {
//				int lev = levelPrev;
//				if (visibleChars == 0)
//					lev |= SC_FOLDLEVELWHITEFLAG;
//				if ((levelCurrent > levelPrev) && (visibleChars > 0))
//					lev |= SC_FOLDLEVELHEADERFLAG;
//				styler.SetLevel(lineCurrent, lev);
//				lineCurrent++;
//				levelPrev = levelCurrent;
//			}
			//visibleChars = 0;
		}
		//if (!isspacechar(ch))
		//	visibleChars++;

		if (styler.IsLeadByte(ch)) {
			chNext = styler.SafeGetCharAt(i + 2);
			i += 1;
			continue;
		}

		if (state == SCE_ADA_DEFAULT) {
			styleTokenBegin(ch, i, state, styler);
		} else if (state == SCE_ADA_IDENTIFIER) {
			if (!iswordchar(ch)) {
				classifyWordAda(styler.GetStartSegment(),
								i - 1,
								keywords,
								styler);
				state = SCE_ADA_DEFAULT;
				styleTokenBegin(ch, i, state, styler);
			}
		} else if (state == SCE_ADA_COMMENT) {
			if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i-1, state);
				state = SCE_ADA_DEFAULT;
			}
		} else if (state == SCE_ADA_STRING) {
			if (ch == '"' ) {
				if( chNext == '"' ) {
					i++;
					chNext = styler.SafeGetCharAt(i + 1);
				} else {					
					styler.ColourTo(i, state);
					state = SCE_ADA_DEFAULT;
				}
			} else if (chNext == '\r' || chNext == '\n') {
				styler.ColourTo(i-1, SCE_ADA_STRINGEOL);
				state = SCE_ADA_STRINGEOL;
			}
		} else if (state == SCE_ADA_CHARACTER) {
			if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i-1, SCE_ADA_STRINGEOL);
				state = SCE_ADA_STRINGEOL;
			} else if (ch == '\'' && styler.SafeGetCharAt(i - 2) == '\'') {
				styler.ColourTo(i, state);
				state = SCE_ADA_DEFAULT;
			}
		} else if (state == SCE_ADA_NUMBER) {
			if ( !( isdigit(ch) || ch == '.' || ch == '_' || ch == '#'
				    || ch == 'A' || ch == 'B' || ch == 'C' || ch == 'D'
					|| ch == 'E' || ch == 'F'
					|| ch == 'a' || ch == 'b' || ch == 'c' || ch == 'd'
					|| ch == 'e' || ch == 'f' ) ) {
				styler.ColourTo(i-1, SCE_ADA_NUMBER);
				state = SCE_ADA_DEFAULT;
				styleTokenBegin(ch, i, state, styler);
			}
		}

	}
	styler.ColourTo(lengthDoc - 1, state);

//	// Fill in the real level of the next line, keeping the current flags as they will be filled in later
//	if (fold) {
//		int flagsNext = styler.LevelAt(lineCurrent) & ~SC_FOLDLEVELNUMBERMASK;
//		styler.SetLevel(lineCurrent, levelPrev | flagsNext);
//	}
}

LexerModule lmAda(SCLEX_ADA, ColouriseAdaDoc, "ada");
