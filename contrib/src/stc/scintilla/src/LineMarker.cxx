// Scintilla source code edit control
// LineMarker.cxx - defines the look of a line marker in the margin 
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include "Platform.h"

#include "Scintilla.h"
#include "LineMarker.h"

void LineMarker::Draw(Surface *surface, PRectangle &rc) {
	int minDim = Platform::Minimum(rc.Width(), rc.Height());
	minDim--;	// Ensure does not go beyond edge
	int centreX = (rc.right + rc.left) / 2;
	int centreY = (rc.bottom + rc.top) / 2;
	int dimOn2 = minDim / 2;
	int dimOn4 = minDim / 4;
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
		int armSize = dimOn2-2;
		Point xpts[] = {
    		Point(centreX - armSize, centreY),
    		Point(centreX, centreY),
    		Point(centreX, centreY - armSize),
    		Point(centreX, centreY - armSize),
    		Point(centreX, centreY),
    		Point(centreX + armSize, centreY),
    		Point(centreX + armSize, centreY),
    		Point(centreX, centreY),
    		Point(centreX, centreY + armSize),
    		Point(centreX, centreY + armSize),
    		Point(centreX, centreY),
    		Point(centreX - armSize, centreY),
		};
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
		int armSize = dimOn2-2;
		Point pts[] = {
    		Point(centreX - armSize, centreY - 1),
    		Point(centreX + armSize, centreY -1),
    		Point(centreX + armSize, centreY +1),
    		Point(centreX - armSize, centreY + 1),
		};
		Point xpts[] = {
    		Point(centreX - armSize, centreY),
    		Point(centreX + armSize, centreY),
    		Point(centreX + armSize, centreY),
    		Point(centreX - armSize, centreY),
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
