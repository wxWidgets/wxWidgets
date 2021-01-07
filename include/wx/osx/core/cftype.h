/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/core/cftype.h
// Purpose:     wxCFDictionaryRef class
// Author:      Stefan Csomor
// Modified by:
// Created:     2018/07/27
// Copyright:   (c) 2018 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
/*! @header     wx/osx/core/cftype.h
 @abstract   wxCFTypeRef class and derived classes
 */

#ifndef _WX_OSX_COREFOUNDATION_CFTYPEREF_H__
#define _WX_OSX_COREFOUNDATION_CFTYPEREF_H__

#include "wx/osx/core/cfref.h"
#include "wx/osx/core/cfstring.h"

#ifndef WX_PRECOMP
#include "wx/string.h"
#endif

class wxCFTypeRef : public wxCFRef<CFTypeRef>
{
public:
    typedef wxCFRef<CFTypeRef> super_type;

    wxCFTypeRef(CFTypeRef d)
        : super_type(d)
    {
    }

    template <typename V>
    bool GetValue(V* ptr) const;

    template <typename V>
    bool GetValue(V* ptr, V defaultValue) const
    {
        bool hasKey = GetValue(ptr);

        if (!hasKey)
            *ptr = defaultValue;

        return hasKey;
    }

    template <typename V>
    bool GetValue(V& ref) const
    {
        return GetValue(&ref);
    }

    template <typename V>
    bool GetValue(V& ref, V defaultValue) const
    {
        bool hasKey = GetValue(ref);

        if (!hasKey)
            ref = defaultValue;

        return hasKey;
    }

    // spezialization through overload

    bool GetValue(CGFloat* ptr) const
    {
        if (m_ptr)
            CFNumberGetValue((CFNumberRef)m_ptr, kCFNumberCGFloatType, ptr);

        return m_ptr;
    }

    bool GetValue(int32_t* ptr) const
    {
        if (m_ptr)
            CFNumberGetValue((CFNumberRef)m_ptr, kCFNumberSInt32Type, ptr);

        return m_ptr;
    }

    bool GetValue(uint32_t* ptr) const
    {
        if (m_ptr)
            CFNumberGetValue((CFNumberRef)m_ptr, kCFNumberSInt32Type, ptr);

        return m_ptr;
    }

    bool GetValue(int64_t* ptr) const
    {
        if (m_ptr)
            CFNumberGetValue((CFNumberRef)m_ptr, kCFNumberSInt64Type, ptr);

        return m_ptr;
    }

    bool GetValue(uint64_t* ptr) const
    {
        if (m_ptr)
            CFNumberGetValue((CFNumberRef)m_ptr, kCFNumberSInt64Type, ptr);

        return m_ptr;
    }

    bool GetValue(wxString* s) const
    {
        if (m_ptr)
            *s = wxCFStringRef::AsString((CFStringRef)m_ptr);

        return m_ptr;
    }
};

class wxCFNumberRef : public wxCFTypeRef
{
public:
    wxCFNumberRef(CGFloat v)
        : wxCFTypeRef(CFNumberCreate(NULL, kCFNumberCGFloatType, &v))
    {
    }

    wxCFNumberRef(int v)
        : wxCFTypeRef(CFNumberCreate(NULL, kCFNumberIntType, &v))
    {
    }
};

#endif //ifndef _WX_OSX_COREFOUNDATION_CFTYPEREF_H__
