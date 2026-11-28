///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/preferencescmn.cpp
// Purpose:     wxPreferencesEditor implementation common to all platforms.
// Author:      Vaclav Slavik
// Created:     2013-02-19
// Copyright:   (c) 2013 Vaclav Slavik <vslavik@fastmail.fm>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_PREFERENCES_EDITOR

#include "wx/private/preferences.h"
#include "wx/intl.h"

// ============================================================================
// implementation
// ============================================================================

wxString wxStockPreferencesPage::GetName() const
{
    wxString name;
    switch ( m_kind )
    {
        case Kind_General:
            name = _("General");
            break;
        case Kind_Advanced:
            name = _("Advanced");
            break;
    }
    return name;
}

wxPreferencesEditor::wxPreferencesEditor(const wxString& title)
    : m_impl(wxPreferencesEditorImpl::Create(title))
{
}

wxPreferencesEditor::~wxPreferencesEditor()
{
    m_impl->OwnerDestroyed();
}

void wxPreferencesEditor::AddPage(wxPreferencesPage* page)
{
    wxCHECK_RET( page, "can't set null page" );
    m_impl->CallAddPage(page);
}

void wxPreferencesEditor::Show(wxWindow* parent)
{
    m_impl->CallShow(parent);
}

void wxPreferencesEditor::Dismiss()
{
    m_impl->CallDismiss();
}

void wxPreferencesEditorImpl::CallAddPage(wxPreferencesPage* page)
{
    CallRef keepAlive(this);

    if ( m_ownerAlive )
        AddPage(page);
}

void wxPreferencesEditorImpl::CallShow(wxWindow* parent)
{
    CallRef keepAlive(this);

    if ( m_ownerAlive )
        Show(parent);
}

void wxPreferencesEditorImpl::CallDismiss()
{
    CallRef keepAlive(this);
    Dismiss();
}

void wxPreferencesEditorImpl::OwnerDestroyed()
{
    m_ownerAlive = false;

    // Keep this object alive across the hook even when the owner holds its
    // final reference. Generic modal implementations use it to end their
    // nested event loop, while native lazy implementations need no hook.
    {
        CallRef keepAlive(this);
        OnOwnerDestroyed();
    }
    Release();
}

#endif // wxUSE_PREFERENCES_EDITOR
