// Scintilla source code edit control
/** @file LexNsis.cxx
 ** Lexer for NSIS
 **/
// Copyright 2003 by Angelo Mandato <angelo@spaceblue.com>
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

/*
// Put in SciLexer.h
#define SCLEX_NSIS 34

#define SCE_NSIS_DEFAULT 0
#define SCE_NSIS_COMMENT 1
#define SCE_NSIS_STRINGDQ 2
#define SCE_NSIS_STRINGLQ 3
#define SCE_NSIS_STRINGRQ 4
#define SCE_NSIS_FUNCTION 5
#define SCE_NSIS_VARIABLE 6
#define SCE_NSIS_LABEL 7
#define SCE_NSIS_USERDEFINED 8
#define SCE_NSIS_SECTIONDEF 9
#define SCE_NSIS_SUBSECTIONDEF 10
#define SCE_NSIS_IFDEFINEDEF 11
#define SCE_NSIS_MACRODEF 12
#define SCE_NSIS_STRINGVAR 13
*/

static int classifyWordNsis(unsigned int start, unsigned int end, WordList *keywordLists[], Accessor &styler)
{
	char s[100];

	WordList &Functions = *keywordLists[0];
	WordList &Variables = *keywordLists[1];
	WordList &Lables = *keywordLists[2];
	WordList &UserDefined = *keywordLists[3];

	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++)
	{
		s[i] = static_cast<char>( styler[ start + i ] );
		s[i + 1] = '\0';
	}

	// Check for special words...

	if( strcmp(s, "!macro") == 0 || strcmp(s, "!macroend") == 0 ) // Covers !micro and !microend
		return SCE_NSIS_MACRODEF;

	if( strcmp(s, "!ifdef") == 0 ||  strcmp(s, "!ifndef") == 0 ||  strcmp(s, "!endif") == 0 )
		return SCE_NSIS_IFDEFINEDEF;

	if( strcmp(s, "Section") == 0 || strcmp(s, "SectionEnd") == 0 ) // Covers Section and SectionEnd
		return SCE_NSIS_SECTIONDEF;

	if( strcmp(s, "SubSection") == 0 || strcmp(s, "SubSectionEnd") == 0 ) // Covers SubSection and SubSectionEnd
		return SCE_NSIS_SUBSECTIONDEF;

	if( strcmp(s, "Function") == 0 || strcmp(s, "FunctionEnd") == 0 ) // Covers SubSection and SubSectionEnd
		return SCE_NSIS_FUNCTION;

	if ( Functions.InList(s) )
		return SCE_NSIS_FUNCTION;

	if ( Variables.InList(s) )
		return SCE_NSIS_VARIABLE;

	if ( Lables.InList(s) )
		return SCE_NSIS_LABEL;

	if( UserDefined.InList(s) )
		return SCE_NSIS_USERDEFINED;

	if( strlen(s) > 2 )
	{
		if( s[1] == '{' && s[strlen(s)-1] == '}' )
			return SCE_NSIS_VARIABLE;
	}

	return SCE_NSIS_DEFAULT;
}

static void ColouriseNsisDoc(unsigned int startPos, int length, int, WordList *keywordLists[], Accessor &styler)
{
	int state = SCE_NSIS_DEFAULT;
	styler.StartAt( startPos );
	styler.GetLine( startPos );

	unsigned int nLengthDoc = startPos + length;
	styler.StartSegment( startPos );

	char cCurrChar;
	bool bVarInString = true;

	unsigned int i;
	for( i = startPos; i < nLengthDoc; i++ )
	{
		cCurrChar = styler.SafeGetCharAt( i );
		char cNextChar = styler.SafeGetCharAt( i+1, EOF );



		switch(state)
		{
			case SCE_NSIS_DEFAULT:
        if( cNextChar == EOF )
        {
          styler.ColourTo(i,SCE_NSIS_DEFAULT);
			    break;
        }
				if( cCurrChar == ';' || cCurrChar == '#' ) // we have a comment line
				{
					styler.ColourTo(i-1, state );
					state = SCE_NSIS_COMMENT;
					break;
				}
				if( cCurrChar == '"' )
				{
					styler.ColourTo(i-1, state );
					state = SCE_NSIS_STRINGDQ;
					bVarInString = false;
					break;
				}
				if( cCurrChar == '\'' )
				{
					styler.ColourTo(i-1, state );
					state = SCE_NSIS_STRINGRQ;
					bVarInString = false;
					break;
				}
				if( cCurrChar == '`' )
				{
					styler.ColourTo(i-1, state );
					state = SCE_NSIS_STRINGLQ;
					bVarInString = false;
					break;
				}

				// NSIS KeyWord,Function, Variable, UserDefined:
				if( cCurrChar == '$' || iswordchar(cCurrChar) || cCurrChar == '!' )
				{
					styler.ColourTo(i-1,state);
				  state = SCE_NSIS_FUNCTION;
					break;
				}
				break;
			case SCE_NSIS_COMMENT:
				if( cNextChar == '\n' || cNextChar == '\r' || cNextChar == EOF )
        {
				  styler.ColourTo(i,state);
          state = SCE_NSIS_DEFAULT;
        }
				break;
			case SCE_NSIS_STRINGDQ:
				if( cCurrChar == '"' || cNextChar == '\r' || cNextChar == '\n' )
				{
					styler.ColourTo(i,SCE_NSIS_STRINGDQ);
				  state = SCE_NSIS_DEFAULT;
				}
				break;
			case SCE_NSIS_STRINGLQ:
				if( cCurrChar == '`' || cNextChar == '\r' || cNextChar == '\n' )
				{
					styler.ColourTo(i,SCE_NSIS_STRINGLQ);
				  state = SCE_NSIS_DEFAULT;
				}
				break;
			case SCE_NSIS_STRINGRQ:
				if( cCurrChar == '\'' || cNextChar == '\r' || cNextChar == '\n' )
				{
					styler.ColourTo(i,SCE_NSIS_STRINGRQ);
				  state = SCE_NSIS_DEFAULT;
				}
				break;
			case SCE_NSIS_FUNCTION:

				// NSIS KeyWord:
				if( (iswordchar(cCurrChar) && !iswordchar( cNextChar) && cNextChar != '}') || cCurrChar == '}' )
				{
					state = classifyWordNsis( styler.GetStartSegment(), i, keywordLists, styler);
					styler.ColourTo( i, state);
					state = SCE_NSIS_DEFAULT; // Everything after goes back to the default state
				}
				else if( !iswordchar( cCurrChar ) && cCurrChar != '{' && cCurrChar != '}' )
				{
					state = SCE_NSIS_DEFAULT;

					if( cCurrChar == '"' ) // Next
					{
						state = SCE_NSIS_STRINGDQ;
						bVarInString = false;
					}
					if( cCurrChar == '`' )
					{
						state = SCE_NSIS_STRINGLQ;
						bVarInString = false;
					}
					if( cCurrChar == '\'' )
					{
						state = SCE_NSIS_STRINGRQ;
						bVarInString = false;
					}
					if( cCurrChar == '#' || cCurrChar == ';' )
						state = SCE_NSIS_COMMENT;

					styler.ColourTo( i, state);
				}
				break;
		}

		if( state == SCE_NSIS_COMMENT )
		{
			styler.ColourTo(i,state);
		}
		else if( state == SCE_NSIS_STRINGDQ || state == SCE_NSIS_STRINGLQ || state == SCE_NSIS_STRINGRQ )
		{
			// Check for var in String..
			if( bVarInString && (iswordchar(cCurrChar) || cCurrChar == '}') ) // || cCurrChar == '{' ) )
			{
				int nWordState = classifyWordNsis( styler.GetStartSegment(), i, keywordLists, styler);
				if( nWordState == SCE_NSIS_VARIABLE )
				{
					styler.ColourTo( i, SCE_NSIS_STRINGVAR);
					bVarInString = false;
				}
			}
			if( cCurrChar == '$' )
			{
				styler.ColourTo( i-1, state);
				bVarInString = true;
			}
		}
	}
}


static void FoldNsisDoc(unsigned int startPos, int length, int, WordList *[], Accessor &styler)
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
	int styleNext = styler.StyleAt(startPos);
	int style;

	for (unsigned int i = startPos; i < endPos; i++)
	{
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		// Functions Start: Function, Section, SubSection
		// Functions End: FunctionEnd, SectionEnd, SubSectionEnd
		// Label Start: !ifdef, !ifndef
		// Label End: !endif

		if( style == SCE_NSIS_FUNCTION )
		{
			if( styler.Match(i, "FunctionEnd") )
				levelNext--;
			else if( styler.Match(i, "Function") )
				levelNext++;
		}
		else if( style == SCE_NSIS_SECTIONDEF )
		{
			if( styler.Match(i, "SectionEnd") )
				levelNext--;
			else if( styler.Match(i, "Section") )
				levelNext++;
		}
		else if( style == SCE_NSIS_SUBSECTIONDEF )
		{
			if( styler.Match(i, "SubSectionEnd") )
				levelNext--;
			else if( styler.Match(i, "SubSection") )
				levelNext++;
		}
		else if( style == SCE_NSIS_IFDEFINEDEF )
		{
			if( styler.Match(i, "!endif") )
				levelNext--;
			else if( styler.Match(i, "!ifdef") || styler.Match(i, "!ifndef"))
				levelNext++;
		}
		else if( style == SCE_NSIS_MACRODEF )
		{
			if( styler.Match(i, "!macroend") )
				levelNext--;
			else if( styler.Match(i, "!macro") )
				levelNext++;
		}

		if( atEOL )
		{
			int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (levelUse < levelNext)
				lev |= SC_FOLDLEVELHEADERFLAG;
			if (lev != styler.LevelAt(lineCurrent))
			{
				styler.SetLevel(lineCurrent, lev);
			}
			lineCurrent++;
			levelCurrent = levelNext;
		}
	}

	int levelUse = levelCurrent;
	int lev = levelUse | levelNext << 16;
	if (levelUse < levelNext)
		lev |= SC_FOLDLEVELHEADERFLAG;
	if (lev != styler.LevelAt(lineCurrent))
	{
		styler.SetLevel(lineCurrent, lev);
	}
}

static const char * const nsisWordLists[] = {
	"Functions",
	"Variables",
	"Lables",
	"UserDefined",
	0, };


LexerModule lmNsis(SCLEX_NSIS, ColouriseNsisDoc, "nsis", FoldNsisDoc, nsisWordLists);
