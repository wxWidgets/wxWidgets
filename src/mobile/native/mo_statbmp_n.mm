/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_statbmp_n.mm
// Purpose:     wxMoStaticBitmap class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mobile/native/statbmp.h"

extern WXDLLEXPORT_DATA(const char) wxStaticBitmapNameStr[] = "staticBitmap";

IMPLEMENT_DYNAMIC_CLASS(wxMoStaticBitmap, wxStaticBitmap)

// FIXME stub



#pragma mark -
#pragma mark wxStaticBitmap implementation

wxIMPLEMENT_DYNAMIC_CLASS_XTI(wxStaticBitmap, wxControl, "wx/statbmp.h")



#pragma mark -
#pragma mark wxStaticBitmapBase implementation

wxStaticBitmapBase::~wxStaticBitmapBase()
{
	// FIXME stub
}

wxSize wxStaticBitmapBase::DoGetBestSize() const
{
	// FIXME stub
	wxSize empty(1, 1);
	return empty;
}
