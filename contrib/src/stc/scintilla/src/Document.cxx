// Scintilla source code edit control
/** @file Document.cxx
 ** Text document that handles notifications, DBCS, styling, words and end of line.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
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
#include "RESearch.h"

// This is ASCII specific but is safe with chars >= 0x80
inline bool isspacechar(unsigned char ch) {
	return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

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
	stylingMask = 0;
	for (int ch = 0; ch < 256; ch++) {
		wordchars[ch] = isalnum(ch) || ch == '_';
	}
	endStyled = 0;
	enteredCount = 0;
	enteredReadOnlyCount = 0;
	tabInChars = 8;
	indentInChars = 0;
	useTabs = true;
	tabIndents = true;
	backspaceUnindents = false;
	watchers = 0;
	lenWatchers = 0;

	matchesValid = false;
	pre = 0;
	substituted = 0;
}

Document::~Document() {
	for (int i = 0; i < lenWatchers; i++) {
		watchers[i].watcher->NotifyDeleted(this, watchers[i].userData);
	}
	delete []watchers;
	watchers = 0;
	lenWatchers = 0;
	delete pre;
	pre = 0;
	delete []substituted;
	substituted = 0;
}

// Increase reference count and return its previous value.
int Document::AddRef() {
	return refCount++;
}

// Decrease reference count and return its previous value.
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
		DocModification mh(SC_MOD_CHANGEFOLD | SC_MOD_CHANGEMARKER,
		                   LineStart(line), 0, 0, 0);
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
	while (lineMaxSubord < maxLine - 1) {
		EnsureStyledTo(LineStart(lineMaxSubord + 2));
		if (!IsSubordinate(level, GetLevel(lineMaxSubord + 1)))
			break;
		lineMaxSubord++;
	}
	if (lineMaxSubord > lineParent) {
		if (level > (GetLevel(lineMaxSubord + 1) & SC_FOLDLEVELNUMBERMASK)) {
			// Have chewed up some whitespace that belongs to a parent so seek back
			if (GetLevel(lineMaxSubord) & SC_FOLDLEVELWHITEFLAG) {
				lineMaxSubord--;
			}
		}
	}
	return lineMaxSubord;
}

int Document::GetFoldParent(int line) {
	int level = GetLevel(line);
	int lineLook = line - 1;
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

#if PLAT_WIN
bool Document::IsDBCS(int pos) {
	if (dbcsCodePage) {
		if (SC_CP_UTF8 == dbcsCodePage) {
			unsigned char ch = static_cast<unsigned char>(cb.CharAt(pos));
			return ch >= 0x80;
		} else {
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
	}
	return false;
}
#else
// PLAT_GTK or PLAT_WX
// TODO: support DBCS under GTK+ and WX
bool Document::IsDBCS(int) {
	return false;
}
#endif

int Document::LenChar(int pos) {
	if (IsCrLf(pos)) {
		return 2;
	} else if (SC_CP_UTF8 == dbcsCodePage) {
		unsigned char ch = static_cast<unsigned char>(cb.CharAt(pos));
		if (ch < 0x80)
			return 1;
		int len = 2;
		if (ch >= (0x80 + 0x40 + 0x20))
			len = 3;
		int lengthDoc = Length();
		if ((pos + len) > lengthDoc)
			return lengthDoc -pos;
		else
			return len;
	} else if (IsDBCS(pos)) {
		return 2;
	} else {
		return 1;
	}
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
		if (SC_CP_UTF8 == dbcsCodePage) {
			unsigned char ch = static_cast<unsigned char>(cb.CharAt(pos));
			while ((pos > 0) && (pos < Length()) && (ch >= 0x80) && (ch < (0x80 + 0x40))) {
				// ch is a trail byte
				if (moveDir > 0)
					pos++;
				else
					pos--;
				ch = static_cast<unsigned char>(cb.CharAt(pos));
			}
		} else {
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
	if ((pos + len) > Length())
		return ;
	if (cb.IsReadOnly() && enteredReadOnlyCount == 0) {
		enteredReadOnlyCount++;
		NotifyModifyAttempt();
		enteredReadOnlyCount--;
	}
	if (enteredCount == 0) {
		enteredCount++;
		if (!cb.IsReadOnly()) {
			NotifyModified(
			    DocModification(
			        SC_MOD_BEFOREDELETE | SC_PERFORMED_USER,
			        pos, len,
			        0, 0));
			int prevLinesTotal = LinesTotal();
			bool startSavePoint = cb.IsSavePoint();
			const char *text = cb.DeleteChars(pos * 2, len * 2);
			if (startSavePoint && cb.IsCollectingUndo())
				NotifySavePoint(!startSavePoint);
			if ((pos < Length()) || (pos == 0))
				ModifiedAt(pos);
			else
				ModifiedAt(pos-1);
			NotifyModified(
			    DocModification(
			        SC_MOD_DELETETEXT | SC_PERFORMED_USER,
			        pos, len,
			        LinesTotal() - prevLinesTotal, text));
		}
		enteredCount--;
	}
}

void Document::InsertStyledString(int position, char *s, int insertLength) {
	if (cb.IsReadOnly() && enteredReadOnlyCount == 0) {
		enteredReadOnlyCount++;
		NotifyModifyAttempt();
		enteredReadOnlyCount--;
	}
	if (enteredCount == 0) {
		enteredCount++;
		if (!cb.IsReadOnly()) {
			NotifyModified(
			    DocModification(
			        SC_MOD_BEFOREINSERT | SC_PERFORMED_USER,
			        position / 2, insertLength / 2,
			        0, 0));
			int prevLinesTotal = LinesTotal();
			bool startSavePoint = cb.IsSavePoint();
			const char *text = cb.InsertString(position, s, insertLength);
			if (startSavePoint && cb.IsCollectingUndo())
				NotifySavePoint(!startSavePoint);
			ModifiedAt(position / 2);
			NotifyModified(
			    DocModification(
			        SC_MOD_INSERTTEXT | SC_PERFORMED_USER,
			        position / 2, insertLength / 2,
			        LinesTotal() - prevLinesTotal, text));
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
		for (int step = 0; step < steps; step++) {
			int prevLinesTotal = LinesTotal();
			const Action &action = cb.GetUndoStep();
			if (action.at == removeAction) {
				NotifyModified(DocModification(
				                   SC_MOD_BEFOREINSERT | SC_PERFORMED_UNDO, action));
			} else {
				NotifyModified(DocModification(
				                   SC_MOD_BEFOREDELETE | SC_PERFORMED_UNDO, action));
			}
			cb.PerformUndoStep();
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
			if (step == steps - 1)
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
		for (int step = 0; step < steps; step++) {
			int prevLinesTotal = LinesTotal();
			const Action &action = cb.GetRedoStep();
			if (action.at == insertAction) {
				NotifyModified(DocModification(
				                   SC_MOD_BEFOREINSERT | SC_PERFORMED_REDO, action));
			} else {
				NotifyModified(DocModification(
				                   SC_MOD_BEFOREDELETE | SC_PERFORMED_REDO, action));
			}
			cb.PerformRedoStep();
			ModifiedAt(action.position / 2);
			newPos = action.position / 2;

			int modFlags = SC_PERFORMED_REDO;
			if (action.at == insertAction) {
				newPos += action.lenData;
				modFlags |= SC_MOD_INSERTTEXT;
			} else {
				modFlags |= SC_MOD_DELETETEXT;
			}
			if (step == steps - 1)
				modFlags |= SC_LASTSTEPINUNDOREDO;
			NotifyModified(
			    DocModification(modFlags, action.position / 2, action.lenData,
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
	DeleteChars(pos, LenChar(pos));
}

int Document::DelCharBack(int pos) {
	if (pos <= 0) {
		return pos;
	} else if (IsCrLf(pos - 2)) {
		DeleteChars(pos - 2, 2);
		return pos - 2;
	} else if (SC_CP_UTF8 == dbcsCodePage) {
		int startChar = MovePositionOutsideChar(pos - 1, -1, false);
		DeleteChars(startChar, pos - startChar);
		return startChar;
	} else if (IsDBCS(pos - 1)) {
		DeleteChars(pos - 2, 2);
		return pos - 2;
	} else {
		DeleteChars(pos - 1, 1);
		return pos - 1;
	}
}

static bool isindentchar(char ch) {
	return (ch == ' ') || (ch == '\t');
}

static int NextTab(int pos, int tabSize) {
	return ((pos / tabSize) + 1) * tabSize;
}

static void CreateIndentation(char *linebuf, int length, int indent, int tabSize, bool insertSpaces) {
	length--;	// ensure space for \0
	if (!insertSpaces) {
		while ((indent >= tabSize) && (length > 0)) {
			*linebuf++ = '\t';
			indent -= tabSize;
			length--;
		}
	}
	while ((indent > 0) && (length > 0)) {
		*linebuf++ = ' ';
		indent--;
		length--;
	}
	*linebuf = '\0';
}

int Document::GetLineIndentation(int line) {
	int indent = 0;
	if ((line >= 0) && (line < LinesTotal())) {
		int lineStart = LineStart(line);
		int length = Length();
		for (int i = lineStart;i < length;i++) {
			char ch = cb.CharAt(i);
			if (ch == ' ')
				indent++;
			else if (ch == '\t')
				indent = NextTab(indent, tabInChars);
			else
				return indent;
		}
	}
	return indent;
}

void Document::SetLineIndentation(int line, int indent) {
	int indentOfLine = GetLineIndentation(line);
	if (indent < 0)
		indent = 0;
	if (indent != indentOfLine) {
		char linebuf[1000];
		CreateIndentation(linebuf, sizeof(linebuf), indent, tabInChars, !useTabs);
		int thisLineStart = LineStart(line);
		int indentPos = GetLineIndentPosition(line);
		DeleteChars(thisLineStart, indentPos - thisLineStart);
		InsertString(thisLineStart, linebuf);
	}
}

int Document::GetLineIndentPosition(int line) {
	if (line < 0)
		return 0;
	int pos = LineStart(line);
	int length = Length();
	while ((pos < length) && isindentchar(cb.CharAt(pos))) {
		pos++;
	}
	return pos;
}

int Document::GetColumn(int pos) {
	int column = 0;
	int line = LineFromPosition(pos);
	if ((line >= 0) && (line < LinesTotal())) {
		for (int i = LineStart(line);i < pos;i++) {
			char ch = cb.CharAt(i);
			if (ch == '\t')
				column = NextTab(column, tabInChars);
			else if (ch == '\r')
				return column;
			else if (ch == '\n')
				return column;
			else
				column++;
		}
	}
	return column;
}

void Document::Indent(bool forwards, int lineBottom, int lineTop) {
	// Dedent - suck white space off the front of the line to dedent by equivalent of a tab
	for (int line = lineBottom; line >= lineTop; line--) {
		int indentOfLine = GetLineIndentation(line);
		if (forwards)
			SetLineIndentation(line, indentOfLine + IndentSize());
		else
			SetLineIndentation(line, indentOfLine - IndentSize());
	}
}

void Document::ConvertLineEnds(int eolModeSet) {
	BeginUndoAction();
	for (int pos = 0; pos < Length(); pos++) {
		if (cb.CharAt(pos) == '\r') {
			if (cb.CharAt(pos + 1) == '\n') {
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
	if ((SC_CP_UTF8 == dbcsCodePage) && (ch > 0x80))
		return true;
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
		if (isspacechar(cb.CharAt(pos - 1))) {	// Back up to previous line
			while (pos > 0 && isspacechar(cb.CharAt(pos - 1)))
				pos--;
		} else {
			bool startAtWordChar = IsWordChar(cb.CharAt(pos - 1));
			while (pos > 0 && !isspacechar(cb.CharAt(pos - 1)) && (startAtWordChar == IsWordChar(cb.CharAt(pos - 1))))
				pos--;
		}
	} else {
		bool startAtWordChar = IsWordChar(cb.CharAt(pos));
		while (pos < (Length()) && isspacechar(cb.CharAt(pos)))
			pos++;
		while (pos < (Length()) && !isspacechar(cb.CharAt(pos)) && (startAtWordChar == IsWordChar(cb.CharAt(pos))))
			pos++;
		while (pos < (Length()) && (cb.CharAt(pos) == ' ' || cb.CharAt(pos) == '\t'))
			pos++;
	}
	return pos;
}

/**
 * Check that the character before the given position
 * is not a word character.
 */
bool Document::IsWordStartAt(int pos) {
	if (pos > 0) {
		return !IsWordChar(CharAt(pos - 1));
	}
	return true;
}

/**
 * Check that the character after the given position
 * is not a word character.
 */
bool Document::IsWordEndAt(int pos) {
	if (pos < Length() - 1) {
		return !IsWordChar(CharAt(pos));
	}
	return true;
}

/**
 * Check that the given range is delimited by
 * non word characters.
 */
bool Document::IsWordAt(int start, int end) {
	return IsWordStartAt(start) && IsWordEndAt(end);
}

// The comparison and case changing functions here assume ASCII
// or extended ASCII such as the normal Windows code page.

static inline char MakeUpperCase(char ch) {
	if (ch < 'a' || ch > 'z')
		return ch;
	else
		return static_cast<char>(ch - 'a' + 'A');
}

static inline char MakeLowerCase(char ch) {
	if (ch < 'A' || ch > 'Z')
		return ch;
	else
		return static_cast<char>(ch - 'A' + 'a');
}

// Define a way for the Regular Expression code to access the document
class DocumentIndexer : public CharacterIndexer {
	Document *pdoc;
	int end;
public:
DocumentIndexer(Document *pdoc_, int end_) :
	pdoc(pdoc_), end(end_) {}

	virtual char CharAt(int index) {
		if (index < 0 || index >= end)
			return 0;
		else
			return pdoc->CharAt(index);
	}
};

/**
 * Find text in document, supporting both forward and backward
 * searches (just pass minPos > maxPos to do a backward search)
 * Has not been tested with backwards DBCS searches yet.
 */
long Document::FindText(int minPos, int maxPos, const char *s,
                        bool caseSensitive, bool word, bool wordStart, bool regExp,
                        int *length) {
	if (regExp) {
		if (!pre)
			pre = new RESearch();
		if (!pre)
			return -1;

		int startPos;
		int endPos;

		if (minPos <= maxPos) {
			startPos = minPos;
			endPos = maxPos;
		} else {
			startPos = maxPos;
			endPos = minPos;
		}

		// Range endpoints should not be inside DBCS characters, but just in case, move them.
		startPos = MovePositionOutsideChar(startPos, 1, false);
		endPos = MovePositionOutsideChar(endPos, 1, false);

		const char *errmsg = pre->Compile(s, *length, caseSensitive);
		if (errmsg) {
			return -1;
		}
		// Find a variable in a property file: \$(\([A-Za-z0-9_.]+\))
		// Replace first '.' with '-' in each property file variable reference:
		//     Search: \$(\([A-Za-z0-9_-]+\)\.\([A-Za-z0-9_.]+\))
		//     Replace: $(\1-\2)
		int lineRangeStart = LineFromPosition(startPos);
		int lineRangeEnd = LineFromPosition(endPos);
		if ((startPos >= LineEnd(lineRangeStart)) && (lineRangeStart < lineRangeEnd)) {
			// the start position is at end of line or between line end characters.
			lineRangeStart++;
			startPos = LineStart(lineRangeStart);
		}
		int pos = -1;
		int lenRet = 0;
		char searchEnd = s[*length - 1];
		if (*length == 1) {
			// These produce empty selections so nudge them on if needed
			if (s[0] == '^') {
				if (startPos == LineStart(lineRangeStart))
					startPos++;
			} else if (s[0] == '$') {
				if ((startPos == LineEnd(lineRangeStart)) && (lineRangeStart < lineRangeEnd))
					startPos = LineStart(lineRangeStart + 1);
			}
			lineRangeStart = LineFromPosition(startPos);
			lineRangeEnd = LineFromPosition(endPos);
		}
		for (int line = lineRangeStart; line <= lineRangeEnd; line++) {
			int startOfLine = LineStart(line);
			int endOfLine = LineEnd(line);
			if (line == lineRangeStart) {
				if ((startPos != startOfLine) && (s[0] == '^'))
					continue;	// Can't match start of line if start position after start of line
				startOfLine = startPos;
			}
			if (line == lineRangeEnd) {
				if ((endPos != endOfLine) && (searchEnd == '$'))
					continue;	// Can't match end of line if end position before end of line
				endOfLine = endPos;
			}
			DocumentIndexer di(this, endOfLine);
			int success = pre->Execute(di, startOfLine, endOfLine);
			if (success) {
				pos = pre->bopat[0];
				lenRet = pre->eopat[0] - pre->bopat[0];
				break;
			}
		}
		*length = lenRet;
		return pos;

	} else {

		bool forward = minPos <= maxPos;
		int increment = forward ? 1 : -1;

		// Range endpoints should not be inside DBCS characters, but just in case, move them.
		int startPos = MovePositionOutsideChar(minPos, increment, false);
		int endPos = MovePositionOutsideChar(maxPos, increment, false);

		// Compute actual search ranges needed
		int lengthFind = *length;
		if (lengthFind == -1)
			lengthFind = strlen(s);
		int endSearch = endPos;
		if (startPos <= endPos) {
			endSearch = endPos - lengthFind + 1;
		}
		//Platform::DebugPrintf("Find %d %d %s %d\n", startPos, endPos, ft->lpstrText, lengthFind);
		char firstChar = s[0];
		if (!caseSensitive)
			firstChar = static_cast<char>(MakeUpperCase(firstChar));
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
						if ((!word && !wordStart) ||
						        word && IsWordAt(pos, pos + lengthFind) ||
						        wordStart && IsWordStartAt(pos))
							return pos;
					}
				}
			} else {
				if (MakeUpperCase(ch) == firstChar) {
					bool found = true;
					for (int posMatch = 1; posMatch < lengthFind && found; posMatch++) {
						ch = CharAt(pos + posMatch);
						if (MakeUpperCase(ch) != MakeUpperCase(s[posMatch]))
							found = false;
					}
					if (found) {
						if ((!word && !wordStart) ||
						        word && IsWordAt(pos, pos + lengthFind) ||
						        wordStart && IsWordStartAt(pos))
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
	}
	//Platform::DebugPrintf("Not found\n");
	return -1;
}

const char *Document::SubstituteByPosition(const char *text, int *length) {
	if (!pre)
		return 0;
	delete []substituted;
	substituted = 0;
	DocumentIndexer di(this, Length());
	if (!pre->GrabMatches(di))
		return 0;
	unsigned int lenResult = 0;
	for (int i = 0; i < *length; i++) {
		if ((text[i] == '\\') && (text[i + 1] >= '1' && text[i + 1] <= '9')) {
			unsigned int patNum = text[i + 1] - '0';
			lenResult += pre->eopat[patNum] - pre->bopat[patNum];
			i++;
		} else {
			lenResult++;
		}
	}
	substituted = new char[lenResult + 1];
	if (!substituted)
		return 0;
	char *o = substituted;
	for (int j = 0; j < *length; j++) {
		if ((text[j] == '\\') && (text[j + 1] >= '1' && text[j + 1] <= '9')) {
			unsigned int patNum = text[j + 1] - '0';
			unsigned int len = pre->eopat[patNum] - pre->bopat[patNum];
			if (pre->pat[patNum])	// Will be null if try for a match that did not occur
				memcpy(o, pre->pat[patNum], len);
			o += len;
			j++;
		} else {
			*o++ = text[j];
		}
	}
	*o = '\0';
	*length = lenResult;
	return substituted;
}

int Document::LinesTotal() {
	return cb.Lines();
}

void Document::ChangeCase(Range r, bool makeUpperCase) {
	for (int pos = r.start; pos < r.end; pos++) {
		char ch = CharAt(pos);
		if (dbcsCodePage && IsDBCS(pos)) {
			pos += LenChar(pos);
		} else {
			if (makeUpperCase) {
				if (islower(ch)) {
					ChangeChar(pos, static_cast<char>(MakeUpperCase(ch)));
				}
			} else {
				if (isupper(ch)) {
					ChangeChar(pos, static_cast<char>(MakeLowerCase(ch)));
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
	for (int bit = 0; bit < stylingBits; bit++) {
		stylingBitsMask <<= 1;
		stylingBitsMask |= 1;
	}
}

void Document::StartStyling(int position, char mask) {
	stylingMask = mask;
	endStyled = position;
}

void Document::SetStyleFor(int length, char style) {
	if (enteredCount == 0) {
		enteredCount++;
		int prevEndStyled = endStyled;
		if (cb.SetStyleFor(endStyled, length, style, stylingMask)) {
			DocModification mh(SC_MOD_CHANGESTYLE | SC_PERFORMED_USER,
			                   prevEndStyled, length);
			NotifyModified(mh);
		}
		endStyled += length;
		enteredCount--;
	}
}

void Document::SetStyles(int length, char *styles) {
	if (enteredCount == 0) {
		enteredCount++;
		int prevEndStyled = endStyled;
		bool didChange = false;
		for (int iPos = 0; iPos < length; iPos++, endStyled++) {
			if (cb.SetStyleAt(endStyled, styles[iPos], stylingMask)) {
				didChange = true;
			}
		}
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

bool Document::IsWordPartSeparator(char ch) {
	return ispunct(ch) && IsWordChar(ch);
}

int Document::WordPartLeft(int pos) {
	if (pos > 0) {
		--pos;
		char startChar = cb.CharAt(pos);
		if (IsWordPartSeparator(startChar)) {
			while (pos > 0 && IsWordPartSeparator(cb.CharAt(pos))) {
				--pos;
			}
		}
		if (pos > 0) {
			startChar = cb.CharAt(pos);
			--pos;
			if (islower(startChar)) {
				while (pos > 0 && islower(cb.CharAt(pos)))
					--pos;
				if (!isupper(cb.CharAt(pos)) && !islower(cb.CharAt(pos)))
					++pos;
			} else if (isupper(startChar)) {
				while (pos > 0 && isupper(cb.CharAt(pos)))
					--pos;
				if (!isupper(cb.CharAt(pos)))
					++pos;
			} else if (isdigit(startChar)) {
				while (pos > 0 && isdigit(cb.CharAt(pos)))
					--pos;
				if (!isdigit(cb.CharAt(pos)))
					++pos;
			} else if (ispunct(startChar)) {
				while (pos > 0 && ispunct(cb.CharAt(pos)))
					--pos;
				if (!ispunct(cb.CharAt(pos)))
					++pos;
			} else if (isspacechar(startChar)) {
				while (pos > 0 && isspacechar(cb.CharAt(pos)))
					--pos;
				if (!isspacechar(cb.CharAt(pos)))
					++pos;
			}
		}
	}
	return pos;
}

int Document::WordPartRight(int pos) {
	char startChar = cb.CharAt(pos);
	int length = Length();
	if (IsWordPartSeparator(startChar)) {
		while (pos < length && IsWordPartSeparator(cb.CharAt(pos)))
			++pos;
		startChar = cb.CharAt(pos);
	}
	if (islower(startChar)) {
		while (pos < length && islower(cb.CharAt(pos)))
			++pos;
	} else if (isupper(startChar)) {
		if (islower(cb.CharAt(pos + 1))) {
			++pos;
			while (pos < length && islower(cb.CharAt(pos)))
				++pos;
		} else {
			while (pos < length && isupper(cb.CharAt(pos)))
				++pos;
		}
		if (islower(cb.CharAt(pos)) && isupper(cb.CharAt(pos - 1)))
			--pos;
	} else if (isdigit(startChar)) {
		while (pos < length && isdigit(cb.CharAt(pos)))
			++pos;
	} else if (ispunct(startChar)) {
		while (pos < length && ispunct(cb.CharAt(pos)))
			++pos;
	} else if (isspacechar(startChar)) {
		while (pos < length && isspacechar(cb.CharAt(pos)))
			++pos;
	}
	return pos;
}
