// Scintilla source code edit control
/** @file LexFortran.cxx
 ** Lexer for Fortran.
 ** Writen by Chuan-jian Shen, Last changed Nov. 2002
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
#include "StyleContext.h"
#include "KeyWords.h"
#include "Scintilla.h"
#include "SciLexer.h"

static inline bool IsAWordChar(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '_' || ch == '%');
}

static inline bool IsAWordStart(const int ch) {
	return (ch < 0x80) && (isalnum(ch));
}

inline bool IsABlank(unsigned int ch) {
    return (ch == ' ') || (ch == 0x09) || (ch == 0x0b) ;
}
static void ColouriseFortranDoc(unsigned int startPos, int length, int initStyle, WordList *keywordlists[],
                            Accessor &styler, bool isFixFormat) {

	WordList &keywords = *keywordlists[0];
	WordList &keywords2 = *keywordlists[1];
	WordList &keywords3 = *keywordlists[2];

	int posLineStart = 0, prevState = 0;
	int endPos = startPos + length;

	// backtrack to the beginning of the document, this may be slow for big documents.
	// initStyle = SCE_F_DEFAULT;
	// StyleContext sc(0, startPos+length, initStyle, styler);

	// backtrack to the nearest keyword
	while ((startPos > 1) && (styler.StyleAt(startPos) != SCE_F_WORD)) {
		startPos--;
	}
	startPos = styler.LineStart(styler.GetLine(startPos));
	initStyle = styler.StyleAt(startPos - 1);
	StyleContext sc(startPos, endPos-startPos, initStyle, styler);

	for (; sc.More(); sc.Forward()) {

		// remember the position of the line
		if (sc.atLineStart) {
			posLineStart = sc.currentPos;
			sc.SetState(SCE_F_DEFAULT);
		}

		// Handle line continuation generically.
		if (sc.ch == '&') {
			char chTemp = ' ';
			int j = 1;
			while (IsABlank(chTemp) && j<132) {
				chTemp = static_cast<char>(sc.GetRelative(j));
				j ++;
			}
			if (chTemp == '!') {
				sc.SetState(SCE_F_CONTINUATION);
				if (sc.chNext == '!') sc.ForwardSetState(SCE_F_COMMENT);
			} else if (chTemp == '\r' || chTemp == '\n') {
				int currentState = sc.state;
				sc.SetState(SCE_F_CONTINUATION);
				if (currentState == SCE_F_STRING1 || currentState == SCE_F_STRING2) {
					sc.ForwardSetState(SCE_F_DEFAULT);
					while (IsASpace(sc.ch) && sc.More()) sc.Forward();
					if (sc.ch == '&') {
						sc.SetState(SCE_F_CONTINUATION);
						sc.Forward();
					}
					sc.SetState(currentState);
				}
			}
			continue;
		}

		// Determine if the current state should terminate.
		if (sc.state == SCE_F_OPERATOR) {
			sc.SetState(SCE_F_DEFAULT);
		} else if (sc.state == SCE_F_NUMBER) {
			if (!IsAWordChar(sc.ch)) {
				sc.SetState(SCE_F_DEFAULT);
			}
		} else if (sc.state == SCE_F_IDENTIFIER) {
			if (!IsAWordChar(sc.ch) || (sc.ch == '%')) {
				char s[100];
				sc.GetCurrentLowered(s, sizeof(s));
				if (keywords.InList(s)) {
					sc.ChangeState(SCE_F_WORD);
				} else if (keywords2.InList(s)) {
					sc.ChangeState(SCE_F_WORD2);
				} else if (keywords3.InList(s)) {
					sc.ChangeState(SCE_F_WORD3);
				}
				sc.SetState(SCE_F_DEFAULT);
			}
		} else if (sc.state == SCE_F_COMMENT) {
			if (sc.ch == '\r' || sc.ch == '\n') {
				sc.SetState(SCE_F_DEFAULT);
			}
		} else if (sc.state == SCE_F_STRING1) {
			prevState = sc.state;
			if (sc.ch == '\'') {
				if (sc.chNext == '\'') {
					sc.Forward();
				} else {
					sc.ForwardSetState(SCE_F_DEFAULT);
					prevState = SCE_F_DEFAULT;
				}
			} else if (sc.atLineEnd) {
				if (isFixFormat) {
					sc.ForwardSetState(SCE_F_DEFAULT);
					posLineStart = sc.currentPos;
				} else {
					sc.ChangeState(SCE_F_STRINGEOL);
					sc.ForwardSetState(SCE_F_DEFAULT);
				}
			}
		} else if (sc.state == SCE_F_STRING2) {
			prevState = sc.state;
			if (sc.atLineEnd) {
				if (isFixFormat) {
					sc.ForwardSetState(SCE_F_DEFAULT);
					posLineStart = sc.currentPos;
				} else {
					sc.ChangeState(SCE_F_STRINGEOL);
					sc.ForwardSetState(SCE_F_DEFAULT);
				}
			} else if (sc.ch == '\"') {
				if (sc.chNext == '\"') {
					sc.Forward();
				} else {
					sc.ForwardSetState(SCE_F_DEFAULT);
					prevState = SCE_F_DEFAULT;
				}
			}
		} else if (sc.state == SCE_F_OPERATOR2) {
			if (sc.ch == '.') {
				sc.ForwardSetState(SCE_F_DEFAULT);
			}
		} else if (sc.state == SCE_F_CONTINUATION) {
			sc.SetState(SCE_F_DEFAULT);
		} else if (sc.state == SCE_F_LABEL) {
			if (sc.currentPos >= static_cast<unsigned int>(posLineStart+5)) {
				sc.SetState(SCE_F_DEFAULT);
			}
		}

		// Determine if a new state should be entered.
		if (sc.state == SCE_F_DEFAULT) {
			int toLineStart = sc.currentPos - posLineStart;
			if (isFixFormat && (toLineStart < 6 || toLineStart > 72)) {
				if (sc.atLineStart && (tolower(sc.ch) == 'c' || sc.ch == '*') || sc.ch == '!') {
					sc.SetState(SCE_F_COMMENT);
				} else if (toLineStart > 72) {
					sc.SetState(SCE_F_COMMENT);
				} else if (toLineStart < 5 && !IsASpace(sc.ch)) {
					sc.SetState(SCE_F_LABEL);
				} else if (toLineStart == 5 && (!IsASpace(sc.ch) && sc.ch != '0')) {
					sc.SetState(SCE_F_CONTINUATION);
					sc.ForwardSetState(prevState);
				}
			} else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_F_NUMBER);
			} else if (sc.ch == '.' && isalpha(sc.chNext)) {
				sc.SetState(SCE_F_OPERATOR2);
			} else if (IsAWordStart(sc.ch)) {
				sc.SetState(SCE_F_IDENTIFIER);
			} else if (sc.ch == '!') {
				sc.SetState(SCE_F_COMMENT);
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_F_STRING2);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_F_STRING1);
			} else if (isoperator(static_cast<char>(sc.ch))) {
				sc.SetState(SCE_F_OPERATOR);
			}
		}
	}
	sc.Complete();
}

// The folding depends on the mercy of the programer.
static int classifyFoldPointFortran(const char* s, const char* prevWord) {
	int lev = 0;
	if (strcmp(prevWord, "end") == 0) return lev;
	if ((strcmp(prevWord, "else") == 0 && strcmp(s, "if") == 0) || strcmp(s, "elseif") == 0)
		return -1;
	if (strcmp(s, "associate") == 0 || strcmp(s, "block") == 0
	    || strcmp(s, "blockdata") == 0 || strcmp(s, "select") == 0
	    || strcmp(s, "do") == 0 || strcmp(s, "enum") ==0
	    || strcmp(s, "forall") == 0 || strcmp(s, "function") == 0
	    || strcmp(s, "interface") == 0 || strcmp(s, "module") == 0
	    || strcmp(s, "program") == 0 || strcmp(s, "subroutine") == 0
	    || strcmp(s, "then") == 0 || strcmp(s, "where") == 0) {
		lev = 1;
	} else if (strcmp(s, "end") == 0 || strcmp(s, "continue") == 0
	    || strcmp(s, "endassociate") == 0 || strcmp(s, "endblock") == 0
	    || strcmp(s, "endblockdata") == 0 || strcmp(s, "endselect") == 0
	    || strcmp(s, "enddo") == 0 || strcmp(s, "endenum") ==0
	    || strcmp(s, "endif") == 0
	    || strcmp(s, "endforall") == 0 || strcmp(s, "endfunction") == 0
	    || strcmp(s, "endinterface") == 0 || strcmp(s, "endmodule") == 0
	    || strcmp(s, "endprogram") == 0 || strcmp(s, "endsubroutine") == 0
	    || strcmp(s, "endwhere") == 0 || strcmp(s, "procedure") == 0 ) {
		lev = -1;
	}
	return lev;
}
static void FoldFortranDoc(unsigned int startPos, int length, int initStyle, WordList *[], Accessor &styler) {
	//~ bool foldComment = styler.GetPropertyInt("fold.comment") != 0;
	// Do not know how to fold the comment at the moment.
	bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;
	unsigned int endPos = startPos + length;
	int visibleChars = 0;
	int lineCurrent = styler.GetLine(startPos);
	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;
	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	int lastStart = 0;
	char prevWord[32] = "";

	for (unsigned int i = startPos; i < endPos; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (stylePrev == SCE_F_DEFAULT && style == SCE_F_WORD)
		{
			// Store last word start point.
			lastStart = i;
		}

		if (style == SCE_F_WORD) {
			if(iswordchar(ch) && !iswordchar(chNext)) {
				char s[32];
				unsigned int j;
				for(j = 0; ( j < 31 ) && ( j < i-lastStart+1 ); j++) {
					s[j] = static_cast<char>(tolower(styler[lastStart + j]));
				}
				s[j] = '\0';
				levelCurrent += classifyFoldPointFortran(s, prevWord);
				strcpy(prevWord, s);
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
			strcpy(prevWord, "");
		}

		if (!isspacechar(ch))
			visibleChars++;
	}

	// Fill in the real level of the next line, keeping the current flags as they will be filled in later
	int flagsNext = styler.LevelAt(lineCurrent) & ~SC_FOLDLEVELNUMBERMASK;
	styler.SetLevel(lineCurrent, levelPrev | flagsNext);
}

static const char * const FortranWordLists[] = {
	"Primary keywords and identifiers",
	"Intrinsic functions",
	"Extended and user defined functions",
	0,
};

static void ColouriseFortranDocFreeFormat(unsigned int startPos, int length, int initStyle, WordList *keywordlists[],
                            Accessor &styler) {
	ColouriseFortranDoc(startPos, length, initStyle, keywordlists, styler, false);
}

static void ColouriseFortranDocFixFormat(unsigned int startPos, int length, int initStyle, WordList *keywordlists[],
                            Accessor &styler) {
	ColouriseFortranDoc(startPos, length, initStyle, keywordlists, styler, true);
}


LexerModule lmFortran(SCLEX_FORTRAN, ColouriseFortranDocFreeFormat, "fortran", FoldFortranDoc, FortranWordLists);
LexerModule lmF77(SCLEX_F77, ColouriseFortranDocFixFormat, "f77", FoldFortranDoc, FortranWordLists);
