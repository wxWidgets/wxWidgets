// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/fontenum.h"

// ----------------------------------------------------------------------------
// wxFontEnumerator
// ----------------------------------------------------------------------------

bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding encoding,
                                          bool fixedWidthOnly)
{
    return false;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& facename)
{
    return false;
}
