/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/ObjcRef.h
// Purpose:     wxObjcAutoRef template class
// Author:      David Elliott
// Modified by: 
// Created:     2004/03/28
// RCS-ID:      $Id$
// Copyright:   (c) 2004 David Elliott <dfe@cox.net>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_OBJCREF_H__
#define _WX_COCOA_OBJCREF_H__
/*
wxObjcAutoRefFromAlloc: construct a reference to an object that was
[NSObject -alloc]'ed and thus does not need a retain
wxObjcAutoRef: construct a reference to an object that was
either autoreleased or is retained by something else.
*/

struct objc_object;

// We must do any calls to Objective-C from an Objective-C++ source file
class wxObjcAutoRefBase
{
protected:
    static struct objc_object* ObjcRetain(struct objc_object*);
    static void ObjcRelease(struct objc_object*);
};

// T should be a pointer like NSObject*

template <class T>
class wxObjcAutoRefFromAlloc: wxObjcAutoRefBase
{
public:
    wxObjcAutoRefFromAlloc(T p = 0)
    :   m_ptr(p)
    // NOTE: this is from alloc.  Do NOT retain
    {}
    wxObjcAutoRefFromAlloc(const wxObjcAutoRefFromAlloc& otherRef)
    :   m_ptr(otherRef.m_ptr)
    {   ObjcRetain(m_ptr); }
    ~wxObjcAutoRefFromAlloc()
    {   ObjcRelease(m_ptr); }
    wxObjcAutoRefFromAlloc& operator=(const wxObjcAutoRefFromAlloc& otherRef)
    {   ObjcRetain(otherRef.m_ptr);
        ObjcRelease(m_ptr);
        m_ptr = otherRef.m_ptr;
        return *this;
    }
    operator T() const
    {   return m_ptr; }
    T operator->() const
    {   return m_ptr; }
protected:
    T m_ptr;
};

// The only thing wxObjcAutoRef has to do is retain an initial object
template <class T>
class wxObjcAutoRef: public wxObjcAutoRefFromAlloc<T>
{
public:
    wxObjcAutoRef(T p = 0)
    :   wxObjcAutoRefFromAlloc<T>(p)
    {   ObjcRetain(m_ptr); }
    ~wxObjcAutoRef() {}
    wxObjcAutoRef(const wxObjcAutoRef& otherRef)
    :   wxObjcAutoRefFromAlloc<T>(otherRef)
    {}
    wxObjcAutoRef(const wxObjcAutoRefFromAlloc<T>& otherRef)
    :   wxObjcAutoRefFromAlloc<T>(otherRef)
    {}
    wxObjcAutoRef& operator=(const wxObjcAutoRef& otherRef)
    {   return wxObjcAutoRefFromAlloc<T>::operator=(otherRef); }
};

#endif //ndef _WX_COCOA_OBJCREF_H__
