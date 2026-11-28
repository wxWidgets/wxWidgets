///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/preferences.h
// Purpose:     wxPreferencesEditorImpl declaration.
// Author:      Vaclav Slavik
// Created:     2013-02-19
// Copyright:   (c) 2013 Vaclav Slavik <vslavik@fastmail.fm>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_PREFERENCES_H_
#define _WX_PRIVATE_PREFERENCES_H_

#include "wx/preferences.h"

#if wxUSE_TOOLBAR && defined(__WXOSX_COCOA__) && wxOSX_USE_NATIVE_TOOLBAR
    #define wxHAS_PREF_EDITOR_NATIVE
#endif

// ----------------------------------------------------------------------------
// wxPreferencesEditorImpl: defines wxPreferencesEditor implementation.
// ----------------------------------------------------------------------------

class wxPreferencesEditorImpl
{
public:
    // This is implemented in a platform-specific way.
    static wxPreferencesEditorImpl* Create(const wxString& title);

    // These methods simply mirror the public wxPreferencesEditor ones.
    virtual void AddPage(wxPreferencesPage* page) = 0;
    virtual void Show(wxWindow* parent) = 0;
    virtual void Dismiss() = 0;

    virtual ~wxPreferencesEditorImpl() = default;

    // These entry points keep the implementation alive while calling into
    // application code. In particular, a wxPreferencesPage callback is
    // allowed to destroy the wxPreferencesEditor owning this object.
    void CallAddPage(wxPreferencesPage* page);
    void CallShow(wxWindow* parent);
    void CallDismiss();

    // Called by wxPreferencesEditor's destructor instead of deleting this
    // object directly. This closes any outstanding UI immediately, while an
    // in-progress Call*() keeps the implementation alive until it unwinds.
    void OwnerDestroyed();

protected:
    wxPreferencesEditorImpl() = default;

    bool IsOwnerAlive() const { return m_ownerAlive; }

    // Generic modal implementations override this to end their nested event
    // loop. The default deliberately does nothing: some native backends create
    // their preferences window lazily from Dismiss().
    virtual void OnOwnerDestroyed() { }

private:
    class CallRef
    {
    public:
        explicit CallRef(wxPreferencesEditorImpl* impl) : m_impl(impl)
        {
            m_impl->AddRef();
        }

        ~CallRef()
        {
            m_impl->Release();
        }

    private:
        wxPreferencesEditorImpl* const m_impl;

        wxDECLARE_NO_COPY_CLASS(CallRef);
    };

    void AddRef() { ++m_refCount; }
    void Release()
    {
        if ( --m_refCount == 0 )
            delete this;
    }

    unsigned int m_refCount { 1 };
    bool m_ownerAlive { true };

    wxDECLARE_NO_COPY_CLASS(wxPreferencesEditorImpl);
};

#endif // _WX_PRIVATE_PREFERENCES_H_
