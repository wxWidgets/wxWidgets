// Scintilla source code edit control
/** @file LexCPP.cxx
 ** Lexer for C++, C, Java, and Javascript.
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

static bool IsOKBeforeRE(int ch) {
	return (ch == '(') || (ch == '=') || (ch == ',');
}

static void getRange(unsigned int start,
		unsigned int end,
		Accessor &styler,
		char *s,
		unsigned int len) {
	unsigned int i = 0;
	while ((i < end - start + 1) && (i < len-1)) {
		s[i] = styler[start + i];
		i++;
	}
	s[i] = '\0';
}

inline bool IsASpace(int ch) {
    return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

inline bool IsAWordChar(int  ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '.' || ch == '_');
}

inline bool IsAWordStart(int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '_');
}

inline bool IsADigit(int ch) {
	return (ch >= '0') && (ch <= '9');
}

// All languages handled so far can treat all characters >= 0x80 as one class
// which just continues the current token or starts an identifier if in default.
// DBCS treated specially as the second character can be < 0x80 and hence 
// syntactically significant. UTF-8 avoids this as all trail bytes are >= 0x80
class ColouriseContext {
	Accessor &styler;
	int lengthDoc;
	int currentPos;
	ColouriseContext& operator=(const ColouriseContext&) {
		return *this;
	}
public:
	bool atEOL;
	int state;
	int chPrev;
	int ch;
	int chNext;

	ColouriseContext(unsigned int startPos, int length,
                        int initStyle, Accessor &styler_) : 
		styler(styler_),
		lengthDoc(startPos + length),
		currentPos(startPos), 
		atEOL(false),
		state(initStyle), 
		chPrev(0),
		ch(0), 
		chNext(0) {
		styler.StartAt(startPos);
		styler.StartSegment(startPos);
		int pos = currentPos;
		ch = static_cast<unsigned char>(styler.SafeGetCharAt(pos));
		if (styler.IsLeadByte(static_cast<char>(ch))) {
			pos++;
			ch = ch << 8;
			ch |= static_cast<unsigned char>(styler.SafeGetCharAt(pos));
		}
		chNext = static_cast<unsigned char>(styler.SafeGetCharAt(pos+1));
		if (styler.IsLeadByte(static_cast<char>(chNext))) {
			chNext = chNext << 8;
			chNext |= static_cast<unsigned char>(styler.SafeGetCharAt(pos+2));
		}
		atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
	}
	void Complete() {
		styler.ColourTo(currentPos - 1, state);
	}
	bool More() {
		return currentPos <= lengthDoc;
	}
	void Forward() {
		// A lot of this is repeated from the constructor - TODO: merge code
		chPrev = ch;
		currentPos++;
		if (ch >= 0x100)
			currentPos++;
		ch = chNext;
		chNext = static_cast<unsigned char>(styler.SafeGetCharAt(currentPos+1));
		if (styler.IsLeadByte(static_cast<char>(chNext))) {
			chNext = chNext << 8;
			chNext |= static_cast<unsigned char>(styler.SafeGetCharAt(currentPos + 2));
		}
		// Trigger on CR only (Mac style) or either on LF from CR+LF (Dos/Win) or on LF alone (Unix)
		// Avoid triggering two times on Dos/Win
		// End of line
		atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
	}
	void ChangeState(int state_) {
		state = state_;
	}
	void SetState(int state_) {
		styler.ColourTo(currentPos - 1, state);
		state = state_;
	}
	void ForwardSetState(int state_) {
		Forward();
		styler.ColourTo(currentPos - 1, state);
		state = state_;
	}
	void GetCurrent(char *s, int len) {
		getRange(styler.GetStartSegment(), currentPos - 1, styler, s, len);
	}
	int LengthCurrent() {
		return currentPos - styler.GetStartSegment();
	}
	bool Match(char ch0) {
		return ch == ch0;
	}
	bool Match(char ch0, char ch1) {
		return (ch == ch0) && (chNext == ch1);
	}
	bool Match(const char *s) {
		if (ch != *s)
			return false;
		s++;
		if (chNext != *s)
			return false;
		s++;
		for (int n=2; *s; n++) {
			if (*s != styler.SafeGetCharAt(currentPos+n))
				return false;
			s++;
		}
		return true;
	}
};

static void ColouriseCppDoc(unsigned int startPos, int length, int initStyle, WordList *keywordlists[],
                            Accessor &styler) {

	WordList &keywords = *keywordlists[0];
	WordList &keywords2 = *keywordlists[1];

	bool stylingWithinPreprocessor = styler.GetPropertyInt("styling.within.preprocessor");

	if (initStyle == SCE_C_STRINGEOL)	// Does not leak onto next line
		initStyle = SCE_C_DEFAULT;

	int chPrevNonWhite = ' ';
	int visibleChars = 0;
	bool lastWordWasUUID = false;

	ColouriseContext cc(startPos, length, initStyle, styler);

	for (; cc.More(); cc.Forward()) {

		if (cc.state == SCE_C_STRINGEOL) {
			if (cc.atEOL) {
				cc.SetState(SCE_C_DEFAULT);
			}
		} else if (cc.state == SCE_C_OPERATOR) {
			cc.SetState(SCE_C_DEFAULT);
		} else if (cc.state == SCE_C_NUMBER) {
			if (!IsAWordChar(cc.ch)) {
				cc.SetState(SCE_C_DEFAULT);
			}
		} else if (cc.state == SCE_C_IDENTIFIER) {
			if (!IsAWordChar(cc.ch) || (cc.ch == '.')) {
				char s[100];
				cc.GetCurrent(s, sizeof(s));
				if (keywords.InList(s)) {
					lastWordWasUUID = strcmp(s, "uuid") == 0;
					cc.ChangeState(SCE_C_WORD);
				} else if (keywords2.InList(s)) {
					cc.ChangeState(SCE_C_WORD2);
				}
				cc.SetState(SCE_C_DEFAULT);
			}
		} if (cc.state == SCE_C_PREPROCESSOR) {
			if (stylingWithinPreprocessor) {
				if (IsASpace(cc.ch)) {
					cc.SetState(SCE_C_DEFAULT);
				}
			} else {
				if (cc.atEOL && (cc.chPrev != '\\')) {
					cc.SetState(SCE_C_DEFAULT);
				}
			}
		} else if (cc.state == SCE_C_COMMENT) {
			if (cc.Match('*', '/')) {
				cc.Forward();
				cc.ForwardSetState(SCE_C_DEFAULT);
			}
		} else if (cc.state == SCE_C_COMMENTDOC) {
			if (cc.Match('*', '/')) {
				cc.Forward();
				cc.ForwardSetState(SCE_C_DEFAULT);
			}
		} else if (cc.state == SCE_C_COMMENTLINE || cc.state == SCE_C_COMMENTLINEDOC) {
			if (cc.ch == '\r' || cc.ch == '\n') {
				cc.SetState(SCE_C_DEFAULT);
			}
		} else if (cc.state == SCE_C_STRING) {
			if (cc.ch == '\\') {
				if (cc.chNext == '\"' || cc.chNext == '\'' || cc.chNext == '\\') {
					cc.Forward();
				}
			} else if (cc.ch == '\"') {
				cc.ForwardSetState(SCE_C_DEFAULT);
			} else if ((cc.atEOL) && (cc.chPrev != '\\')) {
				cc.ChangeState(SCE_C_STRINGEOL);
			}
		} else if (cc.state == SCE_C_CHARACTER) {
			if ((cc.ch == '\r' || cc.ch == '\n') && (cc.chPrev != '\\')) {
				cc.ChangeState(SCE_C_STRINGEOL);
			} else if (cc.ch == '\\') {
				if (cc.chNext == '\"' || cc.chNext == '\'' || cc.chNext == '\\') {
					cc.Forward();
				}
			} else if (cc.ch == '\'') {
				cc.ForwardSetState(SCE_C_DEFAULT);
			}
		} else if (cc.state == SCE_C_REGEX) {
			if (cc.ch == '\r' || cc.ch == '\n' || cc.ch == '/') {
				cc.ForwardSetState(SCE_C_DEFAULT);
			} else if (cc.ch == '\\') {
				// Gobble up the quoted character
				if (cc.chNext == '\\' || cc.chNext == '/') {
					cc.Forward();
				}
			}
		} else if (cc.state == SCE_C_VERBATIM) {
			if (cc.ch == '\"') {
				if (cc.chNext == '\"') {
					cc.Forward();
				} else {
					cc.ForwardSetState(SCE_C_DEFAULT);
				}
			}
		} else if (cc.state == SCE_C_UUID) {
			if (cc.ch == '\r' || cc.ch == '\n' || cc.ch == ')') {
				cc.SetState(SCE_C_DEFAULT);
			}
		}

		if (cc.state == SCE_C_DEFAULT) {
			if (cc.Match('@', '\"')) {
				cc.SetState(SCE_C_VERBATIM);
				cc.Forward();
			} else if (IsADigit(cc.ch) || (cc.ch == '.' && IsADigit(cc.chNext))) {
				if (lastWordWasUUID) {
					cc.SetState(SCE_C_UUID);
					lastWordWasUUID = false;
				} else {
					cc.SetState(SCE_C_NUMBER);
				}
			} else if (IsAWordStart(cc.ch) || (cc.ch == '@')) {
				if (lastWordWasUUID) {
					cc.SetState(SCE_C_UUID);
					lastWordWasUUID = false;
				} else {
					cc.SetState(SCE_C_IDENTIFIER);
				}
			} else if (cc.Match('/', '*')) {
				if (cc.Match("/**") || cc.Match("/*!"))	// Support of Qt/Doxygen doc. style
					cc.SetState(SCE_C_COMMENTDOC);
				else
					cc.SetState(SCE_C_COMMENT);
				cc.Forward();	// Eat the * so it isn't used for the end of the comment
			} else if (cc.Match('/', '/')) {
				if (cc.Match("///") || cc.Match("//!"))	// Support of Qt/Doxygen doc. style
					cc.SetState(SCE_C_COMMENTLINEDOC);
				else
					cc.SetState(SCE_C_COMMENTLINE);
			} else if (cc.ch == '/' && IsOKBeforeRE(chPrevNonWhite)) {
				cc.SetState(SCE_C_REGEX);
			} else if (cc.ch == '\"') {
				cc.SetState(SCE_C_STRING);
			} else if (cc.ch == '\'') {
				cc.SetState(SCE_C_CHARACTER);
			} else if (cc.ch == '#' && visibleChars == 0) {
				// Preprocessor commands are alone on their line
				cc.SetState(SCE_C_PREPROCESSOR);
				// Skip whitespace between # and preprocessor word
				do {
					cc.Forward();
				} while (IsASpace(cc.ch) && cc.More());
			} else if (isoperator(static_cast<char>(cc.ch))) {
				cc.SetState(SCE_C_OPERATOR);
			}
		}
		if (cc.atEOL) {
			// Reset states to begining of colourise so no surprises 
			// if different sets of lines lexed.
			chPrevNonWhite = ' ';
			visibleChars = 0;
			lastWordWasUUID = false;
		}
		if (!IsASpace(cc.ch)) {
			chPrevNonWhite = cc.ch;
			visibleChars++;
		}
	}
	cc.Complete();
}

static void FoldCppDoc(unsigned int startPos, int length, int initStyle, WordList *[],
                            Accessor &styler) {
	bool foldComment = styler.GetPropertyInt("fold.comment");
	bool foldCompact = styler.GetPropertyInt("fold.compact", 1);
	unsigned int endPos = startPos + length;
	int visibleChars = 0;
	int lineCurrent = styler.GetLine(startPos);
	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;
	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;
	for (unsigned int i = startPos; i < endPos; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		if (foldComment &&
			(style == SCE_C_COMMENT || style == SCE_C_COMMENTDOC)) {
			if (style != stylePrev) {
				levelCurrent++;
			} else if ((style != styleNext) && !atEOL) {
				// Comments don't end at end of line and the next character may be unstyled.
				levelCurrent--;
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

LexerModule lmCPP(SCLEX_CPP, ColouriseCppDoc, "cpp", FoldCppDoc);
LexerModule lmTCL(SCLEX_TCL, ColouriseCppDoc, "tcl", FoldCppDoc);
