///////////////////////////////////////////////////////////////////////////////
// Name:        univ/inphand.cpp
// Purpose:     (trivial) wxInputHandler implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     18.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
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

bool wxInputHandler::HandleMouseMove(wxInputConsumer * WXUNUSED(consumer),
                                     const wxMouseEvent& WXUNUSED(event))
{
    return FALSE;
}

bool wxInputHandler::HandleFocus(wxInputConsumer *WXUNUSED(consumer),
                                 const wxFocusEvent& WXUNUSED(event))
{
    return FALSE;
}

bool wxInputHandler::HandleActivation(wxInputConsumer *WXUNUSED(consumer),
                                      bool WXUNUSED(activated))
{
    return FALSE;
}

wxInputHandler::~wxInputHandler()
{
}

