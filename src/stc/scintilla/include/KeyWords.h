// SciTE - Scintilla based Text Editor
// KeyWords.h - colourise for particular languages
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

void ColouriseDoc(int codePage, int startPos, int lengthDoc, int initStyle, 
	int language, WordList *keywordlists[], StylingContext &styler);

