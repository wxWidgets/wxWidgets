/////////////////////////////////////////////////////////////////////////////
// Name:        src/uwp/control.cpp
// Purpose:     wxControl class
// Author:      Yann Clotioloman Yéo
// Modified by:
// Created:     02/01/2022
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_CONTROLS

#include "wx/control.h"

#ifndef WX_PRECOMP
#include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
#include "wx/event.h"
#include "wx/app.h"
#include "wx/dcclient.h"
#include "wx/log.h"
#include "wx/settings.h"
#include "wx/ctrlsub.h"
#include "wx/msw/private.h"
#include "wx/msw/missing.h"
#endif

#include "wx/renderer.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxControl, wxWindow);

// ----------------------------------------------------------------------------
// control window creation
// ----------------------------------------------------------------------------

bool wxControl::Create(wxWindow* parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& wxVALIDATOR_PARAM(validator),
    const wxString& name)
{
    return wxWindow::Create(parent, id, pos, size, style, name);
}

#endif // wxUSE_CONTROLS
