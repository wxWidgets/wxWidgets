/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/tooltip.cpp
// Purpose:     wxToolTip implementation
// Author:      Stefan Csomor
// Id:          $Id: tooltip.cpp 55419 2008-09-02 16:53:23Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_TOOLTIPS

#include "wx/tooltip.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/window.h"
    #include "wx/dc.h"
    #include "wx/timer.h"
    #include "wx/nonownedwnd.h"
#endif // WX_PRECOMP

#include "wx/geometry.h"
#include "wx/osx/uma.h"

// FYI a link to help with implementing: http://www.cocoadev.com/index.pl?LittleYellowBox


//-----------------------------------------------------------------------------
// wxToolTip
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxToolTip, wxObject)


wxToolTip::wxToolTip( const wxString &tip )
{
    m_text = tip;
    m_window = NULL;
}

wxToolTip::~wxToolTip()
{
}

void wxToolTip::SetTip( const wxString &tip )
{
    m_text = tip;
}

void wxToolTip::SetWindow( wxWindow *win )
{
    m_window = win ;
}

void wxToolTip::Enable( bool flag )
{
}

void wxToolTip::SetDelay( long msecs )
{
}

void wxToolTip::SetAutoPop( long WXUNUSED(msecs) )
{
}

void wxToolTip::SetReshow( long WXUNUSED(msecs) )
{
}

void wxToolTip::RelayEvent( wxWindow *win , wxMouseEvent &event )
{
}

void wxToolTip::RemoveToolTips()
{
}

// --- mac specific
void wxToolTip::NotifyWindowDelete( WXHWND win )
{
}

#endif // wxUSE_TOOLTIPS
