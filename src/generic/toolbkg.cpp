///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/toolbkg.cpp
// Purpose:     generic implementation of wxToolbook
// Author:      Julian Smart
// Modified by:
// Created:     2006-01-29
// Copyright:   (c) 2006 Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_TOOLBOOK

#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/settings.h"
    #include "wx/toolbar.h"
#endif

#include "wx/imaglist.h"
#include "wx/sysopt.h"
#include "wx/toolbook.h"

#if defined(__WXMAC__) && wxUSE_TOOLBAR && wxUSE_BMPBUTTON
#include "wx/generic/buttonbar.h"
#endif

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxToolbook, wxBookCtrlBase);

wxDEFINE_EVENT( wxEVT_TOOLBOOK_PAGE_CHANGING, wxBookCtrlEvent );
wxDEFINE_EVENT( wxEVT_TOOLBOOK_PAGE_CHANGED,  wxBookCtrlEvent );

wxBEGIN_EVENT_TABLE(wxToolbook, wxBookCtrlBase)
    EVT_SIZE(wxToolbook::OnSize)
    EVT_TOOL(wxID_ANY, wxToolbook::OnToolSelected)
    EVT_IDLE(wxToolbook::OnIdle)
wxEND_EVENT_TABLE()

// ============================================================================
// wxToolbook implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxToolbook creation
// ----------------------------------------------------------------------------

void wxToolbook::Init()
{
    m_needsRealizing = false;
}

bool wxToolbook::Create(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxString& name)
{
    if ( (style & wxBK_ALIGN_MASK) == wxBK_DEFAULT )
        style |= wxBK_TOP;

    // no border for this control
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
        return false;

    int tbFlags = wxTB_TEXT | wxTB_FLAT | wxBORDER_NONE;
    if ( (style & (wxBK_LEFT | wxBK_RIGHT)) != 0 )
        tbFlags |= wxTB_VERTICAL;
    else
        tbFlags |= wxTB_HORIZONTAL;

    if ( style & wxTBK_HORZ_LAYOUT )
        tbFlags |= wxTB_HORZ_LAYOUT;

    // TODO: make more configurable

#if defined(__WXMAC__) && wxUSE_TOOLBAR && wxUSE_BMPBUTTON
    if (style & wxTBK_BUTTONBAR)
    {
        m_bookctrl = new wxButtonToolBar
                 (
                    this,
                    wxID_ANY,
                    wxDefaultPosition,
                    wxDefaultSize,
                    tbFlags
                 );
    }
    else
#endif
    {
        m_bookctrl = new wxToolBar
                 (
                    this,
                    wxID_ANY,
                    wxDefaultPosition,
                    wxDefaultSize,
                    tbFlags | wxTB_NODIVIDER
                 );
    }

    return true;
}

// ----------------------------------------------------------------------------
// wxToolbook geometry management
// ----------------------------------------------------------------------------

void wxToolbook::OnSize(wxSizeEvent& event)
{
    if (m_needsRealizing)
        Realize();

    wxBookCtrlBase::OnSize(event);
}

// ----------------------------------------------------------------------------
// accessing the pages
// ----------------------------------------------------------------------------

bool wxToolbook::SetPageText(size_t n, const wxString& strText)
{
    int toolId = PageToToolId(n);
    wxToolBarToolBase* tool = GetToolBar()->FindById(toolId);
    if (tool)
    {
        tool->SetLabel(strText);
        return true;
    }
    else
        return false;
}

wxString wxToolbook::GetPageText(size_t n) const
{
    int toolId = PageToToolId(n);
    wxToolBarToolBase* tool = GetToolBar()->FindById(toolId);
    if (tool)
        return tool->GetLabel();
    else
        return wxEmptyString;
}

int wxToolbook::GetPageImage(size_t WXUNUSED(n)) const
{
    wxFAIL_MSG( wxT("wxToolbook::GetPageImage() not implemented") );

    return wxNOT_FOUND;
}

bool wxToolbook::SetPageImage(size_t n, int imageId)
{
    wxBitmapBundle bmp = GetBitmapBundle(imageId);
    if ( !bmp.IsOk() )
        return false;

    int toolId = PageToToolId(n);
    GetToolBar()->SetToolNormalBitmap(toolId, bmp);

    return true;
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

wxBookCtrlEvent* wxToolbook::CreatePageChangingEvent() const
{
    return new wxBookCtrlEvent(wxEVT_TOOLBOOK_PAGE_CHANGING, m_windowId);
}

void wxToolbook::MakeChangedEvent(wxBookCtrlEvent &event)
{
    event.SetEventType(wxEVT_TOOLBOOK_PAGE_CHANGED);
}

void wxToolbook::UpdateSelectedPage(size_t newsel)
{
    int toolId = PageToToolId(newsel);
    GetToolBar()->ToggleTool(toolId, true);
}

// Not part of the wxBookctrl API, but must be called in OnIdle or
// by application to realize the toolbar and select the initial page.
void wxToolbook::Realize()
{
    if (m_needsRealizing)
    {
        m_needsRealizing = false;

        GetToolBar()->Realize();
    }

    if (m_selection == wxNOT_FOUND)
        m_selection = 0;

    if (GetPageCount() > 0)
    {
        int sel = m_selection;
        m_selection = wxNOT_FOUND;
        SetSelection(sel);
    }

    DoSize();
}

int wxToolbook::HitTest(const wxPoint& pt, long *flags) const
{
    int pagePos = wxNOT_FOUND;

    if ( flags )
        *flags = wxBK_HITTEST_NOWHERE;

    // convert from wxToolbook coordinates to wxToolBar ones
    const wxToolBarBase * const tbar = GetToolBar();
    const wxPoint tbarPt = tbar->ScreenToClient(ClientToScreen(pt));

    // is the point over the toolbar?
    if ( wxRect(tbar->GetSize()).Contains(tbarPt) )
    {
        const wxToolBarToolBase * const
            tool = tbar->FindToolForPosition(tbarPt.x, tbarPt.y);

        if ( tool )
        {
            pagePos = tbar->GetToolPos(tool->GetId());
            if ( flags )
                *flags = wxBK_HITTEST_ONICON | wxBK_HITTEST_ONLABEL;
        }
    }
    else // not over the toolbar
    {
        if ( flags && GetPageRect().Contains(pt) )
            *flags |= wxBK_HITTEST_ONPAGE;
    }

    return pagePos;
}

void wxToolbook::OnIdle(wxIdleEvent& event)
{
    if (m_needsRealizing)
        Realize();
    event.Skip();
}

// ----------------------------------------------------------------------------
// adding/removing the pages
// ----------------------------------------------------------------------------

bool wxToolbook::InsertPage(size_t n,
                       wxWindow *page,
                       const wxString& text,
                       bool bSelect,
                       int imageId)
{
    if ( !wxBookCtrlBase::InsertPage(n, page, text, bSelect, imageId) )
        return false;

    m_needsRealizing = true;

    wxBitmapBundle bitmap = GetBitmapBundle(imageId);

    int toolId = page->GetId();
    GetToolBar()->InsertTool(n, toolId, text, bitmap, wxBitmapBundle(), wxITEM_RADIO);

    // fix current selection
    if (m_selection == wxNOT_FOUND)
    {
        DoShowPage(page, true);
        m_selection = n;
    }
    else if ((size_t) m_selection >= n)
    {
        DoShowPage(page, false);
        m_selection++;
    }
    else
    {
        DoShowPage(page, false);
    }

    if ( bSelect )
    {
        SetSelection(n);
    }

    InvalidateBestSize();
    return true;
}

wxWindow *wxToolbook::DoRemovePage(size_t page)
{
    int toolId = PageToToolId(page);
    wxWindow *win = wxBookCtrlBase::DoRemovePage(page);

    if ( win )
    {
        GetToolBar()->DeleteTool(toolId);

        DoSetSelectionAfterRemoval(page);
    }

    return win;
}


bool wxToolbook::DeleteAllPages()
{
    GetToolBar()->ClearTools();
    return wxBookCtrlBase::DeleteAllPages();
}

bool wxToolbook::EnablePage(size_t page, bool enable)
{
    int toolId = PageToToolId(page);
    GetToolBar()->EnableTool(toolId, enable);
    if (!enable && GetSelection() == (int)page)
    {
        AdvanceSelection();
    }
    return true;
}

bool wxToolbook::EnablePage(wxWindow *page, bool enable)
{
    const int pageIndex = FindPage(page);
    if (pageIndex == wxNOT_FOUND)
    {
        return false;
    }
    return EnablePage(pageIndex, enable);
}

int wxToolbook::PageToToolId(size_t page) const
{
    wxCHECK_MSG(page < GetPageCount(), wxID_NONE, "Invalid page number");
    return GetPage(page)->GetId();
}

int wxToolbook::ToolIdToPage(int toolId) const
{
    for (size_t i = 0; i < m_pages.size(); i++)
    {
        if (m_pages[i]->GetId() == toolId)
        {
            return (int) i;
        }
    }
    return wxNOT_FOUND;
}

// ----------------------------------------------------------------------------
// wxToolbook events
// ----------------------------------------------------------------------------

void wxToolbook::OnToolSelected(wxCommandEvent& event)
{
    // find page for the tool
    int page = ToolIdToPage(event.GetId());
    if (page == wxNOT_FOUND)
    {
        // we may have gotten an event from something other than our tool, e.g.
        // a menu item from a context menu shown from the application code, so
        // take care to avoid consuming it in this case
        event.Skip();
        return;
    }

    if (page == m_selection )
    {
        // this event can only come from our own Select(m_selection) below
        // which we call when the page change is vetoed, so we should simply
        // ignore it
        return;
    }

    SetSelection(page);

    // change wasn't allowed, return to previous state
    if (m_selection != page)
    {
        GetToolBar()->ToggleTool(m_selection, false);
    }
}

#endif // wxUSE_TOOLBOOK
