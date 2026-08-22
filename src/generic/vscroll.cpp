/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/vscroll.cpp
// Purpose:     wxVScrolledWindow implementation
// Author:      Vadim Zeitlin
// Modified by: Brad Anderson, David Warkentin
// Created:     30.05.03
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
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

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/sizer.h"
#endif

#include "wx/vscroll.h"

#include "wx/utils.h"   // For wxMin/wxMax().
#include "wx/weakref.h"

// ============================================================================
// wxVarScrollHelperEvtHandler declaration
// ============================================================================

// ----------------------------------------------------------------------------
// wxScrollHelperEvtHandler: intercept the events from the window and forward
// them to wxVarScrollHelperBase
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxVarScrollHelperEvtHandler : public wxEvtHandler
{
public:
    wxVarScrollHelperEvtHandler(wxVarScrollHelperBase *scrollHelper,
                                wxWindow *window)
        : m_scrollHelper(scrollHelper),
          m_window(window)
    {
    }

    virtual bool ProcessEvent(wxEvent& event) override;

private:
    wxVarScrollHelperBase *m_scrollHelper;
    wxWeakRef<wxWindow> m_window;

    wxDECLARE_NO_COPY_CLASS(wxVarScrollHelperEvtHandler);
};

// ============================================================================
// wxVarScrollHelperEvtHandler implementation
// ============================================================================

// FIXME: This method totally duplicates a method with the same name in
//        wxScrollHelperEvtHandler, we really should merge them by reusing the
//        common parts in wxAnyScrollHelperBase.
bool wxVarScrollHelperEvtHandler::ProcessEvent(wxEvent& event)
{
    const wxEventType evType = event.GetEventType();

    // The window is allowed to destroy itself from any of its handlers. Keep
    // everything needed after the call below in locals, as destroying the
    // window also removes and deletes this pushed event handler.
    wxEvtHandler * const nextHandler = m_nextHandler;
    wxVarScrollHelperBase * const scrollHelper = m_scrollHelper;
    const wxWeakRef<wxWindow> weakWindow(m_window);

    // Pass it on to the real handler: notice that we must not call
    // ProcessEvent() on this object itself as it wouldn't pass it to the next
    // handler (i.e. the real window) if we're called from a previous handler
    // (as indicated by "process here only" flag being set) and we do want to
    // execute the handler defined in the window we're associated with right
    // now, without waiting until TryAfter() is called from wxEvtHandler.
    bool processed = nextHandler->ProcessEvent(event);

    if ( !weakWindow )
    {
        event.DidntHonourProcessOnlyIn();
        return processed;
    }

    // always process the size events ourselves, even if the user code handles
    // them as well, as we need to AdjustScrollbars()
    //
    // NB: it is important to do it after processing the event in the normal
    //     way as HandleOnSize() may generate a wxEVT_SIZE itself if the
    //     scrollbar[s] (dis)appear and it should be seen by the user code
    //     after this one
    if ( evType == wxEVT_SIZE )
    {
        scrollHelper->HandleOnSize((wxSizeEvent &)event);
        return true;
    }

    // For wxEVT_PAINT the user code can either handle this event as usual or
    // override virtual OnDraw(), so if the event hasn't been handled we need
    // to call this virtual function ourselves.
    if (
#ifndef __WXUNIVERSAL__
          // in wxUniversal "processed" will always be true, because
          // all windows use the paint event to draw themselves.
          // In this case we can't use this flag to determine if a custom
          // paint event handler already drew our window and we just
          // call OnDraw() anyway.
          !processed &&
#endif // !__WXUNIVERSAL__
            evType == wxEVT_PAINT )
    {
        scrollHelper->HandleOnPaint((wxPaintEvent &)event);
        return true;
    }

    // If the user code handled this event, it should prevent the default
    // handling from taking place, so don't do anything else in this case.
    if ( processed )
        return true;

    // reset the skipped flag (which might have been set to true in
    // ProcessEvent() above) to be able to test it below
    bool wasSkipped = event.GetSkipped();
    if ( wasSkipped )
        event.Skip(false);

    if ( evType == wxEVT_SCROLLWIN_TOP ||
         evType == wxEVT_SCROLLWIN_BOTTOM ||
         evType == wxEVT_SCROLLWIN_LINEUP ||
         evType == wxEVT_SCROLLWIN_LINEDOWN ||
         evType == wxEVT_SCROLLWIN_PAGEUP ||
         evType == wxEVT_SCROLLWIN_PAGEDOWN ||
         evType == wxEVT_SCROLLWIN_THUMBTRACK ||
         evType == wxEVT_SCROLLWIN_THUMBRELEASE )
    {
        scrollHelper->HandleOnScroll((wxScrollWinEvent &)event);
        if ( !event.GetSkipped() )
        {
            // it makes sense to indicate that we processed the message as we
            // did scroll the window (and also notice that wxAutoScrollTimer
            // relies on our return value to stop scrolling when we are at top
            // or bottom already)
            processed = true;
            wasSkipped = false;
        }
    }
#if wxUSE_MOUSEWHEEL
    // Use GTK's own scroll wheel handling in GtkScrolledWindow
#ifndef __WXGTK__
    else if ( evType == wxEVT_MOUSEWHEEL )
    {
        scrollHelper->HandleOnMouseWheel((wxMouseEvent &)event);
        return true;
    }
#endif
#endif // wxUSE_MOUSEWHEEL
    else if ( evType == wxEVT_CHAR &&
                (scrollHelper->GetOrientation() == wxVERTICAL) )
    {
        scrollHelper->HandleOnChar((wxKeyEvent &)event);
        if ( !event.GetSkipped() )
        {
            processed = true;
            wasSkipped = false;
        }
    }

    event.Skip(wasSkipped);

    // We called ProcessEvent() on the next handler, meaning that we explicitly
    // worked around the request to process the event in this handler only. As
    // explained above, this is unfortunately really necessary but the trouble
    // is that the event will continue to be post-processed by the previous
    // handler resulting in duplicate calls to event handlers. Call the special
    // function below to prevent this from happening, base class DoTryChain()
    // will check for it and behave accordingly.
    //
    // And if we're not called from DoTryChain(), this won't do anything anyhow.
    event.DidntHonourProcessOnlyIn();

    return processed;
}


// ============================================================================
// wxVarScrollHelperBase implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxVarScrollHelperBase initialization
// ----------------------------------------------------------------------------

wxVarScrollHelperBase::wxVarScrollHelperBase(wxWindow *win)
    : wxAnyScrollHelperBase(win)
{
#if wxUSE_MOUSEWHEEL
    m_sumWheelRotation = 0;
#endif

    m_unitMax = 0;
    m_sizeTotal = 0;
    m_unitFirst = 0;

    m_physicalScrolling = true;
    m_handler = nullptr;

    // by default, the associated window is also the target window
    DoSetTargetWindow(win);
}

wxVarScrollHelperBase::~wxVarScrollHelperBase()
{
    DeleteEvtHandler();
}

// ----------------------------------------------------------------------------
// wxVarScrollHelperBase various helpers
// ----------------------------------------------------------------------------

void
wxVarScrollHelperBase::AssignOrient(wxCoord& x,
                                    wxCoord& y,
                                    wxCoord first,
                                    wxCoord second)
{
    if ( GetOrientation() == wxVERTICAL )
    {
        x = first;
        y = second;
    }
    else // horizontal
    {
        x = second;
        y = first;
    }
}

void
wxVarScrollHelperBase::IncOrient(wxCoord& x, wxCoord& y, wxCoord inc)
{
    if ( GetOrientation() == wxVERTICAL )
        y += inc;
    else
        x += inc;
}

wxCoord wxVarScrollHelperBase::DoEstimateTotalSize() const
{
    // estimate the total height: it is impossible to call
    // OnGetUnitSize() for every unit because there may be too many of
    // them, so we just make a guess using some units in the beginning,
    // some in the end and some in the middle
    static const size_t NUM_UNITS_TO_SAMPLE = 10;

    const size_t unitCount = m_unitMax;
    const wxWeakRef<wxWindow> weakWindow(m_win);

    wxCoord sizeTotal;
    if ( unitCount < 3*NUM_UNITS_TO_SAMPLE )
    {
        // in this case, full calculations are faster and more correct than
        // guessing
        sizeTotal = GetUnitsSize(0, unitCount);
    }
    else // too many units to calculate exactly
    {
        // look at some units in the beginning/middle/end
        sizeTotal = GetUnitsSize(0, NUM_UNITS_TO_SAMPLE);
        if ( !weakWindow || m_unitMax != unitCount )
            return 0;

        sizeTotal += GetUnitsSize(unitCount - NUM_UNITS_TO_SAMPLE,
                                 unitCount);
        if ( !weakWindow || m_unitMax != unitCount )
            return 0;

        sizeTotal +=
            GetUnitsSize(unitCount/2 - NUM_UNITS_TO_SAMPLE/2,
                         unitCount/2 + NUM_UNITS_TO_SAMPLE/2);

        // use the height of the units we looked as the average
        sizeTotal = (wxCoord)
                (((float)sizeTotal / (3*NUM_UNITS_TO_SAMPLE)) * unitCount);
    }

    if ( !weakWindow || m_unitMax != unitCount )
        return 0;

    return sizeTotal;
}

wxCoord wxVarScrollHelperBase::GetUnitsSize(size_t unitMin, size_t unitMax) const
{
    if ( unitMin == unitMax )
        return 0;
    else if ( unitMin > unitMax )
        return -GetUnitsSize(unitMax, unitMin);
    //else: unitMin < unitMax

    const size_t unitCount = m_unitMax;
    if ( unitMin >= unitCount )
        return 0;

    unitMax = wxMin(unitMax, unitCount);

    const wxWeakRef<wxWindow> weakWindow(m_win);

    // let the user code know that we're going to need all these units
    OnGetUnitsSizeHint(unitMin, unitMax);
    if ( !weakWindow || m_unitMax != unitCount )
        return 0;

    // sum up their sizes
    wxCoord size = 0;
    for ( size_t unit = unitMin; unit < unitMax; ++unit )
    {
        const wxCoord unitSize = OnGetUnitSize(unit);
        if ( !weakWindow || m_unitMax != unitCount )
            return 0;

        size += unitSize;
    }

    return size;
}

size_t wxVarScrollHelperBase::FindFirstVisibleFromLast(size_t unitLast, bool full) const
{
    const size_t unitCount = m_unitMax;
    if ( !unitCount )
        return 0;

    unitLast = wxMin(unitLast, unitCount - 1);

    const wxWeakRef<wxWindow> weakWindow(m_win);
    const wxCoord sWindow = GetOrientationTargetSize();

    // go upwards until we arrive at a unit such that unitLast is not visible
    // any more when it is shown
    size_t unitFirst = unitLast;
    wxCoord s = 0;
    for ( ;; )
    {
        const wxCoord unitSize = OnGetUnitSize(unitFirst);
        if ( !weakWindow || m_unitMax != unitCount )
            return 0;

        s += unitSize;

        if ( s > sWindow )
        {
            // for this unit to be fully visible we need to go one unit
            // down, but if it is enough for it to be only partly visible then
            // this unit will do as well
            if ( full )
            {
                ++unitFirst;
            }

            break;
        }

        if ( !unitFirst )
            break;

        --unitFirst;
    }

    return unitFirst;
}

size_t wxVarScrollHelperBase::GetNewScrollPosition(wxScrollWinEvent& event) const
{
    if ( !m_unitMax )
        return 0;

    wxEventType evtType = event.GetEventType();

    if ( evtType == wxEVT_SCROLLWIN_TOP )
    {
        return 0;
    }
    else if ( evtType == wxEVT_SCROLLWIN_BOTTOM )
    {
        return m_unitMax - 1;
    }
    else if ( evtType == wxEVT_SCROLLWIN_LINEUP )
    {
        return m_unitFirst ? m_unitFirst - 1 : 0;
    }
    else if ( evtType == wxEVT_SCROLLWIN_LINEDOWN )
    {
        return wxMin(m_unitFirst + 1, m_unitMax - 1);
    }
    else if ( evtType == wxEVT_SCROLLWIN_PAGEUP )
    {
        // Page up should do at least as much as line up.
        const size_t unitCount = m_unitMax;
        const size_t unitFirst = m_unitFirst;
        const wxWeakRef<wxWindow> weakWindow(m_win);
        const size_t pageFirst = FindFirstVisibleFromLast(unitFirst);
        if ( !weakWindow || m_unitMax != unitCount ||
             m_unitFirst != unitFirst )
        {
            return 0;
        }

        return wxMin(pageFirst, unitFirst ? unitFirst - 1 : 0);
    }
    else if ( evtType == wxEVT_SCROLLWIN_PAGEDOWN )
    {
        // And page down should do at least as much as line down.
        if ( GetVisibleEnd() )
            return wxMax(GetVisibleEnd() - 1, wxMin(m_unitFirst + 1, m_unitMax - 1));
        else
            return wxMax(GetVisibleEnd(), wxMin(m_unitFirst + 1, m_unitMax - 1));
    }
    else if ( evtType == wxEVT_SCROLLWIN_THUMBRELEASE )
    {
        return event.GetPosition();
    }
    else if ( evtType == wxEVT_SCROLLWIN_THUMBTRACK )
    {
        return event.GetPosition();
    }

    // unknown scroll event?
    wxFAIL_MSG( wxT("unknown scroll event type?") );
    return 0;
}

void wxVarScrollHelperBase::UpdateScrollbar()
{
    // if there is nothing to scroll, remove the scrollbar
    if ( !m_unitMax )
    {
        RemoveScrollbar();
        return;
    }

    const size_t unitCount = m_unitMax;
    const wxWeakRef<wxWindow> weakWindow(m_win);

    // see how many units can we fit on screen
    const wxCoord sWindow = GetOrientationTargetSize();

    // do vertical calculations
    wxCoord s = 0;
    size_t unit;
    for ( unit = m_unitFirst; unit < unitCount; ++unit )
    {
        if ( s > sWindow )
            break;

        const wxCoord unitSize = OnGetUnitSize(unit);
        if ( !weakWindow || m_unitMax != unitCount )
            return;

        s += unitSize;
    }

    m_nUnitsVisible = unit - m_unitFirst;

    int unitsPageSize = m_nUnitsVisible;
    if ( s > sWindow )
    {
        // last unit is only partially visible, we still need the scrollbar and
        // so we have to "fix" pageSize because if it is equal to m_unitMax
        // the scrollbar is not shown at all under MSW
        --unitsPageSize;
    }

    // set the scrollbar parameters to reflect this
    m_win->SetScrollbar(GetOrientation(), m_unitFirst, unitsPageSize, unitCount);
}

void wxVarScrollHelperBase::RemoveScrollbar()
{
    m_unitFirst = 0;
    m_nUnitsVisible = m_unitMax;
    m_win->SetScrollbar(GetOrientation(), 0, 0, 0);
}

void wxVarScrollHelperBase::DeleteEvtHandler()
{
    // search for m_handler in the handler list
    if ( m_win && m_handler )
    {
        if ( m_win->RemoveEventHandler(m_handler) )
        {
            delete m_handler;
        }
        //else: something is very wrong, so better [maybe] leak memory than
        //      risk a crash because of double deletion

        m_handler = nullptr;
    }
}

void wxVarScrollHelperBase::DoSetTargetWindow(wxWindow *target)
{
    m_targetWindow = target;
#ifdef __WXMAC__
    target->MacSetClipChildren() ;
#endif

    // install the event handler which will intercept the events we're
    // interested in (but only do it for our real window, not the target window
    // which we scroll - we don't need to hijack its events)
    if ( m_targetWindow == m_win )
    {
        // if we already have a handler, delete it first
        DeleteEvtHandler();

        m_handler = new wxVarScrollHelperEvtHandler(this, m_win);
        m_targetWindow->PushEventHandler(m_handler);
    }
}

// ----------------------------------------------------------------------------
// wxVarScrollHelperBase operations
// ----------------------------------------------------------------------------

void wxVarScrollHelperBase::SetTargetWindow(wxWindow *target)
{
    wxCHECK_RET( target, wxT("target window must not be null") );

    if ( target == m_targetWindow )
        return;

    DoSetTargetWindow(target);
}

void wxVarScrollHelperBase::SetUnitCount(size_t count)
{
    // save the number of units
    m_unitMax = count;

    const wxWeakRef<wxWindow> weakWindow(m_win);

    // and our estimate for their total height
    const wxCoord sizeTotal = EstimateTotalSize();
    if ( !weakWindow || m_unitMax != count )
        return;

    m_sizeTotal = sizeTotal;

    // ScrollToUnit() will update the scrollbar itself if it changes the unit
    // we pass to it because it's out of [new] range
    size_t oldScrollPos = m_unitFirst;
    DoScrollToUnit(m_unitFirst);
    if ( !weakWindow || m_unitMax != count )
        return;

    if ( oldScrollPos == m_unitFirst )
    {
        // but if it didn't do it, we still need to update the scrollbar to
        // reflect the changed number of units ourselves
        UpdateScrollbar();
    }
}

void wxVarScrollHelperBase::RefreshUnit(size_t unit)
{
    const size_t unitCount = m_unitMax;

    // is this unit visible?
    if ( unit >= unitCount || !IsVisible(unit) )
    {
        // no, it is useless to do anything
        return;
    }

    // calculate the rect occupied by this unit on screen
    const wxWeakRef<wxWindow> weakWindow(m_win);
    const wxWeakRef<wxWindow> weakTarget(m_targetWindow);
    wxRect rect;
    const wxCoord unitSize = OnGetUnitSize(unit);
    if ( !weakWindow || !weakTarget ||
         weakTarget.get() != m_targetWindow ||
         m_unitMax != unitCount )
        return;

    AssignOrient(rect.width, rect.height,
                 GetNonOrientationTargetSize(), unitSize);

    for ( size_t n = GetVisibleBegin(); n < unit; ++n )
    {
        const wxCoord size = OnGetUnitSize(n);
        if ( !weakWindow || !weakTarget ||
             weakTarget.get() != m_targetWindow ||
             m_unitMax != unitCount )
            return;

        IncOrient(rect.x, rect.y, size);
    }

    // do refresh it
    m_targetWindow->RefreshRect(rect);
}

void wxVarScrollHelperBase::RefreshUnits(size_t from, size_t to)
{
    wxASSERT_MSG( from <= to, wxT("RefreshUnits(): empty range") );
    if ( from > to )
        return;

    const size_t unitCount = m_unitMax;
    if ( !unitCount || from >= unitCount )
        return;

    const size_t visibleBegin = GetVisibleBegin();
    const size_t visibleEnd = wxMin(GetVisibleEnd(), unitCount);

    // Clamp the inclusive input range to the half-open visible range.
    if ( visibleBegin >= visibleEnd || to < visibleBegin || from >= visibleEnd )
        return;

    from = wxMax(from, visibleBegin);
    to = wxMin(to, visibleEnd - 1);

    const wxWeakRef<wxWindow> weakWindow(m_win);
    const wxWeakRef<wxWindow> weakTarget(m_targetWindow);

    // calculate the rect occupied by these units on screen
    int orient_size = 0,
        orient_pos = 0;

    int nonorient_size = GetNonOrientationTargetSize();

    for ( size_t nBefore = visibleBegin;
          nBefore < from;
          nBefore++ )
    {
        const wxCoord size = OnGetUnitSize(nBefore);
        if ( !weakWindow || !weakTarget ||
             weakTarget.get() != m_targetWindow ||
             m_unitMax != unitCount )
            return;

        orient_pos += size;
    }

    for ( size_t nBetween = from; nBetween <= to; nBetween++ )
    {
        const wxCoord size = OnGetUnitSize(nBetween);
        if ( !weakWindow || !weakTarget ||
             weakTarget.get() != m_targetWindow ||
             m_unitMax != unitCount )
            return;

        orient_size += size;
    }

    wxRect rect;
    AssignOrient(rect.x, rect.y, 0, orient_pos);
    AssignOrient(rect.width, rect.height, nonorient_size, orient_size);

    // do refresh it
    m_targetWindow->RefreshRect(rect);
}

void wxVarScrollHelperBase::RefreshAll()
{
    const size_t unitCount = m_unitMax;
    const wxWeakRef<wxWindow> weakWindow(m_win);
    const wxWeakRef<wxWindow> weakTarget(m_targetWindow);

    UpdateScrollbar();
    if ( !weakWindow || !weakTarget ||
         weakTarget.get() != m_targetWindow ||
         m_unitMax != unitCount )
        return;

    m_targetWindow->Refresh();
}

bool wxVarScrollHelperBase::ScrollLayout()
{
    if ( m_targetWindow->GetSizer() && m_physicalScrolling )
    {
        // adjust the sizer dimensions/position taking into account the
        // virtual size and scrolled position of the window.

        int x, y;
        AssignOrient(x, y, 0, -GetScrollOffset());

        int w, h;
        m_targetWindow->GetVirtualSize(&w, &h);

        m_targetWindow->GetSizer()->SetDimension(x, y, w, h);
        return true;
    }

    // fall back to default for LayoutConstraints
    return m_targetWindow->wxWindow::Layout();
}

int wxVarScrollHelperBase::VirtualHitTest(wxCoord coord) const
{
    if ( coord < 0 )
        return wxNOT_FOUND;

    const size_t unitCount = m_unitMax;
    const size_t unitMax = wxMin(GetVisibleEnd(), unitCount);
    const wxWeakRef<wxWindow> weakWindow(m_win);

    for ( size_t unit = GetVisibleBegin(); unit < unitMax; ++unit )
    {
        const wxCoord unitSize = OnGetUnitSize(unit);
        if ( !weakWindow || m_unitMax != unitCount )
            return wxNOT_FOUND;

        coord -= unitSize;
        if ( coord < 0 )
            return static_cast<int>(unit);
    }

    return wxNOT_FOUND;
}

// ----------------------------------------------------------------------------
// wxVarScrollHelperBase scrolling
// ----------------------------------------------------------------------------

bool wxVarScrollHelperBase::DoScrollToUnit(size_t unit)
{
    if ( !m_unitMax )
    {
        // we're empty, code below doesn't make sense in this case
        return false;
    }

    const size_t unitCount = m_unitMax;
    const wxWeakRef<wxWindow> weakWindow(m_win);
    const wxWeakRef<wxWindow> weakTarget(m_targetWindow);

    // determine the real first unit to scroll to: we shouldn't scroll beyond
    // the end
    size_t unitFirstLast = FindFirstVisibleFromLast(unitCount - 1, true);
    if ( !weakWindow || !weakTarget ||
         weakTarget.get() != m_targetWindow ||
         m_unitMax != unitCount )
        return false;

    if ( unit > unitFirstLast )
        unit = unitFirstLast;

    // anything to do?
    if ( unit == m_unitFirst )
    {
        // no
        return false;
    }


    // remember the currently shown units for the refresh code below
    size_t unitFirstOld = GetVisibleBegin(),
           unitLastOld = GetVisibleEnd();

    m_unitFirst = unit;


    // the size of scrollbar thumb could have changed
    UpdateScrollbar();
    if ( !weakWindow || !weakTarget ||
         weakTarget.get() != m_targetWindow ||
         m_unitMax != unitCount )
        return false;

    // finally refresh the display -- but only redraw as few units as possible
    // to avoid flicker.  We can't do this if we have children because they
    // won't be scrolled
    if ( m_targetWindow->GetChildren().empty() &&
         (GetVisibleBegin() >= unitLastOld || GetVisibleEnd() <= unitFirstOld) )
    {
        // the simplest case: we don't have any old units left, just redraw
        // everything
        m_targetWindow->Refresh();
    }
    else // scroll the window
    {
        // Avoid scrolling visible parts of the screen on Mac
#ifdef __WXMAC__
        if (m_physicalScrolling && m_targetWindow->IsShownOnScreen())
#else
        if ( m_physicalScrolling )
#endif
        {
            wxCoord dx = 0,
                    dy = GetUnitsSize(GetVisibleBegin(), unitFirstOld);
            if ( !weakWindow || !weakTarget ||
                 weakTarget.get() != m_targetWindow ||
                 m_unitMax != unitCount )
                return false;

            if ( GetOrientation() == wxHORIZONTAL )
            {
                wxCoord tmp = dx;
                dx = dy;
                dy = tmp;
            }

            m_targetWindow->ScrollWindow(dx, dy);
        }
        else // !m_physicalScrolling
        {
            // we still need to invalidate but we can't use ScrollWindow
            // because physical scrolling is disabled (the user either didn't
            // want children scrolled and/or doesn't want pixels to be
            // physically scrolled).
            m_targetWindow->Refresh();
        }
    }

    return true;
}

bool wxVarScrollHelperBase::DoScrollUnits(int units)
{
    units += m_unitFirst;
    if ( units < 0 )
        units = 0;

    return DoScrollToUnit(units);
}

bool wxVarScrollHelperBase::DoScrollPages(int pages)
{
    if ( !m_unitMax || !pages )
        return false;

    const size_t unitCount = m_unitMax;
    const wxWeakRef<wxWindow> weakWindow(m_win);

    bool didSomething = false;

    while ( pages )
    {
        size_t unit;
        if ( pages > 0 )
        {
            unit = GetVisibleEnd();
            if ( unit )
                --unit;
            --pages;
        }
        else // pages < 0
        {
            unit = FindFirstVisibleFromLast(GetVisibleEnd());
            if ( !weakWindow || m_unitMax != unitCount )
                return didSomething;

            ++pages;
        }

        didSomething = DoScrollToUnit(unit);
        if ( !weakWindow || m_unitMax != unitCount )
            return didSomething;
    }

    return didSomething;
}

// ----------------------------------------------------------------------------
// event handling
// ----------------------------------------------------------------------------

void wxVarScrollHelperBase::HandleOnSize(wxSizeEvent& event)
{
    const size_t unitCount = m_unitMax;
    const wxWeakRef<wxWindow> weakWindow(m_win);

    if ( unitCount )
    {
        // sometimes change in varscrollable window's size can result in
        // unused empty space after the last item. Fix it by decrementing
        // first visible item position according to the available space.

        // determine free space
        const wxCoord sWindow = GetOrientationTargetSize();
        wxCoord s = 0;
        size_t unit;
        for ( unit = m_unitFirst; unit < unitCount; ++unit )
        {
            if ( s > sWindow )
                break;

            const wxCoord unitSize = OnGetUnitSize(unit);
            if ( !weakWindow || m_unitMax != unitCount )
            {
                event.Skip();
                return;
            }

            s += unitSize;
        }
        wxCoord freeSpace = sWindow - s;

        // decrement first visible item index as long as there is free space
        size_t idealUnitFirst;
        for ( idealUnitFirst = m_unitFirst;
              idealUnitFirst > 0;
              idealUnitFirst-- )
        {
            const wxCoord us = OnGetUnitSize(idealUnitFirst-1);
            if ( !weakWindow || m_unitMax != unitCount )
            {
                event.Skip();
                return;
            }

            if ( freeSpace < us )
                break;
            freeSpace -= us;
        }
        m_unitFirst = idealUnitFirst;
    }

    UpdateScrollbar();

    event.Skip();
}

void wxVarScrollHelperBase::HandleOnScroll(wxScrollWinEvent& event)
{
    if (GetOrientation() != event.GetOrientation())
    {
        event.Skip();
        return;
    }

    DoScrollToUnit(GetNewScrollPosition(event));

#ifdef __WXMAC__
    UpdateMacScrollWindow();
#endif // __WXMAC__
}

void wxVarScrollHelperBase::DoPrepareDC(wxDC& dc)
{
    if ( m_physicalScrolling )
    {
        wxPoint pt = dc.GetDeviceOrigin();

        IncOrient(pt.x, pt.y, -GetScrollOffset());

        dc.SetDeviceOrigin(pt.x, pt.y);
    }
}

int wxVarScrollHelperBase::DoCalcScrolledPosition(int coord) const
{
    return coord - GetScrollOffset();
}

int wxVarScrollHelperBase::DoCalcUnscrolledPosition(int coord) const
{
    return coord + GetScrollOffset();
}

#if wxUSE_MOUSEWHEEL

void wxVarScrollHelperBase::HandleOnMouseWheel(wxMouseEvent& event)
{
    // we only want to process wheel events for vertical implementations.
    // There is no way to determine wheel orientation (and on MSW horizontal
    // wheel rotation just fakes scroll events, rather than sending a MOUSEWHEEL
    // event).
    if ( GetOrientation() != wxVERTICAL )
        return;

    m_sumWheelRotation += event.GetWheelRotation();
    int delta = event.GetWheelDelta();

    // how much to scroll this time
    int units_to_scroll = -(m_sumWheelRotation/delta);
    if ( !units_to_scroll )
        return;

    m_sumWheelRotation += units_to_scroll*delta;

    if ( !event.IsPageScroll() )
        DoScrollUnits( units_to_scroll*event.GetLinesPerAction() );
    else // scroll pages instead of units
        DoScrollPages( units_to_scroll );
}

#endif // wxUSE_MOUSEWHEEL


// ============================================================================
// wxVarHVScrollHelper implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxVarHVScrollHelper operations
// ----------------------------------------------------------------------------

void wxVarHVScrollHelper::SetRowColumnCount(size_t rowCount, size_t columnCount)
{
    SetRowCount(rowCount);
    SetColumnCount(columnCount);
}

bool wxVarHVScrollHelper::ScrollToRowColumn(size_t row, size_t column)
{
    bool result = false;
    result |= ScrollToRow(row);
    result |= ScrollToColumn(column);
    return result;
}

void wxVarHVScrollHelper::RefreshRowColumn(size_t row, size_t column)
{
    // is this unit visible?
    if ( !IsRowVisible(row) || !IsColumnVisible(column) )
    {
        // no, it is useless to do anything
        return;
    }

    // calculate the rect occupied by this cell on screen
    wxRect v_rect, h_rect;
    v_rect.height = OnGetRowHeight(row);
    h_rect.width = OnGetColumnWidth(column);

    size_t n;

    for ( n = GetVisibleRowsBegin(); n < row; n++ )
    {
        v_rect.y += OnGetRowHeight(n);
    }

    for ( n = GetVisibleColumnsBegin(); n < column; n++ )
    {
        h_rect.x += OnGetColumnWidth(n);
    }

    // refresh but specialize the behaviour if we have a single target window
    if ( wxVarVScrollHelper::GetTargetWindow() == wxVarHScrollHelper::GetTargetWindow() )
    {
        v_rect.x = h_rect.x;
        v_rect.width = h_rect.width;
        wxVarVScrollHelper::GetTargetWindow()->RefreshRect(v_rect);
    }
    else
    {
        v_rect.x = 0;
        v_rect.width = wxVarVScrollHelper::GetNonOrientationTargetSize();
        h_rect.y = 0;
        h_rect.width = wxVarHScrollHelper::GetNonOrientationTargetSize();

        wxVarVScrollHelper::GetTargetWindow()->RefreshRect(v_rect);
        wxVarHScrollHelper::GetTargetWindow()->RefreshRect(h_rect);
    }
}

void wxVarHVScrollHelper::RefreshRowsColumns(size_t fromRow, size_t toRow,
                                             size_t fromColumn, size_t toColumn)
{
    wxASSERT_MSG( fromRow <= toRow || fromColumn <= toColumn,
        wxT("RefreshRowsColumns(): empty range") );

    // clump the range to just the visible units -- it is useless to refresh
    // the other ones
    if ( fromRow < GetVisibleRowsBegin() )
        fromRow = GetVisibleRowsBegin();

    if ( toRow > GetVisibleRowsEnd() )
        toRow = GetVisibleRowsEnd();

    if ( fromColumn < GetVisibleColumnsBegin() )
        fromColumn = GetVisibleColumnsBegin();

    if ( toColumn > GetVisibleColumnsEnd() )
        toColumn = GetVisibleColumnsEnd();

    // calculate the rect occupied by these units on screen
    wxRect v_rect, h_rect;
    size_t nBefore, nBetween;

    for ( nBefore = GetVisibleRowsBegin();
          nBefore < fromRow;
          nBefore++ )
    {
        v_rect.y += OnGetRowHeight(nBefore);
    }

    for ( nBetween = fromRow; nBetween <= toRow; nBetween++ )
    {
        v_rect.height += OnGetRowHeight(nBetween);
    }

    for ( nBefore = GetVisibleColumnsBegin();
          nBefore < fromColumn;
          nBefore++ )
    {
        h_rect.x += OnGetColumnWidth(nBefore);
    }

    for ( nBetween = fromColumn; nBetween <= toColumn; nBetween++ )
    {
        h_rect.width += OnGetColumnWidth(nBetween);
    }

    // refresh but specialize the behaviour if we have a single target window
    if ( wxVarVScrollHelper::GetTargetWindow() == wxVarHScrollHelper::GetTargetWindow() )
    {
        v_rect.x = h_rect.x;
        v_rect.width = h_rect.width;
        wxVarVScrollHelper::GetTargetWindow()->RefreshRect(v_rect);
    }
    else
    {
        v_rect.x = 0;
        v_rect.width = wxVarVScrollHelper::GetNonOrientationTargetSize();
        h_rect.y = 0;
        h_rect.width = wxVarHScrollHelper::GetNonOrientationTargetSize();

        wxVarVScrollHelper::GetTargetWindow()->RefreshRect(v_rect);
        wxVarHScrollHelper::GetTargetWindow()->RefreshRect(h_rect);
    }
}

wxPosition wxVarHVScrollHelper::VirtualHitTest(wxCoord x, wxCoord y) const
{
    return wxPosition(wxVarVScrollHelper::VirtualHitTest(y),
                      wxVarHScrollHelper::VirtualHitTest(x));
}

void wxVarHVScrollHelper::DoPrepareDC(wxDC& dc)
{
    wxVarVScrollHelper::DoPrepareDC(dc);
    wxVarHScrollHelper::DoPrepareDC(dc);
}

bool wxVarHVScrollHelper::ScrollLayout()
{
    bool layout_result = false;
    layout_result |= wxVarVScrollHelper::ScrollLayout();
    layout_result |= wxVarHScrollHelper::ScrollLayout();
    return layout_result;
}

wxSize wxVarHVScrollHelper::GetRowColumnCount() const
{
    return wxSize(GetColumnCount(), GetRowCount());
}

wxPosition wxVarHVScrollHelper::GetVisibleBegin() const
{
    return wxPosition(GetVisibleRowsBegin(), GetVisibleColumnsBegin());
}

wxPosition wxVarHVScrollHelper::GetVisibleEnd() const
{
    return wxPosition(GetVisibleRowsEnd(), GetVisibleColumnsEnd());
}

bool wxVarHVScrollHelper::IsVisible(size_t row, size_t column) const
{
    return IsRowVisible(row) && IsColumnVisible(column);
}


// ============================================================================
// wx[V/H/HV]ScrolledWindow implementations
// ============================================================================

// NB: skipping wxVScrolled<T> in wxRTTI information because being a template,
//     it doesn't and can't implement wxRTTI support
wxIMPLEMENT_ABSTRACT_CLASS(wxVScrolledWindow, wxPanel);
wxIMPLEMENT_ABSTRACT_CLASS(wxVScrolledCanvas, wxWindow);

wxIMPLEMENT_ABSTRACT_CLASS(wxHScrolledWindow, wxPanel);
wxIMPLEMENT_ABSTRACT_CLASS(wxHVScrolledWindow, wxPanel);
