/////////////////////////////////////////////////////////////////////////////
// Name:        dataview.cpp
// Purpose:     wxDataViewCtrl wxWidgets sample
// Author:      Robert Roebling
// Modified by: Francesco Montorsi, Bo Yang
// Created:     06/01/06
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dataview.h"
#include "wx/datetime.h"
#include "wx/splitter.h"
#include "wx/aboutdlg.h"
#include "wx/choicdlg.h"
#include "wx/numdlg.h"
#include "wx/spinctrl.h"
#include "wx/imaglist.h"
#include "wx/notebook.h"

#include "mymodels.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif

#include "wx_small.xpm"

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

class MyFrame : public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString &title, int x, int y, int w, int h);
    ~MyFrame();

public:
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnAddMozart(wxCommandEvent& event);
    void OnDeleteMusic(wxCommandEvent& event);
    void OnDeleteYear(wxCommandEvent& event);
    void OnSelectNinth(wxCommandEvent& event);

    void OnPrependList(wxCommandEvent& event);
    void OnDeleteList(wxCommandEvent& event);

    void OnValueChanged( wxDataViewEvent &event );

    void OnActivated( wxDataViewEvent &event );
    void OnExpanding( wxDataViewEvent &event );
    void OnExpanded( wxDataViewEvent &event );
    void OnCollapsing( wxDataViewEvent &event );
    void OnCollapsed( wxDataViewEvent &event );
    void OnSelectionChanged( wxDataViewEvent &event );

    void OnEditingStarted( wxDataViewEvent &event );
    void OnEditingDone( wxDataViewEvent &event );

    void OnHeaderClick( wxDataViewEvent &event );
    void OnHeaderRightClick( wxDataViewEvent &event );
    void OnSorted( wxDataViewEvent &event );

    void OnContextMenu( wxDataViewEvent &event );

    void OnRightClick( wxMouseEvent &event );
    void OnGoto( wxCommandEvent &event);
    void OnAddMany( wxCommandEvent &event);

    void OnBeginDrag( wxDataViewEvent &event );
    void OnDropPossible( wxDataViewEvent &event );
    void OnDrop( wxDataViewEvent &event );

private:
    wxNotebook* m_notebook;

    // the controls stored in the various tabs of the main notebook:

    wxDataViewCtrl* m_myMusicModelViewCtrl;
    wxObjectDataPtr<MyMusicTreeModel> m_music_model;

    wxDataViewCtrl* m_myListModelViewCtrl;
    wxObjectDataPtr<MyListModel> m_list_model;

    wxDataViewListCtrl* m_listctrl;
    wxDataViewTreeCtrl* m_treectrl;

    // other data:

    wxDataViewColumn* m_col;

    wxTextCtrl* m_log;
    wxLog *m_logOld;

private:
    DECLARE_EVENT_TABLE()
};


// ----------------------------------------------------------------------------
// MyCustomRenderer
// ----------------------------------------------------------------------------

class MyCustomRenderer: public wxDataViewCustomRenderer
{
public:
    MyCustomRenderer( wxDataViewCellMode mode, int alignment ) :
       wxDataViewCustomRenderer( wxString("long"), mode, alignment )
       { m_height = 25; }

    virtual bool Render( wxRect rect, wxDC *dc, int WXUNUSED(state) )
    {
        dc->SetBrush( *wxRED_BRUSH );
        dc->SetPen( *wxTRANSPARENT_PEN );
        dc->DrawRectangle( rect );
        return true;
    }


    virtual bool Activate( wxRect WXUNUSED(cell),
                           wxDataViewModel *WXUNUSED(model), 
                           const wxDataViewItem &WXUNUSED(item), 
                           unsigned int WXUNUSED(col) )
    {
        wxLogMessage( wxT("MyCustomRenderer Activate()") );
        return false;
    }

    virtual bool LeftClick( wxPoint cursor, wxRect WXUNUSED(cell),
                           wxDataViewModel *WXUNUSED(model), 
                           const wxDataViewItem &WXUNUSED(item), 
                           unsigned int WXUNUSED(col) )
    {
        wxLogMessage( wxT("MyCustomRenderer LeftClick( %d, %d )"), cursor.x, cursor.y );
        return false;
    }

    virtual wxSize GetSize() const
    {
        //return wxSize(60,m_height);
        return wxSize(60,20);
    }

    virtual bool SetValue( const wxVariant &value )
    {
        m_height = value;
        return true;
    }

    virtual bool GetValue( wxVariant &WXUNUSED(value) ) const { return true; }

private:
    long m_height;
};


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    MyFrame *frame =
        new MyFrame(NULL, wxT("wxDataViewCtrl sample"), 40, 40, 1000, 540);
    SetTopWindow(frame);

    frame->Show(true);
    return true;
}


// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

enum
{
    // file menu
    ID_ABOUT = wxID_ABOUT,
    ID_EXIT = wxID_EXIT,

    ID_MUSIC_CTRL       = 50,

    ID_ADD_MOZART       = 100,
    ID_DELETE_MUSIC     = 101,
    ID_DELETE_YEAR      = 102,
    ID_SELECT_NINTH     = 103,

    ID_PREPEND_LIST     = 200,
    ID_DELETE_LIST      = 201,
    ID_GOTO             = 202,
    ID_ADD_MANY         = 203
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU( ID_ABOUT, MyFrame::OnAbout )
    EVT_MENU( ID_EXIT, MyFrame::OnQuit )
    EVT_BUTTON( ID_ADD_MOZART, MyFrame::OnAddMozart )
    EVT_BUTTON( ID_DELETE_MUSIC, MyFrame::OnDeleteMusic )
    EVT_BUTTON( ID_DELETE_YEAR, MyFrame::OnDeleteYear )
    EVT_BUTTON( ID_SELECT_NINTH, MyFrame::OnSelectNinth )
    EVT_BUTTON( ID_PREPEND_LIST, MyFrame::OnPrependList )
    EVT_BUTTON( ID_DELETE_LIST, MyFrame::OnDeleteList )
    EVT_BUTTON( ID_GOTO, MyFrame::OnGoto)
    EVT_BUTTON( ID_ADD_MANY, MyFrame::OnAddMany)

    EVT_DATAVIEW_ITEM_VALUE_CHANGED( ID_MUSIC_CTRL, MyFrame::OnValueChanged )

    EVT_DATAVIEW_ITEM_ACTIVATED(ID_MUSIC_CTRL, MyFrame::OnActivated )
    EVT_DATAVIEW_ITEM_EXPANDING(ID_MUSIC_CTRL, MyFrame::OnExpanding)
    EVT_DATAVIEW_ITEM_EXPANDED(ID_MUSIC_CTRL, MyFrame::OnExpanded)
    EVT_DATAVIEW_ITEM_COLLAPSING(ID_MUSIC_CTRL, MyFrame::OnCollapsing)
    EVT_DATAVIEW_ITEM_COLLAPSED(ID_MUSIC_CTRL, MyFrame::OnCollapsed)
    EVT_DATAVIEW_SELECTION_CHANGED(ID_MUSIC_CTRL, MyFrame::OnSelectionChanged)

    EVT_DATAVIEW_ITEM_EDITING_STARTED(ID_MUSIC_CTRL, MyFrame::OnEditingStarted)
    EVT_DATAVIEW_ITEM_EDITING_DONE(ID_MUSIC_CTRL, MyFrame::OnEditingDone)

    EVT_DATAVIEW_COLUMN_HEADER_CLICK(ID_MUSIC_CTRL, MyFrame::OnHeaderClick)
    EVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICKED(ID_MUSIC_CTRL, MyFrame::OnHeaderRightClick)
    EVT_DATAVIEW_COLUMN_SORTED(ID_MUSIC_CTRL, MyFrame::OnSorted)

    EVT_DATAVIEW_ITEM_CONTEXT_MENU(ID_MUSIC_CTRL, MyFrame::OnContextMenu)

    EVT_DATAVIEW_ITEM_BEGIN_DRAG( ID_MUSIC_CTRL, MyFrame::OnBeginDrag )
    EVT_DATAVIEW_ITEM_DROP_POSSIBLE( ID_MUSIC_CTRL, MyFrame::OnDropPossible )
    EVT_DATAVIEW_ITEM_DROP( ID_MUSIC_CTRL, MyFrame::OnDrop )

    EVT_RIGHT_UP(MyFrame::OnRightClick)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, const wxString &title, int x, int y, int w, int h):
  wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    m_log = NULL;
    m_col = NULL;

    SetIcon(wxICON(sample));


    // build the menus
    // ----------------

    wxMenu *file_menu = new wxMenu;
    file_menu->Append(ID_ABOUT, wxT("&About"));
    file_menu->AppendSeparator();
    file_menu->Append(ID_EXIT, wxT("E&xit"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, wxT("&File"));

    SetMenuBar(menu_bar);
    CreateStatusBar();


    // first page of the notebook
    // --------------------------
    
    m_notebook = new wxNotebook( this, wxID_ANY );

    wxPanel *firstPanel = new wxPanel( m_notebook, wxID_ANY );
    m_myMusicModelViewCtrl = 
        new wxDataViewCtrl( firstPanel, ID_MUSIC_CTRL, wxDefaultPosition,
                            wxDefaultSize, wxDV_MULTIPLE|wxDV_VARIABLE_LINE_HEIGHT );

    m_music_model = new MyMusicTreeModel;
    m_myMusicModelViewCtrl->AssociateModel( m_music_model.get() );

    m_myMusicModelViewCtrl->EnableDragSource( wxDF_UNICODETEXT );
    m_myMusicModelViewCtrl->EnableDropTarget( wxDF_UNICODETEXT );

    // column 0 of the view control:

    wxDataViewTextRenderer *tr = 
        new wxDataViewTextRenderer( wxT("string"), wxDATAVIEW_CELL_INERT );
    wxDataViewColumn *column0 = 
        new wxDataViewColumn( wxT("title"), tr, 0, 200, wxALIGN_LEFT,
                              wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE );
    m_myMusicModelViewCtrl->AppendColumn( column0 );
#if 0
    // Call this and sorting is enabled
    // immediatly upon start up.
    column0->SetAsSortKey();
#endif

    // column 1 of the view control:

    tr = new wxDataViewTextRenderer( wxT("string"), wxDATAVIEW_CELL_EDITABLE );
    wxDataViewColumn *column1 = 
        new wxDataViewColumn( wxT("artist"), tr, 1, 150, wxALIGN_LEFT,
                              wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE | 
                              wxDATAVIEW_COL_RESIZABLE );
    column1->SetMinWidth(150); // this column can't be resized to be smaller
    m_myMusicModelViewCtrl->AppendColumn( column1 );

    // column 2 of the view control:

    wxDataViewSpinRenderer *sr = 
        new wxDataViewSpinRenderer( 0, 2010, wxDATAVIEW_CELL_EDITABLE, wxALIGN_RIGHT );
    wxDataViewColumn *column2 = 
        new wxDataViewColumn( wxT("year"), sr, 2, 60, wxALIGN_LEFT,
                              wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE );
    m_myMusicModelViewCtrl->AppendColumn( column2 );

    // column 3 of the view control:

    wxArrayString choices;
    choices.Add( "good" );
    choices.Add( "bad" );
    choices.Add( "lousy" );
    wxDataViewChoiceRenderer *c = 
        new wxDataViewChoiceRenderer( choices, wxDATAVIEW_CELL_EDITABLE, wxALIGN_RIGHT );
    wxDataViewColumn *column3 = 
        new wxDataViewColumn( wxT("rating"), c, 3, 100, wxALIGN_LEFT,
                              wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE );
    m_myMusicModelViewCtrl->AppendColumn( column3 );

    // column 4 of the view control:

    m_myMusicModelViewCtrl->AppendProgressColumn( wxT("popularity"), 4, wxDATAVIEW_CELL_INERT, 80 );

    // column 5 of the view control:

    MyCustomRenderer *cr = new MyCustomRenderer( wxDATAVIEW_CELL_ACTIVATABLE, wxALIGN_RIGHT );
    wxDataViewColumn *column5 = 
        new wxDataViewColumn( wxT("custom"), cr, 5, -1, wxALIGN_LEFT,
                              wxDATAVIEW_COL_RESIZABLE );
    m_myMusicModelViewCtrl->AppendColumn( column5 );

    // complete this page:

    wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );
    button_sizer->Add( new wxButton( firstPanel, ID_ADD_MOZART,  _("Add Mozart")),             0, wxALL, 10 );
    button_sizer->Add( new wxButton( firstPanel, ID_DELETE_MUSIC,_("Delete selected")),        0, wxALL, 10 );
    button_sizer->Add( new wxButton( firstPanel, ID_DELETE_YEAR, _("Delete \"Year\" column")), 0, wxALL, 10 );
    button_sizer->Add( new wxButton( firstPanel, ID_SELECT_NINTH,_("Select Ninth")),           0, wxALL, 10 );

    wxSizer *firstPanelSz = new wxBoxSizer( wxVERTICAL );
    m_myMusicModelViewCtrl->SetMinSize(wxSize(-1, 200));
    firstPanelSz->Add(m_myMusicModelViewCtrl, 1, wxGROW|wxALL, 5);
    firstPanelSz->Add(button_sizer);
    firstPanel->SetSizerAndFit(firstPanelSz);


    // second page of the notebook
    // ---------------------------

    wxPanel *secondPanel = new wxPanel( m_notebook, wxID_ANY );

    m_myListModelViewCtrl = new wxDataViewCtrl( secondPanel, wxID_ANY, wxDefaultPosition,
                                                wxDefaultSize, wxDV_MULTIPLE | wxDV_ROW_LINES);

    m_list_model = new MyListModel;
    m_myListModelViewCtrl->AssociateModel( m_list_model.get() );

    // the various columns
#if 1
    m_myListModelViewCtrl->AppendTextColumn    (wxT("editable string"), 0, wxDATAVIEW_CELL_EDITABLE, 120 );
    m_myListModelViewCtrl->AppendIconTextColumn(wxIcon(wx_small_xpm), 1, wxDATAVIEW_CELL_INERT )->SetTitle( wxT("icon") );
#else
    m_myListModelViewCtrl->AppendTextColumn    (wxT("editable string"), 0, wxDATAVIEW_CELL_EDITABLE );
    m_myListModelViewCtrl->AppendIconTextColumn(wxT("icon"),            1, wxDATAVIEW_CELL_INERT );
#endif
    m_myListModelViewCtrl->AppendColumn(
        new wxDataViewColumn(wxT("attributes"), new wxDataViewTextRendererAttr, 2 ));

    // complete this page:

    wxBoxSizer *button_sizer2 = new wxBoxSizer( wxHORIZONTAL );
    button_sizer2->Add( new wxButton( secondPanel, ID_PREPEND_LIST,_("Prepend")),         0, wxALL, 10 );
    button_sizer2->Add( new wxButton( secondPanel, ID_DELETE_LIST, _("Delete selected")), 0, wxALL, 10 );
    button_sizer2->Add( new wxButton( secondPanel, ID_GOTO,        _("Goto 50")),         0, wxALL, 10 );
    button_sizer2->Add( new wxButton( secondPanel, ID_ADD_MANY,    _("Add 1000")),        0, wxALL, 10 );

    wxSizer *secondPanelSz = new wxBoxSizer( wxVERTICAL );
    secondPanelSz->Add(m_myListModelViewCtrl, 1, wxGROW|wxALL, 5);
    secondPanelSz->Add(button_sizer2);
    secondPanel->SetSizerAndFit(secondPanelSz);



    // third page of the notebook
    // ---------------------------

    wxPanel *thirdPanel = new wxPanel( m_notebook, wxID_ANY );

    m_listctrl = new wxDataViewListCtrl( thirdPanel, wxID_ANY );
    m_listctrl->AppendToggleColumn( wxT("Toggle") );
    m_listctrl->AppendTextColumn( wxT("Text") );

    wxVector<wxVariant> data;
    data.push_back( true );
    data.push_back( "row 1" );
    m_listctrl->AppendItem( data );

    data.clear();
    data.push_back( false );
    data.push_back( "row 3" );
    m_listctrl->AppendItem( data );

    // complete this page:

    wxSizer *thirdPanelSz = new wxBoxSizer( wxVERTICAL );
    thirdPanelSz->Add(m_listctrl, 1, wxGROW|wxALL, 5);
    thirdPanel->SetSizerAndFit(thirdPanelSz);


    // fourth page of the notebook
    // ---------------------------
    
    wxPanel *fourthPanel = new wxPanel( m_notebook, wxID_ANY );

    m_treectrl = new wxDataViewTreeCtrl( fourthPanel, wxID_ANY );

    wxImageList *ilist = new wxImageList( 16, 16 );
    ilist->Add( wxIcon(wx_small_xpm) );
    m_treectrl->SetImageList( ilist );

    wxDataViewItem parent2 = m_treectrl->AppendContainer( wxDataViewItem(0),wxT("Root 1"), 0 );
    m_treectrl->AppendItem( parent2, wxT("Child 1"), 0 );
    m_treectrl->AppendItem( parent2, wxT("Child 2"), 0 );
    m_treectrl->AppendItem( parent2, wxT("Child 3, very long, long, long, long"), 0 );

    // complete this page:

    wxSizer *fourthPanelSz = new wxBoxSizer( wxVERTICAL );
    fourthPanelSz->Add(m_treectrl, 1, wxGROW|wxALL, 5);
    fourthPanel->SetSizerAndFit(fourthPanelSz);


    
    // complete GUI
    // ------------

    m_notebook->AddPage(firstPanel, "MyMusicTreeModel");
    m_notebook->AddPage(secondPanel, "MyListModel");
    m_notebook->AddPage(thirdPanel, "wxDataViewListCtrl");
    m_notebook->AddPage(fourthPanel, "wxDataViewTreeCtrl");

    wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    m_log = new wxTextCtrl( this, wxID_ANY, wxString(), wxDefaultPosition, 
                            wxDefaultSize, wxTE_MULTILINE );
    m_log->SetMinSize(wxSize(-1, 100));
    m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_log));
    wxLogMessage(_("This is the log window"));

    mainSizer->Add( m_notebook, 1, wxGROW );
    mainSizer->Add( m_log, 0, wxGROW );

    SetSizerAndFit(mainSizer);
}

MyFrame::~MyFrame()
{
    delete wxLog::SetActiveTarget(m_logOld);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnAddMozart(wxCommandEvent& WXUNUSED(event) )
{
    m_music_model->AddToClassical( wxT("Kleine Nachtmusik"), wxT("Wolfgang Mozart"), 1787 );
}

void MyFrame::OnDeleteMusic(wxCommandEvent& WXUNUSED(event) )
{
    wxDataViewItemArray items;
    int len = m_myMusicModelViewCtrl->GetSelections( items );
    for( int i = 0; i < len; i ++ )
        if (items[i].IsOk())
            m_music_model->Delete( items[i] );
}

void MyFrame::OnDeleteYear( wxCommandEvent& WXUNUSED(event) )
{
    m_myMusicModelViewCtrl->DeleteColumn( m_myMusicModelViewCtrl->GetColumn( 2 ) );
    FindWindow( ID_DELETE_YEAR )->Disable();
}

void MyFrame::OnSelectNinth( wxCommandEvent& WXUNUSED(event) )
{
    m_myMusicModelViewCtrl->Select( m_music_model->GetNinthItem() );
}

void MyFrame::OnPrependList( wxCommandEvent& WXUNUSED(event) )
{
    m_list_model->Prepend(wxT("Test"));
}

void MyFrame::OnDeleteList( wxCommandEvent& WXUNUSED(event) )
{
    wxDataViewItemArray items;
    int len = m_myListModelViewCtrl->GetSelections( items );
    if (len > 0)
        m_list_model->DeleteItems( items );
}

void MyFrame::OnValueChanged( wxDataViewEvent &event )
{
    if (!m_log)
        return;

    wxLogMessage( wxT("EVT_DATAVIEW_ITEM_VALUE_CHANGED, Item Id: %d;  Column: %d"), 
                  event.GetItem().GetID(), event.GetColumn() );
}

void MyFrame::OnActivated( wxDataViewEvent &event )
{
    if(!m_log)
        return;

    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, Item: %s"), title );

    if (m_myMusicModelViewCtrl->IsExpanded( event.GetItem() ))
        wxLogMessage(wxT("Item: %s is expanded"), title );
}

void MyFrame::OnSelectionChanged( wxDataViewEvent &event )
{
    if(!m_log)
        return;

    wxString title = m_music_model->GetTitle( event.GetItem() );
    if (title.empty())
        title = wxT("None");

    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, First selected Item: %s"), title );
}

void MyFrame::OnExpanding( wxDataViewEvent &event )
{
    if (!m_log)
        return;

    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING, Item: %s"), title );
}


void MyFrame::OnEditingStarted( wxDataViewEvent &event )
{
    if (!m_log)
        return;

    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_STARTED, Item: %s"), title );
}

void MyFrame::OnEditingDone( wxDataViewEvent &event )
{
    if (!m_log)
        return;

    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_DONE, Item: %s"), title );
}

void MyFrame::OnExpanded( wxDataViewEvent &event )
{
    if (!m_log)
        return;

    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDED, Item: %s"), title );
}

void MyFrame::OnCollapsing( wxDataViewEvent &event )
{
    if (!m_log)
        return;

    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSING, Item: %s"), title );
}

void MyFrame::OnCollapsed( wxDataViewEvent &event )
{
    if (!m_log)
        return;

    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSED, Item: %s"),title);
}

void MyFrame::OnContextMenu( wxDataViewEvent &event )
{
    if (!m_log)
        return;

    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, Item: %s"),title );

    wxMenu menu;
    menu.Append( 1, wxT("entry 1") );
    menu.Append( 2, wxT("entry 2") );
    menu.Append( 3, wxT("entry 3") );

    m_myMusicModelViewCtrl->PopupMenu(&menu);

//    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, Item: %s Value: %s"),
//                 title.GetData(), event.GetValue().GetString());
}

void MyFrame::OnHeaderClick( wxDataViewEvent &event )
{
    // we need to skip the event to let the default behaviour of sorting by
    // this column when it is clicked to take place
    event.Skip();

    if(!m_log)
        return;

    int pos = m_myMusicModelViewCtrl->GetColumnPosition( event.GetDataViewColumn() );

    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK, Column position: %d"), pos );
}

void MyFrame::OnHeaderRightClick( wxDataViewEvent &event )
{
    if(!m_log)
        return;

    int pos = m_myMusicModelViewCtrl->GetColumnPosition( event.GetDataViewColumn() );

    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, Column position: %d"), pos );
}

void MyFrame::OnSorted( wxDataViewEvent &event )
{
    if(!m_log)
        return;

    int pos = m_myMusicModelViewCtrl->GetColumnPosition( event.GetDataViewColumn() );

    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED, Column position: %d"), pos );
}

void MyFrame::OnRightClick( wxMouseEvent &event )
{
    if(!m_log)
        return;

    wxLogMessage(wxT("wxEVT_MOUSE_RIGHT_UP, Click Point is X: %d, Y: %d"), 
                 event.GetX(), event.GetY());
}

void MyFrame::OnGoto(wxCommandEvent& WXUNUSED(event))
{
    wxDataViewItem item = m_list_model->GetItem( 50 );
    m_myListModelViewCtrl->EnsureVisible(item,m_col);
}

void MyFrame::OnAddMany(wxCommandEvent& WXUNUSED(event))
{
    m_list_model->AddMany();
}


void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxAboutDialogInfo info;
    info.SetName(_("DataView sample"));
    info.SetDescription(_("This sample demonstrates wxDataViewCtrl"));
    info.SetCopyright(_T("(C) 2007-2009 Robert Roebling"));
    info.AddDeveloper("Robert Roebling");
    info.AddDeveloper("Francesco Montorsi");

    wxAboutBox(info);
}

void MyFrame::OnBeginDrag( wxDataViewEvent &event )
{
    wxDataViewItem item( event.GetItem() );

    // only allow drags for item, not containers
    if (m_music_model->IsContainer( item ) )
    {
        event.Veto();
        return;
    }

    MyMusicTreeModelNode *node = (MyMusicTreeModelNode*) item.GetID();
    wxTextDataObject *obj = new wxTextDataObject;
    obj->SetText( node->m_title );
    event.SetDataObject( obj );
}

void MyFrame::OnDropPossible( wxDataViewEvent &event )
{
    wxDataViewItem item( event.GetItem() );

    // only allow drags for item, not containers
    if (m_music_model->IsContainer( item ) )
        event.Veto();

    if (event.GetDataFormat() != wxDF_UNICODETEXT)
        event.Veto();
}

void MyFrame::OnDrop( wxDataViewEvent &event )
{
    wxDataViewItem item( event.GetItem() );

    // only allow drops for item, not containers
    if (m_music_model->IsContainer( item ) )
    {
        event.Veto();
        return;
    }

    if (event.GetDataFormat() != wxDF_UNICODETEXT)
    {
        event.Veto();
        return;
    }

    wxTextDataObject obj;
    obj.SetData( wxDF_TEXT, event.GetDataSize(), event.GetDataBuffer() );

    wxLogMessage(wxT("Text dropped: %s"), obj.GetText() );
}

