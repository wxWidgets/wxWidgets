// Scintilla source code edit control
/** @file LexNsis.cxx
 ** Lexer for NSIS
 **/
// Copyright 2003, 2004 by Angelo Mandato <angelo [at] spaceblue [dot] com>
// Last Updated: 02/22/2004
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
// located in SciLexer.h
#define SCLEX_NSIS 43

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
#define SCE_NSIS_NUMBER 14
*/

static bool isNsisNumber(char ch)
{
  return (ch >= '0' && ch <= '9');
}

static bool isNsisChar(char ch)
{
  return (ch == '.' ) || (ch == '_' ) || isNsisNumber(ch) || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

static bool isNsisLetter(char ch)
{
  return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

static int NsisCmp( char *s1, char *s2, bool bIgnoreCase )
{
  if( bIgnoreCase )
     return CompareCaseInsensitive( s1, s2);

  return strcmp( s1, s2 );
}

static int calculateFoldNsis(unsigned int start, unsigned int end, int foldlevel, Accessor &styler )
{
  // If the word is too long, it is not what we are looking for
  if( end - start > 13 )
    return foldlevel;

  // Check the style at this point, if it is not valid, then return zero
  if( styler.StyleAt(end) != SCE_NSIS_FUNCTION && styler.StyleAt(end) != SCE_NSIS_SECTIONDEF &&
      styler.StyleAt(end) != SCE_NSIS_SUBSECTIONDEF && styler.StyleAt(end) != SCE_NSIS_IFDEFINEDEF &&
      styler.StyleAt(end) != SCE_NSIS_MACRODEF )
        return foldlevel;

  int newFoldlevel = foldlevel;
  bool bIgnoreCase = false;
  if( styler.GetPropertyInt("nsis.ignorecase") == 1 )
    bIgnoreCase = true;

  char s[15]; // The key word we are looking for has atmost 13 characters
  for (unsigned int i = 0; i < end - start + 1 && i < 14; i++)
	{
		s[i] = static_cast<char>( styler[ start + i ] );
		s[i + 1] = '\0';
	}

  if( s[0] == '!' )
  {
    if( NsisCmp(s, "!ifndef", bIgnoreCase) == 0 || NsisCmp(s, "!ifdef", bIgnoreCase ) == 0 || NsisCmp(s, "!macro", bIgnoreCase ) == 0 )
      newFoldlevel++;
    else if( NsisCmp(s, "!endif", bIgnoreCase) == 0 || NsisCmp(s, "!macroend", bIgnoreCase ) == 0 )
      newFoldlevel--;
  }
  else
  {
    if( NsisCmp(s, "Function", bIgnoreCase) == 0 || NsisCmp(s, "Section", bIgnoreCase ) == 0 || NsisCmp(s, "SubSection", bIgnoreCase ) == 0 )
      newFoldlevel++;
    else if( NsisCmp(s, "FunctionEnd", bIgnoreCase) == 0 || NsisCmp(s, "SectionEnd", bIgnoreCase ) == 0 || NsisCmp(s, "SubSectionEnd", bIgnoreCase ) == 0 )
      newFoldlevel--;
  }

  return newFoldlevel;
}

static int classifyWordNsis(unsigned int start, unsigned int end, WordList *keywordLists[], Accessor &styler )
{
  bool bIgnoreCase = false;
  if( styler.GetPropertyInt("nsis.ignorecase") == 1 )
    bIgnoreCase = true;

  bool bUserVars = false;
  if( styler.GetPropertyInt("nsis.uservars") == 1 )
    bUserVars = true;

	char s[100];

	WordList &Functions = *keywordLists[0];
	WordList &Variables = *keywordLists[1];
	WordList &Lables = *keywordLists[2];
	WordList &UserDefined = *keywordLists[3];

	for (unsigned int i = 0; i < end - start + 1 && i < 99; i++)
	{
    if( bIgnoreCase )
      s[i] = static_cast<char>( tolower(styler[ start + i ] ) );
    else
		  s[i] = static_cast<char>( styler[ start + i ] );
		s[i + 1] = '\0';
	}

	// Check for special words...
	if( NsisCmp(s, "!macro", bIgnoreCase ) == 0 || NsisCmp(s, "!macroend", bIgnoreCase) == 0 ) // Covers !micro and !microend
		return SCE_NSIS_MACRODEF;

	if( NsisCmp(s, "!ifdef", bIgnoreCase ) == 0 ||  NsisCmp(s, "!ifndef", bIgnoreCase) == 0 ||  NsisCmp(s, "!endif", bIgnoreCase) == 0 )
		return SCE_NSIS_IFDEFINEDEF;

	if( NsisCmp(s, "Section", bIgnoreCase ) == 0 || NsisCmp(s, "SectionEnd", bIgnoreCase) == 0 ) // Covers Section and SectionEnd
		return SCE_NSIS_SECTIONDEF;

	if( NsisCmp(s, "SubSection", bIgnoreCase) == 0 || NsisCmp(s, "SubSectionEnd", bIgnoreCase) == 0 ) // Covers SubSection and SubSectionEnd
		return SCE_NSIS_SUBSECTIONDEF;

	if( NsisCmp(s, "Function", bIgnoreCase) == 0 || NsisCmp(s, "FunctionEnd", bIgnoreCase) == 0 ) // Covers SubSection and SubSectionEnd
		return SCE_NSIS_FUNCTION;

	if ( Functions.InList(s) )
		return SCE_NSIS_FUNCTION;

	if ( Variables.InList(s) )
		return SCE_NSIS_VARIABLE;

	if ( Lables.InList(s) )
		return SCE_NSIS_LABEL;

	if( UserDefined.InList(s) )
		return SCE_NSIS_USERDEFINED;

	if( strlen(s) > 3 )
	{
		if( s[1] == '{' && s[strlen(s)-1] == '}' )
			return SCE_NSIS_VARIABLE;
	}

  // See if the variable is a user defined variable
  if( s[0] == '$' && bUserVars )
  {
    bool bHasSimpleNsisChars = true;
    for (unsigned int j = 1; j < end - start + 1 && j < 99; j++)
	  {
      if( !isNsisChar( s[j] ) )
      {
        bHasSimpleNsisChars = false;
        break;
      }
	  }

    if( bHasSimpleNsisChars )
      return SCE_NSIS_VARIABLE;
  }

  // To check for numbers
  if( isNsisNumber( s[0] ) )
  {
    bool bHasSimpleNsisNumber = true;
    for (unsigned int j = 1; j < end - start + 1 && j < 99; j++)
	  {
      if( s[j] == '\0' || s[j] == '\r' || s[j] == '\n' )
        break;

      if( !isNsisNumber( s[j] ) )
      {
        bHasSimpleNsisNumber = false;
        break;
      }
	  }

    if( bHasSimpleNsisNumber )
      return SCE_NSIS_NUMBER;
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
	bool bVarInString = false;
  bool bClassicVarInString = false;

	unsigned int i;
	for( i = startPos; i < nLengthDoc; i++ )
	{
		cCurrChar = styler.SafeGetCharAt( i );
		char cNextChar = styler.SafeGetCharAt(i+1);

		switch(state)
		{
			case SCE_NSIS_DEFAULT:
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
          bClassicVarInString = false;
					break;
				}
				if( cCurrChar == '\'' )
				{
					styler.ColourTo(i-1, state );
					state = SCE_NSIS_STRINGRQ;
					bVarInString = false;
          bClassicVarInString = false;
					break;
				}
				if( cCurrChar == '`' )
				{
					styler.ColourTo(i-1, state );
					state = SCE_NSIS_STRINGLQ;
					bVarInString = false;
          bClassicVarInString = false;
					break;
				}

				// NSIS KeyWord,Function, Variable, UserDefined:
				if( cCurrChar == '$' || isNsisChar(cCurrChar) || cCurrChar == '!' )
				{
					styler.ColourTo(i-1,state);
				  state = SCE_NSIS_FUNCTION;

          // If it is a number, we must check and set style here first...
          if( isNsisNumber(cCurrChar) && (cNextChar == '\t' || cNextChar == ' ' || cNextChar == '\r' || cNextChar == '\n' ) )
              styler.ColourTo( i, SCE_NSIS_NUMBER);

					break;
				}
				break;
			case SCE_NSIS_COMMENT:
				if( cNextChar == '\n' || cNextChar == '\r' )
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
        if( cCurrChar == '$' )
          state = SCE_NSIS_DEFAULT;
        else if( cCurrChar == '\\' && (cNextChar == 'n' || cNextChar == 'r' || cNextChar == 't' ) )
          state = SCE_NSIS_DEFAULT;
				else if( (isNsisChar(cCurrChar) && !isNsisChar( cNextChar) && cNextChar != '}') || cCurrChar == '}' )
				{
					state = classifyWordNsis( styler.GetStartSegment(), i, keywordLists, styler);
					styler.ColourTo( i, state);
					state = SCE_NSIS_DEFAULT;
				}
				else if( !isNsisChar( cCurrChar ) && cCurrChar != '{' && cCurrChar != '}' )
				{
          if( classifyWordNsis( styler.GetStartSegment(), i-1, keywordLists, styler) == SCE_NSIS_NUMBER )
             styler.ColourTo( i-1, SCE_NSIS_NUMBER );

					state = SCE_NSIS_DEFAULT;

					if( cCurrChar == '"' )
					{
						state = SCE_NSIS_STRINGDQ;
						bVarInString = false;
            bClassicVarInString = false;
					}
					else if( cCurrChar == '`' )
					{
						state = SCE_NSIS_STRINGLQ;
						bVarInString = false;
            bClassicVarInString = false;
					}
					else if( cCurrChar == '\'' )
					{
						state = SCE_NSIS_STRINGRQ;
						bVarInString = false;
            bClassicVarInString = false;
					}
					else if( cCurrChar == '#' || cCurrChar == ';' )
          {
						state = SCE_NSIS_COMMENT;
          }
				}
				break;
		}

		if( state == SCE_NSIS_COMMENT )
		{
			styler.ColourTo(i,state);
		}
		else if( state == SCE_NSIS_STRINGDQ || state == SCE_NSIS_STRINGLQ || state == SCE_NSIS_STRINGRQ )
		{
      bool bIngoreNextDollarSign = false;
      bool bUserVars = false;
      if( styler.GetPropertyInt("nsis.uservars") == 1 )
        bUserVars = true;

      if( bVarInString && cCurrChar == '$' )
      {
        bVarInString = false;
        bIngoreNextDollarSign = true;
      }
      else if( bVarInString && cCurrChar == '\\' && (cNextChar == 'n' || cNextChar == 'r' || cNextChar == 't' ) )
      {
        bVarInString = false;
        bIngoreNextDollarSign = true;
      }

      // Covers "$INSTDIR and user vars like $MYVAR"
      else if( bVarInString && !isNsisChar(cNextChar) )
      {
        int nWordState = classifyWordNsis( styler.GetStartSegment(), i, keywordLists, styler);
				if( nWordState == SCE_NSIS_VARIABLE )
					styler.ColourTo( i, SCE_NSIS_STRINGVAR);
        else if( bUserVars )
          styler.ColourTo( i, SCE_NSIS_STRINGVAR);
        bVarInString = false;
      }
      // Covers "${TEST}..."
      else if( bClassicVarInString && cNextChar == '}' )
      {
        styler.ColourTo( i+1, SCE_NSIS_STRINGVAR);
				bClassicVarInString = false;
      }

      // Start of var in string
			if( !bIngoreNextDollarSign && cCurrChar == '$' && cNextChar == '{' )
			{
				styler.ColourTo( i-1, state);
				bClassicVarInString = true;
        bVarInString = false;
			}
      else if( !bIngoreNextDollarSign && cCurrChar == '$' )
      {
        styler.ColourTo( i-1, state);
        bVarInString = true;
        bClassicVarInString = false;
      }
		}
	}

  // Colourise remaining document
  switch( state )
  {
    case SCE_NSIS_COMMENT:
    case SCE_NSIS_STRINGDQ:
    case SCE_NSIS_STRINGLQ:
    case SCE_NSIS_STRINGRQ:
    case SCE_NSIS_VARIABLE:
    case SCE_NSIS_STRINGVAR:
      styler.ColourTo(nLengthDoc-1,state); break;

    default:
      styler.ColourTo(nLengthDoc-1,SCE_NSIS_DEFAULT); break;
  }
}

static void FoldNsisDoc(unsigned int startPos, int length, int, WordList *[], Accessor &styler)
{
	// No folding enabled, no reason to continue...
	if( styler.GetPropertyInt("fold") == 0 )
		return;

  int lineCurrent = styler.GetLine(startPos);
  unsigned int safeStartPos = styler.LineStart( lineCurrent );

  bool bArg1 = true;
  int nWordStart = -1;

  int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent-1) >> 16;
	int levelNext = levelCurrent;

  for (unsigned int i = safeStartPos; i < startPos + length; i++)
	{
    char chCurr = styler.SafeGetCharAt(i);

    if( bArg1 ) //&& chCurr != '\n' )
    {
      if( nWordStart == -1 && (isNsisLetter(chCurr) || chCurr == '!') )
        nWordStart = i;
      else if( !isNsisLetter(chCurr) && nWordStart > -1 )
      {
        int newLevel = calculateFoldNsis( nWordStart, i-1, levelNext, styler );
        if( newLevel != levelNext )
          levelNext = newLevel;
        bArg1 = false;
      }
    }

    if( chCurr == '\n' )
    {
      // If we are on a new line...
      int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (levelUse < levelNext)
				lev |= SC_FOLDLEVELHEADERFLAG;
			if (lev != styler.LevelAt(lineCurrent))
				styler.SetLevel(lineCurrent, lev);

			lineCurrent++;
			levelCurrent = levelNext;
      bArg1 = true; // New line, lets look at first argument again
      nWordStart = -1;
    }
  }

	int levelUse = levelCurrent;
	int lev = levelUse | levelNext << 16;
	if (levelUse < levelNext)
		lev |= SC_FOLDLEVELHEADERFLAG;
	if (lev != styler.LevelAt(lineCurrent))
		styler.SetLevel(lineCurrent, lev);
}

static const char * const nsisWordLists[] = {
	"Functions",
	"Variables",
	"Lables",
	"UserDefined",
	0, };


LexerModule lmNsis(SCLEX_NSIS, ColouriseNsisDoc, "nsis", FoldNsisDoc, nsisWordLists);
