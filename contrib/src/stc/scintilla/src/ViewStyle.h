// Scintilla source code edit control
// ViewStyle.h - store information on how the document is to be viewed
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef VIEWSTYLE_H
#define VIEWSTYLE_H

class MarginStyle {
public:
	bool symbol;
	int width;
	int mask;
	bool sensitive;
	MarginStyle();
};

class ViewStyle {
public:
	Style styles[STYLE_MAX + 1];
	LineMarker markers[MARKER_MAX + 1];
	Indicator indicators[INDIC_MAX + 1];
	int lineHeight;
	unsigned int maxAscent;
	unsigned int maxDescent;
	unsigned int aveCharWidth;
	unsigned int spaceWidth;
	bool selforeset;
	ColourPair selforeground;
	bool selbackset;
	ColourPair selbackground;
	ColourPair selbar;
	ColourPair selbarlight;
	// Margins are ordered: Line Numbers, Selection Margin, Spacing Margin
	int leftMarginWidth;		// Spacing margin on left of text
	int rightMarginWidth;	// Spacing margin on left of text
	enum { margins=3 };
	bool symbolMargin;
	int maskInLine;	// Mask for markers to be put into text because there is nowhere for them to go in margin
	MarginStyle ms[margins];
	int fixedColumnWidth;
	int zoomLevel;
	bool viewWhitespace;
	bool viewEOL;
	bool showMarkedLines;
	ColourPair caretcolour;
	ColourPair edgecolour;
	
	ViewStyle();
	ViewStyle(const ViewStyle &source);
	~ViewStyle();
	void Init();
	void RefreshColourPalette(Palette &pal, bool want);
	void Refresh(Surface &surface);
	void ResetDefaultStyle();
	void ClearStyles();
};

#endif
