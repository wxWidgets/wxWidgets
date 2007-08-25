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



class MyMusicModelNode;
WX_DEFINE_ARRAY_PTR( MyMusicModelNode*, MyMusicModelNodes );

class MyMusicModelNode
{
public:
    MyMusicModelNode( MyMusicModelNode* parent, 
                      const wxString &title, const wxString &artist, const wxString &year )
    { 
        m_parent = parent; 
        m_title = title;
        m_artist = artist;
        m_year = year;
        m_isContainer = false;
    }
    
    MyMusicModelNode( MyMusicModelNode* parent,
                      const wxString &branch )
    { 
        m_parent = parent; 
        m_title = branch;
        m_isContainer = true;
    }
    
    ~MyMusicModelNode()
    { 
        size_t count = m_children.GetCount();
        size_t i;
        for (i = 0; i < count; i++)
        {
            MyMusicModelNode *child = m_children[i];
            delete child;
        }
    }

    bool IsContainer()                                    { return m_isContainer; }

    MyMusicModelNode* GetParent()                         { return m_parent; }
    MyMusicModelNodes &GetChildren()                      { return m_children; }
    MyMusicModelNode* GetNthChild( unsigned int n )       { return m_children.Item( n ); }
    void Insert( MyMusicModelNode* child, unsigned int n) { m_children.Insert( child, n); }
    void Append( MyMusicModelNode* child )                { m_children.Add( child ); }
    unsigned int GetChildCount()                          { return m_children.GetCount(); }

public:
    wxString            m_title;
    wxString            m_artist;
    wxString            m_year;
    
private:
    MyMusicModelNode   *m_parent;
    MyMusicModelNodes   m_children; 
    bool                m_isContainer;
};

 
class MyMusicModel: public wxDataViewModel
{
public:

    // constructor

    MyMusicModel() 
    {
        m_root = new MyMusicModelNode( NULL, "My Music" );
        m_pop = new MyMusicModelNode( m_root, "Pop music" );
        m_root->Append( m_pop );
        m_pop->Append( new MyMusicModelNode( m_pop, 
            "You are not alone", "Michael Jackson", "1995" ) );
        m_pop->Append( new MyMusicModelNode( m_pop, 
            "Take a bow", "Madonna", "1994" ) );
        m_classical = new MyMusicModelNode( m_root, "Classical music" );
        m_root->Append( m_classical );
        m_classical->Append( new MyMusicModelNode( m_classical, 
            "Ninth symphony", "Ludwig van Beethoven", "1824" ) );
        m_classical->Append( new MyMusicModelNode( m_classical, 
            "German Requiem", "Johannes Brahms", "1868" ) );
        m_classicalMusicIsKnownToControl = false;
    }
    
    // helper methods to change the model

    void AddToClassical( const wxString &title, const wxString &artist, const wxString &year )
    {
        // add to data
        MyMusicModelNode *child_node = 
            new MyMusicModelNode( m_classical, title, artist, year );
        
        m_classical->Append( child_node );
        
        if (m_classicalMusicIsKnownToControl)
        {
            // notify control
            wxDataViewItem child( (void*) child_node );
            wxDataViewItem parent( (void*) m_classical );
            ItemAdded( parent, child );
        }
    }

    void Delete( const wxDataViewItem &item )
    {
        MyMusicModelNode *node = (MyMusicModelNode*) item.GetID();
        wxDataViewItem parent( node->GetParent() );
        
        // notify control
        ItemDeleted( parent, item );
        //We must delete the node after we call ItemDeleted
        //The reason is that:
        //When we use wxSortedArray, the array find a node through binary search for speed.
        //And when the array is searching for some node, it call the model's compare function.
        //The compare function need the node to be compared. So we should delete the node later, here.
        node->GetParent()->GetChildren().Remove( node );
        delete node;
    }
    
    // override sorting to always sort branches ascendingly
    
    int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2, 
                 unsigned int column, bool ascending )
    {
        if (IsContainer(item1) && IsContainer(item2))
        {
            wxVariant value1,value2;
            GetValue( value1, item1, 0 );
            GetValue( value2, item2, 0 );

            wxString str1 = value1.GetString();
            wxString str2 = value2.GetString();
            int res = str1.Cmp( str2 );
            if (res) return res;
            
            // items must be different
            unsigned long litem1 = (unsigned long) item1.GetID();    
            unsigned long litem2 = (unsigned long) item2.GetID();    

            return litem1-litem2;
        }
        
        return wxDataViewModel::Compare( item1, item2, column, ascending );
    }

    // implementation of base class virtuals to define model
    
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
        MyMusicModelNode *node = (MyMusicModelNode*) item.GetID();
        switch (col)
        {
            case 0: variant = node->m_title; break;
            case 1: variant = node->m_artist; break;
            case 2: variant = node->m_year; break;
            default: 
            {
                wxLogError( "MyMusicModel::GetValue: wrong column" );
                wxMouseState state = wxGetMouseState();
                if (state.ShiftDown())
                {
                    char *crash = 0;
                    *crash = 0;
                }
            }
        }
    }

    virtual bool SetValue( const wxVariant &variant, 
                           const wxDataViewItem &item, unsigned int col )
    {
        MyMusicModelNode *node = (MyMusicModelNode*) item.GetID();
        switch (col)
        {
            case 0: node->m_title = variant.GetString(); break;
            case 1: node->m_artist  = variant.GetString(); break;
            case 2: node->m_year  = variant.GetString(); break;
            default: wxLogError( "MyMusicModel::SetValue: wrong column" );
        }
    }

    virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const
    {
        // the invisble root node has no parent
        if (!item.IsOk())
            return wxDataViewItem(0);
            
        MyMusicModelNode *node = (MyMusicModelNode*) item.GetID();
        
        // "MyMusic" also has no parent
        if (node == m_root)
            return wxDataViewItem(0);
            
        return wxDataViewItem( (void*) node->GetParent() );
    }

    virtual bool IsContainer( const wxDataViewItem &item ) const
    {
        // the invisble root node can have children (in
        // our model always "MyMusic")
        if (!item.IsOk())
            return true;
    
        MyMusicModelNode *node = (MyMusicModelNode*) item.GetID();
        return node->IsContainer();
    }
    
    virtual wxDataViewItem GetFirstChild( const wxDataViewItem &parent ) const
    {
        MyMusicModelNode *node = (MyMusicModelNode*) parent.GetID();
        if (!node)
            return wxDataViewItem( (void*) m_root );
        
        if (node->GetChildCount() == 0)
            return wxDataViewItem( 0 );
           
        if (node == m_classical)
        {
            MyMusicModel *model = (MyMusicModel*)(const MyMusicModel*) this;
            model->m_classicalMusicIsKnownToControl = true;
        }
        
        MyMusicModelNode *first_child = node->GetChildren().Item( 0 );
        return wxDataViewItem( (void*) first_child );
    }
    
    virtual wxDataViewItem GetNextSibling( const wxDataViewItem &item ) const
    {
        MyMusicModelNode *node = (MyMusicModelNode*) item.GetID();
        
        // "MyMusic" has no siblings in our model
        if (node == m_root)
            return wxDataViewItem(0);

        MyMusicModelNode *parent = node->GetParent();
        int pos = parent->GetChildren().Index( node );
        
        // Something went wrong
        if (pos == wxNOT_FOUND)
            return wxDataViewItem(0);
            
        // No more children
        if (pos == parent->GetChildCount()-1)
            return wxDataViewItem(0);
            
        node = parent->GetChildren().Item( pos+1 );
        return wxDataViewItem( (void*) node );
    } 
    
private:
    MyMusicModelNode*   m_root;
    MyMusicModelNode*   m_pop;
    MyMusicModelNode*   m_classical;
    bool                m_classicalMusicIsKnownToControl;
};

class MyListModel: public wxDataViewIndexListModel
{
public:
    MyListModel() : 
        wxDataViewIndexListModel( 100 )
    {
        unsigned int i;
        for (i = 0; i < 100; i++)
        {
            wxString str;
            str.Printf( "row number %d", i );
            m_array.Add( str );
        }
    }
    
    // helper methods to change the model

    void Prepend( const wxString &text )
    {
        m_array.Insert( text, 0 );
        RowPrepended();
    }

    void DeleteItem( const wxDataViewItem &item )
    {
        unsigned int row = GetRow( item );
        m_array.RemoveAt( row );
        RowDeleted( row );
    }

    // implementation of base class virtuals to define model
    
    virtual unsigned int GetColumnCount() const
    {
        return 2;
    }

    virtual wxString GetColumnType( unsigned int col ) const
    {
        return "string";
    }
    
    virtual unsigned int GetRowCount()
    {
        return m_array.GetCount();
    }
    
    virtual void GetValue( wxVariant &variant, 
                           unsigned int row, unsigned int col ) const
    {
        if (col==0)
        {
            variant = m_array[ row ];
        }
        else
        {
            wxString str;
            str.Printf( "row %d col %d", row, col );
            variant = str;
        }
    }

    virtual bool SetValue( const wxVariant &variant, 
                           unsigned int row, unsigned int col )
    {
        if (col == 0)
        {
            m_array[row] = variant.GetString();
            return true;
        }
    
        return false;
    }
    
    wxArrayString    m_array;
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
    
    void OnAddMozart(wxCommandEvent& event);
    void OnDeleteMusic(wxCommandEvent& event);
    
    void OnPrependList(wxCommandEvent& event);
    void OnDeleteList(wxCommandEvent& event);

    void OnValueChanged( wxDataViewEvent &event );
    void OnItemAdded( wxDataViewEvent &event );
    void OnItemDeleted( wxDataViewEvent &event );
    
    void OnActivated( wxDataViewEvent &event );
    void OnExpanding( wxDataViewEvent &event );
    void OnExpanded( wxDataViewEvent &event );
    void OnCollapsing( wxDataViewEvent &event );
    void OnCollapsed( wxDataViewEvent &event );
    void OnSelected( wxDataViewEvent &event );
    
    void OnEditingStarted( wxDataViewEvent &event );
    void OnEditingDone( wxDataViewEvent &event );
    
    void OnHeaderClick( wxDataViewEvent &event );
    void OnHeaderRightClick( wxDataViewEvent &event );
    void OnSorted( wxDataViewEvent &event );

    void OnRightClick( wxMouseEvent &event );
    void OnGoto( wxCommandEvent &event);

private:
    wxDataViewCtrl* m_musicCtrl;
    wxObjectDataPtr<MyMusicModel> m_music_model;
    
    wxDataViewCtrl* m_listCtrl;
    wxObjectDataPtr<MyListModel> m_list_model;

    wxDataViewColumn * m_col;
    
    wxTextCtrl    * m_log;
    wxLog *m_logOld;

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
        new MyFrame(NULL, wxT("wxDataViewCtrl feature test"), 40, 40, 700, 440);
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
    
    ID_MUSIC_CTRL       = 50,
    
    ID_ADD_MOZART       = 100,
    ID_DELETE_MUSIC     = 101,
     
    ID_PREPEND_LIST     = 200,
    ID_DELETE_LIST      = 201,
    ID_GOTO                  = 202
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU( ID_ABOUT, MyFrame::OnAbout )
    EVT_MENU( ID_EXIT, MyFrame::OnQuit )
    EVT_BUTTON( ID_ADD_MOZART, MyFrame::OnAddMozart )
    EVT_BUTTON( ID_DELETE_MUSIC, MyFrame::OnDeleteMusic )
    EVT_BUTTON( ID_PREPEND_LIST, MyFrame::OnPrependList )
    EVT_BUTTON( ID_DELETE_LIST, MyFrame::OnDeleteList )
    EVT_BUTTON( ID_GOTO, MyFrame::OnGoto)
    
    EVT_DATAVIEW_MODEL_ITEM_ADDED( ID_MUSIC_CTRL, MyFrame::OnItemAdded )
    EVT_DATAVIEW_MODEL_ITEM_DELETED( ID_MUSIC_CTRL, MyFrame::OnItemDeleted )
    EVT_DATAVIEW_MODEL_VALUE_CHANGED( ID_MUSIC_CTRL, MyFrame::OnValueChanged )
    EVT_DATAVIEW_MODEL_ITEM_CHANGED( ID_MUSIC_CTRL, MyFrame::OnValueChanged )
    
    EVT_DATAVIEW_ITEM_ACTIVATED(ID_MUSIC_CTRL, MyFrame::OnActivated )
    EVT_DATAVIEW_ITEM_EXPANDING(ID_MUSIC_CTRL, MyFrame::OnExpanding)
    EVT_DATAVIEW_ITEM_EXPANDED(ID_MUSIC_CTRL, MyFrame::OnExpanded)
    EVT_DATAVIEW_ITEM_COLLAPSING(ID_MUSIC_CTRL, MyFrame::OnCollapsing)
    EVT_DATAVIEW_ITEM_COLLAPSED(ID_MUSIC_CTRL, MyFrame::OnCollapsed)
    EVT_DATAVIEW_ITEM_SELECTED(ID_MUSIC_CTRL, MyFrame::OnSelected)
    
    EVT_DATAVIEW_ITEM_EDITING_STARTED(ID_MUSIC_CTRL, MyFrame::OnEditingStarted)
    EVT_DATAVIEW_ITEM_EDITING_DONE(ID_MUSIC_CTRL, MyFrame::OnEditingDone)
    
    
    EVT_DATAVIEW_COLUMN_HEADER_CLICK(ID_MUSIC_CTRL, MyFrame::OnHeaderClick)
    EVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICKED(ID_MUSIC_CTRL, MyFrame::OnHeaderRightClick)
    EVT_DATAVIEW_COLUMN_SORTED(ID_MUSIC_CTRL, MyFrame::OnSorted)
    
    EVT_RIGHT_UP(MyFrame::OnRightClick)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h):
  wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    m_log = NULL;
    m_col = NULL;

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

    wxBoxSizer *main_sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *data_sizer = new wxBoxSizer( wxHORIZONTAL );

    // MyMusic

    m_musicCtrl = new wxDataViewCtrl( this, ID_MUSIC_CTRL, wxDefaultPosition,
                                    wxDefaultSize, wxDV_MULTIPLE );

    m_music_model = new MyMusicModel;
    m_musicCtrl->AssociateModel( m_music_model.get() );

    wxDataViewColumn *col = m_musicCtrl->AppendTextColumn( "Title", 0, wxDATAVIEW_CELL_INERT, 200, 
                                     DEFAULT_ALIGN, wxDATAVIEW_COL_SORTABLE );
#if 0 
    // Call this and sorting is enabled
    // immediatly upon start up.                                    
    col->SetSortOrder( true );
#endif
    
    m_musicCtrl->AppendTextColumn( "Artist", 1, wxDATAVIEW_CELL_EDITABLE, 200,
                                     DEFAULT_ALIGN, wxDATAVIEW_COL_SORTABLE );
    m_musicCtrl->AppendTextColumn( "Year", 2, wxDATAVIEW_CELL_INERT, 50,
                                     DEFAULT_ALIGN );

    data_sizer->Add( m_musicCtrl, 3, wxGROW );
    
#if 1   
    
    // MyList
    
    m_listCtrl = new wxDataViewCtrl( this, wxID_ANY, wxDefaultPosition,
                                     wxDefaultSize, wxDV_MULTIPLE ); 
    
    m_list_model = new MyListModel;
    m_listCtrl->AssociateModel( m_list_model.get() );
    
    m_listCtrl->AppendTextColumn( "editable string", 0, wxDATAVIEW_CELL_EDITABLE, 120 );
    m_col = m_listCtrl->AppendTextColumn( "index", 1, wxDATAVIEW_CELL_INERT, 120 );
    
    data_sizer->Add( m_listCtrl, 2, wxGROW );
 
#endif   

    main_sizer->Add( data_sizer, 2, wxGROW );
    
    wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );
    
    button_sizer->Add( new wxButton( this, ID_ADD_MOZART, "Add Mozart"), 0, wxALL, 10 );
    button_sizer->Add( new wxButton( this, ID_DELETE_MUSIC, "Delete selected"), 0, wxALL, 10 );
    button_sizer->Add( 10, 10, 1 );
    button_sizer->Add( new wxButton( this, ID_PREPEND_LIST, "Prepend"), 0, wxALL, 10 );
    button_sizer->Add( new wxButton( this, ID_DELETE_LIST, "Delete selected"), 0, wxALL, 10 );
    button_sizer->Add( new wxButton( this, ID_GOTO, "Goto 50"), 0, wxALL, 10 );
    
    main_sizer->Add( button_sizer, 0, wxGROW, 0 );
    
    m_log = new wxTextCtrl( this, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_log));
    wxLogMessage("This is the log window");

    main_sizer->Add( m_log, 1, wxGROW );
    
    SetSizer( main_sizer );
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnAddMozart(wxCommandEvent& WXUNUSED(event) )
{
    m_music_model->AddToClassical( "Kleine Nachtmusik", "Wolfgang Mozart", "1787" );
}

void MyFrame::OnDeleteMusic(wxCommandEvent& WXUNUSED(event) )
{
    wxDataViewItemArray items;
    int len = m_musicCtrl->GetSelections( items );
    for( int i = 0; i < len; i ++ )
        if (items[i].IsOk())
            m_music_model->Delete( items[i] );
}

void MyFrame::OnPrependList( wxCommandEvent& WXUNUSED(event) )
{
    m_list_model->Prepend( "Test" );
}

void MyFrame::OnDeleteList( wxCommandEvent& WXUNUSED(event) )
{
    wxDataViewItemArray items;
    int len = m_listCtrl->GetSelections( items );
    for( int i = 0; i < len; i ++ )
        if (items[i].IsOk())
            m_list_model->DeleteItem( items[i] );
}

void MyFrame::OnItemAdded( wxDataViewEvent &event )
{
    if (!m_log)
        return;
        
    wxLogMessage("wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_ADDED, Item Id: %d",event.GetItem().GetID());
}

void MyFrame::OnItemDeleted( wxDataViewEvent &event )
{
    if (!m_log)
        return;
        
    wxLogMessage( "EVT_DATAVIEW_MODEL_ITEM_DELETED, Item Id: %d", event.GetItem().GetID() );
}

void MyFrame::OnValueChanged( wxDataViewEvent &event )
{
    if (!m_log)
        return;
        
    wxLogMessage( "EVT_DATAVIEW_MODEL_VALUE_CHANGED, Item Id: %d;  Column: %d", event.GetItem().GetID(), event.GetColumn() );
}

void MyFrame::OnActivated( wxDataViewEvent &event )
{
    if(!m_log)
        return;

    wxLogMessage("wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, Item Id: %d;  Column: %d", event.GetItem().GetID(), event.GetColumn());
}

void MyFrame::OnSelected( wxDataViewEvent &event )
{
    if(!m_log)
        return;

    wxLogMessage("wxEVT_COMMAND_DATAVIEW_ITEM_SELECTED, Item Id: %d", event.GetItem().GetID() );
}

void MyFrame::OnExpanding( wxDataViewEvent &event )
{
    if (!m_log)
        return;
        
    wxLogMessage("wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING, Item Id: %d", event.GetItem().GetID() );
}


void MyFrame::OnEditingStarted( wxDataViewEvent &event )
{
    if (!m_log)
        return;
        
    wxLogMessage("wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_STARTED, Item Id: %d", event.GetItem().GetID() );
}

void MyFrame::OnEditingDone( wxDataViewEvent &event )
{
    if (!m_log)
        return;
        
    wxLogMessage("wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_DONE, Item Id: %d", event.GetItem().GetID() );
}

void MyFrame::OnExpanded( wxDataViewEvent &event )
{
    if (!m_log)
        return;
        
    wxLogMessage("wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDED, Item Id: %d", event.GetItem().GetID() );
}

void MyFrame::OnCollapsing( wxDataViewEvent &event )
{
    if (!m_log)
        return;
        
    wxLogMessage("wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSING, Item Id: %d", event.GetItem().GetID() );
}

void MyFrame::OnCollapsed( wxDataViewEvent &event )
{
    if (!m_log)
        return;
        
    wxLogMessage("wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSED, Item Id: %d", event.GetItem().GetID() );
}

void MyFrame::OnHeaderClick( wxDataViewEvent &event )
{
    if(!m_log)
        return;

    wxLogMessage("wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK, Column: %d", event.GetColumn());
}

void MyFrame::OnHeaderRightClick( wxDataViewEvent &event )
{
    if(!m_log)
        return;

    wxLogMessage("wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, Column: %d", event.GetColumn());
}

void MyFrame::OnSorted( wxDataViewEvent &event )
{
    if(!m_log)
        return;

    wxLogMessage("wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED, Column: %d", event.GetColumn());
}

void MyFrame::OnRightClick( wxMouseEvent &event )
{
    if(!m_log)
        return;

    wxLogMessage("wxEVT_MOUSE_RIGHT_UP, Click Point is X: %d, Y: %d", event.GetX(), event.GetY());
}

void MyFrame::OnGoto( wxCommandEvent &event)
{
    wxDataViewItem item = m_list_model->GetItem( 50 );
    m_listCtrl->EnsureVisible(item,m_col);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxAboutDialogInfo info;
    info.SetName(_("DataView sample"));
    info.SetDescription(_("This sample demonstrates the dataview control handling"));
    info.SetCopyright(_T("(C) 2007 Robert Roebling"));

    wxAboutBox(info);
}

