// Scintilla source code edit control
// CallTip.h - interface to the call tip control
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef CALLTIP_H
#define CALLTIP_H

const char callClassName[] = "CallTip";

class CallTip {
	int startHighlight;
	int endHighlight;
	char *val;
	Font font;
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
	
	CallTip();
	~CallTip();
	
	// Claim or accept palette entries for the colours required to paint a calltip
	void RefreshColourPalette(Palette &pal, bool want);
	
	void PaintCT(Surface *surfaceWindow);
	
	// Setup the calltip and return a rectangle of the area required
	PRectangle CallTipStart(int pos, Point pt, const char *defn, 
		const char *faceName, int size);
		
	void CallTipCancel();
	
	// Set a range of characters to be displayed in a highlight style.
	// Commonly used to highlight the current parameter.
	void SetHighlight(int start, int end);
};

#endif
