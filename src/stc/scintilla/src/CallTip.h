// Scintilla source code edit control
/** @file CallTip.h
 ** Interface to the call tip control.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef CALLTIP_H
#define CALLTIP_H

/**
 */
class CallTip {
	int startHighlight;
	int endHighlight;
	char *val;
	Font font;
	// Private so CallTip objects can not be copied
	CallTip(const CallTip &) {}
	CallTip &operator=(const CallTip &) { return *this; }

public:
	Window wCallTip;
	Window wDraw;
	bool inCallTipMode;
	int posStartCallTip;
	ColourPair colourBG;
	ColourPair colourUnSel;
	ColourPair colourSel;
	ColourPair colourShade;
	ColourPair colourLight;
	int codePage;

	CallTip();
	~CallTip();

	/// Claim or accept palette entries for the colours required to paint a calltip.
	void RefreshColourPalette(Palette &pal, bool want);

	void PaintCT(Surface *surfaceWindow);

	/// Setup the calltip and return a rectangle of the area required.
	PRectangle CallTipStart(int pos, Point pt, const char *defn,
		const char *faceName, int size, int codePage_);

	void CallTipCancel();

	/// Set a range of characters to be displayed in a highlight style.
	/// Commonly used to highlight the current parameter.
	void SetHighlight(int start, int end);
};

#endif
