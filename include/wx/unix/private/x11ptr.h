///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/private/x11ptr.h
// Purpose:     Simple RAII helper for calling XFree() automatically
// Author:      Vadim Zeitlin
// Created:     2025-04-28
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_PRIVATE_X11PTR_H_
#define _WX_UNIX_PRIVATE_X11PTR_H_

#include <X11/Xlib.h>

// ----------------------------------------------------------------------------
// Convenience class for calling XFree() automatically
// ----------------------------------------------------------------------------

template <typename T>
class wxX11Ptr
{
public:
    wxX11Ptr() = default;
    explicit wxX11Ptr(const T *p) : m_ptr(const_cast<T*>(p)) { }
    wxX11Ptr(wxX11Ptr&& other) : m_ptr(other.m_ptr) { other.m_ptr = nullptr; }
    ~wxX11Ptr() { reset(); }

    wxX11Ptr& operator=(wxX11Ptr&& other)
    {
        if ( &other != this )
        {
            reset(other.m_ptr);
            other.m_ptr = nullptr;
        }

        return *this;
    }

    const T* get() const { return m_ptr; }

    operator const T*() const { return m_ptr; }
    T* operator->() const { return m_ptr; }

    void reset(T* p = nullptr)
    {
        if ( m_ptr )
            XFree(m_ptr);

        m_ptr = p;
    }

    T** Out()
    {
        wxASSERT_MSG( !m_ptr, wxS("Can't reuse the same object.") );

        return &m_ptr;
    }

private:
    // We store the pointer as non-const because it avoids casts in the dtor
    // and Out(), but it's actually always handled as a const one.
    T* m_ptr = nullptr;

    wxDECLARE_NO_COPY_CLASS(wxX11Ptr);
};

#endif // _WX_UNIX_PRIVATE_X11PTR_H_
