// Scintilla source code edit control

// File: LexTeX.cxx - general context conformant tex coloring scheme
// Author: Hans Hagen - PRAGMA ADE - Hasselt NL - www.pragma-ade.com
// Version: September 28, 2003

// Copyright: 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

// This lexer is derived from the one written for the texwork environment (1999++) which in
// turn is inspired on texedit (1991++) which finds its roots in wdt (1986).

// If you run into strange boundary cases, just tell me and I'll look into it.

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
#include "StyleContext.h"

// val SCE_TEX_DEFAULT = 0
// val SCE_TEX_SPECIAL = 1
// val SCE_TEX_GROUP   = 2
// val SCE_TEX_SYMBOL  = 3
// val SCE_TEX_COMMAND = 4
// val SCE_TEX_TEXT    = 5

// Definitions in SciTEGlobal.properties:
//
// TeX Highlighting
//
// # Default
// style.tex.0=fore:#7F7F00
// # Special
// style.tex.1=fore:#007F7F
// # Group
// style.tex.2=fore:#880000
// # Symbol
// style.tex.3=fore:#7F7F00
// # Command
// style.tex.4=fore:#008800
// # Text
// style.tex.5=fore:#000000

// lexer.tex.interface.default=0
// lexer.tex.comment.process=0

// todo: lexer.tex.auto.if

// Auxiliary functions:

static inline bool endOfLine(Accessor &styler, unsigned int i) {
	return
      (styler[i] == '\n') || ((styler[i] == '\r') && (styler.SafeGetCharAt(i + 1) != '\n')) ;
}

static inline bool isTeXzero(int ch) {
	return
      (ch == '%') ;
}

static inline bool isTeXone(int ch) {
	return
      (ch == '[') || (ch == ']') || (ch == '=') || (ch == '#') ||
      (ch == '(') || (ch == ')') || (ch == '<') || (ch == '>') ||
      (ch == '"') ;
}

static inline bool isTeXtwo(int ch) {
	return
      (ch == '{') || (ch == '}') || (ch == '$') ;
}

static inline bool isTeXthree(int ch) {
	return
      (ch == '~') || (ch == '^') || (ch == '_') || (ch == '&') ||
      (ch == '-') || (ch == '+') || (ch == '\"') || (ch == '`') ||
      (ch == '/') || (ch == '|') || (ch == '%') ;
}

static inline bool isTeXfour(int ch) {
	return
      (ch == '\\') ;
}

static inline bool isTeXfive(int ch) {
	return
      ((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z')) ||
      (ch == '@') || (ch == '!') || (ch == '?') ;
}

static inline bool isTeXsix(int ch) {
	return
      (ch == ' ') ;
}

static inline bool isTeXseven(int ch) {
	return
      (ch == '^') ;
}

// Interface determination

static int CheckTeXInterface(
    unsigned int startPos,
    int length,
    Accessor &styler,
	int defaultInterface) {

    char lineBuffer[1024] ;
	unsigned int linePos = 0 ;

    // some day we can make something lexer.tex.mapping=(all,0)(nl,1)(en,2)...

    if (styler.SafeGetCharAt(0) == '%') {
        for (unsigned int i = 0; i < startPos + length; i++) {
            lineBuffer[linePos++] = styler.SafeGetCharAt(i) ;
            if (endOfLine(styler, i) || (linePos >= sizeof(lineBuffer) - 1)) {
                lineBuffer[linePos] = '\0';
                if (strstr(lineBuffer, "interface=all")) {
                    return 0 ;
				} else if (strstr(lineBuffer, "interface=tex")) {
                    return 1 ;
                } else if (strstr(lineBuffer, "interface=nl")) {
                    return 2 ;
                } else if (strstr(lineBuffer, "interface=en")) {
                    return 3 ;
                } else if (strstr(lineBuffer, "interface=de")) {
                    return 4 ;
                } else if (strstr(lineBuffer, "interface=cz")) {
                    return 5 ;
                } else if (strstr(lineBuffer, "interface=it")) {
                    return 6 ;
                } else if (strstr(lineBuffer, "interface=ro")) {
                    return 7 ;
                } else if (strstr(lineBuffer, "interface=latex")) {
					// we will move latex cum suis up to 91+ when more keyword lists are supported
                    return 8 ;
				} else if (styler.SafeGetCharAt(1) == 'D' && strstr(lineBuffer, "%D \\module")) {
					// better would be to limit the search to just one line
					return 3 ;
                } else {
                    return defaultInterface ;
                }
            }
		}
    }

    return defaultInterface ;
}

static void ColouriseTeXDoc(
    unsigned int startPos,
    int length,
    int,
    WordList *keywordlists[],
    Accessor &styler) {

	styler.StartAt(startPos) ;
	styler.StartSegment(startPos) ;

	bool processComment   = styler.GetPropertyInt("lexer.tex.comment.process",   0) == 1 ;
    bool useKeywords      = styler.GetPropertyInt("lexer.tex.use.keywords",      1) == 1 ;
	bool autoIf           = styler.GetPropertyInt("lexer.tex.auto.if",           1) == 1 ;
    int  defaultInterface = styler.GetPropertyInt("lexer.tex.interface.default", 1) ;

	char key[100] ;
	int  k ;
	bool newifDone = false ;
	bool inComment = false ;

	int currentInterface = CheckTeXInterface(startPos,length,styler,defaultInterface) ;

    if (currentInterface == 0) {
        useKeywords = false ;
        currentInterface = 1 ;
    }

    WordList &keywords = *keywordlists[currentInterface-1] ;

	StyleContext sc(startPos, length, SCE_TEX_TEXT, styler);

	bool going = sc.More() ; // needed because of a fuzzy end of file state

	for (; going; sc.Forward()) {

		if (! sc.More()) { going = false ; } // we need to go one behind the end of text

		if (inComment) {
			if (sc.atLineEnd) {
				sc.SetState(SCE_TEX_TEXT) ;
				newifDone = false ;
				inComment = false ;
			}
		} else {
			if (! isTeXfive(sc.ch)) {
				if (sc.state == SCE_TEX_COMMAND) {
					if (sc.LengthCurrent() == 1) { // \<noncstoken>
						if (isTeXseven(sc.ch) && isTeXseven(sc.chNext)) {
							sc.Forward(2) ; // \^^ and \^^<token>
						}
						sc.ForwardSetState(SCE_TEX_TEXT) ;
					} else {
						sc.GetCurrent(key, sizeof(key)-1) ;
						k = strlen(key) ;
						memmove(key,key+1,k) ; // shift left over escape token
						key[k] = '\0' ;
						k-- ;
						if (! keywords || ! useKeywords) {
							sc.SetState(SCE_TEX_COMMAND) ;
							newifDone = false ;
						} else if (k == 1) { //\<cstoken>
							sc.SetState(SCE_TEX_COMMAND) ;
							newifDone = false ;
						} else if (keywords.InList(key)) {
    						sc.SetState(SCE_TEX_COMMAND) ;
							newifDone = autoIf && (strcmp(key,"newif") == 0) ;
						} else if (autoIf && ! newifDone && (key[0] == 'i') && (key[1] == 'f') && keywords.InList("if")) {
	    					sc.SetState(SCE_TEX_COMMAND) ;
						} else {
							sc.ChangeState(SCE_TEX_TEXT) ;
							sc.SetState(SCE_TEX_TEXT) ;
							newifDone = false ;
						}
					}
				}
				if (isTeXzero(sc.ch)) {
					sc.SetState(SCE_TEX_SYMBOL) ;
					sc.ForwardSetState(SCE_TEX_DEFAULT) ;
					inComment = ! processComment ;
					newifDone = false ;
				} else if (isTeXseven(sc.ch) && isTeXseven(sc.chNext)) {
					sc.SetState(SCE_TEX_TEXT) ;
					sc.ForwardSetState(SCE_TEX_TEXT) ;
				} else if (isTeXone(sc.ch)) {
					sc.SetState(SCE_TEX_SPECIAL) ;
					newifDone = false ;
				} else if (isTeXtwo(sc.ch)) {
					sc.SetState(SCE_TEX_GROUP) ;
					newifDone = false ;
				} else if (isTeXthree(sc.ch)) {
					sc.SetState(SCE_TEX_SYMBOL) ;
					newifDone = false ;
				} else if (isTeXfour(sc.ch)) {
					sc.SetState(SCE_TEX_COMMAND) ;
				} else if (isTeXsix(sc.ch)) {
					sc.SetState(SCE_TEX_TEXT) ;
				} else if (sc.atLineEnd) {
					sc.SetState(SCE_TEX_TEXT) ;
					newifDone = false ;
					inComment = false ;
				} else {
					sc.SetState(SCE_TEX_TEXT) ;
				}
			} else if (sc.state != SCE_TEX_COMMAND) {
				sc.SetState(SCE_TEX_TEXT) ;
			}
		}
	}
	sc.ChangeState(SCE_TEX_TEXT) ;
	sc.Complete();

}


// Hooks into the system:

static const char * const texWordListDesc[] = {
    "TeX, eTeX, pdfTeX, Omega",
    "ConTeXt Dutch",
    "ConTeXt English",
    "ConTeXt German",
    "ConTeXt Czech",
    "ConTeXt Italian",
    "ConTeXt Romanian",
	0,
} ;

LexerModule lmTeX(SCLEX_TEX, ColouriseTeXDoc, "tex", 0, texWordListDesc);
