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

// minimum and maximum table size, in each dimension
#define LIFE_MIN 20
#define LIFE_MAX 200

// some shortcuts
#define ADD_TOOL(a, b, c, d) \
    toolBar->AddTool(a, b, wxNullBitmap, FALSE, -1, -1, (wxObject *)0, c, d)

#define GET_FRAME() \
    ((LifeFrame *) wxGetApp().GetTopWindow())

// --------------------------------------------------------------------------
// headers
// --------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "life.cpp"
    #pragma interface "life.cpp"
#endif

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/statline.h"
#include "wx/spinctrl.h"

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
// classes
// --------------------------------------------------------------------------

class Life;
class LifeShape;
class LifeCanvas;
class LifeTimer;
class LifeFrame;
class LifeApp;
class LifeNewGameDialog;
class LifeSamplesDialog;

// --------------------------------------------------------------------------
// non-GUI classes
// --------------------------------------------------------------------------

// Life
class Life
{
public:
    // ctors and dtors
    Life(int width, int height);
    ~Life();
    void Create(int width, int height);
    void Destroy();

    // accessors
    inline int  GetWidth()  const { return m_width; };
    inline int  GetHeight() const { return m_height; };
    inline bool IsAlive(int x, int y) const;
    inline bool HasChanged(int x, int y) const;

    // flags
    void SetBorderWrap(bool on);

    // game logic
    void Clear();
    void SetCell(int x, int y, bool alive = TRUE);
    void SetShape(LifeShape &shape);
    bool NextTic();

private:
    enum CellFlags {
        CELL_DEAD    = 0x0000,  // is dead
        CELL_ALIVE   = 0x0001,  // is alive
        CELL_MARK    = 0x0002,  // will change / has changed
    };
    typedef int Cell;

    int GetNeighbors(int x, int y) const;
    inline void SetCell(int x, int y, Cell status);

    int   m_width;
    int   m_height;
    Cell *m_cells;
    bool  m_wrap;
};

// LifeShape
class LifeShape
{
public:
    LifeShape::LifeShape(wxString name,
                         wxString desc,
                         int width, int height, char *data,
                         int fieldWidth = 20, int fieldHeight = 20,
                         bool wrap = TRUE)
    {
        m_name        = name;
        m_desc        = desc;
        m_width       = width;
        m_height      = height;
        m_data        = data;
        m_fieldWidth  = fieldWidth;
        m_fieldHeight = fieldHeight;
        m_wrap        = wrap;
    }

    wxString  m_name;
    wxString  m_desc;
    int       m_width;
    int       m_height;
    char     *m_data;
    int       m_fieldWidth;
    int       m_fieldHeight;
    bool      m_wrap;
};

// --------------------------------------------------------------------------
// GUI classes
// --------------------------------------------------------------------------

// Life canvas
class LifeCanvas : public wxScrolledWindow
{
public:
    // ctor and dtor
    LifeCanvas(wxWindow* parent, Life* life, bool interactive = TRUE);
    ~LifeCanvas();

    // member functions
    void Reset();
    void DrawEverything(bool force = FALSE);
    void DrawCell(int i, int j);
    void DrawCell(int i, int j, wxDC &dc);
    inline int CellToCoord(int i) const { return (i * m_cellsize); };
    inline int CoordToCell(int x) const { return ((x >= 0)? (x / m_cellsize) : -1); };

    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnMouse(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()

    enum MouseStatus {
        MOUSE_NOACTION,
        MOUSE_DRAWING,
        MOUSE_ERASING
    };

    Life        *m_life;
    wxBitmap    *m_bmp;
    int          m_height;
    int          m_width;
    int          m_cellsize;
    wxCoord      m_xoffset;
    wxCoord      m_yoffset;
    MouseStatus  m_status;
    bool         m_interactive;
};

// Life timer
class LifeTimer : public wxTimer
{
public:
    void Notify();
};

// Life main frame
class LifeFrame : public wxFrame
{
public:
    // ctor and dtor
    LifeFrame();
    ~LifeFrame();

    // member functions
    void UpdateInfoText();

    // event handlers
    void OnMenu(wxCommandEvent& event);
    void OnNewGame(wxCommandEvent& event);
    void OnSamples(wxCommandEvent& event);
    void OnStart();
    void OnStop();
    void OnTimer();
    void OnSlider(wxScrollEvent& event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()

    Life         *m_life;
    LifeTimer    *m_timer;
    LifeCanvas   *m_canvas;
    wxStaticText *m_text;
    bool          m_running;
    long          m_interval;
    long          m_tics;
};

// Life new game dialog
class LifeNewGameDialog : public wxDialog
{
public:
    // ctor
    LifeNewGameDialog(wxWindow *parent, int *w, int *h);

    // event handlers
    void OnOK(wxCommandEvent& event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();

    int        *m_w;
    int        *m_h;
    wxSpinCtrl *m_spinctrlw;
    wxSpinCtrl *m_spinctrlh;
};

// Life sample configurations dialog
class LifeSamplesDialog : public wxDialog
{
public:
    // ctor and dtor
    LifeSamplesDialog(wxWindow *parent);
    ~LifeSamplesDialog();

    // members
    int GetValue();

    // event handlers
    void OnListBox(wxCommandEvent &event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();

    int         m_value;
    wxListBox  *m_list;
    wxTextCtrl *m_text;
    LifeCanvas *m_canvas;
    Life       *m_life;
};

// Life app
class LifeApp : public wxApp
{
public:
    virtual bool OnInit();
};

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
    ID_SLIDER,

    // listbox in samples dialog
    ID_LISTBOX
};


// built-in sample games
#include "samples.inc"

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

BEGIN_EVENT_TABLE(LifeNewGameDialog, wxDialog)
    EVT_BUTTON         (wxID_OK,    LifeNewGameDialog::OnOK)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(LifeSamplesDialog, wxDialog)
    EVT_LISTBOX        (ID_LISTBOX, LifeSamplesDialog::OnListBox)
END_EVENT_TABLE()


// Create a new application object
IMPLEMENT_APP(LifeApp)

// ==========================================================================
// implementation
// ==========================================================================

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

    // panel
    wxPanel *panel = new wxPanel(this, -1);

    // game
    m_life     = new Life(20, 20);
    m_canvas   = new LifeCanvas(panel, m_life);
    m_timer    = new LifeTimer();
    m_interval = 500;
    m_tics     = 0;
    m_text     = new wxStaticText(panel, -1, "");
    UpdateInfoText();

    // slider
    wxSlider *slider = new wxSlider(panel, ID_SLIDER, 5, 1, 10,
        wxDefaultPosition, wxSize(200, -1), wxSL_HORIZONTAL | wxSL_AUTOTICKS);

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
    int result;

    // stop if it was running
    OnStop();

    // show dialog box
    LifeNewGameDialog dialog(this, &w, &h);
    result = dialog.ShowModal();

    // new game?
    if (result == wxID_OK)
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

    // show dialog box
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
    for (int j = 0; j < m_life->GetWidth(); j++)
        for (int i = 0; i < m_life->GetHeight(); i++)
            if (force || m_life->HasChanged(i, j))
                DrawCell(i, j, dc);

    // bounding rectangle (always drawn - better than clipping region)
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(0, 0, m_width, m_height);

    dc.EndDrawing();
    dc.SelectObject(wxNullBitmap);
}

void LifeCanvas::DrawCell(int i, int j)
{
    wxMemoryDC dc;

    dc.SelectObject(*m_bmp);
    dc.BeginDrawing();

    dc.SetClippingRegion(1, 1, m_width - 2, m_height - 2);
    DrawCell(i, j, dc);

    dc.EndDrawing();
    dc.SelectObject(wxNullBitmap);
}

void LifeCanvas::DrawCell(int i, int j, wxDC &dc)
{
    if (m_life->IsAlive(i, j))
    {
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBrush(*wxBLACK_BRUSH);
        dc.DrawRectangle(CellToCoord(i),
                         CellToCoord(j),
                         m_cellsize,
                         m_cellsize);
    }
    else
    {
        dc.SetPen(*wxLIGHT_GREY_PEN);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(CellToCoord(i),
                         CellToCoord(j),
                         m_cellsize,
                         m_cellsize);
        dc.SetPen(*wxWHITE_PEN);
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.DrawRectangle(CellToCoord(i) + 1,
                         CellToCoord(j) + 1,
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
            m_life->SetCell(i, j, !alive);
            DrawCell(i, j);
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

// --------------------------------------------------------------------------
// LifeNewGameDialog
// --------------------------------------------------------------------------

LifeNewGameDialog::LifeNewGameDialog(wxWindow *parent, int *w, int *h)
                 : wxDialog(parent, -1,
                            _("New game"),
                            wxDefaultPosition,
                            wxDefaultSize,
                            wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL)
{
    m_w = w;
    m_h = h;

    // prompts and text controls
    wxString strw, strh;
    strw.Printf(_("%u"), *m_w);
    strh.Printf(_("%u"), *m_h);
    m_spinctrlw = new wxSpinCtrl( this, -1, strw );
    m_spinctrlh = new wxSpinCtrl( this, -1, strh );

    // component layout
    wxBoxSizer *inputsizer1 = new wxBoxSizer( wxHORIZONTAL );
    inputsizer1->Add( new wxStaticText(this, -1, _("Width")), 1, wxCENTRE | wxLEFT, 20);
    inputsizer1->Add( m_spinctrlw, 2, wxCENTRE | wxLEFT | wxRIGHT, 20 );

    wxBoxSizer *inputsizer2 = new wxBoxSizer( wxHORIZONTAL );
    inputsizer2->Add( new wxStaticText(this, -1, _("Height")), 1, wxCENTRE | wxLEFT, 20);
    inputsizer2->Add( m_spinctrlh, 2, wxCENTRE | wxLEFT | wxRIGHT, 20 );

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
    topsizer->Add( CreateTextSizer(_("Enter board dimensions")), 0, wxALL, 10 );
    topsizer->Add( new wxStaticLine(this, -1), 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    topsizer->Add( inputsizer1, 1, wxGROW | wxLEFT | wxRIGHT, 5 );
    topsizer->Add( inputsizer2, 1, wxGROW | wxLEFT | wxRIGHT, 5 );
    topsizer->Add( new wxStaticLine(this, -1), 0, wxGROW | wxLEFT | wxRIGHT | wxTOP, 10);
    topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxCENTRE | wxALL, 10);

    // activate
    SetSizer(topsizer);
    SetAutoLayout(TRUE);
    topsizer->SetSizeHints(this);
    topsizer->Fit(this);
    Centre(wxBOTH);
}

void LifeNewGameDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    *m_w = m_spinctrlw->GetValue();
    *m_h = m_spinctrlh->GetValue();

    EndModal(wxID_OK);
}

// --------------------------------------------------------------------------
// LifeSamplesDialog
// --------------------------------------------------------------------------

LifeSamplesDialog::LifeSamplesDialog(wxWindow *parent)
                 : wxDialog(parent, -1,
                            _("Sample games"),
                            wxDefaultPosition,
                            wxDefaultSize,
                            wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL)
{
    m_value = 0;
   
    // create and populate the list of available samples
    m_list = new wxListBox( this, ID_LISTBOX,
        wxDefaultPosition,
        wxDefaultSize,
        0, NULL,
        wxLB_SINGLE | wxLB_NEEDED_SB | wxLB_HSCROLL );

    for (unsigned i = 0; i < (sizeof(g_shapes) / sizeof(LifeShape)); i++)
        m_list->Append(g_shapes[i].m_name);

    // descriptions
    wxStaticBox *statbox = new wxStaticBox( this, -1, _("Description"));
    m_life   = new Life( 16, 16 );
    m_life->SetShape(g_shapes[0]);
    m_canvas = new LifeCanvas( this, m_life, FALSE );
    m_text   = new wxTextCtrl( this, -1,
        g_shapes[0].m_desc,
        wxDefaultPosition,
        wxSize(300, 60),
        wxTE_MULTILINE | wxTE_READONLY);

    // layout components
    wxStaticBoxSizer *sizer1 = new wxStaticBoxSizer( statbox, wxVERTICAL );
    sizer1->Add( m_canvas, 2, wxGROW | wxCENTRE | wxALL, 5);
    sizer1->Add( m_text, 1, wxGROW | wxCENTRE | wxALL, 5 );

    wxBoxSizer *sizer2 = new wxBoxSizer( wxHORIZONTAL );
    sizer2->Add( m_list, 0, wxGROW | wxCENTRE | wxALL, 5 );
    sizer2->Add( sizer1, 1, wxGROW | wxCENTRE | wxALL, 5 );

    wxBoxSizer *sizer3 = new wxBoxSizer( wxVERTICAL );
    sizer3->Add( CreateTextSizer(_("Select one configuration")), 0, wxALL, 10 );
    sizer3->Add( new wxStaticLine(this, -1), 0, wxGROW | wxLEFT | wxRIGHT, 10 );
    sizer3->Add( sizer2, 1, wxGROW | wxCENTRE | wxALL, 5 );
    sizer3->Add( new wxStaticLine(this, -1), 0, wxGROW | wxLEFT | wxRIGHT, 10 );
    sizer3->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxCENTRE | wxALL, 10 );

    // activate
    SetSizer(sizer3);
    SetAutoLayout(TRUE);
    sizer3->SetSizeHints(this);
    sizer3->Fit(this);
    Centre(wxBOTH);
}

LifeSamplesDialog::~LifeSamplesDialog()
{
    m_canvas->Destroy();
    delete m_life;
}

int LifeSamplesDialog::GetValue()
{
    return m_value;
}

void LifeSamplesDialog::OnListBox(wxCommandEvent& event)
{
    if (event.GetSelection() != -1)
    {
        m_value = m_list->GetSelection();
        m_text->SetValue(g_shapes[ event.GetSelection() ].m_desc);
        m_life->SetShape(g_shapes[ event.GetSelection() ]);

        m_canvas->DrawEverything(TRUE);     // force redraw everything
        m_canvas->Refresh(FALSE);           // do not erase background
    }
}

// --------------------------------------------------------------------------
// Life
// --------------------------------------------------------------------------

Life::Life(int width, int height)
{            
    m_wrap   = TRUE;
    m_cells  = NULL;
    Create(width, height);
}

Life::~Life()
{
    Destroy();
}

void Life::Create(int width, int height)
{
    wxASSERT(width > 0 && height > 0);

    m_width  = width;
    m_height = height;
    m_cells  = new Cell[m_width * m_height];
    Clear();
}

void Life::Destroy()
{
    delete[] m_cells;
}

void Life::Clear()
{
    for (int i = 0; i < m_width * m_height; i++)
        m_cells[i] = CELL_DEAD;
}

bool Life::IsAlive(int x, int y) const
{
    wxASSERT(x >= 0 && y >= 0 && x < m_width && y < m_height);

    return (m_cells[y * m_width + x] & CELL_ALIVE);
}

bool Life::HasChanged(int x, int y) const
{
    wxASSERT(x >= 0 && y >= 0 && x < m_width && y < m_height);

    return (m_cells[y * m_width + x] & CELL_MARK) != 0;
}

void Life::SetBorderWrap(bool on)
{
    m_wrap = on;
}

void Life::SetCell(int x, int y, bool alive)
{
    wxASSERT(x >= 0 && y >= 0 && x < m_width && y < m_height);

    m_cells[y * m_width + x] = (alive? CELL_ALIVE : CELL_DEAD);
}

void Life::SetShape(LifeShape& shape)
{
    wxASSERT((m_width >= shape.m_width) && (m_height >= shape.m_height));

    int x0  = (m_width - shape.m_width) / 2;
    int y0  = (m_height - shape.m_height) / 2;
    char *p = shape.m_data;

    Clear();
    for (int j = y0; j < y0 + shape.m_height; j++)
        for (int i = x0; i < x0 + shape.m_width; i++)
            SetCell(i, j, *(p++) == '*');
}

bool Life::NextTic()
{
    long changed = 0;
    int  i, j;

    /* 1st pass. Find and mark deaths and births for this generation.
     *
     * Rules:
     *   An organism with <= 1 neighbors will die due to isolation.
     *   An organism with >= 4 neighbors will die due to starvation.
     *   New organisms are born in cells with exactly 3 neighbors.
     */
    for (j = 0; j < m_height; j++)
        for (i = 0; i < m_width; i++)
        {
            int neighbors = GetNeighbors(i, j);
            bool alive    = IsAlive(i, j);

            /* Set CELL_MARK if this cell must change, clear it
             * otherwise. We cannot toggle the CELL_ALIVE bit yet
             * because all deaths and births are simultaneous (it
             * would affect neighbouring cells).
             */
            if ((!alive && neighbors == 3) ||
                (alive && (neighbors <= 1 || neighbors >= 4)))
                m_cells[j * m_width + i] |= CELL_MARK;
            else
                m_cells[j * m_width + i] &= ~CELL_MARK;
        }

    /* 2nd pass. Stabilize.
     */
    for (j = 0; j < m_height; j++)
        for (i = 0; i < m_width; i++)
        {
            /* Toggle CELL_ALIVE for those cells marked in the
             * previous pass. Do not clear the CELL_MARK bit yet;
             * it is useful to know which cells have changed and
             * thus must be updated in the screen.
             */
            if (m_cells[j * m_width + i] & CELL_MARK)
            {
                m_cells[j * m_width + i] ^= CELL_ALIVE;
                changed++;
            }
        }

    return (changed != 0);
}

int Life::GetNeighbors(int x, int y) const
{
    wxASSERT(x >= 0 && y >= 0 && x < m_width && y < m_height);

    int neighbors = 0;

    int i0 = (x)? (x - 1) : 0;
    int j0 = (y)? (y - 1) : 0;
    int i1 = (x < (m_width - 1))? (x + 1) : (m_width - 1);
    int j1 = (y < (m_height - 1))? (y + 1) : (m_height - 1);

    if (m_wrap && ( !x || !y || x == (m_width - 1) || y == (m_height - 1)))
    {
        // this is an outer cell and wraparound is on
        for (int j = y - 1; j <= y + 1; j++)
            for (int i = x - 1; i <= x + 1; i++)
                if (IsAlive( ((i < 0)? (i + m_width ) : (i % m_width)),
                             ((j < 0)? (j + m_height) : (j % m_height)) ))
                     neighbors++;
    }
    else
    {
        // this is an inner cell, or wraparound is off
        for (int j = j0; j <= j1; j++)
            for (int i = i0; i <= i1; i++)
                if (IsAlive(i, j))
                     neighbors++;
    }

    // do not count ourselves
    if (IsAlive(x, y)) neighbors--;

    return neighbors;
}

void Life::SetCell(int x, int y, Cell status)
{
    wxASSERT(x >= 0 && y >= 0 && x < m_width && y < m_height);

    m_cells[y * m_width + x] = status;
}

