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

#include "wx/datetime.h"

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif

#include "null.xpm"

#include "wx/dataview.h"

// -------------------------------------
// MyTextModel
// -------------------------------------

WX_DECLARE_LIST(wxDateTime,wxArrayDate);
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(wxArrayDate)

class MyTextModel: public wxDataViewListModel
{
public:
    MyTextModel()
    {
        unsigned int i;
        for (i = 0; i < 1000; i++)
            m_list.Add( wxT("Test") );
        for (i = 0; i < 500; i++)
            { m_bools.Add( 0 ); m_bools.Add( 1 ); }
        for (i = 0; i < 500; i++)
            { m_colours.Add( wxT("red") ); m_colours.Add( wxT("green") ); }
        for (i = 0; i < 1000; i++)
            { m_progress.Add( i/10 ); }
        for (i = 0; i < 1000; i++)
            {
                wxDateTime *date = new wxDateTime( wxDateTime::Now() );
                m_dates.Append( date );
            }
    }

    virtual unsigned int GetNumberOfRows()
        { return 1000; }
    virtual unsigned int GetNumberOfCols()
        { return 7; }

    // as reported by wxVariant
    virtual wxString GetColType( unsigned int col )
        {
            if (col == 6)
                return wxT("datetime");

            if (col == 5)
                return wxT("long");

            if (col == 3)
                return wxT("bool");

            return wxT("string");
        }

    virtual void GetValue( wxVariant &variant, unsigned int col, unsigned int row )
        {
            if (col == 6)
            {
                variant = (wxDateTime) *m_dates[row];
            } else
            if (col == 5)
            {
                variant = (long) m_progress[row];
            } else
            if (col == 4)
            {
                variant = m_colours[row];
            } else
            if (col == 3)
            {
                variant = (bool) m_bools[row];
            } else
            if (col == 2)
            {
                variant = m_list[row];
            }
            else
            {
                wxString tmp;
                tmp.Printf( wxT("item(%d;%d)"), (int)row, (int)col );
                variant = tmp;
            }
        }
    virtual bool SetValue( wxVariant &value, unsigned int col, unsigned int row )
        {
            if (col == 6)
            {
                *m_dates[row] = value.GetDateTime();
            } else
            if (col == 3)
            {
                m_bools[row] = (int) value.GetBool();
            } else
            if (col == 2)
            {
                m_list[row] = value.GetString();
            }
            return true;
        }

    wxArrayString m_list;
    wxArrayInt    m_bools;
    wxArrayString m_colours;
    wxArrayInt    m_progress;
    wxArrayDate   m_dates;
};

// -------------------------------------
// MyCustomCell
// -------------------------------------

class MyCustomCell: public wxDataViewCustomCell
{
public:
    MyCustomCell() :
        wxDataViewCustomCell( wxT("string"), wxDATAVIEW_CELL_ACTIVATABLE )
    {
        m_colour = wxT("black");
    }
    bool SetValue( const wxVariant &value )
    {
        m_colour = value.GetString();
        return true;
    }
    bool Render( wxRect rect, wxDC *dc, int WXUNUSED(state) )
    {
        dc->SetPen( *wxBLACK_PEN );
        if (m_colour == wxT("red"))
            dc->SetBrush( *wxRED_BRUSH );
        else if (m_colour == wxT("green"))
            dc->SetBrush( *wxGREEN_BRUSH );
        else
            dc->SetBrush( *wxBLACK_BRUSH );
        dc->DrawRectangle( rect );
        return true;
    }
    wxSize GetSize()
    {
        return wxSize(20,8);
    }
    bool Activate( wxRect WXUNUSED(rect),
                   wxDataViewListModel *WXUNUSED(model),
                   unsigned int WXUNUSED(col),
                   unsigned int WXUNUSED(row) )
    {
        return false;
    }

private:
    wxString m_colour;
};

// -------------------------------------
// MyUnsortedTextModel
// -------------------------------------

class MyUnsortedTextModel: public wxDataViewListModel
{
public:
    MyUnsortedTextModel()
    {
        m_list.Add( wxT("This") );
        m_list.Add( wxT("is") );
        m_list.Add( wxT("an") );
        m_list.Add( wxT("unsorted") );
        m_list.Add( wxT("list") );
        m_list.Add( wxT("of") );
        m_list.Add( wxT("words.") );
        
        m_bitmap = wxBitmap( null_xpm );
    }

    virtual unsigned int GetNumberOfRows() { return m_list.GetCount(); }
    virtual unsigned int GetNumberOfCols() { return 2; }
    virtual wxString GetColType( unsigned int WXUNUSED(col) ) { return wxT("string"); }
    virtual void GetValue( wxVariant &variant, unsigned int col, unsigned int row )
    {
        if (col == 0)
        {
            variant = m_list[row];
            return;
        }
        if ((col == 2) || (col == 3))
        {
            variant << m_bitmap;
            return;
        }
        wxString tmp;
        tmp.Printf( wxT("item(%d;%d)"), (int)row, (int)col );
        variant = tmp;
    }
    virtual bool SetValue( wxVariant &variant, unsigned int col, unsigned int row )
    {
        if (col == 0)
        {
            m_list[row] = variant.GetString();
            return true;
        }
        return false;

    }

    void AppendRow( const wxString &text )
    {
        m_list.Add( text );
        RowAppended();
    }

    void PrependRow( const wxString &text )
    {
        m_list.Insert( text, 0 );
        RowPrepended();
    }

    void InsertRowAt1( const wxString &text )
    {
        m_list.Insert( text, 1 );
        RowInserted( 1 );
    }

    void DeleteRow( unsigned int index )
    {
        m_list.RemoveAt( index );
        RowDeleted( index );
    }

    wxArrayString m_list;
    wxBitmap m_bitmap;
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
// MySortingFrame
// -------------------------------------

enum my_events
{
    ID_APPEND_ROW_LEFT = 1000,
    ID_PREPEND_ROW_LEFT,
    ID_INSERT_ROW_LEFT,
    ID_DELETE_ROW_LEFT,
    ID_EDIT_ROW_LEFT,

    ID_APPEND_ROW_RIGHT,
    ID_PREPEND_ROW_RIGHT,
    ID_INSERT_ROW_RIGHT,
    ID_DELETE_ROW_RIGHT,
    ID_EDIT_ROW_RIGHT,
    
    ID_SORTED,
    ID_UNSORTED
};

class MySortingFrame: public wxFrame
{
public:
    MySortingFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h);
    ~MySortingFrame();

public:
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnAppendRowLeft(wxCommandEvent& event);
    void OnPrependRowLeft(wxCommandEvent& event);
    void OnInsertRowLeft(wxCommandEvent& event);
    void OnDeleteRowLeft(wxCommandEvent& event);
    void OnEditRowLeft(wxCommandEvent& event);

    void OnAppendRowRight(wxCommandEvent& event);
    void OnPrependRowRight(wxCommandEvent& event);
    void OnInsertRowRight(wxCommandEvent& event);
    void OnDeleteRowRight(wxCommandEvent& event);
    void OnEditRowRight(wxCommandEvent& event);

    void OnSelectedUnsorted(wxDataViewEvent &event);
    void OnSelectedSorted(wxDataViewEvent &event);

private:
    wxDataViewCtrl* dataview_left;
    wxDataViewCtrl* dataview_right;
    
    wxLog          *m_logOld;
    wxTextCtrl     *m_logWindow;

    MyUnsortedTextModel *m_unsorted_model;

    DECLARE_EVENT_TABLE()
};

// -------------------------------------
// MyApp
// -------------------------------------

#define DYNAMIC_QUIT   wxID_EXIT
#define DYNAMIC_ABOUT  wxID_ABOUT

IMPLEMENT_APP  (MyApp)

bool MyApp::OnInit(void)
{
    MyFrame *frame = new MyFrame(NULL, wxT("wxDataViewCtrl feature test"), 10, 10, 800, 340);
    frame->Show(true);

    MySortingFrame *frame2 = new MySortingFrame(NULL, wxT("wxDataViewCtrl sorting test"), 10, 150, 600, 500);
    frame2->Show(true);

    SetTopWindow(frame);
    
    return true;
}

// -------------------------------------
// MyFrame
// -------------------------------------

MyFrame::MyFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h):
  wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    SetIcon(wxICON(sample));

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

    wxPanel *panel = new wxPanel( this, wxID_ANY );


    // Left wxDataViewCtrl
    dataview_left = new wxDataViewCtrl( panel, wxID_ANY );

    MyTextModel *model = new MyTextModel;
    dataview_left->AssociateModel( model );

    dataview_left->AppendTextColumn( wxT("first"), 0 );
    dataview_left->AppendTextColumn( wxT("second"), 1 );

    wxDataViewTextCell *text_cell = new wxDataViewTextCell( wxT("string"), wxDATAVIEW_CELL_EDITABLE );
    wxDataViewColumn *column = new wxDataViewColumn( wxT("editable"), text_cell, 2 );
    dataview_left->AppendColumn( column );

    dataview_left->AppendToggleColumn( wxT("fourth"), 3 );

    MyCustomCell *custom_cell = new MyCustomCell;
    column = new wxDataViewColumn( wxT("custom"), custom_cell, 4 );
    dataview_left->AppendColumn( column );

    dataview_left->AppendProgressColumn( wxT("progress"), 5 );

    dataview_left->AppendDateColumn( wxT("date"), 6 );

    // Right wxDataViewCtrl using the same model
    dataview_right = new wxDataViewCtrl( panel, wxID_ANY );
    dataview_right->AssociateModel( model );

    text_cell = new wxDataViewTextCell( wxT("string"), wxDATAVIEW_CELL_EDITABLE );
    column = new wxDataViewColumn( wxT("editable"), text_cell, 2 );
    dataview_right->AppendColumn( column );
    dataview_right->AppendTextColumn( wxT("first"), 0 );
    dataview_right->AppendTextColumn( wxT("second"), 1 );
    wxDataViewToggleCell *toggle_cell = new wxDataViewToggleCell( wxT("bool"), wxDATAVIEW_CELL_ACTIVATABLE );
    column = new wxDataViewColumn( wxT("bool"), toggle_cell, 3, 30 );
    dataview_right->AppendColumn( column );

    dataview_right->AppendDateColumn( wxT("date"), 6 );

    // layout dataview controls.

    wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
    sizer->Add( dataview_left, 3, wxGROW );
    sizer->Add(10,10);
    sizer->Add( dataview_right, 2, wxGROW );
    panel->SetSizer( sizer );
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

// -------------------------------------
// MySortingFrame
// -------------------------------------

BEGIN_EVENT_TABLE(MySortingFrame,wxFrame)
    EVT_BUTTON( ID_APPEND_ROW_LEFT, MySortingFrame::OnAppendRowLeft )
    EVT_BUTTON( ID_PREPEND_ROW_LEFT, MySortingFrame::OnPrependRowLeft )
    EVT_BUTTON( ID_INSERT_ROW_LEFT, MySortingFrame::OnInsertRowLeft )
    EVT_BUTTON( ID_DELETE_ROW_LEFT, MySortingFrame::OnDeleteRowLeft )
    EVT_DATAVIEW_ROW_SELECTED( ID_SORTED, MySortingFrame::OnSelectedSorted )
    EVT_DATAVIEW_ROW_SELECTED( ID_UNSORTED, MySortingFrame::OnSelectedUnsorted )
END_EVENT_TABLE()

MySortingFrame::MySortingFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h):
  wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    m_logOld = NULL;
    
    SetIcon(wxICON(sample));

    wxMenu *file_menu = new wxMenu;

    file_menu->Append(DYNAMIC_ABOUT, _T("&About"));
    file_menu->Append(DYNAMIC_QUIT, _T("E&xit"));
    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _T("&File"));
    SetMenuBar(menu_bar);

    // You used to have to do some casting for param 4, but now there are type-safe handlers
    Connect( DYNAMIC_QUIT,  wxID_ANY,
                    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MySortingFrame::OnQuit) );
    Connect( DYNAMIC_ABOUT, wxID_ANY,
                    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MySortingFrame::OnAbout) );

    CreateStatusBar();


    // Left wxDataViewCtrl
    dataview_left = new wxDataViewCtrl( this, ID_UNSORTED, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE );

    m_unsorted_model = new MyUnsortedTextModel;
    dataview_left->AssociateModel( m_unsorted_model );
    wxDataViewTextCell *text_cell = new wxDataViewTextCell( wxT("string"), wxDATAVIEW_CELL_EDITABLE );
    wxDataViewColumn *column = new wxDataViewColumn( wxT("editable"), text_cell, 0 );
    dataview_left->AppendColumn( column );
    dataview_left->AppendTextColumn( wxT("second"), 1 );
    dataview_left->AppendColumn( new wxDataViewColumn( wxT("icon"), new wxDataViewBitmapCell, 2, 25 ) );
    dataview_left->AppendColumn( new wxDataViewColumn( wxT("icon"), new wxDataViewBitmapCell, 3, 25 ) );

    // Right wxDataViewCtrl using the sorting model
    dataview_right = new wxDataViewCtrl( this, ID_SORTED );
    
    wxDataViewSortedListModel *sorted_model =
        new wxDataViewSortedListModel( m_unsorted_model );
    dataview_right->AssociateModel( sorted_model );
    text_cell = new wxDataViewTextCell( wxT("string"), wxDATAVIEW_CELL_EDITABLE );
    column = new wxDataViewColumn( wxT("editable"), text_cell, 0, -1, wxDATAVIEW_COL_SORTABLE|wxDATAVIEW_COL_RESIZABLE );
    dataview_right->AppendColumn( column );
    
    dataview_right->AppendTextColumn( wxT("second"), 1 );

    // layout dataview controls.

    wxBoxSizer *top_sizer = new wxBoxSizer( wxHORIZONTAL );
    top_sizer->Add( dataview_left, 1, wxGROW );
    top_sizer->Add(10,10);
    top_sizer->Add( dataview_right, 1, wxGROW );

    wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );
    button_sizer->Add( 10, 10, 1 );
    wxFlexGridSizer *left_sizer = new wxFlexGridSizer( 2 );
    left_sizer->Add( new wxButton( this, ID_APPEND_ROW_LEFT, wxT("Append") ), 0, wxALL, 5 );
    left_sizer->Add( new wxButton( this, ID_PREPEND_ROW_LEFT, wxT("Prepend") ), 0, wxALL, 5 );
    left_sizer->Add( new wxButton( this, ID_INSERT_ROW_LEFT, wxT("Insert") ), 0, wxALL, 5 );
    left_sizer->Add( new wxButton( this, ID_DELETE_ROW_LEFT, wxT("Delete second") ), 0, wxALL, 5 );
    left_sizer->Add( new wxButton( this, ID_EDIT_ROW_LEFT, wxT("Edit") ), 0, wxALL, 5 );
    button_sizer->Add( left_sizer );
    button_sizer->Add( 10, 10, 2 );
    wxFlexGridSizer *right_sizer = new wxFlexGridSizer( 2 );
    right_sizer->Add( new wxButton( this, ID_APPEND_ROW_RIGHT, wxT("Append") ), 0, wxALL, 5 );
    right_sizer->Add( new wxButton( this, ID_PREPEND_ROW_RIGHT, wxT("Prepend") ), 0, wxALL, 5 );
    right_sizer->Add( new wxButton( this, ID_INSERT_ROW_RIGHT, wxT("Insert") ), 0, wxALL, 5 );
    right_sizer->Add( new wxButton( this, ID_DELETE_ROW_RIGHT, wxT("Delete second") ), 0, wxALL, 5 );
    right_sizer->Add( new wxButton( this, ID_EDIT_ROW_RIGHT, wxT("Edit") ), 0, wxALL, 5 );
    button_sizer->Add( right_sizer );
    button_sizer->Add( 10, 10, 1 );

    wxBoxSizer *main_sizer = new wxBoxSizer( wxVERTICAL );
    main_sizer->Add( top_sizer, 1, wxGROW );
    main_sizer->Add( button_sizer, 0, wxGROW );
    
    m_logWindow = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_MULTILINE | wxSUNKEN_BORDER);
    main_sizer->Add( 20,20 );
    main_sizer->Add( m_logWindow, 1, wxGROW );

    m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_logWindow));

    SetSizer( main_sizer );
}

MySortingFrame::~MySortingFrame()
{
    delete wxLog::SetActiveTarget(m_logOld);
}

void MySortingFrame::OnSelectedUnsorted(wxDataViewEvent &event)
{
    wxLogMessage( wxT("OnSelected from unsorted list, selected %d"), (int) event.GetRow() );
}

void MySortingFrame::OnSelectedSorted(wxDataViewEvent &event)
{
    wxLogMessage( wxT("OnSelected from sorted list, selected %d"), (int) event.GetRow() );
}

void MySortingFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MySortingFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxMessageDialog dialog(this, _T("This demonstrates the dataview control sorting"),
        _T("About DataView"), wxOK);

    dialog.ShowModal();
}

void MySortingFrame::OnAppendRowLeft(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog dialog( this, wxT("Enter text to append") );
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString value = dialog.GetValue();
        if (!value.empty())
            m_unsorted_model->AppendRow( value );
    }
}

void MySortingFrame::OnPrependRowLeft(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog dialog( this, wxT("Enter text to prepend") );
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString value = dialog.GetValue();
        if (!value.empty())
            m_unsorted_model->PrependRow( value );
    }
}

void MySortingFrame::OnInsertRowLeft(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog dialog( this, wxT("Enter text to insert before second") );
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString value = dialog.GetValue();
        if (!value.empty())
            m_unsorted_model->InsertRowAt1( value );
    }
}

void MySortingFrame::OnDeleteRowLeft(wxCommandEvent& WXUNUSED(event))
{
    m_unsorted_model->DeleteRow( 1 );
}

void MySortingFrame::OnEditRowLeft(wxCommandEvent& WXUNUSED(event))
{
}

void MySortingFrame::OnAppendRowRight(wxCommandEvent& WXUNUSED(event))
{
}

void MySortingFrame::OnPrependRowRight(wxCommandEvent& WXUNUSED(event))
{
}

void MySortingFrame::OnInsertRowRight(wxCommandEvent& WXUNUSED(event))
{
}

void MySortingFrame::OnDeleteRowRight(wxCommandEvent& WXUNUSED(event))
{
}

void MySortingFrame::OnEditRowRight(wxCommandEvent& WXUNUSED(event))
{
}

