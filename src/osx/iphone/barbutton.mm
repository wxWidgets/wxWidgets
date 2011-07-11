/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/barbutton.mm
// Purpose:     wxBarButton class
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

#include "wx/barbutton.h"

#ifndef WX_PRECOMP
#include "wx/app.h"
#include "wx/brush.h"
#include "wx/panel.h"
#include "wx/bmpbuttn.h"
#include "wx/settings.h"
#include "wx/dcscreen.h"
#include "wx/dcclient.h"
#include "wx/toplevel.h"
#include "wx/image.h"
#endif

#include "wx/mstream.h"
#include "wx/arrimpl.cpp"

WX_DEFINE_EXPORTED_OBJARRAY(wxBarButtonArray);


#include "wx/stockitem.h"

IMPLEMENT_DYNAMIC_CLASS(wxBarButton, wxEvtHandler)

BEGIN_EVENT_TABLE(wxBarButton, wxEvtHandler)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

bool wxBarButton::Create(wxWindow *parent,
                           wxWindowID id,
                           const wxString& label1,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxValidator& WXUNUSED(validator),
                           const wxString& WXUNUSED(name))
{
    // FIXME stub
    
    return true;
}

bool wxBarButton::Create(wxWindow *parent,
                           wxWindowID id,
                           const wxBitmap& bitmap1,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxValidator& WXUNUSED(validator),
                           const wxString& WXUNUSED(name))
{
    // FIXME stub
    
    return true;
}

wxBarButton::~wxBarButton()
{
}

void wxBarButton::Init()
{
    // FIXME stub
}

void wxBarButton::Copy(const wxBarButton& item)
{
    // FIXME stub
}
