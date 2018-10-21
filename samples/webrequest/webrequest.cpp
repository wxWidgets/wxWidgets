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

#include "wx/notebook.h"
#include "wx/artprov.h"
#include "wx/webrequest.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

class WebRequestFrame : public wxFrame
{
public:
    enum Pages
    {
        Page_Image,
        Page_Text,
        Page_Download,
        Page_Advanced
    };

    WebRequestFrame(const wxString& title):
        wxFrame(NULL, wxID_ANY, title)
    {
        // set the frame icon
        SetIcon(wxICON(sample));

        // Prepare UI controls

        // If menus are not available add a button to access the about box
        wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        mainSizer->Add(new wxStaticText(this, wxID_ANY, "Request URL:"),
            wxSizerFlags().Border());
        m_urlTextCtrl = new wxTextCtrl(this, wxID_ANY,
            "https://www.wxwidgets.org/downloads/logos/blocks.png");
        mainSizer->Add(m_urlTextCtrl,
            wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT));

        m_notebook = new wxNotebook(this, wxID_ANY);
        m_notebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &WebRequestFrame::OnNotebookPageChanged, this);

        // Image page
        wxPanel* imagePanel = new wxPanel(m_notebook);
        wxSizer* imageSizer = new wxBoxSizer(wxVERTICAL);

        m_imageStaticBitmap = new wxStaticBitmap(imagePanel,
            wxID_ANY, wxArtProvider::GetBitmap(wxART_MISSING_IMAGE));
        imageSizer->Add(m_imageStaticBitmap, wxSizerFlags(1).Expand());

        imagePanel->SetSizer(imageSizer);
        m_notebook->AddPage(imagePanel, "Image", true);

        // Text page
        wxPanel* textPanel = new wxPanel(m_notebook);
        wxSizer* textSizer = new wxBoxSizer(wxVERTICAL);

        m_postCheckBox = new wxCheckBox(textPanel, wxID_ANY, "Post request body");
        textSizer->Add(m_postCheckBox, wxSizerFlags().Border());
        m_postCheckBox->Bind(wxEVT_CHECKBOX, &WebRequestFrame::OnPostCheckBox, this);

        m_postRequestTextCtrl = new wxTextCtrl(textPanel, wxID_ANY, "",
            wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
        textSizer->Add(m_postRequestTextCtrl,
            wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT));

        textSizer->Add(new wxStaticText(textPanel, wxID_ANY, "Request body content type:"),
            wxSizerFlags().Border());
        m_postContentTypeTextCtrl = new wxTextCtrl(textPanel, wxID_ANY,
            "application/json");
        textSizer->Add(m_postContentTypeTextCtrl,
            wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT));

        textSizer->Add(new wxStaticText(textPanel, wxID_ANY, "Response body:"),
            wxSizerFlags().Border());
        m_textResponseTextCtrl = new wxTextCtrl(textPanel, wxID_ANY, "",
            wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
        m_textResponseTextCtrl->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        textSizer->Add(m_textResponseTextCtrl,
            wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM));

        textPanel->SetSizer(textSizer);
        m_notebook->AddPage(textPanel, "Text");

        // Download page
        wxPanel* downloadPanel = new wxPanel(m_notebook);

        m_notebook->AddPage(downloadPanel, "Download");

        // Advanced page
        wxPanel* advancedPanel = new wxPanel(m_notebook);

        m_notebook->AddPage(advancedPanel, "Advanced");

        mainSizer->Add(m_notebook, wxSizerFlags(1).Expand().Border());

        m_startButton = new wxButton(this, wxID_ANY, "&Start Request");
        m_startButton->Bind(wxEVT_BUTTON, &WebRequestFrame::OnStartButton, this);
        mainSizer->Add(m_startButton, wxSizerFlags().Border());

        wxCommandEvent evt;
        OnPostCheckBox(evt);

        SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        SetSizer(mainSizer);

        SetSize(FromDIP(wxSize(400, 500)));

        CreateStatusBar();
    }

    void OnStartButton(wxCommandEvent& WXUNUSED(evt))
    {
        GetStatusBar()->SetStatusText("Started request...");

        // Create request for the specified URL from the default session
        wxObjectDataPtr<wxWebRequest> request(wxWebSession::GetDefault().CreateRequest(
            m_urlTextCtrl->GetValue()));

        // Bind event for failure
        request->Bind(wxEVT_WEBREQUEST_FAILED, &WebRequestFrame::OnWebRequestFailed, this);

        // Prepare request based on selected action
        switch (m_notebook->GetSelection())
        {
        case Page_Image:
            // Bind completion event to response as image
            request->Bind(wxEVT_WEBREQUEST_READY, &WebRequestFrame::OnImageRequestReady, this);
            break;
        case Page_Text:
            if (m_postCheckBox->IsChecked())
            {
                request->SetData(m_postRequestTextCtrl->GetValue(),
                    m_postContentTypeTextCtrl->GetValue());
            }

            request->Bind(wxEVT_WEBREQUEST_READY, &WebRequestFrame::OnTextRequestReady, this);
            break;
        case Page_Download:
            // TODO: implement
            break;
        case Page_Advanced:
            // TODO: implement
            break;
        }

        m_startButton->Disable();

        // Start the request (events will be called on success or failure)
        request->Start();
    }

    void OnImageRequestReady(wxWebRequestEvent& evt)
    {
        wxImage img(*evt.GetResponse()->GetStream());
        m_imageStaticBitmap->SetBitmap(img);
        m_notebook->GetPage(Page_Image)->Layout();
        GetStatusBar()->SetStatusText(wxString::Format("Loaded %lld bytes image data", evt.GetResponse()->GetContentLength()));
        m_startButton->Enable();
    }

    void OnTextRequestReady(wxWebRequestEvent& evt)
    {
        m_textResponseTextCtrl->SetValue(evt.GetResponse()->AsString());
        GetStatusBar()->SetStatusText(wxString::Format("Loaded %lld bytes text data (Status: %d %s)",
            evt.GetResponse()->GetContentLength(),
            evt.GetResponse()->GetStatus(),
            evt.GetResponse()->GetStatusText()));
        m_startButton->Enable();
    }

    void OnWebRequestFailed(wxWebRequestEvent& evt)
    {
        wxLogError("Web Request failed: %s", evt.GetErrorDescription());
        GetStatusBar()->SetStatusText("");
        m_startButton->Enable();
    }

    void OnPostCheckBox(wxCommandEvent& WXUNUSED(evt))
    {
        m_postContentTypeTextCtrl->Enable(m_postCheckBox->IsChecked());
        m_postRequestTextCtrl->Enable(m_postCheckBox->IsChecked());
        wxColour textBg = wxSystemSettings::GetColour(
            (m_postCheckBox->IsChecked()) ? wxSYS_COLOUR_WINDOW : wxSYS_COLOUR_BTNFACE);

        m_postContentTypeTextCtrl->SetBackgroundColour(textBg);
        m_postRequestTextCtrl->SetBackgroundColour(textBg);
    }

    void OnNotebookPageChanged(wxBookCtrlEvent& event)
    {
        wxString defaultURL;
        switch (event.GetSelection())
        {
        case Page_Image:
            defaultURL = "https://www.wxwidgets.org/downloads/logos/blocks.png";
            break;
        case Page_Text:
            defaultURL = "https://api.github.com";
            break;
        case Page_Download:
            defaultURL = "https://www.wxwidgets.com/download.zip";
            break;
        case Page_Advanced:
            defaultURL = "https://www.wxwidgets.com/adv.zip";
            break;
        }
        m_urlTextCtrl->SetValue(defaultURL);
    }

private:
    wxNotebook* m_notebook;
    wxTextCtrl* m_urlTextCtrl;
    wxButton* m_startButton;
    wxStaticBitmap* m_imageStaticBitmap;

    wxCheckBox* m_postCheckBox;
    wxTextCtrl* m_postContentTypeTextCtrl;
    wxTextCtrl* m_postRequestTextCtrl;
    wxTextCtrl* m_textResponseTextCtrl;
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
