// Scintilla source code edit control
// ContractionState.cxx - manages visibility of lines for folding
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include "Platform.h"

#include "ContractionState.h"

OneLine::OneLine() {
	displayLine = 0;
	docLine = 0;
	visible = true;
	expanded = true;
}

ContractionState::ContractionState() {
	lines = 0;
	size = 0;
	linesInDoc = 1;
	linesInDisplay = 1;
	valid = false;
}

ContractionState::~ContractionState() {
	Clear();
}

void ContractionState::MakeValid() const {
	if (!valid) {
		// Could be cleverer by keeping the index of the last still valid entry 
		// rather than invalidating all.
		int linePrev = -1;
		int lineDisplay = 0;
		for (int line=0; line<linesInDoc; line++) {
			lines[line].displayLine = lineDisplay;
			if (lines[line].visible) {
				lines[lineDisplay].docLine = line;
				lineDisplay++;
			}
		}
		valid = true;
	}
}

void ContractionState::Clear() {
	delete []lines;
	lines = 0;
	size = 0;
	linesInDoc = 1;
	linesInDisplay = 1;
}

int ContractionState::LinesInDoc() const {
	return linesInDoc;
}

int ContractionState::LinesDisplayed() const {
	return linesInDisplay;
}

int ContractionState::DisplayFromDoc(int lineDoc) const {
	if (size == 0) {
		return lineDoc;
	}
	MakeValid();
	if ((lineDoc >= 0) && (lineDoc < linesInDoc)) {
		return lines[lineDoc].displayLine;
	}
	return -1;
}

int ContractionState::DocFromDisplay(int lineDisplay) const {
	if (lineDisplay <= 0)
		return 0;
	if (lineDisplay >= linesInDisplay)
		return linesInDoc-1;
	if (size == 0)
		return lineDisplay;
	MakeValid();
	return lines[lineDisplay].docLine;
}

void ContractionState::Grow(int sizeNew) {
	OneLine *linesNew = new OneLine[sizeNew];
	if (linesNew) {
		int i = 0;
		for (; i < size; i++) {
			linesNew[i] = lines[i];
		}
		for (; i < sizeNew; i++) {
			linesNew[i].displayLine = i;
		}
		delete []lines;
		lines = linesNew;
		size = sizeNew;
		valid = false;
	} else {
		Platform::DebugPrintf("No memory available\n");
		// TODO: Blow up
	}
}

void ContractionState::InsertLines(int lineDoc, int lineCount) {
	if (size == 0) {
		linesInDoc += lineCount;
		linesInDisplay += lineCount;
		return;
	}
	//Platform::DebugPrintf("InsertLine[%d] = %d\n", lineDoc);
	if ((linesInDoc + lineCount + 2) >= size) {
		Grow(linesInDoc + lineCount + growSize);
	}
	linesInDoc += lineCount;
	linesInDisplay += lineCount;
	for (int i = linesInDoc + 1; i >= lineDoc + lineCount; i--) {
		lines[i].visible = lines[i - lineCount].visible;
		lines[i].expanded = lines[i - lineCount].expanded;
	}
	for (int d=0;d<lineCount;d++) {
		lines[lineDoc+d].visible = true;	// Should inherit visibility from context ?
		lines[lineDoc+d].expanded = true;
	}
	valid = false;
}

void ContractionState::DeleteLines(int lineDoc, int lineCount) {
	if (size == 0) {
		linesInDoc -= lineCount;
		linesInDisplay -= lineCount;
		return;
	}
	int delta = 0;
	for (int d=0;d<lineCount;d++)
		if (lines[lineDoc+d].visible)
			delta--;
	for (int i = lineDoc; i < linesInDoc-lineCount; i++) {
		lines[i].visible = lines[i + lineCount].visible;
		lines[i].expanded = lines[i + lineCount].expanded;
	}
	linesInDoc -= lineCount;
	linesInDisplay += delta;
	valid = false;
}

bool ContractionState::GetVisible(int lineDoc) const {
	if (size == 0)
		return true;
	if ((lineDoc >= 0) && (lineDoc < linesInDoc)) {
		return lines[lineDoc].visible;
	} else {
		return false;
	}
}

bool ContractionState::SetVisible(int lineDocStart, int lineDocEnd, bool visible) {
	if (size == 0) {
		Grow(linesInDoc + growSize);
	}
	// TODO: modify docLine members to mirror displayLine
	int delta = 0;
	// Change lineDocs
	if ((lineDocStart <= lineDocEnd) && (lineDocStart >= 0) && (lineDocEnd < linesInDoc)) {
		for (int line=lineDocStart; line <= lineDocEnd; line++) {
			if (lines[line].visible != visible) {
				delta += visible ? 1 : -1;		
				lines[line].visible = visible;
			}
			lines[line].displayLine += delta;
		}
		if (delta != 0) {
			for (int line=lineDocEnd+1; line <= linesInDoc; line++) {
				lines[line].displayLine += delta;
			}
		}
	}
	linesInDisplay += delta;
	valid = false;
	return delta != 0;
}

bool ContractionState::GetExpanded(int lineDoc) const {
	if (size == 0) 
		return true;
	if ((lineDoc >= 0) && (lineDoc < linesInDoc)) {
		return lines[lineDoc].expanded;
	} else {
		return false;
	}
}

bool ContractionState::SetExpanded(int lineDoc, bool expanded) {
	if (size == 0) {
		Grow(linesInDoc + growSize);
	}
	if ((lineDoc >= 0) && (lineDoc < linesInDoc)) {
		if (lines[lineDoc].expanded != expanded) {
			lines[lineDoc].expanded = expanded;
			return true;
		}
	}
	return false;
}
