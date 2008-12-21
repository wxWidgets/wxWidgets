/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/clipcmn.cpp
// Purpose:     common (to all ports) wxClipboard functions
// Author:      Robert Roebling
// Modified by:
// Created:     28.06.99
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CLIPBOARD

#include "wx/clipbrd.h"

#ifndef WX_PRECOMP
    #include "wx/module.h"
#endif

// ---------------------------------------------------------
// wxClipboardEvent
// ---------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboardEvent,wxEvent)

DEFINE_EVENT_TYPE(wxEVT_CLIPBOARD_CHANGED)

bool wxClipboardEvent::SupportsFormat( const wxDataFormat &format ) const
{ 
    wxVector<wxDataFormat>::size_type n;
    for (n = 0; n < m_formats.size(); n++)
        { if (m_formats[n] == format) return true; }
    return false;
}     
        
void wxClipboardEvent::AddFormat( const wxDataFormat &format ) 
{ 
    m_formats.push_back( format );
}

// ---------------------------------------------------------
// wxClipboardBase
// ---------------------------------------------------------

static wxClipboard *gs_clipboard = NULL;

/*static*/ wxClipboard *wxClipboardBase::Get()
{
    if ( !gs_clipboard )
    {
        gs_clipboard = new wxClipboard;
    }
    return gs_clipboard;
}

// ----------------------------------------------------------------------------
// wxClipboardModule: module responsible for destroying the global clipboard
// object
// ----------------------------------------------------------------------------

class wxClipboardModule : public wxModule
{
public:
    bool OnInit() { return true; }
    void OnExit() { wxDELETE(gs_clipboard); }

private:
    DECLARE_DYNAMIC_CLASS(wxClipboardModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxClipboardModule, wxModule)

#endif // wxUSE_CLIPBOARD
