// SciTE - Scintilla based Text Editor
// KeyWords.cxx - colourise for particular languages
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

inline bool IsLeadByte(int codePage, char ch) {
#if PLAT_GTK
	// TODO: support DBCS under GTK+
	return false;
#elif PLAT_WIN 
	return codePage && IsDBCSLeadByteEx(codePage, ch);
#elif PLAT_WX 
	return false;
#endif 
}

inline bool iswordchar(char ch) {
	return isalnum(ch) || ch == '.' || ch == '_';
}

inline bool iswordstart(char ch) {
	return isalnum(ch) || ch == '_';
}

enum { wsSpace = 1, wsTab = 2, wsSpaceTab = 4, wsInconsistent=8};

static int IndentAmount(StylingContext &styler, int line, int *flags) {
	int end = styler.Length();
	int spaceFlags = 0;
	
	// Determines the indentation level of the current line and also checks for consistent 
	// indentation compared to the previous line.
	// Indentation is judged consistent when the indentation whitespace of each line lines 
	// the same or the indentation of one line is a prefix of the other.
	
	int pos = styler.LineStart(line);
	char ch = styler[pos];
	int indent = 0;
	bool inPrevPrefix = line > 0;
	int posPrev = inPrevPrefix ? styler.LineStart(line-1) : 0;
	while ((ch == ' ' || ch == '\t') && (pos < end)) {
		if (inPrevPrefix) {
			char chPrev = styler[posPrev++];
			if (chPrev == ' ' || chPrev == '\t') {
				if (chPrev != ch)
					spaceFlags |= wsInconsistent;
			} else {
				inPrevPrefix = false;
			}
		}
		if (ch == ' ') {
			spaceFlags |= wsSpace;
			indent++;
		} else {	// Tab
			spaceFlags |= wsTab;
			if (spaceFlags & wsSpace)
				spaceFlags |= wsSpaceTab;
			indent = (indent / 8 + 1) * 8;
		}
		ch = styler[++pos];
	}
	
	*flags = spaceFlags;
	indent += SC_FOLDLEVELBASE;
	if (isspace(ch)) // Completely empty line
		return indent | SC_FOLDLEVELWHITEFLAG;
	else
		return indent;
}

inline bool isoperator(char ch) {
	if (isalnum(ch))
		return false;
	// '.' left out as it is used to make up numbers
	if (ch == '%' || ch == '^' || ch == '&' || ch == '*' ||
	        ch == '(' || ch == ')' || ch == '-' || ch == '+' ||
	        ch == '=' || ch == '|' || ch == '{' || ch == '}' ||
	        ch == '[' || ch == ']' || ch == ':' || ch == ';' ||
	        ch == '<' || ch == '>' || ch == ',' || ch == '/' ||
	        ch == '?' || ch == '!' || ch == '.' || ch == '~')
		return true;
	return false;
}

static void classifyWordCpp(unsigned int start, unsigned int end, WordList &keywords, StylingContext &styler) {
	char s[100];
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.');
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = styler[start + i];
		s[i + 1] = '\0';
	}
	char chAttr = SCE_C_IDENTIFIER;
	if (wordIsNumber)
		chAttr = SCE_C_NUMBER;
	else {
		if (keywords.InList(s))
			chAttr = SCE_C_WORD;
	}
	styler.ColourSegment(start, end, chAttr);
}

static void ColouriseCppDoc(int codePage, int startPos, int length,
                            int initStyle, WordList &keywords, StylingContext &styler) {

	bool fold = styler.GetPropSet().GetInt("fold");
	int lineCurrent = styler.GetLine(startPos);
	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;

	int state = initStyle;
	char chPrev = ' ';
	char chNext = styler[startPos];
	int startSeg = startPos;
	int lengthDoc = startPos + length;
	int visChars = 0;
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

		if (IsLeadByte(codePage, ch)) {	// dbcs
			chNext = styler.SafeGetCharAt(i + 2);
			chPrev = ' ';
			i += 1;
			continue;
		}

		if (state == SCE_C_STRINGEOL) {
			if (ch != '\r' && ch != '\n') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_C_DEFAULT;
				startSeg = i;
			}
		}
		if (state == SCE_C_DEFAULT) {
			if (iswordstart(ch)) {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_C_WORD;
				startSeg = i;
			} else if (ch == '/' && chNext == '*') {
				styler.ColourSegment(startSeg, i - 1, state);
				if (styler.SafeGetCharAt(i + 2) == '*')
					state = SCE_C_COMMENTDOC;
				else
					state = SCE_C_COMMENT;
				startSeg = i;
			} else if (ch == '/' && chNext == '/') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_C_COMMENTLINE;
				startSeg = i;
			} else if (ch == '\"') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_C_STRING;
				startSeg = i;
			} else if (ch == '\'') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_C_CHARACTER;
				startSeg = i;
			} else if (ch == '#') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_C_PREPROCESSOR;
				startSeg = i;
			} else if (isoperator(ch)) {
				styler.ColourSegment(startSeg, i - 1, state);
				styler.ColourSegment(i, i, SCE_C_OPERATOR);
				startSeg = i + 1;
				if ((ch == '{') || (ch == '}')) {
					levelCurrent += (ch == '{') ? 1 : -1;
				}
			}
		} else if (state == SCE_C_WORD) {
			if (!iswordchar(ch)) {
				classifyWordCpp(startSeg, i - 1, keywords, styler);
				state = SCE_C_DEFAULT;
				startSeg = i;
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
					styler.ColourSegment(startSeg, i, SCE_C_OPERATOR);
					startSeg = i + 1;
					if ((ch == '{') || (ch == '}')) {
						levelCurrent += (ch == '{') ? 1 : -1;
					}
				}
			}
		} else {
			if (state == SCE_C_PREPROCESSOR) {
				if ((ch == '\r' || ch == '\n') && (chPrev != '\\')) {
					styler.ColourSegment(startSeg, i - 1, state);
					state = SCE_C_DEFAULT;
					startSeg = i;
				}
			} else if (state == SCE_C_COMMENT) {
				if (ch == '/' && chPrev == '*' && (
				            (i > startSeg + 2) || ((initStyle == SCE_C_COMMENT) && (startSeg == startPos)))) {
					styler.ColourSegment(startSeg, i, state);
					state = SCE_C_DEFAULT;
					startSeg = i + 1;
				}
			} else if (state == SCE_C_COMMENTDOC) {
				if (ch == '/' && chPrev == '*' && (
				            (i > startSeg + 3) || ((initStyle == SCE_C_COMMENTDOC) && (startSeg == startPos)))) {
					styler.ColourSegment(startSeg, i, state);
					state = SCE_C_DEFAULT;
					startSeg = i + 1;
				}
			} else if (state == SCE_C_COMMENTLINE) {
				if (ch == '\r' || ch == '\n') {
					styler.ColourSegment(startSeg, i - 1, state);
					state = SCE_C_DEFAULT;
					startSeg = i;
				}
			} else if (state == SCE_C_STRING) {
				if ((ch == '\r' || ch == '\n') && (chPrev != '\\')) {
					styler.ColourSegment(startSeg, i - 1, state);
					state = SCE_C_STRINGEOL;
					startSeg = i;
				} else if (ch == '\\') {
					if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\"') {
					styler.ColourSegment(startSeg, i, state);
					state = SCE_C_DEFAULT;
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
					startSeg = i;
				}
			} else if (state == SCE_C_CHARACTER) {
				if ((ch == '\r' || ch == '\n') && (chPrev != '\\')) {
					styler.ColourSegment(startSeg, i - 1, state);
					state = SCE_C_STRINGEOL;
					startSeg = i;
				} else if (ch == '\\') {
					if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\'') {
					styler.ColourSegment(startSeg, i, state);
					state = SCE_C_DEFAULT;
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
					startSeg = i;
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
					styler.ColourSegment(startSeg, i, SCE_C_OPERATOR);
					startSeg = i + 1;
					if ((ch == '{') || (ch == '}')) {
						levelCurrent += (ch == '{') ? 1 : -1;
					}
				}
			}
		}
		chPrev = ch;
	}
	if (startSeg < lengthDoc)
		styler.ColourSegment(startSeg, lengthDoc - 1, state);
	// Fill in the real level of the next line, keeping the current flags as they will be filled in later
	if (fold) {
		int flagsNext = styler.LevelAt(lineCurrent) & ~SC_FOLDLEVELNUMBERMASK;
		//styler.SetLevel(lineCurrent, levelCurrent | flagsNext);
		styler.SetLevel(lineCurrent, levelPrev | flagsNext);
		
	}
}

inline bool isPerlOperator(char ch) {
	if (isalnum(ch))
		return false;
	// '.' left out as it is used to make up numbers
	if (ch == '%' || ch == '^' || ch == '&' || ch == '*' || ch == '\\' ||
	        ch == '(' || ch == ')' || ch == '-' || ch == '+' ||
	        ch == '=' || ch == '|' || ch == '{' || ch == '}' ||
	        ch == '[' || ch == ']' || ch == ':' || ch == ';' ||
	        ch == '<' || ch == '>' || ch == ',' || ch == '/' ||
	        ch == '?' || ch == '!' || ch == '.' || ch == '~')
		return true;
	return false;
}

static int classifyWordPerl(unsigned int start, unsigned int end, WordList &keywords, StylingContext &styler) {
	char s[100];
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.');
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = styler[start + i];
		s[i + 1] = '\0';
	}
	char chAttr = SCE_PL_IDENTIFIER;
	if (wordIsNumber)
		chAttr = SCE_PL_NUMBER;
	else {
		if (keywords.InList(s))
			chAttr = SCE_PL_WORD;
	}
	styler.ColourSegment(start, end, chAttr);
	return chAttr;
}

static bool isEndVar(char ch) {
	return !isalnum(ch) && ch != '#' && ch != '$' &&
	       ch != '_' && ch != '\'';
}

static bool isMatch(StylingContext &styler, int lengthDoc, int pos, const char *val) {
	if ((pos + static_cast<int>(strlen(val))) >= lengthDoc) {
		return false;
	}
	while (*val) {
		if (*val != styler[pos++]) {
			return false;
		}
		val++;
	}
	return true;
}

static bool isOKQuote(char ch) {
	if (isalnum(ch))
		return false;
	if (isspace(ch))
		return false;
	if (iscntrl(ch))
		return false;
	return true;
}

static char opposite(char ch) {
	if (ch == '(')
		return ')';
	if (ch == '[')
		return ']';
	if (ch == '{')
		return '}';
	if (ch == '<')
		return '>';
	return ch;
}

static void ColourisePerlDoc(int codePage, int startPos, int length, int initStyle,
                             WordList &keywords, StylingContext &styler) {
	char sooked[100];
	int quotes = 0;
	char quoteDown = 'd';
	char quoteUp = 'd';
	int quoteRep = 1;
	int sookedpos = 0;
	bool preferRE = true;
	sooked[sookedpos] = '\0';
	int state = initStyle;
	int lengthDoc = startPos + length;
	// If in a long distance lexical state, seek to the beginning  to find quote characters
	if (state == SCE_PL_HERE || state == SCE_PL_REGEX || 
		state == SCE_PL_REGSUBST || state == SCE_PL_LONGQUOTE) {
		while ((startPos > 1) && (styler.StyleAt(startPos - 1) == state)) {
			startPos--;
		}
		state = SCE_PL_DEFAULT;
	}
	styler.StartAt(startPos);
	char chPrev = ' ';
	char chNext = styler[startPos];
	int startSeg = startPos;
	for (int i = startPos; i <= lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		char chNext2 = styler.SafeGetCharAt(i + 2);

		if (IsLeadByte(codePage, ch)) {	// dbcs
			chNext = styler.SafeGetCharAt(i + 2);
			chPrev = ' ';
			i += 1;
			continue;
		}

		if (state == SCE_PL_DEFAULT) {
			if (iswordstart(ch)) {
				styler.ColourSegment(startSeg, i - 1, state);
				if (ch == 's' && !isalnum(chNext)) {
					state = SCE_PL_REGSUBST;
					quotes = 0;
					quoteUp = '\0';
					quoteDown = '\0';
					quoteRep = 2;
					startSeg = i;
				} else if (ch == 'm' && !isalnum(chNext)) {
					state = SCE_PL_REGEX;
					quotes = 0;
					quoteUp = '\0';
					quoteDown = '\0';
					quoteRep = 1;
					startSeg = i;
				} else if (ch == 't' && chNext == 'r' && !isalnum(chNext2)) {
					state = SCE_PL_REGSUBST;
					quotes = 0;
					quoteUp = '\0';
					quoteDown = '\0';
					quoteRep = 2;
					startSeg = i;
					i++;
					chNext = chNext2;
				} else if (ch == 'q' && (chNext == 'q' || chNext == 'r' || chNext == 'w' || chNext == 'x') && !isalnum(chNext2)) {
					state = SCE_PL_LONGQUOTE;
					startSeg = i;
					i++;
					chNext = chNext2;
					quotes = 0;
					quoteUp = '\0';
					quoteDown = '\0';
					quoteRep = 1;
				} else {
					state = SCE_PL_WORD;
					startSeg = i;
					preferRE = false;
				}
			} else if (ch == '#') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_PL_COMMENTLINE;
				startSeg = i;
			} else if (ch == '\"') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_PL_STRING;
				startSeg = i;
			} else if (ch == '\'') {
				if (chPrev == '&') {
					// Archaic call
					styler.ColourSegment(i, i, state);
					startSeg = i + 1;
				} else {
					styler.ColourSegment(startSeg, i - 1, state);
					state = SCE_PL_CHARACTER;
					startSeg = i;
				}
			} else if (ch == '`') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_PL_BACKTICKS;
				startSeg = i;
			} else if (ch == '$') {
				preferRE = false;
				styler.ColourSegment(startSeg, i - 1, state);
				if (isalnum(chNext) || chNext == '#' || chNext == '$' || chNext == '_') {
					state = SCE_PL_SCALAR;
					startSeg = i;
				} else if (chNext != '{' && chNext != '[') {
					styler.ColourSegment(i - 1, i, SCE_PL_SCALAR);
					i++;
					startSeg = i + 1;
					ch = ' ';
					chNext = ' ';
				} else {
					styler.ColourSegment(i, i, SCE_PL_SCALAR);
					startSeg = i + 1;
				}
			} else if (ch == '@') {
				preferRE = false;
				styler.ColourSegment(startSeg, i - 1, state);
				if (isalpha(chNext) || chNext == '#' || chNext == '$' || chNext == '_') {
					state = SCE_PL_ARRAY;
					startSeg = i;
				} else if (chNext != '{' && chNext != '[') {
					styler.ColourSegment(i - 1, i, SCE_PL_ARRAY);
					i++;
					startSeg = i + 1;
					ch = ' ';
				} else {
					styler.ColourSegment(i, i, SCE_PL_ARRAY);
					startSeg = i + 1;
				}
			} else if (ch == '%') {
				preferRE = false;
				styler.ColourSegment(startSeg, i - 1, state);
				if (isalpha(chNext) || chNext == '#' || chNext == '$' || chNext == '_') {
					state = SCE_PL_HASH;
					startSeg = i;
				} else if (chNext != '{' && chNext != '[') {
					styler.ColourSegment(i - 1, i, SCE_PL_HASH);
					i++;
					startSeg = i + 1;
					ch = ' ';
				} else {
					styler.ColourSegment(i, i, SCE_PL_HASH);
					startSeg = i + 1;
				}
			} else if (ch == '*') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_PL_SYMBOLTABLE;
				startSeg = i;
			} else if (ch == '/' && preferRE) {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_PL_REGEX;
				quoteUp = '/';
				quoteDown = '/';
				quotes = 1;
				quoteRep = 1;
				startSeg = i;
			} else if (ch == '<' && chNext == '<') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_PL_HERE;
				startSeg = i;
				i++;
				ch = chNext;
				chNext = chNext2;
				quotes = 0;
				sookedpos = 0;
				sooked[sookedpos] = '\0';
			} else if (ch == '=' && isalpha(chNext)) {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_PL_POD;
				startSeg = i;
				quotes = 0;
				sookedpos = 0;
				sooked[sookedpos] = '\0';
			} else if (isPerlOperator(ch)) {
				if (ch == ')' || ch == ']')
					preferRE = false;
				else
					preferRE = true;
				styler.ColourSegment(startSeg, i - 1, state);
				styler.ColourSegment(i, i, SCE_PL_OPERATOR);
				startSeg = i + 1;
			}
		} else if (state == SCE_PL_WORD) {
			if (!iswordchar(ch) && ch != '\'') {	// Archaic Perl has quotes inside names
				if (isMatch(styler, lengthDoc, startSeg, "__DATA__")) {
					styler.ColourSegment(startSeg, i, SCE_PL_DATASECTION);
					state = SCE_PL_DATASECTION;
				} else if (isMatch(styler, lengthDoc, startSeg, "__END__")) {
					styler.ColourSegment(startSeg, i, SCE_PL_DATASECTION);
					state = SCE_PL_DATASECTION;
				} else {
					if (classifyWordPerl(startSeg, i - 1, keywords, styler) == SCE_PL_WORD)
						preferRE = true;
					state = SCE_PL_DEFAULT;
					startSeg = i;
					if (ch == '#') {
						state = SCE_PL_COMMENTLINE;
					} else if (ch == '\"') {
						state = SCE_PL_STRING;
					} else if (ch == '\'') {
						state = SCE_PL_CHARACTER;
					} else if (ch == '<' && chNext == '<') {
						state = SCE_PL_HERE;
						quotes = 0;
						startSeg = i;
						sookedpos = 0;
						sooked[sookedpos] = '\0';
					} else if (isPerlOperator(ch)) {
						if (ch == ')' || ch == ']')
							preferRE = false;
						else
							preferRE = true;
						styler.ColourSegment(startSeg, i, SCE_PL_OPERATOR);
						state = SCE_PL_DEFAULT;
						startSeg = i + 1;
					}
				}
			}
		} else {
			if (state == SCE_PL_COMMENTLINE) {
				if (ch == '\r' || ch == '\n') {
					styler.ColourSegment(startSeg, i - 1, state);
					state = SCE_PL_DEFAULT;
					startSeg = i;
				}
			} else if (state == SCE_PL_HERE) {
				if (isalnum(ch) && quotes < 2) {
					sooked[sookedpos++] = ch;
					sooked[sookedpos] = '\0';
					if (quotes == 0)
						quotes = 1;
				} else {
					quotes++;
				}

				if (quotes > 1 && isMatch(styler, lengthDoc, i, sooked)) {
					styler.ColourSegment(startSeg, i + sookedpos - 1, SCE_PL_HERE);
					state = SCE_PL_DEFAULT;
					i += sookedpos;
					startSeg = i;
					chNext = ' ';
				}
			} else if (state == SCE_PL_STRING) {
				if (ch == '\\') {
					if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\"') {
					styler.ColourSegment(startSeg, i, state);
					state = SCE_PL_DEFAULT;
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
					startSeg = i;
				}
			} else if (state == SCE_PL_CHARACTER) {
				if (ch == '\\') {
					if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\'') {
					styler.ColourSegment(startSeg, i, state);
					state = SCE_PL_DEFAULT;
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
					startSeg = i;
				}
			} else if (state == SCE_PL_BACKTICKS) {
				if (ch == '`') {
					styler.ColourSegment(startSeg, i, state);
					state = SCE_PL_DEFAULT;
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
					startSeg = i;
				}
			} else if (state == SCE_PL_POD) {
				if (ch == '=') {
					if (isMatch(styler, lengthDoc, i, "=cut")) {
						styler.ColourSegment(startSeg, i - 1 + 4, state);
						i += 4;
						startSeg = i;
						state = SCE_PL_DEFAULT;
						chNext = ' ';
						ch = ' ';
					}
				}
			} else if (state == SCE_PL_SCALAR) {
				if (isEndVar(ch)) {
					styler.ColourSegment(startSeg, i - 1, state);
					startSeg = i;
					state = SCE_PL_DEFAULT;
				}
			} else if (state == SCE_PL_ARRAY) {
				if (isEndVar(ch)) {
					styler.ColourSegment(startSeg, i - 1, state);
					startSeg = i;
					state = SCE_PL_DEFAULT;
				}
			} else if (state == SCE_PL_HASH) {
				if (isEndVar(ch)) {
					styler.ColourSegment(startSeg, i - 1, state);
					startSeg = i;
					state = SCE_PL_DEFAULT;
				}
			} else if (state == SCE_PL_SYMBOLTABLE) {
				if (isEndVar(ch)) {
					styler.ColourSegment(startSeg, i - 1, state);
					startSeg = i;
					state = SCE_PL_DEFAULT;
				}
			} else if (state == SCE_PL_REF) {
				if (isEndVar(ch)) {
					styler.ColourSegment(startSeg, i - 1, state);
					startSeg = i;
					state = SCE_PL_DEFAULT;
				}
			} else if (state == SCE_PL_REGEX) {
				if (!quoteUp && !isspace(ch)) {
					quoteUp = ch;
					quoteDown = opposite(ch);
					quotes++;
				} else {
					if (ch == quoteDown && chPrev != '\\') {
						quotes--;
						if (quotes == 0) {
							quoteRep--;
							if (quoteUp == quoteDown) {
								quotes++;
							}
						}
						if (!isalpha(chNext)) {
							if (quoteRep <= 0) {
								styler.ColourSegment(startSeg, i, state);
								startSeg = i + 1;
								state = SCE_PL_DEFAULT;
								ch = ' ';
							}
						}
					} else if (ch == quoteUp && chPrev != '\\') {
						quotes++;
					} else if (!isalpha(chNext)) {
						if (quoteRep <= 0) {
							styler.ColourSegment(startSeg, i, state);
							startSeg = i + 1;
							state = SCE_PL_DEFAULT;
							ch = ' ';
						}
					}
				}
			} else if (state == SCE_PL_REGSUBST) {
				if (!quoteUp && !isspace(ch)) {
					quoteUp = ch;
					quoteDown = opposite(ch);
					quotes++;
				} else {
					if (ch == quoteDown && chPrev != '\\') {
						quotes--;
						if (quotes == 0) {
							quoteRep--;
						}
						if (!isalpha(chNext)) {
							if (quoteRep <= 0) {
								styler.ColourSegment(startSeg, i, state);
								startSeg = i + 1;
								state = SCE_PL_DEFAULT;
								ch = ' ';
							}
						}
						if (quoteUp == quoteDown) {
							quotes++;
						}
					} else if (ch == quoteUp && chPrev != '\\') {
						quotes++;
					} else if (!isalpha(chNext)) {
						if (quoteRep <= 0) {
							styler.ColourSegment(startSeg, i, state);
							startSeg = i + 1;
							state = SCE_PL_DEFAULT;
							ch = ' ';
						}
					}
				}
			} else if (state == SCE_PL_LONGQUOTE) {
				if (!quoteDown && !isspace(ch)) {
					quoteUp = ch;
					quoteDown = opposite(quoteUp);
					quotes++;
				} else if (ch == quoteDown) {
					quotes--;
					if (quotes == 0) {
						quoteRep--;
						if (quoteRep <= 0) {
							styler.ColourSegment(startSeg, i, state);
							startSeg = i + 1;
							state = SCE_PL_DEFAULT;
							ch = ' ';
						}
						if (quoteUp == quoteDown) {
							quotes++;
						}
					}
				} else if (ch == quoteUp) {
					quotes++;
				}
			}

			if (state == SCE_PL_DEFAULT) {    // One of the above succeeded
				if (ch == '#') {
					state = SCE_PL_COMMENTLINE;
				} else if (ch == '\"') {
					state = SCE_PL_STRING;
				} else if (ch == '\'') {
					state = SCE_PL_CHARACTER;
				} else if (iswordstart(ch)) {
					state = SCE_PL_WORD;
					preferRE = false;
				} else if (isoperator(ch)) {
					styler.ColourSegment(startSeg, i, SCE_PL_OPERATOR);
					startSeg = i + 1;
				}
			}
		}
		chPrev = ch;
	}
	if (startSeg < lengthDoc)
		styler.ColourSegment(startSeg, lengthDoc, state);
}


static int classifyWordVB(unsigned int start, unsigned int end, WordList &keywords, StylingContext &styler) {
	char s[100];
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.');
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = tolower(styler[start + i]);
		s[i + 1] = '\0';
	}
	char chAttr = SCE_C_DEFAULT;
	if (wordIsNumber)
		chAttr = SCE_C_NUMBER;
	else {
		if (keywords.InList(s)) {
			chAttr = SCE_C_WORD;
			if (strcmp(s, "rem") == 0)
				chAttr = SCE_C_COMMENTLINE;
		}
	}
	styler.ColourSegment(start, end, chAttr);
	if (chAttr == SCE_C_COMMENTLINE)
		return SCE_C_COMMENTLINE;
	else
		return SCE_C_DEFAULT;
}

static void ColouriseVBDoc(int codePage, int startPos, int length, int initStyle,
                           WordList &keywords, StylingContext &styler) {
	int state = initStyle;
	char chNext = styler[startPos];
	int startSeg = startPos;
	int lengthDoc = startPos + length;
	for (int i = startPos; i < lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if (IsLeadByte(codePage, ch)) {	// dbcs
			chNext = styler.SafeGetCharAt(i + 2);
			i += 1;
			continue;
		}

		if (state == SCE_C_DEFAULT) {
			if (iswordstart(ch)) {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_C_WORD;
				startSeg = i;
			} else if (ch == '\'') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_C_COMMENTLINE;
				startSeg = i;
			} else if (ch == '\"') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_C_STRING;
				startSeg = i;
			}
		} else if (state == SCE_C_WORD) {
			if (!iswordchar(ch)) {
				state = classifyWordVB(startSeg, i - 1, keywords, styler);
				if (state == SCE_C_DEFAULT) {
					startSeg = i;
					if (ch == '\'') {
						state = SCE_C_COMMENTLINE;
					} else if (ch == '\"') {
						state = SCE_C_STRING;
					}
				}
			}
		} else {
			if (state == SCE_C_COMMENTLINE) {
				if (ch == '\r' || ch == '\n') {
					styler.ColourSegment(startSeg, i - 1, state);
					state = SCE_C_DEFAULT;
					startSeg = i;
				}
			} else if (state == SCE_C_STRING) {
				// VB doubles quotes to preserve them
				if (ch == '\"') {
					styler.ColourSegment(startSeg, i, state);
					state = SCE_C_DEFAULT;
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
					startSeg = i;
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
	if (startSeg < lengthDoc)
		styler.ColourSegment(startSeg, lengthDoc, state);
}

static void classifyWordPy(unsigned int start, unsigned int end, WordList &keywords, StylingContext &styler, char *prevWord) {
	char s[100];
	bool wordIsNumber = isdigit(styler[start]);
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = styler[start + i];
		s[i + 1] = '\0';
	}
	char chAttr = SCE_P_IDENTIFIER;
	if (0 == strcmp(prevWord, "class"))
		chAttr = SCE_P_CLASSNAME;
	else if (0 == strcmp(prevWord, "def"))
		chAttr = SCE_P_DEFNAME;
	else if (wordIsNumber)
		chAttr = SCE_P_NUMBER;
	else if (keywords.InList(s))
		chAttr = SCE_P_WORD;
	styler.ColourSegment(start, end, chAttr);
	strcpy(prevWord, s);
}

static void ColourisePyDoc(int codePage, int startPos, int length, int initStyle, WordList &keywords, StylingContext &styler) {
	//Platform::DebugPrintf("Python coloured\n");
	bool fold = styler.GetPropSet().GetInt("fold");
	int whingeLevel = styler.GetPropSet().GetInt("tab.timmy.whinge.level");
	char prevWord[200];
	prevWord[0] = '\0';
	if (length == 0)
		return ;
	int lineCurrent = styler.GetLine(startPos);
	int spaceFlags = 0;
	// TODO: Need to check previous line for indentation for both folding and bad indentation
	int indentCurrent = IndentAmount(styler, lineCurrent, &spaceFlags);

	int state = initStyle & 31;
	char chPrev = ' ';
	char chPrev2 = ' ';
	char chNext = styler[startPos];
	char chNext2 = styler[startPos];
	int startSeg = startPos;
	int lengthDoc = startPos + length;
	bool atStartLine = true;
	for (int i = startPos; i <= lengthDoc; i++) {
	
		if (atStartLine) {
			if (whingeLevel == 1) {
				styler.SetFlags((spaceFlags & wsInconsistent) ? 64 : 0, state);
			} else if (whingeLevel == 2) {
				styler.SetFlags((spaceFlags & wsSpaceTab) ? 64 : 0, state);
			} else if (whingeLevel == 3) {
				styler.SetFlags((spaceFlags & wsSpace) ? 64 : 0, state);
			} else if (whingeLevel == 4) {
				styler.SetFlags((spaceFlags & wsTab) ? 64 : 0, state);
			}
			atStartLine = false;
		}
		
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		chNext2 = styler.SafeGetCharAt(i + 2);
		
		if ((ch == '\r' && chNext != '\n') || (ch == '\n')) {
			if ((state == SCE_P_DEFAULT) || (state == SCE_P_TRIPLE) || (state == SCE_P_TRIPLEDOUBLE)) {
				// Perform colourisation of white space and triple quoted strings at end of each line to allow
				// tab marking to work inside white space and triple quoted strings
				styler.ColourSegment(startSeg, i, state);
				startSeg = i + 1;
			}

			int lev = indentCurrent;
			int indentNext = IndentAmount(styler, lineCurrent + 1, &spaceFlags);
			if (!(indentCurrent & SC_FOLDLEVELWHITEFLAG)) {
				// Only non whitespace lines can be headers
				if ((indentCurrent & SC_FOLDLEVELNUMBERMASK) < (indentNext & SC_FOLDLEVELNUMBERMASK)) {
					lev |= SC_FOLDLEVELHEADERFLAG;
				}
			}
			indentCurrent = indentNext;
			if (fold) {
				styler.SetLevel(lineCurrent, lev);
			}
			lineCurrent++;
			atStartLine = true;
		}

		if (IsLeadByte(codePage, ch)) {	// dbcs
			chNext = styler.SafeGetCharAt(i + 2);
			chPrev = ' ';
			chPrev2 = ' ';
			i += 1;
			continue;
		}

		if (state == SCE_P_DEFAULT) {
			if (iswordstart(ch)) {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_P_WORD;
				startSeg = i;
			} else if (ch == '#') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_P_COMMENTLINE;
				startSeg = i;
			} else if (ch == '\"') {
				styler.ColourSegment(startSeg, i - 1, state);
				startSeg = i;
				if (chNext == '\"' && chNext2 == '\"') {
					i += 2;
					state = SCE_P_TRIPLEDOUBLE;
					ch = ' ';
					chPrev = ' ';
					chNext = styler.SafeGetCharAt(i + 1);
				} else {
					state = SCE_P_STRING;
				}
			} else if (ch == '\'') {
				styler.ColourSegment(startSeg, i - 1, state);
				startSeg = i;
				if (chNext == '\'' && chNext2 == '\'') {
					i += 2;
					state = SCE_P_TRIPLE;
					ch = ' ';
					chPrev = ' ';
					chNext = styler.SafeGetCharAt(i + 1);
				} else {
					state = SCE_P_CHARACTER;
				}
			} else if (isoperator(ch)) {
				styler.ColourSegment(startSeg, i - 1, state);
				styler.ColourSegment(i, i, SCE_P_OPERATOR);
				startSeg = i + 1;
			}
		} else if (state == SCE_P_WORD) {
			if (!iswordchar(ch)) {
				classifyWordPy(startSeg, i - 1, keywords, styler, prevWord);
				state = SCE_P_DEFAULT;
				startSeg = i;
				if (ch == '#') {
					state = SCE_P_COMMENTLINE;
				} else if (ch == '\"') {
					if (chNext == '\"' && chNext2 == '\"') {
						i += 2;
						state = SCE_P_TRIPLEDOUBLE;
						ch = ' ';
						chPrev = ' ';
						chNext = styler.SafeGetCharAt(i + 1);
					} else {
						state = SCE_P_STRING;
					}
				} else if (ch == '\'') {
					if (chNext == '\'' && chNext2 == '\'') {
						i += 2;
						state = SCE_P_TRIPLE;
						ch = ' ';
						chPrev = ' ';
						chNext = styler.SafeGetCharAt(i + 1);
					} else {
						state = SCE_P_CHARACTER;
					}
				} else if (isoperator(ch)) {
					styler.ColourSegment(startSeg, i, SCE_P_OPERATOR);
					startSeg = i + 1;
				}
			}
		} else {
			if (state == SCE_P_COMMENTLINE) {
				if (ch == '\r' || ch == '\n') {
					styler.ColourSegment(startSeg, i - 1, state);
					state = SCE_P_DEFAULT;
					startSeg = i;
				}
			} else if (state == SCE_P_STRING) {
				if (ch == '\\') {
					if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\"') {
					styler.ColourSegment(startSeg, i, state);
					state = SCE_P_DEFAULT;
					startSeg = i + 1;
				}
			} else if (state == SCE_P_CHARACTER) {
				if (ch == '\\') {
					if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\'') {
					styler.ColourSegment(startSeg, i, state);
					state = SCE_P_DEFAULT;
					startSeg = i + 1;
				}
			} else if (state == SCE_P_TRIPLE) {
				if (ch == '\'' && chPrev == '\'' && chPrev2 == '\'') {
					styler.ColourSegment(startSeg, i, state);
					state = SCE_P_DEFAULT;
					startSeg = i + 1;
				}
			} else if (state == SCE_P_TRIPLEDOUBLE) {
				if (ch == '\"' && chPrev == '\"' && chPrev2 == '\"') {
					styler.ColourSegment(startSeg, i, state);
					state = SCE_P_DEFAULT;
					startSeg = i + 1;
				}
			}
		}
		chPrev2 = chPrev;
		chPrev = ch;
	}
	if (startSeg <= lengthDoc) {
		if (state == SCE_P_DEFAULT) {
			styler.ColourSegment(startSeg, lengthDoc, state);
		} else if (state == SCE_P_WORD) {
			classifyWordPy(startSeg, lengthDoc, keywords, styler, prevWord);
		} else if (state == SCE_P_COMMENTLINE) {
			styler.ColourSegment(startSeg, lengthDoc, state);
		} else if (state == SCE_P_STRING) {
			styler.ColourSegment(startSeg, lengthDoc, state);
		} else if (state == SCE_P_CHARACTER) {
			styler.ColourSegment(startSeg, lengthDoc, state);
		} else if (state == SCE_P_TRIPLE) {
			styler.ColourSegment(startSeg, lengthDoc, state);
		} else if (state == SCE_P_TRIPLEDOUBLE) {
			styler.ColourSegment(startSeg, lengthDoc, state);
		}
	}
}

static void ColouriseBatchLine(char *lineBuffer, int lengthLine, StylingContext &styler) {
	if (0 == strncmp(lineBuffer, "REM", 3)) {
		styler.ColourSegment(0, lengthLine - 1, 1);
	} else if (0 == strncmp(lineBuffer, "rem", 3)) {
		styler.ColourSegment(0, lengthLine - 1, 1);
	} else if (0 == strncmp(lineBuffer, "SET", 3)) {
		styler.ColourSegment(0, lengthLine - 1, 2);
	} else if (0 == strncmp(lineBuffer, "set", 3)) {
		styler.ColourSegment(0, lengthLine - 1, 2);
	} else if (lineBuffer[0] == ':') {
		styler.ColourSegment(0, lengthLine - 1, 3);
	} else {
		styler.ColourSegment(0, lengthLine - 1, 0);
	}
}

static void ColouriseBatchDoc(int startPos, int length, int, StylingContext &styler) {
	char lineBuffer[1024];
	unsigned int linePos = 0;
	for (int i = startPos; i < startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if (styler[i] == '\r' || styler[i] == '\n' || (linePos >= sizeof(lineBuffer) - 1)) {
			ColouriseBatchLine(lineBuffer, linePos, styler);
			linePos = 0;
		}
	}
	if (linePos > 0)
		ColouriseBatchLine(lineBuffer, linePos, styler);
}

enum { eScriptNone, eScriptJS, eScriptVBS, eScriptPython };
static int segIsScriptingIndicator(StylingContext &styler, unsigned int start, unsigned int end, int prevValue) {
	char s[100];
	s[0] = '\0';
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = tolower(styler[start + i]);
		s[i + 1] = '\0';
	}
Platform::DebugPrintf("Scripting indicator [%s]\n", s);
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
	for (int cPos=start; cPos <= end && i < 30; cPos++) {
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

static void ColouriseHyperTextDoc(int codePage, int startPos, int length,
                                  int initStyle, WordList &keywords, WordList &keywords2, WordList &keywords3, WordList &keywords4, 
				  StylingContext &styler) {
	
	styler.StartAt(startPos, 63);
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
	styler.StartAt(startPos, 63);

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

		if (IsLeadByte(codePage, ch)) {	// dbcs
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
					state = SCE_HB_START;
				else if  (defaultScript == eScriptPython)
				 	state = SCE_HP_START;
				else
					state = SCE_HJ_START;
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
			styler.ColourTo(i - 1, state);
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
		} else {
			if (state == SCE_HP_COMMENTLINE) {
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
		chPrev2 = chPrev;
		chPrev = ch;
	}
	styler.ColourTo(lengthDoc - 1, state);
}

static void ColourisePropsLine(char *lineBuffer, int lengthLine, StylingContext &styler) {
	int i = 0;
	while (isspace(lineBuffer[i]) && (i < lengthLine))	// Skip initial spaces
		i++;
	if (lineBuffer[i] == '#' || lineBuffer[i] == '!' || lineBuffer[i] == ';') {
		styler.ColourSegment(0, lengthLine - 1, 1);
	} else if (lineBuffer[i] == '[') {
		styler.ColourSegment(0, lengthLine - 1, 2);
	} else if (lineBuffer[i] == '@') {
		styler.ColourSegment(0, i, 4);
		if (lineBuffer[++i] == '=')
			styler.ColourSegment(i, i, 3);
		if (++i < lengthLine)
			styler.ColourSegment(i, lengthLine - 1, 0);
	} else {
		while (lineBuffer[i] != '=' && (i < lengthLine))	// Search the '=' character
			i++;
		if (lineBuffer[i] == '=') {
			styler.ColourSegment(0, i - 1, 0);
			styler.ColourSegment(i, i, 3);
			if (++i < lengthLine)
				styler.ColourSegment(i, lengthLine - 1, 0);
		} else
			styler.ColourSegment(0, lengthLine - 1, 0);
	}
}

static void ColourisePropsDoc(int startPos, int length, int, StylingContext &styler) {
	char lineBuffer[1024];
	unsigned int linePos = 0;
	for (int i = startPos; i <= startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if (styler[i] == '\r' || styler[i] == '\n' || (linePos >= sizeof(lineBuffer) - 1)) {
			lineBuffer[linePos] = '\0';
			ColourisePropsLine(lineBuffer, linePos, styler);
			linePos = 0;
		}
	}
	if (linePos > 0)
		ColourisePropsLine(lineBuffer, linePos, styler);
}

static void ColouriseMakeLine(char *lineBuffer, int lengthLine, StylingContext &styler) {
	int i = 0;
	while (isspace(lineBuffer[i]) && (i < lengthLine))
		i++;
	if (lineBuffer[i] == '#' || lineBuffer[i] == '!') {
		styler.ColourSegment(0, lengthLine - 1, 1);
	} else {
		styler.ColourSegment(0, lengthLine - 1, 0);
	}
}

static void ColouriseMakeDoc(int startPos, int length, int, StylingContext &styler) {
	char lineBuffer[1024];
	unsigned int linePos = 0;
	for (int i = startPos; i <= startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if (styler[i] == '\r' || styler[i] == '\n' || (linePos >= sizeof(lineBuffer) - 1)) {
			ColouriseMakeLine(lineBuffer, linePos, styler);
			linePos = 0;
		}
	}
	if (linePos > 0)
		ColouriseMakeLine(lineBuffer, linePos, styler);
}

static void ColouriseErrorListLine(char *lineBuffer, int lengthLine, StylingContext &styler) {
	if (lineBuffer[0] == '>') {
		// Command or return status
		styler.ColourSegment(0, lengthLine - 1, 4);
	} else if (strstr(lineBuffer, "File \"") && strstr(lineBuffer, ", line ")) {
		styler.ColourSegment(0, lengthLine - 1, 1);
	} else if (0 == strncmp(lineBuffer, "Error ", strlen("Error "))) {
		// Borland error message
		styler.ColourSegment(0, lengthLine - 1, 5);
	} else if (0 == strncmp(lineBuffer, "Warning ", strlen("Warning "))) {
		// Borland warning message
		styler.ColourSegment(0, lengthLine - 1, 5);
	} else {
		// Look for <filename>:<line>:message
		// Look for <filename>(line)message
		// Look for <filename>(line,pos)message
		int state = 0;
		for (int i = 0; i < lengthLine; i++) {
			if (state == 0 && lineBuffer[i] == ':' && isdigit(lineBuffer[i + 1])) {
				state = 1;
			} else if (state == 0 && lineBuffer[i] == '(') {
				state = 10;
			} else if (state == 1 && isdigit(lineBuffer[i])) {
				state = 2;
			} else if (state == 2 && lineBuffer[i] == ':') {
				state = 3;
				break;
			} else if (state == 2 && !isdigit(lineBuffer[i])) {
				state = 99;
			} else if (state == 10 && isdigit(lineBuffer[i])) {
				state = 11;
			} else if (state == 11 && lineBuffer[i] == ',') {
				state = 14;
			} else if (state == 11 && lineBuffer[i] == ')') {
				state = 12;
				break;
			} else if (state == 12 && lineBuffer[i] == ':') {
				state = 13;
			} else if (state == 14 && lineBuffer[i] == ')') {
				state = 15;
				break;
			} else if (((state == 11) || (state == 14)) && !((lineBuffer[i] == ' ') || isdigit(lineBuffer[i]))) {
				state = 99;
			}
		}
		if (state == 3) {
			styler.ColourSegment(0, lengthLine - 1, 2);
		} else if ((state == 14) || (state == 15)) {
			styler.ColourSegment(0, lengthLine - 1, 3);
		} else {
			styler.ColourSegment(0, lengthLine - 1, 0);
		}
	}
}

static void ColouriseErrorListDoc(int startPos, int length, int, StylingContext &styler) {
	char lineBuffer[1024];
	unsigned int linePos = 0;
	for (int i = startPos; i <= startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if (styler[i] == '\r' || styler[i] == '\n' || (linePos >= sizeof(lineBuffer) - 1)) {
			ColouriseErrorListLine(lineBuffer, linePos, styler);
			linePos = 0;
		}
	}
	if (linePos > 0)
		ColouriseErrorListLine(lineBuffer, linePos, styler);
}

static void classifyWordSQL(unsigned int start, unsigned int end, WordList &keywords, StylingContext &styler) {
	char s[100];
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.');
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = toupper(styler[start + i]);
		s[i + 1] = '\0';
	}
	char chAttr = SCE_C_IDENTIFIER;
	if (wordIsNumber)
		chAttr = SCE_C_NUMBER;
	else {
		if (keywords.InList(s))
			chAttr = SCE_C_WORD;
	}
	styler.ColourSegment(start, end, chAttr);
}

static void ColouriseSQLDoc(int codePage, int startPos, int length,
                            int initStyle, WordList &keywords, StylingContext &styler) {

	bool fold = styler.GetPropSet().GetInt("fold");
	int lineCurrent = styler.GetLine(startPos);
	int spaceFlags = 0;
	int indentCurrent = 0;

	int state = initStyle;
	char chPrev = ' ';
	char chNext = styler[startPos];
	int startSeg = startPos;
	int lengthDoc = startPos + length;
	bool prevCr = false;
	for (int i = startPos; i <= lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if ((ch == '\r' && chNext != '\n') || (ch == '\n')) {
			indentCurrent = IndentAmount(styler, lineCurrent, &spaceFlags);
			int lev = indentCurrent;
			if (!(indentCurrent & SC_FOLDLEVELWHITEFLAG)) {
				// Only non whitespace lines can be headers
				int indentNext = IndentAmount(styler, lineCurrent + 1, &spaceFlags);
				if (indentCurrent < (indentNext & ~SC_FOLDLEVELWHITEFLAG)) {
					lev |= SC_FOLDLEVELHEADERFLAG;
				}
			}
			if (fold) {
				styler.SetLevel(lineCurrent, lev);
			}
		}

		if (IsLeadByte(codePage, ch)) {	// dbcs
			chNext = styler.SafeGetCharAt(i + 2);
			chPrev = ' ';
			i += 1;
			continue;
		}

		if (state == SCE_C_DEFAULT) {
			if (iswordstart(ch)) {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_C_WORD;
				startSeg = i;
			} else if (ch == '/' && chNext == '*') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_C_COMMENT;
				startSeg = i;
			} else if (ch == '-' && chNext == '-') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_C_COMMENTLINE;
				startSeg = i;
			} else if (ch == '\'') {
				styler.ColourSegment(startSeg, i - 1, state);
				state = SCE_C_STRING;
				startSeg = i;
			} else if (isoperator(ch)) {
				styler.ColourSegment(startSeg, i - 1, state);
				styler.ColourSegment(i, i, SCE_C_OPERATOR);
				startSeg = i + 1;
			}
		} else if (state == SCE_C_WORD) {
			if (!iswordchar(ch)) {
				classifyWordSQL(startSeg, i - 1, keywords, styler);
				state = SCE_C_DEFAULT;
				startSeg = i;
				if (ch == '/' && chNext == '*') {
					state = SCE_C_COMMENT;
				} else if (ch == '-' && chNext == '-') {
					state = SCE_C_COMMENTLINE;
				} else if (ch == '\'') {
					state = SCE_C_STRING;
				} else if (isoperator(ch)) {
					styler.ColourSegment(startSeg, i, SCE_C_OPERATOR);
					startSeg = i + 1;
				}
			}
		} else {
			if (state == SCE_C_COMMENT) {
				if (ch == '/' && chPrev == '*' && (
				            (i > startSeg + 2) || ((initStyle == SCE_C_COMMENT) && (startSeg == startPos)))) {
					state = SCE_C_DEFAULT;
					styler.ColourSegment(startSeg, i, state);
					startSeg = i + 1;
				}
			} else if (state == SCE_C_COMMENTLINE) {
				if (ch == '\r' || ch == '\n') {
					styler.ColourSegment(startSeg, i - 1, state);
					state = SCE_C_DEFAULT;
					startSeg = i;
				}
			} else if (state == SCE_C_STRING) {
				if (ch == '\'') {
					if ( chNext == '\'' ) {
						i++;
					} else {
						styler.ColourSegment(startSeg, i, state);
						state = SCE_C_DEFAULT;
						i++;
						startSeg = i;
					}
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			}
			if (state == SCE_C_DEFAULT) {    // One of the above succeeded
				if (ch == '/' && chNext == '*') {
					state = SCE_C_COMMENT;
				} else if (ch == '-' && chNext == '-') {
					state = SCE_C_COMMENTLINE;
				} else if (ch == '\'') {
					state = SCE_C_STRING;
				} else if (iswordstart(ch)) {
					state = SCE_C_WORD;
				} else if (isoperator(ch)) {
					styler.ColourSegment(startSeg, i, SCE_C_OPERATOR);
					startSeg = i + 1;
				}
			}
		}
		chPrev = ch;
	}
	if (startSeg < lengthDoc)
		styler.ColourSegment(startSeg, lengthDoc - 1, state);
}

void ColouriseDoc(int codePage, int startPos, int lengthDoc, int initStyle,
                  int language, WordList *keywordlists[], StylingContext &styler) {
	//Platform::DebugPrintf("ColouriseDoc <%s>\n", language);
	if (language == SCLEX_PYTHON) {
		// Python uses a different mask because bad indentation is marked by oring with 32
		styler.StartAt(startPos, 127);
		ColourisePyDoc(codePage, startPos, lengthDoc, initStyle, *keywordlists[0], styler);
	} else if (language == SCLEX_PERL) {
		// Lexer for perl often has to backtrack to start of current style to determine
		// which characters are being used as quotes, how deeply nested is the
		// start position and what the termination string is for here documents
		ColourisePerlDoc(codePage, startPos, lengthDoc, initStyle, *keywordlists[0], styler);
	} else if ((language == SCLEX_HTML) || (language == SCLEX_XML)) {
		// Lexer for HTML requires more lexical states (6 bits worth) than most lexers
		ColouriseHyperTextDoc(codePage, startPos, lengthDoc, initStyle, 
			*keywordlists[0], *keywordlists[1], *keywordlists[2], *keywordlists[3], styler);
	} else {
		styler.StartAt(startPos);
		if (language == SCLEX_CPP) {
			ColouriseCppDoc(codePage, startPos, lengthDoc, initStyle, *keywordlists[0], styler);
		} else if (language == SCLEX_SQL) {
			ColouriseSQLDoc(codePage, startPos, lengthDoc, initStyle, *keywordlists[0], styler);
		} else if (language == SCLEX_VB) {
			ColouriseVBDoc(codePage, startPos, lengthDoc, initStyle, *keywordlists[0], styler);
		} else if (language == SCLEX_PROPERTIES) {
			ColourisePropsDoc(startPos, lengthDoc, initStyle, styler);
		} else if (language == SCLEX_ERRORLIST) {
			ColouriseErrorListDoc(startPos, lengthDoc, initStyle, styler);
		} else if (language == SCLEX_MAKEFILE) {
			ColouriseMakeDoc(startPos, lengthDoc, initStyle, styler);
		} else if (language == SCLEX_BATCH) {
			ColouriseBatchDoc(startPos, lengthDoc, initStyle, styler);
		} else {
			// Null language means all style bytes are 0 so just mark the end - no need to fill in.
			styler.StartAt(startPos + lengthDoc - 1);
			styler.ColourSegment(0, 0, 0);
		}
	}
}
