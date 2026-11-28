/////////////////////////////////////////////////////////////////////////////
// Name:        samples/showcase/scrolledpage.h
// Purpose:     Content-driven scrolling policy shared by Showcase pages
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SHOWCASE_SCROLLEDPAGE_H_
#define _WX_SHOWCASE_SCROLLEDPAGE_H_

#include "wx/scrolwin.h"
#if wxUSE_COLLPANE
#include "wx/collpane.h"
#endif

// This is sample policy, not a change to wxCollapsiblePane's default contract
// of fitting its top-level window. Showcase keeps its frame fixed and grows the
// containing page's virtual extent instead.
class ShowcaseScrolledPage final : public wxScrolledWindow
{
public:
    explicit ShowcaseScrolledPage(wxWindow* parent)
        : wxScrolledWindow(parent, wxID_ANY)
    {
        SetScrollRate(0, FromDIP(12));
        Bind(wxEVT_SHOW, [this](wxShowEvent& event)
        {
            event.Skip();
            if ( event.IsShown() && m_contentReady )
                FitInside();
        });
#if wxUSE_COLLPANE
        Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent& event)
        {
            event.Skip();
            if ( m_contentReady )
                FitInside();
        });
#endif
    }

    void FinalizeContent()
    {
        m_contentReady = true;
        // SetVirtualSize also lays out the sizer and adjusts the scrollbars.
        // Subsequent allocation changes retain wxScrolledWindow's normal
        // EVT_SIZE path, including the first final Notebook allocation.
        FitInside();
    }

private:
    bool m_contentReady = false;
};

#endif // _WX_SHOWCASE_SCROLLEDPAGE_H_
