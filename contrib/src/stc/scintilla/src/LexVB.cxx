// SciTE - Scintilla based Text Editor
// LexVB.cxx - lexer for Visual Basic and VBScript
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
	styler.ColourTo(end, chAttr);
	if (chAttr == SCE_C_COMMENTLINE)
		return SCE_C_COMMENTLINE;
	else
		return SCE_C_DEFAULT;
}

static void ColouriseVBDoc(unsigned int startPos, int length, int initStyle,
                           WordList *keywordlists[], StylingContext &styler) {

	WordList &keywords = *keywordlists[0];
	
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
			i += 1;
			continue;
		}

		if (state == SCE_C_DEFAULT) {
			if (iswordstart(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_WORD;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_STRING;
			}
		} else if (state == SCE_C_WORD) {
			if (!iswordchar(ch)) {
				state = classifyWordVB(styler.GetStartSegment(), i - 1, keywords, styler);
				if (state == SCE_C_DEFAULT) {
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
					styler.ColourTo(i - 1, state);
					state = SCE_C_DEFAULT;
				}
			} else if (state == SCE_C_STRING) {
				// VB doubles quotes to preserve them
				if (ch == '\"') {
					styler.ColourTo(i, state);
					state = SCE_C_DEFAULT;
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
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
	styler.ColourTo(lengthDoc, state);
}

LexerModule lmVB(SCLEX_VB, ColouriseVBDoc);
