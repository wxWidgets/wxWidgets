///////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_notebook_g.cpp
// Purpose:     wxMoNotebook class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mobile/generic/notebook.h"

#ifndef WX_PRECOMP
    #include "wx/sizer.h"
    #include "wx/string.h"
    #include "wx/dc.h"
    #include "wx/log.h"
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/control.h"
#endif  // WX_PRECOMP

#include "wx/imaglist.h"
#include "wx/sysopt.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(page) ((page) < GetPageCount())

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

#if 0
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING)
#endif

BEGIN_EVENT_TABLE(wxMoNotebook, wxBookCtrlBase)
    EVT_TAB_SEL_CHANGING(wxID_ANY, wxMoNotebook::OnTabChanging)
    EVT_TAB_SEL_CHANGED(wxID_ANY, wxMoNotebook::OnTabChanged)

#if USE_NOTEBOOK_ANTIFLICKER
    EVT_ERASE_BACKGROUND(wxMoNotebook::OnEraseBackground)
    EVT_PAINT(wxMoNotebook::OnPaint)
#endif // USE_NOTEBOOK_ANTIFLICKER
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxMoNotebook, wxBookCtrlBase)

// IMPLEMENT_DYNAMIC_CLASS(wxNotebookEvent, wxNotifyEvent)

// common part of all ctors
void wxMoNotebook::Init()
{
    m_imageList = NULL;
    m_selection = wxNOT_FOUND;
    m_tabCtrl = NULL;
    SetInternalBorder(0);
}

wxMoNotebook::wxMoNotebook()
{
    Init();
}

wxMoNotebook::wxMoNotebook(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    Init();

    Create(parent, id, pos, size, style, name);
}

bool wxMoNotebook::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name)
{
    if ( (style & wxBK_ALIGN_MASK) == wxBK_DEFAULT )
    {
        style |= wxBK_TOP;
    }

    if ((style & (wxBK_TEXT|wxBK_BITMAP)) == 0)
        style |= wxBK_TEXT|wxBK_BITMAP;

    if ( !wxBookCtrlBase::Create(parent, id, pos, size, style, name) )
        return false;

    int tabStyle = 0;
    if (style & wxBK_TEXT)
        tabStyle |= wxTAB_TEXT;
    if (style & wxBK_BITMAP)
        tabStyle |= wxTAB_BITMAP;

    m_tabCtrl = new wxMoTabCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, tabStyle);
    m_bookctrl = m_tabCtrl;

    return true;
}

wxMoNotebook::~wxMoNotebook()
{
}

// ----------------------------------------------------------------------------
// wxMoNotebook accessors
// ----------------------------------------------------------------------------

size_t wxMoNotebook::GetPageCount() const
{
    // consistency check
    wxASSERT( (int)m_pages.Count() == m_tabCtrl->GetItemCount() );

    return m_pages.Count();
}

int wxMoNotebook::GetRowCount() const
{
    return m_tabCtrl->GetRowCount();
}

// get the currently selected page
int wxMoNotebook::GetSelection() const
{
    return m_selection;
}

int wxMoNotebook::SetSelection(size_t page)
{
    return DoSetSelection(page, SetSelection_SendEvent);
}

int wxMoNotebook::ChangeSelection(size_t page)
{
    return DoSetSelection(page);
}

bool wxMoNotebook::SetPageText(size_t page, const wxString& strText)
{
    wxCHECK_MSG( IS_VALID_PAGE(page), false, wxT("notebook page out of range") );

    return m_tabCtrl->SetItemText(page, strText);
}

wxString wxMoNotebook::GetPageText(size_t page) const
{
    wxCHECK_MSG( IS_VALID_PAGE(page), wxEmptyString, wxT("notebook page out of range") );

    return m_tabCtrl->GetItemText(page);
}

int wxMoNotebook::GetPageImage(size_t page) const
{
    wxCHECK_MSG( IS_VALID_PAGE(page), wxNOT_FOUND, wxT("notebook page out of range") );

    return m_tabCtrl->GetItemImage(page);
}

bool wxMoNotebook::SetPageImage(size_t page, int nImage)
{
    wxCHECK_MSG( IS_VALID_PAGE(page), false, wxT("notebook page out of range") );

    return m_tabCtrl->SetItemImage(page, nImage);
}

void wxMoNotebook::SetImageList(wxImageList* imageList)
{
    wxBookCtrlBase::SetImageList(imageList);

    m_tabCtrl->SetImageList(imageList);
}

void wxMoNotebook::SetPadding(const wxSize& WXUNUSED(padding))
{
}

wxSize wxMoNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    // we need to add the size of the choice control and the border between
    const wxSize sizeTabCtrl = GetControllerSize();

    wxSize size = sizePage;
    if ( IsVertical() )
    {
        if ( sizeTabCtrl.x > sizePage.x )
            size.x = sizeTabCtrl.x;
        size.y += sizeTabCtrl.y + GetInternalBorder();
    }
    else // left/right aligned
    {
        size.x += sizeTabCtrl.x + GetInternalBorder();
        if ( sizeTabCtrl.y > sizePage.y )
            size.y = sizeTabCtrl.y;
    }

    return size;
}

void wxMoNotebook::UpdateSelectedPage(size_t newsel)
{
    m_selection = wx_static_cast(int, newsel);
    m_tabCtrl->SetSelection(m_selection);
}

// get the size which the choice control should have
wxSize wxMoNotebook::GetControllerSize() const
{
    const wxSize sizeClient = GetClientSize();
    wxSize sizeChoice;
    if (m_controlSizer)
        sizeChoice = m_controlSizer->CalcMin();
    else
        sizeChoice = m_tabCtrl->GetBestSize();

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


// remove one page from the notebook, without deleting
wxNotebookPage *wxMoNotebook::DoRemovePage(size_t page)
{
    const size_t page_count = GetPageCount();
    wxWindow *win = wxBookCtrlBase::DoRemovePage(page);

    if ( win )
    {
        m_tabCtrl->DeleteItem(page);

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

// remove all pages
bool wxMoNotebook::DeleteAllPages()
{
    m_selection = wxNOT_FOUND;
    m_tabCtrl->DeleteAllItems();

    return wxBookCtrlBase::DeleteAllPages();
}

// same as AddPage() but does it at given position
bool wxMoNotebook::InsertPage(size_t page,
                            wxNotebookPage *pPage,
                            const wxString& strText,
                            bool bSelect,
                            int imageId)
{
    if ( !wxBookCtrlBase::InsertPage(page, pPage, strText, bSelect, imageId) )
        return false;

    m_tabCtrl->InsertItem(page, strText, imageId);

    // if the inserted page is before the selected one, we must update the
    // index of the selected page
    if ( int(page) <= m_selection )
    {
        // one extra page added
        m_selection++;
        m_tabCtrl->SetSelection(m_selection);
    }

    // some page should be selected: either this one or the first one if there
    // is still no selection
    int selNew = wxNOT_FOUND;
    if ( bSelect )
        selNew = page;
    else if ( m_selection == wxNOT_FOUND )
        selNew = 0;

    if ( selNew != m_selection )
        pPage->Hide();

    if ( selNew != wxNOT_FOUND )
        SetSelection(selNew);

    return true;
}

int wxMoNotebook::HitTest(const wxPoint& WXUNUSED(pt), long *WXUNUSED(flags)) const
{
    return -1;
}

void wxMoNotebook::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // do nothing here
}

void wxMoNotebook::OnTabChanging(wxTabEvent& event)
{
    if (event.GetEventObject() != m_tabCtrl)
    {
        event.Skip();
        return;
    }

    wxNotebookEvent nbEvent(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, m_windowId);

    nbEvent.SetSelection(event.GetSelection());
    nbEvent.SetOldSelection(event.GetSelection());
    nbEvent.SetEventObject(this);

    bool allowed = !GetEventHandler()->ProcessEvent(nbEvent) || event.IsAllowed();
    if (allowed)
        event.Allow();
    else
        event.Veto();
}

void wxMoNotebook::OnTabChanged(wxTabEvent& event)
{
    if (event.GetEventObject() != m_tabCtrl)
    {
        event.Skip();
        return;
    }

    const int selNew = event.GetSelection();

    if ( selNew == m_selection )
    {
        // this event can only come from our own Select(m_selection) below
        // which we call when the page change is vetoed, so we should simply
        // ignore it
        return;
    }

    ChangeSelection(selNew);

    wxNotebookEvent nbEvent(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, m_windowId);

    nbEvent.SetSelection(event.GetSelection());
    nbEvent.SetOldSelection(event.GetSelection());
    nbEvent.SetEventObject(this);

    GetEventHandler()->ProcessEvent(nbEvent);
}

wxBookCtrlBaseEvent* wxMoNotebook::CreatePageChangingEvent() const
{
    return new wxNotebookEvent(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, m_windowId);
}

void wxMoNotebook::MakeChangedEvent(wxBookCtrlBaseEvent &event)
{
    event.SetEventType(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED);
}

// Set a text badge for the given item
bool wxMoNotebook::SetBadge(int item, const wxString& badge)
{
    return m_tabCtrl->SetBadge(item, badge);
}

// Get the text badge for the given item
wxString wxMoNotebook::GetBadge(int item) const
{
    return m_tabCtrl->GetBadge(item);
}


