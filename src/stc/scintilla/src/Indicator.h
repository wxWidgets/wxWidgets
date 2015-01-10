// Scintilla source code edit control
/** @file Indicator.h
 ** Defines the style of indicators which are text decorations such as underlining.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef INDICATOR_H
#define INDICATOR_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

/**
 */
class Indicator {
public:
	int style;
	ColourDesired fore;
	bool under;
	int fillAlpha;
	int outlineAlpha;
	Indicator() : style(INDIC_PLAIN), fore(ColourDesired(0,0,0)), under(false), fillAlpha(30), outlineAlpha(50) {
	}
	Indicator(int style_, ColourDesired fore_=ColourDesired(0,0,0), bool under_=false, int fillAlpha_=30, int outlineAlpha_=50) :
		style(style_), fore(fore_), under(under_), fillAlpha(fillAlpha_), outlineAlpha(outlineAlpha_) {
	}
	void Draw(Surface *surface, const PRectangle &rc, const PRectangle &rcLine) const;
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
