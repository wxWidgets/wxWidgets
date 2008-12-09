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
#include "wx/menu.h"

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif

#include "null.xpm"

/* XPM */
static const char *small1_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 6 1",
". c Black",
"o c #FFFFFF",
"X c #000080",
"O c #FFFF00",
"  c None",
"+ c #FF0000",
/* pixels */
"                ",
"                ",
"                ",
"    .......     ",
"    .XXXXX.     ",
"    .oXXXX.     ",
"    .oXXX.......",
".....oXXX.OOOOO.",
".+++.XXXX.oOOOO.",
".o++......oOOOO.",
".o++++.  .oOOOO.",
".o++++.  .OOOOO.",
".+++++.  .......",
".......         ",
"                ",
"                "
};


#define DEFAULT_ALIGN                   wxALIGN_LEFT
#define DATAVIEW_DEFAULT_STYLE          (wxDV_MULTIPLE|wxDV_HORIZ_RULES|wxDV_VERT_RULES)

// -------------------------------------
// MyMusicModel
// -------------------------------------

/*
Implement this data model
            Title               Artist               Year        Judgement
--------------------------------------------------------------------------
1: My Music:
    2:  Pop music
        3:  You are not alone   Michael Jackson      1995        good
        4:  Take a bow          Madonna              1994        good
    5:  Classical music
        6:  Ninth Symphony      Ludwig v. Beethoven  1824        good
        7:  German Requiem      Johannes Brahms      1868        good
*/



class MyMusicModelNode;
WX_DEFINE_ARRAY_PTR( MyMusicModelNode*, MyMusicModelNodes );

class MyMusicModelNode
{
public:
    MyMusicModelNode( MyMusicModelNode* parent,
                      const wxString &title, const wxString &artist, int year )
    {
        m_parent = parent;
        m_title = title;
        m_artist = artist;
        m_year = year;
        m_quality = "good";
        m_isContainer = false;
    }

    MyMusicModelNode( MyMusicModelNode* parent,
                      const wxString &branch )
    {
        m_parent = parent;
        m_title = branch;
        m_year = -1;
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
    int                 m_year;
    wxString            m_quality;

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
        m_root = new MyMusicModelNode( NULL, wxT("My Music" ));
        m_pop = new MyMusicModelNode( m_root, wxT("Pop music") );
        m_root->Append( m_pop );
        m_pop->Append( new MyMusicModelNode( m_pop,
            wxT("You are not alone"), wxT("Michael Jackson"), 1995 ) );
        m_pop->Append( new MyMusicModelNode( m_pop,
            wxT("Take a bow"), wxT("Madonna"), 1994 ) );
        m_classical = new MyMusicModelNode( m_root, wxT("Classical music") );
        m_root->Append( m_classical );
        m_ninth = new MyMusicModelNode( m_classical,
            wxT("Ninth symphony"), wxT("Ludwig van Beethoven"), 1824 );
        m_classical->Append( m_ninth );
        m_classical->Append( new MyMusicModelNode( m_classical,
            wxT("German Requiem"), wxT("Johannes Brahms"), 1868 ) );
        m_classicalMusicIsKnownToControl = false;
    }

    ~MyMusicModel()
    {
        delete m_root;
    }

    // helper method for wxLog

    wxString GetTitle( const wxDataViewItem &item ) const
    {
        MyMusicModelNode *node = (MyMusicModelNode*) item.GetID();
        if (!node)
            return wxEmptyString;

        return node->m_title;
    }

    int GetYear( const wxDataViewItem &item ) const
    {
        MyMusicModelNode *node = (MyMusicModelNode*) item.GetID();
        if (!node)
            return 2000;

        return node->m_year;
    }

    // helper methods to change the model

    void AddToClassical( const wxString &title, const wxString &artist, int year )
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

        node->GetParent()->GetChildren().Remove( node );
        delete node;

        // notify control
        ItemDeleted( parent, item );
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
            wxUIntPtr litem1 = (wxUIntPtr) item1.GetID();
            wxUIntPtr litem2 = (wxUIntPtr) item2.GetID();

            return litem1-litem2;
        }

        return wxDataViewModel::Compare( item1, item2, column, ascending );
    }

    // implementation of base class virtuals to define model

    virtual unsigned int GetColumnCount() const
    {
        return 6;
    }

    virtual wxString GetColumnType( unsigned int col ) const
    {
        if (col == 2)
            return wxT("long");

        return wxT("string");
    }

    virtual void GetValue( wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col ) const
    {
        MyMusicModelNode *node = (MyMusicModelNode*) item.GetID();
        switch (col)
        {
            case 0: variant = node->m_title; break;
            case 1: variant = node->m_artist; break;
            case 2: variant = (long) node->m_year; break;
            case 3: variant = node->m_quality; break;
            case 4:
               // wxMac doesn't conceal the popularity progress renderer, return 0 for containers
               if (IsContainer(item))
                  variant = (long) 0;
               else
                  variant = (long) 80;  // all music is very 80% popular
               break;
            case 5:
               // Make size of red square depend on year
               if (GetYear(item) < 1900)
                  variant = (long) 35;
               else
                  variant = (long) 25;
               break;
            default:
            {
                wxLogError( wxT("MyMusicModel::GetValue: wrong column %d"), col );

                // provoke a crash when mouse button down
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
            case 0: node->m_title = variant.GetString(); return true;
            case 1: node->m_artist  = variant.GetString(); return true;
            case 2: node->m_year  = variant.GetLong(); return true;
            case 3: node->m_quality  = variant.GetString(); return true;
            default: wxLogError( wxT("MyMusicModel::SetValue: wrong column") );
        }
        return false;
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

    virtual unsigned int GetChildren( const wxDataViewItem &parent, wxDataViewItemArray &array ) const
    {
        MyMusicModelNode *node = (MyMusicModelNode*) parent.GetID();
        if (!node)
        {
            array.Add( wxDataViewItem( (void*) m_root ) );
            return 1;
        }

        if (node == m_classical)
        {
            MyMusicModel *model = (MyMusicModel*)(const MyMusicModel*) this;
            model->m_classicalMusicIsKnownToControl = true;
        }

        if (node->GetChildCount() == 0)
        {
            return 0;
        }

        unsigned int count = node->GetChildren().GetCount();
        unsigned int pos;
        for (pos = 0; pos < count; pos++)
        {
            MyMusicModelNode *child = node->GetChildren().Item( pos );
            array.Add( wxDataViewItem( (void*) child ) );
        }
        return count;
    }

    // DnD

    virtual bool IsDraggable( const wxDataViewItem &item )
        {
            // only drag items
            return (!IsContainer(item));
        }

    virtual size_t GetDragDataSize( const wxDataViewItem &item, const wxDataFormat &WXUNUSED(format) )
        {
            wxPrintf( "GetDragDataSize\n" );

            MyMusicModelNode *node = (MyMusicModelNode*) item.GetID();
            wxString data;
            data += node->m_title; data += wxT(" ");
            data += node->m_artist;
            return strlen( data.utf8_str() ) + 1;
        }
    virtual bool GetDragData( const wxDataViewItem &item, const wxDataFormat &WXUNUSED(format),
                              void* dest, size_t WXUNUSED(size) )
        {
            wxPrintf( "GetDragData\n" );

            MyMusicModelNode *node = (MyMusicModelNode*) item.GetID();
            wxString data;
            data += node->m_title; data += wxT(" ");
            data += node->m_artist;
            wxCharBuffer buffer( data.utf8_str() );
            memcpy( dest, buffer, strlen(buffer)+1 );
            return true;
        }
    
    wxDataViewItem GetNinthItem()
    {
       return wxDataViewItem( m_ninth );
    }

private:
    MyMusicModelNode*   m_root;
    MyMusicModelNode*   m_pop;
    MyMusicModelNode*   m_classical;
    MyMusicModelNode*   m_ninth;
    bool                m_classicalMusicIsKnownToControl;
};


static int my_sort_reverse( int *v1, int *v2 )
{
   return *v2-*v1;
}

static int my_sort( int *v1, int *v2 )
{
   return *v1-*v2;
}

class MyListModel: public wxDataViewVirtualListModel
{
public:
    MyListModel() :
#ifdef __WXMAC__
        wxDataViewVirtualListModel( 1000 + 100 )
#else
        wxDataViewVirtualListModel( 100000 + 100 )
#endif
    {
#ifdef __WXMAC__
        m_virtualItems = 1000;
#else
        m_virtualItems = 100000;
#endif

        unsigned int i;
        for (i = 0; i < 100; i++)
        {
            wxString str;
            str.Printf( wxT("row number %d"), i );
            m_array.Add( str );
        }

        m_icon = wxIcon( null_xpm );
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
        if (row >= m_array.GetCount())
           return;

        m_array.RemoveAt( row );
        RowDeleted( row );
    }

    void DeleteItems( const wxDataViewItemArray &items )
    {
        wxArrayInt rows;
        unsigned int i;
        for (i = 0; i < items.GetCount(); i++)
        {
            unsigned int row = GetRow( items[i] );
            if (row < m_array.GetCount())
                rows.Add( row );
        }

        // Sort in descending order so that the last
        // row will be deleted first. Otherwise the
        // remaining indeces would all be wrong.
        rows.Sort( my_sort_reverse );
        for (i = 0; i < rows.GetCount(); i++)
            m_array.RemoveAt( rows[i] );

        // This is just to test if wxDataViewCtrl can
        // cope with removing rows not sorted in
        // descending order
        rows.Sort( my_sort );
        RowsDeleted( rows );
    }

    void AddMany()
    {
        m_virtualItems += 1000;
        Reset( m_array.GetCount() + m_virtualItems );
    }

    // implementation of base class virtuals to define model

    virtual unsigned int GetColumnCount() const
    {
        return 3;
    }

    virtual wxString GetColumnType( unsigned int col ) const
    {
        if (col == 1)
            return wxT("wxDataViewIconText");

        return wxT("string");
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
            if (row >= m_array.GetCount())
            {
                wxString str;
                str.Printf(wxT("row %d"), row - m_array.GetCount() );
                variant = str;
            }
            else
            {
                variant = m_array[ row ];
            }
        } else
        if (col==1)
        {
            wxDataViewIconText data( wxT("test"), m_icon );
            variant << data;
        } else
        if (col==2)
        {
            if (row >= m_array.GetCount())
                variant = wxT("plain");
            else
                variant = wxT("blue");
        }
    }

    virtual bool GetAttr( unsigned int row, unsigned int col, wxDataViewItemAttr &attr )
    {
        if (col != 2)
            return false;

        if (row < m_array.GetCount())
        {
            attr.SetColour( *wxBLUE );
            attr.SetItalic( true );
        }

        return true;
    }

    virtual bool SetValue( const wxVariant &variant,
                           unsigned int row, unsigned int col )
    {
        if (col == 0)
        {
            if (row >= m_array.GetCount())
               return false;

            m_array[row] = variant.GetString();
            return true;
        }

        return false;
    }

    wxArrayString    m_array;
    wxIcon           m_icon;
    int              m_virtualItems;
};

// -------------------------------------
// MyCustomRenderer
// -------------------------------------

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
                           wxDataViewModel *WXUNUSED(model), const wxDataViewItem &WXUNUSED(item), unsigned int WXUNUSED(col) )
    {
        wxLogMessage( wxT("MyCustomRenderer Activate()") );
        return false;
    }

    virtual bool LeftClick( wxPoint cursor, wxRect WXUNUSED(cell),
                           wxDataViewModel *WXUNUSED(model), const wxDataViewItem &WXUNUSED(item), unsigned int WXUNUSED(col) )
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
        new MyFrame(NULL, wxT("wxDataViewCtrl feature test"), 40, 40, 1000, 540);
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

    EVT_RIGHT_UP(MyFrame::OnRightClick)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, const wxString &title, int x, int y, int w, int h):
  wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    m_log = NULL;
    m_col = NULL;

    SetIcon(wxICON(sample));

    // build the menus:

    wxMenu *file_menu = new wxMenu;
    file_menu->Append(ID_ABOUT, wxT("&About"));
    file_menu->AppendSeparator();
    file_menu->Append(ID_EXIT, wxT("E&xit"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, wxT("&File"));

    SetMenuBar(menu_bar);
    CreateStatusBar();
    
    wxPanel *panel = new wxPanel( this, -1 );

    wxBoxSizer *main_sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *data_sizer = new wxBoxSizer( wxHORIZONTAL );

    // MyMusic

    m_musicCtrl = new wxDataViewCtrl( panel, ID_MUSIC_CTRL, wxDefaultPosition,
                                    wxSize(400,200), wxDV_MULTIPLE|wxDV_VARIABLE_LINE_HEIGHT );

    m_music_model = new MyMusicModel;
    m_musicCtrl->AssociateModel( m_music_model.get() );

    wxDataViewTextRenderer *tr = new wxDataViewTextRenderer( wxT("string"), wxDATAVIEW_CELL_INERT );
    wxDataViewColumn *column0 = new wxDataViewColumn( wxT("title"), tr, 0, 200, wxALIGN_LEFT,
        wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE );
    m_musicCtrl->AppendColumn( column0 );
#if 0
    // Call this and sorting is enabled
    // immediatly upon start up.
    column0->SetAsSortKey();
#endif

    tr = new wxDataViewTextRenderer( wxT("string"), wxDATAVIEW_CELL_EDITABLE );
    wxDataViewColumn *column1 = new wxDataViewColumn( wxT("artist"), tr, 1, 150, wxALIGN_LEFT,
        wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE );
    column1->SetMinWidth(150); // this column can't be resized to be smaller
    m_musicCtrl->AppendColumn( column1 );

    wxDataViewSpinRenderer *sr = new wxDataViewSpinRenderer( 0, 2010, wxDATAVIEW_CELL_EDITABLE, wxALIGN_RIGHT );
    wxDataViewColumn *column2 = new wxDataViewColumn( wxT("year"), sr, 2, 60, wxALIGN_LEFT,
        wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE );
    m_musicCtrl->AppendColumn( column2 );

    wxArrayString choices;
    choices.Add( "good" );
    choices.Add( "bad" );
    choices.Add( "lousy" );
    wxDataViewChoiceRenderer *c = new wxDataViewChoiceRenderer( choices, wxDATAVIEW_CELL_EDITABLE, wxALIGN_RIGHT );
    wxDataViewColumn *column3 = new wxDataViewColumn( wxT("rating"), c, 3, 100, wxALIGN_LEFT,
        wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE );
    m_musicCtrl->AppendColumn( column3 );

    m_musicCtrl->AppendProgressColumn( wxT("popularity"), 4, wxDATAVIEW_CELL_INERT, 80 );

    MyCustomRenderer *cr = new MyCustomRenderer( wxDATAVIEW_CELL_ACTIVATABLE, wxALIGN_RIGHT );
    wxDataViewColumn *column4 = new wxDataViewColumn( wxT("custom"), cr, 5, -1, wxALIGN_LEFT,
        wxDATAVIEW_COL_RESIZABLE );
    m_musicCtrl->AppendColumn( column4 );

    data_sizer->Add( m_musicCtrl, 3, wxGROW );

    // MyList

    m_listCtrl = new wxDataViewCtrl( panel, wxID_ANY, wxDefaultPosition,
                                     wxSize(400,200), wxDV_MULTIPLE | wxDV_ROW_LINES);

    m_list_model = new MyListModel;
    m_listCtrl->AssociateModel( m_list_model.get() );

#if 1
    m_listCtrl->AppendTextColumn    (wxT("editable string"), 0, wxDATAVIEW_CELL_EDITABLE, 120 );
    m_listCtrl->AppendIconTextColumn(wxIcon(small1_xpm), 1, wxDATAVIEW_CELL_INERT )->SetTitle( wxT("icon") );
#else
    m_listCtrl->AppendTextColumn    (wxT("editable string"), 0, wxDATAVIEW_CELL_EDITABLE );
    m_listCtrl->AppendIconTextColumn(wxT("icon"),            1, wxDATAVIEW_CELL_INERT );
#endif

    wxDataViewTextRendererAttr *ra = new wxDataViewTextRendererAttr;
    wxDataViewColumn *column5 = new wxDataViewColumn(wxT("attributes"), ra, 2 );
    m_listCtrl->AppendColumn( column5 );

    data_sizer->Add( m_listCtrl, 2, wxGROW );

    main_sizer->Add( data_sizer, 2, wxGROW );

    wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );

    button_sizer->Add( new wxButton( panel, ID_ADD_MOZART,  _("Add Mozart")),             0, wxALL, 10 );
    button_sizer->Add( new wxButton( panel, ID_DELETE_MUSIC,_("Delete selected")),        0, wxALL, 10 );
    button_sizer->Add( new wxButton( panel, ID_DELETE_YEAR, _("Delete \"Year\" column")), 0, wxALL, 10 );
    button_sizer->Add( new wxButton( panel, ID_SELECT_NINTH, _("Select Ninth")), 0, wxALL, 10 );
    button_sizer->Add( 10, 10, 1 );
    wxFlexGridSizer *grid_sizer = new wxFlexGridSizer( 2, 2 );
    grid_sizer->Add( new wxButton( panel, ID_PREPEND_LIST,_("Prepend")),         0, wxALL, 2 );
    grid_sizer->Add( new wxButton( panel, ID_DELETE_LIST, _("Delete selected")), 0, wxALL, 2 );
    grid_sizer->Add( new wxButton( panel, ID_GOTO,        _("Goto 50")),         0, wxALL, 2 );
    grid_sizer->Add( new wxButton( panel, ID_ADD_MANY,    _("Add 1000")),        0, wxALL, 2 );
    button_sizer->Add( grid_sizer, 0, wxALL, 10 );

    main_sizer->Add( button_sizer, 0, wxGROW, 0 );

    wxBoxSizer *bottom_sizer = new wxBoxSizer( wxHORIZONTAL );

    m_log = new wxTextCtrl( panel, -1, wxString(), wxDefaultPosition, wxSize(100,200), wxTE_MULTILINE );
    m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_log));
    wxLogMessage(_("This is the log window"));

    bottom_sizer->Add( m_log, 1, wxGROW );

#if 1
    // wxDataViewTreeStore

    wxDataViewCtrl *treectrl = new wxDataViewCtrl( panel, -1,
        wxDefaultPosition, wxSize(100,200), wxDV_NO_HEADER );

    wxDataViewTreeStore *store = new wxDataViewTreeStore;
    wxDataViewItem parent = store->AppendContainer( wxDataViewItem(0),wxT("Root 1"), wxIcon(small1_xpm) );
    wxDataViewItem child = store->AppendItem( parent,wxT("Child 1"), wxIcon(small1_xpm) );
    child = store->AppendItem( parent,wxT("Child 2"), wxIcon(small1_xpm) );
    child = store->AppendItem( parent,wxT("Child 3, very long, long, long, long"), wxIcon(small1_xpm) );
    treectrl->AssociateModel( store );
    store->DecRef();

    treectrl->AppendIconTextColumn( wxT("no label"), 0, wxDATAVIEW_CELL_INERT, -1, (wxAlignment) 0,
        wxDATAVIEW_COL_RESIZABLE );

    bottom_sizer->Add( treectrl, 1 );

    // wxDataViewTreeCtrl

    wxDataViewTreeCtrl *treectrl2 = new wxDataViewTreeCtrl( panel, -1, wxDefaultPosition, wxSize(100,200) );

    wxImageList *ilist = new wxImageList( 16, 16 );
    ilist->Add( wxIcon(small1_xpm) );
    treectrl2->SetImageList( ilist );

    parent = treectrl2->AppendContainer( wxDataViewItem(0),wxT("Root 1"), 0 );
    child = treectrl2->AppendItem( parent,wxT("Child 1"), 0 );
    child = treectrl2->AppendItem( parent,wxT("Child 2"), 0 );
    child = treectrl2->AppendItem( parent,wxT("Child 3, very long, long, long, long"), 0 );

    bottom_sizer->Add( treectrl2, 1 );
#endif

    // main sizer

    main_sizer->Add( bottom_sizer, 0, wxGROW );

    panel->SetSizer( main_sizer );
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
    int len = m_musicCtrl->GetSelections( items );
    for( int i = 0; i < len; i ++ )
        if (items[i].IsOk())
            m_music_model->Delete( items[i] );
}

void MyFrame::OnDeleteYear( wxCommandEvent& WXUNUSED(event) )
{
    m_musicCtrl->DeleteColumn( m_musicCtrl->GetColumn( 2 ) );
    FindWindow( ID_DELETE_YEAR )->Disable();
}

void MyFrame::OnSelectNinth( wxCommandEvent& WXUNUSED(event) )
{
    m_musicCtrl->Select( m_music_model->GetNinthItem() );
}

void MyFrame::OnPrependList( wxCommandEvent& WXUNUSED(event) )
{
    m_list_model->Prepend(wxT("Test"));
}

void MyFrame::OnDeleteList( wxCommandEvent& WXUNUSED(event) )
{
    wxDataViewItemArray items;
    int len = m_listCtrl->GetSelections( items );
    if (len > 0)
        m_list_model->DeleteItems( items );
}

void MyFrame::OnValueChanged( wxDataViewEvent &event )
{
    if (!m_log)
        return;

    wxLogMessage( wxT("EVT_DATAVIEW_ITEM_VALUE_CHANGED, Item Id: %d;  Column: %d"), event.GetItem().GetID(), event.GetColumn() );
}

void MyFrame::OnActivated( wxDataViewEvent &event )
{
    if(!m_log)
        return;

    wxString title = m_music_model->GetTitle( event.GetItem() );
    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, Item: %s"), title );
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

    m_musicCtrl->PopupMenu(&menu);

//    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, Item: %s Value: %s"),title.GetData(), event.GetValue().GetString());
}

void MyFrame::OnHeaderClick( wxDataViewEvent &event )
{
    // we need to skip the event to let the default behaviour of sorting by
    // this column when it is clicked to take place
    event.Skip();

    if(!m_log)
        return;

    int pos = m_musicCtrl->GetColumnPosition( event.GetDataViewColumn() );

    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK, Column position: %d"), pos );
}

void MyFrame::OnHeaderRightClick( wxDataViewEvent &event )
{
    if(!m_log)
        return;

    int pos = m_musicCtrl->GetColumnPosition( event.GetDataViewColumn() );

    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, Column position: %d"), pos );
}

void MyFrame::OnSorted( wxDataViewEvent &event )
{
    if(!m_log)
        return;

    int pos = m_musicCtrl->GetColumnPosition( event.GetDataViewColumn() );

    wxLogMessage(wxT("wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED, Column position: %d"), pos );
}

void MyFrame::OnRightClick( wxMouseEvent &event )
{
    if(!m_log)
        return;

    wxLogMessage(wxT("wxEVT_MOUSE_RIGHT_UP, Click Point is X: %d, Y: %d"), event.GetX(), event.GetY());
}

void MyFrame::OnGoto(wxCommandEvent& WXUNUSED(event))
{
    wxDataViewItem item = m_list_model->GetItem( 50 );
    m_listCtrl->EnsureVisible(item,m_col);
}

void MyFrame::OnAddMany(wxCommandEvent& WXUNUSED(event))
{
    m_list_model->AddMany();
}


void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxAboutDialogInfo info;
    info.SetName(_("DataView sample"));
    info.SetDescription(_("This sample demonstrates the dataview control handling"));
    info.SetCopyright(_T("(C) 2007 Robert Roebling"));

    wxAboutBox(info);
}

