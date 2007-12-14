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

#include "wx/dfb/dcscreen.h"
#include "wx/dfb/private.h"

// ===========================================================================
// implementation
// ===========================================================================

//-----------------------------------------------------------------------------
// wxScreenDC
//-----------------------------------------------------------------------------

#warning "FIXME: this doesn't work (neither single app nor multiapp core)
// FIXME: maybe use a subsurface as well?

IMPLEMENT_ABSTRACT_CLASS(wxScreenDCImpl, wxDFBDCImpl)

wxScreenDCImpl::wxScreenDCImpl(wxScreenDC *owner)
              : wxDFBDCImpl(owner)
{
    DFBInit(wxIDirectFB::Get()->GetPrimarySurface());
}

#warning "FIXME: does wxScreenDC need Flip call in dtor?"
