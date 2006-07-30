/////////////////////////////////////////////////////////////////////////////
// Name:        serext.cpp
// Purpose:     Serialization: Other classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     July 1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __SEREXT_H__
#define __SEREXT_H__

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/serbase.h>
#include "serwnd.h"

DECLARE_SERIAL_CLASS(wxSplitterWindow, wxWindow)
DECLARE_SERIAL_CLASS(wxGrid, wxPanel)
DECLARE_SERIAL_CLASS(wxGridCell, wxObject)

#endif
