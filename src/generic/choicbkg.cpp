///////////////////////////////////////////////////////////////////////////////
// Name:        generic/choicbkg.cpp
// Purpose:     generic implementation of wxChoicebook
// Author:      Vadim Zeitlin
// Modified by: Wlodzimierz ABX Skiba from generic/listbkg.cpp
// Created:     15.09.04
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin, Wlodzimierz Skiba
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "choicebook.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CHOICEBOOK

#include "wx/choice.h"
#include "wx/choicebk.h"
#include "wx/imaglist.h"
#include "wx/settings.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// margin between the choice and the page
#if defined(__WXWINCE__)
const wxCoord MARGIN = 1;
#else
const wxCoord MARGIN = 5;
#endif

// ----------------------------------------------------------------------------
// various wxWidgets macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) ((nPage) < GetPageCount())

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxChoicebook, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxChoicebookEvent, wxNotifyEvent)

const wxEventType wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING = wxNewEventType();
const wxEventType wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED = wxNewEventType();
const int wxID_CHOICEBOOKCHOICE = wxNewId();

BEGIN_EVENT_TABLE(wxChoicebook, wxBookCtrl)
    EVT_SIZE(wxChoicebook::OnSize)
    EVT_CHOICE(wxID_CHOICEBOOKCHOICE, wxChoicebook::OnChoiceSelected)
END_EVENT_TABLE()

// ============================================================================
// wxChoicebook implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxChoicebook creation
// ----------------------------------------------------------------------------

void wxChoicebook::Init()
{
    m_choice = NULL;
    m_selection = wxNOT_FOUND;
}

bool
wxChoicebook::Create(wxWindow *parent,
                     wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
    if ( (style & wxCHB_ALIGN_MASK) == wxCHB_DEFAULT )
    {
        style |= wxCHB_TOP;
    }

    // no border for this control, it doesn't look nice together with
    // wxChoice border
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
        return false;

    m_choice = new wxChoice
                 (
                    this,
                    wxID_CHOICEBOOKCHOICE,
                    wxDefaultPosition,
                    wxDefaultSize
                 );

    return true;
}

// ----------------------------------------------------------------------------
// wxChoicebook geometry management
// ----------------------------------------------------------------------------

wxSize wxChoicebook::GetChoiceSize() const
{
    const wxSize sizeClient = GetClientSize(),
                 sizeChoice = m_choice->GetBestSize();

    wxSize size;
    if ( IsVertical() )
    {
        size.x = sizeClient.x;
        size.y = sizeChoice.y;
    }
    else // left/right aligned
    {
        size.x = sizeChoice.x;
        size.y = sizeClient.y;
    }

    return size;
}

wxRect wxChoicebook::GetPageRect() const
{
    const wxSize sizeChoice = m_choice->GetSize();

    wxRect rectPage(wxPoint(0, 0), GetClientSize());
    switch ( GetWindowStyle() & wxCHB_ALIGN_MASK )
    {
        default:
            wxFAIL_MSG( _T("unexpected wxChoicebook alignment") );
            // fall through

        case wxCHB_TOP:
            rectPage.y = sizeChoice.y + MARGIN;
            // fall through

        case wxCHB_BOTTOM:
            rectPage.height -= sizeChoice.y + MARGIN;
            break;

        case wxCHB_LEFT:
            rectPage.x = sizeChoice.x + MARGIN;
            // fall through

        case wxCHB_RIGHT:
            rectPage.width -= sizeChoice.x + MARGIN;
            break;
    }

    return rectPage;
}

void wxChoicebook::OnSize(wxSizeEvent& event)
{
    event.Skip();

    if ( !m_choice )
    {
        // we're not fully created yet
        return;
    }

    // resize the choice control and the page area to fit inside our new size
    const wxSize sizeClient = GetClientSize(),
                 sizeChoice = GetChoiceSize();

    wxPoint posChoice;
    switch ( GetWindowStyle() & wxCHB_ALIGN_MASK )
    {
        default:
            wxFAIL_MSG( _T("unexpected wxChoicebook alignment") );
            // fall through

        case wxCHB_TOP:
        case wxCHB_LEFT:
            // posChoice is already ok
            break;

        case wxCHB_BOTTOM:
            posChoice.y = sizeClient.y - sizeChoice.y;
            break;

        case wxCHB_RIGHT:
            posChoice.x = sizeClient.x - sizeChoice.x;
            break;
    }

    m_choice->Move(posChoice.x, posChoice.y);
    m_choice->SetSize(sizeChoice.x, sizeChoice.y);

    // resize the currently shown page
    if ( m_selection != wxNOT_FOUND )
    {
        wxWindow *page = m_pages[m_selection];
        wxCHECK_RET( page, _T("NULL page in wxChoicebook?") );
        page->SetSize(GetPageRect());
    }
}

wxSize wxChoicebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    // we need to add the size of the choice control and the margin
    const wxSize sizeChoice = GetChoiceSize();

    wxSize size = sizePage;
    if ( IsVertical() )
    {
        size.y += sizeChoice.y + MARGIN;
    }
    else // left/right aligned
    {
        size.x += sizeChoice.x + MARGIN;
    }

    return size;
}


// ----------------------------------------------------------------------------
// accessing the pages
// ----------------------------------------------------------------------------

bool wxChoicebook::SetPageText(size_t n, const wxString& strText)
{
    m_choice->SetString(n, strText);

    return true;
}

wxString wxChoicebook::GetPageText(size_t n) const
{
    return m_choice->GetString(n);
}

int wxChoicebook::GetPageImage(size_t WXUNUSED(n)) const
{
    wxFAIL_MSG( _T("wxChoicebook::GetPageImage() not implemented") );

    return -1;
}

bool wxChoicebook::SetPageImage(size_t WXUNUSED(n), int WXUNUSED(imageId))
{
    wxFAIL_MSG( _T("wxChoicebook::SetPageImage() not implemented") );

    return false;
}

// ----------------------------------------------------------------------------
// image list stuff
// ----------------------------------------------------------------------------

void wxChoicebook::SetImageList(wxImageList *imageList)
{
    // TODO: can be implemented in form of static bitmap near choice control

    wxBookCtrl::SetImageList(imageList);
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

int wxChoicebook::GetSelection() const
{
    return m_selection;
}

int wxChoicebook::SetSelection(size_t n)
{
    wxCHECK_MSG( IS_VALID_PAGE(n), wxNOT_FOUND,
                 wxT("invalid page index in wxChoicebook::SetSelection()") );

    const int oldSel = m_selection;

    if ( int(n) != m_selection )
    {
        wxChoicebookEvent event(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING, m_windowId);
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
            m_choice->Select(n);

            // program allows the page change
            event.SetEventType(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED);
            (void)GetEventHandler()->ProcessEvent(event);
        }
    }

    return oldSel;
}

// ----------------------------------------------------------------------------
// adding/removing the pages
// ----------------------------------------------------------------------------

bool
wxChoicebook::InsertPage(size_t n,
                         wxWindow *page,
                         const wxString& text,
                         bool bSelect,
                         int imageId)
{
    if ( !wxBookCtrl::InsertPage(n, page, text, bSelect, imageId) )
        return false;

    m_choice->Insert(text, n);

    // if the inserted page is before the selected one, we must update the
    // index of the selected page
    if ( int(n) <= m_selection )
    {
        // one extra page added
        m_selection++;
        m_choice->Select(m_selection);
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

wxWindow *wxChoicebook::DoRemovePage(size_t page)
{
    const int page_count = GetPageCount();
    wxWindow *win = wxBookCtrl::DoRemovePage(page);

    if ( win )
    {
        m_choice->Delete(page);

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


bool wxChoicebook::DeleteAllPages()
{
    m_choice->Clear();
    return wxBookCtrl::DeleteAllPages();
}

// ----------------------------------------------------------------------------
// wxChoicebook events
// ----------------------------------------------------------------------------

void wxChoicebook::OnChoiceSelected(wxCommandEvent& eventChoice)
{
    const int selNew = eventChoice.GetSelection();

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
        m_choice->Select(m_selection);
}

#endif // wxUSE_CHOICEBOOK
