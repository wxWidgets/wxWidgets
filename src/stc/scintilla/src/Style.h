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
	bool aliasOfDefaultFont;
	bool bold;
	bool italic;
	int size;
	const char *fontName;
	int characterSet;
	bool eolFilled;
	bool underline;

	Font font;
	unsigned int lineHeight;
	unsigned int ascent;
	unsigned int descent;
	unsigned int externalLeading;
	unsigned int aveCharWidth;
	unsigned int spaceWidth;

	Style();
	Style(const Style &source);
	~Style();
	Style &operator=(const Style &source);
	void Clear(Colour fore_, Colour back_,
           	int size_, 
		const char *fontName_, int characterSet_,
		bool bold_, bool italic_, bool eolFilled_, bool underline_);
	bool EquivalentFontTo(const Style *other) const;
	void Realise(Surface &surface, int zoomLevel, Style *defaultStyle=0);
};

#endif
