// SciTE - Scintilla based Text Editor
// Accessor.cxx - rapid easy access to contents of a Scintilla
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <ctype.h> 
#include <stdio.h>

#include "Platform.h"

#include "PropSet.h"
#include "Accessor.h"
#include "Scintilla.h"

bool Accessor::InternalIsLeadByte(char ch) {
#if PLAT_GTK
	// TODO: support DBCS under GTK+
	return false;
#elif PLAT_WIN 
	return IsDBCSLeadByteEx(codePage, ch);
#elif PLAT_WX 
	return false;
#endif 
}

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

void StylingContext::StartSegment(unsigned int pos) {
	startSeg = pos;
}

void StylingContext::ColourTo(unsigned int pos, int chAttr) {
	// Only perform styling if non empty range
	if (pos != startSeg - 1) {
		if (pos < startSeg) {
			Platform::DebugPrintf("Bad colour positions %d - %d\n", startSeg, pos);
		}

		if (validLen + (pos - startSeg + 1) >= bufferSize)
			Flush();
		if (validLen + (pos - startSeg + 1) >= bufferSize) {
			// Too big for buffer so send directly
			Platform::SendScintilla(id, SCI_SETSTYLING, pos - startSeg + 1, chAttr);
		} else {
			if (chAttr != chWhile)
				chFlags = 0;
			chAttr |= chFlags;
			for (unsigned int i = startSeg; i <= pos; i++) {
				styleBuf[validLen++] = chAttr;
			}
		}
	}
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

int StylingContext::IndentAmount(int line, int *flags, PFNIsCommentLeader pfnIsCommentLeader) {
	int end = Length();
	int spaceFlags = 0;
	
	// Determines the indentation level of the current line and also checks for consistent 
	// indentation compared to the previous line.
	// Indentation is judged consistent when the indentation whitespace of each line lines 
	// the same or the indentation of one line is a prefix of the other.
	
	int pos = LineStart(line);
	char ch = (*this)[pos];
	int indent = 0;
	bool inPrevPrefix = line > 0;
	int posPrev = inPrevPrefix ? LineStart(line-1) : 0;
	while ((ch == ' ' || ch == '\t') && (pos < end)) {
		if (inPrevPrefix) {
			char chPrev = (*this)[posPrev++];
			if (chPrev == ' ' || chPrev == '\t') {
				if (chPrev != ch)
					spaceFlags |= wsInconsistent;
			} else {
				inPrevPrefix = false;
			}
		}
		if (ch == ' ') {
			spaceFlags |= wsSpace;
			indent++;
		} else {	// Tab
			spaceFlags |= wsTab;
			if (spaceFlags & wsSpace)
				spaceFlags |= wsSpaceTab;
			indent = (indent / 8 + 1) * 8;
		}
		ch = (*this)[++pos];
	}
	
	*flags = spaceFlags;
	indent += SC_FOLDLEVELBASE;
	// if completely empty line or the start of a comment...
	if (isspace(ch) || (pfnIsCommentLeader && (*pfnIsCommentLeader)(*this, pos, end-pos)) )
		return indent | SC_FOLDLEVELWHITEFLAG;
	else
		return indent;
}

