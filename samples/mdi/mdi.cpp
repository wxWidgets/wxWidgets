/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.cpp
// Purpose:     MDI sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
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

#include <wx/toolbar.h>

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
    #include "bitmaps/new.xpm"
    #include "bitmaps/open.xpm"
    #include "bitmaps/save.xpm"
    #include "bitmaps/copy.xpm"
    #include "bitmaps/cut.xpm"
    #include "bitmaps/paste.xpm"
    #include "bitmaps/print.xpm"
    #include "bitmaps/help.xpm"
#endif

#include "mdi.h"

IMPLEMENT_APP(MyApp)

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

MyFrame *frame = (MyFrame *) NULL;
wxList my_children;

// For drawing lines in a canvas
static long xpos = -1;
static long ypos = -1;

static int gs_nFrames = 0;

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxMDIParentFrame)
    EVT_MENU(MDI_ABOUT, MyFrame::OnAbout)
    EVT_MENU(MDI_NEW_WINDOW, MyFrame::OnNewWindow)
    EVT_MENU(MDI_QUIT, MyFrame::OnQuit)

    EVT_CLOSE(MyFrame::OnClose)

    EVT_SIZE(MyFrame::OnSize)
END_EVENT_TABLE()

// Note that MDI_NEW_WINDOW and MDI_ABOUT commands get passed
// to the parent window for processing, so no need to
// duplicate event handlers here.
BEGIN_EVENT_TABLE(MyChild, wxMDIChildFrame)
    EVT_MENU(MDI_CHILD_QUIT, MyChild::OnQuit)
    EVT_MENU(MDI_REFRESH, MyChild::OnRefresh)
    EVT_MENU(MDI_CHANGE_TITLE, MyChild::OnChangeTitle)
    EVT_MENU(MDI_CHANGE_POSITION, MyChild::OnChangePosition)
    EVT_MENU(MDI_CHANGE_SIZE, MyChild::OnChangeSize)

    EVT_SIZE(MyChild::OnSize)
    EVT_MOVE(MyChild::OnMove)

    EVT_CLOSE(MyChild::OnClose)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_MOUSE_EVENTS(MyCanvas::OnEvent)
END_EVENT_TABLE()

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// MyApp
// ---------------------------------------------------------------------------

// Initialise this in OnInit, not statically
bool MyApp::OnInit()
{
    // Create the main frame window

    frame = new MyFrame((wxFrame *)NULL, -1, "MDI Demo",
                        wxPoint(-1, -1), wxSize(500, 400),
                        wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
#ifdef __WXMSW__
#if 0
    // Experimental: change the window menu
    wxMenu* windowMenu = new wxMenu;
    windowMenu->Append(5000, "My menu item!");
    frame->SetWindowMenu(windowMenu);
#endif
#endif

    // Give it an icon
#ifdef __WXMSW__
    frame->SetIcon(wxIcon("mdi_icn"));
#else
    frame->SetIcon(wxIcon( mondrian_xpm ));
#endif

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(MDI_NEW_WINDOW, "&New window\tCtrl-N", "Create a new child window");
    file_menu->Append(MDI_QUIT, "&Exit\tAlt-X", "Quit the program");

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(MDI_ABOUT, "&About\tF1");

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, "&File");
    menu_bar->Append(help_menu, "&Help");

    // Associate the menu bar with the frame
    frame->SetMenuBar(menu_bar);

    frame->CreateStatusBar();

    frame->Show(TRUE);

    SetTopWindow(frame);

    return TRUE;
}

// ---------------------------------------------------------------------------
// MyFrame
// ---------------------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame(wxWindow *parent,
                 const wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 const long style)
       : wxMDIParentFrame(parent, id, title, pos, size, style)
{
    textWindow = new wxTextCtrl(this, -1, "A help window",
                                wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE | wxSUNKEN_BORDER);

    CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL);
    InitToolBar(GetToolBar());

    // Accelerators
    wxAcceleratorEntry entries[3];
    entries[0].Set(wxACCEL_CTRL, (int) 'N', MDI_NEW_WINDOW);
    entries[1].Set(wxACCEL_CTRL, (int) 'X', MDI_QUIT);
    entries[2].Set(wxACCEL_CTRL, (int) 'A', MDI_ABOUT);
    wxAcceleratorTable accel(3, entries);
    SetAcceleratorTable(accel);
}

void MyFrame::OnClose(wxCloseEvent& event)
{
    if ( event.CanVeto() && (gs_nFrames > 0) )
    {
        wxString msg;
        msg.Printf(_T("%d windows still open, close anyhow?"), gs_nFrames);
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
    (void)wxMessageBox("wxWindows 2.0 MDI Demo\n"
                       "Author: Julian Smart (c) 1997\n"
                       "Usage: mdi.exe", "About MDI Demo");
}

void MyFrame::OnNewWindow(wxCommandEvent& WXUNUSED(event) )
{
    // Make another frame, containing a canvas
    MyChild *subframe = new MyChild(frame, "Canvas Frame",
                                    wxPoint(-1, -1), wxSize(-1, -1),
                                    wxDEFAULT_FRAME_STYLE);

    wxString title;
    title.Printf(_T("Canvas Frame %d"), ++gs_nFrames);

    subframe->SetTitle(title);

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(MDI_NEW_WINDOW, "&New window");
    file_menu->Append(MDI_CHILD_QUIT, "&Close child", "Close this window");
    file_menu->Append(MDI_QUIT, "&Exit");

    wxMenu *option_menu = new wxMenu;

    option_menu->Append(MDI_REFRESH, "&Refresh picture");
    option_menu->Append(MDI_CHANGE_TITLE, "Change &title...\tCtrl-T");
    option_menu->AppendSeparator();
    option_menu->Append(MDI_CHANGE_POSITION, "Move frame\tCtrl-M");
    option_menu->Append(MDI_CHANGE_SIZE, "Resize frame\tCtrl-S");

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(MDI_ABOUT, "&About");

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, "&File");
    menu_bar->Append(option_menu, "&Child");
    menu_bar->Append(help_menu, "&Help");

    // Associate the menu bar with the frame
    subframe->SetMenuBar(menu_bar);

    subframe->CreateStatusBar();
    subframe->SetStatusText(title);

    int width, height;
    subframe->GetClientSize(&width, &height);
    MyCanvas *canvas = new MyCanvas(subframe, wxPoint(0, 0), wxSize(width, height));
    canvas->SetCursor(wxCursor(wxCURSOR_PENCIL));
    subframe->canvas = canvas;

    // Give it scrollbars
    canvas->SetScrollbars(20, 20, 50, 50);

    subframe->Show(TRUE);
}

void MyFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
    int w, h;
    GetClientSize(&w, &h);

    textWindow->SetSize(0, 0, 200, h);
    GetClientWindow()->SetSize(200, 0, w - 200, h);
}

void MyFrame::InitToolBar(wxToolBar* toolBar)
{
    wxBitmap* bitmaps[8];

#ifdef __WXMSW__
    bitmaps[0] = new wxBitmap("icon1", wxBITMAP_TYPE_RESOURCE);
    bitmaps[1] = new wxBitmap("icon2", wxBITMAP_TYPE_RESOURCE);
    bitmaps[2] = new wxBitmap("icon3", wxBITMAP_TYPE_RESOURCE);
    bitmaps[3] = new wxBitmap("icon4", wxBITMAP_TYPE_RESOURCE);
    bitmaps[4] = new wxBitmap("icon5", wxBITMAP_TYPE_RESOURCE);
    bitmaps[5] = new wxBitmap("icon6", wxBITMAP_TYPE_RESOURCE);
    bitmaps[6] = new wxBitmap("icon7", wxBITMAP_TYPE_RESOURCE);
    bitmaps[7] = new wxBitmap("icon8", wxBITMAP_TYPE_RESOURCE);
#else
    bitmaps[0] = new wxBitmap( new_xpm );
    bitmaps[1] = new wxBitmap( open_xpm );
    bitmaps[2] = new wxBitmap( save_xpm );
    bitmaps[3] = new wxBitmap( copy_xpm );
    bitmaps[4] = new wxBitmap( cut_xpm );
    bitmaps[5] = new wxBitmap( paste_xpm );
    bitmaps[6] = new wxBitmap( print_xpm );
    bitmaps[7] = new wxBitmap( help_xpm );
#endif

#ifdef __WXMSW__
    int width = 24;
#else
    int width = 16;
#endif
    int currentX = 5;

    toolBar->AddTool( MDI_NEW_WINDOW, *(bitmaps[0]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "New file");
    currentX += width + 5;
    toolBar->AddTool(1, *bitmaps[1], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Open file");
    currentX += width + 5;
    toolBar->AddTool(2, *bitmaps[2], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Save file");
    currentX += width + 5;
    toolBar->AddSeparator();
    toolBar->AddTool(3, *bitmaps[3], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Copy");
    currentX += width + 5;
    toolBar->AddTool(4, *bitmaps[4], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Cut");
    currentX += width + 5;
    toolBar->AddTool(5, *bitmaps[5], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Paste");
    currentX += width + 5;
    toolBar->AddSeparator();
    toolBar->AddTool(6, *bitmaps[6], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Print");
    currentX += width + 5;
    toolBar->AddSeparator();
    toolBar->AddTool(7, *bitmaps[7], wxNullBitmap, TRUE, currentX, -1, (wxObject *) NULL, "Help");

    toolBar->Realize();

    int i;
    for (i = 0; i < 8; i++)
        delete bitmaps[i];
}

// ---------------------------------------------------------------------------
// MyCanvas
// ---------------------------------------------------------------------------

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size)
        : wxScrolledWindow(parent, -1, pos, size,
                           wxSUNKEN_BORDER|wxVSCROLL|wxHSCROLL)
{
    SetBackgroundColour(wxColour("WHITE"));

    m_dirty = FALSE;
}

// Define the repainting behaviour
void MyCanvas::OnDraw(wxDC& dc)
{
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

    if (xpos > -1 && ypos > -1 && event.Dragging())
    {
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawLine(xpos, ypos, pt.x, pt.y);

        m_dirty = TRUE;
    }

    xpos = pt.x;
    ypos = pt.y;
}

// ---------------------------------------------------------------------------
// MyChild
// ---------------------------------------------------------------------------

MyChild::MyChild(wxMDIParentFrame *parent, const wxString& title,
                 const wxPoint& pos, const wxSize& size,
                 const long style)
       : wxMDIChildFrame(parent, -1, title, pos, size, style)
{
    canvas = (MyCanvas *) NULL;
    my_children.Append(this);
}

MyChild::~MyChild()
{
    my_children.DeleteObject(this);
}

void MyChild::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyChild::OnRefresh(wxCommandEvent& WXUNUSED(event))
{
    if ( canvas )
        canvas->Refresh();
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
    static wxString s_title = _T("Canvas Frame");

    wxString title = wxGetTextFromUser(_T("Enter the new title for MDI child"),
                                       _T("MDI sample question"),
                                       s_title,
                                       GetParent()->GetParent());
    if ( !title )
        return;

    s_title = title;
    SetTitle(s_title);
}

void MyChild::OnActivate(wxActivateEvent& event)
{
    if ( event.GetActive() && canvas )
        canvas->SetFocus();
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

void MyChild::OnClose(wxCloseEvent& event)
{
    if ( canvas && canvas->IsDirty() )
    {
        if ( wxMessageBox("Really close?", "Please confirm",
                          wxICON_QUESTION | wxYES_NO) != wxYES )
        {
            event.Veto();

            return;
        }
    }

    gs_nFrames--;

    event.Skip();
}


