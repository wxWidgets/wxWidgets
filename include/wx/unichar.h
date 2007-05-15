///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unichar.h
// Purpose:     wxUniChar and wxUniCharRef classes
// Author:      Vaclav Slavik
// Created:     2007-03-19
// RCS-ID:      $Id$
// Copyright:   (c) 2007 REA Elektronik GmbH
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNICHAR_H_
#define _WX_UNICHAR_H_

#include "wx/defs.h"
#include "wx/chartype.h"
#include "wx/stringimpl.h"

// wint_t is just a typedef for wchar_t for many old compilers but for modern
// ones it's a separate type and we must provide a conversion to it to allow
// passing wxUniChar[Ref] to functions taking wint_t such as iswalnum() &c
#if (defined(__GNUC__) && \
        !defined(__DARWIN__) && !defined(__OS2__) && !defined(__DOS__)) || \
    (defined(__VISUALC__) && defined(_NATIVE_WCHAR_T_DEFINED))
    #define wxWINT_T_IS_SEPARATE_TYPE
#endif

// helper macro for doing something dependent on whether wint_t is or isn't a
// typedef inside another macro
#ifdef wxWINT_T_IS_SEPARATE_TYPE
    #define wxIF_WINT_T_TYPE(x) x
#else // !wxWINT_T_IS_SEPARATE_TYPE
    #define wxIF_WINT_T_TYPE(x)
#endif // wxWINT_T_IS_SEPARATE_TYPE/!wxWINT_T_IS_SEPARATE_TYPE

// wchar_t seems to be defined as unsigned short by all Windows compilers but
// unsigned int everywhere else
#ifndef __WIN32__
    #define wxWCHAR_T_IS_UINT
#endif

class WXDLLIMPEXP_BASE wxUniCharRef;
class WXDLLIMPEXP_BASE wxStringIteratorNode;

// This class represents single Unicode character. It can be converted to
// and from char or wchar_t and implements commonly used character operations.
class WXDLLIMPEXP_BASE wxUniChar
{
public:
    // NB: this is not wchar_t on purpose, it needs to represent the entire
    //     Unicode code points range and wchar_t may be too small for that
    //     (e.g. on Win32 where wchar_t* is encoded in UTF-16)
    typedef wxUint32 value_type;

    wxUniChar() : m_value(0) {}

    // Create the character from 8bit character value encoded in the current
    // locale's charset.
    wxUniChar(char c) { m_value = From8bit(c); }
    wxUniChar(unsigned char c) { m_value = From8bit((char)c); }

    // Create the character from a wchar_t character value.
    wxUniChar(wchar_t c) { m_value = c; }
#ifdef wxWINT_T_IS_SEPARATE_TYPE
    wxUniChar(wint_t c) { m_value = c; }
#endif

    wxUniChar(int c) { m_value = c; }

    wxUniChar(const wxUniCharRef& c);

    // Returns Unicode code point value of the character
    value_type GetValue() const { return m_value; }

    // Returns true if the character is an ASCII character:
    bool IsAscii() const { return m_value < 0x80; }

    // Conversions to char and wchar_t types: all of those are needed to be
    // able to pass wxUniChars to verious standard narrow and wide character
    // functions
    operator char() const { return To8bit(m_value); }
    operator wchar_t() const { return m_value; }
    operator int() const { return m_value; }
#ifndef wxWCHAR_T_IS_UINT
    operator unsigned int() const { return m_value; }
#endif

    // More conversions needed for other standard functions: uchar is for VC++
    // _mbxxx() ones (to which toxxx/isxxx() are mapped when _MBCS is defined)
    // and some wide character functions take wint_t which happens to be the
    // same as wchar_t for Windows compilers but not for g++ (except for the
    // special Apple version)
    operator unsigned char() const { return (unsigned char)To8bit(m_value); }
#ifdef wxWINT_T_IS_SEPARATE_TYPE
    operator wint_t() const { return m_value; }
#endif

    // We need this operator for the "*p" part of expressions like "for (
    // const_iterator p = begin() + nStart; *p; ++p )". In this case,
    // compilation would fail without it because the conversion to bool would
    // be ambiguous (there are all these int types conversions...). (And adding
    // operator unspecified_bool_type() would only makes the ambiguity worse.)
    operator bool() const { return m_value != 0; }
    bool operator!() const { return !((bool)*this); }
#if (defined(__VISUALC__) && __VISUALC__ < 1400) || \
    defined(__DIGITALMARS__) || defined(__BORLANDC__)
    // We need this for VC++ < 8 or DigitalMars and expressions like
    // "str[0] && *p":
    bool operator&&(bool v) const { return (bool)*this && v; }
#endif

    // Assignment operators:
    wxUniChar& operator=(const wxUniChar& c) { m_value = c.m_value; return *this; }
    wxUniChar& operator=(char c) { m_value = From8bit(c); return *this; }
    wxUniChar& operator=(wchar_t c) { m_value = c; return *this; }
    wxUniChar& operator=(int c) { m_value = c; return *this; }
#ifdef wxWINT_T_IS_SEPARATE_TYPE
    wxUniChar& operator=(wint_t c) { m_value = c; return *this; }
#endif

    // Comparison operators:

    // define the given comparison operator for all the types
#define wxDEFINE_UNICHAR_OPERATOR(op)                                         \
    bool operator op(const wxUniChar& c) const { return m_value op c.m_value; }\
    bool operator op(char c) const { return m_value op From8bit(c); }         \
    bool operator op(wchar_t c) const { return m_value op (value_type)c; }    \
    wxIF_WINT_T_TYPE( bool operator op(wint_t c) const { return m_value op (value_type)c; } )

    wxFOR_ALL_COMPARISONS(wxDEFINE_UNICHAR_OPERATOR)

#undef wxDEFINE_UNICHAR_OPERATOR

    // this is needed for expressions like 'Z'-c
    int operator-(const wxUniChar& c) const { return m_value - c.m_value; }
    int operator-(char c) const { return m_value - From8bit(c); }
    int operator-(wchar_t c) const { return m_value - (value_type)c; }
#ifdef wxWINT_T_IS_SEPARATE_TYPE
    int operator-(wint_t c) const { return m_value - (value_type)c; }
#endif

private:
    static value_type From8bit(char c);
    static char To8bit(value_type c);

private:
    value_type m_value;
};


// Writeable reference to a character in wxString.
//
// This class can be used in the same way wxChar is used, except that changing
// its value updates the underlying string object.
class WXDLLIMPEXP_BASE wxUniCharRef
{
private:
    typedef wxStringImpl::iterator iterator;

    // create the reference
#if wxUSE_UNICODE_UTF8
    wxUniCharRef(wxStringIteratorNode& node, iterator pos) : m_node(node), m_pos(pos) {}
#else
    wxUniCharRef(iterator pos) : m_pos(pos) {}
#endif

public:
    // NB: we have to make this public, because we don't have wxString
    //     declaration available here and so can't declare wxString::iterator
    //     as friend; so at least don't use a ctor but a static function
    //     that must be used explicitly (this is more than using 'explicit'
    //     keyword on ctor!):
#if wxUSE_UNICODE_UTF8
    static wxUniCharRef CreateForString(wxStringIteratorNode& node, iterator pos)
        { return wxUniCharRef(node, pos); }
#else
    static wxUniCharRef CreateForString(iterator pos)
        { return wxUniCharRef(pos); }
#endif

    wxUniChar::value_type GetValue() const { return UniChar().GetValue(); }
    bool IsAscii() const { return UniChar().IsAscii(); }

    // Assignment operators:
#if wxUSE_UNICODE_UTF8
    wxUniCharRef& operator=(const wxUniChar& c);
#else
    wxUniCharRef& operator=(const wxUniChar& c) { *m_pos = c; return *this; }
#endif

    wxUniCharRef& operator=(const wxUniCharRef& c)
        { return *this = c.UniChar(); }

    wxUniCharRef& operator=(char c) { return *this = wxUniChar(c); }
    wxUniCharRef& operator=(wchar_t c) { return *this = wxUniChar(c); }
    wxUniCharRef& operator=(int c) { return *this = wxUniChar(c); }
#ifdef wxWINT_T_IS_SEPARATE_TYPE
    wxUniCharRef& operator=(wint_t c) { return *this = wxUniChar(c); }
#endif

    // Conversions to the same types as wxUniChar is convertible too:
    operator char() const { return UniChar(); }
    operator int() const { return UniChar(); }
    operator unsigned char() const { return UniChar(); }
    operator wchar_t() const { return UniChar(); }
#ifdef wxWINT_T_IS_SEPARATE_TYPE
    operator wint_t() const { return UniChar(); }
#endif
#ifndef wxWCHAR_T_IS_UINT
    operator unsigned int() const { return UniChar(); }
#endif

    // see wxUniChar::operator bool etc. for explanation
    operator bool() const { return (bool)UniChar(); }
    bool operator!() const { return !UniChar(); }
#if (defined(__VISUALC__) && __VISUALC__ < 1400) || \
    defined(__DIGITALMARS__) || defined(__BORLANDC__)
    bool operator&&(bool v) const { return UniChar() && v; }
#endif

    // Comparison operators:
#define wxDEFINE_UNICHARREF_OPERATOR(op)                                      \
    bool operator op(const wxUniCharRef& c) const { return UniChar() op c.UniChar(); }\
    bool operator op(const wxUniChar& c) const { return UniChar() op c; }     \
    bool operator op(char c) const { return UniChar() op c; }                 \
    bool operator op(wchar_t c) const { return UniChar() op c; }              \
    wxIF_WINT_T_TYPE( bool operator op(wint_t c) const { return UniChar() op c; } )

    wxFOR_ALL_COMPARISONS(wxDEFINE_UNICHARREF_OPERATOR)

#undef wxDEFINE_UNICHARREF_OPERATOR

    // for expressions like c-'A':
    int operator-(const wxUniCharRef& c) const { return UniChar() - c.UniChar(); }
    int operator-(const wxUniChar& c) const { return UniChar() - c; }
    int operator-(char c) const { return UniChar() - c; }
    int operator-(wchar_t c) const { return UniChar() - c; }
#ifdef wxWINT_T_IS_SEPARATE_TYPE
    int operator-(wint_t c) const { return UniChar() - c; }
#endif

private:
#if wxUSE_UNICODE_UTF8
    wxUniChar UniChar() const;
#else
    wxUniChar UniChar() const { return *m_pos; }
#endif

    friend class WXDLLIMPEXP_BASE wxUniChar;

private:
    // reference to the string and pointer to the character in string
#if wxUSE_UNICODE_UTF8
    wxStringIteratorNode& m_node;
#endif
    iterator m_pos;
};

inline wxUniChar::wxUniChar(const wxUniCharRef& c)
{
    m_value = c.UniChar().m_value;
}

// Comparison operators for the case when wxUniChar(Ref) is the second operand
// implemented in terms of member comparison functions

#define wxCMP_REVERSE(c1, c2, op) c2 op c1

wxDEFINE_COMPARISONS(char, const wxUniChar&, wxCMP_REVERSE)
wxDEFINE_COMPARISONS(char, const wxUniCharRef&, wxCMP_REVERSE)

wxDEFINE_COMPARISONS(wchar_t, const wxUniChar&, wxCMP_REVERSE)
wxDEFINE_COMPARISONS(wchar_t, const wxUniCharRef&, wxCMP_REVERSE)

#ifdef wxWINT_T_IS_SEPARATE_TYPE
//wxDEFINE_COMPARISONS(wint_t, const wxUniChar&, wxCMP_REVERSE)
wxDEFINE_COMPARISONS(wint_t, const wxUniCharRef&, wxCMP_REVERSE)
#endif

wxDEFINE_COMPARISONS(const wxUniChar&, const wxUniCharRef&, wxCMP_REVERSE)

#undef wxCMP_REVERSE

// for expressions like c-'A':
inline int operator-(char c1, const wxUniCharRef& c2) { return -(c2 - c1); }
inline int operator-(const wxUniChar& c1, const wxUniCharRef& c2) { return -(c2 - c1); }
inline int operator-(wchar_t c1, const wxUniCharRef& c2) { return -(c2 - c1); }
#ifdef wxWINT_T_IS_SEPARATE_TYPE
inline int operator-(wint_t c1, const wxUniCharRef& c2) { return -(c2 - c1); }
#endif

#endif /* _WX_UNICHAR_H_ */
