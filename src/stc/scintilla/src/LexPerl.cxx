// Scintilla source code edit control
/** @file LexPerl.cxx
 ** Lexer for subset of Perl.
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

static inline bool isEOLChar(char ch) {
	return (ch == '\r') || (ch == '\n');
}

static bool isSingleCharOp(char ch) {
	char strCharSet[2];
	strCharSet[0] = ch;
	strCharSet[1] = '\0';
	return (NULL != strstr("rwxoRWXOezsfdlpSbctugkTBMAC", strCharSet));
}

static inline bool isPerlOperator(char ch) {
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

static inline bool isEndVar(char ch) {
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

	class HereDocCls {
	public:
		int State;		// 0: '<<' encountered
		// 1: collect the delimiter
		// 2: here doc text (lines after the delimiter)
		char Quote;		// the char after '<<'
		bool Quoted;		// true if Quote in ('\'','"','`')
		int DelimiterLength;	// strlen(Delimiter)
		char Delimiter[256];	// the Delimiter, 256: sizeof PL_tokenbuf
		HereDocCls() {
			State = 0;
			DelimiterLength = 0;
			Delimiter[0] = '\0';
		}
	};
	HereDocCls HereDoc;	// TODO: FIFO for stacked here-docs

	class QuoteCls {
		public:
		int  Rep;
		int  Count;
		char Up;
		char Down;
		QuoteCls() {
			this->New(1);
		}
		void New(int r) {
			Rep   = r;
			Count = 0;
			Up    = '\0';
			Down  = '\0';
		}
		void Open(char u) {
			Count++;
			Up    = u;
			Down  = opposite(Up);
		}
	};
	QuoteCls Quote;

	char sooked[100];
	int sookedpos = 0;
	bool preferRE = true;
	sooked[sookedpos] = '\0';
	int state = initStyle;
	unsigned int lengthDoc = startPos + length;

	// If in a long distance lexical state, seek to the beginning  to find quote characters
	if (state == SCE_PL_HERE_Q || state == SCE_PL_HERE_QQ || state == SCE_PL_HERE_QX) {
		while ((startPos > 1) && (styler.StyleAt(startPos) != SCE_PL_HERE_DELIM)) {
			startPos--;
		}
		startPos = styler.LineStart(styler.GetLine(startPos));
		state = styler.StyleAt(startPos - 1);
	}
	if ( state == SCE_PL_STRING_Q
	|| state == SCE_PL_STRING_QQ
	|| state == SCE_PL_STRING_QX
	|| state == SCE_PL_STRING_QR
	|| state == SCE_PL_STRING_QW
	|| state == SCE_PL_REGEX
	|| state == SCE_PL_REGSUBST
	) {
		while ((startPos > 1) && (styler.StyleAt(startPos - 1) == state)) {
			startPos--;
		}
		state = SCE_PL_DEFAULT;
	}

	styler.StartAt(startPos);
	char chPrev = styler.SafeGetCharAt(startPos - 1);
	if (startPos == 0)
		chPrev = '\n';
	char chNext = styler[startPos];
	styler.StartSegment(startPos);

	for (unsigned int i = startPos; i < lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		char chNext2 = styler.SafeGetCharAt(i + 2);

		if (styler.IsLeadByte(ch)) {
			chNext = styler.SafeGetCharAt(i + 2);
			chPrev = ' ';
			i += 1;
			continue;
		}
		if ((chPrev == '\r' && ch == '\n')) {	// skip on DOS/Windows
			chPrev = ch;
			continue;
		}

		if (HereDoc.State == 1 && isEOLChar(ch)) {
			// Begin of here-doc (the line after the here-doc delimiter):
			HereDoc.State = 2;
			styler.ColourTo(i - 1, state);
			if (HereDoc.Quoted) {
				if (state == SCE_PL_HERE_DELIM) {
					// Missing quote at end of string! We are stricter than perl.
					state = SCE_PL_ERROR;
				} else {
					switch (HereDoc.Quote) {
					case '\'':
						state = SCE_PL_HERE_Q ;
						break;
					case '"':
						state = SCE_PL_HERE_QQ;
						break;
					case '`':
						state = SCE_PL_HERE_QX;
						break;
					}
				}
			} else {
				switch (HereDoc.Quote) {
				case '\\':
					state = SCE_PL_HERE_Q ;
					break;
				default :
					state = SCE_PL_HERE_QQ;
				}
			}
		}

		if (state == SCE_PL_DEFAULT) {
			if (iswordstart(ch)) {
				styler.ColourTo(i - 1, state);
				if (ch == 's' && !isalnum(chNext)) {
					state = SCE_PL_REGSUBST;
					Quote.New(2);
				} else if (ch == 'm' && !isalnum(chNext)) {
					state = SCE_PL_REGEX;
					Quote.New(1);
				} else if (ch == 'q' && !isalnum(chNext)) {
					state = SCE_PL_STRING_Q;
					Quote.New(1);
				} else if (ch == 'y' && !isalnum(chNext)) {
					state = SCE_PL_REGSUBST;
					Quote.New(2);
				} else if (ch == 't' && chNext == 'r' && !isalnum(chNext2)) {
					state = SCE_PL_REGSUBST;
					Quote.New(2);
					i++;
					chNext = chNext2;
				} else if (ch == 'q' && (chNext == 'q' || chNext == 'r' || chNext == 'w' || chNext == 'x') && !isalnum(chNext2)) {
					if      (chNext == 'q') state = SCE_PL_STRING_QQ;
					else if (chNext == 'x') state = SCE_PL_STRING_QX;
					else if (chNext == 'r') state = SCE_PL_STRING_QR;
					else if (chNext == 'w') state = SCE_PL_STRING_QW;
					i++;
					chNext = chNext2;
					Quote.New(1);
				} else {
					state = SCE_PL_WORD;
					preferRE = false;
					if ((!iswordchar(chNext) && chNext != '\'')
						|| (chNext == '.' && chNext2 == '.')) {
						// We need that if length of word == 1!
						// This test is copied from the SCE_PL_WORD handler.
						classifyWordPerl(styler.GetStartSegment(), i, keywords, styler);
						state = SCE_PL_DEFAULT;
					}
				}
			} else if (ch == '#') {
				styler.ColourTo(i - 1, state);
				state = SCE_PL_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				state = SCE_PL_STRING;
				Quote.New(1);
				Quote.Open(ch);
			} else if (ch == '\'') {
				if (chPrev == '&') {
					// Archaic call
					styler.ColourTo(i, state);
				} else {
					styler.ColourTo(i - 1, state);
					state = SCE_PL_CHARACTER;
					Quote.New(1);
					Quote.Open(ch);
				}
			} else if (ch == '`') {
				styler.ColourTo(i - 1, state);
				state = SCE_PL_BACKTICKS;
				Quote.New(1);
				Quote.Open(ch);
			} else if (ch == '$') {
				preferRE = false;
				styler.ColourTo(i - 1, state);
				if ((chNext == '{') || isspacechar(chNext)) {
					styler.ColourTo(i, SCE_PL_SCALAR);
				} else {
					state = SCE_PL_SCALAR;
					i++;
					ch = chNext;
					chNext = chNext2;
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
				} else if (chNext == '{') {
					styler.ColourTo(i, SCE_PL_HASH);
				} else {
					styler.ColourTo(i, SCE_PL_OPERATOR);
				}
			} else if (ch == '*') {
				styler.ColourTo(i - 1, state);
				state = SCE_PL_SYMBOLTABLE;
			} else if (ch == '/' && preferRE) {
				styler.ColourTo(i - 1, state);
				state = SCE_PL_REGEX;
				Quote.New(1);
				Quote.Open(ch);
			} else if (ch == '<' && chNext == '<') {
				styler.ColourTo(i - 1, state);
				state = SCE_PL_HERE_DELIM;
				HereDoc.State = 0;
			} else if (ch == '='
			           && isalpha(chNext)
			           && (isEOLChar(chPrev))) {
				styler.ColourTo(i - 1, state);
				state = SCE_PL_POD;
				sookedpos = 0;
				sooked[sookedpos] = '\0';
			} else if (ch == '-'
			           && isSingleCharOp(chNext)
			           && !isalnum((chNext2 = styler.SafeGetCharAt(i+2)))) {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i + 1, SCE_PL_WORD);
				state = SCE_PL_DEFAULT;
				preferRE = false;
				i += 2;
				ch = chNext2;
				chNext = chNext2 = styler.SafeGetCharAt(i + 1);
			} else if (isPerlOperator(ch)) {
				if (ch == ')' || ch == ']')
					preferRE = false;
				else
					preferRE = true;
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_PL_OPERATOR);
			}
		} else if (state == SCE_PL_WORD) {
			if ((!iswordchar(chNext) && chNext != '\'')
				|| (chNext == '.' && chNext2 == '.')) {
				// ".." is always an operator if preceded by a SCE_PL_WORD.
				// Archaic Perl has quotes inside names
				if (isMatch(styler, lengthDoc, styler.GetStartSegment(), "__DATA__")) {
					styler.ColourTo(i, SCE_PL_DATASECTION);
					state = SCE_PL_DATASECTION;
				} else if (isMatch(styler, lengthDoc, styler.GetStartSegment(), "__END__")) {
					styler.ColourTo(i, SCE_PL_DATASECTION);
					state = SCE_PL_DATASECTION;
				} else {
					if (classifyWordPerl(styler.GetStartSegment(), i, keywords, styler) == SCE_PL_WORD)
						preferRE = true;
					state = SCE_PL_DEFAULT;
					ch = ' ';
				}
			}
		} else {
			if (state == SCE_PL_COMMENTLINE) {
				if (isEOLChar(ch)) {
					styler.ColourTo(i - 1, state);
					state = SCE_PL_DEFAULT;
				}
			} else if (state == SCE_PL_HERE_DELIM) {
				//
				// From perldata.pod:
				// ------------------
				// A line-oriented form of quoting is based on the shell ``here-doc''
				// syntax.
				// Following a << you specify a string to terminate the quoted material,
				// and all lines following the current line down to the terminating
				// string are the value of the item.
				// The terminating string may be either an identifier (a word),
				// or some quoted text.
				// If quoted, the type of quotes you use determines the treatment of
				// the text, just as in regular quoting.
				// An unquoted identifier works like double quotes.
				// There must be no space between the << and the identifier.
				// (If you put a space it will be treated as a null identifier,
				// which is valid, and matches the first empty line.)
				// The terminating string must appear by itself (unquoted and with no
				// surrounding whitespace) on the terminating line.
				//
				if (HereDoc.State == 0) { // '<<' encountered
					HereDoc.State = 1;
					HereDoc.Quote = chNext;
					HereDoc.Quoted = false;
					HereDoc.DelimiterLength = 0;
					HereDoc.Delimiter[HereDoc.DelimiterLength] = '\0';
					if (chNext == '\'' || chNext == '"' || chNext == '`') { // a quoted here-doc delimiter
						i++;
						ch = chNext;
						chNext = chNext2;
						HereDoc.Quoted = true;
					} else if (chNext == '\\') { // ref?
						i++;
						ch = chNext;
						chNext = chNext2;
					} else if (isalnum(chNext) || chNext == '_') { // an unquoted here-doc delimiter
					}
					else if (isspacechar(chNext)) { // deprecated here-doc delimiter || TODO: left shift operator
					}
					else { // TODO: ???
					}

				} else if (HereDoc.State == 1) { // collect the delimiter
					if (HereDoc.Quoted) { // a quoted here-doc delimiter
						if (ch == HereDoc.Quote) { // closing quote => end of delimiter
							styler.ColourTo(i, state);
							state = SCE_PL_DEFAULT;
							i++;
							ch = chNext;
							chNext = chNext2;
						} else {
							if (ch == '\\' && chNext == HereDoc.Quote) { // escaped quote
								i++;
								ch = chNext;
								chNext = chNext2;
							}
							HereDoc.Delimiter[HereDoc.DelimiterLength++] = ch;
							HereDoc.Delimiter[HereDoc.DelimiterLength] = '\0';
						}
					} else { // an unquoted here-doc delimiter
						if (isalnum(ch) || ch == '_') {
							HereDoc.Delimiter[HereDoc.DelimiterLength++] = ch;
							HereDoc.Delimiter[HereDoc.DelimiterLength] = '\0';
						} else {
							styler.ColourTo(i - 1, state);
							state = SCE_PL_DEFAULT;
						}
					}
					if (HereDoc.DelimiterLength >= static_cast<int>(sizeof(HereDoc.Delimiter)) - 1) {
						styler.ColourTo(i - 1, state);
						state = SCE_PL_ERROR;
					}
				}
			} else if (HereDoc.State == 2) {
				// state == SCE_PL_HERE_Q || state == SCE_PL_HERE_QQ || state == SCE_PL_HERE_QX
				if (isEOLChar(chPrev) && isMatch(styler, lengthDoc, i, HereDoc.Delimiter)) {
					i += HereDoc.DelimiterLength;
					chNext = styler.SafeGetCharAt(i);
					if (isEOLChar(chNext)) {
						styler.ColourTo(i - 1, state);
						state = SCE_PL_DEFAULT;
						HereDoc.State = 0;
					}
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (state == SCE_PL_POD) {
				if (ch == '=' && isEOLChar(chPrev)) {
					if (isMatch(styler, lengthDoc, i, "=cut")) {
						styler.ColourTo(i - 1 + 4, state);
						i += 4;
						state = SCE_PL_DEFAULT;
						ch = styler.SafeGetCharAt(i);
						chNext = styler.SafeGetCharAt(i + 1);
					}
				}
			} else if (state == SCE_PL_SCALAR) {
				if (isEndVar(ch)) {
					if (i == (styler.GetStartSegment() + 1)) {
						// Special variable: $(, $_ etc.
						styler.ColourTo(i, state);
					} else {
						styler.ColourTo(i - 1, state);
					}
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
			} else if (state == SCE_PL_REGEX
				|| state == SCE_PL_STRING_QR
				) {
				if (!Quote.Up && !isspacechar(ch)) {
					Quote.Open(ch);
				} else if (ch == '\\' && Quote.Up != '\\') {
					// SG: Is it save to skip *every* escaped char?
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				} else {
					if (ch == Quote.Down /*&& chPrev != '\\'*/) {
						Quote.Count--;
						if (Quote.Count == 0) {
							Quote.Rep--;
							if (Quote.Up == Quote.Down) {
								Quote.Count++;
							}
						}
						if (!isalpha(chNext)) {
							if (Quote.Rep <= 0) {
								styler.ColourTo(i, state);
								state = SCE_PL_DEFAULT;
								ch = ' ';
							}
						}
					} else if (ch == Quote.Up /*&& chPrev != '\\'*/) {
						Quote.Count++;
					} else if (!isalpha(chNext)) {
						if (Quote.Rep <= 0) {
							styler.ColourTo(i, state);
							state = SCE_PL_DEFAULT;
							ch = ' ';
						}
					}
				}
			} else if (state == SCE_PL_REGSUBST) {
				if (!Quote.Up && !isspacechar(ch)) {
					Quote.Open(ch);
				} else if (ch == '\\' && Quote.Up != '\\') {
					// SG: Is it save to skip *every* escaped char?
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				} else {
					if (Quote.Count == 0 && Quote.Rep == 1) {
						/* We matched something like s(...) or tr{...}
						* and are looking for the next matcher characters,
						* which could be either bracketed ({...}) or non-bracketed
						* (/.../).
						*
						* Number-signs are problematic.  If they occur after
						* the close of the first part, treat them like
						* a Quote.Up char, even if they actually start comments.
						*
						* If we find an alnum, we end the regsubst, and punt.
						*
						* Eric Promislow   ericp@activestate.com  Aug 9,2000
						*/
						if (isspacechar(ch)) {
							// Keep going
						}
						else if (isalnum(ch)) {
							styler.ColourTo(i, state);
							state = SCE_PL_DEFAULT;
							ch = ' ';
						} else {
							Quote.Open(ch);
						}
					} else if (ch == Quote.Down /*&& chPrev != '\\'*/) {
						Quote.Count--;
						if (Quote.Count == 0) {
							Quote.Rep--;
						}
						if (!isalpha(chNext)) {
							if (Quote.Rep <= 0) {
								styler.ColourTo(i, state);
								state = SCE_PL_DEFAULT;
								ch = ' ';
							}
						}
						if (Quote.Up == Quote.Down) {
							Quote.Count++;
						}
					} else if (ch == Quote.Up /*&& chPrev != '\\'*/) {
						Quote.Count++;
					} else if (!isalpha(chNext)) {
						if (Quote.Rep <= 0) {
							styler.ColourTo(i, state);
							state = SCE_PL_DEFAULT;
							ch = ' ';
						}
					}
				}
			} else if (state == SCE_PL_STRING_Q
				|| state == SCE_PL_STRING_QQ
				|| state == SCE_PL_STRING_QX
				|| state == SCE_PL_STRING_QW
				|| state == SCE_PL_STRING
				|| state == SCE_PL_CHARACTER
				|| state == SCE_PL_BACKTICKS
				) {
				if (!Quote.Down && !isspacechar(ch)) {
					Quote.Open(ch);
				} else if (ch == '\\' && Quote.Up != '\\') {
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				} else if (ch == Quote.Down) {
					Quote.Count--;
					if (Quote.Count == 0) {
						Quote.Rep--;
						if (Quote.Rep <= 0) {
							styler.ColourTo(i, state);
							state = SCE_PL_DEFAULT;
							ch = ' ';
						}
						if (Quote.Up == Quote.Down) {
							Quote.Count++;
						}
					}
				} else if (ch == Quote.Up) {
					Quote.Count++;
				}
			}

			if (state == SCE_PL_DEFAULT) {    // One of the above succeeded
				if (ch == '#') {
					state = SCE_PL_COMMENTLINE;
				} else if (ch == '\"') {
					state = SCE_PL_STRING;
					Quote.New(1);
					Quote.Open(ch);
				} else if (ch == '\'') {
					state = SCE_PL_CHARACTER;
					Quote.New(1);
					Quote.Open(ch);
				} else if (iswordstart(ch)) {
					state = SCE_PL_WORD;
					preferRE = false;
				} else if (isPerlOperator(ch)) {
					if (ch == ')' || ch == ']')
						preferRE = false;
					else
						preferRE = true;
					styler.ColourTo(i, SCE_PL_OPERATOR);
				}
			}
		}
		if (state == SCE_PL_ERROR) {
			break;
		}
		chPrev = ch;
	}
	styler.ColourTo(lengthDoc - 1, state);
}

static const char * const perlWordListDesc[] = {
	"Perl keywords",
	0
};

LexerModule lmPerl(SCLEX_PERL, ColourisePerlDoc, "perl", 0, perlWordListDesc);
