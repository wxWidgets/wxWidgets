/////////////////////////////////////////////////////////////////////////////
// Name:        resource.cpp
// Purpose:     Dialog resource sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/deprecated/setup.h"

#if !wxUSE_WX_RESOURCES
    #error "You should set wxUSE_WX_RESOURCES to 1 to compile this sample"
#endif

#if wxUSE_UNICODE
// #error "This sample can't be compiled in Unicode mode."
#endif // wxUSE_UNICODE

#include "wx/deprecated/resource.h"

#include <ctype.h>
#include "resource.h"

// If we wanted to demonstrate total platform independence,
// then we'd use the dynamic file loading form for all platforms.
// But this shows how to embed the wxWidgets resources
// in the program code/executable for UNIX and Windows
// platforms.

// In order to get the ID of the controls defined in the
// dialog, we include the header automatically generated
// by DialogEd

#include "dialog1.h"

// If you have a Windows compiler that can cope with long strings,
// then you can always use the #include form for simplicity.

// NOTE: Borland's brc32.exe resource compiler doesn't recognize
// the TEXT resource, for some reason, so either run-time file loading
// or file inclusion should be used.

#if defined(__WXMSW__) && !wxUSE_UNICODE
// Under Windows, some compilers can't include
// a whole .wxr file. So we use a .rc user-defined resource
// instead. dialog1 will point to the whole .wxr 'file'.
static wxChar *dialog1 = NULL;
static wxChar *menu1 = NULL;
#else
// Other platforms should have sensible compilers that
// cope with long strings.
#include "dialog1.wxr"
#include "menu1.wxr"
#endif

// Declare two frames
MyFrame   *frame = (MyFrame *) NULL;

IMPLEMENT_APP(MyApp)

// Testing of ressources
MyApp::MyApp()
{
}

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit(void)
{
#if defined(__WXMSW__) && !wxUSE_UNICODE
    // Load the .wxr 'file' from a .rc resource, under Windows.
    // note that the resource really is a char*, not a wxChar*!
    dialog1 = wxLoadUserResource(wxT("dialog1"), wxT("WXRDATA"));
    menu1 = wxLoadUserResource(wxT("menu1"), wxT("WXRDATA"));
    // All resources in the file (only one in this case) get parsed
    // by this call.
    wxResourceParseString(dialog1);
    wxResourceParseString(menu1);
#else
    // Simply parse the data pointed to by the variable dialog1.
    // If there were several resources, there would be several
    // variables, and this would need to be called several times.
    wxResourceParseData(dialog1);
    wxResourceParseData(menu1);
#endif

    // Create the main frame window
    frame = new MyFrame( (wxFrame *) NULL, wxID_ANY,
                         wxT("wxWidgets Resource Sample"),
                         wxDefaultPosition, wxSize(300, 250) );

#if wxUSE_STATUSBAR
    // Give it a status line
    frame->CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

    wxMenuBar *menu_bar = wxResourceCreateMenuBar(wxT("menu1"));

    // Associate the menu bar with the frame
    frame->SetMenuBar(menu_bar);

    // Make a panel
    frame->panel = new MyPanel( frame, wxID_ANY, wxPoint(0, 0), wxSize(400, 400),
                                0, wxT("MyMainFrame") );
    frame->Show(true);

    SetTopWindow(frame);

    return true;
}

MyApp::~MyApp()
{
#if defined(__WXMSW__) && !wxUSE_UNICODE
    delete dialog1;
    delete menu1;
#endif
}

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
    EVT_LEFT_DOWN( MyPanel::OnClick)
END_EVENT_TABLE()

MyPanel::MyPanel
(
    wxWindow *parent, wxWindowID id, const wxPoint& pos,
    const wxSize& size,
    int style, const wxString &name
) : wxPanel( parent, id, pos, size, style, name )
{
}

void MyPanel::OnClick( wxMouseEvent &WXUNUSED(event2) )
{
    MyFrame *frame = (MyFrame*)(wxTheApp->GetTopWindow());
    wxCommandEvent event;
    frame->OnTestDialog( event );
}


BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(RESOURCE_ABOUT, MyFrame::OnAbout)
    EVT_MENU(RESOURCE_QUIT, MyFrame::OnQuit)
    EVT_MENU(RESOURCE_TESTDIALOG, MyFrame::OnTestDialog)
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame
(
    wxWindow *parent, const wxWindowID id,
    const wxString& title, const wxPoint& pos, const wxSize& size
) : wxFrame(parent, id, title, pos, size)
{
    panel = (wxWindow *) NULL;
}

void MyFrame::OnAbout( wxCommandEvent& WXUNUSED(event) )
{
    wxMessageBox(wxT("wxWidgets resource sample.\n")
                 wxT("(c) Julian Smart"), wxT("About wxWidgets sample"),
                 wxICON_INFORMATION | wxOK);
}

void MyFrame::OnQuit( wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnTestDialog(wxCommandEvent& WXUNUSED(event) )
{
    MyDialog *dialog = new MyDialog;

    if (wxLoadFromResource(dialog, this, wxT("dialog1")))
    {
        wxTextCtrl *text = (wxTextCtrl *)wxFindWindowByName(wxT("multitext3"), dialog);
        if (text)
        {
            text->SetValue(wxT("wxWidgets resource demo"));
        }

        dialog->ShowModal();
    }

    dialog->Close(true);
}

BEGIN_EVENT_TABLE(MyDialog, wxDialog)
    //EVT_BUTTON(RESOURCE_OK, MyDialog::OnOk)
    EVT_BUTTON(ID_BUTTON109, MyDialog::OnCancel)
END_EVENT_TABLE()


void MyDialog::OnOk(wxCommandEvent& WXUNUSED(event) )
{
    //  EndModal(RESOURCE_OK);
}

void MyDialog::OnCancel(wxCommandEvent& WXUNUSED(event) )
{
    EndModal(ID_BUTTON109);
}


