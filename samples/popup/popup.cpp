/////////////////////////////////////////////////////////////////////////////
// Name:        popup.cpp
// Purpose:     Popup wxWidgets sample
// Author:      Robert Roebling
// Modified by:
// Created:     2005-02-04
// Copyright:   (c) 2005 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/popupwin.h"
#include "wx/spinctrl.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    Minimal_Quit = wxID_EXIT,
    Minimal_About = wxID_ABOUT,
    Minimal_TestDialog,
    Minimal_StartSimplePopup,
    Minimal_StartScrolledPopup,
    Minimal_LogWindow,
    Minimal_PopupButton,
    Minimal_PopupSpinctrl
};

//----------------------------------------------------------------------------
// SimpleTransientPopup
//----------------------------------------------------------------------------
class SimpleTransientPopup: public wxPopupTransientWindow
{
public:
    SimpleTransientPopup( wxWindow *parent, bool scrolled );
    virtual ~SimpleTransientPopup();

    // wxPopupTransientWindow virtual methods are all overridden to log them
    virtual void Popup(wxWindow *focus = nullptr) override;
    virtual void OnDismiss() override;
    virtual bool ProcessLeftDown(wxMouseEvent& event) override;
    virtual bool Show( bool show = true ) override;

private:
    wxScrolledWindow *m_panel;
    wxButton *m_button;
    wxSpinCtrl *m_spinCtrl;
    wxStaticText *m_mouseText;

private:
    void OnMouse( wxMouseEvent &event );
    void OnSize( wxSizeEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );
    void OnButton( wxCommandEvent& event );
    void OnSpinCtrl( wxSpinEvent& event );

private:
    wxDECLARE_ABSTRACT_CLASS(SimpleTransientPopup);
    wxDECLARE_EVENT_TABLE();
};

//----------------------------------------------------------------------------
// SimpleTransientPopup
//----------------------------------------------------------------------------
wxIMPLEMENT_CLASS(SimpleTransientPopup,wxPopupTransientWindow);

wxBEGIN_EVENT_TABLE(SimpleTransientPopup,wxPopupTransientWindow)
    EVT_MOUSE_EVENTS( SimpleTransientPopup::OnMouse )
    EVT_SIZE( SimpleTransientPopup::OnSize )
    EVT_SET_FOCUS( SimpleTransientPopup::OnSetFocus )
    EVT_KILL_FOCUS( SimpleTransientPopup::OnKillFocus )
    EVT_BUTTON( Minimal_PopupButton, SimpleTransientPopup::OnButton )
    EVT_SPINCTRL( Minimal_PopupSpinctrl, SimpleTransientPopup::OnSpinCtrl )
wxEND_EVENT_TABLE()

SimpleTransientPopup::SimpleTransientPopup( wxWindow *parent, bool scrolled )
                     :wxPopupTransientWindow( parent,
                                              wxBORDER_NONE |
                                              wxPU_CONTAINS_CONTROLS )
{
    wxColour colour = wxSystemSettings::SelectLightDark(*wxLIGHT_GREY, wxColour(90, 90, 90));

    m_panel = new wxScrolledWindow( this, wxID_ANY );
    m_panel->SetBackgroundColour(colour);

    // Keep this code to verify if mouse events work, they're required if
    // you're making a control like a combobox where the items are highlighted
    // under the cursor, the m_panel is set focus in the Popup() function
    m_panel->Bind(wxEVT_MOTION, &SimpleTransientPopup::OnMouse, this);

    wxStaticText *text = new wxStaticText( m_panel, wxID_ANY,
                          "wxPopupTransientWindow is a\n"
                          "wxPopupWindow which disappears\n"
                          "automatically when the user\n"
                          "clicks the mouse outside it or if it\n"
                          "(or its first child) loses focus in \n"
                          "any other way." );

    m_button = new wxButton(m_panel, Minimal_PopupButton, "Press Me");
    m_spinCtrl = new wxSpinCtrl(m_panel, Minimal_PopupSpinctrl, "Hello");
    m_mouseText = new wxStaticText(m_panel, wxID_ANY,
                                   "<- Test Mouse ->");

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    topSizer->Add( text, 0, wxALL, 5 );
    topSizer->Add( m_button, 0, wxALL, 5 );
    topSizer->Add( m_spinCtrl, 0, wxALL, 5 );
    topSizer->Add( new wxTextCtrl(m_panel, wxID_ANY, "Try to type here"),
                   0, wxEXPAND|wxALL, 5 );
    topSizer->Add( m_mouseText, 0, wxCENTRE|wxALL, 5 );

    if ( scrolled )
    {
        // Add a big window to ensure that scrollbars are shown when we set the
        // panel size to a lesser size below.
        topSizer->Add(new wxPanel(m_panel, wxID_ANY, wxDefaultPosition,
                                  wxSize(600, 900)));
    }

    m_panel->SetSizer( topSizer );
    if ( scrolled )
    {
        // Set the fixed size to ensure that the scrollbars are shown.
        m_panel->SetSize(300, 300);

        // And also actually enable them.
        m_panel->SetScrollRate(10, 10);
    }
    else
    {
        // Use the fitting size for the panel if we don't need scrollbars.
        topSizer->Fit(m_panel);
    }

    SetClientSize(m_panel->GetSize());
}

SimpleTransientPopup::~SimpleTransientPopup()
{
}

void SimpleTransientPopup::Popup(wxWindow* WXUNUSED(focus))
{
    wxLogMessage( "%p SimpleTransientPopup::Popup", this );
    wxPopupTransientWindow::Popup();
}

void SimpleTransientPopup::OnDismiss()
{
    wxLogMessage( "%p SimpleTransientPopup::OnDismiss", this );
    wxPopupTransientWindow::OnDismiss();
}

bool SimpleTransientPopup::ProcessLeftDown(wxMouseEvent& event)
{
    wxLogMessage( "%p SimpleTransientPopup::ProcessLeftDown pos(%d, %d)", this, event.GetX(), event.GetY());
    return wxPopupTransientWindow::ProcessLeftDown(event);
}
bool SimpleTransientPopup::Show( bool show )
{
    wxLogMessage( "%p SimpleTransientPopup::Show %d", this, int(show));
    return wxPopupTransientWindow::Show(show);
}

void SimpleTransientPopup::OnSize(wxSizeEvent &event)
{
    wxLogMessage( "%p SimpleTransientPopup::OnSize", this );
    event.Skip();
}

void SimpleTransientPopup::OnSetFocus(wxFocusEvent &event)
{
    wxLogMessage( "%p SimpleTransientPopup::OnSetFocus", this );
    event.Skip();
}

void SimpleTransientPopup::OnKillFocus(wxFocusEvent &event)
{
    wxLogMessage( "%p SimpleTransientPopup::OnKillFocus", this );
    event.Skip();
}

void SimpleTransientPopup::OnMouse(wxMouseEvent &event)
{
    wxRect rect(m_mouseText->GetRect());
    rect.SetX(-100000);
    rect.SetWidth(1000000);
    wxColour colour = wxSystemSettings::SelectLightDark(*wxLIGHT_GREY, wxColour(90, 90, 90));

    if (rect.Contains(event.GetPosition()))
    {
        colour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        wxLogMessage( "%p SimpleTransientPopup::OnMouse pos(%d, %d)",
                      event.GetEventObject(), event.GetX(), event.GetY());
    }

    if (colour != m_mouseText->GetBackgroundColour())
    {
        m_mouseText->SetBackgroundColour(colour);
        m_mouseText->Refresh();
    }
    event.Skip();
}

void SimpleTransientPopup::OnButton(wxCommandEvent& event)
{
    wxLogMessage( "%p SimpleTransientPopup::OnButton ID %d", this, event.GetId());

    wxButton *button = wxDynamicCast(event.GetEventObject(), wxButton);
    if (button->GetLabel() == "Press Me")
        button->SetLabel("Pressed");
    else
        button->SetLabel("Press Me");

    event.Skip();
}

void SimpleTransientPopup::OnSpinCtrl(wxSpinEvent& event)
{
    wxLogMessage( "%p SimpleTransientPopup::OnSpinCtrl ID %d Value %d",
                  this, event.GetId(), event.GetInt());
    event.Skip();
}

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class MyDialog : public wxDialog
{
public:
    MyDialog(const wxString& title);

    void OnStartSimplePopup(wxCommandEvent& event);
    void OnStartScrolledPopup(wxCommandEvent& event);

private:
    SimpleTransientPopup *m_simplePopup;
    SimpleTransientPopup *m_scrolledPopup;
    wxDECLARE_EVENT_TABLE();
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);
    virtual ~MyFrame();

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTestDialog(wxCommandEvent& event);
    void OnStartSimplePopup(wxCommandEvent& event);
    void OnStartScrolledPopup(wxCommandEvent& event);
    void OnActivate(wxActivateEvent& event);

private:
    SimpleTransientPopup *m_simplePopup;
    SimpleTransientPopup *m_scrolledPopup;
    wxTextCtrl *m_logWin;
    wxLog *m_logOld;
    wxDECLARE_EVENT_TABLE();
};

class MyApp : public wxApp
{
public:
    virtual bool OnInit() override;

    MyFrame *m_frame;
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------


wxIMPLEMENT_APP(MyApp);

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    m_frame = new MyFrame("Popup wxWidgets App");

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    m_frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
    EVT_MENU(Minimal_TestDialog, MyFrame::OnTestDialog)
    EVT_ACTIVATE(MyFrame::OnActivate)
    EVT_BUTTON(Minimal_StartSimplePopup, MyFrame::OnStartSimplePopup)
    EVT_BUTTON(Minimal_StartScrolledPopup, MyFrame::OnStartScrolledPopup)
wxEND_EVENT_TABLE()

MyFrame::MyFrame(const wxString& title)
: wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(500,300))
{
    m_simplePopup = m_scrolledPopup = nullptr;

    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

    menuFile->Append(Minimal_TestDialog, "&Test dialog\tAlt-T", "Test dialog");
    menuFile->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
#endif // wxUSE_STATUSBAR

    wxPanel *panel = new wxPanel(this, -1);
    wxButton *button1 = new wxButton( panel, Minimal_StartSimplePopup, "Show simple popup", wxPoint(20,20) );
    wxButton *button2 = new wxButton( panel, Minimal_StartScrolledPopup, "Show scrolled popup", wxPoint(20,70) );

    m_logWin = new wxTextCtrl( panel, wxID_ANY, wxEmptyString, wxDefaultPosition,
                               wxDefaultSize, wxTE_MULTILINE );
    m_logWin->SetEditable(false);
    wxLogTextCtrl* logger = new wxLogTextCtrl( m_logWin );
    m_logOld = logger->SetActiveTarget( logger );
    logger->DisableTimestamp();

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    topSizer->Add( button1, 0, wxALL, 5 );
    topSizer->Add( button2, 0, wxALL, 5 );
    topSizer->Add( m_logWin, 1, wxEXPAND|wxALL, 5 );

    panel->SetSizer( topSizer );

}

MyFrame::~MyFrame()
{
    delete wxLog::SetActiveTarget(m_logOld);
}


// event handlers

void MyFrame::OnActivate(wxActivateEvent& WXUNUSED(event))
{
    wxLogMessage( "In activate..." );
}

void MyFrame::OnStartSimplePopup(wxCommandEvent& event)
{
    wxLogMessage( "================================================" );
    delete m_simplePopup;
    m_simplePopup = new SimpleTransientPopup( this, false );
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    m_simplePopup->Position( pos, sz );
    wxLogMessage( "%p Simple Popup Shown pos(%d, %d) size(%d, %d)", m_simplePopup, pos.x, pos.y, sz.x, sz.y );
    m_simplePopup->Popup();
}

void MyFrame::OnStartScrolledPopup(wxCommandEvent& event)
{
    wxLogMessage( "================================================" );
    delete m_scrolledPopup;
    m_scrolledPopup = new SimpleTransientPopup( this, true );
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    m_scrolledPopup->Position( pos, sz );
    wxLogMessage( "%p Scrolled Popup Shown pos(%d, %d) size(%d, %d)", m_scrolledPopup, pos.x, pos.y, sz.x, sz.y );
    m_scrolledPopup->Popup();
}

void MyFrame::OnTestDialog(wxCommandEvent& WXUNUSED(event))
{
    MyDialog dialog( "Test" );
    dialog.ShowModal();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( "This is the About dialog of the popup sample.\n"
                "Welcome to %s", wxVERSION_STRING);

    wxMessageBox(msg, "About Popup", wxOK | wxICON_INFORMATION, this);
}

// ----------------------------------------------------------------------------
// test dialog
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_BUTTON(Minimal_StartSimplePopup, MyDialog::OnStartSimplePopup)
    EVT_BUTTON(Minimal_StartScrolledPopup, MyDialog::OnStartScrolledPopup)
wxEND_EVENT_TABLE()

MyDialog::MyDialog(const wxString& title)
         :wxDialog(nullptr, wxID_ANY, title, wxPoint(50,50), wxSize(400,300))
{
    m_simplePopup = m_scrolledPopup = nullptr;
    wxPanel *panel = new wxPanel(this, -1);

    wxButton *button1 = new wxButton( panel, Minimal_StartSimplePopup, "Show simple popup", wxPoint(20,20) );
    wxButton *button2 = new wxButton( panel, Minimal_StartScrolledPopup, "Show scrolled popup", wxPoint(20,60) );

    wxButton *okButton = new wxButton( panel, wxID_OK, "OK", wxPoint(20,200) );

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    topSizer->Add( button1, 0, wxALL, 5 );
    topSizer->Add( button2, 0, wxALL, 5 );
    topSizer->AddSpacer(40);
    topSizer->Add( okButton, 0, wxALL, 5 );

    panel->SetSizerAndFit( topSizer );
}

void MyDialog::OnStartSimplePopup(wxCommandEvent& event)
{
    wxLogMessage( "================================================" );
    delete m_simplePopup;
    m_simplePopup = new SimpleTransientPopup( this, false );
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    m_simplePopup->Position( pos, sz );
    wxLogMessage( "%p Dialog Simple Popup Shown pos(%d, %d) size(%d, %d)", m_simplePopup, pos.x, pos.y, sz.x, sz.y );
    m_simplePopup->Popup();
}

void MyDialog::OnStartScrolledPopup(wxCommandEvent& event)
{
    wxLogMessage( "================================================" );
    delete m_scrolledPopup;
    m_scrolledPopup = new SimpleTransientPopup( this, true );
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    m_scrolledPopup->Position( pos, sz );
    wxLogMessage( "%p Dialog Scrolled Popup Shown pos(%d, %d) size(%d, %d)", m_scrolledPopup, pos.x, pos.y, sz.x, sz.y );
    m_scrolledPopup->Popup();
}
