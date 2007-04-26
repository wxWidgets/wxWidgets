///////////////////////////////////////////////////////////////////////////////
// Name:        wx/strvararg.h
// Purpose:     macros for implementing type-safe vararg passing of strings
// Author:      Vaclav Slavik
// Created:     2007-02-19
// RCS-ID:      $Id$
// Copyright:   (c) 2007 REA Elektronik GmbH
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STRVARARG_H_
#define _WX_STRVARARG_H_

#include "wx/platform.h"
#if wxONLY_WATCOM_EARLIER_THAN(1,4)
    #error "OpenWatcom version >= 1.4 is required to compile this code"
#endif

#include "wx/cpp.h"
#include "wx/chartype.h"
#include "wx/wxcrt.h"
#include "wx/strconv.h"
#include "wx/buffer.h"

class WXDLLIMPEXP_BASE wxCStrData;
class WXDLLIMPEXP_BASE wxString;

// ----------------------------------------------------------------------------
// WX_DEFINE_VARARG_FUNC* macros
// ----------------------------------------------------------------------------

// This macro is used to implement type-safe wrappers for variadic functions
// that accept strings as arguments. This makes it possible to pass char*,
// wchar_t* or even wxString (as opposed to having to use wxString::c_str())
// to e.g. wxPrintf().
//
// This is done by defining a set of N template function taking 1..N arguments
// (currently, N is set to 30 in this header). These functions are just thin
// wrappers around another variadic function ('impl' or 'implUtf8' arguments,
// see below) and the only thing the wrapper does is that it normalizes the
// arguments passed in so that they are of the type expected by variadic
// functions taking string arguments, i.e., char* or wchar_t*, depending on the
// build:
//   * char* in the current locale's charset in ANSI build
//   * char* with UTF-8 encoding if wxUSE_UNICODE_UTF8 and the app is running
//     under an UTF-8 locale
//   * wchar_t* if wxUSE_UNICODE_WCHAR or if wxUSE_UNICODE_UTF8 and the current
//     locale is not UTF-8
//
// Parameters:
// [ there are examples in square brackets showing values of the parameters
//   for the wxFprintf() wrapper for fprintf() function with the following
//   prototype:
//   int wxFprintf(FILE *stream, const wxString& format, ...); ]
//
//        rettype   Functions' return type  [int]
//        name      Name of the function  [fprintf]
//        numfixed  The number of leading "fixed" (i.e., not variadic)
//                  arguments of the function (e.g. "stream" and "format"
//                  arguments of fprintf()); their type is _not_ converted
//                  using wxArgNormalizer<T>, unlike the rest of
//                  the function's arguments  [2]
//        fixed     List of types of the leading "fixed" arguments, in
//                  parenthesis  [(FILE*,const wxString&)]
//        impl      Name of the variadic function that implements 'name' for
//                  the native strings representation (wchar_t* if
//                  wxUSE_UNICODE_WCHAR or wxUSE_UNICODE_UTF8 when running under
//                  non-UTF8 locale, char* in ANSI build)  [wxCrt_Fprintf]
//        implUtf8  Like 'impl', but for the UTF-8 char* version to be used
//                  if wxUSE_UNICODE_UTF8 and running under UTF-8 locale
//                  (ignored otherwise)  [fprintf]
//
#define WX_DEFINE_VARARG_FUNC2(rettype, name, numfixed, fixed, impl, implUtf8)\
    _WX_VARARG_DEFINE_FUNC_N0(rettype, name, impl, implUtf8, numfixed, fixed) \
    WX_DEFINE_VARARG_FUNC2_SANS_N0(rettype, name, numfixed, fixed, impl, implUtf8)

// ditto, but without the version with 0 template/vararg arguments
#define WX_DEFINE_VARARG_FUNC2_SANS_N0(rettype, name,                         \
                                       numfixed, fixed, impl, implUtf8)       \
    _WX_VARARG_ITER(_WX_VARARG_MAX_ARGS,                                      \
                    _WX_VARARG_DEFINE_FUNC,                                   \
                    rettype, name, impl, implUtf8, numfixed, fixed)

// like WX_DEFINE_VARARG_FUNC2, but for impl=implUtf8:
#define WX_DEFINE_VARARG_FUNC(rettype, name, numfixed, fixed, impl)           \
    WX_DEFINE_VARARG_FUNC2(rettype, name, numfixed, fixed, impl, impl)

// Like WX_DEFINE_VARARG_FUNC2, but for variadic functions that don't return
// a value.
#define WX_DEFINE_VARARG_FUNC_VOID2(name, numfixed, fixed, impl, implUtf8)    \
    _WX_VARARG_DEFINE_FUNC_VOID_N0(name, impl, implUtf8, numfixed, fixed)     \
    _WX_VARARG_ITER(_WX_VARARG_MAX_ARGS,                                      \
                    _WX_VARARG_DEFINE_FUNC_VOID,                              \
                    void, name, impl, implUtf8, numfixed, fixed)

// like WX_DEFINE_VARARG_FUNC_VOID2, but for impl=implUtf8:
#define WX_DEFINE_VARARG_FUNC_VOID(name, numfixed, fixed, impl)               \
    WX_DEFINE_VARARG_FUNC_VOID2(name, numfixed, fixed, impl, impl)

// Like WX_DEFINE_VARARG_FUNC_VOID, but instead of wrapping an implementation
// function, does nothing in defined functions' bodies.
//
// Used to implement wxLogXXX functions if wxUSE_LOG=0.
#define WX_DEFINE_VARARG_FUNC_NOP(name, numfixed, fixed)                      \
        _WX_VARARG_DEFINE_FUNC_NOP_N0(name, numfixed, fixed)                  \
        _WX_VARARG_ITER(_WX_VARARG_MAX_ARGS,                                  \
                        _WX_VARARG_DEFINE_FUNC_NOP,                           \
                        void, name, dummy, numfixed, fixed)

// Like WX_DEFINE_VARARG_FUNC_CTOR, but for defining template constructors
#define WX_DEFINE_VARARG_FUNC_CTOR(name, numfixed, fixed, impl)               \
    _WX_VARARG_DEFINE_FUNC_CTOR_N0(name, impl, impl, numfixed, fixed)         \
    _WX_VARARG_ITER(_WX_VARARG_MAX_ARGS,                                      \
                    _WX_VARARG_DEFINE_FUNC_CTOR,                              \
                    void, name, impl, impl, numfixed, fixed)

// ----------------------------------------------------------------------------
// wxArgNormalizer*<T> converters
// ----------------------------------------------------------------------------

// Converts an argument passed to wxPrint etc. into standard form expected,
// by wxXXX functions, e.g. all strings (wxString, char*, wchar_t*) are
// converted into wchar_t* or char* depending on the build.
template<typename T>
struct wxArgNormalizer
{
    wxArgNormalizer(const T& value) : m_value(value) {}

    // Returns the value in a form that can be safely passed to real vararg
    // functions. In case of strings, this is char* in ANSI build and wchar_t*
    // in Unicode build.
    const T& get() const { return m_value; }

    const T& m_value;
};

// normalizer for passing arguments to functions working with wchar_t* (and
// until ANSI build is removed, char* in ANSI build as well - FIXME-UTF8)
// string representation
#if !wxUSE_UTF8_LOCALE_ONLY
template<typename T>
struct wxArgNormalizerWchar : public wxArgNormalizer<T>
{
    wxArgNormalizerWchar(const T& value) : wxArgNormalizer<T>(value) {}
};
#endif // !wxUSE_UTF8_LOCALE_ONLY

// normalizer for passing arguments to functions working with UTF-8 encoded
// char* strings
#if wxUSE_UNICODE_UTF8
    template<typename T>
    struct wxArgNormalizerUtf8 : public wxArgNormalizer<T>
    {
        wxArgNormalizerUtf8(const T& value) : wxArgNormalizer<T>(value) {}
    };

    #define wxArgNormalizerNative wxArgNormalizerUtf8
#else // wxUSE_UNICODE_WCHAR
    #define wxArgNormalizerNative wxArgNormalizerWchar
#endif // wxUSE_UNICODE_UTF8 // wxUSE_UNICODE_UTF8



// special cases for converting strings:


// base class for wxArgNormalizer<T> specializations that need to do conversion;
// CharType is either wxStringCharType or wchar_t in UTF-8 build when wrapping
// widechar CRT function
template<typename CharType>
struct wxArgNormalizerWithBuffer
{
    typedef wxCharTypeBuffer<CharType> CharBuffer;

    wxArgNormalizerWithBuffer() {}
    wxArgNormalizerWithBuffer(const CharBuffer& buf) : m_value(buf) {}

    const CharType *get() const { return m_value; }

    CharBuffer m_value;
};

// string objects:
template<>
struct WXDLLIMPEXP_BASE wxArgNormalizerNative<const wxString&>
{
    wxArgNormalizerNative(const wxString& s) : m_value(s) {}
    const wxStringCharType *get() const;

    const wxString& m_value;
};

// c_str() values:
template<>
struct WXDLLIMPEXP_BASE wxArgNormalizerNative<const wxCStrData&>
{
    wxArgNormalizerNative(const wxCStrData& value) : m_value(value) {}
    const wxStringCharType *get() const;

    const wxCStrData& m_value;
};

// wxString/wxCStrData conversion to wchar_t* value
#if wxUSE_UNICODE_UTF8 && !wxUSE_UTF8_LOCALE_ONLY
template<>
struct WXDLLIMPEXP_BASE wxArgNormalizerWchar<const wxString&>
    : public wxArgNormalizerWithBuffer<wchar_t>
{
    wxArgNormalizerWchar(const wxString& s);
};

template<>
struct WXDLLIMPEXP_BASE wxArgNormalizerWchar<const wxCStrData&>
    : public wxArgNormalizerWithBuffer<wchar_t>
{
    wxArgNormalizerWchar(const wxCStrData& s);
};
#endif // wxUSE_UNICODE_UTF8 && !wxUSE_UTF8_LOCALE_ONLY


// C string pointers of the wrong type (wchar_t* for ANSI or UTF8 build,
// char* for wchar_t Unicode build or UTF8):
#if wxUSE_UNICODE_WCHAR

template<>
struct wxArgNormalizerWchar<const char*>
    : public wxArgNormalizerWithBuffer<wchar_t>
{
    wxArgNormalizerWchar(const char* s)
        : wxArgNormalizerWithBuffer<wchar_t>(wxConvLibc.cMB2WC(s)) {}
};

#elif wxUSE_UNICODE_UTF8

template<>
struct wxArgNormalizerUtf8<const wchar_t*>
    : public wxArgNormalizerWithBuffer<char>
{
    wxArgNormalizerUtf8(const wchar_t* s)
        : wxArgNormalizerWithBuffer<char>(wxConvUTF8.cWC2MB(s)) {}
};

template<>
struct wxArgNormalizerUtf8<const char*>
    : public wxArgNormalizerWithBuffer<char>
{
    wxArgNormalizerUtf8(const char* s)
    {
        // FIXME-UTF8: optimize this if current locale is UTF-8 one

        // convert to widechar string first:
        wxWCharBuffer buf(wxConvLibc.cMB2WC(s));

        // then to UTF-8:
        if ( buf )
            m_value = wxConvUTF8.cWC2MB(buf);
    }
};

// UTF-8 build needs conversion to wchar_t* too:
#if !wxUSE_UTF8_LOCALE_ONLY
template<>
struct wxArgNormalizerWchar<const char*>
    : public wxArgNormalizerWithBuffer<wchar_t>
{
    wxArgNormalizerWchar(const char* s)
        : wxArgNormalizerWithBuffer<wchar_t>(wxConvLibc.cMB2WC(s)) {}
};
#endif // !wxUSE_UTF8_LOCALE_ONLY

#else // ANSI - FIXME-UTF8

template<>
struct wxArgNormalizerWchar<const wchar_t*>
    : public wxArgNormalizerWithBuffer<char>
{
    wxArgNormalizerWchar(const wchar_t* s)
        : wxArgNormalizerWithBuffer<char>(wxConvLibc.cWC2MB(s)) {}
};

#endif // wxUSE_UNICODE_WCHAR/wxUSE_UNICODE_UTF8/ANSI


// this macro is used to implement specialization that are exactly same as
// some other specialization, i.e. to "forward" the implementation (e.g. for
// T=wxString and T=const wxString&). Note that the ctor takes BaseT argument,
// not T!
#if wxUSE_UNICODE_UTF8
    #if wxUSE_UTF8_LOCALE_ONLY
        #define WX_ARG_NORMALIZER_FORWARD(T, BaseT)                         \
          _WX_ARG_NORMALIZER_FORWARD_IMPL(wxArgNormalizerUtf8, T, BaseT)
    #else // possibly non-UTF8 locales
        #define WX_ARG_NORMALIZER_FORWARD(T, BaseT)                         \
          _WX_ARG_NORMALIZER_FORWARD_IMPL(wxArgNormalizerWchar, T, BaseT);  \
          _WX_ARG_NORMALIZER_FORWARD_IMPL(wxArgNormalizerUtf8, T, BaseT)
    #endif
#else // wxUSE_UNICODE_WCHAR
    #define WX_ARG_NORMALIZER_FORWARD(T, BaseT)                             \
        _WX_ARG_NORMALIZER_FORWARD_IMPL(wxArgNormalizerWchar, T, BaseT)
#endif // wxUSE_UNICODE_UTF8/wxUSE_UNICODE_WCHAR

#define _WX_ARG_NORMALIZER_FORWARD_IMPL(Normalizer, T, BaseT)               \
    template<>                                                              \
    struct Normalizer<T> : public Normalizer<BaseT>                         \
    {                                                                       \
        Normalizer(BaseT value) : Normalizer<BaseT>(value) {}               \
    }

// non-reference versions of specializations for string objects
WX_ARG_NORMALIZER_FORWARD(wxString, const wxString&);
WX_ARG_NORMALIZER_FORWARD(wxCStrData, const wxCStrData&);

// versions for passing non-const pointers:
WX_ARG_NORMALIZER_FORWARD(char*, const char*);
WX_ARG_NORMALIZER_FORWARD(wchar_t*, const wchar_t*);

// versions for passing wx[W]CharBuffer:
WX_ARG_NORMALIZER_FORWARD(wxCharBuffer, const char*);
WX_ARG_NORMALIZER_FORWARD(const wxCharBuffer&, const char*);
WX_ARG_NORMALIZER_FORWARD(wxWCharBuffer, const wchar_t*);
WX_ARG_NORMALIZER_FORWARD(const wxWCharBuffer&, const wchar_t*);

#undef WX_ARG_NORMALIZER_FORWARD
#undef _WX_ARG_NORMALIZER_FORWARD_IMPL

// ----------------------------------------------------------------------------
// WX_VA_ARG_STRING
// ----------------------------------------------------------------------------

// Replacement for va_arg() for use with strings in functions that accept
// strings normalized by wxArgNormalizer<T>:

struct WXDLLIMPEXP_BASE wxArgNormalizedString
{
    wxArgNormalizedString(const void* ptr) : m_ptr(ptr) {}

    // returns true if non-NULL string was passed in
    bool IsValid() const { return m_ptr != NULL; }
    operator bool() const { return IsValid(); }

    // extracts the string, returns empty string if NULL was passed in
    wxString GetString() const;
    operator wxString() const;

private:
    const void *m_ptr;
};

#define WX_VA_ARG_STRING(ap) wxArgNormalizedString(va_arg(ap, const void*))

// ----------------------------------------------------------------------------
// implementation of the WX_DEFINE_VARARG_* macros
// ----------------------------------------------------------------------------

// NB: The vararg emulation code is limited to 30 variadic and 4 fixed
//     arguments at the moment.
//     If you need more variadic arguments, you need to
//        1) increase the value of _WX_VARARG_MAX_ARGS
//        2) add _WX_VARARG_JOIN_* and _WX_VARARG_ITER_* up to the new
//           _WX_VARARG_MAX_ARGS value to the lists below
//     If you need more fixed arguments, you need to
//        1) increase the value of _WX_VARARG_MAX_FIXED_ARGS
//        2) add _WX_VARARG_FIXED_EXPAND_* and _WX_VARARG_FIXED_UNUSED_EXPAND_*
//           macros below
#define _WX_VARARG_MAX_ARGS        30
#define _WX_VARARG_MAX_FIXED_ARGS   4

#define _WX_VARARG_JOIN_1(m)                                 m(1)
#define _WX_VARARG_JOIN_2(m)       _WX_VARARG_JOIN_1(m),     m(2)
#define _WX_VARARG_JOIN_3(m)       _WX_VARARG_JOIN_2(m),     m(3)
#define _WX_VARARG_JOIN_4(m)       _WX_VARARG_JOIN_3(m),     m(4)
#define _WX_VARARG_JOIN_5(m)       _WX_VARARG_JOIN_4(m),     m(5)
#define _WX_VARARG_JOIN_6(m)       _WX_VARARG_JOIN_5(m),     m(6)
#define _WX_VARARG_JOIN_7(m)       _WX_VARARG_JOIN_6(m),     m(7)
#define _WX_VARARG_JOIN_8(m)       _WX_VARARG_JOIN_7(m),     m(8)
#define _WX_VARARG_JOIN_9(m)       _WX_VARARG_JOIN_8(m),     m(9)
#define _WX_VARARG_JOIN_10(m)      _WX_VARARG_JOIN_9(m),     m(10)
#define _WX_VARARG_JOIN_11(m)      _WX_VARARG_JOIN_10(m),    m(11)
#define _WX_VARARG_JOIN_12(m)      _WX_VARARG_JOIN_11(m),    m(12)
#define _WX_VARARG_JOIN_13(m)      _WX_VARARG_JOIN_12(m),    m(13)
#define _WX_VARARG_JOIN_14(m)      _WX_VARARG_JOIN_13(m),    m(14)
#define _WX_VARARG_JOIN_15(m)      _WX_VARARG_JOIN_14(m),    m(15)
#define _WX_VARARG_JOIN_16(m)      _WX_VARARG_JOIN_15(m),    m(16)
#define _WX_VARARG_JOIN_17(m)      _WX_VARARG_JOIN_16(m),    m(17)
#define _WX_VARARG_JOIN_18(m)      _WX_VARARG_JOIN_17(m),    m(18)
#define _WX_VARARG_JOIN_19(m)      _WX_VARARG_JOIN_18(m),    m(19)
#define _WX_VARARG_JOIN_20(m)      _WX_VARARG_JOIN_19(m),    m(20)
#define _WX_VARARG_JOIN_21(m)      _WX_VARARG_JOIN_20(m),    m(21)
#define _WX_VARARG_JOIN_22(m)      _WX_VARARG_JOIN_21(m),    m(22)
#define _WX_VARARG_JOIN_23(m)      _WX_VARARG_JOIN_22(m),    m(23)
#define _WX_VARARG_JOIN_24(m)      _WX_VARARG_JOIN_23(m),    m(24)
#define _WX_VARARG_JOIN_25(m)      _WX_VARARG_JOIN_24(m),    m(25)
#define _WX_VARARG_JOIN_26(m)      _WX_VARARG_JOIN_25(m),    m(26)
#define _WX_VARARG_JOIN_27(m)      _WX_VARARG_JOIN_26(m),    m(27)
#define _WX_VARARG_JOIN_28(m)      _WX_VARARG_JOIN_27(m),    m(28)
#define _WX_VARARG_JOIN_29(m)      _WX_VARARG_JOIN_28(m),    m(29)
#define _WX_VARARG_JOIN_30(m)      _WX_VARARG_JOIN_29(m),    m(30)

#define _WX_VARARG_ITER_1(m,a,b,c,d,e,f)                                    m(1,a,b,c,d,e,f)
#define _WX_VARARG_ITER_2(m,a,b,c,d,e,f)  _WX_VARARG_ITER_1(m,a,b,c,d,e,f)  m(2,a,b,c,d,e,f)
#define _WX_VARARG_ITER_3(m,a,b,c,d,e,f)  _WX_VARARG_ITER_2(m,a,b,c,d,e,f)  m(3,a,b,c,d,e,f)
#define _WX_VARARG_ITER_4(m,a,b,c,d,e,f)  _WX_VARARG_ITER_3(m,a,b,c,d,e,f)  m(4,a,b,c,d,e,f)
#define _WX_VARARG_ITER_5(m,a,b,c,d,e,f)  _WX_VARARG_ITER_4(m,a,b,c,d,e,f)  m(5,a,b,c,d,e,f)
#define _WX_VARARG_ITER_6(m,a,b,c,d,e,f)  _WX_VARARG_ITER_5(m,a,b,c,d,e,f)  m(6,a,b,c,d,e,f)
#define _WX_VARARG_ITER_7(m,a,b,c,d,e,f)  _WX_VARARG_ITER_6(m,a,b,c,d,e,f)  m(7,a,b,c,d,e,f)
#define _WX_VARARG_ITER_8(m,a,b,c,d,e,f)  _WX_VARARG_ITER_7(m,a,b,c,d,e,f)  m(8,a,b,c,d,e,f)
#define _WX_VARARG_ITER_9(m,a,b,c,d,e,f)  _WX_VARARG_ITER_8(m,a,b,c,d,e,f)  m(9,a,b,c,d,e,f)
#define _WX_VARARG_ITER_10(m,a,b,c,d,e,f) _WX_VARARG_ITER_9(m,a,b,c,d,e,f)  m(10,a,b,c,d,e,f)
#define _WX_VARARG_ITER_11(m,a,b,c,d,e,f) _WX_VARARG_ITER_10(m,a,b,c,d,e,f) m(11,a,b,c,d,e,f)
#define _WX_VARARG_ITER_12(m,a,b,c,d,e,f) _WX_VARARG_ITER_11(m,a,b,c,d,e,f) m(12,a,b,c,d,e,f)
#define _WX_VARARG_ITER_13(m,a,b,c,d,e,f) _WX_VARARG_ITER_12(m,a,b,c,d,e,f) m(13,a,b,c,d,e,f)
#define _WX_VARARG_ITER_14(m,a,b,c,d,e,f) _WX_VARARG_ITER_13(m,a,b,c,d,e,f) m(14,a,b,c,d,e,f)
#define _WX_VARARG_ITER_15(m,a,b,c,d,e,f) _WX_VARARG_ITER_14(m,a,b,c,d,e,f) m(15,a,b,c,d,e,f)
#define _WX_VARARG_ITER_16(m,a,b,c,d,e,f) _WX_VARARG_ITER_15(m,a,b,c,d,e,f) m(16,a,b,c,d,e,f)
#define _WX_VARARG_ITER_17(m,a,b,c,d,e,f) _WX_VARARG_ITER_16(m,a,b,c,d,e,f) m(17,a,b,c,d,e,f)
#define _WX_VARARG_ITER_18(m,a,b,c,d,e,f) _WX_VARARG_ITER_17(m,a,b,c,d,e,f) m(18,a,b,c,d,e,f)
#define _WX_VARARG_ITER_19(m,a,b,c,d,e,f) _WX_VARARG_ITER_18(m,a,b,c,d,e,f) m(19,a,b,c,d,e,f)
#define _WX_VARARG_ITER_20(m,a,b,c,d,e,f) _WX_VARARG_ITER_19(m,a,b,c,d,e,f) m(20,a,b,c,d,e,f)
#define _WX_VARARG_ITER_21(m,a,b,c,d,e,f) _WX_VARARG_ITER_20(m,a,b,c,d,e,f) m(21,a,b,c,d,e,f)
#define _WX_VARARG_ITER_22(m,a,b,c,d,e,f) _WX_VARARG_ITER_21(m,a,b,c,d,e,f) m(22,a,b,c,d,e,f)
#define _WX_VARARG_ITER_23(m,a,b,c,d,e,f) _WX_VARARG_ITER_22(m,a,b,c,d,e,f) m(23,a,b,c,d,e,f)
#define _WX_VARARG_ITER_24(m,a,b,c,d,e,f) _WX_VARARG_ITER_23(m,a,b,c,d,e,f) m(24,a,b,c,d,e,f)
#define _WX_VARARG_ITER_25(m,a,b,c,d,e,f) _WX_VARARG_ITER_24(m,a,b,c,d,e,f) m(25,a,b,c,d,e,f)
#define _WX_VARARG_ITER_26(m,a,b,c,d,e,f) _WX_VARARG_ITER_25(m,a,b,c,d,e,f) m(26,a,b,c,d,e,f)
#define _WX_VARARG_ITER_27(m,a,b,c,d,e,f) _WX_VARARG_ITER_26(m,a,b,c,d,e,f) m(27,a,b,c,d,e,f)
#define _WX_VARARG_ITER_28(m,a,b,c,d,e,f) _WX_VARARG_ITER_27(m,a,b,c,d,e,f) m(28,a,b,c,d,e,f)
#define _WX_VARARG_ITER_29(m,a,b,c,d,e,f) _WX_VARARG_ITER_28(m,a,b,c,d,e,f) m(29,a,b,c,d,e,f)
#define _WX_VARARG_ITER_30(m,a,b,c,d,e,f) _WX_VARARG_ITER_29(m,a,b,c,d,e,f) m(30,a,b,c,d,e,f)


#define _WX_VARARG_FIXED_EXPAND_1(t1) \
         t1 f1
#define _WX_VARARG_FIXED_EXPAND_2(t1,t2) \
         t1 f1, t2 f2
#define _WX_VARARG_FIXED_EXPAND_3(t1,t2,t3) \
         t1 f1, t2 f2, t3 f3
#define _WX_VARARG_FIXED_EXPAND_4(t1,t2,t3,t4) \
         t1 f1, t2 f2, t3 f3, t4 f4

#define _WX_VARARG_FIXED_UNUSED_EXPAND_1(t1) \
         t1 WXUNUSED(f1)
#define _WX_VARARG_FIXED_UNUSED_EXPAND_2(t1,t2) \
         t1 WXUNUSED(f1), t2 WXUNUSED(f2)
#define _WX_VARARG_FIXED_UNUSED_EXPAND_3(t1,t2,t3) \
         t1 WXUNUSED(f1), t2 WXUNUSED(f2), t3 WXUNUSED(f3)
#define _WX_VARARG_FIXED_UNUSED_EXPAND_4(t1,t2,t3,t4) \
         t1 WXUNUSED(f1), t2 WXUNUSED(f2), t3 WXUNUSED(f3), t4 WXUNUSED(f4)


// This macro expands N-items tuple of fixed arguments types into part of
// function's declaration. For example,
// "_WX_VARARG_FIXED_EXPAND(3, (int, char*, int))" expands into
// "int f1, char* f2, int f3".
#define _WX_VARARG_FIXED_EXPAND(N, args) \
                _WX_VARARG_FIXED_EXPAND_IMPL(N, args)
#define _WX_VARARG_FIXED_EXPAND_IMPL(N, args) \
                _WX_VARARG_FIXED_EXPAND_##N args

// Ditto for unused arguments
#define _WX_VARARG_FIXED_UNUSED_EXPAND(N, args) \
                _WX_VARARG_FIXED_UNUSED_EXPAND_IMPL(N, args)
#define _WX_VARARG_FIXED_UNUSED_EXPAND_IMPL(N, args) \
                _WX_VARARG_FIXED_UNUSED_EXPAND_##N args


// This macro calls another macro 'm' passed as second argument 'N' times,
// with its only argument set to 1..N, and concatenates the results using
// comma as separator.
//
// An example:
//     #define foo(i)  x##i
//     // this expands to "x1,x2,x3,x4"
//     _WX_VARARG_JOIN(4, foo)
//
//
// N must not be greater than _WX_VARARG_MAX_ARGS (=30).
#define _WX_VARARG_JOIN(N, m)             _WX_VARARG_JOIN_IMPL(N, m)
#define _WX_VARARG_JOIN_IMPL(N, m)        _WX_VARARG_JOIN_##N(m)


// This macro calls another macro 'm' passed as second argument 'N' times, with
// its first argument set to 1..N and the remaining arguments set to 'a', 'b',
// 'c', 'd', 'e' and 'f'. The results are separated with whitespace in the
// expansion.
//
// An example:
//     // this macro expands to:
//     //     foo(1,a,b,c,d,e,f)
//     //     foo(2,a,b,c,d,e,f)
//     //     foo(3,a,b,c,d,e,f)
//     _WX_VARARG_ITER(3, foo, a, b, c, d, e, f)
//
// N must not be greater than _WX_VARARG_MAX_ARGS (=30).
#define _WX_VARARG_ITER(N,m,a,b,c,d,e,f) \
        _WX_VARARG_ITER_IMPL(N,m,a,b,c,d,e,f)
#define _WX_VARARG_ITER_IMPL(N,m,a,b,c,d,e,f) \
        _WX_VARARG_ITER_##N(m,a,b,c,d,e,f)

// Generates code snippet for i-th "variadic" argument in vararg function's
// prototype:
#define _WX_VARARG_ARG(i)               T##i a##i

// Like _WX_VARARG_ARG_UNUSED, but outputs argument's type with WXUNUSED:
#define _WX_VARARG_ARG_UNUSED(i)             T##i WXUNUSED(a##i)

// Generates code snippet for i-th type in vararg function's template<...>:
#define _WX_VARARG_TEMPL(i)             typename T##i

// Generates code snippet for passing i-th argument of vararg function
// wrapper to its implementation, normalizing it in the process:
#define _WX_VARARG_PASS_WCHAR(i)        wxArgNormalizerWchar<T##i>(a##i).get()
#define _WX_VARARG_PASS_UTF8(i)         wxArgNormalizerUtf8<T##i>(a##i).get()


// And the same for fixed arguments, _not_ normalizing it:
// FIXME-UTF8: this works, but uses wxString's implicit conversion to wxChar*
//             for the 'format' argument (which is const wxString&) _if_ the
//             implementation function has C sting argument; we need to
//             have wxFixedArgNormalizer<T> here that will pass everything
//             as-is except for wxString (for which wx_str() would be used),
//             but OTOH, we don't want to do that if the implementation takes
//             wxString argument
#define _WX_VARARG_PASS_FIXED(i)        f##i

#if wxUSE_UNICODE_UTF8
    #define _WX_VARARG_DO_CALL_UTF8(return_kw, impl, implUtf8, N, numfixed)   \
        return_kw implUtf8(_WX_VARARG_JOIN(numfixed, _WX_VARARG_PASS_FIXED),  \
                        _WX_VARARG_JOIN(N, _WX_VARARG_PASS_UTF8))
    #define _WX_VARARG_DO_CALL0_UTF8(return_kw, impl, implUtf8, numfixed)     \
        return_kw implUtf8(_WX_VARARG_JOIN(numfixed, _WX_VARARG_PASS_FIXED))
#endif // wxUSE_UNICODE_UTF8

#define _WX_VARARG_DO_CALL_WCHAR(return_kw, impl, implUtf8, N, numfixed)      \
    return_kw impl(_WX_VARARG_JOIN(numfixed, _WX_VARARG_PASS_FIXED),          \
                    _WX_VARARG_JOIN(N, _WX_VARARG_PASS_WCHAR))
#define _WX_VARARG_DO_CALL0_WCHAR(return_kw, impl, implUtf8, numfixed)        \
    return_kw impl(_WX_VARARG_JOIN(numfixed, _WX_VARARG_PASS_FIXED))

#if wxUSE_UNICODE_UTF8
    #if wxUSE_UTF8_LOCALE_ONLY
        #define _WX_VARARG_DO_CALL _WX_VARARG_DO_CALL_UTF8
        #define _WX_VARARG_DO_CALL0 _WX_VARARG_DO_CALL0_UTF8
    #else // possibly non-UTF8 locales
        #define _WX_VARARG_DO_CALL(return_kw, impl, implUtf8, N, numfixed)    \
            if ( wxLocaleIsUtf8 )                                             \
              _WX_VARARG_DO_CALL_UTF8(return_kw, impl, implUtf8, N, numfixed);\
            else                                                              \
              _WX_VARARG_DO_CALL_WCHAR(return_kw, impl, implUtf8, N, numfixed)

        #define _WX_VARARG_DO_CALL0(return_kw, impl, implUtf8, numfixed)      \
            if ( wxLocaleIsUtf8 )                                             \
              _WX_VARARG_DO_CALL0_UTF8(return_kw, impl, implUtf8, numfixed);  \
            else                                                              \
              _WX_VARARG_DO_CALL0_WCHAR(return_kw, impl, implUtf8, numfixed)
    #endif // wxUSE_UTF8_LOCALE_ONLY or not
#else // wxUSE_UNICODE_WCHAR or ANSI
    #define _WX_VARARG_DO_CALL _WX_VARARG_DO_CALL_WCHAR
    #define _WX_VARARG_DO_CALL0 _WX_VARARG_DO_CALL0_WCHAR
#endif // wxUSE_UNICODE_UTF8 / wxUSE_UNICODE_WCHAR


// Macro to be used with _WX_VARARG_ITER in the implementation of
// WX_DEFINE_VARARG_FUNC (see its documentation for the meaning of arguments)
#define _WX_VARARG_DEFINE_FUNC(N, rettype, name,                              \
                               impl, implUtf8, numfixed, fixed)               \
    template<_WX_VARARG_JOIN(N, _WX_VARARG_TEMPL)>                            \
    rettype name(_WX_VARARG_FIXED_EXPAND(numfixed, fixed),                    \
                 _WX_VARARG_JOIN(N, _WX_VARARG_ARG))                          \
    {                                                                         \
        _WX_VARARG_DO_CALL(return, impl, implUtf8, N, numfixed);              \
    }

#define _WX_VARARG_DEFINE_FUNC_N0(rettype, name,                              \
                                  impl, implUtf8, numfixed, fixed)            \
    inline rettype name(_WX_VARARG_FIXED_EXPAND(numfixed, fixed))             \
    {                                                                         \
        _WX_VARARG_DO_CALL0(return, impl, implUtf8, numfixed);                \
    }

// Macro to be used with _WX_VARARG_ITER in the implementation of
// WX_DEFINE_VARARG_FUNC_VOID (see its documentation for the meaning of
// arguments; rettype is ignored and is used only to satisfy _WX_VARARG_ITER's
// requirements).
#define _WX_VARARG_DEFINE_FUNC_VOID(N, rettype, name,                         \
                                    impl, implUtf8, numfixed, fixed)          \
    template<_WX_VARARG_JOIN(N, _WX_VARARG_TEMPL)>                            \
    void name(_WX_VARARG_FIXED_EXPAND(numfixed, fixed),                       \
                 _WX_VARARG_JOIN(N, _WX_VARARG_ARG))                          \
    {                                                                         \
        _WX_VARARG_DO_CALL(wxEMPTY_PARAMETER_VALUE,                           \
                           impl, implUtf8, N, numfixed);                      \
    }

#define _WX_VARARG_DEFINE_FUNC_VOID_N0(name, impl, implUtf8, numfixed, fixed) \
    inline void name(_WX_VARARG_FIXED_EXPAND(numfixed, fixed))                \
    {                                                                         \
        _WX_VARARG_DO_CALL0(wxEMPTY_PARAMETER_VALUE,                          \
                            impl, implUtf8, numfixed);                        \
    }

// Macro to be used with _WX_VARARG_ITER in the implementation of
// WX_DEFINE_VARARG_FUNC_CTOR (see its documentation for the meaning of
// arguments; rettype is ignored and is used only to satisfy _WX_VARARG_ITER's
// requirements).
#define _WX_VARARG_DEFINE_FUNC_CTOR(N, rettype, name,                         \
                                    impl, implUtf8, numfixed, fixed)          \
    template<_WX_VARARG_JOIN(N, _WX_VARARG_TEMPL)>                            \
    name(_WX_VARARG_FIXED_EXPAND(numfixed, fixed),                            \
                _WX_VARARG_JOIN(N, _WX_VARARG_ARG))                           \
    {                                                                         \
        _WX_VARARG_DO_CALL(wxEMPTY_PARAMETER_VALUE,                           \
                           impl, implUtf8, N, numfixed);                      \
    }

#define _WX_VARARG_DEFINE_FUNC_CTOR_N0(name, impl, implUtf8, numfixed, fixed) \
    inline name(_WX_VARARG_FIXED_EXPAND(numfixed, fixed))                     \
    {                                                                         \
        _WX_VARARG_DO_CALL0(wxEMPTY_PARAMETER_VALUE,                          \
                            impl, implUtf8, numfixed);                        \
    }

// Macro to be used with _WX_VARARG_ITER in the implementation of
// WX_DEFINE_VARARG_FUNC_NOP, i.e. empty stub for a disabled vararg function.
// The rettype and impl arguments are ignored.
#define _WX_VARARG_DEFINE_FUNC_NOP(N, rettype, name,                          \
                                   impl, implUtf8, numfixed, fixed)           \
    template<_WX_VARARG_JOIN(N, _WX_VARARG_TEMPL)>                            \
    void name(_WX_VARARG_FIXED_UNUSED_EXPAND(numfixed, fixed),                \
                 _WX_VARARG_JOIN(N, _WX_VARARG_ARG_UNUSED))                   \
    {}

#define _WX_VARARG_DEFINE_FUNC_NOP_N0(name, numfixed, fixed)                  \
    inline void name(_WX_VARARG_FIXED_UNUSED_EXPAND(numfixed, fixed))         \
    {}

#endif // _WX_STRVARARG_H_
