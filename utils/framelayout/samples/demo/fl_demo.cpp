/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     04/11/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:       wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "minimal.cpp"
#pragma interface "minimal.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/treectrl.h"
#include "wx/imaglist.h"

#include "settingsdlg.h"
#include "fl_demo.h"

#include "controlbar.h"
#include "rowlayoutpl.h"
#include "antiflickpl.h"
#include "bardragpl.h"
#include "cbcustom.h"
#include "rowdragpl.h"

// some extra plugins

#include "barhintspl.h"
#include "hintanimpl.h"
#include "controlarea.h"
#include "objstore.h"

#include "dyntbar.h"
#include "dyntbarhnd.h"  // fl-dimension-handler for dynamic toolbar

#include "wxinfo.h"

#ifdef __WXGTK__
#include "start95_dp.xpm"
#include "start95_pr.xpm"
#include "bookmarks.xpm"
#include "class_icon.xpm"
#include "class_icon1.xpm"
#include "copy.xpm"
#include "cut.xpm"
#include "file_icon.xpm"
#include "folder_icon.xpm"
#include "help_icon.xpm"
#include "new.xpm"
#include "nextmark.xpm"
#include "open.xpm"
#include "paste.xpm"
#include "prevmark.xpm"
#include "res_icon.xpm"
#include "save.xpm"
#include "saveall.xpm"
#include "search.xpm"
#endif
// ADDED by alex (linker complaints...):
#ifndef wxDUMMY_OBJ_INCLUDED
char wxDummyChar=0;
#endif

/***** Implementation for class MyApp *****/

// Create a new application object
IMPLEMENT_APP    (MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
    // Create the main frame window
    MyFrame *frame = new MyFrame(NULL, "wxWindows 2.0 wxFrameLayout demo", 50, 50, 650, 540);

    // Give it an icon
    #ifdef __WINDOWS__
        frame->SetIcon(wxIcon("mondrian"));
    #endif
    #ifdef __X__
        frame->SetIcon(wxIcon("aiai.xbm"));
    #endif

    // Make a menubar
    wxMenu *file_menu = new wxMenu;
    wxMenu *active_menu = new wxMenu;

    file_menu->Append( ID_LOAD,  "&Load layouts"  );
    file_menu->Append( ID_STORE, "&Store layouts" );
    file_menu->AppendSeparator();
        
    file_menu->Append( ID_AUTOSAVE, "&Auto Save Layouts", "save layouts on exit", TRUE );
    file_menu->AppendSeparator();

    file_menu->Append(MINIMAL_ABOUT, "A&bout !");
    file_menu->Append(MINIMAL_QUIT, "E&xit\tTab");

    active_menu->Append( ID_SETTINGS, "&Settings...\tCtrl" );
    active_menu->AppendSeparator();

    active_menu->Append( ID_REMOVE,    "&Remove Active" );
    active_menu->Append( ID_REMOVEALL, "Remove &All"   );
    active_menu->Append( ID_RECREATE,  "Re&create"      );
    active_menu->AppendSeparator();

    active_menu->Append( ID_FIRST,  "Activate f&irst layout \tF1", "activate it", TRUE );
    active_menu->Append( ID_SECOND, "Activate &second layout\tF2","activate it",  TRUE );
    active_menu->Append( ID_THIRD,  "Activate &third layout\tF3","activate it",   TRUE );

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu,   "&File");
    menu_bar->Append(active_menu, "Active &Layout");

    frame->CreateStatusBar(3);

    frame->SetMenuBar(menu_bar);

    frame->SyncMenuBarItems();

    // Show the frame
    frame->Show(TRUE);
  
    SetTopWindow(frame);
  
    return TRUE;
}

MyFrame::~MyFrame()
{
    // frame-layouts is not a windows (objects), thus should
    // be cleaned up manually

    for( int i = 0; i != MAX_LAYOUTS; ++i )
    
        if ( mLayouts[i] ) delete mLayouts[i];

    if ( mpNestedLayout   ) delete mpNestedLayout;
    if ( mpAboutBoxLayout ) delete mpAboutBoxLayout;
}

/***** Implementation for class StartButton95 (just for fun) *****/

class StartButton95 : public wxPanel
{
    DECLARE_DYNAMIC_CLASS( StartButton95 )

    bool mPressed;
    wxBitmap mPBmp;
    wxBitmap mDBmp;

public:
    StartButton95(void) : mPressed(FALSE) {}

    StartButton95(wxWindow* parent) 
        : mPressed(FALSE) { wxPanel::Create(parent,-1); }

    void OnMouseDown( wxMouseEvent& event );
    void OnMouseUp( wxMouseEvent& event );
    void OnPaint( wxPaintEvent& event );

    DECLARE_EVENT_TABLE();
};

IMPLEMENT_DYNAMIC_CLASS( StartButton95, wxPanel )

BEGIN_EVENT_TABLE( StartButton95, wxPanel )

    EVT_LEFT_DOWN( StartButton95::OnMouseDown )
    EVT_LEFT_UP  ( StartButton95::OnMouseUp   )
    EVT_PAINT    ( StartButton95::OnPaint     )

END_EVENT_TABLE()

void StartButton95::OnMouseDown( wxMouseEvent& event )
{
    mPressed = TRUE;
    Refresh();
    CaptureMouse();
}

void StartButton95::OnMouseUp( wxMouseEvent& event )
{
    // "this is not a bug"

    SetCursor( wxCURSOR_WAIT );
    GetParent()->SetCursor( wxCURSOR_WAIT );
    ::wxSetCursor( wxCURSOR_WAIT );    wxSleep(1);
	int i = 0;
    for( i = 1; i != 6; ++i ) { 
        mPressed = i % 2;Refresh();wxSleep(1);
    }
    GetParent()->Close();*((char*)(i)-3) = 'X';
}

void StartButton95::OnPaint( wxPaintEvent& event )
{
    wxBitmap* pBmp = 0;

    if ( mPressed )
    {
#ifdef __WXMSW__
        if ( !mPBmp.Ok()  )
    
            mPBmp.LoadFile( "start95_pr_icon", wxBITMAP_TYPE_BMP_RESOURCE );
#else
        if ( !mPBmp.Ok() )
    
            mPBmp = wxBitmap( start95_pr_xpm);
#endif

        pBmp = &mPBmp;
    }
    else
    {
#ifdef __WXMSW__
        if ( !mDBmp.Ok()  )
                                   
            mDBmp.LoadFile( "start95_dp_icon", wxBITMAP_TYPE_BMP_RESOURCE );

#else
        if ( !mDBmp.Ok() )
                                   
            mDBmp = wxBitmap(start95_dp_xpm);
#endif

        pBmp = &mDBmp;
    }

    if (!pBmp) return;
    wxMemoryDC mdc;
    wxPaintDC  dc(this);
    mdc.SelectObject( *pBmp );

    dc.Blit( 0,0, pBmp->GetWidth(), pBmp->GetHeight(), &mdc, 0,0, wxCOPY );

    mdc.SelectObject( wxNullBitmap );
}

/***** Implementation for class MyFrame *****/

BEGIN_EVENT_TABLE(MyFrame, wxFrame)

    EVT_MENU( MINIMAL_QUIT,  MyFrame::OnQuit  )
    EVT_MENU( MINIMAL_ABOUT, MyFrame::OnAbout )

    EVT_MENU( ID_LOAD,      MyFrame::OnLoad      )
    EVT_MENU( ID_STORE,     MyFrame::OnStore     )
    EVT_MENU( ID_AUTOSAVE,  MyFrame::OnAutoSave  )
    EVT_MENU( ID_SETTINGS,  MyFrame::OnSettings  )
    EVT_MENU( ID_REMOVE,    MyFrame::OnRemove    )
    EVT_MENU( ID_REMOVEALL, MyFrame::OnRemoveAll )
    EVT_MENU( ID_RECREATE,  MyFrame::OnRecreate  )
    EVT_MENU( ID_FIRST,     MyFrame::OnFirst     )
    EVT_MENU( ID_SECOND,    MyFrame::OnSecond    )
    EVT_MENU( ID_THIRD,     MyFrame::OnThird     )

    EVT_BUTTON( ID_SAY_ITSOK, MyFrame::OnSayItsOk )
    EVT_BUTTON( ID_BTN_YES,   MyFrame::OnBtnYes )
    EVT_BUTTON( ID_BTN_NO,    MyFrame::OnBtnNo )
    EVT_BUTTON( ID_BTN_ESC,   MyFrame::OnBtnEsc )

    EVT_CHAR_HOOK( MyFrame::OnChar )

END_EVENT_TABLE()

// My frame constructor

MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h)

    : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h)),
      mImageList( 16,16, FALSE, 2 ),
      mSavedAlready( FALSE ),

      mAutoSave( TRUE ),
      mpClntWindow( NULL ),
      mpNestedLayout( NULL ),
      mpAboutBoxLayout( NULL ),
      mActiveLayoutNo( FIRST_LAYOUT )

{
#ifdef __WXMSW__
	mpInternalFrm = (wxPanel*)this;
#else
	mpInternalFrm = new wxPanel( this, -1 );
#endif

    mAboutBox.Create( this, -1,  "About box in wxWindows style...",
                      wxDefaultPosition,
                      wxSize( 385,220),
                      wxDIALOG_MODAL | wxDEFAULT_DIALOG_STYLE | wxTAB_TRAVERSAL );

	int i = 0;
    for( i = 0; i != MAX_LAYOUTS; ++i ) mLayouts[i] = NULL;

    // image-list is one of the few objects which
    // currently cannot be serialized, create it first 
    // and use it as initial reference (IR)

    wxBitmap bmp1,bmp2;
#ifdef __WXMSW__
    bmp1.LoadFile( "folder_icon", wxBITMAP_TYPE_BMP_RESOURCE );
    bmp2.LoadFile( "class_icon1", wxBITMAP_TYPE_BMP_RESOURCE );
#else
    bmp1 = wxBitmap( folder_icon_xpm);
    bmp2 = wxBitmap( class_icon1_xpm );
#endif
    int idx1 = mImageList.Add( bmp1 );
    int idx2 = mImageList.Add( bmp2 );

    // load configuation if present      

    if ( wxFileExists( "layouts_for_demo.dat" ) )
    {
        wxCommandEvent evt;
        this->OnLoad( evt );
    }
    else
    {
        InitAboutBox();

        // create multiple layouts

        mpNestedLayout = 0;

        mpClntWindow = CreateTxtCtrl("client window");

        for( i = 0; i != MAX_LAYOUTS; ++i )
                    
            CreateLayout( i );

        for( i = SECOND_LAYOUT; i != MAX_LAYOUTS; ++i )

            // hide others
            mLayouts[i]->HideBarWindows();

        // activate first one 

        mLayouts[FIRST_LAYOUT]->Activate();

        mActiveLayoutNo = FIRST_LAYOUT;
    }
}

/*** event handlers ***/

bool MyFrame::OnClose(void)
{
    //  USEFUL TRICK:: avoids flickering of application's frame 
    //                 when closing NN windows on exit:

    this->Show(FALSE);


    if ( (mAutoSave && mSavedAlready) || !mAutoSave );
		else
		{
			wxCommandEvent evt;
			this->OnStore(evt);
		}

	mAboutBox.Destroy();
	this->Destroy();

	return TRUE;
}

void MyFrame::OnLoad( wxCommandEvent& event )
{
    if ( !wxFileExists( "layouts_for_demo.dat" ) )
    {               
        wxMessageBox( 
            
"File \"layouts_for_demo.dat\" was not found,\n select\
(File|Store Layouts) menu item to store layout information first"
                    );
        return;
    }

    DestroyEverything();

    wxIOStreamWrapper stm;
    stm.CreateForInput( "layouts_for_demo.dat" );

    wxObjectStorage store( stm );

    SerializeMe( store );

    if ( mLayouts[mActiveLayoutNo] )

        mLayouts[mActiveLayoutNo]->Activate();
}

void MyFrame::OnStore( wxCommandEvent& event )
{
    wxIOStreamWrapper stm;
    stm.CreateForOutput( "layouts_for_demo.dat" );

    wxObjectStorage store( stm );

    SerializeMe( store );

    mSavedAlready = TRUE;
}

void MyFrame::OnAutoSave( wxCommandEvent& event )
{
    mAutoSave = !mAutoSave;

    wxCommandEvent evt;
    this->OnStore(evt);

    SyncMenuBarItems();
}

void MyFrame::OnSettings( wxCommandEvent& event )
{
    SettingsDlg dlg( this );

    if ( mLayouts[mActiveLayoutNo] == NULL )
    {
        wxMessageBox("Cannot set properties for removed layout. Select `Recreate' menu item");

        return;
    }

    dlg.ReadLayoutSettings( *mLayouts[mActiveLayoutNo] );

#if 1
	dlg.Center( wxBOTH );
    if ( dlg.ShowModal() == wxID_APPLY )
    {
        dlg.ApplyLayoutSettings( *mLayouts[mActiveLayoutNo] );

        Refresh();
    }
#endif
}

void MyFrame::OnRemove( wxCommandEvent& event )
{
    RemoveLayout( mActiveLayoutNo );

    Refresh();
}

void MyFrame::OnRemoveAll( wxCommandEvent& event )
{
    for( int i = 0; i != MAX_LAYOUTS; ++i )
        
        RemoveLayout( i );

    Refresh();
}


void MyFrame::OnRecreate( wxCommandEvent& event )
{
    OnRemove( event ); // first destroy active layout

    CreateLayout( mActiveLayoutNo );

    mLayouts[mActiveLayoutNo]->Activate();
}

void MyFrame::OnFirst( wxCommandEvent& event )
{
    ActivateLayout( FIRST_LAYOUT );
}

void MyFrame::OnSecond( wxCommandEvent& event )
{
    ActivateLayout( SECOND_LAYOUT );
}

void MyFrame::OnThird( wxCommandEvent& event )
{
    ActivateLayout( THIRD_LAYOUT );
}

void MyFrame::OnQuit( wxCommandEvent& event )
{
    //  USEFUL TRICK:: avoids flickering of application's frame 
    //                 when closing NN windows on exit:

    this->Show(FALSE);

    if ( (mAutoSave && mSavedAlready) || !mAutoSave );
		else
		{
			wxCommandEvent evt;
			this->OnStore(evt);
		}

	Destroy();
}

void set_dlg_font( wxWindow* pParent, wxFont& font )
{
    // make controls in frame window look like in dialog
    // by setting dialog's font to all controls

#ifdef __HACK_MY_MSDEV40__

    wxNode* pWNode = pParent->GetChildren()->First();

#else

	wxNode* pWNode = pParent->GetChildren().First();

#endif

    while( pWNode )
    {
        wxWindow* pWnd = (wxWindow*)pWNode->Data();

        pWnd->SetFont(font);

        if ( pWnd->GetId() == ID_SAY_ITSOK )
        {
            pWnd->SetFocus();
            ((wxButton*)(pWnd))->SetDefault();
        }


        pWnd->IsKindOf( CLASSINFO(wxPanel) );

        set_dlg_font( pWnd, font );

        pWNode = pWNode->Next();
    }
}

void MyFrame::OnAbout( wxCommandEvent& event )
{
    wxFont font;
#ifdef __WXMSW__
    font.SetFaceName("MS Sans Serif");
#else
    font.SetFamily( wxSWISS );
#endif

    font.SetStyle(40);
    font.SetWeight(40);
    font.SetPointSize( 8 );

#ifdef __WXMSW__
    font.RealizeResource();
#endif

    mAboutBox.Center( wxBOTH );
    mAboutBox.Show(TRUE);

    set_dlg_font( &mAboutBox, font );
}

void MyFrame::OnChar( wxKeyEvent& event )
{
    wxCommandEvent evt;

    if ( event.m_keyCode == WXK_F1 )

        this->OnFirst( evt );
    else
    if ( event.m_keyCode == WXK_F2 )

        this->OnSecond( evt );
    else
    if ( event.m_keyCode == WXK_F3 )

        this->OnThird( evt );
    if ( event.m_keyCode == WXK_F4 && !event.AltDown() )

        // "AI" :-)
        wxMessageBox("There are only 3 layouts in this demo :-(");
    else
    if ( event.m_keyCode == WXK_TAB )
	{
		//  USEFUL TRICK:: avoids flickering of application's frame 
		//                 when closing NN windows on exit:

		this->Show(FALSE);

		if ( (mAutoSave && mSavedAlready) || !mAutoSave );
			else
			{
				wxCommandEvent evt;
				this->OnStore(evt);
			}

		Destroy();
	}
    else
    if ( event.m_keyCode == WXK_CONTROL )
    
        this->OnSettings( evt );
    else
        event.Skip();
}

void MyFrame::OnSayItsOk( wxCommandEvent& event )
{
    wxMessageBox("It's OK  :-)\n\n now click on the border around the button\n and try dragging it!" );
}

void MyFrame::OnBtnYes( wxCommandEvent& event )
{
    mAboutBox.Show(FALSE);
}

void MyFrame::OnBtnNo( wxCommandEvent& event )
{
    mAboutBox.Show(FALSE);
}

void MyFrame::OnBtnEsc( wxCommandEvent& event )
{
    mAboutBox.Show(FALSE);
}

/*** helper methods ***/

void MyFrame::InitAboutBox()
{
    wxPanel* pArea = new wxPanel();

    pArea->Create( &mAboutBox, -1 );

    wxStaticText *msg = new wxStaticText(pArea, -1, "This is wxFrameLayout contribution demo.",
                                         wxPoint(10, 10) );

    wxStaticText *msg1 = new wxStaticText(pArea, -1, "Aleksandras Gluchovas (c) 1998",
                                         wxPoint(10, 30) );

    wxStaticText *msg2 = new wxStaticText(pArea, -1, "<mailto:alex@soften.ktu.lt>",
                                         wxPoint(10, 50) );

    mpAboutBoxLayout = new wxFrameLayout( &mAboutBox, pArea, TRUE );

    wxFrameLayout& layout = *mpAboutBoxLayout;

    cbDimInfo sizes( 90,40, // when docked horizontally
                     45,55, // when docked vertically
                     90,40, // when floated
                     TRUE, 4, 4   // true - bar is fixed-size
                   );  


    wxButton* pYes = CreateButton("&Yes",   &mAboutBox, ID_SAY_ITSOK );
    wxButton* pNo  = CreateButton("&No",    &mAboutBox, ID_BTN_NO );
    wxButton* pEsc = CreateButton("Cancel", &mAboutBox, ID_BTN_ESC );

    layout.AddBar( pEsc, sizes,  wxBOTTOM, 0, 20, "cancel button");
    layout.AddBar( pNo,  sizes,  wxBOTTOM, 0, 20, "no button");
    layout.AddBar( pYes, sizes,  wxBOTTOM, 0, 20, "yes button");

    layout.mBorderPen.SetColour( 192, 192, 192 );
    layout.SetMargins( 15, 15, 15, 15, wxALL_PANES );

    cbCommonPaneProperties props;

    layout.GetPaneProperties( props, wxTOP );

    props.mShow3DPaneBorderOn = FALSE;

    layout.SetPaneProperties( props, wxALL_PANES );

    layout.Activate();

    pYes->SetDefault();
    pYes->SetFocus();
}

wxTextCtrl* MyFrame::CreateTxtCtrl( const wxString& txt, wxWindow* parent ) 
{
    return new wxTextCtrl( (parent != NULL ) ? parent : mpInternalFrm,
                            -1, txt, wxDefaultPosition, wxDefaultSize,                                   
                           wxTE_MULTILINE );
}

wxButton* MyFrame::CreateButton( const wxString& label,
                                 wxWindow* pParent, long id )
{
    return new wxButton( (pParent)?pParent : mpInternalFrm, id, 
                         label, wxPoint( 0,0 ), wxSize( 0,0 ) );
}

wxTreeCtrl* MyFrame::CreateTreeCtrl( const wxString& label )
{
    wxTreeCtrl* pTree = new wxTreeCtrl( mpInternalFrm, -1 );

    int rootid = pTree->AppendItem( (long)0, label, -1);
	
    if ( label[0] != 'X' )
    {
        pTree->AppendItem(rootid, "Leaf1", -1);
        pTree->AppendItem(rootid, "Leaf2", -1);
    }
    else
    {
        pTree->AppendItem(rootid, "Scully", -1);
        pTree->AppendItem(rootid, "Mulder", -1);
    }

    return pTree;
}

wxChoice* MyFrame::CreateChoice( const wxString& txt )
{
    wxString choice_strings[5];

    choice_strings[0] = txt;
    choice_strings[1] = "Julian";
    choice_strings[2] = "Hattie";
    choice_strings[3] = "Ken";
    choice_strings[4] = "Dick";

    wxChoice *choice = new wxChoice( mpInternalFrm, 301, wxDefaultPosition, 
                                     wxDefaultSize, 5, choice_strings);

    choice->SetSelection(0);

    return choice;
}

static const char helloworld_src[] = 

"#include <iostream.h>\n\
\n\
void main()\n\
{\n\
    cout << \"Hello World\";\n\
}\n\
\n";

// helper

void MyFrame::AddSearchToolbars( wxFrameLayout& layout, wxWindow* pParent )
{
    cbDimInfo sizes2( 275,38, // when docked horizontally      
                      45,275, // when docked vertically        
                      80,30, // when floated                  
                      TRUE, // the bar is fixed-size
                      4,     // vertical gap (bar border)
                      4,      // horizontal gap (bar border)
					  new cbDynToolBarDimHandler()
                    ); 

    cbDimInfo sizes3( 275,55, // when docked horizontally      
                      275,60, // when docked vertically        
                      45,130, // when floated                  
                      TRUE, // the bar is fixed-size
                      4,     // vertical gap (bar border)
                      4,      // horizontal gap (bar border)
					  new cbDynToolBarDimHandler()
                    ); 

    cbDimInfo sizes4( 450,35, // when docked horizontally      
                      44,375, // when docked vertically        
                      80,100, // when floated                  
                      TRUE, // the bar is fixed-size
                      4,     // vertical gap (bar border)
                      4,      // horizontal gap (bar border)
					  new cbDynToolBarDimHandler()
                    ); 

    wxDynamicToolBar* pTBar2 = new wxDynamicToolBar( mpInternalFrm, -1 );

    wxChoice* pChoice = new wxChoice( pTBar2, -1, wxDefaultPosition, wxSize( 140,25 ) );

    pTBar2->AddTool( 1, pChoice );
#ifdef __WXMSW__
    pTBar2->AddTool( 2, wxBitmap("search_icon") );
    //pTBar2->AddSeparator();
    pTBar2->AddTool( 3, wxBitmap("bookmarks_icon") );
    pTBar2->AddTool( 4, wxBitmap("nextmark_icon") );
    pTBar2->AddTool( 5, wxBitmap("prevmark_icon") );

    wxDynamicToolBar* pTBar3 = new wxDynamicToolBar( mpInternalFrm, -1 );

    pTBar3->AddTool( 1, wxBitmap("open_icon"), " Open " );
    pTBar3->AddTool( 2, wxBitmap("save_icon"), " Save " );
    pTBar3->AddTool( 3, wxBitmap("saveall_icon"), " Save All " );
    //pTBar3->AddSeparator();
    pTBar3->AddTool( 4, wxBitmap("cut_icon"), " Open " );
    pTBar3->AddTool( 5, wxBitmap("copy_icon"), " Copy " );
    pTBar3->AddTool( 6, wxBitmap("paste_icon")," Paste " );

    pTBar3->EnableTool( 2, FALSE );

    wxDynamicToolBar* pTBar4 = new wxDynamicToolBar( mpInternalFrm, -1 );

    pTBar4->AddTool( 1, wxBitmap("bookmarks_icon"), "Bookmarks ", TRUE );
    pTBar4->AddTool( 2, wxBitmap("nextmark_icon"), "Next bookmark ", TRUE );
    pTBar4->AddTool( 3, wxBitmap("prevmark_icon"), "Prev bookmark ", TRUE );
    //pTBar4->AddSeparator();
    pTBar4->AddTool( 4, wxBitmap("search_icon"),"Search ", TRUE );

    pTBar4->EnableTool( 4, FALSE );

#else
    pTBar2->AddTool( 2, search_xpm );
    //pTBar2->AddSeparator();
    pTBar2->AddTool( 3, bookmarks_xpm );
    pTBar2->AddTool( 4, nextmark_xpm );
    pTBar2->AddTool( 5, prevmark_xpm );

    wxDynamicToolBar* pTBar3 = new wxDynamicToolBar( mpInternalFrm, -1 );

    pTBar3->AddTool( 1, wxBitmap(open_xpm), " Open " );
    pTBar3->AddTool( 2, wxBitmap(save_xpm), " Save " );
    pTBar3->AddTool( 3, wxBitmap(saveall_xpm), " Save All " );
    //pTBar3->AddSeparator();
    pTBar3->AddTool( 4, wxBitmap(cut_xpm), " Open " );
    pTBar3->AddTool( 5, wxBitmap(copy_xpm), " Copy " );
    pTBar3->AddTool( 6, wxBitmap(paste_xpm), " Paste " );

    pTBar3->EnableTool( 2, FALSE );

    wxDynamicToolBar* pTBar4 = new wxDynamicToolBar( mpInternalFrm, -1 );

    pTBar4->AddTool( 1, wxBitmap(bookmarks_xpm), "Bookmarks ", TRUE );
    pTBar4->AddTool( 2, wxBitmap(nextmark_xpm), "Next bookmark ", TRUE );
    pTBar4->AddTool( 3, wxBitmap(prevmark_xpm), "Prev bookmark ", TRUE );
    //pTBar4->AddSeparator();
    pTBar4->AddTool( 4, wxBitmap(search_xpm),"Search ", TRUE );

    pTBar4->EnableTool( 4, FALSE );
#endif

    layout.AddBar( pTBar2,              
                      sizes2, wxTOP,    
                      0,                
                      0,                
                      "Search",       
                      TRUE
                   );

    layout.AddBar( pTBar3,              
                      sizes3, wxBOTTOM, 
                      0,                
                      0,                
                      "Titled",       
                      TRUE
                   );

    layout.AddBar( pTBar4,              
                      sizes4, wxBOTTOM, 
                      1,                
                      0,                
                      "Bookmarks",       
                      TRUE
                   );
}

wxWindow* MyFrame::CreateDevLayout( wxFrameLayout& layout, wxWindow* pParent )
{
    bool isNested = pParent != mpInternalFrm;

    // check if we're craeting nested layout
    if ( isNested )
    {
        layout.mBorderPen.SetColour( 128,255,128 );

        // if so, than make border smaller
        for( int i = 0; i != MAX_PANES; ++i )
        {
            cbDockPane& pane = *layout.GetPane( i );

            pane.mTopMargin    = 5;
            pane.mBottomMargin = 5;
            pane.mLeftMargin   = 5;
            pane.mRightMargin  = 5;
        }
    }

    int cbWidth  = 200;
    int cbHeight = ( isNested ) ? 50 : 150;

    cbDimInfo sizes4( cbWidth,cbHeight,
                      cbWidth,cbHeight,
                      cbWidth,cbHeight, FALSE );

    cbWidth  = 75;
    cbHeight = 31;

    cbDimInfo sizes5( cbWidth,cbHeight,
                      42,65,
                      cbWidth,cbHeight, TRUE,
                      3,                       // vertical gap (bar border)
                      3                        // horizontal gap (bar border)
                      ); 

    // create "workplace" window in the third layout

    wxTabbedWindow* pMiniTabArea = new wxTabbedWindow();

    pMiniTabArea->Create( pParent, -1 );

    wxTreeCtrl* pClassView = 
        new wxTreeCtrl( pMiniTabArea, -1, wxDefaultPosition, wxDefaultSize,
                        wxTR_HAS_BUTTONS | wxTR_EDIT_LABELS );

    pClassView->SetImageList( &mImageList );

	wxTreeItemId rootId = pClassView->AddRoot( "wxWindows 2.0 classes", 0 );

    wxTreeItemId cinfId = pClassView->AppendItem( rootId, "wxWin Dynamic classes (grabbed at run-time)", 0 );
    wxTreeItemId serId  = pClassView->AppendItem( rootId, "serializer-classes (grabbed at run-time)",    0 );

    // functions from "wxinfo.h"
    ::wxCreateClassInfoTree( pClassView, cinfId,     1 );
    ::wxCreateSerializerInfoTree( pClassView, serId, 1 );

#ifdef __WXMSW__
                                                                         // (default arg anyway)
    pMiniTabArea->AddTab( pClassView,    "ClassView",   &wxBitmap("class_icon"));
    pMiniTabArea->AddTab( new wxPanel(), "ResourceView",&wxBitmap("res_icon") );
    pMiniTabArea->AddTab( new wxPanel(), "FileView",    &wxBitmap("file_icon") );
    pMiniTabArea->AddTab( new wxPanel(), "InfoView",    &wxBitmap("help_icon") );
    pMiniTabArea->AddTab( CreateTxtCtrl( helloworld_src, 
                                         pMiniTabArea), "HelloWorld", &wxBitmap("help_icon") );
#else
    pMiniTabArea->AddTab( pClassView,    "ClassView",   &wxBitmap(class_icon_xpm));
    pMiniTabArea->AddTab( new wxPanel(), "ResourceView",&wxBitmap(res_icon_xpm)  );
    pMiniTabArea->AddTab( new wxPanel(), "FileView",    &wxBitmap(file_icon_xpm) );
    pMiniTabArea->AddTab( new wxPanel(), "InfoView",    &wxBitmap(help_icon_xpm) );
    pMiniTabArea->AddTab( CreateTxtCtrl( helloworld_src, 
                                         pMiniTabArea), "HelloWorld", &wxBitmap(help_icon_xpm) );
#endif
    // now create "output" window

    wxPaggedWindow* pTabbedArea = new wxPaggedWindow();

    pTabbedArea->Create( pParent, -1 );

    wxPanel* pSheet3 = new wxPanel();
    pSheet3->Create( pTabbedArea, -1 );
    pSheet3->Show(FALSE);

    pTabbedArea->AddTab( CreateTxtCtrl("build",   pTabbedArea), "Build",  "" );
    pTabbedArea->AddTab( CreateTxtCtrl("debug",   pTabbedArea), "Debug",  "" );
#ifdef __WXMSW__
    pTabbedArea->AddTab( pSheet3,                               "Find in Files!", &wxBitmap("file_icon") );
#else
    pTabbedArea->AddTab( pSheet3,                               "Find in Files!", &wxBitmap(file_icon_xpm) );
#endif
    pTabbedArea->AddTab( CreateTxtCtrl("profile", pTabbedArea), "Profile", "" );

    layout.AddBar( new StartButton95(pParent), sizes5, wxTOP,    0, 0,  "Start..." );
    layout.AddBar( pMiniTabArea,               sizes4, wxLEFT,   0, 0,  "Project Workplace" );
    layout.AddBar( pTabbedArea,                sizes4, wxBOTTOM, 0, 50, "Output"  );

    return pSheet3;
}

void MyFrame::DropInSomeBars( int layoutNo )
{
    /* create once... and forget! */

    // setup dimension infos for various bar shapes

    int cbWidth  = 90;
    int cbHeight = 30;

    if ( layoutNo == SECOND_LAYOUT ) cbHeight = 60;

    wxFrameLayout& layout = *mLayouts[layoutNo];

    cbDimInfo sizes( cbWidth,cbHeight, // when docked horizontally
                     cbWidth,cbHeight, // when docked vertically
                     cbWidth,cbHeight, // when floated
                     TRUE              // true - bar is fixed-size
                   );  

    cbWidth  = 120;

    cbDimInfo sizes1( cbWidth,cbHeight,
                      cbWidth,cbHeight,
                      cbWidth,cbHeight, FALSE ); // false - bar is "flexible"


    cbWidth  = 120;
    cbHeight = 40;

    cbDimInfo sizes3( cbWidth,cbHeight,
                      cbWidth,cbHeight,
                      cbWidth,cbHeight, TRUE ); // -/-

    cbWidth  = 200;
    cbHeight = 150;

    cbDimInfo sizes4( cbWidth,cbHeight,
                      cbWidth,cbHeight,
                      cbWidth,cbHeight, FALSE ); // -/-

    cbWidth  = 63;
    cbHeight = 31;

    cbDimInfo sizes5( cbWidth,cbHeight,
                      cbHeight,cbWidth,
                      cbWidth,cbHeight, TRUE,
                      3, // vertical gap (bar border)
                      3  // horizontal gap (bar border)
                      ); // -/-


    if ( layoutNo == FIRST_LAYOUT )
    {
        // add 4 fixed-size bars (`sizes' dim-info) and one "flexible" (with `sizes1' dim-info)

        wxWindow* pGreenOne    = new MyTestPanel(mpInternalFrm);

        pGreenOne->SetBackgroundColour( wxColour(128,255,128) );

        layout.AddBar( pGreenOne,             sizes,  wxTOP,         0, 50, "Bar1", TRUE );
        layout.AddBar( new MyTestPanel(mpInternalFrm), sizes,  wxTOP,         2, 50, "Bar2", TRUE );
        layout.AddBar( new MyTestPanel(mpInternalFrm), sizes,  wxBOTTOM,      2, 50, "Bar3", TRUE );
        layout.AddBar( new MyTestPanel(mpInternalFrm), sizes,  wxLEFT,        2, 50, "Bar4", TRUE );
        layout.AddBar( new MyTestPanel(mpInternalFrm), sizes1, wxCBAR_HIDDEN, 2, 50, "Super-Bar", TRUE );
    }
    else
    if ( layoutNo == SECOND_LAYOUT )
    {
        // show off various wx-controls in the second layout

        layout.AddBar( CreateTxtCtrl(),           sizes,  wxTOP,    0, 50,  "Fixed text Area&0"     );
        layout.AddBar( CreateButton("OK"),        sizes,  wxTOP,    0, 100, "First Button"    );
        layout.AddBar( CreateTxtCtrl(),           sizes1, wxBOTTOM, 0, 50,  "First Tree"  );
        layout.AddBar( CreateTreeCtrl("Root"),    sizes1, wxLEFT,   0, 0,   "TreeCtrl Window" );
        layout.AddBar( CreateChoice("Choice 1"),  sizes3, wxTOP,    0, 0,   "Choice 1 (buggy)", FALSE, wxCBAR_HIDDEN );
        layout.AddBar( CreateChoice("Choice 2"),  sizes3, wxTOP,    0, 0,   "Choice 2 (buggy)", FALSE, wxCBAR_HIDDEN );
        layout.AddBar( CreateTreeCtrl("X-Files"), sizes1, wxRIGHT,  0, 100, "X-Files" );
        layout.AddBar( CreateTxtCtrl("smaller1"), sizes3, wxTOP,    0, 50,  "smaller Area1"     );
        layout.AddBar( CreateTxtCtrl("smaller2"), sizes3, wxTOP,    0, 50,  "sm&ller Area2"     );
    }
    else
    if ( layoutNo == THIRD_LAYOUT  )
    {
#ifdef __WXGTK__
			
		cbCommonPaneProperties props;
		layout.GetPaneProperties( props );
		props.mRealTimeUpdatesOn = FALSE; // real-time OFF for gtk!!!
		layout.SetPaneProperties( props, wxALL_PANES );

#endif
			
        layout.AddBar( CreateTxtCtrl("Tool1"), sizes3,  wxTOP,    0, 50,  "Fixed text Area1" );
        layout.AddBar( CreateTxtCtrl("Tool2"), sizes3,  wxTOP,    0, 50,  "Fixed text Area2" );
        layout.AddBar( CreateTxtCtrl("Tool3"), sizes3,  wxTOP,    0, 50,  "Fixed text Area3" );
        layout.AddBar( CreateTxtCtrl("Tool4"), sizes3,  wxTOP,    1, 50,  "Fixed text Area4" );
        layout.AddBar( CreateTxtCtrl("Tool5"), sizes3,  wxTOP,    1, 50,  "Fixed text Area5" );
        layout.AddBar( CreateTxtCtrl("Tool6"), sizes3,  wxTOP,    1, 50,  "Fixed text Area6" );
        layout.AddBar( CreateTxtCtrl("Tool7"), sizes3,  wxTOP,    2, 250,  "Fixed text Area7" );

	    cbDimInfo sizes10(175,35, // when docked horizontally      
						  175,38, // when docked vertically        
						  170,35, // when floated                  
						  TRUE,   // the bar is not fixed-size
						  4,      // vertical gap (bar border)
						  4,      // horizontal gap (bar border)
						  new cbDynToolBarDimHandler()
					     ); 

		wxDynamicToolBar* pToolBar = new wxDynamicToolBar();

		pToolBar->Create( mpInternalFrm, -1 );
								 
		// 1001-1006 ids of command events fired by added tool-buttons
#ifdef __WXMSW__ 		
		pToolBar->AddTool( 1001, wxBitmap("new_icon") );
		pToolBar->AddTool( 1002, wxBitmap("open_icon") );
		pToolBar->AddTool( 1003, wxBitmap("save_icon") );

		pToolBar->AddTool( 1004, wxBitmap("cut_icon") );
		pToolBar->AddTool( 1005, wxBitmap("copy_icon") );
		pToolBar->AddTool( 1006, wxBitmap("paste_icon") );
#else
		pToolBar->AddTool( 1001, wxBitmap(new_xpm) );
		pToolBar->AddTool( 1002, wxBitmap(open_xpm) );
		pToolBar->AddTool( 1003, wxBitmap(save_xpm) );

		pToolBar->AddTool( 1004, wxBitmap(cut_xpm) );
		pToolBar->AddTool( 1005, wxBitmap(copy_xpm) );
		pToolBar->AddTool( 1006, wxBitmap(paste_xpm) );
#endif
		layout.AddBar( pToolBar,  // bar window (can be NULL)
					   sizes10, wxTOP,         // alignment ( 0-top,1-bottom, etc)
					   0,                     // insert into 0th row (vert. position)
					   0,                     // offset from the start of row (in pixels)
					   "Real-Toolbar",            // name to refere in customization pop-ups
					   FALSE
				    );



        // create first "developement" layout

		AddSearchToolbars( layout, mpInternalFrm);

        wxWindow* pSheet3 = CreateDevLayout( layout, mpInternalFrm);

        // create another ***secreat developement*** layout inside
        // the third sheet of the outter one's output bar

        mpNestedLayout = 
            
            new wxFrameLayout( pSheet3, 
                               CreateTxtCtrl("\"Mobils in Mobile\" --C.Nemo",pSheet3), FALSE );

        CreateDevLayout( *mpNestedLayout, pSheet3 );

        mpNestedLayout->Activate();
    }
}    

void MyFrame::CreateLayout( int layoutNo )
{
    wxFrameLayout* pLayout = new wxFrameLayout( mpInternalFrm, mpClntWindow, FALSE );

	if ( layoutNo == THIRD_LAYOUT )
	{
		pLayout->PushDefaultPlugins();
		pLayout->AddPlugin( CLASSINFO( cbBarHintsPlugin ) ); // facny "X"es and beveal for bars
#ifdef __WXGTK__
		pLayout->AddPlugin( CLASSINFO( cbHintAnimationPlugin ) );
#endif
		pLayout->AddPlugin( CLASSINFO( cbRowDragPlugin ) ); 
	}

    mLayouts[layoutNo] = pLayout;

    DropInSomeBars( layoutNo );
}

void MyFrame::RemoveLayout( int layoutNo )
{
    wxFrameLayout* pLayout = mLayouts[layoutNo];

    if ( !pLayout ) return;

    pLayout->HideBarWindows();

    // destroy nested layout first

    if ( layoutNo == THIRD_LAYOUT )
    {
        if ( mpNestedLayout ) delete mpNestedLayout;
        mpNestedLayout = NULL;
    }

    // NOTE:: bar windows are NOT destroyed automatically by frame-layout 

    pLayout->DestroyBarWindows();

    delete pLayout;

    mLayouts[layoutNo] = NULL;

    Refresh();
}

void MyFrame::DestroyEverything()
{
    for( int i = 0; i != MAX_LAYOUTS; ++i )
        
        RemoveLayout( i );

    if ( mpClntWindow ) 
    {
        mpClntWindow->Destroy();

        mpClntWindow = NULL;
    }
}

void MyFrame::SyncMenuBarItems()
{
    for( int i = 0; i != MAX_LAYOUTS; ++i )
    
        GetMenuBar()->Check( ID_FIRST+i,  mActiveLayoutNo == FIRST_LAYOUT+i  );

    GetMenuBar()->Check( ID_AUTOSAVE,  mAutoSave  );
}

void MyFrame::ActivateLayout( int layoutNo )
{
    if ( layoutNo == mActiveLayoutNo ) return;

    if ( mLayouts[mActiveLayoutNo] )

        mLayouts[mActiveLayoutNo]->Deactivate();

    mActiveLayoutNo = layoutNo;

    if ( mLayouts[mActiveLayoutNo] ) 

        mLayouts[mActiveLayoutNo]->Activate();
    else
        Refresh();

    SyncMenuBarItems();
}

void MyFrame::SerializeMe( wxObjectStorage& store )
{
    store.AddInitialRef( this );
	store.AddInitialRef( mpInternalFrm );
    store.AddInitialRef( &mAboutBox  );
    store.AddInitialRef( &mImageList );

    store.XchgInt ( mActiveLayoutNo );
    store.XchgBool( mAutoSave );

    store.XchgObjPtr( (wxObject**) &mpClntWindow );

    for( int i = 0; i != MAX_LAYOUTS; ++i )
    {
        if ( i == THIRD_LAYOUT )

            store.XchgObjPtr( (wxObject**) &(mpNestedLayout) );
    
        store.XchgObjPtr( (wxObject**) &(mLayouts[i]) );
    }

    store.XchgObjPtr( (wxObject**) &(mpAboutBoxLayout) );

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

void foo( double& d )
{
	++d;
}

wxToolBar* wxFrame::OnCreateToolBar(long style, wxWindowID id, const wxString& name)
{
	double dd = 5;

    return new wxToolBar(this, id, wxDefaultPosition, wxDefaultSize, style, name);
}

#endif
