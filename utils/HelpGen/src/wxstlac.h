/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     27/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WXSTLAC_G__
#define __WXSTLAC_G__

#ifdef new
#undef new
#endif

#include <stddef.h>
#if !defined(__WXMAC__) || defined(__DARWIN__)
#  include <sys/types.h>
#endif
#include <memory.h>
#include <limits.h>
/* #include <new.h> */

// the below macro used internally (see actual interface after this macro)

// arguments:
//
//      ARG_IS_UNIQUE
//      ASSOC_CONT_CLASS_NAME
//
//      ARG_VALUE_TYPE
//      ARG_KEY_TYPE
//      ARG_ACTUAL_VALUE_TYPE
//
//      _KEY_NAME
//      _VALUE_NAME
//
//      _X_KEY_NAME
//      _X_VALUE_NAME
//
//      _INSERT_METHOD_DEFINITION

#define __DEFINE_ASOC_CLASS( ARG_IS_UNIQUE, \
FUNCTOR,\
ASSOC_CONT_CLASS_NAME, \
ARG_VALUE_TYPE, \
ARG_KEY_TYPE, \
ARG_ACTUAL_VALUE_TYPE, \
_KEY_NAME, \
_VALUE_NAME, \
_X_KEY_NAME, \
_X_VALUE_NAME, \
_INSERT_METHOD_DEFINITION \
) class \
ASSOC_CONT_CLASS_NAME\
{\
protected:\
\
public:\
    typedef ARG_VALUE_TYPE        value_type;\
    typedef ARG_KEY_TYPE          key_type;\
    typedef ARG_ACTUAL_VALUE_TYPE actual_value_type;\
\
	typedef value_type*			  pointer;\
	typedef value_type&			  reference;\
\
	typedef const value_type&	  const_reference;\
\
	typedef FUNCTOR				  key_compare;\
	typedef key_compare           Compare;\
\
protected:\
\
	struct tree_node \
	{\
		tree_node*  m_pParent;\
		tree_node*  mpLeft;\
		tree_node*  mpRight;\
\
		value_type  mData;\
	};\
\
public:\
	typedef tree_node* node_ref_type;\
protected:\
\
	node_ref_type   mpRoot;\
	node_ref_type   mpLeftMost;\
	node_ref_type   mpRightMost;\
\
	node_ref_type   mpFreeListHead;\
	int             mKeyIsUnique;\
\
	key_compare     mCmpFunctorObj;\
\
public:\
\
	static inline node_ref_type next( node_ref_type pNode )\
	{\
		if ( pNode->mpRight ) \
		{\
			pNode = pNode->mpRight;\
\
			while ( pNode->mpLeft ) pNode = pNode->mpLeft;\
\
			return pNode;\
		}\
		else\
		if ( pNode->m_pParent )\
		{\
			if ( pNode == pNode->m_pParent->mpLeft )\
\
				return pNode->m_pParent;\
\
			pNode = pNode->m_pParent;\
\
			node_ref_type prevNode = pNode;\
			pNode = pNode->m_pParent;\
\
			while(pNode)\
			{\
				if ( pNode->mpRight &&\
					 pNode->mpRight != prevNode\
				   ) return pNode;\
\
				prevNode = pNode;\
				pNode= pNode->m_pParent;\
			}\
\
			return 0;\
		}\
		else\
			return 0;\
	}\
\
	static inline node_ref_type prev( node_ref_type pNode )\
	{\
		if ( pNode->mpLeft ) \
		{\
			pNode = pNode->mpLeft;\
\
			while ( pNode->mpRight ) pNode = pNode->mpRight;\
\
			return pNode;\
		}\
		else\
		if ( pNode->m_pParent )\
		{\
			if ( pNode == pNode->m_pParent->mpRight )\
				return pNode->m_pParent;\
\
			pNode = pNode->m_pParent;\
\
			node_ref_type prevNode = pNode;\
			pNode = pNode->m_pParent;\
\
			while(pNode)\
			{\
				if ( pNode->mpLeft &&\
					 pNode->mpLeft != prevNode\
				   ) return pNode;\
\
				prevNode = pNode;\
				pNode= pNode->m_pParent;\
			}\
\
			return 0;\
		}\
		else \
			return 0;\
	}\
\
protected:\
\
	inline int are_equel( const key_type& x, const key_type& y )\
	{\
		return ( !mCmpFunctorObj(x,y) && !mCmpFunctorObj(y,x) );\
	}\
\
	inline int is_less( const key_type& x, const key_type& y )\
	{\
		return mCmpFunctorObj(x,y);\
	}\
\
	static inline const actual_value_type& value( node_ref_type pNode )\
	{\
		return pNode->_VALUE_NAME;\
	}\
\
	static inline const key_type& key( node_ref_type pNode )\
	{\
		return pNode->_KEY_NAME;\
	}\
\
	inline node_ref_type AllocNode() \
	{ \
		if ( mpFreeListHead ) \
		{\
			node_ref_type pFreeNode = mpFreeListHead;\
			mpFreeListHead = mpFreeListHead->mpLeft;\
\
			return pFreeNode;\
		}\
		else\
		{\
			char* pHeapBlock = new char[sizeof(tree_node)];\
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
			mpFreeListHead = mpFreeListHead->mpLeft;\
\
			delete [](char*)tmp;\
		}\
	}\
\
	inline void RecycleNode( node_ref_type pNode ) \
	{\
		pNode->mpLeft = mpFreeListHead;\
		mpFreeListHead = pNode;\
	}\
\
	inline node_ref_type do_insert(const value_type& x = value_type() )\
	{\
		node_ref_type pNewNode = AllocNode();\
\
		pNewNode->m_pParent = \
			pNewNode->mpLeft =\
				pNewNode->mpRight = 0;\
\
	    node_ref_type pCurrent = mpRoot;\
		node_ref_type pParent = 0;\
    \
		while (pCurrent) \
		{\
			if ( mKeyIsUnique && are_equel( _X_KEY_NAME, value(pCurrent) ) )\
			{\
				RecycleNode(pNewNode);\
				return 0;\
			}\
\
			pParent = pCurrent;\
\
			pCurrent = is_less( _X_KEY_NAME, value(pCurrent) ) \
				? pCurrent->mpLeft \
				: pCurrent->mpRight;\
		}\
    \
		pNewNode->m_pParent = pParent;\
\
	    if(pParent)\
\
			if( is_less(_X_KEY_NAME, value(pParent) ) )\
            \
				pParent->mpLeft = pNewNode;\
			else\
				pParent->mpRight = pNewNode;\
			else\
				mpRoot = pNewNode;\
\
		new ( &pNewNode->_KEY_NAME   ) key_type(_X_KEY_NAME);\
		new ( &pNewNode->_VALUE_NAME ) actual_value_type(_X_VALUE_NAME);\
\
		if ( prev(pNewNode) == 0 ) mpLeftMost = pNewNode;\
		if ( next(pNewNode) == 0 ) mpRightMost = pNewNode;\
\
		return pNewNode;\
	}\
\
	friend class iterator;\
\
public:\
\
	class iterator;\
	class const_iterator;\
\
	class iterator \
	{\
	public:\
		node_ref_type mpNode;\
		friend class CONT_CLASS_NAME;\
		friend class const_iterator;\
		friend class const_reverse_iterator;\
\
		inline iterator( node_ref_type pNode )\
		{\
			mpNode = pNode;\
		}\
	\
	public:\
		inline iterator() {}\
		inline int operator==( const iterator& rhs ) const { return (mpNode == rhs.mpNode); }\
		inline int operator!=( const iterator& rhs ) const { return (mpNode != rhs.mpNode); }\
\
		inline iterator( const iterator& other )\
		{\
			mpNode = other.mpNode;\
		}\
\
		inline const iterator& operator=( const iterator& other )\
		{\
			mpNode = other.mpNode;\
			return *this;\
		}\
\
		inline const iterator& operator--() \
		{\
			mpNode = ASSOC_CONT_CLASS_NAME::prev(mpNode);\
			return *this;\
		}\
\
		inline iterator operator--(int)\
		{\
			iterator tmp = *this;\
			mpNode = ASSOC_CONT_CLASS_NAME::prev(mpNode);\
			return tmp;\
		}\
\
		inline const iterator& operator++() \
		{\
			mpNode = ASSOC_CONT_CLASS_NAME::next(mpNode);\
			return *this;\
		}\
\
		inline iterator operator++(int)\
		{\
			iterator tmp = *this;\
			mpNode = ASSOC_CONT_CLASS_NAME::next(mpNode);\
			return tmp;\
		}\
\
		inline reference operator*() const { return mpNode->mData; }\
	};\
\
\
	class const_iterator \
	{\
	public:\
		node_ref_type mpNode;\
		friend class CONT_CLASS_NAME;\
		friend class const_reverse_iterator;\
\
		inline const_iterator( node_ref_type pNode )\
		{\
			mpNode = pNode;\
		}\
	\
	public:\
		inline const_iterator() {}\
\
		inline int operator==( const const_iterator& rhs ) const { return (mpNode == rhs.mpNode); }\
		inline int operator!=( const const_iterator& rhs ) const { return (mpNode != rhs.mpNode); }\
\
		inline const_iterator( const iterator& other )\
		{\
			mpNode = other.mpNode;\
		}\
\
		inline const_iterator( const const_iterator& other )\
		{\
			mpNode = other.mpNode;\
		}\
\
		inline const const_iterator& operator=( const const_iterator& other )\
		{\
			mpNode = other.mpNode;\
			return *this;\
		}\
\
		inline const const_iterator& operator--() \
		{\
			mpNode = ASSOC_CONT_CLASS_NAME::prev(mpNode);\
			return *this;\
		}\
\
		inline const_iterator operator--(int)\
		{\
			const_iterator tmp = *this;\
			mpNode = ASSOC_CONT_CLASS_NAME::prev(mpNode);\
			return tmp;\
		}\
\
		inline const const_iterator& operator++() \
		{\
			mpNode = ASSOC_CONT_CLASS_NAME::next(mpNode);\
			return *this;\
		}\
\
		inline const_iterator operator++(int)\
		{\
			const_iterator tmp = *this;\
			mpNode = ASSOC_CONT_CLASS_NAME::next(mpNode);\
			return tmp;\
		}\
\
		inline const_reference operator*() const { return mpNode->mData; }\
	};\
\
public:\
\
	inline ASSOC_CONT_CLASS_NAME( key_compare cmpFunctorObj = key_compare(),\
								  int keyIsUnique = ARG_IS_UNIQUE )\
			: mpFreeListHead( 0 ),\
			  mKeyIsUnique( keyIsUnique ),\
			  mCmpFunctorObj( cmpFunctorObj )\
	{\
		mpLeftMost = 0;\
		mpRightMost = 0;\
		mpRoot = 0;\
	}\
\
	inline ~ASSOC_CONT_CLASS_NAME() \
	{ \
		erase( begin(), end() ); \
\
		DestroyFreeList();\
	}\
\
	inline iterator begin() { return mpLeftMost; }\
	inline iterator end()   { return 0; }\
\
	inline const_iterator begin() const { return mpLeftMost; }\
	inline const_iterator end()   const { return 0; }\
\
	inline iterator lower_bound( const key_type& x )\
	{ \
		node_ref_type pCurrent = mpRoot;\
    \
		while( pCurrent )\
		{\
			node_ref_type pParent = pCurrent;\
\
			if( are_equel( x, key(pCurrent) ) )\
				\
				return (pCurrent);\
			else\
				pCurrent = is_less( x, key(pCurrent) ) \
					? pCurrent->mpLeft \
					: pCurrent->mpRight;\
\
			if ( !pCurrent ) return (pParent);\
		}\
\
		return begin();\
	}\
\
	inline const_iterator lower_bound( const key_type& x ) const\
\
		{ return const_iterator( lower_bound(x).mpNode ); }\
\
	inline iterator upper_bound( const key_type& x )\
	{\
		node_ref_type pCurrent = mpRoot;\
    \
		while( pCurrent )\
		{\
			node_ref_type pParent = pCurrent;\
\
			if( are_equel( x, key(pCurrent) ) )\
				\
				return (pCurrent);\
			else\
				pCurrent = is_less( x, key(pCurrent) ) \
					? pCurrent->mpLeft \
					: pCurrent->mpRight;\
\
			if ( !pCurrent ) return next(pParent);\
		}\
\
		return end();\
	}\
\
	inline const_iterator upper_bound( const key_type& x ) const\
\
		{ return const_iterator( upper_bound(x).mpNode ); }\
\
	inline iterator find( const key_type& x )\
	{\
		node_ref_type pCurrent = mpRoot;\
    \
		while( pCurrent )\
		{\
			if( are_equel( x, key(pCurrent) ) )\
				\
				return (pCurrent);\
			else\
				pCurrent = is_less( x, key(pCurrent) ) \
					? pCurrent->mpLeft \
					: pCurrent->mpRight;\
		}\
\
		return end();\
	}\
\
	inline const_iterator find( const key_type& x ) const\
\
		{ return const_iterator( find(x).mpNode ); }\
\
	inline void erase(iterator first, iterator last)\
	{\
		if ( first.mpNode == 0 ) return;\
\
		while( first != last ) \
		{\
			iterator next = first;\
			++next;\
			erase( first );\
			first = next;\
		}\
	}\
\
	inline void erase(iterator position)\
	{\
		if ( position.mpNode == 0 ) return;\
\
		node_ref_type pZ = position.mpNode;\
		node_ref_type pX, pY;\
\
		if ( pZ == mpLeftMost ) mpLeftMost   = next(pZ);\
		if ( pZ == mpRightMost ) mpRightMost = prev( pZ );\
\
        if ( !pZ->mpLeft || !pZ->mpRight )\
	    \
            pY = pZ;\
        else \
		{\
            pY = pZ->mpRight;\
	    \
            while (pY->mpLeft) \
				\
				pY = pY->mpLeft;\
        }\
	    \
        if ( pY->mpLeft)\
	    \
            pX = pY->mpLeft;\
        else\
            pX = pY->mpRight;\
	    \
        if ( pX ) pX->m_pParent = pY->m_pParent;\
	    \
        if (pY->m_pParent)\
	    \
            if (pY == pY->m_pParent->mpLeft )\
	    \
                pY->m_pParent->mpLeft = pX;\
		    else\
                pY->m_pParent->mpRight = pX;\
        else\
            mpRoot = pX;\
	    \
		node_ref_type toRemove = 0;\
		\
        if (pY != pZ) {\
	    \
            pY->mpLeft = pZ->mpLeft;\
	    \
            if (pY->mpLeft) pY->mpLeft->m_pParent = pY;\
	    \
            pY->mpRight = pZ->mpRight;\
	    \
            if ( pY->mpRight ) \
				\
				pY->mpRight->m_pParent = pY;\
	    \
            pY->m_pParent = pZ->m_pParent;\
	    \
            if (pZ->m_pParent)\
	    \
                if (pZ == pZ->m_pParent->mpLeft)\
	    \
                    pZ->m_pParent->mpLeft = pY;\
                else\
                    pZ->m_pParent->mpRight = pY;\
            else\
                mpRoot = pY;\
	    \
            toRemove = pZ;\
        } \
		else \
            toRemove = pY;\
	    \
		value(toRemove).~actual_value_type();\
		key(toRemove).~actual_value_type();\
\
		RecycleNode( toRemove );\
	}\
\
	_INSERT_METHOD_DEFINITION\
}

// do not undefine ___WXSTL_COMMA, where associated containers are defined!
// (it is used as workaround for constraints of C-Preprocessor's nested macros)

#define ___WXSTL_COMMA ,

#define __DEFINE_MAP(ARG_IS_UNIQUE, KEY_TYPE, VAL_TYPE, FUNCTOR ) \
__DEFINE_ASOC_CLASS( ARG_IS_UNIQUE,\
FUNCTOR,\
__WXSTLMAP_##KEY_TYPE##VAL_TYPE##ARG_IS_UNIQUE, \
struct key_value_pair   { KEY_TYPE first ; \
                          VAL_TYPE second;\
                          key_value_pair() {}\
                          key_value_pair( const KEY_TYPE& key ___WXSTL_COMMA const VAL_TYPE& value ) \
                            : first(key) ___WXSTL_COMMA second( value ) {} \
                         } , \
KEY_TYPE,\
VAL_TYPE,\
mData.first, mData.second, x.first, x.second, \
struct insert_result_iterator\
{\
	iterator first;\
	int      second;\
};\
inline insert_result_iterator insert( const value_type& x )\
{\
	insert_result_iterator result;\
\
	result.first = do_insert(x);\
	result.second  = ( result.first == end() ) ? 0 : 1;\
\
	return result;\
} )

#define __DEFINE_SET(ARG_IS_UNIQUE, KEY_TYPE, FUNCTOR ) \
__DEFINE_ASOC_CLASS( ARG_IS_UNIQUE,\
FUNCTOR,\
__WXSTLSET_##TYPE##ARG_IS_UNIQUE, \
KEY_TYPE,\
KEY_TYPE,\
KEY_TYPE,\
mData, mData, x, x, \
struct insert_result_iterator\
{\
	iterator first;\
	int      second;\
};\
inline insert_result_iterator insert( const value_type& x )\
{\
	insert_result_iterator result;\
\
	result.first = do_insert(x);\
	result.second  = ( result.first == end() ) ? 0 : 1;\
\
	return result;\
} )

// helper macros to create functor objects for associative containers of the given type

#define LESS_THEN_FUNCTOR(TYPE) struct \
{ inline int operator()(const TYPE& x, const TYPE& y ) const { return x < y; } }

#define GREATER_THEN_FUNCTOR(TYPE) struct \
{ inline int operator()(const TYPE& x, const TYPE& y ) const { return x > y; } }

// functor argument should be created using the two above macros
// or passing own class with method "operator()(const TYPE&,cosnt TYPE&)" defined in it

#define WXSTL_MAP( KEY_TYPE, VALUE_TYPE, FUNCTOR )      __DEFINE_MAP( 1 ,KEY_TYPE, VALUE_TYPE, FUNCTOR)
#define WXSTL_MULTIMAP( KEY_TYPE, VALUE_TYPE, FUNCTOR ) __DEFINE_MAP( 0 ,KEY_TYPE, VALUE_TYPE, FUNCTOR)
#define WXSTL_SET( KEY_TYPE, FUNCTOR )                  __DEFINE_SET( 1 ,KEY_TYPE, FUNCTOR )
#define WXSTL_MULTISET( KEY_TYPE, FUNCTOR )             __DEFINE_SET( 0 ,KEY_TYPE, FUNCTOR )

#endif
