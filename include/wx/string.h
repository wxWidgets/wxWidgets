///////////////////////////////////////////////////////////////////////////////
// Name:        string.h
// Purpose:     wxString and wxArrayString classes
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WXSTRINGH__
#define _WX_WXSTRINGH__

#ifdef __GNUG__
#pragma interface "string.h"
#endif

#ifdef __WXMAC__
    #include <ctype.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <stdlib.h>

#ifdef HAVE_STRINGS_H
    #include <strings.h>    // for strcasecmp()
#endif // AIX

#ifndef  WX_PRECOMP
  #include "wx/defs.h"

  #ifdef    WXSTRING_IS_WXOBJECT
    #include "wx/object.h"
  #endif
#endif // !PCH

#include "wx/debug.h"
#include "wx/wxchar.h"
#include "wx/buffer.h"

/*
    Efficient string class [more or less] compatible with MFC CString,
    wxWindows version 1 wxString and std::string and some handy functions
    missing from string.h.
*/

// ---------------------------------------------------------------------------
// macros
// ---------------------------------------------------------------------------

// compile the std::string compatibility functions if defined
#define   wxSTD_STRING_COMPATIBILITY

// define to derive wxString from wxObject
#ifdef    WXSTRING_IS_WXOBJECT
#undef    WXSTRING_IS_WXOBJECT
#endif

// maximum possible length for a string means "take all string" everywhere
//  (as sizeof(StringData) is unknown here we substract 100)
const unsigned int wxSTRING_MAXLEN = UINT_MAX - 100;

// 'naughty' cast
#define   WXSTRINGCAST (wxChar *)(const wxChar *)
#define   MBSTRINGCAST (char *)(const char *)
#define   WCSTRINGCAST (wchar_t *)(const wchar_t *)

// implementation only
#define   ASSERT_VALID_INDEX(i) wxASSERT( (unsigned)(i) <= Len() )

// ---------------------------------------------------------------------------
// Global functions complementing standard C string library replacements for
// strlen() and portable strcasecmp()
//---------------------------------------------------------------------------
// USE wx* FUNCTIONS IN wx/wxchar.h INSTEAD - THIS IS ONLY FOR BINARY COMPATIBILITY

// checks whether the passed in pointer is NULL and if the string is empty
inline bool WXDLLEXPORT IsEmpty(const char *p) { return (!p || !*p); }

// safe version of strlen() (returns 0 if passed NULL pointer)
inline size_t WXDLLEXPORT Strlen(const char *psz)
  { return psz ? strlen(psz) : 0; }

// portable strcasecmp/_stricmp
inline int WXDLLEXPORT Stricmp(const char *psz1, const char *psz2)
{
#if     defined(__VISUALC__) || ( defined(__MWERKS__) && defined(__INTEL__) )
  return _stricmp(psz1, psz2);
#elif     defined(__SC__)
  return _stricmp(psz1, psz2);
#elif     defined(__SALFORDC__)
  return stricmp(psz1, psz2);
#elif defined(__BORLANDC__)
  return stricmp(psz1, psz2);
#elif defined(__WATCOMC__)
  return stricmp(psz1, psz2);
#elif   defined(__UNIX__) || defined(__GNUWIN32__)
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

// ----------------------------------------------------------------------------
// global data
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const wxChar*) wxEmptyString;

// global pointer to empty string
WXDLLEXPORT_DATA(extern const wxChar*) g_szNul;

// return an empty wxString
class WXDLLEXPORT wxString; // not yet defined
inline const wxString& wxGetEmptyString() { return *(wxString *)&g_szNul; }

// ---------------------------------------------------------------------------
// string data prepended with some housekeeping info (used by wxString class),
// is never used directly (but had to be put here to allow inlining)
// ---------------------------------------------------------------------------
struct WXDLLEXPORT wxStringData
{
  int     nRefs;        // reference count
  size_t  nDataLength,  // actual string length
          nAllocLength; // allocated memory size

  // mimics declaration 'wxChar data[nAllocLength]'
  wxChar* data() const { return (wxChar*)(this + 1); }

  // empty string has a special ref count so it's never deleted
  bool  IsEmpty()   const { return (nRefs == -1); }
  bool  IsShared()  const { return (nRefs > 1);   }

  // lock/unlock
  void  Lock()   { if ( !IsEmpty() ) nRefs++;                    }
  void  Unlock() { if ( !IsEmpty() && --nRefs == 0) free(this);  }

  // if we had taken control over string memory (GetWriteBuf), it's
  // intentionally put in invalid state
  void  Validate(bool b)  { nRefs = (b ? 1 : 0); }
  bool  IsValid() const   { return (nRefs != 0); }
};

// ---------------------------------------------------------------------------
// types of multibyte<->Unicode conversions
// ---------------------------------------------------------------------------
class WXDLLEXPORT wxMBConv
{
 public:
  virtual size_t MB2WC(wchar_t *buf, const char *psz, size_t n) const;
  virtual size_t WC2MB(char *buf, const wchar_t *psz, size_t n) const;
  const wxWCharBuffer cMB2WC(const char *psz) const
    {
      size_t nLen = MB2WC((wchar_t *) NULL, psz, 0);
      wxWCharBuffer buf(nLen);
      MB2WC(WCSTRINGCAST buf, psz, nLen);
      return buf;
    }
  const wxCharBuffer cWC2MB(const wchar_t *psz) const
    {
      size_t nLen = WC2MB((char *) NULL, psz, 0);
      wxCharBuffer buf(nLen);
      WC2MB(MBSTRINGCAST buf, psz, nLen);
      return buf;
    }
#if wxUSE_UNICODE
  const wxWCharBuffer cMB2WX(const char *psz) const { return cMB2WC(psz); }
  const wxCharBuffer cWX2MB(const wchar_t *psz) const { return cWC2MB(psz); }
  const wchar_t* cWC2WX(const wchar_t *psz) const { return psz; }
  const wchar_t* cMB2WC(const wchar_t *psz) const { return psz; }
#else
  const char* cMB2WX(const char *psz) const { return psz; }
  const char* cWX2MB(const char *psz) const { return psz; }
  const wxCharBuffer cWC2WX(const wchar_t *psz) const { return cWC2MB(psz); }
  const wxWCharBuffer cWX2WC(const char *psz) const { return cMB2WC(psz); }
#endif
};
WXDLLEXPORT_DATA(extern wxMBConv) wxConv_libc;

#define wxANOTHER_MBCONV(type) \
class type : public wxMBConv { \
 public: \
  virtual size_t MB2WC(wchar_t *buf, const char *psz, size_t n) const; \
  virtual size_t WC2MB(char *buf, const wchar_t *psz, size_t n) const; \
}

WXDLLEXPORT_DATA(extern wxANOTHER_MBCONV(wxMBConv_file)) wxConv_file;
WXDLLEXPORT_DATA(extern wxANOTHER_MBCONV(wxMBConv_UTF7)) wxConv_UTF7;
WXDLLEXPORT_DATA(extern wxANOTHER_MBCONV(wxMBConv_UTF8)) wxConv_UTF8;
#if defined(__WXGTK__) && (GTK_MINOR_VERSION > 0)
    WXDLLEXPORT_DATA(extern wxANOTHER_MBCONV(wxMBConv_gdk)) wxConv_gdk;
#endif // GTK > 1.0

class wxCharacterSet;
class WXDLLEXPORT wxCSConv : public wxMBConv
{
 private:
  wxChar *m_name;
  wxCharacterSet *m_cset;
  bool m_deferred;
 public:
  wxCSConv(const wxChar *charset);
  virtual ~wxCSConv();
  void LoadNow();
  virtual size_t MB2WC(wchar_t *buf, const char *psz, size_t n) const;
  virtual size_t WC2MB(char *buf, const wchar_t *psz, size_t n) const;
};

WXDLLEXPORT_DATA(extern wxCSConv) wxConv_local;

WXDLLEXPORT_DATA(extern wxMBConv *) wxConv_current;

// filenames are multibyte on Unix and probably widechar on Windows?
#ifdef __UNIX__
#define wxMBFILES 1
#else
#define wxMBFILES 0
#endif

#if wxMBFILES
#define wxFNCONV(name) wxConv_file.cWX2MB(name)
#define FNSTRINGCAST MBSTRINGCAST
#else
#define wxFNCONV(name) name
#define FNSTRINGCAST WXSTRINGCAST
#endif

// ---------------------------------------------------------------------------
// This is (yet another one) String class for C++ programmers. It doesn't use
// any of "advanced" C++ features (i.e. templates, exceptions, namespaces...)
// thus you should be able to compile it with practicaly any C++ compiler.
// This class uses copy-on-write technique, i.e. identical strings share the
// same memory as long as neither of them is changed.
//
// This class aims to be as compatible as possible with the new standard
// std::string class, but adds some additional functions and should be at
// least as efficient than the standard implementation.
//
// Performance note: it's more efficient to write functions which take "const
// String&" arguments than "const char *" if you assign the argument to
// another string.
//
// It was compiled and tested under Win32, Linux (libc 5 & 6), Solaris 5.5.
//
// To do:
//  - ressource support (string tables in ressources)
//  - more wide character (UNICODE) support
//  - regular expressions support
// ---------------------------------------------------------------------------

#ifdef  WXSTRING_IS_WXOBJECT
class WXDLLEXPORT wxString : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxString)
#else   //WXSTRING_IS_WXOBJECT
class WXDLLEXPORT wxString
{
#endif  //WXSTRING_IS_WXOBJECT

friend class WXDLLEXPORT wxArrayString;

  // NB: special care was taken in arranging the member functions in such order
  //     that all inline functions can be effectively inlined, verify that all
  //     performace critical functions are still inlined if you change order!
private:
  // points to data preceded by wxStringData structure with ref count info
  wxChar *m_pchData;

  // accessor to string data
  wxStringData* GetStringData() const { return (wxStringData*)m_pchData - 1; }

  // string (re)initialization functions
    // initializes the string to the empty value (must be called only from
    // ctors, use Reinit() otherwise)
  void Init() { m_pchData = (wxChar *)g_szNul; }
    // initializaes the string with (a part of) C-string
  void InitWith(const wxChar *psz, size_t nPos = 0, size_t nLen = wxSTRING_MAXLEN);
    // as Init, but also frees old data
  void Reinit() { GetStringData()->Unlock(); Init(); }

  // memory allocation
    // allocates memory for string of lenght nLen
  void AllocBuffer(size_t nLen);
    // copies data to another string
  void AllocCopy(wxString&, int, int) const;
    // effectively copies data to string
  void AssignCopy(size_t, const wxChar *);

  // append a (sub)string
  void ConcatSelf(int nLen, const wxChar *src);

  // functions called before writing to the string: they copy it if there
  // are other references to our data (should be the only owner when writing)
  void CopyBeforeWrite();
  void AllocBeforeWrite(size_t);

  // this method is not implemented - there is _no_ conversion from int to
  // string, you're doing something wrong if the compiler wants to call it!
  //
  // try `s << i' or `s.Printf("%d", i)' instead
  wxString(int);
  wxString(long);

public:
  // constructors and destructor
    // ctor for an empty string
  wxString() { Init(); }
    // copy ctor
  wxString(const wxString& stringSrc)
  {
    wxASSERT( stringSrc.GetStringData()->IsValid() );

    if ( stringSrc.IsEmpty() ) {
      // nothing to do for an empty string
      Init();
    }
    else {
      m_pchData = stringSrc.m_pchData;            // share same data
      GetStringData()->Lock();                    // => one more copy
    }
  }
    // string containing nRepeat copies of ch
  wxString(wxChar ch, size_t nRepeat = 1);
    // ctor takes first nLength characters from C string
    // (default value of wxSTRING_MAXLEN means take all the string)
  wxString(const wxChar *psz, size_t nLength = wxSTRING_MAXLEN)
    { InitWith(psz, 0, nLength); }
#if wxUSE_UNICODE
    // from multibyte string
    // (NB: nLength is right now number of Unicode characters, not
    //  characters in psz! So try not to use it yet!)
  wxString(const char *psz, wxMBConv& conv = wxConv_libc, size_t nLength = wxSTRING_MAXLEN);
    // from wxWCharBuffer (i.e. return from wxGetString)
  wxString(const wxWCharBuffer& psz)
    { InitWith(psz, 0, wxSTRING_MAXLEN); }
#else
    // from C string (for compilers using unsigned char)
  wxString(const unsigned char* psz, size_t nLength = wxSTRING_MAXLEN)
    { InitWith((const char*)psz, 0, nLength); }
    // from multibyte string
  wxString(const char *psz, wxMBConv& WXUNUSED(conv), size_t nLength = wxSTRING_MAXLEN)
    { InitWith(psz, 0, nLength); }
    // from wide (Unicode) string
  wxString(const wchar_t *pwz);
    // from wxCharBuffer
  wxString(const wxCharBuffer& psz)
    { InitWith(psz, 0, wxSTRING_MAXLEN); }
#endif
    // dtor is not virtual, this class must not be inherited from!
 ~wxString() { GetStringData()->Unlock(); }

  // generic attributes & operations
    // as standard strlen()
  size_t Len() const { return GetStringData()->nDataLength; }
    // string contains any characters?
  bool IsEmpty() const { return Len() == 0; }
    // empty string is "FALSE", so !str will return TRUE
  bool operator!() const { return IsEmpty(); }
    // empty string contents
  void Empty()
  {
    if ( !IsEmpty() )
      Reinit();

    // should be empty
    wxASSERT( GetStringData()->nDataLength == 0 );
  }
    // empty the string and free memory
  void Clear()
  {
    if ( !GetStringData()->IsEmpty() )
      Reinit();

    wxASSERT( GetStringData()->nDataLength == 0 );  // should be empty
    wxASSERT( GetStringData()->nAllocLength == 0 ); // and not own any memory
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
    wxChar  GetChar(size_t n) const
      { ASSERT_VALID_INDEX( n );  return m_pchData[n]; }
    // read/write access
    wxChar& GetWritableChar(size_t n)
      { ASSERT_VALID_INDEX( n ); CopyBeforeWrite(); return m_pchData[n]; }
    // write access
    void  SetChar(size_t n, wxChar ch)
      { ASSERT_VALID_INDEX( n ); CopyBeforeWrite(); m_pchData[n] = ch; }

    // get last character
    wxChar  Last() const
      { wxASSERT( !IsEmpty() ); return m_pchData[Len() - 1]; }
    // get writable last character
    wxChar& Last()
      { wxASSERT( !IsEmpty() ); CopyBeforeWrite(); return m_pchData[Len()-1]; }

    // under Unix it is tested with configure, assume it works on other
    // platforms (there might be overloading problems if size_t and int are
    // the same type)
#if !defined(__UNIX__) || wxUSE_SIZE_T_STRING_OPERATOR
    // operator version of GetChar
    wxChar  operator[](size_t n) const
      { ASSERT_VALID_INDEX( n ); return m_pchData[n]; }
#endif

    // operator version of GetChar
    wxChar  operator[](int n) const
      { ASSERT_VALID_INDEX( n ); return m_pchData[n]; }
    // operator version of GetWritableChar
    wxChar& operator[](size_t n)
      { ASSERT_VALID_INDEX( n ); CopyBeforeWrite(); return m_pchData[n]; }

    // implicit conversion to C string
    operator const wxChar*() const { return m_pchData; }
    // explicit conversion to C string (use this with printf()!)
    const wxChar* c_str()   const { return m_pchData; }
    //
    const wxChar* GetData() const { return m_pchData; }
#if wxUSE_UNICODE
    const wxCharBuffer mb_str(wxMBConv& conv = wxConv_libc) const { return conv.cWC2MB(m_pchData); }
    const wxChar* wc_str(wxMBConv& WXUNUSED(conv) = wxConv_libc) const { return m_pchData; }
#if wxMBFILES
    const wxCharBuffer fn_str() const { return mb_str(wxConv_file); }
#else
    const wxChar* fn_str() const { return m_pchData; }
#endif
#else
    const wxChar* mb_str(wxMBConv& WXUNUSED(conv) = wxConv_libc ) const { return m_pchData; }
    const wxWCharBuffer wc_str(wxMBConv& conv) const { return conv.cMB2WC(m_pchData); }
    const wxChar* fn_str() const { return m_pchData; }
#endif
    // for convenience
    const wxWX2MBbuf mbc_str() const { return mb_str(*wxConv_current); }

  // overloaded assignment
    // from another wxString
  wxString& operator=(const wxString& stringSrc);
    // from a character
  wxString& operator=(wxChar ch);
    // from a C string
  wxString& operator=(const wxChar *psz);
#if wxUSE_UNICODE
    // from wxWCharBuffer
  wxString& operator=(const wxWCharBuffer& psz) { return operator=((const wchar_t *)psz); }
#else
    // from another kind of C string
  wxString& operator=(const unsigned char* psz);
    // from a wide string
  wxString& operator=(const wchar_t *pwz);
    // from wxCharBuffer
  wxString& operator=(const wxCharBuffer& psz) { return operator=((const char *)psz); }
#endif

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
    wxASSERT( s.GetStringData()->IsValid() );

    ConcatSelf(s.Len(), s);
    return *this;
  }
      // string += C string
  wxString& operator<<(const wxChar *psz)
    { ConcatSelf(wxStrlen(psz), psz); return *this; }
      // string += char
  wxString& operator<<(wxChar ch) { ConcatSelf(1, &ch); return *this; }

      // string += string
  void operator+=(const wxString& s) { (void)operator<<(s); }
      // string += C string
  void operator+=(const wxChar *psz) { (void)operator<<(psz); }
      // string += char
  void operator+=(wxChar ch) { (void)operator<<(ch); }

      // string += buffer (i.e. from wxGetString)
#if wxUSE_UNICODE
  wxString& operator<<(const wxWCharBuffer& s) { (void)operator<<((const wchar_t *)s); return *this; }
  void operator+=(const wxWCharBuffer& s) { (void)operator<<((const wchar_t *)s); }
#else
  wxString& operator<<(const wxCharBuffer& s) { (void)operator<<((const char *)s); return *this; }
  void operator+=(const wxCharBuffer& s) { (void)operator<<((const char *)s); }
#endif

    // string += C string
  wxString& Append(const wxChar* psz)
    { ConcatSelf(wxStrlen(psz), psz); return *this; }
    // append count copies of given character
  wxString& Append(wxChar ch, size_t count = 1u)
    { wxString str(ch, count); return *this << str; }

    // prepend a string, return the string itself
  wxString& Prepend(const wxString& str)
    { *this = str + *this; return *this; }

    // non-destructive concatenation
      //
  friend wxString WXDLLEXPORT operator+(const wxString& string1,  const wxString& string2);
      //
  friend wxString WXDLLEXPORT operator+(const wxString& string, wxChar ch);
      //
  friend wxString WXDLLEXPORT operator+(wxChar ch, const wxString& string);
      //
  friend wxString WXDLLEXPORT operator+(const wxString& string, const wxChar *psz);
      //
  friend wxString WXDLLEXPORT operator+(const wxChar *psz, const wxString& string);

  // stream-like functions
      // insert an int into string
  wxString& operator<<(int i);
      // insert a float into string
  wxString& operator<<(float f);
      // insert a double into string
  wxString& operator<<(double d);

  // string comparison
    // case-sensitive comparison (returns a value < 0, = 0 or > 0)
  int  Cmp(const wxChar *psz) const { return wxStrcmp(c_str(), psz); }
    // same as Cmp() but not case-sensitive
  int  CmpNoCase(const wxChar *psz) const { return wxStricmp(c_str(), psz); }
    // test for the string equality, either considering case or not
    // (if compareWithCase then the case matters)
  bool IsSameAs(const wxChar *psz, bool compareWithCase = TRUE) const
    { return (compareWithCase ? Cmp(psz) : CmpNoCase(psz)) == 0; }

  // simple sub-string extraction
      // return substring starting at nFirst of length nCount (or till the end
      // if nCount = default value)
  wxString Mid(size_t nFirst, size_t nCount = wxSTRING_MAXLEN) const;

    // operator version of Mid()
  wxString  operator()(size_t start, size_t len) const
    { return Mid(start, len); }

      // get first nCount characters
  wxString Left(size_t nCount) const;
      // get last nCount characters
  wxString Right(size_t nCount) const;
      // get all characters before the first occurence of ch
      // (returns the whole string if ch not found)
  wxString BeforeFirst(wxChar ch) const;
      // get all characters before the last occurence of ch
      // (returns empty string if ch not found)
  wxString BeforeLast(wxChar ch) const;
      // get all characters after the first occurence of ch
      // (returns empty string if ch not found)
  wxString AfterFirst(wxChar ch) const;
      // get all characters after the last occurence of ch
      // (returns the whole string if ch not found)
  wxString AfterLast(wxChar ch) const;

    // for compatibility only, use more explicitly named functions above
  wxString Before(wxChar ch) const { return BeforeLast(ch); }
  wxString After(wxChar ch) const { return AfterFirst(ch); }

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
  wxString& Trim(bool bFromRight = TRUE);
      // add nCount copies chPad in the beginning or at the end (default)
  wxString& Pad(size_t nCount, wxChar chPad = _T(' '), bool bFromRight = TRUE);
      // truncate string to given length
  wxString& Truncate(size_t uiLen);

  // searching and replacing
      // searching (return starting index, or -1 if not found)
  int Find(wxChar ch, bool bFromEnd = FALSE) const;   // like strchr/strrchr
      // searching (return starting index, or -1 if not found)
  int Find(const wxChar *pszSub) const;               // like strstr
      // replace first (or all of bReplaceAll) occurences of substring with
      // another string, returns the number of replacements made
  size_t Replace(const wxChar *szOld,
                 const wxChar *szNew,
                 bool bReplaceAll = TRUE);

    // check if the string contents matches a mask containing '*' and '?'
  bool Matches(const wxChar *szMask) const;

  // formated input/output
    // as sprintf(), returns the number of characters written or < 0 on error
  int Printf(const wxChar *pszFormat, ...);
    // as vprintf(), returns the number of characters written or < 0 on error
  int PrintfV(const wxChar* pszFormat, va_list argptr);

  // raw access to string memory
    // ensure that string has space for at least nLen characters
    // only works if the data of this string is not shared
  void Alloc(size_t nLen);
    // minimize the string's memory
    // only works if the data of this string is not shared
  void Shrink();
    // get writable buffer of at least nLen bytes. Unget() *must* be called
    // a.s.a.p. to put string back in a reasonable state!
  wxChar *GetWriteBuf(size_t nLen);
    // call this immediately after GetWriteBuf() has been used
  void UngetWriteBuf();

  // wxWindows version 1 compatibility functions

  // use Mid()
  wxString SubString(size_t from, size_t to) const
      { return Mid(from, (to - from + 1)); }
    // values for second parameter of CompareTo function
  enum caseCompare {exact, ignoreCase};
    // values for first parameter of Strip function
  enum stripType {leading = 0x1, trailing = 0x2, both = 0x3};

    // use Printf()
  int sprintf(const wxChar *pszFormat, ...);

    // use Cmp()
  inline int CompareTo(const wxChar* psz, caseCompare cmp = exact) const
    { return cmp == exact ? Cmp(psz) : CmpNoCase(psz); }

    // use Len
  size_t Length() const { return Len(); }
    // Count the number of characters
  int Freq(wxChar ch) const;
    // use MakeLower
  void LowerCase() { MakeLower(); }
    // use MakeUpper
  void UpperCase() { MakeUpper(); }
    // use Trim except that it doesn't change this string
  wxString Strip(stripType w = trailing) const;

    // use Find (more general variants not yet supported)
  size_t Index(const wxChar* psz) const { return Find(psz); }
  size_t Index(wxChar ch)         const { return Find(ch);  }
    // use Truncate
  wxString& Remove(size_t pos) { return Truncate(pos); }
  wxString& RemoveLast() { return Truncate(Len() - 1); }

  wxString& Remove(size_t nStart, size_t nLen) { return erase( nStart, nLen ); }

    // use Find()
  int First( const wxChar ch ) const { return Find(ch); }
  int First( const wxChar* psz ) const { return Find(psz); }
  int First( const wxString &str ) const { return Find(str); }
  int Last( const wxChar ch ) const { return Find(ch, TRUE); }
  bool Contains(const wxString& str) const { return Find(str) != -1; }

    // use IsEmpty()
  bool IsNull() const { return IsEmpty(); }

#ifdef  wxSTD_STRING_COMPATIBILITY
  // std::string compatibility functions

  // an 'invalid' value for string index
  static const size_t npos;

  // constructors
    // take nLen chars starting at nPos
  wxString(const wxString& str, size_t nPos, size_t nLen)
  {
    wxASSERT( str.GetStringData()->IsValid() );
    InitWith(str.c_str(), nPos, nLen == npos ? 0 : nLen);
  }
    // take all characters from pStart to pEnd
  wxString(const void *pStart, const void *pEnd);

  // lib.string.capacity
    // return the length of the string
  size_t size() const { return Len(); }
    // return the length of the string
  size_t length() const { return Len(); }
    // return the maximum size of the string
  size_t max_size() const { return wxSTRING_MAXLEN; }
    // resize the string, filling the space with c if c != 0
  void resize(size_t nSize, wxChar ch = _T('\0'));
    // delete the contents of the string
  void clear() { Empty(); }
    // returns true if the string is empty
  bool empty() const { return IsEmpty(); }

  // lib.string.access
    // return the character at position n
  wxChar at(size_t n) const { return GetChar(n); }
    // returns the writable character at position n
  wxChar& at(size_t n) { return GetWritableChar(n); }

  // lib.string.modifiers
    // append a string
  wxString& append(const wxString& str)
    { *this += str; return *this; }
    // append elements str[pos], ..., str[pos+n]
  wxString& append(const wxString& str, size_t pos, size_t n)
    { ConcatSelf(n, str.c_str() + pos); return *this; }
    // append first n (or all if n == npos) characters of sz
  wxString& append(const wxChar *sz, size_t n = npos)
    { ConcatSelf(n == npos ? wxStrlen(sz) : n, sz); return *this; }

    // append n copies of ch
  wxString& append(size_t n, wxChar ch) { return Pad(n, ch); }

    // same as `this_string = str'
  wxString& assign(const wxString& str) { return (*this) = str; }
    // same as ` = str[pos..pos + n]
  wxString& assign(const wxString& str, size_t pos, size_t n)
    { return *this = wxString((const wxChar *)str + pos, n); }
    // same as `= first n (or all if n == npos) characters of sz'
  wxString& assign(const wxChar *sz, size_t n = npos)
    { return *this = wxString(sz, n); }
    // same as `= n copies of ch'
  wxString& assign(size_t n, wxChar ch)
    { return *this = wxString(ch, n); }

    // insert another string
  wxString& insert(size_t nPos, const wxString& str);
    // insert n chars of str starting at nStart (in str)
  wxString& insert(size_t nPos, const wxString& str, size_t nStart, size_t n)
    { return insert(nPos, wxString((const wxChar *)str + nStart, n)); }

    // insert first n (or all if n == npos) characters of sz
  wxString& insert(size_t nPos, const wxChar *sz, size_t n = npos)
    { return insert(nPos, wxString(sz, n)); }
    // insert n copies of ch
  wxString& insert(size_t nPos, size_t n, wxChar ch)
    { return insert(nPos, wxString(ch, n)); }

    // delete characters from nStart to nStart + nLen
  wxString& erase(size_t nStart = 0, size_t nLen = npos);

    // replaces the substring of length nLen starting at nStart
  wxString& replace(size_t nStart, size_t nLen, const wxChar* sz);
    // replaces the substring with nCount copies of ch
  wxString& replace(size_t nStart, size_t nLen, size_t nCount, wxChar ch);
    // replaces a substring with another substring
  wxString& replace(size_t nStart, size_t nLen,
                    const wxString& str, size_t nStart2, size_t nLen2);
    // replaces the substring with first nCount chars of sz
    wxString& replace(size_t nStart, size_t nLen,
                      const wxChar* sz, size_t nCount);

    // swap two strings
  void swap(wxString& str);

    // All find() functions take the nStart argument which specifies the
    // position to start the search on, the default value is 0. All functions
    // return npos if there were no match.

    // find a substring
  size_t find(const wxString& str, size_t nStart = 0) const;

  // VC++ 1.5 can't cope with this syntax.
#if !defined(__VISUALC__) || defined(__WIN32__)
    // find first n characters of sz
  size_t find(const wxChar* sz, size_t nStart = 0, size_t n = npos) const;
#endif

  // Gives a duplicate symbol (presumably a case-insensitivity problem)
#if !defined(__BORLANDC__)
    // find the first occurence of character ch after nStart
  size_t find(wxChar ch, size_t nStart = 0) const;
#endif
    // rfind() family is exactly like find() but works right to left

    // as find, but from the end
  size_t rfind(const wxString& str, size_t nStart = npos) const;

  // VC++ 1.5 can't cope with this syntax.
#if !defined(__VISUALC__) || defined(__WIN32__)
    // as find, but from the end
  size_t rfind(const wxChar* sz, size_t nStart = npos,
          size_t n = npos) const;
    // as find, but from the end
  size_t rfind(wxChar ch, size_t nStart = npos) const;
#endif

    // find first/last occurence of any character in the set

    //
  size_t find_first_of(const wxString& str, size_t nStart = 0) const;
    //
  size_t find_first_of(const wxChar* sz, size_t nStart = 0) const;
    // same as find(char, size_t)
  size_t find_first_of(wxChar c, size_t nStart = 0) const;
    //
  size_t find_last_of (const wxString& str, size_t nStart = npos) const;
    //
  size_t find_last_of (const wxChar* s, size_t nStart = npos) const;
    // same as rfind(char, size_t)
  size_t find_last_of (wxChar c, size_t nStart = npos) const;

    // find first/last occurence of any character not in the set

    //
  size_t find_first_not_of(const wxString& str, size_t nStart = 0) const;
    //
  size_t find_first_not_of(const wxChar* s, size_t nStart = 0) const;
    //
  size_t find_first_not_of(wxChar ch, size_t nStart = 0) const;
    //
  size_t find_last_not_of(const wxString& str, size_t nStart=npos) const;
    //
  size_t find_last_not_of(const wxChar* s, size_t nStart = npos) const;
    //
  size_t find_last_not_of(wxChar ch, size_t nStart = npos) const;

    // All compare functions return -1, 0 or 1 if the [sub]string is less,
    // equal or greater than the compare() argument.

    // just like strcmp()
  int compare(const wxString& str) const { return Cmp(str); }
    // comparison with a substring
  int compare(size_t nStart, size_t nLen, const wxString& str) const;
    // comparison of 2 substrings
  int compare(size_t nStart, size_t nLen,
              const wxString& str, size_t nStart2, size_t nLen2) const;
    // just like strcmp()
  int compare(const wxChar* sz) const { return Cmp(sz); }
    // substring comparison with first nCount characters of sz
  int compare(size_t nStart, size_t nLen,
              const wxChar* sz, size_t nCount = npos) const;

  // substring extraction
  wxString substr(size_t nStart = 0, size_t nLen = npos) const;
#endif // wxSTD_STRING_COMPATIBILITY
};

// ----------------------------------------------------------------------------
// The string array uses it's knowledge of internal structure of the wxString
// class to optimize string storage. Normally, we would store pointers to
// string, but as wxString is, in fact, itself a pointer (sizeof(wxString) is
// sizeof(char *)) we store these pointers instead. The cast to "wxString *" is
// really all we need to turn such pointer into a string!
//
// Of course, it can be called a dirty hack, but we use twice less memory and
// this approach is also more speed efficient, so it's probably worth it.
//
// Usage notes: when a string is added/inserted, a new copy of it is created,
// so the original string may be safely deleted. When a string is retrieved
// from the array (operator[] or Item() method), a reference is returned.
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxArrayString
{
public:
  // type of function used by wxArrayString::Sort()
  typedef int (*CompareFunction)(const wxString& first,
                                 const wxString& second);

  // constructors and destructor
    // default ctor
  wxArrayString();
    // copy ctor
  wxArrayString(const wxArrayString& array);
    // assignment operator
  wxArrayString& operator=(const wxArrayString& src);
    // not virtual, this class should not be derived from
 ~wxArrayString();

  // memory management
    // empties the list, but doesn't release memory
  void Empty();
    // empties the list and releases memory
  void Clear();
    // preallocates memory for given number of items
  void Alloc(size_t nCount);
    // minimzes the memory usage (by freeing all extra memory)
  void Shrink();

  // simple accessors
    // number of elements in the array
  size_t GetCount() const { return m_nCount; }
    // is it empty?
  bool IsEmpty() const { return m_nCount == 0; }
    // number of elements in the array (GetCount is preferred API)
  size_t Count() const { return m_nCount; }

  // items access (range checking is done in debug version)
    // get item at position uiIndex
  wxString& Item(size_t nIndex) const
    { wxASSERT( nIndex < m_nCount ); return *(wxString *)&(m_pItems[nIndex]); }
    // same as Item()
  wxString& operator[](size_t nIndex) const { return Item(nIndex); }
    // get last item
  wxString& Last() const { wxASSERT( !IsEmpty() ); return Item(Count() - 1); }

  // item management
    // Search the element in the array, starting from the beginning if
    // bFromEnd is FALSE or from end otherwise. If bCase, comparison is case
    // sensitive (default). Returns index of the first item matched or
    // wxNOT_FOUND
  int  Index (const wxChar *sz, bool bCase = TRUE, bool bFromEnd = FALSE) const;
    // add new element at the end
  void Add(const wxString& str);
    // add new element at given position
  void Insert(const wxString& str, size_t uiIndex);
    // remove first item matching this value
  void Remove(const wxChar *sz);
    // remove item by index
  void Remove(size_t nIndex);

  // sorting
    // sort array elements in alphabetical order (or reversed alphabetical
    // order if reverseOrder parameter is TRUE)
  void Sort(bool reverseOrder = FALSE);
    // sort array elements using specified comparaison function
  void Sort(CompareFunction compareFunction);

private:
  void    Grow();     // makes array bigger if needed
  void    Free();     // free the string stored

  void    DoSort();   // common part of all Sort() variants

  size_t  m_nSize,    // current size of the array
          m_nCount;   // current number of elements

  wxChar  **m_pItems;   // pointer to data
};

// ---------------------------------------------------------------------------
// wxString comparison functions: operator versions are always case sensitive
// ---------------------------------------------------------------------------
//
inline bool operator==(const wxString& s1, const wxString& s2) { return (s1.Cmp(s2) == 0); }
//
inline bool operator==(const wxString& s1, const wxChar  * s2) { return (s1.Cmp(s2) == 0); }
//
inline bool operator==(const wxChar  * s1, const wxString& s2) { return (s2.Cmp(s1) == 0); }
//
inline bool operator!=(const wxString& s1, const wxString& s2) { return (s1.Cmp(s2) != 0); }
//
inline bool operator!=(const wxString& s1, const wxChar  * s2) { return (s1.Cmp(s2) != 0); }
//
inline bool operator!=(const wxChar  * s1, const wxString& s2) { return (s2.Cmp(s1) != 0); }
//
inline bool operator< (const wxString& s1, const wxString& s2) { return (s1.Cmp(s2) < 0); }
//
inline bool operator< (const wxString& s1, const wxChar  * s2) { return (s1.Cmp(s2) <  0); }
//
inline bool operator< (const wxChar  * s1, const wxString& s2) { return (s2.Cmp(s1) >  0); }
//
inline bool operator> (const wxString& s1, const wxString& s2) { return (s1.Cmp(s2) >  0); }
//
inline bool operator> (const wxString& s1, const wxChar  * s2) { return (s1.Cmp(s2) >  0); }
//
inline bool operator> (const wxChar  * s1, const wxString& s2) { return (s2.Cmp(s1) <  0); }
//
inline bool operator<=(const wxString& s1, const wxString& s2) { return (s1.Cmp(s2) <= 0); }
//
inline bool operator<=(const wxString& s1, const wxChar  * s2) { return (s1.Cmp(s2) <= 0); }
//
inline bool operator<=(const wxChar  * s1, const wxString& s2) { return (s2.Cmp(s1) >= 0); }
//
inline bool operator>=(const wxString& s1, const wxString& s2) { return (s1.Cmp(s2) >= 0); }
//
inline bool operator>=(const wxString& s1, const wxChar  * s2) { return (s1.Cmp(s2) >= 0); }
//
inline bool operator>=(const wxChar  * s1, const wxString& s2) { return (s2.Cmp(s1) <= 0); }

wxString WXDLLEXPORT operator+(const wxString& string1,  const wxString& string2);
wxString WXDLLEXPORT operator+(const wxString& string, wxChar ch);
wxString WXDLLEXPORT operator+(wxChar ch, const wxString& string);
wxString WXDLLEXPORT operator+(const wxString& string, const wxChar *psz);
wxString WXDLLEXPORT operator+(const wxChar *psz, const wxString& string);
#if wxUSE_UNICODE
wxString WXDLLEXPORT operator+(const wxString& string, const wxWCharBuffer buf)
{ return string + (const wchar_t *)buf; }
#else
wxString WXDLLEXPORT operator+(const wxString& string, const wxCharBuffer buf)
{ return string + (const char *)buf; }
#endif

// ---------------------------------------------------------------------------
// Implementation only from here until the end of file
// ---------------------------------------------------------------------------

#ifdef wxSTD_STRING_COMPATIBILITY

#include "wx/ioswrap.h"

WXDLLEXPORT istream& operator>>(istream& is, wxString& str);

#endif  // wxSTD_STRING_COMPATIBILITY

#endif  // _WX_WXSTRINGH__
