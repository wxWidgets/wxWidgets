/////////////////////////////////////////////////////////////////////////////
// Name:        fontenum.cpp
// Purpose:     wxFontEnumerator class for Windows
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/font.h"
#endif

#include <wx/msw/private.h>

int CALLBACK wxFontEnumeratorProc(LPLOGFONT lplf, LPTEXTMETRIC lptm,
                           DWORD dwStyle, LONG lParam)
{
    // Get rid of any fonts that we don't want...
	if (dwStyle != TRUETYPE_FONTTYPE)
		return 1;

	wxFontEnumerator *fontEnum = (wxFontEnumerator *)lParam;

    wxFont font = wxCreateFontFromLogFont(lplf);

	if (fontEnum->OnFont(font))
	    return 1 ;
    else
        return 0 ;
}

IMPLEMENT_CLASS(wxFontEnumerator, wxObject)

bool wxFontEnumerator::Enumerate()
{
    m_faceNames.Clear();

	HDC hDC = ::GetDC(NULL);
#ifdef __WIN32__
    ::EnumFontFamilies(hDC, (LPTSTR) NULL, (FONTENUMPROC) wxFontEnumeratorProc, (LPARAM) (void*) this) ;
#else
    ::EnumFonts(hDC, (LPTSTR) NULL, (FONTENUMPROC) wxFontEnumeratorProc, (LPARAM) (void*) this) ;
#endif
	::ReleaseDC(NULL, hDC);
	return TRUE;
}	

bool wxFontEnumerator::OnFont(const wxFont& font)
{
    m_faceNames.Add(font.GetFaceName());
    return TRUE;
}

