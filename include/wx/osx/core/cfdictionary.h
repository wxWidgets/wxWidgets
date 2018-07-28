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

#ifndef _WX_OSX_COREFOUNDATION_CFICTIONARYREF_H__
#define _WX_OSX_COREFOUNDATION_CFICTIONARYREF_H__

#include "wx/osx/core/cfref.h"
#include "wx/osx/core/cfstring.h"

#include <CoreFoundation/CFDictionary.h>

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
        if ( m_ptr )
            CFNumberGetValue((CFNumberRef)m_ptr, kCFNumberCGFloatType, ptr);
        
        return m_ptr;
    }

    bool GetValue(int32_t* ptr) const
    {
        if ( m_ptr )
            CFNumberGetValue((CFNumberRef)m_ptr, kCFNumberSInt32Type, ptr);
        
        return m_ptr;
    }
    
    bool GetValue(uint32_t* ptr) const
    {
        if ( m_ptr )
            CFNumberGetValue((CFNumberRef)m_ptr, kCFNumberSInt32Type, ptr);
        
        return m_ptr;
    }
    
    bool GetValue(int64_t* ptr) const
    {
        if ( m_ptr )
            CFNumberGetValue((CFNumberRef)m_ptr, kCFNumberSInt64Type, ptr);
        
        return m_ptr;
    }
    
    bool GetValue(uint64_t* ptr) const
    {
        if ( m_ptr )
            CFNumberGetValue((CFNumberRef)m_ptr, kCFNumberSInt64Type, ptr);
        
        return m_ptr;
    }

    bool GetValue(wxString *s) const
    {
        if ( m_ptr )
            *s = wxCFStringRef::AsString((CFStringRef)m_ptr);
        
        return m_ptr;
        
    }

};

class wxCFNumberRef : public wxCFTypeRef
{
public:
    wxCFNumberRef(CGFloat v)
    : wxCFTypeRef(CFNumberCreate( NULL, kCFNumberCGFloatType, &v))
    {
    }
    
    wxCFNumberRef(int v)
    : wxCFTypeRef(CFNumberCreate( NULL, kCFNumberIntType, &v))
    {
    }
};

/*! @class wxCFDictionaryRef
 @discussion Properly retains/releases reference to CoreFoundation data objects
 */
template<typename T> class wxCFDictionaryRefCommon : public wxCFRef< T >
{
public:
    typedef wxCFRef<T> super_type;

    /*! @method     wxCFDictionaryRef
     @abstract   Assumes ownership of r and creates a reference to it.
     @param r        The dictionary reference to assume ownership of.  May be NULL.
     @discussion Like shared_ptr, it is assumed that the caller has a strong reference to r and intends
     to transfer ownership of that reference to this ref holder.  If the object comes from
     a Create or Copy method then this is the correct behaviour.  If the object comes from
     a Get method then you must CFRetain it yourself before passing it to this constructor.
     A handy way to do this is to use the non-member wxCFRefFromGet factory funcion.
     This method is templated and takes an otherType *p.  This prevents implicit conversion
     using an operator refType() in a different ref-holding class type.
     */
    explicit wxCFDictionaryRefCommon(T r)
    :   super_type(r)
    {}

    /*! @method     wxCFDictionaryRef
     @abstract   Copies a ref holder of the same type
     @param otherRef The other ref holder to copy.
     @discussion Ownership will be shared by the original ref and the newly created ref. That is,
     the object will be explicitly retained by this new ref.
     */
    wxCFDictionaryRefCommon(const wxCFDictionaryRefCommon& otherRef)
    :  super_type( otherRef )
    {}

    wxCFTypeRef GetValue(const void *key)
    {
        CFTypeRef val = CFDictionaryGetValue( this->m_ptr, key);
        if ( val )
            ::CFRetain(val);
        return val;
    }
};

class wxCFDictionaryRef : public wxCFDictionaryRefCommon< CFDictionaryRef >
{
public:
    explicit wxCFDictionaryRef(CFDictionaryRef r)
    :   wxCFDictionaryRefCommon(r)
    {}
};

class wxCFMutableDictionaryRef : public wxCFDictionaryRefCommon< CFMutableDictionaryRef >
{
public:
    wxCFMutableDictionaryRef() : wxCFDictionaryRefCommon(CFDictionaryCreateMutable(kCFAllocatorDefault, 0,&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks))
    {
    }

    explicit wxCFMutableDictionaryRef(CFMutableDictionaryRef r)
    :   wxCFDictionaryRefCommon(r)
    {}

    void SetValue(const void *key, const void *data)
    {
        CFDictionarySetValue( this->m_ptr, key, data);
    }
    
    void SetValue(const void*key, CGFloat v)
    {
        SetValue( key, wxCFNumberRef(v));
    }
};

#endif //ifndef _WX_OSX_COREFOUNDATION_CFICTIONARYREF_H__

