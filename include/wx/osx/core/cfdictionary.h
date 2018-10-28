/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/core/cfdictionaryref.h
// Purpose:     wxCFDictionaryRef class
// Author:      Stefan Csomor
// Modified by:
// Created:     2018/07/27
// Copyright:   (c) 2018 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
/*! @header     wx/osx/core/cfdictionaryref.h
 @abstract   wxCFDictionaryRef class
 */

#ifndef _WX_OSX_COREFOUNDATION_CFDICTIONARYREF_H__
#define _WX_OSX_COREFOUNDATION_CFDICTIONARYREF_H__

#include "wx/osx/core/cfref.h"
#include "wx/osx/core/cfstring.h"
#include "wx/osx/core/cftype.h"

#include <CoreFoundation/CFDictionary.h>

/*! @class wxCFDictionaryRef
 @discussion Properly retains/releases reference to CoreFoundation data objects
 */
template <typename T>
class wxCFDictionaryRefCommon : public wxCFRef<T>
{
public:
    typedef wxCFRef<T> super_type;

    explicit wxCFDictionaryRefCommon()
        : super_type()
    {
    }

    /*! @method     wxCFDictionaryRef
     @abstract   Assumes ownership of r and creates a reference to it.
     @param r        The dictionary reference to assume ownership of.  May be NULL.
     @discussion Like shared_ptr, it is assumed that the caller has a strong reference to r and intends
     to transfer ownership of that reference to this ref holder.  If the object comes from
     a Create or Copy method then this is the correct behaviour.  If the object comes from
     a Get method then you must CFRetain it yourself before passing it to this constructor.
     A handy way to do this is to use the non-member wxCFRefFromGet factory function.
     This method is templated and takes an otherType *p.  This prevents implicit conversion
     using an operator refType() in a different ref-holding class type.
     */
    explicit wxCFDictionaryRefCommon(T r)
        : super_type(r)
    {
    }

    /*! @method     wxCFDictionaryRef
     @abstract   Copies a ref holder of the same type
     @param otherRef The other ref holder to copy.
     @discussion Ownership will be shared by the original ref and the newly created ref. That is,
     the object will be explicitly retained by this new ref.
     */
    wxCFDictionaryRefCommon(const wxCFDictionaryRefCommon& otherRef)
        : super_type(otherRef)
    {
    }

    wxCFTypeRef GetValue(const void* key)
    {
        CFTypeRef val = CFDictionaryGetValue(this->m_ptr, key);
        if (val)
            ::CFRetain(val);
        return val;
    }
};

class wxCFMutableDictionaryRef;

class wxCFDictionaryRef : public wxCFDictionaryRefCommon<CFDictionaryRef>
{
public:
    wxCFDictionaryRef()
    {
    }

    wxCFDictionaryRef(CFDictionaryRef r)
        : wxCFDictionaryRefCommon(r)
    {
    }

    wxCFDictionaryRef& operator=(const wxCFMutableDictionaryRef& other);

    CFDictionaryRef CreateCopy() const
    {
        return CFDictionaryCreateCopy(kCFAllocatorDefault, this->m_ptr);
    }

    CFMutableDictionaryRef CreateMutableCopy() const
    {
        return CFDictionaryCreateMutableCopy(kCFAllocatorDefault, 0, this->m_ptr);
    }
};

class wxCFMutableDictionaryRef : public wxCFDictionaryRefCommon<CFMutableDictionaryRef>
{
public:
    wxCFMutableDictionaryRef()
        : wxCFDictionaryRefCommon(CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks))
    {
    }

    wxCFMutableDictionaryRef(CFMutableDictionaryRef r)
        : wxCFDictionaryRefCommon(r)
    {
    }

    void SetValue(const void* key, const void* data)
    {
        CFDictionarySetValue(this->m_ptr, key, data);
    }

    void SetValue(const void* key, CGFloat v)
    {
        SetValue(key, wxCFNumberRef(v));
    }
    
    CFMutableDictionaryRef CreateCopy() const
    {
        return CFDictionaryCreateMutableCopy(kCFAllocatorDefault, 0, this->m_ptr);
    }

    friend class wxCFDictionaryRef;
};

inline wxCFDictionaryRef& wxCFDictionaryRef::operator=(const wxCFMutableDictionaryRef& otherRef)
{
    wxCFRetain(otherRef.m_ptr);
    wxCFRelease(m_ptr);
    m_ptr = (CFDictionaryRef)otherRef.m_ptr;

    return *this;
}

#endif //ifndef _WX_OSX_COREFOUNDATION_CFDICTIONARYREF_H__
