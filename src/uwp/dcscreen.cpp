/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dcscreen.cpp
// Purpose:     wxScreenDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/msw/dcscreen.h"

#ifndef WX_PRECOMP
   #include "wx/string.h"
   #include "wx/window.h"
#endif

#include "wx/msw/private.h"

wxIMPLEMENT_ABSTRACT_CLASS(wxScreenDCImpl, wxMSWDCImpl);

// Create a DC representing the whole virtual screen (all monitors)
wxScreenDCImpl::wxScreenDCImpl( wxScreenDC *owner ) :
    wxMSWDCImpl( owner )
{
    m_hDC = (WXHDC) ::GetDC((HWND) NULL);

    // the background mode is only used for text background and is set in
    // DrawText() to OPAQUE as required, otherwise always TRANSPARENT
    ::SetBkMode( GetHdc(), TRANSPARENT );
}

// Return the size of the whole virtual screen (all monitors)
void wxScreenDCImpl::DoGetSize(int *width, int *height) const
{
    if ( width )
        *width = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    if ( height )
        *height = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
}
