/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/dcclient.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling, Markus Holzem, Chris Breeze
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcclient.h"
#endif

#include "wx/dcclient.h"
#include "wx/dcmemory.h"

IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxClientDC)
IMPLEMENT_DYNAMIC_CLASS(wxClientDC,wxWindowDC)
