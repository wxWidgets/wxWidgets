/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.cpp
// Purpose:     MDI sample
// Author:      Julian Smart
// Modified by: 2008-10-31 Vadim Zeitlin: big clean up
// Created:     04/01/98
// Copyright:   (c) 1997 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/mdi.h"
#endif

#include "wx/toolbar.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
    #include "chart.xpm"
#endif

#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/cut.xpm"
#include "bitmaps/paste.xpm"
#include "bitmaps/print.xpm"
#include "bitmaps/help.xpm"

// replace this 0 with 1 to build the sample using the generic MDI classes (you
// may also need to add src/generic/mdig.cpp to the build)
#if 0
    #include "wx/generic/mdig.h"
    #define wxMDIParentFrame wxGenericMDIParentFrame
    #define wxMDIChildFrame wxGenericMDIChildFrame
    #define wxMDIClientWindow wxGenericMDIClientWindow
#endif

#include "mdi.h"

wxIMPLEMENT_APP(MyApp);

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxMDIParentFrame)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(wxID_NEW, MyFrame::OnNewWindow)
    EVT_MENU(MDI_FULLSCREEN, MyFrame::OnFullScreen)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)

    EVT_MENU(wxID_CLOSE_ALL, MyFrame::OnCloseAll)

    EVT_MENU_OPEN(MyFrame::OnMenuOpen)
    EVT_MENU_HIGHLIGHT(wxID_ABOUT, MyFrame::OnMenuHighlight)
    EVT_MENU_HIGHLIGHT(MDI_REFRESH, MyFrame::OnMenuHighlight)
    EVT_MENU_CLOSE(MyFrame::OnMenuClose)

    EVT_CLOSE(MyFrame::OnClose)
wxEND_EVENT_TABLE()

// Note that wxID_NEW and wxID_ABOUT commands get passed
// to the parent window for processing, so no need to
// duplicate event handlers here.
wxBEGIN_EVENT_TABLE(MyChild, wxMDIChildFrame)
    EVT_MENU(wxID_CLOSE, MyChild::OnClose)
    EVT_MENU(MDI_REFRESH, MyChild::OnRefresh)
    EVT_MENU(MDI_CHANGE_TITLE, MyChild::OnChangeTitle)
    EVT_MENU(MDI_CHANGE_POSITION, MyChild::OnChangePosition)
    EVT_MENU(MDI_CHANGE_SIZE, MyChild::OnChangeSize)

#if wxUSE_CLIPBOARD
    EVT_MENU(wxID_PASTE, MyChild::OnPaste)
    EVT_UPDATE_UI(wxID_PASTE, MyChild::OnUpdatePaste)
#endif // wxUSE_CLIPBOARD

    EVT_SIZE(MyChild::OnSize)
    EVT_MOVE(MyChild::OnMove)

    EVT_MENU_OPEN(MyChild::OnMenuOpen)
    EVT_MENU_HIGHLIGHT(wxID_ABOUT, MyChild::OnMenuHighlight)
    EVT_MENU_HIGHLIGHT(MDI_REFRESH, MyChild::OnMenuHighlight)
    EVT_MENU_CLOSE(MyChild::OnMenuClose)

    EVT_CLOSE(MyChild::OnCloseWindow)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_CONTEXT_MENU(MyCanvas::OnMenu)
    EVT_MOUSE_EVENTS(MyCanvas::OnEvent)

    EVT_MENU_OPEN(MyCanvas::OnMenuOpen)
    EVT_MENU_HIGHLIGHT(wxID_ABOUT, MyCanvas::OnMenuHighlight)
    EVT_MENU_HIGHLIGHT(MDI_REFRESH, MyCanvas::OnMenuHighlight)
    EVT_MENU_CLOSE(MyCanvas::OnMenuClose)

    EVT_UPDATE_UI(MDI_DISABLED_FROM_CANVAS, MyCanvas::OnUpdateUIDisable)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(MyChild::EventHandler, wxEvtHandler)
    EVT_MENU(MDI_REFRESH, MyChild::EventHandler::OnRefresh)

    EVT_UPDATE_UI(MDI_DISABLED_FROM_CHILD, MyChild::OnUpdateUIDisable)
wxEND_EVENT_TABLE()

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// MyApp
// ---------------------------------------------------------------------------

// Initialise this in OnInit, not statically
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main frame window

    MyFrame *frame = new MyFrame;

    frame->Show(true);

    return true;
}

// ---------------------------------------------------------------------------
// MyFrame
// ---------------------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame()
       : wxMDIParentFrame(NULL, wxID_ANY, "wxWidgets MDI Sample",
                          wxDefaultPosition, wxSize(500, 400)),
         MenuEventLogger("parent", this)
{
    SetIcon(wxICON(sample));

    // Make a menubar
#if wxUSE_MENUS
    // Associate the menu bar with the frame
    SetMenuBar(CreateMainMenubar());


    // This shows that the standard window menu may be customized:
    wxMenu * const windowMenu = GetWindowMenu();
    if ( windowMenu )
    {
        // we can change the labels of standard items (which also means we can
        // set up accelerators for them as they're part of the label)
        windowMenu->SetLabel(wxID_MDI_WINDOW_TILE_HORZ,
                             "&Tile horizontally\tCtrl-Shift-H");
        windowMenu->SetLabel(wxID_MDI_WINDOW_TILE_VERT,
                             "&Tile vertically\tCtrl-Shift-V");

        // we can also change the help string
        windowMenu->SetHelpString(wxID_MDI_WINDOW_CASCADE,
                                  "Arrange windows in cascade");

        // we can remove some items
        windowMenu->Delete(wxID_MDI_WINDOW_ARRANGE_ICONS);

        // and we can add completely custom commands -- but then we must handle
        // them ourselves, see OnCloseAll()
        windowMenu->AppendSeparator();
        windowMenu->Append(wxID_CLOSE_ALL, "&Close all windows\tCtrl-Shift-C",
                           "Close all open windows");

        SetWindowMenu(windowMenu);
    }
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR


    m_textWindow = new wxTextCtrl(this, wxID_ANY, "A log window\n",
                                  wxDefaultPosition, wxDefaultSize,
                                  wxTE_MULTILINE | wxTE_READONLY);

    // don't clutter the text window with time stamps
    wxLog::DisableTimestamp();
    delete wxLog::SetActiveTarget(new wxLogTextCtrl(m_textWindow));

#if wxUSE_TOOLBAR
    CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL);
    InitToolBar(GetToolBar());
#endif // wxUSE_TOOLBAR

#if wxUSE_ACCEL
    // Accelerators
    wxAcceleratorEntry entries[3];
    entries[0].Set(wxACCEL_CTRL, (int) 'N', wxID_NEW);
    entries[1].Set(wxACCEL_CTRL, (int) 'X', wxID_EXIT);
    entries[2].Set(wxACCEL_CTRL, (int) 'A', wxID_ABOUT);
    wxAcceleratorTable accel(3, entries);
    SetAcceleratorTable(accel);
#endif // wxUSE_ACCEL

    // connect it only now, after creating m_textWindow
    Bind(wxEVT_SIZE, &MyFrame::OnSize, this);
}

MyFrame::~MyFrame()
{
    // and disconnect it to prevent accessing already deleted m_textWindow in
    // the size event handler if it's called during destruction
    Unbind(wxEVT_SIZE, &MyFrame::OnSize, this);

    // also prevent its use as log target
    delete wxLog::SetActiveTarget(NULL);
}

#if wxUSE_MENUS
/* static */
wxMenuBar *MyFrame::CreateMainMenubar()
{
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(wxID_NEW, "&New window\tCtrl-N", "Create a new child window");
    menuFile->AppendCheckItem(MDI_FULLSCREEN, "Show &full screen\tCtrl-F");
    menuFile->Append(wxID_EXIT, "&Exit\tAlt-X", "Quit the program");

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT, "&About\tF1");

    wxMenuBar *mbar = new wxMenuBar;
    mbar->Append(menuFile, "&File");
    mbar->Append(menuHelp, "&Help");

    return mbar;
}
#endif // wxUSE_MENUS

void MyFrame::OnClose(wxCloseEvent& event)
{
    unsigned numChildren = MyChild::GetChildrenCount();
    if ( event.CanVeto() && (numChildren > 1) )
    {
        wxString msg;
        msg.Printf("%d windows still open, close anyhow?", numChildren);
        if ( wxMessageBox(msg, "Please confirm",
                          wxICON_QUESTION | wxYES_NO) != wxYES )
        {
            event.Veto();

            return;
        }
    }

    event.Skip();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    (void)wxMessageBox("wxWidgets 2.0 MDI Demo\n"
                       "Author: Julian Smart (c) 1997\n"
                       "Usage: mdi.exe", "About MDI Demo");
}

void MyFrame::OnNewWindow(wxCommandEvent& WXUNUSED(event) )
{
    // create and show another child frame
    MyChild *subframe = new MyChild(this);
    subframe->Show(true);
}

void MyFrame::OnFullScreen(wxCommandEvent& event)
{
    ShowFullScreen(event.IsChecked());
}

void MyFrame::OnCloseAll(wxCommandEvent& WXUNUSED(event))
{
    for ( wxWindowList::const_iterator i = GetChildren().begin();
          i != GetChildren().end();
          ++i )
    {
        if ( wxDynamicCast(*i, wxMDIChildFrame) )
            (*i)->Close();
    }
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    int w, h;
    GetClientSize(&w, &h);

    m_textWindow->SetSize(0, 0, 200, h);
    GetClientWindow()->SetSize(200, 0, w - 200, h);

    // FIXME: On wxX11, we need the MDI frame to process this
    // event, but on other platforms this should not
    // be done.
#ifdef __WXUNIVERSAL__
    event.Skip();
#else
    wxUnusedVar(event);
#endif
}

#if wxUSE_TOOLBAR
void MyFrame::InitToolBar(wxToolBar* toolBar)
{
    wxBitmap bitmaps[8];

    bitmaps[0] = wxBitmap( new_xpm );
    bitmaps[1] = wxBitmap( open_xpm );
    bitmaps[2] = wxBitmap( save_xpm );
    bitmaps[3] = wxBitmap( copy_xpm );
    bitmaps[4] = wxBitmap( cut_xpm );
    bitmaps[5] = wxBitmap( paste_xpm );
    bitmaps[6] = wxBitmap( print_xpm );
    bitmaps[7] = wxBitmap( help_xpm );

    toolBar->AddTool(wxID_NEW, "New", bitmaps[0], "New file");
    toolBar->AddTool(1, "Open", bitmaps[1], "Open file");
    toolBar->AddTool(2, "Save", bitmaps[2], "Save file");
    toolBar->AddSeparator();
    toolBar->AddTool(3, "Copy", bitmaps[3], "Copy");
    toolBar->AddTool(4, "Cut", bitmaps[4], "Cut");
    toolBar->AddTool(5, "Paste", bitmaps[5], "Paste");
    toolBar->AddSeparator();
    toolBar->AddTool(6, "Print", bitmaps[6], "Print");
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_ABOUT, "About", bitmaps[7], "Help");

    toolBar->Realize();
}
#endif // wxUSE_TOOLBAR

// ---------------------------------------------------------------------------
// MyCanvas
// ---------------------------------------------------------------------------

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxFrame *parent, const wxPoint& pos, const wxSize& size)
        : wxScrolledWindow(parent, wxID_ANY, pos, size,
                           wxSUNKEN_BORDER | wxVSCROLL | wxHSCROLL),
          MenuEventLogger("canvas", parent)

{
    SetBackgroundColour(*wxWHITE);
    SetCursor(wxCursor(wxCURSOR_PENCIL));

    SetScrollbars(20, 20, 50, 50);

    m_dirty = false;
}

// Define the repainting behaviour
void MyCanvas::OnDraw(wxDC& dc)
{
    if ( !m_text.empty() )
        dc.DrawText(m_text, 10, 10);

    dc.SetFont(*wxSWISS_FONT);
    dc.SetPen(*wxGREEN_PEN);
    dc.DrawLine(0, 0, 200, 200);
    dc.DrawLine(200, 0, 0, 200);

    dc.SetBrush(*wxCYAN_BRUSH);
    dc.SetPen(*wxRED_PEN);
    dc.DrawRectangle(100, 100, 100, 50);
    dc.DrawRoundedRectangle(150, 150, 100, 50, 20);

    dc.DrawEllipse(250, 250, 100, 50);
#if wxUSE_SPLINES
    dc.DrawSpline(50, 200, 50, 100, 200, 10);
#endif // wxUSE_SPLINES
    dc.DrawLine(50, 230, 200, 230);
    dc.DrawText("This is a test string", 50, 230);

    wxPoint points[3];
    points[0].x = 200; points[0].y = 300;
    points[1].x = 100; points[1].y = 400;
    points[2].x = 300; points[2].y = 400;

    dc.DrawPolygon(3, points);
}

// This implements a tiny doodling program! Drag the mouse using the left
// button.
void MyCanvas::OnEvent(wxMouseEvent& event)
{
    wxClientDC dc(this);
    PrepareDC(dc);

    wxPoint pt(event.GetLogicalPosition(dc));

    static long xpos = -1;
    static long ypos = -1;

    if (xpos > -1 && ypos > -1 && event.Dragging())
    {
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawLine(xpos, ypos, pt.x, pt.y);

        m_dirty = true;
    }
    else
    {
        event.Skip();
    }

    xpos = pt.x;
    ypos = pt.y;
}

void MyCanvas::OnMenu(wxContextMenuEvent& event)
{
    wxMenu menu;
    menu.Append(MDI_REFRESH, "&Refresh picture");
    menu.Append(MDI_DISABLED_FROM_CANVAS, "Item disabled by canvas");
    menu.Append(MDI_DISABLED_FROM_CHILD, "Item disabled by child");
    PopupMenu(&menu, ScreenToClient(event.GetPosition()));
}

// ---------------------------------------------------------------------------
// MyChild
// ---------------------------------------------------------------------------

unsigned MyChild::ms_numChildren = 0;

MyChild::MyChild(wxMDIParentFrame *parent)
       : wxMDIChildFrame
         (
            parent,
            wxID_ANY,
            wxString::Format("Child %u", ++ms_numChildren)
         ),
         MenuEventLogger("child", this)
{
    m_canvas = new MyCanvas(this, wxPoint(0, 0), GetClientSize());

    SetIcon(wxICON(chart));

    const bool canBeResized = !IsAlwaysMaximized();

    // create our menu bar: it will be shown instead of the main frame one when
    // we're active
#if wxUSE_MENUS
    wxMenuBar *mbar = MyFrame::CreateMainMenubar();
    mbar->GetMenu(0)->Insert(1, wxID_CLOSE, "&Close child\tCtrl-W",
                             "Close this window");

    wxMenu *menuChild = new wxMenu;

    menuChild->Append(MDI_REFRESH, "&Refresh picture");
    menuChild->Append(MDI_CHANGE_TITLE, "Change &title...\tCtrl-T");
    if ( canBeResized )
    {
        menuChild->AppendSeparator();
        menuChild->Append(MDI_CHANGE_POSITION, "Move frame\tCtrl-M");
        menuChild->Append(MDI_CHANGE_SIZE, "Resize frame\tCtrl-S");
    }
#if wxUSE_CLIPBOARD
    menuChild->AppendSeparator();
    menuChild->Append(MDI_DISABLED_FROM_CANVAS, "Item not disabled by canvas");
    menuChild->Append(MDI_DISABLED_FROM_CHILD, "Item disabled by child");
    menuChild->AppendSeparator();
    menuChild->Append(wxID_PASTE, "Copy text from clipboard\tCtrl-V");
#endif // wxUSE_CLIPBOARD

    mbar->Insert(1, menuChild, "&Child");

    // Associate the menu bar with the frame
    SetMenuBar(mbar);
#endif // wxUSE_MENUS

    // this should work for MDI frames as well as for normal ones, provided
    // they can be resized at all
    if ( canBeResized )
        SetSizeHints(100, 100);

    // test that event handlers pushed on top of MDI children do work (this
    // used to be broken, see #11225)
    PushEventHandler(new EventHandler(ms_numChildren));
}

MyChild::~MyChild()
{
    PopEventHandler(true);

    ms_numChildren--;
}

void MyChild::OnClose(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyChild::OnRefresh(wxCommandEvent& WXUNUSED(event))
{
    if ( m_canvas )
        m_canvas->Refresh();
}

void MyChild::OnChangePosition(wxCommandEvent& WXUNUSED(event))
{
    Move(10, 10);
}

void MyChild::OnChangeSize(wxCommandEvent& WXUNUSED(event))
{
    SetClientSize(100, 100);
}

void MyChild::OnChangeTitle(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_TEXTDLG
    static wxString s_title = "Canvas Frame";

    wxString title = wxGetTextFromUser("Enter the new title for MDI child",
                                       "MDI sample question",
                                       s_title,
                                       GetParent()->GetParent());
    if ( !title )
        return;

    s_title = title;
    SetTitle(s_title);
#endif // wxUSE_TEXTDLG
}

void MyChild::OnActivate(wxActivateEvent& event)
{
    if ( event.GetActive() && m_canvas )
        m_canvas->SetFocus();
}

void MyChild::OnMove(wxMoveEvent& event)
{
    // VZ: here everything is totally wrong under MSW, the positions are
    //     different and both wrong (pos2 is off by 2 pixels for me which seems
    //     to be the width of the MDI canvas border)
    wxPoint pos1 = event.GetPosition(),
            pos2 = GetPosition();
    wxLogStatus("position from event: (%d, %d), from frame (%d, %d)",
                pos1.x, pos1.y, pos2.x, pos2.y);

    event.Skip();
}

void MyChild::OnSize(wxSizeEvent& event)
{
    // VZ: under MSW the size event carries the client size (quite
    //     unexpectedly) *except* for the very first one which has the full
    //     size... what should it really be? TODO: check under wxGTK
    wxSize size1 = event.GetSize(),
           size2 = GetSize(),
           size3 = GetClientSize();
    wxLogStatus("size from event: %dx%d, from frame %dx%d, client %dx%d",
                size1.x, size1.y, size2.x, size2.y, size3.x, size3.y);

    event.Skip();
}

void MyChild::OnCloseWindow(wxCloseEvent& event)
{
    if ( m_canvas && m_canvas->IsDirty() )
    {
        if ( wxMessageBox("Really close?", "Please confirm",
                          wxICON_QUESTION | wxYES_NO) != wxYES )
        {
            event.Veto();

            return;
        }
    }

    event.Skip();
}

#if wxUSE_CLIPBOARD

#include "wx/clipbrd.h"

void MyChild::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    wxClipboardLocker lock;
    wxTextDataObject data;
    m_canvas->SetText(wxTheClipboard->GetData(data)
                        ? data.GetText()
                        : wxString("No text on clipboard"));
}

void MyChild::OnUpdatePaste(wxUpdateUIEvent& event)
{
    wxClipboardLocker lock;
    event.Enable( wxTheClipboard->IsSupported(wxDF_TEXT) );
}

#endif // wxUSE_CLIPBOARD
