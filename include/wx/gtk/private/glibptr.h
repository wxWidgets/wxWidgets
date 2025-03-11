///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/glibptr.h
// Purpose:     Simple RAII helper for calling g_free() automatically
// Author:      Vadim Zeitlin
// Created:     2024-12-28
// Copyright:   (c) 2024 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_OWNEDPTR_H_
#define _WX_GTK_PRIVATE_OWNEDPTR_H_

// ----------------------------------------------------------------------------
// Convenience class for calling g_free() automatically
// ----------------------------------------------------------------------------

template <typename T>
class wxGlibPtr
{
public:
    wxGlibPtr() = default;
    explicit wxGlibPtr(const T *p) : m_ptr(const_cast<T*>(p)) { }
    wxGlibPtr(wxGlibPtr&& other) : m_ptr(other.m_ptr) { other.m_ptr = nullptr; }
    ~wxGlibPtr() { g_free(m_ptr); }

    wxGlibPtr& operator=(wxGlibPtr&& other)
    {
        if ( &other != this )
        {
            g_free(m_ptr);
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }

        return *this;
    }

    const T* get() const { return m_ptr; }

    operator const T*() const { return m_ptr; }

    T** Out()
    {
        wxASSERT_MSG( !m_ptr, wxS("Can't reuse the same object.") );

        return &m_ptr;
    }

private:
    // We store the pointer as non-const because it avoids casts in the dtor
    // and Out(), but it's actually always handled as a const one.
    T* m_ptr = nullptr;

    wxDECLARE_NO_COPY_CLASS(wxGlibPtr);
};

#endif // _WX_GTK_PRIVATE_OWNEDPTR_H_
