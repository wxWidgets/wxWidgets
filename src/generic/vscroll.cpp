/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/vscroll.cpp
// Purpose:     wxVScrolledWindow implementation
// Author:      Vadim Zeitlin
// Modified by: Brad Anderson
// Created:     30.05.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/vscroll.h"
#include "wx/sizer.h"
#include "wx/dc.h"

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxVScrolledWindow, wxPanel)
    EVT_SIZE(wxVScrolledWindow::OnSize)
    EVT_SCROLLWIN(wxVScrolledWindow::OnScroll)
#if wxUSE_MOUSEWHEEL
    EVT_MOUSEWHEEL(wxVScrolledWindow::OnMouseWheel)
#endif
END_EVENT_TABLE()


// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_ABSTRACT_CLASS(wxVScrolledWindow, wxPanel)

// ----------------------------------------------------------------------------
// initialization
// ----------------------------------------------------------------------------

void wxVScrolledWindow::Init()
{
    // we're initially empty
    m_lineMax =
    m_lineFirst = 0;

    // this one should always be strictly positive
    m_nVisible = 1;

    m_heightTotal = 0;

#if wxUSE_MOUSEWHEEL
    m_sumWheelRotation = 0;
#endif
}

// ----------------------------------------------------------------------------
// various helpers
// ----------------------------------------------------------------------------

wxCoord wxVScrolledWindow::EstimateTotalHeight() const
{
    // estimate the total height: it is impossible to call
    // OnGetLineHeight() for every line because there may be too many of
    // them, so we just make a guess using some lines in the beginning,
    // some in the end and some in the middle
    static const size_t NUM_LINES_TO_SAMPLE = 10;

    wxCoord heightTotal;
    if ( m_lineMax < 3*NUM_LINES_TO_SAMPLE )
    {
        // in this case calculating exactly is faster and more correct than
        // guessing
        heightTotal = GetLinesHeight(0, m_lineMax);
    }
    else // too many lines to calculate exactly
    {
        // look at some lines in the beginning/middle/end
        heightTotal =
            GetLinesHeight(0, NUM_LINES_TO_SAMPLE) +
                GetLinesHeight(m_lineMax - NUM_LINES_TO_SAMPLE, m_lineMax) +
                    GetLinesHeight(m_lineMax/2 - NUM_LINES_TO_SAMPLE/2,
                                   m_lineMax/2 + NUM_LINES_TO_SAMPLE/2);

        // use the height of the lines we looked as the average
        heightTotal = (wxCoord)
                (((float)heightTotal / (3*NUM_LINES_TO_SAMPLE)) * m_lineMax);
    }

    return heightTotal;
}

wxCoord wxVScrolledWindow::GetLinesHeight(size_t lineMin, size_t lineMax) const
{
    if ( lineMin == lineMax )
        return 0;
    else if ( lineMin > lineMax )
        return -GetLinesHeight(lineMax, lineMin);
    //else: lineMin < lineMax

    // let the user code know that we're going to need all these lines
    OnGetLinesHint(lineMin, lineMax);

    // do sum up their heights
    wxCoord height = 0;
    for ( size_t line = lineMin; line < lineMax; line++ )
    {
        height += OnGetLineHeight(line);
    }

    return height;
}

size_t wxVScrolledWindow::FindFirstFromBottom(size_t lineLast, bool full)
{
    const wxCoord hWindow = GetClientSize().y;

    // go upwards until we arrive at a line such that lineLast is not visible
    // any more when it is shown
    size_t lineFirst = lineLast;
    wxCoord h = 0;
    for ( ;; )
    {
        h += OnGetLineHeight(lineFirst);

        if ( h > hWindow )
        {
            // for this line to be fully visible we need to go one line
            // down, but if it is enough for it to be only partly visible then
            // this line will do as well
            if ( full )
            {
                lineFirst++;
            }

            break;
        }

        if ( !lineFirst )
            break;

        lineFirst--;
    }

    return lineFirst;
}

void wxVScrolledWindow::UpdateScrollbar()
{
    // see how many lines can we fit on screen
    const wxCoord hWindow = GetClientSize().y;

    wxCoord h = 0;
    size_t line;
    for ( line = m_lineFirst; line < m_lineMax; line++ )
    {
        if ( h > hWindow )
            break;

        h += OnGetLineHeight(line);
    }

    m_nVisible = line - m_lineFirst;

    int pageSize = m_nVisible;
    if ( h > hWindow )
    {
        // last line is only partially visible, we still need the scrollbar and
        // so we have to "fix" pageSize because if it is equal to m_lineMax the
        // scrollbar is not shown at all under MSW
        pageSize--;
    }

    // set the scrollbar parameters to reflect this
    SetScrollbar(wxVERTICAL, m_lineFirst, pageSize, m_lineMax);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void wxVScrolledWindow::SetLineCount(size_t count)
{
    // save the number of lines
    m_lineMax = count;

    // and our estimate for their total height
    m_heightTotal = EstimateTotalHeight();

    // recalculate the scrollbars parameters
    m_lineFirst = 1;    // make sure it is != 0
    ScrollToLine(0);
}

void wxVScrolledWindow::RefreshLine(size_t line)
{
    // is this line visible?
    if ( !IsVisible(line) )
    {
        // no, it is useless to do anything
        return;
    }

    // calculate the rect occupied by this line on screen
    wxRect rect;
    rect.width = GetClientSize().x;
    rect.height = OnGetLineHeight(line);
    for ( size_t n = GetVisibleBegin(); n < line; n++ )
    {
        rect.y += OnGetLineHeight(n);
    }

    // do refresh it
    RefreshRect(rect);
}

void wxVScrolledWindow::RefreshLines(size_t from, size_t to)
{
    wxASSERT_MSG( from <= to, _T("RefreshLines(): empty range") );

    // clump the range to just the visible lines -- it is useless to refresh
    // the other ones
    if ( from < GetVisibleBegin() )
        from = GetVisibleBegin();

    if ( to >= GetVisibleEnd() )
        to = GetVisibleEnd();
    else
        to++;

    // calculate the rect occupied by these lines on screen
    wxRect rect;
    rect.width = GetClientSize().x;
    for ( size_t nBefore = GetVisibleBegin(); nBefore < from; nBefore++ )
    {
        rect.y += OnGetLineHeight(nBefore);
    }

    for ( size_t nBetween = from; nBetween < to; nBetween++ )
    {
        rect.height += OnGetLineHeight(nBetween);
    }

    // do refresh it
    RefreshRect(rect);
}

void wxVScrolledWindow::RefreshAll()
{
    UpdateScrollbar();

    Refresh();
}

bool wxVScrolledWindow::Layout()
{
    if(GetSizer())
    {
        // adjust the sizer dimensions/position taking into account the
        // virtual size and scrolled position of the window.

        int y, w, h; // x is always 0 so no variable needed

        y = -GetLinesHeight(0, GetFirstVisibleLine());
        GetVirtualSize(&w, &h);
        GetSizer()->SetDimension(0, y, w, h);
        return true;
    }

    // fall back to default for LayoutConstraints
    return wxPanel::Layout();
}

int wxVScrolledWindow::HitTest(wxCoord WXUNUSED(x), wxCoord y) const
{
    const size_t lineMax = GetVisibleEnd();
    for ( size_t line = GetVisibleBegin(); line < lineMax; line++ )
    {
        y -= OnGetLineHeight(line);
        if ( y < 0 )
            return line;
    }

    return wxNOT_FOUND;
}

// ----------------------------------------------------------------------------
// scrolling
// ----------------------------------------------------------------------------

bool wxVScrolledWindow::ScrollToLine(size_t line)
{
    if ( !m_lineMax )
    {
        // we're empty, code below doesn't make sense in this case
        return false;
    }

    // determine the real first line to scroll to: we shouldn't scroll beyond
    // the end
    size_t lineFirstLast = FindFirstFromBottom(m_lineMax - 1, true);
    if ( line > lineFirstLast )
        line = lineFirstLast;

    // anything to do?
    if ( line == m_lineFirst )
    {
        // no
        return false;
    }


    // remember the currently shown lines for the refresh code below
    size_t lineFirstOld = GetVisibleBegin();

    m_lineFirst = line;


    // the size of scrollbar thumb could have changed
    UpdateScrollbar();

    // finally, scroll the actual window
    ScrollWindow(0, GetLinesHeight(GetVisibleBegin(), lineFirstOld));

    return true;
}

bool wxVScrolledWindow::ScrollLines(int lines)
{
    lines += m_lineFirst;
    if ( lines < 0 )
        lines = 0;

    return ScrollToLine(lines);
}

bool wxVScrolledWindow::ScrollPages(int pages)
{
    bool didSomething = false;

    while ( pages )
    {
        int line;
        if ( pages > 0 )
        {
            line = GetVisibleEnd();
            if ( line )
                line--;
            pages--;
        }
        else // pages < 0
        {
            line = FindFirstFromBottom(GetVisibleBegin());
            pages++;
        }

        didSomething = ScrollToLine(line);
    }

    return didSomething;
}

// ----------------------------------------------------------------------------
// event handling
// ----------------------------------------------------------------------------

void wxVScrolledWindow::OnSize(wxSizeEvent& event)
{
    UpdateScrollbar();

    event.Skip();
}

void wxVScrolledWindow::OnScroll(wxScrollWinEvent& event)
{
    size_t lineFirstNew;

    const wxEventType evtType = event.GetEventType();

    if ( evtType == wxEVT_SCROLLWIN_TOP )
    {
        lineFirstNew = 0;
    }
    else if ( evtType == wxEVT_SCROLLWIN_BOTTOM )
    {
        lineFirstNew = m_lineMax;
    }
    else if ( evtType == wxEVT_SCROLLWIN_LINEUP )
    {
        lineFirstNew = m_lineFirst ? m_lineFirst - 1 : 0;
    }
    else if ( evtType == wxEVT_SCROLLWIN_LINEDOWN )
    {
        lineFirstNew = m_lineFirst + 1;
    }
    else if ( evtType == wxEVT_SCROLLWIN_PAGEUP )
    {
        lineFirstNew = FindFirstFromBottom(m_lineFirst);
    }
    else if ( evtType == wxEVT_SCROLLWIN_PAGEDOWN )
    {
        lineFirstNew = GetVisibleEnd();
        if ( lineFirstNew )
            lineFirstNew--;
    }
    else if ( evtType == wxEVT_SCROLLWIN_THUMBRELEASE )
    {
        lineFirstNew = event.GetPosition();
    }
    else if ( evtType == wxEVT_SCROLLWIN_THUMBTRACK )
    {
        lineFirstNew = event.GetPosition();
    }

    else // unknown scroll event?
    {
        wxFAIL_MSG( _T("unknown scroll event type?") );
        return;
    }

    ScrollToLine(lineFirstNew);

#ifdef __WXMAC__
    Update();
#endif // __WXMAC__
}

#if wxUSE_MOUSEWHEEL

void wxVScrolledWindow::OnMouseWheel(wxMouseEvent& event)
{
    m_sumWheelRotation += event.GetWheelRotation();
    int delta = event.GetWheelDelta();

    // how much to scroll this time
    int units_to_scroll = -(m_sumWheelRotation/delta);
    if ( !units_to_scroll )
        return;

    m_sumWheelRotation += units_to_scroll*delta;

    if ( !event.IsPageScroll() )
        ScrollLines( units_to_scroll*event.GetLinesPerAction() );
    else
        // scroll pages instead of lines
        ScrollPages( units_to_scroll );
}

#endif




// ----------------------------------------------------------------------------
// wxVarScrolled Window event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxHVScrolledWindow, wxPanel)
    EVT_SIZE(wxHVScrolledWindow::OnSize)
    EVT_SCROLLWIN(wxHVScrolledWindow::OnScroll)
#if wxUSE_MOUSEWHEEL
    EVT_MOUSEWHEEL(wxHVScrolledWindow::OnMouseWheel)
#endif
END_EVENT_TABLE()


// ============================================================================
// wxVarScrolled implementation
// ============================================================================

IMPLEMENT_ABSTRACT_CLASS(wxHVScrolledWindow, wxPanel)

// ----------------------------------------------------------------------------
// initialization
// ----------------------------------------------------------------------------

void wxHVScrolledWindow::Init()
{
    // we're initially empty
    m_rowsMax =
    m_columnsMax =
    m_rowsFirst =
    m_columnsFirst = 0;

    // these should always be strictly positive
    m_nRowsVisible =
    m_nColumnsVisible = 1;

    m_widthTotal =
    m_heightTotal = 0;

    m_physicalScrolling = true;

#if wxUSE_MOUSEWHEEL
    m_sumWheelRotation = 0;
#endif
}

// ----------------------------------------------------------------------------
// various helpers
// ----------------------------------------------------------------------------

wxCoord wxHVScrolledWindow::EstimateTotalHeight() const
{
    // estimate the total height: it is impossible to call
    // OnGetLineHeight() for every line because there may be too many of
    // them, so we just make a guess using some lines in the beginning,
    // some in the end and some in the middle
    static const size_t NUM_LINES_TO_SAMPLE = 10;

    wxCoord heightTotal;
    if ( m_rowsMax < 3*NUM_LINES_TO_SAMPLE )
    {
        // in this case calculating exactly is faster and more correct than
        // guessing
        heightTotal = GetRowsHeight(0, m_rowsMax);
    }
    else // too many lines to calculate exactly
    {
        // look at some lines in the beginning/middle/end
        heightTotal =
            GetRowsHeight(0, NUM_LINES_TO_SAMPLE) +
                GetRowsHeight(m_rowsMax - NUM_LINES_TO_SAMPLE,
                              m_rowsMax) +
                    GetRowsHeight(m_rowsMax/2 - NUM_LINES_TO_SAMPLE/2,
                                  m_rowsMax/2 + NUM_LINES_TO_SAMPLE/2);

        // use the height of the lines we looked as the average
        heightTotal = (wxCoord)
                (((float)heightTotal / (3*NUM_LINES_TO_SAMPLE)) * m_rowsMax);
    }

    return heightTotal;
}

wxCoord wxHVScrolledWindow::EstimateTotalWidth() const
{
    // estimate the total width: it is impossible to call
    // OnGetLineWidth() for every line because there may be too many of
    // them, so we just make a guess using some lines in the beginning,
    // some in the end and some in the middle
    static const size_t NUM_LINES_TO_SAMPLE = 10;

    wxCoord widthTotal;
    if ( m_columnsMax < 3*NUM_LINES_TO_SAMPLE )
    {
        // in this case calculating exactly is faster and more correct than
        // guessing
        widthTotal = GetColumnsWidth(0, m_columnsMax);
    }
    else // too many lines to calculate exactly
    {
        // look at some lines in the beginning/middle/end
        widthTotal =
            GetColumnsWidth(0, NUM_LINES_TO_SAMPLE) +
                GetColumnsWidth(m_columnsMax - NUM_LINES_TO_SAMPLE,
                                m_columnsMax) +
                    GetColumnsWidth(m_columnsMax/2 - NUM_LINES_TO_SAMPLE/2,
                                    m_columnsMax/2 + NUM_LINES_TO_SAMPLE/2);

        // use the width of the lines we looked as the average
        widthTotal = (wxCoord)
                (((float)widthTotal / (3*NUM_LINES_TO_SAMPLE)) * m_columnsMax);
    }

    return widthTotal;
}

wxCoord wxHVScrolledWindow::GetRowsHeight(size_t rowMin, size_t rowMax) const
{
    if ( rowMin == rowMax )
        return 0;
    else if ( rowMin > rowMax )
        return -GetRowsHeight(rowMax, rowMin);
    //else: lineMin < lineMax

    // let the user code know that we're going to need all these lines
    OnGetRowsHeightHint(rowMin, rowMax);

    // do sum up their heights
    wxCoord height = 0;
    for ( size_t row = rowMin; row < rowMax; row++ )
    {
        height += OnGetRowHeight(row);
    }

    return height;
}

wxCoord wxHVScrolledWindow::GetColumnsWidth(size_t columnMin, size_t columnMax) const
{
    if ( columnMin == columnMax )
        return 0;
    else if ( columnMin > columnMax )
        return -GetColumnsWidth(columnMax, columnMin);
    //else: lineMin < lineMax

    // let the user code know that we're going to need all these lines
    OnGetColumnsWidthHint(columnMin, columnMax);

    // do sum up their widths
    wxCoord width = 0;
    for ( size_t column = columnMin; column < columnMax; column++ )
    {
        width += OnGetColumnWidth(column);
    }

    return width;
}

size_t wxHVScrolledWindow::FindFirstFromBottom(size_t rowLast, bool full)
{
    const wxCoord hWindow = GetClientSize().y;

    // go upwards until we arrive at a line such that lineLast is not visible
    // any more when it is shown
    size_t lineFirst = rowLast;
    wxCoord h = 0;
    for ( ;; )
    {
        h += OnGetRowHeight(lineFirst);

        if ( h > hWindow )
        {
            // for this line to be fully visible we need to go one line
            // down, but if it is enough for it to be only partly visible then
            // this line will do as well
            if ( full )
            {
                lineFirst++;
            }

            break;
        }

        if ( !lineFirst )
            break;

        lineFirst--;
    }

    return lineFirst;
}

size_t wxHVScrolledWindow::FindFirstFromRight(size_t columnLast, bool full)
{
    const wxCoord wWindow = GetClientSize().x;

    // go upwards until we arrive at a line such that lineLast is not visible
    // any more when it is shown
    size_t lineFirst = columnLast;
    wxCoord w = 0;
    for ( ;; )
    {
        w += OnGetColumnWidth(lineFirst);

        if ( w > wWindow )
        {
            // for this line to be fully visible we need to go one line
            // down, but if it is enough for it to be only partly visible then
            // this line will do as well
            if ( full )
            {
                lineFirst++;
            }

            break;
        }

        if ( !lineFirst )
            break;

        lineFirst--;
    }

    return lineFirst;
}

void wxHVScrolledWindow::UpdateScrollbars()
{
    // see how many lines can we fit on screen (on both axes)
    const wxCoord wWindow = GetClientSize().x;
    const wxCoord hWindow = GetClientSize().y;

    // first do the horizontal calculations
    wxCoord w = 0;
    size_t column;
    for ( column = m_columnsFirst; column < m_columnsMax; column++ )
    {
        if ( w > wWindow )
            break;

        w += OnGetColumnWidth(column);
    }

    m_nColumnsVisible = column - m_columnsFirst;

    int columnsPageSize = m_nColumnsVisible;
    if ( w > wWindow )
    {
        // last line is only partially visible, we still need the scrollbar and
        // so we have to "fix" pageSize because if it is equal to
        // m_horizLineMax the scrollbar is not shown at all under MSW
        columnsPageSize--;
    }

    // set the scrollbar parameters to reflect this
    SetScrollbar(wxHORIZONTAL, m_columnsFirst, columnsPageSize, m_columnsMax);


    // now do the vertical calculations
    wxCoord h = 0;
    size_t row;
    for ( row = m_rowsFirst; row < m_rowsMax; row++ )
    {
        if ( h > hWindow )
            break;

        h += OnGetRowHeight(row);
    }

    m_nRowsVisible = row - m_rowsFirst;

    int rowsPageSize = m_nRowsVisible;
    if ( h > hWindow )
    {
        // last line is only partially visible, we still need the scrollbar and
        // so we have to "fix" pageSize because if it is equal to m_vertLineMax
        // the scrollbar is not shown at all under MSW
        rowsPageSize--;
    }

    // set the scrollbar parameters to reflect this
    SetScrollbar(wxVERTICAL, m_rowsFirst, rowsPageSize, m_rowsMax);
}

void wxHVScrolledWindow::PrepareDC(wxDC& dc)
{
    if(m_physicalScrolling)
    {
        dc.SetDeviceOrigin(-GetColumnsWidth(0, GetVisibleColumnsBegin()),
                           -GetRowsHeight(0, GetVisibleRowsBegin()));
    }
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void wxHVScrolledWindow::SetRowColumnCounts(size_t rowCount, size_t columnCount)
{
    // save the number of lines
    m_rowsMax = rowCount;
    m_columnsMax = columnCount;

    // and our estimate for their total height and width
    m_heightTotal = EstimateTotalHeight();
    m_widthTotal = EstimateTotalWidth();

    // recalculate the scrollbars parameters
    if(m_rowsFirst >= rowCount)
        m_rowsFirst = (rowCount > 0) ? rowCount - 1 : 0;

    if(m_columnsFirst >= columnCount)
        m_columnsFirst = (columnCount > 0) ? columnCount - 1 : 0;

#if 0
    // checks disabled due to size_t type of members
    // but leave them here if anyone would want to do some debugging
    if(m_rowsFirst < 0)
        m_rowsFirst = 0;

    if(m_columnsFirst < 0)
        m_columnsFirst = 0;
#endif

    ScrollToRowColumn(m_rowsFirst, m_columnsFirst);
}

void wxHVScrolledWindow::RefreshColumn(size_t column)
{
    // is this line visible?
    if ( !IsColumnVisible(column) )
    {
        // no, it is useless to do anything
        return;
    }

    // calculate the rect occupied by this line on screen
    wxRect rect;
    rect.width = OnGetColumnWidth(column);
    rect.height = GetClientSize().y;
    for ( size_t n = GetVisibleColumnsBegin(); n < column; n++ )
    {
        rect.y += OnGetColumnWidth(n);
    }

    // do refresh it
    RefreshRect(rect);
}

void wxHVScrolledWindow::RefreshRow(size_t row)
{
    // is this line visible?
    if ( !IsRowVisible(row) )
    {
        // no, it is useless to do anything
        return;
    }

    // calculate the rect occupied by this line on screen
    wxRect rect;
    rect.width = GetClientSize().x;
    rect.height = OnGetRowHeight(row);
    for ( size_t n = GetVisibleRowsBegin(); n < row; n++ )
    {
        rect.y += OnGetRowHeight(n);
    }

    // do refresh it
    RefreshRect(rect);
}

void wxHVScrolledWindow::RefreshRowColumn(size_t row, size_t column)
{
    // is this line visible?
    if ( !IsRowVisible(row) || !IsColumnVisible(column) )
    {
        // no, it is useless to do anything
        return;
    }

    // calculate the rect occupied by this cell on screen
    wxRect rect;
    rect.height = OnGetRowHeight(row);
    rect.width = OnGetColumnWidth(column);

    size_t n;

    for ( n = GetVisibleRowsBegin(); n < row; n++ )
    {
        rect.y += OnGetRowHeight(n);
    }

    for ( n = GetVisibleColumnsBegin(); n < column; n++ )
    {
        rect.x += OnGetColumnWidth(n);
    }

    // do refresh it
    RefreshRect(rect);
}

void wxHVScrolledWindow::RefreshRows(size_t from, size_t to)
{
    wxASSERT_MSG( from <= to, _T("RefreshRows(): empty range") );

    // clump the range to just the visible lines -- it is useless to refresh
    // the other ones
    if ( from < GetVisibleRowsBegin() )
        from = GetVisibleRowsBegin();

    if ( to > GetVisibleRowsEnd() )
        to = GetVisibleRowsEnd();

    // calculate the rect occupied by these lines on screen
    wxRect rect;
    rect.width = GetClientSize().x;
    for ( size_t nBefore = GetVisibleRowsBegin();
          nBefore < from;
          nBefore++ )
    {
        rect.y += OnGetRowHeight(nBefore);
    }

    for ( size_t nBetween = from; nBetween <= to; nBetween++ )
    {
        rect.height += OnGetRowHeight(nBetween);
    }

    // do refresh it
    RefreshRect(rect);
}

void wxHVScrolledWindow::RefreshColumns(size_t from, size_t to)
{
    wxASSERT_MSG( from <= to, _T("RefreshColumns(): empty range") );

    // clump the range to just the visible lines -- it is useless to refresh
    // the other ones
    if ( from < GetVisibleColumnsBegin() )
        from = GetVisibleColumnsBegin();

    if ( to > GetVisibleColumnsEnd() )
        to = GetVisibleColumnsEnd();

    // calculate the rect occupied by these lines on screen
    wxRect rect;
    rect.height = GetClientSize().y;
    for ( size_t nBefore = GetVisibleColumnsBegin();
          nBefore < from;
          nBefore++ )
    {
        rect.x += OnGetColumnWidth(nBefore);
    }

    for ( size_t nBetween = from; nBetween <= to; nBetween++ )
    {
        rect.width += OnGetColumnWidth(nBetween);
    }

    // do refresh it
    RefreshRect(rect);
}

void wxHVScrolledWindow::RefreshRowsColumns(size_t fromRow, size_t toRow,
                                            size_t fromColumn, size_t toColumn)
{
    wxASSERT_MSG( fromRow <= toRow || fromColumn <= toColumn,
        _T("RefreshRowsColumns(): empty range") );

    // clump the range to just the visible lines -- it is useless to refresh
    // the other ones
    if ( fromRow < GetVisibleRowsBegin() )
        fromRow = GetVisibleRowsBegin();

    if ( toRow > GetVisibleRowsEnd() )
        toRow = GetVisibleRowsEnd();

    if ( fromColumn < GetVisibleColumnsBegin() )
        fromColumn = GetVisibleColumnsBegin();

    if ( toColumn > GetVisibleColumnsEnd() )
        toColumn = GetVisibleColumnsEnd();

    // calculate the rect occupied by these lines on screen
    wxRect rect;
    size_t nBefore, nBetween;

    for ( nBefore = GetVisibleRowsBegin();
          nBefore < fromRow;
          nBefore++ )
    {
        rect.y += OnGetRowHeight(nBefore);
    }

    for ( nBetween = fromRow; nBetween <= toRow; nBetween++ )
    {
        rect.height += OnGetRowHeight(nBetween);
    }

    for ( nBefore = GetVisibleColumnsBegin();
          nBefore < fromColumn;
          nBefore++ )
    {
        rect.x += OnGetColumnWidth(nBefore);
    }

    for ( nBetween = fromColumn; nBetween <= toColumn; nBetween++ )
    {
        rect.width += OnGetColumnWidth(nBetween);
    }

    // do refresh it
    RefreshRect(rect);
}

void wxHVScrolledWindow::RefreshAll()
{
    UpdateScrollbars();

    Refresh();
}

bool wxHVScrolledWindow::Layout()
{
    if(GetSizer() && m_physicalScrolling)
    {
        // adjust the sizer dimensions/position taking into account the
        // virtual size and scrolled position of the window.

        int x, y, w, h;

        y = -GetRowsHeight(0, GetVisibleRowsBegin());
        x = -GetColumnsWidth(0, GetVisibleColumnsBegin());
        GetVirtualSize(&w, &h);
        GetSizer()->SetDimension(0, y, w, h);
        return true;
    }

    // fall back to default for LayoutConstraints
    return wxPanel::Layout();
}

wxPoint wxHVScrolledWindow::HitTest(wxCoord x, wxCoord y) const
{
    const size_t rowMax = GetVisibleRowsEnd();
    const size_t columnMax = GetVisibleColumnsEnd();

    wxPoint hit(wxNOT_FOUND, wxNOT_FOUND);
    for ( size_t row = GetVisibleRowsBegin();
          row <= rowMax;
          row++ )
    {
        y -= OnGetRowHeight(row);
        if ( y < 0 )
            hit.y = row;
    }

    for ( size_t column = GetVisibleColumnsBegin();
          column <= columnMax;
          column++ )
    {
        x -= OnGetColumnWidth(column);
        if ( x < 0 )
            hit.x = column;
    }

    return hit;
}

// ----------------------------------------------------------------------------
// scrolling
// ----------------------------------------------------------------------------

bool wxHVScrolledWindow::ScrollToRowColumn(size_t row, size_t column)
{
    if ( !m_rowsMax && !m_columnsMax )
    {
        // we're empty, code below doesn't make sense in this case
        return false;
    }

    bool scrolled = false;
    scrolled |= ScrollToRow(row);
    scrolled |= ScrollToColumn(column);

    return scrolled;
}

bool wxHVScrolledWindow::ScrollToRow(size_t row)
{
    if ( !m_rowsMax )
    {
        // we're empty, code below doesn't make sense in this case
        return false;
    }

    // determine the real first line to scroll to: we shouldn't scroll beyond
    // the end
    size_t lineFirstLast = FindFirstFromBottom(m_rowsMax - 1, true);
    if ( row > lineFirstLast )
        row = lineFirstLast;

    // anything to do?
    if ( row == m_rowsFirst )
    {
        // no
        return false;
    }


    // remember the currently shown lines for the refresh code below
    size_t lineFirstOld = GetVisibleRowsBegin();

    m_rowsFirst = row;


    // the size of scrollbar thumb could have changed
    UpdateScrollbars();


    // finally, scroll the actual window contents vertically
    if(m_physicalScrolling)
        ScrollWindow(0, GetRowsHeight(GetVisibleRowsBegin(), lineFirstOld));

    return true;
}

bool wxHVScrolledWindow::ScrollToColumn(size_t column)
{
    if ( !m_columnsMax )
    {
        // we're empty, code below doesn't make sense in this case
        return false;
    }

    // determine the real first line to scroll to: we shouldn't scroll beyond
    // the end
    size_t lineFirstLast = FindFirstFromRight(m_columnsMax - 1, true);
    if ( column > lineFirstLast )
        column = lineFirstLast;

    // anything to do?
    if ( column == m_columnsFirst )
    {
        // no
        return false;
    }


    // remember the currently shown lines for the refresh code below
    size_t lineFirstOld = GetVisibleColumnsBegin();

    m_columnsFirst = column;


    // the size of scrollbar thumb could have changed
    UpdateScrollbars();

    // finally, scroll the actual window contents horizontally
    if(m_physicalScrolling)
        ScrollWindow(GetColumnsWidth(GetVisibleColumnsBegin(), lineFirstOld), 0);

    return true;
}

bool wxHVScrolledWindow::ScrollRows(int rows)
{
    rows += m_rowsFirst;
    if ( rows < 0 )
        rows = 0;

    return ScrollToRow(rows);
}

bool wxHVScrolledWindow::ScrollColumns(int columns)
{
    columns += m_columnsFirst;
    if ( columns < 0 )
        columns = 0;

    return ScrollToColumn(columns);
}

bool wxHVScrolledWindow::ScrollRowsColumns(int rows, int columns)
{
    rows += m_rowsFirst;
    if ( rows < 0 )
        rows = 0;

    columns += m_columnsFirst;
    if ( columns < 0 )
        columns = 0;

    return ScrollToRowColumn(rows, columns);
}

bool wxHVScrolledWindow::ScrollRowPages(int pages)
{
    bool didSomething = false;

    while ( pages )
    {
        int line;
        if ( pages > 0 )
        {
            line = GetVisibleRowsEnd();
            if ( line )
                line--;
            pages--;
        }
        else // pages < 0
        {
            line = FindFirstFromBottom(GetVisibleRowsEnd());
            pages++;
        }

        didSomething = ScrollToRow(line);
    }

    return didSomething;
}

bool wxHVScrolledWindow::ScrollColumnPages(int pages)
{
    bool didSomething = false;

    while ( pages )
    {
        int line;
        if ( pages > 0 )
        {
            line = GetVisibleColumnsEnd();
            if ( line )
                line--;
            pages--;
        }
        else // pages < 0
        {
            line = FindFirstFromRight(GetVisibleColumnsEnd());
            pages++;
        }

        didSomething = ScrollToColumn(line);
    }

    return didSomething;
}

bool wxHVScrolledWindow::ScrollPages(int rowPages, int columnPages)
{
    bool didSomething = false;

    while ( rowPages )
    {
        int line;
        if ( rowPages > 0 )
        {
            line = GetVisibleRowsEnd();
            if ( line )
                line--;
            rowPages--;
        }
        else // rowPages < 0
        {
            line = FindFirstFromBottom(GetVisibleRowsBegin());
            rowPages++;
        }

        didSomething = ScrollToRow(line);
    }

    while ( columnPages )
    {
        int line;
        if ( columnPages > 0 )
        {
            line = GetVisibleColumnsEnd();
            if ( line )
                line--;
            columnPages--;
        }
        else // columnPages < 0
        {
            line = FindFirstFromRight(GetVisibleColumnsBegin());
            columnPages++;
        }

        didSomething |= ScrollToColumn(line);
    }

    return didSomething;
}

// ----------------------------------------------------------------------------
// event handling
// ----------------------------------------------------------------------------

void wxHVScrolledWindow::OnSize(wxSizeEvent& event)
{
    UpdateScrollbars();
    Layout();

    event.Skip();
}

void wxHVScrolledWindow::OnScroll(wxScrollWinEvent& event)
{
    if(event.GetOrientation() == wxHORIZONTAL)
    {
        size_t columnsFirstNew;
        const wxEventType evtType = event.GetEventType();

        if ( evtType == wxEVT_SCROLLWIN_TOP )
        {
            columnsFirstNew = 0;
        }
        else if ( evtType == wxEVT_SCROLLWIN_BOTTOM )
        {
            columnsFirstNew = m_columnsMax;
        }
        else if ( evtType == wxEVT_SCROLLWIN_LINEUP )
        {
            columnsFirstNew = m_columnsFirst ? m_columnsFirst - 1 : 0;
        }
        else if ( evtType == wxEVT_SCROLLWIN_LINEDOWN )
        {
            columnsFirstNew = m_columnsFirst + 1;
        }
        else if ( evtType == wxEVT_SCROLLWIN_PAGEUP )
        {
            columnsFirstNew = FindFirstFromRight(m_columnsFirst);
        }
        else if ( evtType == wxEVT_SCROLLWIN_PAGEDOWN )
        {
            columnsFirstNew = GetVisibleColumnsEnd();
            if ( columnsFirstNew )
                columnsFirstNew--;
        }
        else if ( evtType == wxEVT_SCROLLWIN_THUMBRELEASE )
        {
            columnsFirstNew = event.GetPosition();
        }
        else if ( evtType == wxEVT_SCROLLWIN_THUMBTRACK )
        {
            columnsFirstNew = event.GetPosition();
        }

        else // unknown scroll event?
        {
            wxFAIL_MSG( _T("unknown scroll event type?") );
            return;
        }

        ScrollToColumn(columnsFirstNew);
    }
    else if(event.GetOrientation() == wxVERTICAL)
    {
        size_t rowsFirstNew;
        const wxEventType evtType = event.GetEventType();

        if ( evtType == wxEVT_SCROLLWIN_TOP )
        {
            rowsFirstNew = 0;
        }
        else if ( evtType == wxEVT_SCROLLWIN_BOTTOM )
        {
            rowsFirstNew = m_rowsMax;
        }
        else if ( evtType == wxEVT_SCROLLWIN_LINEUP )
        {
            rowsFirstNew = m_rowsFirst ? m_rowsFirst - 1 : 0;
        }
        else if ( evtType == wxEVT_SCROLLWIN_LINEDOWN )
        {
            rowsFirstNew = m_rowsFirst + 1;
        }
        else if ( evtType == wxEVT_SCROLLWIN_PAGEUP )
        {
            rowsFirstNew = FindFirstFromBottom(m_rowsFirst);
        }
        else if ( evtType == wxEVT_SCROLLWIN_PAGEDOWN )
        {
            rowsFirstNew = GetVisibleRowsEnd();
            if ( rowsFirstNew )
                rowsFirstNew--;
        }
        else if ( evtType == wxEVT_SCROLLWIN_THUMBRELEASE )
        {
            rowsFirstNew = event.GetPosition();
        }
        else if ( evtType == wxEVT_SCROLLWIN_THUMBTRACK )
        {
            rowsFirstNew = event.GetPosition();
        }

        else // unknown scroll event?
        {
            wxFAIL_MSG( _T("unknown scroll event type?") );
            return;
        }

        ScrollToRow(rowsFirstNew);
    }


#ifdef __WXMAC__
    Update();
#endif // __WXMAC__
}

#if wxUSE_MOUSEWHEEL

void wxHVScrolledWindow::OnMouseWheel(wxMouseEvent& event)
{
    m_sumWheelRotation += event.GetWheelRotation();
    int delta = event.GetWheelDelta();

    // how much to scroll this time
    int units_to_scroll = -(m_sumWheelRotation/delta);
    if ( !units_to_scroll )
        return;

    m_sumWheelRotation += units_to_scroll*delta;

    if ( !event.IsPageScroll() )
        ScrollRows( units_to_scroll*event.GetLinesPerAction() );
    else
        // scroll pages instead of lines
        ScrollRowPages( units_to_scroll );
}

#endif
