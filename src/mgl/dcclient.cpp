/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.cpp
// Purpose:
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
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
