/////////////////////////////////////////////////////////////////////////////
// Name:        samples/webrequest.cpp
// Purpose:     wxWebRequest Sample
// Author:      Tobias Taschner
// Created:     2018-10-15
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/notebook.h>
#include <wx/artprov.h>

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

class WebRequestFrame : public wxFrame
{
public:
    WebRequestFrame(const wxString& title):
        wxFrame(NULL, wxID_ANY, title)
    {
        // set the frame icon
        SetIcon(wxICON(sample));

        // Prepare UI controls

        // If menus are not available add a button to access the about box
        wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        wxNotebook* notebook = new wxNotebook(this, wxID_ANY);

        // Get image page
        wxPanel* getPanel = new wxPanel(notebook);
        wxSizer* getSizer = new wxBoxSizer(wxVERTICAL);
        getSizer->Add(new wxStaticText(getPanel, wxID_ANY, "Image URL to load:"),
            wxSizerFlags().Border());
        m_getURLTextCtrl = new wxTextCtrl(getPanel, wxID_ANY,
            "https://www.wxwidgets.org/downloads/logos/blocks.png");
        getSizer->Add(m_getURLTextCtrl,
            wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT));

        wxButton* getLoadButton = new wxButton(getPanel, wxID_ANY, "&Load");
        getLoadButton->Bind(wxEVT_BUTTON, &WebRequestFrame::OnGetLoadButton, this);
        getSizer->Add(getLoadButton, wxSizerFlags().Border());

        wxStaticBoxSizer* getImageBox =
            new wxStaticBoxSizer(wxVERTICAL, getPanel, "Image");
        m_getStaticBitmap = new wxStaticBitmap(getImageBox->GetStaticBox(),
            wxID_ANY, wxArtProvider::GetBitmap(wxART_MISSING_IMAGE));
        getImageBox->Add(m_getStaticBitmap, wxSizerFlags(1).Expand());
        getSizer->Add(getImageBox, wxSizerFlags(1).Expand().Border());

        getPanel->SetSizer(getSizer);
        notebook->AddPage(getPanel, "GET Image", true);

        // POST Text page
        wxPanel* postPanel = new wxPanel(notebook);
        wxSizer* postSizer = new wxBoxSizer(wxVERTICAL);
        postSizer->Add(new wxStaticText(postPanel, wxID_ANY, "Request URL:"),
            wxSizerFlags().Border());
        m_postURLTextCtrl = new wxTextCtrl(postPanel, wxID_ANY,
            "https://api.github.com/");
        postSizer->Add(m_postURLTextCtrl,
            wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT));

        postSizer->Add(new wxStaticText(postPanel, wxID_ANY, "Content type:"),
            wxSizerFlags().Border());
        m_postContentTypeTextCtrl = new wxTextCtrl(postPanel, wxID_ANY,
            "application/json");
        postSizer->Add(m_postContentTypeTextCtrl,
            wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT));

        postSizer->Add(new wxStaticText(postPanel, wxID_ANY, "Request body:"),
            wxSizerFlags().Border());
        m_postRequestTextCtrl = new wxTextCtrl(postPanel, wxID_ANY, "",
            wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
        postSizer->Add(m_postRequestTextCtrl,
            wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT));

        wxButton* postSendButton = new wxButton(postPanel, wxID_ANY, "&Send");
        postSendButton->Bind(wxEVT_BUTTON, &WebRequestFrame::OnPostSendButton, this);
        postSizer->Add(postSendButton, wxSizerFlags().Border());

        postSizer->Add(new wxStaticText(postPanel, wxID_ANY, "Response body:"),
            wxSizerFlags().Border());
        m_postResponseTextCtrl = new wxTextCtrl(postPanel, wxID_ANY, "",
            wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
        m_postResponseTextCtrl->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        postSizer->Add(m_postResponseTextCtrl,
            wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM));

        postPanel->SetSizer(postSizer);
        notebook->AddPage(postPanel, "POST Text");

        mainSizer->Add(notebook, wxSizerFlags(1).Expand().Border());

        SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        SetSizer(mainSizer);

        SetSize(FromDIP(wxSize(400, 500)));

        CreateStatusBar();
    }

    void OnGetLoadButton(wxCommandEvent& WXUNUSED(evt))
    {
        GetStatusBar()->SetStatusText("Requesting image...");
    }

    void OnPostSendButton(wxCommandEvent& WXUNUSED(evt))
    {
        GetStatusBar()->SetStatusText("Requesting text...");
    }

private:
    wxTextCtrl* m_getURLTextCtrl;
    wxStaticBitmap* m_getStaticBitmap;

    wxTextCtrl* m_postURLTextCtrl;
    wxTextCtrl* m_postContentTypeTextCtrl;
    wxTextCtrl* m_postRequestTextCtrl;
    wxTextCtrl* m_postResponseTextCtrl;
};

class WebRequestApp : public wxApp
{
public:
    virtual bool OnInit() wxOVERRIDE
    {
        if ( !wxApp::OnInit() )
            return false;

        // create the main application window
        WebRequestFrame *frame = new WebRequestFrame("wxWebRequest Sample App");
        frame->Show(true);

        return true;
    }
};

wxIMPLEMENT_APP(WebRequestApp);
