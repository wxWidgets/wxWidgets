/////////////////////////////////////////////////////////////////////////////
// Name:        serbase.cpp
// Purpose:     wxStream base classes
// Author:      Robert Roebling
// Modified by:
// Created:     11/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "stream.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include <wx/serbase.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// ----------------------------------------------------------------------------
// wxObject_Serialize
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxObject_Serialize,wxObject)
#endif
