///////////////////////////////////////////////////////////////////////////////
// Name:        univ/inphand.cpp
// Purpose:     (trivial) wxInputHandler implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     18.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "inphand.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/univ/inphand.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxInputHandler
// ----------------------------------------------------------------------------

bool wxInputHandler::HandleMouseMove(wxControl * WXUNUSED(control),
                                     const wxMouseEvent& WXUNUSED(event))
{
    return FALSE;
}

bool wxInputHandler::HandleFocus(wxControl *WXUNUSED(control),
                                 const wxFocusEvent& WXUNUSED(event))
{
    return FALSE;
}

bool wxInputHandler::HandleActivation(wxControl *WXUNUSED(control),
                                      bool WXUNUSED(activated))
{
    return FALSE;
}

wxInputHandler::~wxInputHandler()
{
}

