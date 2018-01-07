// Scintilla source code edit control
/** @file SplitVector.h
 ** Main data structure for holding arrays that handle insertions
 ** and deletions efficiently.
 **/
// Copyright 1998-2007 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef SPLITVECTOR_H
#define SPLITVECTOR_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

template <typename T>
class SplitVector {
protected:
	std::vector<T> body;
	T empty;	/// Returned as the result of out-of-bounds access.
	int lengthBody;
	int part1Length;
	int gapLength;	/// invariant: gapLength == body.size() - lengthBody
	int growSize;

	/// Move the gap to a particular position so that insertion and
	/// deletion at that point will not require much copying and
	/// hence be fast.
	void GapTo(int position) {
		if (position != part1Length) {
			if (position < part1Length) {
				// Moving the gap towards start so moving elements towards end
				std::move_backward(
					body.data() + position,
					body.data() + part1Length,
					body.data() + gapLength + part1Length);
			} else {	// position > part1Length
				// Moving the gap towards end so moving elements towards start
				std::move(
					body.data() + part1Length + gapLength,
					body.data() + gapLength + position,
					body.data() + part1Length);
			}
			part1Length = position;
		}
	}

	/// Check that there is room in the buffer for an insertion,
	/// reallocating if more space needed.
	void RoomFor(int insertionLength) {
		if (gapLength <= insertionLength) {
			while (growSize < static_cast<int>(body.size() / 6))
				growSize *= 2;
			ReAllocate(static_cast<int>(body.size()) + insertionLength + growSize);
		}
	}

	void Init() {
		body.clear();
		body.shrink_to_fit();
		growSize = 8;
		lengthBody = 0;
		part1Length = 0;
		gapLength = 0;
	}

public:
	/// Construct a split buffer.
	SplitVector() : empty() {
		Init();
	}

	// Deleted so SplitVector objects can not be copied.
	SplitVector(const SplitVector &) = delete;
	void operator=(const SplitVector &) = delete;

	~SplitVector() {
	}

	int GetGrowSize() const {
		return growSize;
	}

	void SetGrowSize(int growSize_) {
		growSize = growSize_;
	}

	/// Reallocate the storage for the buffer to be newSize and
	/// copy exisiting contents to the new buffer.
	/// Must not be used to decrease the size of the buffer.
	void ReAllocate(int newSize) {
		if (newSize < 0)
			throw std::runtime_error("SplitVector::ReAllocate: negative size.");

		if (newSize > static_cast<int>(body.size())) {
			// Move the gap to the end
			GapTo(lengthBody);
			gapLength += newSize - static_cast<int>(body.size());
			// RoomFor implements a growth strategy but so does vector::resize so
			// ensure vector::resize allocates exactly the amount wanted by
			// calling reserve first.
			body.reserve(newSize);
			body.resize(newSize);
		}
	}

	/// Retrieve the element at a particular position.
	/// Retrieving positions outside the range of the buffer returns empty or 0.
	const T& ValueAt(int position) const {
		if (position < part1Length) {
			if (position < 0) {
				return empty;
			} else {
				return body[position];
			}
		} else {
			if (position >= lengthBody) {
				return empty;
			} else {
				return body[gapLength + position];
			}
		}
	}

	/// Set the element at a particular position.
	/// Setting positions outside the range of the buffer performs no assignment
	/// but asserts in debug builds.
	template <typename ParamType>
	void SetValueAt(int position, ParamType&& v) {
		if (position < part1Length) {
			PLATFORM_ASSERT(position >= 0);
			if (position < 0) {
				;
			} else {
				body[position] = std::move(v);
			}
		} else {
			PLATFORM_ASSERT(position < lengthBody);
			if (position >= lengthBody) {
				;
			} else {
				body[gapLength + position] = std::move(v);
			}
		}
	}

	/// Retrieve the element at a particular position.
	/// The position must be within bounds or an assertion is triggered.
	const T &operator[](int position) const {
		PLATFORM_ASSERT(position >= 0 && position < lengthBody);
		if (position < part1Length) {
			return body[position];
		} else {
			return body[gapLength + position];
		}
	}

	/// Retrieve reference to the element at a particular position.
	/// This, instead of the const variant, can be used to mutate in-place.
	/// The position must be within bounds or an assertion is triggered.
	T &operator[](int position) {
		PLATFORM_ASSERT(position >= 0 && position < lengthBody);
		if (position < part1Length) {
			return body[position];
		} else {
			return body[gapLength + position];
		}
	}

	/// Retrieve the length of the buffer.
	int Length() const {
		return lengthBody;
	}

	/// Insert a single value into the buffer.
	/// Inserting at positions outside the current range fails.
	void Insert(int position, T v) {
		PLATFORM_ASSERT((position >= 0) && (position <= lengthBody));
		if ((position < 0) || (position > lengthBody)) {
			return;
		}
		RoomFor(1);
		GapTo(position);
		body[part1Length] = std::move(v);
		lengthBody++;
		part1Length++;
		gapLength--;
	}

	/// Insert a number of elements into the buffer setting their value.
	/// Inserting at positions outside the current range fails.
	void InsertValue(int position, int insertLength, T v) {
		PLATFORM_ASSERT((position >= 0) && (position <= lengthBody));
		if (insertLength > 0) {
			if ((position < 0) || (position > lengthBody)) {
				return;
			}
			RoomFor(insertLength);
			GapTo(position);
			std::fill(body.data() + part1Length, body.data() + part1Length + insertLength, v);
			lengthBody += insertLength;
			part1Length += insertLength;
			gapLength -= insertLength;
		}
	}

	/// Add some new empty elements.
	/// InsertValue is good for value objects but not for unique_ptr objects
	/// since they can only be moved from once.
	void InsertEmpty(int position, int insertLength) {
		PLATFORM_ASSERT((position >= 0) && (position <= lengthBody));
		if (insertLength > 0) {
			if ((position < 0) || (position > lengthBody)) {
				return;
			}
			RoomFor(insertLength);
			GapTo(position);
			for (int elem = part1Length; elem < part1Length + insertLength; elem++) {
				T emptyOne = {};
				body[elem] = std::move(emptyOne);
			}
			lengthBody += insertLength;
			part1Length += insertLength;
			gapLength -= insertLength;
		}
	}

	/// Ensure at least length elements allocated,
	/// appending zero valued elements if needed.
	void EnsureLength(int wantedLength) {
		if (Length() < wantedLength) {
			InsertEmpty(Length(), wantedLength - Length());
		}
	}

	/// Insert text into the buffer from an array.
	void InsertFromArray(int positionToInsert, const T s[], int positionFrom, int insertLength) {
		PLATFORM_ASSERT((positionToInsert >= 0) && (positionToInsert <= lengthBody));
		if (insertLength > 0) {
			if ((positionToInsert < 0) || (positionToInsert > lengthBody)) {
				return;
			}
			RoomFor(insertLength);
			GapTo(positionToInsert);
			std::copy(s + positionFrom, s + positionFrom + insertLength, body.data() + part1Length);
			lengthBody += insertLength;
			part1Length += insertLength;
			gapLength -= insertLength;
		}
	}

	/// Delete one element from the buffer.
	void Delete(int position) {
		PLATFORM_ASSERT((position >= 0) && (position < lengthBody));
		if ((position < 0) || (position >= lengthBody)) {
			return;
		}
		DeleteRange(position, 1);
	}

	/// Delete a range from the buffer.
	/// Deleting positions outside the current range fails.
	void DeleteRange(int position, int deleteLength) {
		PLATFORM_ASSERT((position >= 0) && (position + deleteLength <= lengthBody));
		if ((position < 0) || ((position + deleteLength) > lengthBody)) {
			return;
		}
		if ((position == 0) && (deleteLength == lengthBody)) {
			// Full deallocation returns storage and is faster
			body.clear();
			body.shrink_to_fit();
			Init();
		} else if (deleteLength > 0) {
			GapTo(position);
			lengthBody -= deleteLength;
			gapLength += deleteLength;
		}
	}

	/// Delete all the buffer contents.
	void DeleteAll() {
		DeleteRange(0, static_cast<int>(lengthBody));
	}

	/// Retrieve a range of elements into an array
	void GetRange(T *buffer, int position, int retrieveLength) const {
		// Split into up to 2 ranges, before and after the split then use memcpy on each.
		int range1Length = 0;
		if (position < part1Length) {
			const int part1AfterPosition = part1Length - position;
			range1Length = retrieveLength;
			if (range1Length > part1AfterPosition)
				range1Length = part1AfterPosition;
		}
		std::copy(body.data() + position, body.data() + position + range1Length, buffer);
		buffer += range1Length;
		position = static_cast<int>(position + range1Length + gapLength);
		int range2Length = retrieveLength - range1Length;
		std::copy(body.data() + position, body.data() + position + range2Length, buffer);
	}

	/// Compact the buffer and return a pointer to the first element.
	T *BufferPointer() {
		RoomFor(1);
		GapTo(lengthBody);
		T emptyOne = {};
		body[lengthBody] = std::move(emptyOne);
		return body.data();
	}

	/// Return a pointer to a range of elements, first rearranging the buffer if
	/// needed to make that range contiguous.
	T *RangePointer(int position, int rangeLength) {
		if (position < part1Length) {
			if ((position + rangeLength) > part1Length) {
				// Range overlaps gap, so move gap to start of range.
				GapTo(position);
				return body.data() + position + gapLength;
			} else {
				return body.data() + position;
			}
		} else {
			return body.data() + position + gapLength;
		}
	}

	/// Return the position of the gap within the buffer.
	int GapPosition() const {
		return part1Length;
	}
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
