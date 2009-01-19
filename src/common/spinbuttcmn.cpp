/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/spinbuttcmn.cpp
// Purpose:     define wxSpinButton event types
// Author:      Peter Most
// Created:     01.11.08
// RCS-ID:      $Id$
// Copyright:   (c) 2008-2009 wxWidgets team
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/spinbutt.h"

wxDEFINE_EVENT_REFERENCE( wxEVT_SPIN_UP,   wxSpinEvent, wxEVT_SCROLL_LINEUP )
wxDEFINE_EVENT_REFERENCE( wxEVT_SPIN_DOWN, wxSpinEvent, wxEVT_SCROLL_LINEDOWN )
wxDEFINE_EVENT_REFERENCE( wxEVT_SPIN,      wxSpinEvent, wxEVT_SCROLL_THUMBTRACK )

