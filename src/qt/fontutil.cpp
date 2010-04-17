/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/fontutil.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/fontutil.h"
#include "wx/encinfo.h"

bool wxNativeEncodingInfo::FromString(const wxString& s)
{
    return false;
}

wxString wxNativeEncodingInfo::ToString() const
{
    return wxString();
}

//#############################################################################

bool wxGetNativeFontEncoding(wxFontEncoding encoding,
                          wxNativeEncodingInfo *info)
{
    *info = wxNativeEncodingInfo();
    
    return false;
}


bool wxTestFontEncoding(const wxNativeEncodingInfo& info)
{
    return false;
}

