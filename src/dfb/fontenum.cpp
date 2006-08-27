/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/fontenum.cpp
// Purpose:     wxFontEnumerator class
// Author:      Vaclav Slavik
// Created:     2006-08-10
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/fontenum.h"

// ----------------------------------------------------------------------------
// wxFontEnumerator
// ----------------------------------------------------------------------------

bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding WXUNUSED(encoding),
                                          bool WXUNUSED(fixedWidthOnly))
{
    // FIXME_DFB
    OnFacename(_T("Default"));
    return true;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& WXUNUSED(family))
{
    // FIXME_DFB
    return false;
}
