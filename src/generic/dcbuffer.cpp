/////////////////////////////////////////////////////////////////////////////
// Name:        generic/dcbuffer.cpp
// Purpose:     wxBufferedDC class
// Author:      Ron Lee <ron@debian.org>
// Modified by: Vadim Zeitlin (refactored, added bg preservation)
// Created:     16/03/02
// RCS-ID:      $Id$
// Copyright:   (c) Ron Lee
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "dcbuffer.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif

#include "wx/dcbuffer.h"

// ----------------------------------------------------------------------------
// Double buffering helper.
// ----------------------------------------------------------------------------

wxBufferedDC::wxBufferedDC(wxDC *dc, const wxBitmap& buffer)
            : m_dc( dc ),
              m_buffer( buffer )
{
    UseBuffer();
}

wxBufferedDC::wxBufferedDC(wxDC *dc, const wxSize& area, int flags)
            : m_dc( dc ),
              m_buffer( area.GetWidth(), area.GetHeight() )
{
    UseBuffer();

    SaveBg(area, flags);
}

void wxBufferedDC::Init( wxDC *dc, const wxBitmap &buffer )
{
    wxASSERT_MSG( m_dc == 0 && m_buffer == wxNullBitmap,
                  _T("wxBufferedDC already initialised") );

    m_dc = dc;
    m_buffer = buffer;

    UseBuffer();
}

void wxBufferedDC::Init(wxDC *dc, const wxSize& area, int flags)
{
    Init(dc, wxBitmap(area.GetWidth(), area.GetHeight()));

    SaveBg(area, flags);
}

void wxBufferedDC::UnMask()
{
    wxASSERT_MSG( m_dc != 0,
                  _T("No underlying DC associated with wxBufferedDC (anymore)") );

    m_dc->Blit( 0, 0, m_buffer.GetWidth(), m_buffer.GetHeight(), this, 0, 0 );
    m_dc = NULL;
}

