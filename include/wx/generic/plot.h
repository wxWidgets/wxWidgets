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

#include "wx/defs.h"

#if wxUSE_PLOT

#include "wx/scrolwin.h"
#include "wx/event.h"
#include "wx/dynarray.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPlotEvent;
class WXDLLEXPORT wxPlotCurve;
class WXDLLEXPORT wxPlotValues;
class WXDLLEXPORT wxPlotArea;
class WXDLLEXPORT wxPlotXAxisArea;
class WXDLLEXPORT wxPlotYAxisArea;
class WXDLLEXPORT wxPlotWindow;

//-----------------------------------------------------------------------------
// consts
//-----------------------------------------------------------------------------

#define wxPLOT_X_AXIS          0x0004
#define wxPLOT_Y_AXIS          0x0008
#define wxPLOT_BUTTON_MOVE     0x0010
#define wxPLOT_BUTTON_ZOOM     0x0020
#define wxPLOT_BUTTON_ENLARGE  0x0040

#define wxPLOT_BUTTON_ALL  (wxPLOT_BUTTON_MOVE|wxPLOT_BUTTON_ZOOM|wxPLOT_BUTTON_ENLARGE)
#define wxPLOT_DEFAULT     (wxPLOT_X_AXIS|wxPLOT_Y_AXIS | wxPLOT_BUTTON_ALL)

//-----------------------------------------------------------------------------
// wxPlotEvent
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPlotEvent: public wxNotifyEvent
{
public:
    wxPlotEvent( wxEventType commandType = wxEVT_NULL, int id = 0 );

    wxPlotCurve *GetCurve()
        { return m_curve; }
    void SetCurve( wxPlotCurve *curve )
        { m_curve = curve; }

    double GetZoom()
        { return m_zoom; }
    void SetZoom( double zoom )
        { m_zoom = zoom; }

    wxInt32 GetPosition()
        { return m_position; }
    void SetPosition( wxInt32 pos )
        { m_position = pos; }

private:
    wxPlotCurve   *m_curve;
    double         m_zoom;
    wxInt32        m_position;
};

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
       
    void SetPenNormal( const wxPen &pen )
       { m_penNormal = pen; }
    void SetPenSelected( const wxPen &pen )
       { m_penSelected = pen; }

private:
    int     m_offsetY;
    double  m_startY;
    double  m_endY;
    wxPen   m_penNormal;
    wxPen   m_penSelected;

    DECLARE_ABSTRACT_CLASS(wxPlotCurve)
};

//-----------------------------------------------------------------------------
// wxPlotOnOffCurve
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPlotOnOff
{
public:
    wxPlotOnOff() { }

    wxInt32   m_on;
    wxInt32   m_off;
    void     *m_clientData;
};

WX_DECLARE_EXPORTED_OBJARRAY(wxPlotOnOff, wxArrayPlotOnOff);

class WXDLLEXPORT wxPlotOnOffCurve: public wxObject
{
public:
    wxPlotOnOffCurve( int offsetY );

    wxInt32 GetStartX()
        { return m_minX; }
    wxInt32 GetEndX()
        { return m_maxX; }

    void SetOffsetY( int offsetY )
       { m_offsetY = offsetY; }
    int GetOffsetY()
       { return m_offsetY; }
       
    void Add( wxInt32 on, wxInt32 off, void *clientData = NULL );
    size_t GetCount();
    
    wxInt32 GetOn( size_t index );
    wxInt32 GetOff( size_t index );
    void* GetClientData( size_t index );
    wxPlotOnOff *GetAt( size_t index );

    virtual void DrawOnLine( wxDC &dc, wxCoord y, wxCoord start, wxCoord end, void *clientData );
    virtual void DrawOffLine( wxDC &dc, wxCoord y, wxCoord start, wxCoord end );

private:
    int               m_offsetY;
    wxInt32           m_minX;
    wxInt32           m_maxX;
    
    wxArrayPlotOnOff   m_marks;

    DECLARE_CLASS(wxPlotOnOffCurve)
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
    void DrawOnOffCurve( wxDC *dc, wxPlotOnOffCurve *curve, int from = -1, int to = -1 );
    void DeleteCurve( wxPlotCurve *curve, int from = -1, int to = -1 );

    virtual void ScrollWindow( int dx, int dy, const wxRect *rect );

private:
    wxPlotWindow     *m_owner;
    bool              m_zooming;

    DECLARE_CLASS(wxPlotArea)
    DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxPlotXAxisArea
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPlotXAxisArea: public wxWindow
{
public:
    wxPlotXAxisArea() {}
    wxPlotXAxisArea( wxPlotWindow *parent );

    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );

private:
    wxPlotWindow   *m_owner;

    DECLARE_CLASS(wxPlotXAxisArea)
    DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxPlotYAxisArea
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPlotYAxisArea: public wxWindow
{
public:
    wxPlotYAxisArea() {}
    wxPlotYAxisArea( wxPlotWindow *parent );

    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );

private:
    wxPlotWindow   *m_owner;

    DECLARE_CLASS(wxPlotYAxisArea)
    DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxPlotWindow
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPlotWindow: public wxScrolledWindow
{
public:
    wxPlotWindow() {}
    wxPlotWindow( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, int flags = wxPLOT_DEFAULT );
    ~wxPlotWindow();

    // curve accessors
    // ---------------

    void Add( wxPlotCurve *curve );
    void Delete( wxPlotCurve* curve );

    size_t GetCount();
    wxPlotCurve *GetAt( size_t n );

    void SetCurrent( wxPlotCurve* current );
    wxPlotCurve *GetCurrent();

    // mark list accessors
    // -------------------

    void Add( wxPlotOnOffCurve *curve );
    void Delete( wxPlotOnOffCurve* curve );
    
    size_t GetOnOffCurveCount();
    wxPlotOnOffCurve *GetOnOffCurveAt( size_t n );

    // vertical representation
    // -----------------------

    void Move( wxPlotCurve* curve, int pixels_up );
    void Enlarge( wxPlotCurve *curve, double factor );

    // horizontal representation
    // -------------------------

    void SetUnitsPerValue( double upv );
    double GetUnitsPerValue()
        { return m_xUnitsPerValue; }

    void SetZoom( double zoom );
    double GetZoom()
        { return m_xZoom; }

    // options
    // -------

    void SetScrollOnThumbRelease( bool scrollOnThumbRelease = TRUE )
        { m_scrollOnThumbRelease = scrollOnThumbRelease; }
    bool GetScrollOnThumbRelease()
        { return m_scrollOnThumbRelease; }

    void SetEnlargeAroundWindowCentre( bool enlargeAroundWindowCentre = TRUE )
        { m_enlargeAroundWindowCentre = enlargeAroundWindowCentre; }
    bool GetEnlargeAroundWindowCentre()
        { return m_enlargeAroundWindowCentre; }

    // events (may be overridden)
    // --------------------------

    void OnMoveUp( wxCommandEvent& event );
    void OnMoveDown( wxCommandEvent& event );

    void OnEnlarge( wxCommandEvent& event );
    void OnShrink( wxCommandEvent& event );
    void OnZoomIn( wxCommandEvent& event );
    void OnZoomOut( wxCommandEvent& event );

    void OnScroll2( wxScrollWinEvent& event );

    // utilities
    // ---------

    void RedrawEverything();
    void RedrawXAxis();
    void RedrawYAxis();

    void ResetScrollbar();

private:
    friend wxPlotArea;
    friend wxPlotXAxisArea;
    friend wxPlotYAxisArea;

    double             m_xUnitsPerValue;
    double             m_xZoom;

    wxList             m_curves;
    wxList             m_onOffCurves;
    
    wxPlotArea        *m_area;
    wxPlotXAxisArea   *m_xaxis;
    wxPlotYAxisArea   *m_yaxis;
    wxPlotCurve       *m_current;

    bool               m_scrollOnThumbRelease;
    bool               m_enlargeAroundWindowCentre;

    DECLARE_CLASS(wxPlotWindow)
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// calendar events macros
// ----------------------------------------------------------------------------

#define EVT_PLOT(id, fn) { wxEVT_PLOT_DOUBLECLICKED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_SEL_CHANGING(id, fn) { wxEVT_PLOT_SEL_CHANGING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_SEL_CHANGED(id, fn) { wxEVT_PLOT_SEL_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_CLICKED(id, fn) { wxEVT_PLOT_CLICKED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_DOUBLECLICKED(id, fn) { wxEVT_PLOT_DOUBLECLICKED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_ZOOM_IN(id, fn) { wxEVT_PLOT_ZOOM_IN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_ZOOM_OUT(id, fn) { wxEVT_PLOT_ZOOM_OUT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_VALUE_SEL_CREATING(id, fn) { wxEVT_PLOT_VALUE_SEL_CREATING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_VALUE_SEL_CREATED(id, fn) { wxEVT_PLOT_VALUE_SEL_CREATED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_VALUE_SEL_CHANGING(id, fn) { wxEVT_PLOT_VALUE_SEL_CHANGING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_VALUE_SEL_CHANGED(id, fn) { wxEVT_PLOT_VALUE_SEL_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_AREA_SEL_CREATING(id, fn) { wxEVT_PLOT_AREA_SEL_CREATING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_AREA_SEL_CREATED(id, fn) { wxEVT_PLOT_AREA_SEL_CREATED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_AREA_SEL_CHANGING(id, fn) { wxEVT_PLOT_AREA_SEL_CHANGING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_AREA_SEL_CHANGED(id, fn) { wxEVT_PLOT_AREA_SEL_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_BEGIN_X_LABEL_EDIT(id, fn) { wxEVT_PLOT_BEGIN_X_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_END_X_LABEL_EDIT(id, fn) { wxEVT_PLOT_END_X_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_BEGIN_Y_LABEL_EDIT(id, fn) { wxEVT_PLOT_BEGIN_Y_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_END_Y_LABEL_EDIT(id, fn) { wxEVT_PLOT_END_Y_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_BEGIN_TITLE_EDIT(id, fn) { wxEVT_PLOT_BEGIN_TITLE_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_END_TITLE_EDIT(id, fn) { wxEVT_PLOT_END_TITLE_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },

#endif // wxUSE_PLOT

#endif
   // _WX_PLOT_H_
