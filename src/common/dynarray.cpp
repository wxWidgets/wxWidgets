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

// ============================================================================
// constants
// ============================================================================

// size increment = max(50% of current size, ARRAY_MAXSIZE_INCREMENT)
#define   ARRAY_MAXSIZE_INCREMENT    4096

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxBaseArray - dynamice array of 'long's
// ----------------------------------------------------------------------------

// ctor
wxBaseArray::wxBaseArray()
{
  m_nSize  =
  m_nCount = 0;
  m_pItems  = (long *) NULL;
}

// copy ctor
wxBaseArray::wxBaseArray(const wxBaseArray& src)
{
  m_nSize  = // not src.m_nSize to save memory
  m_nCount = src.m_nCount;

  if ( m_nSize != 0 ) {
    m_pItems = new long[m_nSize];
    memcpy(m_pItems, src.m_pItems, m_nCount*sizeof(long));
  }
  else
    m_pItems = (long *) NULL;
}

// assignment operator
wxBaseArray& wxBaseArray::operator=(const wxBaseArray& src)
{
#if 0
  wxDELETEA(m_pItems);
#else
	if ( (m_pItems)) { 
		delete (m_pItems); 
		(m_pItems) = 0; 
	}
#endif

  m_nSize  = // not src.m_nSize to save memory
  m_nCount = src.m_nCount;

  if ( m_nSize != 0 ) {
    m_pItems = new long[m_nSize];
    memcpy(m_pItems, src.m_pItems, m_nCount*sizeof(long));
  }
  else
    m_pItems = (long *) NULL;

  return *this;
}

// grow the array
void wxBaseArray::Grow()
{
  // only do it if no more place
  if( m_nCount == m_nSize ) {
    if( m_nSize == 0 ) {
      // was empty, alloc some memory
      m_nSize = WX_ARRAY_DEFAULT_INITIAL_SIZE;
      m_pItems = new long[m_nSize];
    }
    else
    {
      // add 50% but not too much
      size_t nIncrement = m_nSize < WX_ARRAY_DEFAULT_INITIAL_SIZE 
                         ? WX_ARRAY_DEFAULT_INITIAL_SIZE : m_nSize >> 1;
      if ( nIncrement > ARRAY_MAXSIZE_INCREMENT )
        nIncrement = ARRAY_MAXSIZE_INCREMENT;
      m_nSize += nIncrement;
      long *pNew = new long[m_nSize];

      // copy data to new location
      memcpy(pNew, m_pItems, m_nCount*sizeof(long));
      delete [] m_pItems;
      m_pItems = pNew;
    }
  }
}

// dtor
wxBaseArray::~wxBaseArray()
{
  wxDELETEA(m_pItems);
}

// clears the list
void wxBaseArray::Clear()
{
  m_nSize  =
  m_nCount = 0;

  wxDELETEA(m_pItems);
}

// pre-allocates memory (frees the previous data!)
void wxBaseArray::Alloc(size_t nSize)
{
  wxASSERT( nSize > 0 );

  // only if old buffer was not big enough
  if ( nSize > m_nSize ) {
    wxDELETEA(m_pItems);
    m_pItems = new long[nSize];
    m_nSize  = nSize;
  }

  m_nCount = 0;
}

// minimizes the memory usage by freeing unused memory
void wxBaseArray::Shrink()
{
  // only do it if we have some memory to free
  if( m_nCount < m_nSize ) {
    // allocates exactly as much memory as we need
    long *pNew = new long[m_nCount];

    // copy data to new location
    memcpy(pNew, m_pItems, m_nCount*sizeof(long));
    delete [] m_pItems;
    m_pItems = pNew;
  }
}

// searches the array for an item (forward or backwards)
int wxBaseArray::Index(long lItem, bool bFromEnd) const
{
  if ( bFromEnd ) {
    if ( m_nCount > 0 ) {
      size_t n = m_nCount;
      do {
        if ( m_pItems[--n] == lItem )
          return n;
      }
      while ( n != 0 );
    }
  }
  else {
    for( size_t n = 0; n < m_nCount; n++ ) {
      if( m_pItems[n] == lItem )
        return n;
    }
  }

  return wxNOT_FOUND;
}

// search for an item in a sorted array (binary search)
int wxBaseArray::Index(long lItem, CMPFUNC fnCompare) const
{
  size_t i,
       lo = 0,
       hi = m_nCount;
  int res;

  while ( lo < hi ) {
    i = (lo + hi)/2;

    res = (*fnCompare)((const void *)lItem, (const void *)m_pItems[i]);
    if ( res < 0 )
      hi = i;
    else if ( res > 0 )
      lo = i + 1;
    else
      return i;
  }

  return wxNOT_FOUND;
}
// add item at the end
void wxBaseArray::Add(long lItem)
{
  Grow();
  m_pItems[m_nCount++] = lItem;
}

// add item assuming the array is sorted with fnCompare function
void wxBaseArray::Add(long lItem, CMPFUNC fnCompare)
{
  size_t i,
       lo = 0,
       hi = m_nCount;
  int res;

  while ( lo < hi ) {
    i = (lo + hi)/2;

    res = (*fnCompare)((const void *)lItem, (const void *)m_pItems[i]);
    if ( res < 0 )
      hi = i;
    else if ( res > 0 )
      lo = i + 1;
    else {
      lo = hi = i;
      break;
    }
  }

  wxASSERT( lo == hi ); // I hope I got binary search right :-)

  Insert(lItem, lo);
}

// add item at the given position
void wxBaseArray::Insert(long lItem, size_t nIndex)
{
  wxCHECK_RET( nIndex <= m_nCount, wxT("bad index in wxArray::Insert") );

  Grow();

  memmove(&m_pItems[nIndex + 1], &m_pItems[nIndex],
          (m_nCount - nIndex)*sizeof(long));
  m_pItems[nIndex] = lItem;
  m_nCount++;
}

// removes item from array (by index)
void wxBaseArray::RemoveAt(size_t nIndex)
{
  wxCHECK_RET( nIndex <= m_nCount, wxT("bad index in wxArray::RemoveAt") );

  memmove(&m_pItems[nIndex], &m_pItems[nIndex + 1],
          (m_nCount - nIndex - 1)*sizeof(long));
  m_nCount--;
}

// removes item from array (by value)
void wxBaseArray::Remove(long lItem)
{
  int iIndex = Index(lItem);

  wxCHECK_RET( iIndex != wxNOT_FOUND,
               wxT("removing inexistent item in wxArray::Remove") );

  Remove((size_t)iIndex);
}

// sort array elements using passed comparaison function
void wxBaseArray::Sort(CMPFUNC fCmp)
{
  qsort(m_pItems, m_nCount, sizeof(long), fCmp);
}
