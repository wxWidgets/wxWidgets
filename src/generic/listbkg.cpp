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
#include "wx/settings.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// margin between the list and the page, should be bigger than wxStaticLine
// size
const wxCoord MARGIN = 5;

// ----------------------------------------------------------------------------
// various wxWidgets macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) ((nPage) < GetPageCount())

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListbook, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxListbookEvent, wxNotifyEvent)

const wxEventType wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING = wxNewEventType();
const wxEventType wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED = wxNewEventType();
const int wxID_LISTBOOKLISTVIEW = wxNewId();

BEGIN_EVENT_TABLE(wxListbook, wxBookCtrlBase)
    EVT_SIZE(wxListbook::OnSize)
    EVT_LIST_ITEM_SELECTED(wxID_LISTBOOKLISTVIEW, wxListbook::OnListSelected)
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
#if wxUSE_LINE_IN_LISTBOOK
    m_line = NULL;
#endif // wxUSE_LINE_IN_LISTBOOK
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

    // no border for this control, it doesn't look nice together with
    // wxListCtrl border
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
        return false;

    m_list = new wxListView
                 (
                    this,
                    wxID_LISTBOOKLISTVIEW,
                    wxDefaultPosition,
                    wxDefaultSize,
                    wxLC_ICON | wxLC_SINGLE_SEL |
                        (IsVertical() ? wxLC_ALIGN_LEFT : wxLC_ALIGN_TOP)
                 );

#if wxUSE_LINE_IN_LISTBOOK
    m_line = new wxStaticLine
                 (
                    this,
                    wxID_ANY,
                    wxDefaultPosition,
                    wxDefaultSize,
                    IsVertical() ? wxLI_HORIZONTAL : wxLI_VERTICAL
                 );
#endif // wxUSE_LINE_IN_LISTBOOK

#ifdef __WXMSW__
    // On XP with themes enabled the GetViewRect used in GetListSize to
    // determine the space needed for the list view will incorrectly return
    // (0,0,0,0) the first time.  So send a pending event so OnSize will be
    // called again after the window is ready to go.  Technically we don't
    // need to do this on non-XP windows, but if things are already sized
    // correctly then nothing changes and so there is no harm.
    wxSizeEvent evt;
    GetEventHandler()->AddPendingEvent(evt);
#endif
    return true;
}

// ----------------------------------------------------------------------------
// wxListbook geometry management
// ----------------------------------------------------------------------------

wxSize wxListbook::GetListSize() const
{
    const wxSize sizeClient = GetClientSize(),
                 sizeList = m_list->GetViewRect().GetSize();

    wxSize size;
    if ( IsVertical() )
    {
        size.x = sizeClient.x;
        size.y = sizeList.y;
    }
    else // left/right aligned
    {
        size.x = sizeList.x;
        size.y = sizeClient.y;
    }

    return size;
}

wxRect wxListbook::GetPageRect() const
{
    const wxSize sizeList = m_list->GetSize();

    wxPoint pt;
    wxRect rectPage(pt, GetClientSize());
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

    // arrange the icons before calling SetClientSize(), otherwise it wouldn't
    // account for the scrollbars the list control might need and, at least
    // under MSW, we'd finish with an ugly looking list control with both
    // vertical and horizontal scrollbar (with one of them being added because
    // the other one is not accounted for in client size computations)
    m_list->Arrange();
    if ( m_list->GetPosition() != posList )
        m_list->Move(posList.x, posList.y);
    m_list->SetClientSize(sizeList.x, sizeList.y);

#if wxUSE_LINE_IN_LISTBOOK
    if ( m_line )
    {
        wxRect rectLine(sizeClient);

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
#endif // wxUSE_LINE_IN_LISTBOOK

    // resize the currently shown page
    if (m_selection != wxNOT_FOUND )
    {
        wxWindow *page = m_pages[m_selection];
        wxCHECK_RET( page, _T("NULL page in wxListbook?") );
        page->SetSize(GetPageRect());
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
    return m_list->SetItemImage(n, imageId);
}

// ----------------------------------------------------------------------------
// image list stuff
// ----------------------------------------------------------------------------

void wxListbook::SetImageList(wxImageList *imageList)
{
    m_list->SetImageList(imageList, wxIMAGE_LIST_NORMAL);

    wxBookCtrlBase::SetImageList(imageList);
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
    wxCHECK_MSG( IS_VALID_PAGE(n), wxNOT_FOUND,
                 wxT("invalid page index in wxListbook::SetSelection()") );

    const int oldSel = m_selection;

    if ( int(n) != m_selection )
    {
        wxListbookEvent event(wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING, m_windowId);
        event.SetSelection(n);
        event.SetOldSelection(m_selection);
        event.SetEventObject(this);
        if ( !GetEventHandler()->ProcessEvent(event) || event.IsAllowed() )
        {
            if ( m_selection != wxNOT_FOUND )
                m_pages[m_selection]->Hide();

            wxWindow *page = m_pages[n];
            page->SetSize(GetPageRect());
            page->Show();

            // change m_selection now to ignore the selection change event
            m_selection = n;
            m_list->Select(n);
            m_list->Focus(n);

            // program allows the page change
            event.SetEventType(wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED);
            (void)GetEventHandler()->ProcessEvent(event);
        }
    }

    return oldSel;
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
    if ( !wxBookCtrlBase::InsertPage(n, page, text, bSelect, imageId) )
        return false;

    m_list->InsertItem(n, text, imageId);

    // if the inserted page is before the selected one, we must update the
    // index of the selected page
    if ( int(n) <= m_selection )
    {
        // one extra page added
        m_selection++;
        m_list->Select(m_selection);
        m_list->Focus(m_selection);
    }

    // some page should be selected: either this one or the first one if there
    // is still no selection
    int selNew = -1;
    if ( bSelect )
        selNew = n;
    else if ( m_selection == -1 )
        selNew = 0;

    if ( selNew != m_selection )
        page->Hide();

    if ( selNew != -1 )
        SetSelection(selNew);

    InvalidateBestSize();
    return true;
}

wxWindow *wxListbook::DoRemovePage(size_t page)
{
    const int page_count = GetPageCount();
    wxWindow *win = wxBookCtrlBase::DoRemovePage(page);

    if ( win )
    {
        m_list->DeleteItem(page);

        if (m_selection >= (int)page)
        {
            // force new sel valid if possible
            int sel = m_selection - 1;
            if (page_count == 1)
                sel = wxNOT_FOUND;
            else if ((page_count == 2) || (sel == -1))
                sel = 0;

            // force sel invalid if deleting current page - don't try to hide it
            m_selection = (m_selection == (int)page) ? wxNOT_FOUND : m_selection - 1;

            if ((sel != wxNOT_FOUND) && (sel != m_selection))
                SetSelection(sel);
        }
    }

    return win;
}


bool wxListbook::DeleteAllPages()
{
    m_list->DeleteAllItems();
    return wxBookCtrlBase::DeleteAllPages();
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

    SetSelection(selNew);

    // change wasn't allowed, return to previous state
    if (m_selection != selNew)
    {
        m_list->Select(m_selection);
        m_list->Focus(m_selection);
    }
}

#endif // wxUSE_LISTBOOK

