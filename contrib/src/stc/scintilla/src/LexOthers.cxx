// Scintilla source code edit control
/** @file LexOthers.cxx
 ** Lexers for batch files, diff results, properties files, make files and error lists.
 ** Also lexer for LaTeX documents.
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

static bool Is0To9(char ch) {
	return (ch >= '0') && (ch <= '9');
}

static bool Is1To9(char ch) {
	return (ch >= '1') && (ch <= '9');
}

static inline bool AtEOL(Accessor &styler, unsigned int i) {
	return (styler[i] == '\n') ||
		((styler[i] == '\r') && (styler.SafeGetCharAt(i + 1) != '\n'));
}

static void ColouriseBatchLine(
    char *lineBuffer,
    unsigned int lengthLine,
    unsigned int startLine,
    unsigned int endPos,
    WordList &keywords,
    Accessor &styler) {

	unsigned int i = 0;
	unsigned int state = SCE_BAT_DEFAULT;

	while ((i < lengthLine) && isspacechar(lineBuffer[i])) {	// Skip initial spaces
		i++;
	}
	if (lineBuffer[i] == '@') {	// Hide command (ECHO OFF)
		styler.ColourTo(startLine + i, SCE_BAT_HIDE);
		i++;
		while ((i < lengthLine) && isspacechar(lineBuffer[i])) {	// Skip next spaces
			i++;
		}
	}
	if (lineBuffer[i] == ':') {
		// Label
		if (lineBuffer[i + 1] == ':') {
			// :: is a fake label, similar to REM, see http://content.techweb.com/winmag/columns/explorer/2000/21.htm
			styler.ColourTo(endPos, SCE_BAT_COMMENT);
		} else {	// Real label
			styler.ColourTo(endPos, SCE_BAT_LABEL);
		}
	} else {
		// Check if initial word is a keyword
		char wordBuffer[21];
		unsigned int wbl = 0, offset = i;
		// Copy word in buffer
		for (; offset < lengthLine && wbl < 20 &&
		        !isspacechar(lineBuffer[offset]); wbl++, offset++) {
			wordBuffer[wbl] = static_cast<char>(tolower(lineBuffer[offset]));
		}
		wordBuffer[wbl] = '\0';
		// Check if it is a comment
		if (CompareCaseInsensitive(wordBuffer, "rem") == 0) {
			styler.ColourTo(endPos, SCE_BAT_COMMENT);
			return ;
		}
		// Check if it is in the list
		if (keywords.InList(wordBuffer)) {
			styler.ColourTo(startLine + offset - 1, SCE_BAT_WORD);	// Regular keyword
		} else {
			// Search end of word (can be a long path)
			while (offset < lengthLine &&
			        !isspacechar(lineBuffer[offset])) {
				offset++;
			}
			styler.ColourTo(startLine + offset - 1, SCE_BAT_COMMAND);	// External command / program
		}
		// Remainder of the line: colourise the variables.

		while (offset < lengthLine) {
			if (state == SCE_BAT_DEFAULT && lineBuffer[offset] == '%') {
				styler.ColourTo(startLine + offset - 1, state);
				if (Is0To9(lineBuffer[offset + 1])) {
					styler.ColourTo(startLine + offset + 1, SCE_BAT_IDENTIFIER);
					offset += 2;
				} else if (lineBuffer[offset + 1] == '%' &&
				           !isspacechar(lineBuffer[offset + 2])) {
					// Should be safe, as there is CRLF at the end of the line...
					styler.ColourTo(startLine + offset + 2, SCE_BAT_IDENTIFIER);
					offset += 3;
				} else {
					state = SCE_BAT_IDENTIFIER;
				}
			} else if (state == SCE_BAT_IDENTIFIER && lineBuffer[offset] == '%') {
				styler.ColourTo(startLine + offset, state);
				state = SCE_BAT_DEFAULT;
			} else if (state == SCE_BAT_DEFAULT &&
			           (lineBuffer[offset] == '*' ||
			            lineBuffer[offset] == '?' ||
			            lineBuffer[offset] == '=' ||
			            lineBuffer[offset] == '<' ||
			            lineBuffer[offset] == '>' ||
			            lineBuffer[offset] == '|')) {
				styler.ColourTo(startLine + offset - 1, state);
				styler.ColourTo(startLine + offset, SCE_BAT_OPERATOR);
			}
			offset++;
		}
		//		if (endPos > startLine + offset - 1) {
		styler.ColourTo(endPos, SCE_BAT_DEFAULT);		// Remainder of line, currently not lexed
		//		}
	}

}
// ToDo: (not necessarily at beginning of line) GOTO, [IF] NOT, ERRORLEVEL
// IF [NO] (test) (command) -- test is EXIST (filename) | (string1)==(string2) | ERRORLEVEL (number)
// FOR %%(variable) IN (set) DO (command) -- variable is [a-zA-Z] -- eg for %%X in (*.txt) do type %%X
// ToDo: %n (parameters), %EnvironmentVariable% colourising
// ToDo: Colourise = > >> < | "

static void ColouriseBatchDoc(
    unsigned int startPos,
    int length,
    int /*initStyle*/,
    WordList *keywordlists[],
    Accessor &styler) {

	char lineBuffer[1024];
	WordList &keywords = *keywordlists[0];

	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	unsigned int linePos = 0;
	unsigned int startLine = startPos;
	for (unsigned int i = startPos; i < startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if (AtEOL(styler, i) || (linePos >= sizeof(lineBuffer) - 1)) {
			// End of line (or of line buffer) met, colourise it
			lineBuffer[linePos] = '\0';
			ColouriseBatchLine(lineBuffer, linePos, startLine, i, keywords, styler);
			linePos = 0;
			startLine = i + 1;
		}
	}
	if (linePos > 0) {	// Last line does not have ending characters
		ColouriseBatchLine(lineBuffer, linePos, startLine, startPos + length - 1,
		                   keywords, styler);
	}
}

static void ColouriseDiffLine(char *lineBuffer, int endLine, Accessor &styler) {
	// It is needed to remember the current state to recognize starting
	// comment lines before the first "diff " or "--- ". If a real
	// difference starts then each line starting with ' ' is a whitespace
	// otherwise it is considered a comment (Only in..., Binary file...)
	if (0 == strncmp(lineBuffer, "diff ", 3)) {
		styler.ColourTo(endLine, SCE_DIFF_COMMAND);
	} else if (0 == strncmp(lineBuffer, "--- ", 3)) {
		styler.ColourTo(endLine, SCE_DIFF_HEADER);
	} else if (0 == strncmp(lineBuffer, "+++ ", 3)) {
		styler.ColourTo(endLine, SCE_DIFF_HEADER);
	} else if (0 == strncmp(lineBuffer, "====", 4)) {  // For p4's diff
		styler.ColourTo(endLine, SCE_DIFF_HEADER);
 	} else if (0 == strncmp(lineBuffer, "***", 3)) {
		styler.ColourTo(endLine, SCE_DIFF_HEADER);
	} else if (0 == strncmp(lineBuffer, "? ", 2)) {    // For difflib
		styler.ColourTo(endLine, SCE_DIFF_HEADER);
	} else if (lineBuffer[0] == '@') {
		styler.ColourTo(endLine, SCE_DIFF_POSITION);
	} else if (lineBuffer[0] == '-' || lineBuffer[0] == '<') {
		styler.ColourTo(endLine, SCE_DIFF_DELETED);
	} else if (lineBuffer[0] == '+' || lineBuffer[0] == '>') {
		styler.ColourTo(endLine, SCE_DIFF_ADDED);
	} else if (lineBuffer[0] != ' ') {
		styler.ColourTo(endLine, SCE_DIFF_COMMENT);
	} else {
		styler.ColourTo(endLine, SCE_DIFF_DEFAULT);
	}
}

static void ColouriseDiffDoc(unsigned int startPos, int length, int, WordList *[], Accessor &styler) {
	char lineBuffer[1024];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	unsigned int linePos = 0;
	for (unsigned int i = startPos; i < startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if (AtEOL(styler, i) || (linePos >= sizeof(lineBuffer) - 1)) {
			// End of line (or of line buffer) met, colourise it
			lineBuffer[linePos] = '\0';
			ColouriseDiffLine(lineBuffer, i, styler);
			linePos = 0;
		}
	}
	if (linePos > 0) {	// Last line does not have ending characters
		ColouriseDiffLine(lineBuffer, startPos + length - 1, styler);
	}
}

static void ColourisePropsLine(
    char *lineBuffer,
    unsigned int lengthLine,
    unsigned int startLine,
    unsigned int endPos,
    Accessor &styler) {

	unsigned int i = 0;
	while ((i < lengthLine) && isspacechar(lineBuffer[i]))	// Skip initial spaces
		i++;
	if (i < lengthLine) {
		if (lineBuffer[i] == '#' || lineBuffer[i] == '!' || lineBuffer[i] == ';') {
			styler.ColourTo(endPos, SCE_PROPS_COMMENT);
		} else if (lineBuffer[i] == '[') {
			styler.ColourTo(endPos, SCE_PROPS_SECTION);
		} else if (lineBuffer[i] == '@') {
			styler.ColourTo(startLine + i, SCE_PROPS_DEFVAL);
			if (lineBuffer[++i] == '=')
				styler.ColourTo(startLine + i, SCE_PROPS_ASSIGNMENT);
			styler.ColourTo(endPos, SCE_PROPS_DEFAULT);
		} else {
			// Search for the '=' character
			while ((i < lengthLine) && (lineBuffer[i] != '='))
				i++;
			if ((i < lengthLine) && (lineBuffer[i] == '=')) {
				styler.ColourTo(startLine + i - 1, SCE_PROPS_DEFAULT);
				styler.ColourTo(startLine + i, 3);
				styler.ColourTo(endPos, SCE_PROPS_DEFAULT);
			} else {
				styler.ColourTo(endPos, SCE_PROPS_DEFAULT);
			}
		}
	} else {
		styler.ColourTo(endPos, SCE_PROPS_DEFAULT);
	}
}

static void ColourisePropsDoc(unsigned int startPos, int length, int, WordList *[], Accessor &styler) {
	char lineBuffer[1024];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	unsigned int linePos = 0;
	unsigned int startLine = startPos;
	for (unsigned int i = startPos; i < startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if (AtEOL(styler, i) || (linePos >= sizeof(lineBuffer) - 1)) {
			// End of line (or of line buffer) met, colourise it
			lineBuffer[linePos] = '\0';
			ColourisePropsLine(lineBuffer, linePos, startLine, i, styler);
			linePos = 0;
			startLine = i + 1;
		}
	}
	if (linePos > 0) {	// Last line does not have ending characters
		ColourisePropsLine(lineBuffer, linePos, startLine, startPos + length - 1, styler);
	}
}

static void ColouriseMakeLine(
    char *lineBuffer,
    unsigned int lengthLine,
    unsigned int startLine,
    unsigned int endPos,
    Accessor &styler) {

	unsigned int i = 0;
	int lastNonSpace = -1;
	unsigned int state = SCE_MAKE_DEFAULT;
	bool bSpecial = false;
	// Skip initial spaces
	while ((i < lengthLine) && isspacechar(lineBuffer[i])) {
		i++;
	}
	if (lineBuffer[i] == '#') {	// Comment
		styler.ColourTo(endPos, SCE_MAKE_COMMENT);
		return;
	}
	if (lineBuffer[i] == '!') {	// Special directive
		styler.ColourTo(endPos, SCE_MAKE_PREPROCESSOR);
		return;
	}
	while (i < lengthLine) {
		if (lineBuffer[i] == '$' && lineBuffer[i + 1] == '(') {
			styler.ColourTo(startLine + i - 1, state);
			state = SCE_MAKE_IDENTIFIER;
		} else if (state == SCE_MAKE_IDENTIFIER && lineBuffer[i] == ')') {
			styler.ColourTo(startLine + i, state);
			state = SCE_MAKE_DEFAULT;
		}
		if (!bSpecial) {
			if (lineBuffer[i] == ':') {
				// We should check that no colouring was made since the beginning of the line,
				// to avoid colouring stuff like /OUT:file
				if (lastNonSpace >= 0)
					styler.ColourTo(startLine + lastNonSpace, SCE_MAKE_TARGET);
				styler.ColourTo(startLine + i - 1, SCE_MAKE_DEFAULT);
				styler.ColourTo(startLine + i, SCE_MAKE_OPERATOR);
				bSpecial = true;	// Only react to the first ':' of the line
				state = SCE_MAKE_DEFAULT;
			} else if (lineBuffer[i] == '=') {
				if (lastNonSpace >= 0)
					styler.ColourTo(startLine + lastNonSpace, SCE_MAKE_IDENTIFIER);
				styler.ColourTo(startLine + i - 1, SCE_MAKE_DEFAULT);
				styler.ColourTo(startLine + i, SCE_MAKE_OPERATOR);
				bSpecial = true;	// Only react to the first '=' of the line
				state = SCE_MAKE_DEFAULT;
			}
		}
		if (!isspacechar(lineBuffer[i])) {
			lastNonSpace = i;
		}
		i++;
	}
	if (state == SCE_MAKE_IDENTIFIER) {
		styler.ColourTo(endPos, SCE_MAKE_IDEOL);	// Error, variable reference not ended
	} else {
		styler.ColourTo(endPos, SCE_MAKE_DEFAULT);
	}
}

static void ColouriseMakeDoc(unsigned int startPos, int length, int, WordList *[], Accessor &styler) {
	char lineBuffer[1024];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	unsigned int linePos = 0;
	unsigned int startLine = startPos;
	for (unsigned int i = startPos; i < startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if (AtEOL(styler, i) || (linePos >= sizeof(lineBuffer) - 1)) {
			// End of line (or of line buffer) met, colourise it
			lineBuffer[linePos] = '\0';
			ColouriseMakeLine(lineBuffer, linePos, startLine, i, styler);
			linePos = 0;
			startLine = i + 1;
		}
	}
	if (linePos > 0) {	// Last line does not have ending characters
		ColouriseMakeLine(lineBuffer, linePos, startLine, startPos + length - 1, styler);
	}
}

static bool strstart(char *haystack, char *needle) {
	return strncmp(haystack, needle, strlen(needle)) == 0;
}

static void ColouriseErrorListLine(
    char *lineBuffer,
    unsigned int lengthLine,
    //		unsigned int startLine,
    unsigned int endPos,
    Accessor &styler) {
	const int unRecognized = 99;
	if (lineBuffer[0] == '>') {
		// Command or return status
		styler.ColourTo(endPos, SCE_ERR_CMD);
	} else if (lineBuffer[0] == '<') {
		// Diff removal, but not interested. Trapped to avoid hitting CTAG cases.
		styler.ColourTo(endPos, SCE_ERR_DEFAULT);
	} else if (lineBuffer[0] == '!') {
		styler.ColourTo(endPos, SCE_ERR_DIFF_CHANGED);
	} else if (lineBuffer[0] == '+') {
		styler.ColourTo(endPos, SCE_ERR_DIFF_ADDITION);
	} else if (lineBuffer[0] == '-' && lineBuffer[1] == '-' && lineBuffer[2] == '-') {
		styler.ColourTo(endPos, SCE_ERR_DIFF_MESSAGE);
	} else if (lineBuffer[0] == '-') {
		styler.ColourTo(endPos, SCE_ERR_DIFF_DELETION);
	} else if (strstr(lineBuffer, "File \"") && strstr(lineBuffer, ", line ")) {
		styler.ColourTo(endPos, SCE_ERR_PYTHON);
	} else if (strstr(lineBuffer, " in ") && strstr(lineBuffer, " on line ")) {
		styler.ColourTo(endPos, SCE_ERR_PHP);
	} else if ((strstart(lineBuffer, "Error ") ||
		strstart(lineBuffer, "Warning ")) &&
		strstr(lineBuffer, " at (") &&
		strstr(lineBuffer, ") : ") &&
		(strstr(lineBuffer, " at (") < strstr(lineBuffer, ") : "))) {
		// Intel Fortran Compiler error/warning message
		styler.ColourTo(endPos, SCE_ERR_IFC);
	} else if (strstart(lineBuffer, "Error ")) {
		// Borland error message
		styler.ColourTo(endPos, SCE_ERR_BORLAND);
	} else if (strstart(lineBuffer, "Warning ")) {
		// Borland warning message
		styler.ColourTo(endPos, SCE_ERR_BORLAND);
	} else if (strstr(lineBuffer, "at line " ) &&
	           (strstr(lineBuffer, "at line " ) < (lineBuffer + lengthLine)) &&
	           strstr(lineBuffer, "file ") &&
	           (strstr(lineBuffer, "file ") < (lineBuffer + lengthLine))) {
		// Lua error message
		styler.ColourTo(endPos, SCE_ERR_LUA);
	} else if (strstr(lineBuffer, " at " ) &&
	           (strstr(lineBuffer, " at " ) < (lineBuffer + lengthLine)) &&
	           strstr(lineBuffer, " line ") &&
	           (strstr(lineBuffer, " line ") < (lineBuffer + lengthLine)) &&
			   (strstr(lineBuffer, " at " ) < (strstr(lineBuffer, " line ")))) {
		// perl error message
		styler.ColourTo(endPos, SCE_ERR_PERL);
	} else if ((memcmp(lineBuffer, "   at ", 6) == 0) &&
		strstr(lineBuffer, ":line ")) {
		// A .NET traceback
		styler.ColourTo(endPos, SCE_ERR_NET);
	} else if (strstart(lineBuffer, "Line ") &&
		strstr(lineBuffer, ", file ")) {
		// Essential Lahey Fortran error message
		styler.ColourTo(endPos, SCE_ERR_ELF);
	} else {
		// Look for GCC <filename>:<line>:message
		// Look for Microsoft <filename>(line) :message
		// Look for Microsoft <filename>(line,pos)message
		// Look for CTags \tmessage
		int state = 0;
		for (unsigned int i = 0; i < lengthLine; i++) {
			char ch = lineBuffer[i];
			char chNext = ' ';
			if ((i+1) < lengthLine)
				chNext = lineBuffer[i+1];
			if (state == 0) {
				if (ch == ':') {
					// May be GCC
					if ((chNext != '\\') && (chNext != '/')) {
						// This check is not completely accurate as may be on
						// GTK+ with a file name that includes ':'.
						state = 1;
					}
				} else if ((ch == '(') && Is1To9(chNext)) {
					// May be Microsoft
					// Check againt '0' often removes phone numbers
					state = 10;
				} else if (ch == '\t') {
					// May be CTags
					state = 20;
				}
			} else if (state == 1) {
				state = Is1To9(ch) ? 2 : unRecognized;
			} else if (state == 2) {
				if (ch == ':') {
					state = 3;	// :9.*: is GCC
					break;
				} else if (!Is0To9(ch)) {
					state = unRecognized;
				}
			} else if (state == 10) {
				state = Is0To9(ch) ? 11 : unRecognized;
			} else if (state == 11) {
				if (ch == ',') {
					state = 14;
				} else if (ch == ')') {
					state = 12;
				} else if ((ch != ' ') && !Is0To9(ch)) {
					state = unRecognized;
				}
			} else if (state == 12) {
				if ((ch == ' ') && (chNext == ':'))
					state = 13;
				else
					state = unRecognized;
			} else if (state == 14) {
				if (ch == ')') {
					state = 15;
					break;
				} else if ((ch != ' ') && !Is0To9(ch)) {
					state = unRecognized;
				}
			} else if (state == 20) {
				if ((lineBuffer[i-1] == '\t') &&
					((ch == '/' && lineBuffer[i+1] == '^') || Is0To9(ch))) {
					state = 24;
					break;
				} else if ((ch == '/') && (lineBuffer[i+1] == '^')) {
					state = 21;
				}
			} else if ((state == 21) && ((lineBuffer[i] == '$') && (lineBuffer[i+1] == '/'))) {
				state = 22;
				break;
			}
		}
		if (state == 3) {
			styler.ColourTo(endPos, SCE_ERR_GCC);
		} else if ((state == 13) || (state == 14) || (state == 15)) {
			styler.ColourTo(endPos, SCE_ERR_MS);
		} else if (((state == 22) || (state == 24)) && (lineBuffer[0] != '\t')) {
			styler.ColourTo(endPos, SCE_ERR_CTAG);
		} else {
			styler.ColourTo(endPos, SCE_ERR_DEFAULT);
		}
	}
}

static void ColouriseErrorListDoc(unsigned int startPos, int length, int, WordList *[], Accessor &styler) {
	char lineBuffer[1024];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	unsigned int linePos = 0;
	for (unsigned int i = startPos; i < startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if (AtEOL(styler, i) || (linePos >= sizeof(lineBuffer) - 1)) {
			// End of line (or of line buffer) met, colourise it
			lineBuffer[linePos] = '\0';
			ColouriseErrorListLine(lineBuffer, linePos, i, styler);
			linePos = 0;
		}
	}
	if (linePos > 0) {	// Last line does not have ending characters
		ColouriseErrorListLine(lineBuffer, linePos, startPos + length - 1, styler);
	}
}

static int isSpecial(char s) {
	return (s == '\\') || (s == ',') || (s == ';') || (s == '\'') || (s == ' ') ||
	       (s == '\"') || (s == '`') || (s == '^') || (s == '~');
}

static int isTag(int start, Accessor &styler) {
	char s[6];
	unsigned int i = 0, e = 1;
	while (i < 5 && e) {
		s[i] = styler[start + i];
		i++;
		e = styler[start + i] != '{';
	}
	s[i] = '\0';
	return (strcmp(s, "begin") == 0) || (strcmp(s, "end") == 0);
}

static void ColouriseLatexDoc(unsigned int startPos, int length, int initStyle,
                              WordList *[], Accessor &styler) {

	styler.StartAt(startPos);

	int state = initStyle;
	char chNext = styler[startPos];
	styler.StartSegment(startPos);
	int lengthDoc = startPos + length;

	for (int i = startPos; i < lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if (styler.IsLeadByte(ch)) {
			chNext = styler.SafeGetCharAt(i + 2);
			i++;
			continue;
		}
		switch (state) {
		case SCE_L_DEFAULT :
			switch (ch) {
			case '\\' :
				styler.ColourTo(i - 1, state);
				if (isSpecial(styler[i + 1])) {
					styler.ColourTo(i + 1, SCE_L_COMMAND);
					i++;
					chNext = styler.SafeGetCharAt(i + 1);
				} else {
					if (isTag(i + 1, styler))
						state = SCE_L_TAG;
					else
						state = SCE_L_COMMAND;
				}
				break;
			case '$' :
				styler.ColourTo(i - 1, state);
				state = SCE_L_MATH;
				if (chNext == '$') {
					i++;
					chNext = styler.SafeGetCharAt(i + 1);
				}
				break;
			case '%' :
				styler.ColourTo(i - 1, state);
				state = SCE_L_COMMENT;
				break;
			}
			break;
		case SCE_L_COMMAND :
			if (chNext == '[' || chNext == '{' || chNext == '}' ||
			        chNext == ' ' || chNext == '\r' || chNext == '\n') {
				styler.ColourTo(i, state);
				state = SCE_L_DEFAULT;
				i++;
				chNext = styler.SafeGetCharAt(i + 1);
			}
			break;
		case SCE_L_TAG :
			if (ch == '}') {
				styler.ColourTo(i, state);
				state = SCE_L_DEFAULT;
			}
			break;
		case SCE_L_MATH :
			if (ch == '$') {
				if (chNext == '$') {
					i++;
					chNext = styler.SafeGetCharAt(i + 1);
				}
				styler.ColourTo(i, state);
				state = SCE_L_DEFAULT;
			}
			break;
		case SCE_L_COMMENT :
			if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i - 1, state);
				state = SCE_L_DEFAULT;
			}
		}
	}
	styler.ColourTo(lengthDoc-1, state);
}

static const char * const batchWordListDesc[] = {
	"Keywords",
	0
};

static const char * const emptyWordListDesc[] = {
	0
};

LexerModule lmBatch(SCLEX_BATCH, ColouriseBatchDoc, "batch", 0, batchWordListDesc);
LexerModule lmDiff(SCLEX_DIFF, ColouriseDiffDoc, "diff", 0, emptyWordListDesc);
LexerModule lmProps(SCLEX_PROPERTIES, ColourisePropsDoc, "props", 0, emptyWordListDesc);
LexerModule lmMake(SCLEX_MAKEFILE, ColouriseMakeDoc, "makefile", 0, emptyWordListDesc);
LexerModule lmErrorList(SCLEX_ERRORLIST, ColouriseErrorListDoc, "errorlist", 0, emptyWordListDesc);
LexerModule lmLatex(SCLEX_LATEX, ColouriseLatexDoc, "latex", 0, emptyWordListDesc);
