/////////////////////////////////////////////////////////////////////////////
// Name:        minimal.cpp
// Purpose:     Minimal wxWindows sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "minimal.cpp"
#pragma interface "minimal.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

/*
#ifdef __BORLANDC__
#pragma hdrstop
#endif
*/

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/textctrl.h"

#include "controlbar.h"     // core API
#include "fl_test.h"

// extra plugins
#include "barhintspl.h"    // beveal for bars with "X"s and grooves
#include "rowdragpl.h"     // NC-look with dragable rows
#include "cbcustom.h"      // customization plugin
#include "hintanimpl.h"

// beuty-care
#include "gcupdatesmgr.h"  // smooth d&d
#include "antiflickpl.h"   // double-buffered repaint of decorations

#include "dyntbar.h"       // auto-layouting toolbar
#include "dyntbarhnd.h"    // control-bar dimension handler for it

// comment it out if it breaks, (this is my workaround for MSDev 4.0 linker)

#ifndef wxDUMMY_OBJ_INCLUDED
char  wxDummyChar;
#endif


IMPLEMENT_APP	(MyApp)

bool MyApp::OnInit(void)
{
	MyFrame *frame = new MyFrame(NULL);

	frame->SetBackgroundColour( wxColour(192,192,192) );

	wxMenu *file_menu = new wxMenu;

	file_menu->Append( NEW_TEST_EXIT, "E&xit" );

	wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu,   "&File");

	frame->SetMenuBar(menu_bar);

	frame->CreateStatusBar(3);

	frame->Show(TRUE);

	frame->mpClientWnd->Refresh();

	SetTopWindow(frame);

	return TRUE;

	/*
	wxMessageBox("Hello, this demo has a bunch of yet-not-fixed-bugs and misssing functionality\n\
The ONLY purpose is to demostrate self-layouting toolbars,\n flat-bitmapped-buttons and 2-new FL-plugins\
 (cbRowDragPlugin & cbBarHintsPlugin)\n\n\
BTW, disabled images and label-text are rendered at run-time" );
*/


	return TRUE;
}

/***** Implementation for class MyFrame *****/

BEGIN_EVENT_TABLE( MyFrame, wxFrame )

// EVT_CHAR_HOOK(MyFrame::OnKeyDown)
//	EVT_PAINT( MyFrame::OnPaint )
	EVT_MENU( NEW_TEST_EXIT, MyFrame::OnExit )

END_EVENT_TABLE()

void MyFrame::OnExit( wxCommandEvent& event )
{
	Destroy();
}

wxTextCtrl* MyFrame::CreateTextCtrl( const wxString& value )
{
	wxTextCtrl* pCtrl = 
			
    	new wxTextCtrl( mpInternalFrm, -1, value, 
                           wxDefaultPosition, wxSize(0,0), wxTE_MULTILINE );

	pCtrl->SetBackgroundColour( wxColour( 255,255,255 ) );

	return pCtrl;
}

MyFrame::MyFrame(wxFrame *frame)

	: wxFrame( frame, -1, "wxWindows 2.0 wxFrameLayout Test Application", wxDefaultPosition, 
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


    mpLayout = new wxFrameLayout( mpInternalFrm, mpClientWnd );


#ifdef __WXGTK__

	cbCommonPaneProperties props;
	mpLayout->GetPaneProperties( props );

	props.mRealTimeUpdatesOn = FALSE; // real-time OFF!!!

	mpLayout->SetPaneProperties( props, wxALL_PANES );

#endif
	
	mpLayout->SetUpdatesManager( new cbGCUpdatesMgr() );

	// this is now default...
	//mpLayout->SetMargins( 1,1,1,1 ); // gaps for vertical/horizontal/right/left panes

	// setup plugins for testing

	mpLayout->PushDefaultPlugins();

	mpLayout->AddPlugin( CLASSINFO( cbBarHintsPlugin ) ); // facny "X"es and beveal for bars

	mpLayout->AddPlugin( CLASSINFO( cbHintAnimationPlugin ) );
	mpLayout->AddPlugin( CLASSINFO( cbRowDragPlugin  ) );
	mpLayout->AddPlugin( CLASSINFO( cbAntiflickerPlugin ) );
	mpLayout->AddPlugin( CLASSINFO( cbSimpleCustomizationPlugin ) );

	// drop in some bars

    cbDimInfo sizes0(200,45, // when docked horizontally      
                     200,85, // when docked vertically        
                     175,35, // when floated                  
                     FALSE,  // the bar is not fixed-size
                     4,      // vertical gap (bar border)
                     4       // horizontal gap (bar border)
                   ); 

    cbDimInfo sizes1(150,35, // when docked horizontally      
                     150,85, // when docked vertically        
                     175,35, // when floated                  
                     TRUE,   // the bar is not fixed-size
                     4,      // vertical gap (bar border)
                     4       // horizontal gap (bar border)
                   ); 

    cbDimInfo sizes2(175,45, // when docked horizontally      
                     175,37, // when docked vertically        
                     170,35, // when floated                  
                     TRUE,   // the bar is not fixed-size
                     4,      // vertical gap (bar border)
                     4,      // horizontal gap (bar border)
					 new cbDynToolBarDimHandler()
                   ); 
	
    mpLayout->AddBar( CreateTextCtrl("Hello"), // bar window
                      sizes0, wxTOP,           // alignment ( 0-top,1-bottom, etc)
                      0,                       // insert into 0th row (vert. position)
                      0,                       // offset from the start of row (in pixels)
                      "InfoViewer1",           // name to refere in customization pop-ups
					  TRUE
                   );
	  
    mpLayout->AddBar( CreateTextCtrl("Bye"), // bar window
                      sizes0, wxTOP,           // alignment ( 0-top,1-bottom, etc)
                      1,                       // insert into 0th row (vert. position)
                      0,                       // offset from the start of row (in pixels)
                      "InfoViewer2",           // name to refere in customization pop-ups
					  TRUE
                   );

    mpLayout->AddBar( CreateTextCtrl("Fixed0"),  // bar window
                      sizes1, wxTOP,         // alignment ( 0-top,1-bottom, etc)
                      0,                     // insert into 0th row (vert. position)
                      0,                     // offset from the start of row (in pixels)
                      "ToolBar1",            // name to refere in customization pop-ups
					  TRUE
                   );

	wxDynamicToolBar* pToolBar = new wxDynamicToolBar();

	pToolBar->Create( mpInternalFrm, -1 );
							 
	// 1001-1006 ids of command events fired by added tool-buttons
	
	pToolBar->AddTool( 1001, "new.bmp" );
	pToolBar->AddTool( 1002, "open.bmp" );
	pToolBar->AddTool( 1003, "save.bmp" );

	pToolBar->AddTool( 1004, "cut.bmp" );
	pToolBar->AddTool( 1005, "copy.bmp" );
	pToolBar->AddTool( 1006, "paste.bmp" );


    mpLayout->AddBar( pToolBar,  // bar window (can be NULL)
                      sizes2, wxTOP,         // alignment ( 0-top,1-bottom, etc)
                      0,                     // insert into 0th row (vert. position)
                      0,                     // offset from the start of row (in pixels)
                      "ToolBar2",            // name to refere in customization pop-ups
					  FALSE
                   );

	mpLayout->EnableFloating( TRUE ); // off, thinking bout wxGtk...
}

MyFrame::~MyFrame()
{
	if ( mpLayout) delete mpLayout; // should be destroyed manually
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
