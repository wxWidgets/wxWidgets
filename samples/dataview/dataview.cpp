/////////////////////////////////////////////////////////////////////////////
// Name:        dataview.cpp
// Purpose:     wxDataViewCtrl wxWidgets sample
// Author:      Robert Roebling
// Modified by: Francesco Montorsi
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

#include "wx/datetime.h"
#include "wx/splitter.h"
#include "wx/aboutdlg.h"
#include "wx/choicdlg.h"
#include "wx/numdlg.h"
#include "wx/dataview.h"
#include "wx/spinctrl.h"

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif

#include "null.xpm"


#define DEFAULT_ALIGN                   wxALIGN_LEFT
#define DATAVIEW_DEFAULT_STYLE          (wxDV_MULTIPLE|wxDV_HORIZ_RULES|wxDV_VERT_RULES)



// -------------------------------------
// MyMusicModel
// -------------------------------------

/*
Implement this data model
            Title               Artist               Year
-------------------------------------------------------------
1: My Music:
    2:  Pop music
        3:  You are not alone   Michael Jackson      1995
        4:  Take a bow          Madonna              1994
    5:  Classical music
        6:  Ninth Symphony      Ludwig v. Beethoven  1824
        7:  German Requiem      Johannes Brahms      1868
*/

 
class MyMusicModel: public wxDataViewModel
{
public:
    MyMusicModel() {}
    
    virtual unsigned int GetColumnCount() const
    {
        return 3;
    }

    virtual wxString GetColumnType( unsigned int col ) const
    {
        return "string";
    }

    virtual void GetValue( wxVariant &variant, 
                           const wxDataViewItem &item, unsigned int col ) const
    {
        variant = wxString("");
        int ID = item.GetID();
        switch (ID)
        {
            case 1: if (col == 0) variant = wxString("My Music"); break;
            case 2: if (col == 0) variant = wxString("Pop music"); break;
            case 5: if (col == 0) variant = wxString("Classical music"); break;
            case 3:
            {
                switch (col)
                {
                    case 0: variant = wxString("You are not alone"); break;
                    case 1: variant = wxString("Michael Jackson"); break;
                    case 2: variant = wxString("1995");
                }
            }
            break;
            case 4:
            {
                switch (col)
                {
                    case 0: variant = wxString("Take a bow"); break;
                    case 1: variant = wxString("Madonna"); break;
                    case 2: variant = wxString("1994");
                }
            }
            break;
            case 6:
            {
                switch (col)
                {
                    case 0: variant = wxString("Ninth symphony"); break;
                    case 1: variant = wxString("Ludwig v. Beethoven"); break;
                    case 2: variant = wxString("1824");
                }
            }
            break;
            case 7:
            {
                switch (col)
                {
                    case 0: variant = wxString("German requiem"); break;
                    case 1: variant = wxString("Johannes Brahms"); break;
                    case 2: variant = wxString("1868");
                }
            }
            break;
        }
            
    }

    virtual bool SetValue( const wxVariant &variant, 
                           const wxDataViewItem &item, unsigned int col )
    {
        // readonly
        return true;
    }

    /*****************************************************************
    If wxDataViewItem is not valid in the two methods I quote above
    then it means "return the child item from the invisible root".
    ******************************************************************/
    
    virtual bool HasChildren( const wxDataViewItem &item ) const
    {
        int ID = item.GetID();
        return ((ID == 1) || (ID == 2) || (ID == 5) || (ID == 0));
    }
    
    virtual wxDataViewItem GetFirstChild( const wxDataViewItem &parent ) const
    {
        int ID = parent.GetID();
        switch (ID)
        {
            case 0: return wxDataViewItem( 1 );
            case 1: return wxDataViewItem( 2 );
            case 2: return wxDataViewItem( 3 );
            case 5: return wxDataViewItem( 6 );
        }
        
        return wxDataViewItem(0);
    }
    virtual wxDataViewItem GetNextSibling( const wxDataViewItem &item ) const
    {
        int ID = item.GetID();
        switch (ID)
        {
            case 2: return wxDataViewItem( 5 );
            case 3: return wxDataViewItem( 4 );
            case 6: return wxDataViewItem( 7 );
        }
        
        return wxDataViewItem(0);
    } 
};

// -------------------------------------
// MyApp
// -------------------------------------

class MyApp: public wxApp
{
public:
    bool OnInit(void);
    int OnExit();
};

// -------------------------------------
// MyFrame
// -------------------------------------

class MyFrame : public wxFrame
{
public:
    MyFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h);

public:
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    wxDataViewCtrl* m_dataview;

private:
    DECLARE_EVENT_TABLE()
};

// -------------------------------------
// MyApp
// -------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(void)
{
    if ( !wxApp::OnInit() )
        return false;

    // build the first frame
    MyFrame *frame = 
        new MyFrame(NULL, wxT("wxDataViewCtrl feature test"), 10, 10, 800, 340);
    frame->Show(true);

    SetTopWindow(frame);
    return true;
}

int MyApp::OnExit()
{
    return 0;
}


// -------------------------------------
// MyFrame
// -------------------------------------

enum
{
    // file menu
    ID_ABOUT = wxID_ABOUT,
    ID_EXIT = wxID_EXIT,
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU( ID_ABOUT, MyFrame::OnAbout )
    EVT_MENU( ID_EXIT, MyFrame::OnQuit )
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h):
  wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    SetIcon(wxICON(sample));

    // build the menus:

    wxMenu *file_menu = new wxMenu;
    file_menu->Append(ID_ABOUT, "&About");
    file_menu->AppendSeparator();
    file_menu->Append(ID_EXIT, "E&xit");

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, "&File");

    SetMenuBar(menu_bar);
    CreateStatusBar();

    m_dataview = new wxDataViewCtrl( this, wxID_ANY, wxDefaultPosition,
                                     wxDefaultSize );

    wxObjectDataPtr<MyMusicModel> model(new MyMusicModel);
    m_dataview->AssociateModel( model.get() );

    m_dataview->AppendTextColumn( "Title", 0, wxDATAVIEW_CELL_INERT, 200, 
                                     DEFAULT_ALIGN, wxDATAVIEW_COL_SORTABLE );
    m_dataview->AppendTextColumn( "Artist", 1, wxDATAVIEW_CELL_INERT, 200,
                                     DEFAULT_ALIGN, wxDATAVIEW_COL_SORTABLE );
    m_dataview->AppendTextColumn( "Year", 2, wxDATAVIEW_CELL_INERT, 50,
                                     DEFAULT_ALIGN );
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxAboutDialogInfo info;
    info.SetName(_("DataView sample"));
    info.SetDescription(_("This sample demonstrates the dataview control handling"));
    info.SetCopyright(_T("(C) 2007 Robert Roebling"));

    wxAboutBox(info);
}

