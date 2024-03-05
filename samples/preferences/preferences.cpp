///////////////////////////////////////////////////////////////////////////////
// Name:        samples/preferences/preferences.cpp
// Purpose:     Sample demonstrating wxPreferencesEditor use.
// Author:      Vaclav Slavik
// Created:     2013-02-19
// Copyright:   (c) 2013 Vaclav Slavik <vslavik@fastmail.fm>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////


#include "wx/wxprec.h"


#include "wx/preferences.h"

#include "wx/app.h"
#include "wx/config.h"
#include "wx/panel.h"
#include "wx/menu.h"
#include "wx/checkbox.h"
#include "wx/listbox.h"
#include "wx/stattext.h"
#include "wx/sizer.h"
#include "wx/artprov.h"
#include "wx/frame.h"

#include <memory>

// This struct combines the settings edited in the preferences dialog.
struct MySettings
{
    MySettings()
    {
        // Normally we would initialize values by loading them from some
        // persistent storage, e.g. using wxConfig.
        // For demonstration purposes, we just set hardcoded values here.
        m_useMarkdown = true;
        m_spellcheck = false;
    }

    bool m_useMarkdown;
    bool m_spellcheck;

    // We don't do the same thing for the second preferences page fields, but
    // we would have included them in a real application.
};

class MyApp : public wxApp
{
public:
    virtual bool OnInit() override;

    void ShowPreferencesEditor(wxWindow* parent);
    void DismissPreferencesEditor();

    const MySettings& GetSettings() const { return m_settings; }
    void UpdateSettings(const MySettings& settings);

private:
    class MyFrame* m_frame;
    std::unique_ptr<wxPreferencesEditor> m_prefEditor;
    MySettings m_settings;
};

wxIMPLEMENT_APP(MyApp);


class MyFrame : public wxFrame
{
public:
    MyFrame() : wxFrame(nullptr, wxID_ANY, "Preferences sample")
    {
        wxMenu *fileMenu = new wxMenu;
        fileMenu->Append(wxID_PREFERENCES);
        fileMenu->Append(wxID_EXIT);

        wxMenuBar *menuBar = new wxMenuBar();
        menuBar->Append(fileMenu, "&File");
        SetMenuBar(menuBar);

        Bind(wxEVT_MENU, &MyFrame::OnPref, this, wxID_PREFERENCES);
        Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
        Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnClose, this);

        wxPanel* const panel = new wxPanel(this);
        m_textMarkdownSyntax = new wxStaticText(panel, wxID_ANY, "");
        m_textSpellcheck = new wxStaticText(panel, wxID_ANY, "");

        wxSizer* const sizer = new wxFlexGridSizer(2, wxSize(5, 5));
        sizer->Add(new wxStaticText(panel, wxID_ANY, "Markdown syntax:"),
                   wxSizerFlags().Center().Right());
        sizer->Add(m_textMarkdownSyntax,
                   wxSizerFlags().Center());
        sizer->Add(new wxStaticText(panel, wxID_ANY, "Spell checking:"),
                   wxSizerFlags().Center().Right());
        sizer->Add(m_textSpellcheck,
                   wxSizerFlags().Center());
        panel->SetSizer(sizer);

        // Show the initial values.
        UpdateSettings();
    }

    void UpdateSettings()
    {
        // Here we should update the settings we use. As we don't actually do
        // anything in this sample, just update their values shown on screen.
        const MySettings& settings = wxGetApp().GetSettings();
        m_textMarkdownSyntax->SetLabel(settings.m_useMarkdown ? "yes" : "no");
        m_textSpellcheck->SetLabel(settings.m_spellcheck ? "on" : "off");
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

    wxStaticText* m_textMarkdownSyntax;
    wxStaticText* m_textSpellcheck;
};


class PrefsPageGeneralPanel : public wxPanel
{
public:
    PrefsPageGeneralPanel(wxWindow *parent) : wxPanel(parent)
    {
        m_useMarkdown = new wxCheckBox(this, wxID_ANY, "Use Markdown syntax");
        m_spellcheck = new wxCheckBox(this, wxID_ANY, "Check spelling");

        wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(m_useMarkdown, wxSizerFlags().Border());
        sizer->Add(m_spellcheck, wxSizerFlags().Border());

        SetSizerAndFit(sizer);

        m_useMarkdown->Bind(wxEVT_CHECKBOX,
                            &PrefsPageGeneralPanel::ChangedUseMarkdown,
                            this);
        m_spellcheck->Bind(wxEVT_CHECKBOX,
                           &PrefsPageGeneralPanel::ChangedSpellcheck,
                           this);
    }

    virtual bool TransferDataToWindow() override
    {
        m_settingsCurrent = wxGetApp().GetSettings();
        m_useMarkdown->SetValue(m_settingsCurrent.m_useMarkdown);
        m_spellcheck->SetValue(m_settingsCurrent.m_spellcheck);
        return true;
    }

    virtual bool TransferDataFromWindow() override
    {
        // Called on platforms with modal preferences dialog to save and apply
        // the changes.
        wxGetApp().UpdateSettings(m_settingsCurrent);
        return true;
    }

private:
    void UpdateSettingsIfNecessary()
    {
        // On some platforms (OS X, GNOME), changes to preferences are applied
        // immediately rather than after the OK or Apply button is pressed, so
        // we need to take them into account as soon as they happen. On others
        // (MSW), we need to wait until the changes are accepted by the user by
        // pressing the "OK" button. To reuse the same code for both cases, we
        // always update m_settingsCurrent object under all platforms, but only
        // update the real application settings if necessary here.
        if ( wxPreferencesEditor::ShouldApplyChangesImmediately() )
        {
            wxGetApp().UpdateSettings(m_settingsCurrent);
        }
    }

    void ChangedUseMarkdown(wxCommandEvent& e)
    {
        m_settingsCurrent.m_useMarkdown = e.IsChecked();
        UpdateSettingsIfNecessary();
    }

    void ChangedSpellcheck(wxCommandEvent& e)
    {
        m_settingsCurrent.m_spellcheck = e.IsChecked();
        UpdateSettingsIfNecessary();
    }

    wxCheckBox *m_useMarkdown;
    wxCheckBox *m_spellcheck;

    // Settings corresponding to the current values in this dialog.
    MySettings m_settingsCurrent;
};

class PrefsPageGeneral : public wxStockPreferencesPage
{
public:
    PrefsPageGeneral() : wxStockPreferencesPage(Kind_General) {}
    virtual wxWindow *CreateWindow(wxWindow *parent) override
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
            m_fulltext->Bind(wxEVT_CHECKBOX,
                             &PrefsPageTopicsPanel::ChangedFulltext,
                             this);
        }
    }

    virtual bool TransferDataToWindow() override
    {
        // This is the place where you can initialize values, e.g. from wxConfig.
        // For demonstration purposes, we just set hardcoded values.
        m_fulltext->SetValue(true);
        // TODO: handle the listbox
        return true;
    }

    virtual bool TransferDataFromWindow() override
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
    virtual wxString GetName() const override { return "Topics"; }
    virtual wxBitmapBundle GetIcon() const override
        { return wxArtProvider::GetBitmapBundle(wxART_HELP, wxART_TOOLBAR); }
    virtual wxWindow *CreateWindow(wxWindow *parent) override
        { return new PrefsPageTopicsPanel(parent); }
};



bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

#ifdef __WXGTK__
    // Many version of wxGTK generate spurious diagnostic messages when
    // destroying wxNotebook (or removing pages from it), allow wxWidgets to
    // suppress them.
    GTKAllowDiagnosticsControl();
#endif // __WXGTK__

    // This will be used in the title of the preferences dialog under some
    // platforms, don't leave it as default "Preferences" because this would
    // result in rather strange "Preferences Preferences" title.
    SetAppDisplayName("wxWidgets Sample");

    m_frame = new MyFrame();
    m_frame->Show(true);

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

void MyApp::UpdateSettings(const MySettings& settings)
{
    m_settings = settings;
    m_frame->UpdateSettings();
}
