// Scintilla source code edit control
/** @file LineMarker.cxx
 ** Defines the look of a line marker in the margin .
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include "Platform.h"

#include "Scintilla.h"
#include "LineMarker.h"

static void DrawBox(Surface *surface, int centreX, int centreY, int armSize, Colour fore, Colour back) {
	PRectangle rc;
	rc.left = centreX - armSize;
	rc.top = centreY - armSize;
	rc.right = centreX + armSize + 1;
	rc.bottom = centreY + armSize + 1;
	surface->RectangleDraw(rc, back, fore);
}

static void DrawCircle(Surface *surface, int centreX, int centreY, int armSize, Colour fore, Colour back) {
	PRectangle rcCircle;
	rcCircle.left = centreX - armSize;
	rcCircle.top = centreY - armSize;
	rcCircle.right = centreX + armSize + 1;
	rcCircle.bottom = centreY + armSize + 1;
	surface->Ellipse(rcCircle, back, fore);
}

static void DrawPlus(Surface *surface, int centreX, int centreY, int armSize, Colour fore) {
	PRectangle rcV(centreX, centreY - armSize + 2, centreX + 1, centreY + armSize - 2 + 1);
	surface->FillRectangle(rcV, fore);
	PRectangle rcH(centreX - armSize + 2, centreY, centreX + armSize - 2 + 1, centreY+1);
	surface->FillRectangle(rcH, fore);
}

static void DrawMinus(Surface *surface, int centreX, int centreY, int armSize, Colour fore) {
	PRectangle rcH(centreX - armSize + 2, centreY, centreX + armSize - 2 + 1, centreY+1);
	surface->FillRectangle(rcH, fore);
}

void LineMarker::Draw(Surface *surface, PRectangle &rcWhole) {
	// Restrict most shapes a bit
	PRectangle rc = rcWhole;
	rc.top++;
	rc.bottom--;
	int minDim = Platform::Minimum(rc.Width(), rc.Height());
	minDim--;	// Ensure does not go beyond edge
	int centreX = (rc.right + rc.left) / 2;
	int centreY = (rc.bottom + rc.top) / 2;
	int dimOn2 = minDim / 2;
	int dimOn4 = minDim / 4;
	int blobSize = dimOn2-1;
	int armSize = dimOn2-2;
	if (rc.Width() > (rc.Height() * 2)) {
		// Wide column is line number so move to left to try to avoid overlapping number
		centreX = rc.left + dimOn2 + 1;
	}
	if (markType == SC_MARK_ROUNDRECT) {
		PRectangle rcRounded = rc;
		rcRounded.left = rc.left + 1;
		rcRounded.right = rc.right - 1;
		surface->RoundedRectangle(rcRounded, fore.allocated, back.allocated);
	} else if (markType == SC_MARK_CIRCLE) {
		PRectangle rcCircle;
		rcCircle.left = centreX - dimOn2;
		rcCircle.top = centreY - dimOn2;
		rcCircle.right = centreX + dimOn2;
		rcCircle.bottom = centreY + dimOn2;
		surface->Ellipse(rcCircle, fore.allocated, back.allocated);
	} else if (markType == SC_MARK_ARROW) {
		Point pts[] = {
    		Point(centreX - dimOn4, centreY - dimOn2),
    		Point(centreX - dimOn4, centreY + dimOn2),
    		Point(centreX + dimOn2 - dimOn4, centreY),
		};
		surface->Polygon(pts, sizeof(pts) / sizeof(pts[0]),
                 		fore.allocated, back.allocated);

	} else if (markType == SC_MARK_ARROWDOWN) {
		Point pts[] = {
    		Point(centreX - dimOn2, centreY - dimOn4),
    		Point(centreX + dimOn2, centreY - dimOn4),
    		Point(centreX, centreY + dimOn2 - dimOn4),
		};
		surface->Polygon(pts, sizeof(pts) / sizeof(pts[0]),
                 		fore.allocated, back.allocated);

	} else if (markType == SC_MARK_PLUS) {
		Point pts[] = {
    		Point(centreX - armSize, centreY - 1),
    		Point(centreX - 1, centreY - 1),
    		Point(centreX - 1, centreY - armSize),
    		Point(centreX + 1, centreY - armSize),
    		Point(centreX + 1, centreY - 1),
    		Point(centreX + armSize, centreY -1),
    		Point(centreX + armSize, centreY +1),
    		Point(centreX + 1, centreY + 1),
    		Point(centreX + 1, centreY + armSize),
    		Point(centreX - 1, centreY + armSize),
    		Point(centreX - 1, centreY + 1),
    		Point(centreX - armSize, centreY + 1),
		};
		surface->Polygon(pts, sizeof(pts) / sizeof(pts[0]),
                 		fore.allocated, back.allocated);

	} else if (markType == SC_MARK_MINUS) {
		Point pts[] = {
    		Point(centreX - armSize, centreY - 1),
    		Point(centreX + armSize, centreY -1),
    		Point(centreX + armSize, centreY +1),
    		Point(centreX - armSize, centreY + 1),
		};
		surface->Polygon(pts, sizeof(pts) / sizeof(pts[0]),
                 		fore.allocated, back.allocated);

	} else if (markType == SC_MARK_SMALLRECT) {
		PRectangle rcSmall;
		rcSmall.left = rc.left + 1;
		rcSmall.top = rc.top + 2;
		rcSmall.right = rc.right - 1;
		rcSmall.bottom = rc.bottom - 2;
		surface->RectangleDraw(rcSmall, fore.allocated, back.allocated);
		
	} else if (markType == SC_MARK_EMPTY) {
		// An invisible marker so don't draw anything
		
	} else if (markType == SC_MARK_VLINE) {
		surface->PenColour(back.allocated);
		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, rcWhole.bottom);
		
	} else if (markType == SC_MARK_LCORNER) {
		surface->PenColour(back.allocated);
		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, rc.top + dimOn2);
		surface->LineTo(rc.right - 2, rc.top + dimOn2);
		
	} else if (markType == SC_MARK_TCORNER) {
		surface->PenColour(back.allocated);
		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, rcWhole.bottom);
		surface->MoveTo(centreX, rc.top + dimOn2);
		surface->LineTo(rc.right - 2, rc.top + dimOn2);
		
	} else if (markType == SC_MARK_LCORNERCURVE) {
		surface->PenColour(back.allocated);
		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, rc.top + dimOn2-3);
		surface->LineTo(centreX+3, rc.top + dimOn2);
		surface->LineTo(rc.right - 1, rc.top + dimOn2);
		
	} else if (markType == SC_MARK_TCORNERCURVE) {
		surface->PenColour(back.allocated);
		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, rcWhole.bottom);
		
		surface->MoveTo(centreX, rc.top + dimOn2-3);
		surface->LineTo(centreX+3, rc.top + dimOn2);
		surface->LineTo(rc.right - 1, rc.top + dimOn2);
		
	} else if (markType == SC_MARK_BOXPLUS) {
		surface->PenColour(back.allocated);
		DrawBox(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		DrawPlus(surface, centreX, centreY, blobSize, back.allocated);
		
	} else if (markType == SC_MARK_BOXPLUSCONNECTED) {
		surface->PenColour(back.allocated);
		DrawBox(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		DrawPlus(surface, centreX, centreY, blobSize, back.allocated);
		
		surface->MoveTo(centreX, centreY + blobSize);
		surface->LineTo(centreX, rcWhole.bottom);
		
		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, centreY - blobSize);
		
	} else if (markType == SC_MARK_BOXMINUS) {
		surface->PenColour(back.allocated);
		DrawBox(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		DrawMinus(surface, centreX, centreY, blobSize, back.allocated);
		
		surface->MoveTo(centreX, centreY + blobSize);
		surface->LineTo(centreX, rcWhole.bottom);
		
	} else if (markType == SC_MARK_BOXMINUSCONNECTED) {
		surface->PenColour(back.allocated);
		DrawBox(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		DrawMinus(surface, centreX, centreY, blobSize, back.allocated);
		
		surface->MoveTo(centreX, centreY + blobSize);
		surface->LineTo(centreX, rcWhole.bottom);
		
		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, centreY - blobSize);
		
	} else if (markType == SC_MARK_CIRCLEPLUS) {
		DrawCircle(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		surface->PenColour(back.allocated);
		DrawPlus(surface, centreX, centreY, blobSize, back.allocated);
		
	} else if (markType == SC_MARK_CIRCLEPLUSCONNECTED) {
		DrawCircle(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		surface->PenColour(back.allocated);
		DrawPlus(surface, centreX, centreY, blobSize, back.allocated);
		
		surface->MoveTo(centreX, centreY + blobSize);
		surface->LineTo(centreX, rcWhole.bottom);
		
		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, centreY - blobSize);
		
	} else if (markType == SC_MARK_CIRCLEMINUS) {
		DrawCircle(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		surface->PenColour(back.allocated);
		DrawMinus(surface, centreX, centreY, blobSize, back.allocated);
		
		surface->MoveTo(centreX, centreY + blobSize);
		surface->LineTo(centreX, rcWhole.bottom);
		
	} else if (markType == SC_MARK_CIRCLEMINUSCONNECTED) {
		DrawCircle(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		surface->PenColour(back.allocated);
		DrawMinus(surface, centreX, centreY, blobSize, back.allocated);
		
		surface->MoveTo(centreX, centreY + blobSize);
		surface->LineTo(centreX, rcWhole.bottom);
		
		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, centreY - blobSize);
		
	} else { // SC_MARK_SHORTARROW
		Point pts[] = {
			Point(centreX, centreY + dimOn2),
			Point(centreX + dimOn2, centreY),
			Point(centreX, centreY - dimOn2),
			Point(centreX, centreY - dimOn4),
			Point(centreX - dimOn4, centreY - dimOn4),
			Point(centreX - dimOn4, centreY + dimOn4),
			Point(centreX, centreY + dimOn4),
			Point(centreX, centreY + dimOn2),
		};
		surface->Polygon(pts, sizeof(pts) / sizeof(pts[0]),
				fore.allocated, back.allocated);
	}
}
