/////////////////////////////////////////////////////////////////////////////
// Name:        plot.h
// Purpose:     wxPlotWindow
// Author:      Robert Roebling
// Modified by:
// Created:     12/1/2000
// Copyright:   (c) Robert Roebling
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PLOT_H_
#define _WX_PLOT_H_

#ifdef __GNUG__
#pragma interface "plot.h"
#endif

#include "wx/scrolwin.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxPlotCurve;
class wxPlotArea;
class wxPlotWindow;

//-----------------------------------------------------------------------------
// wxPlotCurve
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPlotCurve: public wxObject
{
public:
    wxPlotCurve( int offsetY, double startY, double endY );
    
    virtual wxInt32 GetStartX() = 0;
    virtual wxInt32 GetEndX() = 0;

    virtual double GetY( wxInt32 x ) = 0;

    void SetStartY( double startY )
        { m_startY = startY; }
    double GetStartY()
        { return m_startY; }
    void SetEndY( double endY )
        { m_endY = endY; }
    double GetEndY()
        { return m_endY; }
    void SetOffsetY( int offsetY )
       { m_offsetY = offsetY; }
    int GetOffsetY()
       { return m_offsetY; }
    
private:
    int     m_offsetY;
    double  m_startY;
    double  m_endY;

    DECLARE_ABSTRACT_CLASS(wxPlotCurve)
};

//-----------------------------------------------------------------------------
// wxPlotArea
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPlotArea: public wxWindow
{
public:
    wxPlotArea() {}
    wxPlotArea( wxPlotWindow *parent );
    
    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );

    void DrawCurve( wxDC *dc, wxPlotCurve *curve, int from = -1, int to = -1 );
    void DeleteCurve( wxPlotCurve *curve, int from = -1, int to = -1 );
    
private:
    wxPlotWindow   *m_owner;

    DECLARE_CLASS(wxPlotArea)
    DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxPlotWindow
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPlotWindow: public wxScrolledWindow
{
public:
    wxPlotWindow() {}
    wxPlotWindow( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, int flags );
    ~wxPlotWindow();
    
    void Add( wxPlotCurve *curve );
    size_t GetCount();
    wxPlotCurve *GetAt( size_t n );

    void SetCurrent( wxPlotCurve* current );
    wxPlotCurve *GetCurrent();
    
    void Move( wxPlotCurve* curve, int pixels_up );
    void Enlarge( wxPlotCurve *curve, double factor );

    void OnMoveUp( wxCommandEvent& event );
    void OnMoveDown( wxCommandEvent& event );
    
    void OnEnlarge100( wxCommandEvent& event );
    void OnEnlarge50( wxCommandEvent& event );
    void OnShrink50( wxCommandEvent& event );
    void OnShrink33( wxCommandEvent& event );
    
    void OnPaint( wxPaintEvent &event );

    void RedrawYAxis();
    
private:
    friend wxPlotArea;
    
    wxList         m_curves;
    wxPlotArea    *m_area;
    wxPlotCurve   *m_current;

    DECLARE_CLASS(wxPlotWindow)
    DECLARE_EVENT_TABLE()
};

#endif
   // _WX_PLOT_H_
