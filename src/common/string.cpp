/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/string.cpp
// Purpose:     wxString class
// Author:      Vadim Zeitlin, Ryan Norton
// Created:     29/01/98
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
//              (c) 2004 Ryan Norton <wxprojects@comcast.net>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// headers, declarations, constants
// ===========================================================================

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/wxcrtvararg.h"
    #include "wx/log.h"
#endif

#include <ctype.h>

#include <errno.h>

#include <string.h>
#include <stdlib.h>

#include "wx/uilocale.h"
#include "wx/vector.h"
#include "wx/xlocale.h"

#ifdef __WINDOWS__
    #include "wx/msw/wrapwin.h"
#endif // __WINDOWS__

#if wxUSE_STD_IOSTREAM
    #include <sstream>
#endif

// define a function declared in wx/buffer.h here as we don't have buffer.cpp
// and don't want to add it just because of this simple function
namespace wxPrivate
{

// wxXXXBuffer classes can be (implicitly) used during global statics
// initialization so wrap the status UntypedBufferData variable in a function
// to make it safe to access it even before all global statics are initialized
UntypedBufferData *GetUntypedNullData()
{
    static UntypedBufferData s_untypedNullData(nullptr, 0);

    return &s_untypedNullData;
}

} // namespace wxPrivate

// ---------------------------------------------------------------------------
// static class variables definition
// ---------------------------------------------------------------------------

//According to STL _must_ be a -1 size_t
const size_t wxString::npos = (size_t) -1;

// FIXME-UTF8: get rid of this, have only one wxEmptyString
#if wxUSE_UNICODE_UTF8
const wxStringCharType WXDLLIMPEXP_BASE *wxEmptyStringImpl = "";
#endif
const wxChar WXDLLIMPEXP_BASE *wxEmptyString = wxT("");
#if wxUSE_STRING_POS_CACHE

/* static */
wxString::Cache& wxString::GetCache()
{
    static wxTHREAD_SPECIFIC_DECL Cache s_cache;
    return s_cache;
}

// gdb seems to be unable to display thread-local variables correctly, at least
// not my 6.4.98 version under amd64, so provide this debugging helper to do it
#if wxDEBUG_LEVEL >= 2

struct wxStrCacheDumper
{
    static void ShowAll()
    {
        puts("*** wxString cache dump:");
        for ( unsigned n = 0; n < wxString::Cache::SIZE; n++ )
        {
            const wxString::Cache::Element&
                c = wxString::GetCacheBegin()[n];

            printf("\t%u%s\t%p: pos=(%lu, %lu), len=%ld\n",
                   n,
                   n == wxString::LastUsedCacheElement() ? " [*]" : "",
                   c.str,
                   (unsigned long)c.pos,
                   (unsigned long)c.impl,
                   (long)c.len);
        }
    }
};

void wxDumpStrCache() { wxStrCacheDumper::ShowAll(); }

#endif // wxDEBUG_LEVEL >= 2

#ifdef wxPROFILE_STRING_CACHE

wxString::CacheStats wxString::ms_cacheStats;

struct wxStrCacheStatsDumper
{
    ~wxStrCacheStatsDumper()
    {
        const wxString::CacheStats& stats = wxString::ms_cacheStats;

        if ( stats.postot )
        {
            puts("*** wxString cache statistics:");
            printf("\tTotal non-trivial calls to PosToImpl(): %u\n",
                   stats.postot);
            printf("\tHits %u (of which %u not used) or %.2f%%\n",
                   stats.poshits,
                   stats.mishits,
                   100.*float(stats.poshits - stats.mishits)/stats.postot);
            printf("\tAverage position requested: %.2f\n",
                   float(stats.sumpos) / stats.postot);
            printf("\tAverage offset after cached hint: %.2f\n",
                   float(stats.sumofs) / stats.postot);
        }

        if ( stats.lentot )
        {
            printf("\tNumber of calls to length(): %u, hits=%.2f%%\n",
                   stats.lentot, 100.*float(stats.lenhits)/stats.lentot);
        }
    }
};

static wxStrCacheStatsDumper s_showCacheStats;

#endif // wxPROFILE_STRING_CACHE

#endif // wxUSE_STRING_POS_CACHE

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM

#include <iostream>

std::ostream& operator<<(std::ostream& os, const wxCStrData& str)
{
#if !wxUSE_UNICODE_UTF8
    return os << wxConvWhateverWorks.cWX2MB(str);
#else
    return os << str.AsInternal();
#endif
}

std::ostream& operator<<(std::ostream& os, const wxString& str)
{
    return os << str.c_str();
}

std::ostream& operator<<(std::ostream& os, const wxScopedCharBuffer& str)
{
    return os << str.data();
}

std::ostream& operator<<(std::ostream& os, const wxScopedWCharBuffer& str)
{
    // There is no way to write wide character data to std::ostream directly,
    // but we need to define this operator for compatibility, as we provided it
    // since basically always, even if it never worked correctly before. So do
    // the only reasonable thing and output it as UTF-8.
    return os << wxConvWhateverWorks.cWC2MB(str.data());
}

#if defined(HAVE_WOSTREAM)

std::wostream& operator<<(std::wostream& wos, const wxString& str)
{
    return wos << str.wc_str();
}

std::wostream& operator<<(std::wostream& wos, const wxCStrData& str)
{
    return wos << str.AsWChar();
}

std::wostream& operator<<(std::wostream& wos, const wxScopedWCharBuffer& str)
{
    return wos << str.data();
}

#endif  // defined(HAVE_WOSTREAM)

#endif // wxUSE_STD_IOSTREAM

// ===========================================================================
// wxString class core
// ===========================================================================

#if wxUSE_UNICODE_UTF8

void wxString::PosLenToImpl(size_t pos, size_t len,
                            size_t *implPos, size_t *implLen) const
{
    if ( pos == npos )
    {
        *implPos = npos;
    }
    else // have valid start position
    {
        const const_iterator b = GetIterForNthChar(pos);
        *implPos = wxStringImpl::const_iterator(b.impl()) - m_impl.begin();
        if ( len == npos )
        {
            *implLen = npos;
        }
        else // have valid length too
        {
            // we need to handle the case of length specifying a substring
            // going beyond the end of the string, just as std::string does
            const const_iterator e(end());
            const_iterator i(b);
            while ( len && i < e )
            {
                ++i;
                --len;
            }

            *implLen = i.impl() - b.impl();
        }
    }
}

#endif // wxUSE_UNICODE_UTF8

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
    // FIXME-UTF8: do this only if locale is not UTF8 if wxUSE_UNICODE_UTF8
    DeleteStringFromConversionCache(gs_stringsCharCache, this);
#if !wxUSE_UNICODE_WCHAR
    DeleteStringFromConversionCache(gs_stringsWCharCache, this);
#endif
}
#endif

// ===========================================================================
// wxString class core
// ===========================================================================

// ---------------------------------------------------------------------------
// construction and conversion
// ---------------------------------------------------------------------------

#if wxUSE_UNICODE_WCHAR
/* static */
wxString::SubstrBufFromMB wxString::ConvertStr(const char *psz, size_t nLength,
                                               const wxMBConv& conv)
{
    // anything to do?
    if ( !psz || nLength == 0 )
        return SubstrBufFromMB(wxWCharBuffer(L""), 0);

    if ( nLength == npos )
        nLength = wxNO_LEN;

    size_t wcLen;
    wxScopedWCharBuffer wcBuf(conv.cMB2WC(psz, nLength, &wcLen));
    if ( !wcLen )
        return SubstrBufFromMB(wxWCharBuffer(L""), 0);
    else
        return SubstrBufFromMB(wcBuf, wcLen);
}
#endif // wxUSE_UNICODE_WCHAR

#if wxUSE_UNICODE_UTF8
/* static */
wxString::SubstrBufFromMB wxString::ConvertStr(const char *psz, size_t nLength,
                                               const wxMBConv& conv)
{
    // anything to do?
    if ( !psz || nLength == 0 )
        return SubstrBufFromMB(wxCharBuffer(""), 0);

    // if psz is already in UTF-8, we don't have to do the roundtrip to
    // wchar_t* and back:
    if ( conv.IsUTF8() )
    {
        // we need to validate the input because UTF8 iterators assume valid
        // UTF-8 sequence and psz may be invalid:
        if ( wxStringOperations::IsValidUtf8String(psz, nLength) )
        {
            // we must pass the real string length to SubstrBufFromMB ctor
            if ( nLength == npos )
                nLength = psz ? strlen(psz) : 0;
            return SubstrBufFromMB(wxScopedCharBuffer::CreateNonOwned(psz, nLength),
                                   nLength);
        }
        // else: do the roundtrip through wchar_t*
    }

    if ( nLength == npos )
        nLength = wxNO_LEN;

    // first convert to wide string:
    size_t wcLen;
    wxScopedWCharBuffer wcBuf(conv.cMB2WC(psz, nLength, &wcLen));
    if ( !wcLen )
        return SubstrBufFromMB(wxCharBuffer(""), 0);

    // and then to UTF-8:
    SubstrBufFromMB buf(ConvertStr(wcBuf, wcLen, wxMBConvStrictUTF8()));
    // widechar -> UTF-8 conversion isn't supposed to ever fail:
    wxASSERT_MSG( buf.data, wxT("conversion to UTF-8 failed") );

    return buf;
}
#endif // wxUSE_UNICODE_UTF8

#if wxUSE_UNICODE_UTF8
/* static */
wxString::SubstrBufFromWC wxString::ConvertStr(const wchar_t *pwz, size_t nLength,
                                               const wxMBConv& conv)
{
    // anything to do?
    if ( !pwz || nLength == 0 )
        return SubstrBufFromWC(wxCharBuffer(""), 0);

    if ( nLength == npos )
        nLength = wxNO_LEN;

    size_t mbLen;
    wxScopedCharBuffer mbBuf(conv.cWC2MB(pwz, nLength, &mbLen));
    if ( !mbLen )
        return SubstrBufFromWC(wxCharBuffer(""), 0);
    else
        return SubstrBufFromWC(mbBuf, mbLen);
}
#endif // wxUSE_UNICODE_UTF8

// This std::string::c_str()-like method returns a wide char pointer to string
// contents. In wxUSE_UNICODE_WCHAR case it is trivial as it can simply return
// a pointer to the internal representation. Otherwise a conversion is required
// and it returns a temporary buffer.
//
// However for compatibility with c_str() and to avoid breaking existing code
// doing
//
//      for ( const wchar_t *p = s.wc_str(); *p; p++ )
//          ... use *p...
//
// we actually need to ensure that the returned buffer is _not_ temporary and
// so we use wxString::m_convertedToWChar to store the returned data
#if !wxUSE_UNICODE_WCHAR

const wchar_t *wxString::AsWChar(const wxMBConv& conv) const
{
    const char * const strMB = m_impl.c_str();
    const size_t lenMB = m_impl.length();

    // find out the size of the buffer needed
    const size_t lenWC = conv.ToWChar(nullptr, 0, strMB, lenMB);
    if ( lenWC == wxCONV_FAILED )
        return nullptr;

    // keep the same buffer if the string size didn't change: this is not only
    // an optimization but also ensure that code which modifies string
    // character by character (without changing its length) can continue to use
    // the pointer returned by a previous wc_str() call even after changing the
    // string

    // TODO-UTF8: we could check for ">" instead of "!=" here as this would
    //            allow to save on buffer reallocations but at the cost of
    //            consuming (even) more memory, we should benchmark this to
    //            determine if it's worth doing
    if ( !m_convertedToWChar.m_str || lenWC != m_convertedToWChar.m_len )
    {
        if ( !const_cast<wxString *>(this)->m_convertedToWChar.Extend(lenWC) )
            return nullptr;
    }

    // finally do convert
    m_convertedToWChar.m_str[lenWC] = L'\0';
    if ( conv.ToWChar(m_convertedToWChar.m_str, lenWC,
                      strMB, lenMB) == wxCONV_FAILED )
        return nullptr;

    return m_convertedToWChar.m_str;
}

#endif // !wxUSE_UNICODE_WCHAR


// Same thing for mb_str() which returns a normal char pointer to string
// contents: this always requires converting it to the specified encoding
// except if we need to convert to UTF-8 and this is what we already use
// internally.
const char *wxString::AsChar(const wxMBConv& conv) const
{
#if wxUSE_UNICODE_UTF8
    if ( conv.IsUTF8() )
        return m_impl.c_str();

    const wchar_t * const strWC = AsWChar(wxMBConvStrictUTF8());
    const size_t lenWC = m_convertedToWChar.m_len;
#else // wxUSE_UNICODE_WCHAR
    const wchar_t * const strWC = m_impl.c_str();
    const size_t lenWC = m_impl.length();
#endif // wxUSE_UNICODE_UTF8/wxUSE_UNICODE_WCHAR

    const size_t lenMB = conv.FromWChar(nullptr, 0, strWC, lenWC);
    if ( lenMB == wxCONV_FAILED )
        return nullptr;

    if ( !m_convertedToChar.m_str || lenMB != m_convertedToChar.m_len )
    {
        if ( !const_cast<wxString *>(this)->m_convertedToChar.Extend(lenMB) )
            return nullptr;
    }

    m_convertedToChar.m_str[lenMB] = '\0';
    if ( conv.FromWChar(m_convertedToChar.m_str, lenMB,
                        strWC, lenWC) == wxCONV_FAILED )
        return nullptr;

    return m_convertedToChar.m_str;
}

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
    wxString s = str1;
    s += str2;

    return s;
}

wxString operator+(const wxString& str, wxUniChar ch)
{
    wxString s = str;
    s += ch;

    return s;
}

wxString operator+(wxUniChar ch, const wxString& str)
{
    wxString s = ch;
    s += str;

    return s;
}

wxString operator+(const wxString& str, const char *psz)
{
    wxString s;
    if ( !s.Alloc(strlen(psz) + str.length()) ) {
        wxFAIL_MSG( wxT("out of memory in wxString::operator+") );
    }
    s += str;
    s += psz;

    return s;
}

wxString operator+(const wxString& str, const wchar_t *pwz)
{
    wxString s;
    if ( !s.Alloc(wxWcslen(pwz) + str.length()) ) {
        wxFAIL_MSG( wxT("out of memory in wxString::operator+") );
    }
    s += str;
    s += pwz;

    return s;
}

wxString operator+(const char *psz, const wxString& str)
{
    wxString s;
    if ( !s.Alloc(strlen(psz) + str.length()) ) {
        wxFAIL_MSG( wxT("out of memory in wxString::operator+") );
    }
    s = psz;
    s += str;

    return s;
}

wxString operator+(const wchar_t *pwz, const wxString& str)
{
    wxString s;
    if ( !s.Alloc(wxWcslen(pwz) + str.length()) ) {
        wxFAIL_MSG( wxT("out of memory in wxString::operator+") );
    }
    s = pwz;
    s += str;

    return s;
}

// ---------------------------------------------------------------------------
// string comparison
// ---------------------------------------------------------------------------

bool wxString::IsSameAs(wxUniChar c, bool compareWithCase) const
{
    return (length() == 1) && (compareWithCase ? GetChar(0u) == c
                               : wxToupper(GetChar(0u)) == wxToupper(c));
}

// NB: Comparison code works with UTF-8 encoded strings too, thanks to UTF-8's
//     design which allows us to sort strings in characters code point order by
//     sorting the byte sequence in byte values order (i.e. what strcmp() and
//     memcmp() do).

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

// ---------------------------------------------------------------------------
// find_{first,last}_[not]_of functions
// ---------------------------------------------------------------------------

#if wxUSE_UNICODE_UTF8

// NB: All these functions are implemented with the argument always being
//     wchar_t*, even in UTF-8 build in which the native string representation
//     is char*. This is because we couldn't use memchr() to determine if a
//     character is in a set encoded as UTF-8.

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
    wxASSERT_MSG( nStart <= length(),  wxT("invalid index") );

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
    wxASSERT_MSG( nStart <= length(),  wxT("invalid index") );

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
        wxASSERT_MSG( nStart <= len, wxT("invalid index") );
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
        wxASSERT_MSG( nStart <= len, wxT("invalid index") );
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
    wxASSERT_MSG( nStart <= length(),  wxT("invalid index") );

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
        wxASSERT_MSG( nStart <= len, wxT("invalid index") );
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
// build; below are implementations for the char* arguments
size_t wxString::find_first_of(const char* sz, size_t nStart) const
    { return find_first_of(wxConvLibc.cMB2WC(sz), nStart); }

size_t wxString::find_first_of(const char* sz, size_t nStart,
                               size_t n) const
    { return find_first_of(wxConvLibc.cMB2WC(sz, n, nullptr), nStart, n); }
size_t wxString::find_last_of(const char* sz, size_t nStart) const
    { return find_last_of(wxConvLibc.cMB2WC(sz), nStart); }
size_t wxString::find_last_of(const char* sz, size_t nStart,
                              size_t n) const
    { return find_last_of(wxConvLibc.cMB2WC(sz, n, nullptr), nStart, n); }
size_t wxString::find_first_not_of(const char* sz, size_t nStart) const
    { return find_first_not_of(wxConvLibc.cMB2WC(sz), nStart); }
size_t wxString::find_first_not_of(const char* sz, size_t nStart,
                                   size_t n) const
    { return find_first_not_of(wxConvLibc.cMB2WC(sz, n, nullptr), nStart, n); }
size_t wxString::find_last_not_of(const char* sz, size_t nStart) const
    { return find_last_not_of(wxConvLibc.cMB2WC(sz), nStart); }
size_t wxString::find_last_not_of(const char* sz, size_t nStart,
                                  size_t n) const
    { return find_last_not_of(wxConvLibc.cMB2WC(sz, n, nullptr), nStart, n); }

#endif // wxUSE_UNICODE_UTF8

// ===========================================================================
// other common string functions
// ===========================================================================

int wxString::CmpNoCase(const wxString& s) const
{
#if !wxUSE_UNICODE_UTF8
    // We compare NUL-delimited chunks of the strings inside the loop. We will
    // do as many iterations as there are embedded NULs in the string, i.e.
    // usually we will run it just once.

    typedef const wxStringImpl::value_type *pchar_type;
    const pchar_type thisBegin = m_impl.c_str();
    const pchar_type thatBegin = s.m_impl.c_str();

    const pchar_type thisEnd = thisBegin + m_impl.length();
    const pchar_type thatEnd = thatBegin + s.m_impl.length();

    pchar_type thisCur = thisBegin;
    pchar_type thatCur = thatBegin;

    int rc;
    for ( ;; )
    {
        // Compare until the next NUL, if the strings differ this is the final
        // result.
        rc = wxStricmp(thisCur, thatCur);
        if ( rc )
            break;

        const size_t lenChunk = wxStrlen(thisCur);
        thisCur += lenChunk;
        thatCur += lenChunk;

        // Skip all the NULs as wxStricmp() doesn't handle them.
        for ( ; !*thisCur; thisCur++, thatCur++ )
        {
            // Check if we exhausted either of the strings.
            if ( thisCur == thisEnd )
            {
                // This one is exhausted, is the other one too?
                return thatCur == thatEnd ? 0 : -1;
            }

            if ( thatCur == thatEnd )
            {
                // Because of the test above we know that this one is not
                // exhausted yet so it's greater than the other one that is.
                return 1;
            }

            if ( *thatCur )
            {
                // Anything non-NUL is greater than NUL.
                return -1;
            }
        }
    }

    return rc;
#else // wxUSE_UNICODE_UTF8
    // CRT functions can't be used for case-insensitive comparison of UTF-8
    // strings so do it in the naive, simple and inefficient way.

    // FIXME-UTF8: use wxUniChar::ToLower/ToUpper once added
    const_iterator i1 = begin();
    const_iterator end1 = end();
    const_iterator i2 = s.begin();
    const_iterator end2 = s.end();

    for ( ; i1 != end1 && i2 != end2; ++i1, ++i2 )
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
#endif // !wxUSE_UNICODE_UTF8/wxUSE_UNICODE_UTF8
}


wxString wxString::FromAscii(const char *ascii, size_t len)
{
    wxString res;

    if (!ascii || len == 0)
       return res;

    {
        wxStringInternalBuffer buf(res, len);
        wxStringCharType *dest = buf;

        for ( ; len > 0; --len )
        {
            unsigned char c = (unsigned char)*ascii++;
            wxASSERT_MSG( c < 0x80,
                          wxT("Non-ASCII value passed to FromAscii().") );

            *dest++ = static_cast<wxStringCharType>(c);
        }
    }

    return res;
}

wxString wxString::FromAscii(const char *ascii)
{
    return FromAscii(ascii, wxStrlen(ascii));
}

wxString wxString::FromAscii(char ascii)
{
    // What do we do with '\0' ?

    unsigned char c = (unsigned char)ascii;

    wxASSERT_MSG( c < 0x80, wxT("Non-ASCII value passed to FromAscii().") );

    // NB: the cast to wchar_t causes interpretation of 'ascii' as Latin1 value
    return wxString(wxUniChar((wchar_t)c));
}

const wxScopedCharBuffer wxString::ToAscii(char replaceWith) const
{
    // this will allocate enough space for the terminating NUL too
    wxCharBuffer buffer(length());
    char *dest = buffer.data();

    for ( const_iterator i = begin(); i != end(); ++i )
    {
        wxUniChar c(*i);
        // FIXME-UTF8: unify substituted char ('_') with wxUniChar ('?')
        *dest++ = c.IsAscii() ? (char)c : replaceWith;

        // the output string can't have embedded NULs anyhow, so we can safely
        // stop at first of them even if we do have any
        if ( !c )
            break;
    }

    return buffer;
}

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
    if ( nFirst > nLen )
    {
        // AllocCopy() will return empty string
        return wxEmptyString;
    }

    if ( nCount > nLen - nFirst )
    {
        nCount = nLen - nFirst;
    }

    wxString dest(*this, nFirst, nCount);
    if ( dest.length() != nCount )
    {
        wxFAIL_MSG( wxT("out of memory in wxString::Mid") );
    }

    return dest;
}

// check that the string starts with prefix and return the rest of the string
// in the provided pointer if it is not null, otherwise return false
bool wxString::StartsWith(const wxString& prefix, wxString *rest) const
{
    if ( compare(0, prefix.length(), prefix) != 0 )
        return false;

    if ( rest )
    {
        // put the rest of the string into provided pointer
        rest->assign(*this, prefix.length(), npos);
    }

    return true;
}


// check that the string ends with suffix and return the rest of it in the
// provided pointer if it is not null, otherwise return false
bool wxString::EndsWith(const wxString& suffix, wxString *rest) const
{
    int start = length() - suffix.length();

    if ( start < 0 || compare(start, npos, suffix) != 0 )
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
    wxFAIL_MSG( wxT("out of memory in wxString::Right") );
  }
  return dest;
}

// get all characters after the last occurrence of ch
// (returns the whole string if ch not found)
wxString wxString::AfterLast(wxUniChar ch) const
{
  wxString str;
  int iPos = Find(ch, true);
  if ( iPos == wxNOT_FOUND )
    str = *this;
  else
    str.assign(*this, iPos + 1, npos);

  return str;
}

// extract nCount first (leftmost) characters
wxString wxString::Left(size_t nCount) const
{
  if ( nCount > length() )
    nCount = length();

  wxString dest(*this, 0, nCount);
  if ( dest.length() != nCount ) {
    wxFAIL_MSG( wxT("out of memory in wxString::Left") );
  }
  return dest;
}

// get all characters before the first occurrence of ch
// (returns the whole string if ch not found)
wxString wxString::BeforeFirst(wxUniChar ch, wxString *rest) const
{
  int iPos = Find(ch);
  if ( iPos == wxNOT_FOUND )
  {
    iPos = length();
    if ( rest )
      rest->clear();
  }
  else
  {
    if ( rest )
      rest->assign(*this, iPos + 1, npos);
  }

  return wxString(*this, 0, iPos);
}

/// get all characters before the last occurrence of ch
/// (returns empty string if ch not found)
wxString wxString::BeforeLast(wxUniChar ch, wxString *rest) const
{
  wxString str;
  int iPos = Find(ch, true);
  if ( iPos != wxNOT_FOUND )
  {
    if ( iPos != 0 )
      str.assign(*this, 0, iPos);

    if ( rest )
      rest->assign(*this, iPos + 1, npos);
  }
  else
  {
    if ( rest )
      *rest = *this;
  }

  return str;
}

/// get all characters after the first occurrence of ch
/// (returns empty string if ch not found)
wxString wxString::AfterFirst(wxUniChar ch) const
{
  wxString str;
  int iPos = Find(ch);
  if ( iPos != wxNOT_FOUND )
      str.assign(*this, iPos + 1, npos);

  return str;
}

// replace first (or all) occurrences of some substring with another one
size_t wxString::Replace(const wxString& strOld,
                         const wxString& strNew, bool bReplaceAll)
{
    // if we tried to replace an empty string we'd enter an infinite loop below
    wxCHECK_MSG( !strOld.empty(), 0,
                 wxT("wxString::Replace(): invalid parameter") );

    wxSTRING_INVALIDATE_CACHE();

    size_t uiCount = 0;   // count of replacements made

    // optimize the special common case: replacement of one character by
    // another one (in UTF-8 case we can only do this for ASCII characters)
    //
    // benchmarks show that this special version is around 3 times faster
    // (depending on the proportion of matching characters and UTF-8/wchar_t
    // build)
    if ( strOld.m_impl.length() == 1 && strNew.m_impl.length() == 1 )
    {
        const wxStringCharType chOld = strOld.m_impl[0],
                               chNew = strNew.m_impl[0];

        // this loop is the simplified version of the one below
        for ( size_t pos = 0; ; )
        {
            pos = m_impl.find(chOld, pos);
            if ( pos == npos )
                break;

            m_impl[pos++] = chNew;

            uiCount++;

            if ( !bReplaceAll )
                break;
        }
    }
    else if ( !bReplaceAll)
    {
        size_t pos = m_impl.find(strOld.m_impl, 0);
        if ( pos != npos )
        {
            m_impl.replace(pos, strOld.m_impl.length(), strNew.m_impl);
            uiCount = 1;
        }
    }
    else // replace all occurrences
    {
        const size_t uiOldLen = strOld.m_impl.length();
        const size_t uiNewLen = strNew.m_impl.length();

        // first scan the string to find all positions at which the replacement
        // should be made
        wxVector<size_t> replacePositions;

        size_t pos;
        for ( pos = m_impl.find(strOld.m_impl, 0);
              pos != npos;
              pos = m_impl.find(strOld.m_impl, pos + uiOldLen))
        {
            replacePositions.push_back(pos);
            ++uiCount;
        }

        if ( !uiCount )
            return 0;

        // allocate enough memory for the whole new string
        wxString tmp;
        tmp.m_impl.reserve(m_impl.length() + uiCount*(uiNewLen - uiOldLen));

        // copy this string to tmp doing replacements on the fly
        size_t replNum = 0;
        for ( pos = 0; replNum < uiCount; replNum++ )
        {
            const size_t nextReplPos = replacePositions[replNum];

            if ( pos != nextReplPos )
            {
                tmp.m_impl.append(m_impl, pos, nextReplPos - pos);
            }

            tmp.m_impl.append(strNew.m_impl);
            pos = nextReplPos + uiOldLen;
        }

        if ( pos != m_impl.length() )
        {
            // append the rest of the string unchanged
            tmp.m_impl.append(m_impl, pos, m_impl.length() - pos);
        }

        swap(tmp);
    }

    return uiCount;
}

bool wxString::IsAscii() const
{
    for ( const_iterator i = begin(); i != end(); ++i )
    {
        if ( !(*i).IsAscii() )
            return false;
    }

    return true;
}

bool wxString::IsWord() const
{
    for ( const_iterator i = begin(); i != end(); ++i )
    {
        if ( !wxIsalpha(*i) )
            return false;
    }

    return true;
}

bool wxString::IsNumber() const
{
    if ( empty() )
        return true;

    const_iterator i = begin();

    if ( *i == wxT('-') || *i == wxT('+') )
        ++i;

    for ( ; i != end(); ++i )
    {
        if ( !wxIsdigit(*i) )
            return false;
    }

    return true;
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

wxString& wxString::MakeCapitalized()
{
    const iterator en = end();
    iterator it = begin();
    if ( it != en )
    {
        *it = (wxChar)wxToupper(*it);
        for ( ++it; it != en; ++it )
            *it = (wxChar)wxTolower(*it);
    }

    return *this;
}

// ---------------------------------------------------------------------------
// trimming and padding
// ---------------------------------------------------------------------------

// some compilers (VC++ 6.0 not to name them) return true for a call to
// isspace('\xEA') in the C locale which seems to be broken to me, but we have
// to live with this by checking that the character is a 7 bit one - even if
// this may fail to detect some spaces (I don't know if Unicode doesn't have
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
                ++psz;

            // truncate at trailing space start
            erase(psz.base(), end());
        }
        else
        {
            // find first non-space character
            iterator psz = begin();
            while ( (psz != end()) && wxSafeIsspace(*psz) )
                ++psz;

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
        wxSTRING_SET_CACHED_LENGTH(uiLen);
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

namespace
{

// Tiny helper to preserve errno: it's used in the functions below because it
// would be unexpected if they changed errno, especially in the case of
// wxString::Format() that might be used when reporting errors.
class PreserveErrno
{
public:
    PreserveErrno() : m_errnoOrig{errno} {}
    ~PreserveErrno() { errno = m_errnoOrig; }

private:
    const int m_errnoOrig;

    wxDECLARE_NO_COPY_CLASS(PreserveErrno);
};

// The implementation of all the functions below is exactly the same so factor
// it out in this template helper taking the function to call to actually
// perform the conversion to some larger type R and a function to check that
// the conversion result is in the correct range for the type T.
//
// Note that number extraction works correctly on UTF-8 strings, so
// we can use wxStringCharType and wx_str() for maximum efficiency.
template <typename R, typename T>
bool
ToNumeric(T* pVal,
          R (*convert)(const wxStringCharType*, wxStringCharType**, int),
          const wxStringCharType* start,
          int base,
          bool (*rangeCheck)(R))
{
    wxASSERT_MSG(!base || (base > 1 && base <= 36), wxT("invalid base"));
    wxCHECK_MSG( pVal, false, wxT("null output pointer") );

    PreserveErrno preserveErrno;
    errno = 0;

    wxStringCharType *end;

    const R res = convert(start, &end, base);
    if ( rangeCheck && !rangeCheck(res) )
        return false;

    // notice that we return false without modifying the output parameter at all if
    // nothing could be parsed but we do modify it and return false then if we did
    // parse something successfully but not the entire string
    if ( end == start || errno == ERANGE )
        return false;

    *pVal = static_cast<T>(res);
    return !*end;
}

// This is a simplified version which uses the conversion function returning
// the desired type directly, and hence no range checking function at all.
template <typename T>
bool
ToNumeric(T* pVal,
          T (*convert)(const wxStringCharType*, wxStringCharType**, int),
          const wxStringCharType* start,
          int base = 0)
{
    return ToNumeric<T>(pVal, convert, start, base, nullptr);
}

} // anonymous namespace

bool wxString::ToInt(int *pVal, int base) const
{
    return ToNumeric<wxLongLong_t>
           (
            pVal, wxStrtoll, wx_str(), base,
            [](wxLongLong_t val) { return val >= INT_MIN && val <= INT_MAX; }
           );
}

bool wxString::ToUInt(unsigned int *pVal, int base) const
{
    return ToNumeric<wxULongLong_t>
           (
            pVal, wxStrtoull, wx_str(), base,
            [](wxULongLong_t val) { return val <= UINT_MAX; }
           );
}

bool wxString::ToLong(long *pVal, int base) const
{
    return ToNumeric(pVal, wxStrtol, wx_str(), base);
}

bool wxString::ToULong(unsigned long *pVal, int base) const
{
    return ToNumeric(pVal, wxStrtoul, wx_str(), base);
}

bool wxString::ToLongLong(wxLongLong_t *pVal, int base) const
{
    return ToNumeric(pVal, wxStrtoll, wx_str(), base);
}

bool wxString::ToULongLong(wxULongLong_t *pVal, int base) const
{
    return ToNumeric(pVal, wxStrtoull, wx_str(), base);
}

bool wxString::ToDouble(double *pVal) const
{
    // Use a hack to allow calling wxStrtod() with an unused "base" parameter
    // for consistency with the other functions.
    return ToNumeric<double>
           (
            pVal,
            [](const wxStringCharType* start, wxStringCharType** endptr, int)
            {
                return wxStrtod(start, endptr);
            },
            wx_str()
           );
}

// There are several possibilities for implementing the conversion functions
// always using "C" locale:
//
//  1. Preferred one: use C++17 <charconv>, this is the fastest way to do it.
//  2. Use <xlocale.h> if it's available.
//  3. Use standard locale-dependent C functions and adjust them for the
//     current locale (slowest and the least robust).

// Check if C++17 <charconv> is available: even though normally it should be
// available in any compiler claiming C++17 support, there are actually some
// compilers (e.g. gcc 7) that don't have it, so do it in this way instead:
#if wxHAS_CXX17_INCLUDE(<charconv>)
    // This should define __cpp_lib_to_chars checked below.
    #include <charconv>
#endif

// Now check if the functions we need are present in it (normally they ought
// to if the compiler claims to support C++17, but it doesn't hurt to check).
#ifdef __cpp_lib_to_chars

namespace
{

// Helper of ToCLong() and ToCULong() taking care of prefix and base-related
// stuff: because from_chars() doesn't skip leading whitespace and doesn't
// recognize base==0 nor "0x" prefix even if base 16 is explicitly specified,
// we need to skip the leading space and prefix indicating the base to use if
// it's present and adjust "base" itself instead.
//
// Return false if base is already specified but is incompatible with the
// prefix used.
bool SkipOptPrefixAndSetBase(int& base, const char*& start, const char* end)
{
    // Start by skipping whitespace.
    while ( wxSafeIsspace(*start) )
        ++start;

    // Also skip optional "+" which std::from_chars() doesn't accept either.
    if ( *start == '+' )
        ++start;

    // Then check for the base prefix.
    if ( end - start > 1 && *start == '0' )
    {
        ++start;
        if ( *start == 'x' || *start == 'X' )
        {
            ++start;
            if ( base == 0 )
                base = 16;
            else if ( base != 16 )
                return false;
        }
        else
        {
            if ( base == 0 )
                base = 8;
        }
    }

    if ( base == 0 )
        base = 10;

    return true;
}

} // anonymous namespace

bool wxString::ToCLong(long *pVal, int base) const
{
    wxCHECK_MSG( pVal, false, "null output pointer" );

    const wxScopedCharBuffer& buf = utf8_str();
    auto start = buf.data();
    const auto end = start + buf.length();

    if ( !SkipOptPrefixAndSetBase(base, start, end) )
        return false;

    const auto res = std::from_chars(start, end, *pVal, base);

    return res.ec == std::errc{} && res.ptr == end;
}

bool wxString::ToCULong(unsigned long *pVal, int base) const
{
    wxCHECK_MSG( pVal, false, "null output pointer" );

    const wxScopedCharBuffer& buf = utf8_str();
    auto start = buf.data();
    const auto end = start + buf.length();

    if ( !SkipOptPrefixAndSetBase(base, start, end) )
        return false;

    // Extra complication: for compatibility reasons, this function does accept
    // "-1" as valid input (as strtoul() does!), but from_chars() doesn't, for
    // unsigned values, so check for this separately.
    if ( *start == '-' )
    {
        long l;
        const auto res = std::from_chars(start, end, l, base);

        if ( res.ec != std::errc{} || res.ptr != end )
            return false;

        *pVal = static_cast<unsigned long>(l);

        return true;
    }

    const auto res = std::from_chars(start, end, *pVal, base);

    return res.ec == std::errc{} && res.ptr == end;
}

bool wxString::ToCDouble(double *pVal) const
{
    wxCHECK_MSG( pVal, false, "null output pointer" );

    const wxScopedCharBuffer& buf = utf8_str();
    auto start = buf.data();
    const auto end = start + buf.length();

    // Retain compatibility with the strtod() function by allowing starting spaces
    // and a leading + sign, which from_chars() does not accept.
    int base = 0;
    SkipOptPrefixAndSetBase(base, start, end);

    std::chars_format flags = std::chars_format::general;

    if ( base == 16 )
        flags = std::chars_format::hex;

    const auto res = std::from_chars(start, end, *pVal, flags);

    return res.ec == std::errc{} && res.ptr == end;
}

wxString wxString::FromCDouble(double val, int precision)
{
    wxCHECK_MSG( precision >= -1, wxString(), "Invalid negative precision" );

    // 64 digits is more than enough for any double.
    char buf[64];
    const auto start = buf;
    const auto end = buf + sizeof(buf);

    std::to_chars_result res;

    // Note that we must explicitly specify the precision to remain compatible
    // with the behaviour of sprintf("%g"): by default, the result would be the
    // shortest string avoiding precision loss, but "%g" is supposed to
    // truncate, so use its default precision explicitly to achieve this here.
    if ( precision == -1 )
        res = std::to_chars(start, end, val, std::chars_format::general, 6);
    else
        res = std::to_chars(start, end, val, std::chars_format::fixed, precision);

    if ( res.ec != std::errc{} )
        return {};

    *res.ptr = '\0';

    return wxString::FromAscii(buf);
}

#elif wxUSE_XLOCALE

bool wxString::ToCLong(long *pVal, int base) const
{
    return ToNumeric<long>
           (
            pVal,
            [](const wxStringCharType* start, wxStringCharType** endptr, int base)
            {
#if wxUSE_UNICODE_UTF8 && defined(wxHAS_XLOCALE_SUPPORT)
                return wxStrtol_lA(start, endptr, base, wxCLocale);
#else
                return wxStrtol_l(start, endptr, base, wxCLocale);
#endif
            },
            wx_str(), base
           );
}

bool wxString::ToCULong(unsigned long *pVal, int base) const
{
    return ToNumeric<unsigned long>
           (
            pVal,
            [](const wxStringCharType* start, wxStringCharType** endptr, int base)
            {
#if wxUSE_UNICODE_UTF8 && defined(wxHAS_XLOCALE_SUPPORT)
                return wxStrtoul_lA(start, endptr, base, wxCLocale);
#else
                return wxStrtoul_l(start, endptr, base, wxCLocale);
#endif
            },
            wx_str(), base
           );
}

bool wxString::ToCDouble(double *pVal) const
{
    return ToNumeric<double>
           (
            pVal,
            [](const wxStringCharType* start, wxStringCharType** endptr, int)
            {
#if wxUSE_UNICODE_UTF8 && defined(wxHAS_XLOCALE_SUPPORT)
                return wxStrtod_lA(start, endptr, wxCLocale);
#else
                return wxStrtod_l(start, endptr, wxCLocale);
#endif
            },
            wx_str()
           );
}

#else // wxUSE_XLOCALE

// Provide implementation of these functions even when wxUSE_XLOCALE is
// disabled, we still need them in wxWidgets internal code.

// For integers we just assume the current locale uses the same number
// representation as the C one as there is nothing else we can do.
bool wxString::ToCLong(long *pVal, int base) const
{
    return ToLong(pVal, base);
}

bool wxString::ToCULong(unsigned long *pVal, int base) const
{
    return ToULong(pVal, base);
}

// For floating point numbers we have to handle the problem of the decimal
// point which is different in different locales.
bool wxString::ToCDouble(double *pVal) const
{
    // See the explanations in FromCDouble() below for the reasons for all this.

    // Try parsing using the current locale separator.
    if ( ToDouble(pVal) )
    {
        if ( find(',') != npos )
        {
            // Can't be a valid number in C locale.
            return false;
        }

        // Either current decimal separator is the point or this string doesn't
        // contain any decimal separator at all, in either case the result must
        // be correct and we don't have anything else to do.
        return true;
    }

    // Try to replace the separator with the only alternative value.
    const size_t posPeriod = find('.');
    if ( posPeriod == npos )
    {
        // No separator at all, so no need to retry with an alternative one.
        return false;
    }

    wxString cstr(*this);
    cstr[posPeriod] = ',';

    return cstr.ToDouble(pVal);
}

#endif  // wxUSE_XLOCALE/!wxUSE_XLOCALE

// ----------------------------------------------------------------------------
// number to string conversion
// ----------------------------------------------------------------------------

/* static */
wxString wxString::FromDouble(double val, int precision)
{
    wxCHECK_MSG( precision >= -1, wxString(), "Invalid negative precision" );

    wxString format;
    if ( precision == -1 )
    {
        format = "%g";
    }
    else // Use fixed precision.
    {
        format.Printf("%%.%df", precision);
    }

    return wxString::Format(format, val);
}

#ifndef __cpp_lib_to_chars

/* static */
wxString wxString::FromCDouble(double val, int precision)
{
    wxCHECK_MSG( precision >= -1, wxString(), "Invalid negative precision" );

    // Without std::to_chars() there is no portable way to get the number
    // directly in the C locale and while some platforms provide special
    // functions to do this (e.g. _sprintf_l() in MSVS or sprintf_l() in BSD
    // systems), some systems we still support don't have them, so just use
    // the hack below and replace any occurrences of a comma (which is the only
    // alternative decimal separator that can be really used) with a period.

    wxString s = FromDouble(val, precision);
    const size_t posComma = s.find(',');
    if ( posComma != npos )
        s[posComma] = '.';

    return s;
}

#endif // !__cpp_lib_to_chars

// ---------------------------------------------------------------------------
// formatted output
// ---------------------------------------------------------------------------

/* static */
wxString wxString::FormatV(const wxString& format, va_list argptr)
{
    wxString s;
    s.PrintfV(format, argptr);
    return s;
}

#if !wxUSE_UTF8_LOCALE_ONLY
int wxString::DoPrintfWchar(const wxChar *format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    int iLen = PrintfV(format, argptr);

    va_end(argptr);

    return iLen;
}
#endif // !wxUSE_UTF8_LOCALE_ONLY

#if wxUSE_UNICODE_UTF8
int wxString::DoPrintfUtf8(const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    int iLen = PrintfV(wxString::FromUTF8(format), argptr);

    va_end(argptr);

    return iLen;
}
#endif // wxUSE_UNICODE_UTF8

/*
    Uses wxVsnprintf and places the result into the this string.

    wxVsnprintf() is effectively vswprintf() and, according to ISO C99 (and
    thus SUSv3) standard, a conforming vswprintf implementations must
    just return a negative number and is not even required to set errno, which
    makes the standard behaviour totally useless as there is no way to
    determine if the error occurred due to a (fatal) problem with either the
    format string or the arguments or to a (correctable) problem with the
    buffer just not being big enough.

    In practice some implementations (including our own implementation of
    vsprintf(), if we use it) do set errno to EILSEQ or EINVAL.  Both of
    those are defined in the standard and backed up by several conformance
    statements.  Note that ENOMEM mentioned in the manual page does not
    apply to swprintf, only wprintf and fwprintf.

    Official manual page:
    http://www.opengroup.org/onlinepubs/009695399/functions/swprintf.html

    Some conformance statements (AIX, Solaris):
    http://www.opengroup.org/csq/view.mhtml?RID=ibm%2FSD1%2F3
    http://www.theopengroup.org/csq/view.mhtml?norationale=1&noreferences=1&RID=Fujitsu%2FSE2%2F10

    So we can check for these specific errno values to detect invalid format
    string or arguments. Unfortunately not all implementations set them and, in
    particular, glibc, use under Linux, never sets errno at all. This means
    that we have no choice but to try increasing the buffer size because we
    can't distinguish between the unrecoverable errors and buffer just being too
    small. Of course, continuing increasing the size forever will sooner or
    later result in out of memory error and crashing, so we also have to impose
    some arbitrary limit on it.
*/
#if wxUSE_UNICODE_UTF8
template<typename BufferType>
#else
// we only need one version in non-UTF8 builds and at least two Windows
// compilers have problems with this function template, so use just one
// normal function here
#endif
static int DoStringPrintfV(wxString& str,
                           const wxString& format, va_list argptr)
{
    size_t size = 1024;
    PreserveErrno preserveErrno;

    for ( ;; )
    {
#if wxUSE_UNICODE_UTF8
        BufferType tmp(str, size + 1);
        typename BufferType::CharType *buf = tmp;
#else
        wxStringBuffer tmp(str, size + 1);
        wxChar *buf = tmp;
#endif

        if ( !buf )
        {
            // out of memory
            return -1;
        }

        // wxVsnprintf() may modify the original arg pointer, so pass it
        // only a copy
        va_list argptrcopy;
        wxVaCopy(argptrcopy, argptr);

        // Set errno to 0 to make it determinate if wxVsnprintf fails to set it.
        errno = 0;
        int len = wxVsnprintf(buf, size, format, argptrcopy);
        va_end(argptrcopy);

        // some implementations of vsnprintf() don't NUL terminate
        // the string if there is not enough space for it so
        // always do it manually
        // FIXME: This really seems to be the wrong and would be an off-by-one
        // bug except the code above allocates an extra character.
        buf[size] = wxT('\0');

        // Handle all possible results that we can get depending on the build
        // options.
        if ( len < 0 )
        {
            // When vswprintf() returns an error, it can leave invalid bytes in
            // the buffer, e.g. using "%c" with an invalid character results in
            // U+FFFFFFFF in the buffer, which would trigger an assert when we
            // try to copy it back to wxString as UTF-8 in "tmp" buffer dtor,
            // so ensure we don't try to do it.
            buf[0] = L'\0';

            // assume it only returns error if there is not enough space, but
            // as we don't know how much we need, double the current size of
            // the buffer
            if( (errno == EILSEQ) || (errno == EINVAL) )
            {
                // If errno was set to one of the two well-known hard errors
                // then fail immediately to avoid an infinite loop.
                return -1;
            }

            // still not enough, as we don't know how much we need, double the
            // current size of the buffer -- unless it's already too big, as we
            // have to stop at some point to avoid running out of memory and
            // crashing or worse (e.g. triggering OOM killer and accidentally
            // killing some other innocent process)

            // The limit is completely arbitrary, it's supposed to be big
            // enough to never become a problem in practice, but not so big as
            // to result in out of memory crashes.
            static const size_t MAX_BUFFER_SIZE = 128*1024*1024;

            if ( size >= MAX_BUFFER_SIZE )
                return -1;

            // Note that doubling the size here will never overflow for size
            // less than the limit.
            size *= 2;
        }
        else if ( static_cast<size_t>(len) >= size )
        {
            // we got back the needed size, but it doesn't include space for
            // NUL, so add it ourselves
            size = len + 1;
        }
        else // ok, there was enough space
        {
            break;
        }
    }

    // we could have overshot
    str.Shrink();

    return str.length();
}

int wxString::PrintfV(const wxString& format, va_list argptr)
{
#if wxUSE_UTF8_LOCALE_ONLY
    return DoStringPrintfV<wxUTF8StringBuffer>(*this, format, argptr);
#else
    #if wxUSE_UNICODE_UTF8
    if ( wxLocaleIsUtf8 )
        return DoStringPrintfV<wxUTF8StringBuffer>(*this, format, argptr);
    else
        // wxChar* version
        return DoStringPrintfV<wxStringBuffer>(*this, format, argptr);
    #else
        return DoStringPrintfV(*this, format, argptr);
    #endif // UTF8/WCHAR
#endif
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

    pattern += wxT('^');
    while ( *pszMask )
    {
        switch ( *pszMask )
        {
            case wxT('?'):
                pattern += wxT('.');
                break;

            case wxT('*'):
                pattern += wxT(".*");
                break;

            case wxT('^'):
            case wxT('.'):
            case wxT('$'):
            case wxT('('):
            case wxT(')'):
            case wxT('|'):
            case wxT('+'):
            case wxT('\\'):
                // these characters are special in a RE, quote them
                // (however note that we don't quote '[' and ']' to allow
                // using them for Unix shell like matching)
                pattern += wxT('\\');
                wxFALLTHROUGH;

            default:
                pattern += *pszMask;
        }

        pszMask++;
    }
    pattern += wxT('$');

    // and now use it
    return wxRegEx(pattern, wxRE_NOSUB | wxRE_EXTENDED).Matches(c_str());
#else // !wxUSE_REGEX
  // TODO: this is, of course, awfully inefficient...

  // FIXME-UTF8: implement using iterators, remove #if
#if wxUSE_UNICODE_UTF8
  const wxScopedWCharBuffer maskBuf = mask.wc_str();
  const wxScopedWCharBuffer txtBuf = wc_str();
  const wxChar *pszMask = maskBuf.data();
  const wxChar *pszTxt = txtBuf.data();
#else
  const wxChar *pszMask = mask.wx_str();
  // the char currently being checked
  const wxChar *pszTxt = wx_str();
#endif

  // the last location where '*' matched
  const wxChar *pszLastStarInText = nullptr;
  const wxChar *pszLastStarInMask = nullptr;

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

          if ( pEndMask != nullptr ) {
            // we have to match the string between two metachars
            uiLenMask = pEndMask - pszMask;
          }
          else {
            // we have to match the remainder of the string
            uiLenMask = wxStrlen(pszMask);
          }

          wxString strToMatch(pszMask, uiLenMask);
          const wxChar* pMatch = wxStrstr(pszTxt, strToMatch);
          if ( pMatch == nullptr )
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

    pszLastStarInText = nullptr;

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

