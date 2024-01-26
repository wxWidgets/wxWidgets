/////////////////////////////////////////////////////////////////////////////
// Name:        samples/nativdlg/nativdlg.cpp
// Purpose:     Native Windows dialog sample
// Author:      Julian Smart
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifndef __WXMSW__
#error Sorry, this sample is only appropriate under Windows.
#endif

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

#include <ctype.h>
#include "nativdlg.h"
#include "resource.h"




wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit(void)
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main frame window
    MyFrame* frame = new MyFrame(nullptr, wxID_ANY, "wxWidgets Native Dialog Sample", wxDefaultPosition, wxDefaultSize);

#if wxUSE_STATUSBAR
    // Give it a status line
    frame->CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

    // Make a menubar
    wxMenu* file_menu = new wxMenu;

    file_menu->Append(RESOURCE_TEST1, "&Dialog box test", "Test dialog box resource");
    file_menu->Append(RESOURCE_QUIT, "E&xit", "Quit program");

    wxMenuBar* menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, "&File");

    // Associate the menu bar with the frame
    frame->SetMenuBar(menu_bar);

    // Make a panel
    frame->panel = new wxWindow(frame, wxID_ANY);
    frame->Show(true);

    return true;
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(RESOURCE_QUIT, MyFrame::OnQuit)
EVT_MENU(RESOURCE_TEST1, MyFrame::OnTest1)
wxEND_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame(wxWindow* parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame(parent, id, title, pos, size)
{
    SetIcon(wxICON(sample));

    panel = nullptr;
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnTest1(wxCommandEvent& WXUNUSED(event))
{
#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    MyDialog dialog;
    if ( dialog.LoadNativeDialog(this, "dialog1") )
    {
        dialog.ShowModal();
    }
#else
    wxMessageBox("No native dialog support", "Platform limitation");
#endif
}

wxBEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_BUTTON(wxID_OK, MyDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, MyDialog::OnCancel)
wxEND_EVENT_TABLE()


void MyDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_OK);
}

void MyDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_CANCEL);
}
