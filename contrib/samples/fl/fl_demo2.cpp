/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by: Sebastian Haase (June 21, 2001)
// Created:     04/11/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
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

// wxWidgets headers.
#include "wx/treectrl.h"
#include "wx/imaglist.h"
#include "wx/notebook.h"

// fl headers.
#include "wx/fl/controlbar.h"
#include "wx/fl/rowlayoutpl.h"
#include "wx/fl/antiflickpl.h"
#include "wx/fl/bardragpl.h"
#include "wx/fl/cbcustom.h"
#include "wx/fl/rowdragpl.h"

// some extra fl plugins.
#include "wx/fl/barhintspl.h"
#include "wx/fl/hintanimpl.h"

#include "wx/fl/dyntbar.h"
#include "wx/fl/dyntbarhnd.h"  // fl-dimension-handler for dynamic toolbar

#include "fl_demo2.h"

/***** Implementation for class MyApp *****/

// Create a new application object
IMPLEMENT_APP    (MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
    // Create the main frame window
    MyFrame *frame = new MyFrame(NULL, _("wxWidgets 2.0 wxFrameLayout demo"), 50, 50, 650, 540);

    // Give it an icon
#ifdef __WINDOWS__
    frame->SetIcon(wxIcon(wxT("mondrian")));
#endif
#ifdef __X__
    frame->SetIcon(wxIcon(wxT("aiai.xbm")));
#endif

    // Make a menubar
    wxMenu *file_menu = new wxMenu;
    wxMenu *active_menu = new wxMenu;

    file_menu->Append( ID_LOAD,  _("&Load layouts")  );
    file_menu->Append( ID_STORE, _("&Store layouts") );
    file_menu->AppendSeparator();

    file_menu->Append( ID_AUTOSAVE, _("&Auto Save Layouts"), _("save layouts on exit"), wxITEM_CHECK );
    file_menu->AppendSeparator();

    file_menu->Append(MINIMAL_ABOUT, _("A&bout !"));
    file_menu->Append(MINIMAL_QUIT, _("E&xit\tTab"));

    //active_menu->Append( ID_SETTINGS, _("&Settings...\tCtrl") );
    //active_menu->AppendSeparator();

    active_menu->Append( ID_REMOVE,    _("&Remove Active") );
    active_menu->Append( ID_REMOVEALL, _("Remove &All") );
    active_menu->Append( ID_RECREATE,  _("Re&create") );
    active_menu->AppendSeparator();

    active_menu->Append( ID_FIRST,  _("Activate f&irst layout \tF1"), _("activate it"), wxITEM_CHECK );
    active_menu->Append( ID_SECOND, _("Activate &second layout\tF2"), _("activate it"),  wxITEM_CHECK );
    active_menu->Append( ID_THIRD,  _("Activate &third layout\tF3"), _("activate it"),   wxITEM_CHECK );

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu,   _("&File"));
    menu_bar->Append(active_menu, _("Active &Layout"));

#if wxUSE_STATUSBAR
    frame->CreateStatusBar(3);
#endif // wxUSE_STATUSBAR

    frame->SetMenuBar(menu_bar);

    frame->SyncMenuBarItems();

    // Show the frame
    frame->Show(true);

    SetTopWindow(frame);

    return true;
}

MyFrame::~MyFrame()
{
    // frame-layouts is not a windows (objects), thus should
    // be cleaned up manually

    for( int i = 0; i != MAX_LAYOUTS; ++i )
    {
        if ( mLayouts[i] )
            delete mLayouts[i];
    }

    if ( mpNestedLayout   )
        delete mpNestedLayout;
    if ( mpAboutBoxLayout )
        delete mpAboutBoxLayout;
}

/***** Implementation for class MyFrame *****/

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU( MINIMAL_QUIT,  MyFrame::OnQuit  )
    EVT_MENU( MINIMAL_ABOUT, MyFrame::OnAbout )

    EVT_MENU( ID_LOAD,      MyFrame::OnLoad      )
    EVT_MENU( ID_STORE,     MyFrame::OnStore     )
    EVT_MENU( ID_AUTOSAVE,  MyFrame::OnAutoSave  )
    //EVT_MENU( ID_SETTINGS,  MyFrame::OnSettings  )
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

MyFrame::MyFrame(wxFrame *frame, const wxChar *title, int x, int y, int w, int h)
    : wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h)),
    mpNestedLayout( NULL ),
    mpAboutBoxLayout( NULL ),

    mActiveLayoutNo( FIRST_LAYOUT ),
    mAutoSave( true ),
    mSavedAlready( false ),
    mpClntWindow( NULL ),

    mImageList( 16,16, false, 2 )
{
    int i;

    mpInternalFrm = (wxPanel*)this;

    mAboutBox.Create( this, wxID_ANY,  _T("About box in wxWidgets style..."),
                      wxDefaultPosition,
                      wxSize( 385,220),
                      wxDEFAULT_DIALOG_STYLE | wxTAB_TRAVERSAL );

    for( i = 0; i != MAX_LAYOUTS; ++i )
        mLayouts[i] = NULL;

    // image-list is one of the few objects which
    // currently cannot be serialized, create it first
    // and use it as initial reference (IR)

    wxBitmap bmp1,bmp2;

    if ( wxFileExists( wxString(wxT(BMP_DIR)) + wxT("folder_icon.bmp") ) )
        bmp1.LoadFile( wxString(wxT(BMP_DIR)) + wxT("folder_icon.bmp"), wxBITMAP_TYPE_BMP );

    if ( wxFileExists( wxString(wxT(BMP_DIR)) + wxT("class_icon1.bmp") ) )
        bmp2.LoadFile( wxString(wxT(BMP_DIR)) + wxT("class_icon1.bmp"), wxBITMAP_TYPE_BMP );

    mImageList.Add( bmp1 );
    mImageList.Add( bmp2 );

    InitAboutBox();

    // create multiple layouts

    mpNestedLayout = 0;

    mpClntWindow = CreateTxtCtrl(wxT("client window"));

    // Create all layouts
    for( i = 0; i != MAX_LAYOUTS; ++i )
    {
        CreateLayout( i );
    }
    // hide others
    for( i = SECOND_LAYOUT; i != MAX_LAYOUTS; ++i )
    {
        mLayouts[i]->HideBarWindows();
    }

    // activate first one
    mLayouts[FIRST_LAYOUT]->Activate();
    mActiveLayoutNo = FIRST_LAYOUT;
}

/*** event handlers ***/

bool MyFrame::OnClose(void)
{
    //  USEFUL TRICK:: avoids flickering of application's frame
    //                 when closing NN windows on exit:

    Show(false);

    if ( (mAutoSave && mSavedAlready) || !mAutoSave )
    {
    }
    else
    {
        wxCommandEvent evt;
        OnStore(evt);
    }

    mAboutBox.Destroy();
    Destroy();

    return true;
}

void MyFrame::OnLoad( wxCommandEvent& WXUNUSED(event) )
{
    wxMessageBox(_("Hey - you found a BIG question-mark !!"));
}

void MyFrame::OnStore( wxCommandEvent& WXUNUSED(event) )
{
    wxMessageBox(_("Hey - you found another BIG question-mark !!"));
}

void MyFrame::OnAutoSave( wxCommandEvent& WXUNUSED(event) )
{
    mAutoSave = !mAutoSave;

    wxCommandEvent evt;
    OnStore(evt);

    SyncMenuBarItems();
}

void MyFrame::OnRemove( wxCommandEvent& WXUNUSED(event) )
{
    RemoveLayout( mActiveLayoutNo );

    Refresh();
}

void MyFrame::OnRemoveAll( wxCommandEvent& WXUNUSED(event) )
{
    for( int i = 0; i != MAX_LAYOUTS; ++i )
    {
        RemoveLayout( i );
    }

    Refresh();
}


void MyFrame::OnRecreate( wxCommandEvent& event )
{
    OnRemove( event ); // first destroy active layout

    CreateLayout( mActiveLayoutNo );

    mLayouts[mActiveLayoutNo]->Activate();
}

void MyFrame::OnFirst( wxCommandEvent& WXUNUSED(event) )
{
    ActivateLayout( FIRST_LAYOUT );
}

void MyFrame::OnSecond( wxCommandEvent& WXUNUSED(event) )
{
    ActivateLayout( SECOND_LAYOUT );
}

void MyFrame::OnThird( wxCommandEvent& WXUNUSED(event) )
{
    ActivateLayout( THIRD_LAYOUT );
}

void MyFrame::OnQuit( wxCommandEvent& WXUNUSED(event) )
{
    //  USEFUL TRICK:: avoids flickering of application's frame
    //                 when closing NN windows on exit:

    Show(false);

    if ( (mAutoSave && mSavedAlready) || !mAutoSave )
    {
    }
    else
    {
        wxCommandEvent evt;
        OnStore(evt);
    }

    Destroy();
}

void MyFrame::OnAbout( wxCommandEvent& WXUNUSED(event) )
{
    wxFont font;
#ifdef __WXMSW__
    font.SetFaceName(wxT("MS Sans Serif"));
#else
    font.SetFamily( wxSWISS );
#endif

    font.SetStyle( wxSLANT );
    font.SetWeight( wxNORMAL );
    font.SetPointSize( 8 );

#ifdef __WXMSW__
    font.RealizeResource();
#endif

    mAboutBox.Center( wxBOTH );
    mAboutBox.Show(true);

}

void MyFrame::OnChar( wxKeyEvent& event )
{
    wxCommandEvent evt;

    if ( event.m_keyCode == WXK_F1 )
    {
        OnFirst( evt );
    }
    else
    {
        if ( event.m_keyCode == WXK_F2 )
        {
            OnSecond( evt );
        }
        else
        {
            if ( event.m_keyCode == WXK_F3 )
            {
                OnThird( evt );
            }
            if ( event.m_keyCode == WXK_F4 && !event.AltDown() )
            {
                // "AI" :-)
                wxMessageBox(_("There are only 3 layouts in this demo :-("));
            }
            else
            {
                if ( event.m_keyCode == WXK_TAB )
                {
                    //  USEFUL TRICK:: avoids flickering of application's frame
                    //                 when closing NN windows on exit:

                    Show(false);

                    if ( (mAutoSave && mSavedAlready) || !mAutoSave )
                    {
                    }
                    else
                    {
                        wxCommandEvent evt;
                        OnStore(evt);
                    }

                    Destroy();
                }
                else
                {
                    event.Skip();
                }
            }
        }
    }
}

void MyFrame::OnSayItsOk( wxCommandEvent& WXUNUSED(event) )
{
    wxMessageBox(_("It's OK  :-)\n\n now click on the border around the button\n and try dragging it!") );
}

void MyFrame::OnBtnYes( wxCommandEvent& WXUNUSED(event) )
{
    mAboutBox.Show(false);
}

void MyFrame::OnBtnNo( wxCommandEvent& WXUNUSED(event) )
{
    mAboutBox.Show(false);
}

void MyFrame::OnBtnEsc( wxCommandEvent& WXUNUSED(event) )
{
    mAboutBox.Show(false);
}

/*** helper methods ***/

void MyFrame::InitAboutBox()
{
    wxPanel* pArea = new wxPanel();

    pArea->Create( &mAboutBox, wxID_ANY );

    new wxStaticText(pArea, wxID_ANY, _("This is wxFrameLayout contribution demo."),
        wxPoint(10, 10) );

    new wxStaticText(pArea, wxID_ANY, _("Aleksandras Gluchovas (c) 1998"),
        wxPoint(10, 30) );

    new wxStaticText(pArea, wxID_ANY, _("<mailto:alex@soften.ktu.lt>"),
        wxPoint(10, 50) );

    mpAboutBoxLayout = new wxFrameLayout( &mAboutBox, pArea, true );

    wxFrameLayout& layout = *mpAboutBoxLayout;

    cbDimInfo sizes( 90,40,     // when docked horizontally
                     45,55,     // when docked vertically
                     90,40,     // when floated
                     true, 4, 4 // true - bar is fixed-size
                   );


    wxButton* pYes = CreateButton(_("&Yes"),   &mAboutBox, ID_SAY_ITSOK );
    wxButton* pNo  = CreateButton(_("&No"),    &mAboutBox, ID_BTN_NO );
    wxButton* pEsc = CreateButton(_("Cancel"), &mAboutBox, ID_BTN_ESC );

    layout.AddBar( pEsc, sizes,  FL_ALIGN_BOTTOM, 0, 20, _("cancel button"));
    layout.AddBar( pNo,  sizes,  FL_ALIGN_BOTTOM, 0, 20, _("no button"));
    layout.AddBar( pYes, sizes,  FL_ALIGN_BOTTOM, 0, 20, _("yes button"));

    layout.mBorderPen.SetColour( 192, 192, 192 );
    layout.SetMargins( 15, 15, 15, 15, wxALL_PANES );

    cbCommonPaneProperties props;

    layout.GetPaneProperties( props, FL_ALIGN_TOP );

    props.mShow3DPaneBorderOn = false;

    layout.SetPaneProperties( props, wxALL_PANES );

    layout.Activate();

    pYes->SetDefault();
    pYes->SetFocus();
}

wxTextCtrl* MyFrame::CreateTxtCtrl( const wxString& txt, wxWindow* parent )
{
    return new wxTextCtrl( (parent != NULL ) ? parent : mpInternalFrm,
                            wxID_ANY, txt, wxDefaultPosition, wxDefaultSize,
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
    wxTreeCtrl* pTree = new wxTreeCtrl( mpInternalFrm, wxID_ANY );

    const wxTreeItemId rootid = pTree->AddRoot(label);

    if ( label.StartsWith(_T("X")) )
    {
        pTree->AppendItem(rootid, _("Scully"));
        pTree->AppendItem(rootid, _("Mulder"));
    }
    else
    {
        pTree->AppendItem(rootid, _("Leaf1"));
        pTree->AppendItem(rootid, _("Leaf2"));
    }

    return pTree;
}

wxChoice* MyFrame::CreateChoice( const wxString& txt )
{
    wxString choice_strings[5];

    choice_strings[0] = txt;
    choice_strings[1] = _("Julian");
    choice_strings[2] = _("Hattie");
    choice_strings[3] = _("Ken");
    choice_strings[4] = _("Dick");

    wxChoice *choice = new wxChoice( mpInternalFrm, 301, wxDefaultPosition,
                                        wxDefaultSize, 5, choice_strings);

    choice->SetSelection(0);

    return choice;
}

// helper

void MyFrame::AddSearchToolbars( wxFrameLayout& layout, wxWindow* WXUNUSED(pParent) )
{
    cbDimInfo sizes2( 275,38,   // when docked horizontally
                      45,275,   // when docked vertically
                      80,30,    // when floated
                      true,     // the bar is fixed-size
                      4,        // vertical gap (bar border)
                      4,        // horizontal gap (bar border)
                      new cbDynToolBarDimHandler()
                    );

    cbDimInfo sizes3( 275,55,   // when docked horizontally
                      275,60,   // when docked vertically
                      45,130,   // when floated
                      true,     // the bar is fixed-size
                      4,        // vertical gap (bar border)
                      4,        // horizontal gap (bar border)
                      new cbDynToolBarDimHandler()
                    );

    cbDimInfo sizes4( 430,35,   // when docked horizontally
                      44,375,   // when docked vertically
                      80,100,   // when floated
                      true,     // the bar is fixed-size
                      4,        // vertical gap (bar border)
                      4,        // horizontal gap (bar border)
                      new cbDynToolBarDimHandler()
                    );

    wxDynamicToolBar* pTBar2 = new wxDynamicToolBar( mpInternalFrm, wxID_ANY );

    wxChoice* pChoice = new wxChoice( pTBar2, wxID_ANY, wxDefaultPosition, wxSize( 140,25 ) );

    pTBar2->AddTool( 1, pChoice );
    pTBar2->AddTool( 2, wxString(wxT(BMP_DIR)) + wxT("search.bmp") );
    //pTBar2->AddSeparator();
    pTBar2->AddTool( 3, wxString(wxT(BMP_DIR)) + wxT("bookmarks.bmp") );
    pTBar2->AddTool( 4, wxString(wxT(BMP_DIR)) + wxT("nextmark.bmp") );
    pTBar2->AddTool( 5, wxString(wxT(BMP_DIR)) + wxT("prevmark.bmp") );

    wxDynamicToolBar* pTBar3 = new wxDynamicToolBar( mpInternalFrm, wxID_ANY );

    pTBar3->AddTool( 1, wxString(wxT(BMP_DIR)) + wxT("open.bmp"), wxBITMAP_TYPE_BMP, wxString(_(" Open ")) );
    pTBar3->AddTool( 2, wxString(wxT(BMP_DIR)) + wxT("save.bmp"), wxBITMAP_TYPE_BMP, wxString(_(" Save ")) );
    pTBar3->AddTool( 3, wxString(wxT(BMP_DIR)) + wxT("saveall.bmp"), wxBITMAP_TYPE_BMP, wxString(_(" Save All ")) );
    //pTBar3->AddSeparator();
    pTBar3->AddTool( 4, wxString(wxT(BMP_DIR)) + wxT("cut.bmp"),   wxBITMAP_TYPE_BMP, wxString(_(" Open ")) );
    pTBar3->AddTool( 5, wxString(wxT(BMP_DIR)) + wxT("copy.bmp"),  wxBITMAP_TYPE_BMP, wxString(_(" Copy ")) );
    pTBar3->AddTool( 6, wxString(wxT(BMP_DIR)) + wxT("paste.bmp"), wxBITMAP_TYPE_BMP, wxString(_(" Paste ")) );

#ifdef __WXMSW__
    pTBar3->EnableTool( 2, false );
#endif

    wxDynamicToolBar* pTBar4 = new wxDynamicToolBar( mpInternalFrm, wxID_ANY );

    pTBar4->AddTool( 1, wxString(wxT(BMP_DIR)) + wxT("bookmarks.bmp"), wxBITMAP_TYPE_BMP, wxString(_("Bookmarks ")), true );
    pTBar4->AddTool( 2, wxString(wxT(BMP_DIR)) + wxT("nextmark.bmp"),  wxBITMAP_TYPE_BMP, wxString(_("Next bookmark ")), true );
    pTBar4->AddTool( 3, wxString(wxT(BMP_DIR)) + wxT("prevmark.bmp"),  wxBITMAP_TYPE_BMP, wxString(_("Prev bookmark ")), true );
    //pTBar4->AddSeparator();
    pTBar4->AddTool( 4, wxString(wxT(BMP_DIR)) + wxT("search.bmp"), wxBITMAP_TYPE_BMP, wxString(_("Search ")), true );

#ifdef __WXMSW__
    pTBar4->EnableTool( 4, false );
#endif

    layout.AddBar( pTBar2,
                   sizes2, FL_ALIGN_TOP,
                   0,
                   0,
                   wxT("Search"),
                   true
                 );

    layout.AddBar( pTBar3,
                   sizes3, FL_ALIGN_BOTTOM,
                   0,
                   0,
                   wxT("Titled"),
                   true
                 );

    layout.AddBar( pTBar4,
                   sizes4, FL_ALIGN_BOTTOM,
                   1,
                   0,
                   wxT("Bookmarks"),
                   true
                 );
}

wxWindow* MyFrame::CreateDevLayout( wxFrameLayout& layout, wxWindow* pParent )
{
    bool isNested = (pParent != mpInternalFrm);

    // check if we're creating nested layout
    if ( isNested )
    {
        layout.mBorderPen.SetColour( 128,255,128 );

        // if so, than make border smaller
        for( int i = 0; i != MAX_PANES; ++i  )
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
                      cbWidth,cbHeight, false );

    cbWidth  = 75;
    cbHeight = 31;

    cbDimInfo sizes5( cbWidth,cbHeight,
                      42,65,
                      cbWidth,cbHeight, true,
                      3,                       // vertical gap (bar border)
                      3                        // horizontal gap (bar border)
                    );

    // create "workplace" window in the third layout
    // SEB: originally here was a wxpp (wxWorkshop) class demotrated
    //    wxTabbedWindow* pMiniTabArea = new wxTabbedWindow();
    //    pMiniTabArea->Create( pParent, wxID_ANY );


    wxTreeCtrl* pClassView = new wxTreeCtrl( pParent, wxID_ANY,
                wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_EDIT_LABELS );

    pClassView->SetImageList( &mImageList );

    wxTreeItemId rootId = pClassView->AddRoot( wxT("wxWidgets 2.0 classes"), 0 );

    pClassView->AppendItem( rootId, _("wxWin Dynamic classes (grabbed at run-time)"), 0 );
    pClassView->AppendItem( rootId, _("serializer-classes (grabbed at run-time)"),    0 );

    // now create "output" window
    wxNotebook* pTabbedArea = new wxNotebook(pParent, wxID_ANY);
    // SEB: originally here was a wxpp (wxWorkshop) class used
    //    wxPaggedWindow* pTabbedArea = new wxPaggedWindow();
    //    pTabbedArea->Create( pParent, wxID_ANY );

    wxPanel* pSheet3 = new wxPanel();
    pSheet3->Create( pTabbedArea, wxID_ANY );
    pSheet3->Show(false);

    pTabbedArea->AddPage( CreateTxtCtrl(wxT("build"), pTabbedArea), wxT("Build"));
    pTabbedArea->AddPage( CreateTxtCtrl(wxT("debug"), pTabbedArea), wxT("Debug"));
    pTabbedArea->AddPage( pSheet3, wxT("is THIS recursive - or what !?"));
    pTabbedArea->AddPage( CreateTxtCtrl(wxT("profile"), pTabbedArea), wxT("Profile"));

    layout.AddBar( new StartButton95(pParent), sizes5, FL_ALIGN_TOP, 0, 0, wxT("Start...") );
    layout.AddBar( pClassView, sizes4, FL_ALIGN_LEFT, 0, 0, wxT("Project Workplace") );
    layout.AddBar( pTabbedArea, sizes4, FL_ALIGN_BOTTOM, 0, 50, wxT("Output") );

    return pSheet3;
}

void MyFrame::DropInSomeBars( int layoutNo )
{
    /* create once... and forget! */

    // setup dimension infos for various bar shapes

    int cbWidth  = 90;
    int cbHeight = 30;

    if ( layoutNo == SECOND_LAYOUT )
        cbHeight = 60;

    wxFrameLayout& layout = *mLayouts[layoutNo];

    cbDimInfo sizes( cbWidth,cbHeight, // when docked horizontally
                     cbWidth,cbHeight, // when docked vertically
                     cbWidth,cbHeight, // when floated
                     true              // true - bar is fixed-size
                   );

    cbWidth  = 120;

    cbDimInfo sizes1( cbWidth,cbHeight,
                      cbWidth,cbHeight,
                      cbWidth,cbHeight, false ); // false - bar is "flexible"

    cbWidth  = 120;
    cbHeight = 40;

    cbDimInfo sizes3( cbWidth,cbHeight,
                      cbWidth,cbHeight,
                      cbWidth,cbHeight, true ); // -/-

    cbWidth  = 200;
    cbHeight = 150;

    cbDimInfo sizes4( cbWidth,cbHeight,
                      cbWidth,cbHeight,
                      cbWidth,cbHeight, false ); // -/-

    cbWidth  = 63;
    cbHeight = 31;

    cbDimInfo sizes5( cbWidth,cbHeight,
                      cbHeight,cbWidth,
                      cbWidth,cbHeight, true,
                      3,                        // vertical gap (bar border)
                      3                         // horizontal gap (bar border)
                    ); // -/-


    if ( layoutNo == FIRST_LAYOUT )
    {
        // add 4 fixed-size bars (`sizes' dim-info) and one "flexible" (with `sizes1' dim-info)

        wxWindow* pGreenOne    = new MyTestPanel(mpInternalFrm);

        pGreenOne->SetBackgroundColour( wxColour(128,255,128) );

        layout.AddBar( pGreenOne,                      sizes,  FL_ALIGN_TOP,         0, 50, wxT("Bar1"), true );
        layout.AddBar( new MyTestPanel(mpInternalFrm), sizes,  FL_ALIGN_TOP,         2, 50, wxT("Bar2"), true );
        layout.AddBar( new MyTestPanel(mpInternalFrm), sizes,  FL_ALIGN_BOTTOM,      2, 50, wxT("Bar3"), true );
        layout.AddBar( new MyTestPanel(mpInternalFrm), sizes,  FL_ALIGN_LEFT,        2, 50, wxT("Bar4"), true );
        layout.AddBar( new MyTestPanel(mpInternalFrm), sizes1, wxCBAR_HIDDEN, 2, 50, wxT("Super-Bar"), true );
    }
    else
    {
        if ( layoutNo == SECOND_LAYOUT )
        {
            // show off various wx-controls in the second layout

            layout.AddBar( CreateTxtCtrl(),                sizes,  FL_ALIGN_TOP,    0, 50,  wxT("Fixed text Area&0")     );
            layout.AddBar( CreateButton(wxT("OK")),        sizes,  FL_ALIGN_TOP,    0, 100, wxT("First Button")    );
            layout.AddBar( CreateTxtCtrl(),                sizes1, FL_ALIGN_BOTTOM, 0, 50,  wxT("First Tree")  );
            layout.AddBar( CreateTreeCtrl(wxT("Root")),    sizes1, FL_ALIGN_LEFT,   0, 0,   wxT("TreeCtrl Window") );
            layout.AddBar( CreateChoice(wxT("Choice 1")),  sizes3, FL_ALIGN_TOP,    0, 0,   wxT("Choice 1 (buggy)"), false, wxCBAR_HIDDEN );
            layout.AddBar( CreateChoice(wxT("Choice 2")),  sizes3, FL_ALIGN_TOP,    0, 0,   wxT("Choice 2 (buggy)"), false, wxCBAR_HIDDEN );
            layout.AddBar( CreateTreeCtrl(wxT("X-Files")), sizes1, FL_ALIGN_RIGHT,  0, 100, wxT("X-Files") );
            layout.AddBar( CreateTxtCtrl(wxT("smaller1")), sizes3, FL_ALIGN_TOP,    0, 50,  wxT("smaller Area1")     );
            layout.AddBar( CreateTxtCtrl(wxT("smaller2")), sizes3, FL_ALIGN_TOP,    0, 50,  wxT("sm&ller Area2")     );
        }
        else
        {
            if ( layoutNo == THIRD_LAYOUT  )
            {
#if defined(__WXGTK__) || defined(__WXX11__)
                cbCommonPaneProperties props;
                layout.GetPaneProperties( props );
                props.mRealTimeUpdatesOn = false; // real-time OFF for gtk!!!
                layout.SetPaneProperties( props, wxALL_PANES );
#endif

                layout.AddBar( CreateTxtCtrl(wxT("Tool1")), sizes3,  FL_ALIGN_TOP,  0, 50,  wxT("Fixed text Area1") );
                layout.AddBar( CreateTxtCtrl(wxT("Tool2")), sizes3,  FL_ALIGN_TOP,  0, 50,  wxT("Fixed text Area2") );
                layout.AddBar( CreateTxtCtrl(wxT("Tool3")), sizes3,  FL_ALIGN_TOP,  0, 50,  wxT("Fixed text Area3") );
                layout.AddBar( CreateTxtCtrl(wxT("Tool4")), sizes3,  FL_ALIGN_TOP,  1, 50,  wxT("Fixed text Area4") );
                layout.AddBar( CreateTxtCtrl(wxT("Tool5")), sizes3,  FL_ALIGN_TOP,  1, 50,  wxT("Fixed text Area5") );
                layout.AddBar( CreateTxtCtrl(wxT("Tool6")), sizes3,  FL_ALIGN_TOP,  1, 50,  wxT("Fixed text Area6") );
                layout.AddBar( CreateTxtCtrl(wxT("Tool7")), sizes3,  FL_ALIGN_TOP,  2,250,  wxT("Fixed text Area7") );

                cbDimInfo sizes10( 175,35, // when docked horizontally
                                   175,38, // when docked vertically
                                   170,35, // when floated
                                   true,   // the bar is not fixed-size
                                   4,      // vertical gap (bar border)
                                   4,      // horizontal gap (bar border)
                                   new cbDynToolBarDimHandler()
                                 );

                wxDynamicToolBar* pToolBar = new wxDynamicToolBar();

                pToolBar->Create( mpInternalFrm, wxID_ANY );

                // 1001-1006 ids of command events fired by added tool-buttons

                pToolBar->AddTool( 1001, wxString(wxT(BMP_DIR)) + wxT("new.bmp") );
                pToolBar->AddTool( 1002, wxString(wxT(BMP_DIR)) + wxT("open.bmp") );
                pToolBar->AddTool( 1003, wxString(wxT(BMP_DIR)) + wxT("save.bmp") );

                pToolBar->AddTool( 1004, wxString(wxT(BMP_DIR)) + wxT("cut.bmp") );
                pToolBar->AddTool( 1005, wxString(wxT(BMP_DIR)) + wxT("copy.bmp") );
                pToolBar->AddTool( 1006, wxString(wxT(BMP_DIR)) + wxT("paste.bmp") );

                layout.AddBar( pToolBar,              // bar window (can be NULL)
                               sizes10, FL_ALIGN_TOP, // alignment ( 0-top,1-bottom, etc)
                               0,                     // insert into 0th row (vert. position)
                               0,                     // offset from the start of row (in pixels)
                               wxT("Real-Toolbar"),        // name to refere in customization pop-ups
                               false
                             );

                // create first "developement" layout
                AddSearchToolbars( layout, mpInternalFrm);

                wxWindow* pSheet3 = CreateDevLayout( layout, mpInternalFrm);

                // create another ***secreat developement*** layout inside
                // the third sheet of the outter one's output bar

                mpNestedLayout = new wxFrameLayout( pSheet3,
                    CreateTxtCtrl(wxT("\"Mobils in Mobile\" --C.Nemo"),pSheet3), false );

                CreateDevLayout( *mpNestedLayout, pSheet3 );

                mpNestedLayout->Activate();
            }
        }
    }
}

void MyFrame::CreateLayout( int layoutNo )
{
    wxFrameLayout* pLayout = new wxFrameLayout( mpInternalFrm, mpClntWindow, false );

    if ( layoutNo == THIRD_LAYOUT )
    {
        pLayout->PushDefaultPlugins();
        pLayout->AddPlugin( CLASSINFO( cbBarHintsPlugin ) ); // facny "X"es and beveal for bars
#if defined(__WXGTK__) || defined(__WXX11__)
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

    if ( !pLayout )
        return;

    pLayout->HideBarWindows();

    // destroy nested layout first

    if ( layoutNo == THIRD_LAYOUT )
    {
        if ( mpNestedLayout )
            delete mpNestedLayout;
        mpNestedLayout = NULL;
    }

    // NOTE:: bar windows are NOT destroyed automatically by frame-layout

    pLayout->DestroyBarWindows();

    delete pLayout;

    mLayouts[layoutNo] = NULL;

    Refresh();
}

void MyFrame::SyncMenuBarItems()
{
    for( int i = 0; i != MAX_LAYOUTS; ++i )
    {
        GetMenuBar()->Check( ID_FIRST+i,  mActiveLayoutNo == FIRST_LAYOUT+i );
    }

    GetMenuBar()->Check( ID_AUTOSAVE,  mAutoSave  );
}

void MyFrame::ActivateLayout( int layoutNo )
{
    if ( layoutNo == mActiveLayoutNo )
        return;

    if ( mLayouts[mActiveLayoutNo] )
        mLayouts[mActiveLayoutNo]->Deactivate();

    mActiveLayoutNo = layoutNo;

    if ( mLayouts[mActiveLayoutNo] )
        mLayouts[mActiveLayoutNo]->Activate();
    else
        Refresh();

    SyncMenuBarItems();
}

/***** Implementation for class StartButton95 (just for fun) *****/

IMPLEMENT_DYNAMIC_CLASS( StartButton95, wxPanel )

BEGIN_EVENT_TABLE( StartButton95, wxPanel )
    EVT_LEFT_DOWN( StartButton95::OnMouseDown )
    EVT_LEFT_UP  ( StartButton95::OnMouseUp   )
    EVT_PAINT    ( StartButton95::OnPaint     )
END_EVENT_TABLE()

void StartButton95::OnMouseDown( wxMouseEvent& WXUNUSED(event) )
{
    m_bPressed = true;
    Refresh();
    CaptureMouse();
}

void StartButton95::OnMouseUp( wxMouseEvent& WXUNUSED(event) )
{
    // "this is not a bug"

    SetCursor( wxCURSOR_WAIT );
    GetParent()->SetCursor( wxCURSOR_WAIT );
    ::wxSetCursor( wxCURSOR_WAIT );
    wxSleep(1);

    for( int i = 1; i != 6; ++i )
    {
        m_bPressed = (i % 2) != 0;
        Refresh();
        wxSleep(1);
    }
    GetParent()->Close();
    //*((char*)(i)-3) = 'X'; // Aleks what's the meaning of this???
}

void StartButton95::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxBitmap* pBmp;

    if ( m_bPressed )
    {
        if ( !m_PBmp.Ok() && wxFileExists( wxString(wxT(BMP_DIR)) + wxT("start95_pr.bmp") ) )

            m_PBmp.LoadFile( wxString(wxT(BMP_DIR)) + wxT("start95_pr.bmp"), wxBITMAP_TYPE_BMP );

        pBmp = &m_PBmp;
    }
    else
    {
        if ( !m_DBmp.Ok() && wxFileExists( wxString(wxT(BMP_DIR)) + wxT("start95_dp.bmp") ) )

            m_DBmp.LoadFile( wxString(wxT(BMP_DIR)) + wxT("start95_dp.bmp"), wxBITMAP_TYPE_BMP );

        pBmp = &m_DBmp;
    }

    if (!pBmp) return;
    wxMemoryDC mdc;
    wxPaintDC  dc(this);
    mdc.SelectObject( *pBmp );

    dc.Blit( 0,0, pBmp->GetWidth(), pBmp->GetHeight(), &mdc, 0,0, wxCOPY );

    mdc.SelectObject( wxNullBitmap );
}
