// Scintilla source code edit control
// LineMarker.h - defines the look of a line marker in the margin 
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef LINEMARKER_H
#define LINEMARKER_H

class LineMarker {
public:
	int markType;
	ColourPair fore;
	ColourPair back;
	LineMarker() {
		markType = SC_MARK_CIRCLE;
		fore = Colour(0,0,0);
		back = Colour(0xff,0xff,0xff);
	}
	void Draw(Surface *surface, PRectangle &rc);
};

#endif
