/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/string.cpp
// Purpose:     wxString class
// Author:      Vadim Zeitlin, Ryan Norton
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
//              (c) 2004 Ryan Norton <wxprojects@comcast.net>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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
    #include "wx/string.h"
#endif

#include <ctype.h>

#ifndef __WXWINCE__
    #include <errno.h>
#endif

#include <string.h>
#include <stdlib.h>

#ifdef __SALFORDC__
    #include <clib.h>
#endif

#include "wx/hashmap.h"

// string handling functions used by wxString:
#if wxUSE_UNICODE_UTF8
    #define wxStringMemcpy   memcpy
    #define wxStringMemcmp   memcmp
    #define wxStringMemchr   memchr
    #define wxStringStrlen   strlen
#else
    #define wxStringMemcpy   wxTmemcpy
    #define wxStringMemcmp   wxTmemcmp
    #define wxStringMemchr   wxTmemchr
    #define wxStringStrlen   wxStrlen
#endif


// ---------------------------------------------------------------------------
// static class variables definition
// ---------------------------------------------------------------------------

//According to STL _must_ be a -1 size_t
const size_t wxString::npos = (size_t) -1;

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM

#include <iostream>

wxSTD ostream& operator<<(wxSTD ostream& os, const wxCStrData& str)
{
// FIXME-UTF8: always, not only if wxUSE_UNICODE
#if wxUSE_UNICODE && !defined(__BORLANDC__)
    return os << str.AsWChar();
#else
    return os << str.AsChar();
#endif
}

wxSTD ostream& operator<<(wxSTD ostream& os, const wxString& str)
{
    return os << str.c_str();
}

wxSTD ostream& operator<<(wxSTD ostream& os, const wxCharBuffer& str)
{
    return os << str.data();
}

#ifndef __BORLANDC__
wxSTD ostream& operator<<(wxSTD ostream& os, const wxWCharBuffer& str)
{
    return os << str.data();
}
#endif

#endif // wxUSE_STD_IOSTREAM

// ----------------------------------------------------------------------------
// wxCStrData converted strings caching
// ----------------------------------------------------------------------------

// FIXME-UTF8: temporarily disabled because it doesn't work with global
//             string objects; re-enable after fixing this bug and benchmarking
//             performance to see if using a hash is a good idea at all
#if 0

// For backward compatibility reasons, it must be possible to assign the value
// returned by wxString::c_str() to a char* or wchar_t* variable and work with
// it. Returning wxCharBuffer from (const char*)c_str() wouldn't do the trick,
// because the memory would be freed immediately, but it has to be valid as long
// as the string is not modified, so that code like this still works:
//
// const wxChar *s = str.c_str();
// while ( s ) { ... }

// FIXME-UTF8: not thread safe!
// FIXME-UTF8: we currently clear the cached conversion only when the string is
//             destroyed, but we should do it when the string is modified, to
//             keep memory usage down
// FIXME-UTF8: we do the conversion every time As[W]Char() is called, but if we
//             invalidated the cache on every change, we could keep the previous
//             conversion
// FIXME-UTF8: add tracing of usage of these two methods - new code is supposed
//             to use mb_str() or wc_str() instead of (const [w]char*)c_str()

template<typename T>
static inline void DeleteStringFromConversionCache(T& hash, const wxString *s)
{
    typename T::iterator i = hash.find(wxConstCast(s, wxString));
    if ( i != hash.end() )
    {
        free(i->second);
        hash.erase(i);
    }
}

#if wxUSE_UNICODE
// NB: non-STL implementation doesn't compile with "const wxString*" key type,
//     so we have to use wxString* here and const-cast when used
WX_DECLARE_HASH_MAP(wxString*, char*, wxPointerHash, wxPointerEqual,
                    wxStringCharConversionCache);
static wxStringCharConversionCache gs_stringsCharCache;

const char* wxCStrData::AsChar() const
{
    // remove previously cache value, if any (see FIXMEs above):
    DeleteStringFromConversionCache(gs_stringsCharCache, m_str);

    // convert the string and keep it:
    const char *s = gs_stringsCharCache[wxConstCast(m_str, wxString)] =
        m_str->mb_str().release();

    return s + m_offset;
}
#endif // wxUSE_UNICODE

#if !wxUSE_UNICODE_WCHAR
WX_DECLARE_HASH_MAP(wxString*, wchar_t*, wxPointerHash, wxPointerEqual,
                    wxStringWCharConversionCache);
static wxStringWCharConversionCache gs_stringsWCharCache;

const wchar_t* wxCStrData::AsWChar() const
{
    // remove previously cache value, if any (see FIXMEs above):
    DeleteStringFromConversionCache(gs_stringsWCharCache, m_str);

    // convert the string and keep it:
    const wchar_t *s = gs_stringsWCharCache[wxConstCast(m_str, wxString)] =
        m_str->wc_str().release();

    return s + m_offset;
}
#endif // !wxUSE_UNICODE_WCHAR

wxString::~wxString()
{
#if wxUSE_UNICODE
    // FIXME-UTF8: do this only if locale is not UTF8 if wxUSE_UNICODE_UTF8
    DeleteStringFromConversionCache(gs_stringsCharCache, this);
#endif
#if !wxUSE_UNICODE_WCHAR
    DeleteStringFromConversionCache(gs_stringsWCharCache, this);
#endif
}
#endif

#if wxUSE_UNICODE
const char* wxCStrData::AsChar() const
{
    wxString *str = wxConstCast(m_str, wxString);

    // convert the string:
    wxCharBuffer buf(str->mb_str());

    // FIXME-UTF8: do the conversion in-place in the existing buffer
    if ( str->m_convertedToChar &&
         strlen(buf) == strlen(str->m_convertedToChar) )
    {
        // keep the same buffer for as long as possible, so that several calls
        // to c_str() in a row still work:
        strcpy(str->m_convertedToChar, buf);
    }
    else
    {
        str->m_convertedToChar = buf.release();
    }

    // and keep it:
    return str->m_convertedToChar + m_offset;
}
#endif // wxUSE_UNICODE

#if !wxUSE_UNICODE_WCHAR
const wchar_t* wxCStrData::AsWChar() const
{
    wxString *str = wxConstCast(m_str, wxString);

    // convert the string:
    wxWCharBuffer buf(str->wc_str());

    // FIXME-UTF8: do the conversion in-place in the existing buffer
    if ( str->m_convertedToWChar &&
         wxWcslen(buf) == wxWcslen(str->m_convertedToWChar) )
    {
        // keep the same buffer for as long as possible, so that several calls
        // to c_str() in a row still work:
        memcpy(str->m_convertedToWChar, buf, sizeof(wchar_t) * wxWcslen(buf));
    }
    else
    {
        str->m_convertedToWChar = buf.release();
    }

    // and keep it:
    return str->m_convertedToWChar + m_offset;
}
#endif // !wxUSE_UNICODE_WCHAR

// ===========================================================================
// wxString class core
// ===========================================================================

// ---------------------------------------------------------------------------
// construction and conversion
// ---------------------------------------------------------------------------

#if wxUSE_UNICODE
/* static */
wxString::SubstrBufFromMB wxString::ConvertStr(const char *psz, size_t nLength,
                                               const wxMBConv& conv)
{
    // anything to do?
    if ( !psz || nLength == 0 )
        return SubstrBufFromMB();

    if ( nLength == npos )
        nLength = wxNO_LEN;

    size_t wcLen;
    wxWCharBuffer wcBuf(conv.cMB2WC(psz, nLength, &wcLen));
    if ( !wcLen )
        return SubstrBufFromMB();
    else
        return SubstrBufFromMB(wcBuf, wcLen);
}
#else
/* static */
wxString::SubstrBufFromWC wxString::ConvertStr(const wchar_t *pwz, size_t nLength,
                                               const wxMBConv& conv)
{
    // anything to do?
    if ( !pwz || nLength == 0 )
        return SubstrBufFromWC();

    if ( nLength == npos )
        nLength = wxNO_LEN;

    size_t mbLen;
    wxCharBuffer mbBuf(conv.cWC2MB(pwz, nLength, &mbLen));
    if ( !mbLen )
        return SubstrBufFromWC();
    else
        return SubstrBufFromWC(mbBuf, mbLen);
}
#endif


#if wxUSE_UNICODE

//Convert wxString in Unicode mode to a multi-byte string
const wxCharBuffer wxString::mb_str(const wxMBConv& conv) const
{
    return conv.cWC2MB(c_str(), length() + 1 /* size, not length */, NULL);
}

#else // ANSI

#if wxUSE_WCHAR_T

//Converts this string to a wide character string if unicode
//mode is not enabled and wxUSE_WCHAR_T is enabled
const wxWCharBuffer wxString::wc_str(const wxMBConv& conv) const
{
    return conv.cMB2WC(c_str(), length() + 1 /* size, not length */, NULL);
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

// deprecated compatibility code:
#if WXWIN_COMPATIBILITY_2_8 && !wxUSE_STL_BASED_WXSTRING && !wxUSE_UNICODE_UTF8
wxChar *wxString::GetWriteBuf(size_t nLen)
{
    return DoGetWriteBuf(nLen);
}

void wxString::UngetWriteBuf()
{
    DoUngetWriteBuf();
}

void wxString::UngetWriteBuf(size_t nLen)
{
    DoUngetWriteBuf(nLen);
}
#endif // WXWIN_COMPATIBILITY_2_8 && !wxUSE_STL_BASED_WXSTRING && !wxUSE_UNICODE_UTF8


// ---------------------------------------------------------------------------
// data access
// ---------------------------------------------------------------------------

// all functions are inline in string.h

// ---------------------------------------------------------------------------
// concatenation operators
// ---------------------------------------------------------------------------

/*
 * concatenation functions come in 5 flavours:
 *  string + string
 *  char   + string      and      string + char
 *  C str  + string      and      string + C str
 */

wxString operator+(const wxString& str1, const wxString& str2)
{
#if !wxUSE_STL_BASED_WXSTRING
    wxASSERT( str1.IsValid() );
    wxASSERT( str2.IsValid() );
#endif

    wxString s = str1;
    s += str2;

    return s;
}

wxString operator+(const wxString& str, wxUniChar ch)
{
#if !wxUSE_STL_BASED_WXSTRING
    wxASSERT( str.IsValid() );
#endif

    wxString s = str;
    s += ch;

    return s;
}

wxString operator+(wxUniChar ch, const wxString& str)
{
#if !wxUSE_STL_BASED_WXSTRING
    wxASSERT( str.IsValid() );
#endif

    wxString s = ch;
    s += str;

    return s;
}

wxString operator+(const wxString& str, const char *psz)
{
#if !wxUSE_STL_BASED_WXSTRING
    wxASSERT( str.IsValid() );
#endif

    wxString s;
    if ( !s.Alloc(strlen(psz) + str.length()) ) {
        wxFAIL_MSG( _T("out of memory in wxString::operator+") );
    }
    s += str;
    s += psz;

    return s;
}

wxString operator+(const wxString& str, const wchar_t *pwz)
{
#if !wxUSE_STL_BASED_WXSTRING
    wxASSERT( str.IsValid() );
#endif

    wxString s;
    if ( !s.Alloc(wxWcslen(pwz) + str.length()) ) {
        wxFAIL_MSG( _T("out of memory in wxString::operator+") );
    }
    s += str;
    s += pwz;

    return s;
}

wxString operator+(const char *psz, const wxString& str)
{
#if !wxUSE_STL_BASED_WXSTRING
    wxASSERT( str.IsValid() );
#endif

    wxString s;
    if ( !s.Alloc(strlen(psz) + str.length()) ) {
        wxFAIL_MSG( _T("out of memory in wxString::operator+") );
    }
    s = psz;
    s += str;

    return s;
}

wxString operator+(const wchar_t *pwz, const wxString& str)
{
#if !wxUSE_STL_BASED_WXSTRING
    wxASSERT( str.IsValid() );
#endif

    wxString s;
    if ( !s.Alloc(wxWcslen(pwz) + str.length()) ) {
        wxFAIL_MSG( _T("out of memory in wxString::operator+") );
    }
    s = pwz;
    s += str;

    return s;
}

// ---------------------------------------------------------------------------
// string comparison
// ---------------------------------------------------------------------------

#ifdef HAVE_STD_STRING_COMPARE

// NB: Comparison code (both if HAVE_STD_STRING_COMPARE and if not) works with
//     UTF-8 encoded strings too, thanks to UTF-8's design which allows us to
//     sort strings in characters code point order by sorting the byte sequence
//     in byte values order (i.e. what strcmp() and memcmp() do).

int wxString::compare(const wxString& str) const
{
    return m_impl.compare(str.m_impl);
}

int wxString::compare(size_t nStart, size_t nLen,
                      const wxString& str) const
{
    size_t pos, len;
    PosLenToImpl(nStart, nLen, &pos, &len);
    return m_impl.compare(pos, len, str.m_impl);
}

int wxString::compare(size_t nStart, size_t nLen,
                      const wxString& str,
                      size_t nStart2, size_t nLen2) const
{
    size_t pos, len;
    PosLenToImpl(nStart, nLen, &pos, &len);

    size_t pos2, len2;
    str.PosLenToImpl(nStart2, nLen2, &pos2, &len2);

    return m_impl.compare(pos, len, str.m_impl, pos2, len2);
}

int wxString::compare(const char* sz) const
{
    return m_impl.compare(ImplStr(sz));
}

int wxString::compare(const wchar_t* sz) const
{
    return m_impl.compare(ImplStr(sz));
}

int wxString::compare(size_t nStart, size_t nLen,
                      const char* sz, size_t nCount) const
{
    size_t pos, len;
    PosLenToImpl(nStart, nLen, &pos, &len);

    SubstrBufFromMB str(ImplStr(sz, nCount));

    return m_impl.compare(pos, len, str.data, str.len);
}

int wxString::compare(size_t nStart, size_t nLen,
                      const wchar_t* sz, size_t nCount) const
{
    size_t pos, len;
    PosLenToImpl(nStart, nLen, &pos, &len);

    SubstrBufFromWC str(ImplStr(sz, nCount));

    return m_impl.compare(pos, len, str.data, str.len);
}

#else // !HAVE_STD_STRING_COMPARE

static inline int wxDoCmp(const wxStringCharType* s1, size_t l1,
                          const wxStringCharType* s2, size_t l2)
{
    if( l1 == l2 )
        return wxStringMemcmp(s1, s2, l1);
    else if( l1 < l2 )
    {
        int ret = wxStringMemcmp(s1, s2, l1);
        return ret == 0 ? -1 : ret;
    }
    else
    {
        int ret = wxStringMemcmp(s1, s2, l2);
        return ret == 0 ? +1 : ret;
    }
}

int wxString::compare(const wxString& str) const
{
    return ::wxDoCmp(m_impl.data(), m_impl.length(),
                     str.m_impl.data(), str.m_impl.length());
}

int wxString::compare(size_t nStart, size_t nLen,
                      const wxString& str) const
{
    wxASSERT(nStart <= length());
    size_type strLen = length() - nStart;
    nLen = strLen < nLen ? strLen : nLen;

    size_t pos, len;
    PosLenToImpl(nStart, nLen, &pos, &len);

    return ::wxDoCmp(m_impl.data() + pos,  len,
                     str.m_impl.data(), str.m_impl.length());
}

int wxString::compare(size_t nStart, size_t nLen,
                      const wxString& str,
                      size_t nStart2, size_t nLen2) const
{
    wxASSERT(nStart <= length());
    wxASSERT(nStart2 <= str.length());
    size_type strLen  =     length() - nStart,
              strLen2 = str.length() - nStart2;
    nLen  = strLen  < nLen  ? strLen  : nLen;
    nLen2 = strLen2 < nLen2 ? strLen2 : nLen2;

    size_t pos, len;
    PosLenToImpl(nStart, nLen, &pos, &len);
    size_t pos2, len2;
    str.PosLenToImpl(nStart2, nLen2, &pos2, &len2);

    return ::wxDoCmp(m_impl.data() + pos, len,
                     str.m_impl.data() + pos2, len2);
}

int wxString::compare(const char* sz) const
{
    SubstrBufFromMB str(ImplStr(sz, npos));
    if ( str.len == npos )
        str.len = wxStringStrlen(str.data);
    return ::wxDoCmp(m_impl.data(), m_impl.length(), str.data, str.len);
}

int wxString::compare(const wchar_t* sz) const
{
    SubstrBufFromWC str(ImplStr(sz, npos));
    if ( str.len == npos )
        str.len = wxStringStrlen(str.data);
    return ::wxDoCmp(m_impl.data(), m_impl.length(), str.data, str.len);
}

int wxString::compare(size_t nStart, size_t nLen,
                      const char* sz, size_t nCount) const
{
    wxASSERT(nStart <= length());
    size_type strLen = length() - nStart;
    nLen = strLen < nLen ? strLen : nLen;

    size_t pos, len;
    PosLenToImpl(nStart, nLen, &pos, &len);

    SubstrBufFromMB str(ImplStr(sz, nCount));
    if ( str.len == npos )
        str.len = wxStringStrlen(str.data);

    return ::wxDoCmp(m_impl.data() + pos, len, str.data, str.len);
}

int wxString::compare(size_t nStart, size_t nLen,
                      const wchar_t* sz, size_t nCount) const
{
    wxASSERT(nStart <= length());
    size_type strLen = length() - nStart;
    nLen = strLen < nLen ? strLen : nLen;

    size_t pos, len;
    PosLenToImpl(nStart, nLen, &pos, &len);

    SubstrBufFromWC str(ImplStr(sz, nCount));
    if ( str.len == npos )
        str.len = wxStringStrlen(str.data);

    return ::wxDoCmp(m_impl.data() + pos, len, str.data, str.len);
}

#endif // HAVE_STD_STRING_COMPARE/!HAVE_STD_STRING_COMPARE


// ---------------------------------------------------------------------------
// find_{first,last}_[not]_of functions
// ---------------------------------------------------------------------------

#if !wxUSE_STL_BASED_WXSTRING || wxUSE_UNICODE_UTF8

// NB: All these functions are implemented  with the argument being wxChar*,
//     i.e. widechar string in any Unicode build, even though native string
//     representation is char* in the UTF-8 build. This is because we couldn't
//     use memchr() to determine if a character is in a set encoded as UTF-8.

size_t wxString::find_first_of(const wxChar* sz, size_t nStart) const
{
    return find_first_of(sz, nStart, wxStrlen(sz));
}

size_t wxString::find_first_not_of(const wxChar* sz, size_t nStart) const
{
    return find_first_not_of(sz, nStart, wxStrlen(sz));
}

size_t wxString::find_first_of(const wxChar* sz, size_t nStart, size_t n) const
{
    wxASSERT_MSG( nStart <= length(),  _T("invalid index") );

    size_t idx = nStart;
    for ( const_iterator i = begin() + nStart; i != end(); ++idx, ++i )
    {
        if ( wxTmemchr(sz, *i, n) )
            return idx;
    }

    return npos;
}

size_t wxString::find_first_not_of(const wxChar* sz, size_t nStart, size_t n) const
{
    wxASSERT_MSG( nStart <= length(),  _T("invalid index") );

    size_t idx = nStart;
    for ( const_iterator i = begin() + nStart; i != end(); ++idx, ++i )
    {
        if ( !wxTmemchr(sz, *i, n) )
            return idx;
    }

    return npos;
}


size_t wxString::find_last_of(const wxChar* sz, size_t nStart) const
{
    return find_last_of(sz, nStart, wxStrlen(sz));
}

size_t wxString::find_last_not_of(const wxChar* sz, size_t nStart) const
{
    return find_last_not_of(sz, nStart, wxStrlen(sz));
}

size_t wxString::find_last_of(const wxChar* sz, size_t nStart, size_t n) const
{
    size_t len = length();

    if ( nStart == npos )
    {
        nStart = len - 1;
    }
    else
    {
        wxASSERT_MSG( nStart <= len, _T("invalid index") );
    }

    size_t idx = nStart;
    for ( const_reverse_iterator i = rbegin() + (len - nStart - 1);
          i != rend(); --idx, ++i )
    {
        if ( wxTmemchr(sz, *i, n) )
            return idx;
    }

    return npos;
}

size_t wxString::find_last_not_of(const wxChar* sz, size_t nStart, size_t n) const
{
    size_t len = length();

    if ( nStart == npos )
    {
        nStart = len - 1;
    }
    else
    {
        wxASSERT_MSG( nStart <= len, _T("invalid index") );
    }

    size_t idx = nStart;
    for ( const_reverse_iterator i = rbegin() + (len - nStart - 1);
          i != rend(); --idx, ++i )
    {
        if ( !wxTmemchr(sz, *i, n) )
            return idx;
    }

    return npos;
}

size_t wxString::find_first_not_of(wxUniChar ch, size_t nStart) const
{
    wxASSERT_MSG( nStart <= length(),  _T("invalid index") );

    size_t idx = nStart;
    for ( const_iterator i = begin() + nStart; i != end(); ++idx, ++i )
    {
        if ( *i != ch )
            return idx;
    }

    return npos;
}

size_t wxString::find_last_not_of(wxUniChar ch, size_t nStart) const
{
    size_t len = length();

    if ( nStart == npos )
    {
        nStart = len - 1;
    }
    else
    {
        wxASSERT_MSG( nStart <= len, _T("invalid index") );
    }

    size_t idx = nStart;
    for ( const_reverse_iterator i = rbegin() + (len - nStart - 1);
          i != rend(); --idx, ++i )
    {
        if ( *i != ch )
            return idx;
    }

    return npos;
}

// the functions above were implemented for wchar_t* arguments in Unicode
// build and char* in ANSI build; below are implementations for the other
// version:
#if wxUSE_UNICODE
    #define wxOtherCharType char
    #define STRCONV         (const wxChar*)wxConvLibc.cMB2WC
#else
    #define wxOtherCharType wchar_t
    #define STRCONV         (const wxChar*)wxConvLibc.cWC2MB
#endif

size_t wxString::find_first_of(const wxOtherCharType* sz, size_t nStart) const
    { return find_first_of(STRCONV(sz), nStart); }

size_t wxString::find_first_of(const wxOtherCharType* sz, size_t nStart,
                               size_t n) const
    { return find_first_of(STRCONV(sz, n, NULL), nStart, n); }
size_t wxString::find_last_of(const wxOtherCharType* sz, size_t nStart) const
    { return find_last_of(STRCONV(sz), nStart); }
size_t wxString::find_last_of(const wxOtherCharType* sz, size_t nStart,
                              size_t n) const
    { return find_last_of(STRCONV(sz, n, NULL), nStart, n); }
size_t wxString::find_first_not_of(const wxOtherCharType* sz, size_t nStart) const
    { return find_first_not_of(STRCONV(sz), nStart); }
size_t wxString::find_first_not_of(const wxOtherCharType* sz, size_t nStart,
                                   size_t n) const
    { return find_first_not_of(STRCONV(sz, n, NULL), nStart, n); }
size_t wxString::find_last_not_of(const wxOtherCharType* sz, size_t nStart) const
    { return find_last_not_of(STRCONV(sz), nStart); }
size_t wxString::find_last_not_of(const wxOtherCharType* sz, size_t nStart,
                                  size_t n) const
    { return find_last_not_of(STRCONV(sz, n, NULL), nStart, n); }

#undef wxOtherCharType
#undef STRCONV

#endif // !wxUSE_STL_BASED_WXSTRING || wxUSE_UNICODE_UTF8

// ===========================================================================
// other common string functions
// ===========================================================================

int wxString::CmpNoCase(const wxString& s) const
{
    // FIXME-UTF8: use wxUniChar::ToLower/ToUpper once added

    size_t idx = 0;
    const_iterator i1 = begin();
    const_iterator end1 = end();
    const_iterator i2 = s.begin();
    const_iterator end2 = s.end();

    for ( ; i1 != end1 && i2 != end2; ++idx, ++i1, ++i2 )
    {
        wxUniChar lower1 = (wxChar)wxTolower(*i1);
        wxUniChar lower2 = (wxChar)wxTolower(*i2);
        if ( lower1 != lower2 )
            return lower1 < lower2 ? -1 : 1;
    }

    size_t len1 = length();
    size_t len2 = s.length();

    if ( len1 < len2 )
        return -1;
    else if ( len1 > len2 )
        return 1;
    return 0;
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


    char *dest = buffer.data();

    const wchar_t *pwc = c_str();
    for ( ;; )
    {
        *dest++ = (char)(*pwc > SCHAR_MAX ? wxT('_') : *pwc);

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
        return wxEmptyString;
    }

    wxString dest(*this, nFirst, nCount);
    if ( dest.length() != nCount )
    {
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


// check that the string ends with suffix and return the rest of it in the
// provided pointer if it is not NULL, otherwise return false
bool wxString::EndsWith(const wxChar *suffix, wxString *rest) const
{
    wxASSERT_MSG( suffix, _T("invalid parameter in wxString::EndssWith") );

    int start = length() - wxStrlen(suffix);
    if ( start < 0 || wxStrcmp(wx_str() + start, suffix) != 0 )
        return false;

    if ( rest )
    {
        // put the rest of the string into provided pointer
        rest->assign(*this, 0, start);
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
wxString wxString::AfterLast(wxUniChar ch) const
{
  wxString str;
  int iPos = Find(ch, true);
  if ( iPos == wxNOT_FOUND )
    str = *this;
  else
    str = wx_str() + iPos + 1;

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
wxString wxString::BeforeFirst(wxUniChar ch) const
{
  int iPos = Find(ch);
  if ( iPos == wxNOT_FOUND ) iPos = length();
  return wxString(*this, 0, iPos);
}

/// get all characters before the last occurence of ch
/// (returns empty string if ch not found)
wxString wxString::BeforeLast(wxUniChar ch) const
{
  wxString str;
  int iPos = Find(ch, true);
  if ( iPos != wxNOT_FOUND && iPos != 0 )
    str = wxString(c_str(), iPos);

  return str;
}

/// get all characters after the first occurence of ch
/// (returns empty string if ch not found)
wxString wxString::AfterFirst(wxUniChar ch) const
{
  wxString str;
  int iPos = Find(ch);
  if ( iPos != wxNOT_FOUND )
    str = wx_str() + iPos + 1;

  return str;
}

// replace first (or all) occurences of some substring with another one
size_t wxString::Replace(const wxString& strOld,
                         const wxString& strNew, bool bReplaceAll)
{
    // if we tried to replace an empty string we'd enter an infinite loop below
    wxCHECK_MSG( !strOld.empty(), 0,
                 _T("wxString::Replace(): invalid parameter") );

    size_t uiCount = 0;   // count of replacements made

    size_t uiOldLen = strOld.length();
    size_t uiNewLen = strNew.length();

    size_t dwPos = 0;

    while ( (*this)[dwPos] != wxT('\0') )
    {
        //DO NOT USE STRSTR HERE
        //this string can contain embedded null characters,
        //so strstr will function incorrectly
        dwPos = find(strOld, dwPos);
        if ( dwPos == npos )
            break;                  // exit the loop
        else
        {
            //replace this occurance of the old string with the new one
            replace(dwPos, uiOldLen, strNew, uiNewLen);

            //move up pos past the string that was replaced
            dwPos += uiNewLen;

            //increase replace count
            ++uiCount;

            // stop now?
            if ( !bReplaceAll )
                break;                  // exit the loop
        }
    }

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
    if ( !empty() &&
         (
          (bFromRight && wxSafeIsspace(GetChar(length() - 1))) ||
          (!bFromRight && wxSafeIsspace(GetChar(0u)))
         )
       )
    {
        if ( bFromRight )
        {
            // find last non-space character
            reverse_iterator psz = rbegin();
            while ( (psz != rend()) && wxSafeIsspace(*psz) )
                psz++;

            // truncate at trailing space start
            erase(psz.base(), end());
        }
        else
        {
            // find first non-space character
            iterator psz = begin();
            while ( (psz != end()) && wxSafeIsspace(*psz) )
                psz++;

            // fix up data and length
            erase(begin(), psz);
        }
    }

    return *this;
}

// adds nCount characters chPad to the string from either side
wxString& wxString::Pad(size_t nCount, wxUniChar chPad, bool bFromRight)
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
    if ( uiLen < length() )
    {
        erase(begin() + uiLen, end());
    }
    //else: nothing to do, string is already short enough

    return *this;
}

// ---------------------------------------------------------------------------
// finding (return wxNOT_FOUND if not found and index otherwise)
// ---------------------------------------------------------------------------

// find a character
int wxString::Find(wxUniChar ch, bool bFromEnd) const
{
    size_type idx = bFromEnd ? find_last_of(ch) : find_first_of(ch);

    return (idx == npos) ? wxNOT_FOUND : (int)idx;
}

// ----------------------------------------------------------------------------
// conversion to numbers
// ----------------------------------------------------------------------------

// the implementation of all the functions below is exactly the same so factor
// it out

template <typename T, typename F>
bool wxStringToIntType(const wxChar *start,
                       T *val,
                       int base,
                       F func)
{
    wxCHECK_MSG( val, false, _T("NULL output pointer") );
    wxASSERT_MSG( !base || (base > 1 && base <= 36), _T("invalid base") );

#ifndef __WXWINCE__
    errno = 0;
#endif

    wxChar *end;
    *val = (*func)(start, &end, base);

    // return true only if scan was stopped by the terminating NUL and if the
    // string was not empty to start with and no under/overflow occurred
    return !*end && (end != start)
#ifndef __WXWINCE__
        && (errno != ERANGE)
#endif
    ;
}

bool wxString::ToLong(long *val, int base) const
{
    return wxStringToIntType((const wxChar*)c_str(), val, base, wxStrtol);
}

bool wxString::ToULong(unsigned long *val, int base) const
{
    return wxStringToIntType((const wxChar*)c_str(), val, base, wxStrtoul);
}

bool wxString::ToLongLong(wxLongLong_t *val, int base) const
{
#ifdef wxHAS_STRTOLL
    return wxStringToIntType((const wxChar*)c_str(), val, base, wxStrtoll);
#else
    // TODO: implement this ourselves
    wxUnusedVar(val);
    wxUnusedVar(base);
    return false;
#endif // wxHAS_STRTOLL
}

bool wxString::ToULongLong(wxULongLong_t *val, int base) const
{
#ifdef wxHAS_STRTOLL
    return wxStringToIntType((const wxChar*)c_str(), val, base, wxStrtoull);
#else
    // TODO: implement this ourselves
    wxUnusedVar(val);
    wxUnusedVar(base);
    return false;
#endif
}

bool wxString::ToDouble(double *val) const
{
    wxCHECK_MSG( val, false, _T("NULL pointer in wxString::ToDouble") );

#ifndef __WXWINCE__
    errno = 0;
#endif

    const wxChar *start = c_str();
    wxChar *end;
    *val = wxStrtod(start, &end);

    // return true only if scan was stopped by the terminating NUL and if the
    // string was not empty to start with and no under/overflow occurred
    return !*end && (end != start)
#ifndef __WXWINCE__
        && (errno != ERANGE)
#endif
    ;
}

// ---------------------------------------------------------------------------
// formatted output
// ---------------------------------------------------------------------------

/* static */
#ifdef wxNEEDS_WXSTRING_PRINTF_MIXIN
wxString wxStringPrintfMixinBase::DoFormat(const wxChar *format, ...)
#else
wxString wxString::DoFormat(const wxChar *format, ...)
#endif
{
    va_list argptr;
    va_start(argptr, format);

    wxString s;
    s.PrintfV(format, argptr);

    va_end(argptr);

    return s;
}

/* static */
wxString wxString::FormatV(const wxString& format, va_list argptr)
{
    wxString s;
    s.PrintfV(format, argptr);
    return s;
}

#ifdef wxNEEDS_WXSTRING_PRINTF_MIXIN
int wxStringPrintfMixinBase::DoPrintf(const wxChar *format, ...)
#else
int wxString::DoPrintf(const wxChar *format, ...)
#endif
{
    va_list argptr;
    va_start(argptr, format);

#ifdef wxNEEDS_WXSTRING_PRINTF_MIXIN
    // get a pointer to the wxString instance; we have to use dynamic_cast<>
    // because it's the only cast that works safely for downcasting when
    // multiple inheritance is used:
    wxString *str = static_cast<wxString*>(this);
#else
    wxString *str = this;
#endif

    int iLen = str->PrintfV(format, argptr);

    va_end(argptr);

    return iLen;
}

int wxString::PrintfV(const wxString& format, va_list argptr)
{
    int size = 1024;

    for ( ;; )
    {
        wxStringBuffer tmp(*this, size + 1);
        wxChar *buf = tmp;

        if ( !buf )
        {
            // out of memory
            return -1;
        }

        // wxVsnprintf() may modify the original arg pointer, so pass it
        // only a copy
        va_list argptrcopy;
        wxVaCopy(argptrcopy, argptr);
        int len = wxVsnprintf(buf, size, format, argptrcopy);
        va_end(argptrcopy);

        // some implementations of vsnprintf() don't NUL terminate
        // the string if there is not enough space for it so
        // always do it manually
        buf[size] = _T('\0');

        // vsnprintf() may return either -1 (traditional Unix behaviour) or the
        // total number of characters which would have been written if the
        // buffer were large enough (newer standards such as Unix98)
        if ( len < 0 )
        {
#if wxUSE_WXVSNPRINTF
            // we know that our own implementation of wxVsnprintf() returns -1
            // only for a format error - thus there's something wrong with
            // the user's format string
            return -1;
#else // assume that system version only returns error if not enough space
            // still not enough, as we don't know how much we need, double the
            // current size of the buffer
            size *= 2;
#endif // wxUSE_WXVSNPRINTF/!wxUSE_WXVSNPRINTF
        }
        else if ( len >= size )
        {
#if wxUSE_WXVSNPRINTF
            // we know that our own implementation of wxVsnprintf() returns 
            // size+1 when there's not enough space but that's not the size
            // of the required buffer!
            size *= 2;      // so we just double the current size of the buffer
#else
            // some vsnprintf() implementations NUL-terminate the buffer and
            // some don't in len == size case, to be safe always add 1
            size = len + 1;
#endif
        }
        else // ok, there was enough space
        {
            break;
        }
    }

    // we could have overshot
    Shrink();

    return length();
}

// ----------------------------------------------------------------------------
// misc other operations
// ----------------------------------------------------------------------------

// returns true if the string matches the pattern which may contain '*' and
// '?' metacharacters (as usual, '?' matches any character and '*' any number
// of them)
bool wxString::Matches(const wxString& mask) const
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

  // FIXME-UTF8: implement using iterators, remove #if
#if wxUSE_UNICODE_UTF8
  wxWCharBuffer maskBuf = mask.wc_str();
  wxWCharBuffer txtBuf = wc_str();
  const wxChar *pszMask = maskBuf.data();
  const wxChar *pszTxt = txtBuf.data();
#else
  const wxChar *pszMask = mask.wx_str();
  // the char currently being checked
  const wxChar *pszTxt = wx_str();
#endif

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
int wxString::Freq(wxUniChar ch) const
{
    int count = 0;
    for ( const_iterator i = begin(); i != end(); ++i )
    {
        if ( *i == ch )
            count ++;
    }
    return count;
}

// convert to upper case, return the copy of the string
wxString wxString::Upper() const
{ wxString s(*this); return s.MakeUpper(); }

// convert to lower case, return the copy of the string
wxString wxString::Lower() const { wxString s(*this); return s.MakeLower(); }
