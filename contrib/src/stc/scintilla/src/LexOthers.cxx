// SciTE - Scintilla based Text Editor
// LexOthers.cxx - lexers for properties files, batch files, make files and error lists
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

static void ColouriseBatchLine(char *lineBuffer, int endLine, Accessor &styler) {
	if (0 == strncmp(lineBuffer, "REM", 3)) {
		styler.ColourTo(endLine, 1);
	} else if (0 == strncmp(lineBuffer, "rem", 3)) {
		styler.ColourTo(endLine, 1);
	} else if (0 == strncmp(lineBuffer, "SET", 3)) {
		styler.ColourTo(endLine, 2);
	} else if (0 == strncmp(lineBuffer, "set", 3)) {
		styler.ColourTo(endLine, 2);
	} else if (lineBuffer[0] == ':') {
		styler.ColourTo(endLine, 3);
	} else {
		styler.ColourTo(endLine, 0);
	}
}

static void ColouriseBatchDoc(unsigned int startPos, int length, int, WordList *[], Accessor &styler) {
	char lineBuffer[1024];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	unsigned int linePos = 0;
	for (unsigned int i = startPos; i < startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if (styler[i] == '\r' || styler[i] == '\n' || (linePos >= sizeof(lineBuffer) - 1)) {
			ColouriseBatchLine(lineBuffer, i, styler);
			linePos = 0;
		}
	}
	if (linePos > 0)
		ColouriseBatchLine(lineBuffer, startPos + length, styler);
}

static void ColourisePropsLine(char *lineBuffer, int lengthLine, int startLine, int endPos, Accessor &styler) {
	int i = 0;
	while (isspace(lineBuffer[i]) && (i < lengthLine))	// Skip initial spaces
		i++;
	if (lineBuffer[i] == '#' || lineBuffer[i] == '!' || lineBuffer[i] == ';') {
		styler.ColourTo(endPos, 1);
	} else if (lineBuffer[i] == '[') {
		styler.ColourTo(endPos, 2);
	} else if (lineBuffer[i] == '@') {
		styler.ColourTo(startLine+i, 4);
		if (lineBuffer[++i] == '=')
			styler.ColourTo(startLine+i, 3);
		styler.ColourTo(endPos, 0);
	} else {
		while (lineBuffer[i] != '=' && (i < lengthLine))	// Search the '=' character
			i++;
		if (lineBuffer[i] == '=') {
			styler.ColourTo(startLine+i-1, 0);
			styler.ColourTo(startLine+i, 3);
			styler.ColourTo(endPos, 0);
		} else {
			styler.ColourTo(endPos, 0);
		}
	}
}

static void ColourisePropsDoc(unsigned int startPos, int length, int, WordList *[], Accessor &styler) {
	char lineBuffer[1024];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	unsigned int linePos = 0;
	int startLine = startPos;
	for (unsigned int i = startPos; i <= startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if ((styler[i] == '\r' && styler.SafeGetCharAt(i+1) != '\n') || 
			styler[i] == '\n' || 
			(linePos >= sizeof(lineBuffer) - 1)) {
			lineBuffer[linePos] = '\0';
			ColourisePropsLine(lineBuffer, linePos, startLine, i, styler);
			linePos = 0;
			startLine = i+1;
		}
	}
	if (linePos > 0)
		ColourisePropsLine(lineBuffer, linePos, startLine, startPos + length, styler);
}

static void ColouriseMakeLine(char *lineBuffer, int lengthLine, int endPos, Accessor &styler) {
	int i = 0;
	while (isspace(lineBuffer[i]) && (i < lengthLine))
		i++;
	if (lineBuffer[i] == '#' || lineBuffer[i] == '!') {
		styler.ColourTo(endPos, 1);
	} else {
		styler.ColourTo(endPos, 0);
	}
}

static void ColouriseMakeDoc(unsigned int startPos, int length, int, WordList *[], Accessor &styler) {
	char lineBuffer[1024];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	unsigned int linePos = 0;
	for (unsigned int i = startPos; i <= startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if (styler[i] == '\r' || styler[i] == '\n' || (linePos >= sizeof(lineBuffer) - 1)) {
			ColouriseMakeLine(lineBuffer, linePos, i, styler);
			linePos = 0;
		}
	}
	if (linePos > 0)
		ColouriseMakeLine(lineBuffer, linePos, startPos + length, styler);
}

static void ColouriseErrorListLine(char *lineBuffer, int lengthLine, int endPos, Accessor &styler) {
	if (lineBuffer[0] == '>') {
		// Command or return status
		styler.ColourTo(endPos, 4);
	} else if (strstr(lineBuffer, "File \"") && strstr(lineBuffer, ", line ")) {
		styler.ColourTo(endPos, 1);
	} else if (0 == strncmp(lineBuffer, "Error ", strlen("Error "))) {
		// Borland error message
		styler.ColourTo(endPos, 5);
	} else if (0 == strncmp(lineBuffer, "Warning ", strlen("Warning "))) {
		// Borland warning message
		styler.ColourTo(endPos, 5);
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
			styler.ColourTo(endPos, 2);
		} else if ((state == 13) || (state == 14) || (state == 15)) {
			styler.ColourTo(endPos, 3);
		} else {
			styler.ColourTo(endPos, 0);
		}
	}
}

static void ColouriseErrorListDoc(unsigned int startPos, int length, int, WordList *[], Accessor &styler) {
	char lineBuffer[1024];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	unsigned int linePos = 0;
	for (unsigned int i = startPos; i <= startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if (styler[i] == '\r' || styler[i] == '\n' || (linePos >= sizeof(lineBuffer) - 1)) {
			ColouriseErrorListLine(lineBuffer, linePos, i, styler);
			linePos = 0;
		}
	}
	if (linePos > 0)
		ColouriseErrorListLine(lineBuffer, linePos, startPos + length, styler);
}

LexerModule lmProps(SCLEX_PROPERTIES, ColourisePropsDoc);
LexerModule lmErrorList(SCLEX_ERRORLIST, ColouriseErrorListDoc);
LexerModule lmMake(SCLEX_MAKEFILE, ColouriseMakeDoc);
LexerModule lmBatch(SCLEX_BATCH, ColouriseBatchDoc);
