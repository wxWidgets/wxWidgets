// Scintilla source code edit control
/** @file CellBuffer.h
 ** Manages the text of the document.
 **/
// Copyright 1998-2004 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef CELLBUFFER_H
#define CELLBUFFER_H

/**
 * This holds the marker identifier and the marker type to display.
 * MarkerHandleNumbers are members of lists.
 */
struct MarkerHandleNumber {
	int handle;
	int number;
	MarkerHandleNumber *next;
};

/**
 * A marker handle set contains any number of MarkerHandleNumbers.
 */
class MarkerHandleSet {
	MarkerHandleNumber *root;

public:
	MarkerHandleSet();
	~MarkerHandleSet();
	int Length();
	int NumberFromHandle(int handle);
	int MarkValue();	///< Bit set of marker numbers.
	bool Contains(int handle);
	bool InsertHandle(int handle, int markerNum);
	void RemoveHandle(int handle);
	bool RemoveNumber(int markerNum);
	void CombineWith(MarkerHandleSet *other);
};

/**
 * Each line stores the starting position of the first character of the line in the cell buffer
 * and potentially a marker handle set. Often a line will not have any attached markers.
 */
struct LineData {
	int startPosition;
	MarkerHandleSet *handleSet;
	LineData() : startPosition(0), handleSet(0) {
	}
};

/**
 * The line vector contains information about each of the lines in a cell buffer.
 */
class LineVector {
public:
	int growSize;
	int lines;
	LineData *linesData;
	int size;
	int *levels;
	int sizeLevels;

	/// Handles are allocated sequentially and should never have to be reused as 32 bit ints are very big.
	int handleCurrent;

	LineVector();
	~LineVector();
	void Init();

	void Expand(int sizeNew);
	void ExpandLevels(int sizeNew=-1);
	void ClearLevels();
	void InsertValue(int pos, int value);
	void SetValue(int pos, int value);
	void Remove(int pos);
	int LineFromPosition(int pos);

	int AddMark(int line, int marker);
	void MergeMarkers(int pos);
	void DeleteMark(int line, int markerNum, bool all);
	void DeleteMarkFromHandle(int markerHandle);
	int LineFromHandle(int markerHandle);
};

enum actionType { insertAction, removeAction, startAction };

/**
 * Actions are used to store all the information required to perform one undo/redo step.
 */
class Action {
public:
	actionType at;
	int position;
	char *data;
	int lenData;
	bool mayCoalesce;

	Action();
	~Action();
	void Create(actionType at_, int position_=0, char *data_=0, int lenData_=0, bool mayCoalesce_=true);
	void Destroy();
	void Grab(Action *source);
};

/**
 *
 */
class UndoHistory {
	Action *actions;
	int lenActions;
	int maxAction;
	int currentAction;
	int undoSequenceDepth;
	int savePoint;

	void EnsureUndoRoom();

public:
	UndoHistory();
	~UndoHistory();

	void AppendAction(actionType at, int position, char *data, int length);

	void BeginUndoAction();
	void EndUndoAction();
	void DropUndoSequence();
	void DeleteUndoHistory();

	/// The save point is a marker in the undo stack where the container has stated that
	/// the buffer was saved. Undo and redo can move over the save point.
	void SetSavePoint();
	bool IsSavePoint() const;

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
	char *body;		///< The cell buffer itself.
	int size;		///< Allocated size of the buffer.
	int length;		///< Total length of the data.
	int part1len;	///< Length of the first part.
	int gaplen;		///< Length of the gap between the two parts.
	char *part2body;	///< The second part of the cell buffer.
						///< Doesn't point after the gap but set so that
						///< part2body[position] is consistent with body[position].
	bool readOnly;
	int growSize;

	bool collectingUndo;
	UndoHistory uh;

	LineVector lv;

	SVector lineStates;

	void GapTo(int position);
	void RoomFor(int insertionLength);

	inline char ByteAt(int position);
	void SetByteAt(int position, char ch);

public:

	CellBuffer(int initialLength = 4000);
	~CellBuffer();

	/// Retrieving positions outside the range of the buffer works and returns 0
	char CharAt(int position);
	void GetCharRange(char *buffer, int position, int lengthRetrieve);
	char StyleAt(int position);

	int ByteLength();
	int Length();
	void Allocate(int newSize);
	int Lines();
	int LineStart(int line);
	int LineFromPosition(int pos) { return lv.LineFromPosition(pos); }
	const char *InsertString(int position, char *s, int insertLength);

	/// Setting styles for positions outside the range of the buffer is safe and has no effect.
	/// @return true if the style of a character is changed.
	bool SetStyleAt(int position, char style, char mask='\377');
	bool SetStyleFor(int position, int length, char style, char mask);

	const char *DeleteChars(int position, int deleteLength);

	bool IsReadOnly();
	void SetReadOnly(bool set);

	/// The save point is a marker in the undo stack where the container has stated that
	/// the buffer was saved. Undo and redo can move over the save point.
	void SetSavePoint();
	bool IsSavePoint();

	/// Line marker functions
	int AddMark(int line, int markerNum);
	void DeleteMark(int line, int markerNum);
	void DeleteMarkFromHandle(int markerHandle);
	int GetMark(int line);
	void DeleteAllMarks(int markerNum);
	int LineFromHandle(int markerHandle);

	/// Actions without undo
	void BasicInsertString(int position, char *s, int insertLength);
	void BasicDeleteChars(int position, int deleteLength);

	bool SetUndoCollection(bool collectUndo);
	bool IsCollectingUndo();
	void BeginUndoAction();
	void EndUndoAction();
	void DeleteUndoHistory();

	/// To perform an undo, StartUndo is called to retrieve the number of steps, then UndoStep is
	/// called that many times. Similarly for redo.
	bool CanUndo();
	int StartUndo();
	const Action &GetUndoStep() const;
	void PerformUndoStep();
	bool CanRedo();
	int StartRedo();
	const Action &GetRedoStep() const;
	void PerformRedoStep();

	int SetLineState(int line, int state);
	int GetLineState(int line);
	int GetMaxLineState();

	int SetLevel(int line, int level);
	int GetLevel(int line);
	void ClearLevels();
};

#define CELL_SIZE	2

#endif
