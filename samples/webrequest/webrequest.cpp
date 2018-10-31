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
#include "wx/filedlg.h"

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

        m_postRequestTextCtrl = new wxTextCtrl(textPanel, wxID_ANY,
            "app=WebRequestSample&version=1",
            wxDefaultPosition, wxSize(-1, FromDIP(60)), wxTE_MULTILINE);
        textSizer->Add(m_postRequestTextCtrl,
            wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT));

        textSizer->Add(new wxStaticText(textPanel, wxID_ANY, "Request body content type:"),
            wxSizerFlags().Border());
        m_postContentTypeTextCtrl = new wxTextCtrl(textPanel, wxID_ANY,
            "application/x-www-form-urlencoded");
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
        wxSizer* downloadSizer = new wxBoxSizer(wxVERTICAL);
        wxStaticText* downloadHeader = new wxStaticText(downloadPanel, wxID_ANY,
            "The URL will be downloaded to a file.\n"\
            "Progress will be shown and you will be asked, where\n"\
            "to save the file when the download completed.");
        downloadSizer->Add(downloadHeader, wxSizerFlags().Expand().Border());
        downloadSizer->AddStretchSpacer();
        m_downloadGauge = new wxGauge(downloadPanel, wxID_ANY, 100);
        downloadSizer->Add(m_downloadGauge, wxSizerFlags().Expand().Border());
        m_downloadStaticText = new wxStaticText(downloadPanel, wxID_ANY, "");
        downloadSizer->Add(m_downloadStaticText, wxSizerFlags().Expand().Border());

        downloadSizer->AddStretchSpacer();

        downloadPanel->SetSizer(downloadSizer);
        m_notebook->AddPage(downloadPanel, "Download");

        // Advanced page
        wxPanel* advancedPanel = new wxPanel(m_notebook);
        wxSizer* advSizer = new wxBoxSizer(wxVERTICAL);
        wxStaticText* advHeader = new wxStaticText(advancedPanel, wxID_ANY,
            "As an example of processing data while\n"\
            "it's being received from the server every\n"\
            "zero byte in the response will be counted below.");
        advSizer->Add(advHeader, wxSizerFlags().Expand().Border());

        advSizer->AddStretchSpacer();
        m_advCountStaticText = new wxStaticText(advancedPanel, wxID_ANY, "0",
            wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL | wxST_NO_AUTORESIZE);
        m_advCountStaticText->SetFont(m_advCountStaticText->GetFont()
            .MakeBold().MakeLarger().MakeLarger());
        advSizer->Add(m_advCountStaticText, wxSizerFlags().Expand().Border());
        advSizer->AddStretchSpacer();

        advancedPanel->SetSizer(advSizer);

        m_notebook->AddPage(advancedPanel, "Advanced");

        mainSizer->Add(m_notebook, wxSizerFlags(1).Expand().Border());

        m_startButton = new wxButton(this, wxID_ANY, "&Start Request");
        m_startButton->Bind(wxEVT_BUTTON, &WebRequestFrame::OnStartButton, this);
        mainSizer->Add(m_startButton, wxSizerFlags().Border());

        wxCommandEvent evt;
        OnPostCheckBox(evt);

        SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        SetSizer(mainSizer);

        SetSize(FromDIP(wxSize(540, 500)));

        CreateStatusBar();

        m_downloadProgressTimer.Bind(wxEVT_TIMER,
            &WebRequestFrame::OnProgressTimer, this);

        m_downloadRequest = NULL;
    }

    void OnStartButton(wxCommandEvent& WXUNUSED(evt))
    {
        GetStatusBar()->SetStatusText("Started request...");

        // Create request for the specified URL from the default session
        wxObjectDataPtr<wxWebRequest> request(wxWebSession::GetDefault().CreateRequest(
            m_urlTextCtrl->GetValue()));

        // Bind event for state change
        request->Bind(wxEVT_WEBREQUEST_STATE, &WebRequestFrame::OnWebRequestState, this);

        // Prepare request based on selected action
        switch (m_notebook->GetSelection())
        {
        case Page_Image:
            // Reset static bitmap image
            m_imageStaticBitmap->SetBitmap(wxArtProvider::GetBitmap(wxART_MISSING_IMAGE));
            break;
        case Page_Text:
            // Reset response text control
            m_textResponseTextCtrl->Clear();

            // Set postdata if checked
            if (m_postCheckBox->IsChecked())
            {
                request->SetData(m_postRequestTextCtrl->GetValue(),
                    m_postContentTypeTextCtrl->GetValue());
            }
            break;
        case Page_Download:
            m_downloadRequest = request.get();
            request->SetStorage(wxWebRequest::Storage_File);
            m_downloadGauge->SetValue(0);
            m_downloadGauge->Pulse();
            m_downloadStaticText->SetLabel("");
            m_downloadProgressTimer.Start(500);
            GetStatusBar()->SetStatusText("");
            break;
        case Page_Advanced:
            request->SetStorage(wxWebRequest::Storage_None);
            request->Bind(wxEVT_WEBREQUEST_DATA, &WebRequestFrame::OnRequestData, this);

            GetStatusBar()->SetStatusText("Counting...");
            m_advCount = 0;
            m_advCountStaticText->SetLabel("0");
            break;
        }

        m_startButton->Disable();

        // Start the request (events will be called on success or failure)
        request->Start();
    }

    void OnWebRequestState(wxWebRequestEvent& evt)
    {
        m_startButton->Enable(evt.GetState() != wxWebRequest::State_Active);

        switch (evt.GetState())
        {
            case wxWebRequest::State_Completed:
                switch (m_notebook->GetSelection())
                {
                case Page_Image:
                {
                    wxImage img(*evt.GetResponse()->GetStream());
                    m_imageStaticBitmap->SetBitmap(img);
                    m_notebook->GetPage(Page_Image)->Layout();
                    GetStatusBar()->SetStatusText(wxString::Format("Loaded %lld bytes image data", evt.GetResponse()->GetContentLength()));
                    break;
                }
                case Page_Text:
                    m_textResponseTextCtrl->SetValue(evt.GetResponse()->AsString());
                    GetStatusBar()->SetStatusText(wxString::Format("Loaded %lld bytes text data (Status: %d %s)",
                        evt.GetResponse()->GetContentLength(),
                        evt.GetResponse()->GetStatus(),
                        evt.GetResponse()->GetStatusText()));
                    break;
                case Page_Download:
                {
                    // Force last progress update
                    wxTimerEvent timerEvt;
                    OnProgressTimer(timerEvt);

                    // Stop updating download progress
                    m_downloadRequest = NULL;
                    m_downloadProgressTimer.Stop();

                    GetStatusBar()->SetStatusText("Download completed");

                    // Ask the user where to save the file
                    wxFileDialog fileDlg(this, "Save download", "",
                        evt.GetResponse()->GetSuggestedFileName(), "*.*",
                        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
                    if ( fileDlg.ShowModal() == wxID_OK )
                    {
                        if (!wxRenameFile(evt.GetResponseFileName(), fileDlg.GetPath()))
                            wxLogError("Could not move file");
                    }

                    break;
                }
                case Page_Advanced:
                    UpdateAdvCount();
                    GetStatusBar()->SetStatusText("");
                    break;
                }
                break;

            case wxWebRequest::State_Failed:
                // Stop updating download progress
                m_downloadRequest = NULL;
                m_downloadProgressTimer.Stop();
                wxLogError("Web Request failed: %s", evt.GetErrorDescription());
                GetStatusBar()->SetStatusText("");
                break;
        }
    }

    void OnRequestData(wxWebRequestEvent& evt)
    {
        // Count zero bytes in data buffer
        bool notify = false;

        const char* p = (const char*) evt.GetDataBuffer();
        for ( size_t i = 0; i < evt.GetDataSize(); i++ )
        {
            if ( *p == 0 )
            {
                m_advCount++;
                notify = true;
            }

            p++;
        }

        if ( notify )
            CallAfter(&WebRequestFrame::UpdateAdvCount);
    }

    void UpdateAdvCount()
    {
        m_advCountStaticText->SetLabel(wxString::Format("%lld", m_advCount));
    }

    void OnProgressTimer(wxTimerEvent& WXUNUSED(evt))
    {
        if (!m_downloadRequest || m_downloadRequest->GetBytesExpectedToReceive() <= 0)
            return;

        
        m_downloadGauge->SetValue((m_downloadRequest->GetBytesReceived() * 100) /
            m_downloadRequest->GetBytesExpectedToReceive());

        m_downloadStaticText->SetLabelText(wxString::Format("%lld/%lld",
            m_downloadRequest->GetBytesReceived(), m_downloadRequest->GetBytesExpectedToReceive()));
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
            defaultURL = "https://httpbin.org/post";
            break;
        case Page_Download:
            defaultURL = "https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.1/wxWidgets-3.1.1.7z";
            break;
        case Page_Advanced:
            defaultURL = "https://httpbin.org/bytes/64000";
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

    wxGauge* m_downloadGauge;
    wxStaticText* m_downloadStaticText;
    wxTimer m_downloadProgressTimer;
    wxWebRequest* m_downloadRequest;

    wxStaticText* m_advCountStaticText;
    long long m_advCount;
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
