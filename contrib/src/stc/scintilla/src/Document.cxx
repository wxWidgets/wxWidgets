// Scintilla source code edit control
// Document.cxx - text document that handles notifications, DBCS, styling, words and end of line
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "Platform.h"

#include "Scintilla.h"
#include "SVector.h"
#include "CellBuffer.h"
#include "Document.h"

Document::Document() {
	refCount = 0;
#ifdef unix
	eolMode = SC_EOL_LF;
#else
	eolMode = SC_EOL_CRLF;
#endif
	dbcsCodePage = 0;
	stylingBits = 5;
	stylingBitsMask = 0x1F;
	stylingPos = 0;
	stylingMask = 0;
	for (int ch = 0; ch < 256; ch++) {
		wordchars[ch] = isalnum(ch) || ch == '_';
	}
	endStyled = 0;
	enteredCount = 0;
	tabInChars = 8;
	watchers = 0;
	lenWatchers = 0;
}

Document::~Document() {
	for (int i = 0; i < lenWatchers; i++) {
		watchers[i].watcher->NotifyDeleted(this, watchers[i].userData);
	}
	delete []watchers;
	watchers = 0;
	lenWatchers = 0;
}

// Increase reference count and return its previous value.
int Document::AddRef() {
	return refCount++;
}

// Decrease reference count and return its provius value.
// Delete the document if reference count reaches zero.
int Document::Release() {
	int curRefCount = --refCount;
	if (curRefCount == 0)
		delete this;
	return curRefCount;
}

void Document::SetSavePoint() {
	cb.SetSavePoint();
	NotifySavePoint(true);
}

int Document::AddMark(int line, int markerNum) { 
	int prev = cb.AddMark(line, markerNum); 
	DocModification mh(SC_MOD_CHANGEMARKER, LineStart(line), 0, 0, 0);
	NotifyModified(mh);
	return prev;
}

void Document::DeleteMark(int line, int markerNum) { 
	cb.DeleteMark(line, markerNum); 
	DocModification mh(SC_MOD_CHANGEMARKER, LineStart(line), 0, 0, 0);
	NotifyModified(mh);
}

void Document::DeleteMarkFromHandle(int markerHandle) { 
	cb.DeleteMarkFromHandle(markerHandle); 
	DocModification mh(SC_MOD_CHANGEMARKER, 0, 0, 0, 0);
	NotifyModified(mh);
}

void Document::DeleteAllMarks(int markerNum) { 
	cb.DeleteAllMarks(markerNum); 
	DocModification mh(SC_MOD_CHANGEMARKER, 0, 0, 0, 0);
	NotifyModified(mh);
}

int Document::LineStart(int line) {
	return cb.LineStart(line);
}

int Document::LineEnd(int line) {
	if (line == LinesTotal() - 1) {
		return LineStart(line + 1);
	} else {
		int position = LineStart(line + 1) - 1;
		// When line terminator is CR+LF, may need to go back one more
		if ((position > LineStart(line)) && (cb.CharAt(position - 1) == '\r')) {
			position--;
		}
		return position;
	}
}

int Document::LineFromPosition(int pos) {
	return cb.LineFromPosition(pos);
}

int Document::LineEndPosition(int position) {
	return LineEnd(LineFromPosition(position));
}

int Document::VCHomePosition(int position) {
	int line = LineFromPosition(position);
	int startPosition = LineStart(line);
	int endLine = LineStart(line + 1) - 1;
	int startText = startPosition;
	while (startText < endLine && (cb.CharAt(startText) == ' ' || cb.CharAt(startText) == '\t' ) )
		startText++;
	if (position == startText)
		return startPosition;
	else
		return startText;
}

int Document::SetLevel(int line, int level) { 
	int prev = cb.SetLevel(line, level); 
	if (prev != level) {
		DocModification mh(SC_MOD_CHANGEFOLD, LineStart(line), 0, 0, 0);
		mh.line = line;
		mh.foldLevelNow = level;
		mh.foldLevelPrev = prev;
		NotifyModified(mh);
	}
	return prev;
}

static bool IsSubordinate(int levelStart, int levelTry) {
	if (levelTry & SC_FOLDLEVELWHITEFLAG)
		return true;
	else 
		return (levelStart & SC_FOLDLEVELNUMBERMASK) < (levelTry & SC_FOLDLEVELNUMBERMASK);
}

int Document::GetLastChild(int lineParent, int level) {
	if (level == -1)
		level = GetLevel(lineParent) & SC_FOLDLEVELNUMBERMASK;
	int maxLine = LinesTotal();
	int lineMaxSubord = lineParent;
	while (lineMaxSubord < maxLine-1) {
		EnsureStyledTo(LineStart(lineMaxSubord+2));
		if (!IsSubordinate(level, GetLevel(lineMaxSubord+1)))
			break;
		lineMaxSubord++;
	}
	if (lineMaxSubord > lineParent) {
		if (level > (GetLevel(lineMaxSubord+1) & SC_FOLDLEVELNUMBERMASK)) {
			// Have chewed up some whitespace that belongs to a parent so seek back 
			if ((lineMaxSubord > lineParent) && (GetLevel(lineMaxSubord) & SC_FOLDLEVELWHITEFLAG)) {
				lineMaxSubord--;
			}
		}
	}
	return lineMaxSubord;
}

int Document::GetFoldParent(int line) {
	int level = GetLevel(line);
	int lineLook = line-1;
	while ((lineLook > 0) && (
		(!(GetLevel(lineLook) & SC_FOLDLEVELHEADERFLAG)) || 
		((GetLevel(lineLook) & SC_FOLDLEVELNUMBERMASK) >= level))
	) {
		lineLook--;
	}
	if ((GetLevel(lineLook) & SC_FOLDLEVELHEADERFLAG) &&
		((GetLevel(lineLook) & SC_FOLDLEVELNUMBERMASK) < level)) {
		return lineLook;
	} else {
		return -1;
	}
}

int Document::ClampPositionIntoDocument(int pos) {
	return Platform::Clamp(pos, 0, Length());
}

bool Document::IsCrLf(int pos) {
	if (pos < 0)
		return false;
	if (pos >= (Length() - 1))
		return false;
	return (cb.CharAt(pos) == '\r') && (cb.CharAt(pos + 1) == '\n');
}

bool Document::IsDBCS(int pos) {
#if PLAT_WIN
	if (dbcsCodePage) {
		// Anchor DBCS calculations at start of line because start of line can
		// not be a DBCS trail byte.
		int startLine = pos;
		while (startLine > 0 && cb.CharAt(startLine) != '\r' && cb.CharAt(startLine) != '\n')
			startLine--;
		while (startLine <= pos) {
			if (IsDBCSLeadByteEx(dbcsCodePage, cb.CharAt(startLine))) {
				startLine++;
				if (startLine >= pos)
					return true;
			}
			startLine++;
		}
	}
	return false;
#else
	return false;
#endif
}

// Normalise a position so that it is not halfway through a two byte character.
// This can occur in two situations -
// When lines are terminated with \r\n pairs which should be treated as one character.
// When displaying DBCS text such as Japanese.
// If moving, move the position in the indicated direction.
int Document::MovePositionOutsideChar(int pos, int moveDir, bool checkLineEnd) {
	//Platform::DebugPrintf("NoCRLF %d %d\n", pos, moveDir);
	// If out of range, just return value - should be fixed up after
	if (pos < 0)
		return pos;
	if (pos > Length())
		return pos;

	// Position 0 and Length() can not be between any two characters
	if (pos == 0)
		return pos;
	if (pos == Length())
		return pos;

	// assert pos > 0 && pos < Length()
	if (checkLineEnd && IsCrLf(pos - 1)) {
		if (moveDir > 0)
			return pos + 1;
		else
			return pos - 1;
	}

	// Not between CR and LF

#if PLAT_WIN
	if (dbcsCodePage) {
		// Anchor DBCS calculations at start of line because start of line can
		// not be a DBCS trail byte.
		int startLine = pos;
		while (startLine > 0 && cb.CharAt(startLine) != '\r' && cb.CharAt(startLine) != '\n')
			startLine--;
		bool atLeadByte = false;
		while (startLine < pos) {
			if (atLeadByte)
				atLeadByte = false;
			else if (IsDBCSLeadByteEx(dbcsCodePage, cb.CharAt(startLine)))
				atLeadByte = true;
			else
				atLeadByte = false;
			startLine++;
			//Platform::DebugPrintf("DBCS %s\n", atlead ? "D" : "-");
		}

		if (atLeadByte) {
			// Position is between a lead byte and a trail byte
			if (moveDir > 0)
				return pos + 1;
			else
				return pos - 1;
		}
	}
#endif

	return pos;
}

void Document::ModifiedAt(int pos) {
	if (endStyled > pos)
		endStyled = pos;
}

// Document only modified by gateways DeleteChars, InsertStyledString, Undo, Redo, and SetStyleAt.
// SetStyleAt does not change the persistent state of a document

// Unlike Undo, Redo, and InsertStyledString, the pos argument is a cell number not a char number
void Document::DeleteChars(int pos, int len) {
	if (enteredCount == 0) {
		enteredCount++;
		if (cb.IsReadOnly())
			NotifyModifyAttempt();
		if (!cb.IsReadOnly()) {
			int prevLinesTotal = LinesTotal();
			bool startSavePoint = cb.IsSavePoint();
			const char *text = cb.DeleteChars(pos*2, len * 2);
			if (startSavePoint && cb.IsCollectingUndo())
				NotifySavePoint(!startSavePoint);
			ModifiedAt(pos);
			int modFlags = SC_MOD_DELETETEXT | SC_PERFORMED_USER;
			DocModification mh(modFlags, pos, len, LinesTotal() - prevLinesTotal, text);
			NotifyModified(mh);
		}
		enteredCount--;
	}
}

void Document::InsertStyledString(int position, char *s, int insertLength) {
	if (enteredCount == 0) {
		enteredCount++;
		if (cb.IsReadOnly())
			NotifyModifyAttempt();
		if (!cb.IsReadOnly()) {
			int prevLinesTotal = LinesTotal();
			bool startSavePoint = cb.IsSavePoint();
			const char *text = cb.InsertString(position, s, insertLength);
			if (startSavePoint && cb.IsCollectingUndo())
				NotifySavePoint(!startSavePoint);
			ModifiedAt(position / 2);
	
			int modFlags = SC_MOD_INSERTTEXT | SC_PERFORMED_USER;
			DocModification mh(modFlags, position / 2, insertLength / 2, LinesTotal() - prevLinesTotal, text);
			NotifyModified(mh);
		}
		enteredCount--;
	}
}

int Document::Undo() {
	int newPos = 0;
	if (enteredCount == 0) {
		enteredCount++;
		bool startSavePoint = cb.IsSavePoint();
		int steps = cb.StartUndo();
		//Platform::DebugPrintf("Steps=%d\n", steps);
		for (int step=0; step<steps; step++) {
			int prevLinesTotal = LinesTotal();
			const Action &action = cb.UndoStep();
			int cellPosition = action.position / 2;
			ModifiedAt(cellPosition);
			newPos = cellPosition;
			
			int modFlags = SC_PERFORMED_UNDO;
			// With undo, an insertion action becomes a deletion notification
			if (action.at == removeAction) {
				newPos += action.lenData;
				modFlags |= SC_MOD_INSERTTEXT;
			} else {
				modFlags |= SC_MOD_DELETETEXT;
			}
			if (step == steps-1)
				modFlags |= SC_LASTSTEPINUNDOREDO;
			NotifyModified(DocModification(modFlags, cellPosition, action.lenData, 
				LinesTotal() - prevLinesTotal, action.data));
		}
	
		bool endSavePoint = cb.IsSavePoint();
		if (startSavePoint != endSavePoint)
			NotifySavePoint(endSavePoint);
		enteredCount--;
	}
	return newPos;
}

int Document::Redo() {
	int newPos = 0;
	if (enteredCount == 0) {
		enteredCount++;
		bool startSavePoint = cb.IsSavePoint();
		int steps = cb.StartRedo();
		for (int step=0; step<steps; step++) {
			int prevLinesTotal = LinesTotal();
			const Action &action = cb.RedoStep();
			int cellPosition = action.position / 2;
			ModifiedAt(cellPosition);
			newPos = cellPosition;
			
			int modFlags = SC_PERFORMED_REDO;
			if (action.at == insertAction) {
				newPos += action.lenData;
				modFlags |= SC_MOD_INSERTTEXT;
			} else {
				modFlags |= SC_MOD_DELETETEXT;
			}
			if (step == steps-1)
				modFlags |= SC_LASTSTEPINUNDOREDO;
			NotifyModified(DocModification(modFlags, cellPosition, action.lenData, 
				LinesTotal() - prevLinesTotal, action.data));
		}
	
		bool endSavePoint = cb.IsSavePoint();
		if (startSavePoint != endSavePoint)
			NotifySavePoint(endSavePoint);
		enteredCount--;
	}
	return newPos;
}

void Document::InsertChar(int pos, char ch) {
	char chs[2];
	chs[0] = ch;
	chs[1] = 0;
	InsertStyledString(pos*2, chs, 2);
}

// Insert a null terminated string
void Document::InsertString(int position, const char *s) {
	InsertString(position, s, strlen(s));
}

// Insert a string with a length
void Document::InsertString(int position, const char *s, int insertLength) {
	char *sWithStyle = new char[insertLength * 2];
	if (sWithStyle) {
		for (int i = 0; i < insertLength; i++) {
			sWithStyle[i*2] = s[i];
			sWithStyle[i*2 + 1] = 0;
		}
		InsertStyledString(position*2, sWithStyle, insertLength*2);
		delete []sWithStyle;
	}
}

void Document::ChangeChar(int pos, char ch) {
	DeleteChars(pos, 1);
	InsertChar(pos, ch);
}

void Document::DelChar(int pos) {
	if (IsCrLf(pos)) {
		DeleteChars(pos, 2);
	} else if (IsDBCS(pos)) {
		DeleteChars(pos, 2);
	} else if (pos < Length()) {
		DeleteChars(pos, 1);
	}
}

int Document::DelCharBack(int pos) {
	if (pos <= 0) {
		return pos;
	} else if (IsCrLf(pos - 2)) {
		DeleteChars(pos - 2, 2);
		return pos - 2;
	} else if (IsDBCS(pos - 1)) {
		DeleteChars(pos - 2, 2);
		return pos - 2;
	} else {
		DeleteChars(pos - 1, 1);
		return pos - 1;
	}
}

void Document::Indent(bool forwards, int lineBottom, int lineTop) {
	if (forwards) {
		// Indent by a tab
		for (int line = lineBottom; line >= lineTop; line--) {
			InsertChar(LineStart(line), '\t');
		}
	} else {
		// Dedent - suck white space off the front of the line to dedent by equivalent of a tab
		for (int line = lineBottom; line >= lineTop; line--) {
			int ispc = 0;
			while (ispc < tabInChars && cb.CharAt(LineStart(line) + ispc) == ' ')
				ispc++;
			int posStartLine = LineStart(line);
			if (ispc == tabInChars) {
				DeleteChars(posStartLine, ispc);
			} else if (cb.CharAt(posStartLine + ispc) == '\t') {
				DeleteChars(posStartLine, ispc + 1);
			} else {	// Hit a non-white
				DeleteChars(posStartLine, ispc);
			}
		}
	}
}

void Document::ConvertLineEnds(int eolModeSet) {
	BeginUndoAction();
	for (int pos = 0; pos < Length(); pos++) {
		if (cb.CharAt(pos) == '\r') {
			if (cb.CharAt(pos+1) == '\n') {
				if (eolModeSet != SC_EOL_CRLF) {
					DeleteChars(pos, 2);
					if (eolModeSet == SC_EOL_CR)
						InsertString(pos, "\r", 1);
					else
						InsertString(pos, "\n", 1);
				} else {
					pos++;
				}
			} else {
				if (eolModeSet != SC_EOL_CR) {
					DeleteChars(pos, 1);
					if (eolModeSet == SC_EOL_CRLF) {
						InsertString(pos, "\r\n", 2);
						pos++;
					} else {
						InsertString(pos, "\n", 1);
					}
				}
			}
		} else if (cb.CharAt(pos) == '\n') {
			if (eolModeSet != SC_EOL_LF) {
				DeleteChars(pos, 1);
				if (eolModeSet == SC_EOL_CRLF) {
					InsertString(pos, "\r\n", 2);
					pos++;
				} else {
					InsertString(pos, "\r", 1);
				}
			}
		}
	}
	EndUndoAction();
}

bool Document::IsWordChar(unsigned char ch) {
	return wordchars[ch];
}

int Document::ExtendWordSelect(int pos, int delta) {
	if (delta < 0) {
		while (pos > 0 && IsWordChar(cb.CharAt(pos - 1)))
			pos--;
	} else {
		while (pos < (Length()) && IsWordChar(cb.CharAt(pos)))
			pos++;
	}
	return pos;
}

int Document::NextWordStart(int pos, int delta) {
	if (delta < 0) {
		while (pos > 0 && (cb.CharAt(pos - 1) == ' ' || cb.CharAt(pos - 1) == '\t'))
			pos--;
		if (isspace(cb.CharAt(pos - 1))) {	// Back up to previous line
			while (pos > 0 && isspace(cb.CharAt(pos - 1)))
				pos--;
		} else {
			bool startAtWordChar = IsWordChar(cb.CharAt(pos - 1));
			while (pos > 0 && !isspace(cb.CharAt(pos - 1)) && (startAtWordChar == IsWordChar(cb.CharAt(pos - 1))))
				pos--;
		}
	} else {
		bool startAtWordChar = IsWordChar(cb.CharAt(pos));
		while (pos < (Length()) && isspace(cb.CharAt(pos)))
			pos++;
		while (pos < (Length()) && !isspace(cb.CharAt(pos)) && (startAtWordChar == IsWordChar(cb.CharAt(pos))))
			pos++;
		while (pos < (Length()) && (cb.CharAt(pos) == ' ' || cb.CharAt(pos) == '\t'))
			pos++;
	}
	return pos;
}

bool Document::IsWordAt(int start, int end) {
	int lengthDoc = Length();
	if (start > 0) {
		char ch = CharAt(start - 1);
		if (IsWordChar(ch))
			return false;
	}
	if (end < lengthDoc - 1) {
		char ch = CharAt(end);
		if (IsWordChar(ch))
			return false;
	}
	return true;
}

// Find text in document, supporting both forward and backward
// searches (just pass minPos > maxPos to do a backward search)
// Has not been tested with backwards DBCS searches yet.
long Document::FindText(int minPos, int maxPos, const char *s, bool caseSensitive, bool word) {
 	bool forward = minPos <= maxPos;
	int increment = forward ? 1 : -1;

	// Range endpoints should not be inside DBCS characters, but just in case, move them.
	int startPos = MovePositionOutsideChar(minPos, increment, false);
	int endPos = MovePositionOutsideChar(maxPos, increment, false);
 	
	// Compute actual search ranges needed
	int lengthFind = strlen(s);
 	int endSearch = 0;
 	if (startPos <= endPos) {
 		endSearch = endPos - lengthFind + 1;
 	} else {
 		endSearch = endPos;
 	}
	//Platform::DebugPrintf("Find %d %d %s %d\n", startPos, endPos, ft->lpstrText, lengthFind);
	char firstChar = s[0];
	if (!caseSensitive)
		firstChar = toupper(firstChar);
	int pos = startPos;
	while (forward ? (pos < endSearch) : (pos >= endSearch)) {
		char ch = CharAt(pos);
		if (caseSensitive) {
			if (ch == firstChar) {
				bool found = true;
				for (int posMatch = 1; posMatch < lengthFind && found; posMatch++) {
					ch = CharAt(pos + posMatch);
					if (ch != s[posMatch])
						found = false;
				}
				if (found) {
					if ((!word) || IsWordAt(pos, pos + lengthFind))
						return pos;
				}
			}
		} else {
			if (toupper(ch) == firstChar) {
				bool found = true;
				for (int posMatch = 1; posMatch < lengthFind && found; posMatch++) {
					ch = CharAt(pos + posMatch);
					if (toupper(ch) != toupper(s[posMatch]))
						found = false;
				}
				if (found) {
					if ((!word) || IsWordAt(pos, pos + lengthFind))
						return pos;
				}
			}
		}
		pos += increment;
		if (dbcsCodePage) {
			// Ensure trying to match from start of character
			pos = MovePositionOutsideChar(pos, increment, false);
		}
	}
	//Platform::DebugPrintf("Not found\n");
	return - 1;
}

int Document::LinesTotal() {
	return cb.Lines();
}

void Document::ChangeCase(Range r, bool makeUpperCase) {
	for (int pos=r.start; pos<r.end; pos++) {
		char ch = CharAt(pos);
		if (dbcsCodePage && IsDBCS(pos)) {
			pos++;
		} else {
			if (makeUpperCase) {
				if (islower(ch)) {
					ChangeChar(pos, toupper(ch));
				}
			} else {
				if (isupper(ch)) {
					ChangeChar(pos, tolower(ch));
				}
			}
		}
	}
}

void Document::SetWordChars(unsigned char *chars) {
	int ch;
	for (ch = 0; ch < 256; ch++) {
		wordchars[ch] = false;
	}
	if (chars) {
		while (*chars) {
			wordchars[*chars] = true;
			chars++;
		}
	} else {
		for (ch = 0; ch < 256; ch++) {
			wordchars[ch] = isalnum(ch) || ch == '_';
		}
	}
}

void Document::SetStylingBits(int bits) {
	stylingBits = bits;
	stylingBitsMask = 0;
	for (int bit=0; bit<stylingBits; bit++) {
		stylingBitsMask <<= 1;
		stylingBitsMask |= 1;
	}
}

void Document::StartStyling(int position, char mask) {
	stylingPos = position;
	stylingMask = mask;
}

void Document::SetStyleFor(int length, char style) {
	if (enteredCount == 0) {
		enteredCount++;
		int prevEndStyled = endStyled;
		if (cb.SetStyleFor(stylingPos, length, style, stylingMask)) {
			DocModification mh(SC_MOD_CHANGESTYLE | SC_PERFORMED_USER, 
				prevEndStyled, length);
			NotifyModified(mh);
		}
		stylingPos += length;
		endStyled = stylingPos;
		enteredCount--;
	}
}

void Document::SetStyles(int length, char *styles) {
	if (enteredCount == 0) {
		enteredCount++;
		int prevEndStyled = endStyled;
		bool didChange = false;
		for (int iPos = 0; iPos < length; iPos++, stylingPos++) {
			if (cb.SetStyleAt(stylingPos, styles[iPos], stylingMask)) {
				didChange = true;
			}
		}
		endStyled = stylingPos;
		if (didChange) {
			DocModification mh(SC_MOD_CHANGESTYLE | SC_PERFORMED_USER, 
				prevEndStyled, endStyled - prevEndStyled);
			NotifyModified(mh);
		}
		enteredCount--;
	}
}

bool Document::EnsureStyledTo(int pos) {
	// Ask the watchers to style, and stop as soon as one responds.
	for (int i = 0; pos > GetEndStyled() && i < lenWatchers; i++)
		watchers[i].watcher->NotifyStyleNeeded(this, watchers[i].userData, pos);
	return pos <= GetEndStyled();
}

bool Document::AddWatcher(DocWatcher *watcher, void *userData) {
	for (int i = 0; i < lenWatchers; i++) {
		if ((watchers[i].watcher == watcher) &&
		        (watchers[i].userData == userData))
			return false;
	}
	WatcherWithUserData *pwNew = new WatcherWithUserData[lenWatchers + 1];
	if (!pwNew)
		return false;
	for (int j = 0; j < lenWatchers; j++)
		pwNew[j] = watchers[j];
	pwNew[lenWatchers].watcher = watcher;
	pwNew[lenWatchers].userData = userData;
	delete []watchers;
	watchers = pwNew;
	lenWatchers++;
	return true;
}

bool Document::RemoveWatcher(DocWatcher *watcher, void *userData) {
	for (int i = 0; i < lenWatchers; i++) {
		if ((watchers[i].watcher == watcher) &&
		        (watchers[i].userData == userData)) {
			if (lenWatchers == 1) {
				delete []watchers;
				watchers = 0;
				lenWatchers = 0;
			} else {
				WatcherWithUserData *pwNew = new WatcherWithUserData[lenWatchers];
				if (!pwNew)
					return false;
				for (int j = 0; j < lenWatchers - 1; j++) {
					pwNew[j] = (j < i) ? watchers[j] : watchers[j + 1];
				}
				delete []watchers;
				watchers = pwNew;
				lenWatchers--;
			}
			return true;
		}
	}
	return false;
}

void Document::NotifyModifyAttempt() {
	for (int i = 0; i < lenWatchers; i++) {
		watchers[i].watcher->NotifyModifyAttempt(this, watchers[i].userData);
	}
}

void Document::NotifySavePoint(bool atSavePoint) {
	for (int i = 0; i < lenWatchers; i++) {
		watchers[i].watcher->NotifySavePoint(this, watchers[i].userData, atSavePoint);
	}
}

void Document::NotifyModified(DocModification mh) {
	for (int i = 0; i < lenWatchers; i++) {
		watchers[i].watcher->NotifyModified(this, mh, watchers[i].userData);
	}
}
