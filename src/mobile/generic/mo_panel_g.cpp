/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/geneic/mo_panel_g.cpp
// Purpose:     wxMoPanel class
// Author:      Julian Smart
// Modified by:
// Created:     2009-06-07
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mobile/generic/panel.h"
#include "wx/mobile/settings.h"

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/font.h"
    #include "wx/colour.h"
    #include "wx/settings.h"
    #include "wx/log.h"
    #include "wx/panel.h"
    #include "wx/containr.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxMoPanel, wxMoWindow)

BEGIN_EVENT_TABLE(wxMoPanel, wxMoWindow)
    EVT_SIZE(wxMoPanel::OnSize)

    WX_EVENT_TABLE_CONTROL_CONTAINER(wxMoPanel)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

WX_DELEGATE_TO_CONTROL_CONTAINER(wxMoPanel, wxMoWindow)

// ----------------------------------------------------------------------------
// wxMoPanel creation
// ----------------------------------------------------------------------------

void wxMoPanel::Init()
{
    m_container.SetContainerWindow(this);
}

bool wxMoPanel::Create(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
    if ( !wxMoWindow::Create(parent, id, pos, size, style, name) )
        return false;

    // so that non-solid background renders correctly under GTK+:
    SetThemeEnabled(true);

    // Force a solid background colour
    SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_PANEL_BG));

    return true;
}

wxMoPanel::~wxMoPanel()
{
}

void wxMoPanel::InitDialog()
{
    wxInitDialogEvent event(GetId());
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void wxMoPanel::OnSize(wxSizeEvent& event)
{
    if (GetAutoLayout())
        Layout();

    event.Skip();
}
