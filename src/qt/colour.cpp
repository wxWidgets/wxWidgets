/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/colour.h
// Purpose:     wxColour class implementation for wxQt
// Author:      Kolya Kosenko
// Created:     2010-05-12
// Copyright:   (c) 2010 Kolya Kosenko
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "wx/qt/private/utils.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

int wxColour::GetPixel() const
{
    wxMISSING_IMPLEMENTATION( "wxColour::GetPixel" );
    return 0;
}

#include "wx/colour.h"
