/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/srchcmn.cpp
// Purpose:     common (to all ports) bits of wxSearchCtrl
// Author:      Robin Dunn
// Modified by:
// Created:     19-Dec-2006
// Copyright:   (c) wxWidgets team
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


#if wxUSE_SEARCHCTRL

#include "wx/srchctrl.h"

#ifndef WX_PRECOMP
#endif

// ----------------------------------------------------------------------------

const char wxSearchCtrlNameStr[] = "searchCtrl";

wxDEFINE_EVENT(wxEVT_SEARCH_CANCEL, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_SEARCH, wxCommandEvent);


#endif // wxUSE_SEARCHCTRL
