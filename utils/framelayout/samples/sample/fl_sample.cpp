/////////////////////////////////////////////////////////////////////////////
// Name:        main.cpp
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     24/11/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:       wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "fl_sample.cpp"
#pragma interface "fl_sample.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "controlbar.h"
#include "objstore.h"

// plugins used
#include "barhintspl.h"
#include "hintanimpl.h"

#include "wx/textctrl.h"

// ADDED by alex (linker complaints...):
#ifndef wxDUMMY_OBJ_INCLUDED
char wxDummyChar=0;
#endif

#define ID_LOAD  102
#define ID_STORE 103
#define ID_QUIT  104

#define LAYOUT_FILE "layouts.dat"

class MyApp: public wxApp
{ 
public:
    bool OnInit(void);
};

class MyFrame: public wxFrame
{
protected:
    wxFrameLayout* mpLayout;
    wxWindow*      mpClientWnd;
	wxPanel*       mpInternalFrm;

    void SerializeMe( wxObjectStorage& store );

    wxTextCtrl* CreateTextCtrl( const wxString& value );


public:
    MyFrame( wxWindow* parent, char *title );
    ~MyFrame();
    
    void OnLoad( wxCommandEvent& event );
	void OnStore( wxCommandEvent& event );
    void OnQuit( wxCommandEvent& event );

    bool OnClose(void) { return TRUE; }

    DECLARE_EVENT_TABLE()
};

/***** Implementation for class MyApp *****/

IMPLEMENT_APP    (MyApp)

bool MyApp::OnInit(void)
{
    // wxWindows boiler-plate:

    MyFrame *frame = new MyFrame(NULL, "wxFrameLayout sample");

    wxMenu *file_menu = new wxMenu;

    file_menu->Append( ID_LOAD,  "&Load layout"  );
    file_menu->Append( ID_STORE, "&Store layout"  );
    file_menu->AppendSeparator();

    file_menu->Append( ID_QUIT, "E&xit" );

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, "&File");

    frame->CreateStatusBar(3);
    frame->SetMenuBar(menu_bar);

    frame->Show(TRUE);

    SetTopWindow(frame);

    return TRUE;
}

/***** Immlementation for class MyFrame *****/

BEGIN_EVENT_TABLE(MyFrame, wxFrame)

    EVT_MENU( ID_LOAD,  MyFrame::OnLoad  )
    EVT_MENU( ID_STORE, MyFrame::OnStore )
    EVT_MENU( ID_QUIT,  MyFrame::OnQuit  )

END_EVENT_TABLE()

MyFrame::MyFrame( wxWindow* parent, char *title )
    
	: wxFrame( parent, -1, "NewTest-II", wxDefaultPosition,
			   wxSize( 700, 500 ),
			   wxCLIP_CHILDREN | wxMINIMIZE_BOX | wxMAXIMIZE_BOX |
			   wxTHICK_FRAME   | wxSYSTEM_MENU  | wxCAPTION,
			   "freimas" )
{
#ifdef __WXMSW__
	mpInternalFrm = (wxPanel*)this;
#else
	mpInternalFrm = new wxPanel( this, -1 );	
#endif
	
    mpClientWnd = CreateTextCtrl( "Client window" );

	// btw, creation of internal frame is needed for wxGtk version
	// to act correctly (since menu-bar is a separate window there..)
    
    mpLayout = new wxFrameLayout( mpInternalFrm, mpClientWnd );

#ifdef __WXGTK__

	// real-time dosn't work well under wxGtk yet
	cbCommonPaneProperties props;
	mpLayout->GetPaneProperties( props );

	props.mRealTimeUpdatesOn = FALSE; // off
	
	mpLayout->SetPaneProperties( props, wxALL_PANES );
	
#endif
	
	mpLayout->PushDefaultPlugins();
	mpLayout->AddPlugin( CLASSINFO( cbBarHintsPlugin ) ); // facny "X"es and beveal for barso
	//mpLayout->AddPlugin( CLASSINFO( cbHintAnimationPlugin ) );

    cbDimInfo sizes( 80,65, // when docked horizontally      
                     80,65,    // when docked vertically        
                     80,30, // when floated                  
                     TRUE,  // the bar is fixed-size
                     5,     // vertical gap (bar border)
                     5      // horizontal gap (bar border)
                   ); 

    // drop-in 20 bars

    for( int i = 1; i <= 10; ++i )
    {
        char buf[4];
		sprintf( buf, "%d", i );
        wxString name = wxString("Bar-");
		name += buf;
    
        sizes.mIsFixed = i % 5 > 0; // every fifth bar is not fixed-size

        if ( !sizes.mIsFixed ) name += " (flexible)";

        mpLayout->AddBar( CreateTextCtrl(name),// bar window
                          sizes, i % MAX_PANES,// alignment ( 0-top,1-bottom, etc)
                          0,                   // insert into 0th row (vert. position)
                          0,                   // offset from the start of row (in pixels)
                          name                 // name to refere in customization pop-ups
                       );
    }
}

MyFrame::~MyFrame()
{
    // layout is not a window, should be released manually

    if ( mpLayout ) delete mpLayout;
}

wxTextCtrl* MyFrame::CreateTextCtrl( const wxString& value )
{
    wxTextCtrl* pCtrl = 
			
		new wxTextCtrl( mpInternalFrm, -1, value, 
                        wxPoint(0,0), wxSize(1,1), wxTE_MULTILINE );

	pCtrl->SetBackgroundColour( wxColour( 255,255,255 ) );

	return pCtrl;
}

void MyFrame::OnLoad( wxCommandEvent& event )
{
    if ( !wxFileExists( LAYOUT_FILE ) )
    {               
        wxMessageBox( "layout data file `layout.dat' not found\n\n store layout first" );

        return;
    }

    mpLayout->HideBarWindows();       // hide first, to avoid flickered destruction
    mpLayout->DestroyBarWindows();

	if ( mpClientWnd )
	{
    	mpClientWnd->Destroy();
    	delete mpLayout;

		mpClientWnd = NULL;
	}

    wxIOStreamWrapper stm;
    stm.CreateForInput( LAYOUT_FILE ); // TRUE - create stream for input

    wxObjectStorage store( stm );

    SerializeMe( store );

    mpLayout->Activate();
}

void MyFrame::OnStore( wxCommandEvent& event )
{
    wxIOStreamWrapper stm;
    stm.CreateForOutput( LAYOUT_FILE ); // FALSE - create stream for output

    wxObjectStorage store( stm );

    SerializeMe( store );
}

void MyFrame::OnQuit( wxCommandEvent& event )
{
    Show( FALSE ); // TRICK:: hide it, to avoid flickered destruction

    Close(TRUE);
}

void MyFrame::SerializeMe( wxObjectStorage& store )
{
	// mark contaienr-frames as not serializable
		
	store.AddInitialRef( mpInternalFrm );
	store.AddInitialRef( this );

	// does all the rest for as

    store.XchgObjPtr( (wxObject**) &(mpLayout) );
	store.XchgObjPtr( (wxObject**) &(mpClientWnd) );

	store.Finalize(); // finish serialization
}

#ifdef __HACK_MY_MSDEV40__

////////////// new 2.0-magic (linker errors...) ////////////////

wxToolBar* wxFrame::CreateToolBar(long style, wxWindowID id, const wxString& name)
{
    wxCHECK_MSG( m_frameToolBar == NULL, FALSE,
               "recreating toolbar in wxFrame" );

    wxToolBar* toolBar = OnCreateToolBar(style, id, name);
    if (toolBar)
    {
        SetToolBar(toolBar);
        PositionToolBar();
        return toolBar;
    }
    else
    {
        return NULL;
    }
}

wxToolBar* wxFrame::OnCreateToolBar(long style, wxWindowID id, const wxString& name)
{
    return new wxToolBar(this, id, wxDefaultPosition, wxDefaultSize, style, name);
}

#endif
