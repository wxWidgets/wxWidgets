// Scintilla source code edit control
/** @file LexClw.cxx
 ** Lexer for Clarion.
 **/
// Copyright 2003 by Ron Schofield <ron@schofieldcomputer.com>
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

static char MakeUpperCase(char ch) {
	if (ch < 'a' || ch > 'z')
		return ch;
	else
		return static_cast<char>(ch - 'a' + 'A');
}

static void MakeUpperCaseString(char *s) {
	while (*s) {
		*s = MakeUpperCase(*s);
		s++;
	}
}

// Is a label start character
inline bool IsALabelStart(const int iChar) {
	return(isalpha(iChar) || iChar == '_');
}

// Is a label character
inline bool IsALabelCharacter(const int iChar) {
	return(isalnum(iChar) || iChar == '_' || iChar == ':');
}

// Is the character is a ! and the the next character is not a !
inline bool IsACommentStart(StyleContext &scDoc) {
	return(scDoc.ch == '!' && scDoc.chNext != '!');
}

// Is the character a Clarion hex character (ABCDEF)
inline bool IsAHexCharacter(const int iChar, bool bCaseSensitive) {
	// Case insensitive.
	if (!bCaseSensitive) {
		if (strchr("ABCDEFabcdef", iChar) != NULL) {
			return(true);
		}
	}
	// Case sensitive
	else {
		if (strchr("ABCDEF", iChar) != NULL) {
			return(true);
		}
	}
	return(false);
}

// Is the character a Clarion base character (B=Binary, O=Octal, H=Hex)
inline bool IsANumericBaseCharacter(const int iChar, bool bCaseSensitive) {
	// Case insensitive.
	if (!bCaseSensitive) {
		// If character is a numeric base character
		if (strchr("BOHboh", iChar) != NULL) {
			return(true);
		}
	}
	// Case sensitive
	else {
		// If character is a numeric base character
		if (strchr("BOH", iChar) != NULL) {
			return(true);
		}
	}
	return(false);
}

// Set the correct numeric constant state
inline bool SetNumericConstantState(StyleContext &scDoc) {
	int iPoints = 0;			// Point counter
	char cNumericString[100];	// Numeric string buffer

	// Buffer the current numberic string
	scDoc.GetCurrent(cNumericString, sizeof(cNumericString));
	// Loop through the string until end of string (NULL termination)
	for (int iIndex = 0; cNumericString[iIndex] != '\0'; iIndex++) {
		// Depending on the character
		switch (cNumericString[iIndex]) {
			// Is a . (point)
			case '.' :
				// Increment point counter
				iPoints++;
				break;
			default :
				break;
		}
	}
	// If points found (can be more than one for improper formatted number
	if (iPoints > 0) {
		return(true);
	}
	// Else no points found
	else {
		return(false);
	}
}

// Clarion Language Colouring Procedure
static void ColouriseClwDoc(unsigned int uiStartPos, int iLength, int iInitStyle, WordList *wlKeywords[], Accessor &accStyler, bool bCaseSensitive) {

	int iParenthesesLevel=0;		// Parenthese Level

	WordList &wlClarionKeywords = *wlKeywords[0];		// Clarion Keywords
	WordList &wlCompilerDirectives = *wlKeywords[1];	// Compiler Directives
	WordList &wlBuiltInProcsFuncs = *wlKeywords[2];		// Builtin Procedures and Functions
	WordList &wlStructsDataTypes = *wlKeywords[3];		// Structures and Data Types
	WordList &wlAttributes = *wlKeywords[4];			// Procedure Attributes
	WordList &wlStandardEquates = *wlKeywords[5];		// Standard Equates
	WordList &wlReservedWords = *wlKeywords[6];			// Clarion Reserved Keywords

	StyleContext scDoc(uiStartPos, iLength, iInitStyle, accStyler);

	// lex source code
    for (; scDoc.More(); scDoc.Forward())
	{
		//
		// Determine if the current state should terminate.
		//

		// Label State Handling
		if (scDoc.state == SCE_CLW_LABEL) {
			// If the character is not a valid label
			if (!IsALabelCharacter(scDoc.ch)) {
				// If the character is a . (dot syntax)
				if (scDoc.ch == '.') {
					// Uncolour the . (dot) to default state, move forward one character,
					// and change back to the label state.
					scDoc.SetState(SCE_CLW_DEFAULT);
					scDoc.Forward();
					scDoc.SetState(SCE_CLW_LABEL);
				}
				// Else terminate the label state
				else {
					char cLabel[100];		// Label buffer
					// Buffer the current label string
					scDoc.GetCurrent(cLabel,sizeof(cLabel));
					// If case insensitive, convert string to UPPERCASE to match passed keywords.
					if (!bCaseSensitive) {
						MakeUpperCaseString(cLabel);
					}
					// If label string is in the Clarion reserved keyword list
					if (wlReservedWords.InList(cLabel)){
						// change to error state
						scDoc.ChangeState(SCE_CLW_ERROR);
					}
					// Else if label string is in the compiler directive keyword list
					else if (wlCompilerDirectives.InList(cLabel)) {
						// change the state to compiler directive state
						scDoc.ChangeState(SCE_CLW_COMPILER_DIRECTIVE);
					}
					// Terminate the label state and set to default state
					scDoc.SetState(SCE_CLW_DEFAULT);
				}
			}
		}
		// Keyword State Handling
		else if (scDoc.state == SCE_CLW_KEYWORD) {
			// If character is : (colon)
			if (scDoc.ch == ':') {
				char cEquate[100];		// Equate buffer
				// Move forward to include : (colon) in buffer
				scDoc.Forward();
				// Buffer the equate string
				scDoc.GetCurrent(cEquate,sizeof(cEquate));
				// If case insensitive, convert string to UPPERCASE to match passed keywords.
				if (!bCaseSensitive) {
					MakeUpperCaseString(cEquate);
				}
				// If statement string is in the equate list
				if (wlStandardEquates.InList(cEquate)) {
					// Change to equate state
					scDoc.ChangeState(SCE_CLW_STANDARD_EQUATE);
				}
			}
			// If the character is not a valid label character
			else if (!IsALabelCharacter(scDoc.ch)) {
				char cStatement[100];		// Statement buffer
				// Buffer the statement string
				scDoc.GetCurrent(cStatement,sizeof(cStatement));
				// If case insensitive, convert string to UPPERCASE to match passed keywords.
				if (!bCaseSensitive) {
					MakeUpperCaseString(cStatement);
				}
				// If statement string is in the Clarion keyword list
				if (wlClarionKeywords.InList(cStatement)) {
					// Set to the Clarion keyword state
					scDoc.ChangeState(SCE_CLW_KEYWORD);
				}
				// Else if statement string is in the compiler directive keyword list
				else if (wlCompilerDirectives.InList(cStatement)) {
					// Set to the compiler directive state
					scDoc.ChangeState(SCE_CLW_COMPILER_DIRECTIVE);
				}
				// Else if statement string is in the builtin procedures and functions keyword list
				else if (wlBuiltInProcsFuncs.InList(cStatement)) {
					// Set to the builtin procedures and functions state
					scDoc.ChangeState(SCE_CLW_BUILTIN_PROCEDURES_FUNCTION);
				}
				// Else if statement string is in the tructures and data types keyword list
				else if (wlStructsDataTypes.InList(cStatement)) {
					// Set to the structures and data types state
					scDoc.ChangeState(SCE_CLW_STRUCTURE_DATA_TYPE);
				}
				// Else if statement string is in the procedure attribute keyword list
				else if (wlAttributes.InList(cStatement)) {
					// Set to the procedure attribute state
					scDoc.ChangeState(SCE_CLW_ATTRIBUTE);
				}
				// Else if statement string is in the standard equate keyword list
				else if (wlStandardEquates.InList(cStatement)) {
					// Set to the standard equate state
					scDoc.ChangeState(SCE_CLW_STANDARD_EQUATE);
				}
				// Terminate the keyword state and set to default state
				scDoc.SetState(SCE_CLW_DEFAULT);
			}
		}
		// String State Handling
		else if (scDoc.state == SCE_CLW_STRING) {
			// If the character is an ' (single quote)
			if (scDoc.ch == '\'') {
				// Set the state to default and move forward colouring
				// the ' (single quote) as default state
				// terminating the string state
				scDoc.SetState(SCE_CLW_DEFAULT);
				scDoc.Forward();
			}
			// If the next character is an ' (single quote)
			if (scDoc.chNext == '\'') {
				// Move forward one character and set to default state
				// colouring the next ' (single quote) as default state
				// terminating the string state
				scDoc.ForwardSetState(SCE_CLW_DEFAULT);
				scDoc.Forward();
			}
		}
		// Picture String State Handling
		else if (scDoc.state == SCE_CLW_PICTURE_STRING) {
			// If the character is an ( (open parenthese)
			if (scDoc.ch == '(') {
				// Increment the parenthese level
				iParenthesesLevel++;
			}
			// Else if the character is a ) (close parenthese)
			else if (scDoc.ch == ')') {
				// If the parenthese level is set to zero
				// parentheses matched
				if (!iParenthesesLevel) {
					scDoc.SetState(SCE_CLW_DEFAULT);
				}
				// Else parenthese level is greater than zero
				// still looking for matching parentheses
				else {
					// Decrement the parenthese level
					iParenthesesLevel--;
				}
			}
		}
		// Standard Equate State Handling
		else if (scDoc.state == SCE_CLW_STANDARD_EQUATE) {
			if (!isalnum(scDoc.ch)) {
				scDoc.SetState(SCE_CLW_DEFAULT);
			}
		}
		// Integer Constant State Handling
		else if (scDoc.state == SCE_CLW_INTEGER_CONSTANT) {
			// If the character is not a digit (0-9)
			// or character is not a hexidecimal character (A-F)
			// or character is not a . (point)
			// or character is not a numberic base character (B,O,H)
			if (!(isdigit(scDoc.ch)
			|| IsAHexCharacter(scDoc.ch, bCaseSensitive)
			|| scDoc.ch == '.'
			|| IsANumericBaseCharacter(scDoc.ch, bCaseSensitive))) {
				// If the number was a real
				if (SetNumericConstantState(scDoc)) {
					// Colour the matched string to the real constant state
					scDoc.ChangeState(SCE_CLW_REAL_CONSTANT);
				}
				// Else the number was an integer
				else {
					// Colour the matched string to an integer constant state
					scDoc.ChangeState(SCE_CLW_INTEGER_CONSTANT);
				}
				// Terminate the integer constant state and set to default state
				scDoc.SetState(SCE_CLW_DEFAULT);
			}
		}

		//
		// Determine if a new state should be entered.
		//

		// Beginning of Line Handling
		if (scDoc.atLineStart) {
			// If column 1 character is a label start character
			if (IsALabelStart(scDoc.ch)) {
				// Set the state to label
				scDoc.SetState(SCE_CLW_LABEL);
			}
			// else if character is a space or tab
			else if (IsASpace(scDoc.ch)){
				// Set to default state
				scDoc.SetState(SCE_CLW_DEFAULT);
			}
			// else if the start of a comment or is an * (asterisk)
			else if (IsACommentStart(scDoc) || scDoc.ch == '*' ) {
				// then set the state to comment.
				scDoc.SetState(SCE_CLW_COMMENT);
			}
			// else the character is a ? (question mark)
			else if (scDoc.ch == '?') {
				// Change to the compiler directive state, move forward,
				// colouring the ? (question mark), change back to default state.
				scDoc.ChangeState(SCE_CLW_COMPILER_DIRECTIVE);
				scDoc.Forward();
				scDoc.SetState(SCE_CLW_DEFAULT);
			}
			// else an invalid character in column 1
			else {
				// Set to error state
				scDoc.SetState(SCE_CLW_ERROR);
			}
		}
		// End of Line Handling
		else if (scDoc.atLineEnd) {
			// Reset to the default state at the end of each line.
			scDoc.SetState(SCE_CLW_DEFAULT);
		}
		// Default Handling
		else {
			// If in default state
			if (scDoc.state == SCE_CLW_DEFAULT) {
				// If is a letter could be a possible statement
				if (isalpha(scDoc.ch)) {
					// Set the state to Clarion Keyword and verify later
					scDoc.SetState(SCE_CLW_KEYWORD);
				}
				// else is a number
				else if (isdigit(scDoc.ch)) {
					// Set the state to Integer Constant and verify later
					scDoc.SetState(SCE_CLW_INTEGER_CONSTANT);
				}
				// else if the start of a comment or a | (line continuation)
				else if (IsACommentStart(scDoc) || scDoc.ch == '|') {
					// then set the state to comment.
					scDoc.SetState(SCE_CLW_COMMENT);
				}
				// else if the character is a ' (single quote)
				else if (scDoc.ch == '\'') {
					// If the character is also a ' (single quote)
					// Embedded Apostrophe
					if (scDoc.chNext == '\'') {
						// Move forward colouring it as default state
						scDoc.ForwardSetState(SCE_CLW_DEFAULT);
					}
					else {
						// move to the next character and then set the state to comment.
						scDoc.ForwardSetState(SCE_CLW_STRING);
					}
				}
				// else the character is an @ (apersand)
				else if (scDoc.ch == '@') {
					// Case insensitive.
					if (!bCaseSensitive) {
						// If character is a valid picture token character
						if (strchr("DEKNPSTdeknpst", scDoc.chNext) != NULL) {
							// Set to the picture string state
							scDoc.SetState(SCE_CLW_PICTURE_STRING);
						}
					}
					// Case sensitive
					else {
						// If character is a valid picture token character
						if (strchr("DEKNPST", scDoc.chNext) != NULL) {
							// Set the picture string state
							scDoc.SetState(SCE_CLW_PICTURE_STRING);
						}
					}
				}
			}
		}
	}
	// lexing complete
	scDoc.Complete();
}

// Clarion Language Case Sensitive Colouring Procedure
static void ColouriseClwDocSensitive(unsigned int uiStartPos, int iLength, int iInitStyle, WordList *wlKeywords[], Accessor &accStyler) {
	ColouriseClwDoc(uiStartPos, iLength, iInitStyle, wlKeywords, accStyler, true);
}

// Clarion Language Case Insensitive Colouring Procedure
static void ColouriseClwDocInsensitive(unsigned int uiStartPos, int iLength, int iInitStyle, WordList *wlKeywords[], Accessor &accStyler) {
	ColouriseClwDoc(uiStartPos, iLength, iInitStyle, wlKeywords, accStyler, false);
}

// Clarion Language Folding Procedure
#ifdef FOLDING_IMPLEMENTED
static void FoldClwDoc(unsigned int uiStartPos, int iLength, int iInitStyle, WordList *wlKeywords[], Accessor &accStyler) {

}
#endif

// Word List Descriptions
static const char * const rgWordListDescriptions[] = {
	"Clarion Keywords",
	"Compiler Directives",
	"Built-in Procedures and Functions",
	"Structure and Data Types",
	"Attributes",
	"Standard Equates",
	"Reserved Words",
	0,
};

// Case Sensitive Clarion Language Lexer
LexerModule lmClw(SCLEX_CLW, ColouriseClwDocSensitive, "clw", NULL, rgWordListDescriptions);

// Case Insensitive Clarion Language Lexer
LexerModule lmClwNoCase(SCLEX_CLWNOCASE, ColouriseClwDocInsensitive, "clwnocase", NULL, rgWordListDescriptions);
