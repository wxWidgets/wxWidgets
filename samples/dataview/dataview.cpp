/////////////////////////////////////////////////////////////////////////////
// Name:        dataview.cpp
// Purpose:     DataVewCtrl  wxWidgets sample
// Author:      Robert Roebling
// Modified by:
// Created:     06/01/06
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
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

#ifndef __WXMSW__
#include "mondrian.xpm"
#endif

#include "wx/dataview.h"

// -------------------------------------
// MyTextModel
// -------------------------------------

class MyTextModel: public wxDataViewListModel
{
public:
    MyTextModel() {}
    
    virtual size_t GetNumberOfRows() 
        { return 1000; }
    virtual size_t GetNumberOfCols()
        { return 3; }
    // as reported by wxVariant
    virtual wxString GetColType( size_t col )
        { return wxT("string"); }
    virtual wxVariant GetValue( size_t col, size_t row )
        {   wxString tmp; 
            tmp.Printf( wxT("item(%d;%d)"), (int)row, (int)col ); 
            return tmp;
        }
}

// -------------------------------------
// MyApp
// -------------------------------------

class MyApp: public wxApp
{
public:
    bool OnInit(void);
};

// -------------------------------------
// MyFrame
// -------------------------------------

class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h);

public:
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    
private:
    wxDataViewCtrl* dataview;
};

// -------------------------------------
// MyApp
// -------------------------------------

#define DYNAMIC_QUIT   wxID_EXIT
#define DYNAMIC_ABOUT  wxID_ABOUT

IMPLEMENT_APP  (MyApp)

bool MyApp::OnInit(void)
{
    MyFrame *frame = new MyFrame(NULL, _T("Dynamic wxWidgets App"), 50, 50, 450, 340);

    frame->Show(true);

    SetTopWindow(frame);

    return true;
}

// -------------------------------------
// MyFrame
// -------------------------------------

MyFrame::MyFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h):
  wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
#ifdef __WXMSW__
    SetIcon(wxIcon(_T("mondrian")));
#else
    SetIcon(wxIcon(mondrian_xpm));
#endif

    wxMenu *file_menu = new wxMenu;

    file_menu->Append(DYNAMIC_ABOUT, _T("&About"));
    file_menu->Append(DYNAMIC_QUIT, _T("E&xit"));
    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _T("&File"));
    SetMenuBar(menu_bar);

    // You used to have to do some casting for param 4, but now there are type-safe handlers
    Connect( DYNAMIC_QUIT,  wxID_ANY,
                    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnQuit) );
    Connect( DYNAMIC_ABOUT, wxID_ANY,
                    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnAbout) );

    CreateStatusBar();
    
    
    dataview = new wxDataViewCtrl( this, -1 );
    dataview->AppendStringColumn( wxT("first") );
    dataview->AppendStringColumn( wxT("second") );
    dataview->AppendStringColumn( wxT("third") );
    
    MyTextModel *model = new MyTextModel;
    dataview->AssociateModel( Model );
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxMessageDialog dialog(this, _T("This demonstrates the dataview control handling"),
        _T("About DataView"), wxOK);

    dialog.ShowModal();
}


