/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/geneic/mo_panel_n.mm
// Purpose:     wxMoPanel class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mobile/native/panel.h"
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
    // FIXME stub
}

bool wxMoPanel::Create(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
    // FIXME stub

    return true;
}

wxMoPanel::~wxMoPanel()
{
}

void wxMoPanel::InitDialog()
{
    // FIXME stub
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void wxMoPanel::OnSize(wxSizeEvent& event)
{
    // FIXME stub
}
