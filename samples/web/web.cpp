/////////////////////////////////////////////////////////////////////////////
// Name:        wxiepanel.cpp
// Purpose:     wxBetterHTMLControl test
// Author:      Marianne Gagnon
// Id:          $Id$
// Copyright:   (c) 2010 Marianne Gagnon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/notifmsg.h>
#include <wx/settings.h>

#if wxUSE_STC
#include <wx/stc/stc.h> 
#else
#error "wxStyledTextControl is needed by this sample"
#endif

#include "wx/webview.h"
#include "wxlogo.xpm"
#include "back.xpm"
#include "forward.xpm"
#include "stop.xpm"
#include "refresh.xpm"

// --------------------------------------------------------------------------------------------------
//                                          SOURCE VIEW FRAME
// --------------------------------------------------------------------------------------------------
enum
{
    MARGIN_LINE_NUMBERS,
    //MARGIN_DIVIDER,
    //MARGIN_FOLDING
};

class SourceViewDialog : public wxDialog
{
public:

    void onClose(wxCloseEvent& evt)
    {
        EndModal( GetReturnCode() );
    }

    SourceViewDialog(wxWindow* parent, wxString source) :
        wxDialog(parent, wxID_ANY, "Source Code",
                 wxDefaultPosition, wxSize(700,500),
                 wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    {
        wxStyledTextCtrl* text = new wxStyledTextCtrl(this, wxID_ANY);

        //text->SetLexer(wxSTC_LEX_HTML);
        text->SetMarginWidth (MARGIN_LINE_NUMBERS, 50);
        text->StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (75, 75, 75) );
        text->StyleSetBackground (wxSTC_STYLE_LINENUMBER, wxColour (220, 220, 220));
        text->SetMarginType (MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER);
        
        text->SetWrapMode (wxSTC_WRAP_WORD);
        
        text->SetText(source);
        
        text->StyleClearAll();
        text->SetLexer(wxSTC_LEX_HTML);
        text->StyleSetForeground (wxSTC_H_DOUBLESTRING,     wxColour(255,0,0));
        text->StyleSetForeground (wxSTC_H_SINGLESTRING,     wxColour(255,0,0));
        text->StyleSetForeground (wxSTC_H_ENTITY,           wxColour(255,0,0));
        text->StyleSetForeground (wxSTC_H_TAG,              wxColour(0,150,0));
        text->StyleSetForeground (wxSTC_H_TAGUNKNOWN,       wxColour(0,150,0));
        text->StyleSetForeground (wxSTC_H_ATTRIBUTE,        wxColour(0,0,150));
        text->StyleSetForeground (wxSTC_H_ATTRIBUTEUNKNOWN, wxColour(0,0,150));
        text->StyleSetForeground (wxSTC_H_COMMENT,          wxColour(150,150,150));


        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(text, 1, wxEXPAND);
        SetSizer(sizer);
        
        Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(SourceViewDialog::onClose), NULL, this);
    }
    
    
};

// --------------------------------------------------------------------------------------------------
//                                           MAIN BROWSER CLASS
// --------------------------------------------------------------------------------------------------
class wxMiniApp : public wxApp
{
    wxTextCtrl* url;
    wxWebView* m_browser_ctrl;
    wxFrame* frame;
     
    wxToolBarToolBase* back;
    wxToolBarToolBase* forward;
    wxToolBarToolBase* stop;
    wxToolBarToolBase* reload;
    wxToolBarToolBase* tools;
    
    wxMenu* toolsMenu;
    wxMenuItem* tinySize;
    wxMenuItem* smallSize;
    wxMenuItem* mediumSize;
    wxMenuItem* largeSize;
    wxMenuItem* largestSize;
    
    //wxMenuItem* offlineMode;
    //wxMenuItem* onlineMode;
    
    wxLogWindow* logging;
    wxToolBar* m_toolbar;
    
    wxTimer* m_timer;
    int m_animation_angle;
    
    wxPanel* m_notification_panel;
    wxStaticText* m_notification_text;
    
public:
    // function called at the application initialization
    virtual bool OnInit();
    
    /** 
      * Implement timer to display the loading animation (OK, I admit this is totally irrelevant to
      * the HTML control being demonstrated here, but it's fun ;)
      */
    void onAnimationTimer(wxTimerEvent& evt)
    {
        m_animation_angle += 15;
        if (m_animation_angle > 360) m_animation_angle -= 360;
        
        wxBitmap image(32, 32);
        
        {
        wxMemoryDC dc;
        dc.SelectObject(image);
        dc.SetBackground(wxBrush(wxColour(255,0,255)));
        dc.Clear();
        
        if (m_animation_angle >= 0 && m_animation_angle <= 180)
        {
            dc.SetBrush(*wxYELLOW_BRUSH);
            dc.SetPen(*wxYELLOW_PEN);
            dc.DrawCircle(16 - int(sin(m_animation_angle*0.01745f /* convert to radians */)*14.0f),
                          16 + int(cos(m_animation_angle*0.01745f /* convert to radians */)*14.0f), 3 );
        }
        
        dc.DrawBitmap(wxBitmap(wxlogo_xpm), 0, 0, true);
        
        if (m_animation_angle > 180)
        {
            dc.SetBrush(*wxYELLOW_BRUSH);
            dc.SetPen(*wxYELLOW_PEN);
            dc.DrawCircle(16 - int(sin(m_animation_angle*0.01745f /* convert to radians */)*14.0f),
                          16 + int(cos(m_animation_angle*0.01745f /* convert to radians */)*14.0f), 3 );
        }
        }
        
        image.SetMask(new wxMask(image, wxColour(255,0,255)));
        m_toolbar->SetToolNormalBitmap(tools->GetId(), image);
    }
    
    /**
     * Method that retrieves the current state from the web control and updates the GUI
     * the reflect this current state.
     */
    void updateState()
    {
        m_toolbar->EnableTool( back->GetId(), m_browser_ctrl->CanGoBack() );
        m_toolbar->EnableTool( forward->GetId(), m_browser_ctrl->CanGoForward() );
        
        if (m_browser_ctrl->IsBusy())
        {
            //tools->SetLabel(_("Loading..."));
            
            if (m_timer == NULL)
            {
                m_timer = new wxTimer(this);
                this->Connect(wxEVT_TIMER, wxTimerEventHandler(wxMiniApp::onAnimationTimer), NULL, this);
            }
            m_timer->Start(100); // start animation timer
            
            m_toolbar->EnableTool( stop->GetId(), true );    
        }
        else
        {
            if (m_timer != NULL) m_timer->Stop(); // stop animation timer
            
            //tools->SetLabel(_("Tools"));
            m_toolbar->SetToolNormalBitmap(tools->GetId(), wxBitmap(wxlogo_xpm));
            m_toolbar->EnableTool( stop->GetId(), false );            
        }
        
        frame->SetTitle( m_browser_ctrl->GetCurrentTitle() );
        url->SetValue( m_browser_ctrl->GetCurrentURL() );
    }
    
    /**
     * Callback invoked when user entered an URL and pressed enter
     */
    void onUrl(wxCommandEvent& evt)
    {
        if (m_notification_panel->IsShown())
        {
            m_notification_panel->Hide();
            frame->Layout();
        }
        
        m_browser_ctrl->LoadUrl( url->GetValue() );
        updateState();
    }
    
    /**
     * Callback invoked when user pressed the "back" button
     */
    void onBack(wxCommandEvent& evt)
    {
        // First, hide notification panel if it was shown
        if (m_notification_panel->IsShown())
        {
            m_notification_panel->Hide();
            frame->Layout();
        }
        
        m_browser_ctrl->GoBack();
        updateState();
    }
    
    /**
     * Callback invoked when user pressed the "forward" button
     */
    void onForward(wxCommandEvent& evt)
    {
        // First, hide notification panel if it was shown
        if (m_notification_panel->IsShown())
        {
            m_notification_panel->Hide();
            frame->Layout();
        }
        
        m_browser_ctrl->GoForward();
        updateState();
    }
    
    /**
     * Callback invoked when user pressed the "stop" button
     */
    void onStop(wxCommandEvent& evt)
    {
        m_browser_ctrl->Stop();
        updateState();
    }
    
    /**
     * Callback invoked when user pressed the "reload" button
     */
    void onReload(wxCommandEvent& evt)
    {
        // First, hide notification panel if it was shown
        if (m_notification_panel->IsShown())
        {
            m_notification_panel->Hide();
            frame->Layout();
        }
        
        m_browser_ctrl->Reload();
        updateState();
    }
    
    /**
     * Callback invoked when there is a request to load a new page (for instance
     * when the user clicks a link)
     */
    void onNavigationRequest(wxWebNavigationEvent& evt)
    {
        // First, hide notification panel if it was shown
        if (m_notification_panel->IsShown())
        {
            m_notification_panel->Hide();
            frame->Layout();
        }
        
        wxLogMessage("%s", "Navigation request to '" + evt.GetHref() + "' (target='" +
                   evt.GetTarget() + "')");
        
        wxASSERT(m_browser_ctrl->IsBusy());
        
        // Uncomment this to see how to block navigation requests
        //int answer = wxMessageBox("Proceed with navigation to '" + evt.GetHref() + "'?",
        //                          "Proceed with navigation?", wxYES_NO );
        //if (answer != wxYES)
        //{
        //    evt.Veto();
        //}
        updateState();
    }
    
    /**
     * Callback invoked when a navigation request was accepted
     */
    void onNavigationComplete(wxWebNavigationEvent& evt)
    {
        wxLogMessage("%s", "Navigation complete; url='" + evt.GetHref() + "'");
        updateState();
    }
    
    /**
     * Callback invoked when a page is finished loading
     */
    void onDocumentLoaded(wxWebNavigationEvent& evt)
    {
        wxLogMessage("%s", "Document loaded; url='" + evt.GetHref() + "'");
        updateState();
        
        m_browser_ctrl->GetZoom();
    }
    
    /**
     * Invoked when user selects the "View Source" menu item
     */
    void onViewSourceRequest(wxCommandEvent& evt)
    {
        SourceViewDialog dlg(frame, m_browser_ctrl->GetPageSource());
        dlg.Center();
        dlg.ShowModal();
    }
    
    /**
     * Invoked when user selects the "Menu" item
     */
    void onToolsClicked(wxCommandEvent& evt)
    {
        tinySize->Check(false);
        smallSize->Check(false); 
        mediumSize->Check(false);
        largeSize->Check(false);
        largestSize->Check(false);
        
        wxWebViewZoom zoom = m_browser_ctrl->GetZoom();
        switch (zoom)
        {
            case wxWEB_VIEW_ZOOM_TINY:
                tinySize->Check();
                break;
            case wxWEB_VIEW_ZOOM_SMALL:
                smallSize->Check();
                break;
            case wxWEB_VIEW_ZOOM_MEDIUM:
                mediumSize->Check();
                break;
            case wxWEB_VIEW_ZOOM_LARGE:
                largeSize->Check();
                break;
            case wxWEB_VIEW_ZOOM_LARGEST:
                largestSize->Check();
                break;
        }
        
        //    bool IsOfflineMode();
        //    void SetOfflineMode(bool offline);
        
        //offlineMode->Check(false);
        //onlineMode->Check(false);
        //const bool offline = m_browser_ctrl->IsOfflineMode();
        //if (offline) offlineMode->Check();
        //else         onlineMode->Check();
        
        wxPoint position = frame->ScreenToClient( wxGetMousePosition() );
        frame->PopupMenu(toolsMenu, position.x, position.y);
    }
    
    /**
     * Invoked when user selects the zoom size in the menu
     */
    void onSetZoom(wxCommandEvent& evt)
    {
        if (evt.GetId() == tinySize->GetId())
        {
            m_browser_ctrl->SetZoom(wxWEB_VIEW_ZOOM_TINY);
        }
        else if (evt.GetId() == smallSize->GetId())
        {
            m_browser_ctrl->SetZoom(wxWEB_VIEW_ZOOM_SMALL);
        }
        else if (evt.GetId() == mediumSize->GetId())
        {
            m_browser_ctrl->SetZoom(wxWEB_VIEW_ZOOM_MEDIUM);
        }
        else if (evt.GetId() == largeSize->GetId())
        {
            m_browser_ctrl->SetZoom(wxWEB_VIEW_ZOOM_LARGE);
        }
        else if (evt.GetId() == largestSize->GetId())
        {
            m_browser_ctrl->SetZoom(wxWEB_VIEW_ZOOM_LARGEST);
        }
        else
        {
            wxASSERT(false);
        }
    }
    
    /*
    void onChangeOnlineMode(wxCommandEvent& evt)
    {
        if (evt.GetId() == offlineMode->GetId())
        {
            m_browser_ctrl->SetOfflineMode(true);
            m_browser_ctrl->SetPage("<html><body><h1>You are now in offline mode.</h1></body></html>");
        }
        else if (evt.GetId() == onlineMode->GetId())
        {
            m_browser_ctrl->SetOfflineMode(false);
        }
        else
        {
            wxASSERT(false);
        }
    }
    */
    
    /**
     * Callback invoked when a loading error occurs
     */
    void onError(wxWebNavigationEvent& evt)
    {
        wxString errorCategory;
        switch (evt.GetInt())
        {
            case  wxWEB_NAV_ERR_CONNECTION:
            errorCategory = "wxWEB_NAV_ERR_CONNECTION";
            break;
            
            case wxWEB_NAV_ERR_CERTIFICATE:
            errorCategory = "wxWEB_NAV_ERR_CERTIFICATE";
            break;
            
            case wxWEB_NAV_ERR_AUTH:
            errorCategory = "wxWEB_NAV_ERR_AUTH";
            break;
            
            case wxWEB_NAV_ERR_SECURITY:
            errorCategory = "wxWEB_NAV_ERR_SECURITY";
            break;
            
            case wxWEB_NAV_ERR_NOT_FOUND:
            errorCategory = "wxWEB_NAV_ERR_NOT_FOUND";
            break;
            
            case wxWEB_NAV_ERR_REQUEST:
            errorCategory = "wxWEB_NAV_ERR_REQUEST";
            break;
            
            case wxWEB_NAV_ERR_USER_CANCELLED:
            errorCategory = "wxWEB_NAV_ERR_USER_CANCELLED";
            break;
            
            case wxWEB_NAV_ERR_OTHER:
            errorCategory = "wxWEB_NAV_ERR_OTHER";
            break;
        }
        
        wxLogMessage("Error; url='" + evt.GetHref() + "', error='" + errorCategory + "' (" + evt.GetString() + ")");
        
        // show the notification panel
        m_notification_text->SetLabel(_("An error occurred loading ") + evt.GetHref() + "\n" +
                                      "'" + errorCategory + "' (" + evt.GetString() + ")");
        m_notification_panel->Layout();
        m_notification_panel->GetSizer()->SetSizeHints(m_notification_panel);
        m_notification_panel->Show();
        frame->Layout();
    
        updateState();
    }
    
    /**
     * Invoked when user clicks "Hide" in the notification panel
     */
    void onHideNotifBar(wxCommandEvent& evt)
    {
        m_notification_panel->Hide();
        frame->Layout();
    }
    
    void onClose(wxCloseEvent& evt)
    {
        frame->Destroy();
    }
    
    void onQuitMenu(wxCommandEvent& evt)
    {
        frame->Destroy();
    }
    
    /**
     * Invoked when user selects "Print" from the menu
     */
    void onPrint(wxCommandEvent& evt)
    {
        m_browser_ctrl->Print();
    }
};

IMPLEMENT_APP(wxMiniApp);

bool wxMiniApp::OnInit()
{
    m_timer = NULL;
    m_animation_angle = 0;
    
    frame = new wxFrame( NULL, -1, _("wxBetterHTMLControl Browser Example"), wxDefaultPosition, wxSize(800, 600) );
    
    // wx has a default mechanism to expand the only control of a frame; but since this mechanism
    // does not involve sizers, invoking ->Layout on the frame does not udpate the layout which is
    // not good.
    wxBoxSizer* expandSizer = new wxBoxSizer(wxHORIZONTAL);
    wxPanel* mainpane = new wxPanel(frame, wxID_ANY);
    expandSizer->Add(mainpane, 1, wxEXPAND);
    frame->SetSizer(expandSizer);

    wxLog::SetLogLevel(wxLOG_Max);
    logging = new wxLogWindow(frame, _("Logging"));
    wxLog::SetLogLevel(wxLOG_Max);
    
    // ---- Create the Tools menu
    toolsMenu = new wxMenu();
    wxMenuItem* print = toolsMenu->Append(wxID_ANY , _("Print"));
    wxMenuItem* viewSource = toolsMenu->Append(wxID_ANY , _("View Source"));
    toolsMenu->AppendSeparator();
    tinySize    = toolsMenu->AppendCheckItem(wxID_ANY, _("Tiny"));
    smallSize   = toolsMenu->AppendCheckItem(wxID_ANY, _("Small"));
    mediumSize  = toolsMenu->AppendCheckItem(wxID_ANY, _("Medium"));
    largeSize   = toolsMenu->AppendCheckItem(wxID_ANY, _("Large"));
    largestSize = toolsMenu->AppendCheckItem(wxID_ANY, _("Largest"));
    //toolsMenu->AppendSeparator();
    //offlineMode = toolsMenu->AppendCheckItem(wxID_ANY, _("Offline Mode"));
    //onlineMode  = toolsMenu->AppendCheckItem(wxID_ANY, _("Online Mode"));

    // ---- Create the Toolbar
    m_toolbar = frame->CreateToolBar(/*wxNO_BORDER |*/ wxTB_TEXT);
    m_toolbar->SetToolBitmapSize(wxSize(32, 32));
    
    back    = m_toolbar->AddTool(wxID_ANY, _("Back"),    wxBitmap(back_xpm));
    forward = m_toolbar->AddTool(wxID_ANY, _("Forward"), wxBitmap(forward_xpm));
    stop    = m_toolbar->AddTool(wxID_ANY, _("Stop"),    wxBitmap(stop_xpm));
    reload  = m_toolbar->AddTool(wxID_ANY, _("Reload"),  wxBitmap(refresh_xpm));
    
    url = new wxTextCtrl(m_toolbar, wxID_ANY, wxT("http://www.google.com"), 
                         wxDefaultPosition, wxSize(400, -1), wxTE_PROCESS_ENTER );
    m_toolbar->AddControl(url, _("URL"));    
    tools   = m_toolbar->AddTool(wxID_ANY, _("Menu"), wxBitmap(wxlogo_xpm));
    //m_toolbar->SetDropdownMenu(tools->GetId(), toolsMenu);

    m_toolbar->Realize();

    m_toolbar->Connect(back->GetId(),    wxEVT_COMMAND_TOOL_CLICKED,
                                         wxCommandEventHandler(wxMiniApp::onBack),    NULL, this );
    m_toolbar->Connect(forward->GetId(), wxEVT_COMMAND_TOOL_CLICKED,
                                         wxCommandEventHandler(wxMiniApp::onForward), NULL, this );
    m_toolbar->Connect(stop->GetId(),    wxEVT_COMMAND_TOOL_CLICKED,
                                         wxCommandEventHandler(wxMiniApp::onStop),    NULL, this );
    m_toolbar->Connect(reload->GetId(),  wxEVT_COMMAND_TOOL_CLICKED,
                                         wxCommandEventHandler(wxMiniApp::onReload),  NULL, this );
    m_toolbar->Connect(tools->GetId(),   wxEVT_COMMAND_TOOL_CLICKED,
                                         wxCommandEventHandler(wxMiniApp::onToolsClicked),  NULL, this );

    url->Connect(url->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(wxMiniApp::onUrl), NULL, this );


    frame->Connect(viewSource->GetId(), wxEVT_COMMAND_MENU_SELECTED,
                   wxCommandEventHandler(wxMiniApp::onViewSourceRequest),  NULL, this );
    frame->Connect(print->GetId(), wxEVT_COMMAND_MENU_SELECTED,
                   wxCommandEventHandler(wxMiniApp::onPrint),  NULL, this );
    
    frame->Connect(tinySize->GetId(), wxEVT_COMMAND_MENU_SELECTED,
                   wxCommandEventHandler(wxMiniApp::onSetZoom),  NULL, this );
    frame->Connect(smallSize->GetId(), wxEVT_COMMAND_MENU_SELECTED,
                   wxCommandEventHandler(wxMiniApp::onSetZoom),  NULL, this );
    frame->Connect(mediumSize->GetId(), wxEVT_COMMAND_MENU_SELECTED,
                   wxCommandEventHandler(wxMiniApp::onSetZoom),  NULL, this );
    frame->Connect(largeSize->GetId(), wxEVT_COMMAND_MENU_SELECTED,
                   wxCommandEventHandler(wxMiniApp::onSetZoom),  NULL, this );
    frame->Connect(largestSize->GetId(), wxEVT_COMMAND_MENU_SELECTED,
                   wxCommandEventHandler(wxMiniApp::onSetZoom),  NULL, this );

    // ---- Create the web view
    m_browser_ctrl = wxWebView::New(mainpane, wxID_ANY);
    
    // ---- Create the notification panel
    {
    wxBoxSizer* notification_sizer = new wxBoxSizer(wxHORIZONTAL);        
    m_notification_panel = new wxPanel(mainpane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    m_notification_text = new wxStaticText(m_notification_panel, wxID_ANY, "[No message]");
    notification_sizer->Add( new wxStaticBitmap(m_notification_panel, wxID_ANY,
                                                    wxArtProvider::GetBitmap(wxART_WARNING, wxART_OTHER , wxSize(48, 48))),
                                                    0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );    
    notification_sizer->Add(m_notification_text, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALL, 5);
    wxButton* hideNotif = new wxButton(m_notification_panel, wxID_ANY, _("Hide"));
    notification_sizer->Add(hideNotif, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    m_notification_panel->SetSizer(notification_sizer);
    m_notification_panel->SetBackgroundColour(wxColor(255,225,110));
    m_notification_panel->Hide();
    hideNotif->Connect(hideNotif->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
                       wxCommandEventHandler(wxMiniApp::onHideNotifBar), NULL, this);
    }
    
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_notification_panel, 0,  wxEXPAND | wxALL, 5);
    sizer->Add(m_browser_ctrl, 1, wxEXPAND | wxALL, 5);
    
    mainpane->SetSizer(sizer);
    frame->Layout();
    frame->Center();
    frame->Show();
    
    m_browser_ctrl->Connect(m_browser_ctrl->GetId(), wxEVT_COMMAND_WEB_VIEW_NAVIGATING,
                      wxWebNavigationEventHandler(wxMiniApp::onNavigationRequest), NULL, this);
    
    m_browser_ctrl->Connect(m_browser_ctrl->GetId(), wxEVT_COMMAND_WEB_VIEW_NAVIGATED,
                      wxWebNavigationEventHandler(wxMiniApp::onNavigationComplete), NULL, this);
    
    m_browser_ctrl->Connect(m_browser_ctrl->GetId(), wxEVT_COMMAND_WEB_VIEW_LOADED,
                      wxWebNavigationEventHandler(wxMiniApp::onDocumentLoaded), NULL, this);
            
    m_browser_ctrl->Connect(m_browser_ctrl->GetId(), wxEVT_COMMAND_WEB_VIEW_ERROR,
                      wxWebNavigationEventHandler(wxMiniApp::onError), NULL, this);

    frame->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(wxMiniApp::onClose), NULL, this);
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxMiniApp::onQuitMenu), NULL, this);

    // You can test different zoom types (if supported by the backend)
    // if (m_browser_ctrl->CanSetZoomType(wxWEB_VIEW_ZOOM_TYPE_LAYOUT))
    //     m_browser_ctrl->SetZoomType(wxWEB_VIEW_ZOOM_TYPE_LAYOUT);

    SetTopWindow(frame);
    frame->Layout();

    return true;
}
