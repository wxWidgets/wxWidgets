/////////////////////////////////////////////////////////////////////////////
// Name:        life.cpp
// Purpose:     The game of Life, created by J. H. Conway
// Author:      Guillermo Rodriguez Garcia, <guille@iies.es>
// Modified by:
// Created:     Jan/2000
// Copyright:   (c) 2000, Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ==========================================================================
// headers, declarations, constants
// ==========================================================================

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/statline.h"
#include "wx/wfstream.h"
#include "wx/filedlg.h"
#include "wx/stockitem.h"
#include "wx/dcbuffer.h"

#include "life.h"
#include "game.h"
#include "dialogs.h"
#include "reader.h"

// --------------------------------------------------------------------------
// resources
// --------------------------------------------------------------------------

#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__) || defined(__WXQT__)
    // application icon
    #include "mondrian.xpm"

    // bitmap buttons for the toolbar
    #include "bitmaps/reset.xpm"
    #include "bitmaps/open.xpm"
    #include "bitmaps/play.xpm"
    #include "bitmaps/stop.xpm"
    #include "bitmaps/zoomin.xpm"
    #include "bitmaps/zoomout.xpm"
    #include "bitmaps/info.xpm"

    // navigator
    #include "bitmaps/north.xpm"
    #include "bitmaps/south.xpm"
    #include "bitmaps/east.xpm"
    #include "bitmaps/west.xpm"
    #include "bitmaps/center.xpm"
#endif

// --------------------------------------------------------------------------
// constants
// --------------------------------------------------------------------------

// IDs for the controls and the menu commands. Exluding those already defined
// by wxWidgets, such as wxID_NEW.
enum
{
    // timer
    ID_TIMER = wxID_HIGHEST,

    // file menu
    ID_SAMPLES,

    // view menu
    ID_SHOWNAV,
    ID_ORIGIN,
    ID_CENTER,
    ID_NORTH,
    ID_SOUTH,
    ID_EAST,
    ID_WEST,
    ID_INFO,

    // game menu
    ID_START,
    ID_STEP,
    ID_TOPSPEED,

    // speed selection slider
    ID_SLIDER
};

// --------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// --------------------------------------------------------------------------

// Event tables
wxBEGIN_EVENT_TABLE(LifeFrame, wxFrame)
    EVT_MENU            (wxID_NEW,     LifeFrame::OnMenu)
#if wxUSE_FILEDLG
    EVT_MENU            (wxID_OPEN,    LifeFrame::OnOpen)
#endif
    EVT_MENU            (ID_SAMPLES,   LifeFrame::OnSamples)
    EVT_MENU            (wxID_ABOUT,   LifeFrame::OnMenu)
    EVT_MENU            (wxID_EXIT,    LifeFrame::OnMenu)
    EVT_MENU            (ID_SHOWNAV,   LifeFrame::OnMenu)
    EVT_MENU            (ID_ORIGIN,    LifeFrame::OnNavigate)
    EVT_BUTTON          (ID_CENTER,    LifeFrame::OnNavigate)
    EVT_BUTTON          (ID_NORTH,     LifeFrame::OnNavigate)
    EVT_BUTTON          (ID_SOUTH,     LifeFrame::OnNavigate)
    EVT_BUTTON          (ID_EAST,      LifeFrame::OnNavigate)
    EVT_BUTTON          (ID_WEST,      LifeFrame::OnNavigate)
    EVT_MENU            (wxID_ZOOM_IN, LifeFrame::OnZoom)
    EVT_MENU            (wxID_ZOOM_OUT,LifeFrame::OnZoom)
    EVT_MENU            (ID_INFO,      LifeFrame::OnMenu)
    EVT_MENU            (ID_START,     LifeFrame::OnMenu)
    EVT_MENU            (ID_STEP,      LifeFrame::OnMenu)
    EVT_MENU            (wxID_STOP,    LifeFrame::OnMenu)
    EVT_MENU            (ID_TOPSPEED,  LifeFrame::OnMenu)
    EVT_COMMAND_SCROLL  (ID_SLIDER,    LifeFrame::OnSlider)
    EVT_TIMER           (ID_TIMER,     LifeFrame::OnTimer)
    EVT_CLOSE           (              LifeFrame::OnClose)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(LifeNavigator, wxMiniFrame)
    EVT_CLOSE           (             LifeNavigator::OnClose)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(LifeCanvas, wxWindow)
    EVT_PAINT           (             LifeCanvas::OnPaint)
    EVT_SCROLLWIN       (             LifeCanvas::OnScroll)
    EVT_SIZE            (             LifeCanvas::OnSize)
    EVT_MOTION          (             LifeCanvas::OnMouse)
    EVT_LEFT_DOWN       (             LifeCanvas::OnMouse)
    EVT_LEFT_UP         (             LifeCanvas::OnMouse)
    EVT_LEFT_DCLICK     (             LifeCanvas::OnMouse)
    EVT_ERASE_BACKGROUND(             LifeCanvas::OnEraseBackground)
wxEND_EVENT_TABLE()


// Create a new application object
wxIMPLEMENT_APP(LifeApp);


// ==========================================================================
// implementation
// ==========================================================================

// some shortcuts
#define ADD_TOOL(id, bmp, tooltip, help) \
    toolBar->AddTool(id, wxEmptyString, bmp, wxNullBitmap, wxITEM_NORMAL, tooltip, help)


// --------------------------------------------------------------------------
// LifeApp
// --------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool LifeApp::OnInit()
{
    // create the main application window
    LifeFrame *frame = new LifeFrame();

    // show it
    frame->Show(true);

    // just for Motif
#ifdef __WXMOTIF__
    frame->UpdateInfoText();
#endif

    // enter the main message loop and run the app
    return true;
}

// --------------------------------------------------------------------------
// LifeFrame
// --------------------------------------------------------------------------

// frame constructor
LifeFrame::LifeFrame() :
  wxFrame( (wxFrame *) NULL, wxID_ANY, _("Life!"), wxDefaultPosition ),
  m_navigator(NULL)
{
    // frame icon
    SetIcon(wxICON(mondrian));

    // menu bar
    wxMenu *menuFile = new wxMenu(wxMENU_TEAROFF);
    wxMenu *menuView = new wxMenu(wxMENU_TEAROFF);
    wxMenu *menuGame = new wxMenu(wxMENU_TEAROFF);
    wxMenu *menuHelp = new wxMenu(wxMENU_TEAROFF);

    menuFile->Append(wxID_NEW, wxEmptyString, _("Start a new game"));
#if wxUSE_FILEDLG
    menuFile->Append(wxID_OPEN, wxEmptyString, _("Open an existing Life pattern"));
#endif
    menuFile->Append(ID_SAMPLES, _("&Sample game..."), _("Select a sample configuration"));
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    menuView->Append(ID_SHOWNAV, _("Navigation &toolbox"), _("Show or hide toolbox"), wxITEM_CHECK);
    menuView->Check(ID_SHOWNAV, true);
    menuView->AppendSeparator();

    menuView->Append(ID_ORIGIN, _("&Absolute origin"), _("Go to (0, 0)"));
    menuView->Append(ID_CENTER, _("&Center of mass"), _("Find center of mass"));
    menuView->Append(ID_NORTH, _("&North"), _("Find northernmost cell"));
    menuView->Append(ID_SOUTH, _("&South"), _("Find southernmost cell"));
    menuView->Append(ID_EAST, _("&East"), _("Find easternmost cell"));
    menuView->Append(ID_WEST, _("&West"), _("Find westernmost cell"));
    menuView->AppendSeparator();
    menuView->Append(wxID_ZOOM_IN, wxEmptyString, _("Zoom in"));
    menuView->Append(wxID_ZOOM_OUT, wxEmptyString, _("Zoom out"));
    menuView->Append(ID_INFO, _("&Description\tCtrl-D"), _("View pattern description"));

    menuGame->Append(ID_START, _("&Start\tCtrl-S"), _("Start"));
    menuGame->Append(ID_STEP, _("&Next\tCtrl-N"), _("Single step"));
    menuGame->Append(wxID_STOP, wxEmptyString, _("Stop"));
    menuGame->Enable(wxID_STOP, false);
    menuGame->AppendSeparator();
    menuGame->Append(ID_TOPSPEED, _("T&op speed!"), _("Go as fast as possible"));

    menuHelp->Append(wxID_ABOUT, _("&About\tCtrl-A"), _("Show about dialog"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _("&File"));
    menuBar->Append(menuView, _("&View"));
    menuBar->Append(menuGame, _("&Game"));
    menuBar->Append(menuHelp, _("&Help"));
    SetMenuBar(menuBar);

    // tool bar
    wxBitmap tbBitmaps[7];

    tbBitmaps[0] = wxBITMAP(reset);
    tbBitmaps[1] = wxBITMAP(open);
    tbBitmaps[2] = wxBITMAP(zoomin);
    tbBitmaps[3] = wxBITMAP(zoomout);
    tbBitmaps[4] = wxBITMAP(info);
    tbBitmaps[5] = wxBITMAP(play);
    tbBitmaps[6] = wxBITMAP(stop);

    wxToolBar *toolBar = CreateToolBar();
    toolBar->SetMargins(5, 5);
    toolBar->SetToolBitmapSize(wxSize(16, 16));

    ADD_TOOL(wxID_NEW, tbBitmaps[0], wxGetStockLabel(wxID_NEW, wxSTOCK_NOFLAGS), _("Start a new game"));
#if wxUSE_FILEDLG
    ADD_TOOL(wxID_OPEN, tbBitmaps[1], wxGetStockLabel(wxID_OPEN, wxSTOCK_NOFLAGS), _("Open an existing Life pattern"));
#endif // wxUSE_FILEDLG

    toolBar->AddSeparator();
    ADD_TOOL(wxID_ZOOM_IN, tbBitmaps[2], wxGetStockLabel(wxID_ZOOM_IN, wxSTOCK_NOFLAGS), _("Zoom in"));
    ADD_TOOL(wxID_ZOOM_OUT, tbBitmaps[3], wxGetStockLabel(wxID_ZOOM_OUT, wxSTOCK_NOFLAGS), _("Zoom out"));
    ADD_TOOL(ID_INFO, tbBitmaps[4], _("Description"), _("Show description"));
    toolBar->AddSeparator();
    ADD_TOOL(ID_START, tbBitmaps[5], _("Start"), _("Start"));
    ADD_TOOL(wxID_STOP, tbBitmaps[6], _("Stop"), _("Stop"));

    toolBar->Realize();
    toolBar->EnableTool(wxID_STOP, false);    // must be after Realize() !

#if wxUSE_STATUSBAR
    // status bar
    CreateStatusBar(2);
    SetStatusText(_("Welcome to Life!"));
#endif // wxUSE_STATUSBAR

    // game and timer
    m_life     = new Life();
    m_timer    = new wxTimer(this, ID_TIMER);
    m_running  = false;
    m_topspeed = false;
    m_interval = 500;
    m_tics     = 0;

    // We use two different panels to reduce flicker in wxGTK, because
    // some widgets (like wxStaticText) don't have their own X11 window,
    // and thus updating the text would result in a refresh of the canvas
    // if they belong to the same parent.

    wxPanel *panel1 = new wxPanel(this, wxID_ANY);
    wxPanel *panel2 = new wxPanel(this, wxID_ANY);

    // canvas
    m_canvas = new LifeCanvas(panel1, m_life);

    // info panel
    m_text = new wxStaticText(panel2, wxID_ANY,
        wxEmptyString,
        wxDefaultPosition,
        wxDefaultSize,
        wxALIGN_CENTER | wxST_NO_AUTORESIZE);

    wxSlider *slider = new wxSlider(panel2, ID_SLIDER,
        5, 1, 10,
        wxDefaultPosition,
        wxSize(200, wxDefaultCoord),
        wxSL_HORIZONTAL | wxSL_AUTOTICKS);

    UpdateInfoText();

    // component layout
    wxBoxSizer *sizer1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *sizer2 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *sizer3 = new wxBoxSizer(wxVERTICAL);

#if wxUSE_STATLINE
    sizer1->Add( new wxStaticLine(panel1, wxID_ANY), 0, wxGROW );
#endif // wxUSE_STATLINE
    sizer1->Add( m_canvas, 1, wxGROW | wxALL, 2 );
#if wxUSE_STATLINE
    sizer1->Add( new wxStaticLine(panel1, wxID_ANY), 0, wxGROW );
#endif // wxUSE_STATLINE
    panel1->SetSizer( sizer1 );
    sizer1->Fit( panel1 );

    sizer2->Add( m_text, 0, wxGROW | wxTOP, 4 );
    sizer2->Add( slider, 0, wxCENTRE | wxALL, 4 );

    panel2->SetSizer( sizer2 );
    sizer2->Fit( panel2 );

    sizer3->Add( panel1, 1, wxGROW );
    sizer3->Add( panel2, 0, wxGROW );
    SetSizer( sizer3 );

    sizer3->Fit( this );

    // set minimum frame size
    sizer3->SetSizeHints( this );

    // navigator frame - not appropriate for small devices
    m_navigator = new LifeNavigator(this);

}

LifeFrame::~LifeFrame()
{
    delete m_timer;
}

void LifeFrame::UpdateInfoText()
{
    wxString msg;

    msg.Printf(_(" Generation: %lu (T: %lu ms),  Population: %lu "),
               m_tics,
               m_topspeed? 0 : m_interval,
               static_cast<unsigned long>(m_life->GetNumCells()));
    m_text->SetLabel(msg);
}

// Enable or disable tools and menu entries according to the current
// state. See also wxEVT_UPDATE_UI events for a slightly different
// way to do this.
void LifeFrame::UpdateUI()
{
    // start / stop
    GetToolBar()->EnableTool(ID_START, !m_running);
    GetToolBar()->EnableTool(wxID_STOP,  m_running);
    GetMenuBar()->Enable(ID_START, !m_running);
    GetMenuBar()->Enable(ID_STEP,  !m_running);
    GetMenuBar()->Enable(wxID_STOP,  m_running);
    GetMenuBar()->Enable(ID_TOPSPEED, !m_topspeed);

    // zooming
    int cellsize = m_canvas->GetCellSize();
    GetToolBar()->EnableTool(wxID_ZOOM_IN,  cellsize < 32);
    GetToolBar()->EnableTool(wxID_ZOOM_OUT, cellsize > 1);
    GetMenuBar()->Enable(wxID_ZOOM_IN,  cellsize < 32);
    GetMenuBar()->Enable(wxID_ZOOM_OUT, cellsize > 1);
}

// Event handlers -----------------------------------------------------------

// OnMenu handles all events which don't have their own event handler
void LifeFrame::OnMenu(wxCommandEvent& event)
{
    switch (event.GetId())
    {
        case wxID_NEW:
        {
            // stop if it was running
            OnStop();
            m_life->Clear();
            m_canvas->Recenter(0, 0);
            m_tics = 0;
            UpdateInfoText();
            break;
        }
        case wxID_ABOUT:
        {
            LifeAboutDialog dialog(this);
            dialog.ShowModal();
            break;
        }
        case wxID_EXIT:
        {
            // true is to force the frame to close
            Close(true);
            break;
        }
        case ID_SHOWNAV:
        {
            bool checked = GetMenuBar()->GetMenu(1)->IsChecked(ID_SHOWNAV);
            if (m_navigator)
                m_navigator->Show(checked);
            break;
        }
        case ID_INFO:
        {
            wxString desc = m_life->GetDescription();

            if ( desc.empty() )
                desc = _("Not available");

            // should we make the description editable here?
            wxMessageBox(desc, _("Description"), wxOK | wxICON_INFORMATION);

            break;
        }
        case ID_START   : OnStart(); break;
        case ID_STEP    : OnStep(); break;
        case wxID_STOP  : OnStop(); break;
        case ID_TOPSPEED:
        {
            m_running = true;
            m_topspeed = true;
            UpdateUI();

            const long YIELD_INTERVAL = 1000 / 30;
            wxMilliClock_t lastyield = 0, now;

            while (m_running && m_topspeed)
            {
                OnStep();
                if ( (now=wxGetLocalTimeMillis()) - lastyield > YIELD_INTERVAL)
                {
                    wxYield();
                    lastyield = now;
                }
            }

            break;
        }
    }
}

#if wxUSE_FILEDLG
void LifeFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog filedlg(this,
                         _("Choose a file to open"),
                         wxEmptyString,
                         wxEmptyString,
                         _("Life patterns (*.lif)|*.lif|All files (*.*)|*.*"),
                         wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (filedlg.ShowModal() == wxID_OK)
    {
        wxFileInputStream stream(filedlg.GetPath());
        LifeReader reader(stream);

        // the reader handles errors itself, no need to do anything here
        if (reader.IsOk())
        {
            // stop if running and put the pattern
            OnStop();
            m_life->Clear();
            m_life->SetPattern(reader.GetPattern());

            // recenter canvas
            m_canvas->Recenter(0, 0);
            m_tics = 0;
            UpdateInfoText();
        }
    }
}
#endif

void LifeFrame::OnSamples(wxCommandEvent& WXUNUSED(event))
{
    // stop if it was running
    OnStop();

    // dialog box
    LifeSamplesDialog dialog(this);

    if (dialog.ShowModal() == wxID_OK)
    {
        const LifePattern pattern = dialog.GetPattern();

        // put the pattern
        m_life->Clear();
        m_life->SetPattern(pattern);

        // recenter canvas
        m_canvas->Recenter(0, 0);
        m_tics = 0;
        UpdateInfoText();
    }
}

void LifeFrame::OnZoom(wxCommandEvent& event)
{
    int cellsize = m_canvas->GetCellSize();

    if ((event.GetId() == wxID_ZOOM_IN) && cellsize < 32)
    {
        m_canvas->SetCellSize(cellsize * 2);
        UpdateUI();
    }
    else if ((event.GetId() == wxID_ZOOM_OUT) && cellsize > 1)
    {
        m_canvas->SetCellSize(cellsize / 2);
        UpdateUI();
    }
}

void LifeFrame::OnNavigate(wxCommandEvent& event)
{
    LifeCell c;

    switch (event.GetId())
    {
        case ID_NORTH:  c = m_life->FindNorth(); break;
        case ID_SOUTH:  c = m_life->FindSouth(); break;
        case ID_WEST:   c = m_life->FindWest(); break;
        case ID_EAST:   c = m_life->FindEast(); break;
        case ID_CENTER: c = m_life->FindCenter(); break;
        default :
            wxFAIL;
            wxFALLTHROUGH;
        case ID_ORIGIN: c.i = c.j = 0; break;
    }

    m_canvas->Recenter(c.i, c.j);
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

void LifeFrame::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    OnStep();
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

void LifeFrame::OnStart()
{
    if (!m_running)
    {
        m_timer->Start(m_interval);
        m_running = true;
        UpdateUI();
    }
}

void LifeFrame::OnStop()
{
    if (m_running)
    {
        m_timer->Stop();
        m_running = false;
        m_topspeed = false;
        UpdateUI();
    }
}

void LifeFrame::OnStep()
{
    if (m_life->NextTic())
    {
        m_tics++;
        m_canvas->Refresh();
        UpdateInfoText();
    }
    else
        OnStop();
}


// --------------------------------------------------------------------------
// LifeNavigator miniframe
// --------------------------------------------------------------------------

LifeNavigator::LifeNavigator(wxWindow *parent)
             : wxMiniFrame(parent, wxID_ANY,
                           _("Navigation"),
                           wxDefaultPosition,
                           wxDefaultSize,
                           wxCAPTION | wxSIMPLE_BORDER)
{
    wxPanel    *panel  = new wxPanel(this, wxID_ANY);
    wxBoxSizer *sizer1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *sizer2 = new wxBoxSizer(wxHORIZONTAL);

    // create bitmaps and masks for the buttons
    wxBitmap
        bmpn = wxBITMAP(north),
        bmpw = wxBITMAP(west),
        bmpc = wxBITMAP(center),
        bmpe = wxBITMAP(east),
        bmps = wxBITMAP(south);

#if !defined(__WXGTK__) && !defined(__WXMOTIF__) && !defined(__WXMAC__)
    bmpn.SetMask(new wxMask(bmpn, *wxLIGHT_GREY));
    bmpw.SetMask(new wxMask(bmpw, *wxLIGHT_GREY));
    bmpc.SetMask(new wxMask(bmpc, *wxLIGHT_GREY));
    bmpe.SetMask(new wxMask(bmpe, *wxLIGHT_GREY));
    bmps.SetMask(new wxMask(bmps, *wxLIGHT_GREY));
#endif

    // create the buttons and attach tooltips to them
    wxBitmapButton
        *bn = new wxBitmapButton(panel, ID_NORTH,  bmpn),
        *bw = new wxBitmapButton(panel, ID_WEST ,  bmpw),
        *bc = new wxBitmapButton(panel, ID_CENTER, bmpc),
        *be = new wxBitmapButton(panel, ID_EAST ,  bmpe),
        *bs = new wxBitmapButton(panel, ID_SOUTH,  bmps);

#if wxUSE_TOOLTIPS
    bn->SetToolTip(_("Find northernmost cell"));
    bw->SetToolTip(_("Find westernmost cell"));
    bc->SetToolTip(_("Find center of mass"));
    be->SetToolTip(_("Find easternmost cell"));
    bs->SetToolTip(_("Find southernmost cell"));
#endif

    // add buttons to sizers
    sizer2->Add( bw, 0, wxCENTRE | wxWEST,  4 );
    sizer2->Add( bc, 0, wxCENTRE);
    sizer2->Add( be, 0, wxCENTRE | wxEAST,  4 );
    sizer1->Add( bn, 0, wxCENTRE | wxNORTH, 4 );
    sizer1->Add( sizer2 );
    sizer1->Add( bs, 0, wxCENTRE | wxSOUTH, 4 );

    // set the panel and miniframe size
    panel->SetSizer(sizer1);

    sizer1->Fit(panel);
    SetClientSize(panel->GetSize());
    wxSize sz = GetSize();
    SetSizeHints(sz.x, sz.y, sz.x, sz.y);

    // move it to a sensible position
    wxRect parentRect = parent->GetRect();
    wxSize childSize  = GetSize();
    int x = parentRect.GetX() +
            parentRect.GetWidth();
    int y = parentRect.GetY() +
            (parentRect.GetHeight() - childSize.GetHeight()) / 4;
    Move(x, y);

    // done
    Show(true);
}

void LifeNavigator::OnClose(wxCloseEvent& event)
{
    // avoid if we can
    if (event.CanVeto())
        event.Veto();
    else
        Destroy();
}


// --------------------------------------------------------------------------
// LifeCanvas
// --------------------------------------------------------------------------

// canvas constructor
LifeCanvas::LifeCanvas(wxWindow *parent, Life *life, bool interactive)
          : wxWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(100, 100),
            wxFULL_REPAINT_ON_RESIZE | wxHSCROLL | wxVSCROLL
            |wxSUNKEN_BORDER
            )
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
    SetBackgroundStyle(wxBG_STYLE_PAINT);
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
    Refresh(false);
}

// set the cell size and refresh display
void LifeCanvas::SetCellSize(int cellsize)
{
    m_cellsize = cellsize;

    // find current center
    wxInt32 cx = m_viewportX + m_viewportW / 2;
    wxInt32 cy = m_viewportY + m_viewportH / 2;

    // get current canvas size and adjust viewport accordingly
    int w, h;
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

    Refresh(false);
}

// draw a cell
void LifeCanvas::DrawCell(wxInt32 i, wxInt32 j, bool alive)
{
    wxClientDC dc(this);

    dc.SetPen(alive? *wxBLACK_PEN : *wxWHITE_PEN);
    dc.SetBrush(alive? *wxBLACK_BRUSH : *wxWHITE_BRUSH);

    DrawCell(i, j, dc);
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

// draw all changed cells, currently not in use
void LifeCanvas::DrawChanged()
{
    wxClientDC dc(this);

    size_t ncells;
    LifeCell *cells;
    bool done = false;

    m_life->BeginFind(m_viewportX,
                      m_viewportY,
                      m_viewportX + m_viewportW,
                      m_viewportY + m_viewportH,
                      true);

    if (m_cellsize == 1)
    {
        dc.SetPen(*wxWHITE_PEN);
    }
    else
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(*wxWHITE_BRUSH);
    }
    dc.SetLogicalFunction(wxXOR);

    while (!done)
    {
        done = m_life->FindMore(&cells, &ncells);

        for (size_t m = 0; m < ncells; m++)
            DrawCell(cells[m].i, cells[m].j, dc);
    }
}

// event handlers
void LifeCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxAutoBufferedPaintDC dc(this);
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
    LifeCell *cells;

    m_life->BeginFind(i0, j0, i1, j1, false);
    bool done = m_life->FindMore(&cells, &ncells);

    // erase all damaged cells and draw the grid
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
}

void LifeCanvas::OnMouse(wxMouseEvent& event)
{
    if (!m_interactive)
        return;

    // which cell are we pointing at?
    wxInt32 i = XToCell( event.GetX() );
    wxInt32 j = YToCell( event.GetY() );

#if wxUSE_STATUSBAR
    // set statusbar text
    wxString msg;
    msg.Printf(_("Cell: (%d, %d)"), i, j);
    ((LifeFrame *) wxGetApp().GetTopWindow())->SetStatusText(msg, 1);
#endif // wxUSE_STATUSBAR

    // NOTE that wxMouseEvent::LeftDown() and wxMouseEvent::LeftIsDown()
    // have different semantics. The first one is used to signal that the
    // button was just pressed (i.e., in "button down" events); the second
    // one just describes the current status of the button, independently
    // of the mouse event type. LeftIsDown is typically used in "mouse
    // move" events, to test if the button is _still_ pressed.

    // is the button down?
    if (!event.LeftIsDown())
    {
        m_status = MOUSE_NOACTION;
        return;
    }

    // was it pressed just now?
    if (event.LeftDown())
    {
        // yes: start a new action and toggle this cell
        m_status = (m_life->IsAlive(i, j)? MOUSE_ERASING : MOUSE_DRAWING);

        m_mi = i;
        m_mj = j;
        m_life->SetCell(i, j, m_status == MOUSE_DRAWING);
        DrawCell(i, j, m_status == MOUSE_DRAWING);
    }
    else if ((m_mi != i) || (m_mj != j))
    {
        // no: continue ongoing action
        bool alive = (m_status == MOUSE_DRAWING);

        // prepare DC and pen + brush to optimize drawing
        wxClientDC dc(this);
        dc.SetPen(alive? *wxBLACK_PEN : *wxWHITE_PEN);
        dc.SetBrush(alive? *wxBLACK_BRUSH : *wxWHITE_BRUSH);

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
                m_life->SetCell(ii, jj, alive);
                DrawCell(ii, jj, dc);
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
                m_life->SetCell(ii, jj, alive);
                DrawCell(ii, jj, dc);
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
        m_life->SetCell(ii, jj, alive);
        DrawCell(ii, jj, dc);
        m_mi = ii;
        m_mj = jj;
    }

    ((LifeFrame *) wxGetApp().GetTopWindow())->UpdateInfoText();
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
    WXTYPE type = (WXTYPE)event.GetEventType();
    int pos     = event.GetPosition();
    int orient  = event.GetOrientation();

    // calculate scroll increment
    int scrollinc = 0;
    if (type == wxEVT_SCROLLWIN_TOP)
    {
        if (orient == wxHORIZONTAL)
            scrollinc = -m_viewportW;
        else
            scrollinc = -m_viewportH;
    }
    else
    if (type == wxEVT_SCROLLWIN_BOTTOM)
    {
        if (orient == wxHORIZONTAL)
            scrollinc = m_viewportW;
        else
            scrollinc = m_viewportH;
    }
    else
    if (type == wxEVT_SCROLLWIN_LINEUP)
    {
        scrollinc = -1;
    }
    else
    if (type == wxEVT_SCROLLWIN_LINEDOWN)
    {
        scrollinc = +1;
    }
    else
    if (type == wxEVT_SCROLLWIN_PAGEUP)
    {
        scrollinc = -10;
    }
    else
    if (type == wxEVT_SCROLLWIN_PAGEDOWN)
    {
        scrollinc = +10;
    }
    else
    if (type == wxEVT_SCROLLWIN_THUMBTRACK)
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
    }
    else
    if (type == wxEVT_SCROLLWIN_THUMBRELEASE)
    {
        m_thumbX = m_viewportW;
        m_thumbY = m_viewportH;
    }

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    // wxGTK and wxMotif update the thumb automatically (wxMSW doesn't);
    // so reset it back as we always want it to be in the same position.
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
