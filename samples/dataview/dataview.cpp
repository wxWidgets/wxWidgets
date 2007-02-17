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
#include "wx/splitter.h"
#include "wx/aboutdlg.h"

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif

#include "null.xpm"

#include "wx/dataview.h"

// -------------------------------------
// MyTextModel
// -------------------------------------

WX_DECLARE_OBJARRAY(wxDateTime,wxArrayDate);
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayDate)

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
                wxDateTime date( wxDateTime::Now() );
                m_dates.Add( date );
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
                variant = (wxDateTime) m_dates[row];
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
                variant = (bool) (m_bools[row] != 0);
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
                m_dates[row] = value.GetDateTime();
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
// MyCustomRenderer
// -------------------------------------

class MyCustomRenderer: public wxDataViewCustomRenderer
{
public:
    MyCustomRenderer() :
        wxDataViewCustomRenderer( wxT("string"), wxDATAVIEW_CELL_ACTIVATABLE )
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
    void OnNewSortingFrame(wxCommandEvent& event);

    void OnSingleSelMode(wxCommandEvent& event);
    void OnMultipleSelMode(wxCommandEvent& event);
    void OnResizeableColumn(wxCommandEvent& event);
    void OnSortableColumn(wxCommandEvent& event);
    void OnHideColumn(wxCommandEvent& event);
    void OnChooseAlign(wxCommandEvent& event);

private:
    wxDataViewCtrl* dataview_left;
    wxDataViewCtrl* dataview_right;
    wxSplitterWindow *m_splitter;

    wxAlignment m_align;

    void CreateControls();

private:
    DECLARE_EVENT_TABLE()
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
    
    ID_SELECT,
    ID_UNSELECT_ALL,

    ID_APPEND_ROW_RIGHT,
    ID_PREPEND_ROW_RIGHT,
    ID_INSERT_ROW_RIGHT,
    ID_DELETE_ROW_RIGHT,
    ID_EDIT_ROW_RIGHT,
    
    ID_SORTED,
    ID_UNSORTED,
    ID_ACTIVATED
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

    void OnSelect(wxCommandEvent& event);
    void OnUnselectAll(wxCommandEvent& event);
    
    void OnSelectedUnsorted(wxDataViewEvent &event);
    void OnSelectedSorted(wxDataViewEvent &event);
    void OnActivatedUnsorted(wxDataViewEvent &event);

    void OnHeaderClickSorted(wxDataViewEvent &event);
    void OnHeaderClickUnsorted(wxDataViewEvent &event);

private:
    wxDataViewCtrl* dataview_left;
    wxDataViewCtrl* dataview_right;
    
    wxLog          *m_logOld;
    wxTextCtrl     *m_logWindow;

    MyUnsortedTextModel        *m_unsorted_model;
    wxDataViewSortedListModel  *m_sorted_model;

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
    ID_NEW_SORT_FRAME,
    ID_EXIT = wxID_EXIT,

    // dataviewctrl menu
    ID_SINGLE_SEL_MODE,
    ID_MULTIPLE_SEL_MODE,

    ID_RESIZEABLE_COLUMNS,
    ID_SORTABLE_COLUMNS,
    ID_HIDDEN_COLUMNS,

    ID_CHOOSE_ALIGNMENT
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)

    // file menu
    EVT_MENU( ID_ABOUT, MyFrame::OnAbout )
    EVT_MENU( ID_NEW_SORT_FRAME, MyFrame::OnNewSortingFrame )
    EVT_MENU( ID_EXIT, MyFrame::OnQuit )

    // dataviewctrl menu
    EVT_MENU( ID_SINGLE_SEL_MODE, MyFrame::OnSingleSelMode )
    EVT_MENU( ID_MULTIPLE_SEL_MODE, MyFrame::OnMultipleSelMode )

    EVT_MENU( ID_RESIZEABLE_COLUMNS, MyFrame::OnResizeableColumn )
    EVT_MENU( ID_SORTABLE_COLUMNS, MyFrame::OnSortableColumn )
    EVT_MENU( ID_HIDDEN_COLUMNS, MyFrame::OnHideColumn )

    EVT_MENU( ID_CHOOSE_ALIGNMENT, MyFrame::OnChooseAlign )

END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h):
  wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    SetIcon(wxICON(sample));

    // build the menus:

    wxMenu *file_menu = new wxMenu;
    file_menu->Append(ID_NEW_SORT_FRAME, _T("&New sorting frame"));
    file_menu->AppendSeparator();
    file_menu->Append(ID_ABOUT, _T("&About"));
    file_menu->AppendSeparator();
    file_menu->Append(ID_EXIT, _T("E&xit"));

    wxMenu *data_menu = new wxMenu;
    data_menu->AppendRadioItem(ID_SINGLE_SEL_MODE, _T("&Single selection mode"));
    data_menu->AppendRadioItem(ID_MULTIPLE_SEL_MODE, _T("&Multiple selection mode"));
    data_menu->AppendSeparator();
    data_menu->AppendCheckItem(ID_RESIZEABLE_COLUMNS, _T("Make columns resizeable"));
    data_menu->AppendCheckItem(ID_SORTABLE_COLUMNS, _T("Make columns sortable"));
    data_menu->AppendCheckItem(ID_HIDDEN_COLUMNS, _T("Make columns hidden"));
    data_menu->AppendSeparator();
    data_menu->Append(ID_CHOOSE_ALIGNMENT, _T("Set alignment..."));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _T("&File"));
    menu_bar->Append(data_menu, _T("&DataViewCtrl"));

    SetMenuBar(menu_bar);
    CreateStatusBar();


    // build the other controls:

    m_splitter = new wxSplitterWindow( this, wxID_ANY );
    m_splitter->SetSashGravity(0.5);

    m_align = wxALIGN_CENTER;
    dataview_left = dataview_right = NULL;
    CreateControls();

    m_splitter->SplitVertically(dataview_left, dataview_right);
}

void MyFrame::CreateControls()
{
    wxDataViewCtrl *old1 = NULL, *old2 = NULL;

    if (dataview_left)
        old1 = dataview_left;
    if (dataview_right)
        old2 = dataview_right;

    // styles:

    long style = 0;
    if (GetMenuBar()->FindItem(ID_MULTIPLE_SEL_MODE)->IsChecked())
        style |= wxDV_MULTIPLE;

    int flags = 0;
    if (GetMenuBar()->FindItem(ID_RESIZEABLE_COLUMNS)->IsChecked())
        flags |= wxDATAVIEW_COL_RESIZABLE;
    if (GetMenuBar()->FindItem(ID_SORTABLE_COLUMNS)->IsChecked())
        flags |= wxDATAVIEW_COL_SORTABLE;
    if (GetMenuBar()->FindItem(ID_HIDDEN_COLUMNS)->IsChecked())
        flags |= wxDATAVIEW_COL_HIDDEN;


    // Left wxDataViewCtrl
    dataview_left = new wxDataViewCtrl( m_splitter, wxID_ANY, wxDefaultPosition,
                                        wxDefaultSize, style );

    MyTextModel *model = new MyTextModel;
    dataview_left->AssociateModel( model );
    model->DecRef(); // don't leak memory

    dataview_left->AppendTextColumn( wxT("first"), 0, wxDATAVIEW_CELL_INERT, -1, 
                                     m_align, flags );
    dataview_left->AppendTextColumn( wxT("second"), 1, wxDATAVIEW_CELL_INERT, -1, 
                                     m_align, flags );

    wxDataViewTextRenderer *text_renderer = 
        new wxDataViewTextRenderer( wxT("string"), wxDATAVIEW_CELL_EDITABLE );
    wxDataViewColumn *column = new wxDataViewColumn( wxT("editable"), text_renderer, 2,
                                                     -1, m_align, flags );
    dataview_left->AppendColumn( column );

    dataview_left->AppendToggleColumn( wxT("fourth"), 3, wxDATAVIEW_CELL_INERT, -1,  
                                       m_align, flags );

    MyCustomRenderer *custom_renderer = new MyCustomRenderer;
    column = new wxDataViewColumn( wxT("custom"), custom_renderer, 4, -1,  
                                   m_align, flags );
    dataview_left->AppendColumn( column );

    dataview_left->AppendProgressColumn( wxT("progress"), 5, wxDATAVIEW_CELL_INERT, -1,  
                                         m_align, flags );

    dataview_left->AppendDateColumn( wxT("date"), 6, wxDATAVIEW_CELL_INERT, -1,  
                                     m_align, flags );


    // Right wxDataViewCtrl using the same model
    dataview_right = new wxDataViewCtrl( m_splitter, wxID_ANY, wxDefaultPosition,
                                         wxDefaultSize, style );
    dataview_right->AssociateModel( model );

    text_renderer = new wxDataViewTextRenderer( wxT("string"), wxDATAVIEW_CELL_EDITABLE );
    column = new wxDataViewColumn( wxT("editable"), text_renderer, 2 );
    dataview_right->AppendColumn( column );
    dataview_right->AppendTextColumn( wxT("first"), 0 );
    dataview_right->AppendTextColumn( wxT("second"), 1 );
    wxDataViewToggleRenderer *toggle_renderer = 
        new wxDataViewToggleRenderer( wxT("bool"), wxDATAVIEW_CELL_ACTIVATABLE );
    column = new wxDataViewColumn( wxT("bool"), toggle_renderer, 3, 30 );
    dataview_right->AppendColumn( column );

    dataview_right->AppendDateColumn( wxT("date"), 6 );

    // layout dataview controls
    if (old1)
    {
        m_splitter->ReplaceWindow(old1, dataview_left);
        delete old1;
    }

    if (old2)
    {
        m_splitter->ReplaceWindow(old2, dataview_right);
        delete old2;
    }
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

void MyFrame::OnNewSortingFrame(wxCommandEvent& WXUNUSED(event) )
{
    MySortingFrame *frame2 = 
        new MySortingFrame(NULL, wxT("wxDataViewCtrl sorting test"), 10, 150, 600, 500);
    frame2->Show(true);
}

void MyFrame::OnSingleSelMode(wxCommandEvent& event)
{
    CreateControls();
}

void MyFrame::OnMultipleSelMode(wxCommandEvent& event)
{
    CreateControls();
}

void MyFrame::OnResizeableColumn(wxCommandEvent& event)
{
    CreateControls();
}

void MyFrame::OnSortableColumn(wxCommandEvent& event)
{
    CreateControls();
}

void MyFrame::OnHideColumn(wxCommandEvent& event)
{
    CreateControls();
}

void MyFrame::OnChooseAlign(wxCommandEvent& event)
{
    const wxString choices[] =
    {
        wxT("Left"),
        wxT("Center horizontally"),
        wxT("Right"),
        wxT("Top"),
        wxT("Center vertically"),
        wxT("Bottom"),
        wxT("Center")
    };

    wxAlignment flags[] =
    {
        wxALIGN_LEFT,
        wxALIGN_CENTER_HORIZONTAL,
        wxALIGN_RIGHT,
        wxALIGN_TOP,
        wxALIGN_CENTER_VERTICAL,
        wxALIGN_BOTTOM,
        wxALIGN_CENTER
    };

    int choice = wxGetSingleChoiceIndex(
        wxT("Select the alignment for the cells of the wxDataViewCtrl:"),
        wxT("Alignment"),
        WXSIZEOF(choices), choices,
        this);

    if (choice == wxNOT_FOUND)
        return;

    m_align = flags[choice];
    CreateControls();
}


// -------------------------------------
// MySortingFrame
// -------------------------------------

BEGIN_EVENT_TABLE(MySortingFrame,wxFrame)
    EVT_BUTTON( ID_APPEND_ROW_LEFT, MySortingFrame::OnAppendRowLeft )
    EVT_BUTTON( ID_PREPEND_ROW_LEFT, MySortingFrame::OnPrependRowLeft )
    EVT_BUTTON( ID_INSERT_ROW_LEFT, MySortingFrame::OnInsertRowLeft )
    EVT_BUTTON( ID_DELETE_ROW_LEFT, MySortingFrame::OnDeleteRowLeft )
    EVT_BUTTON( ID_SELECT, MySortingFrame::OnSelect )
    EVT_BUTTON( ID_UNSELECT_ALL, MySortingFrame::OnUnselectAll )
    EVT_DATAVIEW_ROW_SELECTED( ID_SORTED, MySortingFrame::OnSelectedSorted )
    EVT_DATAVIEW_ROW_SELECTED( ID_UNSORTED, MySortingFrame::OnSelectedUnsorted )
    EVT_DATAVIEW_ROW_ACTIVATED( ID_UNSORTED, MySortingFrame::OnActivatedUnsorted )
    EVT_DATAVIEW_COLUMN_HEADER_CLICK( ID_SORTED, MySortingFrame::OnHeaderClickSorted )
    EVT_DATAVIEW_COLUMN_HEADER_CLICK( ID_UNSORTED, MySortingFrame::OnHeaderClickUnsorted )
END_EVENT_TABLE()

MySortingFrame::MySortingFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h):
  wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    m_logOld = NULL;
    
    SetIcon(wxICON(sample));
    CreateStatusBar();

    wxPanel *main = new wxPanel(this);

    // Left wxDataViewCtrl
    dataview_left = new wxDataViewCtrl( main, ID_UNSORTED, wxDefaultPosition, 
                                        wxDefaultSize, wxDV_MULTIPLE );

    m_unsorted_model = new MyUnsortedTextModel;
    dataview_left->AssociateModel( m_unsorted_model );
    m_unsorted_model->DecRef(); // don't leak memory

    wxDataViewTextRenderer *text_renderer = 
        new wxDataViewTextRenderer( wxT("string"), wxDATAVIEW_CELL_EDITABLE );
    wxDataViewColumn *column = new wxDataViewColumn( wxT("editable"), text_renderer, 0 );
    dataview_left->AppendColumn( column );
    dataview_left->AppendTextColumn( wxT("second"), 1 );
    dataview_left->AppendColumn( new wxDataViewColumn( wxBitmap(null_xpm), 
                                 new wxDataViewBitmapRenderer, 2, 25 ) );
    dataview_left->AppendColumn( new wxDataViewColumn( wxT("icon"), 
                                 new wxDataViewBitmapRenderer, 3, 25 ) );

    // Right wxDataViewCtrl using the sorting model
    dataview_right = new wxDataViewCtrl( main, ID_SORTED );
    
    m_sorted_model = new wxDataViewSortedListModel( m_unsorted_model );
    dataview_right->AssociateModel( m_sorted_model );
    m_sorted_model->DecRef(); // don't leak memory

    text_renderer = new wxDataViewTextRenderer( wxT("string"), wxDATAVIEW_CELL_EDITABLE );
    column = new wxDataViewColumn( wxT("editable"), text_renderer, 0, -1, 
                                   wxALIGN_CENTER,
                                   wxDATAVIEW_COL_SORTABLE|wxDATAVIEW_COL_RESIZABLE );
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
    left_sizer->Add( new wxButton( main, ID_APPEND_ROW_LEFT, wxT("Append") ), 0, wxALL, 5 );
    left_sizer->Add( new wxButton( main, ID_PREPEND_ROW_LEFT, wxT("Prepend") ), 0, wxALL, 5 );
    left_sizer->Add( new wxButton( main, ID_INSERT_ROW_LEFT, wxT("Insert") ), 0, wxALL, 5 );
    left_sizer->Add( new wxButton( main, ID_DELETE_ROW_LEFT, wxT("Delete second") ), 0, wxALL, 5 );
    left_sizer->Add( new wxButton( main, ID_EDIT_ROW_LEFT, wxT("Edit") ), 0, wxALL, 5 );
    left_sizer->Add( 5,5 );
    left_sizer->Add( new wxButton( main, ID_SELECT, wxT("Select third") ), 0, wxALL, 5 );
    left_sizer->Add( new wxButton( main, ID_UNSELECT_ALL, wxT("Unselect all") ), 0, wxALL, 5 );
    button_sizer->Add( left_sizer );
    button_sizer->Add( 10, 10, 2 );
    wxFlexGridSizer *right_sizer = new wxFlexGridSizer( 2 );
    right_sizer->Add( new wxButton( main, ID_APPEND_ROW_RIGHT, wxT("Append") ), 0, wxALL, 5 );
    right_sizer->Add( new wxButton( main, ID_PREPEND_ROW_RIGHT, wxT("Prepend") ), 0, wxALL, 5 );
    right_sizer->Add( new wxButton( main, ID_INSERT_ROW_RIGHT, wxT("Insert") ), 0, wxALL, 5 );
    right_sizer->Add( new wxButton( main, ID_DELETE_ROW_RIGHT, wxT("Delete second") ), 0, wxALL, 5 );
    right_sizer->Add( new wxButton( main, ID_EDIT_ROW_RIGHT, wxT("Edit") ), 0, wxALL, 5 );
    button_sizer->Add( right_sizer );
    button_sizer->Add( 10, 10, 1 );

    wxBoxSizer *main_sizer = new wxBoxSizer( wxVERTICAL );
    main_sizer->Add( top_sizer, 1, wxGROW );
    main_sizer->Add( button_sizer, 0, wxGROW );
    
    m_logWindow = new wxTextCtrl(main, wxID_ANY, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_MULTILINE | wxSUNKEN_BORDER);
    main_sizer->Add( 20,20 );
    main_sizer->Add( m_logWindow, 1, wxGROW );

    m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_logWindow));

    main->SetSizer( main_sizer );
}

MySortingFrame::~MySortingFrame()
{
    delete wxLog::SetActiveTarget(m_logOld);
}

void MySortingFrame::OnSelectedUnsorted(wxDataViewEvent &event)
{
    int row = event.GetRow();
    wxLogMessage( wxT("OnSelected from unsorted list, selected %d"), row );
    if (row >= 0)
        wxLogMessage( wxT("wxDataViewCtrl::IsSelected( %d ): %d (as int)"), 
                    row, (int) dataview_right->IsSelected( row ) );
}

void MySortingFrame::OnSelectedSorted(wxDataViewEvent &event)
{
    wxLogMessage( wxT("OnSelected from sorted list, selected %d"), (int) event.GetRow() );
}

void MySortingFrame::OnActivatedUnsorted(wxDataViewEvent &event)
{
    wxLogMessage( wxT("OnActivated from unsorted list, activated %d"), (int) event.GetRow() );
}

void MySortingFrame::OnHeaderClickSorted(wxDataViewEvent &event)
{
    wxDataViewColumn *col = event.GetDataViewColumn();
    wxLogMessage( wxT("OnHeaderClick from sorted list, column %s"), col->GetTitle().c_str() );
    
    if (col->GetTitle() == wxT("editable"))
    {
        // this is the sorting column
        if (col->IsSortOrderAscending())
        {
            col->SetSortOrder( false );
            m_sorted_model->SetAscending( false );
            m_sorted_model->Resort();
        }
        else
        {
            col->SetSortOrder( true );
            m_sorted_model->SetAscending( true );
            m_sorted_model->Resort();
        }
    }
}

void MySortingFrame::OnHeaderClickUnsorted(wxDataViewEvent &event)
{
    wxLogMessage( wxT("OnHeaderClick from unsorted list, column %s"), 
                  event.GetDataViewColumn()->GetTitle().c_str() );
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

void MySortingFrame::OnSelect(wxCommandEvent& WXUNUSED(event))
{
    dataview_left->SetSelection( 2 );
}

void MySortingFrame::OnUnselectAll(wxCommandEvent& WXUNUSED(event))
{
    dataview_left->ClearSelection();
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

