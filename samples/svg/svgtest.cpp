/////////////////////////////////////////////////////////////////////////////
// Name:        svgtest.cpp
// Purpose:     SVG sample
// Author:      Chris Elliott
// Modified by:
// RCS-ID:      $Id$
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
#include "wx/dcsvg.h"
#include "wx/vector.h"

#include "bitmaps/new.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/help.xpm"
#include "SVGlogo24.xpm"

#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

class MyChild;
class MyCanvas;

// ---------------------------------------------------------------------------
// classes
// ---------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    bool OnInit();
};

class MyFrame : public wxMDIParentFrame
{
public:
    MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos, const wxSize& size, const long style);

    void InitToolBar(wxToolBar* toolBar);

    void OnSize(wxSizeEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnNewWindow(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void FileSavePicture (wxCommandEvent& event);

    unsigned int GetCountOfChildren() const
        { return m_nWinCreated; }

private:
    unsigned int m_nWinCreated;

    DECLARE_EVENT_TABLE()
};

class MyChild: public wxMDIChildFrame
{
public:
    MyChild(wxMDIParentFrame *parent, const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            const long style = wxDEFAULT_FRAME_STYLE);
    ~MyChild();

    void OnActivate(wxActivateEvent& event);
    void OnQuit(wxCommandEvent& event);
    bool OnSave(wxString filename);

    MyFrame* GetFrame()
        { return m_frame; }

private:
    MyCanvas *m_canvas;
    MyFrame  *m_frame;

    DECLARE_EVENT_TABLE()
};

class MyCanvas : public wxScrolledWindow
{
public:
    MyCanvas(MyChild *parent, const wxPoint& pos, const wxSize& size);
    virtual void OnDraw(wxDC& dc);

private:
    int m_index;
    MyChild* m_child;

    DECLARE_EVENT_TABLE()
};

// ---------------------------------------------------------------------------
// constants
// ---------------------------------------------------------------------------

// menu items ids
enum
{
    MDI_QUIT = 100,
    MDI_NEW_WINDOW,
    MDI_SAVE,
    MDI_REFRESH,
    MDI_CHILD_QUIT,
    MDI_ABOUT
};

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxMDIParentFrame)
    EVT_MENU(MDI_ABOUT, MyFrame::OnAbout)
    EVT_MENU(MDI_NEW_WINDOW, MyFrame::OnNewWindow)
    EVT_MENU(MDI_QUIT, MyFrame::OnQuit)
    EVT_MENU (MDI_SAVE, MyFrame::FileSavePicture)

    EVT_SIZE(MyFrame::OnSize)
END_EVENT_TABLE()

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// MyApp
// ---------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    // Create the main frame window

    MyFrame* frame = new MyFrame((wxFrame *)NULL, -1, wxT("SVG Demo"),
                                 wxDefaultPosition, wxSize(500, 400),
                                 wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);

    frame->Show(true);

    return true;
}


// ---------------------------------------------------------------------------
// MyFrame
// ---------------------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title,
                 const wxPoint& pos, const wxSize& size, const long style)
        : wxMDIParentFrame(parent, id, title, pos, size, style)
{
    m_nWinCreated = 0;

    SetIcon(wxICON(sample));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(MDI_NEW_WINDOW, wxT("&New test\tCtrl+N"));
    file_menu->Append(MDI_QUIT, wxT("&Exit\tAlt+X"));

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(MDI_ABOUT, wxT("&About"));

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, wxT("&File"));
    menu_bar->Append(help_menu, wxT("&Help"));

    // Associate the menu bar with the frame
    SetMenuBar(menu_bar);

#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR

    CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL);
    InitToolBar(GetToolBar());
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    (void)wxMessageBox(wxT("wxWidgets SVG sample\n")
        wxT("Author: Chris Elliott (c) 2002-2009\n")
        wxT("Usage: click File|New to show tests"),
        wxT("About SVG Test"));
}

void MyFrame::OnNewWindow(wxCommandEvent& WXUNUSED(event) )
{
    // Make another frame, containing a canvas
    MyChild *subframe = new MyChild(this, wxT("SVG Frame"));

    wxString title;
    title.Printf(wxT("SVG Test Window %d"), m_nWinCreated );

    // counts number of children previously, even if now closed
    m_nWinCreated ++;

    // Give it a title and icon
    subframe->SetTitle(title);
    subframe->SetIcon(wxICON(sample));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(MDI_NEW_WINDOW, wxT("&Another test\tCtrl+N"));
    file_menu->Append(MDI_SAVE, wxT("&Save\tCtrl+S"), wxT("Save in SVG format"));
    file_menu->Append(MDI_CHILD_QUIT, wxT("&Close child\tCtrl+F4"));
    file_menu->Append(MDI_QUIT, wxT("&Exit\tAlt+X"));

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(MDI_ABOUT, wxT("&About"));

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, wxT("&File"));
    menu_bar->Append(help_menu, wxT("&Help"));

    // Associate the menu bar with the frame
    subframe->SetMenuBar(menu_bar);

    subframe->Show(true);
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    int w, h;
    GetClientSize(&w, &h);

    GetClientWindow()->SetSize(0, 0, w, h);
    event.Skip();
}

void MyFrame::InitToolBar(wxToolBar* toolBar)
{
    const int maxBitmaps = 3;
    wxBitmap* bitmaps[maxBitmaps];

    bitmaps[0] = new wxBitmap( new_xpm );
    bitmaps[1] = new wxBitmap( save_xpm );
    bitmaps[2] = new wxBitmap( help_xpm );

    toolBar->AddTool(MDI_NEW_WINDOW, wxEmptyString, *(bitmaps[0]), wxS("New SVG test window"));
    toolBar->AddTool(MDI_SAVE, wxEmptyString, *bitmaps[1], wxS("Save test in SVG format"));
    toolBar->AddSeparator();
    toolBar->AddTool(MDI_ABOUT, wxEmptyString, *bitmaps[2], wxS("Help"));

    toolBar->Realize();

    int i;
    for (i = 0; i < maxBitmaps; i++)
        delete bitmaps[i];
}

void MyFrame::FileSavePicture (wxCommandEvent & WXUNUSED(event) )
{
#if wxUSE_FILEDLG
    MyChild * pChild = (MyChild *)GetActiveChild();
    if (pChild == NULL)
    {
        return;
    }

    wxFileDialog dialog(this, wxT("Save Picture as"), wxEmptyString, pChild->GetTitle(),
        wxT("SVG vector picture files (*.svg)|*.svg"),
        wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK)
    {
        if (!pChild->OnSave ( dialog.GetPath() ))
        {
            return;
        }
    }
    return;
#endif // wxUSE_FILEDLG
}


// ---------------------------------------------------------------------------
// MyCanvas
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(MyChild *parent, const wxPoint& pos, const wxSize& size)
    : wxScrolledWindow(parent, wxID_ANY, pos, size, wxSUNKEN_BORDER|wxVSCROLL|wxHSCROLL)
{
    SetBackgroundColour(wxColour(wxT("WHITE")));

    m_child = parent;
    m_index = m_child->GetFrame()->GetCountOfChildren() % 7;
}

// Define the repainting behaviour
void MyCanvas::OnDraw(wxDC& dc)
{
    // vars to use ...
#if wxUSE_STATUSBAR
    wxString s;
#endif // wxUSE_STATUSBAR
    wxPen wP;
    wxBrush wB;
    wxPoint points[6];
    wxColour wC;
    wxFont wF;

    dc.SetFont(*wxSWISS_FONT);
    dc.SetPen(*wxGREEN_PEN);

    switch (m_index)
    {
        default:
        case 0:
            // draw lines to make a cross
            dc.DrawLine(0, 0, 200, 200);
            dc.DrawLine(200, 0, 0, 200);
            // draw point colored line and spline
            wP = *wxCYAN_PEN;
            wP.SetWidth(3);
            dc.SetPen(wP);

            dc.DrawPoint (25,15);
            dc.DrawLine(50, 30, 200, 30);
            dc.DrawSpline(50, 200, 50, 100, 200, 10);
#if wxUSE_STATUSBAR
            s = wxT("Green Cross, Cyan Line and spline");
#endif // wxUSE_STATUSBAR
            break;

        case 1:
            // draw standard shapes
            dc.SetBrush(*wxCYAN_BRUSH);
            dc.SetPen(*wxRED_PEN);
            dc.DrawRectangle(10, 10, 100, 70);
            wB = wxBrush (wxT("DARK ORCHID"), wxBRUSHSTYLE_TRANSPARENT);
            dc.SetBrush (wB);
            dc.DrawRoundedRectangle(50, 50, 100, 70, 20);
            dc.SetBrush (wxBrush(wxT("GOLDENROD")) );
            dc.DrawEllipse(100, 100, 100, 50);

            points[0].x = 100; points[0].y = 200;
            points[1].x = 70; points[1].y = 260;
            points[2].x = 160; points[2].y = 230;
            points[3].x = 40; points[3].y = 230;
            points[4].x = 130; points[4].y = 260;
            points[5].x = 100; points[5].y = 200;

            dc.DrawPolygon(5, points);
            dc.DrawLines (6, points, 160);
#if wxUSE_STATUSBAR
            s = wxT("Blue rectangle, red edge, clear rounded rectangle, gold ellipse, gold and clear stars");
#endif // wxUSE_STATUSBAR
            break;

        case 2:
            // draw text in Arial or similar font
            dc.DrawLine(50,25,50,35);
            dc.DrawLine(45,30,55,30);
            dc.DrawText(wxT("This is a Swiss-style string"), 50, 30);
            wC = dc.GetTextForeground();
            dc.SetTextForeground (wxT("FIREBRICK"));

            // no effect in msw ??
            dc.SetTextBackground (wxT("WHEAT"));
            dc.DrawText(wxT("This is a Red string"), 50, 200);
            dc.DrawRotatedText(wxT("This is a 45 deg string"), 50, 200, 45);
            dc.DrawRotatedText(wxT("This is a 90 deg string"), 50, 200, 90);
            wF = wxFont ( 18, wxROMAN, wxITALIC, wxBOLD, false, wxT("Times New Roman"));
            dc.SetFont(wF);
            dc.SetTextForeground (wC);
            dc.DrawText(wxT("This is a Times-style string"), 50, 60);
#if wxUSE_STATUSBAR
            s = wxT("Swiss, Times text; red text, rotated and colored orange");
#endif // wxUSE_STATUSBAR
            break;

        case 3 :
            // four arcs start and end points, center
            dc.SetBrush(*wxGREEN_BRUSH);
            dc.DrawArc ( 200,300, 370,230, 300,300 );
            dc.SetBrush(*wxBLUE_BRUSH);
            dc.DrawArc ( 270-50, 270-86, 270-86, 270-50, 270,270 );
            dc.SetDeviceOrigin(-10,-10);
            dc.DrawArc ( 270-50, 270-86, 270-86, 270-50, 270,270 );
            dc.SetDeviceOrigin(0,0);

            wP.SetColour (wxT("CADET BLUE"));
            dc.SetPen(wP);
            dc.DrawArc ( 75,125, 110, 40, 75, 75 );

            wP.SetColour (wxT("SALMON"));
            dc.SetPen(wP);
            dc.SetBrush(*wxRED_BRUSH);
            //top left corner, width and height, start and end angle
                                 // 315 same center and x-radius as last pie-arc, half Y radius
            dc.DrawEllipticArc(25,50,100,50,180.0,45.0);

            wP = *wxCYAN_PEN;
            wP.SetWidth(3);
            dc.SetPen(wP);
                                 //wxTRANSPARENT));
            dc.SetBrush (wxBrush (wxT("SALMON")));
            dc.DrawEllipticArc(300,  0,200,100, 0.0,145.0);
                                 //same end point
            dc.DrawEllipticArc(300, 50,200,100,90.0,145.0);
            dc.DrawEllipticArc(300,100,200,100,90.0,345.0);

#if wxUSE_STATUSBAR
            s = wxT("This is an arc test page");
#endif // wxUSE_STATUSBAR
            break;

        case 4:
            dc.DrawCheckMark ( 30,30,25,25);
            dc.SetBrush (wxBrush (wxT("SALMON"),wxBRUSHSTYLE_TRANSPARENT));
            dc.DrawCheckMark ( 80,50,75,75);
            dc.DrawRectangle ( 80,50,75,75);
#if wxUSE_STATUSBAR
            s = wxT("Two check marks");
#endif // wxUSE_STATUSBAR
            break;

        case 5:
            wF = wxFont ( 18, wxROMAN, wxITALIC, wxBOLD, false, wxT("Times New Roman"));
            dc.SetFont(wF);
            dc.DrawLine(0, 0, 200, 200);
            dc.DrawLine(200, 0, 0, 200);
            dc.DrawText(wxT("This is an 18pt string"), 50, 60);

            // rescale and draw in blue
            wP = *wxCYAN_PEN;
            dc.SetPen(wP);
            dc.SetUserScale (2.0,0.5);
            dc.SetDeviceOrigin(200,0);
            dc.DrawLine(0, 0, 200, 200);
            dc.DrawLine(200, 0, 0, 200);
            dc.DrawText(wxT("This is an 18pt string 2 x 0.5 UserScaled"), 50, 60);
            dc.SetUserScale (2.0,2.0);
            dc.SetDeviceOrigin(200,200);
            dc.DrawText(wxT("This is an 18pt string 2 x 2 UserScaled"), 50, 60);

            wP = *wxRED_PEN;
            dc.SetPen(wP);
            dc.SetUserScale (1.0,1.0);
            dc.SetDeviceOrigin(0,10);
            dc.SetMapMode (wxMM_METRIC); //svg ignores this
            dc.DrawLine(0, 0, 200, 200);
            dc.DrawLine(200, 0, 0, 200);
            dc.DrawText(wxT("This is an 18pt string in MapMode"), 50, 60);
#if wxUSE_STATUSBAR
            s = wxT("Scaling test page");
#endif // wxUSE_STATUSBAR
            break;

        case 6:
            dc.DrawIcon( wxICON(sample), 10, 10 );
            dc.DrawBitmap ( wxBitmap(svgbitmap_xpm), 50,15);
#if wxUSE_STATUSBAR
            s = wxT("Icon and Bitmap ");
#endif // wxUSE_STATUSBAR
            break;

    }
#if wxUSE_STATUSBAR
    m_child->SetStatusText(s);
#endif // wxUSE_STATUSBAR
}


// ---------------------------------------------------------------------------
// MyChild
// ---------------------------------------------------------------------------

// Note that MDI_NEW_WINDOW and MDI_ABOUT commands get passed
// to the parent window for processing, so no need to
// duplicate event handlers here.
BEGIN_EVENT_TABLE(MyChild, wxMDIChildFrame)
    EVT_MENU(MDI_CHILD_QUIT, MyChild::OnQuit)
END_EVENT_TABLE()

MyChild::MyChild(wxMDIParentFrame *parent, const wxString& title,
                 const wxPoint& pos, const wxSize& size,
                 const long style)
    : wxMDIChildFrame(parent, wxID_ANY, title, pos, size, style)
{
    m_frame = (MyFrame *) parent;

#if wxUSE_STATUSBAR
    CreateStatusBar();
    SetStatusText(title);
#endif // wxUSE_STATUSBAR

    m_canvas = new MyCanvas(this, wxPoint(0, 0), GetClientSize());

    // Give it scrollbars
    m_canvas->SetScrollbars(20, 20, 50, 50);
}

MyChild::~MyChild()
{
}

void MyChild::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

bool MyChild::OnSave(wxString filename)
{
    wxSVGFileDC svgDC (filename, 600, 650);
    m_canvas->OnDraw (svgDC);
    return svgDC.IsOk();
}

void MyChild::OnActivate(wxActivateEvent& event)
{
    if ( event.GetActive() && m_canvas )
        m_canvas->SetFocus();
}

