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

#include  <wx/wxprec.h>

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#include "wx/dynarray.h"

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
  m_uiSize  =
  m_uiCount = 0;
  m_pItems  = NULL;
}

// copy ctor
wxBaseArray::wxBaseArray(const wxBaseArray& src)
{
  m_uiSize  = // not src.m_uiSize to save memory
  m_uiCount = src.m_uiCount;

  if ( m_uiSize != 0 ) {
    m_pItems = new long[m_uiSize];
    memcpy(m_pItems, src.m_pItems, m_uiCount*sizeof(long));
  }
  else
    m_pItems = NULL;
}

// assignment operator
wxBaseArray& wxBaseArray::operator=(const wxBaseArray& src)
{
  DELETEA(m_pItems);

  m_uiSize  = // not src.m_uiSize to save memory
  m_uiCount = src.m_uiCount;

  if ( m_uiSize != 0 ) {
    m_pItems = new long[m_uiSize];
    memcpy(m_pItems, src.m_pItems, m_uiCount*sizeof(long));
  }
  else
    m_pItems = NULL;

  return *this;
}

// grow the array
void wxBaseArray::Grow()
{
  // only do it if no more place
  if( m_uiCount == m_uiSize ) {
    if( m_uiSize == 0 ) {
      // was empty, alloc some memory
      m_uiSize = WX_ARRAY_DEFAULT_INITIAL_SIZE;
      m_pItems = new long[m_uiSize];
    }
    else
    {
      // add 50% but not too much
      uint uiIncrement = m_uiSize >> 1;
      if ( uiIncrement > ARRAY_MAXSIZE_INCREMENT )
        uiIncrement = ARRAY_MAXSIZE_INCREMENT;
      m_uiSize += uiIncrement;
      long *pNew = new long[m_uiSize];

      // copy data to new location
      memcpy(pNew, m_pItems, m_uiCount*sizeof(long));
      delete [] m_pItems;
      m_pItems = pNew;
    }
  }
}

// dtor
wxBaseArray::~wxBaseArray()
{
  DELETEA(m_pItems);
}

// clears the list
void wxBaseArray::Clear()
{
  m_uiSize  =
  m_uiCount = 0;

  DELETEA(m_pItems);
  m_pItems = NULL;
}

// pre-allocates memory (frees the previous data!)
void wxBaseArray::Alloc(uint uiSize)
{
  wxASSERT( uiSize > 0 );

  // only if old buffer was not big enough
  if ( uiSize > m_uiSize ) {
    DELETEA(m_pItems);
    m_pItems = new long[uiSize];
    m_uiSize  = uiSize;
  }

  m_uiCount = 0;
}

// searches the array for an item (forward or backwards)
int wxBaseArray::Index(long lItem, bool bFromEnd) const
{
  if ( bFromEnd ) {
    if ( m_uiCount > 0 ) {
      uint ui = m_uiCount;
      do {
        if ( m_pItems[--ui] == lItem )
          return ui;
      }
      while ( ui != 0 );
    }
  }
  else {
    for( uint ui = 0; ui < m_uiCount; ui++ ) {
      if( m_pItems[ui] == lItem )
        return ui;
    }
  }

  return NOT_FOUND;
}

// search for an item in a sorted array (binary search)
int wxBaseArray::Index(long lItem, CMPFUNC fnCompare)
{
  uint i,
       lo = 0,
       hi = m_uiCount;
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

  return NOT_FOUND;
}
// add item at the end
void wxBaseArray::Add(long lItem)
{
  Grow();
  m_pItems[m_uiCount++] = lItem;
}

// add item assuming the array is sorted with fnCompare function
void wxBaseArray::Add(long lItem, CMPFUNC fnCompare)
{
  uint i,
       lo = 0,
       hi = m_uiCount;
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
void wxBaseArray::Insert(long lItem, uint uiIndex)
{
  wxCHECK_RET( uiIndex <= m_uiCount, "bad index in wxArray::Insert" );

  Grow();

  memmove(&m_pItems[uiIndex + 1], &m_pItems[uiIndex],
          (m_uiCount - uiIndex)*sizeof(long));
  m_pItems[uiIndex] = lItem;
  m_uiCount++;
}

// removes item from array (by index)
void wxBaseArray::Remove(uint uiIndex)
{
  wxCHECK_RET( uiIndex <= m_uiCount, "bad index in wxArray::Remove" );

  memmove(&m_pItems[uiIndex], &m_pItems[uiIndex + 1],
          (m_uiCount - uiIndex - 1)*sizeof(long));
  m_uiCount--;
}

// removes item from array (by value)
void wxBaseArray::Remove(long lItem)
{
  int iIndex = Index(lItem);

  wxCHECK_RET( iIndex != NOT_FOUND,
               "removing inexistent item in wxArray::Remove" );

  Remove((uint)iIndex);
}

// sort array elements using passed comparaison function
void wxBaseArray::Sort(CMPFUNC fCmp)
{
  qsort(m_pItems, m_uiCount, sizeof(long), fCmp);
}
