/////////////////////////////////////////////////////////////////////////////
// Name:        wx/deprecated/tbarsmpl.h
// Purpose:     wxToolBarSimple class
// Author:      Julian Smart
// Modified by: VZ on 14.12.99 during wxToolBar classes reorganization
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TBARSMPLH__
#define _WX_TBARSMPLH__

#include "wx/tbarbase.h"

#include "wx/deprecated/setup.h"

#if wxUSE_TOOLBAR && wxUSE_TOOLBAR_SIMPLE

class WXDLLEXPORT wxMemoryDC;

// ----------------------------------------------------------------------------
// wxToolBarSimple is a generic toolbar implementation in pure wxWidgets
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_DEPRECATED wxToolBarSimple : public wxToolBarBase
{
public:
    // ctors and dtor
    wxToolBarSimple() { Init(); }

    wxToolBarSimple(wxWindow *parent,
                    wxWindowID winid,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxNO_BORDER | wxTB_HORIZONTAL,
                    const wxString& name = wxToolBarNameStr)
    {
        Init();

        Create(parent, winid, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID winid,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxNO_BORDER | wxTB_HORIZONTAL,
                const wxString& name = wxToolBarNameStr);

    virtual ~wxToolBarSimple();

    // override/implement base class virtuals
    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const;

    virtual bool Realize();

    virtual void SetRows(int nRows);

    // implementation from now on
    // --------------------------

    // SCROLLING: this has to be copied from wxScrolledWindow since
    // wxToolBarBase inherits from wxControl. This could have been put into
    // wxToolBarSimple, but we might want any derived toolbar class to be
    // scrollable.

    // Number of pixels per user unit (0 or -1 for no scrollbar)
    // Length of virtual canvas in user units
    virtual void SetScrollbars(int horizontal, int vertical,
            int x_length, int y_length,
            int x_pos = 0, int y_pos = 0);

    // Physically scroll the window
    virtual void Scroll(int x_pos, int y_pos);
    virtual void GetScrollPixelsPerUnit(int *x_unit, int *y_unit) const;
    virtual void EnableScrolling(bool x_scrolling, bool y_scrolling);
    virtual void AdjustScrollbars();

    // Prepare the DC by translating it according to the current scroll position
    virtual void PrepareDC(wxDC& dc);

    int GetScrollPageSize(int orient) const ;
    void SetScrollPageSize(int orient, int pageSize);

    // Get the view start
    virtual void ViewStart(int *x, int *y) const;

    // Actual size in pixels when scrolling is taken into account
    virtual void GetVirtualSize(int *x, int *y) const;

    int CalcScrollInc(wxScrollEvent& event);

    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    void OnScroll(wxScrollEvent& event);

protected:
    // common part of all ctors
    void Init();

    // implement base class pure virtuals
    virtual wxToolBarToolBase *DoAddTool
                               (
                                   int toolid,
                                   const wxString& label,
                                   const wxBitmap& bitmap,
                                   const wxBitmap& bmpDisabled,
                                   wxItemKind kind,
                                   const wxString& shortHelp = wxEmptyString,
                                   const wxString& longHelp = wxEmptyString,
                                   wxObject *clientData = NULL,
                                   wxCoord xPos = wxDefaultCoord,
                                   wxCoord yPos = wxDefaultCoord
                               );

    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool);
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool);

    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable);
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle);
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle);

    virtual wxToolBarToolBase *CreateTool(int winid,
                                          const wxString& label,
                                          const wxBitmap& bmpNormal,
                                          const wxBitmap& bmpDisabled,
                                          wxItemKind kind,
                                          wxObject *clientData,
                                          const wxString& shortHelp,
                                          const wxString& longHelp);
    virtual wxToolBarToolBase *CreateTool(wxControl *control);

    // helpers
    void DrawTool(wxToolBarToolBase *tool);
    virtual void DrawTool(wxDC& dc, wxToolBarToolBase *tool);
    virtual void SpringUpButton(int index);

    int  m_currentRowsOrColumns;

    int  m_pressedTool, m_currentTool;

    wxCoord m_lastX, m_lastY;
    wxCoord m_maxWidth, m_maxHeight;
    wxCoord m_xPos, m_yPos;

    // scrolling data
    int                   m_xScrollPixelsPerLine;
    int                   m_yScrollPixelsPerLine;
    bool                  m_xScrollingEnabled;
    bool                  m_yScrollingEnabled;
    int                   m_xScrollPosition;
    int                   m_yScrollPosition;
    int                   m_xScrollLines;
    int                   m_yScrollLines;
    int                   m_xScrollLinesPerPage;
    int                   m_yScrollLinesPerPage;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxToolBarSimple)
};

#endif // wxUSE_TOOLBAR_SIMPLE

#endif
    // _WX_TBARSMPLH__

