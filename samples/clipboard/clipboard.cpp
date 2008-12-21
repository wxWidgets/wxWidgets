/////////////////////////////////////////////////////////////////////////////
// Name:        clipboard.cpp
// Purpose:     clipbaord wxWidgets sample
// Author:      Robert Roebling
// RCS-ID:      $Id: minimal.cpp 53461 2008-05-05 23:30:33Z VZ $
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
 
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/clipbrd.h"

#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

enum AsyncRequestState
{
   Idle,
   Waiting,
   Finished
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);

    void OnQuit(wxCommandEvent&event);
    void OnAbout(wxCommandEvent&event);
    void OnWriteClipboardContents(wxCommandEvent&event);
    void OnUpdateUI(wxUpdateUIEvent&event);
    void OnClipboardChange(wxClipboardEvent&event);

private:
    wxTextCtrl        *m_textctrl;
    AsyncRequestState  m_request;
    bool               m_clipboardSupportsText;

    DECLARE_EVENT_TABLE()
};

enum
{
    ID_Quit   = wxID_EXIT,
    ID_About  = wxID_ABOUT,
    ID_Write  = 100,
    ID_Text   = 101
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Quit,  MyFrame::OnQuit)
    EVT_MENU(ID_About, MyFrame::OnAbout)
    EVT_BUTTON(ID_Write, MyFrame::OnWriteClipboardContents)
    EVT_UPDATE_UI(ID_Write, MyFrame::OnUpdateUI)
    EVT_CLIPBOARD_CHANGED(MyFrame::OnClipboardChange)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    MyFrame *frame = new MyFrame("wxClipboard sample");
    frame->Show(true);
    
    return true;
}

MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(sample));
    
    m_request = Idle;
    m_clipboardSupportsText = false;

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(ID_About, "&About...\tF1", "Show about dialog");

    fileMenu->Append(ID_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

    wxPanel *panel = new wxPanel( this, -1 );
    
    wxBoxSizer *main_sizer = new wxBoxSizer( wxVERTICAL );
    main_sizer->Add( new wxButton( panel, ID_Write, "Get clipboard text" ) );
    m_textctrl = new wxTextCtrl( panel, ID_Text, "", wxDefaultPosition,
      wxDefaultSize, wxTE_MULTILINE );
    main_sizer->Add( m_textctrl, 1, wxGROW );
    panel->SetSizer( main_sizer );
}

void MyFrame::OnWriteClipboardContents(wxCommandEvent& event)
{
   if (wxTheClipboard->Open())
   {
        if (wxTheClipboard->IsSupported( wxDF_UNICODETEXT ))
        {
            wxTextDataObject data;
            wxTheClipboard->GetData( data );
            m_textctrl->Clear();
            m_textctrl->SetValue( data.GetText() );
            
        }
        wxTheClipboard->Close();
   }
}

void MyFrame::OnClipboardChange(wxClipboardEvent&event)
{
    m_clipboardSupportsText = event.SupportsFormat( wxDF_UNICODETEXT );
    m_request = Finished;
}

void MyFrame::OnUpdateUI(wxUpdateUIEvent&event)
{
    if (m_request == Idle)
    {
        wxTheClipboard->IsSupportedAsync( this );
        m_request = Waiting;
        event.Enable( m_clipboardSupportsText ); // not yet known, assume last value
    } 
    else if (m_request == Waiting)
    {
        event.Enable( m_clipboardSupportsText ); // not yet known, assume last value
    }
    else if (m_request == Finished)
    {
        event.Enable( m_clipboardSupportsText );
        m_request = Idle;
    }
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
}


