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

LexerModule *LexerModule::base = 0;

LexerModule::LexerModule(int language_, LexerFunction fn_) :
	language(language_), fn(fn_) {
	next = base;
	base = this;
}

void LexerModule::Colourise(unsigned int startPos, int lengthDoc, int initStyle,
		int language, WordList *keywordlists[], StylingContext &styler) {
	LexerModule *lm = base;
	while (lm) {
		if (lm->language == language) {
			lm->fn(startPos, lengthDoc, initStyle, keywordlists, styler);
			return;
		}
		lm = lm->next;
	}
	// Unknown language
	// Null language means all style bytes are 0 so just mark the end - no need to fill in.
	if (lengthDoc > 0) {
		styler.StartAt(startPos + lengthDoc - 1);
		styler.StartSegment(startPos + lengthDoc - 1);
		styler.ColourTo(startPos + lengthDoc - 1, 0);
	}
}
