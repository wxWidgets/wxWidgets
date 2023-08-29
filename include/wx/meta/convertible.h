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

#include <type_traits>

// NOTE: this class is obsolete and provided only for compatibility, please use
// the standard class instead.
//
// Helper to decide if an object of type D is convertible to type B (the test
// succeeds in particular when D derives from B)
template <class D, class B>
using wxConvertibleTo = std::is_convertible<D*, B*>;

// This is similar to wxConvertibleTo, except that the case of D deriving from
// B non-publicly will be detected and the correct value (false) will be
// deduced.
template <class D, class B>
struct wxIsPubliclyDerived
{
    enum
    {
        value = std::is_base_of<B, D>::value && std::is_convertible<D*, B*>::value
    };
};

#endif // _WX_META_CONVERTIBLE_H_

