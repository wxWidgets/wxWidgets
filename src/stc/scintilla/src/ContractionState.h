// Scintilla source code edit control
// ContractionState.h - manages visibility of lines for folding
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef CONTRACTIONSTATE_H
#define CONTRACTIONSTATE_H

class OneLine {
public:
	int displayLine;	// position within set of visible lines
	int docLine;		// inverse of displayLine
	bool visible;
	bool expanded;
	
	OneLine();
	virtual ~OneLine() {}
};

class ContractionState {
	void Grow(int sizeNew);
	enum { growSize = 4000 };
	int linesInDoc;
	int linesInDisplay;
	mutable OneLine *lines;
	int size;
	mutable bool valid;
	void MakeValid() const;
public:
	ContractionState();
	virtual ~ContractionState();
	
	void Clear();
	
	int LinesInDoc() const;	
	int LinesDisplayed() const;	
	int DisplayFromDoc(int lineDoc) const;
	int DocFromDisplay(int lineDisplay) const;
	
	void InsertLines(int lineDoc, int lineCount);
	void DeleteLines(int lineDoc, int lineCount);
	
	bool GetVisible(int lineDoc) const;
	bool SetVisible(int lineDocStart, int lineDocEnd, bool visible);
	
	bool GetExpanded(int lineDoc) const;
	bool SetExpanded(int lineDoc, bool expanded);
};

#endif
