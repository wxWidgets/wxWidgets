// Scintilla source code edit control
// Style.h - defines the font and colour style for a class of text
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef STYLE_H
#define STYLE_H

class Style {
public:
	ColourPair fore;
	ColourPair back;
	bool bold;
	bool italic;
	int size;
	char fontName[100];
	bool eolFilled;

	Font font;
	unsigned int lineHeight;
	unsigned int ascent;
	unsigned int descent;
	unsigned int externalLeading;
	unsigned int aveCharWidth;
	unsigned int spaceWidth;

	Style();
	~Style();
	Style &operator=(const Style &source);
	void Clear(Colour fore_=Colour(0,0,0), Colour back_=Colour(0xff,0xff,0xff),
           	int size_=Platform::DefaultFontSize(), 
		const char *fontName_=Platform::DefaultFont(), 
		bool bold_=false, bool italic_=false, bool eolFilled_=false);
	void Realise(Surface &surface, int zoomLevel);
};

#endif
