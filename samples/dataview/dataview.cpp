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

// -------------------- wxDataViewControl -------------------- 

// wxDataViewStore

class wxDataViewStore
{
public:
    wxDataViewStore() { }
    virtual ~wxDataViewStore() { }
    
protected:
    DECLARE_NO_COPY_CLASS(wxDataViewStore)
};


// wxDataViewListStoreBase

class wxDataViewListStoreBase: public wxDataViewStore
{
public:
    wxDataViewListStoreBase() { }

    virtual bool AppendRow() = 0;
    
protected:
    DECLARE_NO_COPY_CLASS(wxDataViewListStoreBase)
};


// wxDataViewCtrlBase

class wxDataViewCtrlBase: public wxControl
{
public:
    wxDataViewCtrlBase();

    // Define public API here
    
    virtual bool AppendStringColumn( const wxString &label, int index ) = 0;
    
    virtual bool AssociateStore( wxDataViewStore *store );
    wxDataViewStore* GetStore();
    
private:
    wxDataViewStore     *m_store;

protected:
    DECLARE_NO_COPY_CLASS(wxDataViewCtrlBase)
};


// -------------------- GTK2 header -------------------- 

#ifdef __WXGTK20__

#include "wx/gtk/private.h"

class wxDataViewListStore: public wxDataViewListStoreBase
{
public:
    wxDataViewListStore();
    
    // interface
    virtual bool AppendRow();
    
    // implementation
    GtkListStore* GetGtkListStore() { return m_store; }
    
private:
    GtkListStore  *m_store;

protected:
    DECLARE_NO_COPY_CLASS(wxDataViewListStore)
};

class wxDataViewCtrl: public wxDataViewCtrlBase
{
public:
    wxDataViewCtrl() 
    {
        Init();
    }
    
    wxDataViewCtrl( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator )
    {
        Create(parent, id, pos, size, style, validator );
    }

    virtual ~wxDataViewCtrl();

    void Init();

    bool Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator );

    virtual bool AppendStringColumn( const wxString &label, int index );

    virtual bool AssociateStore( wxDataViewStore *store );
    
    
private:
    DECLARE_DYNAMIC_CLASS(wxDataViewCtrl)
    DECLARE_NO_COPY_CLASS(wxDataViewCtrl)
};

#endif

// -------------------- wxDataViewControl -------------------- 

wxDataViewCtrlBase::wxDataViewCtrlBase() 
{ 
    m_store = NULL;
}

bool wxDataViewCtrlBase::AssociateStore( wxDataViewStore *store )
{
    m_store = store;
    
    return true;
}
    
wxDataViewStore* wxDataViewCtrlBase::GetStore()
{
    return m_store;
}

IMPLEMENT_DYNAMIC_CLASS(wxDataViewCtrl,wxControl)

// -------------------- GTK2 implementaion -------------------- 

#ifdef __WXGTK20__

// wxDataViewListStore

wxDataViewListStore::wxDataViewListStore()
{
    m_store = gtk_list_store_new( 3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING );
}

bool wxDataViewListStore::AppendRow()
{
    GtkTreeIter iter;
    gtk_list_store_append( m_store, &iter );
    
    return true;
}


// wxDataViewCtrl

wxDataViewCtrl::~wxDataViewCtrl()
{
}

void wxDataViewCtrl::Init()
{
}

bool wxDataViewCtrl::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos, const wxSize& size, 
           long style, const wxValidator& validator )
{
    Init();
    
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator ))
    {
        wxFAIL_MSG( wxT("wxDataViewCtrl creation failed") );
        return FALSE;
    }
    
    m_widget = gtk_tree_view_new();
    
    m_parent->DoAddChild( this );

    PostCreation(size);

    return true;
}

bool wxDataViewCtrl::AppendStringColumn( const wxString &label, int index )
{
    GtkCellRenderer *renderer 
        = gtk_cell_renderer_text_new();
    
    GtkTreeViewColumn *column
        = gtk_tree_view_column_new_with_attributes( wxGTK_CONV(label), renderer, "text", index, NULL );

    gtk_tree_view_append_column( GTK_TREE_VIEW(m_widget), column );

    return true;
}

bool wxDataViewCtrl::AssociateStore( wxDataViewStore *store )
{
    wxDataViewCtrlBase::AssociateStore( store );

    // Right now we only have the GTK+ port's
    // list store variant, so cast to that...
    
    wxDataViewListStore *liststore = (wxDataViewListStore*) store;
    
    gtk_tree_view_set_model( GTK_TREE_VIEW(m_widget), GTK_TREE_MODEL(liststore->GetGtkListStore()) );
    
    return true;
}

#endif

// -------------------- wxDataViewControl -------------------- 

class MyApp: public wxApp
{
public:
    bool OnInit(void);
};

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

// ID for the menu commands
#define DYNAMIC_QUIT   wxID_EXIT
#define DYNAMIC_ABOUT  wxID_ABOUT

// Create a new application object
IMPLEMENT_APP  (MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
    // Create the main frame window
    MyFrame *frame = new MyFrame(NULL, _T("Dynamic wxWidgets App"), 50, 50, 450, 340);

    // Show the frame
    frame->Show(true);

    SetTopWindow(frame);

    return true;
}

// -------------------------------------
// MyFrame
// -------------------------------------

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h):
  wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    // Give it an icon
#ifdef __WXMSW__
    SetIcon(wxIcon(_T("mondrian")));
#else
    SetIcon(wxIcon(mondrian_xpm));
#endif

    // Make a menubar
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
    dataview->AppendStringColumn( wxT("first"), 0 );
    dataview->AppendStringColumn( wxT("second"), 1 );
    dataview->AppendStringColumn( wxT("third"), 2 );
    
    wxDataViewListStore *store = new wxDataViewListStore;
    store->AppendRow();
    store->AppendRow();
    store->AppendRow();
    store->AppendRow();
    
    dataview->AssociateStore( store );
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


