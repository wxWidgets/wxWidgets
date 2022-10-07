/////////////////////////////////////////////////////////////////////////////
// Name:        wx/meta/convertible.h
// Purpose:     Test if types are convertible
// Author:      Arne Steinarson
// Created:     2008-01-10
// Copyright:   (c) 2008 Arne Steinarson
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_META_CONVERTIBLE_H_
#define _WX_META_CONVERTIBLE_H_

//
// Introduce an extra class to make this header compilable with g++3.2
//
template <class D, class B>
struct wxConvertibleTo_SizeHelper
{
    static char Match(B* pb);
    static int  Match(...);
};

// Helper to decide if an object of type D is convertible to type B (the test
// succeeds in particular when D derives from B)
template <class D, class B>
struct wxConvertibleTo
{
    enum
    {
        value =
            sizeof(wxConvertibleTo_SizeHelper<D,B>::Match(static_cast<D*>(NULL)))
            ==
            sizeof(char)
    };
};

// This is similar to wxConvertibleTo, except that when using a C++11 compiler,
// the case of D deriving from B non-publicly will be detected and the correct
// value (false) will be deduced instead of getting a compile-time error as
// with wxConvertibleTo. For pre-C++11 compilers there is no difference between
// this helper and wxConvertibleTo.
template <class D, class B>
struct wxIsPubliclyDerived
{
    enum
    {
        value = std::is_base_of<B, D>::value && std::is_convertible<D*, B*>::value
    };
};

#endif // _WX_META_CONVERTIBLE_H_

