// SciTE - Scintilla based Text Editor
// Accessor.cxx - rapid easy access to contents of a Scintilla
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <stdio.h>

#include "Platform.h"

#include "PropSet.h"
#include "Accessor.h"
#include "Scintilla.h"

void Accessor::Fill(int position) {
	if (lenDoc == -1)
		lenDoc = Platform::SendScintilla(id, WM_GETTEXTLENGTH, 0, 0);
	startPos = position - slopSize;
	if (startPos + bufferSize > lenDoc)
		startPos = lenDoc - bufferSize;
	if (startPos < 0)
		startPos = 0;
	endPos = startPos + bufferSize;
	if (endPos > lenDoc)
		endPos = lenDoc;

	TEXTRANGE tr = {{startPos, endPos}, buf};
	Platform::SendScintilla(id, EM_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));
}

char Accessor::StyleAt(int position) {
	return static_cast<char>(Platform::SendScintilla(
		id, SCI_GETSTYLEAT, position, 0));
}

int Accessor::GetLine(int position) {
	return Platform::SendScintilla(id, EM_LINEFROMCHAR, position, 0);
}

int Accessor::LineStart(int line) {
	return Platform::SendScintilla(id, EM_LINEINDEX, line, 0);
}

int Accessor::LevelAt(int line) {
	return Platform::SendScintilla(id, SCI_GETFOLDLEVEL, line, 0);
}

int Accessor::Length() { 
	if (lenDoc == -1) 
		lenDoc = Platform::SendScintilla(id, WM_GETTEXTLENGTH, 0, 0);
	return lenDoc; 
}

int Accessor::GetLineState(int line) {
	return Platform::SendScintilla(id, SCI_GETLINESTATE, line);
}

int Accessor::SetLineState(int line, int state) {
	return Platform::SendScintilla(id, SCI_SETLINESTATE, line, state);
}

void StylingContext::StartAt(unsigned int start, char chMask) {
	Platform::SendScintilla(id, SCI_STARTSTYLING, start, chMask);
}

void StylingContext::ColourSegment(unsigned int start, unsigned int end, int chAttr) {
	// Only perform styling if non empty range
	if (end != start - 1) {
		if (end < start) {
			Platform::DebugPrintf("Bad colour positions %d - %d\n", start, end);
		}

		if (validLen + (end - start + 1) >= bufferSize)
			Flush();
		if (validLen + (end - start + 1) >= bufferSize) {
			// Too big for buffer so send directly
			Platform::SendScintilla(id, SCI_SETSTYLING, end - start + 1, chAttr);
		} else {
			if (chAttr != chWhile)
				chFlags = 0;
			chAttr |= chFlags;
			for (unsigned int i = start; i <= end; i++) {
				styleBuf[validLen++] = chAttr;
			}
		}
	}
}

void StylingContext::StartSegment(unsigned int pos) {
	startSeg = pos;
}

void StylingContext::ColourTo(unsigned int pos, int chAttr) {
	ColourSegment(startSeg, pos, chAttr);
	startSeg = pos+1;
}

int StylingContext::GetLine(int position) {
	return Platform::SendScintilla(id, EM_LINEFROMCHAR, position, 0);
}

void StylingContext::SetLevel(int line, int level) {
	Platform::SendScintilla(id, SCI_SETFOLDLEVEL, line, level);
}

void StylingContext::Flush() {
	if (validLen > 0) {
		Platform::SendScintilla(id, SCI_SETSTYLINGEX, validLen, 
			reinterpret_cast<LPARAM>(styleBuf));
		validLen = 0;
	}
}
