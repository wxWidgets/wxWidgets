// Scintilla source code edit control
/** @file LexPerl.cxx
 ** Lexer for subset of Perl.
 **/
// Lexical analysis fixes by Kein-Hong Man <mkh@pl.jaring.my> 2003-2004
// Copyright 1998-2004 by Neil Hodgson <neilh@scintilla.org>
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

#define PERLNUM_DECIMAL 1
#define PERLNUM_NON_DEC 2
#define PERLNUM_FLOAT 3
#define PERLNUM_VECTOR 4
#define PERLNUM_V_VECTOR 5

#define HERE_DELIM_MAX 256

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
	if (ch == '^' || ch == '&' || ch == '\\' ||
	        ch == '(' || ch == ')' || ch == '-' || ch == '+' ||
	        ch == '=' || ch == '|' || ch == '{' || ch == '}' ||
	        ch == '[' || ch == ']' || ch == ':' || ch == ';' ||
	        ch == '>' || ch == ',' || 
	        ch == '?' || ch == '!' || ch == '.' || ch == '~')
		return true;
	// these chars are already tested before this call
	// ch == '%' || ch == '*' || ch == '<' || ch == '/' ||
	return false;
}

static int classifyWordPerl(unsigned int start, unsigned int end, WordList &keywords, Accessor &styler) {
	char s[100];
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = styler[start + i];
		s[i + 1] = '\0';
	}
	char chAttr = SCE_PL_IDENTIFIER;
	if (keywords.InList(s))
		chAttr = SCE_PL_WORD;
	styler.ColourTo(end, chAttr);
	return chAttr;
}

static inline bool isEndVar(char ch) {
	return !isalnum(ch) && ch != '#' && ch != '$' &&
	       ch != '_' && ch != '\'';
}


static inline bool isNonQuote(char ch) {
	return isalnum(ch) || ch == '_';
}

static inline char actualNumStyle(int numberStyle) {
	switch (numberStyle) {
	case PERLNUM_VECTOR:
	case PERLNUM_V_VECTOR:
		return SCE_PL_STRING;
	case PERLNUM_DECIMAL:
	case PERLNUM_NON_DEC:
	case PERLNUM_FLOAT:
	default:
		return SCE_PL_NUMBER;
	}
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
		char *Delimiter;	// the Delimiter, 256: sizeof PL_tokenbuf
		HereDocCls() {
			State = 0;
			DelimiterLength = 0;
			Delimiter = new char[HERE_DELIM_MAX];
			Delimiter[0] = '\0';
		}
		~HereDocCls() {
			delete []Delimiter;
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

	int state = initStyle;
	char numState = PERLNUM_DECIMAL;
	int dotCount = 0;
	unsigned int lengthDoc = startPos + length;
	//int sookedpos = 0; // these have no apparent use, see POD state
	//char sooked[100];
	//sooked[sookedpos] = '\0';

	// If in a long distance lexical state, seek to the beginning to find quote characters
	// Perl strings can be multi-line with embedded newlines, so backtrack.
	// Perl numbers have additional state during lexing, so backtrack too.
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
	|| state == SCE_PL_STRING
	|| state == SCE_PL_BACKTICKS
	|| state == SCE_PL_CHARACTER
	|| state == SCE_PL_NUMBER
	|| state == SCE_PL_IDENTIFIER
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
		// if the current character is not consumed due to the completion of an
		// earlier style, lexing can be restarted via a simple goto
	restartLexer:
		chNext = styler.SafeGetCharAt(i + 1);
		char chNext2 = styler.SafeGetCharAt(i + 2);

		if (styler.IsLeadByte(ch)) {
			chNext = styler.SafeGetCharAt(i + 2);
			chPrev = ' ';
			i += 1;
			continue;
		}
		if ((chPrev == '\r' && ch == '\n')) {	// skip on DOS/Windows
			styler.ColourTo(i, state);
			chPrev = ch;
			continue;
		}

		if (HereDoc.State == 1 && isEOLChar(ch)) {
			// Begin of here-doc (the line after the here-doc delimiter):
			// Lexically, the here-doc starts from the next line after the >>, but the
			// first line of here-doc seem to follow the style of the last EOL sequence
			HereDoc.State = 2;
			if (HereDoc.Quoted) {
				if (state == SCE_PL_HERE_DELIM) {
					// Missing quote at end of string! We are stricter than perl.
					// Colour here-doc anyway while marking this bit as an error.
					state = SCE_PL_ERROR;
				}
				styler.ColourTo(i - 1, state);
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
			} else {
				styler.ColourTo(i - 1, state);
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
			if (isdigit(ch) || (isdigit(chNext) &&
				(ch == '.' || ch == 'v'))) {
				state = SCE_PL_NUMBER;
				numState = PERLNUM_DECIMAL;
				dotCount = 0;
				if (ch == '0') {	// hex,bin,octal
					if (chNext == 'x' || chNext == 'b' || isdigit(chNext)) {
						numState = PERLNUM_NON_DEC;
					}
				} else if (ch == 'v') {	// vector
					numState = PERLNUM_V_VECTOR;
				}
			} else if (iswordstart(ch)) {
				if (chPrev == '>' && styler.SafeGetCharAt(i - 2) == '-') {
					state = SCE_PL_IDENTIFIER;	// part of "->" expr
					if ((!iswordchar(chNext) && chNext != '\'')
						|| (chNext == '.' && chNext2 == '.')) {
						// We need that if length of word == 1!
						styler.ColourTo(i, SCE_PL_IDENTIFIER);
						state = SCE_PL_DEFAULT;
					}
				} else if (ch == 's' && !isNonQuote(chNext)) {
					state = SCE_PL_REGSUBST;
					Quote.New(2);
				} else if (ch == 'm' && !isNonQuote(chNext)) {
					state = SCE_PL_REGEX;
					Quote.New(1);
				} else if (ch == 'q' && !isNonQuote(chNext)) {
					state = SCE_PL_STRING_Q;
					Quote.New(1);
				} else if (ch == 'y' && !isNonQuote(chNext)) {
					state = SCE_PL_REGSUBST;
					Quote.New(2);
				} else if (ch == 't' && chNext == 'r' && !isNonQuote(chNext2)) {
					state = SCE_PL_REGSUBST;
					Quote.New(2);
					i++;
					chNext = chNext2;
				} else if (ch == 'q' && (chNext == 'q' || chNext == 'r' || chNext == 'w' || chNext == 'x') && !isNonQuote(chNext2)) {
					if      (chNext == 'q') state = SCE_PL_STRING_QQ;
					else if (chNext == 'x') state = SCE_PL_STRING_QX;
					else if (chNext == 'r') state = SCE_PL_STRING_QR;
					else if (chNext == 'w') state = SCE_PL_STRING_QW;
					i++;
					chNext = chNext2;
					Quote.New(1);
				} else if (ch == 'x' && (chNext == '=' ||	// repetition
					   (chNext != '_' && !isalnum(chNext)) ||
					   (isdigit(chPrev) && isdigit(chNext)))) {
					styler.ColourTo(i, SCE_PL_OPERATOR);
				} else {
					state = SCE_PL_WORD;
					if ((!iswordchar(chNext) && chNext != '\'')
						|| (chNext == '.' && chNext2 == '.')) {
						// We need that if length of word == 1!
						// This test is copied from the SCE_PL_WORD handler.
						classifyWordPerl(styler.GetStartSegment(), i, keywords, styler);
						state = SCE_PL_DEFAULT;
					}
				}
			} else if (ch == '#') {
				state = SCE_PL_COMMENTLINE;
			} else if (ch == '\"') {
				state = SCE_PL_STRING;
				Quote.New(1);
				Quote.Open(ch);
			} else if (ch == '\'') {
				if (chPrev == '&') {
					// Archaic call
					styler.ColourTo(i, state);
				} else {
					state = SCE_PL_CHARACTER;
					Quote.New(1);
					Quote.Open(ch);
				}
			} else if (ch == '`') {
				state = SCE_PL_BACKTICKS;
				Quote.New(1);
				Quote.Open(ch);
			} else if (ch == '$') {
				if ((chNext == '{') || isspacechar(chNext)) {
					styler.ColourTo(i, SCE_PL_SCALAR);
				} else {
					state = SCE_PL_SCALAR;
					if (chNext == '`' && chNext2 == '`') {
						i += 2;
						ch = styler.SafeGetCharAt(i);
						chNext = styler.SafeGetCharAt(i + 1);
					} else {
						i++;
						ch = chNext;
						chNext = chNext2;
					}
				}
			} else if (ch == '@') {
				if (isalpha(chNext) || chNext == '#' || chNext == '$'
						    || chNext == '_' || chNext == '+') {
					state = SCE_PL_ARRAY;
				} else if (chNext != '{' && chNext != '[') {
					styler.ColourTo(i, SCE_PL_ARRAY);
					i++;
					ch = ' ';
				} else {
					styler.ColourTo(i, SCE_PL_ARRAY);
				}
			} else if (ch == '%') {
				if (isalpha(chNext) || chNext == '#' || chNext == '$' || chNext == '_') {
					state = SCE_PL_HASH;
				} else if (chNext == '{') {
					styler.ColourTo(i, SCE_PL_HASH);
				} else {
					styler.ColourTo(i, SCE_PL_OPERATOR);
				}
			} else if (ch == '*') {
				if (isalpha(chNext) || chNext == '_' || chNext == '{') {
					state = SCE_PL_SYMBOLTABLE;
				} else {
					if (chNext == '*') {	// exponentiation
						i++;
						ch = chNext;
						chNext = chNext2;
					}
					styler.ColourTo(i, SCE_PL_OPERATOR);
				}
			} else if (ch == '/') {
				// Explicit backward peeking to set a consistent preferRE for
				// any slash found, so no longer need to track preferRE state.
				// Find first previous significant lexed element and interpret.
				bool preferRE = false;
				unsigned int bk = (i > 0)? i - 1: 0;
				char bkch;
				styler.Flush();
				while ((bk > 0) && (styler.StyleAt(bk) == SCE_PL_DEFAULT ||
					styler.StyleAt(bk) == SCE_PL_COMMENTLINE)) {
					bk--;
				}
				if (bk == 0) {
					// position 0 won't really be checked; rarely happens
					// hard to fix due to an unsigned index i
					preferRE = true;
				} else {
					int bkstyle = styler.StyleAt(bk);
					bkch = styler.SafeGetCharAt(bk);
					switch(bkstyle) {
					case SCE_PL_OPERATOR:
						preferRE = true;
						if (bkch == ')' || bkch == ']') {
							preferRE = false;
						} else if (bkch == '}') {
							// backtrack further, count balanced brace pairs
							// if a brace pair found, see if it's a variable
							int braceCount = 1;
							while (--bk > 0) {
								bkstyle = styler.StyleAt(bk);
								if (bkstyle == SCE_PL_OPERATOR) {
									bkch = styler.SafeGetCharAt(bk);
									if (bkch == ';') {	// early out
										break;
									} else if (bkch == '}') {
										braceCount++;
									} else if (bkch == '{') {
										if (--braceCount == 0)
											break;
									}
								}
							}
							if (bk == 0) {
								// at beginning, true
							} else if (braceCount == 0) {
								// balanced { found, bk>0, skip more whitespace
								if (styler.StyleAt(--bk) == SCE_PL_DEFAULT) {
									while (bk > 0) {
										bkstyle = styler.StyleAt(--bk);
										if (bkstyle != SCE_PL_DEFAULT)
											break;
									}
								}
								bkstyle = styler.StyleAt(bk);
								if (bkstyle == SCE_PL_SCALAR
								 || bkstyle == SCE_PL_ARRAY
								 || bkstyle == SCE_PL_HASH
								 || bkstyle == SCE_PL_SYMBOLTABLE
								 || bkstyle == SCE_PL_OPERATOR) {
									preferRE = false;
								}
							}
						}
						break;
					case SCE_PL_IDENTIFIER:
						preferRE = true;
						if (bkch == '>') {	// inputsymbol
							preferRE = false;
							break;
						}
						// backtrack to find "->" or "::" before identifier
						while (bk > 0 && styler.StyleAt(bk) == SCE_PL_IDENTIFIER) {
							bk--;
						}
						while (bk > 0) {
							bkstyle = styler.StyleAt(bk);
							if (bkstyle == SCE_PL_DEFAULT ||
							    bkstyle == SCE_PL_COMMENTLINE) {
							} else if (bkstyle == SCE_PL_OPERATOR) {
								// gcc 3.2.3 bloats if more compact form used
								bkch = styler.SafeGetCharAt(bk);
								if (bkch == '>') { // "->"
									if (styler.SafeGetCharAt(bk - 1) == '-') {
										preferRE = false;
										break;
									}
								} else if (bkch == ':') { // "::"
									if (styler.SafeGetCharAt(bk - 1) == ':') {
										preferRE = false;
										break;
									}
								}
							} else {// bare identifier, usually a function call but Perl
								// optimizes them as pseudo-constants, then the next
								// '/' will be a divide; favour divide over regex
								// if there is a whitespace after the '/'
								if (isspacechar(chNext)) {
									preferRE = false;
								}
								break;
							}
							bk--;
						}
						break;
					// other styles uses the default, preferRE=false
					case SCE_PL_WORD:
					case SCE_PL_POD:
					case SCE_PL_HERE_Q:
					case SCE_PL_HERE_QQ:
					case SCE_PL_HERE_QX:
						preferRE = true;
						break;
					}
				}
				if (preferRE) {
					state = SCE_PL_REGEX;
					Quote.New(1);
					Quote.Open(ch);
				} else {
					styler.ColourTo(i, SCE_PL_OPERATOR);
				}
			} else if (ch == '<' && chNext == '<') {
				state = SCE_PL_HERE_DELIM;
				HereDoc.State = 0;
			} else if (ch == '<') {
				// looks forward for matching > on same line
				unsigned int fw = i + 1;
				while (fw < lengthDoc) {
					char fwch = styler.SafeGetCharAt(fw);
					if (isEOLChar(fwch) || isspacechar(fwch))
						break;
					else if (fwch == '>') {
						if ((fw - i) == 2 &&	// '<=>' case
						    styler.SafeGetCharAt(fw-1) == '=') {
							styler.ColourTo(fw, SCE_PL_OPERATOR);
						} else {
							styler.ColourTo(fw, SCE_PL_IDENTIFIER);
						}
						i = fw;
						ch = fwch;
						chNext = styler.SafeGetCharAt(i+1);
					}
					fw++;
				}
				styler.ColourTo(i, SCE_PL_OPERATOR);
			} else if (ch == '='	// POD
			           && isalpha(chNext)
			           && (isEOLChar(chPrev))) {
				state = SCE_PL_POD;
				//sookedpos = 0;
				//sooked[sookedpos] = '\0';
			} else if (ch == '-'	// file test operators
			           && isSingleCharOp(chNext)
			           && !isalnum((chNext2 = styler.SafeGetCharAt(i+2)))) {
				styler.ColourTo(i + 1, SCE_PL_WORD);
				state = SCE_PL_DEFAULT;
				i++;
				ch = chNext;
				chNext = chNext2;
			} else if (isPerlOperator(ch)) {
				if (ch == '.' && chNext == '.') { // .. and ...
					i++;
					if (chNext2 == '.') { i++; }
					state = SCE_PL_DEFAULT;
					ch = styler.SafeGetCharAt(i);
					chNext = styler.SafeGetCharAt(i + 1);
				}
				styler.ColourTo(i, SCE_PL_OPERATOR);
			} else {
				// keep colouring defaults to make restart easier
				styler.ColourTo(i, SCE_PL_DEFAULT);
			}
		} else if (state == SCE_PL_NUMBER) {
			if (ch == '.') {
				if (chNext == '.') {
					// double dot is always an operator
					goto numAtEnd;
				} else if (numState == PERLNUM_NON_DEC || numState == PERLNUM_FLOAT) {
					// non-decimal number or float exponent, consume next dot
					styler.ColourTo(i - 1, SCE_PL_NUMBER);
					styler.ColourTo(i, SCE_PL_OPERATOR);
					state = SCE_PL_DEFAULT;
				} else { // decimal or vectors allows dots
					dotCount++;
					if (numState == PERLNUM_DECIMAL) {
						if (dotCount > 1) {
							if (isdigit(chNext)) { // really a vector
								numState = PERLNUM_VECTOR;
							} else	// number then dot
								goto numAtEnd;
						}
					} else { // vectors
						if (!isdigit(chNext))	// vector then dot
							goto numAtEnd;
					}
				}
			} else if (ch == '_' && numState == PERLNUM_DECIMAL) {
				if (!isdigit(chNext)) {
					goto numAtEnd;
				}
			} else if (isalnum(ch)) {
				if (numState == PERLNUM_VECTOR || numState == PERLNUM_V_VECTOR) {
					if (isalpha(ch)) {
						if (dotCount == 0) { // change to word
							state = SCE_PL_WORD;
						} else { // vector then word
							goto numAtEnd;
						}
					}
				} else if (numState == PERLNUM_DECIMAL) {
					if (ch == 'E' || ch == 'e') { // exponent
						numState = PERLNUM_FLOAT;
						if (chNext == '+' || chNext == '-') {
							i++;
							ch = chNext;
							chNext = chNext2;
						}
					} else if (!isdigit(ch)) { // number then word
						goto numAtEnd;
					}
				} else if (numState == PERLNUM_FLOAT) {
					if (!isdigit(ch)) { // float then word
						goto numAtEnd;
					}
				} else {// (numState == PERLNUM_NON_DEC)
					// allow alphanum for bin,hex,oct for now
				}
			} else {
				// complete current number or vector
			numAtEnd:
				styler.ColourTo(i - 1, actualNumStyle(numState));
				state = SCE_PL_DEFAULT;
				goto restartLexer;
			}
		} else if (state == SCE_PL_WORD) {
			if ((!iswordchar(chNext) && chNext != '\'')
				|| chNext == '.') {
				// ".." is always an operator if preceded by a SCE_PL_WORD.
				// "." never used in Perl variable names
				// Archaic Perl has quotes inside names
				if (isMatch(styler, lengthDoc, styler.GetStartSegment(), "__DATA__")
				 || isMatch(styler, lengthDoc, styler.GetStartSegment(), "__END__")) {
					styler.ColourTo(i, SCE_PL_DATASECTION);
					state = SCE_PL_DATASECTION;
				} else {
					classifyWordPerl(styler.GetStartSegment(), i, keywords, styler);
					state = SCE_PL_DEFAULT;
					ch = ' ';
				}
			}
		} else if (state == SCE_PL_IDENTIFIER) {
			if ((!iswordchar(chNext) && chNext != '\'')
				|| chNext == '.') {
				styler.ColourTo(i, SCE_PL_IDENTIFIER);
				state = SCE_PL_DEFAULT;
				ch = ' ';
			}
		} else {
			if (state == SCE_PL_COMMENTLINE) {
				if (isEOLChar(ch)) {
					styler.ColourTo(i - 1, state);
					state = SCE_PL_DEFAULT;
					goto restartLexer;
				} else if (isEOLChar(chNext)) {
					styler.ColourTo(i, state);
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
				// (This is deprecated, -w warns of this syntax)
				// The terminating string must appear by itself (unquoted and with no
				// surrounding whitespace) on the terminating line.
				//
				// From Bash info:
				// ---------------
				// Specifier format is: <<[-]WORD
				// Optional '-' is for removal of leading tabs from here-doc.
				// Whitespace acceptable after <<[-] operator.
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
					} else if (isalpha(chNext) || chNext == '_') {
						// an unquoted here-doc delimiter, no special handling
					} else if (isspacechar(chNext) || isdigit(chNext) || chNext == '\\'
						|| chNext == '=' || chNext == '$' || chNext == '@') {
						// left shift << or <<= operator cases
						styler.ColourTo(i, SCE_PL_OPERATOR);
						state = SCE_PL_DEFAULT;
						HereDoc.State = 0;
					} else {
						// symbols terminates; deprecated zero-length delimiter
					}

				} else if (HereDoc.State == 1) { // collect the delimiter
					if (HereDoc.Quoted) { // a quoted here-doc delimiter
						if (ch == HereDoc.Quote) { // closing quote => end of delimiter
							styler.ColourTo(i, state);
							state = SCE_PL_DEFAULT;
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
							goto restartLexer;
						}
					}
					if (HereDoc.DelimiterLength >= HERE_DELIM_MAX - 1) {
						styler.ColourTo(i - 1, state);
						state = SCE_PL_ERROR;
						goto restartLexer;
					}
				}
			} else if (HereDoc.State == 2) {
				// state == SCE_PL_HERE_Q || state == SCE_PL_HERE_QQ || state == SCE_PL_HERE_QX
				if (isEOLChar(chPrev) && isMatch(styler, lengthDoc, i, HereDoc.Delimiter)) {
					i += HereDoc.DelimiterLength;
					chPrev = styler.SafeGetCharAt(i - 1);
					ch = styler.SafeGetCharAt(i);
					if (isEOLChar(ch)) {
						styler.ColourTo(i - 1, state);
						state = SCE_PL_DEFAULT;
						HereDoc.State = 0;
						goto restartLexer;
					}
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (state == SCE_PL_POD) {
				if (ch == '=' && isEOLChar(chPrev)) {
					if (isMatch(styler, lengthDoc, i, "=cut")) {
						styler.ColourTo(i - 1 + 4, state);
						i += 4;
						state = SCE_PL_DEFAULT;
						ch = styler.SafeGetCharAt(i);
						//chNext = styler.SafeGetCharAt(i + 1);
						goto restartLexer;
					}
				}
			} else if (state == SCE_PL_SCALAR	// variable names
				|| state == SCE_PL_ARRAY
				|| state == SCE_PL_HASH
				|| state == SCE_PL_SYMBOLTABLE) {
				if (ch == ':' && chNext == ':') {	// skip ::
					i++;
					ch = chNext;
					chNext = chNext2;
				}
				else if (isEndVar(ch)) {
					if ((state == SCE_PL_SCALAR || state == SCE_PL_ARRAY)
					    && i == (styler.GetStartSegment() + 1)) {
						// Special variable: $(, $_ etc.
						styler.ColourTo(i, state);
						state = SCE_PL_DEFAULT;
					} else {
						styler.ColourTo(i - 1, state);
						state = SCE_PL_DEFAULT;
						goto restartLexer;
					}
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
		}
		if (state == SCE_PL_ERROR) {
			break;
		}
		chPrev = ch;
	}
	styler.ColourTo(lengthDoc - 1, state);
}

static void FoldPerlDoc(unsigned int startPos, int length, int, WordList *[],
                            Accessor &styler) {
	bool foldComment = styler.GetPropertyInt("fold.comment") != 0;
	bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;
	unsigned int endPos = startPos + length;
	int visibleChars = 0;
	int lineCurrent = styler.GetLine(startPos);
	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;
	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	for (unsigned int i = startPos; i < endPos; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		int style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		if (foldComment && (style == SCE_PL_COMMENTLINE)) {
			if ((ch == '/') && (chNext == '/')) {
				char chNext2 = styler.SafeGetCharAt(i + 2);
				if (chNext2 == '{') {
					levelCurrent++;
				} else if (chNext2 == '}') {
					levelCurrent--;
				}
			}
		}
		if (style == SCE_C_OPERATOR) {
			if (ch == '{') {
				levelCurrent++;
			} else if (ch == '}') {
				levelCurrent--;
			}
		}
		if (atEOL) {
			int lev = levelPrev;
			if (visibleChars == 0 && foldCompact)
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

static const char * const perlWordListDesc[] = {
	"Keywords",
	0
};

LexerModule lmPerl(SCLEX_PERL, ColourisePerlDoc, "perl", FoldPerlDoc, perlWordListDesc);

