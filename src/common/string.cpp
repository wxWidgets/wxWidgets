/////////////////////////////////////////////////////////////////////////////
// Name:        string.cpp
// Purpose:     wxString class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
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
#endif

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#if wxUSE_WCSRTOMBS
  #include <wchar.h>    // for wcsrtombs(), see comments where it's used
#endif // GNU

#ifdef  WXSTRING_IS_WXOBJECT
  IMPLEMENT_DYNAMIC_CLASS(wxString, wxObject)
#endif  //WXSTRING_IS_WXOBJECT

// allocating extra space for each string consumes more memory but speeds up
// the concatenation operations (nLen is the current string's length)
// NB: EXTRA_ALLOC must be >= 0!
#define EXTRA_ALLOC       (19 - nLen % 16)

// ---------------------------------------------------------------------------
// static class variables definition
// ---------------------------------------------------------------------------

#ifdef  STD_STRING_COMPATIBILITY
  const size_t wxString::npos = STRING_MAXLEN;
#endif

// ----------------------------------------------------------------------------
// static data
// ----------------------------------------------------------------------------

// for an empty string, GetStringData() will return this address: this
// structure has the same layout as wxStringData and it's data() method will
// return the empty string (dummy pointer)
static const struct
{
  wxStringData data;
  char dummy;
} g_strEmpty = { {-1, 0, 0}, '\0' };

// empty C style string: points to 'string data' byte of g_strEmpty
extern const char *g_szNul = &g_strEmpty.dummy;

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

// we want to find out if the current platform supports vsnprintf()-like
// function: for Unix this is done with configure, for Windows we test the
// compiler explicitly.
#ifdef __WXMSW__
    #ifdef _MSC_VER
        #define wxVsprintf     _vsnprintf
    #endif
#else   // !Windows
    #ifdef HAVE_VSNPRINTF
        #define wxVsprintf       vsnprintf
    #endif
#endif  // Windows/!Windows

#ifndef wxVsprintf
    // in this case we'll use vsprintf() (which is ANSI and thus should be
    // always available), but it's unsafe because it doesn't check for buffer
    // size - so give a warning
    #define wxVsprintf(buffer,len,format,argptr) vsprintf(buffer,format, argptr)
#ifndef __SC__
    #pragma message("Using sprintf() because no snprintf()-like function defined")
#endif
#endif

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

#ifdef  STD_STRING_COMPATIBILITY

// MS Visual C++ version 5.0 provides the new STL headers as well as the old
// iostream ones.
//
// ATTN: you can _not_ use both of these in the same program!
#if wxUSE_IOSTREAMH
#include <iostream.h>
#define   NAMESPACE
#else
#include <iostream>
#  ifdef _MSC_VER
      using namespace std;
#  endif
// for msvc (bcc50+ also) you don't need these NAMESPACE defines,
// using namespace std; takes care of that.
#define   NAMESPACE   std::
#endif

#ifdef __WXMSW__
    #ifdef _MSC_VER
        #define wxVsprintf     _vsnprintf
    #endif
#else
    #if defined ( HAVE_VSNPRINTF )
        #define wxVsprintf       vsnprintf
    #endif
#endif

#ifndef wxVsprintf
    // vsprintf() is ANSI so we can always use it, but it's unsafe!
    #define wxVsprintf(buffer,len,format,argptr) vsprintf(buffer,format, argptr)
    #pragma message("Using sprintf() because no snprintf()-like function defined")
#endif

NAMESPACE istream& operator>>(NAMESPACE istream& is, wxString& WXUNUSED(str))
{
#if 0
  int w = is.width(0);
  if ( is.ipfx(0) ) {
    NAMESPACE streambuf *sb = is.rdbuf();
    str.erase();
    while ( true ) {
      int ch = sb->sbumpc ();
      if ( ch == EOF ) {
        is.setstate(NAMESPACE ios::eofbit);
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
    is.setstate(NAMESPACE ios::failbit);
#endif
  return is;
}

#endif  //std::string compatibility

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// this small class is used to gather statistics for performance tuning
//#define WXSTRING_STATISTICS
#ifdef  WXSTRING_STATISTICS
  class Averager
  {
  public:
    Averager(const char *sz) { m_sz = sz; m_nTotal = m_nCount = 0; }
   ~Averager()
   { printf("wxString: average %s = %f\n", m_sz, ((float)m_nTotal)/m_nCount); }

    void Add(size_t n) { m_nTotal += n; m_nCount++; }

  private:
    size_t m_nCount, m_nTotal;
    const char *m_sz;
  } g_averageLength("allocation size"),
    g_averageSummandLength("summand length"),
    g_averageConcatHit("hit probability in concat"),
    g_averageInitialLength("initial string length");

  #define STATISTICS_ADD(av, val) g_average##av.Add(val)
#else
  #define STATISTICS_ADD(av, val)
#endif // WXSTRING_STATISTICS

// ===========================================================================
// wxString class core
// ===========================================================================

// ---------------------------------------------------------------------------
// construction
// ---------------------------------------------------------------------------

// constructs string of <nLength> copies of character <ch>
wxString::wxString(char ch, size_t nLength)
{
  Init();

  if ( nLength > 0 ) {
    AllocBuffer(nLength);

    wxASSERT( sizeof(char) == 1 );  // can't use memset if not

    memset(m_pchData, ch, nLength);
  }
}

// takes nLength elements of psz starting at nPos
void wxString::InitWith(const char *psz, size_t nPos, size_t nLength)
{
  Init();

  wxASSERT( nPos <= Strlen(psz) );

  if ( nLength == STRING_MAXLEN )
    nLength = Strlen(psz + nPos);

  STATISTICS_ADD(InitialLength, nLength);

  if ( nLength > 0 ) {
    // trailing '\0' is written in AllocBuffer()
    AllocBuffer(nLength);
    memcpy(m_pchData, psz + nPos, nLength*sizeof(char));
  }
}

// the same as previous constructor, but for compilers using unsigned char
wxString::wxString(const unsigned char* psz, size_t nLength)
{
  InitWith((const char *)psz, 0, nLength);
}

#ifdef  STD_STRING_COMPATIBILITY

// poor man's iterators are "void *" pointers
wxString::wxString(const void *pStart, const void *pEnd)
{
  InitWith((const char *)pStart, 0,
           (const char *)pEnd - (const char *)pStart);
}

#endif  //std::string compatibility

// from wide string
wxString::wxString(const wchar_t *pwz)
{
  // first get necessary size

  // NB: GNU libc5 wcstombs() is completely broken, don't use it (it doesn't
  //     honor the 3rd parameter, thus it will happily crash here).
#if wxUSE_WCSRTOMBS
  // don't know if it's really needed (or if we can pass NULL), but better safe
  // than quick
  mbstate_t mbstate;  
  size_t nLen = wcsrtombs((char *) NULL, &pwz, 0, &mbstate);
#else  // !GNU libc
  size_t nLen = wcstombs((char *) NULL, pwz, 0);
#endif // GNU

  // empty?
  if ( nLen != 0 ) {
    AllocBuffer(nLen);
    wcstombs(m_pchData, pwz, nLen);
  }
  else {
    Init();
  }
}

// ---------------------------------------------------------------------------
// memory allocation
// ---------------------------------------------------------------------------

// allocates memory needed to store a C string of length nLen
void wxString::AllocBuffer(size_t nLen)
{
  wxASSERT( nLen >  0         );    //
  wxASSERT( nLen <= INT_MAX-1 );    // max size (enough room for 1 extra)

  STATISTICS_ADD(Length, nLen);

  // allocate memory:
  // 1) one extra character for '\0' termination
  // 2) sizeof(wxStringData) for housekeeping info
  wxStringData* pData = (wxStringData*)
    malloc(sizeof(wxStringData) + (nLen + EXTRA_ALLOC + 1)*sizeof(char));
  pData->nRefs        = 1;
  pData->nDataLength  = nLen;
  pData->nAllocLength = nLen + EXTRA_ALLOC;
  m_pchData           = pData->data();  // data starts after wxStringData
  m_pchData[nLen]     = '\0';
}

// must be called before changing this string
void wxString::CopyBeforeWrite()
{
  wxStringData* pData = GetStringData();

  if ( pData->IsShared() ) {
    pData->Unlock();                // memory not freed because shared
    size_t nLen = pData->nDataLength;
    AllocBuffer(nLen);
    memcpy(m_pchData, pData->data(), nLen*sizeof(char));
  }

  wxASSERT( !GetStringData()->IsShared() );  // we must be the only owner
}

// must be called before replacing contents of this string
void wxString::AllocBeforeWrite(size_t nLen)
{
  wxASSERT( nLen != 0 );  // doesn't make any sense

  // must not share string and must have enough space
  wxStringData* pData = GetStringData();
  if ( pData->IsShared() || (nLen > pData->nAllocLength) ) {
    // can't work with old buffer, get new one
    pData->Unlock();
    AllocBuffer(nLen);
  }
  else {
    // update the string length
    pData->nDataLength = nLen;
  }

  wxASSERT( !GetStringData()->IsShared() );  // we must be the only owner
}

// allocate enough memory for nLen characters
void wxString::Alloc(size_t nLen)
{
  wxStringData *pData = GetStringData();
  if ( pData->nAllocLength <= nLen ) {
    if ( pData->IsEmpty() ) {
      nLen += EXTRA_ALLOC;

      wxStringData* pData = (wxStringData*)
        malloc(sizeof(wxStringData) + (nLen + 1)*sizeof(char));
      pData->nRefs = 1;
      pData->nDataLength = 0;
      pData->nAllocLength = nLen;
      m_pchData = pData->data();  // data starts after wxStringData
      m_pchData[0u] = '\0';
    }
    else if ( pData->IsShared() ) {
      pData->Unlock();                // memory not freed because shared
      size_t nOldLen = pData->nDataLength;
      AllocBuffer(nLen);
      memcpy(m_pchData, pData->data(), nOldLen*sizeof(char));
    }
    else {
      nLen += EXTRA_ALLOC;

      wxStringData *p = (wxStringData *)
        realloc(pData, sizeof(wxStringData) + (nLen + 1)*sizeof(char));

      if ( p == NULL ) {
        // @@@ what to do on memory error?
        return;
      }

      // it's not important if the pointer changed or not (the check for this
      // is not faster than assigning to m_pchData in all cases)
      p->nAllocLength = nLen;
      m_pchData = p->data();
    }
  }
  //else: we've already got enough
}

// shrink to minimal size (releasing extra memory)
void wxString::Shrink()
{
  wxStringData *pData = GetStringData();

  // this variable is unused in release build, so avoid the compiler warning by
  // just not declaring it
#ifdef __WXDEBUG__
  void *p =
#endif
  realloc(pData, sizeof(wxStringData) + (pData->nDataLength + 1)*sizeof(char));

  wxASSERT( p != NULL );  // can't free memory?
  wxASSERT( p == pData ); // we're decrementing the size - block shouldn't move!
}

// get the pointer to writable buffer of (at least) nLen bytes
char *wxString::GetWriteBuf(size_t nLen)
{
  AllocBeforeWrite(nLen);

  wxASSERT( GetStringData()->nRefs == 1 );
  GetStringData()->Validate(FALSE);

  return m_pchData;
}

// put string back in a reasonable state after GetWriteBuf
void wxString::UngetWriteBuf()
{
  GetStringData()->nDataLength = strlen(m_pchData);
  GetStringData()->Validate(TRUE);
}

// ---------------------------------------------------------------------------
// data access
// ---------------------------------------------------------------------------

// all functions are inline in string.h

// ---------------------------------------------------------------------------
// assignment operators
// ---------------------------------------------------------------------------

// helper function: does real copy
void wxString::AssignCopy(size_t nSrcLen, const char *pszSrcData)
{
  if ( nSrcLen == 0 ) {
    Reinit();
  }
  else {
    AllocBeforeWrite(nSrcLen);
    memcpy(m_pchData, pszSrcData, nSrcLen*sizeof(char));
    GetStringData()->nDataLength = nSrcLen;
    m_pchData[nSrcLen] = '\0';
  }
}

// assigns one string to another
wxString& wxString::operator=(const wxString& stringSrc)
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
wxString& wxString::operator=(char ch)
{
  AssignCopy(1, &ch);
  return *this;
}

// assigns C string
wxString& wxString::operator=(const char *psz)
{
  AssignCopy(Strlen(psz), psz);
  return *this;
}

// same as 'signed char' variant
wxString& wxString::operator=(const unsigned char* psz)
{
  *this = (const char *)psz;
  return *this;
}

wxString& wxString::operator=(const wchar_t *pwz)
{
  wxString str(pwz);
  *this = str;
  return *this;
}

// ---------------------------------------------------------------------------
// string concatenation
// ---------------------------------------------------------------------------

// add something to this string
void wxString::ConcatSelf(int nSrcLen, const char *pszSrcData)
{
  STATISTICS_ADD(SummandLength, nSrcLen);

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
      AllocBuffer(nNewLen);
      memcpy(m_pchData, pOldData->data(), nLen*sizeof(char));
      pOldData->Unlock();
    }
    else if ( nNewLen > pData->nAllocLength ) {
      STATISTICS_ADD(ConcatHit, 0);

      // we have to grow the buffer
      Alloc(nNewLen);
    }
    else {
      STATISTICS_ADD(ConcatHit, 1);

      // the buffer is already big enough
    }

    // should be enough space
    wxASSERT( nNewLen <= GetStringData()->nAllocLength );

    // fast concatenation - all is done in our buffer
    memcpy(m_pchData + nLen, pszSrcData, nSrcLen*sizeof(char));

    m_pchData[nNewLen] = '\0';              // put terminating '\0'
    GetStringData()->nDataLength = nNewLen; // and fix the length
  }
  //else: the string to append was empty
}

/*
 * concatenation functions come in 5 flavours:
 *  string + string
 *  char   + string      and      string + char
 *  C str  + string      and      string + C str
 */

wxString operator+(const wxString& string1, const wxString& string2)
{
  wxASSERT( string1.GetStringData()->IsValid() );
  wxASSERT( string2.GetStringData()->IsValid() );

  wxString s = string1;
  s += string2;

  return s;
}

wxString operator+(const wxString& string, char ch)
{
  wxASSERT( string.GetStringData()->IsValid() );

  wxString s = string;
  s += ch;

  return s;
}

wxString operator+(char ch, const wxString& string)
{
  wxASSERT( string.GetStringData()->IsValid() );

  wxString s = ch;
  s += string;

  return s;
}

wxString operator+(const wxString& string, const char *psz)
{
  wxASSERT( string.GetStringData()->IsValid() );

  wxString s;
  s.Alloc(Strlen(psz) + string.Len());
  s = string;
  s += psz;

  return s;
}

wxString operator+(const char *psz, const wxString& string)
{
  wxASSERT( string.GetStringData()->IsValid() );

  wxString s;
  s.Alloc(Strlen(psz) + string.Len());
  s = psz;
  s += string;

  return s;
}

// ===========================================================================
// other common string functions
// ===========================================================================

// ---------------------------------------------------------------------------
// simple sub-string extraction
// ---------------------------------------------------------------------------

// helper function: clone the data attached to this string
void wxString::AllocCopy(wxString& dest, int nCopyLen, int nCopyIndex) const
{
  if ( nCopyLen == 0 ) {
    dest.Init();
  }
  else {
    dest.AllocBuffer(nCopyLen);
    memcpy(dest.m_pchData, m_pchData + nCopyIndex, nCopyLen*sizeof(char));
  }
}

// extract string of length nCount starting at nFirst
wxString wxString::Mid(size_t nFirst, size_t nCount) const
{
  wxStringData *pData = GetStringData();
  size_t nLen = pData->nDataLength;

  // default value of nCount is STRING_MAXLEN and means "till the end"
  if ( nCount == STRING_MAXLEN )
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

  wxString dest;
  AllocCopy(dest, nCount, nFirst);

  return dest;
}

// extract nCount last (rightmost) characters
wxString wxString::Right(size_t nCount) const
{
  if ( nCount > (size_t)GetStringData()->nDataLength )
    nCount = GetStringData()->nDataLength;

  wxString dest;
  AllocCopy(dest, nCount, GetStringData()->nDataLength - nCount);
  return dest;
}

// get all characters after the last occurence of ch
// (returns the whole string if ch not found)
wxString wxString::Right(char ch) const
{
  wxString str;
  int iPos = Find(ch, TRUE);
  if ( iPos == NOT_FOUND )
    str = *this;
  else
    str = c_str() + iPos + 1;

  return str;
}

// extract nCount first (leftmost) characters
wxString wxString::Left(size_t nCount) const
{
  if ( nCount > (size_t)GetStringData()->nDataLength )
    nCount = GetStringData()->nDataLength;

  wxString dest;
  AllocCopy(dest, nCount, 0);
  return dest;
}

// get all characters before the first occurence of ch
// (returns the whole string if ch not found)
wxString wxString::Left(char ch) const
{
  wxString str;
  for ( const char *pc = m_pchData; *pc != '\0' && *pc != ch; pc++ )
    str += *pc;

  return str;
}

/// get all characters before the last occurence of ch
/// (returns empty string if ch not found)
wxString wxString::Before(char ch) const
{
  wxString str;
  int iPos = Find(ch, TRUE);
  if ( iPos != NOT_FOUND && iPos != 0 )
    str = wxString(c_str(), iPos);

  return str;
}

/// get all characters after the first occurence of ch
/// (returns empty string if ch not found)
wxString wxString::After(char ch) const
{
  wxString str;
  int iPos = Find(ch);
  if ( iPos != NOT_FOUND )
    str = c_str() + iPos + 1;

  return str;
}

// replace first (or all) occurences of some substring with another one
size_t wxString::Replace(const char *szOld, const char *szNew, bool bReplaceAll)
{
  size_t uiCount = 0;   // count of replacements made

  size_t uiOldLen = Strlen(szOld);

  wxString strTemp;
  const char *pCurrent = m_pchData;
  const char *pSubstr;
  while ( *pCurrent != '\0' ) {
    pSubstr = strstr(pCurrent, szOld);
    if ( pSubstr == NULL ) {
      // strTemp is unused if no replacements were made, so avoid the copy
      if ( uiCount == 0 )
        return 0;

      strTemp += pCurrent;    // copy the rest
      break;                  // exit the loop
    }
    else {
      // take chars before match
      strTemp.ConcatSelf(pSubstr - pCurrent, pCurrent);
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
  *this = strTemp;

  return uiCount;
}

bool wxString::IsAscii() const
{
  const char *s = (const char*) *this;
  while(*s){
    if(!isascii(*s)) return(FALSE);
    s++;
  }
  return(TRUE);
}

bool wxString::IsWord() const
{
  const char *s = (const char*) *this;
  while(*s){
    if(!isalpha(*s)) return(FALSE);
    s++;
  }
  return(TRUE);
}

bool wxString::IsNumber() const
{
  const char *s = (const char*) *this;
  while(*s){
    if(!isdigit(*s)) return(FALSE);
    s++;
  }
  return(TRUE);
}

wxString wxString::Strip(stripType w) const
{
    wxString s = *this;
    if ( w & leading ) s.Trim(FALSE);
    if ( w & trailing ) s.Trim(TRUE);
    return s;
}

// ---------------------------------------------------------------------------
// case conversion
// ---------------------------------------------------------------------------

wxString& wxString::MakeUpper()
{
  CopyBeforeWrite();

  for ( char *p = m_pchData; *p; p++ )
    *p = (char)toupper(*p);

  return *this;
}

wxString& wxString::MakeLower()
{
  CopyBeforeWrite();

  for ( char *p = m_pchData; *p; p++ )
    *p = (char)tolower(*p);

  return *this;
}

// ---------------------------------------------------------------------------
// trimming and padding
// ---------------------------------------------------------------------------

// trims spaces (in the sense of isspace) from left or right side
wxString& wxString::Trim(bool bFromRight)
{
  // first check if we're going to modify the string at all
  if ( !IsEmpty() && 
       ( 
        (bFromRight && isspace(GetChar(Len() - 1))) ||
        (!bFromRight && isspace(GetChar(0u)))
       )
     )
  {
    // ok, there is at least one space to trim
    CopyBeforeWrite();

    if ( bFromRight )
    {
      // find last non-space character
      char *psz = m_pchData + GetStringData()->nDataLength - 1;
      while ( isspace(*psz) && (psz >= m_pchData) )
        psz--;

      // truncate at trailing space start
      *++psz = '\0';
      GetStringData()->nDataLength = psz - m_pchData;
    }
    else
    {
      // find first non-space character
      const char *psz = m_pchData;
      while ( isspace(*psz) )
        psz++;

      // fix up data and length
      int nDataLength = GetStringData()->nDataLength - (psz - m_pchData);
      memmove(m_pchData, psz, (nDataLength + 1)*sizeof(char));
      GetStringData()->nDataLength = nDataLength;
    }
  }

  return *this;
}

// adds nCount characters chPad to the string from either side
wxString& wxString::Pad(size_t nCount, char chPad, bool bFromRight)
{
  wxString s(chPad, nCount);

  if ( bFromRight )
    *this += s;
  else
  {
    s += *this;
    *this = s;
  }

  return *this;
}

// truncate the string
wxString& wxString::Truncate(size_t uiLen)
{
  if ( uiLen < Len() ) {
    CopyBeforeWrite();

    *(m_pchData + uiLen) = '\0';
    GetStringData()->nDataLength = uiLen;
  }
  //else: nothing to do, string is already short enough

  return *this;
}

// ---------------------------------------------------------------------------
// finding (return NOT_FOUND if not found and index otherwise)
// ---------------------------------------------------------------------------

// find a character
int wxString::Find(char ch, bool bFromEnd) const
{
  const char *psz = bFromEnd ? strrchr(m_pchData, ch) : strchr(m_pchData, ch);

  return (psz == NULL) ? NOT_FOUND : psz - m_pchData;
}

// find a sub-string (like strstr)
int wxString::Find(const char *pszSub) const
{
  const char *psz = strstr(m_pchData, pszSub);

  return (psz == NULL) ? NOT_FOUND : psz - m_pchData;
}

// ---------------------------------------------------------------------------
// stream-like operators
// ---------------------------------------------------------------------------
wxString& wxString::operator<<(int i)
{
    wxString res;
    res.Printf("%d", i);

    return (*this) << res;
}

wxString& wxString::operator<<(float f)
{
    wxString res;
    res.Printf("%f", f);

    return (*this) << res;
}

wxString& wxString::operator<<(double d)
{
    wxString res;
    res.Printf("%g", d);

    return (*this) << res;
}

// ---------------------------------------------------------------------------
// formatted output
// ---------------------------------------------------------------------------
int wxString::Printf(const char *pszFormat, ...)
{
  va_list argptr;
  va_start(argptr, pszFormat);

  int iLen = PrintfV(pszFormat, argptr);

  va_end(argptr);

  return iLen;
}

int wxString::PrintfV(const char* pszFormat, va_list argptr)
{
  // static buffer to avoid dynamic memory allocation each time
  static char s_szScratch[1024];

  // NB: wxVsprintf() may return either less than the buffer size or -1 if there
  //     is not enough place depending on implementation
  int iLen = wxVsprintf(s_szScratch, WXSIZEOF(s_szScratch), pszFormat, argptr);
  char *buffer;
  if ( iLen < (int)WXSIZEOF(s_szScratch) ) {
    buffer = s_szScratch;
  }
  else {
      int size = WXSIZEOF(s_szScratch) * 2;
      buffer = (char *)malloc(size);
      while ( buffer != NULL ) {
          iLen = wxVsprintf(buffer, WXSIZEOF(s_szScratch), pszFormat, argptr);
          if ( iLen < size ) {
              // ok, there was enough space
              break;
          }

          // still not enough, double it again
          buffer = (char *)realloc(buffer, size *= 2);
      }

      if ( !buffer ) {
          // out of memory
          return -1;
      }
  }

  AllocBeforeWrite(iLen);
  strcpy(m_pchData, buffer);

  if ( buffer != s_szScratch )
      free(buffer);

  return iLen;
}

// ----------------------------------------------------------------------------
// misc other operations
// ----------------------------------------------------------------------------
bool wxString::Matches(const char *pszMask) const
{
  // check char by char
  const char *pszTxt;
  for ( pszTxt = c_str(); *pszMask != '\0'; pszMask++, pszTxt++ ) {
    switch ( *pszMask ) {
      case '?':
        if ( *pszTxt == '\0' )
          return FALSE;

        pszTxt++;
        pszMask++;
        break;

      case '*':
        {
          // ignore special chars immediately following this one
          while ( *pszMask == '*' || *pszMask == '?' )
            pszMask++;

          // if there is nothing more, match
          if ( *pszMask == '\0' )
            return TRUE;

          // are there any other metacharacters in the mask?
          size_t uiLenMask;
          const char *pEndMask = strpbrk(pszMask, "*?");

          if ( pEndMask != NULL ) {
            // we have to match the string between two metachars
            uiLenMask = pEndMask - pszMask;
          }
          else {
            // we have to match the remainder of the string
            uiLenMask = strlen(pszMask);
          }

          wxString strToMatch(pszMask, uiLenMask);
          const char* pMatch = strstr(pszTxt, strToMatch);
          if ( pMatch == NULL )
            return FALSE;

          // -1 to compensate "++" in the loop
          pszTxt = pMatch + uiLenMask - 1;
          pszMask += uiLenMask - 1;
        }
        break;

      default:
        if ( *pszMask != *pszTxt )
          return FALSE;
        break;
    }
  }

  // match only if nothing left
  return *pszTxt == '\0';
}

// Count the number of chars
int wxString::Freq(char ch) const
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

// ---------------------------------------------------------------------------
// standard C++ library string functions
// ---------------------------------------------------------------------------
#ifdef  STD_STRING_COMPATIBILITY

wxString& wxString::insert(size_t nPos, const wxString& str)
{
  wxASSERT( str.GetStringData()->IsValid() );
  wxASSERT( nPos <= Len() );

  if ( !str.IsEmpty() ) {
    wxString strTmp;
    char *pc = strTmp.GetWriteBuf(Len() + str.Len());
    strncpy(pc, c_str(), nPos);
    strcpy(pc + nPos, str);
    strcpy(pc + nPos + str.Len(), c_str() + nPos);
    strTmp.UngetWriteBuf();
    *this = strTmp;
  }

  return *this;
}

size_t wxString::find(const wxString& str, size_t nStart) const
{
  wxASSERT( str.GetStringData()->IsValid() );
  wxASSERT( nStart <= Len() );

  const char *p = strstr(c_str() + nStart, str);

  return p == NULL ? npos : p - c_str();
}

// VC++ 1.5 can't cope with the default argument in the header.
#if ! (defined(_MSC_VER) && !defined(__WIN32__))
size_t wxString::find(const char* sz, size_t nStart, size_t n) const
{
  return find(wxString(sz, n == npos ? 0 : n), nStart);
}
#endif

// Gives a duplicate symbol (presumably a case-insensitivity problem)
#if !defined(__BORLANDC__)
size_t wxString::find(char ch, size_t nStart) const
{
  wxASSERT( nStart <= Len() );

  const char *p = strchr(c_str() + nStart, ch);

  return p == NULL ? npos : p - c_str();
}
#endif

size_t wxString::rfind(const wxString& str, size_t nStart) const
{
  wxASSERT( str.GetStringData()->IsValid() );
  wxASSERT( nStart <= Len() );

  // # could be quicker than that
  const char *p = c_str() + (nStart == npos ? Len() : nStart);
  while ( p >= c_str() + str.Len() ) {
    if ( strncmp(p - str.Len(), str, str.Len()) == 0 )
      return p - str.Len() - c_str();
    p--;
  }

  return npos;
}

// VC++ 1.5 can't cope with the default argument in the header.
#if ! (defined(_MSC_VER) && !defined(__WIN32__))
size_t wxString::rfind(const char* sz, size_t nStart, size_t n) const
{
  return rfind(wxString(sz, n == npos ? 0 : n), nStart);
}

size_t wxString::rfind(char ch, size_t nStart) const
{
  wxASSERT( nStart <= Len() );

  const char *p = strrchr(c_str() + nStart, ch);

  return p == NULL ? npos : p - c_str();
}
#endif

wxString wxString::substr(size_t nStart, size_t nLen) const
{
  // npos means 'take all'
  if ( nLen == npos )
    nLen = 0;

  wxASSERT( nStart + nLen <= Len() );

  return wxString(c_str() + nStart, nLen == npos ? 0 : nLen);
}

wxString& wxString::erase(size_t nStart, size_t nLen)
{
  wxString strTmp(c_str(), nStart);
  if ( nLen != npos ) {
    wxASSERT( nStart + nLen <= Len() );

    strTmp.append(c_str() + nStart + nLen);
  }

  *this = strTmp;
  return *this;
}

wxString& wxString::replace(size_t nStart, size_t nLen, const char *sz)
{
  wxASSERT( nStart + nLen <= Strlen(sz) );

  wxString strTmp;
  if ( nStart != 0 )
    strTmp.append(c_str(), nStart);
  strTmp += sz;
  strTmp.append(c_str() + nStart + nLen);

  *this = strTmp;
  return *this;
}

wxString& wxString::replace(size_t nStart, size_t nLen, size_t nCount, char ch)
{
  return replace(nStart, nLen, wxString(ch, nCount));
}

wxString& wxString::replace(size_t nStart, size_t nLen,
                            const wxString& str, size_t nStart2, size_t nLen2)
{
  return replace(nStart, nLen, str.substr(nStart2, nLen2));
}

wxString& wxString::replace(size_t nStart, size_t nLen,
                        const char* sz, size_t nCount)
{
  return replace(nStart, nLen, wxString(sz, nCount));
}

#endif  //std::string compatibility

// ============================================================================
// ArrayString
// ============================================================================

// size increment = max(50% of current size, ARRAY_MAXSIZE_INCREMENT)
#define   ARRAY_MAXSIZE_INCREMENT       4096
#ifndef   ARRAY_DEFAULT_INITIAL_SIZE    // also defined in dynarray.h
  #define   ARRAY_DEFAULT_INITIAL_SIZE    (16)
#endif

#define   STRING(p)   ((wxString *)(&(p)))

// ctor
wxArrayString::wxArrayString()
{
  m_nSize  =
  m_nCount = 0;
  m_pItems = (char **) NULL;
}

// copy ctor
wxArrayString::wxArrayString(const wxArrayString& src)
{
  m_nSize  =
  m_nCount = 0;
  m_pItems = (char **) NULL;

  *this = src;
}

// assignment operator
wxArrayString& wxArrayString::operator=(const wxArrayString& src)
{
  if ( m_nSize > 0 )
    Clear();

  if ( src.m_nCount > ARRAY_DEFAULT_INITIAL_SIZE )
    Alloc(src.m_nCount);

  // we can't just copy the pointers here because otherwise we would share
  // the strings with another array
  for ( size_t n = 0; n < src.m_nCount; n++ )
    Add(src[n]);

  if ( m_nCount != 0 )
    memcpy(m_pItems, src.m_pItems, m_nCount*sizeof(char *));

  return *this;
}

// grow the array
void wxArrayString::Grow()
{
  // only do it if no more place
  if( m_nCount == m_nSize ) {
    if( m_nSize == 0 ) {
      // was empty, alloc some memory
      m_nSize = ARRAY_DEFAULT_INITIAL_SIZE;
      m_pItems = new char *[m_nSize];
    }
    else {
      // otherwise when it's called for the first time, nIncrement would be 0
      // and the array would never be expanded
      wxASSERT( ARRAY_DEFAULT_INITIAL_SIZE != 0 );

      // add 50% but not too much
      size_t nIncrement = m_nSize < ARRAY_DEFAULT_INITIAL_SIZE
                          ? ARRAY_DEFAULT_INITIAL_SIZE : m_nSize >> 1;
      if ( nIncrement > ARRAY_MAXSIZE_INCREMENT )
        nIncrement = ARRAY_MAXSIZE_INCREMENT;
      m_nSize += nIncrement;
      char **pNew = new char *[m_nSize];

      // copy data to new location
      memcpy(pNew, m_pItems, m_nCount*sizeof(char *));

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

// pre-allocates memory (frees the previous data!)
void wxArrayString::Alloc(size_t nSize)
{
  wxASSERT( nSize > 0 );

  // only if old buffer was not big enough
  if ( nSize > m_nSize ) {
    Free();
    wxDELETEA(m_pItems);
    m_pItems = new char *[nSize];
    m_nSize  = nSize;
  }

  m_nCount = 0;
}

// searches the array for an item (forward or backwards)
int wxArrayString::Index(const char *sz, bool bCase, bool bFromEnd) const
{
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

  return NOT_FOUND;
}

// add item at the end
void wxArrayString::Add(const wxString& str)
{
  wxASSERT( str.GetStringData()->IsValid() );

  Grow();

  // the string data must not be deleted!
  str.GetStringData()->Lock();
  m_pItems[m_nCount++] = (char *)str.c_str();
}

// add item at the given position
void wxArrayString::Insert(const wxString& str, size_t nIndex)
{
  wxASSERT( str.GetStringData()->IsValid() );

  wxCHECK_RET( nIndex <= m_nCount, ("bad index in wxArrayString::Insert") );

  Grow();

  memmove(&m_pItems[nIndex + 1], &m_pItems[nIndex],
          (m_nCount - nIndex)*sizeof(char *));

  str.GetStringData()->Lock();
  m_pItems[nIndex] = (char *)str.c_str();

  m_nCount++;
}

// removes item from array (by index)
void wxArrayString::Remove(size_t nIndex)
{
  wxCHECK_RET( nIndex <= m_nCount, _("bad index in wxArrayString::Remove") );

  // release our lock
  Item(nIndex).GetStringData()->Unlock();

  memmove(&m_pItems[nIndex], &m_pItems[nIndex + 1],
          (m_nCount - nIndex - 1)*sizeof(char *));
  m_nCount--;
}

// removes item from array (by value)
void wxArrayString::Remove(const char *sz)
{
  int iIndex = Index(sz);

  wxCHECK_RET( iIndex != NOT_FOUND,
               _("removing inexistent element in wxArrayString::Remove") );

  Remove(iIndex);
}

// sort array elements using passed comparaison function

void wxArrayString::Sort(bool WXUNUSED(bCase), bool WXUNUSED(bReverse) )
{
  //@@@@ TO DO
  //qsort(m_pItems, m_nCount, sizeof(char *), fCmp);
}
