// Scintilla source code edit control
/** @file LexHTML.cxx
 ** Lexer for HTML.
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

#define SCE_HA_JS (SCE_HJA_START - SCE_HJ_START)
#define SCE_HA_VBS (SCE_HBA_START - SCE_HB_START)
#define SCE_HA_PYTHON (SCE_HPA_START - SCE_HP_START)

enum { eScriptNone = 0, eScriptJS, eScriptVBS, eScriptPython, eScriptPHP, eScriptXML };
enum { eHtml = 0, eNonHtmlScript, eNonHtmlPreProc, eNonHtmlScriptPreProc };

static int segIsScriptingIndicator(Accessor &styler, unsigned int start, unsigned int end, int prevValue) {
	char s[30 + 1];
	s[0] = '\0';
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = static_cast<char>(tolower(styler[start + i]));
		s[i + 1] = '\0';
	}
	//Platform::DebugPrintf("Scripting indicator [%s]\n", s);
	if (strstr(s, "src"))	// External script
		return eScriptNone;
	if (strstr(s, "vbs"))
		return eScriptVBS;
	if (strstr(s, "pyth"))
		return eScriptPython;
	if (strstr(s, "javas"))
		return eScriptJS;
	if (strstr(s, "jscr"))
		return eScriptJS;
	if (strstr(s, "php"))
		return eScriptPHP;
	if (strstr(s, "xml"))
		return eScriptXML;

	return prevValue;
}

static int PrintScriptingIndicatorOffset(Accessor &styler, unsigned int start, unsigned int end) {
	int iResult = 0;
	char s[30 + 1];
	s[0] = '\0';
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = static_cast<char>(tolower(styler[start + i]));
		s[i + 1] = '\0';
	}
	if (0 == strncmp(s, "php", 3)) {
		iResult = 3;
	}

	return iResult;
}

static int ScriptOfState(int state) {
	int scriptLanguage;

	if ((state >= SCE_HP_START) && (state <= SCE_HP_IDENTIFIER)) {
		scriptLanguage = eScriptPython;
	} else if ((state >= SCE_HB_START) && (state <= SCE_HB_STRINGEOL)) {
		scriptLanguage = eScriptVBS;
	} else if ((state >= SCE_HJ_START) && (state <= SCE_HJ_REGEX)) {
		scriptLanguage = eScriptJS;
	} else if ((state >= SCE_HPHP_DEFAULT) && (state <= SCE_HPHP_COMMENTLINE)) {
		scriptLanguage = eScriptPHP;
	} else {
		//		scriptLanguage = defaultScript;
		scriptLanguage = eScriptNone;
	}

	return scriptLanguage;
}

static int statePrintForState(int state, int inScriptType) {
	int StateToPrint;

	if ((state >= SCE_HP_START) && (state <= SCE_HP_IDENTIFIER)) {
		StateToPrint = state + ((inScriptType == eNonHtmlScript) ? 0 : SCE_HA_PYTHON);
	} else if ((state >= SCE_HB_START) && (state <= SCE_HB_STRINGEOL)) {
		StateToPrint = state + ((inScriptType == eNonHtmlScript) ? 0 : SCE_HA_VBS);
	} else if ((state >= SCE_HJ_START) && (state <= SCE_HJ_REGEX)) {
		StateToPrint = state + ((inScriptType == eNonHtmlScript) ? 0 : SCE_HA_JS);
	} else {
		StateToPrint = state;
	}

	return StateToPrint;
}

static int stateForPrintState(int StateToPrint) {
	int state;

	if ((StateToPrint >= SCE_HPA_START) && (StateToPrint <= SCE_HPA_IDENTIFIER)) {
		state = StateToPrint - SCE_HA_PYTHON;
	} else if ((StateToPrint >= SCE_HBA_START) && (StateToPrint <= SCE_HBA_STRINGEOL)) {
		state = StateToPrint - SCE_HA_VBS;
	} else if ((StateToPrint >= SCE_HJA_START) && (StateToPrint <= SCE_HJA_REGEX)) {
		state = StateToPrint - SCE_HA_JS;
	} else {
		state = StateToPrint;
	}

	return state;
}

static inline bool IsNumber(unsigned int start, Accessor &styler) {
	return isdigit(styler[start]) || (styler[start] == '.') ||
	       (styler[start] == '-') || (styler[start] == '#');
}

static inline bool isStringState(int state) {
	bool bResult;

	switch (state) {
	case SCE_HJ_DOUBLESTRING:
	case SCE_HJ_SINGLESTRING:
	case SCE_HJA_DOUBLESTRING:
	case SCE_HJA_SINGLESTRING:
	case SCE_HB_STRING:
	case SCE_HBA_STRING:
	case SCE_HP_STRING:
	case SCE_HPA_STRING:
	case SCE_HPHP_HSTRING:
	case SCE_HPHP_SIMPLESTRING:
		bResult = true;
		break;
	default :
		bResult = false;
		break;
	}
	return bResult;
}

// not really well done, since it's only comments that should lex the %> and <%
static inline bool isCommentASPState(int state) {
	bool bResult;

	switch (state) {
	case SCE_HJ_COMMENT:
	case SCE_HJ_COMMENTLINE:
	case SCE_HJ_COMMENTDOC:
	case SCE_HB_COMMENTLINE:
	case SCE_HP_COMMENTLINE:
	case SCE_HPHP_COMMENT:
	case SCE_HPHP_COMMENTLINE:
		bResult = true;
		break;
	default :
		bResult = false;
		break;
	}
	return bResult;
}

static void classifyAttribHTML(unsigned int start, unsigned int end, WordList &keywords, Accessor &styler) {
	bool wordIsNumber = IsNumber(start, styler);
	char chAttr = SCE_H_ATTRIBUTEUNKNOWN;
	if (wordIsNumber) {
		chAttr = SCE_H_NUMBER;
	} else {
		char s[30 + 1];
		s[0] = '\0';
		for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
			s[i] = static_cast<char>(tolower(styler[start + i]));
			s[i + 1] = '\0';
		}
		if (keywords.InList(s))
			chAttr = SCE_H_ATTRIBUTE;
	}
	if ((chAttr == SCE_H_ATTRIBUTEUNKNOWN) && !keywords)
		// No keywords -> all are known
		chAttr = SCE_H_ATTRIBUTE;
	styler.ColourTo(end, chAttr);
}

static int classifyTagHTML(unsigned int start, unsigned int end,
                           WordList &keywords, Accessor &styler) {
	char s[30 + 1];
	// Copy after the '<'
	unsigned int i = 0;
	for (unsigned int cPos = start; cPos <= end && i < 30; cPos++) {
		char ch = styler[cPos];
		if (ch != '<')
			s[i++] = static_cast<char>(tolower(ch));
	}
	s[i] = '\0';
	bool isScript = false;
	char chAttr = SCE_H_TAGUNKNOWN;
	if (s[0] == '!' && s[1] == '-' && s[2] == '-') {	//Comment
		chAttr = SCE_H_COMMENT;
	} else if (strcmp(s, "![cdata[") == 0) {	// In lower case because already converted
		chAttr = SCE_H_CDATA;
	} else if (s[0] == '!') {
		chAttr = SCE_H_SGML;
	} else if (s[0] == '/') {	// Closing tag
		if (keywords.InList(s + 1))
			chAttr = SCE_H_TAG;
	} else {
		if (keywords.InList(s)) {
			chAttr = SCE_H_TAG;
			isScript = 0 == strcmp(s, "script");
		}
	}
	if ((chAttr == SCE_H_TAGUNKNOWN) && !keywords)
		// No keywords -> all are known
		chAttr = SCE_H_TAG;
	styler.ColourTo(end, chAttr);
	return isScript ? SCE_H_SCRIPT : chAttr;
}

static void classifyWordHTJS(unsigned int start, unsigned int end,
                             WordList &keywords, Accessor &styler, int inScriptType) {
	char chAttr = SCE_HJ_WORD;
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.');
	if (wordIsNumber)
		chAttr = SCE_HJ_NUMBER;
	else {
		char s[30 + 1];
		for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
			s[i] = styler[start + i];
			s[i + 1] = '\0';
		}
		if (keywords.InList(s))
			chAttr = SCE_HJ_KEYWORD;
	}
	styler.ColourTo(end, statePrintForState(chAttr, inScriptType));
}

static int classifyWordHTVB(unsigned int start, unsigned int end, WordList &keywords, Accessor &styler, int inScriptType) {
	char chAttr = SCE_HB_IDENTIFIER;
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.');
	if (wordIsNumber)
		chAttr = SCE_HB_NUMBER;
	else {
		char s[30 + 1];
		for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
			s[i] = static_cast<char>(tolower(styler[start + i]));
			s[i + 1] = '\0';
		}
		if (keywords.InList(s)) {
			chAttr = SCE_HB_WORD;
			if (strcmp(s, "rem") == 0)
				chAttr = SCE_HB_COMMENTLINE;
		}
	}
	styler.ColourTo(end, statePrintForState(chAttr, inScriptType));
	if (chAttr == SCE_HB_COMMENTLINE)
		return SCE_HB_COMMENTLINE;
	else
		return SCE_HB_DEFAULT;
}

static void classifyWordHTPy(unsigned int start, unsigned int end, WordList &keywords, Accessor &styler, char *prevWord, int inScriptType) {
	bool wordIsNumber = isdigit(styler[start]);
	char s[30 + 1];
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = styler[start + i];
		s[i + 1] = '\0';
	}
	char chAttr = SCE_HP_IDENTIFIER;
	if (0 == strcmp(prevWord, "class"))
		chAttr = SCE_HP_CLASSNAME;
	else if (0 == strcmp(prevWord, "def"))
		chAttr = SCE_HP_DEFNAME;
	else if (wordIsNumber)
		chAttr = SCE_HP_NUMBER;
	else if (keywords.InList(s))
		chAttr = SCE_HP_WORD;
	styler.ColourTo(end, statePrintForState(chAttr, inScriptType));
	strcpy(prevWord, s);
}

// Update the word colour to default or keyword
// Called when in a PHP word
static void classifyWordHTPHP(unsigned int start, unsigned int end, WordList &keywords, Accessor &styler) {
	char chAttr = SCE_HPHP_DEFAULT;
	bool wordIsNumber = isdigit(styler[start]);
	if (wordIsNumber)
		chAttr = SCE_HPHP_NUMBER;
	else {
		char s[30 + 1];
		for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
			s[i] = styler[start + i];
			s[i + 1] = '\0';
		}
		if (keywords.InList(s))
			chAttr = SCE_HPHP_WORD;
	}
	styler.ColourTo(end, chAttr);
}

// Return the first state to reach when entering a scripting language
static int StateForScript(int scriptLanguage) {
	int Result;
	switch (scriptLanguage) {
	case eScriptVBS:
		Result = SCE_HB_START;
		break;
	case eScriptPython:
		Result = SCE_HP_START;
		break;
	case eScriptPHP:
		Result = SCE_HPHP_DEFAULT;
		break;
	case eScriptXML:
		Result = SCE_H_TAGUNKNOWN;
		break;
	default :
		Result = SCE_HJ_START;
		break;
	}
	return Result;
}

inline bool ishtmlwordchar(char ch) {
	return isalnum(ch) || ch == '.' || ch == '-' || ch == '_' || ch == ':' || ch == '!' || ch == '#';
}

static bool InTagState(int state) {
	return state == SCE_H_TAG || state == SCE_H_TAGUNKNOWN ||
	       state == SCE_H_SCRIPT ||
	       state == SCE_H_ATTRIBUTE || state == SCE_H_ATTRIBUTEUNKNOWN ||
	       state == SCE_H_NUMBER || state == SCE_H_OTHER ||
	       state == SCE_H_DOUBLESTRING || state == SCE_H_SINGLESTRING;
}

static bool isLineEnd(char ch) {
	return ch == '\r' || ch == '\n';
}

static bool isOKBeforeRE(char ch) {
	return (ch == '(') || (ch == '=') || (ch == ',');
}

static bool isPHPStringState(int state) {
	return 
		(state == SCE_HPHP_HSTRING) ||
		(state == SCE_HPHP_SIMPLESTRING) ||
		(state == SCE_HPHP_HSTRING_VARIABLE);
}

static void ColouriseHyperTextDoc(unsigned int startPos, int length, int initStyle, WordList *keywordlists[],
                                  Accessor &styler) {

	WordList &keywords = *keywordlists[0];
	WordList &keywords2 = *keywordlists[1];
	WordList &keywords3 = *keywordlists[2];
	WordList &keywords4 = *keywordlists[3];
	WordList &keywords5 = *keywordlists[4];

	// Lexer for HTML requires more lexical states (7 bits worth) than most lexers
	styler.StartAt(startPos, 127);
	char prevWord[200];
	prevWord[0] = '\0';
	int StateToPrint = initStyle;
	int state = stateForPrintState(StateToPrint);

	// If inside a tag, it may be a script tag, so reread from the start to ensure any language tags are seen
	if (InTagState(state)) {
		while ((startPos > 0) && (InTagState(styler.StyleAt(startPos - 1)))) {
			startPos--;
            length++;
		}
		state = SCE_H_DEFAULT;
	}
	styler.StartAt(startPos, 127);

	int lineState = eScriptVBS;
	int lineCurrent = styler.GetLine(startPos);
	if (lineCurrent > 0)
		lineState = styler.GetLineState(lineCurrent);
	int inScriptType  = (lineState >> 0) & 0x03; // 2 bits of scripting type
	bool tagOpened    = (lineState >> 2) & 0x01; // 1 bit to know if we are in an opened tag
	bool tagClosing   = (lineState >> 3) & 0x01; // 1 bit to know if we are in a closing tag
	int defaultScript = (lineState >> 4) & 0x0F; // 4 bits of script name
	int beforePreProc = (lineState >> 8) & 0xFF; // 8 bits of state

	int scriptLanguage = ScriptOfState(state);

	bool fold = styler.GetPropertyInt("fold");
	bool foldHTML = styler.GetPropertyInt("fold.html",0);
	bool foldCompact = styler.GetPropertyInt("fold.compact",1);

	fold = foldHTML && fold;

	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;
	int visibleChars;

	visibleChars = 0;

	char chPrev = ' ';
	char ch = ' ';
	char chPrevNonWhite = ' ';
	styler.StartSegment(startPos);
	int lengthDoc = startPos + length;
	for (int i = startPos; i < lengthDoc; i++) {
		char chPrev2 = chPrev;
		chPrev = ch;
		if (ch != ' ' && ch != '\t')
			chPrevNonWhite = ch;
		ch = styler[i];
		char chNext = styler.SafeGetCharAt(i + 1);
		char chNext2 = styler.SafeGetCharAt(i + 2);

		// Handle DBCS codepages
		if (styler.IsLeadByte(ch)) {
			chPrev = ' ';
			i += 1;
			continue;
		}

		if ((!isspacechar(ch) || !foldCompact) && fold)
			visibleChars++;

		// decide what is the current state to print (depending of the script tag)
		StateToPrint = statePrintForState(state, inScriptType);

		// handle script folding
		if (fold) {
			switch (scriptLanguage) {
			case eScriptJS:
			case eScriptPHP:
				//not currently supported				case eScriptVBS:

				if ((state != SCE_HPHP_COMMENT) && (state != SCE_HPHP_COMMENTLINE) && (state != SCE_HJ_COMMENT) && (state != SCE_HJ_COMMENTLINE) && (state != SCE_HJ_COMMENTDOC)) {
					if ((ch == '{') || (ch == '}')) {
						levelCurrent += (ch == '{') ? 1 : -1;
					}
				}
				break;
			case eScriptPython:
				if (state != SCE_HP_COMMENTLINE) {
					if ((ch == ':') && ((chNext == '\n') || (chNext == '\r' && chNext2 == '\n'))) {
						levelCurrent++;
					} else if ((ch == '\n') && !((chNext == '\r') && (chNext2 == '\n')) && (chNext != '\n')) {
						// check if the number of tabs is lower than the level
						int Findlevel = (levelCurrent & ~SC_FOLDLEVELBASE) * 8;
						for (int j = 0;Findlevel > 0;j++) {
							char chTmp = styler.SafeGetCharAt(i + j + 1);
							if (chTmp == '\t') {
								Findlevel -= 8;
							}	else if (chTmp == ' ') {
								Findlevel--;
							}	else break;
						}

						if (Findlevel > 0) {
							levelCurrent -= Findlevel / 8;
							if (Findlevel % 8) levelCurrent--;
						}
					}
				}
				break;
			}
		}

		if ((ch == '\r' && chNext != '\n') || (ch == '\n')) {
			// Trigger on CR only (Mac style) or either on LF from CR+LF (Dos/Win) or on LF alone (Unix)
			// Avoid triggering two times on Dos/Win
			// New line -> record any line state onto /next/ line
			if (fold) {
				int lev = levelPrev;
				if (visibleChars == 0)
					lev |= SC_FOLDLEVELWHITEFLAG;
				if ((levelCurrent > levelPrev) && (visibleChars > 0))
					lev |= SC_FOLDLEVELHEADERFLAG;

				styler.SetLevel(lineCurrent, lev);
				visibleChars = 0;
				levelPrev = levelCurrent;
			}
			lineCurrent++;
			styler.SetLineState(lineCurrent,
			                    ((inScriptType  & 0x03) << 0) |
								((tagOpened     & 0x01) << 2) |
								((tagClosing    & 0x01) << 3) |
			                    ((defaultScript & 0x0F) << 4) |
			                    ((beforePreProc & 0xFF) << 8));
		}

		// generic end of script processing
		else if ((inScriptType == eNonHtmlScript) && (ch == '<') && (chNext == '/')) {
			// Check if it's the end of the script tag (or any other HTML tag)
			switch (state) {
				// in these cases, you can embed HTML tags (to confirm !!!!!!!!!!!!!!!!!!!!!!)
			case SCE_H_DOUBLESTRING:
			case SCE_H_SINGLESTRING:
			case SCE_HJ_COMMENT:
			case SCE_HJ_COMMENTDOC:
			// SCE_HJ_COMMENTLINE removed as this is a common thing done to hide
			// the end of script marker from some JS interpreters.
			//case SCE_HJ_COMMENTLINE:
			case SCE_HJ_DOUBLESTRING:
			case SCE_HJ_SINGLESTRING:
			case SCE_HB_STRING:
			case SCE_HP_STRING:
			case SCE_HP_TRIPLE:
			case SCE_HP_TRIPLEDOUBLE:
				break;
			default :
				// maybe we should check here if it's a tag and if it's SCRIPT
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_H_TAGUNKNOWN;
				inScriptType = eHtml;
				scriptLanguage = eScriptNone;
				i += 2;
				// unfold closing script
				levelCurrent--;
				continue;
			}
		}

		/////////////////////////////////////
		// handle the start of PHP pre-processor = Non-HTML
		else if ((state != SCE_H_ASPAT) && 
				!isPHPStringState(state) && 
				(state != SCE_HPHP_COMMENT) && 
				(ch == '<') && 
				(chNext == '?')) {
			styler.ColourTo(i - 1, StateToPrint);
			beforePreProc = state;
			scriptLanguage = segIsScriptingIndicator(styler, styler.GetStartSegment() + 2, i + 10, eScriptPHP);
			i++;
			i += PrintScriptingIndicatorOffset(styler, styler.GetStartSegment() + 2, i + 10);
			if (scriptLanguage == eScriptXML)
				styler.ColourTo(i, SCE_H_XMLSTART);
			else
				styler.ColourTo(i, SCE_H_QUESTION);
			state = StateForScript(scriptLanguage);
			if (inScriptType == eNonHtmlScript)
				inScriptType = eNonHtmlScriptPreProc;
			else
				inScriptType = eNonHtmlPreProc;
			// fold whole script
			levelCurrent++;
			if (scriptLanguage == eScriptXML)
				levelCurrent--; // no folding of the XML first tag (all XML-like tags in this case)
			// should be better
			ch = styler.SafeGetCharAt(i);
			continue;
		}

		// handle the start of ASP pre-processor = Non-HTML
		else if (!isCommentASPState(state) && (ch == '<') && (chNext == '%')) {
			styler.ColourTo(i - 1, StateToPrint);
			beforePreProc = state;
			if (inScriptType == eNonHtmlScript)
				inScriptType = eNonHtmlScriptPreProc;
			else
				inScriptType = eNonHtmlPreProc;

			if (chNext2 == '@') {
				i += 2; // place as if it was the second next char treated
				state = SCE_H_ASPAT;
			} else if ((chNext2 == '-') && (styler.SafeGetCharAt(i + 3) == '-')) {
				styler.ColourTo(i + 3, SCE_H_ASP);
				state = SCE_H_XCCOMMENT;
				scriptLanguage = eScriptVBS;
				continue;
			} else {
				if (chNext2 == '=') {
					i += 2; // place as if it was the second next char treated
				} else {
					i++; // place as if it was the next char treated
				}

				state = StateForScript(defaultScript);
			}
			scriptLanguage = eScriptVBS;
			styler.ColourTo(i, SCE_H_ASP);
			// fold whole script
			levelCurrent++;
			// should be better
			ch = styler.SafeGetCharAt(i);
			continue;
		}

		// handle the end of a pre-processor = Non-HTML
		else if (
			((inScriptType == eNonHtmlPreProc)
				|| (inScriptType == eNonHtmlScriptPreProc)) && (
				((scriptLanguage == eScriptPHP) && (ch == '?') && !isPHPStringState(state) && (state != SCE_HPHP_COMMENT)) || 
				((scriptLanguage != eScriptNone) && !isStringState(state) &&
				 (ch == '%'))
			) && (chNext == '>')) {
			if (state == SCE_H_ASPAT) {
				defaultScript = segIsScriptingIndicator(styler,
					styler.GetStartSegment(), i - 1, defaultScript);
			}
			// Bounce out of any ASP mode
			switch (state) {
			case SCE_HJ_WORD:
				classifyWordHTJS(styler.GetStartSegment(), i - 1, keywords2, styler, inScriptType);
				break;
			case SCE_HB_WORD:
				classifyWordHTVB(styler.GetStartSegment(), i - 1, keywords3, styler, inScriptType);
				break;
			case SCE_HP_WORD:
				classifyWordHTPy(styler.GetStartSegment(), i - 1, keywords4, styler, prevWord, inScriptType);
				break;
			case SCE_HPHP_WORD:
				classifyWordHTPHP(styler.GetStartSegment(), i - 1, keywords5, styler);
				break;
			case SCE_H_XCCOMMENT:
				styler.ColourTo(i - 1, state);
				break;
			default :
				styler.ColourTo(i - 1, StateToPrint);
				break;
			}
			i++;
			if (ch == '%')
				styler.ColourTo(i, SCE_H_ASP);
			else if (scriptLanguage == eScriptXML)
				styler.ColourTo(i, SCE_H_XMLEND);
			else
				styler.ColourTo(i, SCE_H_QUESTION);
			state = beforePreProc;
			if (inScriptType == eNonHtmlScriptPreProc)
				inScriptType = eNonHtmlScript;
			else
				inScriptType = eHtml;
			scriptLanguage = eScriptNone;
			// unfold all scripting languages
			levelCurrent--;
			continue;
		}
		/////////////////////////////////////

		switch (state) {
		case SCE_H_DEFAULT:
			if (ch == '<') {
				// in HTML, fold on tag open and unfold on tag close
				tagOpened = true;
				if (chNext == '/') {
					tagClosing = true;
				} else {
					tagClosing = false;
				}

				styler.ColourTo(i - 1, StateToPrint);
				if (chNext == '!' && chNext2 == '-' && styler.SafeGetCharAt(i + 3) == '-') {
					// should be better
					i += 3;
					levelCurrent++;
					state = SCE_H_COMMENT;
				} else
					state = SCE_H_TAGUNKNOWN;
			} else if (ch == '&') {
				styler.ColourTo(i - 1, SCE_H_DEFAULT);
				state = SCE_H_ENTITY;
			}
			break;
		case SCE_H_COMMENT:
			if ((ch == '>') && (chPrev == '-') && (chPrev2 == '-')) {
				// unfold HTML comment
				levelCurrent--;
				styler.ColourTo(i, StateToPrint);
				state = SCE_H_DEFAULT;
				tagOpened = false;
			}
			break;
		case SCE_H_CDATA:
			if ((ch == '>') && (chPrev == ']') && (chPrev2 == ']')) {
				styler.ColourTo(i, StateToPrint);
				state = SCE_H_DEFAULT;
				tagOpened = false;
			}
			break;
		case SCE_H_SGML:
			if (ch == '>') {
				levelCurrent--;
				styler.ColourTo(i, StateToPrint);
				state = SCE_H_DEFAULT;
				tagOpened = false;
			}
			break;
		case SCE_H_ENTITY:
			if (ch == ';') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_H_DEFAULT;
			}
			if (ch != '#' && !isalnum(ch)) {	// Should check that '#' follows '&', but it is unlikely anyway...
				styler.ColourTo(i, SCE_H_TAGUNKNOWN);
				state = SCE_H_DEFAULT;
			}
			break;
		case SCE_H_TAGUNKNOWN:
			if (!ishtmlwordchar(ch) && !((ch == '/') && (chPrev == '<')) && ch != '[') {
				int eClass = classifyTagHTML(styler.GetStartSegment(), i - 1, keywords, styler);
				if (eClass == SCE_H_SCRIPT) {
					inScriptType = eNonHtmlScript;
					scriptLanguage = defaultScript;
					eClass = SCE_H_TAG;
				}
				if (ch == '>') {
					styler.ColourTo(i, eClass);
					if (inScriptType == eNonHtmlScript) {
						state = StateForScript(scriptLanguage);
					} else {
						state = SCE_H_DEFAULT;
					}
					tagOpened = false;
					if (tagClosing)
						levelCurrent--;
					else
						levelCurrent++;
					tagClosing = false;
			    } else if (ch == '/' && chNext == '>') {
					if (eClass == SCE_H_TAGUNKNOWN) {
					    styler.ColourTo(i + 1, SCE_H_TAGUNKNOWN);
					} else {
					    styler.ColourTo(i - 1, StateToPrint);
					    styler.ColourTo(i + 1, SCE_H_TAGEND);
					}
				    i++;
				    ch = chNext;
				    state = SCE_H_DEFAULT;
					tagOpened = false;
				} else {
					if (eClass != SCE_H_TAGUNKNOWN) {
						if (eClass == SCE_H_CDATA) {
							state = SCE_H_CDATA;
						} else if (eClass == SCE_H_SGML) {
							state = SCE_H_SGML;
						} else {
							state = SCE_H_OTHER;
						}
					}
				}
			}
			break;
		case SCE_H_ATTRIBUTE:
			if (!ishtmlwordchar(ch) && ch != '/' && ch != '-') {
				if (inScriptType == eNonHtmlScript) {
					int scriptLanguagePrev = scriptLanguage;
					scriptLanguage = segIsScriptingIndicator(styler, styler.GetStartSegment(), i - 1, scriptLanguage);
					if ((scriptLanguagePrev != scriptLanguage) && (scriptLanguage == eScriptNone))
						inScriptType = eHtml;
				}
				classifyAttribHTML(styler.GetStartSegment(), i - 1, keywords, styler);
				if (ch == '>') {
					styler.ColourTo(i, SCE_H_TAG);
					if (inScriptType == eNonHtmlScript) {
						state = StateForScript(scriptLanguage);
					} else {
						state = SCE_H_DEFAULT;
					}
					tagOpened = false;
					if (tagClosing)
						levelCurrent--;
					else
						levelCurrent++;
					tagClosing = false;
				} else if (ch == '=') {
					styler.ColourTo(i, SCE_H_OTHER);
					state = SCE_H_VALUE;
				} else {
					state = SCE_H_OTHER;
				}
			}
			break;
		case SCE_H_OTHER:
			if (ch == '>') {
				styler.ColourTo(i - 1, StateToPrint);
				styler.ColourTo(i, SCE_H_TAG);
				if (inScriptType == eNonHtmlScript) {
					state = StateForScript(scriptLanguage);
				} else {
					state = SCE_H_DEFAULT;
				}
				tagOpened = false;
				if (tagClosing)
					levelCurrent--;
				else
					levelCurrent++;
				tagClosing = false;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_H_DOUBLESTRING;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_H_SINGLESTRING;
			} else if (ch == '=') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_H_VALUE;
			} else if (ch == '/' && chNext == '>') {
				styler.ColourTo(i - 1, StateToPrint);
				styler.ColourTo(i + 1, SCE_H_TAGEND);
				i++;
				ch = chNext;
				state = SCE_H_DEFAULT;
				tagOpened = false;
			} else if (ch == '?' && chNext == '>') {
				styler.ColourTo(i - 1, StateToPrint);
				styler.ColourTo(i + 1, SCE_H_XMLEND);
				i++;
				ch = chNext;
				state = SCE_H_DEFAULT;
			} else if (ishtmlwordchar(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_H_ATTRIBUTE;
			}
			break;
		case SCE_H_DOUBLESTRING:
			if (ch == '\"') {
				if (inScriptType == eNonHtmlScript) {
					scriptLanguage = segIsScriptingIndicator(styler, styler.GetStartSegment(), i, scriptLanguage);
				}
				styler.ColourTo(i, SCE_H_DOUBLESTRING);
				state = SCE_H_OTHER;
			}
			break;
		case SCE_H_SINGLESTRING:
			if (ch == '\'') {
				if (inScriptType == eNonHtmlScript) {
					scriptLanguage = segIsScriptingIndicator(styler, styler.GetStartSegment(), i, scriptLanguage);
				}
				styler.ColourTo(i, SCE_H_SINGLESTRING);
				state = SCE_H_OTHER;
			}
			break;
		case SCE_H_VALUE:
			if (!ishtmlwordchar(ch)) {
				if (ch == '\"') {
					// Should really test for being first character
					state = SCE_H_DOUBLESTRING;
				} else if (ch == '\'') {
					state = SCE_H_SINGLESTRING;
				} else {
					if (IsNumber(styler.GetStartSegment(), styler)) {
						styler.ColourTo(i - 1, SCE_H_NUMBER);
					} else {
						styler.ColourTo(i - 1, StateToPrint);
					}
					if (ch == '>') {
						styler.ColourTo(i, SCE_H_TAG);
						if (inScriptType == eNonHtmlScript) {
							state = StateForScript(scriptLanguage);
						} else {
							state = SCE_H_DEFAULT;
						}
						tagOpened = false;
						if (tagClosing)
							levelCurrent--;
						else
							levelCurrent++;
						tagClosing = false;
					} else {
						state = SCE_H_OTHER;
					}
				}
			}
			break;
		case SCE_HJ_DEFAULT:
		case SCE_HJ_START:
		case SCE_HJ_SYMBOLS:
			if (iswordstart(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_WORD;
			} else if (ch == '/' && chNext == '*') {
				styler.ColourTo(i - 1, StateToPrint);
				if (chNext2 == '*')
					state = SCE_HJ_COMMENTDOC;
				else
					state = SCE_HJ_COMMENT;
			} else if (ch == '/' && chNext == '/') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_COMMENTLINE;
			} else if (ch == '/' && isOKBeforeRE(chPrevNonWhite)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_REGEX;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_DOUBLESTRING;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_SINGLESTRING;
			} else if ((ch == '<') && (chNext == '!') && (chNext2 == '-') &&
			           styler.SafeGetCharAt(i + 3) == '-') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_COMMENTLINE;
			} else if ((ch == '-') && (chNext == '-') && (chNext2 == '>')) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_COMMENTLINE;
				i += 2;
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				styler.ColourTo(i, statePrintForState(SCE_HJ_SYMBOLS, inScriptType));
				state = SCE_HJ_DEFAULT;
			} else if ((ch == ' ') || (ch == '\t')) {
				if (state == SCE_HJ_START) {
					styler.ColourTo(i - 1, StateToPrint);
					state = SCE_HJ_DEFAULT;
				}
			}
			break;
		case SCE_HJ_WORD:
			if (!iswordchar(ch)) {
				classifyWordHTJS(styler.GetStartSegment(), i - 1, keywords2, styler, inScriptType);
				//styler.ColourTo(i - 1, eHTJSKeyword);
				state = SCE_HJ_DEFAULT;
				if (ch == '/' && chNext == '*') {
					if (chNext2 == '*')
						state = SCE_HJ_COMMENTDOC;
					else
						state = SCE_HJ_COMMENT;
				} else if (ch == '/' && chNext == '/') {
					state = SCE_HJ_COMMENTLINE;
				} else if (ch == '\"') {
					state = SCE_HJ_DOUBLESTRING;
				} else if (ch == '\'') {
					state = SCE_HJ_SINGLESTRING;
				} else if ((ch == '-') && (chNext == '-') && (chNext2 == '>')) {
					styler.ColourTo(i - 1, StateToPrint);
					state = SCE_HJ_COMMENTLINE;
					i += 2;
				} else if (isoperator(ch)) {
					styler.ColourTo(i, statePrintForState(SCE_HJ_SYMBOLS, inScriptType));
					state = SCE_HJ_DEFAULT;
				}
			}
			break;
		case SCE_HJ_COMMENT:
		case SCE_HJ_COMMENTDOC:
			if (ch == '/' && chPrev == '*') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HJ_DEFAULT;
			}
			break;
		case SCE_HJ_COMMENTLINE:
			if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i - 1, statePrintForState(SCE_HJ_COMMENTLINE, inScriptType));
				state = SCE_HJ_DEFAULT;
			}
			break;
		case SCE_HJ_DOUBLESTRING:
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
				}
			} else if (ch == '\"') {
				styler.ColourTo(i, statePrintForState(SCE_HJ_DOUBLESTRING, inScriptType));
				state = SCE_HJ_DEFAULT;
			} else if ((inScriptType == eNonHtmlScript) && (ch == '-') && (chNext == '-') && (chNext2 == '>')) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_COMMENTLINE;
				i += 2;
			} else if (isLineEnd(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_STRINGEOL;
			}
			break;
		case SCE_HJ_SINGLESTRING:
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
				}
			} else if (ch == '\'') {
				styler.ColourTo(i, statePrintForState(SCE_HJ_SINGLESTRING, inScriptType));
				state = SCE_HJ_DEFAULT;
			} else if ((inScriptType == eNonHtmlScript) && (ch == '-') && (chNext == '-') && (chNext2 == '>')) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_COMMENTLINE;
				i += 2;
			} else if (isLineEnd(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_STRINGEOL;
			}
			break;
		case SCE_HJ_STRINGEOL:
			if (!isLineEnd(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_DEFAULT;
			} else if (!isLineEnd(chNext)) {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HJ_DEFAULT;
			}
			break;
		case SCE_HJ_REGEX:
			if (ch == '\r' || ch == '\n' || ch == '/') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HJ_DEFAULT;
			} else if (ch == '\\') {
				// Gobble up the quoted character
				if (chNext == '\\' || chNext == '/') {
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			}
			break;
		case SCE_HB_DEFAULT:
		case SCE_HB_START:
			if (iswordstart(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HB_WORD;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HB_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HB_STRING;
			} else if ((ch == '<') && (chNext == '!') && (chNext2 == '-') &&
			           styler.SafeGetCharAt(i + 3) == '-') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HB_COMMENTLINE;
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				styler.ColourTo(i, statePrintForState(SCE_HB_DEFAULT, inScriptType));
				state = SCE_HB_DEFAULT;
			} else if ((ch == ' ') || (ch == '\t')) {
				if (state == SCE_HB_START) {
					styler.ColourTo(i - 1, StateToPrint);
					state = SCE_HB_DEFAULT;
				}
			}
			break;
		case SCE_HB_WORD:
			if (!iswordchar(ch)) {
				state = classifyWordHTVB(styler.GetStartSegment(), i - 1, keywords3, styler, inScriptType);
				if (state == SCE_HB_DEFAULT) {
					if (ch == '\"') {
						state = SCE_HB_STRING;
					} else if (ch == '\'') {
						state = SCE_HB_COMMENTLINE;
					} else if (isoperator(ch)) {
						styler.ColourTo(i, statePrintForState(SCE_HB_DEFAULT, inScriptType));
						state = SCE_HB_DEFAULT;
					}
				}
			}
			break;
		case SCE_HB_STRING:
			if (ch == '\"') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HB_DEFAULT;
			} else if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HB_STRINGEOL;
			}
			break;
		case SCE_HB_COMMENTLINE:
			if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HB_DEFAULT;
			}
			break;
		case SCE_HB_STRINGEOL:
			if (!isLineEnd(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HB_DEFAULT;
			} else if (!isLineEnd(chNext)) {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HB_DEFAULT;
			}
			break;
		case SCE_HP_DEFAULT:
		case SCE_HP_START:
			if (iswordstart(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HP_WORD;
			} else if ((ch == '<') && (chNext == '!') && (chNext2 == '-') &&
			           styler.SafeGetCharAt(i + 3) == '-') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HP_COMMENTLINE;
			} else if (ch == '#') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HP_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, StateToPrint);
				if (chNext == '\"' && chNext2 == '\"') {
					i += 2;
					state = SCE_HP_TRIPLEDOUBLE;
					ch = ' ';
					chPrev = ' ';
					chNext = styler.SafeGetCharAt(i + 1);
				} else {
					//					state = statePrintForState(SCE_HP_STRING,inScriptType);
					state = SCE_HP_STRING;
				}
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, StateToPrint);
				if (chNext == '\'' && chNext2 == '\'') {
					i += 2;
					state = SCE_HP_TRIPLE;
					ch = ' ';
					chPrev = ' ';
					chNext = styler.SafeGetCharAt(i + 1);
				} else {
					state = SCE_HP_CHARACTER;
				}
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				styler.ColourTo(i, statePrintForState(SCE_HP_OPERATOR, inScriptType));
			} else if ((ch == ' ') || (ch == '\t')) {
				if (state == SCE_HP_START) {
					styler.ColourTo(i - 1, StateToPrint);
					state = SCE_HP_DEFAULT;
				}
			}
			break;
		case SCE_HP_WORD:
			if (!iswordchar(ch)) {
				classifyWordHTPy(styler.GetStartSegment(), i - 1, keywords4, styler, prevWord, inScriptType);
				state = SCE_HP_DEFAULT;
				if (ch == '#') {
					state = SCE_HP_COMMENTLINE;
				} else if (ch == '\"') {
					if (chNext == '\"' && chNext2 == '\"') {
						i += 2;
						state = SCE_HP_TRIPLEDOUBLE;
						ch = ' ';
						chPrev = ' ';
						chNext = styler.SafeGetCharAt(i + 1);
					} else {
						state = SCE_HP_STRING;
					}
				} else if (ch == '\'') {
					if (chNext == '\'' && chNext2 == '\'') {
						i += 2;
						state = SCE_HP_TRIPLE;
						ch = ' ';
						chPrev = ' ';
						chNext = styler.SafeGetCharAt(i + 1);
					} else {
						state = SCE_HP_CHARACTER;
					}
				} else if (isoperator(ch)) {
					styler.ColourTo(i, statePrintForState(SCE_HP_OPERATOR, inScriptType));
				}
			}
			break;
		case SCE_HP_COMMENTLINE:
			if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HP_DEFAULT;
			}
			break;
		case SCE_HP_STRING:
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (ch == '\"') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HP_DEFAULT;
			}
			break;
		case SCE_HP_CHARACTER:
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (ch == '\'') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HP_DEFAULT;
			}
			break;
		case SCE_HP_TRIPLE:
			if (ch == '\'' && chPrev == '\'' && chPrev2 == '\'') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HP_DEFAULT;
			}
			break;
		case SCE_HP_TRIPLEDOUBLE:
			if (ch == '\"' && chPrev == '\"' && chPrev2 == '\"') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HP_DEFAULT;
			}
			break;
			///////////// start - PHP state handling
		case SCE_HPHP_WORD:
			if (!iswordstart(ch)) {
				classifyWordHTPHP(styler.GetStartSegment(), i - 1, keywords5, styler);
				if (ch == '/' && chNext == '*') {
					i++;
					state = SCE_HPHP_COMMENT;
				} else if (ch == '/' && chNext == '/') {
					i++;
					state = SCE_HPHP_COMMENTLINE;
				} else if (ch == '#') {
					state = SCE_HPHP_COMMENTLINE;
				} else if (ch == '\"') {
					state = SCE_HPHP_HSTRING;
				} else if (ch == '\'') {
					state = SCE_HPHP_SIMPLESTRING;
				} else if (ch == '$') {
					state = SCE_HPHP_VARIABLE;
				} else if (isoperator(ch)) {
					state = SCE_HPHP_OPERATOR;
				} else {
					state = SCE_HPHP_DEFAULT;
				}
			}
			break;
		case SCE_HPHP_NUMBER:
			if (!isdigit(ch)) {
				styler.ColourTo(i - 1, SCE_HPHP_NUMBER);
				if (isoperator(ch)) 
					state =SCE_HPHP_OPERATOR;
				else 
					state = SCE_HPHP_DEFAULT;
			}
			break;
		case SCE_HPHP_VARIABLE:
			if (!iswordstart(ch)) {
				styler.ColourTo(i - 1, SCE_HPHP_VARIABLE);
				if (isoperator(ch)) 
					state =SCE_HPHP_OPERATOR;
				else 
					state = SCE_HPHP_DEFAULT;
			}
			break;
		case SCE_HPHP_COMMENT:
			if (ch == '/' && chPrev == '*') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HPHP_DEFAULT;
			}
			break;
		case SCE_HPHP_COMMENTLINE:
			if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HPHP_DEFAULT;
			}
			break;
		case SCE_HPHP_HSTRING:
			if (ch == '\\') {
				// skip the next char
				i++;
			} else if (ch == '$') {
				styler.ColourTo(i-1, StateToPrint);
				state = SCE_HPHP_HSTRING_VARIABLE;
			} else if (ch == '\"') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HPHP_DEFAULT;
			}
			break;
		case SCE_HPHP_SIMPLESTRING:
			if (ch == '\\') {
				// skip the next char
				i++;
			} else if (ch == '\'') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HPHP_DEFAULT;
			}
			break;
		case SCE_HPHP_HSTRING_VARIABLE:
			if (!iswordstart(ch)) {
				styler.ColourTo(i-1, StateToPrint);
				i--; // strange but it works
				state = SCE_HPHP_HSTRING;
			}
			break;
		case SCE_HPHP_OPERATOR:
  		case SCE_HPHP_DEFAULT:
			styler.ColourTo(i - 1, StateToPrint);
			if (isdigit(ch)) {
				state = SCE_HPHP_NUMBER;
			} else if (iswordstart(ch)) {
				state = SCE_HPHP_WORD;
			} else if (ch == '/' && chNext == '*') {
				i++;
				state = SCE_HPHP_COMMENT;
			} else if (ch == '/' && chNext == '/') {
				i++;
				state = SCE_HPHP_COMMENTLINE;
			} else if (ch == '#') {
				state = SCE_HPHP_COMMENTLINE;
			} else if (ch == '\"') {
				state = SCE_HPHP_HSTRING;
			} else if (ch == '\'') {
				state = SCE_HPHP_SIMPLESTRING;
			} else if (ch == '$') {
				state = SCE_HPHP_VARIABLE;
			} else if (isoperator(ch)) {
				state = SCE_HPHP_OPERATOR;
			} else if ((state == SCE_HPHP_OPERATOR) && (isspacechar(ch))) {
				state = SCE_HPHP_DEFAULT;
			}
			break;
			///////////// end - PHP state handling
		}

		// Some of the above terminated their lexeme but since the same character starts
		// the same class again, only reenter if non empty segment.
		bool nonEmptySegment = i >= static_cast<int>(styler.GetStartSegment());
		if (state == SCE_HB_DEFAULT) {    // One of the above succeeded
			if ((ch == '\"') && (nonEmptySegment)) {
				state = SCE_HB_STRING;
			} else if (ch == '\'') {
				state = SCE_HB_COMMENTLINE;
			} else if (iswordstart(ch)) {
				state = SCE_HB_WORD;
			} else if (isoperator(ch)) {
				styler.ColourTo(i, SCE_HB_DEFAULT);
			}
		} else if (state == SCE_HBA_DEFAULT) {    // One of the above succeeded
			if ((ch == '\"') && (nonEmptySegment)) {
				state = SCE_HBA_STRING;
			} else if (ch == '\'') {
				state = SCE_HBA_COMMENTLINE;
			} else if (iswordstart(ch)) {
				state = SCE_HBA_WORD;
			} else if (isoperator(ch)) {
				styler.ColourTo(i, SCE_HBA_DEFAULT);
			}
		} else if (state == SCE_HJ_DEFAULT) {    // One of the above succeeded
			if (ch == '/' && chNext == '*') {
				if (styler.SafeGetCharAt(i + 2) == '*')
					state = SCE_HJ_COMMENTDOC;
				else
					state = SCE_HJ_COMMENT;
			} else if (ch == '/' && chNext == '/') {
				state = SCE_HJ_COMMENTLINE;
			} else if ((ch == '\"') && (nonEmptySegment)) {
				state = SCE_HJ_DOUBLESTRING;
			} else if ((ch == '\'')  && (nonEmptySegment)) {
				state = SCE_HJ_SINGLESTRING;
			} else if (iswordstart(ch)) {
				state = SCE_HJ_WORD;
			} else if (isoperator(ch)) {
				styler.ColourTo(i, statePrintForState(SCE_HJ_SYMBOLS, inScriptType));
			}
		}
	}

	StateToPrint = statePrintForState(state, inScriptType);
	styler.ColourTo(lengthDoc - 1, StateToPrint);

	// Fill in the real level of the next line, keeping the current flags as they will be filled in later
	if (fold) {
		int flagsNext = styler.LevelAt(lineCurrent) & ~SC_FOLDLEVELNUMBERMASK;
		styler.SetLevel(lineCurrent, levelPrev | flagsNext);
	}
}

LexerModule lmHTML(SCLEX_HTML, ColouriseHyperTextDoc, "hypertext");
LexerModule lmXML(SCLEX_XML, ColouriseHyperTextDoc, "xml");

