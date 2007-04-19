///////////////////////////////////////////////////////////////////////////////
// Name:        wx/string.h
// Purpose:     wxString class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/*
    Efficient string class [more or less] compatible with MFC CString,
    wxWidgets version 1 wxString and std::string and some handy functions
    missing from string.h.
*/

#ifndef _WX_WXSTRING_H__
#define _WX_WXSTRING_H__

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"        // everybody should include this

#if defined(__WXMAC__) || defined(__VISAGECPP__)
    #include <ctype.h>
#endif

#if defined(__VISAGECPP__) && __IBMCPP__ >= 400
   // problem in VACPP V4 with including stdlib.h multiple times
   // strconv includes it anyway
#  include <stdio.h>
#  include <string.h>
#  include <stdarg.h>
#  include <limits.h>
#else
#  include <string.h>
#  include <stdio.h>
#  include <stdarg.h>
#  include <limits.h>
#  include <stdlib.h>
#endif

#ifdef HAVE_STRCASECMP_IN_STRINGS_H
    #include <strings.h>    // for strcasecmp()
#endif // HAVE_STRCASECMP_IN_STRINGS_H

#ifdef __WXPALMOS__
    #include <StringMgr.h>
#endif

#include "wx/wxchar.h"      // for wxChar, wxStrlen() etc.
#include "wx/strvararg.h"
#include "wx/buffer.h"      // for wxCharBuffer
#include "wx/strconv.h"     // for wxConvertXXX() macros and wxMBConv classes
#include "wx/stringimpl.h"
#include "wx/stringops.h"
#include "wx/unichar.h"

class WXDLLIMPEXP_BASE wxString;

// unless this symbol is predefined to disable the compatibility functions, do
// use them
#ifndef WXWIN_COMPATIBILITY_STRING_PTR_AS_ITER
    #define WXWIN_COMPATIBILITY_STRING_PTR_AS_ITER 1
#endif

// ---------------------------------------------------------------------------
// macros
// ---------------------------------------------------------------------------

// casts [unfortunately!] needed to call some broken functions which require
// "char *" instead of "const char *"
#define   WXSTRINGCAST (wxChar *)(const wxChar *)
#define   wxCSTRINGCAST (wxChar *)(const wxChar *)
#define   wxMBSTRINGCAST (char *)(const char *)
#define   wxWCSTRINGCAST (wchar_t *)(const wchar_t *)

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_2_6

// deprecated in favour of wxString::npos, don't use in new code
//
// maximum possible length for a string means "take all string" everywhere
#define wxSTRING_MAXLEN wxString::npos

#endif // WXWIN_COMPATIBILITY_2_6

// ---------------------------------------------------------------------------
// global functions complementing standard C string library replacements for
// strlen() and portable strcasecmp()
//---------------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_2_8
// Use wxXXX() functions from wxcrt.h instead! These functions are for
// backwards compatibility only.

// checks whether the passed in pointer is NULL and if the string is empty
wxDEPRECATED( inline bool IsEmpty(const char *p) );
inline bool IsEmpty(const char *p) { return (!p || !*p); }

// safe version of strlen() (returns 0 if passed NULL pointer)
wxDEPRECATED( inline size_t Strlen(const char *psz) );
inline size_t Strlen(const char *psz)
  { return psz ? strlen(psz) : 0; }

// portable strcasecmp/_stricmp
wxDEPRECATED( inline int Stricmp(const char *psz1, const char *psz2) );
inline int Stricmp(const char *psz1, const char *psz2)
{
#if defined(__VISUALC__) && defined(__WXWINCE__)
  register char c1, c2;
  do {
    c1 = tolower(*psz1++);
    c2 = tolower(*psz2++);
  } while ( c1 && (c1 == c2) );

  return c1 - c2;
#elif defined(__VISUALC__) || ( defined(__MWERKS__) && defined(__INTEL__) )
  return _stricmp(psz1, psz2);
#elif defined(__SC__)
  return _stricmp(psz1, psz2);
#elif defined(__SALFORDC__)
  return stricmp(psz1, psz2);
#elif defined(__BORLANDC__)
  return stricmp(psz1, psz2);
#elif defined(__WATCOMC__)
  return stricmp(psz1, psz2);
#elif defined(__DJGPP__)
  return stricmp(psz1, psz2);
#elif defined(__EMX__)
  return stricmp(psz1, psz2);
#elif defined(__WXPM__)
  return stricmp(psz1, psz2);
#elif defined(__WXPALMOS__) || \
      defined(HAVE_STRCASECMP_IN_STRING_H) || \
      defined(HAVE_STRCASECMP_IN_STRINGS_H) || \
      defined(__GNUWIN32__)
  return strcasecmp(psz1, psz2);
#elif defined(__MWERKS__) && !defined(__INTEL__)
  register char c1, c2;
  do {
    c1 = tolower(*psz1++);
    c2 = tolower(*psz2++);
  } while ( c1 && (c1 == c2) );

  return c1 - c2;
#else
  // almost all compilers/libraries provide this function (unfortunately under
  // different names), that's why we don't implement our own which will surely
  // be more efficient than this code (uncomment to use):
  /*
    register char c1, c2;
    do {
      c1 = tolower(*psz1++);
      c2 = tolower(*psz2++);
    } while ( c1 && (c1 == c2) );

    return c1 - c2;
  */

  #error  "Please define string case-insensitive compare for your OS/compiler"
#endif  // OS/compiler
}

#endif // WXWIN_COMPATIBILITY_2_8

// ----------------------------------------------------------------------------
// wxCStrData
// ----------------------------------------------------------------------------

// Lightweight object returned by wxString::c_str() and implicitly convertible
// to either const char* or const wchar_t*.
class WXDLLIMPEXP_BASE wxCStrData
{
private:
    // Ctors; for internal use by wxString and wxCStrData only
    wxCStrData(const wxString *str, size_t offset = 0, bool owned = false)
        : m_str(str), m_offset(offset), m_owned(owned) {}

public:
    // Ctor constructs the object from char literal; they are needed to make
    // operator?: compile and they intentionally take char*, not const char*
    wxCStrData(char *buf);
    wxCStrData(wchar_t *buf);

    inline ~wxCStrData();

    // methods defined inline below must be declared inline or mingw32 3.4.5
    // warns about "<symbol> defined locally after being referenced with
    // dllimport linkage"
#if wxUSE_UNICODE_WCHAR
    inline
#endif
    const wchar_t* AsWChar() const;
    operator const wchar_t*() const { return AsWChar(); }

    inline operator bool() const;

#if !wxUSE_UNICODE
    inline
#endif
    const char* AsChar() const;
    const unsigned char* AsUnsignedChar() const
        { return (const unsigned char *) AsChar(); }
    operator const char*() const { return AsChar(); }
    operator const unsigned char*() const { return AsUnsignedChar(); }

    operator const void*() const { return AsChar(); }

    inline const wxCharBuffer AsCharBuf() const;
    inline const wxWCharBuffer AsWCharBuf() const;

    inline wxString AsString() const;

    // allow expressions like "c_str()[0]":
    inline wxUniChar operator[](size_t n) const;
    wxUniChar operator[](int n) const { return operator[](size_t(n)); }
    wxUniChar operator[](long n) const { return operator[](size_t(n)); }
#ifndef wxSIZE_T_IS_UINT
    wxUniChar operator[](unsigned int n) const { return operator[](size_t(n)); }
#endif // size_t != unsigned int

    // these operators are needed to emulate the pointer semantics of c_str():
    // expressions like "wxChar *p = str.c_str() + 1;" should continue to work
    // (we need both versions to resolve ambiguities):
    wxCStrData operator+(int n) const
        { return wxCStrData(m_str, m_offset + n, m_owned); }
    wxCStrData operator+(long n) const
        { return wxCStrData(m_str, m_offset + n, m_owned); }
    wxCStrData operator+(size_t n) const
        { return wxCStrData(m_str, m_offset + n, m_owned); }

    // and these for "str.c_str() + n - 2":
    wxCStrData operator-(int n) const
    {
        wxASSERT_MSG( n <= (int)m_offset,
                      _T("attempt to construct address before the beginning of the string") );
        return wxCStrData(m_str, m_offset - n, m_owned);
    }
    wxCStrData operator-(long n) const
    {
        wxASSERT_MSG( n <= (int)m_offset,
                      _T("attempt to construct address before the beginning of the string") );
        return wxCStrData(m_str, m_offset - n, m_owned);
    }
    wxCStrData operator-(size_t n) const
    {
        wxASSERT_MSG( n <= m_offset,
                      _T("attempt to construct address before the beginning of the string") );
        return wxCStrData(m_str, m_offset - n, m_owned);
    }

    // this operator is needed to make expressions like "*c_str()" or
    // "*(c_str() + 2)" work
    inline wxUniChar operator*() const;

private:
    const wxString *m_str;
    size_t m_offset;
    bool m_owned;

    friend class WXDLLIMPEXP_BASE wxString;
};

// ----------------------------------------------------------------------------
// wxStringPrintfMixin
// ---------------------------------------------------------------------------

// NB: VC6 has a bug that causes linker errors if you have template methods
//     in a class using __declspec(dllimport). The solution is to split such
//     class into two classes, one that contains the template methods and does
//     *not* use WXDLLIMPEXP_BASE and another class that contains the rest
//     (with DLL linkage).
//
//     We only do this for VC6 here, because the code is less efficient
//     (Printf() has to use dynamic_cast<>) and because OpenWatcom compiler
//     cannot compile this code.

#if defined(__VISUALC__) && __VISUALC__ < 1300
    #define wxNEEDS_WXSTRING_PRINTF_MIXIN
#endif

#ifdef wxNEEDS_WXSTRING_PRINTF_MIXIN
// this class contains implementation of wxString's vararg methods, it's
// exported from wxBase DLL
class WXDLLIMPEXP_BASE wxStringPrintfMixinBase
{
protected:
    wxStringPrintfMixinBase() {}

    int DoPrintf(const wxChar *format, ...) ATTRIBUTE_PRINTF_2;
    static wxString DoFormat(const wxChar *format, ...) ATTRIBUTE_PRINTF_1;
};

// this class contains template wrappers for wxString's vararg methods, it's
// intentionally *not* exported from the DLL in order to fix the VC6 bug
// described above
class wxStringPrintfMixin : public wxStringPrintfMixinBase
{
private:
    // to further complicate things, we can't return wxString from
    // wxStringPrintfMixin::Format() because wxString is not yet declared at
    // this point; the solution is to use this fake type trait template - this
    // way the compiler won't know the return type until Format() is used
    // (this doesn't compile with Watcom, but VC6 compiles it just fine):
    template<typename T> struct StringReturnType
    {
        typedef wxString type;
    };

public:
    // these are duplicated wxString methods, they're also declared below
    // if !wxNEEDS_WXSTRING_PRINTF_MIXIN:

    // int Printf(const wxChar *pszFormat, ...);
    WX_DEFINE_VARARG_FUNC(int, Printf, DoPrintf)
    // static wxString Format(const wxChar *pszFormat, ...) ATTRIBUTE_PRINTF_1;
    WX_DEFINE_VARARG_FUNC(static typename StringReturnType<T1>::type,
                          Format, DoFormat)
    // int sprintf(const wxChar *pszFormat, ...) ATTRIBUTE_PRINTF_2;
    WX_DEFINE_VARARG_FUNC(int, sprintf, DoPrintf)

protected:
    wxStringPrintfMixin() : wxStringPrintfMixinBase() {}
};
#endif // wxNEEDS_WXSTRING_PRINTF_MIXIN


// ----------------------------------------------------------------------------
// wxString: string class trying to be compatible with std::string, MFC
//           CString and wxWindows 1.x wxString all at once
// ---------------------------------------------------------------------------

#ifdef wxNEEDS_WXSTRING_PRINTF_MIXIN
    // "non dll-interface class 'wxStringPrintfMixin' used as base interface
    // for dll-interface class 'wxString'" -- this is OK in our case
    #pragma warning (disable:4275)
#endif

class WXDLLIMPEXP_BASE wxString
#ifdef wxNEEDS_WXSTRING_PRINTF_MIXIN
                                : public wxStringPrintfMixin
#endif
{
  // NB: special care was taken in arranging the member functions in such order
  //     that all inline functions can be effectively inlined, verify that all
  //     performance critical functions are still inlined if you change order!
public:
  // an 'invalid' value for string index, moved to this place due to a CW bug
  static const size_t npos;

private:
  // if we hadn't made these operators private, it would be possible to
  // compile "wxString s; s = 17;" without any warnings as 17 is implicitly
  // converted to char in C and we do have operator=(char)
  //
  // NB: we don't need other versions (short/long and unsigned) as attempt
  //     to assign another numeric type to wxString will now result in
  //     ambiguity between operator=(char) and operator=(int)
  wxString& operator=(int);

  // these methods are not implemented - there is _no_ conversion from int to
  // string, you're doing something wrong if the compiler wants to call it!
  //
  // try `s << i' or `s.Printf("%d", i)' instead
  wxString(int);


  // buffer for holding temporary substring when using any of the methods
  // that take (char*,size_t) or (wchar_t*,size_t) arguments:
  template<typename T>
  struct SubstrBufFromType
  {
      T data;
      size_t len;

      SubstrBufFromType(const T& data_, size_t len_)
          : data(data_), len(len_) {}
  };

#if wxUSE_UNICODE_UTF8
  // even char* -> char* needs conversion, from locale charset to UTF-8
  typedef SubstrBufFromType<wxCharBuffer>    SubstrBufFromWC;
  typedef SubstrBufFromType<wxCharBuffer>    SubstrBufFromMB;
#elif wxUSE_UNICODE_WCHAR
  typedef SubstrBufFromType<const wchar_t*>  SubstrBufFromWC;
  typedef SubstrBufFromType<wxWCharBuffer>   SubstrBufFromMB;
#else
  typedef SubstrBufFromType<const char*>     SubstrBufFromMB;
  typedef SubstrBufFromType<wxCharBuffer>    SubstrBufFromWC;
#endif


  // Functions implementing primitive operations on string data; wxString
  // methods and iterators are implemented in terms of it. The differences
  // between UTF-8 and wchar_t* representations of the string are mostly
  // contained here.

#if wxUSE_UNICODE_UTF8
  static SubstrBufFromMB ConvertStr(const char *psz, size_t nLength,
                                    const wxMBConv& conv);
  static SubstrBufFromWC ConvertStr(const wchar_t *pwz, size_t nLength,
                                    const wxMBConv& conv);
#elif wxUSE_UNICODE_WCHAR
  static SubstrBufFromMB ConvertStr(const char *psz, size_t nLength,
                                    const wxMBConv& conv);
#else
  static SubstrBufFromWC ConvertStr(const wchar_t *pwz, size_t nLength,
                                    const wxMBConv& conv);
#endif

#if !wxUSE_UNICODE_UTF8 // wxUSE_UNICODE_WCHAR or !wxUSE_UNICODE
  // returns C string encoded as the implementation expects:
  #if wxUSE_UNICODE
  static const wchar_t* ImplStr(const wchar_t* str)
    { return str ? str : wxT(""); }
  static const SubstrBufFromWC ImplStr(const wchar_t* str, size_t n)
    { return SubstrBufFromWC(str, (str && n == npos) ? wxWcslen(str) : n); }
  static wxWCharBuffer ImplStr(const char* str,
                               const wxMBConv& conv = wxConvLibc)
    { return ConvertStr(str, npos, conv).data; }
  static SubstrBufFromMB ImplStr(const char* str, size_t n,
                                 const wxMBConv& conv = wxConvLibc)
    { return ConvertStr(str, n, conv); }
  #else
  static const char* ImplStr(const char* str,
                             const wxMBConv& WXUNUSED(conv) = wxConvLibc)
    { return str ? str : ""; }
  static const SubstrBufFromMB ImplStr(const char* str, size_t n,
                                       const wxMBConv& WXUNUSED(conv) = wxConvLibc)
    { return SubstrBufFromMB(str, (str && n == npos) ? wxStrlen(str) : n); }
  static wxCharBuffer ImplStr(const wchar_t* str)
    { return ConvertStr(str, npos, wxConvLibc).data; }
  static SubstrBufFromWC ImplStr(const wchar_t* str, size_t n)
    { return ConvertStr(str, n, wxConvLibc); }
  #endif

  // translates position index in wxString to/from index in underlying
  // wxStringImpl:
  static size_t PosToImpl(size_t pos) { return pos; }
  static void PosLenToImpl(size_t pos, size_t len,
                           size_t *implPos, size_t *implLen)
    { *implPos = pos; *implLen = len; }
  static size_t LenToImpl(size_t len) { return len; }
  static size_t PosFromImpl(size_t pos) { return pos; }

#else // wxUSE_UNICODE_UTF8

  // FIXME-UTF8: return as-is without copying under UTF8 locale, return
  //             converted string under other locales - needs wxCharBuffer
  //             changes
  static wxCharBuffer ImplStr(const char* str,
                              const wxMBConv& conv = wxConvLibc)
    { return ConvertStr(str, npos, conv).data; }
  static SubstrBufFromMB ImplStr(const char* str, size_t n,
                                 const wxMBConv& conv = wxConvLibc)
    { return ConvertStr(str, n, conv); }

  static wxCharBuffer ImplStr(const wchar_t* str)
    { return ConvertStr(str, npos, wxConvUTF8).data; }
  static SubstrBufFromWC ImplStr(const wchar_t* str, size_t n)
    { return ConvertStr(str, n, wxConvUTF8); }

  size_t PosToImpl(size_t pos) const
  {
      if ( pos == 0 || pos == npos )
          return pos;
      else
          return (begin() + pos).impl() - m_impl.begin();
  }

  void PosLenToImpl(size_t pos, size_t len, size_t *implPos, size_t *implLen) const;

  size_t LenToImpl(size_t len) const
  {
      size_t pos, len2;
      PosLenToImpl(0, len, &pos, &len2);
      return len2;
  }

  size_t PosFromImpl(size_t pos) const
  {
      if ( pos == 0 || pos == npos )
          return pos;
      else
          return const_iterator(m_impl.begin() + pos) - begin();
  }
#endif // !wxUSE_UNICODE_UTF8/wxUSE_UNICODE_UTF8

public:
  // standard types
  typedef wxUniChar value_type;
  typedef wxUniChar char_type;
  typedef wxUniCharRef reference;
  typedef wxChar* pointer;
  typedef const wxChar* const_pointer;

  typedef size_t size_type;
  typedef wxUniChar const_reference;

#if wxUSE_STL
  #if wxUSE_UNICODE_UTF8
    // random access is not O(1), as required by Random Access Iterator
    #define WX_STR_ITERATOR_TAG std::bidirectional_iterator_tag
  #else
    #define WX_STR_ITERATOR_TAG std::random_access_iterator_tag
  #endif
#else
  #define WX_STR_ITERATOR_TAG void /* dummy type */
#endif

  #define WX_STR_ITERATOR_IMPL(iterator_name, pointer_type,                 \
                               reference_type, reference_ctor)              \
      private:                                                              \
          typedef wxStringImpl::iterator_name underlying_iterator;          \
      public:                                                               \
          typedef WX_STR_ITERATOR_TAG iterator_category;                    \
          typedef wxUniChar value_type;                                     \
          typedef int difference_type;                                      \
          typedef reference_type reference;                                 \
          typedef pointer_type pointer;                                     \
                                                                            \
          reference operator*() const { return reference_ctor; }            \
          reference operator[](size_t n) const { return *(*this + n); }     \
                                                                            \
          iterator_name& operator++()                                       \
            { wxStringOperations::IncIter(m_cur); return *this; }           \
          iterator_name& operator--()                                       \
            { wxStringOperations::DecIter(m_cur); return *this; }           \
          iterator_name operator++(int)                                     \
          {                                                                 \
              iterator_name tmp = *this;                                    \
              wxStringOperations::IncIter(m_cur);                           \
              return tmp;                                                   \
          }                                                                 \
          iterator_name operator--(int)                                     \
          {                                                                 \
              iterator_name tmp = *this;                                    \
              wxStringOperations::DecIter(m_cur);                           \
              return tmp;                                                   \
          }                                                                 \
                                                                            \
          iterator_name& operator+=(int n)                                  \
          {                                                                 \
              m_cur = wxStringOperations::AddToIter(m_cur, n);              \
              return *this;                                                 \
          }                                                                 \
          iterator_name& operator+=(size_t n)                               \
          {                                                                 \
              m_cur = wxStringOperations::AddToIter(m_cur, (int)n);         \
              return *this;                                                 \
          }                                                                 \
          iterator_name& operator-=(int n)                                  \
          {                                                                 \
              m_cur = wxStringOperations::AddToIter(m_cur, -n);             \
              return *this;                                                 \
          }                                                                 \
          iterator_name& operator-=(size_t n)                               \
          {                                                                 \
              m_cur = wxStringOperations::AddToIter(m_cur, -(int)n);        \
              return *this;                                                 \
          }                                                                 \
                                                                            \
          difference_type operator-(const iterator_name& i) const           \
            { return wxStringOperations::DiffIters(m_cur, i.m_cur); }       \
                                                                            \
          bool operator==(const iterator_name& i) const                     \
            { return m_cur == i.m_cur; }                                    \
          bool operator!=(const iterator_name& i) const                     \
            { return m_cur != i.m_cur; }                                    \
                                                                            \
          bool operator<(const iterator_name& i) const                      \
            { return m_cur < i.m_cur; }                                     \
          bool operator>(const iterator_name& i) const                      \
            { return m_cur > i.m_cur; }                                     \
          bool operator<=(const iterator_name& i) const                     \
            { return m_cur <= i.m_cur; }                                    \
          bool operator>=(const iterator_name& i) const                     \
            { return m_cur >= i.m_cur; }                                    \
                                                                            \
      private:                                                              \
          /* for internal wxString use only: */                             \
          underlying_iterator impl() const { return m_cur; }                \
                                                                            \
          friend class WXDLLIMPEXP_BASE wxString;                           \
          friend class WXDLLIMPEXP_BASE wxCStrData;                         \
                                                                            \
      private:                                                              \
          underlying_iterator m_cur

  class const_iterator;

#if wxUSE_UNICODE_UTF8
  class iterator
  {
      // NB: In UTF-8 build, (non-const) iterator needs to keep reference
      //     to the underlying wxStringImpl, because UTF-8 is variable-length
      //     encoding and changing the value pointer to by an iterator using
      //     its operator* requires calling wxStringImpl::replace() if the old
      //     and new values differ in their encoding's length.

      WX_STR_ITERATOR_IMPL(iterator, wxChar*, wxUniCharRef,
                           wxUniCharRef::CreateForString(m_str, m_cur));

  public:
      iterator(const iterator& i) : m_cur(i.m_cur), m_str(i.m_str) {}

      iterator operator+(int n) const
        { return iterator(m_str, wxStringOperations::AddToIter(m_cur, n)); }
      iterator operator+(size_t n) const
        { return iterator(m_str, wxStringOperations::AddToIter(m_cur, (int)n)); }
      iterator operator-(int n) const
        { return iterator(m_str, wxStringOperations::AddToIter(m_cur, -n)); }
      iterator operator-(size_t n) const
        { return iterator(m_str, wxStringOperations::AddToIter(m_cur, -(int)n)); }

  private:
      iterator(wxString *str, underlying_iterator ptr)
          : m_cur(ptr), m_str(str->m_impl) {}
      iterator(wxStringImpl& str, underlying_iterator ptr)
          : m_cur(ptr), m_str(str) {}

      wxStringImpl& m_str;

      friend class const_iterator;
  };

  size_t IterToImplPos(wxString::iterator i) const
    { return wxStringImpl::const_iterator(i.impl()) - m_impl.begin(); }

#else // !wxUSE_UNICODE_UTF8

  class iterator
  {
      WX_STR_ITERATOR_IMPL(iterator, wxChar*, wxUniCharRef,
                           wxUniCharRef::CreateForString(m_cur));

  public:
      iterator(const iterator& i) : m_cur(i.m_cur) {}

      iterator operator+(int n) const
        { return iterator(wxStringOperations::AddToIter(m_cur, n)); }
      iterator operator+(size_t n) const
        { return iterator(wxStringOperations::AddToIter(m_cur, (int)n)); }
      iterator operator-(int n) const
        { return iterator(wxStringOperations::AddToIter(m_cur, -n)); }
      iterator operator-(size_t n) const
        { return iterator(wxStringOperations::AddToIter(m_cur, -(int)n)); }

  private:
      // for internal wxString use only:
      iterator(underlying_iterator ptr) : m_cur(ptr) {}
      iterator(wxString *WXUNUSED(str), underlying_iterator ptr) : m_cur(ptr) {}

      friend class const_iterator;
  };
#endif // wxUSE_UNICODE_UTF8/!wxUSE_UNICODE_UTF8

  class const_iterator
  {
      // NB: reference_type is intentionally value, not reference, the character
      //     may be encoded differently in wxString data:
      WX_STR_ITERATOR_IMPL(const_iterator, const wxChar*, wxUniChar,
                           wxStringOperations::DecodeChar(m_cur));

  public:
      const_iterator(const const_iterator& i) : m_cur(i.m_cur) {}
      const_iterator(const iterator& i) : m_cur(i.m_cur) {}

      const_iterator operator+(int n) const
        { return const_iterator(wxStringOperations::AddToIter(m_cur, n)); }
      const_iterator operator+(size_t n) const
        { return const_iterator(wxStringOperations::AddToIter(m_cur, (int)n)); }
      const_iterator operator-(int n) const
        { return const_iterator(wxStringOperations::AddToIter(m_cur, -n)); }
      const_iterator operator-(size_t n) const
        { return const_iterator(wxStringOperations::AddToIter(m_cur, -(int)n)); }

  private:
      // for internal wxString use only:
      const_iterator(underlying_iterator ptr) : m_cur(ptr) {}
  };

  #undef WX_STR_ITERATOR_TAG
  #undef WX_STR_ITERATOR_IMPL

  friend class iterator;
  friend class const_iterator;

  template <typename T>
  class reverse_iterator_impl
  {
  public:
      typedef T iterator_type;

      typedef typename T::iterator_category iterator_category;
      typedef typename T::value_type value_type;
      typedef typename T::difference_type difference_type;
      typedef typename T::reference reference;
      typedef typename T::pointer *pointer;

      reverse_iterator_impl(iterator_type i) : m_cur(i) {}
      reverse_iterator_impl(const reverse_iterator_impl& ri)
          : m_cur(ri.m_cur) {}

      iterator_type base() const { return m_cur; }

      reference operator*() const { return *(m_cur-1); }
      reference operator[](size_t n) const { return *(*this + n); }

      reverse_iterator_impl& operator++()
        { --m_cur; return *this; }
      reverse_iterator_impl operator++(int)
        { reverse_iterator_impl tmp = *this; --m_cur; return tmp; }
      reverse_iterator_impl& operator--()
        { ++m_cur; return *this; }
      reverse_iterator_impl operator--(int)
        { reverse_iterator_impl tmp = *this; ++m_cur; return tmp; }

      // NB: explicit <T> in the functions below is to keep BCC 5.5 happy
      reverse_iterator_impl operator+(int n) const
        { return reverse_iterator_impl<T>(m_cur - n); }
      reverse_iterator_impl operator+(size_t n) const
        { return reverse_iterator_impl<T>(m_cur - n); }
      reverse_iterator_impl operator-(int n) const
        { return reverse_iterator_impl<T>(m_cur + n); }
      reverse_iterator_impl operator-(size_t n) const
        { return reverse_iterator_impl<T>(m_cur + n); }
      reverse_iterator_impl operator+=(int n)
        { m_cur -= n; return *this; }
      reverse_iterator_impl operator+=(size_t n)
        { m_cur -= n; return *this; }
      reverse_iterator_impl operator-=(int n)
        { m_cur += n; return *this; }
      reverse_iterator_impl operator-=(size_t n)
        { m_cur += n; return *this; }

      unsigned operator-(const reverse_iterator_impl& i) const
        { return i.m_cur - m_cur; }

      bool operator==(const reverse_iterator_impl& ri) const
        { return m_cur == ri.m_cur; }
      bool operator!=(const reverse_iterator_impl& ri) const
        { return !(*this == ri); }

      bool operator<(const reverse_iterator_impl& i) const
        { return m_cur > i.m_cur; }
      bool operator>(const reverse_iterator_impl& i) const
        { return m_cur < i.m_cur; }
      bool operator<=(const reverse_iterator_impl& i) const
        { return m_cur >= i.m_cur; }
      bool operator>=(const reverse_iterator_impl& i) const
        { return m_cur <= i.m_cur; }

  private:
      iterator_type m_cur;
  };

  typedef reverse_iterator_impl<iterator> reverse_iterator;
  typedef reverse_iterator_impl<const_iterator> const_reverse_iterator;

private:
  // used to transform an expression built using c_str() (and hence of type
  // wxCStrData) to an iterator into the string
  static const_iterator CreateConstIterator(const wxCStrData& data)
  {
      return const_iterator(data.m_str->begin() + data.m_offset);
  }

  // in UTF-8 STL build, creation from std::string requires conversion under
  // non-UTF8 locales, so we can't have and use wxString(wxStringImpl) ctor;
  // instead we define dummy type that lets us have wxString ctor for creation
  // from wxStringImpl that couldn't be used by user code (in all other builds,
  // "standard" ctors can be used):
#if wxUSE_UNICODE_UTF8 && wxUSE_STL_BASED_WXSTRING
  struct CtorFromStringImplTag {};

  wxString(CtorFromStringImplTag* WXUNUSED(dummy), const wxStringImpl& src)
      : m_impl(src) {}

  static wxString FromImpl(const wxStringImpl& src)
      { return wxString((CtorFromStringImplTag*)NULL, src); }
#else
  #if !wxUSE_STL_BASED_WXSTRING
  wxString(const wxStringImpl& src) : m_impl(src) { }
  // else: already defined as wxString(wxStdString) below
  #endif
  static wxString FromImpl(const wxStringImpl& src) { return wxString(src); }
#endif

public:
  // constructors and destructor
    // ctor for an empty string
  wxString() {}

    // copy ctor
  wxString(const wxString& stringSrc) : m_impl(stringSrc.m_impl) { }

    // string containing nRepeat copies of ch
  wxString(wxUniChar ch, size_t nRepeat = 1)
    { assign(nRepeat, ch); }
  wxString(size_t nRepeat, wxUniChar ch)
    { assign(nRepeat, ch); }
  wxString(wxUniCharRef ch, size_t nRepeat = 1)
    { assign(nRepeat, ch); }
  wxString(size_t nRepeat, wxUniCharRef ch)
    { assign(nRepeat, ch); }
  wxString(char ch, size_t nRepeat = 1)
    { assign(nRepeat, ch); }
  wxString(size_t nRepeat, char ch)
    { assign(nRepeat, ch); }
  wxString(wchar_t ch, size_t nRepeat = 1)
    { assign(nRepeat, ch); }
  wxString(size_t nRepeat, wchar_t ch)
    { assign(nRepeat, ch); }

    // ctors from char* strings:
  wxString(const char *psz)
    : m_impl(ImplStr(psz)) {}
  wxString(const char *psz, const wxMBConv& conv)
    : m_impl(ImplStr(psz, conv)) {}
  wxString(const char *psz, size_t nLength)
    { assign(psz, nLength); }
  wxString(const char *psz, const wxMBConv& conv, size_t nLength)
  {
    SubstrBufFromMB str(ImplStr(psz, nLength, conv));
    m_impl.assign(str.data, str.len);
  }

    // and unsigned char*:
  wxString(const unsigned char *psz)
    : m_impl(ImplStr((const char*)psz)) {}
  wxString(const unsigned char *psz, const wxMBConv& conv)
    : m_impl(ImplStr((const char*)psz, conv)) {}
  wxString(const unsigned char *psz, size_t nLength)
    { assign((const char*)psz, nLength); }
  wxString(const unsigned char *psz, const wxMBConv& conv, size_t nLength)
  {
    SubstrBufFromMB str(ImplStr((const char*)psz, nLength, conv));
    m_impl.assign(str.data, str.len);
  }

    // ctors from wchar_t* strings:
  wxString(const wchar_t *pwz)
    : m_impl(ImplStr(pwz)) {}
  wxString(const wchar_t *pwz, const wxMBConv& WXUNUSED(conv))
    : m_impl(ImplStr(pwz)) {}
  wxString(const wchar_t *pwz, size_t nLength)
    { assign(pwz, nLength); }
  wxString(const wchar_t *pwz, const wxMBConv& WXUNUSED(conv), size_t nLength)
    { assign(pwz, nLength); }

  wxString(const wxCharBuffer& buf)
    { assign(buf.data()); } // FIXME-UTF8: fix for embedded NUL and buffer length
  wxString(const wxWCharBuffer& buf)
    { assign(buf.data()); } // FIXME-UTF8: fix for embedded NUL and buffer length

  wxString(const wxCStrData& cstr)
      : m_impl(cstr.AsString().m_impl) { }

    // as we provide both ctors with this signature for both char and unsigned
    // char string, we need to provide one for wxCStrData to resolve ambiguity
  wxString(const wxCStrData& cstr, size_t nLength)
      : m_impl(cstr.AsString().Mid(0, nLength).m_impl) {}

    // and because wxString is convertible to wxCStrData and const wxChar *
    // we also need to provide this one
  wxString(const wxString& str, size_t nLength)
      : m_impl(str.Mid(0, nLength).m_impl) {}

  // even if we're not built with wxUSE_STL == 1 it is very convenient to allow
  // implicit conversions from std::string to wxString and vice verse as this
  // allows to use the same strings in non-GUI and GUI code, however we don't
  // want to unconditionally add this ctor as it would make wx lib dependent on
  // libstdc++ on some Linux versions which is bad, so instead we ask the
  // client code to define this wxUSE_STD_STRING symbol if they need it
#if wxUSE_STD_STRING

  #if wxUSE_UNICODE_WCHAR
    wxString(const wxStdWideString& str) : m_impl(str) {}
  #else // UTF-8 or ANSI
    wxString(const wxStdWideString& str)
        { assign(str.c_str(), str.length()); }
  #endif

  #if !wxUSE_UNICODE // ANSI build
    // FIXME-UTF8: do this in UTF8 build #if wxUSE_UTF8_LOCALE_ONLY, too
    wxString(const std::string& str) : m_impl(str) {}
  #else // Unicode
    wxString(const std::string& str)
        { assign(str.c_str(), str.length()); }
  #endif

  #if wxUSE_UNICODE_WCHAR && wxUSE_STL_BASED_WXSTRING
    // wxStringImpl is std::string in the encoding we want
    operator const wxStdWideString&() const { return m_impl; }
  #else
    // wxStringImpl is either not std::string or needs conversion
    operator wxStdWideString() const
        // FIXME-UTF8: broken for embedded NULs
        { return wxStdWideString(wc_str()); }
  #endif

  #if !wxUSE_UNICODE && wxUSE_STL_BASED_WXSTRING
    // FIXME-UTF8: do this in UTF8 build #if wxUSE_UTF8_LOCALE_ONLY, too
    // wxStringImpl is std::string in the encoding we want
    operator const std::string&() const { return m_impl; }
  #else
    // wxStringImpl is either not std::string or needs conversion
    operator std::string() const
        // FIXME-UTF8: broken for embedded NULs
        { return std::string(mb_str()); }
  #endif

#endif // wxUSE_STD_STRING

  // first valid index position
  const_iterator begin() const { return const_iterator(m_impl.begin()); }
  iterator begin() { return iterator(this, m_impl.begin()); }
  // position one after the last valid one
  const_iterator end() const { return const_iterator(m_impl.end()); }
  iterator end() { return iterator(this, m_impl.end()); }

  // first element of the reversed string
  const_reverse_iterator rbegin() const
    { return const_reverse_iterator(end()); }
  reverse_iterator rbegin()
    { return reverse_iterator(end()); }
  // one beyond the end of the reversed string
  const_reverse_iterator rend() const
    { return const_reverse_iterator(begin()); }
  reverse_iterator rend()
    { return reverse_iterator(begin()); }

  // std::string methods:
#if wxUSE_UNICODE_UTF8
  size_t length() const { return end() - begin(); } // FIXME-UTF8: optimize!
#else
  size_t length() const { return m_impl.length(); }
#endif

  size_type size() const { return length(); }
  size_type max_size() const { return npos; }

  bool empty() const { return m_impl.empty(); }

  size_type capacity() const { return m_impl.capacity(); } // FIXME-UTF8
  void reserve(size_t sz) { m_impl.reserve(sz); } // FIXME-UTF8

  void resize(size_t nSize, wxUniChar ch = wxT('\0'))
  {
#if wxUSE_UNICODE_UTF8
    if ( !ch.IsAscii() )
    {
        size_t len = length();
        if ( nSize == len)
            return;
        else if ( nSize < len )
            erase(nSize);
        else
            append(nSize - len, ch);
    }
    else
#endif
        m_impl.resize(nSize, (wxStringCharType)ch);
  }

  wxString substr(size_t nStart = 0, size_t nLen = npos) const
  {
    size_t pos, len;
    PosLenToImpl(nStart, nLen, &pos, &len);
    return FromImpl(m_impl.substr(pos, len));
  }

  // generic attributes & operations
    // as standard strlen()
  size_t Len() const { return length(); }
    // string contains any characters?
  bool IsEmpty() const { return empty(); }
    // empty string is "false", so !str will return true
  bool operator!() const { return empty(); }
    // truncate the string to given length
  wxString& Truncate(size_t uiLen);
    // empty string contents
  void Empty()
  {
    Truncate(0);

    wxASSERT_MSG( empty(), _T("string not empty after call to Empty()?") );
  }
    // empty the string and free memory
  void Clear()
  {
    wxString tmp(wxEmptyString);
    swap(tmp);
  }

  // contents test
    // Is an ascii value
  bool IsAscii() const;
    // Is a number
  bool IsNumber() const;
    // Is a word
  bool IsWord() const;

  // data access (all indexes are 0 based)
    // read access
    wxUniChar at(size_t n) const
      { return *(begin() + n); } // FIXME-UTF8: optimize?
    wxUniChar GetChar(size_t n) const
      { return at(n); }
    // read/write access
    wxUniCharRef at(size_t n)
      { return *(begin() + n); } // FIXME-UTF8: optimize?
    wxUniCharRef GetWritableChar(size_t n)
      { return at(n); }
    // write access
    void  SetChar(size_t n, wxUniChar ch)
      { at(n) = ch; }

    // get last character
    wxUniChar Last() const
      {
          wxASSERT_MSG( !empty(), _T("wxString: index out of bounds") );

          return at(length() - 1);
      }

    // get writable last character
    wxUniCharRef Last()
      {
          wxASSERT_MSG( !empty(), _T("wxString: index out of bounds") );
          return at(length() - 1);
      }

    /*
       Note that we we must define all of the overloads below to avoid
       ambiguity when using str[0].
     */
    wxUniChar operator[](int n) const
      { return at(n); }
    wxUniChar operator[](long n) const
      { return at(n); }
    wxUniChar operator[](size_t n) const
      { return at(n); }
#ifndef wxSIZE_T_IS_UINT
    wxUniChar operator[](unsigned int n) const
      { return at(n); }
#endif // size_t != unsigned int

    // operator versions of GetWriteableChar()
    wxUniCharRef operator[](int n)
      { return at(n); }
    wxUniCharRef operator[](long n)
      { return at(n); }
    wxUniCharRef operator[](size_t n)
      { return at(n); }
#ifndef wxSIZE_T_IS_UINT
    wxUniCharRef operator[](unsigned int n)
      { return at(n); }
#endif // size_t != unsigned int

    // explicit conversion to C string (use this with printf()!)
    wxCStrData c_str() const { return wxCStrData(this); }
    wxCStrData data() const { return c_str(); }

    // implicit conversion to C string
    operator wxCStrData() const { return c_str(); }
    operator const char*() const { return c_str(); }
    operator const wchar_t*() const { return c_str(); }

    // identical to c_str(), for MFC compatibility
    const wxCStrData GetData() const { return c_str(); }

    // explicit conversion to C string in internal representation (char*,
    // wchar_t*, UTF-8-encoded char*, depending on the build):
    const wxStringCharType *wx_str() const { return m_impl.c_str(); }

    // conversion to *non-const* multibyte or widestring buffer; modifying
    // returned buffer won't affect the string, these methods are only useful
    // for passing values to const-incorrect functions
    wxWritableCharBuffer char_str(const wxMBConv& conv = wxConvLibc) const
        { return mb_str(conv); }
    wxWritableWCharBuffer wchar_str() const { return wc_str(); }

    // conversion to/from plain (i.e. 7 bit) ASCII: this is useful for
    // converting numbers or strings which are certain not to contain special
    // chars (typically system functions, X atoms, environment variables etc.)
    //
    // the behaviour of these functions with the strings containing anything
    // else than 7 bit ASCII characters is undefined, use at your own risk.
#if wxUSE_UNICODE
    static wxString FromAscii(const char *ascii);  // string
    static wxString FromAscii(const char ascii);   // char
    const wxCharBuffer ToAscii() const;
#else // ANSI
    static wxString FromAscii(const char *ascii) { return wxString( ascii ); }
    static wxString FromAscii(const char ascii) { return wxString( ascii ); }
    const char *ToAscii() const { return c_str(); }
#endif // Unicode/!Unicode

    // conversions with (possible) format conversions: have to return a
    // buffer with temporary data
    //
    // the functions defined (in either Unicode or ANSI) mode are mb_str() to
    // return an ANSI (multibyte) string, wc_str() to return a wide string and
    // fn_str() to return a string which should be used with the OS APIs
    // accepting the file names. The return value is always the same, but the
    // type differs because a function may either return pointer to the buffer
    // directly or have to use intermediate buffer for translation.
#if wxUSE_UNICODE
    const wxCharBuffer mb_str(const wxMBConv& conv = wxConvLibc) const;

    const wxWX2MBbuf mbc_str() const { return mb_str(*wxConvCurrent); }

#if wxUSE_UNICODE_WCHAR
    const wxChar* wc_str() const { return wx_str(); }
#elif wxUSE_UNICODE_UTF8
    const wxWCharBuffer wc_str() const;
#endif
    // for compatibility with !wxUSE_UNICODE version
    const wxWX2WCbuf wc_str(const wxMBConv& WXUNUSED(conv)) const
      { return wc_str(); }

#if wxMBFILES
    const wxCharBuffer fn_str() const { return mb_str(wxConvFile); }
#else // !wxMBFILES
    const wxWX2WCbuf fn_str() const { return wc_str(); }
#endif // wxMBFILES/!wxMBFILES

#else // ANSI
    const wxChar* mb_str() const { return wx_str(); }

    // for compatibility with wxUSE_UNICODE version
    const wxChar* mb_str(const wxMBConv& WXUNUSED(conv)) const { return wx_str(); }

    const wxWX2MBbuf mbc_str() const { return mb_str(); }

#if wxUSE_WCHAR_T
    const wxWCharBuffer wc_str(const wxMBConv& conv = wxConvLibc) const;
#endif // wxUSE_WCHAR_T
#ifdef __WXOSX__
    const wxCharBuffer fn_str() const { return wxConvFile.cWC2WX( wc_str( wxConvLocal ) ); }
#else
    const wxChar* fn_str() const { return c_str(); }
#endif
#endif // Unicode/ANSI

  // overloaded assignment
    // from another wxString
  wxString& operator=(const wxString& stringSrc)
    { m_impl = stringSrc.m_impl; return *this; }
  wxString& operator=(const wxCStrData& cstr)
    { return *this = cstr.AsString(); }
    // from a character
  wxString& operator=(wxUniChar ch)
    { m_impl = wxStringOperations::EncodeChar(ch); return *this; }
  wxString& operator=(wxUniCharRef ch)
    { return operator=((wxUniChar)ch); }
  wxString& operator=(char ch)
    { return operator=(wxUniChar(ch)); }
  wxString& operator=(unsigned char ch)
    { return operator=(wxUniChar(ch)); }
  wxString& operator=(wchar_t ch)
    { return operator=(wxUniChar(ch)); }
    // from a C string - STL probably will crash on NULL,
    // so we need to compensate in that case
#if wxUSE_STL_BASED_WXSTRING
  wxString& operator=(const char *psz)
    { if (psz) m_impl = ImplStr(psz); else Clear(); return *this; }
  wxString& operator=(const wchar_t *pwz)
    { if (pwz) m_impl = ImplStr(pwz); else Clear(); return *this; }
#else
  wxString& operator=(const char *psz)
    { m_impl = ImplStr(psz); return *this; }
  wxString& operator=(const wchar_t *pwz)
    { m_impl = ImplStr(pwz); return *this; }
#endif
  wxString& operator=(const unsigned char *psz)
    { return operator=((const char*)psz); }

    // from wxWCharBuffer
  wxString& operator=(const wxWCharBuffer& s)
    { return operator=(s.data()); } // FIXME-UTF8: fix for embedded NULs
    // from wxCharBuffer
  wxString& operator=(const wxCharBuffer& s)
    { return operator=(s.data()); } // FIXME-UTF8: fix for embedded NULs

  // string concatenation
    // in place concatenation
    /*
        Concatenate and return the result. Note that the left to right
        associativity of << allows to write things like "str << str1 << str2
        << ..." (unlike with +=)
     */
      // string += string
  wxString& operator<<(const wxString& s)
  {
#if WXWIN_COMPATIBILITY_2_8 && !wxUSE_STL_BASED_WXSTRING && !wxUSE_UNICODE_UTF8
    wxASSERT_MSG( s.IsValid(),
                  _T("did you forget to call UngetWriteBuf()?") );
#endif

    append(s);
    return *this;
  }
      // string += C string
  wxString& operator<<(const char *psz)
    { append(psz); return *this; }
  wxString& operator<<(const wchar_t *pwz)
    { append(pwz); return *this; }
  wxString& operator<<(const wxCStrData& psz)
    { append(psz.AsString()); return *this; }
      // string += char
  wxString& operator<<(wxUniChar ch) { append(1, ch); return *this; }
  wxString& operator<<(wxUniCharRef ch) { append(1, ch); return *this; }
  wxString& operator<<(char ch) { append(1, ch); return *this; }
  wxString& operator<<(unsigned char ch) { append(1, ch); return *this; }
  wxString& operator<<(wchar_t ch) { append(1, ch); return *this; }

      // string += buffer (i.e. from wxGetString)
  wxString& operator<<(const wxWCharBuffer& s)
    { return operator<<((const wchar_t *)s); }
  wxString& operator+=(const wxWCharBuffer& s)
    { return operator<<((const wchar_t *)s); }

  wxString& operator<<(const wxCharBuffer& s)
    { return operator<<((const char *)s); }
  wxString& operator+=(const wxCharBuffer& s)
    { return operator<<((const char *)s); }

    // string += C string
  wxString& Append(const wxString& s)
    {
        // test for empty() to share the string if possible
        if ( empty() )
            *this = s;
        else
            append(s);
        return *this;
    }
  wxString& Append(const wxCStrData& psz)
    { append(psz); return *this; }
  wxString& Append(const char* psz)
    { append(psz); return *this; }
  wxString& Append(const wchar_t* pwz)
    { append(pwz); return *this; }
    // append count copies of given character
  wxString& Append(wxUniChar ch, size_t count = 1u)
    { append(count, ch); return *this; }
  wxString& Append(wxUniCharRef ch, size_t count = 1u)
    { append(count, ch); return *this; }
  wxString& Append(char ch, size_t count = 1u)
    { append(count, ch); return *this; }
  wxString& Append(unsigned char ch, size_t count = 1u)
    { append(count, ch); return *this; }
  wxString& Append(wchar_t ch, size_t count = 1u)
    { append(count, ch); return *this; }
  wxString& Append(const char* psz, size_t nLen)
    { append(psz, nLen); return *this; }
  wxString& Append(const wchar_t* pwz, size_t nLen)
    { append(pwz, nLen); return *this; }

    // prepend a string, return the string itself
  wxString& Prepend(const wxString& str)
    { *this = str + *this; return *this; }

    // non-destructive concatenation
      // two strings
  friend wxString WXDLLIMPEXP_BASE operator+(const wxString& string1,
                                             const wxString& string2);
      // string with a single char
  friend wxString WXDLLIMPEXP_BASE operator+(const wxString& string, wxUniChar ch);
      // char with a string
  friend wxString WXDLLIMPEXP_BASE operator+(wxUniChar ch, const wxString& string);
      // string with C string
  friend wxString WXDLLIMPEXP_BASE operator+(const wxString& string,
                                             const char *psz);
  friend wxString WXDLLIMPEXP_BASE operator+(const wxString& string,
                                             const wchar_t *pwz);
      // C string with string
  friend wxString WXDLLIMPEXP_BASE operator+(const char *psz,
                                             const wxString& string);
  friend wxString WXDLLIMPEXP_BASE operator+(const wchar_t *pwz,
                                             const wxString& string);

  // stream-like functions
      // insert an int into string
  wxString& operator<<(int i)
    { return (*this) << Format(_T("%d"), i); }
      // insert an unsigned int into string
  wxString& operator<<(unsigned int ui)
    { return (*this) << Format(_T("%u"), ui); }
      // insert a long into string
  wxString& operator<<(long l)
    { return (*this) << Format(_T("%ld"), l); }
      // insert an unsigned long into string
  wxString& operator<<(unsigned long ul)
    { return (*this) << Format(_T("%lu"), ul); }
#if defined wxLongLong_t && !defined wxLongLongIsLong
      // insert a long long if they exist and aren't longs
  wxString& operator<<(wxLongLong_t ll)
    {
      const wxChar *fmt = _T("%") wxLongLongFmtSpec _T("d");
      return (*this) << Format(fmt, ll);
    }
      // insert an unsigned long long
  wxString& operator<<(wxULongLong_t ull)
    {
      const wxChar *fmt = _T("%") wxLongLongFmtSpec _T("u");
      return (*this) << Format(fmt , ull);
    }
#endif
      // insert a float into string
  wxString& operator<<(float f)
    { return (*this) << Format(_T("%f"), f); }
      // insert a double into string
  wxString& operator<<(double d)
    { return (*this) << Format(_T("%g"), d); }

  // string comparison
    // case-sensitive comparison (returns a value < 0, = 0 or > 0)
  int Cmp(const char *psz) const
    { return compare(psz); }
  int Cmp(const wchar_t *pwz) const
    { return compare(pwz); }
  int Cmp(const wxString& s) const
    { return compare(s); }
    // same as Cmp() but not case-sensitive
  int CmpNoCase(const wxString& s) const;
  int CmpNoCase(const char *psz) const
    { return CmpNoCase(wxString(psz)); }
  int CmpNoCase(const wchar_t *pwz) const
    { return CmpNoCase(wxString(pwz)); }
    // test for the string equality, either considering case or not
    // (if compareWithCase then the case matters)
  bool IsSameAs(const wxString& str, bool compareWithCase = true) const
    { return (compareWithCase ? Cmp(str) : CmpNoCase(str)) == 0; }
  bool IsSameAs(const char *str, bool compareWithCase = true) const
    { return (compareWithCase ? Cmp(str) : CmpNoCase(str)) == 0; }
  bool IsSameAs(const wchar_t *str, bool compareWithCase = true) const
    { return (compareWithCase ? Cmp(str) : CmpNoCase(str)) == 0; }
    // comparison with a single character: returns true if equal
  bool IsSameAs(wxUniChar c, bool compareWithCase = true) const
    {
      return (length() == 1) && (compareWithCase ? GetChar(0u) == c
                              : wxToupper(GetChar(0u)) == wxToupper(c));
    }
  // FIXME-UTF8: remove these overloads
  bool IsSameAs(wxUniCharRef c, bool compareWithCase = true) const
    { return IsSameAs(wxUniChar(c), compareWithCase); }
  bool IsSameAs(char c, bool compareWithCase = true) const
    { return IsSameAs(wxUniChar(c), compareWithCase); }
  bool IsSameAs(unsigned char c, bool compareWithCase = true) const
    { return IsSameAs(wxUniChar(c), compareWithCase); }
  bool IsSameAs(wchar_t c, bool compareWithCase = true) const
    { return IsSameAs(wxUniChar(c), compareWithCase); }
  bool IsSameAs(int c, bool compareWithCase = true) const
    { return IsSameAs(wxUniChar(c), compareWithCase); }

  // simple sub-string extraction
      // return substring starting at nFirst of length nCount (or till the end
      // if nCount = default value)
  wxString Mid(size_t nFirst, size_t nCount = npos) const;

      // operator version of Mid()
  wxString  operator()(size_t start, size_t len) const
    { return Mid(start, len); }

      // check if the string starts with the given prefix and return the rest
      // of the string in the provided pointer if it is not NULL; otherwise
      // return false
  bool StartsWith(const wxChar *prefix, wxString *rest = NULL) const;
      // check if the string ends with the given suffix and return the
      // beginning of the string before the suffix in the provided pointer if
      // it is not NULL; otherwise return false
  bool EndsWith(const wxChar *suffix, wxString *rest = NULL) const;

      // get first nCount characters
  wxString Left(size_t nCount) const;
      // get last nCount characters
  wxString Right(size_t nCount) const;
      // get all characters before the first occurance of ch
      // (returns the whole string if ch not found)
  wxString BeforeFirst(wxUniChar ch) const;
      // get all characters before the last occurence of ch
      // (returns empty string if ch not found)
  wxString BeforeLast(wxUniChar ch) const;
      // get all characters after the first occurence of ch
      // (returns empty string if ch not found)
  wxString AfterFirst(wxUniChar ch) const;
      // get all characters after the last occurence of ch
      // (returns the whole string if ch not found)
  wxString AfterLast(wxUniChar ch) const;

    // for compatibility only, use more explicitly named functions above
  wxString Before(wxUniChar ch) const { return BeforeLast(ch); }
  wxString After(wxUniChar ch) const { return AfterFirst(ch); }

  // case conversion
      // convert to upper case in place, return the string itself
  wxString& MakeUpper();
      // convert to upper case, return the copy of the string
      // Here's something to remember: BC++ doesn't like returns in inlines.
  wxString Upper() const ;
      // convert to lower case in place, return the string itself
  wxString& MakeLower();
      // convert to lower case, return the copy of the string
  wxString Lower() const ;

  // trimming/padding whitespace (either side) and truncating
      // remove spaces from left or from right (default) side
  wxString& Trim(bool bFromRight = true);
      // add nCount copies chPad in the beginning or at the end (default)
  wxString& Pad(size_t nCount, wxUniChar chPad = wxT(' '), bool bFromRight = true);

  // searching and replacing
      // searching (return starting index, or -1 if not found)
  int Find(wxUniChar ch, bool bFromEnd = false) const;   // like strchr/strrchr
  int Find(wxUniCharRef ch, bool bFromEnd = false) const
    { return Find(wxUniChar(ch), bFromEnd); }
  int Find(char ch, bool bFromEnd = false) const
    { return Find(wxUniChar(ch), bFromEnd); }
  int Find(unsigned char ch, bool bFromEnd = false) const
    { return Find(wxUniChar(ch), bFromEnd); }
  int Find(wchar_t ch, bool bFromEnd = false) const
    { return Find(wxUniChar(ch), bFromEnd); }
      // searching (return starting index, or -1 if not found)
  // FIXME-UTF8: keep wxString overload only
  int Find(const wxString& sub) const               // like strstr
  {
    size_type idx = find(sub);
    return (idx == npos) ? wxNOT_FOUND : (int)idx;
  }
  int Find(const char *sub) const               // like strstr
  {
    size_type idx = find(sub);
    return (idx == npos) ? wxNOT_FOUND : (int)idx;
  }
  int Find(const wchar_t *sub) const               // like strstr
  {
    size_type idx = find(sub);
    return (idx == npos) ? wxNOT_FOUND : (int)idx;
  }

      // replace first (or all of bReplaceAll) occurences of substring with
      // another string, returns the number of replacements made
  size_t Replace(const wxString& strOld,
                 const wxString& strNew,
                 bool bReplaceAll = true);

    // check if the string contents matches a mask containing '*' and '?'
  bool Matches(const wxString& mask) const;

    // conversion to numbers: all functions return true only if the whole
    // string is a number and put the value of this number into the pointer
    // provided, the base is the numeric base in which the conversion should be
    // done and must be comprised between 2 and 36 or be 0 in which case the
    // standard C rules apply (leading '0' => octal, "0x" => hex)
        // convert to a signed integer
    bool ToLong(long *val, int base = 10) const;
        // convert to an unsigned integer
    bool ToULong(unsigned long *val, int base = 10) const;
        // convert to wxLongLong
#if defined(wxLongLong_t)
    bool ToLongLong(wxLongLong_t *val, int base = 10) const;
        // convert to wxULongLong
    bool ToULongLong(wxULongLong_t *val, int base = 10) const;
#endif // wxLongLong_t
        // convert to a double
    bool ToDouble(double *val) const;


#ifndef wxNEEDS_WXSTRING_PRINTF_MIXIN
  // formatted input/output
    // as sprintf(), returns the number of characters written or < 0 on error
    // (take 'this' into account in attribute parameter count)
  // int Printf(const wxChar *pszFormat, ...);
  WX_DEFINE_VARARG_FUNC(int, Printf, DoPrintf)
#endif // !wxNEEDS_WXSTRING_PRINTF_MIXIN
    // as vprintf(), returns the number of characters written or < 0 on error
  int PrintfV(const wxString& format, va_list argptr);

#ifndef wxNEEDS_WXSTRING_PRINTF_MIXIN
    // returns the string containing the result of Printf() to it
  // static wxString Format(const wxChar *pszFormat, ...) ATTRIBUTE_PRINTF_1;
  WX_DEFINE_VARARG_FUNC(static wxString, Format, DoFormat)
#endif
    // the same as above, but takes a va_list
  static wxString FormatV(const wxString& format, va_list argptr);

  // raw access to string memory
    // ensure that string has space for at least nLen characters
    // only works if the data of this string is not shared
  bool Alloc(size_t nLen) { reserve(nLen); /*return capacity() >= nLen;*/ return true; }
    // minimize the string's memory
    // only works if the data of this string is not shared
  bool Shrink();
#if WXWIN_COMPATIBILITY_2_8 && !wxUSE_STL_BASED_WXSTRING && !wxUSE_UNICODE_UTF8
    // These are deprecated, use wxStringBuffer or wxStringBufferLength instead
    //
    // get writable buffer of at least nLen bytes. Unget() *must* be called
    // a.s.a.p. to put string back in a reasonable state!
  wxDEPRECATED( wxChar *GetWriteBuf(size_t nLen) );
    // call this immediately after GetWriteBuf() has been used
  wxDEPRECATED( void UngetWriteBuf() );
  wxDEPRECATED( void UngetWriteBuf(size_t nLen) );
#endif // WXWIN_COMPATIBILITY_2_8 && !wxUSE_STL_BASED_WXSTRING && wxUSE_UNICODE_UTF8

  // wxWidgets version 1 compatibility functions

  // use Mid()
  wxString SubString(size_t from, size_t to) const
      { return Mid(from, (to - from + 1)); }
    // values for second parameter of CompareTo function
  enum caseCompare {exact, ignoreCase};
    // values for first parameter of Strip function
  enum stripType {leading = 0x1, trailing = 0x2, both = 0x3};

#ifndef wxNEEDS_WXSTRING_PRINTF_MIXIN
  // use Printf()
  // (take 'this' into account in attribute parameter count)
  // int sprintf(const wxChar *pszFormat, ...) ATTRIBUTE_PRINTF_2;
  WX_DEFINE_VARARG_FUNC(int, sprintf, DoPrintf)
#endif // wxNEEDS_WXSTRING_PRINTF_MIXIN

    // use Cmp()
  inline int CompareTo(const wxChar* psz, caseCompare cmp = exact) const
    { return cmp == exact ? Cmp(psz) : CmpNoCase(psz); }

    // use length()
  size_t Length() const { return length(); }
    // Count the number of characters
  int Freq(wxUniChar ch) const;
    // use MakeLower
  void LowerCase() { MakeLower(); }
    // use MakeUpper
  void UpperCase() { MakeUpper(); }
    // use Trim except that it doesn't change this string
  wxString Strip(stripType w = trailing) const;

    // use Find (more general variants not yet supported)
  size_t Index(const wxChar* psz) const { return Find(psz); }
  size_t Index(wxUniChar ch)         const { return Find(ch);  }
    // use Truncate
  wxString& Remove(size_t pos) { return Truncate(pos); }
  wxString& RemoveLast(size_t n = 1) { return Truncate(length() - n); }

  wxString& Remove(size_t nStart, size_t nLen)
      { return (wxString&)erase( nStart, nLen ); }

    // use Find()
  int First( wxUniChar ch ) const { return Find(ch); }
  int First( wxUniCharRef ch ) const { return Find(ch); }
  int First( char ch ) const { return Find(ch); }
  int First( unsigned char ch ) const { return Find(ch); }
  int First( wchar_t ch ) const { return Find(ch); }
  int First( const wxChar* psz ) const { return Find(psz); }
  int First( const wxString& str ) const { return Find(str); }
  int Last( wxUniChar ch ) const { return Find(ch, true); }
  bool Contains(const wxString& str) const { return Find(str) != wxNOT_FOUND; }

    // use empty()
  bool IsNull() const { return empty(); }

  // std::string compatibility functions

    // take nLen chars starting at nPos
  wxString(const wxString& str, size_t nPos, size_t nLen)
      : m_impl(str.m_impl, nPos, nLen) { }
    // take all characters from first to last
  wxString(const_iterator first, const_iterator last)
      : m_impl(first.impl(), last.impl()) { }
#if WXWIN_COMPATIBILITY_STRING_PTR_AS_ITER
    // the 2 overloads below are for compatibility with the existing code using
    // pointers instead of iterators
  wxString(const char *first, const char *last)
  {
      SubstrBufFromMB str(ImplStr(first, last - first));
      m_impl.assign(str.data, str.len);
  }
  wxString(const wchar_t *first, const wchar_t *last)
  {
      SubstrBufFromWC str(ImplStr(first, last - first));
      m_impl.assign(str.data, str.len);
  }
    // and this one is needed to compile code adding offsets to c_str() result
  wxString(const wxCStrData& first, const wxCStrData& last)
      : m_impl(CreateConstIterator(first).impl(),
               CreateConstIterator(last).impl())
  {
      wxASSERT_MSG( first.m_str == last.m_str,
                    _T("pointers must be into the same string") );
  }
#endif // WXWIN_COMPATIBILITY_STRING_PTR_AS_ITER

  // lib.string.modifiers
    // append elements str[pos], ..., str[pos+n]
  wxString& append(const wxString& str, size_t pos, size_t n)
  {
    size_t from, len;
    str.PosLenToImpl(pos, n, &from, &len);
    m_impl.append(str.m_impl, from, len);
    return *this;
  }
    // append a string
  wxString& append(const wxString& str)
    { m_impl.append(str.m_impl); return *this; }
  wxString& append(const wxCStrData& str)
    { m_impl.append(str.AsString().m_impl); return *this; }
    // append first n (or all if n == npos) characters of sz
  wxString& append(const char *sz)
    { m_impl.append(ImplStr(sz)); return *this; }
  wxString& append(const wchar_t *sz)
    { m_impl.append(ImplStr(sz)); return *this; }
  wxString& append(const char *sz, size_t n)
  {
    SubstrBufFromMB str(ImplStr(sz, n));
    m_impl.append(str.data, str.len);
    return *this;
  }
  wxString& append(const wchar_t *sz, size_t n)
  {
    SubstrBufFromWC str(ImplStr(sz, n));
    m_impl.append(str.data, str.len);
    return *this;
  }
    // append n copies of ch
  wxString& append(size_t n, wxUniChar ch)
  {
#if wxUSE_UNICODE_UTF8
    if ( !ch.IsAscii() )
        m_impl.append(wxStringOperations::EncodeNChars(n, ch));
    else
#endif
        m_impl.append(n, (wxStringCharType)ch);
    return *this;
  }
    // append from first to last
  wxString& append(const_iterator first, const_iterator last)
    { m_impl.append(first.impl(), last.impl()); return *this; }
#if WXWIN_COMPATIBILITY_STRING_PTR_AS_ITER
  wxString& append(const char *first, const char *last)
    { return append(first, last - first); }
  wxString& append(const wchar_t *first, const wchar_t *last)
    { return append(first, last - first); }
  wxString& append(const wxCStrData& first, const wxCStrData& last)
    { return append(CreateConstIterator(first), CreateConstIterator(last)); }
#endif // WXWIN_COMPATIBILITY_STRING_PTR_AS_ITER

    // same as `this_string = str'
  wxString& assign(const wxString& str)
    { m_impl = str.m_impl; return *this; }
  wxString& assign(const wxString& str, size_t len)
  {
    m_impl.assign(str.m_impl, 0, str.LenToImpl(len));
    return *this;
  }
    // same as ` = str[pos..pos + n]
  wxString& assign(const wxString& str, size_t pos, size_t n)
  {
    size_t from, len;
    str.PosLenToImpl(pos, n, &from, &len);
    m_impl.assign(str.m_impl, from, len);
    return *this;
  }
    // same as `= first n (or all if n == npos) characters of sz'
  wxString& assign(const char *sz)
    { m_impl.assign(ImplStr(sz)); return *this; }
  wxString& assign(const wchar_t *sz)
    { m_impl.assign(ImplStr(sz)); return *this; }
  wxString& assign(const char *sz, size_t n)
  {
    SubstrBufFromMB str(ImplStr(sz, n));
    m_impl.assign(str.data, str.len);
    return *this;
  }
  wxString& assign(const wchar_t *sz, size_t n)
  {
    SubstrBufFromWC str(ImplStr(sz, n));
    m_impl.assign(str.data, str.len);
    return *this;
  }
    // same as `= n copies of ch'
  wxString& assign(size_t n, wxUniChar ch)
  {
#if wxUSE_UNICODE_UTF8
    if ( !ch.IsAscii() )
        m_impl.assign(wxStringOperations::EncodeNChars(n, ch));
    else
#endif
        m_impl.assign(n, (wxStringCharType)ch);
    return *this;
  }

  wxString& assign(size_t n, wxUniCharRef ch)
    { return assign(n, wxUniChar(ch)); }
  wxString& assign(size_t n, char ch)
    { return assign(n, wxUniChar(ch)); }
  wxString& assign(size_t n, unsigned char ch)
    { return assign(n, wxUniChar(ch)); }
  wxString& assign(size_t n, wchar_t ch)
    { return assign(n, wxUniChar(ch)); }

    // assign from first to last
  wxString& assign(const_iterator first, const_iterator last)
    { m_impl.assign(first.impl(), last.impl()); return *this; }
#if WXWIN_COMPATIBILITY_STRING_PTR_AS_ITER
  wxString& assign(const char *first, const char *last)
    { return assign(first, last - first); }
  wxString& assign(const wchar_t *first, const wchar_t *last)
    { return assign(first, last - first); }
  wxString& assign(const wxCStrData& first, const wxCStrData& last)
    { return assign(CreateConstIterator(first), CreateConstIterator(last)); }
#endif // WXWIN_COMPATIBILITY_STRING_PTR_AS_ITER

    // string comparison
  int compare(const wxString& str) const;
    // comparison with a substring
  int compare(size_t nStart, size_t nLen, const wxString& str) const;
    // comparison of 2 substrings
  int compare(size_t nStart, size_t nLen,
              const wxString& str, size_t nStart2, size_t nLen2) const;
    // just like strcmp()
  int compare(const char* sz) const;
  int compare(const wchar_t* sz) const;
    // substring comparison with first nCount characters of sz
  int compare(size_t nStart, size_t nLen,
              const char* sz, size_t nCount = npos) const;
  int compare(size_t nStart, size_t nLen,
              const wchar_t* sz, size_t nCount = npos) const;

    // insert another string
  wxString& insert(size_t nPos, const wxString& str)
    { insert(begin() + nPos, str.begin(), str.end()); return *this; }
    // insert n chars of str starting at nStart (in str)
  wxString& insert(size_t nPos, const wxString& str, size_t nStart, size_t n)
  {
    size_t from, len;
    str.PosLenToImpl(nStart, n, &from, &len);
    m_impl.insert(PosToImpl(nPos), str.m_impl, from, len);
    return *this;
  }
    // insert first n (or all if n == npos) characters of sz
  wxString& insert(size_t nPos, const char *sz)
    { m_impl.insert(PosToImpl(nPos), ImplStr(sz)); return *this; }
  wxString& insert(size_t nPos, const wchar_t *sz)
    { m_impl.insert(PosToImpl(nPos), ImplStr(sz)); return *this; }
  wxString& insert(size_t nPos, const char *sz, size_t n)
  {
    SubstrBufFromMB str(ImplStr(sz, n));
    m_impl.insert(PosToImpl(nPos), str.data, str.len);
    return *this;
  }
  wxString& insert(size_t nPos, const wchar_t *sz, size_t n)
  {
    SubstrBufFromWC str(ImplStr(sz, n));
    m_impl.insert(PosToImpl(nPos), str.data, str.len);
    return *this;
  }
    // insert n copies of ch
  wxString& insert(size_t nPos, size_t n, wxUniChar ch)
  {
#if wxUSE_UNICODE_UTF8
    if ( !ch.IsAscii() )
        m_impl.insert(PosToImpl(nPos), wxStringOperations::EncodeNChars(n, ch));
    else
#endif
        m_impl.insert(PosToImpl(nPos), n, (wxStringCharType)ch);
    return *this;
  }
  iterator insert(iterator it, wxUniChar ch)
  {
#if wxUSE_UNICODE_UTF8
    if ( !ch.IsAscii() )
    {
        size_t pos = IterToImplPos(it);
        m_impl.insert(pos, wxStringOperations::EncodeChar(ch));
        return iterator(this, m_impl.begin() + pos);
    }
    else
#endif
        return iterator(this, m_impl.insert(it.impl(), (wxStringCharType)ch));
  }
  void insert(iterator it, const_iterator first, const_iterator last)
    { m_impl.insert(it.impl(), first.impl(), last.impl()); }
#if WXWIN_COMPATIBILITY_STRING_PTR_AS_ITER
  void insert(iterator it, const char *first, const char *last)
    { insert(it - begin(), first, last - first); }
  void insert(iterator it, const wchar_t *first, const wchar_t *last)
    { insert(it - begin(), first, last - first); }
  void insert(iterator it, const wxCStrData& first, const wxCStrData& last)
    { insert(it, CreateConstIterator(first), CreateConstIterator(last)); }
#endif // WXWIN_COMPATIBILITY_STRING_PTR_AS_ITER

  void insert(iterator it, size_type n, wxUniChar ch)
  {
#if wxUSE_UNICODE_UTF8
    if ( !ch.IsAscii() )
        m_impl.insert(IterToImplPos(it), wxStringOperations::EncodeNChars(n, ch));
    else
#endif
        m_impl.insert(it.impl(), n, (wxStringCharType)ch);
  }

    // delete characters from nStart to nStart + nLen
  wxString& erase(size_type pos = 0, size_type n = npos)
  {
    size_t from, len;
    PosLenToImpl(pos, n, &from, &len);
    m_impl.erase(from, len);
    return *this;
  }
    // delete characters from first up to last
  iterator erase(iterator first, iterator last)
    { return iterator(this, m_impl.erase(first.impl(), last.impl())); }
  iterator erase(iterator first)
    { return iterator(this, m_impl.erase(first.impl())); }

#ifdef wxSTRING_BASE_HASNT_CLEAR
  void clear() { erase(); }
#else
  void clear() { m_impl.clear(); }
#endif

    // replaces the substring of length nLen starting at nStart
  wxString& replace(size_t nStart, size_t nLen, const char* sz)
  {
    size_t from, len;
    PosLenToImpl(nStart, nLen, &from, &len);
    m_impl.replace(from, len, ImplStr(sz));
    return *this;
  }
  wxString& replace(size_t nStart, size_t nLen, const wchar_t* sz)
  {
    size_t from, len;
    PosLenToImpl(nStart, nLen, &from, &len);
    m_impl.replace(from, len, ImplStr(sz));
    return *this;
  }
    // replaces the substring of length nLen starting at nStart
  wxString& replace(size_t nStart, size_t nLen, const wxString& str)
  {
    size_t from, len;
    PosLenToImpl(nStart, nLen, &from, &len);
    m_impl.replace(from, len, str.m_impl);
    return *this;
  }
    // replaces the substring with nCount copies of ch
  wxString& replace(size_t nStart, size_t nLen, size_t nCount, wxUniChar ch)
  {
    size_t from, len;
    PosLenToImpl(nStart, nLen, &from, &len);
#if wxUSE_UNICODE_UTF8
    if ( !ch.IsAscii() )
        m_impl.replace(from, len, wxStringOperations::EncodeNChars(nCount, ch));
    else
#endif
        m_impl.replace(from, len, nCount, (wxStringCharType)ch);
    return *this;
  }
    // replaces a substring with another substring
  wxString& replace(size_t nStart, size_t nLen,
                    const wxString& str, size_t nStart2, size_t nLen2)
  {
    size_t from, len;
    PosLenToImpl(nStart, nLen, &from, &len);

    size_t from2, len2;
    str.PosLenToImpl(nStart2, nLen2, &from2, &len2);

    m_impl.replace(from, len, str.m_impl, from2, len2);
    return *this;
  }
     // replaces the substring with first nCount chars of sz
  wxString& replace(size_t nStart, size_t nLen,
                    const char* sz, size_t nCount)
  {
    size_t from, len;
    PosLenToImpl(nStart, nLen, &from, &len);

    SubstrBufFromMB str(ImplStr(sz, nCount));

    m_impl.replace(from, len, str.data, str.len);
    return *this;
  }
  wxString& replace(size_t nStart, size_t nLen,
                    const wchar_t* sz, size_t nCount)
  {
    size_t from, len;
    PosLenToImpl(nStart, nLen, &from, &len);

    SubstrBufFromWC str(ImplStr(sz, nCount));

    m_impl.replace(from, len, str.data, str.len);
    return *this;
  }
  wxString& replace(size_t nStart, size_t nLen,
                    const wxString& s, size_t nCount)
  {
    size_t from, len;
    PosLenToImpl(nStart, nLen, &from, &len);
    m_impl.replace(from, len, s.m_impl.c_str(), s.LenToImpl(nCount));
    return *this;
  }

  wxString& replace(iterator first, iterator last, const char* s)
    { m_impl.replace(first.impl(), last.impl(), ImplStr(s)); return *this; }
  wxString& replace(iterator first, iterator last, const wchar_t* s)
    { m_impl.replace(first.impl(), last.impl(), ImplStr(s)); return *this; }
  wxString& replace(iterator first, iterator last, const char* s, size_type n)
  {
    SubstrBufFromMB str(ImplStr(s, n));
    m_impl.replace(first.impl(), last.impl(), str.data, str.len);
    return *this;
  }
  wxString& replace(iterator first, iterator last, const wchar_t* s, size_type n)
  {
    SubstrBufFromWC str(ImplStr(s, n));
    m_impl.replace(first.impl(), last.impl(), str.data, str.len);
    return *this;
  }
  wxString& replace(iterator first, iterator last, const wxString& s)
    { m_impl.replace(first.impl(), last.impl(), s.m_impl); return *this; }
  wxString& replace(iterator first, iterator last, size_type n, wxUniChar ch)
  {
#if wxUSE_UNICODE_UTF8
    if ( !ch.IsAscii() )
        m_impl.replace(first.impl(), last.impl(),
                       wxStringOperations::EncodeNChars(n, ch));
    else
#endif
        m_impl.replace(first.impl(), last.impl(), n, (wxStringCharType)ch);
    return *this;
  }
  wxString& replace(iterator first, iterator last,
                    const_iterator first1, const_iterator last1)
  {
    m_impl.replace(first.impl(), last.impl(), first1.impl(), last1.impl());
    return *this;
  }
  wxString& replace(iterator first, iterator last,
                    const char *first1, const char *last1)
    { replace(first, last, first1, last1 - first1); return *this; }
  wxString& replace(iterator first, iterator last,
                    const wchar_t *first1, const wchar_t *last1)
    { replace(first, last, first1, last1 - first1); return *this; }

  // swap two strings
  void swap(wxString& str)
    { m_impl.swap(str.m_impl); }

    // find a substring
  size_t find(const wxString& str, size_t nStart = 0) const
    { return PosFromImpl(m_impl.find(str.m_impl, PosToImpl(nStart))); }

    // find first n characters of sz
  size_t find(const char* sz, size_t nStart = 0, size_t n = npos) const
  {
      SubstrBufFromMB str(ImplStr(sz, n));
      return PosFromImpl(m_impl.find(str.data, PosToImpl(nStart), str.len));
  }
  size_t find(const wchar_t* sz, size_t nStart = 0, size_t n = npos) const
  {
      SubstrBufFromWC str(ImplStr(sz, n));
      return PosFromImpl(m_impl.find(str.data, PosToImpl(nStart), str.len));
  }

    // find the first occurence of character ch after nStart
  size_t find(wxUniChar ch, size_t nStart = 0) const
  {
    return PosFromImpl(m_impl.find(wxStringOperations::EncodeChar(ch),
                                   PosToImpl(nStart)));
  }
  size_t find(wxUniCharRef ch, size_t nStart = 0) const
    {  return find(wxUniChar(ch), nStart); }
  size_t find(char ch, size_t nStart = 0) const
    {  return find(wxUniChar(ch), nStart); }
  size_t find(unsigned char ch, size_t nStart = 0) const
    {  return find(wxUniChar(ch), nStart); }
  size_t find(wchar_t ch, size_t nStart = 0) const
    {  return find(wxUniChar(ch), nStart); }

    // rfind() family is exactly like find() but works right to left

    // as find, but from the end
  size_t rfind(const wxString& str, size_t nStart = npos) const
    { return PosFromImpl(m_impl.rfind(str.m_impl, PosToImpl(nStart))); }

    // as find, but from the end
  size_t rfind(const char* sz, size_t nStart = npos, size_t n = npos) const
  {
      SubstrBufFromMB str(ImplStr(sz, n));
      return PosFromImpl(m_impl.rfind(str.data, PosToImpl(nStart), str.len));
  }
  size_t rfind(const wchar_t* sz, size_t nStart = npos, size_t n = npos) const
  {
      SubstrBufFromWC str(ImplStr(sz, n));
      return PosFromImpl(m_impl.rfind(str.data, PosToImpl(nStart), str.len));
  }
    // as find, but from the end
  size_t rfind(wxUniChar ch, size_t nStart = npos) const
  {
    return PosFromImpl(m_impl.rfind(wxStringOperations::EncodeChar(ch),
                                    PosToImpl(nStart)));
  }
  size_t rfind(wxUniCharRef ch, size_t nStart = npos) const
    {  return rfind(wxUniChar(ch), nStart); }
  size_t rfind(char ch, size_t nStart = npos) const
    {  return rfind(wxUniChar(ch), nStart); }
  size_t rfind(unsigned char ch, size_t nStart = npos) const
    {  return rfind(wxUniChar(ch), nStart); }
  size_t rfind(wchar_t ch, size_t nStart = npos) const
    {  return rfind(wxUniChar(ch), nStart); }

  // find first/last occurence of any character (not) in the set:
#if wxUSE_STL_BASED_WXSTRING && !wxUSE_UNICODE_UTF8
  // FIXME-UTF8: this is not entirely correct, because it doesn't work if
  //             sizeof(wchar_t)==2 and surrogates are present in the string;
  //             should we care? Probably not.
  size_t find_first_of(const wxString& str, size_t nStart = 0) const
    { return m_impl.find_first_of(str.m_impl, nStart); }
  size_t find_first_of(const char* sz, size_t nStart = 0) const
    { return m_impl.find_first_of(ImplStr(sz), nStart); }
  size_t find_first_of(const wchar_t* sz, size_t nStart = 0) const
    { return m_impl.find_first_of(ImplStr(sz), nStart); }
  size_t find_first_of(const char* sz, size_t nStart, size_t n) const
    { return m_impl.find_first_of(ImplStr(sz), nStart, n); }
  size_t find_first_of(const wchar_t* sz, size_t nStart, size_t n) const
    { return m_impl.find_first_of(ImplStr(sz), nStart, n); }
  size_t find_first_of(wxUniChar c, size_t nStart = 0) const
    { return m_impl.find_first_of((wxChar)c, nStart); }

  size_t find_last_of(const wxString& str, size_t nStart = npos) const
    { return m_impl.find_last_of(str.m_impl, nStart); }
  size_t find_last_of(const char* sz, size_t nStart = npos) const
    { return m_impl.find_last_of(ImplStr(sz), nStart); }
  size_t find_last_of(const wchar_t* sz, size_t nStart = npos) const
    { return m_impl.find_last_of(ImplStr(sz), nStart); }
  size_t find_last_of(const char* sz, size_t nStart, size_t n) const
    { return m_impl.find_last_of(ImplStr(sz), nStart, n); }
  size_t find_last_of(const wchar_t* sz, size_t nStart, size_t n) const
    { return m_impl.find_last_of(ImplStr(sz), nStart, n); }
  size_t find_last_of(wxUniChar c, size_t nStart = npos) const
    { return m_impl.find_last_of((wxChar)c, nStart); }

  size_t find_first_not_of(const wxString& str, size_t nStart = 0) const
    { return m_impl.find_first_not_of(str.m_impl, nStart); }
  size_t find_first_not_of(const char* sz, size_t nStart = 0) const
    { return m_impl.find_first_not_of(ImplStr(sz), nStart); }
  size_t find_first_not_of(const wchar_t* sz, size_t nStart = 0) const
    { return m_impl.find_first_not_of(ImplStr(sz), nStart); }
  size_t find_first_not_of(const char* sz, size_t nStart, size_t n) const
    { return m_impl.find_first_not_of(ImplStr(sz), nStart, n); }
  size_t find_first_not_of(const wchar_t* sz, size_t nStart, size_t n) const
    { return m_impl.find_first_not_of(ImplStr(sz), nStart, n); }
  size_t find_first_not_of(wxUniChar c, size_t nStart = 0) const
    { return m_impl.find_first_not_of((wxChar)c, nStart); }

  size_t find_last_not_of(const wxString& str, size_t nStart = npos) const
    { return m_impl.find_last_not_of(str.m_impl, nStart); }
  size_t find_last_not_of(const char* sz, size_t nStart = npos) const
    { return m_impl.find_last_not_of(ImplStr(sz), nStart); }
  size_t find_last_not_of(const wchar_t* sz, size_t nStart = npos) const
    { return m_impl.find_last_not_of(ImplStr(sz), nStart); }
  size_t find_last_not_of(const char* sz, size_t nStart, size_t n) const
    { return m_impl.find_last_not_of(ImplStr(sz), nStart, n); }
  size_t find_last_not_of(const wchar_t* sz, size_t nStart, size_t n) const
    { return m_impl.find_last_not_of(ImplStr(sz), nStart, n); }
  size_t find_last_not_of(wxUniChar c, size_t nStart = npos) const
    { return m_impl.find_last_not_of((wxChar)c, nStart); }
#else
  // we can't use std::string implementation in UTF-8 build, because the
  // character sets would be interpreted wrongly:

    // as strpbrk() but starts at nStart, returns npos if not found
  size_t find_first_of(const wxString& str, size_t nStart = 0) const
#if wxUSE_UNICODE // FIXME-UTF8: temporary
    { return find_first_of(str.mb_str().data(), nStart); }
#else
    { return find_first_of((const wxChar*)str.c_str(), nStart); }
#endif
    // same as above
  size_t find_first_of(const char* sz, size_t nStart = 0) const;
  size_t find_first_of(const wchar_t* sz, size_t nStart = 0) const;
  size_t find_first_of(const char* sz, size_t nStart, size_t n) const;
  size_t find_first_of(const wchar_t* sz, size_t nStart, size_t n) const;
    // same as find(char, size_t)
  size_t find_first_of(wxUniChar c, size_t nStart = 0) const
    { return find(c, nStart); }
    // find the last (starting from nStart) char from str in this string
  size_t find_last_of (const wxString& str, size_t nStart = npos) const
#if wxUSE_UNICODE // FIXME-UTF8: temporary
    { return find_last_of(str.mb_str().data(), nStart); }
#else
    { return find_last_of((const wxChar*)str.c_str(), nStart); }
#endif
    // same as above
  size_t find_last_of (const char* sz, size_t nStart = npos) const;
  size_t find_last_of (const wchar_t* sz, size_t nStart = npos) const;
  size_t find_last_of(const char* sz, size_t nStart, size_t n) const;
  size_t find_last_of(const wchar_t* sz, size_t nStart, size_t n) const;
    // same as above
  size_t find_last_of(wxUniChar c, size_t nStart = npos) const
    { return rfind(c, nStart); }

    // find first/last occurence of any character not in the set

    // as strspn() (starting from nStart), returns npos on failure
  size_t find_first_not_of(const wxString& str, size_t nStart = 0) const
#if wxUSE_UNICODE // FIXME-UTF8: temporary
    { return find_first_not_of(str.mb_str().data(), nStart); }
#else
    { return find_first_not_of((const wxChar*)str.c_str(), nStart); }
#endif
    // same as above
  size_t find_first_not_of(const char* sz, size_t nStart = 0) const;
  size_t find_first_not_of(const wchar_t* sz, size_t nStart = 0) const;
  size_t find_first_not_of(const char* sz, size_t nStart, size_t n) const;
  size_t find_first_not_of(const wchar_t* sz, size_t nStart, size_t n) const;
    // same as above
  size_t find_first_not_of(wxUniChar ch, size_t nStart = 0) const;
    //  as strcspn()
  size_t find_last_not_of(const wxString& str, size_t nStart = npos) const
#if wxUSE_UNICODE // FIXME-UTF8: temporary
    { return find_last_not_of(str.mb_str().data(), nStart); }
#else
    { return find_last_not_of((const wxChar*)str.c_str(), nStart); }
#endif
    // same as above
  size_t find_last_not_of(const char* sz, size_t nStart = npos) const;
  size_t find_last_not_of(const wchar_t* sz, size_t nStart = npos) const;
  size_t find_last_not_of(const char* sz, size_t nStart, size_t n) const;
  size_t find_last_not_of(const wchar_t* sz, size_t nStart, size_t n) const;
    // same as above
  size_t find_last_not_of(wxUniChar ch, size_t nStart = npos) const;
#endif // wxUSE_STL_BASED_WXSTRING && !wxUSE_UNICODE_UTF8 or not

  // provide char/wchar_t/wxUniCharRef overloads for char-finding functions
  // above to resolve ambiguities:
  size_t find_first_of(wxUniCharRef ch, size_t nStart = 0) const
    {  return find_first_of(wxUniChar(ch), nStart); }
  size_t find_first_of(char ch, size_t nStart = 0) const
    {  return find_first_of(wxUniChar(ch), nStart); }
  size_t find_first_of(unsigned char ch, size_t nStart = 0) const
    {  return find_first_of(wxUniChar(ch), nStart); }
  size_t find_first_of(wchar_t ch, size_t nStart = 0) const
    {  return find_first_of(wxUniChar(ch), nStart); }
  size_t find_last_of(wxUniCharRef ch, size_t nStart = npos) const
    {  return find_last_of(wxUniChar(ch), nStart); }
  size_t find_last_of(char ch, size_t nStart = npos) const
    {  return find_last_of(wxUniChar(ch), nStart); }
  size_t find_last_of(unsigned char ch, size_t nStart = npos) const
    {  return find_last_of(wxUniChar(ch), nStart); }
  size_t find_last_of(wchar_t ch, size_t nStart = npos) const
    {  return find_last_of(wxUniChar(ch), nStart); }
  size_t find_first_not_of(wxUniCharRef ch, size_t nStart = 0) const
    {  return find_first_not_of(wxUniChar(ch), nStart); }
  size_t find_first_not_of(char ch, size_t nStart = 0) const
    {  return find_first_not_of(wxUniChar(ch), nStart); }
  size_t find_first_not_of(unsigned char ch, size_t nStart = 0) const
    {  return find_first_not_of(wxUniChar(ch), nStart); }
  size_t find_first_not_of(wchar_t ch, size_t nStart = 0) const
    {  return find_first_not_of(wxUniChar(ch), nStart); }
  size_t find_last_not_of(wxUniCharRef ch, size_t nStart = npos) const
    {  return find_last_not_of(wxUniChar(ch), nStart); }
  size_t find_last_not_of(char ch, size_t nStart = npos) const
    {  return find_last_not_of(wxUniChar(ch), nStart); }
  size_t find_last_not_of(unsigned char ch, size_t nStart = npos) const
    {  return find_last_not_of(wxUniChar(ch), nStart); }
  size_t find_last_not_of(wchar_t ch, size_t nStart = npos) const
    {  return find_last_not_of(wxUniChar(ch), nStart); }

      // string += string
  wxString& operator+=(const wxString& s)
    { m_impl += s.m_impl; return *this; }
      // string += C string
  wxString& operator+=(const char *psz)
    { m_impl += ImplStr(psz); return *this; }
  wxString& operator+=(const wchar_t *pwz)
    { m_impl += ImplStr(pwz); return *this; }
  wxString& operator+=(const wxCStrData& s)
    { m_impl += s.AsString().m_impl; return *this; }
      // string += char
  wxString& operator+=(wxUniChar ch)
    { m_impl += wxStringOperations::EncodeChar(ch); return *this; }
  wxString& operator+=(wxUniCharRef ch) { return *this += wxUniChar(ch); }
  wxString& operator+=(int ch) { return *this += wxUniChar(ch); }
  wxString& operator+=(char ch) { return *this += wxUniChar(ch); }
  wxString& operator+=(unsigned char ch) { return *this += wxUniChar(ch); }
  wxString& operator+=(wchar_t ch) { return *this += wxUniChar(ch); }

private:
#if !wxUSE_STL_BASED_WXSTRING && !wxUSE_UNICODE_UTF8
  // helpers for wxStringBuffer and wxStringBufferLength
  wxStringCharType *DoGetWriteBuf(size_t nLen)
    { return m_impl.DoGetWriteBuf(nLen); }
  void DoUngetWriteBuf()
    { m_impl.DoUngetWriteBuf(); }
  void DoUngetWriteBuf(size_t nLen)
    { m_impl.DoUngetWriteBuf(nLen); }

  friend class WXDLLIMPEXP_BASE wxStringBuffer;
  friend class WXDLLIMPEXP_BASE wxStringBufferLength;
#endif // !wxUSE_STL_BASED_WXSTRING && !wxUSE_UNICODE_UTF8

#ifndef wxNEEDS_WXSTRING_PRINTF_MIXIN
  int DoPrintf(const wxChar *format, ...) ATTRIBUTE_PRINTF_2;
  static wxString DoFormat(const wxChar *format, ...) ATTRIBUTE_PRINTF_1;
#endif

#if !wxUSE_STL_BASED_WXSTRING
  // check string's data validity
  bool IsValid() const { return m_impl.GetStringData()->IsValid(); }
#endif

private:
  wxStringImpl m_impl;

  // buffers for compatibility conversion from (char*)c_str() and
  // (wchar_t*)c_str():
  // FIXME-UTF8: bechmark various approaches to keeping compatibility buffers
  template<typename T>
  struct ConvertedBuffer
  {
      ConvertedBuffer() : m_buf(NULL) {}
      ~ConvertedBuffer()
          { free(m_buf); }

      operator T*() const { return m_buf; }

      ConvertedBuffer& operator=(T *str)
      {
          free(m_buf);
          m_buf = str;
          return *this;
      }

      T *m_buf;
  };
#if wxUSE_UNICODE
  ConvertedBuffer<char> m_convertedToChar;
#endif
#if !wxUSE_UNICODE_WCHAR
  ConvertedBuffer<wchar_t> m_convertedToWChar;
#endif
  friend class WXDLLIMPEXP_BASE wxCStrData;
};

#ifdef wxNEEDS_WXSTRING_PRINTF_MIXIN
    #pragma warning (default:4275)
#endif

// string iterator operators that satisfy STL Random Access Iterator
// requirements:
inline wxString::iterator operator+(int n, wxString::iterator i)
  { return i + n; }
inline wxString::iterator operator+(size_t n, wxString::iterator i)
  { return i + n; }
inline wxString::const_iterator operator+(int n, wxString::const_iterator i)
  { return i + n; }
inline wxString::const_iterator operator+(size_t n, wxString::const_iterator i)
  { return i + n; }
inline wxString::reverse_iterator operator+(int n, wxString::reverse_iterator i)
  { return i + n; }
inline wxString::reverse_iterator operator+(size_t n, wxString::reverse_iterator i)
  { return i + n; }
inline wxString::const_reverse_iterator operator+(int n, wxString::const_reverse_iterator i)
  { return i + n; }
inline wxString::const_reverse_iterator operator+(size_t n, wxString::const_reverse_iterator i)
  { return i + n; }

// notice that even though for many compilers the friend declarations above are
// enough, from the point of view of C++ standard we must have the declarations
// here as friend ones are not injected in the enclosing namespace and without
// them the code fails to compile with conforming compilers such as xlC or g++4
wxString WXDLLIMPEXP_BASE operator+(const wxString& string1, const wxString& string2);
wxString WXDLLIMPEXP_BASE operator+(const wxString& string, const char *psz);
wxString WXDLLIMPEXP_BASE operator+(const wxString& string, const wchar_t *pwz);
wxString WXDLLIMPEXP_BASE operator+(const char *psz, const wxString& string);
wxString WXDLLIMPEXP_BASE operator+(const wchar_t *pwz, const wxString& string);

wxString WXDLLIMPEXP_BASE operator+(const wxString& string, wxUniChar ch);
wxString WXDLLIMPEXP_BASE operator+(wxUniChar ch, const wxString& string);

inline wxString operator+(const wxString& string, wxUniCharRef ch)
    { return string + (wxUniChar)ch; }
inline wxString operator+(const wxString& string, char ch)
    { return string + wxUniChar(ch); }
inline wxString operator+(const wxString& string, wchar_t ch)
    { return string + wxUniChar(ch); }
inline wxString operator+(wxUniCharRef ch, const wxString& string)
    { return (wxUniChar)ch + string; }
inline wxString operator+(char ch, const wxString& string)
    { return wxUniChar(ch) + string; }
inline wxString operator+(wchar_t ch, const wxString& string)
    { return wxUniChar(ch) + string; }


#if wxUSE_STL_BASED_WXSTRING
    // return an empty wxString (not very useful with wxUSE_STL == 1)
    inline const wxString wxGetEmptyString() { return wxString(); }
#else // !wxUSE_STL_BASED_WXSTRING
    // return an empty wxString (more efficient than wxString() here)
    inline const wxString& wxGetEmptyString()
    {
        return *(wxString *)&wxEmptyString;
    }
#endif // wxUSE_STL_BASED_WXSTRING/!wxUSE_STL_BASED_WXSTRING

// ----------------------------------------------------------------------------
// wxStringBuffer: a tiny class allowing to get a writable pointer into string
// ----------------------------------------------------------------------------

#if wxUSE_STL_BASED_WXSTRING || wxUSE_UNICODE_UTF8

class WXDLLIMPEXP_BASE wxStringBuffer
{
public:
    wxStringBuffer(wxString& str, size_t lenWanted = 1024)
        : m_str(str), m_buf(lenWanted)
        { }

    ~wxStringBuffer() { m_str.assign(m_buf.data(), wxStrlen(m_buf.data())); }

    operator wxChar*() { return m_buf.data(); }

private:
    wxString& m_str;
#if wxUSE_UNICODE
    wxWCharBuffer m_buf;
#else
    wxCharBuffer m_buf;
#endif

    DECLARE_NO_COPY_CLASS(wxStringBuffer)
};

class WXDLLIMPEXP_BASE wxStringBufferLength
{
public:
    wxStringBufferLength(wxString& str, size_t lenWanted = 1024)
        : m_str(str), m_buf(lenWanted), m_len(0), m_lenSet(false)
        { }

    ~wxStringBufferLength()
    {
        wxASSERT(m_lenSet);
        m_str.assign(m_buf.data(), m_len);
    }

    operator wxChar*() { return m_buf.data(); }
    void SetLength(size_t length) { m_len = length; m_lenSet = true; }

private:
    wxString& m_str;
#if wxUSE_UNICODE
    wxWCharBuffer m_buf;
#else
    wxCharBuffer  m_buf;
#endif
    size_t        m_len;
    bool          m_lenSet;

    DECLARE_NO_COPY_CLASS(wxStringBufferLength)
};

#else // if !wxUSE_STL_BASED_WXSTRING && !wxUSE_UNICODE_UTF8

class WXDLLIMPEXP_BASE wxStringBuffer
{
public:
    wxStringBuffer(wxString& str, size_t lenWanted = 1024)
        : m_str(str), m_buf(NULL)
        { m_buf = m_str.DoGetWriteBuf(lenWanted); }

    ~wxStringBuffer() { m_str.DoUngetWriteBuf(); }

    operator wxChar*() const { return m_buf; }

private:
    wxString& m_str;
    wxChar   *m_buf;

    DECLARE_NO_COPY_CLASS(wxStringBuffer)
};

class WXDLLIMPEXP_BASE wxStringBufferLength
{
public:
    wxStringBufferLength(wxString& str, size_t lenWanted = 1024)
        : m_str(str), m_buf(NULL), m_len(0), m_lenSet(false)
    {
        m_buf = m_str.DoGetWriteBuf(lenWanted);
        wxASSERT(m_buf != NULL);
    }

    ~wxStringBufferLength()
    {
        wxASSERT(m_lenSet);
        m_str.DoUngetWriteBuf(m_len);
    }

    operator wxChar*() const { return m_buf; }
    void SetLength(size_t length) { m_len = length; m_lenSet = true; }

private:
    wxString& m_str;
    wxChar   *m_buf;
    size_t    m_len;
    bool      m_lenSet;

    DECLARE_NO_COPY_CLASS(wxStringBufferLength)
};

#endif // !wxUSE_STL_BASED_WXSTRING && !wxUSE_UNICODE_UTF8

// ---------------------------------------------------------------------------
// wxString comparison functions: operator versions are always case sensitive
// ---------------------------------------------------------------------------

#define wxCMP_WXCHAR_STRING(p, s, op) 0 op s.Cmp(p)

wxDEFINE_ALL_COMPARISONS(const wxChar *, const wxString&, wxCMP_WXCHAR_STRING)

#undef wxCMP_WXCHAR_STRING

// note that there is an optimization in operator==() and !=(): we (quickly)
// checks the strings length first, before comparing their data
inline bool operator==(const wxString& s1, const wxString& s2)
    { return (s1.Len() == s2.Len()) && (s1.Cmp(s2) == 0); }
inline bool operator!=(const wxString& s1, const wxString& s2)
    { return (s1.Len() != s2.Len()) || (s1.Cmp(s2) != 0); }
inline bool operator< (const wxString& s1, const wxString& s2)
    { return s1.Cmp(s2) < 0; }
inline bool operator> (const wxString& s1, const wxString& s2)
    { return s1.Cmp(s2) >  0; }
inline bool operator<=(const wxString& s1, const wxString& s2)
    { return s1.Cmp(s2) <= 0; }
inline bool operator>=(const wxString& s1, const wxString& s2)
    { return s1.Cmp(s2) >= 0; }

#if wxUSE_UNICODE
inline bool operator==(const wxString& s1, const wxWCharBuffer& s2)
    { return (s1.Cmp((const wchar_t *)s2) == 0); }
inline bool operator==(const wxWCharBuffer& s1, const wxString& s2)
    { return (s2.Cmp((const wchar_t *)s1) == 0); }
inline bool operator!=(const wxString& s1, const wxWCharBuffer& s2)
    { return (s1.Cmp((const wchar_t *)s2) != 0); }
inline bool operator!=(const wxWCharBuffer& s1, const wxString& s2)
    { return (s2.Cmp((const wchar_t *)s1) != 0); }
#else // !wxUSE_UNICODE
inline bool operator==(const wxString& s1, const wxCharBuffer& s2)
    { return (s1.Cmp((const char *)s2) == 0); }
inline bool operator==(const wxCharBuffer& s1, const wxString& s2)
    { return (s2.Cmp((const char *)s1) == 0); }
inline bool operator!=(const wxString& s1, const wxCharBuffer& s2)
    { return (s1.Cmp((const char *)s2) != 0); }
inline bool operator!=(const wxCharBuffer& s1, const wxString& s2)
    { return (s2.Cmp((const char *)s1) != 0); }
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

#if wxUSE_UNICODE
inline wxString operator+(const wxString& string, const wxWCharBuffer& buf)
    { return string + (const wchar_t *)buf; }
inline wxString operator+(const wxWCharBuffer& buf, const wxString& string)
    { return (const wchar_t *)buf + string; }
#else // !wxUSE_UNICODE
inline wxString operator+(const wxString& string, const wxCharBuffer& buf)
    { return string + (const char *)buf; }
inline wxString operator+(const wxCharBuffer& buf, const wxString& string)
    { return (const char *)buf + string; }
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

// comparison with char
inline bool operator==(const wxUniChar& c, const wxString& s) { return s.IsSameAs(c); }
inline bool operator==(const wxUniCharRef& c, const wxString& s) { return s.IsSameAs(c); }
inline bool operator==(char c, const wxString& s) { return s.IsSameAs(c); }
inline bool operator==(wchar_t c, const wxString& s) { return s.IsSameAs(c); }
inline bool operator==(int c, const wxString& s) { return s.IsSameAs(c); }
inline bool operator==(const wxString& s, const wxUniChar& c) { return s.IsSameAs(c); }
inline bool operator==(const wxString& s, const wxUniCharRef& c) { return s.IsSameAs(c); }
inline bool operator==(const wxString& s, char c) { return s.IsSameAs(c); }
inline bool operator==(const wxString& s, wchar_t c) { return s.IsSameAs(c); }
inline bool operator!=(const wxUniChar& c, const wxString& s) { return !s.IsSameAs(c); }
inline bool operator!=(const wxUniCharRef& c, const wxString& s) { return !s.IsSameAs(c); }
inline bool operator!=(char c, const wxString& s) { return !s.IsSameAs(c); }
inline bool operator!=(wchar_t c, const wxString& s) { return !s.IsSameAs(c); }
inline bool operator!=(int c, const wxString& s) { return !s.IsSameAs(c); }
inline bool operator!=(const wxString& s, const wxUniChar& c) { return !s.IsSameAs(c); }
inline bool operator!=(const wxString& s, const wxUniCharRef& c) { return !s.IsSameAs(c); }
inline bool operator!=(const wxString& s, char c) { return !s.IsSameAs(c); }
inline bool operator!=(const wxString& s, wchar_t c) { return !s.IsSameAs(c); }

// comparison with C string in Unicode build
#if wxUSE_UNICODE

#define wxCMP_CHAR_STRING(p, s, op) wxString(p) op s

wxDEFINE_ALL_COMPARISONS(const char *, const wxString&, wxCMP_CHAR_STRING)

#undef wxCMP_CHAR_STRING

#endif // wxUSE_UNICODE

// we also need to provide the operators for comparison with wxCStrData to
// resolve ambiguity between operator(const wxChar *,const wxString &) and
// operator(const wxChar *, const wxChar *) for "p == s.c_str()"
//
// notice that these are (shallow) pointer comparisons, not (deep) string ones
#define wxCMP_CHAR_CSTRDATA(p, s, op) p op s.AsChar()
#define wxCMP_WCHAR_CSTRDATA(p, s, op) p op s.AsWChar()

wxDEFINE_ALL_COMPARISONS(const wchar_t *, const wxCStrData&, wxCMP_WCHAR_CSTRDATA)
wxDEFINE_ALL_COMPARISONS(const char *, const wxCStrData&, wxCMP_CHAR_CSTRDATA)

#undef wxCMP_CHAR_CSTRDATA
#undef wxCMP_WCHAR_CSTRDATA

// ---------------------------------------------------------------------------
// Implementation only from here until the end of file
// ---------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM

#include "wx/iosfwrap.h"

WXDLLIMPEXP_BASE wxSTD ostream& operator<<(wxSTD ostream&, const wxString&);
WXDLLIMPEXP_BASE wxSTD ostream& operator<<(wxSTD ostream&, const wxCStrData&);
WXDLLIMPEXP_BASE wxSTD ostream& operator<<(wxSTD ostream&, const wxCharBuffer&);
#ifndef __BORLANDC__
WXDLLIMPEXP_BASE wxSTD ostream& operator<<(wxSTD ostream&, const wxWCharBuffer&);
#endif

#endif  // wxSTD_STRING_COMPATIBILITY

// ---------------------------------------------------------------------------
// wxCStrData implementation
// ---------------------------------------------------------------------------

inline wxCStrData::wxCStrData(char *buf)
    : m_str(new wxString(buf)), m_offset(0), m_owned(true) {}
inline wxCStrData::wxCStrData(wchar_t *buf)
    : m_str(new wxString(buf)), m_offset(0), m_owned(true) {}

inline wxCStrData::~wxCStrData()
{
    if ( m_owned )
        delete m_str;
}

inline wxCStrData::operator bool() const
{
    return !m_str->empty();
}

// simple cases for AsChar() and AsWChar(), the complicated ones are
// in string.cpp
#if wxUSE_UNICODE_WCHAR
inline const wchar_t* wxCStrData::AsWChar() const
{
    return m_str->wx_str() + m_offset;
}
#endif // wxUSE_UNICODE_WCHAR

#if !wxUSE_UNICODE
inline const char* wxCStrData::AsChar() const
{
    return m_str->wx_str() + m_offset;
}
#endif // !wxUSE_UNICODE

inline const wxCharBuffer wxCStrData::AsCharBuf() const
{
#if !wxUSE_UNICODE
    return wxCharBuffer::CreateNonOwned(AsChar());
#else
    return AsString().mb_str();
#endif
}

inline const wxWCharBuffer wxCStrData::AsWCharBuf() const
{
#if wxUSE_UNICODE_WCHAR
    return wxWCharBuffer::CreateNonOwned(AsWChar());
#else
    return AsString().wc_str();
#endif
}

inline wxString wxCStrData::AsString() const
{
    if ( m_offset == 0 )
        return *m_str;
    else
        return m_str->Mid(m_offset);
}

inline wxUniChar wxCStrData::operator*() const
{
    if ( m_str->empty() )
        return wxUniChar(_T('\0'));
    else
        return (*m_str)[m_offset];
}

inline wxUniChar wxCStrData::operator[](size_t n) const
{
    // NB: we intentionally use operator[] and not at() here because the former
    //     works for the terminating NUL while the latter does not
    return (*m_str)[m_offset + n];
}

// ----------------------------------------------------------------------------
// more wxCStrData operators
// ----------------------------------------------------------------------------

// we need to define those to allow "size_t pos = p - s.c_str()" where p is
// some pointer into the string
inline size_t operator-(const char *p, const wxCStrData& cs)
{
    return p - cs.AsChar();
}

inline size_t operator-(const wchar_t *p, const wxCStrData& cs)
{
    return p - cs.AsWChar();
}

// ----------------------------------------------------------------------------
// implementation of wx[W]CharBuffer inline methods using wxCStrData
// ----------------------------------------------------------------------------

// FIXME-UTF8: move this to buffer.h
inline wxCharBuffer::wxCharBuffer(const wxCStrData& cstr)
                    : wxCharTypeBufferBase(cstr.AsCharBuf())
{
}

inline wxWCharBuffer::wxWCharBuffer(const wxCStrData& cstr)
                    : wxCharTypeBufferBase(cstr.AsWCharBuf())
{
}

#endif  // _WX_WXSTRING_H_
