/////////////////////////////////////////////////////////////////////////////
// Name:        libsample.cpp
// Purpose:     The source of a dummy sample wx-based library
// Author:      Francesco Montorsi
// Modified by:
// Created:     26/11/06
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// ============================================================================
// implementation
// ============================================================================

void MyUtilityFunction()
{
    wxPrintf(wxT("Hello world!\n"));
    fflush(stdout);
}
