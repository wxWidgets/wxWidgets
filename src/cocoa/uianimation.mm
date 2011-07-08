///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/uianimation.mm
// Purpose:     Platform specific animation definitions
// Author:      Chiciu Bogdan Gabriel
// Created:     2011-07-06
// RCS-ID:      $Id$
// Copyright:   (c) 2011 wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/uianimation.h"

template<>
const wxString wxPropertyType<wxANIMATION_TARGET_PROPERTY_BACKGROUND_COLOR>::OsXKeyPath = "backgroundColor";
template<>
const wxString wxPropertyType<wxANIMATION_TARGET_PROPERTY_POSITION>::OsXKeyPath = "position";
template<>
const wxString wxPropertyType<wxANIMATION_TARGET_PROPERTY_OPACITY>::OsXKeyPath = "opacity";