///////////////////////////////////////////////////////////////////////////////
// Name:        wx/vector.h
// Purpose:     STL vector clone
// Author:      Lindsay Mathieson
// Modified by: Vaclav Slavik - make it a template
// Created:     30.07.2001
// Copyright:   (c) 2001 Lindsay Mathieson <lindsay@mathieson.org>,
//                  2007 Vaclav Slavik <vslavik@fastmail.fm>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// This entire header is obsolete, just use std::vector<> directly instead of
// referencing wxVector in any way, shape or form.

#ifndef _WX_VECTOR_H_
#define _WX_VECTOR_H_

#include "wx/defs.h"

#include <vector>
#include <algorithm>

template<typename T>
using wxVector = std::vector<T>;

template<typename T>
inline void wxVectorSort(wxVector<T>& v)
{
    std::sort(v.begin(), v.end());
}

template<typename T>
inline bool wxVectorContains(const wxVector<T>& v, const T& obj)
{
    return std::find(v.begin(), v.end(), obj) != v.end();
}

// This is an obsolete function provided only for compatibility, just call
// shrink_to_fit() directly in the new code.
template<typename T>
inline void wxShrinkToFit(wxVector<T>& v)
{
    v.shrink_to_fit();
}

#endif // _WX_VECTOR_H_
