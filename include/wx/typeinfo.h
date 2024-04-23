///////////////////////////////////////////////////////////////////////////////
// Name:        wx/typeinfo.h
// Purpose:     wxTypeId implementation
// Author:      Jaakko Salli
// Created:     2009-11-19
// Copyright:   (c) wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TYPEINFO_H_
#define _WX_TYPEINFO_H_

//
// This file defines wxTypeId macro that should be used internally in
// wxWidgets instead of typeid(), for compatibility with builds that do
// not implement C++ RTTI. Also, type defining macros in this file are
// intended for internal use only at this time and may change in future
// versions.
//
// The reason why we need this simple RTTI system in addition to the older
// wxObject-based one is that the latter does not work in template
// classes.
//

#include "wx/defs.h"

//
// Left for compatibility reasons
//

#define _WX_DECLARE_TYPEINFO_CUSTOM(CLS, IDENTFUNC)
#define WX_DECLARE_TYPEINFO_INLINE(CLS)
#define WX_DECLARE_TYPEINFO(CLS)
#define WX_DEFINE_TYPEINFO(CLS)
#define WX_DECLARE_ABSTRACT_TYPEINFO(CLS)

#ifndef wxNO_RTTI

//
// Let's trust that Visual C++ versions 9.0 and later implement C++
// RTTI well enough, so we can use it and work around harmless memory
// leaks reported by the static run-time libraries.
//
#ifdef __VISUALC__
    #define wxTRUST_CPP_RTTI    1
#else
    #define wxTRUST_CPP_RTTI    0
#endif

#include <typeinfo>
#include <cstring>

#if wxTRUST_CPP_RTTI

#define wxTypeId    typeid

#else /*  !wxTRUST_CPP_RTTI */

//
// For improved type-safety, let's make the check using class name
// comparison. Most modern compilers already do this, but we cannot
// rely on all supported compilers to work this well. However, in
// cases where we'd know that typeid() would be flawless (as such),
// wxTypeId could of course simply be defined as typeid.
//

class wxTypeIdentifier
{
public:
    wxTypeIdentifier(const char* className)
    {
        m_className = className;
    }

    bool operator==(const wxTypeIdentifier& other) const
    {
        return strcmp(m_className, other.m_className) == 0;
    }

    bool operator!=(const wxTypeIdentifier& other) const
    {
        return !(*this == other);
    }
private:
    const char* m_className;
};

#define wxTypeId(OBJ) wxTypeIdentifier(typeid(OBJ).name())

#endif /*  wxTRUST_CPP_RTTI/!wxTRUST_CPP_RTTI */

#else // if !wxNO_RTTI

#define wxTRUST_CPP_RTTI    0

//
// When C++ RTTI is not available, we will have to make the type comparison
// using pointer to a dummy static member variable. This will fail if
// declared type is used across DLL boundaries, although using
// WX_DECLARE_TYPEINFO() and WX_DEFINE_TYPEINFO() pair instead of
// WX_DECLARE_TYPEINFO_INLINE() should fix this. However, that approach is
// usually not possible when type info needs to be declared for a template
// class.
//

class wxTypeIdentifier
{
public:
    bool operator==(const wxTypeIdentifier& other) const
    {
        return m_ptr == other.m_ptr;
    }

    bool operator!=(const wxTypeIdentifier& other) const
    {
        return !(*this == other);
    }
private:
    template<typename>
    struct wxDummy
    {
        #ifdef __VISUALC__
        // Workaround for msvc's non conforming optimization (/Gy and /OPT:ICF)
        static char ms_wxClassInfo;
        #else // !__VISUALC__
        static const char ms_wxClassInfo;
        #endif // __VISUALC__/!__VISUALC__
    };

    wxTypeIdentifier(const char* ptr) : m_ptr{ptr} { }

    template<typename T>
    friend wxTypeIdentifier wxTypeId(const T&);

    const char* m_ptr;
};

#ifdef __VISUALC__
// Workaround for msvc's non conforming optimization (/Gy and /OPT:ICF)
template<typename T>
char wxTypeIdentifier::wxDummy<T>::ms_wxClassInfo;
#else // !__VISUALC__
template<typename T>
const char wxTypeIdentifier::wxDummy<T>::ms_wxClassInfo{};
#endif // __VISUALC__/!__VISUALC__

template<typename T>
wxTypeIdentifier wxTypeId(const T&)
{
    return wxTypeIdentifier{&wxTypeIdentifier::wxDummy<T>::ms_wxClassInfo};
}

#endif // wxNO_RTTI/!wxNO_RTTI

#endif // _WX_TYPEINFO_H_
