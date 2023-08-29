/////////////////////////////////////////////////////////////////////////////
// Name:        wx/meta/movable.h
// Purpose:     Test if a type is movable using memmove() etc.
// Author:      Vaclav Slavik
// Created:     2008-01-21
// Copyright:   (c) 2008 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_META_MOVABLE_H_
#define _WX_META_MOVABLE_H_

#include "wx/meta/pod.h"
#include "wx/string.h" // for wxIsMovable<wxString> specialization

// Helper to decide if an object of type T is "movable", i.e. if it can be
// copied to another memory location using memmove() or realloc() C functions.
// C++ only guarantees that POD types (including primitive types) are
// movable.
template<typename T>
struct wxIsMovable
{
    static const bool value = wxIsPod<T>::value;
};

// Macro to add wxIsMovable<T> specialization for given type that marks it
// as movable:
#define WX_DECLARE_TYPE_MOVABLE(type)                       \
    template<> struct wxIsMovable<type>                     \
    {                                                       \
        static const bool value = true;                     \
    };

#endif // _WX_META_MOVABLE_H_
