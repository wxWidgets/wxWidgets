// Scintilla source code edit control
/** @file CallTip.cxx
 ** Code for displaying call tips.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>

#include "Platform.h"

#include "Scintilla.h"
#include "CallTip.h"

CallTip::CallTip() {
	wCallTip = 0;
	inCallTipMode = false;
	posStartCallTip = 0;
	val = 0;
	startHighlight = 0;
	endHighlight = 0;

	colourBG.desired = ColourDesired(0xff, 0xff, 0xff);
	colourUnSel.desired = ColourDesired(0x80, 0x80, 0x80);
	colourSel.desired = ColourDesired(0, 0, 0x80);
	colourShade.desired = ColourDesired(0, 0, 0);
	colourLight.desired = ColourDesired(0xc0, 0xc0, 0xc0);
}

CallTip::~CallTip() {
	font.Release();
	wCallTip.Destroy();
	delete []val;
	val = 0;
}

void CallTip::RefreshColourPalette(Palette &pal, bool want) {
	pal.WantFind(colourBG, want);
	pal.WantFind(colourUnSel, want);
	pal.WantFind(colourSel, want);
	pal.WantFind(colourShade, want);
	pal.WantFind(colourLight, want);
}

void CallTip::PaintCT(Surface *surfaceWindow) {
	if (!val)
		return ;
	PRectangle rcClientPos = wCallTip.GetClientPosition();
	PRectangle rcClientSize(0, 0, rcClientPos.right - rcClientPos.left,
	                        rcClientPos.bottom - rcClientPos.top);
	PRectangle rcClient(1, 1, rcClientSize.right - 1, rcClientSize.bottom - 1);

	surfaceWindow->FillRectangle(rcClient, colourBG.allocated);
	// To make a nice small call tip window, it is only sized to fit most normal characters without accents
	int lineHeight = surfaceWindow->Height(font);
	int ascent = surfaceWindow->Ascent(font) - surfaceWindow->InternalLeading(font);

	// For each line...
	// Draw the definition in three parts: before highlight, highlighted, after highlight
	int ytext = rcClient.top + ascent + 1;
	char *chunkVal = val;
	bool moreChunks = true;
	while (moreChunks) {
		char *chunkEnd = strchr(chunkVal, '\n');
		if (chunkEnd == NULL) {
			chunkEnd = chunkVal + strlen(chunkVal);
			moreChunks = false;
		}
		int chunkOffset = chunkVal - val;
		int chunkLength = chunkEnd - chunkVal;
		int chunkEndOffset = chunkOffset + chunkLength;
		int thisStartHighlight = Platform::Maximum(startHighlight, chunkOffset);
		thisStartHighlight = Platform::Minimum(thisStartHighlight, chunkEndOffset);
		thisStartHighlight -= chunkOffset;
		int thisEndHighlight = Platform::Maximum(endHighlight, chunkOffset);
		thisEndHighlight = Platform::Minimum(thisEndHighlight, chunkEndOffset);
		thisEndHighlight -= chunkOffset;
		int x = 5;
		int xEnd = x + surfaceWindow->WidthText(font, chunkVal, thisStartHighlight);
		rcClient.left = x;
		rcClient.top = ytext - ascent - 1;
		rcClient.right = xEnd;
		surfaceWindow->DrawTextNoClip(rcClient, font, ytext,
		                        chunkVal, thisStartHighlight,
		                        colourUnSel.allocated, colourBG.allocated);
		x = xEnd;

		xEnd = x + surfaceWindow->WidthText(font, chunkVal + thisStartHighlight,
		                                    thisEndHighlight - thisStartHighlight);
		rcClient.top = ytext;
		rcClient.left = x;
		rcClient.right = xEnd;
		surfaceWindow->DrawTextNoClip(rcClient, font, ytext,
		                        chunkVal + thisStartHighlight, thisEndHighlight - thisStartHighlight,
		                        colourSel.allocated, colourBG.allocated);
		x = xEnd;

		xEnd = x + surfaceWindow->WidthText(font, chunkVal + thisEndHighlight,
		                                    chunkLength - thisEndHighlight);
		rcClient.left = x;
		rcClient.right = xEnd;
		surfaceWindow->DrawTextNoClip(rcClient, font, ytext,
		                        chunkVal + thisEndHighlight, chunkLength - thisEndHighlight,
		                        colourUnSel.allocated, colourBG.allocated);
		chunkVal = chunkEnd + 1;
		ytext += lineHeight;
	}
	// Draw a raised border around the edges of the window
	surfaceWindow->MoveTo(0, rcClientSize.bottom - 1);
	surfaceWindow->PenColour(colourShade.allocated);
	surfaceWindow->LineTo(rcClientSize.right - 1, rcClientSize.bottom - 1);
	surfaceWindow->LineTo(rcClientSize.right - 1, 0);
	surfaceWindow->PenColour(colourLight.allocated);
	surfaceWindow->LineTo(0, 0);
	surfaceWindow->LineTo(0, rcClientSize.bottom - 1);
}

PRectangle CallTip::CallTipStart(int pos, Point pt, const char *defn,
                                 const char *faceName, int size,
                                 int codePage_) {
	if (val)
		delete []val;
	val = new char[strlen(defn) + 1];
	if (!val)
		return PRectangle();
	strcpy(val, defn);
	codePage = codePage_;
	Surface *surfaceMeasure = Surface::Allocate();
	if (!surfaceMeasure)
		return PRectangle();
	surfaceMeasure->Init();
	surfaceMeasure->SetUnicodeMode(SC_CP_UTF8 == codePage);
	surfaceMeasure->SetDBCSMode(codePage);
	startHighlight = 0;
	endHighlight = 0;
	inCallTipMode = true;
	posStartCallTip = pos;
	int deviceHeight = surfaceMeasure->DeviceHeightFont(size);
	font.Create(faceName, SC_CHARSET_DEFAULT, deviceHeight, false, false);
	// Look for multiple lines in the text
	// Only support \n here - simply means container must avoid \r!
	int width = 0;
	int numLines = 1;
	const char *newline;
	const char *look = val;
	while ((newline = strchr(look, '\n')) != NULL) {
		int thisWidth = surfaceMeasure->WidthText(font, look, newline - look);
		width = Platform::Maximum(width, thisWidth);
		look = newline + 1;
		numLines++;
	}
	int lastWidth = surfaceMeasure->WidthText(font, look, static_cast<int>(strlen(look)));
	width = Platform::Maximum(width, lastWidth) + 10;
	int lineHeight = surfaceMeasure->Height(font);
	// Extra line for border and an empty line at top and bottom
	int height = lineHeight * numLines - surfaceMeasure->InternalLeading(font) + 2 + 2;
	delete surfaceMeasure;
	return PRectangle(pt.x -5, pt.y + 1, pt.x + width - 5, pt.y + 1 + height);
}

void CallTip::CallTipCancel() {
	inCallTipMode = false;
	if (wCallTip.Created()) {
		wCallTip.Destroy();
	}
}

void CallTip::SetHighlight(int start, int end) {
	// Avoid flashing by checking something has really changed
	if ((start != startHighlight) || (end != endHighlight)) {
		startHighlight = start;
		endHighlight = end;
		if (wCallTip.Created()) {
			wCallTip.InvalidateAll();
		}
	}
}
