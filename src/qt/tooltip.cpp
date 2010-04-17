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

void wxToolTip::Enable(bool flag)
{
}

void wxToolTip::SetDelay(long milliseconds)
{
}

void wxToolTip::SetAutoPop(long milliseconds)
{
}

void wxToolTip::SetReshow(long milliseconds)
{
}

wxToolTip::wxToolTip(const wxString &tip)
{
}

void wxToolTip::SetTip(const wxString& tip)
{
}

const wxString &wxToolTip::GetTip() const
{
    static wxString s_string;

    return s_string;
}

