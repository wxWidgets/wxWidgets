///////////////////////////////////////////////////////////////////////////////
// Name:        msw/renderer.cpp
// Purpose:     implementation of wxRendererBase for Windows
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.07.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
#endif //WX_PRECOMP

#include "wx/renderer.h"

// ----------------------------------------------------------------------------
// wxRendererMSW: our wxRendererBase implementation
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRendererMSW : public wxDelegateRendererBase
{
};

// ============================================================================
// implementation
// ============================================================================

/* static */
wxRendererNative& wxRendererMSW::Get()
{
    static wxRendererMSW s_rendererMSW;

    return s_rendererMSW;
}

