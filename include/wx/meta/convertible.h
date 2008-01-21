/////////////////////////////////////////////////////////////////////////////
// Name:        wx/meta/convertible.h
// Purpose:     Test if types are convertible
// Author:      Arne Steinarson
// Created:     2008-01-10
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Arne Steinarson
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_META_CONVERTIBLE_H_
#define _WX_META_CONVERTIBLE_H_

// Helper to decide if an object of type D is convertible to type B (the test
// succeeds in particular when D derives from B)
template <class D, class B>
struct wxConvertibleTo
{
    static char Match(B* pb);
    static int Match(...);

    enum { value = sizeof(Match(static_cast<D*>(NULL))) == sizeof(char) };
};

#endif // _WX_META_CONVERTIBLE_H_

