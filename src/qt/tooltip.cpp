/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/tooltip.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/tooltip.h"
#include "wx/qt/utils.h"

/* static */ void wxToolTip::Enable(bool flag)
{
    wxMISSING_FUNCTION();
}

/* static */ void wxToolTip::SetDelay(long milliseconds)
{
    wxMISSING_FUNCTION();
}

/* static */ void wxToolTip::SetAutoPop(long milliseconds)
{
    wxMISSING_FUNCTION();
}

/* static */ void wxToolTip::SetReshow(long milliseconds)
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

