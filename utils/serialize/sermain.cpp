/////////////////////////////////////////////////////////////////////////////
// Name:        sermain.cpp
// Purpose:     Serialization: main
// Author:      Guilhem Lavaux
// Modified by:
// Created:     July 1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "serbase.h"
#endif

#include <wx/dynlib.h>
#include <wx/serbase.h>

#include "sercore.h"
#include "serwnd.h"
#include "sergdi.h"
#include "serctrl.h"
#include "serext.h"

IMPLEMENT_DYNAMIC_CLASS(wxObject_Serialize, wxObject)

WXDLL_ENTRY_FUNCTION()
