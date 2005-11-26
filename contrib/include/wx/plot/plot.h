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

#include "wx/defs.h"

#include "wx/dynarray.h"
#include "wx/event.h"
#include "wx/pen.h"
#include "wx/scrolwin.h"

#ifdef WXMAKINGDLL_PLOT
    #define WXDLLIMPEXP_PLOT WXEXPORT
    #define WXDLLIMPEXP_DATA_PLOT(type) WXEXPORT type
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_PLOT WXIMPORT
    #define WXDLLIMPEXP_DATA_PLOT(type) WXIMPORT type
#else // not making nor using DLL
    #define WXDLLIMPEXP_PLOT
    #define WXDLLIMPEXP_DATA_PLOT(type) type
#endif

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_PLOT wxPlotEvent;
class WXDLLIMPEXP_PLOT wxPlotCurve;
class WXDLLIMPEXP_PLOT wxPlotValues;
class WXDLLIMPEXP_PLOT wxPlotArea;
class WXDLLIMPEXP_PLOT wxPlotXAxisArea;
class WXDLLIMPEXP_PLOT wxPlotYAxisArea;
class WXDLLIMPEXP_PLOT wxPlotWindow;
class WXDLLIMPEXP_CORE wxStaticText;

//-----------------------------------------------------------------------------
// consts
//-----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_SEL_CHANGING,  941)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_SEL_CHANGED, 921)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_CLICKED, 922)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_DOUBLECLICKED, 923)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_ZOOM_IN, 924)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_ZOOM_OUT, 925)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_VALUE_SEL_CREATING, 926)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_VALUE_SEL_CREATED, 927)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_VALUE_SEL_CHANGING, 928)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_VALUE_SEL_CHANGED, 929)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_AREA_SEL_CREATING, 930)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_AREA_SEL_CREATED, 931)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_AREA_SEL_CHANGING, 932)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_AREA_SEL_CHANGED, 933)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_BEGIN_X_LABEL_EDIT, 934)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_END_X_LABEL_EDIT, 935)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_BEGIN_Y_LABEL_EDIT, 936)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_END_Y_LABEL_EDIT, 937)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_BEGIN_TITLE_EDIT, 938)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_END_TITLE_EDIT, 939)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PLOT, wxEVT_PLOT_AREA_CREATE, 940)
END_DECLARE_EVENT_TYPES()

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

class WXDLLIMPEXP_PLOT wxPlotEvent: public wxNotifyEvent
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

class WXDLLIMPEXP_PLOT wxPlotCurve: public wxObject
{
public:
    wxPlotCurve( int offsetY, double startY, double endY );

    virtual wxInt32 GetStartX() = 0;
    virtual wxInt32 GetEndX() = 0;

    virtual double GetY( wxInt32 x ) = 0;

    void SetStartY( double startY )
        { m_startY = startY; }
    double GetStartY() const
        { return m_startY; }
    void SetEndY( double endY )
        { m_endY = endY; }
    double GetEndY() const
        { return m_endY; }
    void SetOffsetY( int offsetY )
       { m_offsetY = offsetY; }
    int GetOffsetY() const
       { return m_offsetY; }

    void SetPenNormal( const wxPen &pen )
       { m_penNormal = pen; }
    void SetPenSelected( const wxPen &pen )
       { m_penSelected = pen; }

    const wxPen& GetPenNormal() const
       { return m_penNormal; }
    const wxPen& GetPenSelected() const
       { return m_penSelected; }

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

class WXDLLIMPEXP_PLOT wxPlotOnOff
{
public:
    wxPlotOnOff() { }

    wxInt32   m_on;
    wxInt32   m_off;
    void     *m_clientData;
};

WX_DECLARE_OBJARRAY_WITH_DECL(wxPlotOnOff, wxArrayPlotOnOff,
                              class WXDLLIMPEXP_PLOT);

class WXDLLIMPEXP_PLOT wxPlotOnOffCurve: public wxObject
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

class WXDLLIMPEXP_PLOT wxPlotArea: public wxWindow
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

class WXDLLIMPEXP_PLOT wxPlotXAxisArea: public wxWindow
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

class WXDLLIMPEXP_PLOT wxPlotYAxisArea: public wxWindow
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

class WXDLLIMPEXP_PLOT wxPlotWindow: public wxScrolledWindow
{
public:
    wxPlotWindow() {}
    wxPlotWindow(wxWindow *parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 int flags = wxPLOT_DEFAULT);
    ~wxPlotWindow();

    // curve accessors
    // ---------------

    void Add( wxPlotCurve *curve );
    void Delete( wxPlotCurve* curve );

    size_t GetCount();
    wxPlotCurve *GetAt( size_t n );

    void SetCurrentCurve( wxPlotCurve* current );
    wxPlotCurve *GetCurrentCurve();

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

    void SetScrollOnThumbRelease( bool scrollOnThumbRelease = true )
        { m_scrollOnThumbRelease = scrollOnThumbRelease; }
    bool GetScrollOnThumbRelease()
        { return m_scrollOnThumbRelease; }

    void SetEnlargeAroundWindowCentre( bool enlargeAroundWindowCentre = true )
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

    void AddChartTitle( const wxString&, wxFont = *wxNORMAL_FONT, wxColour = *wxBLACK );

private:
    friend class wxPlotArea;
    friend class wxPlotXAxisArea;
    friend class wxPlotYAxisArea;

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

    wxString           m_title;
    wxFont             m_titleFont;
    wxColour           m_titleColour;
    wxStaticText*      m_titleStaticText;
    wxBoxSizer*        m_plotAndTitleSizer;

    void DrawChartTitle();

    DECLARE_CLASS(wxPlotWindow)
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// plot event macros
// ----------------------------------------------------------------------------

typedef void (wxEvtHandler::*wxPlotEventFunction)(wxPlotEvent&);

#if WXWIN_COMPATIBILITY_EVENT_TYPES

#define EVT_PLOT(id, fn) { wxEVT_PLOT_DOUBLECLICKED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_SEL_CHANGING(id, fn) { wxEVT_PLOT_SEL_CHANGING, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_SEL_CHANGED(id, fn) { wxEVT_PLOT_SEL_CHANGED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_CLICKED(id, fn) { wxEVT_PLOT_CLICKED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_DOUBLECLICKED(id, fn) { wxEVT_PLOT_DOUBLECLICKED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_ZOOM_IN(id, fn) { wxEVT_PLOT_ZOOM_IN, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_ZOOM_OUT(id, fn) { wxEVT_PLOT_ZOOM_OUT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_VALUE_SEL_CREATING(id, fn) { wxEVT_PLOT_VALUE_SEL_CREATING, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_VALUE_SEL_CREATED(id, fn) { wxEVT_PLOT_VALUE_SEL_CREATED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_VALUE_SEL_CHANGING(id, fn) { wxEVT_PLOT_VALUE_SEL_CHANGING, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_VALUE_SEL_CHANGED(id, fn) { wxEVT_PLOT_VALUE_SEL_CHANGED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_AREA_SEL_CREATING(id, fn) { wxEVT_PLOT_AREA_SEL_CREATING, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_AREA_SEL_CREATED(id, fn) { wxEVT_PLOT_AREA_SEL_CREATED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_AREA_SEL_CHANGING(id, fn) { wxEVT_PLOT_AREA_SEL_CHANGING, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_AREA_SEL_CHANGED(id, fn) { wxEVT_PLOT_AREA_SEL_CHANGED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_BEGIN_X_LABEL_EDIT(id, fn) { wxEVT_PLOT_BEGIN_X_LABEL_EDIT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_END_X_LABEL_EDIT(id, fn) { wxEVT_PLOT_END_X_LABEL_EDIT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_BEGIN_Y_LABEL_EDIT(id, fn) { wxEVT_PLOT_BEGIN_Y_LABEL_EDIT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_END_Y_LABEL_EDIT(id, fn) { wxEVT_PLOT_END_Y_LABEL_EDIT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_BEGIN_TITLE_EDIT(id, fn) { wxEVT_PLOT_BEGIN_TITLE_EDIT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },
#define EVT_PLOT_END_TITLE_EDIT(id, fn) { wxEVT_PLOT_END_TITLE_EDIT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxPlotEventFunction) & fn, (wxObject *) NULL },

#else

#define EVT_PLOT(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_DOUBLECLICKED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_SEL_CHANGING(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_SEL_CHANGING, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_SEL_CHANGED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_SEL_CHANGED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_CLICKED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_CLICKED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_DOUBLECLICKED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_DOUBLECLICKED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_ZOOM_IN(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_ZOOM_IN, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_ZOOM_OUT(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_ZOOM_OUT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_VALUE_SEL_CREATING(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_VALUE_SEL_CREATING, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_VALUE_SEL_CREATED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_VALUE_SEL_CREATED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_VALUE_SEL_CHANGING(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_VALUE_SEL_CHANGING, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_VALUE_SEL_CHANGED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_VALUE_SEL_CHANGED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_AREA_SEL_CREATING(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_AREA_SEL_CREATING, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_AREA_SEL_CREATED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_AREA_SEL_CREATED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_AREA_SEL_CHANGING(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_AREA_SEL_CHANGING, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_AREA_SEL_CHANGED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_AREA_SEL_CHANGED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_BEGIN_X_LABEL_EDIT(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_BEGIN_X_LABEL_EDIT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_END_X_LABEL_EDIT(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_END_X_LABEL_EDIT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_BEGIN_Y_LABEL_EDIT(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_BEGIN_Y_LABEL_EDIT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_END_Y_LABEL_EDIT(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_END_Y_LABEL_EDIT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_BEGIN_TITLE_EDIT(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_BEGIN_TITLE_EDIT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PLOT_END_TITLE_EDIT(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_PLOT_END_TITLE_EDIT, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),

#endif

#endif
   // _WX_PLOT_H_
