/////////////////////////////////////////////////////////////////////////////
// Name:        icon.cpp
// Purpose:     wxIcon class
// Author:      David Webster
// Modified by:
// Created:     10/09/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/list.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/icon.h"
#endif

#include "wx/os2/private.h"
#include "assert.h"

#include "wx/icon.h"

    IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxIconBase)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxIconRefData
// ----------------------------------------------------------------------------

void wxIconRefData::Free()
{
    if (m_hIcon)
        ::WinFreeFileIcon((HPOINTER)m_hIcon);
}

// ----------------------------------------------------------------------------
// wxIcon
// ----------------------------------------------------------------------------

wxIcon::wxIcon()
{
}

wxIcon::wxIcon(
  const char                        WXUNUSED(bits)[]
, int                               WXUNUSED(nWidth)
, int                               WXUNUSED(nHeight)
)
{
}

wxIcon::wxIcon(
  const wxString&                   rIconFile
, long                              lFlags
, int                               nDesiredWidth
, int                               nDesiredHeight
)
{
    LoadFile( rIconFile
             ,lFlags
             ,nDesiredWidth
             ,nDesiredHeight
            );
}

wxIcon::~wxIcon()
{
}

bool wxIcon::LoadFile(
  const wxString&                   rFilename
, long                              lType
, int                               nDesiredWidth
, int                               nDesiredHeight
)
{
    wxGDIImageHandler*              pHandler = FindHandler(lType);

    UnRef();
    m_refData = new wxIconRefData;

    if (pHandler)
        return(pHandler->Load( this
                              ,rFilename
                              ,lType
                              ,nDesiredWidth
                              ,nDesiredHeight
                             ));
    else
        return(FALSE);
}

