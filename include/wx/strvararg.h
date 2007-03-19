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

#include "wx/chartype.h"

class WXDLLIMPEXP_BASE wxCStrData;
class WXDLLIMPEXP_BASE wxString;
class WXDLLIMPEXP_BASE wxCharBuffer;
class WXDLLIMPEXP_BASE wxWCharBuffer;


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
// wrappers around another variadic function (@a impl) and the only thing
// the wrapper does is that it normalizes the arguments passed in so that
// they are of the type expected by variadic functions taking string
// arguments, i.e., char* or wchar_t*, depending on the build:
//   * char* in the current locale's charset in ANSI build
//   * whchar_t* in the Unicode build
//
// Parameters:
//        rettype   Functions' return type.
//        name      Name of the function.
//        impl      Name of the variadic function that implements 'name'.
#define WX_DEFINE_VARARG_FUNC(rettype, name, impl)                        \
        _WX_VARARG_ITER(_WX_VARARG_MAX_ARGS,                              \
                        _WX_VARARG_DEFINE_FUNC,                           \
                        rettype, name, impl)

// Like WX_DEFINE_VARARG_FUNC, but for variadic functions that don't return
// a value.
#define WX_DEFINE_VARARG_FUNC_VOID(name, impl)                            \
        _WX_VARARG_ITER(_WX_VARARG_MAX_ARGS,                              \
                        _WX_VARARG_DEFINE_FUNC_VOID,                      \
                        void, name, impl)

// Like WX_DEFINE_VARARG_FUNC_VOID, but instead of wrapping an implementation
// function, does nothing in defined functions' bodies.
//
// Used to implement wxLogXXX functions if wxUSE_LOG=0.
#define WX_DEFINE_VARARG_FUNC_NOP(name)                                   \
        _WX_VARARG_ITER(_WX_VARARG_MAX_ARGS,                              \
                        _WX_VARARG_DEFINE_FUNC_NOP,                       \
                        void, name, dummy)

// ----------------------------------------------------------------------------
// implementation
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

// special cases for converting strings:

// FIXME-UTF8: move this to chartype.h!
#if wxUSE_UNICODE
    /* for now, all Unicode builds are wchar_t* based: */
    #define wxUSE_UNICODE_WCHAR 1
#else
    #define wxUSE_UNICODE_WCHAR 0
#endif

// FIXME-UTF8: include wx/chartype.h and use wxChar after headers split
// FIXME-UTF8: this will be char* in UTF-8 build and wchar_t* on Windows
#if wxUSE_UNICODE_WCHAR
    typedef wchar_t wxArgNativeCharType;
#else
    typedef char wxArgNativeCharType;
#endif

template<>
struct WXDLLIMPEXP_BASE wxArgNormalizer<const wxCStrData&>
{
    wxArgNormalizer(const wxCStrData& value) : m_value(value) {}
    const wxArgNativeCharType *get() const;

    const wxCStrData& m_value;
};

template<>
struct wxArgNormalizer<wxCStrData> : public wxArgNormalizer<const wxCStrData&>
{
    wxArgNormalizer(const wxCStrData& value)
        : wxArgNormalizer<const wxCStrData&>(value) {}
};

template<>
struct WXDLLIMPEXP_BASE wxArgNormalizer<const wxString&>
{
    wxArgNormalizer(const wxString& value) : m_value(value) {}
    const wxArgNativeCharType *get() const;

    const wxString& m_value;
};

template<>
struct wxArgNormalizer<wxString> : public wxArgNormalizer<const wxString&>
{
    wxArgNormalizer(const wxString& value)
        : wxArgNormalizer<const wxString&>(value) {}
};

#if wxUSE_UNICODE_WCHAR

template<>
struct WXDLLIMPEXP_BASE wxArgNormalizer<const char*>
{
    wxArgNormalizer(const char *value);
    ~wxArgNormalizer();
    const wchar_t *get() const;

    wxWCharBuffer *m_value;
};

template<>
struct wxArgNormalizer<char*> : public wxArgNormalizer<const char*>
{
    wxArgNormalizer(char *value)
        : wxArgNormalizer<const char*>(value) {}
};

#elif wxUSE_WCHAR_T // !wxUSE_UNICODE_WCHAR && wxUSE_WCHAR_T

template<>
struct WXDLLIMPEXP_BASE wxArgNormalizer<const wchar_t*>
{
    wxArgNormalizer(const wchar_t *value);
    ~wxArgNormalizer();
    const char *get() const;

    wxCharBuffer *m_value;
};

template<>
struct wxArgNormalizer<wchar_t*> : public wxArgNormalizer<const wchar_t*>
{
    wxArgNormalizer(wchar_t *value)
        : wxArgNormalizer<const wchar_t*>(value) {}
};

#endif // wxUSE_UNICODE_WCHAR / !wxUSE_UNICODE_WCHAR && wxUSE_WCHAR_T

// NB: The vararg emulation code is limited to 30 arguments at the moment.
//     If you need more, you need to
//        1) increase the value of _WX_VARARG_MAX_ARGS
//        2) add _WX_VARARG_JOIN_* and _WX_VARARG_ITER_* up to the new
//           _WX_VARARG_MAX_ARGS value to the lists below
#define _WX_VARARG_MAX_ARGS        30

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

#define _WX_VARARG_ITER_1(m,a,b,c)                               m(1,a,b,c)
#define _WX_VARARG_ITER_2(m,a,b,c)   _WX_VARARG_ITER_1(m,a,b,c)  m(2,a,b,c)
#define _WX_VARARG_ITER_3(m,a,b,c)   _WX_VARARG_ITER_2(m,a,b,c)  m(3,a,b,c)
#define _WX_VARARG_ITER_4(m,a,b,c)   _WX_VARARG_ITER_3(m,a,b,c)  m(4,a,b,c)
#define _WX_VARARG_ITER_5(m,a,b,c)   _WX_VARARG_ITER_4(m,a,b,c)  m(5,a,b,c)
#define _WX_VARARG_ITER_6(m,a,b,c)   _WX_VARARG_ITER_5(m,a,b,c)  m(6,a,b,c)
#define _WX_VARARG_ITER_7(m,a,b,c)   _WX_VARARG_ITER_6(m,a,b,c)  m(7,a,b,c)
#define _WX_VARARG_ITER_8(m,a,b,c)   _WX_VARARG_ITER_7(m,a,b,c)  m(8,a,b,c)
#define _WX_VARARG_ITER_9(m,a,b,c)   _WX_VARARG_ITER_8(m,a,b,c)  m(9,a,b,c)
#define _WX_VARARG_ITER_10(m,a,b,c)  _WX_VARARG_ITER_9(m,a,b,c)  m(10,a,b,c)
#define _WX_VARARG_ITER_11(m,a,b,c)  _WX_VARARG_ITER_10(m,a,b,c) m(11,a,b,c)
#define _WX_VARARG_ITER_12(m,a,b,c)  _WX_VARARG_ITER_11(m,a,b,c) m(12,a,b,c)
#define _WX_VARARG_ITER_13(m,a,b,c)  _WX_VARARG_ITER_12(m,a,b,c) m(13,a,b,c)
#define _WX_VARARG_ITER_14(m,a,b,c)  _WX_VARARG_ITER_13(m,a,b,c) m(14,a,b,c)
#define _WX_VARARG_ITER_15(m,a,b,c)  _WX_VARARG_ITER_14(m,a,b,c) m(15,a,b,c)
#define _WX_VARARG_ITER_16(m,a,b,c)  _WX_VARARG_ITER_15(m,a,b,c) m(16,a,b,c)
#define _WX_VARARG_ITER_17(m,a,b,c)  _WX_VARARG_ITER_16(m,a,b,c) m(17,a,b,c)
#define _WX_VARARG_ITER_18(m,a,b,c)  _WX_VARARG_ITER_17(m,a,b,c) m(18,a,b,c)
#define _WX_VARARG_ITER_19(m,a,b,c)  _WX_VARARG_ITER_18(m,a,b,c) m(19,a,b,c)
#define _WX_VARARG_ITER_20(m,a,b,c)  _WX_VARARG_ITER_19(m,a,b,c) m(20,a,b,c)
#define _WX_VARARG_ITER_21(m,a,b,c)  _WX_VARARG_ITER_20(m,a,b,c) m(21,a,b,c)
#define _WX_VARARG_ITER_22(m,a,b,c)  _WX_VARARG_ITER_21(m,a,b,c) m(22,a,b,c)
#define _WX_VARARG_ITER_23(m,a,b,c)  _WX_VARARG_ITER_22(m,a,b,c) m(23,a,b,c)
#define _WX_VARARG_ITER_24(m,a,b,c)  _WX_VARARG_ITER_23(m,a,b,c) m(24,a,b,c)
#define _WX_VARARG_ITER_25(m,a,b,c)  _WX_VARARG_ITER_24(m,a,b,c) m(25,a,b,c)
#define _WX_VARARG_ITER_26(m,a,b,c)  _WX_VARARG_ITER_25(m,a,b,c) m(26,a,b,c)
#define _WX_VARARG_ITER_27(m,a,b,c)  _WX_VARARG_ITER_26(m,a,b,c) m(27,a,b,c)
#define _WX_VARARG_ITER_28(m,a,b,c)  _WX_VARARG_ITER_27(m,a,b,c) m(28,a,b,c)
#define _WX_VARARG_ITER_29(m,a,b,c)  _WX_VARARG_ITER_28(m,a,b,c) m(29,a,b,c)
#define _WX_VARARG_ITER_30(m,a,b,c)  _WX_VARARG_ITER_29(m,a,b,c) m(30,a,b,c)

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
// its first argument set to 1..N and the remaining arguments set to 'a', 'b'
// and 'c'. The results are separated with whitespace in the expansion.
//
// An example:
//     // this macro expands to:
//     //     foo(1,a,b,c)
//     //     foo(2,a,b,c)
//     //     foo(3,a,b,c)
//     _WX_VARARG_ITER(3, foo, a, b, c)
//
// N must not be greater than _WX_VARARG_MAX_ARGS (=30).
#define _WX_VARARG_ITER(N, m,a,b,c)       _WX_VARARG_ITER_IMPL(N, m, a, b, c)
#define _WX_VARARG_ITER_IMPL(N, m,a,b,c)  _WX_VARARG_ITER_##N(m, a, b, c)

// Generates code snippet for i-th argument in vararg function's prototype.
#define _WX_VARARG_ARG(i)          T##i a##i

// Like _WX_VARARG_ARG_UNUSED, but outputs argument's type with WXUNUSED.
#define _WX_VARARG_ARG_UNUSED(i)   T##i WXUNUSED(a##i)

// Generates code snippet for i-th type in vararg function's template<...>.
#define _WX_VARARG_TEMPL(i)        typename T##i

// Generates code snippet for passing i-th argument of vararg function
// wrapper to its implementation, normalizing it in the process
#define _WX_VARARG_PASS(i)         wxArgNormalizer<T##i>(a##i).get()


// Macro to be used with _WX_VARARG_ITER in the implementation of
// WX_DEFINE_VARARG_FUNC (see its documentation for the meaning of arguments)
#define _WX_VARARG_DEFINE_FUNC(N, rettype, name, impl)                      \
    template<_WX_VARARG_JOIN(N, _WX_VARARG_TEMPL)>                          \
    rettype name(_WX_VARARG_JOIN(N, _WX_VARARG_ARG))                        \
    {                                                                       \
        return impl(_WX_VARARG_JOIN(N, _WX_VARARG_PASS));                   \
    }

// Macro to be used with _WX_VARARG_ITER in the implementation of
// WX_DEFINE_VARARG_FUNC_VOID (see its documentation for the meaning of
// arguments; rettype is ignored and is used only to satisfy _WX_VARARG_ITER's
// requirements).
#define _WX_VARARG_DEFINE_FUNC_VOID(N, rettype, name, impl)                 \
    template<_WX_VARARG_JOIN(N, _WX_VARARG_TEMPL)>                          \
    void name(_WX_VARARG_JOIN(N, _WX_VARARG_ARG))                           \
    {                                                                       \
        impl(_WX_VARARG_JOIN(N, _WX_VARARG_PASS));                          \
    }

// Macro to be used with _WX_VARARG_ITER in the implementation of
// WX_DEFINE_VARARG_FUNC_NOP, i.e. empty stub for a disabled vararg function.
// The rettype and impl arguments are ignored.
#define _WX_VARARG_DEFINE_FUNC_NOP(N, rettype, name, impl)                  \
    template<_WX_VARARG_JOIN(N, _WX_VARARG_TEMPL)>                          \
    void name(_WX_VARARG_JOIN(N, _WX_VARARG_ARG_UNUSED)) {}


#endif // _WX_STRVARARG_H_
