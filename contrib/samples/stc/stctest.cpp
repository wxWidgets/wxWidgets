/////////////////////////////////////////////////////////////////////////////
// Name:        stctest.cpp
// Purpose:     sample of using wxStyledTextCtrl
// Author:      Robin Dunn
// Modified by:
// Created:     3-Feb-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "stctest.cpp"
    #pragma interface "stctest.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/wfstream.h>

#include <wx/stc/stc.h>

//----------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

//----------------------------------------------------------------------

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnStyleNeeded(wxStyledTextEvent& event);

private:
    wxStyledTextCtrl* ed;

    DECLARE_EVENT_TABLE()
};


// IDs for the controls and the menu commands
enum
{
    // menu items
    ID_Quit = 1,
    ID_About,
    ID_ED
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU            (ID_Quit,  MyFrame::OnQuit)
    EVT_MENU            (ID_About, MyFrame::OnAbout)
    EVT_STC_STYLENEEDED (ID_ED, MyFrame::OnStyleNeeded)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

//----------------------------------------------------------------------
// `Main program' equivalent: the program execution "starts" here

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame("Testing wxStyledTextCtrl",
                                 wxPoint(5, 5), wxSize(400, 600));

    frame->Show(TRUE);
    return TRUE;
}

//----------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = ID_About;
#endif


    // create a menu bar
    wxMenu *menuFile = new wxMenu("", wxMENU_TEAROFF);

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(ID_About, "&About...\tCtrl-A", "Show about dialog");

    menuFile->Append(ID_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    SetStatusText("Testing wxStyledTextCtrl");
#endif // wxUSE_STATUSBAR


    //----------------------------------------
    // Setup the editor
    ed = new wxStyledTextCtrl(this, ID_ED);

    // Default font
    wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
    ed->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    ed->StyleClearAll();

    ed->StyleSetForeground(0,  wxColour(0x80, 0x80, 0x80));
    ed->StyleSetForeground(1,  wxColour(0x00, 0x7f, 0x00));
    //ed->StyleSetForeground(2,  wxColour(0x00, 0x7f, 0x00));
    ed->StyleSetForeground(3,  wxColour(0x7f, 0x7f, 0x7f));
    ed->StyleSetForeground(4,  wxColour(0x00, 0x7f, 0x7f));
    ed->StyleSetForeground(5,  wxColour(0x00, 0x00, 0x7f));
    ed->StyleSetForeground(6,  wxColour(0x7f, 0x00, 0x7f));
    ed->StyleSetForeground(7,  wxColour(0x7f, 0x00, 0x7f));
    ed->StyleSetForeground(8,  wxColour(0x00, 0x7f, 0x7f));
    ed->StyleSetForeground(9,  wxColour(0x7f, 0x7f, 0x7f));
    ed->StyleSetForeground(10, wxColour(0x00, 0x00, 0x00));
    ed->StyleSetForeground(11, wxColour(0x00, 0x00, 0x00));
    ed->StyleSetBold(5,  TRUE);
    ed->StyleSetBold(10, TRUE);

#ifdef __WXMSW__
    ed->StyleSetSpec(2, "fore:#007f00,bold,face:Arial,size:7");
#else
    ed->StyleSetSpec(2, "fore:#007f00,bold,face:Helvetica,size:7");
#endif

    // give it some text to play with
    wxFile   file("stctest.cpp");
    wxString st;

    char* buff = st.GetWriteBuf(file.Length());
    file.Read(buff, file.Length());
    st.UngetWriteBuf();

    ed->InsertText(0, st);
    ed->EmptyUndoBuffer();

    ed->SetLexer(wxSTC_LEX_CPP);
    ed->SetKeyWords(0,
                    "asm auto bool break case catch char class const "
                    "const_cast continue default delete do double "
                    "dynamic_cast else enum explicit export extern "
                    "false float for friend goto if inline int long "
                    "mutable namespace new operator private protected "
                    "public register reinterpret_cast return short signed "
                    "sizeof static static_cast struct switch template this "
                    "throw true try typedef typeid typename union unsigned "
                    "using virtual void volatile wchar_t while");

}


// event handlers

void MyFrame::OnStyleNeeded(wxStyledTextEvent& event) {
    int currEndStyled = ed->GetEndStyled();
    ed->Colourise(currEndStyled, event.GetPosition());
}




void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("Testing wxStyledTextCtrl...\n"));

    wxMessageBox(msg, "About This Test", wxOK | wxICON_INFORMATION, this);
}
