// SciTE - Scintilla based Text Editor
// LexHTML.cxx - lexer for HTML
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

enum { eScriptNone, eScriptJS, eScriptVBS, eScriptPython };
static int segIsScriptingIndicator(StylingContext &styler, unsigned int start, unsigned int end, int prevValue) {
	char s[100];
	s[0] = '\0';
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = tolower(styler[start + i]);
		s[i + 1] = '\0';
	}
//Platform::DebugPrintf("Scripting indicator [%s]\n", s);
	if (strstr(s, "vbs"))
		return eScriptVBS;
	if (strstr(s, "pyth"))
		return eScriptPython;
	if (strstr(s, "javas"))
		return eScriptJS;
	if (strstr(s, "jscr"))
		return eScriptJS;
		
	return prevValue;
}

static void classifyAttribHTML(unsigned int start, unsigned int end, WordList &keywords, StylingContext &styler) {
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.') ||
	                    (styler[start] == '-') || (styler[start] == '#');
	char chAttr = SCE_H_ATTRIBUTEUNKNOWN;
	if (wordIsNumber) {
		chAttr = SCE_H_NUMBER;
	} else {
		char s[100];
		s[0] = '\0';
		for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
			s[i] = tolower(styler[start + i]);
			s[i + 1] = '\0';
		}
		if (keywords.InList(s))
			chAttr = SCE_H_ATTRIBUTE;
	}
	styler.ColourTo(end, chAttr);
}

static int classifyTagHTML(unsigned int start, unsigned int end,
                         WordList &keywords, StylingContext &styler) {
	char s[100];
	// Copy after the '<'
	unsigned int i = 0;
	for (unsigned int cPos=start; cPos <= end && i < 30; cPos++) {
		char ch = styler[cPos];
		if (ch != '<')
			s[i++] = tolower(ch);
	}
	s[i] = '\0';
	char chAttr = SCE_H_TAGUNKNOWN;
	if (s[0] == '!' && s[1] == '-' && s[2] == '-') {	//Comment
		chAttr = SCE_H_COMMENT;
	} else if (s[0] == '/') {	// Closing tag
		if (keywords.InList(s + 1))
			chAttr = SCE_H_TAG;
	} else {
		if (keywords.InList(s)) {
			chAttr = SCE_H_TAG;
			if (0 == strcmp(s, "script"))
				chAttr = SCE_H_SCRIPT;
		}
	}
	styler.ColourTo(end, chAttr);
	return chAttr;
}

static void classifyWordHTJS(unsigned int start, unsigned int end,
                             WordList &keywords, StylingContext &styler) {
	char s[100];
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.');
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = styler[start + i];
		s[i + 1] = '\0';
	}
	char chAttr = SCE_HJ_WORD;
	if (wordIsNumber)
		chAttr = SCE_HJ_NUMBER;
	else {
		if (keywords.InList(s))
			chAttr = SCE_HJ_KEYWORD;
	}
	styler.ColourTo(end, chAttr);
}

static void classifyWordHTJSA(unsigned int start, unsigned int end,
                             WordList &keywords, StylingContext &styler) {
	char s[100];
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.');
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = styler[start + i];
		s[i + 1] = '\0';
	}
	char chAttr = SCE_HJA_WORD;
	if (wordIsNumber)
		chAttr = SCE_HJA_NUMBER;
	else {
		if (keywords.InList(s))
			chAttr = SCE_HJA_KEYWORD;
	}
	styler.ColourTo(end, chAttr);
}

static int classifyWordHTVB(unsigned int start, unsigned int end, WordList &keywords, StylingContext &styler) {
	char s[100];
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.');
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = tolower(styler[start + i]);
		s[i + 1] = '\0';
	}
	char chAttr = SCE_HB_IDENTIFIER;
	if (wordIsNumber)
		chAttr = SCE_HB_NUMBER;
	else {
		if (keywords.InList(s)) {
			chAttr = SCE_HB_WORD;
			if (strcmp(s, "rem") == 0)
				chAttr = SCE_HB_COMMENTLINE;
		}
	}
	styler.ColourTo(end, chAttr);
	if (chAttr == SCE_HB_COMMENTLINE)
		return SCE_HB_COMMENTLINE;
	else
		return SCE_HB_DEFAULT;
}

static int classifyWordHTVBA(unsigned int start, unsigned int end, WordList &keywords, StylingContext &styler) {
	char s[100];
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.');
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = tolower(styler[start + i]);
		s[i + 1] = '\0';
	}
	char chAttr = SCE_HBA_IDENTIFIER;
	if (wordIsNumber)
		chAttr = SCE_HBA_NUMBER;
	else {
		if (keywords.InList(s)) {
			chAttr = SCE_HBA_WORD;
			if (strcmp(s, "rem") == 0)
				chAttr = SCE_HBA_COMMENTLINE;
		}
	}
	styler.ColourTo(end, chAttr);
	if (chAttr == SCE_HBA_COMMENTLINE)
		return SCE_HBA_COMMENTLINE;
	else
		return SCE_HBA_DEFAULT;
}

static void classifyWordHTPy(unsigned int start, unsigned int end, WordList &keywords, StylingContext &styler, char *prevWord) {
	char s[100];
	bool wordIsNumber = isdigit(styler[start]);
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = styler[start + i];
		s[i + 1] = '\0';
	}
	char chAttr = SCE_HP_IDENTIFIER;
	if (0 == strcmp(prevWord, "class"))
		chAttr = SCE_HP_CLASSNAME;
	else if (0 == strcmp(prevWord, "def"))
		chAttr = SCE_HP_DEFNAME;
	else if (wordIsNumber)
		chAttr = SCE_HP_NUMBER;
	else if (keywords.InList(s))
		chAttr = SCE_HP_WORD;
	styler.ColourTo(end, chAttr);
	strcpy(prevWord, s);
}

static void classifyWordHTPyA(unsigned int start, unsigned int end, WordList &keywords, StylingContext &styler, char *prevWord) {
	char s[100];
	bool wordIsNumber = isdigit(styler[start]);
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = styler[start + i];
		s[i + 1] = '\0';
	}
	char chAttr = SCE_HPA_IDENTIFIER;
	if (0 == strcmp(prevWord, "class"))
		chAttr = SCE_HPA_CLASSNAME;
	else if (0 == strcmp(prevWord, "def"))
		chAttr = SCE_HPA_DEFNAME;
	else if (wordIsNumber)
		chAttr = SCE_HPA_NUMBER;
	else if (keywords.InList(s))
		chAttr = SCE_HPA_WORD;
	styler.ColourTo(end, chAttr);
	strcpy(prevWord, s);
}

inline bool ishtmlwordchar(char ch) {
	return isalnum(ch) || ch == '.' || ch == '-' || ch == '_' || ch == ':' || ch == '!' || ch == '#';
}

static bool InTagState(int state) {
	return state == SCE_H_TAG || state == SCE_H_TAGUNKNOWN ||
		state == SCE_H_SCRIPT ||
		state == SCE_H_ATTRIBUTE || state == SCE_H_ATTRIBUTEUNKNOWN ||
		state == SCE_H_NUMBER || state == SCE_H_OTHER ||
		state == SCE_H_DOUBLESTRING || state == SCE_H_SINGLESTRING;
}

static bool isLineEnd(char ch) {
	return ch == '\r' || ch == '\n';
}

static void ColouriseHyperTextDoc(unsigned int startPos, int length, int initStyle, WordList *keywordlists[], 
	StylingContext &styler) {
	
	WordList &keywords=*keywordlists[0];
	WordList &keywords2=*keywordlists[1];
	WordList &keywords3=*keywordlists[2];
	WordList &keywords4=*keywordlists[3];
	
	// Lexer for HTML requires more lexical states (7 bits worth) than most lexers
	styler.StartAt(startPos, 127);
	bool lastTagWasScript = false;
	char prevWord[200];
	prevWord[0] = '\0';
	int scriptLanguage = eScriptJS;
	int state = initStyle;
	// If inside a tag, it may be a script tage, so reread from the start to ensure any language tas are seen
	if (InTagState(state)) {
		while ((startPos > 1) && (InTagState(styler.StyleAt(startPos - 1)))) {
			startPos--;
		}
		state = SCE_H_DEFAULT;
	}
	styler.StartAt(startPos, 127);

	int lineState = eScriptVBS;
	int lineCurrent = styler.GetLine(startPos);
	if (lineCurrent > 0)
		lineState = styler.GetLineState(lineCurrent);
	int defaultScript = lineState &0xff;
	int beforeASP = (lineState >> 8) &0xff;
	int inASP = (lineState >> 16) &0xff;
	
	char chPrev = ' ';
	char chPrev2 = ' ';
	styler.StartSegment(startPos);
	int lengthDoc = startPos + length;
	for (int i = startPos; i <= lengthDoc; i++) {
		char ch = styler[i];
		char chNext = styler.SafeGetCharAt(i + 1);
		char chNext2 = styler.SafeGetCharAt(i + 2);

		if (styler.IsLeadByte(ch)) {
			chPrev2 = ' ';
			chPrev = ' ';
			i += 1;
			continue;
		}

		if ((ch == '\r' && chNext != '\n') || (ch == '\n')) {
			// New line -> record any line state onto /next/ line
			lineCurrent++;
			styler.SetLineState(lineCurrent, 
				defaultScript | (beforeASP << 8) | (inASP << 16));
		}
		
		// Handle ASP even within other constructs as it is a preprocessor
		if ((ch == '<') && (chNext == '%')) {
			beforeASP = state;
			styler.ColourTo(i - 1, state);
			if (chNext2 == '@') {
				styler.ColourTo(i + 2, SCE_H_ASP);
				state = SCE_H_ASPAT;
				i+=2;
			} else {
				if (defaultScript == eScriptVBS)
					state = SCE_HBA_START;
				else if  (defaultScript == eScriptPython)
				 	state = SCE_HPA_START;
				else
					state = SCE_HJA_START;
				if (chNext2 == '=') {
					styler.ColourTo(i + 2, SCE_H_ASP);
					i+=2;
				} else {
					styler.ColourTo(i + 1, SCE_H_ASP);
					i++;
				}
			}
			inASP = 1;
			continue;
		}
		if (inASP && (ch == '%') && (chNext == '>')) {
			if (state == SCE_H_ASPAT)
				defaultScript = segIsScriptingIndicator(styler, styler.GetStartSegment(), i-1, defaultScript);
			// Bounce out of any ASP mode
			if (state == SCE_HJA_WORD) {
				classifyWordHTJSA(styler.GetStartSegment(), i - 1, keywords2, styler);
			} else if (state == SCE_HBA_WORD) {
				classifyWordHTVBA(styler.GetStartSegment(), i - 1, keywords3, styler);
			} else if (state == SCE_HPA_WORD) {
				classifyWordHTPyA(styler.GetStartSegment(), i - 1, keywords4, styler, prevWord);
			} else {
				styler.ColourTo(i - 1, state);
			}
			//if (state == SCE_H_ASPAT)
			//	styler.ColourTo(i+1, SCE_H_ASPAT);
			//else
				styler.ColourTo(i+1, SCE_H_ASP);
			i++;
			state = beforeASP;
			beforeASP = SCE_H_DEFAULT;
			inASP = 0;
			continue;
		}
		
		if (state == SCE_H_DEFAULT) {
			if (ch == '<') {
				styler.ColourTo(i - 1, state);
				state = SCE_H_TAGUNKNOWN;
				if (chNext == '?') {
					styler.ColourTo(i + 1, SCE_H_XMLSTART);
					i++;
					ch = chNext;
				}
			} else if (ch == '&') {
				styler.ColourTo(i - 1, SCE_H_DEFAULT);
				state = SCE_H_ENTITY;
			}
		} else if (state == SCE_H_COMMENT) {
			if ((ch == '>') && (chPrev == '-')) {
				styler.ColourTo(i, state);
				state = SCE_H_DEFAULT;
			}
		} else if (state == SCE_H_ENTITY) {
			if (ch == ';') {
				styler.ColourTo(i, state);
				state = SCE_H_DEFAULT;
			}
		} else if (state == SCE_H_TAGUNKNOWN) {
			if (!ishtmlwordchar(ch) && ch != '/' && ch != '-') {
				int eClass = classifyTagHTML(styler.GetStartSegment(), i - 1, keywords, styler);
				lastTagWasScript = eClass == SCE_H_SCRIPT;
				if (lastTagWasScript) {
					scriptLanguage = eScriptJS;
					eClass = SCE_H_TAG;
				}
				if (ch == '>') {
					styler.ColourTo(i, SCE_H_TAG);
					if (lastTagWasScript) {
						if (scriptLanguage == eScriptVBS)
							state = SCE_HB_START;
						else if  (scriptLanguage == eScriptPython)
						 	state = SCE_HP_START;
						else
							state = SCE_HJ_START;
					} else {
						state = SCE_H_DEFAULT;
					}
				} else {
					if (eClass == SCE_H_COMMENT) {
						state = SCE_H_COMMENT;
					} else {
						state = SCE_H_OTHER;
					}
				}
			}
		} else if (state == SCE_H_ATTRIBUTE) {
			if (!ishtmlwordchar(ch) && ch != '/' && ch != '-') {
				if (lastTagWasScript)
					scriptLanguage = segIsScriptingIndicator(styler, styler.GetStartSegment(), i-1, scriptLanguage);
				classifyAttribHTML(styler.GetStartSegment(), i - 1, keywords, styler);
				if (ch == '>') {
					styler.ColourTo(i, SCE_H_TAG);
					if (lastTagWasScript) {
						if (scriptLanguage == eScriptVBS)
							state = SCE_HB_START;
						else if  (scriptLanguage == eScriptPython)
						 	state = SCE_HP_START;
						else
							state = SCE_HJ_START;
					} else {
						state = SCE_H_DEFAULT;
					}
				} else {
					state = SCE_H_OTHER;
				}
			}
		} else if (state == SCE_H_ASP) {
			if ((ch == '>') && (chPrev == '%')) {
				styler.ColourTo(i, state);
				state = SCE_H_DEFAULT;
			}
		} else if (state == SCE_H_ASPAT) {
			if ((ch == '>') && (chPrev == '%')) {
				styler.ColourTo(i, state);
				state = SCE_H_DEFAULT;
			}
		} else if (state == SCE_H_OTHER) {
			if (ch == '>') {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_H_TAG);
				if (lastTagWasScript) {
					if (scriptLanguage == eScriptVBS)
						state = SCE_HB_START;
					else if  (scriptLanguage == eScriptPython)
					 	state = SCE_HP_START;
					else
						state = SCE_HJ_START;
				} else {
					state = SCE_H_DEFAULT;
				}
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				state = SCE_H_DOUBLESTRING;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, state);
				state = SCE_H_SINGLESTRING;
			} else if (ch == '/' && chNext == '>') {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i + 1, SCE_H_TAGEND);
				i++;
				ch = chNext;
				state = SCE_H_DEFAULT;
			} else if (ch == '?' && chNext == '>') {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i + 1, SCE_H_XMLEND);
				i++;
				ch = chNext;
				state = SCE_H_DEFAULT;
			} else if (ishtmlwordchar(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_H_ATTRIBUTE;
			}
		} else if (state == SCE_H_DOUBLESTRING) {
			if (ch == '\"') {
				if (lastTagWasScript)
					scriptLanguage = segIsScriptingIndicator(styler, styler.GetStartSegment(), i, scriptLanguage);
				styler.ColourTo(i, SCE_H_DOUBLESTRING);
				state = SCE_H_OTHER;
			}
		} else if (state == SCE_H_SINGLESTRING) {
			if (ch == '\'') {
				if (lastTagWasScript)
					scriptLanguage = segIsScriptingIndicator(styler, styler.GetStartSegment(), i, scriptLanguage);
				styler.ColourTo(i, SCE_H_SINGLESTRING);
				state = SCE_H_OTHER;
			}
		} else if (state == SCE_HJ_DEFAULT || state == SCE_HJ_START) {
			if (iswordstart(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_HJ_WORD;
			} else if (ch == '/' && chNext == '*') {
				styler.ColourTo(i - 1, state);
				if (chNext2 == '*')
					state = SCE_HJ_COMMENTDOC;
				else
					state = SCE_HJ_COMMENT;
			} else if (ch == '/' && chNext == '/') {
				styler.ColourTo(i - 1, state);
				state = SCE_HJ_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				state = SCE_HJ_DOUBLESTRING;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, state);
				state = SCE_HJ_SINGLESTRING;
			} else if ((ch == '<') && (chNext == '/')) {
				styler.ColourTo(i - 1, state);
				state = SCE_H_TAGUNKNOWN;
			} else if ((ch == '<') && (chNext == '!') && (chNext2 == '-') &&
				styler.SafeGetCharAt(i + 3) == '-') {
				styler.ColourTo(i - 1, state);
				state = SCE_HJ_COMMENTLINE;
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_HJ_SYMBOLS);
				state = SCE_HJ_DEFAULT;
			} else if ((ch == ' ') || (ch == '\t')) {
				if (state == SCE_HJ_START) {
					styler.ColourTo(i - 1, state);
					state = SCE_HJ_DEFAULT;
				}
			}
		} else if (state == SCE_HJ_WORD) {
			if (!iswordchar(ch)) {
				classifyWordHTJS(styler.GetStartSegment(), i - 1, keywords2, styler);
				//styler.ColourTo(i - 1, eHTJSKeyword);
				state = SCE_HJ_DEFAULT;
				if (ch == '/' && chNext == '*') {
					if (chNext2 == '*')
						state = SCE_HJ_COMMENTDOC;
					else
						state = SCE_HJ_COMMENT;
				} else if (ch == '/' && chNext == '/') {
					state = SCE_HJ_COMMENTLINE;
				} else if (ch == '\"') {
					state = SCE_HJ_DOUBLESTRING;
				} else if (ch == '\'') {
					state = SCE_HJ_SINGLESTRING;
				} else if (isoperator(ch)) {
					styler.ColourTo(i, SCE_HJ_SYMBOLS);
					state = SCE_HJ_DEFAULT;
				}
			}
		} else if (state == SCE_HJ_COMMENT) {
			if (ch == '/' && chPrev == '*') {
				state = SCE_HJ_DEFAULT;
				styler.ColourTo(i, SCE_HJ_COMMENT);
			} else if ((ch == '<') && (chNext == '/')) {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i + 1, SCE_H_TAGEND);
				i++;
				ch = chNext;
				state = SCE_H_DEFAULT;
			}
		} else if (state == SCE_HJ_COMMENTDOC) {
			if (ch == '/' && chPrev == '*') {
				state = SCE_HJ_DEFAULT;
				styler.ColourTo(i, SCE_HJ_COMMENTDOC);
			} else if ((ch == '<') && (chNext == '/')) {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i + 1, SCE_H_TAGEND);
				i++;
				ch = chNext;
				state = SCE_H_DEFAULT;
			}
		} else if (state == SCE_HJ_COMMENTLINE) {
			if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i - 1, SCE_HJ_COMMENTLINE);
				state = SCE_HJ_DEFAULT;
			} else if ((ch == '<') && (chNext == '/')) {
				// Common to hide end script tag in comment
				styler.ColourTo(i - 1, SCE_HJ_COMMENTLINE);
				state = SCE_H_TAGUNKNOWN;
			}
		} else if (state == SCE_HJ_DOUBLESTRING) {
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
				}
			} else if (ch == '\"') {
				styler.ColourTo(i, SCE_HJ_DOUBLESTRING);
				state = SCE_HJ_DEFAULT;
				i++;
				ch = chNext;
			} else if (isLineEnd(ch)) {
				styler.ColourTo(i-1, state);
				state = SCE_HJ_STRINGEOL;
			}
		} else if (state == SCE_HJ_SINGLESTRING) {
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
				}
			} else if (ch == '\'') {
				styler.ColourTo(i, SCE_HJ_SINGLESTRING);
				state = SCE_HJ_DEFAULT;
				i++;
				ch = chNext;
			} else if (isLineEnd(ch)) {
				styler.ColourTo(i-1, state);
				state = SCE_HJ_STRINGEOL;
			}
		} else if (state == SCE_HJ_STRINGEOL) {
			if (!isLineEnd(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_HJ_DEFAULT;
			} else if (!isLineEnd(chNext)) {
				styler.ColourTo(i, state);
				state = SCE_HJ_DEFAULT;
			}
		} else if (state == SCE_HJA_DEFAULT || state == SCE_HJA_START) {
			if (iswordstart(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_HJA_WORD;
			} else if (ch == '/' && chNext == '*') {
				styler.ColourTo(i - 1, state);
				if (chNext2 == '*')
					state = SCE_HJA_COMMENTDOC;
				else
					state = SCE_HJA_COMMENT;
			} else if (ch == '/' && chNext == '/') {
				styler.ColourTo(i - 1, state);
				state = SCE_HJA_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				state = SCE_HJA_DOUBLESTRING;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, state);
				state = SCE_HJA_SINGLESTRING;
			} else if ((ch == '<') && (chNext == '/')) {
				styler.ColourTo(i - 1, state);
				state = SCE_H_TAGUNKNOWN;
			} else if ((ch == '<') && (chNext == '!') && (chNext2 == '-') &&
				styler.SafeGetCharAt(i + 3) == '-') {
				styler.ColourTo(i - 1, state);
				state = SCE_HJA_COMMENTLINE;
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_HJA_SYMBOLS);
				state = SCE_HJA_DEFAULT;
			} else if ((ch == ' ') || (ch == '\t')) {
				if (state == SCE_HJA_START) {
					styler.ColourTo(i - 1, state);
					state = SCE_HJA_DEFAULT;
				}
			}
		} else if (state == SCE_HJA_WORD) {
			if (!iswordchar(ch)) {
				classifyWordHTJSA(styler.GetStartSegment(), i - 1, keywords2, styler);
				//styler.ColourTo(i - 1, eHTJSKeyword);
				state = SCE_HJA_DEFAULT;
				if (ch == '/' && chNext == '*') {
					if (chNext2 == '*')
						state = SCE_HJA_COMMENTDOC;
					else
						state = SCE_HJA_COMMENT;
				} else if (ch == '/' && chNext == '/') {
					state = SCE_HJA_COMMENTLINE;
				} else if (ch == '\"') {
					state = SCE_HJA_DOUBLESTRING;
				} else if (ch == '\'') {
					state = SCE_HJA_SINGLESTRING;
				} else if (isoperator(ch)) {
					styler.ColourTo(i, SCE_HJA_SYMBOLS);
					state = SCE_HJA_DEFAULT;
				}
			}
		} else if (state == SCE_HJA_COMMENT) {
			if (ch == '/' && chPrev == '*') {
				state = SCE_HJA_DEFAULT;
				styler.ColourTo(i, SCE_HJA_COMMENT);
			} else if ((ch == '<') && (chNext == '/')) {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i + 1, SCE_H_TAGEND);
				i++;
				ch = chNext;
				state = SCE_H_DEFAULT;
			}
		} else if (state == SCE_HJA_COMMENTDOC) {
			if (ch == '/' && chPrev == '*') {
				state = SCE_HJA_DEFAULT;
				styler.ColourTo(i, SCE_HJA_COMMENTDOC);
			} else if ((ch == '<') && (chNext == '/')) {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i + 1, SCE_H_TAGEND);
				i++;
				ch = chNext;
				state = SCE_H_DEFAULT;
			}
		} else if (state == SCE_HJA_COMMENTLINE) {
			if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i - 1, SCE_HJA_COMMENTLINE);
				state = SCE_HJA_DEFAULT;
			} else if ((ch == '<') && (chNext == '/')) {
				// Common to hide end script tag in comment
				styler.ColourTo(i - 1, SCE_HJA_COMMENTLINE);
				state = SCE_H_TAGUNKNOWN;
			}
		} else if (state == SCE_HJA_DOUBLESTRING) {
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
				}
			} else if (ch == '\"') {
				styler.ColourTo(i, SCE_HJA_DOUBLESTRING);
				state = SCE_HJA_DEFAULT;
				i++;
				ch = chNext;
			} else if (isLineEnd(ch)) {
				styler.ColourTo(i-1, state);
				state = SCE_HJA_STRINGEOL;
			}
		} else if (state == SCE_HJA_SINGLESTRING) {
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
				}
			} else if (ch == '\'') {
				styler.ColourTo(i, SCE_HJA_SINGLESTRING);
				state = SCE_HJA_DEFAULT;
				i++;
				ch = chNext;
			} else if (isLineEnd(ch)) {
				styler.ColourTo(i-1, state);
				state = SCE_HJA_STRINGEOL;
			}
		} else if (state == SCE_HJA_STRINGEOL) {
			if (!isLineEnd(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_HJA_DEFAULT;
			} else if (!isLineEnd(chNext)) {
				styler.ColourTo(i, state);
				state = SCE_HJA_DEFAULT;
			}
		} else if (state == SCE_HB_DEFAULT || state == SCE_HB_START) {
			if (iswordstart(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_HB_WORD;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, state);
				state = SCE_HB_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				state = SCE_HB_STRING;
			} else if ((ch == '<') && (chNext == '/')) {
				styler.ColourTo(i - 1, state);
				state = SCE_H_TAGUNKNOWN;
			} else if ((ch == '<') && (chNext == '!') && (chNext2 == '-') &&
				styler.SafeGetCharAt(i + 3) == '-') {
				styler.ColourTo(i - 1, state);
				state = SCE_HB_COMMENTLINE;
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_HB_DEFAULT);
				state = SCE_HB_DEFAULT;
			} else if ((ch == ' ') || (ch == '\t')) {
				if (state == SCE_HB_START) {
					styler.ColourTo(i - 1, state);
					state = SCE_HB_DEFAULT;
				}
			}
		} else if (state == SCE_HB_WORD) {
			if (!iswordchar(ch)) {
				state = classifyWordHTVB(styler.GetStartSegment(), i - 1, keywords3, styler);
				if (state == SCE_HB_DEFAULT) {
					if (ch == '\"') {
						state = SCE_HB_STRING;
					} else if (ch == '\'') {
						state = SCE_HB_COMMENTLINE;
					} else if (isoperator(ch)) {
						styler.ColourTo(i, SCE_HB_DEFAULT);
						state = SCE_HB_DEFAULT;
					}
				}
			}
		} else if (state == SCE_HB_STRING) {
			if (ch == '\"') {
				styler.ColourTo(i, state);
				state = SCE_HB_DEFAULT;
				i++;
				ch = chNext;
			} else if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i-1, state);
				state = SCE_HB_STRINGEOL;
			}
		} else if (state == SCE_HB_COMMENTLINE) {
			if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i - 1, state);
				state = SCE_HB_DEFAULT;
			} else if ((ch == '<') && (chNext == '/')) {
				// Common to hide end script tag in comment
				styler.ColourTo(i - 1, state);
				state = SCE_H_TAGUNKNOWN;
			}
		} else if (state == SCE_HB_STRINGEOL) {
			if (!isLineEnd(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_HB_DEFAULT;
			} else if (!isLineEnd(chNext)) {
				styler.ColourTo(i, state);
				state = SCE_HB_DEFAULT;
			}
		} else if (state == SCE_HBA_DEFAULT || state == SCE_HBA_START) {
			if (iswordstart(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_HBA_WORD;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, state);
				state = SCE_HBA_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				state = SCE_HBA_STRING;
			} else if ((ch == '<') && (chNext == '/')) {
				styler.ColourTo(i - 1, state);
				state = SCE_H_TAGUNKNOWN;
			} else if ((ch == '<') && (chNext == '!') && (chNext2 == '-') &&
				styler.SafeGetCharAt(i + 3) == '-') {
				styler.ColourTo(i - 1, state);
				state = SCE_HBA_COMMENTLINE;
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_HBA_DEFAULT);
				state = SCE_HBA_DEFAULT;
			} else if ((ch == ' ') || (ch == '\t')) {
				if (state == SCE_HBA_START) {
					styler.ColourTo(i - 1, state);
					state = SCE_HBA_DEFAULT;
				}
			}
		} else if (state == SCE_HBA_WORD) {
			if (!iswordchar(ch)) {
				state = classifyWordHTVBA(styler.GetStartSegment(), i - 1, keywords3, styler);
				if (state == SCE_HBA_DEFAULT) {
					if (ch == '\"') {
						state = SCE_HBA_STRING;
					} else if (ch == '\'') {
						state = SCE_HBA_COMMENTLINE;
					} else if (isoperator(ch)) {
						styler.ColourTo(i, SCE_HBA_DEFAULT);
						state = SCE_HBA_DEFAULT;
					}
				}
			}
		} else if (state == SCE_HBA_STRING) {
			if (ch == '\"') {
				styler.ColourTo(i, state);
				state = SCE_HBA_DEFAULT;
				i++;
				ch = chNext;
			} else if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i-1, state);
				state = SCE_HBA_STRINGEOL;
			}
		} else if (state == SCE_HBA_COMMENTLINE) {
			if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i - 1, state);
				state = SCE_HBA_DEFAULT;
			} else if ((ch == '<') && (chNext == '/')) {
				// Common to hide end script tag in comment
				styler.ColourTo(i - 1, state);
				state = SCE_H_TAGUNKNOWN;
			}
		} else if (state == SCE_HBA_STRINGEOL) {
			if (!isLineEnd(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_HBA_DEFAULT;
			} else if (!isLineEnd(chNext)) {
				styler.ColourTo(i, state);
				state = SCE_HBA_DEFAULT;
			}
		} else if (state == SCE_HP_DEFAULT || state == SCE_HP_START) {
			if (iswordstart(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_HP_WORD;
			} else if ((ch == '<') && (chNext == '/')) {
				styler.ColourTo(i - 1, state);
				state = SCE_H_TAGUNKNOWN;
			} else if ((ch == '<') && (chNext == '!') && (chNext2 == '-') &&
				styler.SafeGetCharAt(i + 3) == '-') {
				styler.ColourTo(i - 1, state);
				state = SCE_HP_COMMENTLINE;
			} else if (ch == '#') {
				styler.ColourTo(i - 1, state);
				state = SCE_HP_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				if (chNext == '\"' && chNext2 == '\"') {
					i += 2;
					state = SCE_HP_TRIPLEDOUBLE;
					ch = ' ';
					chPrev = ' ';
					chNext = styler.SafeGetCharAt(i + 1);
				} else {
					state = SCE_HP_STRING;
				}
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, state);
				if (chNext == '\'' && chNext2 == '\'') {
					i += 2;
					state = SCE_HP_TRIPLE;
					ch = ' ';
					chPrev = ' ';
					chNext = styler.SafeGetCharAt(i + 1);
				} else {
					state = SCE_HP_CHARACTER;
				}
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_HP_OPERATOR);
			} else if ((ch == ' ') || (ch == '\t')) {
				if (state == SCE_HP_START) {
					styler.ColourTo(i - 1, state);
					state = SCE_HP_DEFAULT;
				}
			}
		} else if (state == SCE_HP_WORD) {
			if (!iswordchar(ch)) {
				classifyWordHTPy(styler.GetStartSegment(), i - 1, keywords4, styler, prevWord);
				state = SCE_HP_DEFAULT;
				if (ch == '#') {
					state = SCE_HP_COMMENTLINE;
				} else if (ch == '\"') {
					if (chNext == '\"' && chNext2 == '\"') {
						i += 2;
						state = SCE_HP_TRIPLEDOUBLE;
						ch = ' ';
						chPrev = ' ';
						chNext = styler.SafeGetCharAt(i + 1);
					} else {
						state = SCE_HP_STRING;
					}
				} else if (ch == '\'') {
					if (chNext == '\'' && chNext2 == '\'') {
						i += 2;
						state = SCE_HP_TRIPLE;
						ch = ' ';
						chPrev = ' ';
						chNext = styler.SafeGetCharAt(i + 1);
					} else {
						state = SCE_HP_CHARACTER;
					}
				} else if (isoperator(ch)) {
					styler.ColourTo(i, SCE_HP_OPERATOR);
				}
			}
		} else if (state == SCE_HP_COMMENTLINE) {
			if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i - 1, state);
				state = SCE_HP_DEFAULT;
			}
		} else if (state == SCE_HP_STRING) {
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (ch == '\"') {
				styler.ColourTo(i, state);
				state = SCE_HP_DEFAULT;
			}
		} else if (state == SCE_HP_CHARACTER) {
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (ch == '\'') {
				styler.ColourTo(i, state);
				state = SCE_HP_DEFAULT;
			}
		} else if (state == SCE_HP_TRIPLE) {
			if (ch == '\'' && chPrev == '\'' && chPrev2 == '\'') {
				styler.ColourTo(i, state);
				state = SCE_HP_DEFAULT;
			}
		} else if (state == SCE_HP_TRIPLEDOUBLE) {
			if (ch == '\"' && chPrev == '\"' && chPrev2 == '\"') {
				styler.ColourTo(i, state);
				state = SCE_HP_DEFAULT;
			}
		} else if (state == SCE_HPA_DEFAULT || state == SCE_HPA_START) {
			if (iswordstart(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_HPA_WORD;
			} else if ((ch == '<') && (chNext == '/')) {
				styler.ColourTo(i - 1, state);
				state = SCE_H_TAGUNKNOWN;
			} else if ((ch == '<') && (chNext == '!') && (chNext2 == '-') &&
				styler.SafeGetCharAt(i + 3) == '-') {
				styler.ColourTo(i - 1, state);
				state = SCE_HPA_COMMENTLINE;
			} else if (ch == '#') {
				styler.ColourTo(i - 1, state);
				state = SCE_HPA_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				if (chNext == '\"' && chNext2 == '\"') {
					i += 2;
					state = SCE_HPA_TRIPLEDOUBLE;
					ch = ' ';
					chPrev = ' ';
					chNext = styler.SafeGetCharAt(i + 1);
				} else {
					state = SCE_HPA_STRING;
				}
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, state);
				if (chNext == '\'' && chNext2 == '\'') {
					i += 2;
					state = SCE_HPA_TRIPLE;
					ch = ' ';
					chPrev = ' ';
					chNext = styler.SafeGetCharAt(i + 1);
				} else {
					state = SCE_HPA_CHARACTER;
				}
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_HPA_OPERATOR);
			} else if ((ch == ' ') || (ch == '\t')) {
				if (state == SCE_HPA_START) {
					styler.ColourTo(i - 1, state);
					state = SCE_HPA_DEFAULT;
				}
			}
		} else if (state == SCE_HPA_WORD) {
			if (!iswordchar(ch)) {
				classifyWordHTPyA(styler.GetStartSegment(), i - 1, keywords4, styler, prevWord);
				state = SCE_HPA_DEFAULT;
				if (ch == '#') {
					state = SCE_HPA_COMMENTLINE;
				} else if (ch == '\"') {
					if (chNext == '\"' && chNext2 == '\"') {
						i += 2;
						state = SCE_HPA_TRIPLEDOUBLE;
						ch = ' ';
						chPrev = ' ';
						chNext = styler.SafeGetCharAt(i + 1);
					} else {
						state = SCE_HPA_STRING;
					}
				} else if (ch == '\'') {
					if (chNext == '\'' && chNext2 == '\'') {
						i += 2;
						state = SCE_HPA_TRIPLE;
						ch = ' ';
						chPrev = ' ';
						chNext = styler.SafeGetCharAt(i + 1);
					} else {
						state = SCE_HPA_CHARACTER;
					}
				} else if (isoperator(ch)) {
					styler.ColourTo(i, SCE_HPA_OPERATOR);
				}
			}
		} else if (state == SCE_HPA_COMMENTLINE) {
			if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i - 1, state);
				state = SCE_HPA_DEFAULT;
			}
		} else if (state == SCE_HPA_STRING) {
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (ch == '\"') {
				styler.ColourTo(i, state);
				state = SCE_HPA_DEFAULT;
			}
		} else if (state == SCE_HPA_CHARACTER) {
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (ch == '\'') {
				styler.ColourTo(i, state);
				state = SCE_HPA_DEFAULT;
			}
		} else if (state == SCE_HPA_TRIPLE) {
			if (ch == '\'' && chPrev == '\'' && chPrev2 == '\'') {
				styler.ColourTo(i, state);
				state = SCE_HPA_DEFAULT;
			}
		} else if (state == SCE_HPA_TRIPLEDOUBLE) {
			if (ch == '\"' && chPrev == '\"' && chPrev2 == '\"') {
				styler.ColourTo(i, state);
				state = SCE_HPA_DEFAULT;
			}
		}
		if (state == SCE_HB_DEFAULT) {    // One of the above succeeded
			if (ch == '\"') {
				state = SCE_HB_STRING;
			} else if (ch == '\'') {
				state = SCE_HB_COMMENTLINE;
			} else if (iswordstart(ch)) {
				state = SCE_HB_WORD;
			} else if (isoperator(ch)) {
				styler.ColourTo(i, SCE_HB_DEFAULT);
			}
		}
		if (state == SCE_HBA_DEFAULT) {    // One of the above succeeded
			if (ch == '\"') {
				state = SCE_HBA_STRING;
			} else if (ch == '\'') {
				state = SCE_HBA_COMMENTLINE;
			} else if (iswordstart(ch)) {
				state = SCE_HBA_WORD;
			} else if (isoperator(ch)) {
				styler.ColourTo(i, SCE_HBA_DEFAULT);
			}
		}
		if (state == SCE_HJ_DEFAULT) {    // One of the above succeeded
			if (ch == '/' && chNext == '*') {
				if (styler.SafeGetCharAt(i + 2) == '*')
					state = SCE_HJ_COMMENTDOC;
				else
					state = SCE_HJ_COMMENT;
			} else if (ch == '/' && chNext == '/') {
				state = SCE_HJ_COMMENTLINE;
			} else if (ch == '\"') {
				state = SCE_HJ_DOUBLESTRING;
			} else if (ch == '\'') {
				state = SCE_HJ_SINGLESTRING;
			} else if (iswordstart(ch)) {
				state = SCE_HJ_WORD;
			} else if (isoperator(ch)) {
				styler.ColourTo(i, SCE_HJ_SYMBOLS);
			}
		}
		if (state == SCE_HJA_DEFAULT) {    // One of the above succeeded
			if (ch == '/' && chNext == '*') {
				if (styler.SafeGetCharAt(i + 2) == '*')
					state = SCE_HJA_COMMENTDOC;
				else
					state = SCE_HJA_COMMENT;
			} else if (ch == '/' && chNext == '/') {
				state = SCE_HJA_COMMENTLINE;
			} else if (ch == '\"') {
				state = SCE_HJA_DOUBLESTRING;
			} else if (ch == '\'') {
				state = SCE_HJA_SINGLESTRING;
			} else if (iswordstart(ch)) {
				state = SCE_HJA_WORD;
			} else if (isoperator(ch)) {
				styler.ColourTo(i, SCE_HJA_SYMBOLS);
			}
		}
		chPrev2 = chPrev;
		chPrev = ch;
	}
	styler.ColourTo(lengthDoc - 1, state);
}

LexerModule lmHTML(SCLEX_HTML, ColouriseHyperTextDoc);
LexerModule lmXML(SCLEX_XML, ColouriseHyperTextDoc);
