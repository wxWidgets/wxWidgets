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
// headers, declarations, constants
// ==========================================================================

#ifdef __GNUG__
    #pragma implementation "life.h"
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
    #include "bitmaps/zoomin.xpm"
    #include "bitmaps/zoomout.xpm"
#endif

// --------------------------------------------------------------------------
// constants
// --------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items and toolbar buttons
    ID_RESET = 1001,
    ID_SAMPLES,
    ID_ABOUT,
    ID_EXIT,
    ID_CENTER,
    ID_START,
    ID_STEP,
    ID_STOP,
    ID_ZOOMIN,
    ID_ZOOMOUT,
    ID_TOPSPEED,

    // speed selection slider
    ID_SLIDER
};

// --------------------------------------------------------------------------
// event tables and other macros for wxWindows
// --------------------------------------------------------------------------

// Event tables
BEGIN_EVENT_TABLE(LifeFrame, wxFrame)
    EVT_MENU            (ID_SAMPLES,  LifeFrame::OnSamples)
    EVT_MENU            (ID_RESET,    LifeFrame::OnMenu)
    EVT_MENU            (ID_ABOUT,    LifeFrame::OnMenu)
    EVT_MENU            (ID_EXIT,     LifeFrame::OnMenu)
    EVT_MENU            (ID_CENTER,   LifeFrame::OnMenu)
    EVT_MENU            (ID_START,    LifeFrame::OnMenu)
    EVT_MENU            (ID_STEP,     LifeFrame::OnMenu)
    EVT_MENU            (ID_STOP,     LifeFrame::OnMenu)
    EVT_MENU            (ID_ZOOMIN,   LifeFrame::OnMenu)
    EVT_MENU            (ID_ZOOMOUT,  LifeFrame::OnMenu)
    EVT_MENU            (ID_TOPSPEED, LifeFrame::OnMenu)
    EVT_COMMAND_SCROLL  (ID_SLIDER,   LifeFrame::OnSlider)
    EVT_CLOSE           (             LifeFrame::OnClose)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(LifeCanvas, wxWindow)
    EVT_PAINT           (             LifeCanvas::OnPaint)
    EVT_SCROLLWIN       (             LifeCanvas::OnScroll)
    EVT_SIZE            (             LifeCanvas::OnSize)
    EVT_MOUSE_EVENTS    (             LifeCanvas::OnMouse)
    EVT_ERASE_BACKGROUND(             LifeCanvas::OnEraseBackground)
END_EVENT_TABLE()


// Create a new application object
IMPLEMENT_APP(LifeApp)


// ==========================================================================
// implementation
// ==========================================================================

// some shortcuts
#define ADD_TOOL(id, bmp, tooltip, help)     \
    toolBar->AddTool(id, bmp, wxNullBitmap, FALSE, -1, -1, (wxObject *)0, tooltip, help)

#define GET_FRAME() ((LifeFrame *) wxGetApp().GetTopWindow())

// --------------------------------------------------------------------------
// LifeApp
// --------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
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

    menuFile->Append(ID_RESET, _("Reset"), _("Start a new game"));
    menuFile->Append(ID_SAMPLES, _("Sample game..."), _("Select a sample configuration"));
    menuFile->AppendSeparator();
    menuFile->Append(ID_ABOUT, _("&About...\tCtrl-A"), _("Show about dialog"));
    menuFile->AppendSeparator();
    menuFile->Append(ID_EXIT, _("E&xit\tAlt-X"), _("Quit this program"));

    menuGame->Append(ID_CENTER, _("Re&center\tCtrl-C"), _("Go to (0, 0)"));
    menuGame->Append(ID_START, _("&Start\tCtrl-S"), _("Start"));
    menuGame->Append(ID_STEP, _("&Next\tCtrl-N"), _("Single step"));
    menuGame->Append(ID_STOP, _("S&top\tCtrl-T"), _("Stop"));
    menuGame->Enable(ID_STOP, FALSE);
    menuGame->AppendSeparator();
    menuGame->Append(ID_TOPSPEED, _("Top speed!"), _("Go as fast as possible"));
    menuGame->AppendSeparator();
    menuGame->Append(ID_ZOOMIN, _("Zoom &in\tCtrl-I"));
    menuGame->Append(ID_ZOOMOUT, _("Zoom &out\tCtrl-O"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _("&File"));
    menuBar->Append(menuGame, _("&Game"));
    SetMenuBar(menuBar);

    // tool bar
    wxBitmap tbBitmaps[5];

    tbBitmaps[0] = wxBITMAP(reset);
    tbBitmaps[1] = wxBITMAP(play);
    tbBitmaps[2] = wxBITMAP(stop);
    tbBitmaps[3] = wxBITMAP(zoomin);
    tbBitmaps[4] = wxBITMAP(zoomout);

    wxToolBar *toolBar = CreateToolBar();
    toolBar->SetMargins(5, 5);
    toolBar->SetToolBitmapSize(wxSize(16, 16));
    ADD_TOOL(ID_RESET, tbBitmaps[0], _("Reset"), _("Start a new game"));
    ADD_TOOL(ID_START, tbBitmaps[1], _("Start"), _("Start"));
    ADD_TOOL(ID_STOP, tbBitmaps[2], _("Stop"), _("Stop"));
    toolBar->AddSeparator();
    ADD_TOOL(ID_ZOOMIN, tbBitmaps[3], _("Zoom in"), _("Zoom in"));
    ADD_TOOL(ID_ZOOMOUT, tbBitmaps[4], _("Zoom out"), _("Zoom out"));
    toolBar->Realize();
    toolBar->EnableTool(ID_STOP, FALSE);    // must be after Realize() !

    // status bar
    CreateStatusBar(2);
    SetStatusText(_("Welcome to Life!"));

    // game and canvas
    wxPanel *panel = new wxPanel(this, -1);
    m_life         = new Life();
    m_canvas       = new LifeCanvas(panel, m_life);
    m_timer        = new LifeTimer();
    m_running      = FALSE;
    m_topspeed     = FALSE;
    m_interval     = 500;
    m_tics         = 0;
    m_text         = new wxStaticText(panel, -1, "");
    UpdateInfoText();

    // speed selection slider
    wxSlider *slider = new wxSlider(panel, ID_SLIDER,
        5, 1, 10,
        wxDefaultPosition,
        wxSize(200, -1),
        wxSL_HORIZONTAL | wxSL_AUTOTICKS);

    // component layout
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticLine(panel, -1), 0, wxGROW | wxCENTRE);
    sizer->Add(m_canvas, 1, wxGROW | wxCENTRE | wxALL, 2);
    sizer->Add(new wxStaticLine(panel, -1), 0, wxGROW | wxCENTRE);
    sizer->Add(m_text, 0, wxCENTRE | wxTOP, 4);
    sizer->Add(slider, 0, wxCENTRE | wxALL, 4);
    panel->SetSizer(sizer);
    panel->SetAutoLayout(TRUE);
    sizer->Fit(this);
    sizer->SetSizeHints(this);
}

LifeFrame::~LifeFrame()
{
    delete m_timer;
}

void LifeFrame::UpdateInfoText()
{
    wxString msg;

    msg.Printf(_("  Generation: %u (T: %u ms),  Population: %u  "),
               m_tics,
               m_topspeed? 0 : m_interval,
               m_life->GetNumCells());
    m_text->SetLabel(msg);
}

// Enable or disable tools and menu entries according to the current
// state. See also wxEVT_UPDATE_UI events for a slightly different
// way to do this.
void LifeFrame::UpdateUI()
{
    // start / stop
    GetToolBar()->EnableTool(ID_START, !m_running);
    GetToolBar()->EnableTool(ID_STOP,  m_running);
    GetMenuBar()->GetMenu(1)->Enable(ID_START, !m_running);
    GetMenuBar()->GetMenu(1)->Enable(ID_STEP,  !m_running);
    GetMenuBar()->GetMenu(1)->Enable(ID_STOP,  m_running);

    // zooming
    int cellsize = m_canvas->GetCellSize();
    GetToolBar()->EnableTool(ID_ZOOMIN,  cellsize < 32);
    GetToolBar()->EnableTool(ID_ZOOMOUT, cellsize > 1);
    GetMenuBar()->GetMenu(1)->Enable(ID_ZOOMIN,  cellsize < 32);
    GetMenuBar()->GetMenu(1)->Enable(ID_ZOOMOUT, cellsize > 1);
}

// event handlers
void LifeFrame::OnMenu(wxCommandEvent& event)
{
    switch (event.GetId())
    {
        case ID_CENTER  : m_canvas->Recenter(0, 0); break;
        case ID_START   : OnStart(); break;
        case ID_STEP    : OnTimer(); break;
        case ID_STOP    : OnStop(); break;
        case ID_ZOOMIN  :
        {
            int cellsize = m_canvas->GetCellSize();
            if (cellsize < 32)
            {
                m_canvas->SetCellSize(cellsize * 2);
                UpdateUI();
            }
            break;
        }
        case ID_ZOOMOUT :
        {
            int cellsize = m_canvas->GetCellSize();
            if (cellsize > 1)
            {
                m_canvas->SetCellSize(cellsize / 2);
                UpdateUI();
            }
            break;
        }
        case ID_TOPSPEED:
        {
            m_running = TRUE;
            m_topspeed = TRUE;
            UpdateUI();
            while (m_running && m_topspeed)           
            {
                OnTimer();
                wxYield();
            }
            break;
        }
        case ID_RESET:
        {
            // stop if it was running
            OnStop();
            m_life->Clear();
            m_canvas->Recenter(0, 0);
            m_tics = 0;
            UpdateInfoText();
            break;
        }
        case ID_ABOUT:
        {
            LifeAboutDialog dialog(this);
            dialog.ShowModal();
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

void LifeFrame::OnClose(wxCloseEvent& WXUNUSED(event))
{
    // Stop if it was running; this is absolutely needed because
    // the frame won't be actually destroyed until there are no
    // more pending events, and this in turn won't ever happen
    // if the timer is running faster than the window can redraw.
    OnStop();
    Destroy();   
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
        const LifeShape shape = dialog.GetShape();

        // put the shape
        m_life->Clear();
        m_life->SetShape(shape);

        // recenter canvas
        m_canvas->Recenter(0, 0);
        m_tics = 0;
        UpdateInfoText();
    }
}

void LifeFrame::OnStart()
{
    if (!m_running)
    {
        m_timer->Start(m_interval);
        m_running = TRUE;
        UpdateUI();
    }
}

void LifeFrame::OnStop()
{
    if (m_running)
    {
        m_timer->Stop();
        m_running = FALSE;
        m_topspeed = FALSE;
        UpdateUI();
    }
}

void LifeFrame::OnTimer()
{
    if (m_life->NextTic())
        m_tics++;
    else
        OnStop();

    m_canvas->DrawChanged();
    UpdateInfoText();
}

void LifeFrame::OnSlider(wxScrollEvent& event)
{
    m_interval = event.GetPosition() * 100;

    if (m_running)
    {
        OnStop();
        OnStart();
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
          : wxWindow(parent, -1, wxPoint(0, 0), wxSize(100, 100),
            wxSUNKEN_BORDER)
{
    m_life        = life;
    m_interactive = interactive;
    m_cellsize    = 8;
    m_status      = MOUSE_NOACTION;
    m_viewportX   = 0;
    m_viewportY   = 0;
    m_viewportH   = 0;
    m_viewportW   = 0;

    if (m_interactive)
        SetCursor(*wxCROSS_CURSOR);

    // reduce flicker if wxEVT_ERASE_BACKGROUND is not available
    SetBackgroundColour(*wxWHITE);       
}

LifeCanvas::~LifeCanvas()
{
    delete m_life;   
}

// recenter at the given position
void LifeCanvas::Recenter(wxInt32 i, wxInt32 j)
{
    m_viewportX = i - m_viewportW / 2;
    m_viewportY = j - m_viewportH / 2;

    // redraw everything
    Refresh(FALSE);
}

// set the cell size and refresh display
void LifeCanvas::SetCellSize(int cellsize)
{
    m_cellsize = cellsize;

    // find current center
    wxInt32 cx = m_viewportX + m_viewportW / 2;
    wxInt32 cy = m_viewportY + m_viewportH / 2;

    // get current canvas size and adjust viewport accordingly
    wxCoord w, h;
    GetClientSize(&w, &h);
    m_viewportW = (w + m_cellsize - 1) / m_cellsize;
    m_viewportH = (h + m_cellsize - 1) / m_cellsize;
    
    // recenter
    m_viewportX = cx - m_viewportW / 2;
    m_viewportY = cy - m_viewportH / 2;

    // adjust scrollbars
    if (m_interactive)
    {
        SetScrollbar(wxHORIZONTAL, m_viewportW, m_viewportW, 3 * m_viewportW);
        SetScrollbar(wxVERTICAL,   m_viewportH, m_viewportH, 3 * m_viewportH);
        m_thumbX = m_viewportW;
        m_thumbY = m_viewportH;
    }
    
    Refresh(FALSE);
}

// draw a cell
void LifeCanvas::DrawCell(wxInt32 i, wxInt32 j, bool alive)
{
    wxClientDC dc(this);

    dc.SetPen(alive? *wxBLACK_PEN : *wxWHITE_PEN);
    dc.SetBrush(alive? *wxBLACK_BRUSH : *wxWHITE_BRUSH);

    dc.BeginDrawing();
    DrawCell(i, j, dc);
    dc.EndDrawing();
}

void LifeCanvas::DrawCell(wxInt32 i, wxInt32 j, wxDC &dc)
{
    wxCoord x = CellToX(i);
    wxCoord y = CellToY(j);

    // if cellsize is 1 or 2, there will be no grid
    switch (m_cellsize)
    {
        case 1:
            dc.DrawPoint(x, y);
            break;
        case 2:
            dc.DrawRectangle(x, y, 2, 2);
            break;
        default:
            dc.DrawRectangle(x + 1, y + 1, m_cellsize - 1, m_cellsize - 1);
    }
}

// draw all changed cells
void LifeCanvas::DrawChanged()
{
    wxClientDC dc(this);

    size_t ncells;
    Cell *cells;
    bool done = FALSE;

    m_life->BeginFind(m_viewportX,
                      m_viewportY,
                      m_viewportX + m_viewportW,
                      m_viewportY + m_viewportH,
                      TRUE);
   
    dc.BeginDrawing();
    dc.SetLogicalFunction(wxINVERT);

    if (m_cellsize == 1)
    {
        // drawn using DrawPoint
        dc.SetPen(*wxBLACK_PEN);
    }
    else
    {
        // drawn using DrawRectangle
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(*wxBLACK_BRUSH);
    }

    while (!done)
    {
        done = m_life->FindMore(&cells, &ncells);

        for (size_t m = 0; m < ncells; m++)
            DrawCell(cells[m].i, cells[m].j, dc);
    }
    dc.EndDrawing();
}

// event handlers
void LifeCanvas::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    wxRect  rect = GetUpdateRegion().GetBox();
    wxCoord x, y, w, h;
    wxInt32 i0, j0, i1, j1;

    // find damaged area
    x = rect.GetX();
    y = rect.GetY();
    w = rect.GetWidth();
    h = rect.GetHeight();

    i0 = XToCell(x);
    j0 = YToCell(y);
    i1 = XToCell(x + w - 1);
    j1 = YToCell(y + h - 1);

    size_t ncells;
    Cell *cells;
    bool done = FALSE;

    m_life->BeginFind(i0, j0, i1, j1, FALSE);
    done = m_life->FindMore(&cells, &ncells);

    // erase all damaged cells and draw the grid
    dc.BeginDrawing();
    dc.SetBrush(*wxWHITE_BRUSH);

    if (m_cellsize <= 2)
    {
       // no grid
       dc.SetPen(*wxWHITE_PEN);
       dc.DrawRectangle(x, y, w, h);
    }
    else
    {
        x = CellToX(i0);
        y = CellToY(j0);
        w = CellToX(i1 + 1) - x + 1;
        h = CellToY(j1 + 1) - y + 1;

        dc.SetPen(*wxLIGHT_GREY_PEN);
        for (wxInt32 yy = y; yy <= (y + h - m_cellsize); yy += m_cellsize)
            dc.DrawRectangle(x, yy, w, m_cellsize + 1);
        for (wxInt32 xx = x; xx <= (x + w - m_cellsize); xx += m_cellsize)
            dc.DrawLine(xx, y, xx, y + h);
    }

    // draw all alive cells
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxBLACK_BRUSH);

    while (!done)
    {
        for (size_t m = 0; m < ncells; m++)
            DrawCell(cells[m].i, cells[m].j, dc);

        done = m_life->FindMore(&cells, &ncells);
    }

    // last set
    for (size_t m = 0; m < ncells; m++)
        DrawCell(cells[m].i, cells[m].j, dc);

    dc.EndDrawing();
}

void LifeCanvas::OnMouse(wxMouseEvent& event)
{
    if (!m_interactive)
        return;

    // which cell are we pointing at?
    wxInt32 i = XToCell( event.GetX() );
    wxInt32 j = YToCell( event.GetY() );

    // set statusbar text
    wxString msg;
    msg.Printf(_("Cell: (%d, %d)"), i, j);
    GET_FRAME()->SetStatusText(msg, 1);

    // button pressed?
    if (!event.LeftIsDown())
    {
        m_status = MOUSE_NOACTION;
        return;
    }

    // button just pressed?
    if (m_status == MOUSE_NOACTION)
    {
        // yes, update status and toggle this cell
        m_status = (m_life->IsAlive(i, j)? MOUSE_ERASING : MOUSE_DRAWING);
            
        m_mi = i;
        m_mj = j;
        m_life->SetCell(i, j, m_status == MOUSE_DRAWING);
        DrawCell(i, j, m_status == MOUSE_DRAWING);
    }
    else if ((m_mi != i) || (m_mj != j))
    {
        // draw a line of cells using Bresenham's algorithm
        wxInt32 d, ii, jj, di, ai, si, dj, aj, sj;
        di = i - m_mi;
        ai = abs(di) << 1;
        si = (di < 0)? -1 : 1;
        dj = j - m_mj;
        aj = abs(dj) << 1;
        sj = (dj < 0)? -1 : 1;

        ii = m_mi;
        jj = m_mj;
  
        if (ai > aj)
        {
            // iterate over i
            d = aj - (ai >> 1);        
               
            while (ii != i)
            {
                m_life->SetCell(ii, jj, m_status == MOUSE_DRAWING);
                DrawCell(ii, jj, m_status == MOUSE_DRAWING);
                if (d >= 0)
                {
                    jj += sj;
                    d  -= ai;   
                }
                ii += si;
                d  += aj;
            }
        }
        else
        {
            // iterate over j
            d = ai - (aj >> 1);

            while (jj != j)
            {
                m_life->SetCell(ii, jj, m_status == MOUSE_DRAWING);
                DrawCell(ii, jj, m_status == MOUSE_DRAWING);
                if (d >= 0)
                {
                    ii += si;
                    d  -= aj;   
                }
                jj += sj;
                d  += ai;
            }
        }

        // last cell
        m_life->SetCell(ii, jj, m_status == MOUSE_DRAWING);
        DrawCell(ii, jj, m_status == MOUSE_DRAWING);
        m_mi = ii;
        m_mj = jj;
    }

    GET_FRAME()->UpdateInfoText();
}

void LifeCanvas::OnSize(wxSizeEvent& event)
{
    // find center
    wxInt32 cx = m_viewportX + m_viewportW / 2;
    wxInt32 cy = m_viewportY + m_viewportH / 2;

    // get new size
    wxCoord w = event.GetSize().GetX();
    wxCoord h = event.GetSize().GetY();
    m_viewportW = (w + m_cellsize - 1) / m_cellsize;
    m_viewportH = (h + m_cellsize - 1) / m_cellsize;

    // recenter
    m_viewportX = cx - m_viewportW / 2;
    m_viewportY = cy - m_viewportH / 2;

    // scrollbars
    if (m_interactive)
    {
        SetScrollbar(wxHORIZONTAL, m_viewportW, m_viewportW, 3 * m_viewportW);
        SetScrollbar(wxVERTICAL,   m_viewportH, m_viewportH, 3 * m_viewportH);
        m_thumbX = m_viewportW;
        m_thumbY = m_viewportH;
    }

    // allow default processing
    event.Skip();
}

void LifeCanvas::OnScroll(wxScrollWinEvent& event)
{
    WXTYPE type = event.GetEventType();
    int pos     = event.GetPosition();
    int orient  = event.GetOrientation();

    // calculate scroll increment
    int scrollinc = 0;
    switch (type)
    {
        case wxEVT_SCROLLWIN_TOP:
        {
            if (orient == wxHORIZONTAL)
                scrollinc = -m_viewportW;
            else
                scrollinc = -m_viewportH;
            break;    
        }
        case wxEVT_SCROLLWIN_BOTTOM:
        {
            if (orient == wxHORIZONTAL)
                scrollinc = m_viewportW;
            else
                scrollinc = m_viewportH;
            break;    
        }
        case wxEVT_SCROLLWIN_LINEUP:   scrollinc = -1; break;
        case wxEVT_SCROLLWIN_LINEDOWN: scrollinc = +1; break;
        case wxEVT_SCROLLWIN_PAGEUP:   scrollinc = -10; break;
        case wxEVT_SCROLLWIN_PAGEDOWN: scrollinc = +10; break;
        case wxEVT_SCROLLWIN_THUMBTRACK:
        {
            if (orient == wxHORIZONTAL)
            {
                scrollinc = pos - m_thumbX;
                m_thumbX = pos;
            }
            else
            {
                scrollinc = pos - m_thumbY;
                m_thumbY = pos;
            }
            break;
        }
        case wxEVT_SCROLLWIN_THUMBRELEASE:
        {
            m_thumbX = m_viewportW;
            m_thumbY = m_viewportH;
        }
    }

#ifdef __WXGTK__ // what about Motif?
    // wxGTK updates the thumb automatically (wxMSW doesn't); reset it back
    if (type != wxEVT_SCROLLWIN_THUMBTRACK)
    {
        SetScrollbar(wxHORIZONTAL, m_viewportW, m_viewportW, 3 * m_viewportW);
        SetScrollbar(wxVERTICAL,   m_viewportH, m_viewportH, 3 * m_viewportH);
    }
#endif

    if (scrollinc == 0) return;
    
    // scroll the window and adjust the viewport
    if (orient == wxHORIZONTAL)
    {
        m_viewportX += scrollinc;
        ScrollWindow( -m_cellsize * scrollinc, 0, (const wxRect *) NULL);
    }
    else
    {
        m_viewportY += scrollinc;    
        ScrollWindow( 0, -m_cellsize * scrollinc, (const wxRect *) NULL);
    }
}

void LifeCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // do nothing. I just don't want the background to be erased, you know.
}
