/////////////////////////////////////////////////////////////////////////////
// Name:        string.cpp
// Purpose:     wxString class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
  #pragma implementation "string.h"
#endif

/*
 * About ref counting:
 *  1) all empty strings use g_strEmpty, nRefs = -1 (set in Init())
 *  2) AllocBuffer() sets nRefs to 1, Lock() increments it by one
 *  3) Unlock() decrements nRefs and frees memory if it goes to 0
 */

// ===========================================================================
// headers, declarations, constants
// ===========================================================================

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/defs.h"
  #include "wx/string.h"
  #include "wx/intl.h"
  #include "wx/thread.h"
#endif

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#ifdef __SALFORDC__
  #include <clib.h>
#endif

// allocating extra space for each string consumes more memory but speeds up
// the concatenation operations (nLen is the current string's length)
// NB: EXTRA_ALLOC must be >= 0!
#define EXTRA_ALLOC       (19 - nLen % 16)

// ---------------------------------------------------------------------------
// static class variables definition
// ---------------------------------------------------------------------------

#if defined(__VISAGECPP__) && __IBMCPP__ >= 400
// must define this static for VA or else you get multiply defined symbols
// everywhere
const unsigned int wxSTRING_MAXLEN = UINT_MAX - 100;
#endif // Visual Age

#if !wxUSE_STL
  const size_t wxStringBase::npos = wxSTRING_MAXLEN;
#endif

// ----------------------------------------------------------------------------
// static data
// ----------------------------------------------------------------------------

#if wxUSE_STL

extern const wxChar WXDLLIMPEXP_BASE *wxEmptyString = _T("");

#else

// for an empty string, GetStringData() will return this address: this
// structure has the same layout as wxStringData and it's data() method will
// return the empty string (dummy pointer)
static const struct
{
  wxStringData data;
  wxChar dummy;
} g_strEmpty = { {-1, 0, 0}, wxT('\0') };

// empty C style string: points to 'string data' byte of g_strEmpty
extern const wxChar WXDLLIMPEXP_BASE *wxEmptyString = &g_strEmpty.dummy;

#endif

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM

// MS Visual C++ version 5.0 provides the new STL headers as well as the old
// iostream ones.
//
// ATTN: you can _not_ use both of these in the same program!

#include <iostream>

wxSTD istream& operator>>(wxSTD istream& is, wxString& WXUNUSED(str))
{
#if 0
  int w = is.width(0);
  if ( is.ipfx(0) ) {
    streambuf *sb = is.rdbuf();
    str.erase();
    while ( true ) {
      int ch = sb->sbumpc ();
      if ( ch == EOF ) {
        is.setstate(ios::eofbit);
        break;
      }
      else if ( isspace(ch) ) {
        sb->sungetc();
        break;
      }

      str += ch;
      if ( --w == 1 )
        break;
    }
  }

  is.isfx();
  if ( str.length() == 0 )
    is.setstate(ios::failbit);
#endif
  return is;
}

wxSTD ostream& operator<<(wxSTD ostream& os, const wxString& str)
{
  os << str.c_str();
  return os;
}

#endif // wxUSE_STD_IOSTREAM

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// this small class is used to gather statistics for performance tuning
//#define WXSTRING_STATISTICS
#ifdef  WXSTRING_STATISTICS
  class Averager
  {
  public:
    Averager(const wxChar *sz) { m_sz = sz; m_nTotal = m_nCount = 0; }
   ~Averager()
   { wxPrintf("wxString: average %s = %f\n", m_sz, ((float)m_nTotal)/m_nCount); }

    void Add(size_t n) { m_nTotal += n; m_nCount++; }

  private:
    size_t m_nCount, m_nTotal;
    const wxChar *m_sz;
  } g_averageLength("allocation size"),
    g_averageSummandLength("summand length"),
    g_averageConcatHit("hit probability in concat"),
    g_averageInitialLength("initial string length");

  #define STATISTICS_ADD(av, val) g_average##av.Add(val)
#else
  #define STATISTICS_ADD(av, val)
#endif // WXSTRING_STATISTICS

#if !wxUSE_STL

// ===========================================================================
// wxStringData class deallocation
// ===========================================================================

#if defined(__VISUALC__) && defined(_MT) && !defined(_DLL)
#  pragma message (__FILE__ ": building with Multithreaded non DLL runtime has a performance impact on wxString!")
void wxStringData::Free()
{
    free(this);
}
#endif

// ===========================================================================
// wxStringBase
// ===========================================================================

// takes nLength elements of psz starting at nPos
void wxStringBase::InitWith(const wxChar *psz, size_t nPos, size_t nLength)
{
  Init();

  // if the length is not given, assume the string to be NUL terminated
  if ( nLength == npos ) {
    wxASSERT_MSG( nPos <= wxStrlen(psz), _T("index out of bounds") );

    nLength = wxStrlen(psz + nPos);
  }

  STATISTICS_ADD(InitialLength, nLength);

  if ( nLength > 0 ) {
    // trailing '\0' is written in AllocBuffer()
    if ( !AllocBuffer(nLength) ) {
      wxFAIL_MSG( _T("out of memory in wxStringBase::InitWith") );
      return;
    }
    wxMemcpy(m_pchData, psz + nPos, nLength);
  }
}

// poor man's iterators are "void *" pointers
wxStringBase::wxStringBase(const void *pStart, const void *pEnd)
{
  InitWith((const wxChar *)pStart, 0,
           (const wxChar *)pEnd - (const wxChar *)pStart);
}

wxStringBase::wxStringBase(size_type n, wxChar ch)
{
  Init();
  append(n, ch);
}

// ---------------------------------------------------------------------------
// memory allocation
// ---------------------------------------------------------------------------

// allocates memory needed to store a C string of length nLen
bool wxStringBase::AllocBuffer(size_t nLen)
{
  // allocating 0 sized buffer doesn't make sense, all empty strings should
  // reuse g_strEmpty
  wxASSERT( nLen >  0 );

  // make sure that we don't overflow
  wxASSERT( nLen < (INT_MAX / sizeof(wxChar)) -
                   (sizeof(wxStringData) + EXTRA_ALLOC + 1) );

  STATISTICS_ADD(Length, nLen);

  // allocate memory:
  // 1) one extra character for '\0' termination
  // 2) sizeof(wxStringData) for housekeeping info
  wxStringData* pData = (wxStringData*)
    malloc(sizeof(wxStringData) + (nLen + EXTRA_ALLOC + 1)*sizeof(wxChar));

  if ( pData == NULL ) {
    // allocation failures are handled by the caller
    return false;
  }

  pData->nRefs        = 1;
  pData->nDataLength  = nLen;
  pData->nAllocLength = nLen + EXTRA_ALLOC;
  m_pchData           = pData->data();  // data starts after wxStringData
  m_pchData[nLen]     = wxT('\0');
  return true;
}

// must be called before changing this string
bool wxStringBase::CopyBeforeWrite()
{
  wxStringData* pData = GetStringData();

  if ( pData->IsShared() ) {
    pData->Unlock();                // memory not freed because shared
    size_t nLen = pData->nDataLength;
    if ( !AllocBuffer(nLen) ) {
      // allocation failures are handled by the caller
      return false;
    }
    wxMemcpy(m_pchData, pData->data(), nLen);
  }

  wxASSERT( !GetStringData()->IsShared() );  // we must be the only owner

  return true;
}

// must be called before replacing contents of this string
bool wxStringBase::AllocBeforeWrite(size_t nLen)
{
  wxASSERT( nLen != 0 );  // doesn't make any sense

  // must not share string and must have enough space
  wxStringData* pData = GetStringData();
  if ( pData->IsShared() || pData->IsEmpty() ) {
    // can't work with old buffer, get new one
    pData->Unlock();
    if ( !AllocBuffer(nLen) ) {
      // allocation failures are handled by the caller
      return false;
    }
  }
  else {
    if ( nLen > pData->nAllocLength ) {
      // realloc the buffer instead of calling malloc() again, this is more
      // efficient
      STATISTICS_ADD(Length, nLen);

      nLen += EXTRA_ALLOC;

      pData = (wxStringData*)
          realloc(pData, sizeof(wxStringData) + (nLen + 1)*sizeof(wxChar));

      if ( pData == NULL ) {
        // allocation failures are handled by the caller
        // keep previous data since reallocation failed
        return false;
      }

      pData->nAllocLength = nLen;
      m_pchData = pData->data();
    }

    // now we have enough space, just update the string length
    pData->nDataLength = nLen;
  }

  wxASSERT( !GetStringData()->IsShared() );  // we must be the only owner

  return true;
}

wxStringBase& wxStringBase::append(size_t n, wxChar ch)
{
    size_type len = length();

    if ( !CopyBeforeWrite() || !Alloc(len + n) ) {
      wxFAIL_MSG( _T("out of memory in wxStringBase::append") );
    }
    GetStringData()->nDataLength = len + n;
    m_pchData[len + n] = '\0';
    for ( size_t i = 0; i < n; ++i )
        m_pchData[len + i] = ch;
    return *this;
}

void wxStringBase::resize(size_t nSize, wxChar ch)
{
    size_t len = length();

    if ( nSize < len )
    {
        erase(begin() + nSize, end());
    }
    else if ( nSize > len )
    {
        append(nSize - len, ch);
    }
    //else: we have exactly the specified length, nothing to do
}

// allocate enough memory for nLen characters
bool wxStringBase::Alloc(size_t nLen)
{
  wxStringData *pData = GetStringData();
  if ( pData->nAllocLength <= nLen ) {
    if ( pData->IsEmpty() ) {
      nLen += EXTRA_ALLOC;

      wxStringData* pData = (wxStringData*)
          malloc(sizeof(wxStringData) + (nLen + 1)*sizeof(wxChar));

      if ( pData == NULL ) {
        // allocation failure handled by caller
        return false;
      }

      pData->nRefs = 1;
      pData->nDataLength = 0;
      pData->nAllocLength = nLen;
      m_pchData = pData->data();  // data starts after wxStringData
      m_pchData[0u] = wxT('\0');
    }
    else if ( pData->IsShared() ) {
      pData->Unlock();                // memory not freed because shared
      size_t nOldLen = pData->nDataLength;
      if ( !AllocBuffer(nLen) ) {
        // allocation failure handled by caller
        return false;
      }
      memcpy(m_pchData, pData->data(), nOldLen*sizeof(wxChar));
    }
    else {
      nLen += EXTRA_ALLOC;

      pData = (wxStringData *)
        realloc(pData, sizeof(wxStringData) + (nLen + 1)*sizeof(wxChar));

      if ( pData == NULL ) {
        // allocation failure handled by caller
        // keep previous data since reallocation failed
        return false;
      }

      // it's not important if the pointer changed or not (the check for this
      // is not faster than assigning to m_pchData in all cases)
      pData->nAllocLength = nLen;
      m_pchData = pData->data();
    }
  }
  //else: we've already got enough
  return true;
}

wxStringBase::iterator wxStringBase::begin()
{
    if (length() > 0)
        CopyBeforeWrite();
    return m_pchData;
}

wxStringBase::iterator wxStringBase::end()
{
    if (length() > 0)
        CopyBeforeWrite();
    return m_pchData + length();
}

wxStringBase::iterator wxStringBase::erase(iterator it)
{
    size_type idx = it - begin();
    erase(idx, 1);
    return begin() + idx;
}

wxStringBase& wxStringBase::erase(size_t nStart, size_t nLen)
{
  wxASSERT(nStart <= length());
  size_t strLen = length() - nStart;
  // delete nLen or up to the end of the string characters
  nLen = strLen < nLen ? strLen : nLen;
  wxString strTmp(c_str(), nStart);
  strTmp.append(c_str() + nStart + nLen, length() - nStart - nLen);

  swap(strTmp);
  return *this;
}

wxStringBase& wxStringBase::insert(size_t nPos, const wxChar *sz, size_t n)
{
  wxASSERT( nPos <= length() );

  if ( n == npos ) n = wxStrlen(sz);
  if ( n == 0 ) return *this;

  if ( !CopyBeforeWrite() || !Alloc(length() + n) ) {
    wxFAIL_MSG( _T("out of memory in wxStringBase::insert") );
  }

  memmove(m_pchData + nPos + n, m_pchData + nPos,
          (length() - nPos) * sizeof(wxChar));
  memcpy(m_pchData + nPos, sz, n * sizeof(wxChar));
  GetStringData()->nDataLength = length() + n;
  m_pchData[length()] = '\0';

  return *this;
}

void wxStringBase::swap(wxStringBase& str)
{
    wxChar* tmp = str.m_pchData;
    str.m_pchData = m_pchData;
    m_pchData = tmp;
}

size_t wxStringBase::find(const wxStringBase& str, size_t nStart) const
{
  wxASSERT( str.GetStringData()->IsValid() );
  wxASSERT( nStart <= length() );

  //anchor
  const wxChar* p = (const wxChar*)wxMemchr(c_str() + nStart,
                                            str.c_str()[0],
                                            length() - nStart);

  if(!p)
      return npos;

  while(p - c_str() + str.length() <= length() &&
        wxMemcmp(p, str.c_str(), str.length()) )
  {
      //anchor again
      p = (const wxChar*)wxMemchr(++p,
                                  str.c_str()[0],
                                  length() - (p - c_str()));

      if(!p)
          return npos;
  }

   return (p - c_str() + str.length() <= length()) ? p - c_str() : npos;
}

size_t wxStringBase::find(const wxChar* sz, size_t nStart, size_t n) const
{
  return find(wxStringBase(sz, n), nStart);
}

size_t wxStringBase::find(wxChar ch, size_t nStart) const
{
  wxASSERT( nStart <= length() );

  const wxChar *p = (const wxChar*)wxMemchr(c_str() + nStart, ch, length() - nStart);

  return p == NULL ? npos : p - c_str();
}

size_t wxStringBase::rfind(const wxStringBase& str, size_t nStart) const
{
    wxASSERT( str.GetStringData()->IsValid() );
    wxASSERT( nStart == npos || nStart <= length() );

    if ( length() >= str.length() )
    {
        // avoids a corner case later
        if ( length() == 0 && str.length() == 0 )
            return 0;

        // "top" is the point where search starts from
        size_t top = length() - str.length();

        if ( nStart == npos )
            nStart = length() - 1;
        if ( nStart < top )
            top = nStart;

        const wxChar *cursor = c_str() + top;
        do
        {
            if ( wxMemcmp(cursor, str.c_str(),
                        str.length()) == 0 )
            {
                return cursor - c_str();
            }
        } while ( cursor-- > c_str() );
    }

    return npos;
}

size_t wxStringBase::rfind(const wxChar* sz, size_t nStart, size_t n) const
{
    return rfind(wxStringBase(sz, n), nStart);
}

size_t wxStringBase::rfind(wxChar ch, size_t nStart) const
{
    if ( nStart == npos )
    {
        nStart = length();
    }
    else
    {
        wxASSERT( nStart <= length() );
    }

    const wxChar *actual;
    for ( actual = c_str() + ( nStart == npos ? length() : nStart + 1 );
          actual > c_str(); --actual )
    {
        if ( *(actual - 1) == ch )
            return (actual - 1) - c_str();
    }

    return npos;
}

size_t wxStringBase::find_first_of(const wxChar* sz, size_t nStart) const
{
    wxASSERT(nStart <= length());

    size_t len = wxStrlen(sz);

    size_t i;
    for(i = nStart; i < this->length(); ++i)
    {
        if (wxMemchr(sz, *(c_str() + i), len))
            break;
    }

    if(i == this->length())
        return npos;
    else
        return i;
}

size_t wxStringBase::find_first_of(const wxChar* sz, size_t nStart,
                                   size_t n) const
{
    return find_first_of(wxStringBase(sz, n), nStart);
}

size_t wxStringBase::find_last_of(const wxChar* sz, size_t nStart) const
{
    if ( nStart == npos )
    {
        nStart = length() - 1;
    }
    else
    {
        wxASSERT_MSG( nStart <= length(),
                        _T("invalid index in find_last_of()") );
    }

    size_t len = wxStrlen(sz);

    for ( const wxChar *p = c_str() + nStart; p >= c_str(); --p )
    {
        if ( wxMemchr(sz, *p, len) )
            return p - c_str();
    }

    return npos;
}

size_t wxStringBase::find_last_of(const wxChar* sz, size_t nStart,
                                   size_t n) const
{
    return find_last_of(wxStringBase(sz, n), nStart);
}

size_t wxStringBase::find_first_not_of(const wxChar* sz, size_t nStart) const
{
    if ( nStart == npos )
    {
        nStart = length();
    }
    else
    {
        wxASSERT( nStart <= length() );
    }

    size_t len = wxStrlen(sz);

    size_t i;
    for(i = nStart; i < this->length(); ++i)
    {
        if (!wxMemchr(sz, *(c_str() + i), len))
            break;
    }

    if(i == this->length())
         return npos;
     else
        return i;
}

size_t wxStringBase::find_first_not_of(const wxChar* sz, size_t nStart,
                                       size_t n) const
{
    return find_first_not_of(wxStringBase(sz, n), nStart);
}

size_t wxStringBase::find_first_not_of(wxChar ch, size_t nStart) const
{
    wxASSERT( nStart <= length() );

    for ( const wxChar *p = c_str() + nStart; *p; p++ )
    {
        if ( *p != ch )
            return p - c_str();
    }

    return npos;
}

size_t wxStringBase::find_last_not_of(const wxChar* sz, size_t nStart) const
{
    if ( nStart == npos )
    {
        nStart = length() - 1;
    }
    else
    {
        wxASSERT( nStart <= length() );
    }

    size_t len = wxStrlen(sz);

    for ( const wxChar *p = c_str() + nStart; p >= c_str(); --p )
    {
        if ( !wxMemchr(sz, *p,len) )
             return p - c_str();
    }

    return npos;
}

size_t wxStringBase::find_last_not_of(const wxChar* sz, size_t nStart,
                                      size_t n) const
{
    return find_last_not_of(wxStringBase(sz, n), nStart);
}

size_t wxStringBase::find_last_not_of(wxChar ch, size_t nStart) const
{
    if ( nStart == npos )
    {
        nStart = length() - 1;
    }
    else
    {
        wxASSERT( nStart <= length() );
    }

    for ( const wxChar *p = c_str() + nStart; p >= c_str(); --p )
    {
        if ( *p != ch )
            return p - c_str();
    }

    return npos;
}

wxStringBase& wxStringBase::replace(size_t nStart, size_t nLen,
                                    const wxChar *sz)
{
  wxASSERT_MSG( nStart <= length(),
                _T("index out of bounds in wxStringBase::replace") );
  size_t strLen = length() - nStart;
  nLen = strLen < nLen ? strLen : nLen;

  wxStringBase strTmp;
  strTmp.reserve(length()); // micro optimisation to avoid multiple mem allocs

  if ( nStart != 0 )
    strTmp.append(c_str(), nStart);
  strTmp.append(sz);
  strTmp.append(c_str() + nStart + nLen);

  swap(strTmp);
  return *this;
}

wxStringBase& wxStringBase::replace(size_t nStart, size_t nLen,
                                    size_t nCount, wxChar ch)
{
  return replace(nStart, nLen, wxStringBase(nCount, ch).c_str());
}

wxStringBase& wxStringBase::replace(size_t nStart, size_t nLen,
                                    const wxStringBase& str,
                                    size_t nStart2, size_t nLen2)
{
  return replace(nStart, nLen, str.substr(nStart2, nLen2));
}

wxStringBase& wxStringBase::replace(size_t nStart, size_t nLen,
                                    const wxChar* sz, size_t nCount)
{
  return replace(nStart, nLen, wxStringBase(sz, nCount).c_str());
}

wxStringBase wxStringBase::substr(size_t nStart, size_t nLen) const
{
  if ( nLen == npos )
    nLen = length() - nStart;
  return wxStringBase(*this, nStart, nLen);
}

// assigns one string to another
wxStringBase& wxStringBase::operator=(const wxStringBase& stringSrc)
{
  wxASSERT( stringSrc.GetStringData()->IsValid() );

  // don't copy string over itself
  if ( m_pchData != stringSrc.m_pchData ) {
    if ( stringSrc.GetStringData()->IsEmpty() ) {
      Reinit();
    }
    else {
      // adjust references
      GetStringData()->Unlock();
      m_pchData = stringSrc.m_pchData;
      GetStringData()->Lock();
    }
  }

  return *this;
}

// assigns a single character
wxStringBase& wxStringBase::operator=(wxChar ch)
{
  if ( !AssignCopy(1, &ch) ) {
    wxFAIL_MSG( _T("out of memory in wxStringBase::operator=(wxChar)") );
  }
  return *this;
}

// assigns C string
wxStringBase& wxStringBase::operator=(const wxChar *psz)
{
  if ( !AssignCopy(wxStrlen(psz), psz) ) {
    wxFAIL_MSG( _T("out of memory in wxStringBase::operator=(const wxChar *)") );
  }
  return *this;
}

// helper function: does real copy
bool wxStringBase::AssignCopy(size_t nSrcLen, const wxChar *pszSrcData)
{
  if ( nSrcLen == 0 ) {
    Reinit();
  }
  else {
    if ( !AllocBeforeWrite(nSrcLen) ) {
      // allocation failure handled by caller
      return false;
    }
    memcpy(m_pchData, pszSrcData, nSrcLen*sizeof(wxChar));
    GetStringData()->nDataLength = nSrcLen;
    m_pchData[nSrcLen] = wxT('\0');
  }
  return true;
}

// ---------------------------------------------------------------------------
// string concatenation
// ---------------------------------------------------------------------------

// add something to this string
bool wxStringBase::ConcatSelf(size_t nSrcLen, const wxChar *pszSrcData,
                              size_t nMaxLen)
{
  STATISTICS_ADD(SummandLength, nSrcLen);

  nSrcLen = nSrcLen < nMaxLen ? nSrcLen : nMaxLen;

  // concatenating an empty string is a NOP
  if ( nSrcLen > 0 ) {
    wxStringData *pData = GetStringData();
    size_t nLen = pData->nDataLength;
    size_t nNewLen = nLen + nSrcLen;

    // alloc new buffer if current is too small
    if ( pData->IsShared() ) {
      STATISTICS_ADD(ConcatHit, 0);

      // we have to allocate another buffer
      wxStringData* pOldData = GetStringData();
      if ( !AllocBuffer(nNewLen) ) {
          // allocation failure handled by caller
          return false;
      }
      memcpy(m_pchData, pOldData->data(), nLen*sizeof(wxChar));
      pOldData->Unlock();
    }
    else if ( nNewLen > pData->nAllocLength ) {
      STATISTICS_ADD(ConcatHit, 0);

      reserve(nNewLen);
      // we have to grow the buffer
      if ( capacity() < nNewLen ) {
          // allocation failure handled by caller
          return false;
      }
    }
    else {
      STATISTICS_ADD(ConcatHit, 1);

      // the buffer is already big enough
    }

    // should be enough space
    wxASSERT( nNewLen <= GetStringData()->nAllocLength );

    // fast concatenation - all is done in our buffer
    memcpy(m_pchData + nLen, pszSrcData, nSrcLen*sizeof(wxChar));

    m_pchData[nNewLen] = wxT('\0');          // put terminating '\0'
    GetStringData()->nDataLength = nNewLen; // and fix the length
  }
  //else: the string to append was empty
  return true;
}

// ---------------------------------------------------------------------------
// simple sub-string extraction
// ---------------------------------------------------------------------------

// helper function: clone the data attached to this string
bool wxStringBase::AllocCopy(wxString& dest, int nCopyLen, int nCopyIndex) const
{
  if ( nCopyLen == 0 ) {
    dest.Init();
  }
  else {
    if ( !dest.AllocBuffer(nCopyLen) ) {
      // allocation failure handled by caller
      return false;
    }
    memcpy(dest.m_pchData, m_pchData + nCopyIndex, nCopyLen*sizeof(wxChar));
  }
  return true;
}

#endif // !wxUSE_STL

#if !wxUSE_STL || !defined(HAVE_STD_STRING_COMPARE)

#if !wxUSE_STL
    #define STRINGCLASS wxStringBase
#else
    #define STRINGCLASS wxString
#endif

static inline int wxDoCmp(const wxChar* s1, size_t l1,
                          const wxChar* s2, size_t l2)
{
    if( l1 == l2 )
        return wxMemcmp(s1, s2, l1);
    else if( l1 < l2 )
    {
        int ret = wxMemcmp(s1, s2, l1);
        return ret == 0 ? -1 : ret;
    }
    else if( l1 > l2 )
    {
        int ret = wxMemcmp(s1, s2, l2);
        return ret == 0 ? +1 : ret;
    }

    wxFAIL;   // must never get there
    return 0; // quiet compilers
}

int STRINGCLASS::compare(const wxStringBase& str) const
{
    return ::wxDoCmp(data(), length(), str.data(), str.length());
}

int STRINGCLASS::compare(size_t nStart, size_t nLen,
                         const wxStringBase& str) const
{
    wxASSERT(nStart <= length());
    size_type strLen = length() - nStart;
    nLen = strLen < nLen ? strLen : nLen;
    return ::wxDoCmp(data() + nStart, nLen, str.data(), str.length());
}

int STRINGCLASS::compare(size_t nStart, size_t nLen,
                         const wxStringBase& str,
                         size_t nStart2, size_t nLen2) const
{
    wxASSERT(nStart <= length());
    wxASSERT(nStart2 <= str.length());
    size_type strLen  =     length() - nStart,
              strLen2 = str.length() - nStart2;
    nLen  = strLen  < nLen  ? strLen  : nLen;
    nLen2 = strLen2 < nLen2 ? strLen2 : nLen2;
    return ::wxDoCmp(data() + nStart, nLen, str.data() + nStart2, nLen2);
}

int STRINGCLASS::compare(const wxChar* sz) const
{
    size_t nLen = wxStrlen(sz);
    return ::wxDoCmp(data(), length(), sz, nLen);
}

int STRINGCLASS::compare(size_t nStart, size_t nLen,
                         const wxChar* sz, size_t nCount) const
{
    wxASSERT(nStart <= length());
    size_type strLen = length() - nStart;
    nLen = strLen < nLen ? strLen : nLen;
    if( nCount == npos )
        nCount = wxStrlen(sz);

    return ::wxDoCmp(data() + nStart, nLen, sz, nCount);
}

#undef STRINGCLASS

#endif // !wxUSE_STL || !defined(HAVE_STD_STRING_COMPARE)

// ===========================================================================
// wxString class core
// ===========================================================================

// ---------------------------------------------------------------------------
// construction and conversion
// ---------------------------------------------------------------------------

#if wxUSE_UNICODE

// from multibyte string
wxString::wxString(const char *psz, wxMBConv& conv, size_t nLength)
{
    // if nLength != npos, then we have to make a NULL-terminated copy
    // of first nLength bytes of psz first because the input buffer to MB2WC
    // must always be NULL-terminated:
    wxCharBuffer inBuf((const char *)NULL);
    if (nLength != npos)
    {
        wxASSERT( psz != NULL );
        wxCharBuffer tmp(nLength);
        memcpy(tmp.data(), psz, nLength);
        tmp.data()[nLength] = '\0';
        inBuf = tmp;
        psz = inBuf.data();
    }

    // first get the size of the buffer we need
    size_t nLen;
    if ( psz )
    {
        // calculate the needed size ourselves or use the provided one
        if (nLength == npos)
            nLen = strlen(psz);
        else
            nLen = nLength;
    }
    else
    {
        // nothing to convert
        nLen = 0;
    }


    // anything to do?
    if ( (nLen != 0) && (nLen != (size_t)-1) )
    {
        //Get length of converted string
        size_t dwConvLen = conv.MB2WC(NULL, psz, 0, nLen);

        //if valid, do the conversion
        if (dwConvLen != (size_t) -1)
        {
            //Get internal buffer
            wxStringBufferLength internalBuffer(*this, dwConvLen + 1);

            //Do the actual conversion & Set the length of the buffer
            internalBuffer.SetLength(
                   conv.MB2WC(internalBuffer, psz, dwConvLen + 1, nLen)
                                    );
        }
    }
}

//Convert wxString in Unicode mode to a multi-byte string
const wxCharBuffer wxString::mb_str(wxMBConv& conv) const
{
    //Get length of converted string
    size_t dwConvLen = conv.WC2MB(NULL, (*this).c_str(), 0, length());

    //if valid, do the conversion
    if (dwConvLen != (size_t) -1)
    {
        //Create good buffer
        wxCharBuffer buffer(dwConvLen + 1);

        //Do the actual conversion
        conv.WC2MB(buffer.data(), (*this).c_str(), dwConvLen + 1, length());

        return buffer;
    }

    //create bogus buffer
    wxCharBuffer buffer(1);
    buffer.data()[0u] = 0;
    return buffer;
}

#else // ANSI

#if wxUSE_WCHAR_T
// from wide string
wxString::wxString(const wchar_t *pwz, wxMBConv& conv, size_t nLength)
{
    // if nLength != npos, then we have to make a NULL-terminated copy
    // of first nLength chars of psz first because the input buffer to WC2MB
    // must always be NULL-terminated:
    wxWCharBuffer inBuf((const wchar_t *)NULL);
    if (nLength != npos)
    {
        wxASSERT( pwz != NULL );
        wxWCharBuffer tmp(nLength);
        memcpy(tmp.data(), pwz, nLength * sizeof(wchar_t));
        tmp.data()[nLength] = '\0';
        inBuf = tmp;
        pwz = inBuf.data();
    }

    // first get the size of the buffer we need
    size_t nLen;
    if ( pwz )
    {
        // calculate the needed size ourselves or use the provided one
        if (nLength == npos)
            nLen = wxWcslen(pwz);
        else
            nLen = nLength;
    }
    else
    {
        // nothing to convert
        nLen = 0;
    }

    // anything to do?
    if ( (nLen != 0) && (nLen != (size_t)-1) )
    {
        //Get length of converted string
        size_t dwConvLen = conv.WC2MB(NULL, pwz, 0, nLen);

        //if valid, do the conversion
        if (dwConvLen != (size_t) -1)
        {
            //Get internal buffer
            wxStringBufferLength internalBuffer(*this, dwConvLen + 1);

            //Do the actual conversion & Set the length of the buffer
            internalBuffer.SetLength(
                   conv.WC2MB(internalBuffer, pwz, dwConvLen + 1, nLen)
                                    );
        }
    }
}

//Converts this string to a wide character string if unicode
//mode is not enabled and wxUSE_WCHAR_T is enabled
const wxWCharBuffer wxString::wc_str(wxMBConv& conv) const
{
    //Get length of converted string
    size_t dwConvLen = conv.MB2WC(NULL, (*this).c_str(), 0, length());

    //if valid, do the conversion
    if (dwConvLen != (size_t) -1)
    {
        //Create good buffer
        wxWCharBuffer buffer(dwConvLen + 1);

        //Do the actual conversion
        conv.MB2WC(buffer.data(), (*this).c_str(), dwConvLen + 1, length());

        return buffer;
    }

    //create bogus buffer
    wxWCharBuffer buffer(1);
    buffer.data()[0u] = 0;
    return buffer;
}

#endif // wxUSE_WCHAR_T

#endif // Unicode/ANSI

// shrink to minimal size (releasing extra memory)
bool wxString::Shrink()
{
  wxString tmp(begin(), end());
  swap(tmp);
  return tmp.length() == length();
}

#if !wxUSE_STL
// get the pointer to writable buffer of (at least) nLen bytes
wxChar *wxString::GetWriteBuf(size_t nLen)
{
  if ( !AllocBeforeWrite(nLen) ) {
    // allocation failure handled by caller
    return NULL;
  }

  wxASSERT( GetStringData()->nRefs == 1 );
  GetStringData()->Validate(false);

  return m_pchData;
}

// put string back in a reasonable state after GetWriteBuf
void wxString::UngetWriteBuf()
{
  GetStringData()->nDataLength = wxStrlen(m_pchData);
  GetStringData()->Validate(true);
}

void wxString::UngetWriteBuf(size_t nLen)
{
  GetStringData()->nDataLength = nLen;
  GetStringData()->Validate(true);
}
#endif

// ---------------------------------------------------------------------------
// data access
// ---------------------------------------------------------------------------

// all functions are inline in string.h

// ---------------------------------------------------------------------------
// assignment operators
// ---------------------------------------------------------------------------

#if !wxUSE_UNICODE

// same as 'signed char' variant
wxString& wxString::operator=(const unsigned char* psz)
{
  *this = (const char *)psz;
  return *this;
}

#if wxUSE_WCHAR_T
wxString& wxString::operator=(const wchar_t *pwz)
{
  wxString str(pwz);
  swap(str);
  return *this;
}
#endif

#endif

/*
 * concatenation functions come in 5 flavours:
 *  string + string
 *  char   + string      and      string + char
 *  C str  + string      and      string + C str
 */

wxString operator+(const wxString& str1, const wxString& str2)
{
#if !wxUSE_STL
  wxASSERT( str1.GetStringData()->IsValid() );
  wxASSERT( str2.GetStringData()->IsValid() );
#endif

  wxString s = str1;
  s += str2;

  return s;
}

wxString operator+(const wxString& str, wxChar ch)
{
#if !wxUSE_STL
  wxASSERT( str.GetStringData()->IsValid() );
#endif

  wxString s = str;
  s += ch;

  return s;
}

wxString operator+(wxChar ch, const wxString& str)
{
#if !wxUSE_STL
  wxASSERT( str.GetStringData()->IsValid() );
#endif

  wxString s = ch;
  s += str;

  return s;
}

wxString operator+(const wxString& str, const wxChar *psz)
{
#if !wxUSE_STL
  wxASSERT( str.GetStringData()->IsValid() );
#endif

  wxString s;
  if ( !s.Alloc(wxStrlen(psz) + str.Len()) ) {
    wxFAIL_MSG( _T("out of memory in wxString::operator+") );
  }
  s = str;
  s += psz;

  return s;
}

wxString operator+(const wxChar *psz, const wxString& str)
{
#if !wxUSE_STL
  wxASSERT( str.GetStringData()->IsValid() );
#endif

  wxString s;
  if ( !s.Alloc(wxStrlen(psz) + str.Len()) ) {
    wxFAIL_MSG( _T("out of memory in wxString::operator+") );
  }
  s = psz;
  s += str;

  return s;
}

// ===========================================================================
// other common string functions
// ===========================================================================

int wxString::Cmp(const wxString& s) const
{
    return compare(s);
}

int wxString::Cmp(const wxChar* psz) const
{
    return compare(psz);
}

static inline int wxDoCmpNoCase(const wxChar* s1, size_t l1,
                                const wxChar* s2, size_t l2)
{
    size_t i;

    if( l1 == l2 )
    {
        for(i = 0; i < l1; ++i)
        {
            if(wxTolower(s1[i]) != wxTolower(s2[i]))
                break;
        }
        return i == l1 ? 0 : s1[i] < s2[i] ? -1 : 1;
    }
    else if( l1 < l2 )
    {
        for(i = 0; i < l1; ++i)
        {
            if(wxTolower(s1[i]) != wxTolower(s2[i]))
                break;
        }
        return i == l1 ? -1 : s1[i] < s2[i] ? -1 : 1;
    }
    else if( l1 > l2 )
    {
        for(i = 0; i < l2; ++i)
        {
            if(wxTolower(s1[i]) != wxTolower(s2[i]))
                break;
        }
        return i == l2 ? 1 : wxTolower(s1[i]) < wxTolower(s2[i]) ? -1 : 1;
    }

    wxFAIL;   // must never get there
    return 0; // quiet compilers
}

int wxString::CmpNoCase(const wxString& s) const
{
    return wxDoCmpNoCase(data(), length(), s.data(), s.length());
}

int wxString::CmpNoCase(const wxChar* psz) const
{
    int nLen = wxStrlen(psz);

    return wxDoCmpNoCase(data(), length(), psz, nLen);
}


#if wxUSE_UNICODE

#ifdef __MWERKS__
#ifndef __SCHAR_MAX__
#define __SCHAR_MAX__ 127
#endif
#endif

wxString wxString::FromAscii(const char *ascii)
{
    if (!ascii)
       return wxEmptyString;

    size_t len = strlen( ascii );
    wxString res;

    if ( len )
    {
        wxStringBuffer buf(res, len);

        wchar_t *dest = buf;

        for ( ;; )
        {
           if ( (*dest++ = (wchar_t)(unsigned char)*ascii++) == L'\0' )
               break;
        }
    }

    return res;
}

wxString wxString::FromAscii(const char ascii)
{
    // What do we do with '\0' ?

    wxString res;
    res += (wchar_t)(unsigned char) ascii;

    return res;
}

const wxCharBuffer wxString::ToAscii() const
{
    // this will allocate enough space for the terminating NUL too
    wxCharBuffer buffer(length());

    signed char *dest = (signed char *)buffer.data();

    const wchar_t *pwc = c_str();
    for ( ;; )
    {
        *dest++ = *pwc > SCHAR_MAX ? wxT('_') : *pwc;

        // the output string can't have embedded NULs anyhow, so we can safely
        // stop at first of them even if we do have any
        if ( !*pwc++ )
            break;
    }

    return buffer;
}

#endif // Unicode

// extract string of length nCount starting at nFirst
wxString wxString::Mid(size_t nFirst, size_t nCount) const
{
  size_t nLen = length();

  // default value of nCount is npos and means "till the end"
  if ( nCount == npos )
  {
    nCount = nLen - nFirst;
  }

  // out-of-bounds requests return sensible things
  if ( nFirst + nCount > nLen )
  {
    nCount = nLen - nFirst;
  }

  if ( nFirst > nLen )
  {
    // AllocCopy() will return empty string
    nCount = 0;
  }

  wxString dest(*this, nFirst, nCount);
  if ( dest.length() != nCount ) {
      wxFAIL_MSG( _T("out of memory in wxString::Mid") );
  }

  return dest;
}

// check that the string starts with prefix and return the rest of the string
// in the provided pointer if it is not NULL, otherwise return false
bool wxString::StartsWith(const wxChar *prefix, wxString *rest) const
{
    wxASSERT_MSG( prefix, _T("invalid parameter in wxString::StartsWith") );

    // first check if the beginning of the string matches the prefix: note
    // that we don't have to check that we don't run out of this string as
    // when we reach the terminating NUL, either prefix string ends too (and
    // then it's ok) or we break out of the loop because there is no match
    const wxChar *p = c_str();
    while ( *prefix )
    {
        if ( *prefix++ != *p++ )
        {
            // no match
            return false;
        }
    }

    if ( rest )
    {
        // put the rest of the string into provided pointer
        *rest = p;
    }

    return true;
}

// extract nCount last (rightmost) characters
wxString wxString::Right(size_t nCount) const
{
  if ( nCount > length() )
    nCount = length();

  wxString dest(*this, length() - nCount, nCount);
  if ( dest.length() != nCount ) {
    wxFAIL_MSG( _T("out of memory in wxString::Right") );
  }
  return dest;
}

// get all characters after the last occurence of ch
// (returns the whole string if ch not found)
wxString wxString::AfterLast(wxChar ch) const
{
  wxString str;
  int iPos = Find(ch, true);
  if ( iPos == wxNOT_FOUND )
    str = *this;
  else
    str = c_str() + iPos + 1;

  return str;
}

// extract nCount first (leftmost) characters
wxString wxString::Left(size_t nCount) const
{
  if ( nCount > length() )
    nCount = length();

  wxString dest(*this, 0, nCount);
  if ( dest.length() != nCount ) {
    wxFAIL_MSG( _T("out of memory in wxString::Left") );
  }
  return dest;
}

// get all characters before the first occurence of ch
// (returns the whole string if ch not found)
wxString wxString::BeforeFirst(wxChar ch) const
{
  int iPos = Find(ch);
  if ( iPos == wxNOT_FOUND ) iPos = length();
  return wxString(*this, 0, iPos);
}

/// get all characters before the last occurence of ch
/// (returns empty string if ch not found)
wxString wxString::BeforeLast(wxChar ch) const
{
  wxString str;
  int iPos = Find(ch, true);
  if ( iPos != wxNOT_FOUND && iPos != 0 )
    str = wxString(c_str(), iPos);

  return str;
}

/// get all characters after the first occurence of ch
/// (returns empty string if ch not found)
wxString wxString::AfterFirst(wxChar ch) const
{
  wxString str;
  int iPos = Find(ch);
  if ( iPos != wxNOT_FOUND )
    str = c_str() + iPos + 1;

  return str;
}

// replace first (or all) occurences of some substring with another one
size_t
wxString::Replace(const wxChar *szOld, const wxChar *szNew, bool bReplaceAll)
{
    // if we tried to replace an empty string we'd enter an infinite loop below
    wxCHECK_MSG( szOld && *szOld && szNew, 0,
                 _T("wxString::Replace(): invalid parameter") );

  size_t uiCount = 0;   // count of replacements made

  size_t uiOldLen = wxStrlen(szOld);

  wxString strTemp;
  const wxChar *pCurrent = c_str();
  const wxChar *pSubstr;
  while ( *pCurrent != wxT('\0') ) {
    pSubstr = wxStrstr(pCurrent, szOld);
    if ( pSubstr == NULL ) {
      // strTemp is unused if no replacements were made, so avoid the copy
      if ( uiCount == 0 )
        return 0;

      strTemp += pCurrent;    // copy the rest
      break;                  // exit the loop
    }
    else {
      // take chars before match
      size_type len = strTemp.length();
      strTemp.append(pCurrent, pSubstr - pCurrent);
      if ( strTemp.length() != (size_t)(len + pSubstr - pCurrent) ) {
        wxFAIL_MSG( _T("out of memory in wxString::Replace") );
        return 0;
      }
      strTemp += szNew;
      pCurrent = pSubstr + uiOldLen;  // restart after match

      uiCount++;

      // stop now?
      if ( !bReplaceAll ) {
        strTemp += pCurrent;    // copy the rest
        break;                  // exit the loop
      }
    }
  }

  // only done if there were replacements, otherwise would have returned above
  swap(strTemp);

  return uiCount;
}

bool wxString::IsAscii() const
{
  const wxChar *s = (const wxChar*) *this;
  while(*s){
    if(!isascii(*s)) return(false);
    s++;
  }
  return(true);
}

bool wxString::IsWord() const
{
  const wxChar *s = (const wxChar*) *this;
  while(*s){
    if(!wxIsalpha(*s)) return(false);
    s++;
  }
  return(true);
}

bool wxString::IsNumber() const
{
  const wxChar *s = (const wxChar*) *this;
  if (wxStrlen(s))
     if ((s[0] == wxT('-')) || (s[0] == wxT('+'))) s++;
  while(*s){
    if(!wxIsdigit(*s)) return(false);
    s++;
  }
  return(true);
}

wxString wxString::Strip(stripType w) const
{
    wxString s = *this;
    if ( w & leading ) s.Trim(false);
    if ( w & trailing ) s.Trim(true);
    return s;
}

// ---------------------------------------------------------------------------
// case conversion
// ---------------------------------------------------------------------------

wxString& wxString::MakeUpper()
{
  for ( iterator it = begin(), en = end(); it != en; ++it )
    *it = (wxChar)wxToupper(*it);

  return *this;
}

wxString& wxString::MakeLower()
{
  for ( iterator it = begin(), en = end(); it != en; ++it )
    *it = (wxChar)wxTolower(*it);

  return *this;
}

// ---------------------------------------------------------------------------
// trimming and padding
// ---------------------------------------------------------------------------

// some compilers (VC++ 6.0 not to name them) return true for a call to
// isspace('ê') in the C locale which seems to be broken to me, but we have to
// live with this by checking that the character is a 7 bit one - even if this
// may fail to detect some spaces (I don't know if Unicode doesn't have
// space-like symbols somewhere except in the first 128 chars), it is arguably
// still better than trimming away accented letters
inline int wxSafeIsspace(wxChar ch) { return (ch < 127) && wxIsspace(ch); }

// trims spaces (in the sense of isspace) from left or right side
wxString& wxString::Trim(bool bFromRight)
{
  // first check if we're going to modify the string at all
  if ( !IsEmpty() &&
       (
        (bFromRight && wxSafeIsspace(GetChar(Len() - 1))) ||
        (!bFromRight && wxSafeIsspace(GetChar(0u)))
       )
     )
  {
    if ( bFromRight )
    {
      // find last non-space character
      iterator psz = begin() + length() - 1;
      while ( wxSafeIsspace(*psz) && (psz >= begin()) )
        psz--;

      // truncate at trailing space start
      *++psz = wxT('\0');
      erase(psz, end());
    }
    else
    {
      // find first non-space character
      iterator psz = begin();
      while ( wxSafeIsspace(*psz) )
        psz++;

      // fix up data and length
      erase(begin(), psz);
    }
  }

  return *this;
}

// adds nCount characters chPad to the string from either side
wxString& wxString::Pad(size_t nCount, wxChar chPad, bool bFromRight)
{
  wxString s(chPad, nCount);

  if ( bFromRight )
    *this += s;
  else
  {
    s += *this;
    swap(s);
  }

  return *this;
}

// truncate the string
wxString& wxString::Truncate(size_t uiLen)
{
  if ( uiLen < Len() ) {
    erase(begin() + uiLen, end());
  }
  //else: nothing to do, string is already short enough

  return *this;
}

// ---------------------------------------------------------------------------
// finding (return wxNOT_FOUND if not found and index otherwise)
// ---------------------------------------------------------------------------

// find a character
int wxString::Find(wxChar ch, bool bFromEnd) const
{
  size_type idx = bFromEnd ? find_last_of(ch) : find_first_of(ch);

  return (idx == npos) ? wxNOT_FOUND : (int)idx;
}

// find a sub-string (like strstr)
int wxString::Find(const wxChar *pszSub) const
{
  size_type idx = find(pszSub);

  return (idx == npos) ? wxNOT_FOUND : (int)idx;
}

// ----------------------------------------------------------------------------
// conversion to numbers
// ----------------------------------------------------------------------------

bool wxString::ToLong(long *val, int base) const
{
    wxCHECK_MSG( val, false, _T("NULL pointer in wxString::ToLong") );
    wxASSERT_MSG( !base || (base > 1 && base <= 36), _T("invalid base") );

    const wxChar *start = c_str();
    wxChar *end;
    *val = wxStrtol(start, &end, base);

    // return true only if scan was stopped by the terminating NUL and if the
    // string was not empty to start with
    return !*end && (end != start);
}

bool wxString::ToULong(unsigned long *val, int base) const
{
    wxCHECK_MSG( val, false, _T("NULL pointer in wxString::ToULong") );
    wxASSERT_MSG( !base || (base > 1 && base <= 36), _T("invalid base") );

    const wxChar *start = c_str();
    wxChar *end;
    *val = wxStrtoul(start, &end, base);

    // return true only if scan was stopped by the terminating NUL and if the
    // string was not empty to start with
    return !*end && (end != start);
}

bool wxString::ToDouble(double *val) const
{
    wxCHECK_MSG( val, false, _T("NULL pointer in wxString::ToDouble") );

    const wxChar *start = c_str();
    wxChar *end;
    *val = wxStrtod(start, &end);

    // return true only if scan was stopped by the terminating NUL and if the
    // string was not empty to start with
    return !*end && (end != start);
}

// ---------------------------------------------------------------------------
// formatted output
// ---------------------------------------------------------------------------

/* static */
wxString wxString::Format(const wxChar *pszFormat, ...)
{
    va_list argptr;
    va_start(argptr, pszFormat);

    wxString s;
    s.PrintfV(pszFormat, argptr);

    va_end(argptr);

    return s;
}

/* static */
wxString wxString::FormatV(const wxChar *pszFormat, va_list argptr)
{
    wxString s;
    s.PrintfV(pszFormat, argptr);
    return s;
}

int wxString::Printf(const wxChar *pszFormat, ...)
{
  va_list argptr;
  va_start(argptr, pszFormat);

  int iLen = PrintfV(pszFormat, argptr);

  va_end(argptr);

  return iLen;
}

int wxString::PrintfV(const wxChar* pszFormat, va_list argptr)
{
    int size = 1024;
    int len;

    for ( ;; )
    {
        {
            wxStringBuffer tmp(*this, size + 1);
            wxChar* buf = tmp;

            if ( !buf )
            {
                // out of memory
                return -1;
            }

            // wxVsnprintf() may modify the original arg pointer, so pass it
            // only a copy
            va_list argptrcopy;
            wxVaCopy(argptrcopy, argptr);
            len = wxVsnprintf(buf, size, pszFormat, argptrcopy);
            va_end(argptrcopy);

            // some implementations of vsnprintf() don't NUL terminate
            // the string if there is not enough space for it so
            // always do it manually
            buf[size] = _T('\0');
        }

        // vsnprintf() may return either -1 (traditional Unix behaviour) or the
        // total number of characters which would have been written if the
        // buffer were large enough
        if ( len >= 0 && len <= size )
        {
            // ok, there was enough space
            break;
        }

        // still not enough, double it again
        size *= 2;
    }

    // we could have overshot
    Shrink();

    return Len();
}

// ----------------------------------------------------------------------------
// misc other operations
// ----------------------------------------------------------------------------

// returns true if the string matches the pattern which may contain '*' and
// '?' metacharacters (as usual, '?' matches any character and '*' any number
// of them)
bool wxString::Matches(const wxChar *pszMask) const
{
    // I disable this code as it doesn't seem to be faster (in fact, it seems
    // to be much slower) than the old, hand-written code below and using it
    // here requires always linking with libregex even if the user code doesn't
    // use it
#if 0 // wxUSE_REGEX
    // first translate the shell-like mask into a regex
    wxString pattern;
    pattern.reserve(wxStrlen(pszMask));

    pattern += _T('^');
    while ( *pszMask )
    {
        switch ( *pszMask )
        {
            case _T('?'):
                pattern += _T('.');
                break;

            case _T('*'):
                pattern += _T(".*");
                break;

            case _T('^'):
            case _T('.'):
            case _T('$'):
            case _T('('):
            case _T(')'):
            case _T('|'):
            case _T('+'):
            case _T('\\'):
                // these characters are special in a RE, quote them
                // (however note that we don't quote '[' and ']' to allow
                // using them for Unix shell like matching)
                pattern += _T('\\');
                // fall through

            default:
                pattern += *pszMask;
        }

        pszMask++;
    }
    pattern += _T('$');

    // and now use it
    return wxRegEx(pattern, wxRE_NOSUB | wxRE_EXTENDED).Matches(c_str());
#else // !wxUSE_REGEX
  // TODO: this is, of course, awfully inefficient...

  // the char currently being checked
  const wxChar *pszTxt = c_str();

  // the last location where '*' matched
  const wxChar *pszLastStarInText = NULL;
  const wxChar *pszLastStarInMask = NULL;

match:
  for ( ; *pszMask != wxT('\0'); pszMask++, pszTxt++ ) {
    switch ( *pszMask ) {
      case wxT('?'):
        if ( *pszTxt == wxT('\0') )
          return false;

        // pszTxt and pszMask will be incremented in the loop statement

        break;

      case wxT('*'):
        {
          // remember where we started to be able to backtrack later
          pszLastStarInText = pszTxt;
          pszLastStarInMask = pszMask;

          // ignore special chars immediately following this one
          // (should this be an error?)
          while ( *pszMask == wxT('*') || *pszMask == wxT('?') )
            pszMask++;

          // if there is nothing more, match
          if ( *pszMask == wxT('\0') )
            return true;

          // are there any other metacharacters in the mask?
          size_t uiLenMask;
          const wxChar *pEndMask = wxStrpbrk(pszMask, wxT("*?"));

          if ( pEndMask != NULL ) {
            // we have to match the string between two metachars
            uiLenMask = pEndMask - pszMask;
          }
          else {
            // we have to match the remainder of the string
            uiLenMask = wxStrlen(pszMask);
          }

          wxString strToMatch(pszMask, uiLenMask);
          const wxChar* pMatch = wxStrstr(pszTxt, strToMatch);
          if ( pMatch == NULL )
            return false;

          // -1 to compensate "++" in the loop
          pszTxt = pMatch + uiLenMask - 1;
          pszMask += uiLenMask - 1;
        }
        break;

      default:
        if ( *pszMask != *pszTxt )
          return false;
        break;
    }
  }

  // match only if nothing left
  if ( *pszTxt == wxT('\0') )
    return true;

  // if we failed to match, backtrack if we can
  if ( pszLastStarInText ) {
    pszTxt = pszLastStarInText + 1;
    pszMask = pszLastStarInMask;

    pszLastStarInText = NULL;

    // don't bother resetting pszLastStarInMask, it's unnecessary

    goto match;
  }

  return false;
#endif // wxUSE_REGEX/!wxUSE_REGEX
}

// Count the number of chars
int wxString::Freq(wxChar ch) const
{
    int count = 0;
    int len = Len();
    for (int i = 0; i < len; i++)
    {
        if (GetChar(i) == ch)
            count ++;
    }
    return count;
}

// convert to upper case, return the copy of the string
wxString wxString::Upper() const
{ wxString s(*this); return s.MakeUpper(); }

// convert to lower case, return the copy of the string
wxString wxString::Lower() const { wxString s(*this); return s.MakeLower(); }

int wxString::sprintf(const wxChar *pszFormat, ...)
  {
    va_list argptr;
    va_start(argptr, pszFormat);
    int iLen = PrintfV(pszFormat, argptr);
    va_end(argptr);
    return iLen;
  }

// ============================================================================
// ArrayString
// ============================================================================

#include "wx/arrstr.h"

#if !wxUSE_STL

// size increment = min(50% of current size, ARRAY_MAXSIZE_INCREMENT)
#define   ARRAY_MAXSIZE_INCREMENT       4096

#ifndef   ARRAY_DEFAULT_INITIAL_SIZE    // also defined in dynarray.h
#define   ARRAY_DEFAULT_INITIAL_SIZE    (16)
#endif

#define   STRING(p)   ((wxString *)(&(p)))

// ctor
void wxArrayString::Init(bool autoSort)
{
  m_nSize  =
  m_nCount = 0;
  m_pItems = (wxChar **) NULL;
  m_autoSort = autoSort;
}

// copy ctor
wxArrayString::wxArrayString(const wxArrayString& src)
{
  Init(src.m_autoSort);

  *this = src;
}

// assignment operator
wxArrayString& wxArrayString::operator=(const wxArrayString& src)
{
  if ( m_nSize > 0 )
    Clear();

  Copy(src);

  m_autoSort = src.m_autoSort;

  return *this;
}

void wxArrayString::Copy(const wxArrayString& src)
{
  if ( src.m_nCount > ARRAY_DEFAULT_INITIAL_SIZE )
    Alloc(src.m_nCount);

  for ( size_t n = 0; n < src.m_nCount; n++ )
    Add(src[n]);
}

// grow the array
void wxArrayString::Grow(size_t nIncrement)
{
  // only do it if no more place
  if ( (m_nSize - m_nCount) < nIncrement ) {
    // if ARRAY_DEFAULT_INITIAL_SIZE were set to 0, the initially empty would
    // be never resized!
    #if ARRAY_DEFAULT_INITIAL_SIZE == 0
      #error "ARRAY_DEFAULT_INITIAL_SIZE must be > 0!"
    #endif

    if ( m_nSize == 0 ) {
      // was empty, alloc some memory
      m_nSize = ARRAY_DEFAULT_INITIAL_SIZE;
      if (m_nSize < nIncrement)
          m_nSize = nIncrement;
      m_pItems = new wxChar *[m_nSize];
    }
    else {
      // otherwise when it's called for the first time, nIncrement would be 0
      // and the array would never be expanded
      // add 50% but not too much
      size_t ndefIncrement = m_nSize < ARRAY_DEFAULT_INITIAL_SIZE
                          ? ARRAY_DEFAULT_INITIAL_SIZE : m_nSize >> 1;
      if ( ndefIncrement > ARRAY_MAXSIZE_INCREMENT )
        ndefIncrement = ARRAY_MAXSIZE_INCREMENT;
      if ( nIncrement < ndefIncrement )
        nIncrement = ndefIncrement;
      m_nSize += nIncrement;
      wxChar **pNew = new wxChar *[m_nSize];

      // copy data to new location
      memcpy(pNew, m_pItems, m_nCount*sizeof(wxChar *));

      // delete old memory (but do not release the strings!)
      wxDELETEA(m_pItems);

      m_pItems = pNew;
    }
  }
}

void wxArrayString::Free()
{
  for ( size_t n = 0; n < m_nCount; n++ ) {
    STRING(m_pItems[n])->GetStringData()->Unlock();
  }
}

// deletes all the strings from the list
void wxArrayString::Empty()
{
  Free();

  m_nCount = 0;
}

// as Empty, but also frees memory
void wxArrayString::Clear()
{
  Free();

  m_nSize  =
  m_nCount = 0;

  wxDELETEA(m_pItems);
}

// dtor
wxArrayString::~wxArrayString()
{
  Free();

  wxDELETEA(m_pItems);
}

void wxArrayString::reserve(size_t nSize)
{
    Alloc(nSize);
}

// pre-allocates memory (frees the previous data!)
void wxArrayString::Alloc(size_t nSize)
{
  // only if old buffer was not big enough
  if ( nSize > m_nSize ) {
    Free();
    wxDELETEA(m_pItems);
    m_pItems = new wxChar *[nSize];
    m_nSize  = nSize;
  }

  m_nCount = 0;
}

// minimizes the memory usage by freeing unused memory
void wxArrayString::Shrink()
{
  // only do it if we have some memory to free
  if( m_nCount < m_nSize ) {
    // allocates exactly as much memory as we need
    wxChar **pNew = new wxChar *[m_nCount];

    // copy data to new location
    memcpy(pNew, m_pItems, m_nCount*sizeof(wxChar *));
    delete [] m_pItems;
    m_pItems = pNew;
  }
}

#if WXWIN_COMPATIBILITY_2_4

// return a wxString[] as required for some control ctors.
wxString* wxArrayString::GetStringArray() const
{
    wxString *array = 0;

    if( m_nCount > 0 )
    {
        array = new wxString[m_nCount];
        for( size_t i = 0; i < m_nCount; i++ )
            array[i] = m_pItems[i];
    }

    return array;
}

#endif // WXWIN_COMPATIBILITY_2_4

// searches the array for an item (forward or backwards)
int wxArrayString::Index(const wxChar *sz, bool bCase, bool bFromEnd) const
{
  if ( m_autoSort ) {
    // use binary search in the sorted array
    wxASSERT_MSG( bCase && !bFromEnd,
                  wxT("search parameters ignored for auto sorted array") );

    size_t i,
           lo = 0,
           hi = m_nCount;
    int res;
    while ( lo < hi ) {
      i = (lo + hi)/2;

      res = wxStrcmp(sz, m_pItems[i]);
      if ( res < 0 )
        hi = i;
      else if ( res > 0 )
        lo = i + 1;
      else
        return i;
    }

    return wxNOT_FOUND;
  }
  else {
    // use linear search in unsorted array
    if ( bFromEnd ) {
      if ( m_nCount > 0 ) {
        size_t ui = m_nCount;
        do {
          if ( STRING(m_pItems[--ui])->IsSameAs(sz, bCase) )
            return ui;
        }
        while ( ui != 0 );
      }
    }
    else {
      for( size_t ui = 0; ui < m_nCount; ui++ ) {
        if( STRING(m_pItems[ui])->IsSameAs(sz, bCase) )
          return ui;
      }
    }
  }

  return wxNOT_FOUND;
}

// add item at the end
size_t wxArrayString::Add(const wxString& str, size_t nInsert)
{
  if ( m_autoSort ) {
    // insert the string at the correct position to keep the array sorted
    size_t i,
           lo = 0,
           hi = m_nCount;
    int res;
    while ( lo < hi ) {
      i = (lo + hi)/2;

      res = str.Cmp(m_pItems[i]);
      if ( res < 0 )
        hi = i;
      else if ( res > 0 )
        lo = i + 1;
      else {
        lo = hi = i;
        break;
      }
    }

    wxASSERT_MSG( lo == hi, wxT("binary search broken") );

    Insert(str, lo, nInsert);

    return (size_t)lo;
  }
  else {
    wxASSERT( str.GetStringData()->IsValid() );

    Grow(nInsert);

    for (size_t i = 0; i < nInsert; i++)
    {
        // the string data must not be deleted!
        str.GetStringData()->Lock();

        // just append
        m_pItems[m_nCount + i] = (wxChar *)str.c_str(); // const_cast
    }
    size_t ret = m_nCount;
    m_nCount += nInsert;
    return ret;
  }
}

// add item at the given position
void wxArrayString::Insert(const wxString& str, size_t nIndex, size_t nInsert)
{
  wxASSERT( str.GetStringData()->IsValid() );

  wxCHECK_RET( nIndex <= m_nCount, wxT("bad index in wxArrayString::Insert") );
  wxCHECK_RET( m_nCount <= m_nCount + nInsert,
               wxT("array size overflow in wxArrayString::Insert") );

  Grow(nInsert);

  memmove(&m_pItems[nIndex + nInsert], &m_pItems[nIndex],
          (m_nCount - nIndex)*sizeof(wxChar *));

  for (size_t i = 0; i < nInsert; i++)
  {
      str.GetStringData()->Lock();
      m_pItems[nIndex + i] = (wxChar *)str.c_str();
  }
  m_nCount += nInsert;
}

// range insert (STL 23.2.4.3)
void
wxArrayString::insert(iterator it, const_iterator first, const_iterator last)
{
    const int idx = it - begin();

    // grow it once
    Grow(last - first);

    // reset "it" since it can change inside Grow()
    it = begin() + idx;

    while ( first != last )
    {
        it = insert(it, *first);

        // insert returns an iterator to the last element inserted but we need
        // insert the next after this one, that is before the next one
        ++it;

        ++first;
    }
}

// expand the array
void wxArrayString::SetCount(size_t count)
{
    Alloc(count);

    wxString s;
    while ( m_nCount < count )
        m_pItems[m_nCount++] = (wxChar *)s.c_str();
}

// removes item from array (by index)
void wxArrayString::RemoveAt(size_t nIndex, size_t nRemove)
{
  wxCHECK_RET( nIndex < m_nCount, wxT("bad index in wxArrayString::Remove") );
  wxCHECK_RET( nIndex + nRemove <= m_nCount,
               wxT("removing too many elements in wxArrayString::Remove") );

  // release our lock
  for (size_t i = 0; i < nRemove; i++)
      Item(nIndex + i).GetStringData()->Unlock();

  memmove(&m_pItems[nIndex], &m_pItems[nIndex + nRemove],
          (m_nCount - nIndex - nRemove)*sizeof(wxChar *));
  m_nCount -= nRemove;
}

// removes item from array (by value)
void wxArrayString::Remove(const wxChar *sz)
{
  int iIndex = Index(sz);

  wxCHECK_RET( iIndex != wxNOT_FOUND,
               wxT("removing inexistent element in wxArrayString::Remove") );

  RemoveAt(iIndex);
}

void wxArrayString::assign(const_iterator first, const_iterator last)
{
    reserve(last - first);
    for(; first != last; ++first)
        push_back(*first);
}

// ----------------------------------------------------------------------------
// sorting
// ----------------------------------------------------------------------------

// we can only sort one array at a time with the quick-sort based
// implementation
#if wxUSE_THREADS
  // need a critical section to protect access to gs_compareFunction and
  // gs_sortAscending variables
  static wxCriticalSection *gs_critsectStringSort = NULL;

  // call this before the value of the global sort vars is changed/after
  // you're finished with them
  #define START_SORT()     wxASSERT( !gs_critsectStringSort );                \
                           gs_critsectStringSort = new wxCriticalSection;     \
                           gs_critsectStringSort->Enter()
  #define END_SORT()       gs_critsectStringSort->Leave();                    \
                           delete gs_critsectStringSort;                      \
                           gs_critsectStringSort = NULL
#else // !threads
  #define START_SORT()
  #define END_SORT()
#endif // wxUSE_THREADS

// function to use for string comparaison
static wxArrayString::CompareFunction gs_compareFunction = NULL;

// if we don't use the compare function, this flag tells us if we sort the
// array in ascending or descending order
static bool gs_sortAscending = true;

// function which is called by quick sort
extern "C" int wxC_CALLING_CONV     // LINKAGEMODE
wxStringCompareFunction(const void *first, const void *second)
{
  wxString *strFirst = (wxString *)first;
  wxString *strSecond = (wxString *)second;

  if ( gs_compareFunction ) {
    return gs_compareFunction(*strFirst, *strSecond);
  }
  else {
    // maybe we should use wxStrcoll
    int result = strFirst->Cmp(*strSecond);

    return gs_sortAscending ? result : -result;
  }
}

// sort array elements using passed comparaison function
void wxArrayString::Sort(CompareFunction compareFunction)
{
  START_SORT();

  wxASSERT( !gs_compareFunction );  // must have been reset to NULL
  gs_compareFunction = compareFunction;

  DoSort();

  // reset it to NULL so that Sort(bool) will work the next time
  gs_compareFunction = NULL;

  END_SORT();
}

typedef  int (wxC_CALLING_CONV * wxStringCompareFn)(const void *first, const void *second);

void wxArrayString::Sort(CompareFunction2 compareFunction)
{
  qsort(m_pItems, m_nCount, sizeof(wxChar *), (wxStringCompareFn)compareFunction);
}

void wxArrayString::Sort(bool reverseOrder)
{
  Sort(reverseOrder ? wxStringSortDescending : wxStringSortAscending);
}

void wxArrayString::DoSort()
{
  wxCHECK_RET( !m_autoSort, wxT("can't use this method with sorted arrays") );

  // just sort the pointers using qsort() - of course it only works because
  // wxString() *is* a pointer to its data
  qsort(m_pItems, m_nCount, sizeof(wxChar *), wxStringCompareFunction);
}

bool wxArrayString::operator==(const wxArrayString& a) const
{
    if ( m_nCount != a.m_nCount )
        return false;

    for ( size_t n = 0; n < m_nCount; n++ )
    {
        if ( Item(n) != a[n] )
            return false;
    }

    return true;
}

#endif // !wxUSE_STL

int wxCMPFUNC_CONV wxStringSortAscending(wxString* s1, wxString* s2)
{
    return  s1->Cmp(*s2);
}

int wxCMPFUNC_CONV wxStringSortDescending(wxString* s1, wxString* s2)
{
    return -s1->Cmp(*s2);
}
