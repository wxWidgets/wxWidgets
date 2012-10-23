///////////////////////////////////////////////////////////////////////////////
// Name:        wx/meta/removeref.h
// Purpose:     Allows to remove a reference from a type.
// Author:      Vadim Zeitlin
// Created:     2012-10-21
// RCS-ID:      $Id$
// Copyright:   (c) 2012 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_META_REMOVEREF_H_
#define _WX_META_REMOVEREF_H_

// wxRemoveRef<> is similar to C++11 std::remove_reference<> but works with any
// compilers (but, to compensate for this, doesn't work with rvalue references).

template <typename T>
struct wxRemoveRef
{
    typedef T type;
};

template <typename T>
struct wxRemoveRef<T&>
{
    typedef T type;
};

#endif // _WX_META_REMOVEREF_H_
