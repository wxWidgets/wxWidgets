/////////////////////////////////////////////////////////////////////////////
// Name:        _window.i
// Purpose:     SWIG interface for wxPanel and wxScrolledWindow
//
// Author:      Robin Dunn
//
// Created:     24-June-1997
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
%}


//---------------------------------------------------------------------------
%newgroup


class wxPanel : public wxWindow
{
public:
    %addtofunc wxPanel         "self._setOORInfo(self)"
    %addtofunc wxPanel()       "val._setOORInfo(self)"

    wxPanel(wxWindow* parent,
            const wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPyPanelNameStr);
    %name(PrePanel)wxPanel();

    bool Create(wxWindow* parent,
                const wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = wxPyPanelNameStr);

    void InitDialog();

};

//---------------------------------------------------------------------------
%newgroup


// TODO: Add wrappers for the wxScrollHelper class, make wxScrolledWindow
//       derive from it and wxPanel.  But what to do about wxGTK where this
//       is not true?

class wxScrolledWindow : public wxPanel
{
public:
    %addtofunc wxScrolledWindow         "self._setOORInfo(self)"
    %addtofunc wxScrolledWindow()       "val._setOORInfo(self)"

    wxScrolledWindow(wxWindow* parent,
                     const wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxHSCROLL | wxVSCROLL,
                     const wxString& name = wxPyPanelNameStr);
    %name(PreScrolledWindow)wxScrolledWindow();

    bool Create(wxWindow* parent,
                const wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHSCROLL | wxVSCROLL,
                const wxString& name = wxPyPanelNameStr);
    

    // configure the scrolling
    virtual void SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY,
                               int noUnitsX, int noUnitsY,
                               int xPos = 0, int yPos = 0,
                               bool noRefresh = FALSE );

    // scroll to the given (in logical coords) position
    virtual void Scroll(int x, int y);

    // get/set the page size for this orientation (wxVERTICAL/wxHORIZONTAL)
    int GetScrollPageSize(int orient) const;
    void SetScrollPageSize(int orient, int pageSize);

    // Set the x, y scrolling increments.
    void SetScrollRate( int xstep, int ystep );

    // get the size of one logical unit in physical ones
    virtual void GetScrollPixelsPerUnit(int *OUTPUT,
                                        int *OUTPUT) const;

    // Enable/disable Windows scrolling in either direction. If TRUE, wxWindows
    // scrolls the canvas and only a bit of the canvas is invalidated; no
    // Clear() is necessary. If FALSE, the whole canvas is invalidated and a
    // Clear() is necessary. Disable for when the scroll increment is used to
    // actually scroll a non-constant distance
    virtual void EnableScrolling(bool x_scrolling, bool y_scrolling);

    // Get the view start
    virtual void GetViewStart(int *OUTPUT, int *OUTPUT) const;

    // Set the scale factor, used in PrepareDC
    void SetScale(double xs, double ys);
    double GetScaleX() const;
    double GetScaleY() const;


    %nokwargs CalcScrolledPosition;
    %nokwargs CalcUnscrolledPosition;
    
    // translate between scrolled and unscrolled coordinates
    void CalcScrolledPosition(int x, int y, int *OUTPUT, int *OUTPUT) const;
    wxPoint CalcScrolledPosition(const wxPoint& pt) const;
    void CalcUnscrolledPosition(int x, int y, int *OUTPUT, int *OUTPUT) const;
    wxPoint CalcUnscrolledPosition(const wxPoint& pt) const;

// TODO: use directors?
//     virtual void DoCalcScrolledPosition(int x, int y, int *xx, int *yy) const;
//     virtual void DoCalcUnscrolledPosition(int x, int y, int *xx, int *yy) const;

    // Adjust the scrollbars
    virtual void AdjustScrollbars();

    // Calculate scroll increment
    virtual int CalcScrollInc(wxScrollWinEvent& event);

    // Normally the wxScrolledWindow will scroll itself, but in some rare
    // occasions you might want it to scroll [part of] another window (e.g. a
    // child of it in order to scroll only a portion the area between the
    // scrollbars (spreadsheet: only cell area will move).
    virtual void SetTargetWindow(wxWindow *target);
    virtual wxWindow *GetTargetWindow() const;

#ifndef __WXGTK__
    void SetTargetRect(const wxRect& rect);
    wxRect GetTargetRect() const;
#endif
};

    
//---------------------------------------------------------------------------
