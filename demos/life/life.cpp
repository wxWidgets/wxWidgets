/////////////////////////////////////////////////////////////////////////////
// Name:        life.cpp
// Purpose:     The game of life, created by J. H. Conway
// Author:      Guillermo Rodriguez Garcia, <guille@iies.es>
// Modified by:
// Created:     Jan/2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000, Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ==========================================================================
// declarations
// ==========================================================================

// --------------------------------------------------------------------------
// headers
// --------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "life.h"
#endif

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/statline.h"

#include "life.h"
#include "game.h"
#include "dialogs.h"

// --------------------------------------------------------------------------
// resources
// --------------------------------------------------------------------------

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    // the application icon
    #include "mondrian.xpm"

    // bitmap buttons for the toolbar
    #include "bitmaps/reset.xpm"
    #include "bitmaps/play.xpm"
    #include "bitmaps/stop.xpm"
#endif

// --------------------------------------------------------------------------
// constants
// --------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items and toolbar buttons
    ID_NEWGAME = 1001,
    ID_SAMPLES,
    ID_ABOUT,
    ID_EXIT,
    ID_CLEAR,
    ID_START,
    ID_STEP,
    ID_STOP,
    ID_WRAP,

    // speed selection slider
    ID_SLIDER
};

// --------------------------------------------------------------------------
// event tables and other macros for wxWindows
// --------------------------------------------------------------------------

// Event tables
BEGIN_EVENT_TABLE(LifeFrame, wxFrame)
    EVT_MENU           (ID_NEWGAME, LifeFrame::OnNewGame)
    EVT_MENU           (ID_SAMPLES, LifeFrame::OnSamples)
    EVT_MENU           (ID_ABOUT,   LifeFrame::OnMenu)
    EVT_MENU           (ID_EXIT,    LifeFrame::OnMenu)
    EVT_MENU           (ID_CLEAR,   LifeFrame::OnMenu)
    EVT_MENU           (ID_START,   LifeFrame::OnMenu)
    EVT_MENU           (ID_STEP,    LifeFrame::OnMenu)
    EVT_MENU           (ID_STOP,    LifeFrame::OnMenu)
    EVT_MENU           (ID_WRAP,    LifeFrame::OnMenu)
    EVT_COMMAND_SCROLL (ID_SLIDER,  LifeFrame::OnSlider)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(LifeCanvas, wxScrolledWindow)
    EVT_PAINT          (            LifeCanvas::OnPaint)
    EVT_SIZE           (            LifeCanvas::OnSize)
    EVT_MOUSE_EVENTS   (            LifeCanvas::OnMouse)
END_EVENT_TABLE()


// Create a new application object
IMPLEMENT_APP(LifeApp)

// ==========================================================================
// implementation
// ==========================================================================

// some shortcuts
#define ADD_TOOL(a, b, c, d) \
    toolBar->AddTool(a, b, wxNullBitmap, FALSE, -1, -1, (wxObject *)0, c, d)

#define GET_FRAME() \
    ((LifeFrame *) wxGetApp().GetTopWindow())

// --------------------------------------------------------------------------
// LifeApp
// --------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool LifeApp::OnInit()
{
    // create the main application window
    LifeFrame *frame = new LifeFrame();

    // show it and tell the application that it's our main window
    frame->Show(TRUE);
    SetTopWindow(frame);

    // enter the main message loop and run the app
    return TRUE;
}

// --------------------------------------------------------------------------
// LifeFrame
// --------------------------------------------------------------------------

// frame constructor
LifeFrame::LifeFrame() : wxFrame((wxFrame *)0, -1, _("Life!"), wxPoint(50, 50))
{
    // frame icon
    SetIcon(wxICON(mondrian));

    // menu bar
    wxMenu *menuFile = new wxMenu("", wxMENU_TEAROFF);
    wxMenu *menuGame = new wxMenu("", wxMENU_TEAROFF);

    menuFile->Append(ID_NEWGAME, _("New game..."), _("Start a new game"));
    menuFile->Append(ID_SAMPLES, _("Sample game..."), _("Select a sample configuration"));
    menuFile->AppendSeparator();
    menuFile->Append(ID_ABOUT, _("&About...\tCtrl-A"), _("Show about dialog"));
    menuFile->AppendSeparator();
    menuFile->Append(ID_EXIT, _("E&xit\tAlt-X"), _("Quit this program"));

    menuGame->Append(ID_CLEAR, _("&Clear\tCtrl-C"), _("Clear game field"));
    menuGame->Append(ID_START, _("&Start\tCtrl-S"), _("Start"));
    menuGame->Append(ID_STEP, _("&Next\tCtrl-N"), _("Single step"));
    menuGame->Append(ID_STOP, _("S&top\tCtrl-T"), _("Stop"));
    menuGame->Enable(ID_STOP, FALSE);
    menuGame->AppendSeparator();
    menuGame->Append(ID_WRAP, _("&Wraparound\tCtrl-W"), _("Wrap around borders"), TRUE);
    menuGame->Check (ID_WRAP, TRUE);

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _("&File"));
    menuBar->Append(menuGame, _("&Game"));
    SetMenuBar(menuBar);

    // tool bar
    wxBitmap tbBitmaps[3];

    tbBitmaps[0] = wxBITMAP(reset);
    tbBitmaps[1] = wxBITMAP(play);
    tbBitmaps[2] = wxBITMAP(stop);

    wxToolBar *toolBar = CreateToolBar();
    toolBar->SetMargins(5, 5);
    toolBar->SetToolBitmapSize(wxSize(16, 16));
    ADD_TOOL(ID_CLEAR, tbBitmaps[0], _("Clear"), _("Clear game board"));
    ADD_TOOL(ID_START, tbBitmaps[1], _("Start"), _("Start"));
    ADD_TOOL(ID_STOP , tbBitmaps[2], _("Stop"),  _("Stop"));
    toolBar->EnableTool(ID_STOP, FALSE);
    toolBar->Realize();

    // status bar
    CreateStatusBar(2);
    SetStatusText(_("Welcome to Life!"));

    // game
    wxPanel *panel = new wxPanel(this, -1);
    m_life         = new Life(20, 20);
    m_canvas       = new LifeCanvas(panel, m_life);
    m_timer        = new LifeTimer();
    m_interval     = 500;
    m_tics         = 0;
    m_text         = new wxStaticText(panel, -1, "");
    UpdateInfoText();

    // slider
    wxSlider *slider = new wxSlider(panel, ID_SLIDER,
        5, 1, 10,
        wxDefaultPosition,
        wxSize(200, -1),
        wxSL_HORIZONTAL | wxSL_AUTOTICKS);

    // component layout
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticLine(panel, -1), 0, wxGROW | wxCENTRE);
    sizer->Add(m_canvas, 1, wxGROW | wxCENTRE | wxALL, 5);
    sizer->Add(new wxStaticLine(panel, -1), 0, wxGROW | wxCENTRE);
    sizer->Add(m_text, 0, wxCENTRE | wxTOP, 5);
    sizer->Add(slider, 0, wxCENTRE | wxALL, 5);
    panel->SetSizer(sizer);
    panel->SetAutoLayout(TRUE);
    sizer->Fit(this);
    sizer->SetSizeHints(this);
}

LifeFrame::~LifeFrame()
{
    delete m_timer;
    delete m_life;
}

void LifeFrame::UpdateInfoText()
{
    wxString msg;

    msg.Printf(_("Generation: %u,  Interval: %u ms"), m_tics, m_interval);
    m_text->SetLabel(msg);
}

// event handlers
void LifeFrame::OnMenu(wxCommandEvent& event)
{
    switch (event.GetId())
    {
        case ID_START   : OnStart(); break;
        case ID_STEP    : OnTimer(); break;
        case ID_STOP    : OnStop(); break;
        case ID_WRAP    :
        {
            bool checked = GetMenuBar()->GetMenu(1)->IsChecked(ID_WRAP);
            m_life->SetBorderWrap(checked);
            break;
        }
        case ID_CLEAR   : 
        {
            OnStop();
            m_life->Clear();
            m_canvas->DrawEverything(TRUE);
            m_canvas->Refresh(FALSE);
            m_tics = 0;
            UpdateInfoText();
            break;
        }
        case ID_ABOUT :
        {
            wxMessageBox(
                _("This is the about dialog of the Life! sample.\n"
                  "(c) 2000 Guillermo Rodriguez Garcia"),
                _("About Life!"),
                wxOK | wxICON_INFORMATION,
                this);
            break;
        }
        case ID_EXIT :
        {
            // TRUE is to force the frame to close
            Close(TRUE);
            break;
        }
    }
}

void LifeFrame::OnNewGame(wxCommandEvent& WXUNUSED(event))
{
    int w = m_life->GetWidth();
    int h = m_life->GetHeight();

    // stop if it was running
    OnStop();

    // dialog box
    LifeNewGameDialog dialog(this, &w, &h);

    // new game?
    if (dialog.ShowModal() == wxID_OK)
    {
        // check dimensions
        if (w >= LIFE_MIN && w <= LIFE_MAX &&
            h >= LIFE_MIN && h <= LIFE_MAX)
        {
            // resize game field
            m_life->Destroy();
            m_life->Create(w, h);
            
            // tell the canvas
            m_canvas->Reset();
            m_canvas->Refresh();
            m_tics = 0;
            UpdateInfoText();
        }
        else
        {
            wxString msg;
            msg.Printf(_("Both dimensions must be within %u and %u.\n"),
                LIFE_MIN, LIFE_MAX);
            wxMessageBox(msg, _("Error!"), wxOK | wxICON_EXCLAMATION, this);
        }
    }
}

void LifeFrame::OnSamples(wxCommandEvent& WXUNUSED(event))
{
    // stop if it was running
    OnStop();

    // dialog box
    LifeSamplesDialog dialog(this);

    // new game?
    if (dialog.ShowModal() == wxID_OK)
    {
        int result = dialog.GetValue();

        if (result == -1)
            return;

        int gw   = g_shapes[result].m_fieldWidth;
        int gh   = g_shapes[result].m_fieldHeight;
        int wrap = g_shapes[result].m_wrap;

        // set wraparound (don't ask the user)
        m_life->SetBorderWrap(wrap);
        GetMenuBar()->GetMenu(1)->Check(ID_WRAP, wrap);

        // need to resize the game field?
        if (gw > m_life->GetWidth() || gh > m_life->GetHeight())
        {
            wxString s;
            s.Printf(_("Your game field is too small for this configuration.\n"
                       "It is recommended to resize it to %u x %u. Proceed?\n"),
                       gw, gh);

            if (wxMessageBox(s, _("Question"), wxYES_NO | wxICON_QUESTION, this) == wxYES)
            {
                m_life->Destroy();
                m_life->Create(gw, gh);
            }
        }

        // put the shape
        m_life->SetShape(g_shapes[result]);

        // tell the canvas about the change
        m_canvas->Reset();
        m_canvas->Refresh();
        m_tics = 0;
        UpdateInfoText();
    }
}

void LifeFrame::OnStart()
{
    if (!m_running)
    {
        GetToolBar()->EnableTool(ID_START, FALSE);
        GetToolBar()->EnableTool(ID_STOP,  TRUE);
        GetMenuBar()->GetMenu(1)->Enable(ID_START, FALSE);
        GetMenuBar()->GetMenu(1)->Enable(ID_STEP,  FALSE);
        GetMenuBar()->GetMenu(1)->Enable(ID_STOP,  TRUE);

        m_timer->Start(m_interval);
        m_running = TRUE;
    }
}

void LifeFrame::OnStop()
{
    if (m_running)
    {
        GetToolBar()->EnableTool(ID_START, TRUE);
        GetToolBar()->EnableTool(ID_STOP,  FALSE);
        GetMenuBar()->GetMenu(1)->Enable(ID_START, TRUE);
        GetMenuBar()->GetMenu(1)->Enable(ID_STEP,  TRUE);
        GetMenuBar()->GetMenu(1)->Enable(ID_STOP,  FALSE);
        
        m_timer->Stop();
        m_running = FALSE;
    }
}

void LifeFrame::OnTimer()
{
    if (m_life->NextTic())
        m_tics++;
    else
        OnStop();

    UpdateInfoText();
    m_canvas->DrawEverything();
    m_canvas->Refresh(FALSE);
}

void LifeFrame::OnSlider(wxScrollEvent& event)
{
    m_interval = event.GetPosition() * 100;

    // restart timer if running, to set the new interval
    if (m_running)
    {
        m_timer->Stop();
        m_timer->Start(m_interval);
    }

    UpdateInfoText();
}

// --------------------------------------------------------------------------
// LifeTimer
// --------------------------------------------------------------------------

void LifeTimer::Notify()
{
    GET_FRAME()->OnTimer();
};

// --------------------------------------------------------------------------
// LifeCanvas
// --------------------------------------------------------------------------

// canvas constructor
LifeCanvas::LifeCanvas(wxWindow *parent, Life *life, bool interactive)
          : wxScrolledWindow(parent, -1, wxPoint(0, 0), wxSize(100, 100))
{
    m_life        = life;
    m_interactive = interactive;
    m_cellsize    = 8;
    m_bmp         = NULL;
    Reset();
}

LifeCanvas::~LifeCanvas()
{
    delete m_bmp;
}

void LifeCanvas::Reset()
{
    if (m_bmp)
        delete m_bmp;

    m_status   = MOUSE_NOACTION;
    m_width    = CellToCoord(m_life->GetWidth()) + 1;
    m_height   = CellToCoord(m_life->GetHeight()) + 1;
    m_bmp      = new wxBitmap(m_width, m_height);
    wxCoord w  = GetClientSize().GetX();
    wxCoord h  = GetClientSize().GetY();
    m_xoffset  = (w > m_width)?  ((w - m_width) / 2)  : 0;
    m_yoffset  = (h > m_height)? ((h - m_height) / 2) : 0;

    // redraw everything
    DrawEverything(TRUE);
    SetScrollbars(10, 10, (m_width + 9) / 10, (m_height + 9) / 10);
}

void LifeCanvas::DrawEverything(bool force)
{
    wxMemoryDC dc;

    dc.SelectObject(*m_bmp);
    dc.BeginDrawing();

    // draw cells
    const CellArray *cells =
        force? m_life->GetCells() : m_life->GetChangedCells();

    for (unsigned i = 0; i < cells->GetCount(); i++)
        DrawCell(cells->Item(i), dc);

    // bounding rectangle (always drawn - better than clipping region)
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(0, 0, m_width, m_height);

    dc.EndDrawing();
    dc.SelectObject(wxNullBitmap);
}

void LifeCanvas::DrawCell(Cell c)
{
    wxMemoryDC dc;

    dc.SelectObject(*m_bmp);
    dc.BeginDrawing();

    dc.SetClippingRegion(1, 1, m_width - 2, m_height - 2);
    DrawCell(c, dc);

    dc.EndDrawing();
    dc.SelectObject(wxNullBitmap);
}

void LifeCanvas::DrawCell(Cell c, wxDC &dc)
{
    if (m_life->IsAlive(c))
    {
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBrush(*wxBLACK_BRUSH);
        dc.DrawRectangle(CellToCoord( m_life->GetX(c) ),
                         CellToCoord( m_life->GetY(c) ),
                         m_cellsize,
                         m_cellsize);
    }
    else
    {
        dc.SetPen(*wxLIGHT_GREY_PEN);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(CellToCoord( m_life->GetX(c) ),
                         CellToCoord( m_life->GetY(c) ),
                         m_cellsize,
                         m_cellsize);
        dc.SetPen(*wxWHITE_PEN);
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.DrawRectangle(CellToCoord( m_life->GetX(c) ) + 1,
                         CellToCoord( m_life->GetY(c) ) + 1,
                         m_cellsize - 1,
                         m_cellsize - 1);
    }
}

// event handlers
void LifeCanvas::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    wxMemoryDC memdc;

    wxRegionIterator upd(GetUpdateRegion());
    wxCoord x, y, w, h, xx, yy;

    dc.BeginDrawing();
    memdc.SelectObject(*m_bmp);

    while(upd)
    {
        x = upd.GetX();
        y = upd.GetY();
        w = upd.GetW();
        h = upd.GetH();
        CalcUnscrolledPosition(x, y, &xx, &yy);

        dc.Blit(x, y, w, h, &memdc, xx - m_xoffset, yy - m_yoffset);
        upd++;
    }

    memdc.SelectObject(wxNullBitmap);
    dc.EndDrawing();
}

void LifeCanvas::OnMouse(wxMouseEvent& event)
{
    if (!m_interactive)
        return;

    int x, y, xx, yy, i, j;

    // which cell are we pointing at?
    x = event.GetX();
    y = event.GetY();
    CalcUnscrolledPosition(x, y, &xx, &yy);
    i = CoordToCell( xx - m_xoffset );
    j = CoordToCell( yy - m_yoffset );

    // adjust x, y to point to the upper left corner of the cell
    CalcScrolledPosition( CellToCoord(i) + m_xoffset,
                          CellToCoord(j) + m_yoffset,
                          &x, &y );

    // set cursor shape and statusbar text
    if (i < 0 || i >= m_life->GetWidth() ||
        j < 0 || j >= m_life->GetHeight())
    {
        GET_FRAME()->SetStatusText(wxEmptyString, 1);
        SetCursor(*wxSTANDARD_CURSOR);
    }
    else
    {
        wxString msg;
        msg.Printf(_("Cell: (%u, %u)"), i, j);
        GET_FRAME()->SetStatusText(msg, 1);
        SetCursor(*wxCROSS_CURSOR);
    }

    // button pressed?
    if (!event.LeftIsDown())
    {
        m_status = MOUSE_NOACTION;
    }
    else if (i >= 0 && i < m_life->GetWidth() &&
             j >= 0 && j < m_life->GetHeight())
    {
        bool alive = m_life->IsAlive(i, j);

        // if just pressed, update status
        if (m_status == MOUSE_NOACTION)
            m_status = (alive? MOUSE_ERASING : MOUSE_DRAWING);

        // toggle cell and refresh if needed
        if (((m_status == MOUSE_ERASING) && alive) ||
            ((m_status == MOUSE_DRAWING) && !alive))
        {
            wxRect rect(x, y, m_cellsize + 1, m_cellsize + 1);
            DrawCell( m_life->SetCell(i, j, !alive) );
            Refresh(FALSE, &rect);
        }
    }
}

void LifeCanvas::OnSize(wxSizeEvent& event)
{
    wxCoord w = event.GetSize().GetX();
    wxCoord h = event.GetSize().GetY();
    m_xoffset = (w > m_width)?  ((w - m_width) / 2)  : 0;
    m_yoffset = (h > m_height)? ((h - m_height) / 2) : 0;

    // allow default processing
    event.Skip();
}
