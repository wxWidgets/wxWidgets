/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.cpp
// Purpose:     wxClientDC class
// Author:      AUTHOR
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcclient.h"
#endif

#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/region.h"
#include <math.h>

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#define RAD2DEG 57.2957795131

//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)
IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxWindowDC)
#endif

/*
 * wxWindowDC
 */

wxWindowDC::wxWindowDC(void)
{
};

wxWindowDC::wxWindowDC( wxWindow *window )
{
};

wxWindowDC::~wxWindowDC(void)
{
};

/*
 * wxClientDC
 */

wxClientDC::wxClientDC(void)
{
};

wxClientDC::wxClientDC( wxWindow *window )
{
};

wxClientDC::~wxClientDC(void)
{
};

/*
 * wxPaintDC
 */

wxPaintDC::wxPaintDC(void)
{
};

wxPaintDC::wxPaintDC( wxWindow *window )
{
};

wxPaintDC::~wxPaintDC(void)
{
};

