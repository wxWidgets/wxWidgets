
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
	return (ch < 0x80) && (isalnum(ch) || ch == '_');
}

static inline bool IsAWordStart(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '/' || ch == '*');
}

inline bool IsABlank(unsigned int ch) {
    return (ch == ' ') || (ch == 0x09) || (ch == 0x0b) ;
}



static void ColouriseAPDLDoc(unsigned int startPos, int length, int initStyle,
                              WordList *keywordlists[], Accessor &styler)
{

    //~ FILE *fp;
    //~ fp = fopen("myoutput.txt", "w");

    WordList &commands = *keywordlists[0];
    WordList &processors = *keywordlists[1];
    WordList &functions = *keywordlists[2];


    // backtrack to the beginning of the document, this may be slow for big documents.
    initStyle = SCE_APDL_DEFAULT;
    StyleContext sc(0, startPos+length, initStyle, styler);

    // backtrack to the nearest keyword
    //~ while ((startPos > 1) && (styler.StyleAt(startPos) != SCE_APDL_WORD)) {
            //~ startPos--;
    //~ }
    //~ startPos = styler.LineStart(styler.GetLine(startPos));
    //~ initStyle = styler.StyleAt(startPos - 1);
    //~ StyleContext sc(startPos, endPos-startPos, initStyle, styler);

    bool firstInLine = true;
    bool atEOL;

    for (; sc.More(); sc.Forward()) {

        atEOL = (sc.ch == '\r' && sc.chNext == '\n') || (sc.ch == '\n');

        //~ if (sc.ch == '\r') {
            //~ fprintf(fp,"CR\t%d\t%d", atEOL, firstInLine);
        //~ } else if (sc.ch == '\n') {
            //~ fprintf(fp,"LF\t%d\t%d", atEOL, firstInLine);
        //~ } else {
            //~ fprintf(fp,"%c\t%d\t%d", sc.ch, atEOL, firstInLine);
        //~ }

        // Determine if the current state should terminate.
        if (sc.state == SCE_APDL_COMMENT) {
            //~ fprintf(fp,"\tCOMMENT");
            if (atEOL) {
                sc.SetState(SCE_APDL_DEFAULT);
            }
        } else if (sc.state == SCE_APDL_COMMENTBLOCK) {
            //~ fprintf(fp,"\tCOMMENTBLOCK");
            if (atEOL) {
                if (sc.ch == '\r') {
                    sc.Forward();
                }
                sc.ForwardSetState(SCE_APDL_DEFAULT);
            }
        } else if (sc.state == SCE_APDL_NUMBER) {
            //~ fprintf(fp,"\tNUMBER");
            if (isdigit(sc.ch)) {
            } else if ((sc.ch == 'e' || sc.ch == 'E') && (isdigit(sc.chNext) || sc.chNext == '+' || sc.chNext == '-')) {
            } else if (sc.ch == '.') {
            } else if ((sc.ch == '+' || sc.ch == '-') && (sc.chPrev == 'e' || sc.chPrev == 'E')) {
            } else {
                sc.SetState(SCE_APDL_DEFAULT);
            }
        } else if (sc.state == SCE_APDL_STRING) {
            //~ fprintf(fp,"\tSTRING");
            if (sc.ch == '\"') {
                //~ sc.ForwardSetState(SCE_APDL_DEFAULT);
                sc.Forward();
                atEOL = (sc.ch == '\r' && sc.chNext == '\n') || (sc.ch == '\n');
                if (atEOL) {
                    firstInLine = true;
                }
                sc.SetState(SCE_APDL_DEFAULT);
            }
        } else if (sc.state == SCE_APDL_WORD) {
            //~ fprintf(fp,"\tWORD");
            if (!IsAWordChar(sc.ch) || sc.ch == '%') {
                char s[100];
                sc.GetCurrentLowered(s, sizeof(s));
                if (commands.InList(s) && firstInLine) {
                    if (IsABlank(sc.ch) || sc.ch == ',' || atEOL) {
                        sc.ChangeState(SCE_APDL_COMMAND);
                    }
                    if (sc.ch != '\n') {
                        firstInLine = false;
                    }
                } else if (processors.InList(s)) {
                    if (IsABlank(sc.ch) || atEOL) {
                        sc.ChangeState(SCE_APDL_PROCESSOR);
                        while (sc.ch != '\n') {
                            sc.Forward();
                        }
                        sc.Forward();
                    }
                } else if (functions.InList(s)) {
                    sc.ChangeState(SCE_APDL_FUNCTION);
                    if (sc.ch != '\n') {
                        firstInLine = false;
                    }
                } else {
                    if (sc.ch != '\n') {
                        firstInLine = false;
                    }
                }
                sc.SetState(SCE_APDL_DEFAULT);
            }
        }

        // Determine if a new state should be entered.
        if (sc.state == SCE_APDL_DEFAULT) {
            if (sc.ch == '!' && sc.chNext != '!') {
                sc.SetState(SCE_APDL_COMMENT);
            } else if (sc.ch == '!' && sc.chNext == '!') {
                sc.SetState(SCE_APDL_COMMENTBLOCK);
            } else if (IsADigit(sc.ch) && !IsAWordChar(sc.chPrev)) {
                sc.SetState(SCE_APDL_NUMBER);
            } else if (sc.ch == '.' && (isoperator(static_cast<char>(sc.chPrev)) ||
		IsABlank(sc.chPrev) || sc.chPrev == '\n' || sc.chPrev == '\r')) {
                sc.SetState(SCE_APDL_NUMBER);
            } else if (sc.ch == '\"') {
                sc.SetState(SCE_APDL_STRING);
            } else if (IsAWordStart(sc.ch) && (!IsADigit(sc.chPrev))) {
                sc.SetState(SCE_APDL_WORD);
            }

        }
        //~ fprintf(fp,"\n");

        if (atEOL) {
            firstInLine = true;
        }

    }
    sc.Complete();
}

static const char * const apdlWordListDesc[] = {
    "Commands",
    "Processors",
    "Functions",
    0
};

LexerModule lmAPDL(SCLEX_APDL, ColouriseAPDLDoc, "apdl", 0, apdlWordListDesc);
