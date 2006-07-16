// Scintilla source code edit control
/** @file CellBuffer.cxx
 ** Manages a buffer of cells.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "Platform.h"

#include "Scintilla.h"
#include "SVector.h"
#include "CellBuffer.h"

MarkerHandleSet::MarkerHandleSet() {
	root = 0;
}

MarkerHandleSet::~MarkerHandleSet() {
	MarkerHandleNumber *mhn = root;
	while (mhn) {
		MarkerHandleNumber *mhnToFree = mhn;
		mhn = mhn->next;
		delete mhnToFree;
	}
	root = 0;
}

int MarkerHandleSet::Length() {
	int c = 0;
	MarkerHandleNumber *mhn = root;
	while (mhn) {
		c++;
		mhn = mhn->next;
	}
	return c;
}

int MarkerHandleSet::NumberFromHandle(int handle) {
	MarkerHandleNumber *mhn = root;
	while (mhn) {
		if (mhn->handle == handle) {
			return mhn->number;
		}
		mhn = mhn->next;
	}
	return - 1;
}

int MarkerHandleSet::MarkValue() {
	unsigned int m = 0;
	MarkerHandleNumber *mhn = root;
	while (mhn) {
		m |= (1 << mhn->number);
		mhn = mhn->next;
	}
	return m;
}

bool MarkerHandleSet::Contains(int handle) {
	MarkerHandleNumber *mhn = root;
	while (mhn) {
		if (mhn->handle == handle) {
			return true;
		}
		mhn = mhn->next;
	}
	return false;
}

bool MarkerHandleSet::InsertHandle(int handle, int markerNum) {
	MarkerHandleNumber *mhn = new MarkerHandleNumber;
	if (!mhn)
		return false;
	mhn->handle = handle;
	mhn->number = markerNum;
	mhn->next = root;
	root = mhn;
	return true;
}

void MarkerHandleSet::RemoveHandle(int handle) {
	MarkerHandleNumber **pmhn = &root;
	while (*pmhn) {
		MarkerHandleNumber *mhn = *pmhn;
		if (mhn->handle == handle) {
			*pmhn = mhn->next;
			delete mhn;
			return ;
		}
		pmhn = &((*pmhn)->next);
	}
}

bool MarkerHandleSet::RemoveNumber(int markerNum) {
	bool performedDeletion = false;
	MarkerHandleNumber **pmhn = &root;
	while (*pmhn) {
		MarkerHandleNumber *mhn = *pmhn;
		if (mhn->number == markerNum) {
			*pmhn = mhn->next;
			delete mhn;
			performedDeletion = true;
		} else {
			pmhn = &((*pmhn)->next);
		}
	}
	return performedDeletion;
}

void MarkerHandleSet::CombineWith(MarkerHandleSet *other) {
	MarkerHandleNumber **pmhn = &root;
	while (*pmhn) {
		pmhn = &((*pmhn)->next);
	}
	*pmhn = other->root;
	other->root = 0;
}

LineVector::LineVector() {
	linesData = 0;
	lines = 0;
	size = 0;
	levels = 0;
	sizeLevels = 0;
	handleCurrent = 1;
	growSize = 1000;

	Init();
}

LineVector::~LineVector() {
	for (int line = 0; line < lines; line++) {
		delete linesData[line].handleSet;
		linesData[line].handleSet = 0;
	}
	delete []linesData;
	linesData = 0;
	delete []levels;
	levels = 0;
}

void LineVector::Init() {
	for (int line = 0; line < lines; line++) {
		delete linesData[line].handleSet;
		linesData[line].handleSet = 0;
	}
	delete []linesData;
	linesData = new LineData[static_cast<int>(growSize)];
	size = growSize;
	lines = 1;
	delete []levels;
	levels = 0;
	sizeLevels = 0;
}

void LineVector::Expand(int sizeNew) {
	LineData *linesDataNew = new LineData[sizeNew];
	if (linesDataNew) {
		for (int i = 0; i < size; i++)
			linesDataNew[i] = linesData[i];
		// Do not delete handleSets here as they are transferred to new linesData
		delete []linesData;
		linesData = linesDataNew;
		size = sizeNew;
	} else {
		Platform::DebugPrintf("No memory available\n");
		// TODO: Blow up
	}

}

void LineVector::ExpandLevels(int sizeNew) {
	if (sizeNew == -1)
		sizeNew = size;
	int *levelsNew = new int[sizeNew];
	if (levelsNew) {
		int i = 0;
		for (; i < sizeLevels; i++)
			levelsNew[i] = levels[i];
		for (; i < sizeNew; i++)
			levelsNew[i] = SC_FOLDLEVELBASE;
		delete []levels;
		levels = levelsNew;
		sizeLevels = sizeNew;
	} else {
		Platform::DebugPrintf("No memory available\n");
		// TODO: Blow up
	}

}

void LineVector::ClearLevels() {
	delete []levels;
	levels = 0;
	sizeLevels = 0;
}

void LineVector::InsertValue(int pos, int value) {
	//Platform::DebugPrintf("InsertValue[%d] = %d\n", pos, value);
	if ((lines + 2) >= size) {
		if (growSize * 6 < size)
			growSize *= 2;
		Expand(size + growSize);
		if (levels) {
			ExpandLevels(size + growSize);
		}
	}
	lines++;
	for (int i = lines; i > pos; i--) {
		linesData[i] = linesData[i - 1];
	}
	linesData[pos].startPosition = value;
	linesData[pos].handleSet = 0;
	if (levels) {
		for (int j = lines; j > pos; j--) {
			levels[j] = levels[j - 1];
		}
		if (pos == 0) {
			levels[pos] = SC_FOLDLEVELBASE;
		} else if (pos == (lines - 1)) {	// Last line will not be a folder
			levels[pos] = SC_FOLDLEVELBASE;
		} else {
			levels[pos] = levels[pos - 1];
		}
	}
}

void LineVector::SetValue(int pos, int value) {
	//Platform::DebugPrintf("SetValue[%d] = %d\n", pos, value);
	if ((pos + 2) >= size) {
		//Platform::DebugPrintf("Resize %d %d\n", size,pos);
		Expand(pos + growSize);
		//Platform::DebugPrintf("end Resize %d %d\n", size,pos);
		lines = pos;
		if (levels) {
			ExpandLevels(pos + growSize);
		}
	}
	linesData[pos].startPosition = value;
}

void LineVector::Remove(int pos) {
	//Platform::DebugPrintf("Remove %d\n", pos);
	// Retain the markers from the deleted line by oring them into the previous line
	if (pos > 0) {
		MergeMarkers(pos - 1);
	}
	for (int i = pos; i < lines; i++) {
		linesData[i] = linesData[i + 1];
	}
	if (levels) {
		// Move up following lines but merge header flag from this line
		// to line before to avoid a temporary disappearence causing expansion.
		int firstHeader = levels[pos] & SC_FOLDLEVELHEADERFLAG;
		for (int j = pos; j < lines; j++) {
			levels[j] = levels[j + 1];
		}
		if (pos > 0)
			levels[pos-1] |= firstHeader;
	}
	lines--;
}

int LineVector::LineFromPosition(int pos) {
	//Platform::DebugPrintf("LineFromPostion %d lines=%d end = %d\n", pos, lines, linesData[lines].startPosition);
	if (lines == 0)
		return 0;
	//Platform::DebugPrintf("LineFromPosition %d\n", pos);
	if (pos >= linesData[lines].startPosition)
		return lines - 1;
	int lower = 0;
	int upper = lines;
	do {
		int middle = (upper + lower + 1) / 2; 	// Round high
		if (pos < linesData[middle].startPosition) {
			upper = middle - 1;
		} else {
			lower = middle;
		}
	} while (lower < upper);
	//Platform::DebugPrintf("LineFromPostion %d %d %d\n", pos, lower, linesData[lower].startPosition, linesData[lower > 1 ? lower - 1 : 0].startPosition);
	return lower;
}

int LineVector::AddMark(int line, int markerNum) {
	handleCurrent++;
	if (!linesData[line].handleSet) {
		// Need new structure to hold marker handle
		linesData[line].handleSet = new MarkerHandleSet;
		if (!linesData[line].handleSet)
			return - 1;
	}
	linesData[line].handleSet->InsertHandle(handleCurrent, markerNum);

	return handleCurrent;
}

void LineVector::MergeMarkers(int pos) {
	if (linesData[pos + 1].handleSet != NULL) {
		if (linesData[pos].handleSet == NULL )
			linesData[pos].handleSet = new MarkerHandleSet;
		linesData[pos].handleSet->CombineWith(linesData[pos + 1].handleSet);
		delete linesData[pos + 1].handleSet;
		linesData[pos + 1].handleSet = NULL;
	}
}

void LineVector::DeleteMark(int line, int markerNum, bool all) {
	if (linesData[line].handleSet) {
		if (markerNum == -1) {
			delete linesData[line].handleSet;
			linesData[line].handleSet = 0;
		} else {
			bool performedDeletion = 
				linesData[line].handleSet->RemoveNumber(markerNum);
			while (all && performedDeletion) {
				performedDeletion = 
					linesData[line].handleSet->RemoveNumber(markerNum);
			}
			if (linesData[line].handleSet->Length() == 0) {
				delete linesData[line].handleSet;
				linesData[line].handleSet = 0;
			}
		}
	}
}

void LineVector::DeleteMarkFromHandle(int markerHandle) {
	int line = LineFromHandle(markerHandle);
	if (line >= 0) {
		linesData[line].handleSet->RemoveHandle(markerHandle);
		if (linesData[line].handleSet->Length() == 0) {
			delete linesData[line].handleSet;
			linesData[line].handleSet = 0;
		}
	}
}

int LineVector::LineFromHandle(int markerHandle) {
	for (int line = 0; line < lines; line++) {
		if (linesData[line].handleSet) {
			if (linesData[line].handleSet->Contains(markerHandle)) {
				return line;
			}
		}
	}
	return - 1;
}

Action::Action() {
	at = startAction;
	position = 0;
	data = 0;
	lenData = 0;
}

Action::~Action() {
	Destroy();
}

void Action::Create(actionType at_, int position_, char *data_, int lenData_, bool mayCoalesce_) {
	delete []data;
	position = position_;
	at = at_;
	data = data_;
	lenData = lenData_;
	mayCoalesce = mayCoalesce_;
}

void Action::Destroy() {
	delete []data;
	data = 0;
}

void Action::Grab(Action *source) {
	delete []data;

	position = source->position;
	at = source->at;
	data = source->data;
	lenData = source->lenData;
	mayCoalesce = source->mayCoalesce;

	// Ownership of source data transferred to this
	source->position = 0;
	source->at = startAction;
	source->data = 0;
	source->lenData = 0;
	source->mayCoalesce = true;
}

// The undo history stores a sequence of user operations that represent the user's view of the
// commands executed on the text.
// Each user operation contains a sequence of text insertion and text deletion actions.
// All the user operations are stored in a list of individual actions with 'start' actions used
// as delimiters between user operations.
// Initially there is one start action in the history.
// As each action is performed, it is recorded in the history. The action may either become
// part of the current user operation or may start a new user operation. If it is to be part of the
// current operation, then it overwrites the current last action. If it is to be part of a new
// operation, it is appended after the current last action.
// After writing the new action, a new start action is appended at the end of the history.
// The decision of whether to start a new user operation is based upon two factors. If a
// compound operation has been explicitly started by calling BeginUndoAction and no matching
// EndUndoAction (these calls nest) has been called, then the action is coalesced into the current
// operation. If there is no outstanding BeginUndoAction call then a new operation is started
// unless it looks as if the new action is caused by the user typing or deleting a stream of text.
// Sequences that look like typing or deletion are coalesced into a single user operation.

UndoHistory::UndoHistory() {

	lenActions = 100;
	actions = new Action[lenActions];
	maxAction = 0;
	currentAction = 0;
	undoSequenceDepth = 0;
	savePoint = 0;

	actions[currentAction].Create(startAction);
}

UndoHistory::~UndoHistory() {
	delete []actions;
	actions = 0;
}

void UndoHistory::EnsureUndoRoom() {
	// Have to test that there is room for 2 more actions in the array
	// as two actions may be created by the calling function
	if (currentAction >= (lenActions - 2)) {
		// Run out of undo nodes so extend the array
		int lenActionsNew = lenActions * 2;
		Action *actionsNew = new Action[lenActionsNew];
		if (!actionsNew)
			return ;
		for (int act = 0; act <= currentAction; act++)
			actionsNew[act].Grab(&actions[act]);
		delete []actions;
		lenActions = lenActionsNew;
		actions = actionsNew;
	}
}

void UndoHistory::AppendAction(actionType at, int position, char *data, int lengthData) {
	EnsureUndoRoom();
	//Platform::DebugPrintf("%% %d action %d %d %d\n", at, position, lengthData, currentAction);
	//Platform::DebugPrintf("^ %d action %d %d\n", actions[currentAction - 1].at,
	//	actions[currentAction - 1].position, actions[currentAction - 1].lenData);
	if (currentAction < savePoint) {
		savePoint = -1;
	}
	if (currentAction >= 1) {
		if (0 == undoSequenceDepth) {
			// Top level actions may not always be coalesced
			Action &actPrevious = actions[currentAction - 1];
			// See if current action can be coalesced into previous action
			// Will work if both are inserts or deletes and position is same
			if (at != actPrevious.at) {
				currentAction++;
			} else if (currentAction == savePoint) {
				currentAction++;
			} else if ((at == insertAction) &&
			           (position != (actPrevious.position + actPrevious.lenData))) {
				// Insertions must be immediately after to coalesce
				currentAction++;
			} else if (!actions[currentAction].mayCoalesce) {
				// Not allowed to coalesce if this set
				currentAction++;
			} else if (at == removeAction) {
				if ((lengthData == 1) || (lengthData == 2)){
					if ((position + lengthData) == actPrevious.position) {
						; // Backspace -> OK
					} else if (position == actPrevious.position) {
						; // Delete -> OK
					} else {
						// Removals must be at same position to coalesce
						currentAction++;
					}
				} else {
					// Removals must be of one character to coalesce
					currentAction++;
				}
			} else {
				//Platform::DebugPrintf("action coalesced\n");
			}

		} else {
			// Actions not at top level are always coalesced unless this is after return to top level
			if (!actions[currentAction].mayCoalesce)
				currentAction++;
		}
	} else {
		currentAction++;
	}
	actions[currentAction].Create(at, position, data, lengthData);
	currentAction++;
	actions[currentAction].Create(startAction);
	maxAction = currentAction;
}

void UndoHistory::BeginUndoAction() {
	EnsureUndoRoom();
	if (undoSequenceDepth == 0) {
		if (actions[currentAction].at != startAction) {
			currentAction++;
			actions[currentAction].Create(startAction);
			maxAction = currentAction;
		}
		actions[currentAction].mayCoalesce = false;
	}
	undoSequenceDepth++;
}

void UndoHistory::EndUndoAction() {
	EnsureUndoRoom();
	undoSequenceDepth--;
	if (0 == undoSequenceDepth) {
		if (actions[currentAction].at != startAction) {
			currentAction++;
			actions[currentAction].Create(startAction);
			maxAction = currentAction;
		}
		actions[currentAction].mayCoalesce = false;
	}
}

void UndoHistory::DropUndoSequence() {
	undoSequenceDepth = 0;
}

void UndoHistory::DeleteUndoHistory() {
	for (int i = 1; i < maxAction; i++)
		actions[i].Destroy();
	maxAction = 0;
	currentAction = 0;
	actions[currentAction].Create(startAction);
	savePoint = 0;
}

void UndoHistory::SetSavePoint() {
	savePoint = currentAction;
}

bool UndoHistory::IsSavePoint() const {
	return savePoint == currentAction;
}

bool UndoHistory::CanUndo() const {
	return (currentAction > 0) && (maxAction > 0);
}

int UndoHistory::StartUndo() {
	// Drop any trailing startAction
	if (actions[currentAction].at == startAction && currentAction > 0)
		currentAction--;

	// Count the steps in this action
	int act = currentAction;
	while (actions[act].at != startAction && act > 0) {
		act--;
	}
	return currentAction - act;
}

const Action &UndoHistory::GetUndoStep() const {
	return actions[currentAction];
}

void UndoHistory::CompletedUndoStep() {
	currentAction--;
}

bool UndoHistory::CanRedo() const {
	return maxAction > currentAction;
}

int UndoHistory::StartRedo() {
	// Drop any leading startAction
	if (actions[currentAction].at == startAction && currentAction < maxAction)
		currentAction++;

	// Count the steps in this action
	int act = currentAction;
	while (actions[act].at != startAction && act < maxAction) {
		act++;
	}
	return act - currentAction;
}

const Action &UndoHistory::GetRedoStep() const {
	return actions[currentAction];
}

void UndoHistory::CompletedRedoStep() {
	currentAction++;
}

CellBuffer::CellBuffer(int initialLength) {
	body = new char[initialLength];
	size = initialLength;
	length = 0;
	part1len = 0;
	gaplen = initialLength;
	part2body = body + gaplen;
	readOnly = false;
	collectingUndo = true;
	growSize = 4000;
}

CellBuffer::~CellBuffer() {
	delete []body;
	body = 0;
}

void CellBuffer::GapTo(int position) {
	if (position == part1len)
		return ;
	if (position < part1len) {
		int diff = part1len - position;
		//Platform::DebugPrintf("Move gap backwards to %d diff = %d part1len=%d length=%d \n", position,diff, part1len, length);
		for (int i = 0; i < diff; i++)
			body[part1len + gaplen - i - 1] = body[part1len - i - 1];
	} else {	// position > part1len
		int diff = position - part1len;
		//Platform::DebugPrintf("Move gap forwards to %d diff =%d\n", position,diff);
		for (int i = 0; i < diff; i++)
			body[part1len + i] = body[part1len + gaplen + i];
	}
	part1len = position;
	part2body = body + gaplen;
}

void CellBuffer::RoomFor(int insertionLength) {
	//Platform::DebugPrintf("need room %d %d\n", gaplen, insertionLength);
	if (gaplen <= insertionLength) {
		//Platform::DebugPrintf("need room %d %d\n", gaplen, insertionLength);
		if (growSize * 6 < size)
			growSize *= 2;
		int newSize = size + insertionLength + growSize;
		Allocate(newSize);
	}
}

// To make it easier to write code that uses ByteAt, a position outside the range of the buffer
// can be retrieved. All characters outside the range have the value '\0'.
char CellBuffer::ByteAt(int position) {
	if (position < part1len) {
		if (position < 0) {
			return '\0';
		} else {
			return body[position];
		}
	} else {
		if (position >= length) {
			return '\0';
		} else {
			return part2body[position];
		}
	}
}

void CellBuffer::SetByteAt(int position, char ch) {

	if (position < 0) {
		//Platform::DebugPrintf("Bad position %d\n",position);
		return ;
	}
	if (position >= length + 11) {
		Platform::DebugPrintf("Very Bad position %d of %d\n", position, length);
		//exit(2);
		return ;
	}
	if (position >= length) {
		//Platform::DebugPrintf("Bad position %d of %d\n",position,length);
		return ;
	}

	if (position < part1len) {
		body[position] = ch;
	} else {
		part2body[position] = ch;
	}
}

char CellBuffer::CharAt(int position) {
	return ByteAt(position*2);
}

void CellBuffer::GetCharRange(char *buffer, int position, int lengthRetrieve) {
	if (lengthRetrieve < 0)
		return ;
	if (position < 0)
		return ;
	int bytePos = position * 2;
	if ((bytePos + lengthRetrieve * 2) > length) {
		Platform::DebugPrintf("Bad GetCharRange %d for %d of %d\n", bytePos,
		                      lengthRetrieve, length);
		return ;
	}
	GapTo(0); 	// Move the buffer so its easy to subscript into it
	char *pb = part2body + bytePos;
	while (lengthRetrieve--) {
		*buffer++ = *pb;
		pb += 2;
	}
}

char CellBuffer::StyleAt(int position) {
	return ByteAt(position*2 + 1);
}

const char *CellBuffer::InsertString(int position, char *s, int insertLength) {
	char *data = 0;
	// InsertString and DeleteChars are the bottleneck though which all changes occur
	if (!readOnly) {
		if (collectingUndo) {
			// Save into the undo/redo stack, but only the characters - not the formatting
			// This takes up about half load time
			data = new char[insertLength / 2];
			for (int i = 0; i < insertLength / 2; i++) {
				data[i] = s[i * 2];
			}
			uh.AppendAction(insertAction, position / 2, data, insertLength / 2);
		}

		BasicInsertString(position, s, insertLength);
	}
	return data;
}

bool CellBuffer::SetStyleAt(int position, char style, char mask) {
	style &= mask;
	char curVal = ByteAt(position * 2 + 1);
	if ((curVal & mask) != style) {
		SetByteAt(position*2 + 1, static_cast<char>((curVal & ~mask) | style));
		return true;
	} else {
		return false;
	}
}

bool CellBuffer::SetStyleFor(int position, int lengthStyle, char style, char mask) {
	int bytePos = position * 2 + 1;
	bool changed = false;
	PLATFORM_ASSERT(lengthStyle == 0 ||
		(lengthStyle > 0 && lengthStyle + position < length));
	while (lengthStyle--) {
		char curVal = ByteAt(bytePos);
		if ((curVal & mask) != style) {
			SetByteAt(bytePos, static_cast<char>((curVal & ~mask) | style));
			changed = true;
		}
		bytePos += 2;
	}
	return changed;
}

const char *CellBuffer::DeleteChars(int position, int deleteLength) {
	// InsertString and DeleteChars are the bottleneck though which all changes occur
	PLATFORM_ASSERT(deleteLength > 0);
	char *data = 0;
	if (!readOnly) {
		if (collectingUndo) {
			// Save into the undo/redo stack, but only the characters - not the formatting
			data = new char[deleteLength / 2];
			for (int i = 0; i < deleteLength / 2; i++) {
				data[i] = ByteAt(position + i * 2);
			}
			uh.AppendAction(removeAction, position / 2, data, deleteLength / 2);
		}

		BasicDeleteChars(position, deleteLength);
	}
	return data;
}

int CellBuffer::ByteLength() {
	return length;
}

int CellBuffer::Length() {
	return ByteLength() / 2;
}

void CellBuffer::Allocate(int newSize) {
	if (newSize > length) {
		GapTo(length);
		char *newBody = new char[newSize];
		memcpy(newBody, body, length);
		delete []body;
		body = newBody;
		gaplen += newSize - size;
		part2body = body + gaplen;
		size = newSize;
	}
}

int CellBuffer::Lines() {
	//Platform::DebugPrintf("Lines = %d\n", lv.lines);
	return lv.lines;
}

int CellBuffer::LineStart(int line) {
	if (line < 0)
		return 0;
	else if (line > lv.lines)
		return Length();
	else
		return lv.linesData[line].startPosition;
}

bool CellBuffer::IsReadOnly() {
	return readOnly;
}

void CellBuffer::SetReadOnly(bool set) {
	readOnly = set;
}

void CellBuffer::SetSavePoint() {
	uh.SetSavePoint();
}

bool CellBuffer::IsSavePoint() {
	return uh.IsSavePoint();
}

int CellBuffer::AddMark(int line, int markerNum) {
	if ((line >= 0) && (line < lv.lines)) {
		return lv.AddMark(line, markerNum);
	}
	return - 1;
}

void CellBuffer::DeleteMark(int line, int markerNum) {
	if ((line >= 0) && (line < lv.lines)) {
		lv.DeleteMark(line, markerNum, false);
	}
}

void CellBuffer::DeleteMarkFromHandle(int markerHandle) {
	lv.DeleteMarkFromHandle(markerHandle);
}

int CellBuffer::GetMark(int line) {
	if ((line >= 0) && (line < lv.lines) && (lv.linesData[line].handleSet))
		return lv.linesData[line].handleSet->MarkValue();
	return 0;
}

void CellBuffer::DeleteAllMarks(int markerNum) {
	for (int line = 0; line < lv.lines; line++) {
		lv.DeleteMark(line, markerNum, true);
	}
}

int CellBuffer::LineFromHandle(int markerHandle) {
	return lv.LineFromHandle(markerHandle);
}

// Without undo

void CellBuffer::BasicInsertString(int position, char *s, int insertLength) {
	//Platform::DebugPrintf("Inserting at %d for %d\n", position, insertLength);
	if (insertLength == 0)
		return ;
	PLATFORM_ASSERT(insertLength > 0);
	RoomFor(insertLength);
	GapTo(position);

	memcpy(body + part1len, s, insertLength);
	length += insertLength;
	part1len += insertLength;
	gaplen -= insertLength;
	part2body = body + gaplen;

	int lineInsert = lv.LineFromPosition(position / 2) + 1;
	// Point all the lines after the insertion point further along in the buffer
	for (int lineAfter = lineInsert; lineAfter <= lv.lines; lineAfter++) {
		lv.linesData[lineAfter].startPosition += insertLength / 2;
	}
	char chPrev = ' ';
	if ((position - 2) >= 0)
		chPrev = ByteAt(position - 2);
	char chAfter = ' ';
	if ((position + insertLength) < length)
		chAfter = ByteAt(position + insertLength);
	if (chPrev == '\r' && chAfter == '\n') {
		//Platform::DebugPrintf("Splitting a crlf pair at %d\n", lineInsert);
		// Splitting up a crlf pair at position
		lv.InsertValue(lineInsert, position / 2);
		lineInsert++;
	}
	char ch = ' ';
	for (int i = 0; i < insertLength; i += 2) {
		ch = s[i];
		if (ch == '\r') {
			//Platform::DebugPrintf("Inserting cr at %d\n", lineInsert);
			lv.InsertValue(lineInsert, (position + i) / 2 + 1);
			lineInsert++;
		} else if (ch == '\n') {
			if (chPrev == '\r') {
				//Platform::DebugPrintf("Patching cr before lf at %d\n", lineInsert-1);
				// Patch up what was end of line
				lv.SetValue(lineInsert - 1, (position + i) / 2 + 1);
			} else {
				//Platform::DebugPrintf("Inserting lf at %d\n", lineInsert);
				lv.InsertValue(lineInsert, (position + i) / 2 + 1);
				lineInsert++;
			}
		}
		chPrev = ch;
	}
	// Joining two lines where last insertion is cr and following text starts with lf
	if (chAfter == '\n') {
		if (ch == '\r') {
			//Platform::DebugPrintf("Joining cr before lf at %d\n", lineInsert-1);
			// End of line already in buffer so drop the newly created one
			lv.Remove(lineInsert - 1);
		}
	}
}

void CellBuffer::BasicDeleteChars(int position, int deleteLength) {
	//Platform::DebugPrintf("Deleting at %d for %d\n", position, deleteLength);
	if (deleteLength == 0)
		return ;

	if ((position == 0) && (deleteLength == length)) {
		// If whole buffer is being deleted, faster to reinitialise lines data
		// than to delete each line.
		//printf("Whole buffer being deleted\n");
		lv.Init();
	} else {
		// Have to fix up line positions before doing deletion as looking at text in buffer
		// to work out which lines have been removed

		int lineRemove = lv.LineFromPosition(position / 2) + 1;
		// Point all the lines after the insertion point further along in the buffer
		for (int lineAfter = lineRemove; lineAfter <= lv.lines; lineAfter++) {
			lv.linesData[lineAfter].startPosition -= deleteLength / 2;
		}
		char chPrev = ' ';
		if (position >= 2)
			chPrev = ByteAt(position - 2);
		char chBefore = chPrev;
		char chNext = ' ';
		if (position < length)
			chNext = ByteAt(position);
		bool ignoreNL = false;
		if (chPrev == '\r' && chNext == '\n') {
			//Platform::DebugPrintf("Deleting lf after cr, move line end to cr at %d\n", lineRemove);
			// Move back one
			lv.SetValue(lineRemove, position / 2);
			lineRemove++;
			ignoreNL = true; 	// First \n is not real deletion
		}

		char ch = chNext;
		for (int i = 0; i < deleteLength; i += 2) {
			chNext = ' ';
			if ((position + i + 2) < length)
				chNext = ByteAt(position + i + 2);
			//Platform::DebugPrintf("Deleting %d %x\n", i, ch);
			if (ch == '\r') {
				if (chNext != '\n') {
					//Platform::DebugPrintf("Removing cr end of line\n");
					lv.Remove(lineRemove);
				}
			} else if (ch == '\n') {
				if (ignoreNL) {
					ignoreNL = false; 	// Further \n are real deletions
				} else {
					//Platform::DebugPrintf("Removing lf end of line\n");
					lv.Remove(lineRemove);
				}
			}

			ch = chNext;
		}
		// May have to fix up end if last deletion causes cr to be next to lf
		// or removes one of a crlf pair
		char chAfter = ' ';
		if ((position + deleteLength) < length)
			chAfter = ByteAt(position + deleteLength);
		if (chBefore == '\r' && chAfter == '\n') {
			//d.printf("Joining cr before lf at %d\n", lineRemove);
			// Using lineRemove-1 as cr ended line before start of deletion
			lv.Remove(lineRemove - 1);
			lv.SetValue(lineRemove - 1, position / 2 + 1);
		}
	}
	GapTo(position);
	length -= deleteLength;
	gaplen += deleteLength;
	part2body = body + gaplen;
}

bool CellBuffer::SetUndoCollection(bool collectUndo) {
	collectingUndo = collectUndo;
	uh.DropUndoSequence();
	return collectingUndo;
}

bool CellBuffer::IsCollectingUndo() {
	return collectingUndo;
}

void CellBuffer::BeginUndoAction() {
	uh.BeginUndoAction();
}

void CellBuffer::EndUndoAction() {
	uh.EndUndoAction();
}

void CellBuffer::DeleteUndoHistory() {
	uh.DeleteUndoHistory();
}

bool CellBuffer::CanUndo() {
	return uh.CanUndo();
}

int CellBuffer::StartUndo() {
	return uh.StartUndo();
}

const Action &CellBuffer::GetUndoStep() const {
	return uh.GetUndoStep();
}

void CellBuffer::PerformUndoStep() {
	const Action &actionStep = uh.GetUndoStep();
	if (actionStep.at == insertAction) {
		BasicDeleteChars(actionStep.position*2, actionStep.lenData*2);
	} else if (actionStep.at == removeAction) {
		char *styledData = new char[actionStep.lenData * 2];
		for (int i = 0; i < actionStep.lenData; i++) {
			styledData[i*2] = actionStep.data[i];
			styledData[i*2 + 1] = 0;
		}
		BasicInsertString(actionStep.position*2, styledData, actionStep.lenData*2);
		delete []styledData;
	}
	uh.CompletedUndoStep();
}

bool CellBuffer::CanRedo() {
	return uh.CanRedo();
}

int CellBuffer::StartRedo() {
	return uh.StartRedo();
}

const Action &CellBuffer::GetRedoStep() const {
	return uh.GetRedoStep();
}

void CellBuffer::PerformRedoStep() {
	const Action &actionStep = uh.GetRedoStep();
	if (actionStep.at == insertAction) {
		char *styledData = new char[actionStep.lenData * 2];
		for (int i = 0; i < actionStep.lenData; i++) {
			styledData[i*2] = actionStep.data[i];
			styledData[i*2 + 1] = 0;
		}
		BasicInsertString(actionStep.position*2, styledData, actionStep.lenData*2);
		delete []styledData;
	} else if (actionStep.at == removeAction) {
		BasicDeleteChars(actionStep.position*2, actionStep.lenData*2);
	}
	uh.CompletedRedoStep();
}

int CellBuffer::SetLineState(int line, int state) {
	int stateOld = lineStates[line];
	lineStates[line] = state;
	return stateOld;
}

int CellBuffer::GetLineState(int line) {
	return lineStates[line];
}

int CellBuffer::GetMaxLineState() {
	return lineStates.Length();
}

int CellBuffer::SetLevel(int line, int level) {
	int prev = 0;
	if ((line >= 0) && (line < lv.lines)) {
		if (!lv.levels) {
			lv.ExpandLevels();
		}
		prev = lv.levels[line];
		if (lv.levels[line] != level) {
			lv.levels[line] = level;
		}
	}
	return prev;
}

int CellBuffer::GetLevel(int line) {
	if (lv.levels && (line >= 0) && (line < lv.lines)) {
		return lv.levels[line];
	} else {
		return SC_FOLDLEVELBASE;
	}
}

void CellBuffer::ClearLevels() {
	lv.ClearLevels();
}
