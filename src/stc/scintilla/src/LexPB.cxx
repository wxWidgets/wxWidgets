// Scintilla source code edit control
/** @file LexPB.cxx
 ** Lexer for PowerBasic by Roland Walter, roland@rowalt.de
 ** Last update: 17.10.2003 (toggling of subs/functions now until next sub/functin - this gives better results)
 **/
//
// Necessary changes in Scintilla project:
//  - In SciLexer.h and Scintilla.iface:
//
//    #define SCLEX_PB 51				//ID for PowerBasic lexer
//    (...)
//    #define SCE_B_DEFAULT 0			//in both VB and PB lexer
//    #define SCE_B_COMMENT 1			//in both VB and PB lexer
//    #define SCE_B_NUMBER 2			//in both VB and PB lexer
//    #define SCE_B_KEYWORD 3			//in both VB and PB lexer
//    #define SCE_B_STRING 4			//in both VB and PB lexer
//    #define SCE_B_PREPROCESSOR 5		//VB lexer only, unsupported by PB lexer
//    #define SCE_B_OPERATOR 6			//in both VB and PB lexer
//    #define SCE_B_IDENTIFIER 7		//in both VB and PB lexer
//    #define SCE_B_DATE 8				//VB lexer only, unsupported by PB lexer

//  - Statement added to KeyWords.cxx:      'LINK_LEXER(lmPB);'
//  - Statement added to scintilla_vc6.mak: '$(DIR_O)\LexPB.obj: ...\src\LexPB.cxx $(LEX_HEADERS)'
//
// Copyright for Scintilla: 1998-2001 by Neil Hodgson <neilh@scintilla.org>
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

static inline bool IsTypeCharacter(const int ch) {
	return ch == '%' || ch == '&' || ch == '@' || ch == '!' || ch == '#' || ch == '$';
}

static inline bool IsAWordChar(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '.' || ch == '_');
}

static inline bool IsAWordStart(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '_');
}

bool MatchUpperCase(Accessor &styler, int pos, const char *s)   //Same as styler.Match() but uppercase comparison (a-z,A-Z and space only)
{
	char ch;
	for (int i=0; *s; i++)
	{
		ch=styler.SafeGetCharAt(pos+i);
		if (ch > 0x60) ch -= '\x20';
		if (*s != ch) return false;
		s++;
	}
	return true;
}

static void ColourisePBDoc(unsigned int startPos, int length, int initStyle,WordList *keywordlists[],
						   Accessor &styler) {

	WordList &keywords = *keywordlists[0];

	styler.StartAt(startPos);

	StyleContext sc(startPos, length, initStyle, styler);

	for (; sc.More(); sc.Forward()) {

		if (sc.state == SCE_B_OPERATOR)
		{
			sc.SetState(SCE_B_DEFAULT);
		}
		else if (sc.state == SCE_B_KEYWORD)
		{
			if (!IsAWordChar(sc.ch))
			{
				if (!IsTypeCharacter(sc.ch))
				{
					if (sc.ch == ']') {sc.Forward();}
					char s[100];
					sc.GetCurrentLowered(s, sizeof(s));
					if (keywords.InList(s))
					{
						if (strcmp(s, "rem") == 0)
						{
							sc.ChangeState(SCE_B_COMMENT);
							if (sc.atLineEnd) {sc.SetState(SCE_B_DEFAULT);}
						}
						else
						{
							sc.SetState(SCE_B_DEFAULT);
						}
					}
					else
					{
						sc.ChangeState(SCE_B_IDENTIFIER);
						sc.SetState(SCE_B_DEFAULT);
					}
				}
			}
		}
		else if (sc.state == SCE_B_NUMBER)
		{
			if (!IsAWordChar(sc.ch)) {sc.SetState(SCE_B_DEFAULT);}
		}
		else if (sc.state == SCE_B_STRING)
		{
			// PB doubles quotes to preserve them, so just end this string
			// state now as a following quote will start again
			if (sc.ch == '\"')
			{
				if (tolower(sc.chNext) == 'c') {sc.Forward();}
				sc.ForwardSetState(SCE_B_DEFAULT);
			}
		}
		else if (sc.state == SCE_B_COMMENT)
		{
			if (sc.atLineEnd) {sc.SetState(SCE_B_DEFAULT);}
		}

		if (sc.state == SCE_B_DEFAULT)
		{
			if (sc.ch == '\'') {sc.SetState(SCE_B_COMMENT);}
			else if (sc.ch == '\"') {sc.SetState(SCE_B_STRING);}
			else if (sc.ch == '#')
			{	int n = 1;
				int chSeek = ' ';
				while ((n < 100) && (chSeek == ' ' || chSeek == '\t'))
				{
					chSeek = sc.GetRelative(n);
					n++;
				}
				sc.SetState(SCE_B_OPERATOR);
			}
			else if (sc.ch == '&' && tolower(sc.chNext) == 'h') {sc.SetState(SCE_B_NUMBER);}
			else if (sc.ch == '&' && tolower(sc.chNext) == 'b') {sc.SetState(SCE_B_NUMBER);}
			else if (sc.ch == '&' && tolower(sc.chNext) == 'o') {sc.SetState(SCE_B_NUMBER);}
			else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {sc.SetState(SCE_B_NUMBER);}
			else if (IsAWordStart(sc.ch) || (sc.ch == '[')) {sc.SetState(SCE_B_KEYWORD);}
			else if (isoperator(static_cast<char>(sc.ch)) || (sc.ch == '\\')) {sc.SetState(SCE_B_OPERATOR);}
		}

	}
	sc.Complete();
}

static void FoldPBDoc(unsigned int startPos, int length, int, WordList *[], Accessor &styler)
{
	// No folding enabled, no reason to continue...
	if( styler.GetPropertyInt("fold") == 0 )
		return;

	unsigned int endPos = startPos + length;
	int lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent-1) >> 16;
	int levelNext = levelCurrent;
	char chNext = styler[startPos];

	bool atEOL=1;
	for (unsigned int i = startPos; i < endPos; i++)
	{
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if( atEOL )			//Begin of a new line (The Sub/Function/Macro keywords may occur at begin of line only)
		{
			if( MatchUpperCase(styler,i,"FUNCTION") )  //else if(
			{
				styler.SetLevel(lineCurrent, (SC_FOLDLEVELBASE << 16) | SC_FOLDLEVELHEADERFLAG);
				levelNext=SC_FOLDLEVELBASE+1;
			}
			else if( MatchUpperCase(styler,i,"CALLBACK FUNCTION") )
			{
				styler.SetLevel(lineCurrent, (SC_FOLDLEVELBASE << 16) | SC_FOLDLEVELHEADERFLAG);
				levelNext=SC_FOLDLEVELBASE+1;
			}
			else if( MatchUpperCase(styler,i,"STATIC FUNCTION") )
			{
				styler.SetLevel(lineCurrent, (SC_FOLDLEVELBASE << 16) | SC_FOLDLEVELHEADERFLAG);
				levelNext=SC_FOLDLEVELBASE+1;
			}
			else if( MatchUpperCase(styler,i,"SUB") )
			{
				styler.SetLevel(lineCurrent, (SC_FOLDLEVELBASE << 16) | SC_FOLDLEVELHEADERFLAG);
				levelNext=SC_FOLDLEVELBASE+1;
			}
			else if( MatchUpperCase(styler,i,"STATIC SUB") )
			{
				styler.SetLevel(lineCurrent, (SC_FOLDLEVELBASE << 16) | SC_FOLDLEVELHEADERFLAG);
				levelNext=SC_FOLDLEVELBASE+1;
			}
			//else if( MatchUpperCase(styler,i,"MACRO") )  //ToDo: What's with single-line macros?
		}

		atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		if( atEOL )
		{
			lineCurrent++;
			levelCurrent = levelNext;
		}
	}

	if (levelNext == SC_FOLDLEVELBASE)
	{
		int levelUse = levelCurrent;
		int lev = levelUse | levelNext << 16;
		styler.SetLevel(lineCurrent, lev);
	}
}

static const char * const pbWordListDesc[] = {
	"Keywords",
	0
};

LexerModule lmPB(SCLEX_POWERBASIC, ColourisePBDoc, "powerbasic", FoldPBDoc, pbWordListDesc);
