/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/colordlg.cpp
// Purpose:     wxColourDialog class
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "colordlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <stdio.h>
    #include "wx/defs.h"
    #include "wx/bitmap.h"
    #include "wx/pen.h"
    #include "wx/brush.h"
    #include "wx/colour.h"
    #include "wx/gdicmn.h"
    #include "wx/utils.h"
    #include "wx/frame.h"
    #include "wx/dialog.h"
    #include "wx/msgdlg.h"
    #include "wx/math.h"
#endif

#if wxUSE_COLOURDLG && !defined(__SMARTPHONE__)

#include "wx/palmos/private.h"
#include "wx/colordlg.h"
#include "wx/cmndata.h"

#include <stdlib.h>
#include <string.h>

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxColourDialog, wxDialog)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxColourDialog
// ----------------------------------------------------------------------------

wxColourDialog::wxColourDialog()
{
}

wxColourDialog::wxColourDialog(wxWindow *parent, wxColourData *data)
{
}

bool wxColourDialog::Create(wxWindow *parent, wxColourData *data)
{
    return false;
}

int wxColourDialog::ShowModal()
{
    return wxID_CANCEL;
}

// ----------------------------------------------------------------------------
// title
// ----------------------------------------------------------------------------

void wxColourDialog::SetTitle(const wxString& title)
{
    m_title = title;
}

wxString wxColourDialog::GetTitle() const
{
    return m_title;
}

// ----------------------------------------------------------------------------
// position/size
// ----------------------------------------------------------------------------

void wxColourDialog::DoGetPosition(int *x, int *y) const
{
    if ( x )
        *x = m_pos.x;
    if ( y )
        *y = m_pos.y;
}

void wxColourDialog::DoSetSize(int x, int y,
                               int WXUNUSED(width), int WXUNUSED(height),
                               int WXUNUSED(sizeFlags))
{
    if ( x != -1 )
        m_pos.x = x;

    if ( y != -1 )
        m_pos.y = y;

    // ignore the size params - we can't change the size of a standard dialog
    return;
}

// NB: of course, both of these functions are completely bogus, but it's better
//     than nothing
void wxColourDialog::DoGetSize(int *width, int *height) const
{
    // the standard dialog size
    if ( width )
        *width = 225;
    if ( height )
        *height = 324;
}

void wxColourDialog::DoGetClientSize(int *width, int *height) const
{
    // the standard dialog size
    if ( width )
        *width = 219;
    if ( height )
        *height = 299;
}

#endif
