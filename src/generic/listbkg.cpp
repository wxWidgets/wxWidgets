///////////////////////////////////////////////////////////////////////////////
// Name:        generic/listbkg.cpp
// Purpose:     generic implementation of wxListbook
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.08.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "listbook.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_LISTBOOK

#include "wx/listctrl.h"
#include "wx/statline.h"
#include "wx/listbook.h"
#include "wx/imaglist.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// margin between the list and the page, should be bigger than wxStaticLine
// size
const wxCoord MARGIN = 5;

// ----------------------------------------------------------------------------
// various wxWindows macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListbook, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxListbookEvent, wxNotifyEvent)

const wxEventType wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING = wxNewEventType();
const wxEventType wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED = wxNewEventType();

BEGIN_EVENT_TABLE(wxListbook, wxBookCtrl)
    EVT_SIZE(wxListbook::OnSize)

    EVT_LIST_ITEM_SELECTED(wxID_ANY, wxListbook::OnListSelected)
END_EVENT_TABLE()

// ============================================================================
// wxListbook implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxListbook creation
// ----------------------------------------------------------------------------

void wxListbook::Init()
{
    m_list = NULL;
    m_line = NULL;
    m_selection = wxNOT_FOUND;
}

bool
wxListbook::Create(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxString& name)
{
    if ( (style & wxLB_ALIGN_MASK) == wxLB_DEFAULT )
    {
#ifdef __WXMAC__
        style |= wxLB_TOP;
#else // !__WXMAC__
        style |= wxLB_LEFT;
#endif // __WXMAC__/!__WXMAC__
    }

    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
        return false;

    m_list = new wxListView
                 (
                    this,
                    -1,
                    wxDefaultPosition,
                    wxDefaultSize,
                    wxLC_ICON | wxLC_SINGLE_SEL
                 );

    m_line = new wxStaticLine
                 (
                    this,
                    -1,
                    wxDefaultPosition,
                    wxDefaultSize,
                    IsVertical() ? wxLI_HORIZONTAL : wxLI_VERTICAL
                 );

    return true;
}

// ----------------------------------------------------------------------------
// wxListbook geometry management
// ----------------------------------------------------------------------------

wxSize wxListbook::GetListSize() const
{
    const wxSize sizeClient = GetClientSize();

    // we need to find the longest/tallest label
    wxCoord widthMax = 0,
            heightMax = 0;
    const int count = m_list->GetItemCount();
    if ( count )
    {
        for ( int i = 0; i < count; i++ )
        {
            wxRect r;
            m_list->GetItemRect(i, r);

            wxCoord w = r.x + r.width,
                    h = r.y + r.height;

            if ( w > widthMax )
                widthMax = w;
            if ( h > heightMax )
                heightMax = h;
        }
    }

    wxSize size;
    if ( IsVertical() )
    {
        size.x = sizeClient.x;
        size.y = heightMax;
    }
    else // left/right aligned
    {
        size.x = widthMax + 10;
        size.y = sizeClient.y;
    }

    return size;
}

wxRect wxListbook::GetPageRect() const
{
    const wxSize sizeList = GetListSize();

    wxRect rectPage(wxPoint(0, 0), GetClientSize());
    switch ( GetWindowStyle() & wxLB_ALIGN_MASK )
    {
        default:
            wxFAIL_MSG( _T("unexpected wxListbook alignment") );
            // fall through

        case wxLB_TOP:
            rectPage.y = sizeList.y + MARGIN;
            // fall through

        case wxLB_BOTTOM:
            rectPage.height -= sizeList.y + MARGIN;
            break;

        case wxLB_LEFT:
            rectPage.x = sizeList.x + MARGIN;
            // fall through

        case wxLB_RIGHT:
            rectPage.width -= sizeList.x + MARGIN;
            break;
    }

    return rectPage;
}

void wxListbook::OnSize(wxSizeEvent& event)
{
    event.Skip();

    if ( !m_list )
    {
        // we're not fully created yet
        return;
    }

    // resize the list control and the page area to fit inside our new size
    const wxSize sizeClient = GetClientSize(),
                 sizeList = GetListSize();

    wxPoint posList;
    switch ( GetWindowStyle() & wxLB_ALIGN_MASK )
    {
        default:
            wxFAIL_MSG( _T("unexpected wxListbook alignment") );
            // fall through

        case wxLB_TOP:
        case wxLB_LEFT:
            // posList is already ok
            break;

        case wxLB_BOTTOM:
            posList.y = sizeClient.y - sizeList.y;
            break;

        case wxLB_RIGHT:
            posList.x = sizeClient.x - sizeList.x;
            break;
    }

    m_list->SetSize(posList.x, posList.y, sizeList.x, sizeList.y);

    if ( m_line )
    {
        wxRect rectLine(wxPoint(0, 0), sizeClient);

        switch ( GetWindowStyle() & wxLB_ALIGN_MASK )
        {
            case wxLB_TOP:
                rectLine.y = sizeList.y + 1;
                rectLine.height = MARGIN - 2;
                break;

            case wxLB_BOTTOM:
                rectLine.height = MARGIN - 2;
                rectLine.y = sizeClient.y - sizeList.y - rectLine.height;
                break;

            case wxLB_LEFT:
                rectLine.x = sizeList.x + 1;
                rectLine.width = MARGIN - 2;
                break;

            case wxLB_RIGHT:
                rectLine.width = MARGIN - 2;
                rectLine.x = sizeClient.x - sizeList.x - rectLine.width;
                break;
        }

        m_line->SetSize(rectLine);
    }

    // we should always have some selection if possible
    if ( m_selection == wxNOT_FOUND && GetPageCount() )
    {
        SetSelection(0);
    }

    if ( m_selection != wxNOT_FOUND )
    {
        wxWindow *page = m_pages[m_selection];
        wxCHECK_RET( page, _T("NULL page in wxListbook?") );

        page->SetSize(GetPageRect());
        if ( !page->IsShown() )
        {
            page->Show();
        }
    }
}

wxSize wxListbook::CalcSizeFromPage(const wxSize& sizePage) const
{
    // we need to add the size of the list control and the margin
    const wxSize sizeList = GetListSize();

    wxSize size = sizePage;
    if ( IsVertical() )
    {
        size.y += sizeList.y + MARGIN;
    }
    else // left/right aligned
    {
        size.x += sizeList.x + MARGIN;
    }

    return size;
}


// ----------------------------------------------------------------------------
// accessing the pages
// ----------------------------------------------------------------------------

bool wxListbook::SetPageText(size_t n, const wxString& strText)
{
    m_list->SetItemText(n, strText);

    return true;
}

wxString wxListbook::GetPageText(size_t n) const
{
    return m_list->GetItemText(n);
}

int wxListbook::GetPageImage(size_t WXUNUSED(n)) const
{
    wxFAIL_MSG( _T("wxListbook::GetPageImage() not implemented") );

    return -1;
}

bool wxListbook::SetPageImage(size_t n, int imageId)
{
    return m_list->SetItemImage(n, imageId, imageId);
}

// ----------------------------------------------------------------------------
// image list stuff
// ----------------------------------------------------------------------------

void wxListbook::SetImageList(wxImageList *imageList)
{
    m_list->SetImageList(imageList, wxIMAGE_LIST_NORMAL);

    wxBookCtrl::SetImageList(imageList);
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

int wxListbook::GetSelection() const
{
    return m_selection;
}

int wxListbook::SetSelection(size_t n)
{
    wxCHECK_MSG( n < GetPageCount(), wxNOT_FOUND,
                 _T("invalid page index in wxListbook::SetSelection()") );

    int selOld = m_selection;

    if ( (int)n != m_selection )
    {
        m_selection = n;

        m_list->Select(m_selection);
        m_list->Focus(m_selection);
    }

    return selOld;
}


// ----------------------------------------------------------------------------
// adding/removing the pages
// ----------------------------------------------------------------------------

bool
wxListbook::InsertPage(size_t n,
                       wxWindow *page,
                       const wxString& text,
                       bool bSelect,
                       int imageId)
{
    if ( !wxBookCtrl::InsertPage(n, page, text, bSelect, imageId) )
        return false;

    m_list->InsertItem(n, text, imageId);

    if ( bSelect )
    {
        m_list->Select(n);
        m_list->Focus(n);
    }
    else // don't select this page
    {
        // it will be shown only when selected
        page->Hide();
    }

    return true;
}

wxWindow *wxListbook::DoRemovePage(size_t page)
{
    wxWindow *win = wxBookCtrl::DoRemovePage(page);
    if ( win )
    {
        m_list->DeleteItem(page);
    }

    return win;
}

// ----------------------------------------------------------------------------
// wxListbook events
// ----------------------------------------------------------------------------

void wxListbook::OnListSelected(wxListEvent& eventList)
{
    const int selNew = eventList.GetIndex();

    if ( selNew == m_selection )
    {
        // this event can only come from our own Select(m_selection) below
        // which we call when the page change is vetoed, so we should simply
        // ignore it
        return;
    }

    // first send "change in progress" event which may be vetoed by user
    wxListbookEvent eventIng(wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING, GetId());

    eventIng.SetEventObject(this);
    eventIng.SetSelection(selNew);
    eventIng.SetOldSelection(m_selection);
    if ( GetEventHandler()->ProcessEvent(eventIng) && !eventIng.IsAllowed() )
    {
        m_list->Select(m_selection);
        return;
    }

    // change allowed: do change the page and notify the user about it
    if ( m_selection != wxNOT_FOUND )
        m_pages[m_selection]->Hide();
    wxWindow *page = m_pages[m_selection = selNew];
    page->SetSize(GetPageRect());
    page->Show();

    wxListbookEvent eventEd(wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED, GetId());

    eventEd.SetEventObject(this);
    eventEd.SetSelection(selNew);
    eventEd.SetOldSelection(m_selection);

    (void)GetEventHandler()->ProcessEvent(eventEd);
}

#endif // wxUSE_LISTBOOK

