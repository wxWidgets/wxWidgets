///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/preferencesg.cpp
// Purpose:     Implementation of wxPreferencesEditor.
// Author:      Vaclav Slavik
// Created:     2013-02-19
// RCS-ID:      $Id$
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/private/preferences.h"

#ifndef wxHAS_PREF_EDITOR_NATIVE

#include "wx/dialog.h"
#include "wx/notebook.h"
#include "wx/sizer.h"
#include "wx/sharedptr.h"
#include "wx/scopedptr.h"
#include "wx/vector.h"

class wxGenericPrefsDialog : public wxDialog
{
public:
    wxGenericPrefsDialog(wxWindow *parent)
        : wxDialog(parent, wxID_ANY, _("Preferences"),
                   wxDefaultPosition, wxDefaultSize,
                   wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX))
    {
        SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

        wxSizer *sizer = new wxBoxSizer(wxVERTICAL);

        m_notebook = new wxNotebook(this, wxID_ANY);
        sizer->Add(m_notebook, wxSizerFlags(1).Expand().DoubleBorder());

#ifdef __WXGTK__
        SetEscapeId(wxID_CLOSE);
        sizer->Add(CreateButtonSizer(wxCLOSE), wxSizerFlags().Expand().DoubleBorder(wxBOTTOM));
#else
        sizer->Add(CreateButtonSizer(wxOK | wxCANCEL),
                   wxSizerFlags().Expand().DoubleBorder(wxLEFT|wxRIGHT|wxBOTTOM));
#endif
        SetSizer(sizer);
    }

    void AddPage(wxPreferencesPage *page)
    {
        wxWindow *win = page->CreateWindow(m_notebook);
        m_notebook->AddPage(win, page->GetName());
    }

private:
    wxNotebook *m_notebook;
};


class wxGenericPreferencesEditorImplBase : public wxPreferencesEditorImpl
{
public:
    virtual void AddPage(wxPreferencesPage* page)
    {
        m_pages.push_back(wxSharedPtr<wxPreferencesPage>(page));
    }

protected:
    wxGenericPrefsDialog *CreateDialog(wxWindow *parent)
    {
        wxGenericPrefsDialog *dlg = new wxGenericPrefsDialog(parent);

        // TODO: Don't create all pages immediately like this, do it on demand
        //       when a page is selected in the notebook (as is done on OS X).
        //
        //       Currently, creating all pages is necessary so that the notebook
        //       can determine its best size. We'll need to extend
        //       wxPreferencesPage with a GetBestSize() virtual method to make
        //       it possible to defer the creation.
        for ( Pages::const_iterator i = m_pages.begin();
              i != m_pages.end();
              ++i )
        {
            dlg->AddPage(i->get());
        }

        return dlg;
    }

    typedef wxVector< wxSharedPtr<wxPreferencesPage> > Pages;
    Pages m_pages;

};


#ifdef wxHAS_PREF_EDITOR_MODELESS

class wxModelessPreferencesEditorImpl : public wxGenericPreferencesEditorImplBase
{
public:
    virtual ~wxModelessPreferencesEditorImpl()
    {
        // m_win may already be destroyed if this destructor is called from
        // wxApp's destructor. In that case, all windows -- including this
        // one -- would already be destroyed by now.
        if ( m_win )
            m_win->Destroy();
    }

    virtual void Show(wxWindow* parent)
    {
        if ( !m_win )
        {
            wxWindow *win = CreateDialog(parent);
            win->Show();
            m_win = win;
        }
        else
        {
            // Ideally, we'd reparent the dialog under 'parent', but it's
            // probably not worth the hassle. We know the old parent is still
            // valid, because otherwise Dismiss() would have been called and
            // m_win cleared.
            m_win->Raise();
        }
    }

    virtual void Dismiss()
    {
        if ( m_win )
        {
            m_win->Close(/*force=*/true);
            m_win = NULL;
        }
    }

private:
    wxWeakRef<wxWindow> m_win;
};

#else // !wxHAS_PREF_EDITOR_MODELESS

class wxModalPreferencesEditorImpl : public wxGenericPreferencesEditorImplBase
{
public:
    virtual void Show(wxWindow* parent)
    {
        wxScopedPtr<wxGenericPrefsDialog> dlg(CreateDialog(parent));
        dlg->Fit();
        dlg->ShowModal();
    }

    virtual void Dismiss()
    {
        // nothing to do
    }
};

#endif // !wxHAS_PREF_EDITOR_MODELESS


/*static*/
wxPreferencesEditorImpl* wxPreferencesEditorImpl::Create()
{
#ifdef wxHAS_PREF_EDITOR_MODELESS
    return new wxModelessPreferencesEditorImpl();
#else
    return new wxModalPreferencesEditorImpl();
#endif
}

#endif // !wxHAS_PREF_EDITOR_NATIVE
