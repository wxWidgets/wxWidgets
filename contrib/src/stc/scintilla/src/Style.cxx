// Scintilla source code edit control
// Style.cxx - defines the font and colour style for a class of text
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <string.h>

#include "Platform.h"

#include "Style.h"

Style::Style() {
	aliasOfDefaultFont = true;
	Clear(Colour(0,0,0), Colour(0xff,0xff,0xff),
	        Platform::DefaultFontSize(), 0,
		false, false, false);
}
	
Style::Style(const Style &source) {
	Clear(Colour(0,0,0), Colour(0xff,0xff,0xff),
	        0, 0,
		false, false, false);
	fore.desired = source.fore.desired;
	back.desired = source.back.desired;
	bold = source.bold;
	italic = source.italic;
	size = source.size;
	eolFilled = source.eolFilled;
}

Style::~Style() {
	if (aliasOfDefaultFont)
		font.SetID(0);
	else
		font.Release();
	aliasOfDefaultFont = false;
}

Style &Style::operator=(const Style &source) {
	if (this == &source)
		return *this;
	Clear(Colour(0,0,0), Colour(0xff,0xff,0xff),
	        0, 0,
		false, false, false);
	fore.desired = source.fore.desired;
	back.desired = source.back.desired;
	bold = source.bold;
	italic = source.italic;
	size = source.size;
	eolFilled = source.eolFilled;
	return *this;
}

void Style::Clear(Colour fore_, Colour back_, int size_, const char *fontName_, 
	bool bold_, bool italic_, bool eolFilled_) {
	fore.desired = fore_;
	back.desired = back_;
	bold = bold_;
	italic = italic_;
	size = size_;
	fontName = fontName_;
	eolFilled = eolFilled_;
	if (aliasOfDefaultFont)
		font.SetID(0);
	else 
		font.Release();
	aliasOfDefaultFont = false;
}

bool Style::EquivalentFontTo(const Style *other) const {
	if (bold != other->bold ||
		italic != other->italic ||
		size != other->size)
		return false;
	if (fontName == other->fontName)
		return true;
	if (!fontName)
		return false;
	if (!other->fontName)
		return false;
	return strcmp(fontName, other->fontName) == 0;
}

void Style::Realise(Surface &surface, int zoomLevel, Style *defaultStyle) {
	int sizeZoomed = size + zoomLevel;
	if (sizeZoomed <= 2)	// Hangs if sizeZoomed <= 1
		sizeZoomed = 2;

	if (aliasOfDefaultFont)
		font.SetID(0);
	else 
		font.Release();
	int deviceHeight = surface.DeviceHeightFont(sizeZoomed);
	aliasOfDefaultFont = defaultStyle && 
		(EquivalentFontTo(defaultStyle) || !fontName);
	if (aliasOfDefaultFont) {
		font.SetID(defaultStyle->font.GetID());
	} else if (fontName) {
		font.Create(fontName, deviceHeight, bold, italic);
	} else {
		font.SetID(0);
	}

	ascent = surface.Ascent(font);
	descent = surface.Descent(font);
	// Probably more typographically correct to include leading
	// but that means more complex drawing as leading must be erased
	//lineHeight = surface.ExternalLeading() + surface.Height();
	externalLeading = surface.ExternalLeading(font);
	lineHeight = surface.Height(font);
	aveCharWidth = surface.AverageCharWidth(font);
	spaceWidth = surface.WidthChar(font, ' ');
}
