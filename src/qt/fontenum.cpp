/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/fontenum.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/fontenum.h"

bool wxFontEnumerator::EnumerateFacenames( wxFontEncoding WXUNUSED(encoding),
                bool WXUNUSED(fixedWidthOnly))
{
    return false;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& WXUNUSED(facename))
{
    return false;
}

#ifdef wxHAS_UTF8_FONTS
bool wxFontEnumerator::EnumerateEncodingsUTF8(const wxString& facename)
{
    return false;
}
#endif
