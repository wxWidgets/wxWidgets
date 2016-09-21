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

#include <stdlib.h>

//
// Introduce an extra class to make this header compilable with g++3.2
//
template <class D, class B>
struct wxConvertibleTo_SizeHelper
{
    static char Match(B*, int);

    // This needs to be a template in order to be able to rely on SFINAE in
    // case D inherits from B but uses non-public inheritance, so that the cast
    // from D* to B* exists but is inaccessible.
    template <typename T>
    static int  Match(D*, T);
};

// Helper to decide if an object of type D is convertible to type B (the test
// succeeds in particular when D derives from B)
template <class D, class B>
struct wxConvertibleTo
{
    enum
    {
        value =
            sizeof(wxConvertibleTo_SizeHelper<D,B>::Match(static_cast<D*>(NULL), 0))
            ==
            sizeof(char)
    };
};

#endif // _WX_META_CONVERTIBLE_H_

