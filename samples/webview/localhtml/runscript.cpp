/////////////////////////////////////////////////////////////////////////////
// Name:        samples/webview/localhtml/runscript.cpp
// Purpose:     Simple wxWebView sample loading a local HTML page and executing JavaScript from C++.
// Author:      Drazen Plavsic <drazen.p@live.com>
// Created:     2026-06-15
// Copyright:   (c) 2019 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include <wx/webview.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

class RunScriptFrame : public wxFrame
{
public:
    RunScriptFrame()
        : wxFrame(nullptr, wxID_ANY, "wxWebView Example", wxDefaultPosition, wxSize(900, 600))
    {
        auto* panel = new wxPanel(this);
        auto* mainSizer = new wxBoxSizer(wxVERTICAL);
        auto* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

        webView_ = wxWebView::New(panel, wxID_ANY);

        auto* page1Button = new wxButton(panel, wxID_ANY, "Show Page 1");
        auto* page2Button = new wxButton(panel, wxID_ANY, "Show Page 2");

        buttonSizer->Add(page1Button, 0, wxALL, 5);
        buttonSizer->Add(page2Button, 0, wxALL, 5);

        mainSizer->Add(buttonSizer, 0, wxEXPAND);
        mainSizer->Add(webView_, 1, wxEXPAND | wxALL, 5);

        panel->SetSizer(mainSizer);

        Bind(wxEVT_BUTTON, &RunScriptFrame::OnShowPage1, this, page1Button->GetId());
        Bind(wxEVT_BUTTON, &RunScriptFrame::OnShowPage2, this, page2Button->GetId());

        LoadLocalPage();
    }

private:
    wxWebView* webView_ = nullptr;

    void LoadLocalPage()
    {
        wxFileName file(wxStandardPaths::Get().GetExecutablePath());
        file.SetFullName("runscript.html");

        if (!file.FileExists())
        {
            wxMessageBox("Could not find runscript.html next to executable.", "Error", wxOK | wxICON_ERROR);
            return;
        }

        webView_->LoadURL(wxFileName::FileNameToURL(file));
    }

    void OnShowPage1(wxCommandEvent&)
    {
        if (webView_)
        {
            webView_->RunScript("showPage('page1');");
        }
    }

    void OnShowPage2(wxCommandEvent&)
    {
        if (webView_)
        {
            webView_->RunScript("showPage('page2');");
        }
    }
};

class MyApp : public wxApp
{
public:
    bool OnInit() override
    {
        auto* frame = new RunScriptFrame();
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);