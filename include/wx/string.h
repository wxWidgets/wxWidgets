/////////////////////////////////////////////////////////////////////////////
// Name:        string.h
// Purpose:     wxString class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __WXSTRINGH__
#define __WXSTRINGH__

#ifdef __GNUG__
#pragma interface "string.h"
#endif

/* Dependencies (should be included before this header):
 *         string.h
 *         stdio.h
 *         stdarg.h
 *         limits.h
 */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#ifndef  WX_PRECOMP
  #include "wx/defs.h"     // Robert Roebling
  #include "wx/object.h"
#endif

#include "wx/debug.h"

/** @name wxString library
    @memo Efficient wxString class [more or less] compatible with MFC CString,
          wxWindows wxString and std::string and some handy functions 
          missing from string.h.
  */
//@{

// ---------------------------------------------------------------------------
// macros
// ---------------------------------------------------------------------------

/** @name Macros 
    @memo You can switch off wxString/std::string compatibility if desired
 */
/// compile the std::string compatibility functions
#define   STD_STRING_COMPATIBILITY

/// define to derive wxString from wxObject
#undef    WXSTRING_IS_WXOBJECT

/// maximum possible length for a string means "take all string" everywhere
//  (as sizeof(StringData) is unknown here we substract 100)
#define   STRING_MAXLEN     (UINT_MAX - 100)

// 'naughty' cast
#define   WXSTRINGCAST (char *)(const char *)

// NB: works only inside wxString class
#define   ASSERT_VALID_INDEX(i) wxASSERT( (unsigned)(i) < Len() )

// ---------------------------------------------------------------------------
/** @name Global functions complementing standard C string library 
    @memo replacements for strlen() and portable strcasecmp()
 */
// ---------------------------------------------------------------------------

/// checks whether the passed in pointer is NULL and if the string is empty
inline bool WXDLLEXPORT IsEmpty(const char *p) { return !p || !*p; }

/// safe version of strlen() (returns 0 if passed NULL pointer)
inline size_t  WXDLLEXPORT Strlen(const char *psz)
  { return psz ? strlen(psz) : 0; }

/// portable strcasecmp/_stricmp
int WXDLLEXPORT Stricmp(const char *, const char *);

// ---------------------------------------------------------------------------
// string data prepended with some housekeeping info (used by String class),
// is never used directly (but had to be put here to allow inlining)
// ---------------------------------------------------------------------------
struct WXDLLEXPORT wxStringData
{
  int     nRefs;        // reference count
  uint    nDataLength,  // actual string length
          nAllocLength; // allocated memory size

  // mimics declaration 'char data[nAllocLength]'
  char* data() const { return (char*)(this + 1); }  

  // empty string has a special ref count so it's never deleted
  bool  IsEmpty()   const { return nRefs == -1; }
  bool  IsShared()  const { return nRefs > 1;   }

  // lock/unlock
  void  Lock()   { if ( !IsEmpty() ) nRefs++;                             }
  void  Unlock() { if ( !IsEmpty() && --nRefs == 0) delete (char *)this;  }

  // if we had taken control over string memory (GetWriteBuf), it's 
  // intentionally put in invalid state
  void  Validate(bool b)  { nRefs = b ? 1 : 0; }
  bool  IsValid() const   { return nRefs != 0; }
};

extern const char *g_szNul; // global pointer to empty string

// ---------------------------------------------------------------------------
/**
    This is (yet another one) String class for C++ programmers. It doesn't use
    any of "advanced" C++ features (i.e. templates, exceptions, namespaces...)
    thus you should be able to compile it with practicaly any C++ compiler.
    This class uses copy-on-write technique, i.e. identical strings share the
    same memory as long as neither of them is changed.

    This class aims to be as compatible as possible with the new standard
    std::string class, but adds some additional functions and should be
    at least as efficient than the standard implementation.
    
    Performance note: it's more efficient to write functions which take 
    "const String&" arguments than "const char *" if you assign the argument 
    to another string.

    It was compiled and tested under Win32, Linux (libc 5 & 6), Solaris 5.5.

    To do:
      - ressource support (string tables in ressources)
      - more wide character (UNICODE) support
      - regular expressions support
    
@memo     A non-template portable wxString class implementing copy-on-write.
@author   VZ
@version  1.3
*/
// ---------------------------------------------------------------------------
#ifdef  WXSTRING_IS_WXOBJECT
  class WXDLLEXPORT wxString : public wxObject
  {
    DECLARE_DYNAMIC_CLASS(wxString)
#else   //WXSTRING_IS_WXOBJECT
  class WXDLLEXPORT wxString
  {
#endif  //WXSTRING_IS_WXOBJECT

friend class wxArrayString;

public:
  /** @name constructors & dtor */
  //@{
    /// ctor for an empty string
  wxString();
    /// copy ctor
  wxString(const wxString& stringSrc);        
    /// string containing nRepeat copies of ch
  wxString(char ch, size_t nRepeat = 1);       
    /// ctor takes first nLength characters from C string
  wxString(const char *psz, size_t nLength = STRING_MAXLEN);
    /// from C string (for compilers using unsigned char)
  wxString(const unsigned char* psz, size_t nLength = STRING_MAXLEN);
    /// from wide (UNICODE) string
  wxString(const wchar_t *pwz);
    /// dtor is not virtual, this class must not be inherited from!
 ~wxString();
  //@}

  /** @name generic attributes & operations */
  //@{
    /// as standard strlen()
  size_t Len() const { return GetStringData()->nDataLength; }
    /// string contains any characters?
  bool IsEmpty() const;
    /// reinitialize string (and free data!)
  void Empty();
    /// Is an ascii value
  bool IsAscii() const;
    /// Is a number
  bool IsNumber() const;
    /// Is a word
  bool IsWord() const;
  //@}

  /** @name data access (all indexes are 0 based) */
  //@{
    /// read access
    char  GetChar(size_t n) const
  	  { ASSERT_VALID_INDEX( n );  return m_pchData[n]; }
    /// read/write access
    char& GetWritableChar(size_t n)
  	  { ASSERT_VALID_INDEX( n ); CopyBeforeWrite(); return m_pchData[n]; }
    /// write access
    void  SetChar(size_t n, char ch)
      { ASSERT_VALID_INDEX( n ); CopyBeforeWrite(); m_pchData[n] = ch; }

    /// get last character
    char  Last() const
      { wxASSERT( !IsEmpty() ); return m_pchData[Len() - 1]; }
    /// get writable last character
    char& Last() 
      { wxASSERT( !IsEmpty() ); CopyBeforeWrite(); return m_pchData[Len()-1]; }

    /// operator version of GetChar
    char  operator[](size_t n) const
      { ASSERT_VALID_INDEX( n ); return m_pchData[n]; }
    /// operator version of GetChar
    char  operator[](int n) const
      { ASSERT_VALID_INDEX( n ); return m_pchData[n]; }
    /// operator version of GetWritableChar
    char& operator[](size_t n)
      { ASSERT_VALID_INDEX( n ); CopyBeforeWrite(); return m_pchData[n]; }

    /// implicit conversion to C string
    operator const char*() const { return m_pchData; } 
    /// explicit conversion to C string (use this with printf()!)
    const char* c_str()   const { return m_pchData; }
    ///
    const char* GetData() const { return m_pchData; }
  //@}

  /** @name overloaded assignment */
  //@{
    ///
  wxString& operator=(const wxString& stringSrc);
    ///
  wxString& operator=(char ch);
    ///
  wxString& operator=(const char *psz);
    ///
  wxString& operator=(const unsigned char* psz);
    ///
  wxString& operator=(const wchar_t *pwz);
  //@}
  
  /** @name string concatenation */
  //@{
    /** @name in place concatenation */
    //@{
      /// string += string
  void operator+=(const wxString& string);
      /// string += C string
  void operator+=(const char *psz);
      /// string += char
  void operator+=(char ch);
    //@}
    /** @name concatenate and return the result
        left to right associativity of << allows to write 
        things like "str << str1 << str2 << ..."          */
    //@{
      /// as +=
  wxString& operator<<(const wxString& string);
      /// as +=
  wxString& operator<<(char ch);
      /// as +=
  wxString& operator<<(const char *psz);
    //@}
    
    /** @name return resulting string */
    //@{
      ///
  friend wxString operator+(const wxString& string1,  const wxString& string2);
      ///
  friend wxString operator+(const wxString& string, char ch);
      ///
  friend wxString operator+(char ch, const wxString& string);
      ///
  friend wxString operator+(const wxString& string, const char *psz);
      ///
  friend wxString operator+(const char *psz, const wxString& string);
    //@}
  //@}
  
  /** @name string comparison */
  //@{
    /** 
    case-sensitive comparaison
    @return 0 if equal, +1 if greater or -1 if less
    @see CmpNoCase, IsSameAs
    */
  int  Cmp(const char *psz) const { return strcmp(c_str(), psz); }
    /**
    case-insensitive comparaison, return code as for wxString::Cmp()
    @see: Cmp, IsSameAs
    */
  int  CmpNoCase(const char *psz) const { return Stricmp(c_str(), psz); }
    /**
    test for string equality, case-sensitive (default) or not
    @param   bCase is TRUE by default (case matters)
    @return  TRUE if strings are equal, FALSE otherwise
    @see     Cmp, CmpNoCase
    */
  bool IsSameAs(const char *psz, bool bCase = TRUE) const 
    { return !(bCase ? Cmp(psz) : CmpNoCase(psz)); }
  //@}
  
  /** @name other standard string operations */
  //@{
    /** @name simple sub-string extraction
     */
    //@{
      /** 
      return substring starting at nFirst of length 
      nCount (or till the end if nCount = default value)
      */
  wxString Mid(size_t nFirst, size_t nCount = STRING_MAXLEN) const;  
      /// get first nCount characters
  wxString Left(size_t nCount) const;
      /// get all characters before the first occurence of ch
      /// (returns the whole string if ch not found)
  wxString Left(char ch) const;
      /// get all characters before the last occurence of ch
      /// (returns empty string if ch not found)
  wxString Before(char ch) const;
      /// get all characters after the first occurence of ch
      /// (returns empty string if ch not found)
  wxString After(char ch) const;
      /// get last nCount characters
  wxString Right(size_t nCount) const;
      /// get all characters after the last occurence of ch
      /// (returns the whole string if ch not found)
  wxString Right(char ch) const;
    //@}
    
    /** @name case conversion */
    //@{ 
      ///
  wxString& MakeUpper();
      ///
  wxString& MakeLower();
    //@}

    /** @name trimming/padding whitespace (either side) and truncating */
    //@{
      /// remove spaces from left or from right (default) side
  wxString& Trim(bool bFromRight = TRUE);
      /// add nCount copies chPad in the beginning or at the end (default)
  wxString& Pad(size_t nCount, char chPad = ' ', bool bFromRight = TRUE);
      /// truncate string to given length
  wxString& Truncate(size_t uiLen);
    //@}
    
    /** @name searching and replacing */
    //@{
      /// searching (return starting index, or -1 if not found)
  int Find(char ch, bool bFromEnd = FALSE) const;   // like strchr/strrchr
      /// searching (return starting index, or -1 if not found)
  int Find(const char *pszSub) const;               // like strstr
      /**
      replace first (or all) occurences of substring with another one
      @param  bReplaceAll: global replace (default) or only the first occurence
      @return the number of replacements made
      */
  uint Replace(const char *szOld, const char *szNew, bool bReplaceAll = TRUE);
    //@}

    /// check if the string contents matches a mask containing '*' and '?'
  bool Matches(const char *szMask) const;
  //@}

  /** @name formated input/output */
  //@{
    /// as sprintf(), returns the number of characters written or < 0 on error
  int Printf(const char *pszFormat, ...);
    /// as vprintf(), returns the number of characters written or < 0 on error
  int PrintfV(const char* pszFormat, va_list argptr);
  //@}
  
  /** @name raw access to string memory */
  //@{
    /** 
        get writable buffer of at least nLen bytes. 
        Unget() *must* be called a.s.a.p. to put string back in a reasonable
        state!
     */
  char *GetWriteBuf(int nLen);
    /// call this immediately after GetWriteBuf() has been used
  void UngetWriteBuf();
  //@}

  /** @name wxWindows compatibility functions */
  //@{
    /// values for second parameter of CompareTo function
  enum caseCompare {exact, ignoreCase};
    /// values for first parameter of Strip function
  enum stripType {leading = 0x1, trailing = 0x2, both = 0x3};
    /// same as Printf()
  inline int sprintf(const char *pszFormat, ...)
  {
    va_list argptr;
    va_start(argptr, pszFormat);
    int iLen = PrintfV(pszFormat, argptr);
    va_end(argptr);
    return iLen;
  }

    /// same as Cmp
  inline int CompareTo(const char* psz, caseCompare cmp = exact) const
  { return cmp == exact ? Cmp(psz) : CmpNoCase(psz); }

    /// same as Mid (substring extraction)
  inline wxString  operator()(size_t start, size_t len) const { return Mid(start, len); }

    /// same as += or <<
  inline wxString& Append(const char* psz) { return *this << psz; }
  inline wxString& Append(char ch, int count = 1) { wxString str(ch, count); (*this) += str; return *this; }

    ///
  wxString& Prepend(const wxString& str) { *this = str + *this; return *this; }
    /// same as Len
  size_t Length() const { return Len(); }
    /// same as MakeLower
  void LowerCase() { MakeLower(); }
    /// same as MakeUpper
  void UpperCase() { MakeUpper(); }
    /// same as Trim except that it doesn't change this string
  wxString Strip(stripType w = trailing) const;

    /// same as Find (more general variants not yet supported)
  size_t Index(const char* psz) const { return Find(psz); }
  size_t Index(char ch)         const { return Find(ch);  }
    /// same as Truncate
  wxString& Remove(size_t pos) { return Truncate(pos); }
  wxString& RemoveLast() { return Truncate(Len() - 1); }

  // Robert Roebling
  
  wxString& Remove(size_t nStart, size_t nLen) { return erase( nStart, nLen ); }
  
  size_t First( const char ch ) const { return find(ch); }
  size_t First( const char* psz ) const { return find(psz); }
  size_t First( const wxString &str ) const { return find(str); }

  size_t Last( const char ch ) const { return rfind(ch,0); }
  size_t Last( const char* psz ) const { return rfind(psz,0); }
  size_t Last( const wxString &str ) const { return rfind(str,0); }

    /// same as IsEmpty
  bool IsNull() const { return IsEmpty(); }
  //@}

#ifdef  STD_STRING_COMPATIBILITY
  /** @name std::string compatibility functions */
  
  /// an 'invalid' value for string index
  static const size_t npos;
        
  //@{
    /** @name constructors */
    //@{
      /// take nLen chars starting at nPos
      wxString(const wxString& s, size_t nPos, size_t nLen = npos);
      /// take all characters from pStart to pEnd
      wxString(const void *pStart, const void *pEnd);
    //@}
    /** @name lib.string.capacity */
    //@{
      /// return the length of the string
      size_t size() const { return Len(); }
      /// return the length of the string
      size_t length() const { return Len(); }
      /// return the maximum size of the string
      size_t max_size() const { return STRING_MAXLEN; } 
      /// resize the string, filling the space with c if c != 0
      void resize(size_t nSize, char ch = '\0');
      /// delete the contents of the string
      void clear() { Empty(); }
      /// returns true if the string is empty
      bool empty() const { return IsEmpty(); }
    //@}
    /** @name lib.string.access */
    //@{
      /// return the character at position n
      char at(size_t n) const { return GetChar(n); }
      /// returns the writable character at position n
      char& at(size_t n) { return GetWritableChar(n); }
    //@}
    /** @name lib.string.modifiers */
    //@{
      /** @name append something to the end of this one */
      //@{
        /// append a string
        wxString& append(const wxString& str) 
          { *this += str; return *this; }
        /// append elements str[pos], ..., str[pos+n]
        wxString& append(const wxString& str, size_t pos, size_t n) 
          { ConcatSelf(n, str.c_str() + pos); return *this; }
        /// append first n (or all if n == npos) characters of sz
        wxString& append(const char *sz, size_t n = npos) 
          { ConcatSelf(n == npos ? Strlen(sz) : n, sz); return *this; }

        /// append n copies of ch
        wxString& append(size_t n, char ch) { return Pad(n, ch); }
      //@}
        
      /** @name replaces the contents of this string with another one */
      //@{
        /// same as `this_string = str'
        wxString& assign(const wxString& str) { return (*this) = str; }
        /// same as ` = str[pos..pos + n]
        wxString& assign(const wxString& str, size_t pos, size_t n) 
          { return *this = wxString((const char *)str + pos, n); }
        /// same as `= first n (or all if n == npos) characters of sz'
        wxString& assign(const char *sz, size_t n = npos) 
          { return *this = wxString(sz, n); }
        /// same as `= n copies of ch'
        wxString& assign(size_t n, char ch) 
          { return *this = wxString(ch, n); }

      //@}
        
      /** @name inserts something at position nPos into this one */  
      //@{
        /// insert another string
        wxString& insert(size_t nPos, const wxString& str);
        /// insert n chars of str starting at nStart (in str)
        wxString& insert(size_t nPos, const wxString& str, size_t nStart, size_t n)
	  	    { return insert(nPos, wxString((const char *)str + nStart, n)); }

        /// insert first n (or all if n == npos) characters of sz
        wxString& insert(size_t nPos, const char *sz, size_t n = npos)
          { return insert(nPos, wxString(sz, n)); }
        /// insert n copies of ch
        wxString& insert(size_t nPos, size_t n, char ch) 
          { return insert(nPos, wxString(ch, n)); }

      //@}
      
      /** @name deletes a part of the string */
      //@{
        /// delete characters from nStart to nStart + nLen
        wxString& erase(size_t nStart = 0, size_t nLen = npos);
      //@}
      
      /** @name replaces a substring of this string with another one */
      //@{
         /// replaces the substring of length nLen starting at nStart
         wxString& replace(size_t nStart, size_t nLen, const char* sz);
         /// replaces the substring with nCount copies of ch
         wxString& replace(size_t nStart, size_t nLen, size_t nCount, char ch);
         /// replaces a substring with another substring
         wxString& replace(size_t nStart, size_t nLen, 
                         const wxString& str, size_t nStart2, size_t nLen2);
         /// replaces the substring with first nCount chars of sz
         wxString& replace(size_t nStart, size_t nLen, 
                         const char* sz, size_t nCount);
      //@}
    //@}
         
    /// swap two strings
    void swap(wxString& str);

    /** @name string operations */
    //@{
      /** All find() functions take the nStart argument which specifies
          the position to start the search on, the default value is 0.
          
          All functions return npos if there were no match.
          
          @name string search 
      */
      //@{
        /**
            @name find a match for the string/character in this string 
        */
        //@{
          /// find a substring
          size_t find(const wxString& str, size_t nStart = 0) const;

          // VC++ 1.5 can't cope with this syntax.
#if ! (defined(_MSC_VER) && !defined(__WIN32__))
          /// find first n characters of sz
          size_t find(const char* sz, size_t nStart = 0, size_t n = npos) const;
#endif
          /// find the first occurence of character ch after nStart
          size_t find(char ch, size_t nStart = 0) const;

		  // wxWin compatibility
		  inline bool Contains(const wxString& str) { return (Find(str) != -1); }

        //@}
        
        /** 
          @name rfind() family is exactly like find() but works right to left
        */
        //@{
        /// as find, but from the end
        size_t rfind(const wxString& str, size_t nStart = npos) const;
        /// as find, but from the end
        // VC++ 1.5 can't cope with this syntax.
#if ! (defined(_MSC_VER) && !defined(__WIN32__))
        size_t rfind(const char* sz, size_t nStart = npos, 
                     size_t n = npos) const;
        /// as find, but from the end
        size_t rfind(char ch, size_t nStart = npos) const;
#endif
        //@}
        
        /**
          @name find first/last occurence of any character in the set
        */
        //@{
          ///
          size_t find_first_of(const wxString& str, size_t nStart = 0) const;
          ///
          size_t find_first_of(const char* sz, size_t nStart = 0) const;
          /// same as find(char, size_t)
          size_t find_first_of(char c, size_t nStart = 0) const;
          
          ///
          size_t find_last_of (const wxString& str, size_t nStart = npos) const;
          ///
          size_t find_last_of (const char* s, size_t nStart = npos) const;
          /// same as rfind(char, size_t)
          size_t find_last_of (char c, size_t nStart = npos) const;
        //@}
        
        /**
          @name find first/last occurence of any character not in the set
        */
        //@{
          ///
          size_t find_first_not_of(const wxString& str, size_t nStart = 0) const;
          ///
          size_t find_first_not_of(const char* s, size_t nStart = 0) const;
          ///
          size_t find_first_not_of(char ch, size_t nStart = 0) const;
          
          ///
          size_t find_last_not_of(const wxString& str, size_t nStart=npos) const;
          ///
          size_t find_last_not_of(const char* s, size_t nStart = npos) const;
          ///
          size_t find_last_not_of(char ch, size_t nStart = npos) const;
        //@}
      //@}
      
      /** 
        All compare functions return -1, 0 or 1 if the [sub]string 
        is less, equal or greater than the compare() argument.
        
        @name comparison
      */
      //@{
        /// just like strcmp()
        int compare(const wxString& str) const { return Cmp(str); }
        /// comparaison with a substring
        int compare(size_t nStart, size_t nLen, const wxString& str) const;
        /// comparaison of 2 substrings
        int compare(size_t nStart, size_t nLen,
                    const wxString& str, size_t nStart2, size_t nLen2) const;
        /// just like strcmp()
        int compare(const char* sz) const { return Cmp(sz); }
        /// substring comparaison with first nCount characters of sz
        int compare(size_t nStart, size_t nLen,
                    const char* sz, size_t nCount = npos) const;
      //@}
    wxString substr(size_t nStart = 0, size_t nLen = npos) const;
  //@}
#endif
    
protected:
  // points to data preceded by wxStringData structure with ref count info
  char *m_pchData;

  // accessor to string data
  wxStringData* GetStringData() const { return (wxStringData*)m_pchData - 1; }

  // string (re)initialization functions
    // initializes the string to the empty value (must be called only from
    // ctors, use Reinit() otherwise)
  void Init() { m_pchData = (char *)g_szNul; }
    // initializaes the string with (a part of) C-string
  void InitWith(const char *psz, size_t nPos = 0, size_t nLen = STRING_MAXLEN);
    // as Init, but also frees old data
  inline void Reinit(); 

  // memory allocation
    // allocates memory for string of lenght nLen
  void AllocBuffer(size_t nLen);
    // copies data to another string
  void AllocCopy(wxString&, int, int) const;
    // effectively copies data to string
  void AssignCopy(size_t, const char *);
  
  // append a (sub)string
  void ConcatCopy(int nLen1, const char *src1, int nLen2, const char *src2);
  void ConcatSelf(int nLen, const char *src);

  // functions called before writing to the string: they copy it if there 
  // other references (should be the only owner when writing)
  void CopyBeforeWrite();
  void AllocBeforeWrite(size_t);
};

// ----------------------------------------------------------------------------
/** The string array uses it's knowledge of internal structure of the String
    class to optimize string storage. Normally, we would store pointers to
    string, but as String is, in fact, itself a pointer (sizeof(String) is
    sizeof(char *)) we store these pointers instead. The cast to "String *"
    is really all we need to turn such pointer into a string!

    Of course, it can be called a dirty hack, but we use twice less memory 
    and this approach is also more speed efficient, so it's probably worth it.

    Usage notes: when a string is added/inserted, a new copy of it is created,
    so the original string may be safely deleted. When a string is retrieved
    from the array (operator[] or Item() method), a reference is returned.

    @name wxArrayString
    @memo probably the most commonly used array type - array of strings
 */
// ----------------------------------------------------------------------------
class wxArrayString
{
public:
  /** @name ctors and dtor */
  //@{
    /// default ctor
  wxArrayString();
    /// copy ctor
  wxArrayString(const wxArrayString& array);
    /// assignment operator
  wxArrayString& operator=(const wxArrayString& src);
    /// not virtual, this class can't be derived from
 ~wxArrayString();
  //@}

  /** @name memory management */
  //@{
    /// empties the list, but doesn't release memory
  void Empty();
    /// empties the list and releases memory
  void Clear();
    /// preallocates memory for given number of items
  void Alloc(size_t nCount);
  //@}

  /** @name simple accessors */
  //@{
    /// number of elements in the array
  uint  Count() const   { return m_nCount;      }
    /// is it empty?
  bool  IsEmpty() const { return m_nCount == 0; }
  //@}

  /** @name items access (range checking is done in debug version) */
  //@{
    /// get item at position uiIndex
  wxString& Item(size_t nIndex) const
    { wxASSERT( nIndex < m_nCount ); return *(wxString *)&(m_pItems[nIndex]); }
    /// same as Item()
  wxString& operator[](size_t nIndex) const { return Item(nIndex); }
    /// get last item
  wxString& Last() const { wxASSERT( !IsEmpty() ); return Item(Count() - 1); }
  //@}

  /** @name item management */
  //@{
    /**
      Search the element in the array, starting from the either side
      @param if bFromEnd reverse search direction
      @param if bCase, comparaison is case sensitive (default)
      @return index of the first item matched or NOT_FOUND
      @see NOT_FOUND
     */
  int  Index (const char *sz, bool bCase = TRUE, bool bFromEnd = FALSE) const;
    /// add new element at the end
  void Add   (const wxString& str);
    /// add new element at given position
  void Insert(const wxString& str, uint uiIndex);
    /// remove first item matching this value
  void Remove(const char *sz);
    /// remove item by index
  void Remove(size_t nIndex);
  //@}

  /// sort array elements
  void Sort(bool bCase = TRUE, bool bReverse = FALSE);

private:
  void    Grow();     // makes array bigger if needed
  void    Free();     // free the string stored

  size_t  m_nSize,    // current size of the array
          m_nCount;   // current number of elements

  char  **m_pItems;   // pointer to data
};

// ---------------------------------------------------------------------------
// implementation of inline functions
// ---------------------------------------------------------------------------

// Put back into class, since BC++ can't create precompiled header otherwise

// ---------------------------------------------------------------------------
/** @name wxString comparaison functions 
    @memo Comparaisons are case sensitive
 */
// ---------------------------------------------------------------------------
//@{
inline bool operator==(const wxString& s1, const wxString& s2) { return s1.Cmp(s2) == 0; }
///
inline bool operator==(const wxString& s1, const char  * s2) { return s1.Cmp(s2) == 0; }
///
inline bool operator==(const char  * s1, const wxString& s2) { return s2.Cmp(s1) == 0; }
///
inline bool operator!=(const wxString& s1, const wxString& s2) { return s1.Cmp(s2) != 0; }
///
inline bool operator!=(const wxString& s1, const char  * s2) { return s1.Cmp(s2) != 0; }
///
inline bool operator!=(const char  * s1, const wxString& s2) { return s2.Cmp(s1) != 0; }
///
inline bool operator< (const wxString& s1, const wxString& s2) { return s1.Cmp(s2) <  0; }
///
inline bool operator< (const wxString& s1, const char  * s2) { return s1.Cmp(s2) <  0; }
///
inline bool operator< (const char  * s1, const wxString& s2) { return s2.Cmp(s1) >  0; }
///
inline bool operator> (const wxString& s1, const wxString& s2) { return s1.Cmp(s2) >  0; }
///
inline bool operator> (const wxString& s1, const char  * s2) { return s1.Cmp(s2) >  0; }
///
inline bool operator> (const char  * s1, const wxString& s2) { return s2.Cmp(s1) <  0; }
///
inline bool operator<=(const wxString& s1, const wxString& s2) { return s1.Cmp(s2) <= 0; }
///
inline bool operator<=(const wxString& s1, const char  * s2) { return s1.Cmp(s2) <= 0; }
///
inline bool operator<=(const char  * s1, const wxString& s2) { return s2.Cmp(s1) >= 0; }
///
inline bool operator>=(const wxString& s1, const wxString& s2) { return s1.Cmp(s2) >= 0; }
///
inline bool operator>=(const wxString& s1, const char  * s2) { return s1.Cmp(s2) >= 0; }
///
inline bool operator>=(const char  * s1, const wxString& s2) { return s2.Cmp(s1) <= 0; }
//@}
    
// ---------------------------------------------------------------------------
/** @name Global functions complementing standard C string library 
    @memo replacements for strlen() and portable strcasecmp()
 */
// ---------------------------------------------------------------------------

#ifdef  STD_STRING_COMPATIBILITY

// fwd decl
class WXDLLEXPORT istream;

istream& WXDLLEXPORT operator>>(istream& is, wxString& str);

#endif  //std::string compatibility

#endif  // __WXSTRINGH__

//@}
