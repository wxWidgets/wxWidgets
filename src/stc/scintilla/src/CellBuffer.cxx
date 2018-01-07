// Scintilla source code edit control
/** @file CellBuffer.cxx
 ** Manages a buffer of cells.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>

#include <stdexcept>
#include <vector>
#include <algorithm>
#include <memory>

#include "Platform.h"

#include "Scintilla.h"
#include "Position.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "CellBuffer.h"
#include "UniConversion.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

LineVector::LineVector() : starts(256), perLine(0) {
	Init();
}

LineVector::~LineVector() {
	starts.DeleteAll();
}

void LineVector::Init() {
	starts.DeleteAll();
	if (perLine) {
		perLine->Init();
	}
}

void LineVector::SetPerLine(PerLine *pl) {
	perLine = pl;
}

void LineVector::InsertText(Sci::Line line, Sci::Position delta) {
	starts.InsertText(line, delta);
}

void LineVector::InsertLine(Sci::Line line, Sci::Position position, bool lineStart) {
	starts.InsertPartition(line, position);
	if (perLine) {
		if ((line > 0) && lineStart)
			line--;
		perLine->InsertLine(line);
	}
}

void LineVector::SetLineStart(Sci::Line line, Sci::Position position) {
	starts.SetPartitionStartPosition(line, position);
}

void LineVector::RemoveLine(Sci::Line line) {
	starts.RemovePartition(line);
	if (perLine) {
		perLine->RemoveLine(line);
	}
}

Sci::Line LineVector::LineFromPosition(Sci::Position pos) const {
	return starts.PartitionFromPosition(pos);
}

Action::Action() {
	at = startAction;
	position = 0;
	lenData = 0;
	mayCoalesce = false;
}

Action::Action(Action &&other) {
	at = other.at;
	position = other.position;
	data = std::move(other.data);
	lenData = other.lenData;
	mayCoalesce = other.mayCoalesce;
}

Action::~Action() {
}

void Action::Create(actionType at_, Sci::Position position_, const char *data_, Sci::Position lenData_, bool mayCoalesce_) {
	data = nullptr;
	position = position_;
	at = at_;
	if (lenData_) {
		data = std::unique_ptr<char []>(new char[lenData_]);
		memcpy(&data[0], data_, lenData_);
	}
	lenData = lenData_;
	mayCoalesce = mayCoalesce_;
}

void Action::Clear() {
	data = nullptr;
	lenData = 0;
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

	actions.resize(3);
	maxAction = 0;
	currentAction = 0;
	undoSequenceDepth = 0;
	savePoint = 0;
	tentativePoint = -1;

	actions[currentAction].Create(startAction);
}

UndoHistory::~UndoHistory() {
}

void UndoHistory::EnsureUndoRoom() {
	// Have to test that there is room for 2 more actions in the array
	// as two actions may be created by the calling function
	if (static_cast<size_t>(currentAction) >= (actions.size() - 2)) {
		// Run out of undo nodes so extend the array
		actions.resize(actions.size() * 2);
	}
}

const char *UndoHistory::AppendAction(actionType at, Sci::Position position, const char *data, Sci::Position lengthData,
	bool &startSequence, bool mayCoalesce) {
	EnsureUndoRoom();
	//Platform::DebugPrintf("%% %d action %d %d %d\n", at, position, lengthData, currentAction);
	//Platform::DebugPrintf("^ %d action %d %d\n", actions[currentAction - 1].at,
	//	actions[currentAction - 1].position, actions[currentAction - 1].lenData);
	if (currentAction < savePoint) {
		savePoint = -1;
	}
	int oldCurrentAction = currentAction;
	if (currentAction >= 1) {
		if (0 == undoSequenceDepth) {
			// Top level actions may not always be coalesced
			int targetAct = -1;
			const Action *actPrevious = &(actions[currentAction + targetAct]);
			// Container actions may forward the coalesce state of Scintilla Actions.
			while ((actPrevious->at == containerAction) && actPrevious->mayCoalesce) {
				targetAct--;
				actPrevious = &(actions[currentAction + targetAct]);
			}
			// See if current action can be coalesced into previous action
			// Will work if both are inserts or deletes and position is same
			if ((currentAction == savePoint) || (currentAction == tentativePoint)) {
				currentAction++;
			} else if (!actions[currentAction].mayCoalesce) {
				// Not allowed to coalesce if this set
				currentAction++;
			} else if (!mayCoalesce || !actPrevious->mayCoalesce) {
				currentAction++;
			} else if (at == containerAction || actions[currentAction].at == containerAction) {
				;	// A coalescible containerAction
			} else if ((at != actPrevious->at) && (actPrevious->at != startAction)) {
				currentAction++;
			} else if ((at == insertAction) &&
			           (position != (actPrevious->position + actPrevious->lenData))) {
				// Insertions must be immediately after to coalesce
				currentAction++;
			} else if (at == removeAction) {
				if ((lengthData == 1) || (lengthData == 2)) {
					if ((position + lengthData) == actPrevious->position) {
						; // Backspace -> OK
					} else if (position == actPrevious->position) {
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
				// Action coalesced.
			}

		} else {
			// Actions not at top level are always coalesced unless this is after return to top level
			if (!actions[currentAction].mayCoalesce)
				currentAction++;
		}
	} else {
		currentAction++;
	}
	startSequence = oldCurrentAction != currentAction;
	const int actionWithData = currentAction;
	actions[currentAction].Create(at, position, data, lengthData, mayCoalesce);
	currentAction++;
	actions[currentAction].Create(startAction);
	maxAction = currentAction;
	return actions[actionWithData].data.get();
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
	PLATFORM_ASSERT(undoSequenceDepth > 0);
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
		actions[i].Clear();
	maxAction = 0;
	currentAction = 0;
	actions[currentAction].Create(startAction);
	savePoint = 0;
	tentativePoint = -1;
}

void UndoHistory::SetSavePoint() {
	savePoint = currentAction;
}

bool UndoHistory::IsSavePoint() const {
	return savePoint == currentAction;
}

void UndoHistory::TentativeStart() {
	tentativePoint = currentAction;
}

void UndoHistory::TentativeCommit() {
	tentativePoint = -1;
	// Truncate undo history
	maxAction = currentAction;
}

int UndoHistory::TentativeSteps() {
	// Drop any trailing startAction
	if (actions[currentAction].at == startAction && currentAction > 0)
		currentAction--;
	if (tentativePoint >= 0)
		return currentAction - tentativePoint;
	else
		return -1;
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
	if (currentAction < maxAction && actions[currentAction].at == startAction)
		currentAction++;

	// Count the steps in this action
	int act = currentAction;
	while (act < maxAction && actions[act].at != startAction) {
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

CellBuffer::CellBuffer() {
	readOnly = false;
	utf8LineEnds = 0;
	collectingUndo = true;
}

CellBuffer::~CellBuffer() {
}

char CellBuffer::CharAt(Sci::Position position) const {
	return substance.ValueAt(position);
}

void CellBuffer::GetCharRange(char *buffer, Sci::Position position, Sci::Position lengthRetrieve) const {
	if (lengthRetrieve <= 0)
		return;
	if (position < 0)
		return;
	if ((position + lengthRetrieve) > substance.Length()) {
		Platform::DebugPrintf("Bad GetCharRange %d for %d of %d\n", position,
		                      lengthRetrieve, substance.Length());
		return;
	}
	substance.GetRange(buffer, position, lengthRetrieve);
}

char CellBuffer::StyleAt(Sci::Position position) const {
	return style.ValueAt(position);
}

void CellBuffer::GetStyleRange(unsigned char *buffer, Sci::Position position, Sci::Position lengthRetrieve) const {
	if (lengthRetrieve < 0)
		return;
	if (position < 0)
		return;
	if ((position + lengthRetrieve) > style.Length()) {
		Platform::DebugPrintf("Bad GetStyleRange %d for %d of %d\n", position,
		                      lengthRetrieve, style.Length());
		return;
	}
	style.GetRange(reinterpret_cast<char *>(buffer), position, lengthRetrieve);
}

const char *CellBuffer::BufferPointer() {
	return substance.BufferPointer();
}

const char *CellBuffer::RangePointer(Sci::Position position, Sci::Position rangeLength) {
	return substance.RangePointer(position, rangeLength);
}

Sci::Position CellBuffer::GapPosition() const {
	return substance.GapPosition();
}

// The char* returned is to an allocation owned by the undo history
const char *CellBuffer::InsertString(Sci::Position position, const char *s, Sci::Position insertLength, bool &startSequence) {
	// InsertString and DeleteChars are the bottleneck though which all changes occur
	const char *data = s;
	if (!readOnly) {
		if (collectingUndo) {
			// Save into the undo/redo stack, but only the characters - not the formatting
			// This takes up about half load time
			data = uh.AppendAction(insertAction, position, s, insertLength, startSequence);
		}

		BasicInsertString(position, s, insertLength);
	}
	return data;
}

bool CellBuffer::SetStyleAt(Sci::Position position, char styleValue) {
	const char curVal = style.ValueAt(position);
	if (curVal != styleValue) {
		style.SetValueAt(position, styleValue);
		return true;
	} else {
		return false;
	}
}

bool CellBuffer::SetStyleFor(Sci::Position position, Sci::Position lengthStyle, char styleValue) {
	bool changed = false;
	PLATFORM_ASSERT(lengthStyle == 0 ||
		(lengthStyle > 0 && lengthStyle + position <= style.Length()));
	while (lengthStyle--) {
		const char curVal = style.ValueAt(position);
		if (curVal != styleValue) {
			style.SetValueAt(position, styleValue);
			changed = true;
		}
		position++;
	}
	return changed;
}

// The char* returned is to an allocation owned by the undo history
const char *CellBuffer::DeleteChars(Sci::Position position, Sci::Position deleteLength, bool &startSequence) {
	// InsertString and DeleteChars are the bottleneck though which all changes occur
	PLATFORM_ASSERT(deleteLength > 0);
	const char *data = 0;
	if (!readOnly) {
		if (collectingUndo) {
			// Save into the undo/redo stack, but only the characters - not the formatting
			// The gap would be moved to position anyway for the deletion so this doesn't cost extra
			data = substance.RangePointer(position, deleteLength);
			data = uh.AppendAction(removeAction, position, data, deleteLength, startSequence);
		}

		BasicDeleteChars(position, deleteLength);
	}
	return data;
}

Sci::Position CellBuffer::Length() const {
	return substance.Length();
}

void CellBuffer::Allocate(Sci::Position newSize) {
	substance.ReAllocate(newSize);
	style.ReAllocate(newSize);
}

void CellBuffer::SetLineEndTypes(int utf8LineEnds_) {
	if (utf8LineEnds != utf8LineEnds_) {
		utf8LineEnds = utf8LineEnds_;
		ResetLineEnds();
	}
}

bool CellBuffer::ContainsLineEnd(const char *s, Sci::Position length) const {
	unsigned char chBeforePrev = 0;
	unsigned char chPrev = 0;
	for (Sci::Position i = 0; i < length; i++) {
		const unsigned char ch = s[i];
		if ((ch == '\r') || (ch == '\n')) {
			return true;
		} else if (utf8LineEnds) {
			const unsigned char back3[3] = { chBeforePrev, chPrev, ch };
			if (UTF8IsSeparator(back3) || UTF8IsNEL(back3 + 1)) {
				return true;
			}
		}
		chBeforePrev = chPrev;
		chPrev = ch;
	}
	return false;
}

void CellBuffer::SetPerLine(PerLine *pl) {
	lv.SetPerLine(pl);
}

Sci::Line CellBuffer::Lines() const {
	return lv.Lines();
}

Sci::Position CellBuffer::LineStart(Sci::Line line) const {
	if (line < 0)
		return 0;
	else if (line >= Lines())
		return Length();
	else
		return lv.LineStart(line);
}

bool CellBuffer::IsReadOnly() const {
	return readOnly;
}

void CellBuffer::SetReadOnly(bool set) {
	readOnly = set;
}

void CellBuffer::SetSavePoint() {
	uh.SetSavePoint();
}

bool CellBuffer::IsSavePoint() const {
	return uh.IsSavePoint();
}

void CellBuffer::TentativeStart() {
	uh.TentativeStart();
}

void CellBuffer::TentativeCommit() {
	uh.TentativeCommit();
}

int CellBuffer::TentativeSteps() {
	return uh.TentativeSteps();
}

bool CellBuffer::TentativeActive() const {
	return uh.TentativeActive();
}

// Without undo

void CellBuffer::InsertLine(Sci::Line line, Sci::Position position, bool lineStart) {
	lv.InsertLine(line, position, lineStart);
}

void CellBuffer::RemoveLine(Sci::Line line) {
	lv.RemoveLine(line);
}

bool CellBuffer::UTF8LineEndOverlaps(Sci::Position position) const {
	const unsigned char bytes[] = {
		static_cast<unsigned char>(substance.ValueAt(position-2)),
		static_cast<unsigned char>(substance.ValueAt(position-1)),
		static_cast<unsigned char>(substance.ValueAt(position)),
		static_cast<unsigned char>(substance.ValueAt(position+1)),
	};
	return UTF8IsSeparator(bytes) || UTF8IsSeparator(bytes+1) || UTF8IsNEL(bytes+1);
}

void CellBuffer::ResetLineEnds() {
	// Reinitialize line data -- too much work to preserve
	lv.Init();

	Sci::Position position = 0;
	Sci::Position length = Length();
	Sci::Line lineInsert = 1;
	bool atLineStart = true;
	lv.InsertText(lineInsert-1, length);
	unsigned char chBeforePrev = 0;
	unsigned char chPrev = 0;
	for (Sci::Position i = 0; i < length; i++) {
		const unsigned char ch = substance.ValueAt(position + i);
		if (ch == '\r') {
			InsertLine(lineInsert, (position + i) + 1, atLineStart);
			lineInsert++;
		} else if (ch == '\n') {
			if (chPrev == '\r') {
				// Patch up what was end of line
				lv.SetLineStart(lineInsert - 1, (position + i) + 1);
			} else {
				InsertLine(lineInsert, (position + i) + 1, atLineStart);
				lineInsert++;
			}
		} else if (utf8LineEnds) {
			const unsigned char back3[3] = {chBeforePrev, chPrev, ch};
			if (UTF8IsSeparator(back3) || UTF8IsNEL(back3+1)) {
				InsertLine(lineInsert, (position + i) + 1, atLineStart);
				lineInsert++;
			}
		}
		chBeforePrev = chPrev;
		chPrev = ch;
	}
}

void CellBuffer::BasicInsertString(Sci::Position position, const char *s, Sci::Position insertLength) {
	if (insertLength == 0)
		return;
	PLATFORM_ASSERT(insertLength > 0);

	const unsigned char chAfter = substance.ValueAt(position);
	bool breakingUTF8LineEnd = false;
	if (utf8LineEnds && UTF8IsTrailByte(chAfter)) {
		breakingUTF8LineEnd = UTF8LineEndOverlaps(position);
	}

	substance.InsertFromArray(position, s, 0, insertLength);
	style.InsertValue(position, insertLength, 0);

	Sci::Line lineInsert = lv.LineFromPosition(position) + 1;
	bool atLineStart = lv.LineStart(lineInsert-1) == position;
	// Point all the lines after the insertion point further along in the buffer
	lv.InsertText(lineInsert-1, insertLength);
	unsigned char chBeforePrev = substance.ValueAt(position - 2);
	unsigned char chPrev = substance.ValueAt(position - 1);
	if (chPrev == '\r' && chAfter == '\n') {
		// Splitting up a crlf pair at position
		InsertLine(lineInsert, position, false);
		lineInsert++;
	}
	if (breakingUTF8LineEnd) {
		RemoveLine(lineInsert);
	}
	unsigned char ch = ' ';
	for (Sci::Position i = 0; i < insertLength; i++) {
		ch = s[i];
		if (ch == '\r') {
			InsertLine(lineInsert, (position + i) + 1, atLineStart);
			lineInsert++;
		} else if (ch == '\n') {
			if (chPrev == '\r') {
				// Patch up what was end of line
				lv.SetLineStart(lineInsert - 1, (position + i) + 1);
			} else {
				InsertLine(lineInsert, (position + i) + 1, atLineStart);
				lineInsert++;
			}
		} else if (utf8LineEnds) {
			const unsigned char back3[3] = {chBeforePrev, chPrev, ch};
			if (UTF8IsSeparator(back3) || UTF8IsNEL(back3+1)) {
				InsertLine(lineInsert, (position + i) + 1, atLineStart);
				lineInsert++;
			}
		}
		chBeforePrev = chPrev;
		chPrev = ch;
	}
	// Joining two lines where last insertion is cr and following substance starts with lf
	if (chAfter == '\n') {
		if (ch == '\r') {
			// End of line already in buffer so drop the newly created one
			RemoveLine(lineInsert - 1);
		}
	} else if (utf8LineEnds && !UTF8IsAscii(chAfter)) {
		// May have end of UTF-8 line end in buffer and start in insertion
		for (int j = 0; j < UTF8SeparatorLength-1; j++) {
			const unsigned char chAt = substance.ValueAt(position + insertLength + j);
			const unsigned char back3[3] = {chBeforePrev, chPrev, chAt};
			if (UTF8IsSeparator(back3)) {
				InsertLine(lineInsert, (position + insertLength + j) + 1, atLineStart);
				lineInsert++;
			}
			if ((j == 0) && UTF8IsNEL(back3+1)) {
				InsertLine(lineInsert, (position + insertLength + j) + 1, atLineStart);
				lineInsert++;
			}
			chBeforePrev = chPrev;
			chPrev = chAt;
		}
	}
}

void CellBuffer::BasicDeleteChars(Sci::Position position, Sci::Position deleteLength) {
	if (deleteLength == 0)
		return;

	if ((position == 0) && (deleteLength == substance.Length())) {
		// If whole buffer is being deleted, faster to reinitialise lines data
		// than to delete each line.
		lv.Init();
	} else {
		// Have to fix up line positions before doing deletion as looking at text in buffer
		// to work out which lines have been removed

		Sci::Line lineRemove = lv.LineFromPosition(position) + 1;
		lv.InsertText(lineRemove-1, - (deleteLength));
		const unsigned char chPrev = substance.ValueAt(position - 1);
		const unsigned char chBefore = chPrev;
		unsigned char chNext = substance.ValueAt(position);
		bool ignoreNL = false;
		if (chPrev == '\r' && chNext == '\n') {
			// Move back one
			lv.SetLineStart(lineRemove, position);
			lineRemove++;
			ignoreNL = true; 	// First \n is not real deletion
		}
		if (utf8LineEnds && UTF8IsTrailByte(chNext)) {
			if (UTF8LineEndOverlaps(position)) {
				RemoveLine(lineRemove);
			}
		}

		unsigned char ch = chNext;
		for (Sci::Position i = 0; i < deleteLength; i++) {
			chNext = substance.ValueAt(position + i + 1);
			if (ch == '\r') {
				if (chNext != '\n') {
					RemoveLine(lineRemove);
				}
			} else if (ch == '\n') {
				if (ignoreNL) {
					ignoreNL = false; 	// Further \n are real deletions
				} else {
					RemoveLine(lineRemove);
				}
			} else if (utf8LineEnds) {
				if (!UTF8IsAscii(ch)) {
					const unsigned char next3[3] = {ch, chNext,
						static_cast<unsigned char>(substance.ValueAt(position + i + 2))};
					if (UTF8IsSeparator(next3) || UTF8IsNEL(next3)) {
						RemoveLine(lineRemove);
					}
				}
			}

			ch = chNext;
		}
		// May have to fix up end if last deletion causes cr to be next to lf
		// or removes one of a crlf pair
		const char chAfter = substance.ValueAt(position + deleteLength);
		if (chBefore == '\r' && chAfter == '\n') {
			// Using lineRemove-1 as cr ended line before start of deletion
			RemoveLine(lineRemove - 1);
			lv.SetLineStart(lineRemove - 1, position + 1);
		}
	}
	substance.DeleteRange(position, deleteLength);
	style.DeleteRange(position, deleteLength);
}

bool CellBuffer::SetUndoCollection(bool collectUndo) {
	collectingUndo = collectUndo;
	uh.DropUndoSequence();
	return collectingUndo;
}

bool CellBuffer::IsCollectingUndo() const {
	return collectingUndo;
}

void CellBuffer::BeginUndoAction() {
	uh.BeginUndoAction();
}

void CellBuffer::EndUndoAction() {
	uh.EndUndoAction();
}

void CellBuffer::AddUndoAction(Sci::Position token, bool mayCoalesce) {
	bool startSequence;
	uh.AppendAction(containerAction, token, 0, 0, startSequence, mayCoalesce);
}

void CellBuffer::DeleteUndoHistory() {
	uh.DeleteUndoHistory();
}

bool CellBuffer::CanUndo() const {
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
		if (substance.Length() < actionStep.lenData) {
			throw std::runtime_error(
				"CellBuffer::PerformUndoStep: deletion must be less than document length.");
		}
		BasicDeleteChars(actionStep.position, actionStep.lenData);
	} else if (actionStep.at == removeAction) {
		BasicInsertString(actionStep.position, actionStep.data.get(), actionStep.lenData);
	}
	uh.CompletedUndoStep();
}

bool CellBuffer::CanRedo() const {
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
		BasicInsertString(actionStep.position, actionStep.data.get(), actionStep.lenData);
	} else if (actionStep.at == removeAction) {
		BasicDeleteChars(actionStep.position, actionStep.lenData);
	}
	uh.CompletedRedoStep();
}

