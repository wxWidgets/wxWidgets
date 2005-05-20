// Scintilla source code edit control
/** @file LexScriptol.cxx
 ** Lexer for Scriptol.
 **/

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

static void ClassifyWordSol(unsigned int start, unsigned int end, WordList &keywords, Accessor &styler, char *prevWord)
{
	char s[100];
	bool wordIsNumber = isdigit(styler[start]) != 0;
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++)
    {
		s[i] = styler[start + i];
		s[i + 1] = '\0';
	}
	char chAttr = SCE_P_IDENTIFIER;
    if (0 == strcmp(prevWord, "class"))       chAttr = SCE_P_CLASSNAME;
    else if (wordIsNumber)                    chAttr = SCE_P_NUMBER;
    else if (keywords.InList(s))              chAttr = SCE_P_WORD;
    else for (unsigned int i = 0; i < end - start + 1; i++)  // test dotted idents
    {
		if (styler[start + i] == '.')
        {
			styler.ColourTo(start + i - 1, chAttr);
			styler.ColourTo(start + i, SCE_P_OPERATOR);
		}
	}
	styler.ColourTo(end, chAttr);
	strcpy(prevWord, s);
}

static bool IsSolComment(Accessor &styler, int pos, int len)
{
//    return len > 0 && styler[pos]=='`';
   char c;
   if(len > 0)
   {
     c = styler[pos];
     if(c == '`') return true;
     if(len > 1)
     {
        if(c == '/')
        {
          c = styler[pos + 1];
          if(c == '/') return true;
          if(c == '*') return true;
        }
     }
   }
   return false;
}

static bool IsSolStringStart(char ch, char /*chNext*/)
{
    if (ch == '\'' || ch == '"')  return true;
    //chNext = chNext;  // for future use

	return false;
}

static bool IsSolWordStart(char ch, char chNext)
{
    return (iswordchar(ch) && !IsSolStringStart(ch, chNext));
}

/* Return the state to use for the string starting at i; *nextIndex will be set to the first index following the quote(s) */
static int GetSolStringState(Accessor &styler, int i, int *nextIndex)
{
	char ch = styler.SafeGetCharAt(i);
	char chNext = styler.SafeGetCharAt(i + 1);

	if (ch != '"' && ch != '\'') {
		*nextIndex = i + 1;
		return SCE_P_DEFAULT;
	}

	if (ch == chNext && ch == styler.SafeGetCharAt(i + 2))
    {
       *nextIndex = i + 3;
       if (ch == '"') return SCE_P_TRIPLEDOUBLE;
       else           return SCE_P_TRIPLE;
	}
    else
    {
       *nextIndex = i + 1;
       if (ch == '"') return SCE_P_STRING;
       else           return SCE_P_CHARACTER;
	}
}

static void ColouriseSolDoc(unsigned int startPos, int length, int initStyle,
						   WordList *keywordlists[], Accessor &styler)
 {

	int lengthDoc = startPos + length;

	// Backtrack to previous line in case need to fix its tab whinging
	if (startPos > 0)
    {
		int lineCurrent = styler.GetLine(startPos);
		if (lineCurrent > 0)
        {
           startPos = styler.LineStart(lineCurrent-1);
           if (startPos == 0) initStyle = SCE_P_DEFAULT;
           else               initStyle = styler.StyleAt(startPos-1);
		}
	}

	styler.StartAt(startPos, 127);

	WordList &keywords = *keywordlists[0];

	int whingeLevel = styler.GetPropertyInt("tab.timmy.whinge.level");
	char prevWord[200];
	prevWord[0] = '\0';
    if (length == 0)  return ;

	int state = initStyle & 31;

	int nextIndex = 0;
	char chPrev = ' ';
	//char chPrev2 = ' ';
	char chNext = styler[startPos];
	styler.StartSegment(startPos);
	bool atStartLine = true;
	int spaceFlags = 0;
	for (int i = startPos; i < lengthDoc; i++)
    {

      if (atStartLine)
      {
         char chBad = static_cast<char>(64);
         char chGood = static_cast<char>(0);
         char chFlags = chGood;

         if (whingeLevel == 1)
         {
				chFlags = (spaceFlags & wsInconsistent) ? chBad : chGood;
         }
         else if (whingeLevel == 2)
         {
				chFlags = (spaceFlags & wsSpaceTab) ? chBad : chGood;
         }
         else if (whingeLevel == 3)
         {
				chFlags = (spaceFlags & wsSpace) ? chBad : chGood;
         }
         else if (whingeLevel == 4)
         {
				chFlags = (spaceFlags & wsTab) ? chBad : chGood;
         }
         styler.SetFlags(chFlags, static_cast<char>(state));
         atStartLine = false;
       }

		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if ((ch == '\r' && chNext != '\n') || (ch == '\n') || (i == lengthDoc))
        {
			if ((state == SCE_P_DEFAULT) || (state == SCE_P_TRIPLE) || (state == SCE_P_TRIPLEDOUBLE))
            {
				styler.ColourTo(i, state);
			}
			atStartLine = true;
         }

		if (styler.IsLeadByte(ch))
        {
			chNext = styler.SafeGetCharAt(i + 2);
			chPrev = ' ';
			//chPrev2 = ' ';
			i += 1;
			continue;
		}

		if (state == SCE_P_STRINGEOL)
        {
			if (ch != '\r' && ch != '\n')
             {
				styler.ColourTo(i - 1, state);
				state = SCE_P_DEFAULT;
             }
        }

		if (state == SCE_P_DEFAULT)
        {
            if (IsSolWordStart(ch, chNext))
            {
				styler.ColourTo(i - 1, state);
				state = SCE_P_WORD;
            }
            else if (ch == '`')
            {
				styler.ColourTo(i - 1, state);
                state = SCE_P_COMMENTLINE;
            }
            else if (ch == '/')
            {
				styler.ColourTo(i - 1, state);
                if(chNext == '/') state = SCE_P_COMMENTLINE;
                if(chNext == '*') state = SCE_P_COMMENTBLOCK;
            }

            else if (ch == '=' && chNext == 'b')
            {
				// =begin indicates the start of a comment (doc) block
               if(styler.SafeGetCharAt(i + 2) == 'e' && styler.SafeGetCharAt(i + 3) == 'g' && styler.SafeGetCharAt(i + 4) == 'i' && styler.SafeGetCharAt(i + 5) == 'n')
               {
					styler.ColourTo(i - 1, state);
					state = SCE_P_TRIPLEDOUBLE; //SCE_C_COMMENT;
               }
            }
            else if (IsSolStringStart(ch, chNext))
            {
               styler.ColourTo(i - 1, state);
               state = GetSolStringState(styler, i, &nextIndex);
               if (nextIndex != i + 1)
               {
					i = nextIndex - 1;
					ch = ' ';
					//chPrev = ' ';
					chNext = styler.SafeGetCharAt(i + 1);
               }
			}
            else if (isoperator(ch))
            {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_P_OPERATOR);
			}
          }
          else if (state == SCE_P_WORD)
          {
              if (!iswordchar(ch))
              {
                 ClassifyWordSol(styler.GetStartSegment(), i - 1, keywords, styler, prevWord);
                 state = SCE_P_DEFAULT;
                 if (ch == '`')
                 {
                     state = chNext == '`' ? SCE_P_COMMENTBLOCK : SCE_P_COMMENTLINE;
                 }
                 else if (IsSolStringStart(ch, chNext))
                 {
                    styler.ColourTo(i - 1, state);
                    state = GetSolStringState(styler, i, &nextIndex);
					if (nextIndex != i + 1)
                    {
						i = nextIndex - 1;
						ch = ' ';
						//chPrev = ' ';
						chNext = styler.SafeGetCharAt(i + 1);
					}
                 }
                 else if (isoperator(ch))
                 {
					styler.ColourTo(i, SCE_P_OPERATOR);
                 }
             }
          }
          else
          {
            if (state == SCE_P_COMMENTLINE)
            {
				if (ch == '\r' || ch == '\n')
                 {
					styler.ColourTo(i - 1, state);
					state = SCE_P_DEFAULT;
                 }
            }
            else if(state == SCE_P_COMMENTBLOCK)
            {
              if(ch == '*' && chNext == '/') state = SCE_P_DEFAULT;
            }
            else if (state == SCE_P_STRING)
            {
				if ((ch == '\r' || ch == '\n') && (chPrev != '\\'))
                {
					styler.ColourTo(i - 1, state);
					state = SCE_P_STRINGEOL;
				}
                else if (ch == '\\')
                {
                   if (chNext == '\"' || chNext == '\'' || chNext == '\\')
                   {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
                   }
				}
                else if (ch == '\"')
                {
					styler.ColourTo(i, state);
					state = SCE_P_DEFAULT;
				}
			}
            else if (state == SCE_P_CHARACTER)
             {
				if ((ch == '\r' || ch == '\n') && (chPrev != '\\'))
                 {
					styler.ColourTo(i - 1, state);
					state = SCE_P_STRINGEOL;
                 }
                 else if (ch == '\\')
                 {
					if (chNext == '\"' || chNext == '\'' || chNext == '\\')
                    {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				}
                else if (ch == '\'')
                {
					styler.ColourTo(i, state);
					state = SCE_P_DEFAULT;
				}
            }
            /*
            else if (state == SCE_P_TRIPLE)
             {
				if (ch == '\'' && chPrev == '\'' && chPrev2 == '\'')
                 {
                    styler.ColourTo(i, state);
					state = SCE_P_DEFAULT;
                 }
              }
            else if (state == SCE_P_TRIPLEDOUBLE)
              {
				// =end terminates the comment block
				if (ch == 'd' && chPrev == 'n' && chPrev2 == 'e')
                {
					if  (styler.SafeGetCharAt(i - 3) == '=')
                     {
						styler.ColourTo(i, state);
						state = SCE_P_DEFAULT;
                     }
                 }
			}
            */
		}
		//chPrev2 = chPrev;
		chPrev = ch;
	}
	if (state == SCE_P_WORD)
    {
       ClassifyWordSol(styler.GetStartSegment(), lengthDoc-1, keywords, styler, prevWord);
	}
    else
    {
		styler.ColourTo(lengthDoc-1, state);
	}
}

static void FoldSolDoc(unsigned int startPos, int length, int initStyle,
						   WordList *[], Accessor &styler)
 {
	int lengthDoc = startPos + length;

	// Backtrack to previous line in case need to fix its fold status
	int lineCurrent = styler.GetLine(startPos);
	if (startPos > 0) {
		if (lineCurrent > 0)
        {
			lineCurrent--;
			startPos = styler.LineStart(lineCurrent);
			if (startPos == 0)
				initStyle = SCE_P_DEFAULT;
			else
				initStyle = styler.StyleAt(startPos-1);
		}
	}
	int state = initStyle & 31;
	int spaceFlags = 0;
        int indentCurrent = styler.IndentAmount(lineCurrent, &spaceFlags, IsSolComment);
	if ((state == SCE_P_TRIPLE) || (state == SCE_P_TRIPLEDOUBLE))
		indentCurrent |= SC_FOLDLEVELWHITEFLAG;
	char chNext = styler[startPos];
	for (int i = startPos; i < lengthDoc; i++)
    {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		int style = styler.StyleAt(i) & 31;

		if ((ch == '\r' && chNext != '\n') || (ch == '\n') || (i == lengthDoc))
        {
			int lev = indentCurrent;
            int indentNext = styler.IndentAmount(lineCurrent + 1, &spaceFlags, IsSolComment);
			if ((style == SCE_P_TRIPLE) || (style== SCE_P_TRIPLEDOUBLE))
				indentNext |= SC_FOLDLEVELWHITEFLAG;
			if (!(indentCurrent & SC_FOLDLEVELWHITEFLAG))
            {
				// Only non whitespace lines can be headers
				if ((indentCurrent & SC_FOLDLEVELNUMBERMASK) < (indentNext & SC_FOLDLEVELNUMBERMASK))
                {
					lev |= SC_FOLDLEVELHEADERFLAG;
				} else if (indentNext & SC_FOLDLEVELWHITEFLAG)
                {
					// Line after is blank so check the next - maybe should continue further?
					int spaceFlags2 = 0;
                    int indentNext2 = styler.IndentAmount(lineCurrent + 2, &spaceFlags2, IsSolComment);
					if ((indentCurrent & SC_FOLDLEVELNUMBERMASK) < (indentNext2 & SC_FOLDLEVELNUMBERMASK))
                    {
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

LexerModule lmScriptol(SCLEX_SCRIPTOL, ColouriseSolDoc, "scriptol", FoldSolDoc);
