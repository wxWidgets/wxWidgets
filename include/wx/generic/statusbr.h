/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/statusbr.h
// Purpose:     wxStatusBarGeneric class
// Author:      Julian Smart
// Modified by: VZ at 05.02.00 to derive from wxStatusBarBase
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_STATUSBR_H_
#define _WX_GENERIC_STATUSBR_H_

#include "wx/defs.h"

#if wxUSE_STATUSBAR

#include "wx/pen.h"
#include "wx/arrstr.h"


// ----------------------------------------------------------------------------
// wxStatusBarGeneric
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxStatusBarGeneric : public wxStatusBarBase
{
public:
    wxStatusBarGeneric() { Init(); }
    wxStatusBarGeneric(wxWindow *parent,
                       wxWindowID winid = wxID_ANY,
                       long style = wxST_SIZEGRIP,
                       const wxString& name = wxStatusBarNameStr)
    {
        Init();

        Create(parent, winid, style, name);
    }

    virtual ~wxStatusBarGeneric();

    bool Create(wxWindow *parent, wxWindowID winid = wxID_ANY,
                long style = wxST_SIZEGRIP,
                const wxString& name = wxStatusBarNameStr);

    // Create status line
    virtual void SetFieldsCount(int number = 1,
                                const int *widths = (const int *) NULL);

    // Set status line text
    virtual void SetStatusText(const wxString& text, int number = 0);

    // Set status line widths
    virtual void SetStatusWidths(int n, const int widths_field[]);

    // Get the position and size of the field's internal bounding rectangle
    virtual bool GetFieldRect(int i, wxRect& rect) const;

    // sets the minimal vertical size of the status bar
    virtual void SetMinHeight(int height);

    virtual int GetBorderX() const { return m_borderX; }
    virtual int GetBorderY() const { return m_borderY; }


protected:      // event handlers

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);

    void OnLeftDown(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);

    // Responds to colour changes
    void OnSysColourChanged(wxSysColourChangedEvent& event);

protected:

    virtual void DrawFieldText(wxDC& dc, const wxRect& rc, int i, int textHeight);
    virtual void DrawField(wxDC& dc, int i, int textHeight);

    void SetBorderX(int x);
    void SetBorderY(int y);

    virtual void InitColours();

    // true if the status bar shows the size grip: for this it must have
    // wxST_SIZEGRIP style and the window it is attached to must be resizeable
    // and not maximized
    bool ShowsSizeGrip() const;

    // returns the position and the size of the size grip
    wxRect GetSizeGripRect() const;

    // common part of all ctors
    void Init();

    // the last known height of the client rect
    int               m_lastClientHeight;

    // the absolute widths of the status bar panes in pixels
    wxArrayInt        m_widthsAbs;

    int               m_borderX;
    int               m_borderY;
    wxPen             m_mediumShadowPen;
    wxPen             m_hilightPen;

    virtual wxSize DoGetBestSize() const;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxStatusBarGeneric)
};

#endif // wxUSE_STATUSBAR

#endif
    // _WX_GENERIC_STATUSBR_H_
