/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     27/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WXSTLVEC_G__
#define __WXSTLVEC_G__

#ifdef new
#undef new
#endif

#include <memory.h>
#include <string.h>  // imports memmove()
#include <stddef.h>
#if !defined(__WXMAC__) || defined(__DARWIN__)
#  include <sys/types.h>
#endif
#include <limits.h>
#include <new>

// the below macro used internally (see actual interface after this macro)

#define __DEFINE_STL_VECTOR_DEEP( vectorClass, Type ) class vectorClass {\
\
public:\
	typedef Type              value_type;\
	typedef value_type*	      iterator;\
	typedef const value_type* const_iterator;\
	typedef iterator		  pointer;\
	typedef const iterator    const_pointer;\
	typedef value_type&       reference;\
	typedef const value_type& const_reference;\
	typedef size_t            size_type;\
	typedef ptrdiff_t         difference_type;\
\
	typedef iterator       OutputIterator;\
	typedef const_iterator InputIterator;\
\
protected:\
\
	inline void PlacementCopy( const_iterator first, const_iterator last, iterator result )\
	{\
		while ( first != last ) \
			new (result++) value_type(*first++);\
	}\
\
	inline void ConstructObjects( iterator first, iterator last, const value_type& pattern )\
	{\
		while( first != last ) \
			new (first++) value_type(pattern);\
	}\
\
	inline void CopyObjects( iterator first, iterator last, iterator result )\
	{\
		while( first != last ) \
			*result++ = *first++;\
	}\
\
	inline void CopyObjectsBack( iterator first, iterator last, iterator result )\
	{\
		result += difference_type(last,first);\
\
		while( first != last ) \
			*(--result) = *(--last);\
	}\
\
public:\
\
	class reverse_iterator \
	{\
		friend class vectorClass;\
		friend class const_reverse_iterator;\
\
	public:\
		iterator mpPos;\
\
	public:\
\
		reverse_iterator() {}\
\
		reverse_iterator ( iterator pPos )\
		{\
			mpPos = pPos;\
		}\
	\
		int operator==( const reverse_iterator& rhs ) const { return (mpPos == rhs.mpPos); }\
		int operator!=( const reverse_iterator& rhs ) const { return (mpPos != rhs.mpPos); }\
\
		inline reverse_iterator( const reverse_iterator& other )\
		{\
			mpPos = other.mpPos;\
		}\
\
		inline const reverse_iterator& operator--() \
		{\
			--mpPos;\
			return *this;\
		}\
\
		inline reverse_iterator operator--(int)\
		{\
			reverse_iterator tmp = *this;\
			--mpPos;\
			return tmp;\
		}\
\
		inline const reverse_iterator & operator++() \
		{\
			++mpPos;\
			return *this;\
		}\
\
		inline reverse_iterator  operator++(int)\
		{\
			reverse_iterator tmp = *this;\
			++mpPos;\
			return tmp;\
		}\
\
		inline const_reference operator*() const { return *mpPos; }\
	};\
\
\
	class const_reverse_iterator \
	{\
	protected:\
		iterator mpPos;\
	public:\
\
		const_reverse_iterator() {}\
\
		const_reverse_iterator( const iterator pPos )\
		{\
			mpPos = pPos;\
		}\
	\
		int operator==( const const_reverse_iterator& rhs ) const { return (mpPos == rhs.mpPos); }\
		int operator!=( const const_reverse_iterator& rhs ) const { return (mpPos != rhs.mpPos); }\
\
		inline const_reverse_iterator( const reverse_iterator& other )\
		{\
			mpPos = other.mpPos;\
		}\
\
		inline const const_reverse_iterator& operator--() \
		{\
			--mpPos;\
			return *this;\
		}\
\
		inline const_reverse_iterator operator--(int)\
		{\
			const_reverse_iterator tmp = *this;\
			--mpPos;\
			return tmp;\
		}\
\
		inline const const_reverse_iterator & operator++() \
		{\
			++mpPos;\
			return *this;\
		}\
\
		inline const_reverse_iterator operator++(int)\
		{\
			const_reverse_iterator tmp = *this;\
			++mpPos;\
			return tmp;\
		}\
\
		inline const_reference operator*() const { return *mpPos; }\
	};\
\
protected:\
	\
	pointer mpStart;\
	pointer mpEnd;\
	pointer mpEndOfBuf;\
\
protected:\
\
	inline void quick_sort(int WXUNUSED(low), int WXUNUSED(hi)) \
	{\
	}\
\
	inline void DestructRange( iterator first, iterator last )\
	{\
		typedef value_type value_type_local;\
\
		while ( first != last ) \
		{\
			first->value_type_local::~value_type_local();\
			++first;\
		}\
	}\
\
	inline iterator DoInsert(iterator position, const value_type& x)\
	{\
		if ( mpEnd < mpEndOfBuf )\
		{\
			new (mpEnd) value_type(*(mpEnd-1) );\
	    \
			CopyObjectsBack( position, mpEnd, position + 1 );\
	    \
			*position = x;\
	    \
			++mpEnd;\
	    \
			return position;\
		}\
	    \
		size_type minBufLen = WXSTL_VECTOR_MIN_BUF_SIZE/sizeof(value_type);\
	    \
		size_type doubledSize = size()*2;\
	    \
		size_type newLen = ( doubledSize < minBufLen ) ? minBufLen : doubledSize;\
	    \
		iterator pNewStart = (iterator)( new char[newLen*sizeof(value_type)] );\
	    \
		PlacementCopy( mpStart, position, pNewStart );\
	    \
		iterator atPosition = pNewStart + difference_type( position - mpStart );\
	    \
		new (atPosition) value_type(x);\
	    \
		iterator newPos = atPosition;\
	    \
		++atPosition;\
	    \
		if ( mpStart ) \
		{\
			PlacementCopy( position, mpEnd, atPosition );\
			DestructRange( mpStart, mpEnd );\
			delete [](char*)mpStart;\
		}\
	    \
		mpEnd = atPosition + difference_type( mpEnd - position );\
	    \
		mpStart    = pNewStart;\
		mpEndOfBuf = pNewStart + newLen;\
	    \
		return newPos;\
	}\
\
public:\
\
	inline vectorClass() : mpStart(0), \
						   mpEnd(0),\
						   mpEndOfBuf(0)\
	{}\
\
	inline vectorClass( const_iterator first, const_iterator last )\
		: mpStart(0),\
		  mpEnd(0),\
		  mpEndOfBuf(0)\
		\
		{ while( first != last ) push_back( *first++ ); }\
\
	inline vectorClass( size_type n, const value_type& value = value_type() )\
		: mpStart(0),\
		  mpEnd(0),\
		  mpEndOfBuf(0)\
		\
		{ for( size_type i = 0; i != n; ++i ) push_back( value ); }\
\
	inline const vectorClass& operator=( const vectorClass& other )\
	{\
		if (mpStart) \
		{\
			DestructRange( begin(), end() );\
			delete [](char*)mpStart; \
		}\
\
		size_t newLen = difference_type( other.mpEndOfBuf - other.mpStart );\
\
		mpStart = (iterator)( new char[newLen*sizeof(value_type)] );\
\
		PlacementCopy( other.begin(), other.end(), mpStart );\
\
		mpEnd = mpStart + other.size();\
\
		mpEndOfBuf = mpStart + newLen;\
\
		return *this;\
	}\
\
	inline vectorClass( const vectorClass& other )\
		: mpStart(0),\
		  mpEnd(0),\
		  mpEndOfBuf(0)\
	{\
		this->operator=( other );\
	}\
\
	inline ~vectorClass() \
	{ \
		if (mpStart) \
		{\
			DestructRange( begin(), end() );\
			delete [](char*)mpStart; \
		}\
	}\
\
	inline iterator begin() { return mpStart; }\
\
	inline const_iterator begin() const { return mpStart; }\
\
	inline iterator end() { return mpEnd; }\
\
	inline const_iterator end() const { return mpEnd; }\
\
	inline size_type size() const { return (size_type)difference_type(mpEnd-mpStart); }\
\
	inline size_type max_size() const { return UINT_MAX/sizeof(value_type); }\
\
	inline size_type capacity() const \
			{ return difference_type(mpEndOfBuf-mpStart)/sizeof(value_type); }\
\
	inline int empty() const { return mpStart == mpEnd; }\
\
	inline reference operator[](size_type n) { return *(mpStart+n); }\
\
	inline const_reference operator[](size_type n) const { return *(mpStart+n); }\
\
	inline reference front() { return (*mpStart); }\
	\
	inline const_reference front() const { return (*mpStart); }\
\
	inline reference back() { return (*(mpEnd-1)); }\
\
	inline const_reference back() const { return (*(mpEnd-1)); }\
\
	inline void reserve(size_type WXUNUSED(n)) {}\
\
	inline void push_back(const value_type& x)\
	{\
		if ( mpEnd != mpEndOfBuf ) \
		{\
			new (mpEnd) value_type(x);\
			++mpEnd;\
		}\
		else\
			DoInsert( mpEnd, x );\
	}\
\
	inline iterator insert(iterator position, const value_type& x = value_type())\
	{\
		if ( position == mpEnd && mpEnd != mpEndOfBuf )\
		{\
			new (mpEnd) value_type(x);\
			++mpEnd;\
			return (mpEnd-1);\
		}\
		else return DoInsert( position, x );\
	}\
\
	inline void pop_back()\
	{\
		DestructRange( mpEnd-1, mpEnd );\
\
		--mpEnd;\
	}\
\
	inline void erase(iterator first, iterator last)\
	{\
		if ( last == mpEnd )\
		{\
			DestructRange( first, last );\
			mpEnd = first;\
			return;\
		}\
	    \
		CopyObjects( last, last + difference_type( mpEnd - last ), first );\
	    \
		iterator newEnd = mpEnd - difference_type( last - first );\
		DestructRange( newEnd, mpEnd );\
	    \
		mpEnd = newEnd;\
	}\
\
	inline void erase( iterator position )\
	{\
		erase( position, position + 1 );\
	}\
\
	inline void sort()\
	{\
		if ( size() < 2 ) return;\
		quick_sort( 0, size()-1 );\
	}\
}

/////////////////////////////// shallow-copy container ///////////////////////

#define __DEFINE_STL_VECTOR_SHALLOW( vectorClass, Type ) class vectorClass {\
\
public:\
	typedef Type              value_type;\
	typedef value_type*	      iterator;\
	typedef const value_type* const_iterator;\
	typedef iterator		  pointer;\
	typedef const iterator    const_pointer;\
	typedef value_type&       reference;\
	typedef const value_type& const_reference;\
	typedef size_t            size_type;\
	typedef ptrdiff_t         difference_type;\
\
	typedef iterator       OutputIterator;\
	typedef const_iterator InputIterator;\
\
protected:\
\
	inline void PlacementCopy( const_iterator first, const_iterator last, iterator result )\
	{\
		memcpy(result, first, int(difference_type(last-first)*sizeof(value_type)) );\
	}\
\
	inline void ConstructObjects( iterator first, iterator last, const value_type& pattern )\
	{\
			if ( sizeof(pattern) == 1 )\
			\
				memset( first, int(difference_type(last-first)/sizeof(value_type)), \
				        int(*((char*)&pattern)) );\
			else\
				while( first != last ) \
					*first++ = pattern;\
	}\
\
	inline void CopyObjects( iterator first, iterator last, iterator result )\
	{\
		memcpy(result, first, int(difference_type(last-first)*sizeof(value_type)) );\
	}\
\
	inline void CopyObjectsBack( iterator first, iterator last, iterator result )\
	{\
		memmove(result, first, int(difference_type(last-first)*sizeof(value_type)) );\
	}\
\
public:\
\
	class reverse_iterator \
	{\
		friend class vectorClass;\
		friend class const_reverse_iterator;\
\
	public:\
		iterator mpPos;\
\
	public:\
\
		reverse_iterator() {}\
\
		reverse_iterator ( iterator pPos )\
		{\
			mpPos = pPos;\
		}\
	\
		int operator==( const reverse_iterator& rhs ) const { return (mpPos == rhs.mpPos); }\
		int operator!=( const reverse_iterator& rhs ) const { return (mpPos != rhs.mpPos); }\
\
		inline reverse_iterator( const reverse_iterator& other )\
		{\
			mpPos = other.mpPos;\
		}\
\
		inline const reverse_iterator& operator--() \
		{\
			--mpPos;\
			return *this;\
		}\
\
		inline reverse_iterator operator--(int)\
		{\
			reverse_iterator tmp = *this;\
			--mpPos;\
			return tmp;\
		}\
\
		inline const reverse_iterator & operator++() \
		{\
			++mpPos;\
			return *this;\
		}\
\
		inline reverse_iterator  operator++(int)\
		{\
			reverse_iterator tmp = *this;\
			++mpPos;\
			return tmp;\
		}\
\
		inline const_reference operator*() const { return *mpPos; }\
	};\
\
\
	class const_reverse_iterator \
	{\
	protected:\
		iterator mpPos;\
	public:\
\
		const_reverse_iterator() {}\
\
		const_reverse_iterator( const iterator pPos )\
		{\
			mpPos = pPos;\
		}\
	\
		int operator==( const const_reverse_iterator& rhs ) const { return (mpPos == rhs.mpPos); }\
		int operator!=( const const_reverse_iterator& rhs ) const { return (mpPos != rhs.mpPos); }\
\
		inline const_reverse_iterator( const reverse_iterator& other )\
		{\
			mpPos = other.mpPos;\
		}\
\
		inline const const_reverse_iterator& operator--() \
		{\
			--mpPos;\
			return *this;\
		}\
\
		inline const_reverse_iterator operator--(int)\
		{\
			const_reverse_iterator tmp = *this;\
			--mpPos;\
			return tmp;\
		}\
\
		inline const const_reverse_iterator & operator++() \
		{\
			++mpPos;\
			return *this;\
		}\
\
		inline const_reverse_iterator operator++(int)\
		{\
			const_reverse_iterator tmp = *this;\
			++mpPos;\
			return tmp;\
		}\
\
		inline const_reference operator*() const { return *mpPos; }\
	};\
\
protected:\
	\
	pointer mpStart;\
	pointer mpEnd;\
	pointer mpEndOfBuf;\
\
protected:\
\
	inline void quick_sort(int WXUNUSED(low), int WXUNUSED(hi)) \
	{\
	}\
\
	inline void DestructRange( iterator WXUNUSED(first), iterator WXUNUSED(last))\
	{\
	}\
\
	inline iterator DoInsert(iterator position, const value_type& x)\
	{\
		if ( mpEnd < mpEndOfBuf )\
		{\
			new (mpEnd) value_type(*(mpEnd-1) );\
	    \
			CopyObjectsBack( position, mpEnd, position + 1 );\
	    \
			*position = x;\
	    \
			++mpEnd;\
	    \
			return position;\
		}\
	    \
		size_type minBufLen = WXSTL_VECTOR_MIN_BUF_SIZE/sizeof(value_type);\
	    \
		size_type doubledSize = size()*2;\
	    \
		size_type newLen = ( doubledSize < minBufLen ) ? minBufLen : doubledSize;\
	    \
		iterator pNewStart = (iterator)( new char[newLen*sizeof(value_type)] );\
	    \
		PlacementCopy( mpStart, position, pNewStart );\
	    \
		iterator atPosition = pNewStart + difference_type( position - mpStart );\
	    \
		new (atPosition) value_type(x);\
	    \
		iterator newPos = atPosition;\
	    \
		++atPosition;\
	    \
		if ( mpStart ) \
		{\
			PlacementCopy( position, mpEnd, atPosition );\
			DestructRange( mpStart, mpEnd );\
			delete [](char*)mpStart;\
		}\
	    \
		mpEnd = atPosition + difference_type( mpEnd - position );\
	    \
		mpStart    = pNewStart;\
		mpEndOfBuf = pNewStart + newLen;\
	    \
		return newPos;\
	}\
\
public:\
\
	inline vectorClass() : mpStart(0), \
						   mpEnd(0),\
						   mpEndOfBuf(0)\
	{}\
\
	inline vectorClass( const_iterator first, const_iterator last )\
		: mpStart(0),\
		  mpEnd(0),\
		  mpEndOfBuf(0)\
		\
		{ while( first != last ) push_back( *first++ ); }\
\
	inline vectorClass( size_type n, const value_type& value = value_type() )\
		: mpStart(0),\
		  mpEnd(0),\
		  mpEndOfBuf(0)\
		\
		{ for( size_type i = 0; i != n; ++i ) push_back( value ); }\
\
	inline const vectorClass& operator=( const vectorClass& other )\
	{\
		if (mpStart) \
		{\
			DestructRange( begin(), end() );\
			delete [](char*)mpStart; \
		}\
\
		size_t newLen = difference_type( other.mpEndOfBuf - other.mpStart );\
\
		mpStart = (iterator)( new char[newLen*sizeof(value_type)] );\
\
		PlacementCopy( other.begin(), other.end(), mpStart );\
\
		mpEnd = mpStart + other.size();\
\
		mpEndOfBuf = mpStart + newLen;\
\
		return *this;\
	}\
\
	inline vectorClass( const vectorClass& other )\
		: mpStart(0),\
		  mpEnd(0),\
		  mpEndOfBuf(0)\
	{\
		this->operator=( other );\
	}\
\
	inline ~vectorClass() \
	{ \
		if (mpStart) \
		{\
			DestructRange( begin(), end() );\
			delete [](char*)mpStart; \
		}\
	}\
\
	inline iterator begin() { return mpStart; }\
\
	inline const_iterator begin() const { return mpStart; }\
\
	inline iterator end() { return mpEnd; }\
\
	inline const_iterator end() const { return mpEnd; }\
\
	inline size_type size() const { return (size_type)difference_type(mpEnd-mpStart); }\
\
	inline size_type max_size() const { return UINT_MAX/sizeof(value_type); }\
\
	inline size_type capacity() const \
			{ return difference_type(mpEndOfBuf-mpStart)/sizeof(value_type); }\
\
	inline int empty() const { return mpStart == mpEnd; }\
\
	inline reference operator[](size_type n) { return *(mpStart+n); }\
\
	inline const_reference operator[](size_type n) const { return *(mpStart+n); }\
\
	inline reference front() { return (*mpStart); }\
	\
	inline const_reference front() const { return (*mpStart); }\
\
	inline reference back() { return (*(mpEnd-1)); }\
\
	inline const_reference back() const { return (*(mpEnd-1)); }\
\
	inline void reserve(size_type WXUNUSED(n)) {}\
\
	inline void push_back(const value_type& x)\
	{\
		if ( mpEnd != mpEndOfBuf ) \
		{\
			new (mpEnd) value_type(x);\
			++mpEnd;\
		}\
		else\
			DoInsert( mpEnd, x );\
	}\
\
	inline iterator insert(iterator position, const value_type& x = value_type())\
	{\
		if ( position == mpEnd && mpEnd != mpEndOfBuf )\
		{\
			new (mpEnd) value_type(x);\
			++mpEnd;\
			return (mpEnd-1);\
		}\
		else return DoInsert( position, x );\
	}\
\
	inline void pop_back()\
	{\
		DestructRange( mpEnd-1, mpEnd );\
\
		--mpEnd;\
	}\
\
	inline void erase(iterator first, iterator last)\
	{\
		if ( last == mpEnd )\
		{\
			DestructRange( first, last );\
			mpEnd = first;\
			return;\
		}\
	    \
		CopyObjects( last, last + difference_type( mpEnd - last ), first );\
	    \
		iterator newEnd = mpEnd - difference_type( last - first );\
		DestructRange( newEnd, mpEnd );\
	    \
		mpEnd = newEnd;\
	}\
\
	inline void erase( iterator position )\
	{\
		erase( position, position + 1 );\
	}\
\
	inline void sort()\
	{\
		if ( size() < 2 ) return;\
		quick_sort( 0, size()-1 );\
	}\
}



// redefine below symbol to change the default allocation unit of vector content buffer
#define WXSTL_VECTOR_MIN_BUF_SIZE 64

// defines vector class, where objects are copied
// using "deep-copy" sematics (i.e. by calling their copy constructors)

#define WXSTL_VECTOR(ELEMENT_CLASS) \
__DEFINE_STL_VECTOR_DEEP(_WXSTL_VECTOR_##ELEMENT_CLASS, ELEMENT_CLASS)

// defines vector class, where objects are copied
// using "shallow-copy" sematics (i.e. instead of calling
// their constructors, memcpy() and memmove() are used to copy their raw data)


#define WXSTL_VECTOR_SHALLOW_COPY(ELEMENT_CLASS) __DEFINE_STL_VECTOR_SHALLOW(_WXSTL_VECTORSC_##ELEMENT_CLASS, ELEMENT_CLASS)

#endif
