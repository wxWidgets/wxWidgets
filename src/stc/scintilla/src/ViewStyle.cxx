// Scintilla source code edit control
/** @file ViewStyle.cxx
 ** Store information on how the document is to be viewed.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <string.h>

#include "Platform.h"

#include "Scintilla.h"
#include "Indicator.h"
#include "XPM.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"

MarginStyle::MarginStyle() :
	symbol(false), width(16), mask(0xffffffff), sensitive(false) {
}

// A list of the fontnames - avoids wasting space in each style
FontNames::FontNames() {
	max = 0;
}

FontNames::~FontNames() {
	Clear();
}

void FontNames::Clear() {
	for (int i=0;i<max;i++) {
		delete []names[i];
	}
	max = 0;
}

const char *FontNames::Save(const char *name) {
	if (!name)
		return 0;
	for (int i=0;i<max;i++) {
		if (strcmp(names[i], name) == 0) {
			return names[i];
		}
	}
	names[max] = new char[strlen(name) + 1];
	strcpy(names[max], name);
	max++;
	return names[max-1];
}

ViewStyle::ViewStyle() {
	Init();
}

ViewStyle::ViewStyle(const ViewStyle &source) {
	Init();
	for (unsigned int sty=0;sty<(sizeof(styles)/sizeof(styles[0]));sty++) {
		styles[sty] = source.styles[sty];
		// Can't just copy fontname as its lifetime is relative to its owning ViewStyle
		styles[sty].fontName = fontNames.Save(source.styles[sty].fontName);
	}
	for (int mrk=0;mrk<=MARKER_MAX;mrk++) {
		markers[mrk] = source.markers[mrk];
	}
	for (int ind=0;ind<=INDIC_MAX;ind++) {
		indicators[ind] = source.indicators[ind];
	}

	selforeset = source.selforeset;
	selforeground.desired = source.selforeground.desired;
	selbackset = source.selbackset;
	selbackground.desired = source.selbackground.desired;
	selbackground2.desired = source.selbackground2.desired;

	foldmarginColourSet = source.foldmarginColourSet;
	foldmarginColour.desired = source.foldmarginColour.desired;
	foldmarginHighlightColourSet = source.foldmarginHighlightColourSet;
	foldmarginHighlightColour.desired = source.foldmarginHighlightColour.desired;

	hotspotForegroundSet = source.hotspotForegroundSet;
	hotspotForeground.desired = source.hotspotForeground.desired;
	hotspotBackgroundSet = source.hotspotBackgroundSet;
	hotspotBackground.desired = source.hotspotBackground.desired;
	hotspotUnderline = source.hotspotUnderline;

	whitespaceForegroundSet = source.whitespaceForegroundSet;
	whitespaceForeground.desired = source.whitespaceForeground.desired;
	whitespaceBackgroundSet = source.whitespaceBackgroundSet;
	whitespaceBackground.desired = source.whitespaceBackground.desired;
	selbar.desired = source.selbar.desired;
	selbarlight.desired = source.selbarlight.desired;
	caretcolour.desired = source.caretcolour.desired;
	showCaretLineBackground = source.showCaretLineBackground;
	caretLineBackground.desired = source.caretLineBackground.desired;
	edgecolour.desired = source.edgecolour.desired;
	edgeState = source.edgeState;
	caretWidth = source.caretWidth;
	leftMarginWidth = source.leftMarginWidth;
	rightMarginWidth = source.rightMarginWidth;
	for (int i=0;i < margins; i++) {
		ms[i] = source.ms[i];
	}
	symbolMargin = source.symbolMargin;
	maskInLine = source.maskInLine;
	fixedColumnWidth = source.fixedColumnWidth;
	zoomLevel = source.zoomLevel;
	viewWhitespace = source.viewWhitespace;
	viewIndentationGuides = source.viewIndentationGuides;
	viewEOL = source.viewEOL;
	showMarkedLines = source.showMarkedLines;
}

ViewStyle::~ViewStyle() {
}

void ViewStyle::Init() {
	fontNames.Clear();
	ResetDefaultStyle();

	indicators[0].style = INDIC_SQUIGGLE;
	indicators[0].fore = ColourDesired(0, 0x7f, 0);
	indicators[1].style = INDIC_TT;
	indicators[1].fore = ColourDesired(0, 0, 0xff);
	indicators[2].style = INDIC_PLAIN;
	indicators[2].fore = ColourDesired(0xff, 0, 0);

	lineHeight = 1;
	maxAscent = 1;
	maxDescent = 1;
	aveCharWidth = 8;
	spaceWidth = 8;

	selforeset = false;
	selforeground.desired = ColourDesired(0xff, 0, 0);
	selbackset = true;
	selbackground.desired = ColourDesired(0xc0, 0xc0, 0xc0);
	selbackground2.desired = ColourDesired(0xb0, 0xb0, 0xb0);

	foldmarginColourSet = false;
	foldmarginColour.desired = ColourDesired(0xff, 0, 0);
	foldmarginHighlightColourSet = false;
	foldmarginHighlightColour.desired = ColourDesired(0xc0, 0xc0, 0xc0);

	whitespaceForegroundSet = false;
	whitespaceForeground.desired = ColourDesired(0, 0, 0);
	whitespaceBackgroundSet = false;
	whitespaceBackground.desired = ColourDesired(0xff, 0xff, 0xff);
	selbar.desired = Platform::Chrome();
	selbarlight.desired = Platform::ChromeHighlight();
	styles[STYLE_LINENUMBER].fore.desired = ColourDesired(0, 0, 0);
	styles[STYLE_LINENUMBER].back.desired = Platform::Chrome();
	caretcolour.desired = ColourDesired(0, 0, 0);
	showCaretLineBackground = false;
	caretLineBackground.desired = ColourDesired(0xff, 0xff, 0);
	edgecolour.desired = ColourDesired(0xc0, 0xc0, 0xc0);
	edgeState = EDGE_NONE;
	caretWidth = 1;
	someStylesProtected = false;

	hotspotForegroundSet = false;
	hotspotForeground.desired = ColourDesired(0, 0, 0xff);
	hotspotBackgroundSet = false;
	hotspotBackground.desired = ColourDesired(0xff, 0xff, 0xff);
	hotspotUnderline = true;

	leftMarginWidth = 1;
	rightMarginWidth = 1;
	ms[0].symbol = false;
	ms[0].width = 0;
	ms[0].mask = 0;
	ms[1].symbol = true;
	ms[1].width = 16;
	ms[1].mask = ~SC_MASK_FOLDERS;
	ms[2].symbol = true;
	ms[2].width = 0;
	ms[2].mask = 0;
	fixedColumnWidth = leftMarginWidth;
	symbolMargin = false;
	maskInLine = 0xffffffff;
	for (int margin=0; margin < margins; margin++) {
		fixedColumnWidth += ms[margin].width;
		symbolMargin = symbolMargin || ms[margin].symbol;
		if (ms[margin].width > 0)
			maskInLine &= ~ms[margin].mask;
	}
	zoomLevel = 0;
	viewWhitespace = wsInvisible;
	viewIndentationGuides = false;
	viewEOL = false;
	showMarkedLines = true;
}

void ViewStyle::RefreshColourPalette(Palette &pal, bool want) {
	unsigned int i;
	for (i=0;i<(sizeof(styles)/sizeof(styles[0]));i++) {
		pal.WantFind(styles[i].fore, want);
		pal.WantFind(styles[i].back, want);
	}
	for (i=0;i<(sizeof(indicators)/sizeof(indicators[0]));i++) {
		pal.WantFind(indicators[i].fore, want);
	}
	for (i=0;i<(sizeof(markers)/sizeof(markers[0]));i++) {
		markers[i].RefreshColourPalette(pal, want);
	}
	pal.WantFind(selforeground, want);
	pal.WantFind(selbackground, want);
	pal.WantFind(selbackground2, want);

	pal.WantFind(foldmarginColour, want);
	pal.WantFind(foldmarginHighlightColour, want);

	pal.WantFind(whitespaceForeground, want);
	pal.WantFind(whitespaceBackground, want);
	pal.WantFind(selbar, want);
	pal.WantFind(selbarlight, want);
	pal.WantFind(caretcolour, want);
	pal.WantFind(caretLineBackground, want);
	pal.WantFind(edgecolour, want);
	pal.WantFind(hotspotForeground, want);
	pal.WantFind(hotspotBackground, want);
}

void ViewStyle::Refresh(Surface &surface) {
	selbar.desired = Platform::Chrome();
	selbarlight.desired = Platform::ChromeHighlight();
	styles[STYLE_DEFAULT].Realise(surface, zoomLevel);
	maxAscent = styles[STYLE_DEFAULT].ascent;
	maxDescent = styles[STYLE_DEFAULT].descent;
	someStylesProtected = false;
	for (unsigned int i=0;i<(sizeof(styles)/sizeof(styles[0]));i++) {
		if (i != STYLE_DEFAULT) {
			styles[i].Realise(surface, zoomLevel, &styles[STYLE_DEFAULT]);
			if (maxAscent < styles[i].ascent)
				maxAscent = styles[i].ascent;
			if (maxDescent < styles[i].descent)
				maxDescent = styles[i].descent;
		}
		if (styles[i].IsProtected()) {
			someStylesProtected = true;
		}
	}

	lineHeight = maxAscent + maxDescent;
	aveCharWidth = styles[STYLE_DEFAULT].aveCharWidth;
	spaceWidth = styles[STYLE_DEFAULT].spaceWidth;

	fixedColumnWidth = leftMarginWidth;
	symbolMargin = false;
	maskInLine = 0xffffffff;
	for (int margin=0; margin < margins; margin++) {
		fixedColumnWidth += ms[margin].width;
		symbolMargin = symbolMargin || ms[margin].symbol;
		if (ms[margin].width > 0)
			maskInLine &= ~ms[margin].mask;
	}
}

void ViewStyle::ResetDefaultStyle() {
	styles[STYLE_DEFAULT].Clear(ColourDesired(0,0,0),
		ColourDesired(0xff,0xff,0xff),
	        Platform::DefaultFontSize(), fontNames.Save(Platform::DefaultFont()),
		SC_CHARSET_DEFAULT,
		false, false, false, false, Style::caseMixed, true, true, false);
}

void ViewStyle::ClearStyles() {
	// Reset all styles to be like the default style
	for (unsigned int i=0;i<(sizeof(styles)/sizeof(styles[0]));i++) {
		if (i != STYLE_DEFAULT) {
			styles[i].ClearTo(styles[STYLE_DEFAULT]);
		}
	}
	styles[STYLE_LINENUMBER].back.desired = Platform::Chrome();
}

void ViewStyle::SetStyleFontName(int styleIndex, const char *name) {
	styles[styleIndex].fontName = fontNames.Save(name);
}

bool ViewStyle::ProtectionActive() const {
    return someStylesProtected;
}
