/////////////////////////////////////////////////////////////////////////////
// Name:        life.h
// Purpose:     The game of life, created by J. H. Conway
// Author:      Guillermo Rodriguez Garcia, <guille@iies.es>
// Modified by:
// Created:     Jan/2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000, Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _LIFE_APP_H_
#define _LIFE_APP_H_

#ifdef __GNUG__
    #pragma interface "life.h"
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

#include "game.h"

// --------------------------------------------------------------------------
// LifeCanvas
// --------------------------------------------------------------------------

class LifeCanvas : public wxScrolledWindow
{
public:
    // ctor and dtor
    LifeCanvas(wxWindow* parent, Life* life, bool interactive = TRUE);
    ~LifeCanvas();

    // member functions
    void Reset();
    void DrawEverything(bool force = FALSE);
    void DrawCell(Cell c);
    void DrawCell(Cell c, wxDC &dc);
    inline int CellToCoord(int i) const { return (i * m_cellsize); };
    inline int CoordToCell(int x) const { return ((x >= 0)? (x / m_cellsize) : -1); };

    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnMouse(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()

    enum MouseStatus
    {
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

// --------------------------------------------------------------------------
// LifeTimer
// --------------------------------------------------------------------------

// Life timer
class LifeTimer : public wxTimer
{
public:
    void Notify();
};

// --------------------------------------------------------------------------
// LifeFrame
// --------------------------------------------------------------------------

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

// --------------------------------------------------------------------------
// LifeApp
// --------------------------------------------------------------------------

class LifeApp : public wxApp
{
public:
    virtual bool OnInit();
};

#endif  // _LIFE_APP_H_
