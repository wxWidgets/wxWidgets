// Scintilla source code edit control
/** @file SparseVector.h
 ** Hold data sparsely associated with elements in a range.
 **/
// Copyright 2016 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef SPARSEVECTOR_H
#define SPARSEVECTOR_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

// SparseVector is similar to RunStyles but is more efficient for cases where values occur
// for one position instead of over a range of positions.
template <typename T>
class SparseVector {
private:
	std::unique_ptr<Partitioning> starts;
	std::unique_ptr<SplitVector<T>> values;
	T empty;
	// Deleted so SparseVector objects can not be copied.
	SparseVector(const SparseVector &) = delete;
	void operator=(const SparseVector &) = delete;
	void ClearValue(int partition) {
		values->SetValueAt(partition, T());
	}
public:
	SparseVector() : empty() {
		starts.reset(new Partitioning(8));
		values.reset(new SplitVector<T>());
		values->InsertEmpty(0, 2);
	}
	~SparseVector() {
		starts.reset();
		// starts dead here but not used by ClearValue.
		for (int part = 0; part < values->Length(); part++) {
			ClearValue(part);
		}
		values.reset();
	}
	int Length() const {
		return starts->PositionFromPartition(starts->Partitions());
	}
	int Elements() const {
		return starts->Partitions();
	}
	int PositionOfElement(int element) const {
		return starts->PositionFromPartition(element);
	}
	const T& ValueAt(int position) const {
		assert(position < Length());
		const int partition = starts->PartitionFromPosition(position);
		const int startPartition = starts->PositionFromPartition(partition);
		if (startPartition == position) {
			return values->ValueAt(partition);
		} else {
			return empty;
		}
	}
	template <typename ParamType>
	void SetValueAt(int position, ParamType &&value) {
		assert(position < Length());
		const int partition = starts->PartitionFromPosition(position);
		const int startPartition = starts->PositionFromPartition(partition);
		if (value == T()) {
			// Setting the empty value is equivalent to deleting the position
			if (position == 0) {
				ClearValue(partition);
			} else if (position == startPartition) {
				// Currently an element at this position, so remove
				ClearValue(partition);
				starts->RemovePartition(partition);
				values->Delete(partition);
			}
			// Else element remains empty
		} else {
			if (position == startPartition) {
				// Already a value at this position, so replace
				ClearValue(partition);
				values->SetValueAt(partition, std::move(value));
			} else {
				// Insert a new element
				starts->InsertPartition(partition + 1, position);
				values->Insert(partition + 1, std::move(value));
			}
		}
	}
	void InsertSpace(int position, int insertLength) {
		assert(position <= Length());	// Only operation that works at end.
		const int partition = starts->PartitionFromPosition(position);
		const int startPartition = starts->PositionFromPartition(partition);
		if (startPartition == position) {
			const bool positionOccupied = values->ValueAt(partition) != T();
			// Inserting at start of run so make previous longer
			if (partition == 0) {
				// Inserting at start of document so ensure start empty
				if (positionOccupied) {
					starts->InsertPartition(1, 0);
					values->InsertEmpty(0, 1);
				}
				starts->InsertText(partition, insertLength);
			} else {
				if (positionOccupied) {
					starts->InsertText(partition - 1, insertLength);
				} else {
					// Insert at end of run so do not extend style
					starts->InsertText(partition, insertLength);
				}
			}
		} else {
			starts->InsertText(partition, insertLength);
		}
	}
	void DeletePosition(int position) {
		assert(position < Length());
		int partition = starts->PartitionFromPosition(position);
		const int startPartition = starts->PositionFromPartition(partition);
		if (startPartition == position) {
			if (partition == 0) {
				ClearValue(0);
			} else if (partition == starts->Partitions()) {
				// This should not be possible
				ClearValue(partition);
				throw std::runtime_error("SparseVector: deleting end partition.");
			} else {
				ClearValue(partition);
				starts->RemovePartition(partition);
				values->Delete(partition);
				// Its the previous partition now that gets smaller 
				partition--;
			}
		}
		starts->InsertText(partition, -1);
	}
	void Check() const {
		if (Length() < 0) {
			throw std::runtime_error("SparseVector: Length can not be negative.");
		}
		if (starts->Partitions() < 1) {
			throw std::runtime_error("SparseVector: Must always have 1 or more partitions.");
		}
		if (starts->Partitions() != values->Length() - 1) {
			throw std::runtime_error("SparseVector: Partitions and values different lengths.");
		}
		// The final element can not be set
		if (values->ValueAt(values->Length() - 1) != T()) {
			throw std::runtime_error("SparseVector: Unused style at end changed.");
		}
	}
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
