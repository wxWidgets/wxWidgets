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
// April 26, 2004 - Fixed # pre-processor statement inside of comment block would invalidly change the color.
//                  Added logic for #include <xyz.au3> to treat the <> as string
//                  Added underscore to IsAOperator.
// May 17, 2004   - Changed the folding logic from indent to keyword folding.
//                  Added Folding logic for blocks of single-commentlines or commentblock.
//                        triggered by: fold.comment=1
//                  Added Folding logic for preprocessor blocks triggered by fold.preprocessor=1
//                  Added Special for #region - #endregion syntax highlight and folding.
// May 30, 2004   - Fixed issue with continuation lines on If statements.
// June 5, 2004   - Added comma to Operators for better readability.
//                  Added fold.compact support set with fold.compact=1
//                  Changed folding inside of #cs-#ce. Default is no keyword folding inside comment blocks when fold.comment=1
//                        it will now only happen when fold.comment=2.
// 
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

static inline bool IsTypeCharacter(const int ch)
{
    return ch == '$';
}
static inline bool IsAWordChar(const int ch)
{
    return (ch < 0x80) && (isalnum(ch) || ch == '_');
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
	    ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == ',' )
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
	if (strcmp(szSpecial,"down")== 0    || strcmp(szSpecial,"up")== 0  ||
		strcmp(szSpecial,"on")== 0      || strcmp(szSpecial,"off")== 0 || 
		strcmp(szSpecial,"toggle")== 0  || nSpecNum == 1 )
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
    WordList &keywords6 = *keywordlists[5];
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
				if (!(IsAWordChar(sc.ch) || (sc.ch == '-' && strcmp(s, "#comments") == 0)))
				{
					if ((strcmp(s, "#ce")== 0 || strcmp(s, "#comments-end")== 0)) 
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
            case SCE_AU3_SPECIAL:
            {
                if (sc.atLineEnd) {sc.SetState(SCE_AU3_DEFAULT);}
                break;
            }
            case SCE_AU3_KEYWORD:
            {
                if (!(IsAWordChar(sc.ch) || (sc.ch == '-' && (strcmp(s, "#comments") == 0 || strcmp(s, "#include") == 0))))
                {
                    if (!IsTypeCharacter(sc.ch))
                    {
						if (strcmp(s, "#cs")== 0 || strcmp(s, "#comments-start")== 0 )
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
							if (strcmp(s, "#include")== 0)
							{
								si = 3;   // use to determine string start for #inlude <>
							}
						}
						else if (keywords6.InList(s)) {
							sc.ChangeState(SCE_AU3_SPECIAL);
							sc.SetState(SCE_AU3_SPECIAL);
						}
						else if (strcmp(s, "_") == 0) {
							sc.ChangeState(SCE_AU3_OPERATOR);
							sc.SetState(SCE_AU3_DEFAULT);
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
            else if (IsAWordStart(sc.ch)) {sc.SetState(SCE_AU3_KEYWORD);}
            else if (IsAOperator(static_cast<char>(sc.ch))) {sc.SetState(SCE_AU3_OPERATOR);}
			else if (sc.atLineEnd) {sc.SetState(SCE_AU3_DEFAULT);}
        }
    }      //for (; sc.More(); sc.Forward())
    sc.Complete();
}

//
static bool IsStreamCommentStyle(int style) {
	return style == SCE_AU3_COMMENT || style == SCE_AU3_COMMENTBLOCK;
}

//
// Routine to find first none space on the current line and return its Style
// needed for comment lines not starting on pos 1 
static int GetStyleFirstWord(unsigned int szLine, Accessor &styler)
{
	int nsPos = styler.LineStart(szLine);
	int nePos = styler.LineStart(szLine+1) - 1;
	while (isspacechar(styler.SafeGetCharAt(nsPos)) && nsPos < nePos)
	{
		nsPos++; // skip to next char

	} // End While
	return styler.StyleAt(nsPos);

} // GetStyleFirstWord()

//
// Routine to check the last "none comment" character on a line to see if its a continuation
// 
static bool IsContinuationLine(unsigned int szLine, Accessor &styler)
{
	int nsPos = styler.LineStart(szLine);
	int nePos = styler.LineStart(szLine+1) - 2;
	//int stylech = styler.StyleAt(nsPos);
	while (nsPos < nePos)
	{
		//stylech = styler.StyleAt(nePos);
		int stylech = styler.StyleAt(nsPos);
		if (!(stylech == SCE_AU3_COMMENT)) {
			char ch = styler.SafeGetCharAt(nePos);
			if (!isspacechar(ch)) {
				if (ch == '_')
					return true;
				else
					return false;
			}
		}
		nePos--; // skip to next char
	} // End While
	return false;
} // IsContinuationLine()


//
static void FoldAU3Doc(unsigned int startPos, int length, int, WordList *[], Accessor &styler)
{
	int endPos = startPos + length;
	// get settings from the config files for folding comments and preprocessor lines
	bool foldComment = styler.GetPropertyInt("fold.comment") != 0;
	bool foldInComment = styler.GetPropertyInt("fold.comment") == 2;
	bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;
	bool foldpreprocessor = styler.GetPropertyInt("fold.preprocessor") != 0;
	// Backtrack to previous line in case need to fix its fold status
	int lineCurrent = styler.GetLine(startPos);
	if (startPos > 0) {
		if (lineCurrent > 0) {
			lineCurrent--;
			startPos = styler.LineStart(lineCurrent);
		}
	}
	// vars for style of previous/current/next lines 
	int style = GetStyleFirstWord(lineCurrent,styler);
	int stylePrev = 0;
	// find the first previous line without continuation character at the end
	while ((lineCurrent > 0 && IsContinuationLine(lineCurrent,styler)) ||
	       (lineCurrent > 1 && IsContinuationLine(lineCurrent-1,styler))) {
		lineCurrent--;
		startPos = styler.LineStart(lineCurrent);
	}
	if (lineCurrent > 0) {
		stylePrev = GetStyleFirstWord(lineCurrent-1,styler);
	}
	// vars for getting first word to check for keywords
	bool FirstWordStart = false;
	bool FirstWordEnd = false;
	char szKeyword[10]="";
	int	 szKeywordlen = 0;
	char szThen[5]="";
	int	 szThenlen = 0;
	bool ThenFoundLast = false;
	// var for indentlevel
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent-1) >> 16;
	int levelNext = levelCurrent;
	//  
	int	visibleChars = 0;
	char chNext = styler.SafeGetCharAt(startPos);
	char chPrev = ' ';
	//
	for (int i = startPos; i < endPos; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		if (IsAWordChar(ch)) {
			visibleChars++;
		}
		// get the syle for the current character neede to check in comment
		int stylech = styler.StyleAt(i);
		// get first word for the line for indent check max 9 characters
		if (FirstWordStart && (!(FirstWordEnd))) {
			if (!IsAWordChar(ch)) {
				FirstWordEnd = true;
				szKeyword[szKeywordlen] = '\0';
			}
			else {
				if (szKeywordlen < 10) {
				szKeyword[szKeywordlen++] = static_cast<char>(tolower(ch));
				}
			}
		}
		// start the capture of the first word 
		if (!(FirstWordStart)) {
			if (IsAWordChar(ch) || IsAWordStart(ch) || ch == ';') {
				FirstWordStart = true;
				szKeyword[szKeywordlen++] = static_cast<char>(tolower(ch));
			}
		}
		// only process this logic when not in comment section
		if (!(stylech == SCE_AU3_COMMENT)) {
			if (ThenFoundLast) {
				if (IsAWordChar(ch)) {
					ThenFoundLast = false;
				}		
			}
			// find out if the word "then" is the last on a "if" line
			if (FirstWordEnd && strcmp(szKeyword,"if") == 0) {
				if (szThenlen == 4) {
					szThen[0] = szThen[1];
					szThen[1] = szThen[2];
					szThen[2] = szThen[3];
					szThen[3] = static_cast<char>(tolower(ch));
					if (strcmp(szThen,"then") == 0 ) {
						ThenFoundLast = true;
					}
				}
				else {
					szThen[szThenlen++] = static_cast<char>(tolower(ch));
					if (szThenlen == 5) {
						szThen[4] = '\0';
					}
				}
			}
		}
		// End of Line found so process the information 
		if ((ch == '\r' && chNext != '\n') || (ch == '\n') || (i == endPos)) {
			// **************************
			// Folding logic for Keywords
			// **************************
			// if a keyword is found on the current line and the line doesn't end with _ (continuation)
			//    and we are not inside a commentblock.
			if (szKeywordlen > 0 && (!(chPrev == '_')) && 
				((!(IsStreamCommentStyle(style)) || foldInComment)) ) {
				szKeyword[szKeywordlen] = '\0';
				// only fold "if" last keyword is "then"  (else its a one line if)
				if (strcmp(szKeyword,"if") == 0  && ThenFoundLast) {
						levelNext++;
				}
				// create new fold for these words 
				if (strcmp(szKeyword,"do") == 0   || strcmp(szKeyword,"for") == 0 ||
					strcmp(szKeyword,"func") == 0 || strcmp(szKeyword,"while") == 0||
					strcmp(szKeyword,"#region") == 0 ) {
						levelNext++;
				}
				// create double Fold for select because Case will subtract one of the current level
				if (strcmp(szKeyword,"select") == 0) {
						levelNext++;
						levelNext++;
				}
				// end the fold for these words before the current line
				if (strcmp(szKeyword,"endfunc") == 0 || strcmp(szKeyword,"endif") == 0 ||
					strcmp(szKeyword,"next") == 0    || strcmp(szKeyword,"until") == 0 || 
					strcmp(szKeyword,"wend") == 0){
						levelNext--;
						levelCurrent--;
				}
				// end the fold for these words before the current line and Start new fold 
				if (strcmp(szKeyword,"case") == 0      || strcmp(szKeyword,"else") == 0 ||
					strcmp(szKeyword,"elseif") == 0 ) {
						levelCurrent--;
				}
				// end the double fold for this word before the current line
				if (strcmp(szKeyword,"endselect") == 0 ) {
						levelNext--;
						levelNext--;
						levelCurrent--;
						levelCurrent--;
				}
				// end the fold for these words on the current line
				if (strcmp(szKeyword,"#endregion") == 0 ) {
						levelNext--;
				}
			}
			// Preprocessor and Comment folding
			int styleNext = GetStyleFirstWord(lineCurrent + 1,styler);
			// *************************************
			// Folding logic for preprocessor blocks
			// *************************************
			// process preprosessor line
			if (foldpreprocessor && style == SCE_AU3_PREPROCESSOR) {
				if (!(stylePrev == SCE_AU3_PREPROCESSOR) && (styleNext == SCE_AU3_PREPROCESSOR)) {
				    levelNext++;
				}
				// fold till the last line for normal comment lines
				else if (stylePrev == SCE_AU3_PREPROCESSOR && !(styleNext == SCE_AU3_PREPROCESSOR)) {
					levelNext--;
				}
			}
			// *********************************
			// Folding logic for Comment blocks
			// *********************************
			if (foldComment && IsStreamCommentStyle(style)) {
				// Start of a comment block
				if (!(stylePrev==style) && IsStreamCommentStyle(styleNext) && styleNext==style) {
				    levelNext++;
				} 
				// fold till the last line for normal comment lines
				else if (IsStreamCommentStyle(stylePrev) 
						&& !(styleNext == SCE_AU3_COMMENT)
						&& stylePrev == SCE_AU3_COMMENT 
						&& style == SCE_AU3_COMMENT) {
					levelNext--;
				}
				// fold till the one but last line for Blockcomment lines
				else if (IsStreamCommentStyle(stylePrev) 
						&& !(styleNext == SCE_AU3_COMMENTBLOCK)
						&& style == SCE_AU3_COMMENTBLOCK) {
					levelNext--;
					levelCurrent--;
				}
			}
			int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (visibleChars == 0 && foldCompact)
				lev |= SC_FOLDLEVELWHITEFLAG;
			if (levelUse < levelNext) {
				lev |= SC_FOLDLEVELHEADERFLAG;
			}
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}
			// reset values for the next line
			lineCurrent++;
			stylePrev = style;
			style = styleNext;
			levelCurrent = levelNext;
			visibleChars = 0;
			// if the last character is an Underscore then don't reset since the line continues on the next line.
			if (!(chPrev == '_')) {
				szKeywordlen = 0;
				szThenlen = 0;
				FirstWordStart = false;
				FirstWordEnd = false;
				ThenFoundLast = false;
			}
		}
		// save the last processed character
		if (!isspacechar(ch)) {
			chPrev = ch;
			visibleChars++;
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
    "#autoit Special",
    0
};
LexerModule lmAU3(SCLEX_AU3, ColouriseAU3Doc, "au3", FoldAU3Doc , AU3WordLists);
