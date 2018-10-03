/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/core/cfarrayref.h
// Purpose:     wxCFArrayRef class
// Author:      Stefan Csomor
// Modified by:
// Created:     2018/07/27
// Copyright:   (c) 2018 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
/*! @header     wx/osx/core/cfarrayref.h
 @abstract   wxCFArrayRef class
 */

#ifndef _WX_OSX_COREFOUNDATION_CFARRAYREF_H__
#define _WX_OSX_COREFOUNDATION_CFARRAYREF_H__

#include "wx/osx/core/cfref.h"
#include "wx/osx/core/cftype.h"

#include <CoreFoundation/CFArray.h>

template <typename T, typename E>
class wxCFArrayRefCommon : public wxCFRef<T>
{
public:
    typedef wxCFRef<T> super_type;
    typedef size_t size_type;

    wxCFArrayRefCommon(T r)
        : super_type(r)
    {
    }

    wxCFArrayRefCommon(const wxCFArrayRefCommon& otherRef)
        : super_type(otherRef)
    {
    }

    size_type size() const
    {
        return (size_type)CFArrayGetCount(this->m_ptr);
    }

    bool empty() const
    {
        return size() == 0;
    }

    wxCFRef<E> at(size_type idx)
    {
        wxASSERT(idx < size());

        return wxCFRefFromGet((E)CFArrayGetValueAtIndex(this->m_ptr, idx));
    }

    operator WX_NSArray() { return (WX_NSArray) this->get(); }

    wxCFRef<E> operator[](size_type idx) { return at(idx); }
    wxCFRef<E> front() { return at(0); }
    wxCFRef<E> back() { return at(size() - 1); }
};


template <typename E>
class wxCFArrayRef : public wxCFArrayRefCommon<CFArrayRef, E>
{
public:
    wxCFArrayRef(CFArrayRef r)
        : wxCFArrayRefCommon<CFArrayRef, E>(r)
    {
    }

    wxCFArrayRef(const wxCFArrayRef& otherRef)
        : wxCFArrayRefCommon<CFArrayRef, E>(otherRef)
    {
    }
};

template <typename E>
class wxCFMutableArrayRef : public wxCFArrayRefCommon<CFMutableArrayRef, E>
{
public:
    wxCFMutableArrayRef()
        : wxCFArrayRefCommon<CFMutableArrayRef, E>(CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks))
    {
    }

    wxCFMutableArrayRef(CFMutableArrayRef r)
        : wxCFArrayRefCommon<CFMutableArrayRef, E>(r)
    {
    }

    wxCFMutableArrayRef(const wxCFMutableArrayRef& otherRef)
        : wxCFArrayRefCommon<CFMutableArrayRef, E>(otherRef)
    {
    }

    void push_back(E v)
    {
        CFArrayAppendValue(this->m_ptr, v);
    }

    void clear()
    {
        CFArrayRemoveAllValues(this->m_ptr);
    }
};

#endif //ifndef _WX_OSX_COREFOUNDATION_CFARRAYREF_H__
