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
        node->GetParent()->GetChildren().Remove( node );
        delete node;
        
        // notify control
        ItemDeleted( item );
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
            default: wxLogError( "MyMusicModel::GetValue: wrong column" );
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

    virtual bool HasChildren( const wxDataViewItem &item ) const
    {
        MyMusicModelNode *node = (MyMusicModelNode*) item.GetID();
        if (!node)
            return true;
    
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
        MyMusicModelNode *parent = node->GetParent();
        if (!parent)
            return wxDataViewItem(0);

        int pos = parent->GetChildren().Index( node );
        if (pos == wxNOT_FOUND)
            return wxDataViewItem(0);
            
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
    void OnAdd(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);

private:
    wxDataViewCtrl* m_dataview;
    wxTextCtrl    * m_log;
    wxObjectDataPtr<MyMusicModel> m_model;

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
        new MyFrame(NULL, wxT("wxDataViewCtrl feature test"), 10, 10, 700, 440);
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
    
    ID_ADD = 100,
    ID_DELETE = 101,
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU( ID_ABOUT, MyFrame::OnAbout )
    EVT_MENU( ID_EXIT, MyFrame::OnQuit )
    EVT_BUTTON( ID_ADD, MyFrame::OnAdd )
    EVT_BUTTON( ID_DELETE, MyFrame::OnDelete )
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

    wxBoxSizer *main_sizer = new wxBoxSizer( wxVERTICAL );

    m_dataview = new wxDataViewCtrl( this, wxID_ANY, wxDefaultPosition,
                                     wxDefaultSize );

    m_model = new MyMusicModel;
    m_dataview->AssociateModel( m_model.get() );

    m_dataview->AppendTextColumn( "Title", 0, wxDATAVIEW_CELL_INERT, 200, 
                                     DEFAULT_ALIGN, wxDATAVIEW_COL_SORTABLE );
    m_dataview->AppendTextColumn( "Artist", 1, wxDATAVIEW_CELL_INERT, 200,
                                     DEFAULT_ALIGN, wxDATAVIEW_COL_SORTABLE );
    m_dataview->AppendTextColumn( "Year", 2, wxDATAVIEW_CELL_INERT, 50,
                                     DEFAULT_ALIGN );

    main_sizer->Add( m_dataview, 2, wxGROW );
    
    wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );
    
    button_sizer->Add( new wxButton( this, ID_ADD, "Add Mozart"), 0, wxALL, 10 );
    button_sizer->Add( new wxButton( this, ID_DELETE, "Delete selected"), 0, wxALL, 10 );
    
    main_sizer->Add( button_sizer, 0, 0, 0 );
    
    m_log = new wxTextCtrl( this, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    
    main_sizer->Add( m_log, 1, wxGROW );
    
    SetSizer( main_sizer );
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnDelete(wxCommandEvent& WXUNUSED(event) )
{
    wxDataViewItem item = m_dataview->GetSelection();
    if (item.IsOk())
        m_model->Delete( item );
}

void MyFrame::OnAdd(wxCommandEvent& WXUNUSED(event) )
{
    m_model->AddToClassical( "Kleine Nachtmusik", "Wolfgang Mozart", "1787" );
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxAboutDialogInfo info;
    info.SetName(_("DataView sample"));
    info.SetDescription(_("This sample demonstrates the dataview control handling"));
    info.SetCopyright(_T("(C) 2007 Robert Roebling"));

    wxAboutBox(info);
}

