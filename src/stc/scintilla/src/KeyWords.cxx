// Scintilla source code edit control
/** @file KeyWords.cxx
 ** Colourise for particular languages.
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

LexerModule *LexerModule::base = 0;
int LexerModule::nextLanguage = SCLEX_AUTOMATIC+1;

LexerModule::LexerModule(int language_, LexerFunction fnLexer_,
	const char *languageName_, LexerFunction fnFolder_) :
	language(language_), 
	languageName(languageName_), 
	fnLexer(fnLexer_), 
	fnFolder(fnFolder_) {
	next = base;
	base = this;
	if (language == SCLEX_AUTOMATIC) {
		language = nextLanguage;
		nextLanguage++;
	}
}

LexerModule *LexerModule::Find(int language) {
	LexerModule *lm = base;
	while (lm) {
		if (lm->language == language) {
			return lm;
		}
		lm = lm->next;
	}
	return 0;
}

LexerModule *LexerModule::Find(const char *languageName) {
	if (languageName) {
		LexerModule *lm = base;
		while (lm) {
			if (lm->languageName && 0 == strcmp(lm->languageName, languageName)) {
				return lm;
			}
			lm = lm->next;
		}
	}
	return 0;
}

void LexerModule::Lex(unsigned int startPos, int lengthDoc, int initStyle,
	  WordList *keywordlists[], Accessor &styler) {
	if (fnLexer)
		fnLexer(startPos, lengthDoc, initStyle, keywordlists, styler);
}

void LexerModule::Fold(unsigned int startPos, int lengthDoc, int initStyle,
	  WordList *keywordlists[], Accessor &styler) {
	if (fnFolder) {
		int lineCurrent = styler.GetLine(startPos);
		// Move back one line in case deletion wrecked current line fold state
		if (lineCurrent > 0) {
			lineCurrent--;
			int newStartPos = styler.LineStart(lineCurrent);
			lengthDoc += startPos - newStartPos;
			startPos = newStartPos;
			initStyle = 0;
			if (startPos > 0) {
				initStyle = styler.StyleAt(startPos - 1);
			}
		}
		fnFolder(startPos, lengthDoc, initStyle, keywordlists, styler);
	}
}

static void ColouriseNullDoc(unsigned int startPos, int length, int, WordList *[],
                            Accessor &styler) {
	// Null language means all style bytes are 0 so just mark the end - no need to fill in.
	if (length > 0) {
		styler.StartAt(startPos + length - 1);
		styler.StartSegment(startPos + length - 1);
		styler.ColourTo(startPos + length - 1, 0);
	}
}

LexerModule lmNull(SCLEX_NULL, ColouriseNullDoc, "null");

#ifdef __vms

// The following code forces a reference to all of the Scintilla lexers.
// If we don't do something like this, then the linker tends to "optimize"
// them away. (eric@sourcegear.com)

// Taken from wxWindow's stc.cpp. Walter.

int wxForceScintillaLexers(void) {
  extern LexerModule lmAda;
  extern LexerModule lmAVE;
  extern LexerModule lmConf;
  extern LexerModule lmDiff;
  extern LexerModule lmLatex;
  extern LexerModule lmPascal;
  extern LexerModule lmCPP;
  extern LexerModule lmHTML;
  extern LexerModule lmXML;
  extern LexerModule lmProps;
  extern LexerModule lmErrorList;
  extern LexerModule lmMake;
  extern LexerModule lmBatch;
  extern LexerModule lmPerl;
  extern LexerModule lmPython;
  extern LexerModule lmSQL;
  extern LexerModule lmVB;
  extern LexerModule lmRuby;

  if (
      &lmAda
      && &lmAVE
      && &lmConf
      && &lmDiff
      && &lmLatex
      && &lmPascal
      && &lmCPP
      && &lmHTML
      && &lmXML
      && &lmProps
      && &lmErrorList
      && &lmMake
      && &lmBatch
      && &lmPerl
      && &lmPython
      && &lmSQL
      && &lmVB
      && &lmRuby      
      )
    {
      return 1;
    }
  else
    {
      return 0;
    }
}
#endif
