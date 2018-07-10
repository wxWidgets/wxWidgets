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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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
    EVT_TOOL_RANGE(1, 50, wxToolbook::OnToolSelected)
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
    // Assume tool ids start from 1
    wxToolBarToolBase* tool = GetToolBar()->FindById(n + 1);
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
    wxToolBarToolBase* tool = GetToolBar()->FindById(n + 1);
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
    wxASSERT( GetImageList() != NULL );
    if (!GetImageList())
        return false;

    GetToolBar()->SetToolNormalBitmap(n + 1, GetImageList()->GetBitmap(imageId));

    return true;
}

// ----------------------------------------------------------------------------
// image list stuff
// ----------------------------------------------------------------------------

void wxToolbook::SetImageList(wxImageList *imageList)
{
    wxBookCtrlBase::SetImageList(imageList);
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
    GetToolBar()->ToggleTool(newsel + 1, true);
}

// Not part of the wxBookctrl API, but must be called in OnIdle or
// by application to realize the toolbar and select the initial page.
void wxToolbook::Realize()
{
    if (m_needsRealizing)
    {
        m_needsRealizing = false;

        GetToolBar()->SetToolBitmapSize(m_maxBitmapSize);

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

    wxASSERT(GetImageList() != NULL);

    if (!GetImageList())
        return false;

    // TODO: make sure all platforms can convert between icon and bitmap,
    // and/or test whether the image is a bitmap or an icon.
#ifdef __WXMAC__
    wxBitmap bitmap = GetImageList()->GetBitmap(imageId);
#else
    // On Windows, we can lose information by using GetBitmap, so extract icon instead
    wxIcon icon = GetImageList()->GetIcon(imageId);
    wxBitmap bitmap;
    bitmap.CopyFromIcon(icon);
#endif

    m_maxBitmapSize.x = wxMax(bitmap.GetWidth(), m_maxBitmapSize.x);
    m_maxBitmapSize.y = wxMax(bitmap.GetHeight(), m_maxBitmapSize.y);

    GetToolBar()->SetToolBitmapSize(m_maxBitmapSize);
    GetToolBar()->AddRadioTool(n + 1, text, bitmap, wxNullBitmap, text);

    if (bSelect)
    {
        GetToolBar()->ToggleTool(n, true);
        m_selection = n;
    }
    else
        page->Hide();

    InvalidateBestSize();
    return true;
}

wxWindow *wxToolbook::DoRemovePage(size_t page)
{
    wxWindow *win = wxBookCtrlBase::DoRemovePage(page);

    if ( win )
    {
        GetToolBar()->DeleteTool(page + 1);

        DoSetSelectionAfterRemoval(page);
    }

    return win;
}


bool wxToolbook::DeleteAllPages()
{
    GetToolBar()->ClearTools();
    return wxBookCtrlBase::DeleteAllPages();
}

// ----------------------------------------------------------------------------
// wxToolbook events
// ----------------------------------------------------------------------------

void wxToolbook::OnToolSelected(wxCommandEvent& event)
{
    const int selNew = event.GetId() - 1;

    if ( selNew == m_selection )
    {
        // this event can only come from our own Select(m_selection) below
        // which we call when the page change is vetoed, so we should simply
        // ignore it
        return;
    }

    SetSelection(selNew);

    // change wasn't allowed, return to previous state
    if (m_selection != selNew)
    {
        GetToolBar()->ToggleTool(m_selection, false);
    }
}

#endif // wxUSE_TOOLBOOK
