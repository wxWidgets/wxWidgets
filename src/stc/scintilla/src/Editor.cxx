// Scintilla source code edit control
// Editor.cxx - main code for the edit control
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "Platform.h"

#include "Scintilla.h"
#include "ContractionState.h"
#include "SVector.h"
#include "CellBuffer.h"
#include "KeyMap.h"
#include "Indicator.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"
#include "Document.h"
#include "Editor.h"

Caret::Caret() :
active(true), on(true), period(500) {}

Timer::Timer() :
ticking(false), ticksToWait(0), tickerID(0) {}

Editor::Editor() {
	ctrlID = 0;

	stylesValid = false;

	hideSelection = false;
	inOverstrike = false;

	bufferedDraw = true;

	lastClickTime = 0;
	ptMouseLast.x = 0;
	ptMouseLast.y = 0;
	firstExpose = true;
	inDragDrop = false;
	dropWentOutside = false;
	posDrag = invalidPosition;
	posDrop = invalidPosition;
	selectionType = selChar;

	lastXChosen = 0;
	lineAnchor = 0;
	originalAnchorPos = 0;

	dragChars = 0;
	lenDrag = 0;
	dragIsRectangle = false;
	selType = selStream;
	xStartSelect = 0;
	xEndSelect = 0;

	caretPolicy = CARET_SLOP;
	caretSlop = 0;

	searchAnchor = 0;
	
	ucWheelScrollLines = 0;
	cWheelDelta = 0;   //wheel delta from roll

	xOffset = 0;
	xCaretMargin = 50;
	
	currentPos = 0;
	anchor = 0;

	topLine = 0;
	posTopLine = 0;
	
	needUpdateUI = true;
	braces[0]=invalidPosition;
	braces[1]=invalidPosition;
	bracesMatchStyle = STYLE_BRACEBAD;
	
	edgeState = EDGE_NONE;
	theEdge = 0;
	
	paintState = notPainting;
	
	modEventMask = SC_MODEVENTMASKALL;

	displayPopupMenu = true;
	
	pdoc = new Document();
	pdoc ->AddRef();
	pdoc->AddWatcher(this, 0);

#ifdef MACRO_SUPPORT
	recordingMacro = 0;
#endif
	foldFlags = 0;
}

Editor::~Editor() {
	pdoc->RemoveWatcher(this, 0);
	pdoc->Release();
	pdoc = 0;
	DropGraphics();

	delete []dragChars;
	dragChars = 0;
	lenDrag = 0;
}

void Editor::Finalise() {
}

void Editor::DropGraphics() {
	pixmapLine.Release();
	pixmapSelMargin.Release();
	pixmapSelPattern.Release();
}

void Editor::InvalidateStyleData() {
	stylesValid = false;
	palette.Release();
	DropGraphics();
}

void Editor::InvalidateStyleRedraw() {
	InvalidateStyleData();
	Redraw();
}

void Editor::RefreshColourPalette(Palette &pal, bool want) {
	vs.RefreshColourPalette(pal, want);
}

void Editor::RefreshStyleData() {
	if (!stylesValid) {
		stylesValid = true;
		Surface surface;
		surface.Init();
		vs.Refresh(surface);
		RefreshColourPalette(palette, true);
		palette.Allocate(wMain);
		RefreshColourPalette(palette, false);
		SetScrollBars();
	}
}

PRectangle Editor::GetClientRectangle() {
	return wDraw.GetClientPosition();
}

PRectangle Editor::GetTextRectangle() {
	PRectangle rc = GetClientRectangle();
	rc.left += vs.fixedColumnWidth;
	rc.right -= vs.rightMarginWidth;
	return rc;
}

int Editor::LinesOnScreen() {
	PRectangle rcClient = GetClientRectangle();
	int htClient = rcClient.bottom - rcClient.top;
	//Platform::DebugPrintf("lines on screen = %d\n", htClient / lineHeight + 1);
	return htClient / vs.lineHeight;
}

int Editor::LinesToScroll() {
	int retVal = LinesOnScreen() - 1;
	if (retVal < 1)
		return 1;
	else
		return retVal;
}

int Editor::MaxScrollPos() {
	//Platform::DebugPrintf("Lines %d screen = %d maxScroll = %d\n",
	//LinesTotal(), LinesOnScreen(), LinesTotal() - LinesOnScreen() + 1);
	int retVal = cs.LinesDisplayed() - LinesOnScreen();
	if (retVal < 0)
		return 0;
	else
		return retVal;
}

bool IsControlCharacter(char ch) {
	// iscntrl returns true for lots of chars > 127 which are displayable
	return ch >= 0 && ch < ' ';
}

const char *ControlCharacterString(char ch) {
	const char *reps[] = {
	    "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL",
	    "BS", "HT", "LF", "VT", "FF", "CR", "SO", "SI",
	    "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
	    "CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US"
	};
	if (ch < (sizeof(reps) / sizeof(reps[0]))) {
		return reps[ch];
	} else {
		return "BAD";
	}
}

Point Editor::LocationFromPosition(unsigned int pos) {
	RefreshStyleData();
	int line = pdoc->LineFromPosition(pos);
	int lineVisible = cs.DisplayFromDoc(line);
	//Platform::DebugPrintf("line=%d\n", line);
	Surface surface;
	surface.Init();
	Point pt;
	pt.y = (lineVisible - topLine) * vs.lineHeight;  	// + half a lineheight?
	unsigned int posLineStart = pdoc->LineStart(line);
	if ((pos - posLineStart) > LineLayout::maxLineLength) {
		// very long line so put x at arbitrary large position
		pt.x = 30000 + vs.fixedColumnWidth - xOffset;
	} else {
		LineLayout ll;
		LayoutLine(line, &surface, vs, ll);
		pt.x = ll.positions[pos - posLineStart] + vs.fixedColumnWidth - xOffset;
	}
	return pt;
}

int Editor::XFromPosition(unsigned int pos) {
	Point pt = LocationFromPosition(pos);
	return pt.x - vs.fixedColumnWidth + xOffset;
}
	
int Editor::LineFromLocation(Point pt) {
	return cs.DocFromDisplay(pt.y / vs.lineHeight + topLine);
}

void Editor::SetTopLine(int topLineNew) {
	topLine = topLineNew;
	posTopLine = pdoc->LineStart(topLine);
}

int Editor::PositionFromLocation(Point pt) {
	RefreshStyleData();
	pt.x = pt.x - vs.fixedColumnWidth + xOffset;
	int line = cs.DocFromDisplay(pt.y / vs.lineHeight + topLine);
	if (pt.y < 0) {	// Division rounds towards 0
		line = cs.DocFromDisplay((pt.y - (vs.lineHeight - 1)) / vs.lineHeight + topLine);
	}
	if (line < 0)
		return 0;
	if (line >= pdoc->LinesTotal())
		return pdoc->Length();
//Platform::DebugPrintf("Position of (%d,%d) line = %d top=%d\n", pt.x, pt.y, line, topLine);
	Surface surface;
	surface.Init();
	unsigned int posLineStart = pdoc->LineStart(line);

	LineLayout ll;
	LayoutLine(line, &surface, vs, ll);
	for (int i = 0; i < ll.numCharsInLine; i++) {
		if (pt.x < ((ll.positions[i] + ll.positions[i + 1]) / 2) || 
			ll.chars[i] == '\r' || ll.chars[i] == '\n') {
			return i + posLineStart;
		}
	}

	return ll.numCharsInLine + posLineStart;
}

int Editor::PositionFromLineX(int line, int x) {
	RefreshStyleData();
	if (line >= pdoc->LinesTotal())
		return pdoc->Length();
	//Platform::DebugPrintf("Position of (%d,%d) line = %d top=%d\n", pt.x, pt.y, line, topLine);
	Surface surface;
	surface.Init();
	unsigned int posLineStart = pdoc->LineStart(line);

	LineLayout ll;
	LayoutLine(line, &surface, vs, ll);
	for (int i = 0; i < ll.numCharsInLine; i++) {
		if (x < ((ll.positions[i] + ll.positions[i + 1]) / 2) || 
			ll.chars[i] == '\r' || ll.chars[i] == '\n') {
			return i + posLineStart;
		}
	}

	return ll.numCharsInLine + posLineStart;
}

void Editor::RedrawRect(PRectangle rc) {
	//Platform::DebugPrintf("Redraw %d %d - %d %d\n", rc.left, rc.top, rc.right, rc.bottom);
	wDraw.InvalidateRectangle(rc);
}

void Editor::Redraw() {
	//Platform::DebugPrintf("Redraw all\n");
	wDraw.InvalidateAll();
}

void Editor::RedrawSelMargin() {
	if (vs.maskInLine) {
		Redraw();
	} else {
		PRectangle rcSelMargin = GetClientRectangle();
		rcSelMargin.right = vs.fixedColumnWidth;
		wDraw.InvalidateRectangle(rcSelMargin);
	}
}

PRectangle Editor::RectangleFromRange(int start, int end) {
	int minPos = start;
	if (minPos > end)
		minPos = end;
	int maxPos = start;
	if (maxPos < end)
		maxPos = end;
	int minLine = cs.DisplayFromDoc(pdoc->LineFromPosition(minPos));
	int maxLine = cs.DisplayFromDoc(pdoc->LineFromPosition(maxPos));
	PRectangle rcClient = GetTextRectangle();
	PRectangle rc;
	rc.left = vs.fixedColumnWidth;
	rc.top = (minLine - topLine) * vs.lineHeight;
	if (rc.top < 0)
		rc.top = 0;
	rc.right = rcClient.right;
	rc.bottom = (maxLine - topLine + 1) * vs.lineHeight;
	// Ensure PRectangle is within 16 bit space
	rc.top = Platform::Clamp(rc.top, -32000, 32000);
	rc.bottom = Platform::Clamp(rc.bottom, -32000, 32000);

	return rc;
}

void Editor::InvalidateRange(int start, int end) {
	RedrawRect(RectangleFromRange(start, end));
}

int Editor::CurrentPosition() {
	return currentPos;
}

bool Editor::SelectionEmpty() {
	return anchor == currentPos;
}

int Editor::SelectionStart(int line) {
	if ((line == -1) || (selType == selStream)) {
		return Platform::Minimum(currentPos, anchor);
	} else { // selType == selRectangle
		int selStart = SelectionStart();
		int selEnd = SelectionEnd();
		int lineStart = pdoc->LineFromPosition(selStart);
		int lineEnd = pdoc->LineFromPosition(selEnd);
		if (line < lineStart || line > lineEnd) {
			return -1;
		} else {
			int minX = Platform::Minimum(xStartSelect, xEndSelect);
			//return PositionFromLineX(line, minX + vs.fixedColumnWidth - xOffset);
			return PositionFromLineX(line, minX);
		}
	} 
}

int Editor::SelectionEnd(int line) {
	if ((line == -1) || (selType == selStream)) {
		return Platform::Maximum(currentPos, anchor);
	} else { // selType == selRectangle
		int selStart = SelectionStart();
		int selEnd = SelectionEnd();
		int lineStart = pdoc->LineFromPosition(selStart);
		int lineEnd = pdoc->LineFromPosition(selEnd);
		if (line < lineStart || line > lineEnd) {
			return -1;
		} else {
			int maxX = Platform::Maximum(xStartSelect, xEndSelect);
			// measure line and return character closest to minx
			return PositionFromLineX(line, maxX);
		}
	}
}

void Editor::SetSelection(int currentPos_, int anchor_) {
	currentPos_ = pdoc->ClampPositionIntoDocument(currentPos_);
	anchor_ = pdoc->ClampPositionIntoDocument(anchor_);
	if ((currentPos != currentPos_) || (anchor != anchor_)) {
		int firstAffected = anchor;
		if (firstAffected > currentPos)
			firstAffected = currentPos;
		if (firstAffected > anchor_)
			firstAffected = anchor_;
		if (firstAffected > currentPos_)
			firstAffected = currentPos_;
		int lastAffected = anchor;
		if (lastAffected < currentPos)
			lastAffected = currentPos;
		if (lastAffected < anchor_)
			lastAffected = anchor_;
		if (lastAffected < (currentPos_ + 1))	// +1 ensures caret repainted
			lastAffected = (currentPos_ + 1);
		currentPos = currentPos_;
		anchor = anchor_;
		needUpdateUI = true;
		InvalidateRange(firstAffected, lastAffected);
	}
	ClaimSelection();
}

void Editor::SetSelection(int currentPos_) {
	currentPos_ = pdoc->ClampPositionIntoDocument(currentPos_);
	if (currentPos != currentPos_) {
		int firstAffected = anchor;
		if (firstAffected > currentPos)
			firstAffected = currentPos;
		if (firstAffected > currentPos_)
			firstAffected = currentPos_;
		int lastAffected = anchor;
		if (lastAffected < currentPos)
			lastAffected = currentPos;
		if (lastAffected < (currentPos_ + 1))	// +1 ensures caret repainted
			lastAffected = (currentPos_ + 1);
		currentPos = currentPos_;
		needUpdateUI = true;
		InvalidateRange(firstAffected, lastAffected);
	}
	ClaimSelection();
}

void Editor::SetEmptySelection(int currentPos_) {
	SetSelection(currentPos_, currentPos_);
}

int Editor::MovePositionTo(int newPos, bool extend) {
	int delta = newPos - currentPos;
	newPos = pdoc->ClampPositionIntoDocument(newPos);
	newPos = pdoc->MovePositionOutsideChar(newPos, delta);
	if (extend) {
		SetSelection(newPos);
	} else {
		SetEmptySelection(newPos);
	}
	EnsureCaretVisible();
	ShowCaretAtCurrentPosition();
	return 0;
}

int Editor::MovePositionSoVisible(int pos, int moveDir) {
	pos = pdoc->ClampPositionIntoDocument(pos);
	pos = pdoc->MovePositionOutsideChar(pos, moveDir);
	int lineDoc = pdoc->LineFromPosition(pos);
	if (cs.GetVisible(lineDoc)) {
		return pos;
	} else {
		int lineDisplay = cs.DisplayFromDoc(lineDoc);
		if (moveDir > 0) {
			lineDisplay = Platform::Clamp(lineDisplay + 1, 0, cs.LinesDisplayed());
			return pdoc->LineStart(cs.DocFromDisplay(lineDisplay));
		} else {
			// lineDisplay is already line before fold as lines in fold use display line of line before fold
			lineDisplay = Platform::Clamp(lineDisplay, 0, cs.LinesDisplayed());
			return pdoc->LineEndPosition(pdoc->LineStart(cs.DocFromDisplay(lineDisplay)));
		}
	}
}

// Choose the x position that the caret will try to stick to as it is moves up and down
void Editor::SetLastXChosen() {
	Point pt = LocationFromPosition(currentPos);
	lastXChosen = pt.x;
}

void Editor::ScrollTo(int line) {
	int topLineNew = Platform::Clamp(line, 0, MaxScrollPos());
	if (topLineNew != topLine) {
		// Try to optimise small scrolls
		int linesToMove = topLine - topLineNew;
		SetTopLine(topLineNew);
		ShowCaretAtCurrentPosition();
		// Perform redraw rather than scroll if many lines would be redrawn anyway.
		if (abs(linesToMove) <= 10) {
			ScrollText(linesToMove);
		} else {
			Redraw();
		}
		SetVerticalScrollPos();
	}
}

void Editor::ScrollText(int linesToMove) {
	//Platform::DebugPrintf("Editor::ScrollText %d\n", linesToMove);
	Redraw();
}

void Editor::HorizontalScrollTo(int xPos) {
	//Platform::DebugPrintf("HorizontalScroll %d\n", xPos);
	xOffset = xPos;
	if (xOffset < 0)
		xOffset = 0;
	SetHorizontalScrollPos();
	Redraw();
}

void Editor::EnsureCaretVisible(bool useMargin) {
	//Platform::DebugPrintf("EnsureCaretVisible %d\n", xOffset);
	PRectangle rcClient = GetTextRectangle();
	int posCaret = currentPos;
	if (posDrag >= 0)
		posCaret = posDrag;
	Point pt = LocationFromPosition(posCaret);
	Point ptEOL = LocationFromPosition(pdoc->LineEndPosition(posCaret));
	Point ptBottomCaret = pt;
	int lineCaret = cs.DisplayFromDoc(pdoc->LineFromPosition(posCaret));
	ptBottomCaret.y += vs.lineHeight - 1;

	// Ensure the caret is reasonably visible in context.
	int xMargin = Platform::Clamp(xCaretMargin, 2, Platform::Maximum(rcClient.Width() - 10, 4) / 2);
	if (!useMargin)
		xMargin = 2;
	
	// Ensure certain amount of text visible on both sides of caretSo move if caret just on edge
	rcClient.left = rcClient.left + xMargin;
	rcClient.right = rcClient.right - xMargin;
	
	if (!rcClient.Contains(pt) || !rcClient.Contains(ptBottomCaret) || (caretPolicy & CARET_STRICT)) {
		//Platform::DebugPrintf("EnsureCaretVisible move, (%d,%d) (%d,%d)\n", pt.x, pt.y, rcClient.left, rcClient.right);
		// It should be possible to scroll the window to show the caret,
		// but this fails to remove the caret on GTK+
		if (caretPolicy & CARET_SLOP) {
			if ((topLine > lineCaret) || ((caretPolicy & CARET_STRICT) && (topLine + caretSlop > lineCaret))) {
				SetTopLine(Platform::Clamp(lineCaret - caretSlop, 0, MaxScrollPos()));
				SetVerticalScrollPos();
				Redraw();
			} else if ((lineCaret > topLine + LinesOnScreen() - 1) || 
				((caretPolicy & CARET_STRICT) && (lineCaret > topLine + LinesOnScreen() - 1 - caretSlop))) {
				SetTopLine(Platform::Clamp(lineCaret - LinesOnScreen() + 1 + caretSlop, 0, MaxScrollPos()));
				SetVerticalScrollPos();
				Redraw();
			}
		} else {
			if ((topLine > lineCaret) || (lineCaret > topLine + LinesOnScreen() - 1) || (caretPolicy & CARET_STRICT)) {
				SetTopLine(Platform::Clamp(lineCaret - LinesOnScreen() / 2 + 1, 0, MaxScrollPos()));
				SetVerticalScrollPos();
				Redraw();
			}
		}
		int xOffsetNew = xOffset;
		if (pt.x < rcClient.left) {
			xOffsetNew = xOffset - (rcClient.left - pt.x);
		} else if (pt.x >= rcClient.right) {
			xOffsetNew = xOffset + (pt.x - rcClient.right);
			int xOffsetEOL = xOffset + (ptEOL.x - rcClient.right) - xMargin + 2;
			//Platform::DebugPrintf("Margin %d %d\n", xOffsetNew, xOffsetEOL);
			// Ensure don't scroll out into empty space
			if (xOffsetNew > xOffsetEOL)
				xOffsetNew = xOffsetEOL;
		}
		if (xOffsetNew < 0)
			xOffsetNew = 0;
		if (xOffset != xOffsetNew) {
			xOffset = xOffsetNew;
			SetHorizontalScrollPos();
			Redraw();
		}
	}
}

void Editor::ShowCaretAtCurrentPosition() {
	if (!wMain.HasFocus()) {
		caret.active = false;
		caret.on = false;
		return;
	}
	caret.active = true;
	caret.on = true;
	SetTicking(true);
}

void Editor::DropCaret() {
	caret.active = false;
	InvalidateCaret();
}

void Editor::InvalidateCaret() {
	if (posDrag >= 0)
		InvalidateRange(posDrag, posDrag + 1);
	else
		InvalidateRange(currentPos, currentPos + 1);
}

void Editor::PaintSelMargin(Surface *surfWindow, PRectangle &rc) {
	if (vs.fixedColumnWidth == 0)
		return;

	PRectangle rcMargin = GetClientRectangle();
	rcMargin.right = vs.fixedColumnWidth;

	if (!rc.Intersects(rcMargin))
		return;

	Surface *surface;
	if (bufferedDraw) {
		surface = &pixmapSelMargin;
	} else {
		surface = surfWindow;
	}

	PRectangle rcSelMargin = rcMargin;
	rcSelMargin.right = rcMargin.left;

	for (int margin=0; margin < vs.margins; margin++) {
		if (vs.ms[margin].width > 0) {
	
			rcSelMargin.left = rcSelMargin.right;
			rcSelMargin.right = rcSelMargin.left + vs.ms[margin].width;

			if (vs.ms[margin].symbol) {
				/* alternate scheme:
				if (vs.ms[margin].mask & SC_MASK_FOLDERS)
					surface->FillRectangle(rcSelMargin, vs.styles[STYLE_DEFAULT].back.allocated); 
				else
					// Required because of special way brush is created for selection margin
					surface->FillRectangle(rcSelMargin, pixmapSelPattern); 
				*/
				if (vs.ms[margin].mask & SC_MASK_FOLDERS)
					// Required because of special way brush is created for selection margin
					surface->FillRectangle(rcSelMargin, pixmapSelPattern); 
				else
					surface->FillRectangle(rcSelMargin, vs.styles[STYLE_LINENUMBER].back.allocated); 
			} else {
				surface->FillRectangle(rcSelMargin, vs.styles[STYLE_LINENUMBER].back.allocated);
			}
			
			int visibleLine = topLine;
			int line = cs.DocFromDisplay(visibleLine);
			int yposScreen = 0;

			while ((visibleLine < cs.LinesDisplayed()) && yposScreen < rcMargin.bottom) {
				int marks = pdoc->GetMark(line);
				if (pdoc->GetLevel(line) & SC_FOLDLEVELHEADERFLAG) {
					if (cs.GetExpanded(line)) {
						marks |= 1 << SC_MARKNUM_FOLDEROPEN;
					} else {
						marks |= 1 << SC_MARKNUM_FOLDER;
					}
				}
				marks &= vs.ms[margin].mask;
				PRectangle rcMarker = rcSelMargin;
				rcMarker.top = yposScreen;
				rcMarker.bottom = yposScreen + vs.lineHeight;
				if (!vs.ms[margin].symbol) {
					char number[100];
					number[0] = '\0';
					sprintf(number, "%d", line + 1);
					if (foldFlags & 64)
						sprintf(number, "%X", pdoc->GetLevel(line));
					int xpos = 0;
					PRectangle rcNumber=rcMarker;
					// Right justify
					int width = surface->WidthText(vs.styles[STYLE_LINENUMBER].font, number, strlen(number));
					xpos = rcNumber.right - width - 3;
					rcNumber.left = xpos;
					if ((visibleLine < cs.LinesDisplayed()) && cs.GetVisible(line)) {
						surface->DrawText(rcNumber, vs.styles[STYLE_LINENUMBER].font,
						                  rcNumber.top + vs.maxAscent, number, strlen(number),
						                  vs.styles[STYLE_LINENUMBER].fore.allocated, 
								  vs.styles[STYLE_LINENUMBER].back.allocated);
					}
				}
				
				if (marks) {
					for (int markBit = 0; (markBit < 32) && marks; markBit++) {
						if (marks & 1) {
							rcMarker.top++;
							rcMarker.bottom--;
							vs.markers[markBit].Draw(surface, rcMarker);
						}
						marks >>= 1;
					}
				}
				
				visibleLine++;
				line = cs.DocFromDisplay(visibleLine);
				yposScreen += vs.lineHeight;
			}
		}
	}

	PRectangle rcBlankMargin = rcMargin;
	rcBlankMargin.left = rcSelMargin.right;
	surface->FillRectangle(rcBlankMargin, vs.styles[STYLE_DEFAULT].back.allocated); 
	
	if (bufferedDraw) {
		surfWindow->Copy(rcMargin, Point(), pixmapSelMargin);
	}
}

void DrawTabArrow(Surface *surface, PRectangle rcTab, int ymid) {
	int ydiff = (rcTab.bottom - rcTab.top) / 2;
	int xhead = rcTab.right - 1 - ydiff;
	if ((rcTab.left + 2) < (rcTab.right - 1))
		surface->MoveTo(rcTab.left + 2, ymid);
	else
		surface->MoveTo(rcTab.right - 1, ymid);
	surface->LineTo(rcTab.right - 1, ymid);
	surface->LineTo(xhead, ymid - ydiff);
	surface->MoveTo(rcTab.right - 1, ymid);
	surface->LineTo(xhead, ymid + ydiff);
}

void Editor::LayoutLine(int line, Surface *surface, ViewStyle &vstyle, LineLayout &ll) {
	int numCharsInLine = 0;
	int posLineStart = pdoc->LineStart(line);
	int posLineEnd = pdoc->LineStart(line + 1);
	Font &ctrlCharsFont = vstyle.styles[STYLE_CONTROLCHAR].font;
	char styleByte = 0;
	int styleMask = pdoc->stylingBitsMask;
	for (int charInDoc = posLineStart; 
		charInDoc < posLineEnd && numCharsInLine < LineLayout::maxLineLength - 1; 
		charInDoc++) {
		char chDoc = pdoc->CharAt(charInDoc);
		styleByte = pdoc->StyleAt(charInDoc);
		if (vstyle.viewEOL || ((chDoc != '\r') && (chDoc != '\n'))) {
			ll.chars[numCharsInLine] = chDoc;
			ll.styles[numCharsInLine] = styleByte & styleMask;
			ll.indicators[numCharsInLine] = styleByte & ~styleMask;
			numCharsInLine++;
		}
	}
	ll.chars[numCharsInLine] = 0;
	ll.styles[numCharsInLine] = styleByte;	// For eolFilled
	ll.indicators[numCharsInLine] = 0;

	// Layout the line, determining the position of each character
	int startseg = 0;
	int startsegx = 0;
	ll.positions[0] = 0;
	unsigned int tabWidth = vstyle.spaceWidth * pdoc->tabInChars;
	
	for (int charInLine = 0; charInLine < numCharsInLine; charInLine++) {
		if ((ll.styles[charInLine] != ll.styles[charInLine + 1]) ||
		        IsControlCharacter(ll.chars[charInLine]) || IsControlCharacter(ll.chars[charInLine + 1])) {
			ll.positions[startseg] = 0;
			if (IsControlCharacter(ll.chars[charInLine])) {
				if (ll.chars[charInLine] == '\t') {
					ll.positions[charInLine + 1] = ((((startsegx + 2) /
					                                   tabWidth) + 1) * tabWidth) - startsegx;
				} else {
					const char *ctrlChar = ControlCharacterString(ll.chars[charInLine]);
					// +3 For a blank on front and rounded edge each side:
					ll.positions[charInLine + 1] = surface->WidthText(ctrlCharsFont, ctrlChar, strlen(ctrlChar)) + 3;
				}
			} else {
				surface->MeasureWidths(vstyle.styles[ll.styles[charInLine]].font, ll.chars + startseg, 
					charInLine - startseg + 1, ll.positions + startseg + 1);
			}
			for (int posToIncrease = startseg; posToIncrease <= (charInLine + 1); posToIncrease++) {
				ll.positions[posToIncrease] += startsegx;
			}
			startsegx = ll.positions[charInLine + 1];
			startseg = charInLine + 1;
		}
	}
	ll.numCharsInLine = numCharsInLine;
}

void Editor::DrawLine(Surface *surface, ViewStyle &vsDraw, int line, int xStart, PRectangle rcLine, LineLayout &ll) {
	
	PRectangle rcSegment = rcLine;
	
	// Using one font for all control characters so it can be controlled independently to ensure
	// the box goes around the characters tightly. Seems to be no way to work out what height
	// is taken by an individual character - internal leading gives varying results.
	Font &ctrlCharsFont = vsDraw.styles[STYLE_CONTROLCHAR].font;

	int marks = 0;
	Colour markBack = Colour(0, 0, 0);
	if (vsDraw.maskInLine) {
		marks = pdoc->GetMark(line) & vsDraw.maskInLine;
		if (marks) {
			for (int markBit = 0; (markBit < 32) && marks; markBit++) {
				if (marks & 1) {
					markBack = vsDraw.markers[markBit].back.allocated;
				}
				marks >>= 1;
			}
		}
		marks = pdoc->GetMark(line) & vsDraw.maskInLine;
	}

	int posLineStart = pdoc->LineStart(line);
	int posLineEnd = pdoc->LineStart(line + 1);

	int selStart = SelectionStart(line);
	int selEnd = SelectionEnd(line);

	int styleMask = pdoc->stylingBitsMask;
	int startseg = 0;
	for (int i = 0; i < ll.numCharsInLine; i++) {

		int iDoc = i + posLineStart;
		// If there is the end of a style run for any reason
		if ((ll.styles[i] != ll.styles[i + 1]) ||
		        IsControlCharacter(ll.chars[i]) || IsControlCharacter(ll.chars[i + 1]) ||
		        ((selStart != selEnd) && ((iDoc + 1 == selStart) || (iDoc + 1 == selEnd))) ||
				(i == (theEdge-1))) {
			int styleMain = ll.styles[i];
			Colour textBack = vsDraw.styles[styleMain].back.allocated;
			Colour textFore = vsDraw.styles[styleMain].fore.allocated;
			Font &textFont = vsDraw.styles[styleMain].font;
			bool inSelection = (iDoc >= selStart) && (iDoc < selEnd) && (selStart != selEnd);
			if (inSelection && !hideSelection) {
				if (vsDraw.selbackset)
					textBack = vsDraw.selbackground.allocated;
				if (vsDraw.selforeset)
					textFore = vsDraw.selforeground.allocated;
			} else {
				if (marks)
					textBack = markBack;
				if ((edgeState == EDGE_BACKGROUND) && (i >= theEdge) && (ll.chars[i] != '\n') && (ll.chars[i] != '\r'))
					textBack = vs.edgecolour.allocated;
			}
			// Manage tab display
			if (ll.chars[i] == '\t') {
				rcSegment.left = ll.positions[i] + xStart;
				rcSegment.right = ll.positions[i + 1] + xStart;
				surface->FillRectangle(rcSegment, textBack);
				if (vsDraw.viewWhitespace) {
					surface->PenColour(textFore);
					PRectangle rcTab(rcSegment.left + 1, rcSegment.top + 4,
					                 rcSegment.right - 1, rcSegment.bottom - vsDraw.maxDescent);
					DrawTabArrow(surface, rcTab, rcSegment.top + vsDraw.lineHeight / 2);
				}
			// Manage control character display
			} else if (IsControlCharacter(ll.chars[i])) {
				const char *ctrlChar = ControlCharacterString(ll.chars[i]);
				rcSegment.left = ll.positions[i] + xStart;
				rcSegment.right = ll.positions[i + 1] + xStart;
				surface->FillRectangle(rcSegment, textBack);
				int normalCharHeight = surface->Ascent(ctrlCharsFont) -
				                       surface->InternalLeading(ctrlCharsFont);
				PRectangle rcCChar = rcSegment;
				rcCChar.left = rcCChar.left + 1;
				rcCChar.top = rcSegment.top + vsDraw.maxAscent - normalCharHeight;
				rcCChar.bottom = rcSegment.top + vsDraw.maxAscent + 1;
				PRectangle rcCentral = rcCChar;
				rcCentral.top++;
				rcCentral.bottom--;
				surface->FillRectangle(rcCentral, textFore);
				PRectangle rcChar = rcCChar;
				rcChar.left++;
				rcChar.right--;
				surface->DrawTextClipped(rcChar, ctrlCharsFont,
				                        rcSegment.top + vsDraw.maxAscent, ctrlChar, strlen(ctrlChar), 
							textBack, textFore);
			// Manage normal display
			} else {
				rcSegment.left = ll.positions[startseg] + xStart;
				rcSegment.right = ll.positions[i + 1] + xStart;
				// Only try to draw if really visible - enhances performance by not calling environment to 
				// draw strings that are completely past the right side of the window.
				if (rcSegment.left <= rcLine.right) {
					surface->DrawText(rcSegment, textFont,
					                  rcSegment.top + vsDraw.maxAscent, ll.chars + startseg,
					                  i - startseg + 1, textFore, textBack);
					if (vsDraw.viewWhitespace) {
						for (int cpos = 0; cpos <= i - startseg; cpos++) {
							if (ll.chars[cpos + startseg] == ' ') {
								int xmid = (ll.positions[cpos + startseg] + ll.positions[cpos + startseg + 1]) / 2;
								PRectangle rcDot(xmid + xStart, rcSegment.top + vsDraw.lineHeight / 2, 0, 0);
								rcDot.right = rcDot.left + 1;
								rcDot.bottom = rcDot.top + 1;
								surface->FillRectangle(rcDot, textFore);
							}
						}
					}
				}
			}
			startseg = i + 1;
		}
	}

	// Draw indicators
	int indStart[INDIC_MAX + 1] = {0};
	for (int indica = 0; indica <= INDIC_MAX; indica++)
		indStart[indica] = 0;

	for (int indicPos = 0; indicPos <= ll.numCharsInLine; indicPos++) {
		if (ll.indicators[indicPos] != ll.indicators[indicPos + 1]) {
			int mask = 1 << pdoc->stylingBits;
			for (int indicnum = 0; mask <= 0x100; indicnum++) {
				if ((ll.indicators[indicPos + 1] & mask) && !(ll.indicators[indicPos] & mask)) {
					indStart[indicnum] = ll.positions[indicPos + 1];
				}
				if (!(ll.indicators[indicPos + 1] & mask) && (ll.indicators[indicPos] & mask)) {
					PRectangle rcIndic(
					    indStart[indicnum] + xStart,
					    rcLine.top + vsDraw.maxAscent,
					    ll.positions[indicPos + 1] + xStart,
					    rcLine.top + vsDraw.maxAscent + 3);
					vsDraw.indicators[indicnum].Draw(surface, rcIndic);
				}
				mask = mask << 1;
			}
		}
	}
	// End of the drawing of the current line

	// Fill in a PRectangle representing the end of line characters
	int xEol = ll.positions[ll.numCharsInLine];
	rcSegment.left = xEol + xStart;
	rcSegment.right = xEol + vsDraw.aveCharWidth + xStart;
	bool eolInSelection = (posLineEnd > selStart) && (posLineEnd <= selEnd) && (selStart != selEnd);
	if (eolInSelection && !hideSelection && vsDraw.selbackset && (line < pdoc->LinesTotal()-1)) {
		surface->FillRectangle(rcSegment, vsDraw.selbackground.allocated);
	} else if (marks) {
		surface->FillRectangle(rcSegment, markBack);
	} else {
		surface->FillRectangle(rcSegment, vsDraw.styles[ll.styles[ll.numCharsInLine] & styleMask].back.allocated);
	}

	rcSegment.left = xEol + vsDraw.aveCharWidth + xStart;
	rcSegment.right = rcLine.right;
	if (marks) {
		surface->FillRectangle(rcSegment, markBack);
	} else if (vsDraw.styles[ll.styles[ll.numCharsInLine] & styleMask].eolFilled) {
		surface->FillRectangle(rcSegment, vsDraw.styles[ll.styles[ll.numCharsInLine] & styleMask].back.allocated);
	} else {
		surface->FillRectangle(rcSegment, vsDraw.styles[STYLE_DEFAULT].back.allocated);
	}
	
	if (edgeState == EDGE_LINE) {
		int edgeX = theEdge * vsDraw.spaceWidth;
		rcSegment.left = edgeX + xStart;
		rcSegment.right = rcSegment.left + 1;
		surface->FillRectangle(rcSegment, vs.edgecolour.allocated);
	}
}

void Editor::Paint(Surface *surfaceWindow, PRectangle rcArea) {
	//Platform::DebugPrintf("Paint %d %d - %d %d\n", rcArea.left, rcArea.top, rcArea.right, rcArea.bottom);
	RefreshStyleData();

	PRectangle rcClient = GetClientRectangle();
	//Platform::DebugPrintf("Client: (%3d,%3d) ... (%3d,%3d)   %d\n",
	//	rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

	if (!pixmapSelPattern.Initialised()) {
		pixmapSelPattern.InitPixMap(8, 8, surfaceWindow);
		// This complex procedure is to reproduce the checker board dithered pattern used by windows
		// for scroll bars and Visual Studio for its selection margin. The colour of this pattern is half
		// way between the chrome colour and the chrome highlight colour making a nice transition
		// between the window chrome and the content area. And it works in low colour depths.
		PRectangle rcPattern(0, 0, 8, 8);
		if (vs.selbarlight.desired == Colour(0xff, 0xff, 0xff)) {
			pixmapSelPattern.FillRectangle(rcPattern, vs.selbar.allocated);
			pixmapSelPattern.PenColour(vs.selbarlight.allocated);
			for (int stripe = 0; stripe < 8; stripe++) {
				pixmapSelPattern.MoveTo(0, stripe * 2);
				pixmapSelPattern.LineTo(8, stripe * 2 - 8);
			}
		} else {
			// User has chosen an unusual chrome colour scheme so just use the highlight edge colour.
			pixmapSelPattern.FillRectangle(rcPattern, vs.selbarlight.allocated);
		}
	}

	if (bufferedDraw) {
		if (!pixmapLine.Initialised()) {
			pixmapLine.InitPixMap(rcClient.Width(), rcClient.Height(),
			                      surfaceWindow);
			pixmapSelMargin.InitPixMap(vs.fixedColumnWidth,
			                           rcClient.Height(), surfaceWindow);
		}
	}

	surfaceWindow->SetPalette(&palette, true);
	pixmapLine.SetPalette(&palette, !wMain.HasFocus());

	//Platform::DebugPrintf("Paint: (%3d,%3d) ... (%3d,%3d)\n",
	//	rcArea.left, rcArea.top, rcArea.right, rcArea.bottom);

	int screenLinePaintFirst = rcArea.top / vs.lineHeight;
	// The area to be painted plus one extra line is styled.
	// The extra line is to determine when a style change, such as statrting a comment flows on to other lines.
	int lineStyleLast = topLine + (rcArea.bottom-1) / vs.lineHeight + 1;
	//Platform::DebugPrintf("Paint lines = %d .. %d\n", topLine + screenLinePaintFirst, lineStyleLast);
	int endPosPaint = pdoc->Length();
	if (lineStyleLast < cs.LinesDisplayed())
		endPosPaint = pdoc->LineStart(cs.DocFromDisplay(lineStyleLast + 1));

	int xStart = vs.fixedColumnWidth - xOffset;
	int ypos = 0;
	if (!bufferedDraw)
		ypos += screenLinePaintFirst * vs.lineHeight;
	int yposScreen = screenLinePaintFirst * vs.lineHeight;

	// Ensure we are styled as far as we are painting.
	pdoc->EnsureStyledTo(endPosPaint);

	if (needUpdateUI) {
		NotifyUpdateUI();
		needUpdateUI = false;
	}
	
	PaintSelMargin(surfaceWindow, rcArea);

	PRectangle rcRightMargin = rcClient;
	rcRightMargin.left = rcRightMargin.right - vs.rightMarginWidth;
	if (rcArea.Intersects(rcRightMargin)) {
		surfaceWindow->FillRectangle(rcRightMargin, vs.styles[STYLE_DEFAULT].back.allocated); 
	}

	if (paintState == paintAbandoned) {
		// Either styling or NotifyUpdateUI noticed that painting is needed
		// outside the current painting rectangle
		//Platform::DebugPrintf("Abandoning paint\n");
		return;
	}
	//Platform::DebugPrintf("start display %d, offset = %d\n", pdoc->Length(), xOffset);

	Surface *surface = 0;
	if (rcArea.right > vs.fixedColumnWidth) {

		if (bufferedDraw) {
			surface = &pixmapLine;
		} else {
			surface = surfaceWindow;
		}

		int visibleLine = topLine + screenLinePaintFirst;
		int line = cs.DocFromDisplay(visibleLine);

		int posCaret = currentPos;
		if (posDrag >= 0)
			posCaret = posDrag;
		int lineCaret = pdoc->LineFromPosition(posCaret);

		// Remove selection margin from drawing area so text will not be drawn
		// on it in unbuffered mode.
		PRectangle rcTextArea = rcClient;
		rcTextArea.left = vs.fixedColumnWidth;
		rcTextArea.right -= vs.rightMarginWidth;
		surfaceWindow->SetClip(rcTextArea);
		//GTimer *tim=g_timer_new();
		while (visibleLine <= cs.LinesDisplayed() && yposScreen < rcArea.bottom) {
			//g_timer_start(tim);
			//Platform::DebugPrintf("Painting line %d\n", line);

			int posLineStart = pdoc->LineStart(line);
			int posLineEnd = pdoc->LineStart(line + 1);
			//Platform::DebugPrintf("line %d %d - %d\n", line, posLineStart, posLineEnd);

			PRectangle rcLine = rcClient;
			rcLine.top = ypos;
			rcLine.bottom = ypos + vs.lineHeight;

			// Copy this line and its styles from the document into local arrays
			// and determine the x position at which each character starts.
			LineLayout ll;
			LayoutLine(line, surface, vs, ll);
			                                
			// Highlight the current braces if any
			if ((braces[0] >= posLineStart) && (braces[0] < posLineEnd))
				ll.styles[braces[0] - posLineStart] = bracesMatchStyle;
			if ((braces[1] >= posLineStart) && (braces[1] < posLineEnd))
				ll.styles[braces[1] - posLineStart] = bracesMatchStyle;
				
			// Draw the line
			if (cs.GetVisible(line))
				DrawLine(surface, vs, line, xStart, rcLine, ll);

			bool expanded = cs.GetExpanded(line);
			if ( (expanded && (foldFlags & 2)) || (!expanded && (foldFlags & 4)) ) {
				if (pdoc->GetLevel(line) & SC_FOLDLEVELHEADERFLAG) {
					PRectangle rcFoldLine = rcLine;
					rcFoldLine.bottom = rcFoldLine.top + 1;
					surface->FillRectangle(rcFoldLine, vs.styles[STYLE_DEFAULT].fore.allocated);
				}
			}
			if ( (expanded && (foldFlags & 8)) || (!expanded && (foldFlags & 16)) ) {
				if (pdoc->GetLevel(line) & SC_FOLDLEVELHEADERFLAG) {
					PRectangle rcFoldLine = rcLine;
					rcFoldLine.top = rcFoldLine.bottom - 1;
					surface->FillRectangle(rcFoldLine, vs.styles[STYLE_DEFAULT].fore.allocated);
				}
			}
			
			// Draw the Caret
			if (line == lineCaret) {
				int xposCaret = ll.positions[posCaret - posLineStart] + xStart;
				int widthOverstrikeCaret =
				    ll.positions[posCaret - posLineStart + 1] - ll.positions[posCaret - posLineStart];
				if (posCaret == pdoc->Length())	// At end of document
					widthOverstrikeCaret = vs.aveCharWidth;
				if ((posCaret - posLineStart) >= ll.numCharsInLine)	// At end of line
					widthOverstrikeCaret = vs.aveCharWidth;
				if (widthOverstrikeCaret < 3)	// Make sure its visible
					widthOverstrikeCaret = 3;
				if (((caret.active && caret.on) || (posDrag >= 0)) && xposCaret >= 0) {
					PRectangle rcCaret = rcLine;
					if (posDrag >= 0) {
						rcCaret.left = xposCaret;
						rcCaret.right = xposCaret + 1;
					} else {
						if (inOverstrike) {
							rcCaret.top = rcCaret.bottom - 2;
							rcCaret.left = xposCaret + 1;
							rcCaret.right = rcCaret.left + widthOverstrikeCaret - 1;
						} else {
							rcCaret.left = xposCaret;
							rcCaret.right = xposCaret + 1;
						}
					}
					surface->FillRectangle(rcCaret, vs.caretcolour.allocated);
				}
			}
			
			if (cs.GetVisible(line)) {
				if (bufferedDraw) {
					Point from(vs.fixedColumnWidth, 0);
					PRectangle rcCopyArea(vs.fixedColumnWidth, yposScreen,
					                  rcClient.right, yposScreen + vs.lineHeight);
					surfaceWindow->Copy(rcCopyArea, from, pixmapLine);
				}
			}

			if (!bufferedDraw) {
				ypos += vs.lineHeight;
			}

			yposScreen += vs.lineHeight;
			visibleLine++;
			line = cs.DocFromDisplay(visibleLine);
			//gdk_flush();
			//g_timer_stop(tim);
			//Platform::DebugPrintf("Paint [%0d] took %g\n", line, g_timer_elapsed(tim, 0));
		}
		//g_timer_destroy(tim);
		PRectangle rcBeyondEOF = rcClient;
		rcBeyondEOF.left = vs.fixedColumnWidth;
		rcBeyondEOF.right = rcBeyondEOF.right;
		rcBeyondEOF.top = (cs.LinesDisplayed() - topLine) * vs.lineHeight;
		if (rcBeyondEOF.top < rcBeyondEOF.bottom) {
			surfaceWindow->FillRectangle(rcBeyondEOF, vs.styles[STYLE_DEFAULT].back.allocated);
			if (edgeState == EDGE_LINE) {
				int edgeX = theEdge * vs.spaceWidth;
				rcBeyondEOF.left = edgeX + xStart;
				rcBeyondEOF.right = rcBeyondEOF.left + 1;
				surfaceWindow->FillRectangle(rcBeyondEOF, vs.edgecolour.allocated);
			}
		}
	}
}

// Space (3 space characters) between line numbers and text when printing.
#define lineNumberPrintSpace "   "

// This is mostly copied from the Paint method but with some things omitted
// such as the margin markers, line numbers, selection and caret
// Should be merged back into a combined Draw method.
long Editor::FormatRange(bool draw, FORMATRANGE *pfr) {
	if (!pfr)
		return 0;

	Surface *surface = new Surface();
	surface->Init(pfr->hdc);
	Surface *surfaceMeasure = new Surface();
	surfaceMeasure->Init(pfr->hdcTarget);
	
	ViewStyle vsPrint(vs);
	
	// Modify the view style for printing as do not normally want any of the transient features to be printed
	// Printing supports only the line number margin.
	int lineNumberIndex = -1;
	for (int margin=0; margin < ViewStyle::margins; margin++) {
		if ((!vsPrint.ms[margin].symbol) && (vsPrint.ms[margin].width > 0)) {
			lineNumberIndex = margin;
		} else {
			vsPrint.ms[margin].width = 0;
		}
	}
	vsPrint.showMarkedLines = false;
	vsPrint.fixedColumnWidth = 0;
	vsPrint.zoomLevel = 0;
	// Don't show the selection when printing
	vsPrint.selbackset = false;
	vsPrint.selforeset = false;
	// White background for the line numbers
	vsPrint.styles[STYLE_LINENUMBER].back.desired = Colour(0xff,0xff,0xff); 
	
	vsPrint.Refresh(*surfaceMeasure);
	// Ensure colours are set up
	vsPrint.RefreshColourPalette(palette, true);
	vsPrint.RefreshColourPalette(palette, false);
	// Determining width must hapen after fonts have been realised in Refresh
	int lineNumberWidth = 0;
	if (lineNumberIndex >= 0) {
		lineNumberWidth = surface->WidthText(vsPrint.styles[STYLE_LINENUMBER].font, 
			"9999" lineNumberPrintSpace, 4 + strlen(lineNumberPrintSpace));
		vsPrint.ms[lineNumberIndex].width = lineNumberWidth;
	}

	int linePrintStart = pdoc->LineFromPosition(pfr->chrg.cpMin);
	int linePrintLast = linePrintStart + (pfr->rc.bottom - pfr->rc.top) / vsPrint.lineHeight - 1;
	if (linePrintLast < linePrintStart)
		linePrintLast = linePrintStart;
	int linePrintMax = pdoc->LineFromPosition(pfr->chrg.cpMax - 1);
	if (linePrintLast > linePrintMax)
		linePrintLast = linePrintMax;
	//Platform::DebugPrintf("Formatting lines=[%0d,%0d,%0d] top=%0d bottom=%0d line=%0d %0d\n",
	//	linePrintStart, linePrintLast, linePrintMax, pfr->rc.top, pfr->rc.bottom, vsPrint.lineHeight,
	//	surfaceMeasure->Height(vsPrint.styles[STYLE_LINENUMBER].font));
	int endPosPrint = pdoc->Length();
	if (linePrintLast < pdoc->LinesTotal())
		endPosPrint = pdoc->LineStart(linePrintLast + 1);

	// Ensure we are styled to where we are formatting.
	pdoc->EnsureStyledTo(endPosPrint);

	int xStart = vsPrint.fixedColumnWidth + pfr->rc.left + lineNumberWidth;
	int ypos = pfr->rc.top;
	int line = linePrintStart;

	if (draw) {	// Otherwise just measuring

		while (line <= linePrintLast && ypos < pfr->rc.bottom) {

			PRectangle rcLine;
			rcLine.left = pfr->rc.left + lineNumberWidth;
			rcLine.top = ypos;
			rcLine.right = pfr->rc.right;
			rcLine.bottom = ypos + vsPrint.lineHeight;
			
			if (lineNumberWidth) {
				char number[100];
				sprintf(number, "%d" lineNumberPrintSpace, line + 1);
				PRectangle rcNumber = rcLine;
				rcNumber.right = rcNumber.left + lineNumberWidth;
				// Right justify
				rcNumber.left -=  
					surface->WidthText(vsPrint.styles[STYLE_LINENUMBER].font, number, strlen(number));
				surface->DrawText(rcNumber, vsPrint.styles[STYLE_LINENUMBER].font,
				                  ypos + vsPrint.maxAscent, number, strlen(number),
				                  vsPrint.styles[STYLE_LINENUMBER].fore.allocated, 
						  vsPrint.styles[STYLE_LINENUMBER].back.allocated);
			}

			// When printing, the hdc and hdcTarget may be the same, so
			// changing the state of surfaceMeasure may change the underlying
			// state of surface. Therefore, any cached state is discarded before 
			// using each surface.
			
			// Copy this line and its styles from the document into local arrays
			// and determine the x position at which each character starts.
			surfaceMeasure->FlushCachedState();
			LineLayout ll;
			LayoutLine(line, surfaceMeasure, vsPrint, ll);
			                                
			// Draw the line
			surface->FlushCachedState();
			DrawLine(surface, vsPrint, line, xStart, rcLine, ll);

			ypos += vsPrint.lineHeight;
			line++;
		}
	}

	delete surface;
	delete surfaceMeasure;

	return endPosPrint;
}

void Editor::SetScrollBarsTo(PRectangle) {
	RefreshStyleData();

	int nMax = cs.LinesDisplayed();
	int nPage = cs.LinesDisplayed() - MaxScrollPos() + 1;
	bool modified = ModifyScrollBars(nMax, nPage);

	// TODO: ensure always showing as many lines as possible
	// May not be, if, for example, window made larger
	if (topLine > MaxScrollPos()) {
		SetTopLine(Platform::Clamp(topLine, 0, MaxScrollPos()));
		SetVerticalScrollPos();
		Redraw();
	}
	if (modified)
		Redraw();
	//Platform::DebugPrintf("end max = %d page = %d\n", nMax, nPage);
}

void Editor::SetScrollBars() {
	PRectangle rsClient = GetClientRectangle();
	SetScrollBarsTo(rsClient);
}

void Editor::AddChar(char ch) {
	bool wasSelection = currentPos != anchor;
	ClearSelection();
	if (inOverstrike && !wasSelection) {
		if (currentPos < (pdoc->Length() - 1)) {
			if ((pdoc->CharAt(currentPos) != '\r') && (pdoc->CharAt(currentPos) != '\n')) {
				pdoc->DelChar(currentPos);
			}
		}
	}
	pdoc->InsertChar(currentPos, ch);
	SetEmptySelection(currentPos + 1);
	EnsureCaretVisible();
	SetLastXChosen();
	NotifyChar(ch);
}

void Editor::ClearSelection() {
	if (selType == selRectangle) {
		pdoc->BeginUndoAction();
		int lineStart = pdoc->LineFromPosition(SelectionStart());
		int lineEnd = pdoc->LineFromPosition(SelectionEnd());
		int startPos = SelectionStart();
		int line;
		for (line=lineStart; line <= lineEnd; line++) {
			startPos = SelectionStart(line);
			unsigned int chars = SelectionEnd(line) - startPos;
			if (0 != chars) {
				pdoc->DeleteChars(startPos, chars);
			}
		}
		SetEmptySelection(startPos);
		selType = selStream;
		pdoc->EndUndoAction();
	} else {
		int startPos = SelectionStart();
		unsigned int chars = SelectionEnd() - startPos;
		SetEmptySelection(startPos);
		if (0 != chars) {
			pdoc->DeleteChars(startPos, chars);
		}
	}
}

void Editor::ClearAll() {
	if (0 != pdoc->Length()) {
		pdoc->DeleteChars(0, pdoc->Length());
	}
	cs.Clear();
	anchor = 0;
	currentPos = 0;
	SetTopLine(0);
	SetVerticalScrollPos();
}

void Editor::Cut() {
	Copy();
	ClearSelection();
}

void Editor::PasteRectangular(int pos, const char *ptr, int len) {
	currentPos = pos;
	int insertPos = currentPos;
	int xInsert = XFromPosition(currentPos);
	int line = pdoc->LineFromPosition(currentPos);
	bool prevCr = false;
	for (int i=0; i<len; i++) {
		if ((ptr[i] == '\r') || (ptr[i] == '\n')) {
			if ((ptr[i] == '\r') || (!prevCr)) 
				line++;
			if (line >= pdoc->LinesTotal()) {
				if (pdoc->eolMode != SC_EOL_LF)
					pdoc->InsertChar(pdoc->Length(), '\r');
				if (pdoc->eolMode != SC_EOL_CR)
					pdoc->InsertChar(pdoc->Length(), '\n');
			}
			currentPos = PositionFromLineX(line, xInsert);
			prevCr = ptr[i] == '\r';
		} else {
			pdoc->InsertString(currentPos, ptr+i, 1);
			currentPos++;
			insertPos = currentPos;
			prevCr = false;
		}
	}
	SetEmptySelection(insertPos);
}

void Editor::Clear() {
	if (currentPos == anchor) {
		DelChar();
	} else {
		ClearSelection();
	}
	SetEmptySelection(currentPos);
}

void Editor::SelectAll() {
	SetSelection(0, pdoc->Length());
	Redraw();
}

void Editor::Undo() {
	if (pdoc->CanUndo()) {
		int newPos = pdoc->Undo();
		SetEmptySelection(newPos);
		EnsureCaretVisible();
	}
}

void Editor::Redo() {
	if (pdoc->CanRedo()) {
		int newPos = pdoc->Redo();
		SetEmptySelection(newPos);
		EnsureCaretVisible();
	}
}

void Editor::DelChar() {
	pdoc->DelChar(currentPos);
}

void Editor::DelCharBack() {
	if (currentPos == anchor) {
		int newPos = pdoc->DelCharBack(currentPos);
		SetEmptySelection(newPos);
	} else {
		ClearSelection();
		SetEmptySelection(currentPos);
	}
}

void Editor::NotifyFocus(bool) {
}

void Editor::NotifyStyleNeeded(int endStyleNeeded) {
	SCNotification scn;
	scn.nmhdr.code = SCN_STYLENEEDED;
	scn.position = endStyleNeeded;
	NotifyParent(scn);
}

void Editor::NotifyStyleNeeded(Document*, void *, int endStyleNeeded) {
	NotifyStyleNeeded(endStyleNeeded);
}

void Editor::NotifyChar(char ch) {
	SCNotification scn;
	scn.nmhdr.code = SCN_CHARADDED;
	scn.ch = ch;
	NotifyParent(scn);
#ifdef MACRO_SUPPORT
	if (recordingMacro) {
		char txt[2];
		txt[0] = ch;
		txt[1] = '\0';
		NotifyMacroRecord(EM_REPLACESEL, 0, (LPARAM) txt);
	}
#endif
}

void Editor::NotifySavePoint(bool isSavePoint) {
	SCNotification scn;
	if (isSavePoint) {
		scn.nmhdr.code = SCN_SAVEPOINTREACHED;
	} else {
		scn.nmhdr.code = SCN_SAVEPOINTLEFT;
	}
	NotifyParent(scn);
}

void Editor::NotifyModifyAttempt() {
	SCNotification scn;
	scn.nmhdr.code = SCN_MODIFYATTEMPTRO;
	NotifyParent(scn);
}

void Editor::NotifyDoubleClick(Point, bool) {
	SCNotification scn;
	scn.nmhdr.code = SCN_DOUBLECLICK;
	NotifyParent(scn);
}

void Editor::NotifyUpdateUI() {
	SCNotification scn;
	scn.nmhdr.code = SCN_UPDATEUI;
	NotifyParent(scn);
}

bool Editor::NotifyMarginClick(Point pt, bool shift, bool ctrl, bool alt) {
	int marginClicked = -1;
	int x = 0;
	for (int margin=0; margin < ViewStyle::margins; margin++) {
		if ((pt.x > x) && (pt.x < x + vs.ms[margin].width))
			marginClicked = margin;
		x += vs.ms[margin].width;
	}
	if ((marginClicked >= 0) && vs.ms[marginClicked].sensitive) {
		SCNotification scn;
		scn.nmhdr.code = SCN_MARGINCLICK;
		scn.modifiers = (shift ? SCI_SHIFT : 0) | (ctrl ? SCI_CTRL : 0) |
		                (alt ? SCI_ALT : 0);
		scn.position = pdoc->LineStart(LineFromLocation(pt));
		scn.margin = marginClicked;
		NotifyParent(scn);
		return true;
	} else {
		return false;
	}
}

void Editor::NotifyNeedShown(int pos, int len) {
	SCNotification scn;
	scn.nmhdr.code = SCN_NEEDSHOWN;
	scn.position = pos;
	scn.length = len;
	NotifyParent(scn);
}

// Notifications from document
void Editor::NotifyModifyAttempt(Document*, void *) {
	//Platform::DebugPrintf("** Modify Attempt\n");
	NotifyModifyAttempt();
}

void Editor::NotifySavePoint(Document*, void *, bool atSavePoint) {
	//Platform::DebugPrintf("** Save Point %s\n", atSavePoint ? "On" : "Off");
	NotifySavePoint(atSavePoint);
}

void Editor::NotifyModified(Document*, DocModification mh, void *) {
	needUpdateUI = true;
	if (paintState == painting) {
		CheckForChangeOutsidePaint(Range(mh.position, mh.position + mh.length));
	} else if (paintState == notPainting) {
		if (mh.modificationType & SC_MOD_CHANGESTYLE) {
			if (mh.position < pdoc->LineStart(topLine)) {
				// Styling performed before this view
				Redraw();
			} else {
				InvalidateRange(mh.position, mh.position + mh.length);
			}
		} else {
			// Move selection and brace highlights
			if (mh.modificationType & SC_MOD_INSERTTEXT) {
				if (currentPos > mh.position) {
					currentPos += mh.length;
				}
				if (anchor > mh.position) {
					anchor += mh.length;
				}
				if (braces[0] > mh.position) {
					braces[0] += mh.length;
				}
				if (braces[1] > mh.position) {
					braces[1] += mh.length;
				}
			} else {	// SC_MOD_DELETETEXT
				int endPos = mh.position + mh.length;
				if (currentPos > mh.position) {
					if (currentPos > endPos) {
						currentPos -= mh.length;
					} else {
						currentPos = endPos;
					}
				}
				if (anchor > mh.position) {
					if (anchor > endPos) {
						anchor -= mh.length;
					} else {
						anchor = endPos;
					}
				}
				if (braces[0] > mh.position) {
					if (braces[0] > endPos) {
						braces[0] -= mh.length;
					} else {
						braces[0] = endPos;
					}
				}
				if (braces[1] > mh.position) {
					if (braces[1] > endPos) {
						braces[1] -= mh.length;
					} else {
						braces[1] = endPos;
					}
				}
			}
			if (mh.linesAdded != 0) {

				// Update contraction state for inserted and removed lines
				// lineOfPos should be calculated in context of state before modification, shouldn't it
				int lineOfPos = pdoc->LineFromPosition(mh.position);
				if (mh.linesAdded > 0) {
					NotifyNeedShown(mh.position, mh.length);
					cs.InsertLines(lineOfPos, mh.linesAdded);
				} else {
					cs.DeleteLines(lineOfPos, -mh.linesAdded);
				}
				// Avoid scrolling of display if change before current display
				if (mh.position < posTopLine) {
					int newTop = Platform::Clamp(topLine + mh.linesAdded, 0, MaxScrollPos());
					if (newTop != topLine) {
						SetTopLine(newTop);
						SetVerticalScrollPos();
					}
				}
				
				//Platform::DebugPrintf("** %x Doc Changed\n", this);
				// TODO: could invalidate from mh.startModification to end of screen
				//InvalidateRange(mh.position, mh.position + mh.length);
				Redraw();
			} else {
				//Platform::DebugPrintf("** %x Line Changed %d .. %d\n", this,
				//	mh.position, mh.position + mh.length);
				InvalidateRange(mh.position, mh.position + mh.length);
			}
		}
	} // else paintState == paintAbandoned so no need to do anything

	if (mh.linesAdded != 0) {
		SetScrollBars();
	}

	if (mh.modificationType & SC_MOD_CHANGEMARKER) {
		RedrawSelMargin();
	}
	
	// If client wants to see this modification
	if (mh.modificationType & modEventMask) {
		if ((mh.modificationType & SC_MOD_CHANGESTYLE) == 0) {
			// Real modification made to text of document.
			NotifyChange();	// Send EN_CHANGE
		}
		SCNotification scn;
		scn.nmhdr.code = SCN_MODIFIED;
		scn.position = mh.position;
		scn.modificationType = mh.modificationType;
		scn.text = mh.text;
		scn.length = mh.length;
		scn.linesAdded = mh.linesAdded;
		scn.line = mh.line;
		scn.foldLevelNow = mh.foldLevelNow;
		scn.foldLevelPrev = mh.foldLevelPrev;
		NotifyParent(scn);
	}
}

void Editor::NotifyDeleted(Document *document, void *userData) {
	/* Do nothing */
}

#ifdef MACRO_SUPPORT
void Editor::NotifyMacroRecord(UINT iMessage, WPARAM wParam, LPARAM lParam) {

	// Enumerates all macroable messages
	switch (iMessage) {
	case WM_CUT:
	case WM_COPY:
	case WM_PASTE:
	case WM_CLEAR:
	case EM_REPLACESEL:
	case SCI_ADDTEXT:
	case SCI_INSERTTEXT:
	case SCI_CLEARALL:
	case SCI_SELECTALL:
	case SCI_GOTOLINE:
	case SCI_GOTOPOS:
	case SCI_SEARCHANCHOR:
	case SCI_SEARCHNEXT:
	case SCI_SEARCHPREV:
	case SCI_LINEDOWN:
	case SCI_LINEDOWNEXTEND:
	case SCI_LINEUP:
	case SCI_LINEUPEXTEND:
	case SCI_CHARLEFT:
	case SCI_CHARLEFTEXTEND:
	case SCI_CHARRIGHT:
	case SCI_CHARRIGHTEXTEND:
	case SCI_WORDLEFT:
	case SCI_WORDLEFTEXTEND:
	case SCI_WORDRIGHT:
	case SCI_WORDRIGHTEXTEND:
	case SCI_HOME:
	case SCI_HOMEEXTEND:
	case SCI_LINEEND:
	case SCI_LINEENDEXTEND:
	case SCI_DOCUMENTSTART:
	case SCI_DOCUMENTSTARTEXTEND:
	case SCI_DOCUMENTEND:
	case SCI_DOCUMENTENDEXTEND:
	case SCI_PAGEUP:
	case SCI_PAGEUPEXTEND:
	case SCI_PAGEDOWN:
	case SCI_PAGEDOWNEXTEND:
	case SCI_EDITTOGGLEOVERTYPE:
	case SCI_CANCEL:
	case SCI_DELETEBACK:
	case SCI_TAB:
	case SCI_BACKTAB:
	case SCI_NEWLINE:
	case SCI_FORMFEED:
	case SCI_VCHOME:
	case SCI_VCHOMEEXTEND:
	case SCI_DELWORDLEFT:
	case SCI_DELWORDRIGHT:
	case SCI_LINECUT:
	case SCI_LINEDELETE:
	case SCI_LINETRANSPOSE:
	case SCI_LOWERCASE:
	case SCI_UPPERCASE:
		break;

	// Filter out all others (display changes, etc)
	default:
//		printf("Filtered out %ld of macro recording\n", iMessage);
		return;
	}

	// Send notification
	SCNotification scn;
	scn.nmhdr.code = SCN_MACRORECORD;
	scn.message = iMessage;
	scn.wParam = wParam;
	scn.lParam = lParam;
	NotifyParent(scn);
}
#endif

// Force scroll and keep position relative to top of window
void Editor::PageMove(int direction, bool extend) {
	Point pt = LocationFromPosition(currentPos);
	int topLineNew = Platform::Clamp(
	                 topLine + direction * LinesToScroll(), 0, MaxScrollPos());
	int newPos = PositionFromLocation(
	                 Point(lastXChosen, pt.y + direction * (vs.lineHeight * LinesToScroll())));
	if (topLineNew != topLine) {
		SetTopLine(topLineNew);
		MovePositionTo(newPos, extend);
		Redraw();
		SetVerticalScrollPos();
	} else {
		MovePositionTo(newPos, extend);
	}
}

void Editor::ChangeCaseOfSelection(bool makeUpperCase) {
	pdoc->BeginUndoAction();
	int startCurrent = currentPos;
	int startAnchor = anchor;
	if (selType == selRectangle) {
		int lineStart = pdoc->LineFromPosition(SelectionStart());
		int lineEnd = pdoc->LineFromPosition(SelectionEnd());
		for (int line=lineStart; line <= lineEnd; line++) {
			pdoc->ChangeCase(
				Range(SelectionStart(line), SelectionEnd(line)), 
				makeUpperCase);
		}
		// Would be nicer to keep the rectangular selection but this is complex
		selType = selStream;
		SetSelection(startCurrent, startCurrent);
	} else {
		pdoc->ChangeCase(Range(SelectionStart(), SelectionEnd()), 
			makeUpperCase);
		SetSelection(startCurrent, startAnchor);
	}
	pdoc->EndUndoAction();
}


void Editor::LineTranspose() {
	int line = pdoc->LineFromPosition(currentPos);
	if (line > 0) {
		int startPrev = pdoc->LineStart(line-1);
		int endPrev = pdoc->LineEnd(line-1);
		int start = pdoc->LineStart(line);
		int end = pdoc->LineEnd(line);
		int startNext = pdoc->LineStart(line+1);
		if (end < pdoc->Length()) {
			end = startNext;
			char *thisLine = CopyRange(start, end);
			pdoc->DeleteChars(start, end-start);
			pdoc->InsertString(startPrev, thisLine, end-start);
			MovePositionTo(startPrev+end-start);
			delete []thisLine;
		} else {
			// Last line so line has no line end
			char *thisLine = CopyRange(start, end);
			char *prevEnd = CopyRange(endPrev, start);
			pdoc->DeleteChars(endPrev, end-endPrev);
			pdoc->InsertString(startPrev, thisLine, end-start);
			pdoc->InsertString(startPrev + end-start, prevEnd, start-endPrev);
			MovePositionTo(startPrev + end-endPrev);
			delete []thisLine;
			delete []prevEnd;
		}
		
	}
}

int Editor::KeyCommand(UINT iMessage) {
	Point pt = LocationFromPosition(currentPos);

	switch (iMessage) {
	case SCI_LINEDOWN:
		MovePositionTo(PositionFromLocation(
		                   Point(lastXChosen, pt.y + vs.lineHeight)));
		break;
	case SCI_LINEDOWNEXTEND:
		MovePositionTo(PositionFromLocation(
		                   Point(lastXChosen, pt.y + vs.lineHeight)), true);
		break;
	case SCI_LINEUP:
		MovePositionTo(PositionFromLocation(
		                   Point(lastXChosen, pt.y - vs.lineHeight)));
		break;
	case SCI_LINEUPEXTEND:
		MovePositionTo(PositionFromLocation(
		                   Point(lastXChosen, pt.y - vs.lineHeight)), true);
		break;
	case SCI_CHARLEFT:
		if (SelectionEmpty()) {
			MovePositionTo(MovePositionSoVisible(currentPos - 1, -1));
		} else {
			MovePositionTo(SelectionStart());
		}
		SetLastXChosen();
		break;
	case SCI_CHARLEFTEXTEND:
		MovePositionTo(MovePositionSoVisible(currentPos - 1, -1), true);
		SetLastXChosen();
		break;
	case SCI_CHARRIGHT:
		if (SelectionEmpty()) {
			MovePositionTo(MovePositionSoVisible(currentPos + 1, 1));
		} else {
			MovePositionTo(SelectionEnd());
		}
		SetLastXChosen();
		break;
	case SCI_CHARRIGHTEXTEND:
		MovePositionTo(MovePositionSoVisible(currentPos + 1, 1), true);
		SetLastXChosen();
		break;
	case SCI_WORDLEFT:
		MovePositionTo(MovePositionSoVisible(pdoc->NextWordStart(currentPos, -1), -1));
		SetLastXChosen();
		break;
	case SCI_WORDLEFTEXTEND:
		MovePositionTo(MovePositionSoVisible(pdoc->NextWordStart(currentPos, -1), -1), true);
		SetLastXChosen();
		break;
	case SCI_WORDRIGHT:
		MovePositionTo(MovePositionSoVisible(pdoc->NextWordStart(currentPos, 1), 1));
		SetLastXChosen();
		break;
	case SCI_WORDRIGHTEXTEND:
		MovePositionTo(MovePositionSoVisible(pdoc->NextWordStart(currentPos, 1), 1), true);
		SetLastXChosen();
		break;
	case SCI_HOME:
		MovePositionTo(pdoc->LineStart(pdoc->LineFromPosition(currentPos)));
		SetLastXChosen();
		break;
	case SCI_HOMEEXTEND:
		MovePositionTo(pdoc->LineStart(pdoc->LineFromPosition(currentPos)), true);
		SetLastXChosen();
		break;
	case SCI_LINEEND:
		MovePositionTo(pdoc->LineEndPosition(currentPos));
		SetLastXChosen();
		break;
	case SCI_LINEENDEXTEND:
		MovePositionTo(pdoc->LineEndPosition(currentPos), true);
		SetLastXChosen();
		break;
	case SCI_DOCUMENTSTART:
		MovePositionTo(0);
		SetLastXChosen();
		break;
	case SCI_DOCUMENTSTARTEXTEND:
		MovePositionTo(0, true);
		SetLastXChosen();
		break;
	case SCI_DOCUMENTEND:
		MovePositionTo(pdoc->Length());
		SetLastXChosen();
		break;
	case SCI_DOCUMENTENDEXTEND:
		MovePositionTo(pdoc->Length(), true);
		SetLastXChosen();
		break;
	case SCI_PAGEUP:
		PageMove( -1);
		break;
	case SCI_PAGEUPEXTEND:
		PageMove( -1, true);
		break;
	case SCI_PAGEDOWN:
		PageMove(1);
		break;
	case SCI_PAGEDOWNEXTEND:
		PageMove(1, true);
		break;
	case SCI_EDITTOGGLEOVERTYPE:
		inOverstrike = !inOverstrike;
		DropCaret();
		ShowCaretAtCurrentPosition();
		break;
	case SCI_CANCEL:  	// Cancel any modes - handled in subclass
		// Also unselect text
		SetEmptySelection(currentPos);
		break;
	case SCI_DELETEBACK:
		DelCharBack();
		EnsureCaretVisible();
		break;
	case SCI_TAB:
		Indent(true);
		break;
	case SCI_BACKTAB:
		Indent(false);
		break;
	case SCI_NEWLINE:
		ClearSelection();
		if (pdoc->eolMode == SC_EOL_CRLF) {
			pdoc->InsertString(currentPos, "\r\n");
			SetEmptySelection(currentPos + 2);
			NotifyChar('\r');
			NotifyChar('\n');
		} else if (pdoc->eolMode == SC_EOL_CR) {
			pdoc->InsertChar(currentPos, '\r');
			SetEmptySelection(currentPos + 1);
			NotifyChar('\r');
		} else if (pdoc->eolMode == SC_EOL_LF) {
			pdoc->InsertChar(currentPos, '\n');
			SetEmptySelection(currentPos + 1);
			NotifyChar('\n');
		}
		SetLastXChosen();
		EnsureCaretVisible();
		break;
	case SCI_FORMFEED:
		AddChar('\f');
		break;
	case SCI_VCHOME:
		MovePositionTo(pdoc->VCHomePosition(currentPos));
		SetLastXChosen();
		break;
	case SCI_VCHOMEEXTEND:
		MovePositionTo(pdoc->VCHomePosition(currentPos), true);
		SetLastXChosen();
		break;
	case SCI_ZOOMIN:
		if (vs.zoomLevel < 20)
			vs.zoomLevel++;
		InvalidateStyleRedraw();
		break;
	case SCI_ZOOMOUT:
		if (vs.zoomLevel > -10)
			vs.zoomLevel--;
		InvalidateStyleRedraw();
		break;
	case SCI_DELWORDLEFT: {
			int startWord = pdoc->NextWordStart(currentPos, -1);
			pdoc->DeleteChars(startWord, currentPos - startWord);
			MovePositionTo(startWord);
		}
		break;
	case SCI_DELWORDRIGHT: {
			int endWord = pdoc->NextWordStart(currentPos, 1);
			pdoc->DeleteChars(currentPos, endWord - currentPos);
			MovePositionTo(currentPos);
		}
		break;
	case SCI_LINECUT: {
			int lineStart = pdoc->LineFromPosition(currentPos);
			int lineEnd = pdoc->LineFromPosition(anchor);
			if (lineStart > lineEnd) {
				int t = lineEnd;
				lineEnd = lineStart;
				lineStart = t;
			}
			int start = pdoc->LineStart(lineStart);
			int end = pdoc->LineStart(lineEnd+1);
			SetSelection(start,end);
			Cut();
		}
		break;
	case SCI_LINEDELETE: {
			int line = pdoc->LineFromPosition(currentPos);
			int start = pdoc->LineStart(line);
			int end = pdoc->LineStart(line+1);
			pdoc->DeleteChars(start, end-start);
			MovePositionTo(start);
		}
		break;
	case SCI_LINETRANSPOSE:
		LineTranspose();
		break;
	case SCI_LOWERCASE:
		ChangeCaseOfSelection(false);
		break;
	case SCI_UPPERCASE:
		ChangeCaseOfSelection(true);
		break;
	}
	return 0;
}

int Editor::KeyDefault(int, int) {
	return 0;
}

int Editor::KeyDown(int key, bool shift, bool ctrl, bool alt) {
	int modifiers = (shift ? SCI_SHIFT : 0) | (ctrl ? SCI_CTRL : 0) |
	                (alt ? SCI_ALT : 0);
	int msg = kmap.Find(key, modifiers);
	if (msg)
		return WndProc(msg, 0, 0);
	else
		return KeyDefault(key, modifiers);
}

void Editor::SetWhitespaceVisible(bool view) {
	vs.viewWhitespace = view;
}

bool Editor::GetWhitespaceVisible() {
	return vs.viewWhitespace;
}

void Editor::Indent(bool forwards) {
	//Platform::DebugPrintf("INdent %d\n", forwards);
	int lineOfAnchor = pdoc->LineFromPosition(anchor);
	int lineCurrentPos = pdoc->LineFromPosition(currentPos);
	if (lineOfAnchor == lineCurrentPos) {
		ClearSelection();
		pdoc->InsertChar(currentPos, '\t');
		//pdoc->InsertChar(currentPos++, '\t');
		SetEmptySelection(currentPos + 1);
	} else {
		int anchorPosOnLine = anchor - pdoc->LineStart(lineOfAnchor);
		int currentPosPosOnLine = currentPos - pdoc->LineStart(lineCurrentPos);
		// Multiple lines selected so indent / dedent
		int lineTopSel = Platform::Minimum(lineOfAnchor, lineCurrentPos);
		int lineBottomSel = Platform::Maximum(lineOfAnchor, lineCurrentPos);
		if (pdoc->LineStart(lineBottomSel) == anchor || pdoc->LineStart(lineBottomSel) == currentPos)
			lineBottomSel--;  	// If not selecting any characters on a line, do not indent
		pdoc->BeginUndoAction();
		pdoc->Indent(forwards, lineBottomSel, lineTopSel);
		pdoc->EndUndoAction();
		if (lineOfAnchor < lineCurrentPos) {
			if (currentPosPosOnLine == 0)
				SetSelection(pdoc->LineStart(lineCurrentPos), pdoc->LineStart(lineOfAnchor));
			else
				SetSelection(pdoc->LineStart(lineCurrentPos + 1), pdoc->LineStart(lineOfAnchor));
		} else {
			if (anchorPosOnLine == 0)
				SetSelection(pdoc->LineStart(lineCurrentPos), pdoc->LineStart(lineOfAnchor));
			else
				SetSelection(pdoc->LineStart(lineCurrentPos), pdoc->LineStart(lineOfAnchor + 1));
		}
	}
}

long Editor::FindText(UINT iMessage, WPARAM wParam, LPARAM lParam) {
	FINDTEXTEX *ft = reinterpret_cast<FINDTEXTEX *>(lParam);
	int pos = pdoc->FindText(ft->chrg.cpMin, ft->chrg.cpMax, ft->lpstrText,
			wParam & FR_MATCHCASE, wParam & FR_WHOLEWORD);
	if (pos != -1) {
		if (iMessage == EM_FINDTEXTEX) {
			ft->chrgText.cpMin = pos;
			ft->chrgText.cpMax = pos + strlen(ft->lpstrText);
		}
	}
	return pos;
}

// Relocatable search support : Searches relative to current selection 
// point and sets the selection to the found text range with 
// each search.

// Anchor following searches at current selection start:  This allows
// multiple incremental interactive searches to be macro recorded
// while still setting the selection to found text so the find/select
// operation is self-contained.
void Editor::SearchAnchor() {
	searchAnchor = SelectionStart();
}

// Find text from current search anchor:  Must call SearchAnchor first.
// Accepts both SCI_SEARCHNEXT and SCI_SEARCHPREV.
// wParam contains search modes : ORed FR_MATCHCASE and FR_WHOLEWORD.
// lParam contains the text to search for.
long Editor::SearchText(UINT iMessage, WPARAM wParam, LPARAM lParam) {
	const char *txt = reinterpret_cast<char *>(lParam);
	int pos;

	if (iMessage == SCI_SEARCHNEXT) {
		pos = pdoc->FindText(searchAnchor, pdoc->Length(), txt, 
		                     wParam & FR_MATCHCASE,
		                     wParam & FR_WHOLEWORD);
	} else {
		pos = pdoc->FindText(searchAnchor, 0, txt, 
		                     wParam & FR_MATCHCASE,
		                     wParam & FR_WHOLEWORD);
	}

	if (pos != -1) {
		SetSelection(pos, pos + strlen(txt));
	}

	return pos;
}

void Editor::GoToLine(int lineNo) {
	if (lineNo > pdoc->LinesTotal())
		lineNo = pdoc->LinesTotal();
	if (lineNo < 0)
		lineNo = 0;
	SetEmptySelection(pdoc->LineStart(lineNo));
	ShowCaretAtCurrentPosition();
	EnsureCaretVisible();
}

static bool Close(Point pt1, Point pt2) {
	if (abs(pt1.x - pt2.x) > 3)
		return false;
	if (abs(pt1.y - pt2.y) > 3)
		return false;
	return true;
}

char *Editor::CopyRange(int start, int end) {
	char *text = 0;
	if (start < end) {
		int len = end - start;
		text = new char[len + 1];
		if (text) {
			for (int i = 0; i < len; i++) {
				text[i] = pdoc->CharAt(start + i);
			}
			text[len] = '\0';
		}
	}
	return text;
}

int Editor::SelectionRangeLength() {
	if (selType == selRectangle) {
		int lineStart = pdoc->LineFromPosition(SelectionStart());
		int lineEnd = pdoc->LineFromPosition(SelectionEnd());
		int totalSize = 0;
		for (int line=lineStart; line <= lineEnd; line++) {
			totalSize += SelectionEnd(line) - SelectionStart(line) + 1;
			if (pdoc->eolMode == SC_EOL_CRLF)
				totalSize++;
		}
		return totalSize;
	} else {
		return SelectionEnd() - SelectionStart();
	}
}

char *Editor::CopySelectionRange() {
	if (selType == selRectangle) {
		char *text = 0;
		int lineStart = pdoc->LineFromPosition(SelectionStart());
		int lineEnd = pdoc->LineFromPosition(SelectionEnd());
		int totalSize = SelectionRangeLength();
		if (totalSize > 0) {
			text = new char[totalSize + 1];
			if (text) {
				int j = 0;
				for (int line=lineStart; line <= lineEnd; line++) {
					for (int i=SelectionStart(line);i<SelectionEnd(line);i++) {
						text[j++] = pdoc->CharAt(i);
					}
					if (pdoc->eolMode != SC_EOL_LF)
						text[j++] = '\r';
					if (pdoc->eolMode != SC_EOL_CR)
						text[j++] = '\n';
				}
				text[totalSize] = '\0';
			}
		}
		return text;
	} else {
		return CopyRange(SelectionStart(), SelectionEnd());
	}
}

void Editor::CopySelectionIntoDrag() {
	delete []dragChars;
	dragChars = 0;
	lenDrag = SelectionRangeLength();
	dragChars = CopySelectionRange();
	dragIsRectangle = selType == selRectangle;
	if (!dragChars) {
		lenDrag = 0;
	}
}

void Editor::SetDragPosition(int newPos) {
	if (newPos >= 0) {
		newPos = pdoc->MovePositionOutsideChar(newPos, 1);
		posDrop = newPos;
	}
	if (posDrag != newPos) {
		caret.on = true;
		SetTicking(true);
		InvalidateCaret();
		posDrag = newPos;
		InvalidateCaret();
	}
}

void Editor::StartDrag() {
	// Always handled by subclasses
	//SetMouseCapture(true);
	//wDraw.SetCursor(Window::cursorArrow);
}

void Editor::DropAt(int position, const char *value, bool moving, bool rectangular) {
	//Platform::DebugPrintf("DropAt %d\n", inDragDrop);
	if (inDragDrop)
		dropWentOutside = false;

	int positionWasInSelection = PositionInSelection(position);

	bool positionOnEdgeOfSelection = 
		(position == SelectionStart()) || (position == SelectionEnd());

	if ((!inDragDrop) || !(0 == positionWasInSelection) || 
		(positionOnEdgeOfSelection && !moving)) {

		int selStart = SelectionStart();
		int selEnd = SelectionEnd();
		
		pdoc->BeginUndoAction();

		int positionAfterDeletion = position;
		if (inDragDrop && moving) {
			// Remove dragged out text
			if (rectangular) {
				int lineStart = pdoc->LineFromPosition(SelectionStart());
				int lineEnd = pdoc->LineFromPosition(SelectionEnd());
				for (int line=lineStart; line <= lineEnd; line++) {
					int startPos = SelectionStart(line);
					int endPos = SelectionEnd(line);
					if (position >= startPos) {
						if (position > endPos) {
							positionAfterDeletion -= endPos - startPos;
						} else {
							positionAfterDeletion -= position - startPos;
						}
					}
				}
			} else {
				if (position > selStart) {
					positionAfterDeletion -= selEnd - selStart;
				}
			}
			ClearSelection();
		}
		position = positionAfterDeletion;
		
		if (rectangular) {
			PasteRectangular(position, value, strlen(value));
			pdoc->EndUndoAction();
			// Should try to select new rectangle but it may not be a rectangle now so just select the drop position
			SetSelection(position, position);	
		} else {
			position = pdoc->MovePositionOutsideChar(position, currentPos - position);
			pdoc->InsertString(position, value);
			pdoc->EndUndoAction();
			SetSelection(position + strlen(value), position);
		}
	} else if (inDragDrop) {
		SetSelection(position, position);
	}
}

static int BeforeInOrAfter(int val, int minim, int maxim) {
	if (val < minim)
		return -1;
	else if (val > maxim)
		return 1;
	else
		return 0;
}

int Editor::PositionInSelection(int pos) {
	pos = pdoc->MovePositionOutsideChar(pos, currentPos - pos);
	if (selType == selRectangle) {
		if (pos < SelectionStart())
			return -1;
		if (pos > SelectionEnd())
			return 1;
		int linePos = pdoc->LineFromPosition(pos);
		return BeforeInOrAfter(pos, SelectionStart(linePos), SelectionEnd(linePos));
	} else {
		if (currentPos > anchor) {
			return BeforeInOrAfter(pos, anchor, currentPos);
		} else if (currentPos < anchor) {
			return BeforeInOrAfter(pos, currentPos, anchor);
		}
	}
	return 1;
}

bool Editor::PointInSelection(Point pt) {
	// TODO: fix up for rectangular selection
	int pos = PositionFromLocation(pt);
	if (0 == PositionInSelection(pos)) {
		if (pos == SelectionStart()) {
			// see if just before selection
			Point locStart = LocationFromPosition(pos);
			if (pt.x < locStart.x)
				return false;
		}
		if (pos == SelectionEnd()) {
			// see if just after selection
			Point locEnd = LocationFromPosition(pos);
			if (pt.x > locEnd.x)
				return false;
		}
		return true;
	}
	return false;
}

bool Editor::PointInSelMargin(Point pt) {
	// Really means: "Point in a margin"
	if (vs.fixedColumnWidth > 0) {	// There is a margin
		PRectangle rcSelMargin = GetClientRectangle();
		rcSelMargin.right = vs.fixedColumnWidth - vs.leftMarginWidth;
		return rcSelMargin.Contains(pt);
	} else {
		return false;
	}
}

void Editor::ButtonDown(Point pt, unsigned int curTime, bool shift, bool ctrl, bool alt) {
	//Platform::DebugPrintf("Scintilla:ButtonDown %d %d = %d alt=%d\n", curTime, lastClickTime, curTime - lastClickTime, alt);
	ptMouseLast = pt;
	int newPos = PositionFromLocation(pt);
	newPos = pdoc->MovePositionOutsideChar(newPos, currentPos - newPos);
	inDragDrop = false;
	
	bool processed = NotifyMarginClick(pt, shift, ctrl, alt);
	if (processed)
		return;
		
	if (shift) {
		SetSelection(newPos);
	}
	if (((curTime - lastClickTime) < Platform::DoubleClickTime()) && Close(pt, lastClick)) {
		//Platform::DebugPrintf("Double click %d %d = %d\n", curTime, lastClickTime, curTime - lastClickTime);
		SetMouseCapture(true);
		SetEmptySelection(newPos);
		bool doubleClick = false;
		// Stop mouse button bounce changing selection type
		if (curTime != lastClickTime) {
			if (selectionType == selChar) {
				selectionType = selWord;
				doubleClick = true;
			} else if (selectionType == selWord) {
				selectionType = selLine;
			} else {
				selectionType = selChar;
				originalAnchorPos = currentPos;
			}
		}

		if (selectionType == selWord) {
			if (currentPos >= originalAnchorPos) {	// Moved forward
				SetSelection(pdoc->ExtendWordSelect(currentPos, 1),
				             pdoc->ExtendWordSelect(originalAnchorPos, -1));
			} else {	// Moved backward
				SetSelection(pdoc->ExtendWordSelect(currentPos, -1),
				             pdoc->ExtendWordSelect(originalAnchorPos, 1));
			}
		} else if (selectionType == selLine) {
			lineAnchor = LineFromLocation(pt);
			SetSelection(pdoc->LineStart(lineAnchor + 1), pdoc->LineStart(lineAnchor));
			//Platform::DebugPrintf("Triple click: %d - %d\n", anchor, currentPos);
		}
		else {
			SetEmptySelection(currentPos);
		}
		//Platform::DebugPrintf("Double click: %d - %d\n", anchor, currentPos);
		if (doubleClick)
			NotifyDoubleClick(pt, shift);
	} else {	// Single click
		if (PointInSelMargin(pt)) {
			if (ctrl) {
				SelectAll();
				lastClickTime = curTime;
				return;
			}
			lineAnchor = LineFromLocation(pt);
			// While experimenting with folding turn off line selection
			if (!shift) {
				// Single click in margin: select whole line
				SetSelection(pdoc->LineStart(lineAnchor + 1), pdoc->LineStart(lineAnchor));
			} else {
				// Single shift+click in margin: select from anchor to beginning of clicked line
				SetSelection(pdoc->LineStart(lineAnchor), anchor);
			}
			SetDragPosition(invalidPosition);
			SetMouseCapture(true);
			selectionType = selLine;
		} else {
			if (!shift) {
				inDragDrop = PointInSelection(pt);
			}
			if (inDragDrop) {
				SetMouseCapture(false);
				SetDragPosition(newPos);
				CopySelectionIntoDrag();
				StartDrag();
			} else {
				selType = alt ? selRectangle : selStream;
				xStartSelect = pt.x - vs.fixedColumnWidth + xOffset;
				xEndSelect = pt.x - vs.fixedColumnWidth + xOffset;
				SetDragPosition(invalidPosition);
				SetMouseCapture(true);
				if (!shift)
					SetEmptySelection(newPos);
				selectionType = selChar;
				originalAnchorPos = currentPos;
			}
		}
	}
	lastClickTime = curTime;
	lastXChosen = pt.x;
	ShowCaretAtCurrentPosition();
}

void Editor::ButtonMove(Point pt) {
	//Platform::DebugPrintf("Move %d %d\n", pt.x, pt.y);
	if (HaveMouseCapture()) {
		xEndSelect = pt.x - vs.fixedColumnWidth + xOffset;
		ptMouseLast = pt;
		int movePos = PositionFromLocation(pt);
		movePos = pdoc->MovePositionOutsideChar(movePos, currentPos - movePos);
		if (posDrag >= 0) {
			SetDragPosition(movePos);
		} else {
			if (selectionType == selChar) {
				SetSelection(movePos);
			} else if (selectionType == selWord) {
				// Continue selecting by word
				if (currentPos > originalAnchorPos) {	// Moved forward
					SetSelection(pdoc->ExtendWordSelect(movePos, 1),
					             pdoc->ExtendWordSelect(originalAnchorPos, -1));
				} else {	// Moved backward
					SetSelection(pdoc->ExtendWordSelect(movePos, -1),
					             pdoc->ExtendWordSelect(originalAnchorPos, 1));
				}
			} else {
				// Continue selecting by line
				int lineMove = LineFromLocation(pt);
				if (lineAnchor < lineMove) {
					SetSelection(pdoc->LineStart(lineMove + 1),
								pdoc->LineStart(lineAnchor));
				} else {
					SetSelection(pdoc->LineStart(lineMove),
								pdoc->LineStart(lineAnchor + 1));
				}
			}
		}
		EnsureCaretVisible(false);
	} else {
		if (vs.fixedColumnWidth > 0) {	// There is a margin
			if (PointInSelMargin(pt)) {
				wDraw.SetCursor(Window::cursorReverseArrow);
				return; 	// No need to test for selection
			}
		}
		// Display regular (drag) cursor over selection
		if (PointInSelection(pt))
			wDraw.SetCursor(Window::cursorArrow);
		else
			wDraw.SetCursor(Window::cursorText);
	}

}

void Editor::ButtonUp(Point pt, unsigned int curTime, bool ctrl) {
	//Platform::DebugPrintf("ButtonUp %d\n", HaveMouseCapture());
	if (HaveMouseCapture()) {
		if (PointInSelMargin(pt)) {
			wDraw.SetCursor(Window::cursorReverseArrow);
		} else {
			wDraw.SetCursor(Window::cursorText);
		}
		xEndSelect = pt.x - vs.fixedColumnWidth + xOffset;
		ptMouseLast = pt;
		SetMouseCapture(false);
		int newPos = PositionFromLocation(pt);
		newPos = pdoc->MovePositionOutsideChar(newPos, currentPos - newPos);
		if (inDragDrop) {
			int selStart = SelectionStart();
			int selEnd = SelectionEnd();
			if (selStart < selEnd) {
				if (dragChars && lenDrag) {
					if (ctrl) {
						pdoc->InsertString(newPos, dragChars, lenDrag);
						SetSelection(newPos, newPos + lenDrag);
					} else if (newPos < selStart) {
						pdoc->DeleteChars(selStart, lenDrag);
						pdoc->InsertString(newPos, dragChars, lenDrag);
						SetSelection(newPos, newPos + lenDrag);
					} else if (newPos > selEnd) {
						pdoc->DeleteChars(selStart, lenDrag);
						newPos -= lenDrag;
						pdoc->InsertString(newPos, dragChars, lenDrag);
						SetSelection(newPos, newPos + lenDrag);
					} else {
						SetEmptySelection(newPos);
					}
					delete []dragChars;
					dragChars = 0;
					lenDrag = 0;
				}
				selectionType = selChar;
			}
		} else {
			if (selectionType == selChar) {
				SetSelection(newPos);
			}
		}
		lastClickTime = curTime;
		lastClick = pt;
		lastXChosen = pt.x;
		inDragDrop = false;
		EnsureCaretVisible(false);
	}
}

// Called frequently to perform background UI including
// caret blinking and automatic scrolling.
void Editor::Tick() {
	if (HaveMouseCapture()) {
		// Auto scroll
		ButtonMove(ptMouseLast);
	}
	if (caret.period > 0) {
		timer.ticksToWait -= timer.tickSize;
		if (timer.ticksToWait <= 0) {
			caret.on = !caret.on;
			timer.ticksToWait = caret.period;
			InvalidateCaret();
		}
	}
}

static bool IsIn(int a, int minimum, int maximum) {
	return (a >= minimum) && (a <= maximum);
}

static bool IsOverlap(int mina, int maxa, int minb, int maxb) {
	return 
		IsIn(mina, minb, maxb) ||
		IsIn(maxa, minb, maxb) ||
		IsIn(minb, mina, maxa) ||
		IsIn(maxb, mina, maxa);
}

void Editor::CheckForChangeOutsidePaint(Range r) {
	if (paintState == painting && !paintingAllText) {
		//Platform::DebugPrintf("Checking range in paint %d-%d\n", r.start, r.end);
		if (!r.Valid())
			return;
			
		PRectangle rcText = GetTextRectangle();
		// Determine number of lines displayed including a possible partially displayed last line
		int linesDisplayed = (rcText.bottom - rcText.top - 1) / vs.lineHeight + 1;
		int bottomLine = topLine + linesDisplayed - 1;
		
		int lineRangeStart = cs.DisplayFromDoc(pdoc->LineFromPosition(r.start));
		int lineRangeEnd = cs.DisplayFromDoc(pdoc->LineFromPosition(r.end));
		if (!IsOverlap(topLine, bottomLine, lineRangeStart, lineRangeEnd)) {
			//Platform::DebugPrintf("No overlap (%d-%d) with window(%d-%d)\n",
			//		lineRangeStart, lineRangeEnd, topLine, bottomLine);
			return;
		}
		
		// Assert rcPaint contained within or equal to rcText
		if (rcPaint.top > rcText.top) {
 			// does range intersect rcText.top .. rcPaint.top
			int paintTopLine = ((rcPaint.top - rcText.top-1) / vs.lineHeight) + topLine;
			// paintTopLine is the top line of the paint rectangle or the line just above if that line is completely inside the paint rectangle
			if (IsOverlap(topLine, paintTopLine, lineRangeStart, lineRangeEnd)) {
				//Platform::DebugPrintf("Change (%d-%d) in top npv(%d-%d)\n", 
				//	lineRangeStart, lineRangeEnd, topLine, paintTopLine);
				paintState = paintAbandoned;
				return;
			}
		}
		if (rcPaint.bottom < rcText.bottom) {
			// does range intersect rcPaint.bottom .. rcText.bottom
			int paintBottomLine = ((rcPaint.bottom - rcText.top-1) / vs.lineHeight + 1) + topLine;
			// paintTopLine is the bottom line of the paint rectangle or the line just below if that line is completely inside the paint rectangle
			if (IsOverlap(paintBottomLine, bottomLine, lineRangeStart, lineRangeEnd)) {
				//Platform::DebugPrintf("Change (%d-%d) in bottom npv(%d-%d)\n", 
				//	lineRangeStart, lineRangeEnd, paintBottomLine, bottomLine);
				paintState = paintAbandoned;
				return;
			}
		}
	}
}

char BraceOpposite(char ch) {
	switch (ch) {
                case '(': return ')';
                case ')': return '(';
                case '[': return ']';
                case ']': return '[';
                case '{': return '}';
                case '}': return '{';
                case '<': return '>';
                case '>': return '<';
                default: return '\0';
	}
}

// TODO: should be able to extend styled region to find matching brace
// TODO: may need to make DBCS safe
// so should be moved into Document
int Editor::BraceMatch(int position, int maxReStyle) {
	char chBrace = pdoc->CharAt(position);
	char chSeek = BraceOpposite(chBrace);
	if (!chSeek)
		return - 1;
	char styBrace = pdoc->StyleAt(position) & pdoc->stylingBitsMask;
	int direction = -1;
	if (chBrace == '(' || chBrace == '[' || chBrace == '{' || chBrace == '<')
		direction = 1;
	int depth = 1;
	position = position + direction;
	while ((position >= 0) && (position < pdoc->Length())) {
		char chAtPos = pdoc->CharAt(position);
		char styAtPos = pdoc->StyleAt(position) & pdoc->stylingBitsMask;
		if ((position > pdoc->GetEndStyled()) || (styAtPos == styBrace)) {
			if (chAtPos == chBrace)
				depth++;
			if (chAtPos == chSeek)
				depth--;
			if (depth == 0)
				return position;
		}
		position = position + direction;
	}
	return - 1;
}

void Editor::SetBraceHighlight(Position pos0, Position pos1, int matchStyle) {
	if ((pos0 != braces[0]) || (pos1 != braces[1]) || (matchStyle != bracesMatchStyle)) {
		if ((braces[0] != pos0)  || (matchStyle != bracesMatchStyle)) {
			CheckForChangeOutsidePaint(Range(braces[0]));
			CheckForChangeOutsidePaint(Range(pos0));
			braces[0] = pos0;
		}
		if ((braces[1] != pos1)  || (matchStyle != bracesMatchStyle)) {
			CheckForChangeOutsidePaint(Range(braces[1]));
			CheckForChangeOutsidePaint(Range(pos1));
			braces[1] = pos1;
		}
		bracesMatchStyle = matchStyle;
		if (paintState == notPainting) {
			Redraw();
		}
	}
}

void Editor::SetDocPointer(Document *document) {
	//Platform::DebugPrintf("** %x setdoc to %x\n", pdoc, document);
	pdoc->RemoveWatcher(this, 0);
	pdoc->Release();
	if (document == NULL) {
		pdoc = new Document();
	} else {
		pdoc = document;
	}
	pdoc->AddRef();
	// Reset the contraction state to fully shown.
	cs.Clear();
	cs.InsertLines(0, pdoc->LinesTotal()-1);

	pdoc->AddWatcher(this, 0);
	Redraw();
	SetScrollBars();
}

// Recursively expand a fold, making lines visible except where they have an unexpanded parent
void Editor::Expand(int &line, bool doExpand) {
	int lineMaxSubord = pdoc->GetLastChild(line);
	line++;
	while (line <= lineMaxSubord) {
		if (doExpand)
			cs.SetVisible(line, line, true);
		int level = pdoc->GetLevel(line);
		if (level & SC_FOLDLEVELHEADERFLAG) {
			if (doExpand && cs.GetExpanded(line)) {
				Expand(line, true);
			} else {
				Expand(line, false);
			}
		} else {
			line++;
		}
	}
}

void Editor::ToggleContraction(int line) {
	if (pdoc->GetLevel(line) & SC_FOLDLEVELHEADERFLAG) {
		if (cs.GetExpanded(line)) {
			int lineMaxSubord = pdoc->GetLastChild(line);
			cs.SetExpanded(line, 0);
			if (lineMaxSubord > line) {
				cs.SetVisible(line+1, lineMaxSubord, false);
				SetScrollBars();
				Redraw();
			}
		} else {
			cs.SetExpanded(line, 1);
			Expand(line, true);
			SetScrollBars();
			Redraw();
		}
	}
}

// Recurse up from this line to find any folds that prevent this line from being visible
// and unfold them all.
void Editor::EnsureLineVisible(int line) {
	if (!cs.GetVisible(line)) {
		int lineParent = pdoc->GetFoldParent(line);
		if (lineParent >= 0) {
			if (line != lineParent)
				EnsureLineVisible(lineParent);
			if (!cs.GetExpanded(lineParent)) {
				cs.SetExpanded(lineParent, 1);
				Expand(lineParent, true);
			}
		}
		SetScrollBars();
		Redraw();
	}
}

LRESULT Editor::WndProc(UINT iMessage, WPARAM wParam, LPARAM lParam) {
	//Platform::DebugPrintf("S start wnd proc %d %d %d\n",iMessage, wParam, lParam);

	// Optional macro recording hook
#ifdef MACRO_SUPPORT
	if (recordingMacro)
		NotifyMacroRecord(iMessage, wParam, lParam);
#endif

	switch (iMessage) {

	case WM_GETTEXT:
		{
			if (lParam == 0)
				return 0;
			char *ptr = reinterpret_cast<char *>(lParam);
			unsigned int iChar = 0;
			for (; iChar < wParam-1; iChar++)
				ptr[iChar] = pdoc->CharAt(iChar);
			ptr[iChar] = '\0';
			return iChar;
		}

	case WM_SETTEXT:
		{
			if (lParam == 0)
				return FALSE;
			pdoc->DeleteChars(0, pdoc->Length());
			SetEmptySelection(0);
			pdoc->InsertString(0, reinterpret_cast<char *>(lParam));
			return TRUE;
		}

	case WM_GETTEXTLENGTH:
		return pdoc->Length();

	case WM_NOTIFY:
		//Platform::DebugPrintf("S notify %d %d\n", wParam, lParam);
		break;

	case WM_CUT:
		Cut();
		SetLastXChosen();
		break;

	case WM_COPY:
		Copy();
		break;

	case WM_PASTE:
		Paste();
		SetLastXChosen();
		EnsureCaretVisible();
		break;

	case WM_CLEAR:
		Clear();
		SetLastXChosen();
		break;

	case WM_UNDO:
		Undo();
		SetLastXChosen();
		break;

		// Edit control mesages

		// Not supported (no-ops):
		//		EM_GETWORDBREAKPROC
		//		EM_GETWORDBREAKPROCEX
		//		EM_SETWORDBREAKPROC
		//		EM_SETWORDBREAKPROCEX
		//		EM_GETWORDWRAPMODE
		//		EM_SETWORDWRAPMODE
		//		EM_LIMITTEXT
		//		EM_EXLIMITTEXT
		//		EM_SETRECT
		//		EM_SETRECTNP
		//		EM_FMTLINES
		//		EM_GETHANDLE
		//		EM_SETHANDLE
		//		EM_GETPASSWORDCHAR
		//		EM_SETPASSWORDCHAR
		//		EM_SETTABSTOPS
		//		EM_FINDWORDBREAK
		//		EM_GETCHARFORMAT
		//		EM_SETCHARFORMAT
		//		EM_GETOLEINTERFACE
		//		EM_SETOLEINTERFACE
		//		EM_SETOLECALLBACK
		//		EM_GETPARAFORMAT
		//		EM_SETPARAFORMAT
		//		EM_PASTESPECIAL
		//		EM_REQUESTRESIZE
		//		EM_GETBKGNDCOLOR
		//		EM_SETBKGNDCOLOR
		//		EM_STREAMIN
		//		EM_STREAMOUT
		//		EM_GETIMECOLOR
		//		EM_SETIMECOLOR
		//		EM_GETIMEOPTIONS
		//		EM_SETIMEOPTIONS
		//		EM_GETOPTIONS
		//		EM_SETOPTIONS
		//		EM_GETPUNCTUATION
		//		EM_SETPUNCTUATION
		//		EM_GETTHUMB

		// Not supported but should be:
		//		EM_GETEVENTMASK
		//		EM_SETEVENTMASK
		//		For printing:
		//			EM_DISPLAYBAND
		//			EM_SETTARGETDEVICE

	case EM_CANUNDO:
		return pdoc->CanUndo() ? TRUE : FALSE;

	case EM_UNDO:
		Undo();
		break;

	case EM_EMPTYUNDOBUFFER:
		pdoc->DeleteUndoHistory();
		return 0;

	case EM_GETFIRSTVISIBLELINE:
		return topLine;

	case EM_GETLINE: {
			if (lParam == 0)
				return 0;
			int lineStart = pdoc->LineStart(wParam);
			int lineEnd = pdoc->LineStart(wParam + 1);
			char *ptr = reinterpret_cast<char *>(lParam);
			WORD *pBufSize = reinterpret_cast<WORD *>(lParam);
			if (*pBufSize < lineEnd - lineStart) {
				ptr[0] = '\0'; 	// If no characters copied have to put a NUL into buffer
				return 0;
			}
			int iPlace = 0;
			for (int iChar = lineStart; iChar < lineEnd; iChar++)
				ptr[iPlace++] = pdoc->CharAt(iChar);
			return iPlace;
		}

	case EM_GETLINECOUNT:
		if (pdoc->LinesTotal() == 0)
			return 1;
		else
			return pdoc->LinesTotal();

	case EM_GETMODIFY:
		return !pdoc->IsSavePoint();

	case EM_SETMODIFY:
		// Not really supported now that there is the save point stuff
		//pdoc->isModified = wParam;
		//return pdoc->isModified;
		return false;

	case EM_GETRECT:
		if (lParam == 0)
			return 0;
		*(reinterpret_cast<PRectangle *>(lParam)) = GetClientRectangle();
		break;

	case EM_GETSEL:
		if (wParam)
			*reinterpret_cast<int *>(wParam) = SelectionStart();
		if (lParam)
			*reinterpret_cast<int *>(lParam) = SelectionEnd();
		return MAKELONG(SelectionStart(), SelectionEnd());

	case EM_EXGETSEL: {
			if (lParam == 0)
				return 0;
			CHARRANGE *pCR = reinterpret_cast<CHARRANGE *>(lParam);
			pCR->cpMin = SelectionStart();
			pCR->cpMax = SelectionEnd();
		}
		break;

	case EM_SETSEL: {
			int nStart = static_cast<int>(wParam);
			int nEnd = static_cast<int>(lParam);
			if (nEnd < 0)
				nEnd = pdoc->Length();
			if (nStart < 0)
				nStart = nEnd; 	// Remove selection
			SetSelection(nEnd, nStart);
			EnsureCaretVisible();
		}
		break;

	case EM_EXSETSEL: {
			if (lParam == 0)
				return 0;
			CHARRANGE *pCR = reinterpret_cast<CHARRANGE *>(lParam);
			if (pCR->cpMax == -1) {
				SetSelection(pCR->cpMin, pdoc->Length());
			} else {
				SetSelection(pCR->cpMin, pCR->cpMax);
			}
			EnsureCaretVisible();
			return pdoc->LineFromPosition(SelectionStart());
		}

	case EM_GETSELTEXT: {
			if (lParam == 0)
				return 0;
			char *ptr = reinterpret_cast<char *>(lParam);
			int selSize = SelectionRangeLength();
			char *text = CopySelectionRange();
			int iChar = 0;
			if (text) {
				for (; iChar < selSize; iChar++)
					ptr[iChar] = text[iChar];
				ptr[iChar] = '\0';
				delete []text;
			}
			return iChar;
		}

	case EM_GETWORDBREAKPROC:
		return 0;

	case EM_SETWORDBREAKPROC:
		break;

	case EM_LIMITTEXT:
		// wParam holds the number of characters control should be limited to
		break;

	case EM_GETLIMITTEXT:
		return 0xffffffff;

	case EM_GETOLEINTERFACE:
		return 0;

	case EM_LINEFROMCHAR:
		if (static_cast<int>(wParam) < 0)
			wParam = SelectionStart();
		return pdoc->LineFromPosition(wParam);

	case EM_EXLINEFROMCHAR:
		if (static_cast<int>(lParam) < 0)
			lParam = SelectionStart(); 	// Not specified, but probably OK
		return pdoc->LineFromPosition(lParam);

	case EM_LINEINDEX:
		if (static_cast<int>(wParam) < 0)
			wParam = pdoc->LineFromPosition(SelectionStart());
		if (wParam == 0)
			return 0; 	// Even if there is no text, there is a first line that starts at 0
		if (static_cast<int>(wParam) > pdoc->LinesTotal())
			return - 1;
		//if (wParam > pdoc->LineFromPosition(pdoc->Length()))	// Useful test, anyway...
		//	return -1;
		return pdoc->LineStart(wParam);

	case EM_LINELENGTH:
		{
			if (static_cast<int>(wParam) < 0)	// Who use this anyway?
				return 0; 	// Should be... Too complex to describe here, see MS specs!
			if (static_cast<int>(wParam) > pdoc->Length())	// Useful test, anyway...
				return 0;
			int line = pdoc->LineFromPosition(wParam);
			int charsOnLine = 0;
			for (int pos = pdoc->LineStart(line); pos < pdoc->LineStart(line + 1); pos++) {
				if ((pdoc->CharAt(pos) != '\r') && (pdoc->CharAt(pos) != '\n'))
					charsOnLine++;
			}
			return charsOnLine;
		}

		// Replacement of the old Scintilla interpretation of EM_LINELENGTH
	case SCI_LINELENGTH:
		if ((static_cast<int>(wParam) < 0) ||
		        (static_cast<int>(wParam) > pdoc->LineFromPosition(pdoc->Length())))
			return 0;
		return pdoc->LineStart(wParam + 1) - pdoc->LineStart(wParam);

	case EM_REPLACESEL: {
			if (lParam == 0)
				return 0;
			pdoc->BeginUndoAction();
			ClearSelection();
			char *replacement = reinterpret_cast<char *>(lParam);
			pdoc->InsertString(currentPos, replacement);
			pdoc->EndUndoAction();
			SetEmptySelection(currentPos + strlen(replacement));
			EnsureCaretVisible();
		}
		break;

	case EM_LINESCROLL:
		ScrollTo(topLine + lParam);
		HorizontalScrollTo(xOffset + wParam * vs.spaceWidth);
		return TRUE;

	case EM_SCROLLCARET:
		EnsureCaretVisible();
		break;

	case EM_SETREADONLY:
		pdoc->SetReadOnly(wParam);
		return TRUE;

	case EM_SETRECT:
		break;

	case EM_CANPASTE:
		return 1;

	case EM_CHARFROMPOS: {
			if (lParam == 0)
				return 0;
			Point *ppt = reinterpret_cast<Point *>(lParam);
			int pos = PositionFromLocation(*ppt);
			int line = pdoc->LineFromPosition(pos);
			return MAKELONG(pos, line);
		}

	case EM_POSFROMCHAR: {
			// The MS specs for this have changed 3 times: using the RichEdit 3 version
			if (wParam == 0)
				return 0;
			Point *ppt = reinterpret_cast<Point *>(wParam);
			if (lParam < 0) {
				*ppt = Point(0, 0);
			} else {
				*ppt = LocationFromPosition(lParam);
			}
			return 0;
		}

	case EM_FINDTEXT:
		return FindText(iMessage, wParam, lParam);

	case EM_FINDTEXTEX:
		return FindText(iMessage, wParam, lParam);

	case EM_GETTEXTRANGE: {
			if (lParam == 0)
				return 0;
			TEXTRANGE *tr = reinterpret_cast<TEXTRANGE *>(lParam);
			int cpMax = tr->chrg.cpMax;
			if (cpMax == -1)
				cpMax = pdoc->Length();
			int len = cpMax - tr->chrg.cpMin; 	// No -1 as cpMin and cpMax are referring to inter character positions
			pdoc->GetCharRange(tr->lpstrText, tr->chrg.cpMin, len);
			// Spec says copied text is terminated with a NUL
			tr->lpstrText[len] = '\0';
			return len; 	// Not including NUL
		}

	case EM_SELECTIONTYPE:
		if (currentPos == anchor)
			return SEL_EMPTY;
		else
			return SEL_TEXT;

	case EM_HIDESELECTION:
		hideSelection = wParam;
		Redraw();
		break;

	case EM_FORMATRANGE:
		return FormatRange(wParam, reinterpret_cast<FORMATRANGE *>(lParam));

	case EM_GETMARGINS:
		return MAKELONG(vs.leftMarginWidth, vs.rightMarginWidth);
		
	case EM_SETMARGINS:
		if (wParam & EC_LEFTMARGIN) {
			vs.leftMarginWidth = LOWORD(lParam);
		}
		if (wParam & EC_RIGHTMARGIN) {
			vs.rightMarginWidth = HIWORD(lParam);
		}
		if (wParam == EC_USEFONTINFO) {
			vs.leftMarginWidth = vs.aveCharWidth / 2;
			vs.rightMarginWidth = vs.aveCharWidth / 2;
		}
		InvalidateStyleRedraw();
		break;
	
		// Control specific mesages

	case SCI_ADDTEXT: {
			if (lParam == 0)
				return 0;
			pdoc->InsertString(CurrentPosition(), reinterpret_cast<char *>(lParam), wParam);
			SetEmptySelection(currentPos + wParam);
			return 0;
		}

	case SCI_ADDSTYLEDTEXT: {
			if (lParam == 0)
				return 0;
			pdoc->InsertStyledString(CurrentPosition() * 2, reinterpret_cast<char *>(lParam), wParam);
			SetEmptySelection(currentPos + wParam / 2);
			return 0;
		}

	case SCI_INSERTTEXT: {
			if (lParam == 0)
				return 0;
			int insertPos = wParam;
			if (static_cast<short>(wParam) == -1)
				insertPos = CurrentPosition();
			int newCurrent = CurrentPosition();
			int newAnchor = anchor;
			char *sz = reinterpret_cast<char *>(lParam);
			pdoc->InsertString(insertPos, sz);
			if (newCurrent > insertPos)
				newCurrent += strlen(sz);
			if (newAnchor > insertPos)
				newAnchor += strlen(sz);
			SetEmptySelection(newCurrent);
			return 0;
		}

	case SCI_CLEARALL:
		ClearAll();
		return 0;

	case SCI_SETUNDOCOLLECTION:
		pdoc->SetUndoCollection(static_cast<enum undoCollectionType>(wParam));
		return 0;

#ifdef INCLUDE_DEPRECATED_FEATURES
	case SCI_APPENDUNDOSTARTACTION:
		// Not just deprecated - now dead
		//pdoc->AppendUndoStartAction();
		return 0;
#endif

	case SCI_BEGINUNDOACTION:
		pdoc->BeginUndoAction();
		return 0;

	case SCI_ENDUNDOACTION:
		pdoc->EndUndoAction();
		return 0;

	case SCI_GETCARETPERIOD:
		return caret.period;

	case SCI_SETCARETPERIOD:
		caret.period = wParam;
		break;

	case SCI_SETWORDCHARS: {
			if (lParam == 0)
				return 0;
			pdoc->SetWordChars(reinterpret_cast<unsigned char *>(lParam));
		}
		break;

	case SCI_GETLENGTH:
		return pdoc->Length();

	case SCI_GETCHARAT:
		return pdoc->CharAt(wParam);

	case SCI_GETCURRENTPOS:
		return currentPos;

	case SCI_GETANCHOR:
		return anchor;

	case SCI_GETSTYLEAT:
		if (static_cast<short>(wParam) >= pdoc->Length())
			return 0;
		else
			return pdoc->StyleAt(wParam);

	case SCI_REDO:
		Redo();
		break;

	case SCI_SELECTALL:
		SelectAll();
		break;

	case SCI_SETSAVEPOINT:
		pdoc->SetSavePoint();
		NotifySavePoint(true);
		break;

	case SCI_GETSTYLEDTEXT: {
			if (lParam == 0)
				return 0;
			TEXTRANGE *tr = reinterpret_cast<TEXTRANGE *>(lParam);
			int iPlace = 0;
			for (int iChar = tr->chrg.cpMin; iChar < tr->chrg.cpMax; iChar++) {
				tr->lpstrText[iPlace++] = pdoc->CharAt(iChar);
				tr->lpstrText[iPlace++] = pdoc->StyleAt(iChar);
			}
			tr->lpstrText[iPlace] = '\0';
			tr->lpstrText[iPlace + 1] = '\0';
			return iPlace;
		}

	case SCI_CANREDO:
		return pdoc->CanRedo() ? TRUE : FALSE;

	case SCI_MARKERLINEFROMHANDLE:
		return pdoc->LineFromHandle(wParam);

	case SCI_MARKERDELETEHANDLE:
		pdoc->DeleteMarkFromHandle(wParam);
		break;

	case SCI_GETVIEWWS:
		return vs.viewWhitespace;

	case SCI_SETVIEWWS:
		vs.viewWhitespace = wParam;
		Redraw();
		break;

	case SCI_GOTOLINE:
		GoToLine(wParam);
		break;

	case SCI_GOTOPOS:
		SetEmptySelection(wParam);
		EnsureCaretVisible();
		Redraw();
		break;

	case SCI_SETANCHOR:
		SetSelection(currentPos, wParam);
		break;

	case SCI_GETCURLINE: {
			if (lParam == 0)
				return 0;
			int lineCurrentPos = pdoc->LineFromPosition(currentPos);
			int lineStart = pdoc->LineStart(lineCurrentPos);
			unsigned int lineEnd = pdoc->LineStart(lineCurrentPos + 1);
			char *ptr = reinterpret_cast<char *>(lParam);
			unsigned int iPlace = 0;
			for (unsigned int iChar = lineStart; iChar < lineEnd && iPlace < wParam; iChar++)
				ptr[iPlace++] = pdoc->CharAt(iChar);
			ptr[iPlace] = '\0';
			return currentPos - lineStart;
		}

	case SCI_GETENDSTYLED:
		return pdoc->GetEndStyled();

	case SCI_GETEOLMODE:
		return pdoc->eolMode;

	case SCI_SETEOLMODE:
		pdoc->eolMode = wParam;
		break;

	case SCI_STARTSTYLING:
		pdoc->StartStyling(wParam, lParam);
		break;

	case SCI_SETSTYLING:
		pdoc->SetStyleFor(wParam, lParam);
		break;

	case SCI_SETSTYLINGEX:   // Specify a complete styling buffer
		if (lParam == 0)
			return 0;
		pdoc->SetStyles(wParam, reinterpret_cast<char *>(lParam));
		break;

#ifdef INCLUDE_DEPRECATED_FEATURES
	case SCI_SETMARGINWIDTH:
		if (wParam < 100) {
			vs.ms[1].width = wParam;
		}
		InvalidateStyleRedraw();
		break;
#endif

	case SCI_SETBUFFEREDDRAW:
		bufferedDraw = wParam;
		break;

	case SCI_SETTABWIDTH:
		if (wParam > 0)
			pdoc->tabInChars = wParam;
		InvalidateStyleRedraw();
		break;

	case SCI_SETCODEPAGE:
		pdoc->dbcsCodePage = wParam;
		break;

#ifdef INCLUDE_DEPRECATED_FEATURES
	case SCI_SETLINENUMBERWIDTH:
		if (wParam < 200) {
			vs.ms[0].width = wParam;
		}
		InvalidateStyleRedraw();
		break;
#endif

	case SCI_SETUSEPALETTE:
		palette.allowRealization = wParam;
		InvalidateStyleRedraw();
		break;

		// Marker definition and setting
	case SCI_MARKERDEFINE:
		if (wParam <= MARKER_MAX)
			vs.markers[wParam].markType = lParam;
		InvalidateStyleData();
		RedrawSelMargin();
		break;
	case SCI_MARKERSETFORE:
		if (wParam <= MARKER_MAX)
			vs.markers[wParam].fore.desired = Colour(lParam);
		InvalidateStyleData();
		RedrawSelMargin();
		break;
	case SCI_MARKERSETBACK:
		if (wParam <= MARKER_MAX)
			vs.markers[wParam].back.desired = Colour(lParam);
		InvalidateStyleData();
		RedrawSelMargin();
		break;
	case SCI_MARKERADD: {
			int markerID = pdoc->AddMark(wParam, lParam);
			return markerID;
		}

	case SCI_MARKERDELETE:
		pdoc->DeleteMark(wParam, lParam);
		break;

	case SCI_MARKERDELETEALL:
		pdoc->DeleteAllMarks(static_cast<int>(wParam));
		break;

	case SCI_MARKERGET:
		return pdoc->GetMark(wParam);

	case SCI_MARKERNEXT: {
			int lt = pdoc->LinesTotal();
			for (int iLine = wParam; iLine < lt; iLine++) {
				if ((pdoc->GetMark(iLine) & lParam) != 0)
					return iLine;
			}
		}
		return -1;

	case SCI_MARKERPREVIOUS: {
			for (int iLine = wParam; iLine >= 0; iLine--) {
				if ((pdoc->GetMark(iLine) & lParam) != 0)
					return iLine;
			}
		}
		return -1;

	case SCI_SETMARGINTYPEN:
		if (wParam >= 0 && wParam < ViewStyle::margins) {
			vs.ms[wParam].symbol = (lParam == SC_MARGIN_SYMBOL);
			InvalidateStyleRedraw();
		}
		break;
		
	case SCI_GETMARGINTYPEN:
		if (wParam >= 0 && wParam < ViewStyle::margins) 
			return vs.ms[wParam].symbol ? SC_MARGIN_SYMBOL : SC_MARGIN_NUMBER;
		else
			return 0;
			
	case SCI_SETMARGINWIDTHN:
		if (wParam >= 0 && wParam < ViewStyle::margins) {
			vs.ms[wParam].width = lParam;
			InvalidateStyleRedraw();
		}
		break;
		
	case SCI_GETMARGINWIDTHN:
		if (wParam >= 0 && wParam < ViewStyle::margins) 
			return vs.ms[wParam].width;
		else
			return 0;
			
	case SCI_SETMARGINMASKN:
		if (wParam >= 0 && wParam < ViewStyle::margins) {
			vs.ms[wParam].mask = lParam;
			InvalidateStyleRedraw();
		}
		break;
		
	case SCI_GETMARGINMASKN:
		if (wParam >= 0 && wParam < ViewStyle::margins) 
			return vs.ms[wParam].mask;
		else
			return 0;
		
	case SCI_SETMARGINSENSITIVEN:
		if (wParam >= 0 && wParam < ViewStyle::margins) {
			vs.ms[wParam].sensitive = lParam;
			InvalidateStyleRedraw();
		}
		break;
		
	case SCI_GETMARGINSENSITIVEN:
		if (wParam >= 0 && wParam < ViewStyle::margins) 
			return vs.ms[wParam].sensitive ? 1 : 0;
		else
			return 0;

	case SCI_STYLECLEARALL:
		vs.ClearStyles();
		InvalidateStyleRedraw();
		break;

	case SCI_STYLESETFORE:
		if (wParam <= STYLE_MAX) {
			vs.styles[wParam].fore.desired = Colour(lParam);
			InvalidateStyleRedraw();
		}
		break;
	case SCI_STYLESETBACK:
		if (wParam <= STYLE_MAX) {
			vs.styles[wParam].back.desired = Colour(lParam);
			InvalidateStyleRedraw();
		}
		break;
	case SCI_STYLESETBOLD:
		if (wParam <= STYLE_MAX) {
			vs.styles[wParam].bold = lParam;
			InvalidateStyleRedraw();
		}
		break;
	case SCI_STYLESETITALIC:
		if (wParam <= STYLE_MAX) {
			vs.styles[wParam].italic = lParam;
			InvalidateStyleRedraw();
		}
		break;
	case SCI_STYLESETEOLFILLED:
		if (wParam <= STYLE_MAX) {
			vs.styles[wParam].eolFilled = lParam;
			InvalidateStyleRedraw();
		}
		break;
	case SCI_STYLESETSIZE:
		if (wParam <= STYLE_MAX) {
			vs.styles[wParam].size = lParam;
			InvalidateStyleRedraw();
		}
		break;
	case SCI_STYLESETFONT:
		if (lParam == 0)
			return 0;
		if (wParam <= STYLE_MAX) {
			vs.SetStyleFontName(wParam, reinterpret_cast<const char *>(lParam));
			InvalidateStyleRedraw();
		}
		break;
		
	case SCI_STYLERESETDEFAULT:
		vs.ResetDefaultStyle();
		InvalidateStyleRedraw();
		break;

	case SCI_SETSTYLEBITS:
		pdoc->SetStylingBits(wParam);
		break;
		
	case SCI_GETSTYLEBITS:
		return pdoc->stylingBits;
		
	case SCI_SETLINESTATE:
		return pdoc->SetLineState(wParam, lParam);
		
	case SCI_GETLINESTATE:
		return pdoc->GetLineState(wParam);
		
	case SCI_GETMAXLINESTATE:
		return pdoc->GetMaxLineState();
		
	// Folding messages
		
	case SCI_VISIBLEFROMDOCLINE:
		return cs.DisplayFromDoc(wParam);
		
	case SCI_DOCLINEFROMVISIBLE:
		return cs.DocFromDisplay(wParam);

	case SCI_SETFOLDLEVEL: {
			int prev = pdoc->SetLevel(wParam, lParam);
			if (prev != lParam)
				RedrawSelMargin();
			return prev;
		}
		
	case SCI_GETFOLDLEVEL:
		return pdoc->GetLevel(wParam);
		
	case SCI_GETLASTCHILD:
		return pdoc->GetLastChild(wParam, lParam);
		
	case SCI_GETFOLDPARENT:
		return pdoc->GetFoldParent(wParam);
		
	case SCI_SHOWLINES:
		cs.SetVisible(wParam, lParam, true);
		SetScrollBars();
		Redraw();
		break;
		
	case SCI_HIDELINES:
		cs.SetVisible(wParam, lParam, false);
		SetScrollBars();
		Redraw();
		break;
		
	case SCI_GETLINEVISIBLE:
		return cs.GetVisible(wParam);
		
	case SCI_SETFOLDEXPANDED:
		if (cs.SetExpanded(wParam, lParam)) {
			RedrawSelMargin();
		}
		break;
	
	case SCI_GETFOLDEXPANDED:
		return cs.GetExpanded(wParam);
		
	case SCI_SETFOLDFLAGS:
		foldFlags = wParam;
		Redraw();
		break;

	case SCI_TOGGLEFOLD:
		ToggleContraction(wParam);
		break;
		
	case SCI_ENSUREVISIBLE:
		EnsureLineVisible(wParam);
		break;
		
	case SCI_SEARCHANCHOR:
		SearchAnchor();
		break;

	case SCI_SEARCHNEXT:
	case SCI_SEARCHPREV:
		return SearchText(iMessage, wParam, lParam);
		break;

	case SCI_SETCARETPOLICY:
		caretPolicy = wParam;
		caretSlop = lParam;
		break;

	case SCI_LINESONSCREEN:
		return LinesOnScreen();

	case SCI_USEPOPUP:
		displayPopupMenu = wParam;
		break;

  #ifdef INCLUDE_DEPRECATED_FEATURES
	case SCI_SETFORE:
		vs.styles[STYLE_DEFAULT].fore.desired = Colour(wParam);
		InvalidateStyleRedraw();
		break;

	case SCI_SETBACK:
		vs.styles[STYLE_DEFAULT].back.desired = Colour(wParam);
		InvalidateStyleRedraw();
		break;

	case SCI_SETBOLD:
		vs.styles[STYLE_DEFAULT].bold = wParam;
		InvalidateStyleRedraw();
		break;

	case SCI_SETITALIC:
		vs.styles[STYLE_DEFAULT].italic = wParam;
		InvalidateStyleRedraw();
		break;

	case SCI_SETSIZE:
		vs.styles[STYLE_DEFAULT].size = wParam;
		InvalidateStyleRedraw();
		break;

	case SCI_SETFONT:
		if (wParam == 0)
			return 0;
		strcpy(vs.styles[STYLE_DEFAULT].fontName, reinterpret_cast<char *>(wParam));
		InvalidateStyleRedraw();
		break;
#endif

	case SCI_SETSELFORE:
		vs.selforeset = wParam;
		vs.selforeground.desired = Colour(lParam);
		InvalidateStyleRedraw();
		break;

	case SCI_SETSELBACK:
		vs.selbackset = wParam;
		vs.selbackground.desired = Colour(lParam);
		InvalidateStyleRedraw();
		break;

	case SCI_SETCARETFORE:
		vs.caretcolour.desired = Colour(wParam);
		InvalidateStyleRedraw();
		break;

	case SCI_ASSIGNCMDKEY:
		kmap.AssignCmdKey(LOWORD(wParam), HIWORD(wParam), lParam);
		break;

	case SCI_CLEARCMDKEY:
		kmap.AssignCmdKey(LOWORD(wParam), HIWORD(wParam), WM_NULL);
		break;

	case SCI_CLEARALLCMDKEYS:
		kmap.Clear();
		break;

	case SCI_INDICSETSTYLE:
		if (wParam <= INDIC_MAX) {
			vs.indicators[wParam].style = lParam;
			InvalidateStyleRedraw();
		}
		break;

	case SCI_INDICGETSTYLE:
		return (wParam <= INDIC_MAX) ? vs.indicators[wParam].style : 0;

	case SCI_INDICSETFORE:
		if (wParam <= INDIC_MAX) {
			vs.indicators[wParam].fore.desired = Colour(lParam);
			InvalidateStyleRedraw();
		}
		break;

	case SCI_INDICGETFORE:
		return (wParam <= INDIC_MAX) ? vs.indicators[wParam].fore.desired.AsLong() : 0;

	case SCI_LINEDOWN:
	case SCI_LINEDOWNEXTEND:
	case SCI_LINEUP:
	case SCI_LINEUPEXTEND:
	case SCI_CHARLEFT:
	case SCI_CHARLEFTEXTEND:
	case SCI_CHARRIGHT:
	case SCI_CHARRIGHTEXTEND:
	case SCI_WORDLEFT:
	case SCI_WORDLEFTEXTEND:
	case SCI_WORDRIGHT:
	case SCI_WORDRIGHTEXTEND:
	case SCI_HOME:
	case SCI_HOMEEXTEND:
	case SCI_LINEEND:
	case SCI_LINEENDEXTEND:
	case SCI_DOCUMENTSTART:
	case SCI_DOCUMENTSTARTEXTEND:
	case SCI_DOCUMENTEND:
	case SCI_DOCUMENTENDEXTEND:
	case SCI_PAGEUP:
	case SCI_PAGEUPEXTEND:
	case SCI_PAGEDOWN:
	case SCI_PAGEDOWNEXTEND:
	case SCI_EDITTOGGLEOVERTYPE:
	case SCI_CANCEL:
	case SCI_DELETEBACK:
	case SCI_TAB:
	case SCI_BACKTAB:
	case SCI_NEWLINE:
	case SCI_FORMFEED:
	case SCI_VCHOME:
	case SCI_VCHOMEEXTEND:
	case SCI_ZOOMIN:
	case SCI_ZOOMOUT:
	case SCI_DELWORDLEFT:
	case SCI_DELWORDRIGHT:
	case SCI_LINECUT:
	case SCI_LINEDELETE:
	case SCI_LINETRANSPOSE:
	case SCI_LOWERCASE:
	case SCI_UPPERCASE:
		return KeyCommand(iMessage);

	case SCI_BRACEHIGHLIGHT:
		SetBraceHighlight(static_cast<int>(wParam), lParam, STYLE_BRACELIGHT);
		break;
		
	case SCI_BRACEBADLIGHT:
		SetBraceHighlight(static_cast<int>(wParam), -1, STYLE_BRACEBAD);
		break;

	case SCI_BRACEMATCH:
		// wParam is position of char to find brace for,
		// lParam is maximum amount of text to restyle to find it
		return BraceMatch(wParam, lParam);

	case SCI_GETVIEWEOL:
		return vs.viewEOL;

	case SCI_SETVIEWEOL:
		vs.viewEOL = wParam;
		Redraw();
		break;

	case SCI_GETEDGECOLUMN:
		return theEdge;
		
	case SCI_SETEDGECOLUMN:
		theEdge = wParam;
		InvalidateStyleRedraw();
		break;
		
	case SCI_GETEDGEMODE:
		return edgeState;
		
	case SCI_SETEDGEMODE:
		edgeState = wParam;
		InvalidateStyleRedraw();
		break;
		
	case SCI_GETEDGECOLOUR:
		return vs.edgecolour.desired.AsLong();
		
	case SCI_SETEDGECOLOUR:
		vs.edgecolour.desired = Colour(wParam);
		InvalidateStyleRedraw();
		break;
		
	case SCI_GETDOCPOINTER:
		return reinterpret_cast<LRESULT>(pdoc);

	case SCI_SETDOCPOINTER:
		SetDocPointer(reinterpret_cast<Document *>(lParam));
		return 0;

	case SCI_SETMODEVENTMASK:
		modEventMask = wParam;
		return 0;
		
	case SCI_CONVERTEOLS:
		pdoc->ConvertLineEnds(wParam);
		SetSelection(currentPos, anchor);	// Ensure selection inside document
		return 0;

#ifdef MACRO_SUPPORT
	case SCI_STARTRECORD:
		recordingMacro = 1;
		return 0;

	case SCI_STOPRECORD:
		recordingMacro = 0;
		return 0;
#endif
		
	default:
		return DefWndProc(iMessage, wParam, lParam);
	}
	//Platform::DebugPrintf("end wnd proc\n");
	return 0l;
}
