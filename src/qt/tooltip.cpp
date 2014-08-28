/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/tooltip.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/tooltip.h"
#include "wx/qt/private/utils.h"

/* static */ void wxToolTip::Enable(bool WXUNUSED(flag))
{
    wxMISSING_FUNCTION();
}

/* static */ void wxToolTip::SetDelay(long WXUNUSED(milliseconds))
{
    wxMISSING_FUNCTION();
}

/* static */ void wxToolTip::SetAutoPop(long WXUNUSED(milliseconds))
{
    wxMISSING_FUNCTION();
}

/* static */ void wxToolTip::SetReshow(long WXUNUSED(milliseconds))
{
    wxMISSING_FUNCTION();
}



wxToolTip::wxToolTip(const wxString &tip)
{
    SetTip( tip );
}

void wxToolTip::SetTip(const wxString& tip)
{
    m_text = tip;
}

const wxString &wxToolTip::GetTip() const
{
    return m_text;
}


void wxToolTip::SetWindow(wxWindow *win)
{
    m_window = win;
    wxMISSING_FUNCTION();
}
