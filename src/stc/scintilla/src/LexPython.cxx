// Scintilla source code edit control
/** @file LexPython.cxx
 ** Lexer for Python.
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

/* Returns true if the "as" word that begins at start follows an import statement */
static bool IsImportAs(unsigned int start, Accessor &styler) {
	unsigned int i;
	unsigned int j;
	char s[10];

	/* Find any import before start but after any statement terminator or quote */
	i = start;
	while (i > 0) {
		char ch = styler[i - 1];

		if (ch == '\n' || ch == '\r' || ch == ';' || ch == '\'' || ch == '"' || ch == '`')
			break;
		if (ch == 't' && i > 5) {
			for (j = 0; j < 6; j++)
				s[j] = styler[(i - 6) + j];
			s[j] = '\0';
			if (strcmp(s, "import") == 0)
				return true;
		}
		i--;
	}

        return false;
}

static void ClassifyWordPy(unsigned int start, unsigned int end, WordList &keywords, Accessor &styler, char *prevWord) {
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
	else if (strcmp(s, "as") == 0 && IsImportAs(start, styler))
		chAttr = SCE_P_WORD;
	// make sure that dot-qualifiers inside the word are lexed correct
	else for (unsigned int i = 0; i < end - start + 1; i++) {
		if (styler[start + i] == '.') {
			styler.ColourTo(start + i - 1, chAttr);
			styler.ColourTo(start + i, SCE_P_OPERATOR);
		}
	}
	styler.ColourTo(end, chAttr);
	strcpy(prevWord, s);
}

static bool IsPyComment(Accessor &styler, int pos, int len) {
	return len>0 && styler[pos]=='#';
}

static bool IsPyStringStart(char ch, char chNext, char chNext2) {
	if (ch == '\'' || ch == '"')
		return true;
	if (ch == 'u' || ch == 'U') {
		if (chNext == '"' || chNext == '\'')
			return true;
		if ((chNext == 'r' || chNext == 'R') && (chNext2 == '"' || chNext2 == '\''))
			return true;
	}
	if ((ch == 'r' || ch == 'R') && (chNext == '"' || chNext == '\''))
		return true;

	return false;
}

static bool IsPyWordStart(char ch, char chNext, char chNext2) {
	return (iswordchar(ch) && !IsPyStringStart(ch, chNext, chNext2));
}

/* Return the state to use for the string starting at i; *nextIndex will be set to the first index following the quote(s) */
static int GetPyStringState(Accessor &styler, int i, int *nextIndex) {
	char ch = styler.SafeGetCharAt(i);
	char chNext = styler.SafeGetCharAt(i + 1);

	// Advance beyond r, u, or ur prefix, but bail if there are any unexpected chars
	if (ch == 'r' || ch == 'R') {
		i++;
		ch = styler.SafeGetCharAt(i);
		chNext = styler.SafeGetCharAt(i + 1);
	}
	else if (ch == 'u' || ch == 'U') {
		if (chNext == 'r' || chNext == 'R')
			i += 2;
		else
			i += 1;
		ch = styler.SafeGetCharAt(i);
		chNext = styler.SafeGetCharAt(i + 1);
	}

	if (ch != '"' && ch != '\'') {
		*nextIndex = i + 1;
		return SCE_P_DEFAULT;
	}

	if (ch == chNext && ch == styler.SafeGetCharAt(i + 2)) {
		*nextIndex = i + 3;

		if (ch == '"')
			return SCE_P_TRIPLEDOUBLE;
		else
			return SCE_P_TRIPLE;
	} else {
		*nextIndex = i + 1;

		if (ch == '"')
			return SCE_P_STRING;
		else
			return SCE_P_CHARACTER;
	}
}

static void ColourisePyDoc(unsigned int startPos, int length, int initStyle,
						   WordList *keywordlists[], Accessor &styler) {

	int lengthDoc = startPos + length;

	// Backtrack to previous line in case need to fix its tab whinging
	int lineCurrent = styler.GetLine(startPos);
	if (startPos > 0) {
		if (lineCurrent > 0) {
			startPos = styler.LineStart(lineCurrent-1);
			if (startPos == 0)
				initStyle = SCE_P_DEFAULT;
			else
				initStyle = styler.StyleAt(startPos-1);
		}
	}

	// Python uses a different mask because bad indentation is marked by oring with 32
	styler.StartAt(startPos, 127);

	WordList &keywords = *keywordlists[0];

	int whingeLevel = styler.GetPropertyInt("tab.timmy.whinge.level");
	char prevWord[200];
	prevWord[0] = '\0';
	if (length == 0)
		return ;

	int state = initStyle & 31;

	int nextIndex = 0;
	char chPrev = ' ';
	char chPrev2 = ' ';
	char chNext = styler[startPos];
	styler.StartSegment(startPos);
	bool atStartLine = true;
	int spaceFlags = 0;
	styler.IndentAmount(lineCurrent, &spaceFlags, IsPyComment);
	for (int i = startPos; i < lengthDoc; i++) {

		if (atStartLine) {
			char chBad = static_cast<char>(64);
			char chGood = static_cast<char>(0);
			char chFlags = chGood;
			if (whingeLevel == 1) {
				chFlags = (spaceFlags & wsInconsistent) ? chBad : chGood;
			} else if (whingeLevel == 2) {
				chFlags = (spaceFlags & wsSpaceTab) ? chBad : chGood;
			} else if (whingeLevel == 3) {
				chFlags = (spaceFlags & wsSpace) ? chBad : chGood;
			} else if (whingeLevel == 4) {
				chFlags = (spaceFlags & wsTab) ? chBad : chGood;
			}
			styler.SetFlags(chFlags, static_cast<char>(state));
			atStartLine = false;
		}

		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		char chNext2 = styler.SafeGetCharAt(i + 2);

		if ((ch == '\r' && chNext != '\n') || (ch == '\n') || (i == lengthDoc)) {
			if ((state == SCE_P_DEFAULT) || (state == SCE_P_TRIPLE) || (state == SCE_P_TRIPLEDOUBLE)) {
				// Perform colourisation of white space and triple quoted strings at end of each line to allow
				// tab marking to work inside white space and triple quoted strings
				styler.ColourTo(i, state);
			}
			lineCurrent++;
			styler.IndentAmount(lineCurrent, &spaceFlags, IsPyComment);
			atStartLine = true;
		}

		if (styler.IsLeadByte(ch)) {
			chNext = styler.SafeGetCharAt(i + 2);
			chPrev = ' ';
			chPrev2 = ' ';
			i += 1;
			continue;
		}

		if (state == SCE_P_STRINGEOL) {
			if (ch != '\r' && ch != '\n') {
				styler.ColourTo(i - 1, state);
				state = SCE_P_DEFAULT;
			}
		}
		if (state == SCE_P_DEFAULT) {
			if (IsPyWordStart(ch, chNext, chNext2)) {
				styler.ColourTo(i - 1, state);
				state = SCE_P_WORD;
			} else if (ch == '#') {
				styler.ColourTo(i - 1, state);
				state = chNext == '#' ? SCE_P_COMMENTBLOCK : SCE_P_COMMENTLINE;
			} else if (IsPyStringStart(ch, chNext, chNext2)) {
				styler.ColourTo(i - 1, state);
				state = GetPyStringState(styler, i, &nextIndex);
				if (nextIndex != i + 1) {
					i = nextIndex - 1;
					ch = ' ';
					chPrev = ' ';
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_P_OPERATOR);
			}
		} else if (state == SCE_P_WORD) {
			if (!iswordchar(ch)) {
				ClassifyWordPy(styler.GetStartSegment(), i - 1, keywords, styler, prevWord);
				state = SCE_P_DEFAULT;
				if (ch == '#') {
					state = chNext == '#' ? SCE_P_COMMENTBLOCK : SCE_P_COMMENTLINE;
				} else if (IsPyStringStart(ch, chNext, chNext2)) {
					styler.ColourTo(i - 1, state);
					state = GetPyStringState(styler, i, &nextIndex);
					if (nextIndex != i + 1) {
						i = nextIndex - 1;
						ch = ' ';
						chPrev = ' ';
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (isoperator(ch)) {
					styler.ColourTo(i, SCE_P_OPERATOR);
				}
			}
		} else {
			if (state == SCE_P_COMMENTLINE || state == SCE_P_COMMENTBLOCK) {
				if (ch == '\r' || ch == '\n') {
					styler.ColourTo(i - 1, state);
					state = SCE_P_DEFAULT;
				}
			} else if (state == SCE_P_STRING) {
				if ((ch == '\r' || ch == '\n') && (chPrev != '\\')) {
					styler.ColourTo(i - 1, state);
					state = SCE_P_STRINGEOL;
				} else if (ch == '\\') {
					if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\"') {
					styler.ColourTo(i, state);
					state = SCE_P_DEFAULT;
				}
			} else if (state == SCE_P_CHARACTER) {
				if ((ch == '\r' || ch == '\n') && (chPrev != '\\')) {
					styler.ColourTo(i - 1, state);
					state = SCE_P_STRINGEOL;
				} else if (ch == '\\') {
					if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				} else if (ch == '\'') {
					styler.ColourTo(i, state);
					state = SCE_P_DEFAULT;
				}
			} else if (state == SCE_P_TRIPLE) {
				if (ch == '\'' && chPrev == '\'' && chPrev2 == '\'') {
					styler.ColourTo(i, state);
					state = SCE_P_DEFAULT;
				}
			} else if (state == SCE_P_TRIPLEDOUBLE) {
				if (ch == '\"' && chPrev == '\"' && chPrev2 == '\"') {
					styler.ColourTo(i, state);
					state = SCE_P_DEFAULT;
				}
			}
		}
		chPrev2 = chPrev;
		chPrev = ch;
	}
	if (state == SCE_P_WORD) {
		ClassifyWordPy(styler.GetStartSegment(), lengthDoc, keywords, styler, prevWord);
	} else {
		styler.ColourTo(lengthDoc, state);
	}
}

static bool IsCommentLine(int line, Accessor &styler) {
	int pos = styler.LineStart(line);
	int eol_pos = styler.LineStart(line+1) - 1;
	for (int i = pos; i < eol_pos; i++) {
		char ch = styler[i];
		if (ch == '#')
			return true;
		else if (ch != ' ' && ch != '\t')
			return false;
	}
	return false;
}

static bool IsQuoteLine(int line, Accessor &styler) {
	int style = styler.StyleAt(styler.LineStart(line)) & 31;
	return ((style == SCE_P_TRIPLE) || (style== SCE_P_TRIPLEDOUBLE));
}

static void FoldPyDoc(unsigned int startPos, int length, int /*initStyle - unused*/,
					WordList *[], Accessor &styler) {
	int maxPos = startPos + length;
	int maxLines = styler.GetLine(maxPos-1);
						
	bool foldComment = styler.GetPropertyInt("fold.comment.python");
	bool foldQuotes = styler.GetPropertyInt("fold.quotes.python");

	// Backtrack to previous non-blank line so we can determine indent level
	// for any white space lines (needed esp. within triple quoted strings)
	// and so we can fix any preceding fold level (which is why we go back
	// at least one line in all cases)
	int spaceFlags = 0;
	int lineCurrent = styler.GetLine(startPos);
	int indentCurrent = styler.IndentAmount(lineCurrent, &spaceFlags, NULL);
	while (lineCurrent > 0) {
		lineCurrent--;
		indentCurrent = styler.IndentAmount(lineCurrent, &spaceFlags, NULL);
		if (!(indentCurrent & SC_FOLDLEVELWHITEFLAG) &&
			(!IsCommentLine(lineCurrent, styler)) &&
			(!IsQuoteLine(lineCurrent, styler)))
			break;
	}
	int indentCurrentLevel = indentCurrent & SC_FOLDLEVELNUMBERMASK;
	
	startPos = styler.LineStart(lineCurrent);
	// Set up initial state
	int prev_state = SCE_P_DEFAULT & 31;
	if (lineCurrent >= 1)
		prev_state = styler.StyleAt(startPos-1) & 31;
	int prevQuote = foldQuotes && ((prev_state == SCE_P_TRIPLE) || (prev_state == SCE_P_TRIPLEDOUBLE));
	int prevComment = 0;
	if (lineCurrent >= 1)
		prevComment = foldComment && IsCommentLine(lineCurrent - 1, styler);

	// Process all characters to end of requested range or end of any triple quote
	// or comment that hangs over the end of the range
	while ((lineCurrent <= maxLines) || prevQuote || prevComment) {

		// Gather info
		int lev = indentCurrent;
		int lineNext = lineCurrent + 1;
		int style = styler.StyleAt(styler.LineStart(lineNext)) & 31;
		int indentNext = styler.IndentAmount(lineNext, &spaceFlags, NULL);
		int quote = foldQuotes && ((style == SCE_P_TRIPLE) || (style== SCE_P_TRIPLEDOUBLE));
		int quote_start = (quote && !prevQuote);
		int quote_continue = (quote && prevQuote);
		int comment = foldComment && IsCommentLine(lineCurrent, styler);
		int comment_start = (comment && !prevComment && 
			IsCommentLine(lineNext, styler) && (lev > SC_FOLDLEVELBASE));
		int comment_continue = (comment && prevComment);
		if ((!quote || !prevQuote) && !comment)
			indentCurrentLevel = indentCurrent & SC_FOLDLEVELNUMBERMASK;
		if (quote)
			indentNext = indentCurrentLevel;
		if (indentNext & SC_FOLDLEVELWHITEFLAG)
			indentNext = SC_FOLDLEVELWHITEFLAG | indentCurrentLevel;

		if (quote_start) {
			// Place fold point at start of triple quoted string
			lev |= SC_FOLDLEVELHEADERFLAG;
		} else if (quote_continue || prevQuote) {
			// Add level to rest of lines in the string
			lev = lev + 1;
		} else if (comment_start) {
			// Place fold point at start of a block of comments
			lev |= SC_FOLDLEVELHEADERFLAG;
		} else if (comment_continue) {
			// Add level to rest of lines in the block
			lev = lev + 1;
		}

		// Skip past any blank lines for next indent level info; we skip also comments
		// starting in column 0 which effectively folds them into surrounding code
		// rather than screwing up folding.  Then set indent level on the lines
		// we skipped to be same as maximum of current and next indent.  This approach
		// does a reasonable job of collapsing white space into surrounding code
		// without getting confused by white space at the start of an indented level.
		while (!quote &&
		       ((indentNext & SC_FOLDLEVELWHITEFLAG) || styler[styler.LineStart(lineNext)] == '#') &&
		       (lineNext < maxLines)) {
			int level = Platform::Maximum(indentCurrent, indentNext);
			if (indentNext & SC_FOLDLEVELWHITEFLAG)
				level = SC_FOLDLEVELWHITEFLAG | indentCurrentLevel;
			styler.SetLevel(lineNext, level);
			lineNext++;
			indentNext = styler.IndentAmount(lineNext, &spaceFlags, NULL);
		}

		// Set fold header on non-quote/non-comment line
		if (!quote && !comment && !(indentCurrent & SC_FOLDLEVELWHITEFLAG) ) {
			if ((indentCurrent & SC_FOLDLEVELNUMBERMASK) < (indentNext & SC_FOLDLEVELNUMBERMASK))
				lev |= SC_FOLDLEVELHEADERFLAG;
		}

		// Keep track of triple quote and block comment state of previous line
		prevQuote = quote;
		prevComment = comment_start || comment_continue;

		// Set fold level for this line and move to next line
		styler.SetLevel(lineCurrent, lev);
		indentCurrent = indentNext;
		lineCurrent = lineNext;
	}

	// Make sure last line indent level is set too
	styler.SetLevel(lineCurrent, indentCurrent);
}

LexerModule lmPython(SCLEX_PYTHON, ColourisePyDoc, "python", FoldPyDoc);
