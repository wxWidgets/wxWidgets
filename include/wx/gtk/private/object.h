///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/object.h
// Purpose:     wxGtkObject class declaration
// Author:      Vadim Zeitlin
// Created:     2008-08-27
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_OBJECT_H_
#define _WX_GTK_PRIVATE_OBJECT_H_

// ----------------------------------------------------------------------------
// Convenience class for calling g_object_unref() automatically
// ----------------------------------------------------------------------------

template <typename T>
class wxGtkObject
{
public:
    wxGtkObject() = default;
    explicit wxGtkObject(T *p) : m_ptr(p) { }
    ~wxGtkObject() { if ( m_ptr ) g_object_unref(m_ptr); }

    operator T *() const { return m_ptr; }

    T** Out()
    {
        wxASSERT_MSG( !m_ptr, wxS("Can't reuse the same object.") );

        return &m_ptr;
    }

private:
    T* m_ptr = nullptr;

    // copying could be implemented by using g_object_ref() but for now there
    // is no need for it so don't implement it
    wxDECLARE_NO_COPY_CLASS(wxGtkObject);
};

#endif // _WX_GTK_PRIVATE_OBJECT_H_

