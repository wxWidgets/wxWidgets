// Scintilla source code edit control
// @file LexAU3.cxx
// Lexer for AutoIt3  http://www.hiddensoft.com/autoit3
// by Jos van der Zande, jvdzande@yahoo.com 
//
// Changes:
// March 28, 2004 - Added the standard Folding code
// April 21, 2004 - Added Preprosessor Table + Syntax Highlighting
//                  Fixed Number highlighting
//                  Changed default isoperator to IsAOperator to have a better match to AutoIt3
//                  Fixed "#comments_start" -> "#comments-start"  
//                  Fixed "#comments_end" -> "#comments-end"  
//                  Fixed Sendkeys in Strings when not terminated with }
//                  Added support for Sendkey strings that have second parameter e.g. {UP 5} or {a down}
// April 26, 2004   Fixed # pre-processor statement inside of comment block would invalidly change the color.
//                  Added logic for #include <xyz.au3> to treat the <> as string
//                  Added underscore to IsAOperator.
// Copyright for Scintilla: 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
// Scintilla source code edit control

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

static bool IsAU3Comment(Accessor &styler, int pos, int len) {
	return len>0 && styler[pos]==';';
}

static inline bool IsTypeCharacter(const int ch)
{
    return ch == '$';
}
static inline bool IsAWordChar(const int ch)
{
    return (ch < 0x80) && (isalnum(ch) || ch == '_' || ch == '-');
}

static inline bool IsAWordStart(const int ch)
{
    return (ch < 0x80) && (isalnum(ch) || ch == '_' || ch == '@' || ch == '#' || ch == '$');
}

static inline bool IsAOperator(char ch) {
	if (isascii(ch) && isalnum(ch))
		return false;
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/' ||
	    ch == '&' || ch == '^' || ch == '=' || ch == '<' || ch == '>' ||
	    ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '_' )
		return true;
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// GetSendKey() filters the portion before and after a/multiple space(s)
// and return the first portion to be looked-up in the table
// also check if the second portion is valid... (up,down.on.off,toggle or a number)
///////////////////////////////////////////////////////////////////////////////

static int GetSendKey(const char *szLine, char *szKey)
{
	int		nFlag	= 0;
	int		nKeyPos	= 0;
	int		nSpecPos= 0;
	int		nSpecNum= 1;
	int		nPos	= 0;
	char	cTemp;
	char	szSpecial[100];

	// split the portion of the sendkey in the part before and after the spaces
	while ( ( (cTemp = szLine[nPos]) != '\0'))
	{
		if ((cTemp == ' ') && (nFlag == 0) ) // get the stuff till first space
		{
			nFlag = 1;
			// Add } to the end of the first bit for table lookup later.
			szKey[nKeyPos++] = '}';
		}
		else if (cTemp == ' ')
		{
			// skip other spaces 
		}
		else if (nFlag == 0)
		{
			// save first portion into var till space or } is hit
			szKey[nKeyPos++] = cTemp;
		}
		else if ((nFlag == 1) && (cTemp != '}'))
		{
			// Save second portion into var...
			szSpecial[nSpecPos++] = cTemp;
			// check if Second portion is all numbers for repeat fuction
			if (isdigit(cTemp) == false) {nSpecNum = 0;} 
		}
		nPos++;									// skip to next char

	} // End While


	// Check if the second portion is either a number or one of these keywords
	szKey[nKeyPos] = '\0';
	szSpecial[nSpecPos] = '\0';
	if (strcmp(szSpecial,"down")==0    || strcmp(szSpecial,"up")==0  ||
		strcmp(szSpecial,"on")==0      || strcmp(szSpecial,"off")==0 || 
		strcmp(szSpecial,"toggle")==0  || nSpecNum == 1 )
	{
		nFlag = 0;
	}
	else
	{
		nFlag = 1;
	}
	return nFlag;  // 1 is bad, 0 is good

} // GetSendKey()

static void ColouriseAU3Doc(unsigned int startPos, 
							int length, int initStyle,
							WordList *keywordlists[],
							Accessor &styler) {

    WordList &keywords = *keywordlists[0];
    WordList &keywords2 = *keywordlists[1];
    WordList &keywords3 = *keywordlists[2];
    WordList &keywords4 = *keywordlists[3];
    WordList &keywords5 = *keywordlists[4];
    styler.StartAt(startPos);

    StyleContext sc(startPos, length, initStyle, styler);
	char si;     // string indicator "=1 '=2
	si=0;
	//$$$
    for (; sc.More(); sc.Forward()) {
		char s[100];
		sc.GetCurrentLowered(s, sizeof(s));
		switch (sc.state)
        {
            case SCE_AU3_COMMENTBLOCK:
            {
				if (!IsAWordChar(sc.ch))
				{
					if ((strcmp(s, "#ce")==0 || strcmp(s, "#comments-end")==0)) 
					{sc.SetState(SCE_AU3_COMMENT);}  // set to comment line for the rest of the line
					else
					{sc.SetState(SCE_AU3_COMMENTBLOCK);}
				}
                break;
			}
            case SCE_AU3_COMMENT:
            {
                if (sc.atLineEnd) {sc.SetState(SCE_AU3_DEFAULT);}
                break;
            }
            case SCE_AU3_OPERATOR:
            {
                sc.SetState(SCE_AU3_DEFAULT);
                break;
            }
            case SCE_AU3_KEYWORD:
            {
                if (!IsAWordChar(sc.ch))
                {
                    if (!IsTypeCharacter(sc.ch))
                    {
						if (strcmp(s, "#cs")==0 || strcmp(s, "#comments-start")==0 )
						{
							sc.ChangeState(SCE_AU3_COMMENTBLOCK);
							sc.SetState(SCE_AU3_COMMENTBLOCK);
						}
						else if (keywords.InList(s)) {
							sc.ChangeState(SCE_AU3_KEYWORD);
							sc.SetState(SCE_AU3_DEFAULT);
						}
						else if (keywords2.InList(s)) {
							sc.ChangeState(SCE_AU3_FUNCTION);
							sc.SetState(SCE_AU3_DEFAULT);
						}
						else if (keywords3.InList(s)) {
							sc.ChangeState(SCE_AU3_MACRO);
							sc.SetState(SCE_AU3_DEFAULT);
						}
						else if (keywords5.InList(s)) {
							sc.ChangeState(SCE_AU3_PREPROCESSOR);
							sc.SetState(SCE_AU3_DEFAULT);
							if (strcmp(s, "#include")==0)
							{
								si = 3;   // use to determine string start for #inlude <>
							}
						}
						else if (!IsAWordChar(sc.ch)) {
							sc.ChangeState(SCE_AU3_DEFAULT);
							sc.SetState(SCE_AU3_DEFAULT);
						}
					}
				}	
                if (sc.atLineEnd) {sc.SetState(SCE_AU3_DEFAULT);}
                break;
            }
            case SCE_AU3_NUMBER:
            {
                if (!IsAWordChar(sc.ch)) {sc.SetState(SCE_AU3_DEFAULT);}
                break;
            }
            case SCE_AU3_VARIABLE:
            {
                if (!IsAWordChar(sc.ch)) {sc.SetState(SCE_AU3_DEFAULT);}
                break;
            }
            case SCE_AU3_STRING:
            {
				// check for " to end a double qouted string or
				// check for ' to end a single qouted string 
	            if ((si == 1 && sc.ch == '\"') || (si == 2 && sc.ch == '\'') || (si == 3 && sc.ch == '>'))
				{
					sc.ForwardSetState(SCE_AU3_DEFAULT);
				}
                if (sc.atLineEnd) {sc.SetState(SCE_AU3_DEFAULT);}
				// find Sendkeys in a STRING
				if (sc.ch == '{') {sc.SetState(SCE_AU3_SENT);}
				if (sc.ch == '+' && sc.chNext == '{') {sc.SetState(SCE_AU3_SENT);}
				if (sc.ch == '!' && sc.chNext == '{') {sc.SetState(SCE_AU3_SENT);}
				if (sc.ch == '^' && sc.chNext == '{') {sc.SetState(SCE_AU3_SENT);}
				if (sc.ch == '#' && sc.chNext == '{') {sc.SetState(SCE_AU3_SENT);}
				break;
            }
            
            case SCE_AU3_SENT:
            {
				// Send key string ended 
				if (sc.chPrev == '}' && sc.ch != '}') 
				{
					// set color to SENDKEY when valid sendkey .. else set back to regular string
					char sk[100];
					// split {111 222} and return {111} and check if 222 is valid.
					// if return code = 1 then invalid 222 so must be string
					if (GetSendKey(s,sk))   
					{
						sc.ChangeState(SCE_AU3_STRING);
					}
					// if single char between {?} then its ok as sendkey for a single character
					else if (strlen(sk) == 3)  
					{
						sc.ChangeState(SCE_AU3_SENT);
					}
					// if sendkey {111} is in table then ok as sendkey
					else if (keywords4.InList(sk)) 
					{
						sc.ChangeState(SCE_AU3_SENT);
					}
					else
					{
						sc.ChangeState(SCE_AU3_STRING);
					}
					sc.SetState(SCE_AU3_STRING);
				}
				// check if next portion is again a sendkey
				if (sc.atLineEnd) 
				{
					sc.SetState(SCE_AU3_DEFAULT);
					si = 0;  // reset string indicator
				}
				if (sc.ch == '{' && sc.chPrev != '{') {sc.SetState(SCE_AU3_SENT);}
				if (sc.ch == '+' && sc.chNext == '{') {sc.SetState(SCE_AU3_SENT);}
				if (sc.ch == '!' && sc.chNext == '{') {sc.SetState(SCE_AU3_SENT);}
				if (sc.ch == '^' && sc.chNext == '{') {sc.SetState(SCE_AU3_SENT);}
				if (sc.ch == '#' && sc.chNext == '{') {sc.SetState(SCE_AU3_SENT);}
				// check to see if the string ended...
				// Sentkey string isn't complete but the string ended....
				if ((si == 1 && sc.ch == '\"') || (si == 2 && sc.ch == '\''))
				{
					sc.ChangeState(SCE_AU3_STRING);
					sc.ForwardSetState(SCE_AU3_DEFAULT);
				}
				break;
            }
        }  //switch (sc.state)

        // Determine if a new state should be entered:

		if (sc.state == SCE_AU3_DEFAULT)
        {
            if (sc.ch == ';') {sc.SetState(SCE_AU3_COMMENT);}
            else if (sc.ch == '#') {sc.SetState(SCE_AU3_KEYWORD);}
            else if (sc.ch == '$') {sc.SetState(SCE_AU3_VARIABLE);}
            else if (sc.ch == '@') {sc.SetState(SCE_AU3_KEYWORD);}
            else if (sc.ch == '<' && si==3) {sc.SetState(SCE_AU3_STRING);}  // string after #include 
            else if (sc.ch == '\"') {
				sc.SetState(SCE_AU3_STRING);
				si = 1;	}
            else if (sc.ch == '\'') {
				sc.SetState(SCE_AU3_STRING);
				si = 2;	}
            else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {sc.SetState(SCE_AU3_NUMBER);}
            else if (IsAOperator(static_cast<char>(sc.ch))) {sc.SetState(SCE_AU3_OPERATOR);}
            else if (IsAWordStart(sc.ch)) {sc.SetState(SCE_AU3_KEYWORD);}
			else if (sc.atLineEnd) {sc.SetState(SCE_AU3_DEFAULT);}
        }
    }      //for (; sc.More(); sc.Forward())
    sc.Complete();
}

//
//
static void FoldAU3Doc(unsigned int startPos, int length, int, WordList *[], Accessor &styler)
{
		int endPos = startPos + length;

	// Backtrack to previous line in case need to fix its fold status
	int lineCurrent = styler.GetLine(startPos);
	if (startPos > 0) {
		if (lineCurrent > 0) {
			lineCurrent--;
			startPos = styler.LineStart(lineCurrent);
		}
	}
	int spaceFlags = 0;
	int indentCurrent = styler.IndentAmount(lineCurrent, &spaceFlags, IsAU3Comment);
	char chNext = styler[startPos];
	for (int i = startPos; i < endPos; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if ((ch == '\r' && chNext != '\n') || (ch == '\n') || (i == endPos)) {
			int lev = indentCurrent;
			int indentNext = styler.IndentAmount(lineCurrent + 1, &spaceFlags, IsAU3Comment);
			if (!(indentCurrent & SC_FOLDLEVELWHITEFLAG)) {
				// Only non whitespace lines can be headers
				if ((indentCurrent & SC_FOLDLEVELNUMBERMASK) < (indentNext & SC_FOLDLEVELNUMBERMASK)) {
					lev |= SC_FOLDLEVELHEADERFLAG;
				} else if (indentNext & SC_FOLDLEVELWHITEFLAG) {
					// Line after is blank so check the next - maybe should continue further?
					int spaceFlags2 = 0;
					int indentNext2 = styler.IndentAmount(lineCurrent + 2, &spaceFlags2, IsAU3Comment);
					if ((indentCurrent & SC_FOLDLEVELNUMBERMASK) < (indentNext2 & SC_FOLDLEVELNUMBERMASK)) {
						lev |= SC_FOLDLEVELHEADERFLAG;
					}
				}
			}
			indentCurrent = indentNext;
			styler.SetLevel(lineCurrent, lev);
			lineCurrent++;
		}
	}

}


//

static const char * const AU3WordLists[] = {
    "#autoit keywords",
    "#autoit functions",
    "#autoit macros",
    "#autoit Sent keys",
    "#autoit Pre-processors",
    0
};
LexerModule lmAU3(SCLEX_AU3, ColouriseAU3Doc, "au3", FoldAU3Doc , AU3WordLists);
