// SciTE - Scintilla based Text Editor
// LexPerl.cxx - lexer for subset of Perl
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

static int classifyWordPerl(unsigned int start, unsigned int end, WordList &keywords, Accessor &styler) {
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
	styler.ColourTo(end, chAttr);
	return chAttr;
}

static bool isEndVar(char ch) {
	return !isalnum(ch) && ch != '#' && ch != '$' &&
	       ch != '_' && ch != '\'';
}

static bool isMatch(Accessor &styler, int lengthDoc, int pos, const char *val) {
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

static void ColourisePerlDoc(unsigned int startPos, int length, int initStyle,
                             WordList *keywordlists[], Accessor &styler) {

	// Lexer for perl often has to backtrack to start of current style to determine
	// which characters are being used as quotes, how deeply nested is the
	// start position and what the termination string is for here documents
	
	WordList &keywords = *keywordlists[0];
	
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
	styler.StartSegment(startPos);
	for (int i = startPos; i < lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		char chNext2 = styler.SafeGetCharAt(i + 2);

		if (styler.IsLeadByte(ch)) {
			chNext = styler.SafeGetCharAt(i + 2);
			chPrev = ' ';
			i += 1;
			continue;
		}

		if (state == SCE_PL_DEFAULT) {
			if (iswordstart(ch)) {
				styler.ColourTo(i - 1, state);
				if (ch == 's' && !isalnum(chNext)) {
					state = SCE_PL_REGSUBST;
					quotes = 0;
					quoteUp = '\0';
					quoteDown = '\0';
					quoteRep = 2;
				} else if (ch == 'm' && !isalnum(chNext)) {
					state = SCE_PL_REGEX;
					quotes = 0;
					quoteUp = '\0';
					quoteDown = '\0';
					quoteRep = 1;
				} else if (ch == 't' && chNext == 'r' && !isalnum(chNext2)) {
					state = SCE_PL_REGSUBST;
					quotes = 0;
					quoteUp = '\0';
					quoteDown = '\0';
					quoteRep = 2;
					i++;
					chNext = chNext2;
				} else if (ch == 'q' && (chNext == 'q' || chNext == 'r' || chNext == 'w' || chNext == 'x') && !isalnum(chNext2)) {
					state = SCE_PL_LONGQUOTE;
					i++;
					chNext = chNext2;
					quotes = 0;
					quoteUp = '\0';
					quoteDown = '\0';
					quoteRep = 1;
				} else {
					state = SCE_PL_WORD;
					preferRE = false;
				}
			} else if (ch == '#') {
				styler.ColourTo(i - 1, state);
				state = SCE_PL_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				state = SCE_PL_STRING;
			} else if (ch == '\'') {
				if (chPrev == '&') {
					// Archaic call
					styler.ColourTo(i, state);
				} else {
					styler.ColourTo(i - 1, state);
					state = SCE_PL_CHARACTER;
				}
			} else if (ch == '`') {
				styler.ColourTo(i - 1, state);
				state = SCE_PL_BACKTICKS;
			} else if (ch == '$') {
				preferRE = false;
				styler.ColourTo(i - 1, state);
				if (isalnum(chNext) || chNext == '#' || chNext == '$' || chNext == '_') {
					state = SCE_PL_SCALAR;
				} else if (chNext != '{' && chNext != '[') {
					styler.ColourTo(i, SCE_PL_SCALAR);
					i++;
					ch = ' ';
					chNext = ' ';
				} else {
					styler.ColourTo(i, SCE_PL_SCALAR);
				}
			} else if (ch == '@') {
				preferRE = false;
				styler.ColourTo(i - 1, state);
				if (isalpha(chNext) || chNext == '#' || chNext == '$' || chNext == '_') {
					state = SCE_PL_ARRAY;
				} else if (chNext != '{' && chNext != '[') {
					styler.ColourTo(i, SCE_PL_ARRAY);
					i++;
					ch = ' ';
				} else {
					styler.ColourTo(i, SCE_PL_ARRAY);
				}
			} else if (ch == '%') {
				preferRE = false;
				styler.ColourTo(i - 1, state);
				if (isalpha(chNext) || chNext == '#' || chNext == '$' || chNext == '_') {
					state = SCE_PL_HASH;
				} else if (chNext != '{' && chNext != '[') {
					styler.ColourTo(i, SCE_PL_HASH);
					i++;
					ch = ' ';
				} else {
					styler.ColourTo(i, SCE_PL_HASH);
				}
			} else if (ch == '*') {
				styler.ColourTo(i - 1, state);
				state = SCE_PL_SYMBOLTABLE;
			} else if (ch == '/' && preferRE) {
				styler.ColourTo(i - 1, state);
				state = SCE_PL_REGEX;
				quoteUp = '/';
				quoteDown = '/';
				quotes = 1;
				quoteRep = 1;
			} else if (ch == '<' && chNext == '<') {
				styler.ColourTo(i - 1, state);
				state = SCE_PL_HERE;
				i++;
				ch = chNext;
				chNext = chNext2;
				quotes = 0;
				sookedpos = 0;
				sooked[sookedpos] = '\0';
			} else if (ch == '=' && isalpha(chNext)) {
				styler.ColourTo(i - 1, state);
				state = SCE_PL_POD;
				quotes = 0;
				sookedpos = 0;
				sooked[sookedpos] = '\0';
			} else if (isPerlOperator(ch)) {
				if (ch == ')' || ch == ']')
					preferRE = false;
				else
					preferRE = true;
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_PL_OPERATOR);
			}
		} else if (state == SCE_PL_WORD) {
			if (!iswordchar(ch) && ch != '\'') {	// Archaic Perl has quotes inside names
				if (isMatch(styler, lengthDoc, styler.GetStartSegment(), "__DATA__")) {
					styler.ColourTo(i, SCE_PL_DATASECTION);
					state = SCE_PL_DATASECTION;
				} else if (isMatch(styler, lengthDoc, styler.GetStartSegment(), "__END__")) {
					styler.ColourTo(i, SCE_PL_DATASECTION);
					state = SCE_PL_DATASECTION;
				} else {
					if (classifyWordPerl(styler.GetStartSegment(), i - 1, keywords, styler) == SCE_PL_WORD)
						preferRE = true;
					state = SCE_PL_DEFAULT;
					if (ch == '#') {
						state = SCE_PL_COMMENTLINE;
					} else if (ch == '\"') {
						state = SCE_PL_STRING;
					} else if (ch == '\'') {
						state = SCE_PL_CHARACTER;
					} else if (ch == '<' && chNext == '<') {
						state = SCE_PL_HERE;
						quotes = 0;
						sookedpos = 0;
						sooked[sookedpos] = '\0';
					} else if (isPerlOperator(ch)) {
						if (ch == ')' || ch == ']')
							preferRE = false;
						else
							preferRE = true;
						styler.ColourTo(i, SCE_PL_OPERATOR);
						state = SCE_PL_DEFAULT;
					}
				}
			}
		} else {
			if (state == SCE_PL_COMMENTLINE) {
				if (ch == '\r' || ch == '\n') {
					styler.ColourTo(i - 1, state);
					state = SCE_PL_DEFAULT;
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
				if (quotes > 1 && isMatch(styler, lengthDoc, i, sooked) && (chPrev == '\n' || chPrev == '\r') ) {
					styler.ColourTo(i + sookedpos - 1, SCE_PL_HERE);
					i += sookedpos;
					chNext = styler.SafeGetCharAt(i);
					if (chNext == '\n' || chNext == '\r') {
						state = SCE_PL_DEFAULT;
					}
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
					styler.ColourTo(i, state);
					state = SCE_PL_DEFAULT;
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (state == SCE_PL_CHARACTER) {
				if (ch == '\\') {
					if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\'') {
					styler.ColourTo(i, state);
					state = SCE_PL_DEFAULT;
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (state == SCE_PL_BACKTICKS) {
				if (ch == '`') {
					styler.ColourTo(i, state);
					state = SCE_PL_DEFAULT;
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (state == SCE_PL_POD) {
				if (ch == '=') {
					if (isMatch(styler, lengthDoc, i, "=cut")) {
						styler.ColourTo(i - 1 + 4, state);
						i += 4;
						state = SCE_PL_DEFAULT;
						chNext = ' ';
						ch = ' ';
					}
				}
			} else if (state == SCE_PL_SCALAR) {
				if (isEndVar(ch)) {
					styler.ColourTo(i - 1, state);
					state = SCE_PL_DEFAULT;
				}
			} else if (state == SCE_PL_ARRAY) {
				if (isEndVar(ch)) {
					styler.ColourTo(i - 1, state);
					state = SCE_PL_DEFAULT;
				}
			} else if (state == SCE_PL_HASH) {
				if (isEndVar(ch)) {
					styler.ColourTo(i - 1, state);
					state = SCE_PL_DEFAULT;
				}
			} else if (state == SCE_PL_SYMBOLTABLE) {
				if (isEndVar(ch)) {
					styler.ColourTo(i - 1, state);
					state = SCE_PL_DEFAULT;
				}
			} else if (state == SCE_PL_REF) {
				if (isEndVar(ch)) {
					styler.ColourTo(i - 1, state);
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
								styler.ColourTo(i, state);
								state = SCE_PL_DEFAULT;
								ch = ' ';
							}
						}
					} else if (ch == quoteUp && chPrev != '\\') {
						quotes++;
					} else if (!isalpha(chNext)) {
						if (quoteRep <= 0) {
							styler.ColourTo(i, state);
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
								styler.ColourTo(i, state);
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
							styler.ColourTo(i, state);
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
							styler.ColourTo(i, state);
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
					styler.ColourTo(i, SCE_PL_OPERATOR);
				}
			}
		}
		chPrev = ch;
	}
	styler.ColourTo(lengthDoc, state);
}

LexerModule lmPerl(SCLEX_PERL, ColourisePerlDoc);
