/////////////////////////////////////////////////////////////////////////////
// Name:        src/moile/generic/mo_viewcontroller_g.cpp
// Purpose:     wxMoViewController class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mobile/generic/viewcontroller.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoViewController, wxEvtHandler)

/*
    wxMoViewController
 */

wxMoViewController::wxMoViewController(const wxString& title, bool autoDelete)
{
    Init();
    SetTitle(title);
    
    m_autoDelete = autoDelete;
}

wxMoViewController::~wxMoViewController()
{
    delete m_item;
    if (m_ownsWindow && m_window)
    {
        delete m_window;
        m_window = NULL;
    }
}

void wxMoViewController::Init()
{
    m_item = new wxMoNavigationItem;
    m_item->SetViewController(this);
    m_autoDelete = true;
    m_ownsWindow = true;

    m_window = NULL;
}

// Sets the view title
void wxMoViewController::SetTitle(const wxString& title)
{
    if (m_item)
        m_item->SetTitle(title);
}

// Gets the view title
wxString wxMoViewController::GetTitle() const
{
    if (m_item)
        return m_item->GetTitle();
    else
        return wxEmptyString;
}
