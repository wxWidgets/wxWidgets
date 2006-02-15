/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/vscroll.h
// Purpose:     wxVScrolledWindow: generalization of wxScrolledWindow
// Author:      Vadim Zeitlin
// Modified by: Brad Anderson
// Created:     30.05.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VSCROLL_H_
#define _WX_VSCROLL_H_

#include "wx/panel.h"           // base class

// ----------------------------------------------------------------------------
// wxVScrolledWindow
// ----------------------------------------------------------------------------

/*
   In the name of this class, "V" may stand for "variable" because it can be
   used for scrolling lines of variable heights; "virtual" because it is not
   necessary to know the heights of all lines in advance -- only those which
   are shown on the screen need to be measured; or, even, "vertical" because
   this class only supports scrolling in one direction currently (this could
   and probably will change in the future however).

   In any case, this is a generalization of the wxScrolledWindow class which
   can be only used when all lines have the same height. It lacks some other
   wxScrolledWindow features however, notably it currently lacks support for
   horizontal scrolling; it can't scroll another window nor only a rectangle
   of the window and not its entire client area.
 */
class WXDLLEXPORT wxVScrolledWindow : public wxPanel
{
public:
    // constructors and such
    // ---------------------

    // default ctor, you must call Create() later
    wxVScrolledWindow() { Init(); }

    // normal ctor, no need to call Create() after this one
    //
    // note that wxVSCROLL is always automatically added to our style, there is
    // no need to specify it explicitly
    wxVScrolledWindow(wxWindow *parent,
                      wxWindowID id = wxID_ANY,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = 0,
                      const wxString& name = wxPanelNameStr)
    {
        Init();

        (void)Create(parent, id, pos, size, style, name);
    }

    // same as the previous ctor but returns status code: true if ok
    //
    // just as with the ctor above, wxVSCROLL style is always used, there is no
    // need to specify it
    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr)
    {
        return wxPanel::Create(parent, id, pos, size, style | wxVSCROLL, name);
    }


    // operations
    // ----------

    // set the number of lines the window contains: the derived class must
    // provide the heights for all lines with indices up to the one given here
    // in its OnGetLineHeight()
    void SetLineCount(size_t count);

    // scroll to the specified line: it will become the first visible line in
    // the window
    //
    // return true if we scrolled the window, false if nothing was done
    bool ScrollToLine(size_t line);

    // scroll by the specified number of lines/pages
    virtual bool ScrollLines(int lines);
    virtual bool ScrollPages(int pages);

    // redraw the specified line
    virtual void RefreshLine(size_t line);

    // redraw all lines in the specified range (inclusive)
    virtual void RefreshLines(size_t from, size_t to);

    // return the item at the specified (in physical coordinates) position or.

    // wxNOT_FOUND if none, i.e. if it is below the last item
    int HitTest(wxCoord x, wxCoord y) const;
    int HitTest(const wxPoint& pt) const { return HitTest(pt.x, pt.y); }

    // recalculate all our parameters and redisplay all lines
    virtual void RefreshAll();


    // accessors
    // ---------

    // get the number of lines this window contains (previously set by
    // SetLineCount())
    size_t GetLineCount() const { return m_lineMax; }

    // get the first currently visible line
    size_t GetVisibleBegin() const { return m_lineFirst; }

    // get the first currently visible line
    size_t GetVisibleEnd() const { return m_lineFirst + m_nVisible; }

    // is this line currently visible?
    bool IsVisible(size_t line) const
        { return line >= GetVisibleBegin() && line < GetVisibleEnd(); }


    // this is the same as GetVisibleBegin(), exists to match
    // GetLastVisibleLine() and for backwards compatibility only
    size_t GetFirstVisibleLine() const { return m_lineFirst; }

    // get the last currently visible line
    //
    // this function is unsafe as it returns (size_t)-1 (i.e. a huge positive
    // number) if the control is empty, use GetVisibleEnd() instead, this one
    // is kept for backwards compatibility
    size_t GetLastVisibleLine() const { return GetVisibleEnd() - 1; }

    // layout the children (including the sizer if needed)
    virtual bool Layout();

protected:
    // this function must be overridden in the derived class and it should
    // return the height of the given line in pixels
    virtual wxCoord OnGetLineHeight(size_t n) const = 0;

    // this function doesn't have to be overridden but it may be useful to do
    // it if calculating the lines heights is a relatively expensive operation
    // as it gives the user code a possibility to calculate several of them at
    // once
    //
    // OnGetLinesHint() is normally called just before OnGetLineHeight() but you
    // shouldn't rely on the latter being called for all lines in the interval
    // specified here. It is also possible that OnGetLineHeight() will be
    // called for the lines outside of this interval, so this is really just a
    // hint, not a promise.
    //
    // finally note that lineMin is inclusive, while lineMax is exclusive, as
    // usual
    virtual void OnGetLinesHint(size_t WXUNUSED(lineMin),
                                size_t WXUNUSED(lineMax)) const { }

    // when the number of lines changes, we try to estimate the total height
    // of all lines which is a rather expensive operation in terms of lines
    // access, so if the user code may estimate the average height
    // better/faster than we do, it should override this function to implement
    // its own logic
    //
    // this function should return the best guess for the total height it may
    // make
    virtual wxCoord EstimateTotalHeight() const;


    // the event handlers
    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollWinEvent& event);
#if wxUSE_MOUSEWHEEL
    void OnMouseWheel(wxMouseEvent& event);
#endif

    // find the index of the line we need to show at the top of the window such
    // that the last (fully or partially) visible line is the given one
    size_t FindFirstFromBottom(size_t lineLast, bool fullyVisible = false);

    // get the total height of the lines between lineMin (inclusive) and
    // lineMax (exclusive)
    wxCoord GetLinesHeight(size_t lineMin, size_t lineMax) const;

    // update the thumb size shown by the scrollbar
    void UpdateScrollbar();

private:
    // common part of all ctors
    void Init();


    // the total number of (logical) lines
    size_t m_lineMax;

    // the total (estimated) height
    wxCoord m_heightTotal;

    // the first currently visible line
    size_t m_lineFirst;

    // the number of currently visible lines (including the last, possibly only
    // partly, visible one)
    size_t m_nVisible;

    // accumulated mouse wheel rotation
#if wxUSE_MOUSEWHEEL
    int m_sumWheelRotation;
#endif

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxVScrolledWindow)
    DECLARE_ABSTRACT_CLASS(wxVScrolledWindow)
};


// ----------------------------------------------------------------------------
// wxHVScrolledWindow
// ----------------------------------------------------------------------------

/*
   This class is strongly influenced by wxVScrolledWindow. In fact, much of
   code is line for line the same except it explicitly states which axis is
   being worked on.  Like wxVScrolledWindow, this class is here to provide
   an easy way to implement variable line sizes.  The difference is that
   wxVScrolledWindow only works with vertical scrolling.  This class extends
   the behavior of wxVScrolledWindow to the horizontal axis in addition to the
   vertical axis.

   The scrolling is also "virtual" in the sense that line widths and heights
   only need to be known for lines that are currently visible.

   Like wxVScrolledWindow, this is a generalization of the wxScrolledWindow
   class which can be only used when all horizontal lines have the same width
   and all of the vertical lines have the same height. Like wxVScrolledWinow
   it lacks some of wxScrolledWindow features such as scrolling another window
   or only scrolling a rectangle of the window and not its entire client area.

   If only vertical scrolling is needed, wxVScrolledWindow is recommended
   because it is simpler to use (and you get to type less).
   
   There is no wxHScrolledWindow but horizontal only scrolling is implemented
   easily enough with this class.  If someone feels the need for such a class,
   implementing it is trivial.
 */
class WXDLLEXPORT wxHVScrolledWindow : public wxPanel
{
public:
    // constructors and such
    // ---------------------

    // default ctor, you must call Create() later
    wxHVScrolledWindow() { Init(); }

    // normal ctor, no need to call Create() after this one
    //
    // note that wxVSCROLL and wxHSCROLL are always automatically added to our
    // style, there is no need to specify them explicitly
    wxHVScrolledWindow(wxWindow *parent,
                       wxWindowID id = wxID_ANY,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = 0,
                       const wxString& name = wxPanelNameStr)
    {
        Init();

        (void)Create(parent, id, pos, size, style, name);
    }

    // same as the previous ctor but returns status code: true if ok
    //
    // just as with the ctor above, wxVSCROLL and wxHSCROLL styles are always
    // used, there is no need to specify them
    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr)
    {
        return wxPanel::Create(parent, id, pos, size,
                style | wxVSCROLL | wxHSCROLL, name);
    }


    // operations
    // ----------

    // set the number of lines the window contains for each axis: the derived
    // class must provide the widths and heights for all lines with indices up
    // to each of the one given here in its OnGetColumnWidth() and
    // OnGetRowHeight()
    void SetRowColumnCounts(size_t rowCount, size_t columnCount);

    // with physical scrolling on, the device origin is changed properly when
    // a wxPaintDC is prepared, children are actually moved and layed out
    // properly, and the contents of the window (pixels) are actually moved
    void EnablePhysicalScrolling(bool scrolling = true)
        { m_physicalScrolling = scrolling; }

    // scroll to the specified line: it will become the first visible line in
    // the window
    //
    // return true if we scrolled the window, false if nothing was done
    bool ScrollToRow(size_t row);
    bool ScrollToColumn(size_t column);
    bool ScrollToRowColumn(size_t row, size_t column);

    // scroll by the specified number of lines/pages
    virtual bool ScrollRows(int rows);
    virtual bool ScrollColumns(int columns);
    virtual bool ScrollRowsColumns(int rows, int columns);
    virtual bool ScrollRowPages(int pages);
    virtual bool ScrollColumnPages(int pages);
    virtual bool ScrollPages(int rowPages, int columnPages);

    // redraw the specified line
    virtual void RefreshRow(size_t line);
    virtual void RefreshColumn(size_t line);
    virtual void RefreshRowColumn(size_t row, size_t column);

    // redraw all lines in the specified range (inclusive)
    virtual void RefreshRows(size_t from, size_t to);
    virtual void RefreshColumns(size_t from, size_t to);
    virtual void RefreshRowsColumns(size_t fromRow, size_t toRow,
                                    size_t fromColumn, size_t toColumn);

    // return the horizontal and vertical line within a wxPoint at the
    // specified (in physical coordinates) position or.

    // wxNOT_FOUND in either or both axes if no line is present at the
    // requested coordinates, i.e. if it is past the last lines
    wxPoint HitTest(wxCoord x, wxCoord y) const;
    wxPoint HitTest(const wxPoint& pt) const { return HitTest(pt.x, pt.y); }

    // recalculate all our parameters and redisplay all lines
    virtual void RefreshAll();


    // accessors
    // ---------

    // get the number of lines this window contains (previously set by
    // SetLineCount())
    size_t GetRowCount() const { return m_rowsMax; }
    size_t GetColumnCount() const { return m_columnsMax; }
    wxSize GetRowColumnCounts() const
        { return wxSize((int)m_columnsMax, (int)m_rowsMax); }

    // get the first currently visible line/lines
    size_t GetVisibleRowsBegin() const { return m_rowsFirst; }
    size_t GetVisibleColumnsBegin() const { return m_columnsFirst; }
    wxPoint GetVisibleBegin() const
        { return wxPoint((int)m_columnsFirst, (int)m_rowsFirst); }

    // get the last currently visible line/lines
    size_t GetVisibleRowsEnd() const
        { return m_rowsFirst + m_nRowsVisible; }
    size_t GetVisibleColumnsEnd() const
        { return m_columnsFirst + m_nColumnsVisible; }
    wxPoint GetVisibleEnd() const
        { return wxPoint((int)(m_columnsFirst + m_nColumnsVisible),
                         (int)(m_rowsFirst + m_nRowsVisible)); }

    // is this line currently visible?
    bool IsRowVisible(size_t row) const
        { return row >= m_rowsFirst &&
                 row < GetVisibleRowsEnd(); }
    bool IsColumnVisible(size_t column) const
        { return column >= m_columnsFirst &&
                 column < GetVisibleColumnsEnd(); }
    bool IsVisible(size_t row, size_t column) const
        { return IsRowVisible(row) && IsColumnVisible(column); }

    // layout the children (including the sizer if needed)
    virtual bool Layout();

protected:
    // these functions must be overridden in the derived class and they should
    // return the width or height of the given line in pixels
    virtual wxCoord OnGetRowHeight(size_t n) const = 0;
    virtual wxCoord OnGetColumnWidth(size_t n) const = 0;
    
    // the following functions don't need to be overridden but it may be useful
    // to do if calculating the lines widths or heights is a relatively
    // expensive operation as it gives the user code a possibility to calculate
    // several of them at once
    //
    // OnGetRowsHeightHint() and OnGetColumnsWidthHint() are normally called
    // just before OnGetRowHeight() and OnGetColumnWidth(), respectively, but
    // you shouldn't rely on the latter methods being called for all lines in
    // the interval specified here. It is also possible that OnGetRowHeight()
    // or OnGetColumnWidth() will be called for the lines outside of this
    // interval, so this is really just a hint, not a promise.
    //
    // finally note that min is inclusive, while max is exclusive, as usual
    virtual void OnGetRowsHeightHint(size_t WXUNUSED(rowMin),
                                     size_t WXUNUSED(rowMax)) const { }
    virtual void OnGetColumnsWidthHint(size_t WXUNUSED(columnMin),
                                       size_t WXUNUSED(columnMax)) const { }
    
    // when the number of lines changes, we try to estimate the total width or
    // height of all lines which is a rather expensive operation in terms of
    // lines access, so if the user code may estimate the average height
    // better/faster than we do, it should override this function to implement
    // its own logic
    //
    // this function should return the best guess for the total height it may
    // make
    virtual wxCoord EstimateTotalHeight() const;
    virtual wxCoord EstimateTotalWidth() const;

    // the event handlers
    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollWinEvent& event);
#if wxUSE_MOUSEWHEEL
    void OnMouseWheel(wxMouseEvent& event);
#endif


    // find the index of the horizontal line we need to show at the top of the
    // window such that the last (fully or partially) visible line is the given
    // one
    size_t FindFirstFromRight(size_t columnLast, bool fullyVisible = false);

    // find the index of the vertical line we need to show at the top of the
    // window such that the last (fully or partially) visible line is the given
    // one
    size_t FindFirstFromBottom(size_t rowLast, bool fullyVisible = false);


    // get the total width or height of the lines between lineMin (inclusive)
    // and lineMax (exclusive)
    wxCoord GetRowsHeight(size_t rowMin, size_t rowMax) const;
    wxCoord GetColumnsWidth(size_t columnMin, size_t columnMax) const;

    // update the thumb size shown by the scrollbar
    void UpdateScrollbars();

    // shifts the specified dc by the scroll position
    void PrepareDC(wxDC& dc);

private:
    // common part of all ctors
    void Init();


    // the total number of (logical) lines for each axis
    size_t m_rowsMax;
    size_t m_columnsMax;

    // the total (estimated) height
    wxCoord m_heightTotal;

    // the total (estimated) width
    wxCoord m_widthTotal;

    // the first currently visible line on each axis
    size_t m_rowsFirst;
    size_t m_columnsFirst;

    // the number of currently visible lines for each axis (including the last,
    // possibly only partly, visible one)
    size_t m_nRowsVisible;
    size_t m_nColumnsVisible;

    // accumulated mouse wheel rotation
#if wxUSE_MOUSEWHEEL
    int m_sumWheelRotation;
#endif

    // do child scrolling (used in DoPrepareDC())
    bool m_physicalScrolling;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxHVScrolledWindow)
    DECLARE_ABSTRACT_CLASS(wxHVScrolledWindow)
};

#endif // _WX_VSCROLL_H_

