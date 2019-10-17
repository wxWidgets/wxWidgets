///////////////////////////////////////////////////////////////////////////////
// Name:        tests/waitforpaint.h
// Purpose:     Helper WaitForPaint class
// Author:      Vadim Zeitlin
// Created:     2019-10-17 (extracted from tests/window/setsize.cpp)
// Copyright:   (c) 2019 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_WAITFORPAINT_H_
#define _WX_TESTS_WAITFORPAINT_H_

#include "wx/stopwatch.h"
#include "wx/window.h"

// Class used to check if we received the (first) paint event: this is
// currently used under GTK only, as MSW doesn't seem to need to wait for the
// things to work, while under Mac nothing works anyhow.
#ifdef __WXGTK__

class WaitForPaint
{
public:
    explicit WaitForPaint(wxWindow* win)
        : m_win(*win),
          m_painted(false),
          m_handler(&m_painted)
    {
        m_win.Bind(wxEVT_PAINT, m_handler);
    }

    // This function waits up to the given number of milliseconds for the paint
    // event to come and returns true if we did get it or false otherwise.
    bool YieldUntilPainted(int timeoutInMS = 250) const
    {
        wxStopWatch sw;
        for ( ;; )
        {
            wxYield();

            if ( m_painted )
                return true;

            if ( sw.Time() > timeoutInMS )
                return false;
        }
    }

    ~WaitForPaint()
    {
        m_win.Unbind(wxEVT_PAINT, m_handler);
    }

private:
    wxWindow& m_win;
    bool m_painted;

    class PaintHandler
    {
    public:
        // Note that we have to use a pointer here, i.e. we can't just store
        // the flag inside the class itself because it's going to be cloned
        // inside wx and querying the flag of the original copy wouldtn' work.
        explicit PaintHandler(bool* painted)
            : m_painted(*painted)
        {
        }

        void operator()(wxPaintEvent& event)
        {
            event.Skip();
            m_painted = true;
        }

    private:
        bool& m_painted;
    } m_handler;
};

#else // !__WXGTK__

class WaitForPaint
{
public:
    explicit WaitForPaint(wxWindow* WXUNUSED(win))
    {
    }

    bool YieldUntilPainted(int WXUNUSED(timeoutInMS) = 250) const
    {
        return true;
    }
};

#endif // __WXGTK__/!__WXGTK__

#endif // _WX_TESTS_WAITFORPAINT_H_
