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

#ifndef __WXSTLLST_G__
#define __WXSTLLST_G__

#ifdef new
#undef new
#endif

#include <stddef.h>
#include <sys/types.h>
#include <memory.h>
#include <limits.h>
#include <new.h>

// VERSION:: 0.2 (copy-constructor/adign-op added)

// FOR NOW:: class-member operators "new" and "delete"
//           are ignored by list class, memory allocated 
//           and freed using global operators

typedef int Type;


// the below macro used internally (see actual interface after this macro)

#define __DEFINE_STL_LIST(listClass,Type) class \
 listClass \
{\
public:\
\
	typedef Type			  value_type;\
	typedef value_type*	      pointer;\
	typedef const value_type* const_pointer;\
	typedef value_type&       reference;\
	typedef const value_type& const_reference;\
	typedef size_t			  size_type;\
	typedef ptrdiff_t		  difference_type;\
\
protected:\
	struct list_node\
	{\
		list_node* mpNext;\
		list_node* mpPrev;\
		value_type mData;\
	};\
\
	typedef list_node* node_ref_type;\
\
	node_ref_type mpFreeListHead;\
	node_ref_type mpTerminator;\
	size_type     mSize;\
\
	inline node_ref_type AllocNode() \
	{ \
		if ( mpFreeListHead ) \
		{\
			node_ref_type pFreeNode = mpFreeListHead;\
			mpFreeListHead = mpFreeListHead->mpPrev;\
\
			return pFreeNode;\
		}\
		else\
		{\
			char* pHeapBlock = new char[sizeof(list_node)];\
\
			return (node_ref_type)pHeapBlock;\
		}\
	}\
\
	inline void DestroyFreeList()\
	{\
		while ( mpFreeListHead )\
		{\
			node_ref_type tmp = mpFreeListHead;\
			mpFreeListHead = mpFreeListHead->mpPrev;\
\
			delete [](char*)tmp;\
		}\
	}\
\
	inline void RecycleNode( node_ref_type pNode ) \
	{\
		pNode->mpPrev = mpFreeListHead;\
		mpFreeListHead = pNode;\
	}\
\
public:\
\
	class iterator \
	{\
	public:\
		node_ref_type mpNode;\
		friend class listClass;\
		friend class const_iterator;\
		friend class const_reverse_iterator;\
\
	protected:\
		iterator( node_ref_type pNode )\
		{\
			mpNode = pNode;\
		}\
	\
	public:\
		iterator() {}\
		int operator==( const iterator& rhs ) const { return (mpNode == rhs.mpNode); }\
		int operator!=( const iterator& rhs ) const { return (mpNode != rhs.mpNode); }\
\
		inline iterator( const iterator& other )\
		{\
			mpNode = other.mpNode;\
		}\
\
		inline const iterator& operator--() \
		{\
			mpNode = mpNode->mpPrev;\
			return *this;\
		}\
\
		inline iterator operator--(int)\
		{\
			iterator tmp = *this;\
			mpNode = mpNode->mpPrev;\
			return tmp;\
		}\
\
		inline const iterator& operator++() \
		{\
			mpNode = mpNode->mpNext;\
			return *this;\
		}\
\
		inline iterator operator++(int)\
		{\
			iterator tmp = *this;\
			mpNode = mpNode->mpNext;\
			return tmp;\
		}\
\
		inline reference operator*()       const { return mpNode->mData; }\
	};\
\
\
	class const_iterator \
	{\
	protected:\
		node_ref_type mpNode;\
		friend class listClass;\
\
	protected:\
		const_iterator( node_ref_type pNode )\
		{\
			mpNode = pNode;\
		}\
	\
	public:\
		\
		const_iterator() {}\
		int operator==( const const_iterator& rhs ) const { return (mpNode == rhs.mpNode); }\
		int operator!=( const const_iterator& rhs ) const { return (mpNode != rhs.mpNode); }\
\
\
		inline const_iterator( const iterator& other )\
		{\
			mpNode = other.mpNode;\
		}\
\
		inline const const_iterator& operator--() \
		{\
			mpNode = mpNode->mpPrev;\
			return *this;\
		}\
\
		inline const_iterator operator--(int)\
		{\
			const_iterator tmp = *this;\
			mpNode = mpNode->mpPrev;\
			return tmp;\
		}\
\
		inline const const_iterator& operator++() \
		{\
			mpNode = mpNode->mpNext;\
			return *this;\
		}\
\
		inline const_iterator operator++(int)\
		{\
			const_iterator tmp = *this;\
			mpNode = mpNode->mpNext;\
			return tmp;\
		}\
\
		inline const_reference operator*() const { return mpNode->mData; }\
	};\
\
	typedef iterator       OutputIterator;\
	typedef const_iterator InputIterator;\
\
	class reverse_iterator \
	{\
	public:\
		node_ref_type mpNode;\
		friend class listClass;\
		friend class const_reverse_iterator;\
\
	protected:\
		reverse_iterator ( node_ref_type pNode )\
		{\
			mpNode = pNode;\
		}\
	\
	public:\
\
		reverse_iterator() {}\
		int operator==( const reverse_iterator& rhs ) const { return (mpNode == rhs.mpNode); }\
		int operator!=( const reverse_iterator& rhs ) const { return (mpNode != rhs.mpNode); }\
\
		inline reverse_iterator( const reverse_iterator& other )\
		{\
			mpNode = other.mpNode;\
		}\
\
		inline const reverse_iterator& operator--() \
		{\
			mpNode = mpNode->mpNext;\
			return *this;\
		}\
\
		inline reverse_iterator operator--(int)\
		{\
			reverse_iterator tmp = *this;\
			mpNode = mpNode->mpPrev;\
			return tmp;\
		}\
\
		inline const reverse_iterator & operator++() \
		{\
			mpNode = mpNode->mpNext;\
			return *this;\
		}\
\
		inline reverse_iterator  operator++(int)\
		{\
			reverse_iterator tmp = *this;\
			mpNode = mpNode->mpPrev;\
			return tmp;\
		}\
\
		inline const_reference operator*() const { return mpNode->mData; }\
	};\
\
\
	class const_reverse_iterator \
	{\
	protected:\
		node_ref_type mpNode;\
		friend class listClass;\
\
	protected:\
		const_reverse_iterator( node_ref_type pNode )\
		{\
			mpNode = pNode;\
		}\
	\
	public:\
\
		const_reverse_iterator() {}\
		int operator==( const const_reverse_iterator& rhs ) const { return (mpNode == rhs.mpNode); }\
		int operator!=( const const_reverse_iterator& rhs ) const { return (mpNode != rhs.mpNode); }\
\
		inline const_reverse_iterator( const reverse_iterator& other )\
		{\
			mpNode = other.mpNode;\
		}\
\
		inline const const_reverse_iterator& operator--() \
		{\
			mpNode = mpNode->mpNext;\
			return *this;\
		}\
\
		inline const_reverse_iterator operator--(int)\
		{\
			const_reverse_iterator tmp = *this;\
			mpNode = mpNode->mpNext;\
			return tmp;\
		}\
\
		inline const const_reverse_iterator& operator++() \
		{\
			mpNode = mpNode->mpPrev;\
			return *this;\
		}\
\
		inline const_reverse_iterator operator++(int)\
		{\
			const_reverse_iterator tmp = *this;\
			mpNode = mpNode->mpPrev;\
			return tmp;\
		}\
\
		inline const_reference operator*() const { return mpNode->mData; }\
	};\
\
public:\
\
    inline listClass()\
			: mpFreeListHead( 0 ),\
			  mSize(0)\
	{\
		mpTerminator = AllocNode();\
		mpTerminator->mpPrev = mpTerminator->mpNext = mpTerminator;\
	}\
\
	listClass( const listClass& other )\
	{\
		mpTerminator = AllocNode();\
		mpTerminator->mpPrev = mpTerminator->mpNext = mpTerminator;\
\
		for( listClass::const_iterator i = other.begin(); i != other.end(); ++i )\
\
			push_back( (*i) );\
	}\
\
	inline const listClass& operator=( const listClass& rhs ) \
	{\
		erase( begin(), end() );\
\
		for( listClass::const_iterator i = rhs.begin(); i != rhs.end(); ++i )\
\
			push_back( (*i) );\
\
		return *this;\
	}\
\
	inline listClass(const_iterator first, const_iterator last)\
			: mpFreeListHead( 0 ),\
			  mSize(0)\
	\
		{ while( first != last ) push_back( *first++ ); }\
\
	inline listClass( size_type n, const value_type& value = value_type() )\
	\
		{ for( size_t i = 0; i != n; ++n ) push_back( value ); }\
\
	inline ~listClass() \
	{ \
		erase( begin(), end() ); \
\
		RecycleNode( mpTerminator );\
		DestroyFreeList();\
	}\
\
	inline iterator begin() { return iterator(mpTerminator->mpNext); }\
	\
	inline const_iterator begin() const \
		{ return const_iterator(mpTerminator->mpNext); }\
	\
	inline iterator end() { return iterator(mpTerminator); }\
\
	inline const_iterator end() const { return const_iterator(mpTerminator); }\
\
	inline reverse_iterator rbegin() \
		{ return reverse_iterator(mpTerminator->mpPrev); }\
\
	inline reverse_iterator rend() \
		{ return reverse_iterator(mpTerminator); }\
\
	inline const_reverse_iterator rbegin() const\
		{ return const_reverse_iterator(mpTerminator->mpPrev); }\
\
	inline const_reverse_iterator  rend() const\
		{ return const_reverse_iterator(mpTerminator); }\
\
	inline int empty() const { return (mSize == 0); }\
\
	inline size_type size() const { return mSize; }\
\
	inline size_type max_size() const { return UINT_MAX/sizeof(list_node); }\
\
	inline reference front() { return mpTerminator->mData; }\
\
	inline const_reference front() const { return mpTerminator->mData; }\
\
	inline reference back() { return mpTerminator->mpPrev->mData; }\
\
	inline const_reference back() const { return mpTerminator->mpPrev->mData; }\
\
	inline void push_front(const value_type& x) { insert( begin(), x ); }\
\
	inline void push_back(const value_type& x) { insert( end(), x ); }\
\
	iterator insert(iterator position, const value_type& x = value_type())\
	{\
		node_ref_type pNew = AllocNode();\
\
		node_ref_type pos = *((node_ref_type*)&position);\
\
		pNew->mpNext = pos;\
		pNew->mpPrev = pos->mpPrev;\
		pos->mpPrev->mpNext = pNew;\
		pos->mpPrev  = pNew;\
\
		new (&pNew->mData) value_type(x);\
\
		++mSize;\
\
		return iterator(pNew);\
	}\
\
	inline void insert(iterator position, const_iterator first, const_iterator last )\
	{\
		while( first != last ) insert( position, *first++ );\
	}\
\
	inline void splice( iterator position, listClass& other )\
	{\
		if ( other.begin() == other.end() ) return;\
\
		node_ref_type pTill = other.mpTerminator->mpPrev;\
		node_ref_type pFrom = other.begin().mpNode;\
\
		mpTerminator->mpPrev->mpNext = pFrom;\
		pFrom->mpPrev = mpTerminator->mpPrev->mpNext;\
\
		pTill->mpNext = mpTerminator;\
		mpTerminator->mpPrev = pTill;\
\
		other.mpTerminator->mpNext = \
			other.mpTerminator->mpPrev = other.mpTerminator;\
\
		mSize += other.mSize;\
		other.mSize = 0;\
	}\
\
	inline void splice( iterator position, listClass& other, iterator first, iterator last )\
	{\
		if ( first == last ) return;\
\
		size_type sz = 0;\
		iterator tmp = first;\
		while( tmp != last ) \
		{\
			++tmp;\
			++sz;\
		}\
\
		mSize += sz;\
		other.mSize -= sz;\
\
		node_ref_type pPos   = position.mpNode;\
		node_ref_type pFirst = first.mpNode;\
		node_ref_type pLast  = last.mpNode;\
		node_ref_type pTill  = last.mpNode->mpPrev;\
\
		pPos->mpPrev->mpNext = pFirst;\
		pPos->mpPrev = pTill;\
\
		pFirst->mpPrev->mpNext = last.mpNode;\
		pLast->mpPrev = pTill;\
\
		pFirst->mpPrev = pPos->mpPrev;\
		pTill->mpNext = pPos;\
	}\
\
	inline void pop_front() { erase( begin() ); }\
	inline void pop_back()  { erase( --end() );   }\
	\
	inline void erase(iterator position)\
	{\
		erase( position, ++position );\
	}\
	\
	inline void erase(iterator first, iterator last)\
	{\
		node_ref_type firstNode = *((node_ref_type*)&first);\
		node_ref_type lastNode = *((node_ref_type*)&last);\
\
		firstNode->mpPrev->mpNext = lastNode;\
		lastNode->mpPrev = firstNode->mpPrev;\
\
		while( firstNode != lastNode )\
		{\
			node_ref_type next = firstNode->mpNext;\
\
			typedef value_type value_type_local;\
			firstNode->mData.value_type_local::~value_type_local();\
\
			RecycleNode( firstNode );\
\
			firstNode = next;\
\
			--mSize;\
		}\
	}\
\
	inline void remove(const value_type& value)\
	{\
		for( iterator i = begin(); i != end(); ++i )\
		\
			if ( (*i) == value ) \
			{\
				erase( i ); break;\
			}\
	}\
\
	void sort()\
	{\
		if ( mSize < 2 ) return;\
\
		iterator from = begin();\
		iterator other_end = end();\
		--other_end;\
\
		for( size_type i = 0; i != mSize; ++i )\
		{\
			size_type nSwaps = 0;\
\
			iterator next = begin();\
			++next;\
\
			for( iterator j = begin(); j != other_end;  ++j )\
			{\
\
				if ( (*next) < (*j) )\
				{\
					value_type tmp = (*j);\
					(*j) = (*next);\
					(*next) = tmp;\
\
					++nSwaps;\
				}\
\
				++next;\
			}\
\
			if ( !nSwaps) break;\
\
			--other_end;\
		}\
	}\
}

// defines list class with the given element type
#define WXSTL_LIST(ELEMENT_CLASS)  __DEFINE_STL_LIST(\
\
_WXSTL_LIST_##ELEMENT_CLASS, ELEMENT_CLASS )

#endif