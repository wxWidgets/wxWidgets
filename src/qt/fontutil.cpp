/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/fontutil.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include <QtGui/QFont>

#include "wx/fontutil.h"
#include "wx/encinfo.h"

bool wxNativeEncodingInfo::FromString(const wxString& WXUNUSED(s))
{
    return false;
}

wxString wxNativeEncodingInfo::ToString() const
{
    return wxString();
}

//#############################################################################

bool wxGetNativeFontEncoding(wxFontEncoding WXUNUSED(encoding),
                          wxNativeEncodingInfo *info)
{
    *info = wxNativeEncodingInfo();

    return false;
}


bool wxTestFontEncoding(const wxNativeEncodingInfo& WXUNUSED(info))
{
    return false;
}

