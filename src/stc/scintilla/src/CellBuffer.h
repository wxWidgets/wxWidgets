// Scintilla source code edit control
/** @file CellBuffer.h
 ** Manages the text of the document.
 **/
// Copyright 1998-2004 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef CELLBUFFER_H
#define CELLBUFFER_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

// Interface to per-line data that wants to see each line insertion and deletion
class PerLine {
public:
	virtual ~PerLine() {}
	virtual void Init()=0;
	virtual void InsertLine(Sci::Line line)=0;
	virtual void RemoveLine(Sci::Line line)=0;
};

/**
 * The line vector contains information about each of the lines in a cell buffer.
 */
class LineVector {

	Partitioning starts;
	PerLine *perLine;

public:

	LineVector();
	// Deleted so LineVector objects can not be copied.
	LineVector(const LineVector &) = delete;
	void operator=(const LineVector &) = delete;
	~LineVector();
	void Init();
	void SetPerLine(PerLine *pl);

	void InsertText(Sci::Line line, Sci::Position delta);
	void InsertLine(Sci::Line line, Sci::Position position, bool lineStart);
	void SetLineStart(Sci::Line line, Sci::Position position);
	void RemoveLine(Sci::Line line);
	Sci::Line Lines() const {
		return starts.Partitions();
	}
	Sci::Line LineFromPosition(Sci::Position pos) const;
	Sci::Position LineStart(Sci::Line line) const {
		return starts.PositionFromPartition(line);
	}
};

enum actionType { insertAction, removeAction, startAction, containerAction };

/**
 * Actions are used to store all the information required to perform one undo/redo step.
 */
class Action {
public:
	actionType at;
	Sci::Position position;
	std::unique_ptr<char[]> data;
	Sci::Position lenData;
	bool mayCoalesce;

	Action();
	// Deleted so Action objects can not be copied.
	Action(const Action &other) = delete;
	Action &operator=(const Action &other) = delete;
	Action &operator=(const Action &&other) = delete;
	// Move constructor allows vector to be resized without reallocating.
	// Could use =default but MSVC 2013 warns.
	Action(Action &&other);
	~Action();
	void Create(actionType at_, Sci::Position position_=0, const char *data_=0, Sci::Position lenData_=0, bool mayCoalesce_=true);
	void Clear();
};

/**
 *
 */
class UndoHistory {
	std::vector<Action> actions;
	int maxAction;
	int currentAction;
	int undoSequenceDepth;
	int savePoint;
	int tentativePoint;

	void EnsureUndoRoom();

public:
	UndoHistory();
	// Deleted so UndoHistory objects can not be copied.
	UndoHistory(const UndoHistory &) = delete;
	void operator=(const UndoHistory &) = delete;
	~UndoHistory();

	const char *AppendAction(actionType at, Sci::Position position, const char *data, Sci::Position lengthData, bool &startSequence, bool mayCoalesce=true);

	void BeginUndoAction();
	void EndUndoAction();
	void DropUndoSequence();
	void DeleteUndoHistory();

	/// The save point is a marker in the undo stack where the container has stated that
	/// the buffer was saved. Undo and redo can move over the save point.
	void SetSavePoint();
	bool IsSavePoint() const;

	// Tentative actions are used for input composition so that it can be undone cleanly
	void TentativeStart();
	void TentativeCommit();
	bool TentativeActive() const { return tentativePoint >= 0; }
	int TentativeSteps();

	/// To perform an undo, StartUndo is called to retrieve the number of steps, then UndoStep is
	/// called that many times. Similarly for redo.
	bool CanUndo() const;
	int StartUndo();
	const Action &GetUndoStep() const;
	void CompletedUndoStep();
	bool CanRedo() const;
	int StartRedo();
	const Action &GetRedoStep() const;
	void CompletedRedoStep();
};

/**
 * Holder for an expandable array of characters that supports undo and line markers.
 * Based on article "Data Structures in a Bit-Mapped Text Editor"
 * by Wilfred J. Hansen, Byte January 1987, page 183.
 */
class CellBuffer {
private:
	SplitVector<char> substance;
	SplitVector<char> style;
	bool readOnly;
	int utf8LineEnds;

	bool collectingUndo;
	UndoHistory uh;

	LineVector lv;

	bool UTF8LineEndOverlaps(Sci::Position position) const;
	void ResetLineEnds();
	/// Actions without undo
	void BasicInsertString(Sci::Position position, const char *s, Sci::Position insertLength);
	void BasicDeleteChars(Sci::Position position, Sci::Position deleteLength);

public:

	CellBuffer();
	// Deleted so CellBuffer objects can not be copied.
	CellBuffer(const CellBuffer &) = delete;
	void operator=(const CellBuffer &) = delete;
	~CellBuffer();

	/// Retrieving positions outside the range of the buffer works and returns 0
	char CharAt(Sci::Position position) const;
	void GetCharRange(char *buffer, Sci::Position position, Sci::Position lengthRetrieve) const;
	char StyleAt(Sci::Position position) const;
	void GetStyleRange(unsigned char *buffer, Sci::Position position, Sci::Position lengthRetrieve) const;
	const char *BufferPointer();
	const char *RangePointer(Sci::Position position, Sci::Position rangeLength);
	Sci::Position GapPosition() const;

	Sci::Position Length() const;
	void Allocate(Sci::Position newSize);
	int GetLineEndTypes() const { return utf8LineEnds; }
	void SetLineEndTypes(int utf8LineEnds_);
	bool ContainsLineEnd(const char *s, Sci::Position length) const;
	void SetPerLine(PerLine *pl);
	Sci::Line Lines() const;
	Sci::Position LineStart(Sci::Line line) const;
	Sci::Line LineFromPosition(Sci::Position pos) const { return lv.LineFromPosition(pos); }
	void InsertLine(Sci::Line line, Sci::Position position, bool lineStart);
	void RemoveLine(Sci::Line line);
	const char *InsertString(Sci::Position position, const char *s, Sci::Position insertLength, bool &startSequence);

	/// Setting styles for positions outside the range of the buffer is safe and has no effect.
	/// @return true if the style of a character is changed.
	bool SetStyleAt(Sci::Position position, char styleValue);
	bool SetStyleFor(Sci::Position position, Sci::Position lengthStyle, char styleValue);

	const char *DeleteChars(Sci::Position position, Sci::Position deleteLength, bool &startSequence);

	bool IsReadOnly() const;
	void SetReadOnly(bool set);

	/// The save point is a marker in the undo stack where the container has stated that
	/// the buffer was saved. Undo and redo can move over the save point.
	void SetSavePoint();
	bool IsSavePoint() const;

	void TentativeStart();
	void TentativeCommit();
	bool TentativeActive() const;
	int TentativeSteps();

	bool SetUndoCollection(bool collectUndo);
	bool IsCollectingUndo() const;
	void BeginUndoAction();
	void EndUndoAction();
	void AddUndoAction(Sci::Position token, bool mayCoalesce);
	void DeleteUndoHistory();

	/// To perform an undo, StartUndo is called to retrieve the number of steps, then UndoStep is
	/// called that many times. Similarly for redo.
	bool CanUndo() const;
	int StartUndo();
	const Action &GetUndoStep() const;
	void PerformUndoStep();
	bool CanRedo() const;
	int StartRedo();
	const Action &GetRedoStep() const;
	void PerformRedoStep();
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
