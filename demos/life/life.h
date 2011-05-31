/////////////////////////////////////////////////////////////////////////////
// Name:        life.h
// Purpose:     The game of Life, created by J. H. Conway
// Author:      Guillermo Rodriguez Garcia, <guille@iies.es>
// Modified by:
// Created:     Jan/2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000, Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _LIFE_APP_H_
#define _LIFE_APP_H_

#include "wx/minifram.h"

#include "game.h"


// --------------------------------------------------------------------------
// LifeCanvas
// --------------------------------------------------------------------------

// Note that in LifeCanvas, all cell coordinates are
// named i, j, while screen coordinates are named x, y.

class LifeCanvas : public wxWindow
{
public:
    // ctor and dtor
    LifeCanvas(wxWindow* parent, Life* life, bool interactive = true);
    virtual ~LifeCanvas();

    // view management
    int  GetCellSize() const { return m_cellsize; };
    void SetCellSize(int cellsize);
    void Recenter(wxInt32 i, wxInt32 j);

    // drawing
    void DrawChanged();
    void DrawCell(wxInt32 i, wxInt32 j, bool alive);

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()

    // draw a cell (parametrized by DC)
    void DrawCell(wxInt32 i, wxInt32 j, wxDC &dc);

    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnMouse(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollWinEvent& event);
    void OnEraseBackground(wxEraseEvent& event);

    // conversion between cell and screen coordinates
    inline wxInt32 XToCell(wxCoord x) const { return (x / m_cellsize) + m_viewportX; };
    inline wxInt32 YToCell(wxCoord y) const { return (y / m_cellsize) + m_viewportY; };
    inline wxCoord CellToX(wxInt32 i) const { return (i - m_viewportX) * m_cellsize; };
    inline wxCoord CellToY(wxInt32 j) const { return (j - m_viewportY) * m_cellsize; };

    // what is the user doing?
    enum MouseStatus
    {
        MOUSE_NOACTION,
        MOUSE_DRAWING,
        MOUSE_ERASING
    };

    Life        *m_life;            // Life object
    int          m_cellsize;        // current cell size, in pixels
    bool         m_interactive;     // is this canvas interactive?
    MouseStatus  m_status;          // what is the user doing?
    wxInt32      m_viewportX;       // first visible cell (x coord)
    wxInt32      m_viewportY;       // first visible cell (y coord)
    wxInt32      m_viewportW;       // number of visible cells (w)
    wxInt32      m_viewportH;       // number of visible cells (h)
    int          m_thumbX;          // horiz. scrollbar thumb position
    int          m_thumbY;          // vert. scrollbar thumb position
    wxInt32      m_mi, m_mj;        // last mouse position
};


// --------------------------------------------------------------------------
// LifeNavigator
// --------------------------------------------------------------------------

class LifeNavigator : public wxMiniFrame
{
public:
    // ctor
    LifeNavigator(wxWindow *parent);

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()

    // event handlers
    void OnClose(wxCloseEvent& event);
};


// --------------------------------------------------------------------------
// LifeFrame
// --------------------------------------------------------------------------

class LifeFrame : public wxFrame
{
public:
    // ctor and dtor
    LifeFrame();
    virtual ~LifeFrame();

    // member functions
    void UpdateInfoText();
    void UpdateUI();

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()

    // event handlers
    void OnMenu(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSamples(wxCommandEvent& event);
    void OnNavigate(wxCommandEvent& event);
    void OnZoom(wxCommandEvent& event);
    void OnSlider(wxScrollEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnClose(wxCloseEvent& event);

    // event handler helpers
    void OnStart();
    void OnStop();
    void OnStep();

    Life           *m_life;
    LifeCanvas     *m_canvas;
    LifeNavigator  *m_navigator;
    wxStaticText   *m_text;
    wxTimer        *m_timer;
    bool            m_running;
    bool            m_topspeed;
    long            m_interval;
    long            m_tics;
};


// --------------------------------------------------------------------------
// LifeApp
// --------------------------------------------------------------------------

class LifeApp : public wxApp
{
public:
    virtual bool OnInit();
};

#endif  // _LIFE_APP_H_
