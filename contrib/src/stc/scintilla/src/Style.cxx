// Scintilla source code edit control
// Style.cxx - defines the font and colour style for a class of text
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <string.h>

#include "Platform.h"

#include "Style.h"

Style::Style() {
	Clear();
}

Style::~Style() {
	font.Release();
}

Style &Style::operator=(const Style &source) {
	if (this == &source)
		return *this;
	Clear();
	fore.desired = source.fore.desired;
	back.desired = source.back.desired;
	bold = source.bold;
	italic = source.italic;
	size = source.size;
	strcpy(fontName, source.fontName);
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
	strcpy(fontName, fontName_);
	eolFilled = eolFilled_;
	font.Release();
}

void Style::Realise(Surface &surface, int zoomLevel) {
	int sizeZoomed = size + zoomLevel;
	if (sizeZoomed <= 2)	// Hangs if sizeZoomed <= 1
		sizeZoomed = 2;
		
	int deviceHeight = (sizeZoomed * surface.LogPixelsY()) / 72;
	font.Create(fontName, deviceHeight, bold, italic);

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
