// Scintilla source code edit control
// SVector.h - a simple expandable vector
// Copyright 1998-1999 by Neil Hodgson <neilh@hare.net.au>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef SVECTOR_H
#define SVECTOR_H

// A simple expandable vector. 
// T must support assignment.
// Storage not allocated for elements until an element is used. 
// This makes it very lightweight unless used so is a good match for optional features.
template<class T, int sizeIncrement>
class SVector {
	T *v;
	unsigned int size;	// Number of elements allocated
	unsigned int len;	// Number of elements in vector
	bool allocFailure;	// A memory allocation call has failed
	
	// Internally allocate more elements than the user wants to avoid 
	// thrashng the memory allocator
	void SizeTo(int newSize) {
		if (newSize < sizeIncrement)
			newSize += sizeIncrement;
		else 
			newSize = (newSize * 3) / 2;
		T* newv = new T[newSize];
		if (!newv) {
			allocFailure = true;
			return;
		}
		size = newSize;
		for (int i=0; i<len; i++) {
			newv[i] = v[i];
		}
		delete []v;
		v = newv;
	}
	
public:
	SVector() {
		allocFailure = false;
		v = 0;
		len = 0;
		size = 0;
	}
	~SVector() {
		Free();
	}
	SVector(const SVector &other) {
		allocFailure = false;
		v = 0;
		len = 0;
		size = 0;
		if (other.Length() > 0) {
			SizeTo(other.Length());
			if (!allocFailure) {
				for (int i=0;i<other.Length();i++)
					v[i] = other.v[i];
				len = other.Length();
			}
		}
	}
	SVector &operator=(const SVector &other) {
		if (this != &other) {
			delete []v;
			allocFailure = false;
			v = 0;
			len = 0;
			size = 0;
			if (other.Length() > 0) {
				SizeTo(other.Length());
				if (!allocFailure) {
					for (int i=0;i<other.Length();i++)
						v[i] = other.v[i];
				}
				len = other.Length();
			}
		}
		return *this;
	}
	T &operator[](unsigned int i) {
		if (i >= len) {
			if (i >= size) {
				SizeTo(i);
			}
			len = i+1;
		}
		return v[i];
	}
	void Free() {
		delete []v;
		v = 0;
		size = 0;
		len = 0;
	}
	void SetLength(int newLen) {
		if (newLength > len) {
			if (newLength >= size) {
				SizeTo(newLength);
			}
		}
		len = newLen;
	}
	int Length() const {
		return len;
	}
};

#endif
