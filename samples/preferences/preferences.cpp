///////////////////////////////////////////////////////////////////////////////
// Name:        samples/preferences/preferences.cpp
// Purpose:     Sample demonstrating wxPreferencesEditor use.
// Author:      Vaclav Slavik
// Created:     2013-02-19
// RCS-ID:      $Id$
// Copyright:   (c) 2013 Vaclav Slavik <vslavik@fastmail.fm>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////


#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/preferences.h"

#include "wx/app.h"
#include "wx/config.h"
#include "wx/panel.h"
#include "wx/scopedptr.h"
#include "wx/menu.h"
#include "wx/checkbox.h"
#include "wx/listbox.h"
#include "wx/stattext.h"
#include "wx/sizer.h"
#include "wx/artprov.h"
#include "wx/frame.h"

class MyApp : public wxApp
{
public:
    virtual bool OnInit();

    void ShowPreferencesEditor(wxWindow* parent);
    void DismissPreferencesEditor();

private:
    wxScopedPtr<wxPreferencesEditor> m_prefEditor;
};

IMPLEMENT_APP(MyApp)


class MyFrame : public wxFrame
{
public:
    MyFrame() : wxFrame(NULL, wxID_ANY, "Preferences sample")
    {
        wxMenu *fileMenu = new wxMenu;
        fileMenu->Append(wxID_PREFERENCES);
        fileMenu->Append(wxID_EXIT);

        wxMenuBar *menuBar = new wxMenuBar();
        menuBar->Append(fileMenu, "&File");
        SetMenuBar(menuBar);

        Connect(wxID_PREFERENCES,
                wxEVT_MENU,
                wxCommandEventHandler(MyFrame::OnPref), NULL, this);
        Connect(wxID_EXIT,
                wxEVT_MENU,
                wxCommandEventHandler(MyFrame::OnExit), NULL, this);
        Connect(wxEVT_CLOSE_WINDOW,
                wxCloseEventHandler(MyFrame::OnClose), NULL, this);
    }

private:
    void OnPref(wxCommandEvent&)
    {
        wxGetApp().ShowPreferencesEditor(this);
    }

    void OnExit(wxCommandEvent&)
    {
        Close();
    }

    void OnClose(wxCloseEvent& e)
    {
        wxGetApp().DismissPreferencesEditor();
        e.Skip();
    }
};


class PrefsPageGeneralPanel : public wxPanel
{
public:
    PrefsPageGeneralPanel(wxWindow *parent) : wxPanel(parent)
    {
        m_useMarkdown = new wxCheckBox(this, wxID_ANY, "User Markdown syntax");
        m_spellcheck = new wxCheckBox(this, wxID_ANY, "Check spelling");

        wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(m_useMarkdown, wxSizerFlags().Border());
        sizer->Add(m_spellcheck, wxSizerFlags().Border());

        SetSizerAndFit(sizer);

        // On some platforms (OS X, GNOME), changes to preferences are applied
        // immediately rather than after the OK or Apply button is pressed.
        if ( wxPreferencesEditor::ShouldApplyChangesImmediately() )
        {
            m_useMarkdown->Connect(wxEVT_CHECKBOX,
                                   wxCommandEventHandler(PrefsPageGeneralPanel::ChangedUseMarkdown),
                                   NULL, this);
            m_spellcheck->Connect(wxEVT_CHECKBOX,
                                  wxCommandEventHandler(PrefsPageGeneralPanel::ChangedSpellcheck),
                                  NULL, this);
        }
    }

    virtual bool TransferDataToWindow()
    {
        // This is the place where you can initialize values, e.g. from wxConfig.
        // For demonstration purposes, we just set hardcoded values.
        m_useMarkdown->SetValue(true);
        m_spellcheck->SetValue(false);
        return true;
    }

    virtual bool TransferDataFromWindow()
    {
        // Called on platforms with modal preferences dialog to save and apply
        // the changes.
        wxCommandEvent dummy;
        ChangedUseMarkdown(dummy);
        ChangedSpellcheck(dummy);
        return true;
    }

private:
    void ChangedUseMarkdown(wxCommandEvent& WXUNUSED(e))
    {
        // save new m_useMarkdown value and apply the change to the app
    }

    void ChangedSpellcheck(wxCommandEvent& WXUNUSED(e))
    {
        // save new m_spellcheck value and apply the change to the app
    }

    wxCheckBox *m_useMarkdown;
    wxCheckBox *m_spellcheck;
};

class PrefsPageGeneral : public wxStockPreferencesPage
{
public:
    PrefsPageGeneral() : wxStockPreferencesPage(Kind_General) {}
    virtual wxWindow *CreateWindow(wxWindow *parent)
        { return new PrefsPageGeneralPanel(parent); }
};


class PrefsPageTopicsPanel : public wxPanel
{
public:
    PrefsPageTopicsPanel(wxWindow *parent) : wxPanel(parent)
    {
        wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(new wxStaticText(this, wxID_ANY, "Search in these topics:"), wxSizerFlags().Border());
        wxListBox *box = new wxListBox(this, wxID_ANY);
        box->SetMinSize(wxSize(400, 300));
        sizer->Add(box, wxSizerFlags(1).Border().Expand());
        m_fulltext = new wxCheckBox(this, wxID_ANY, "Automatically build fulltext index");
        sizer->Add(m_fulltext, wxSizerFlags().Border());

        SetSizerAndFit(sizer);

        if ( wxPreferencesEditor::ShouldApplyChangesImmediately() )
        {
            m_fulltext->Connect(wxEVT_CHECKBOX,
                                wxCommandEventHandler(PrefsPageTopicsPanel::ChangedFulltext),
                                NULL, this);
        }
    }

    virtual bool TransferDataToWindow()
    {
        // This is the place where you can initialize values, e.g. from wxConfig.
        // For demonstration purposes, we just set hardcoded values.
        m_fulltext->SetValue(true);
        // TODO: handle the listbox
        return true;
    }

    virtual bool TransferDataFromWindow()
    {
        // Called on platforms with modal preferences dialog to save and apply
        // the changes.
        wxCommandEvent dummy;
        ChangedFulltext(dummy);
        // TODO: handle the listbox
        return true;
    }

private:
    void ChangedFulltext(wxCommandEvent& WXUNUSED(e))
    {
        // save new m_fulltext value and apply the change to the app
    }

    wxCheckBox *m_fulltext;
};

class PrefsPageTopics : public wxPreferencesPage
{
public:
    virtual wxString GetName() const { return "Topics"; }
    virtual wxBitmap GetLargeIcon() const
        { return wxArtProvider::GetBitmap(wxART_HELP, wxART_TOOLBAR); }
    virtual wxWindow *CreateWindow(wxWindow *parent)
        { return new PrefsPageTopicsPanel(parent); }
};



bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // This will be used in the title of the preferences dialog under some
    // platforms, don't leave it as default "Preferences" because this would
    // result in rather strange "Preferences Preferences" title.
    SetAppDisplayName("wxWidgets Sample");

    MyFrame *frame = new MyFrame();
    frame->Show(true);

    return true;
}

void MyApp::ShowPreferencesEditor(wxWindow* parent)
{
    if ( !m_prefEditor )
    {
        m_prefEditor.reset(new wxPreferencesEditor);
        m_prefEditor->AddPage(new PrefsPageGeneral());
        m_prefEditor->AddPage(new PrefsPageTopics());
    }

    m_prefEditor->Show(parent);
}

void MyApp::DismissPreferencesEditor()
{
    if ( m_prefEditor )
        m_prefEditor->Dismiss();
}
