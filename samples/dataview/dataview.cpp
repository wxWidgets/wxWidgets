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
    MyTextModel() 
    {
        size_t i;
        for (i = 0; i < 1000; i++)
            m_list.Add( wxT("Test") );
    }
    
    virtual size_t GetNumberOfRows() 
        { return 1000; }
    virtual size_t GetNumberOfCols()
        { return 3; }
    // as reported by wxVariant
    virtual wxString GetColType( size_t col )
        { return wxT("string"); }
        
    virtual wxVariant GetValue( size_t col, size_t row )
        {   
            if (col == 2)
            {
                return m_list[row];
            }
            else
            {
                wxString tmp; 
                tmp.Printf( wxT("item(%d;%d)"), (int)row, (int)col ); 
                return tmp;
            }
        }
    virtual bool SetValue( wxVariant &value, size_t col, size_t row )
        {
            if (col == 2)
            {
                m_list[row] = value.GetString();
            }
            return true;
        }
    
    wxArrayString m_list;
};

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
    wxDataViewCtrl* dataview_left;
    wxDataViewCtrl* dataview_right;
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
    
    
    // Left wxDataViewCtrl
    dataview_left = new wxDataViewCtrl( this, -1 );
    
    MyTextModel *model = new MyTextModel;
    dataview_left->AssociateModel( model );
    
    dataview_left->AppendStringColumn( wxT("first"), 0 );
    dataview_left->AppendStringColumn( wxT("second"), 1 );
    wxDataViewTextCell *cell = new wxDataViewTextCell( wxT("string"), wxDATAVIEW_CELL_EDITABLE );
    wxDataViewColumn *column = new wxDataViewColumn( wxT("editable"), cell, 2 );
    dataview_left->AppendColumn( column );
    
    // Right wxDataViewCtrl using the same model
    dataview_right = new wxDataViewCtrl( this, -1 );
    dataview_right->AssociateModel( model );
    
    cell = new wxDataViewTextCell( wxT("string"), wxDATAVIEW_CELL_EDITABLE );
    column = new wxDataViewColumn( wxT("editable"), cell, 2 );
    dataview_right->AppendColumn( column );
    dataview_right->AppendStringColumn( wxT("first"), 0 );
    dataview_right->AppendStringColumn( wxT("second"), 1 );
    
    wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
    sizer->Add( dataview_left, 1, wxGROW );
    sizer->Add(10,10);
    sizer->Add( dataview_right, 1, wxGROW );
    SetSizer( sizer );
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


