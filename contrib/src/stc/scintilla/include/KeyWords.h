// SciTE - Scintilla based Text Editor
// KeyWords.h - colourise for particular languages
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

typedef void (*LexerFunction)(unsigned int startPos, int lengthDoc, int initStyle,
                  WordList *keywordlists[], StylingContext &styler);
                  
class LexerModule {
	static LexerModule *base;
	LexerModule *next;
	int language;
	LexerFunction fn;
public:
	LexerModule(int language_, LexerFunction fn_);
	static void Colourise(unsigned int startPos, int lengthDoc, int initStyle,
                  int language, WordList *keywordlists[], StylingContext &styler);
};

inline bool iswordchar(char ch) {
	return isalnum(ch) || ch == '.' || ch == '_';
}

inline bool iswordstart(char ch) {
	return isalnum(ch) || ch == '_';
}

inline bool isoperator(char ch) {
	if (isalnum(ch))
		return false;
	// '.' left out as it is used to make up numbers
	if (ch == '%' || ch == '^' || ch == '&' || ch == '*' ||
	        ch == '(' || ch == ')' || ch == '-' || ch == '+' ||
	        ch == '=' || ch == '|' || ch == '{' || ch == '}' ||
	        ch == '[' || ch == ']' || ch == ':' || ch == ';' ||
	        ch == '<' || ch == '>' || ch == ',' || ch == '/' ||
	        ch == '?' || ch == '!' || ch == '.' || ch == '~')
		return true;
	return false;
}

