/////////////////////////////////////////////////////////////////////////////
// Name:        web.cpp
// Purpose:     wxWebView sample
// Author:      Marianne Gagnon
// Id:          $Id$
// Copyright:   (c) 2010 Marianne Gagnon, Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/artprov.h>
#include <wx/notifmsg.h>
#include <wx/settings.h>
#include <wx/webview.h>
#include <wx/infobar.h>

#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

#if wxUSE_STC
#include <wx/stc/stc.h> 
#else
#error "wxStyledTextControl is needed by this sample"
#endif

#include "wxlogo.xpm"
#include "back.xpm"
#include "forward.xpm"
#include "stop.xpm"
#include "refresh.xpm"


class WebApp : public wxApp
{
public:
    virtual bool OnInit();
};

class WebFrame : public wxFrame
{
public:
    WebFrame();

    void OnAnimationTimer(wxTimerEvent& evt);
    void UpdateState();
    void OnUrl(wxCommandEvent& evt);
    void OnBack(wxCommandEvent& evt);
    void OnForward(wxCommandEvent& evt);
    void OnStop(wxCommandEvent& evt);
    void OnReload(wxCommandEvent& evt);
    void OnNavigationRequest(wxWebNavigationEvent& evt);
    void OnNavigationComplete(wxWebNavigationEvent& evt);
    void OnDocumentLoaded(wxWebNavigationEvent& evt);
    void OnNewWindow(wxWebNavigationEvent& evt);
    void OnViewSourceRequest(wxCommandEvent& evt);
    void OnToolsClicked(wxCommandEvent& evt);
    void OnSetZoom(wxCommandEvent& evt);
    void OnError(wxWebNavigationEvent& evt);
    void OnPrint(wxCommandEvent& evt);

private:
    wxTextCtrl* m_url;
    wxWebView* m_browser;

    wxToolBar* m_toolbar;
    wxToolBarToolBase* m_toolbar_back;
    wxToolBarToolBase* m_toolbar_forward;
    wxToolBarToolBase* m_toolbar_stop;
    wxToolBarToolBase* m_toolbar_reload;
    wxToolBarToolBase* m_toolbar_tools;

    wxMenu* m_tools_menu;
    wxMenuItem* m_tools_tiny;
    wxMenuItem* m_tools_small;
    wxMenuItem* m_tools_medium;
    wxMenuItem* m_tools_large;
    wxMenuItem* m_tools_largest;

    wxTimer* m_timer;
    int m_animation_angle;


    wxInfoBar *m_info;
    wxStaticText* m_info_text;
};

class SourceViewDialog : public wxDialog
{
public:
    SourceViewDialog(wxWindow* parent, wxString source);
};

IMPLEMENT_APP(WebApp)

// ============================================================================
// implementation
// ============================================================================

bool WebApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    WebFrame *frame = new WebFrame();
    frame->Show();

    return true;
}

WebFrame::WebFrame() : wxFrame(NULL, wxID_ANY, "wxWebView Sample")
{
    // set the frame icon
    SetIcon(wxICON(sample));
    SetTitle("wxWebView Sample");

    m_timer = NULL;
    m_animation_angle = 0;


    wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);

    // Create the toolbar
    m_toolbar = CreateToolBar(wxTB_TEXT);
    m_toolbar->SetToolBitmapSize(wxSize(32, 32));
    
    m_toolbar_back = m_toolbar->AddTool(wxID_ANY, _("Back"),    wxBitmap(back_xpm));
    m_toolbar_forward = m_toolbar->AddTool(wxID_ANY, _("Forward"), wxBitmap(forward_xpm));
    m_toolbar_stop = m_toolbar->AddTool(wxID_ANY, _("Stop"),    wxBitmap(stop_xpm));
    m_toolbar_reload = m_toolbar->AddTool(wxID_ANY, _("Reload"),  wxBitmap(refresh_xpm));
    m_url = new wxTextCtrl(m_toolbar, wxID_ANY, wxT(""),  wxDefaultPosition, wxSize(400, -1), wxTE_PROCESS_ENTER );
    m_toolbar->AddControl(m_url, _("URL"));    
    m_toolbar_tools = m_toolbar->AddTool(wxID_ANY, _("Menu"), wxBitmap(wxlogo_xpm));

    m_toolbar->Realize();

    // Create the info panel
    m_info = new wxInfoBar(this);
    topsizer->Add(m_info, wxSizerFlags().Expand());

    // Create the webview
    m_browser = wxWebView::New(this, wxID_ANY);
    m_browser->LoadUrl("http://www.wxwidgets.org");
    topsizer->Add(m_browser, wxSizerFlags().Expand().Proportion(1));

    SetSizer(topsizer);

    // Create a log window
    new wxLogWindow(this, _("Logging"));

    // Create the Tools menu
    m_tools_menu = new wxMenu();
    wxMenuItem* print = m_tools_menu->Append(wxID_ANY , _("Print"));
    wxMenuItem* viewSource = m_tools_menu->Append(wxID_ANY , _("View Source"));
    m_tools_menu->AppendSeparator();
    m_tools_tiny = m_tools_menu->AppendCheckItem(wxID_ANY, _("Tiny"));
    m_tools_small = m_tools_menu->AppendCheckItem(wxID_ANY, _("Small"));
    m_tools_medium = m_tools_menu->AppendCheckItem(wxID_ANY, _("Medium"));
    m_tools_large = m_tools_menu->AppendCheckItem(wxID_ANY, _("Large"));
    m_tools_largest = m_tools_menu->AppendCheckItem(wxID_ANY, _("Largest"));


    // Connect the toolbar events
    Connect(m_toolbar_back->GetId(), wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(WebFrame::OnBack), NULL, this );
    Connect(m_toolbar_forward->GetId(), wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(WebFrame::OnForward), NULL, this );
    Connect(m_toolbar_stop->GetId(), wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(WebFrame::OnStop), NULL, this );
    Connect(m_toolbar_reload->GetId(), wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(WebFrame::OnReload),NULL, this );
    Connect(m_toolbar_tools->GetId(), wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(WebFrame::OnToolsClicked), NULL, this );

    Connect(m_url->GetId(), wxEVT_COMMAND_TEXT_ENTER, 
            wxCommandEventHandler(WebFrame::OnUrl), NULL, this );

    // Connect the webview events
    Connect(m_browser->GetId(), wxEVT_COMMAND_WEB_VIEW_NAVIGATING,
            wxWebNavigationEventHandler(WebFrame::OnNavigationRequest), NULL, this);
    Connect(m_browser->GetId(), wxEVT_COMMAND_WEB_VIEW_NAVIGATED,
            wxWebNavigationEventHandler(WebFrame::OnNavigationComplete), NULL, this);
    Connect(m_browser->GetId(), wxEVT_COMMAND_WEB_VIEW_LOADED,
            wxWebNavigationEventHandler(WebFrame::OnDocumentLoaded), NULL, this);     
    Connect(m_browser->GetId(), wxEVT_COMMAND_WEB_VIEW_ERROR,
            wxWebNavigationEventHandler(WebFrame::OnError), NULL, this);
    Connect(m_browser->GetId(), wxEVT_COMMAND_WEB_VIEW_NEWWINDOW,
            wxWebNavigationEventHandler(WebFrame::OnNewWindow), NULL, this);

    // Connect the menu events
    Connect(viewSource->GetId(), wxEVT_COMMAND_MENU_SELECTED,
           wxCommandEventHandler(WebFrame::OnViewSourceRequest),  NULL, this );
    Connect(print->GetId(), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WebFrame::OnPrint),  NULL, this );
    Connect(m_tools_tiny->GetId(), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WebFrame::OnSetZoom),  NULL, this );
    Connect(m_tools_small->GetId(), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WebFrame::OnSetZoom),  NULL, this );
    Connect(m_tools_medium->GetId(), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WebFrame::OnSetZoom),  NULL, this );
    Connect(m_tools_large->GetId(), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WebFrame::OnSetZoom),  NULL, this );
    Connect(m_tools_largest->GetId(), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WebFrame::OnSetZoom),  NULL, this );
}

void WebFrame::OnAnimationTimer(wxTimerEvent& evt)
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
    m_toolbar->SetToolNormalBitmap(m_toolbar_tools->GetId(), image);
}

/**
  * Method that retrieves the current state from the web control and updates the GUI
  * the reflect this current state.
  */
void WebFrame::UpdateState()
{
    m_toolbar->EnableTool( m_toolbar_back->GetId(), m_browser->CanGoBack() );
    m_toolbar->EnableTool( m_toolbar_forward->GetId(), m_browser->CanGoForward() );
    
    if (m_browser->IsBusy())
    {
        if (m_timer == NULL)
        {
            m_timer = new wxTimer(this);
            this->Connect(wxEVT_TIMER, wxTimerEventHandler(WebFrame::OnAnimationTimer), NULL, this);
        }
        m_timer->Start(100); // start animation timer
        
        m_toolbar->EnableTool( m_toolbar_stop->GetId(), true );    
    }
    else
    {
        if (m_timer != NULL) m_timer->Stop(); // stop animation timer
        m_toolbar->SetToolNormalBitmap(m_toolbar_tools->GetId(), wxBitmap(wxlogo_xpm));
        m_toolbar->EnableTool( m_toolbar_stop->GetId(), false );            
    }
    
    SetTitle( m_browser->GetCurrentTitle() );
    m_url->SetValue( m_browser->GetCurrentURL() );
}

/**
  * Callback invoked when user entered an URL and pressed enter
  */
void WebFrame::OnUrl(wxCommandEvent& evt)
{
    m_browser->LoadUrl( m_url->GetValue() );
    UpdateState();
}

/**
    * Callback invoked when user pressed the "back" button
    */
void WebFrame::OnBack(wxCommandEvent& evt)
{
    m_browser->GoBack();
    UpdateState();
}

/**
  * Callback invoked when user pressed the "forward" button
  */
void WebFrame::OnForward(wxCommandEvent& evt)
{
    m_browser->GoForward();
    UpdateState();
}

/**
  * Callback invoked when user pressed the "stop" button
  */
void WebFrame::OnStop(wxCommandEvent& evt)
{
    m_browser->Stop();
    UpdateState();
}

/**
  * Callback invoked when user pressed the "reload" button
  */
void WebFrame::OnReload(wxCommandEvent& evt)
{
    m_browser->Reload();
    UpdateState();
}

/**
  * Callback invoked when there is a request to load a new page (for instance
  * when the user clicks a link)
  */
void WebFrame::OnNavigationRequest(wxWebNavigationEvent& evt)
{
    wxLogMessage("%s", "Navigation request to '" + evt.GetHref() + "' (target='" +
    evt.GetTarget() + "')");
    
    wxASSERT(m_browser->IsBusy());
    
    // Uncomment this to see how to block navigation requests
    //int answer = wxMessageBox("Proceed with navigation to '" + evt.GetHref() + "'?",
    //                          "Proceed with navigation?", wxYES_NO );
    //if (answer != wxYES)
    //{
    //    evt.Veto();
    //}
    UpdateState();
}

/**
  * Callback invoked when a navigation request was accepted
  */
void WebFrame::OnNavigationComplete(wxWebNavigationEvent& evt)
{
    wxLogMessage("%s", "Navigation complete; url='" + evt.GetHref() + "'");
    UpdateState();
}

/**
  * Callback invoked when a page is finished loading
  */
void WebFrame::OnDocumentLoaded(wxWebNavigationEvent& evt)
{
    wxLogMessage("%s", "Document loaded; url='" + evt.GetHref() + "'");
    UpdateState();
}

/**
  * On new window, we veto to stop extra windows appearing
  */
void WebFrame::OnNewWindow(wxWebNavigationEvent& evt)
{
    wxLogMessage("%s", "New window; url='" + evt.GetHref() + "'");
    evt.Veto();

    UpdateState();
}

/**
  * Invoked when user selects the "View Source" menu item
  */
void WebFrame::OnViewSourceRequest(wxCommandEvent& evt)
{
    SourceViewDialog dlg(this, m_browser->GetPageSource());
    dlg.ShowModal();
}

/**
  * Invoked when user selects the "Menu" item
  */
void WebFrame::OnToolsClicked(wxCommandEvent& evt)
{
    if(m_browser->GetCurrentURL() == "")
        return;

    m_tools_tiny->Check(false);
    m_tools_small->Check(false); 
    m_tools_medium->Check(false);
    m_tools_large->Check(false);
    m_tools_largest->Check(false);

    wxWebViewZoom zoom = m_browser->GetZoom();
    switch (zoom)
    {
    case wxWEB_VIEW_ZOOM_TINY:
        m_tools_tiny->Check();
        break;
    case wxWEB_VIEW_ZOOM_SMALL:
        m_tools_small->Check();
        break;
    case wxWEB_VIEW_ZOOM_MEDIUM:
        m_tools_medium->Check();
        break;
    case wxWEB_VIEW_ZOOM_LARGE:
        m_tools_large->Check();
        break;
    case wxWEB_VIEW_ZOOM_LARGEST:
        m_tools_largest->Check();
        break;
    }
    
    wxPoint position = ScreenToClient( wxGetMousePosition() );
    PopupMenu(m_tools_menu, position.x, position.y);
}

/**
  * Invoked when user selects the zoom size in the menu
  */
void WebFrame::OnSetZoom(wxCommandEvent& evt)
{
    if (evt.GetId() == m_tools_tiny->GetId())
    {
        m_browser->SetZoom(wxWEB_VIEW_ZOOM_TINY);
    }
    else if (evt.GetId() == m_tools_small->GetId())
    {
        m_browser->SetZoom(wxWEB_VIEW_ZOOM_SMALL);
    }
    else if (evt.GetId() == m_tools_medium->GetId())
    {
        m_browser->SetZoom(wxWEB_VIEW_ZOOM_MEDIUM);
    }
    else if (evt.GetId() == m_tools_large->GetId())
    {
        m_browser->SetZoom(wxWEB_VIEW_ZOOM_LARGE);
    }
    else if (evt.GetId() == m_tools_largest->GetId())
    {
        m_browser->SetZoom(wxWEB_VIEW_ZOOM_LARGEST);
    }
    else
    {
        wxFAIL;
    }
}

/**
  * Callback invoked when a loading error occurs
  */
void WebFrame::OnError(wxWebNavigationEvent& evt)
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
    
    //Show the info bar with an error
    m_info->ShowMessage(_("An error occurred loading ") + evt.GetHref() + "\n" +
    "'" + errorCategory + "' (" + evt.GetString() + ")", wxICON_ERROR);
    
    UpdateState();
}

/**
  * Invoked when user selects "Print" from the menu
  */
void WebFrame::OnPrint(wxCommandEvent& evt)
{
    m_browser->Print();
}

SourceViewDialog::SourceViewDialog(wxWindow* parent, wxString source) :
                  wxDialog(parent, wxID_ANY, "Source Code",
                           wxDefaultPosition, wxSize(700,500),
                           wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    wxStyledTextCtrl* text = new wxStyledTextCtrl(this, wxID_ANY);
    text->SetMarginWidth(1, 30);
    text->SetMarginType(1, wxSTC_MARGIN_NUMBER); 
    text->SetText(source);

    text->StyleClearAll();
    text->SetLexer(wxSTC_LEX_HTML);
    text->StyleSetForeground(wxSTC_H_DOUBLESTRING, wxColour(255,0,0));
    text->StyleSetForeground(wxSTC_H_SINGLESTRING, wxColour(255,0,0));
    text->StyleSetForeground(wxSTC_H_ENTITY, wxColour(255,0,0));
    text->StyleSetForeground(wxSTC_H_TAG, wxColour(0,150,0));
    text->StyleSetForeground(wxSTC_H_TAGUNKNOWN, wxColour(0,150,0));
    text->StyleSetForeground(wxSTC_H_ATTRIBUTE, wxColour(0,0,150));
    text->StyleSetForeground(wxSTC_H_ATTRIBUTEUNKNOWN, wxColour(0,0,150));
    text->StyleSetForeground(wxSTC_H_COMMENT, wxColour(150,150,150));

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(text, 1, wxEXPAND);
    SetSizer(sizer);
}
