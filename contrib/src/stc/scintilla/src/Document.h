// Scintilla source code edit control
/** @file Document.h
 ** Text document that handles notifications, DBCS, styling, words and end of line.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef DOCUMENT_H
#define DOCUMENT_H

/**
 * A Position is a position within a document between two characters or at the beginning or end.
 * Sometimes used as a character index where it identifies the character after the position.
 */
typedef int Position;
const Position invalidPosition = -1;

/**
 * The range class represents a range of text in a document.
 * The two values are not sorted as one end may be more significant than the other
 * as is the case for the selection where the end position is the position of the caret.
 * If either position is invalidPosition then the range is invalid and most operations will fail.
 */
class Range {
public:
	Position start;
	Position end;

	Range(Position pos=0) : 
		start(pos), end(pos) {
	};
	Range(Position start_, Position end_) : 
		start(start_), end(end_) {
	};

	bool Valid() const {
		return (start != invalidPosition) && (end != invalidPosition);
	}

	bool Contains(Position pos) const {
		if (start < end) {
			return (pos >= start && pos <= end);
		} else {
			return (pos <= start && pos >= end);
		}
	}

	bool Contains(Range other) const {
		return Contains(other.start) && Contains(other.end);
	}

	bool Overlaps(Range other) const {
		return 
		Contains(other.start) ||
		Contains(other.end) ||
		other.Contains(start) ||
		other.Contains(end);
	}
};

class DocWatcher;
class DocModification;
class RESearch;

/**
 */
class Document {

public:
	/** Used to pair watcher pointer with user data. */
	class WatcherWithUserData {
	public:
		DocWatcher *watcher;
		void *userData;
		WatcherWithUserData() {
			watcher = 0;
			userData = 0;
		}
	};

private:	
	int refCount;
	CellBuffer cb;
	bool wordchars[256];
	char stylingMask;
	int endStyled;
	int enteredCount;
	int enteredReadOnlyCount;

	WatcherWithUserData *watchers;
	int lenWatchers;

	bool matchesValid;
	RESearch *pre;
	char *substituted;

public:
	int stylingBits;
	int stylingBitsMask;

	int eolMode;
	/// Can also be SC_CP_UTF8 to enable UTF-8 mode
	int dbcsCodePage;
	int tabInChars;
	int indentInChars;
	bool useTabs;
	bool tabIndents;
	bool backspaceUnindents;

	Document();
	virtual ~Document();

	int AddRef();
	int Release();

	int LineFromPosition(int pos);
	int ClampPositionIntoDocument(int pos);
	bool IsCrLf(int pos);
	int LenChar(int pos);
	int MovePositionOutsideChar(int pos, int moveDir, bool checkLineEnd=true);

	// Gateways to modifying document
	void DeleteChars(int pos, int len);
	void InsertStyledString(int position, char *s, int insertLength);
	int Undo();
	int Redo();
	bool CanUndo() { return cb.CanUndo(); }
	bool CanRedo() { return cb.CanRedo(); }
	void DeleteUndoHistory() { cb.DeleteUndoHistory(); }
	bool SetUndoCollection(bool collectUndo) {
		return cb.SetUndoCollection(collectUndo);
	}
	bool IsCollectingUndo() { return cb.IsCollectingUndo(); }
	void BeginUndoAction() { cb.BeginUndoAction(); }
	void EndUndoAction() { cb.EndUndoAction(); }
	void SetSavePoint();
	bool IsSavePoint() { return cb.IsSavePoint(); }

	int GetLineIndentation(int line);
	void SetLineIndentation(int line, int indent);
	int GetLineIndentPosition(int line);
	int GetColumn(int position);
	void Indent(bool forwards, int lineBottom, int lineTop);
	void ConvertLineEnds(int eolModeSet);
	void SetReadOnly(bool set) { cb.SetReadOnly(set); }
	bool IsReadOnly() { return cb.IsReadOnly(); }

	void InsertChar(int pos, char ch);
	void InsertString(int position, const char *s);
	void InsertString(int position, const char *s, int insertLength);
	void ChangeChar(int pos, char ch);
	void DelChar(int pos);
	int DelCharBack(int pos);

	char CharAt(int position) { return cb.CharAt(position); }
	void GetCharRange(char *buffer, int position, int lengthRetrieve) {
		cb.GetCharRange(buffer, position, lengthRetrieve);
	}
	char StyleAt(int position) { return cb.StyleAt(position); }
	int GetMark(int line) { return cb.GetMark(line); }
	int AddMark(int line, int markerNum);
	void DeleteMark(int line, int markerNum);
	void DeleteMarkFromHandle(int markerHandle);
	void DeleteAllMarks(int markerNum);
	int LineFromHandle(int markerHandle) { return cb.LineFromHandle(markerHandle); }
	int LineStart(int line);
	int LineEnd(int line);
	int LineEndPosition(int position);
	int VCHomePosition(int position);

	int SetLevel(int line, int level);
	int GetLevel(int line) { return cb.GetLevel(line); }
	void ClearLevels() { cb.ClearLevels(); }
	int GetLastChild(int lineParent, int level=-1);
	int GetFoldParent(int line);

	void Indent(bool forwards);
	int ExtendWordSelect(int pos, int delta);
	int NextWordStart(int pos, int delta);
	int Length() { return cb.Length(); }
	long FindText(int minPos, int maxPos, const char *s, 
		bool caseSensitive, bool word, bool wordStart, bool regExp, int *length);
	long FindText(int iMessage, unsigned long wParam, long lParam);
	const char *SubstituteByPosition(const char *text, int *length);
	int LinesTotal();

	void ChangeCase(Range r, bool makeUpperCase);

	void SetWordChars(unsigned char *chars);
	void SetStylingBits(int bits);
	void StartStyling(int position, char mask);
	void SetStyleFor(int length, char style);
	void SetStyles(int length, char *styles);
	int GetEndStyled() { return endStyled; }
	bool EnsureStyledTo(int pos);

	int SetLineState(int line, int state) { return cb.SetLineState(line, state); }
	int GetLineState(int line) { return cb.GetLineState(line); }
	int GetMaxLineState() { return cb.GetMaxLineState(); }

	bool AddWatcher(DocWatcher *watcher, void *userData);
	bool RemoveWatcher(DocWatcher *watcher, void *userData);
	const WatcherWithUserData *GetWatchers() const { return watchers; }
	int GetLenWatchers() const { return lenWatchers; }

	bool IsWordPartSeparator(char ch);
	int WordPartLeft(int pos);
	int WordPartRight(int pos);

private:
	bool IsDBCS(int pos);
	bool IsWordChar(unsigned char ch);
	bool IsWordStartAt(int pos);
	bool IsWordEndAt(int pos);
	bool IsWordAt(int start, int end);
	void ModifiedAt(int pos);

	void NotifyModifyAttempt();
	void NotifySavePoint(bool atSavePoint);
	void NotifyModified(DocModification mh);

	int IndentSize() { return indentInChars ? indentInChars : tabInChars; }
};

/**
 * To optimise processing of document modifications by DocWatchers, a hint is passed indicating the
 * scope of the change.
 * If the DocWatcher is a document view then this can be used to optimise screen updating.
 */
class DocModification {
public:
  	int modificationType;
	int position;
 	int length;
 	int linesAdded;	/**< Negative if lines deleted. */
 	const char *text;	/**< Only valid for changes to text, not for changes to style. */
 	int line;
	int foldLevelNow;
	int foldLevelPrev;

	DocModification(int modificationType_, int position_=0, int length_=0, 
		int linesAdded_=0, const char *text_=0) :
		modificationType(modificationType_),
		position(position_),
		length(length_),
		linesAdded(linesAdded_),
		text(text_),
		line(0),
		foldLevelNow(0),
		foldLevelPrev(0) {}

    DocModification(int modificationType_, const Action &act, int linesAdded_=0) :
		modificationType(modificationType_),
		position(act.position / 2),
		length(act.lenData),
		linesAdded(linesAdded_),
		text(act.data),
		line(0),
		foldLevelNow(0),
		foldLevelPrev(0) {}
};

/**
 * A class that wants to receive notifications from a Document must be derived from DocWatcher
 * and implement the notification methods. It can then be added to the watcher list with AddWatcher.
 */
class DocWatcher {
public:
	virtual ~DocWatcher() {}

	virtual void NotifyModifyAttempt(Document *doc, void *userData) = 0;
	virtual void NotifySavePoint(Document *doc, void *userData, bool atSavePoint) = 0;
	virtual void NotifyModified(Document *doc, DocModification mh, void *userData) = 0;
	virtual void NotifyDeleted(Document *doc, void *userData) = 0;
	virtual void NotifyStyleNeeded(Document *doc, void *userData, int endPos) = 0;
};

#endif
