/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.cpp
// Purpose:
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcclient.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/window.h"
#endif

#include <mgraph.hpp>

IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxClientDC)
IMPLEMENT_DYNAMIC_CLASS(wxClientDC,wxWindowDC)

wxWindowDC::wxWindowDC(wxWindow *win) : m_wnd(win)
{
    MGLDC *dc = MGL_wmBeginPaint(m_wnd->GetHandle());
    SetMGLDC(new MGLDevCtx(dc), FALSE);
    // FIXME_MGL -- correctly handle setting device origin and
    //              clipping regions
}

wxWindowDC::~wxWindowDC()
{
    MGL_wmEndPaint(m_wnd->GetHandle());
}

wxClientDC::wxClientDC(wxWindow *win) : wxWindowDC(win)
{
    wxRect r = m_wnd->GetClientRect();
    SetClippingRegion(r);
    SetDeviceOrigin(r.x, r.y);
}
