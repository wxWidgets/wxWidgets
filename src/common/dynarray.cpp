///////////////////////////////////////////////////////////////////////////////
// Name:        dynarray.cpp
// Purpose:     implementation of wxBaseArray class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     12.09.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers
// ============================================================================

#ifdef __GNUG__
#pragma implementation "dynarray.h"
#endif

#include  "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#include "wx/dynarray.h"
#include "wx/intl.h"

#include <stdlib.h>
#include <string.h> // for memmove

#ifndef max
  #define max(a, b)   (((a) > (b)) ? (a) : (b))
#endif

// we cast the value to long from which we cast it to void * in IndexForInsert:
// this can't work if the pointers are not big enough
wxCOMPILE_TIME_ASSERT( sizeof(long) <= sizeof(void *),
                       wxArraySizeOfPtrLessSizeOfLong ); // < 32 symbols

// ============================================================================
// constants
// ============================================================================

// size increment = max(50% of current size, ARRAY_MAXSIZE_INCREMENT)
#define   ARRAY_MAXSIZE_INCREMENT    4096

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxBaseArray - dynamic array of 'T's
// ----------------------------------------------------------------------------

#define _WX_DEFINE_BASEARRAY(T, name)                                       \
/* ctor */                                                                  \
name::name()                                                                \
{                                                                           \
  m_nSize  =                                                                \
  m_nCount = 0;                                                             \
  m_pItems = (T *)NULL;                                                     \
}                                                                           \
                                                                            \
/* copy ctor */                                                             \
name::name(const name& src)                                                 \
{                                                                           \
  m_nSize  = /* not src.m_nSize to save memory */                           \
  m_nCount = src.m_nCount;                                                  \
                                                                            \
  if ( m_nSize != 0 ) {                                                     \
      m_pItems = new T[m_nSize];                                            \
      /* only copy if allocation succeeded */                               \
      if ( m_pItems ) {                                                     \
          memcpy(m_pItems, src.m_pItems, m_nCount*sizeof(T));               \
      }                                                                     \
      else {                                                                \
          m_nSize = 0;                                                      \
      }                                                                     \
  }                                                                         \
  else                                                                      \
    m_pItems = (T *) NULL;                                                  \
}                                                                           \
                                                                            \
/* assignment operator */                                                   \
name& name::operator=(const name& src)                                      \
{                                                                           \
  wxDELETEA(m_pItems);                                                      \
                                                                            \
  m_nSize  = /* not src.m_nSize to save memory */                           \
  m_nCount = src.m_nCount;                                                  \
                                                                            \
  if ( m_nSize != 0 ){                                                      \
      m_pItems = new T[m_nSize];                                            \
      /* only copy if allocation succeeded */                               \
      if ( m_pItems ) {                                                     \
          memcpy(m_pItems, src.m_pItems, m_nCount*sizeof(T));               \
      }                                                                     \
      else {                                                                \
          m_nSize = 0;                                                      \
      }                                                                     \
  }                                                                         \
  else                                                                      \
    m_pItems = (T *) NULL;                                                  \
                                                                            \
  return *this;                                                             \
}                                                                           \
                                                                            \
/* grow the array */                                                        \
void name::Grow(size_t nIncrement)                                          \
{                                                                           \
  /* only do it if no more place */                                         \
  if( (m_nCount == m_nSize) || ((m_nSize - m_nCount) < nIncrement) ) {      \
    if( m_nSize == 0 ) {                                                    \
      /* was empty, determine initial size */                               \
      size_t size = WX_ARRAY_DEFAULT_INITIAL_SIZE;                          \
      if (size < nIncrement) size = nIncrement;                             \
      /* allocate some memory */                                            \
      m_pItems = new T[size];                                               \
      /* only grow if allocation succeeded */                               \
      if ( m_pItems ) {                                                     \
          m_nSize = size;                                                   \
      }                                                                     \
    }                                                                       \
    else                                                                    \
    {                                                                       \
      /* add at least 50% but not too much */                               \
      size_t ndefIncrement = m_nSize < WX_ARRAY_DEFAULT_INITIAL_SIZE        \
                            ? WX_ARRAY_DEFAULT_INITIAL_SIZE : m_nSize >> 1; \
      if ( ndefIncrement > ARRAY_MAXSIZE_INCREMENT )                        \
        ndefIncrement = ARRAY_MAXSIZE_INCREMENT;                            \
      if ( nIncrement < ndefIncrement )                                     \
        nIncrement = ndefIncrement;                                         \
      T *pNew = new T[m_nSize + nIncrement];                                \
      /* only grow if allocation succeeded */                               \
      if ( pNew ) {                                                         \
          m_nSize += nIncrement;                                            \
          /* copy data to new location */                                   \
          memcpy(pNew, m_pItems, m_nCount*sizeof(T));                       \
          delete [] m_pItems;                                               \
          m_pItems = pNew;                                                  \
      }                                                                     \
    }                                                                       \
  }                                                                         \
}                                                                           \
                                                                            \
/* dtor */                                                                  \
name::~name()                                                               \
{                                                                           \
  wxDELETEA(m_pItems);                                                      \
}                                                                           \
                                                                            \
/* clears the list */                                                       \
void name::Clear()                                                          \
{                                                                           \
  m_nSize  =                                                                \
  m_nCount = 0;                                                             \
                                                                            \
  wxDELETEA(m_pItems);                                                      \
}                                                                           \
                                                                            \
/* pre-allocates memory (frees the previous data!) */                       \
void name::Alloc(size_t nSize)                                              \
{                                                                           \
  /* only if old buffer was not big enough */                               \
  if ( nSize > m_nSize ) {                                                  \
    wxDELETEA(m_pItems);                                                    \
    m_nSize  = 0;                                                           \
    m_pItems = new T[nSize];                                                \
    /* only alloc if allocation succeeded */                                \
    if ( m_pItems ) {                                                       \
        m_nSize  = nSize;                                                   \
    }                                                                       \
  }                                                                         \
                                                                            \
  m_nCount = 0;                                                             \
}                                                                           \
                                                                            \
/* minimizes the memory usage by freeing unused memory */                   \
void name::Shrink()                                                         \
{                                                                           \
  /* only do it if we have some memory to free */                           \
  if( m_nCount < m_nSize ) {                                                \
    /* allocates exactly as much memory as we need */                       \
    T *pNew = new T[m_nCount];                                              \
    /* only shrink if allocation succeeded */                               \
    if ( pNew ) {                                                           \
        /* copy data to new location */                                     \
        memcpy(pNew, m_pItems, m_nCount*sizeof(T));                         \
        delete [] m_pItems;                                                 \
        m_pItems = pNew;                                                    \
        m_nSize = m_nCount;                                                 \
    }                                                                       \
  }                                                                         \
}                                                                           \
                                                                            \
/* searches the array for an item (forward or backwards) */                 \
int name::Index(T lItem, bool bFromEnd) const                               \
{                                                                           \
  if ( bFromEnd ) {                                                         \
    if ( m_nCount > 0 ) {                                                   \
      size_t n = m_nCount;                                                  \
      do {                                                                  \
        if ( m_pItems[--n] == lItem )                                       \
          return n;                                                         \
      }                                                                     \
      while ( n != 0 );                                                     \
    }                                                                       \
  }                                                                         \
  else {                                                                    \
    for( size_t n = 0; n < m_nCount; n++ ) {                                \
      if( m_pItems[n] == lItem )                                            \
        return n;                                                           \
    }                                                                       \
  }                                                                         \
                                                                            \
  return wxNOT_FOUND;                                                       \
}                                                                           \
                                                                            \
/* search for a place to insert item into sorted array (binary search) */   \
size_t name::IndexForInsert(T lItem, CMPFUNC fnCompare) const               \
{                                                                           \
  size_t i,                                                                 \
       lo = 0,                                                              \
       hi = m_nCount;                                                       \
  int res;                                                                  \
                                                                            \
  while ( lo < hi ) {                                                       \
    i = (lo + hi)/2;                                                        \
                                                                            \
    res = (*fnCompare)((const void *)(long)lItem,                           \
                       (const void *)(long)(m_pItems[i]));                  \
    if ( res < 0 )                                                          \
      hi = i;                                                               \
    else if ( res > 0 )                                                     \
      lo = i + 1;                                                           \
    else {                                                                  \
      lo = i;                                                               \
      break;                                                                \
    }                                                                       \
  }                                                                         \
                                                                            \
  return lo;                                                                \
}                                                                           \
                                                                            \
/* search for an item in a sorted array (binary search) */                  \
int name::Index(T lItem, CMPFUNC fnCompare) const                           \
{                                                                           \
    size_t n = IndexForInsert(lItem, fnCompare);                            \
                                                                            \
    return n < m_nCount &&                                                  \
           (*fnCompare)((const void *)(long)lItem,                          \
                        ((const void *)(long)m_pItems[n])) ? wxNOT_FOUND    \
                                                           : (int)n;        \
}                                                                           \
                                                                            \
/* add item at the end */                                                   \
void name::Add(T lItem, size_t nInsert)                                     \
{                                                                           \
  if (nInsert == 0)                                                         \
      return;                                                               \
  Grow(nInsert);                                                            \
  for (size_t i = 0; i < nInsert; i++)                                      \
      m_pItems[m_nCount++] = lItem;                                         \
}                                                                           \
                                                                            \
/* add item assuming the array is sorted with fnCompare function */         \
void name::Add(T lItem, CMPFUNC fnCompare)                                  \
{                                                                           \
  Insert(lItem, IndexForInsert(lItem, fnCompare));                          \
}                                                                           \
                                                                            \
/* add item at the given position */                                        \
void name::Insert(T lItem, size_t nIndex, size_t nInsert)                   \
{                                                                           \
  wxCHECK_RET( nIndex <= m_nCount, wxT("bad index in wxArray::Insert") );   \
  wxCHECK_RET( m_nCount <= m_nCount + nInsert,                              \
               wxT("array size overflow in wxArray::Insert") );             \
                                                                            \
  if (nInsert == 0)                                                         \
      return;                                                               \
  Grow(nInsert);                                                            \
                                                                            \
  memmove(&m_pItems[nIndex + nInsert], &m_pItems[nIndex],                   \
          (m_nCount - nIndex)*sizeof(T));                                   \
  for (size_t i = 0; i < nInsert; i++)                                      \
      m_pItems[nIndex + i] = lItem;                                         \
  m_nCount += nInsert;                                                      \
}                                                                           \
                                                                            \
/* removes item from array (by index) */                                    \
void name::RemoveAt(size_t nIndex, size_t nRemove)                          \
{                                                                           \
  wxCHECK_RET( nIndex < m_nCount, wxT("bad index in wxArray::RemoveAt") );  \
  wxCHECK_RET( nIndex + nRemove <= m_nCount,                                \
               wxT("removing too many elements in wxArray::RemoveAt") );    \
                                                                            \
  memmove(&m_pItems[nIndex], &m_pItems[nIndex + nRemove],                   \
          (m_nCount - nIndex - nRemove)*sizeof(T));                         \
  m_nCount -= nRemove;                                                      \
}                                                                           \
                                                                            \
/* removes item from array (by value) */                                    \
void name::Remove(T lItem)                                                  \
{                                                                           \
  int iIndex = Index(lItem);                                                \
                                                                            \
  wxCHECK_RET( iIndex != wxNOT_FOUND,                                       \
               wxT("removing inexistent item in wxArray::Remove") );        \
                                                                            \
  RemoveAt((size_t)iIndex);                                                 \
}                                                                           \
                                                                            \
/* sort array elements using passed comparaison function */                 \
void name::Sort(CMPFUNC fCmp)                                               \
{                                                                           \
  qsort(m_pItems, m_nCount, sizeof(T), fCmp);                               \
}

_WX_DEFINE_BASEARRAY(const void *, wxBaseArrayPtrVoid)
_WX_DEFINE_BASEARRAY(short,        wxBaseArrayShort)
_WX_DEFINE_BASEARRAY(int,          wxBaseArrayInt)
_WX_DEFINE_BASEARRAY(long,         wxBaseArrayLong)
//_WX_DEFINE_BASEARRAY(double,       wxBaseArrayDouble)

