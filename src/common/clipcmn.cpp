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
    #include "wx/dataobj.h"
    #include "wx/module.h"
#endif

// ---------------------------------------------------------
// wxClipboardEvent
// ---------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboardEvent,wxEvent)

wxDEFINE_EVENT( wxEVT_CLIPBOARD_CHANGED, wxClipboardEvent );

// notice that ctors are defined here and not inline to avoid having to include
// wx/dataobj.h from wx/clipbrd.h
wxClipboardEvent::wxClipboardEvent(wxEventType evtType)
    : wxEvent(0, evtType)
{
}

wxClipboardEvent::wxClipboardEvent(const wxClipboardEvent& event)
    : wxEvent(event),
      m_formats(event.m_formats)
{
}

wxEvent* wxClipboardEvent::Clone() const
{
    return new wxClipboardEvent(*this);
}

bool wxClipboardEvent::SupportsFormat( const wxDataFormat &format ) const
{
#ifdef __WXGTK20__
    // GTK has an asynchronous API which reports the supported formats one by
    // one. We may have to add X11 and Motif later.
    for (wxVector<wxDataFormat>::size_type n = 0; n < m_formats.size(); n++)
    {
        if (m_formats[n] == format)
            return true;
    }

    return false;
#else
    // All other ports just query the clipboard directly
    // from here
    wxClipboard* clipboard = (wxClipboard*) GetEventObject();
    return clipboard->IsSupported( format );
#endif
}

void wxClipboardEvent::AddFormat(const wxDataFormat& format)
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

bool wxClipboardBase::IsSupportedAsync( wxEvtHandler *sink )
{
    // We just imitate an asynchronous API on most platforms.
    // This method is overridden uner GTK.
    wxClipboardEvent *event = new wxClipboardEvent(wxEVT_CLIPBOARD_CHANGED);
    event->SetEventObject( this );

    sink->QueueEvent( event );

    return true;
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
