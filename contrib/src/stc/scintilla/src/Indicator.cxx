// Scintilla source code edit control
// Indicator.cxx - defines the style of indicators which are text decorations such as underlining
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include "Platform.h"

#include "Scintilla.h"
#include "Indicator.h"

void Indicator::Draw(Surface *surface, PRectangle &rc) {
	surface->PenColour(fore.allocated);
	int ymid = (rc.bottom + rc.top) / 2;
	if (style == INDIC_SQUIGGLE) {
		surface->MoveTo(rc.left, rc.top);
		int x = rc.left + 2;
		int y = 2;
		while (x < rc.right) {
			surface->LineTo(x, rc.top + y);
			x += 2;
			y = 2 - y;
		}
		surface->LineTo(rc.right, rc.top + y);	// Finish the line
	} else if (style == INDIC_TT) {
		surface->MoveTo(rc.left, ymid);
		int x = rc.left + 5;
		while (x < rc.right) {
			surface->LineTo(x, ymid);
			surface->MoveTo(x-3, ymid);
			surface->LineTo(x-3, ymid+2);
			x++;
			surface->MoveTo(x, ymid);
			x += 5;
		}
		surface->LineTo(rc.right, ymid);	// Finish the line
		if (x - 3 <= rc.right) {
			surface->MoveTo(x-3, ymid);
			surface->LineTo(x-3, ymid+2);
		}
	} else {	// Either INDIC_PLAIN or unknown
		surface->MoveTo(rc.left, ymid);
		surface->LineTo(rc.right, ymid);
	}
}

