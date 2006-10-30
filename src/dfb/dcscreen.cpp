/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/dcscreen.cpp
// Purpose:     wxScreenDC implementation
// Author:      Vaclav Slavik
// Created:     2006-08-16
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcscreen.h"

#include "wx/dfb/private.h"

// ===========================================================================
// implementation
// ===========================================================================

//-----------------------------------------------------------------------------
// wxScreenDC
//-----------------------------------------------------------------------------

#warning "FIXME: verify that wxScreenDC works in 2nd DirectFB app started"
// FIXME: maybe use a subsurface as well?

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxDC)

wxScreenDC::wxScreenDC()
{
    DFBInit(wxIDirectFB::Get()->GetPrimarySurface());
}

#warning "FIXME: does wxScreenDC need Flip call in dtor?"
