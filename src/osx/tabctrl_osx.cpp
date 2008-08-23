/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/tabctrl_osx.cpp
// Purpose:     wxTabCtrl
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: tabctrl.cpp 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_TAB_DIALOG

#include "wx/tabctrl.h"

#ifndef WX_PRECOMP
    #include "wx/control.h"
#endif

#include "wx/osx/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxTabCtrl, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxTabEvent, wxNotifyEvent)

// this control has been superseeded by the native wxNotebook implementation, but
// we leave the skeleton here, just in case it might become useful ...

DEFINE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGING)


BEGIN_EVENT_TABLE(wxTabCtrl, wxControl)
END_EVENT_TABLE()


wxTabCtrl::wxTabCtrl()
{
    m_macIsUserPane = false;
    m_imageList = NULL;
}

bool wxTabCtrl::Create( wxWindow *parent,
    wxWindowID id, const wxPoint& pos, const wxSize& size,
    long style, const wxString& name )
{
    m_macIsUserPane = false;
    m_imageList = NULL;

    if ( !wxControl::Create( parent, id, pos, size, style, wxDefaultValidator, name ) )
        return false;

    m_peer = wxWidgetImpl::CreateTabView( this, parent, id, pos, size, style, GetExtraStyle );

    MacPostControlCreate( pos, size );

    return true;
}

wxTabCtrl::~wxTabCtrl()
{
}

void wxTabCtrl::Command(wxCommandEvent& event)
{
}

bool wxTabCtrl::DeleteAllItems()
{
    // TODO:
    return false;
}

bool wxTabCtrl::DeleteItem(int item)
{
    // TODO:
    return false;
}

int wxTabCtrl::GetSelection() const
{
    // TODO:
    return 0;
}

// Get the tab with the current keyboard focus
//
int wxTabCtrl::GetCurFocus() const
{
    // TODO:
    return 0;
}

wxImageList * wxTabCtrl::GetImageList() const
{
    return m_imageList;
}

int wxTabCtrl::GetItemCount() const
{
    // TODO:
    return 0;
}

// Get the rect corresponding to the tab
bool wxTabCtrl::GetItemRect(int item, wxRect& wxrect) const
{
    // TODO:
    return false;
}

int wxTabCtrl::GetRowCount() const
{
    // TODO:
    return 0;
}

wxString wxTabCtrl::GetItemText(int item) const
{
    // TODO:
    return wxEmptyString;
}

int wxTabCtrl::GetItemImage(int item) const
{
    // TODO:
    return 0;
}

void* wxTabCtrl::GetItemData(int item) const
{
    // TODO:
    return NULL;
}

int wxTabCtrl::HitTest(const wxPoint& pt, long& flags)
{
    // TODO:
    return 0;
}

bool wxTabCtrl::InsertItem(int item, const wxString& text, int imageId, void* data)
{
    // TODO:
    return false;
}

int wxTabCtrl::SetSelection(int item)
{
    // TODO:
    return 0;
}

void wxTabCtrl::SetImageList(wxImageList* imageList)
{
    // TODO:
}

bool wxTabCtrl::SetItemText(int item, const wxString& text)
{
    // TODO:
    return false;
}

bool wxTabCtrl::SetItemImage(int item, int image)
{
    // TODO:
    return false;
}

bool wxTabCtrl::SetItemData(int item, void* data)
{
    // TODO:
    return false;
}

// Set the size for a fixed-width tab control
void wxTabCtrl::SetItemSize(const wxSize& size)
{
    // TODO:
}

// Set the padding between tabs
void wxTabCtrl::SetPadding(const wxSize& padding)
{
    // TODO:
}

#endif // wxUSE_TAB_DIALOG
