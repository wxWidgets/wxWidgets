///////////////////////////////////////////////////////////////////////////////
// Name:        common/buttoncmn.cpp
// Purpose:     wxMenu and wxMenuBar methods common to all ports
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004-08-15
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 2004
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "buttonbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_BUTTON

#include "wx/button.h"

#ifndef __WXGTK20__
bool wxButtonBase::CreateStock(wxWindow *parent,
                               wxWindowID id,
                               wxStockItemID stock,
                               const wxString& descriptiveLabel,
                               const wxPoint& pos,
                               long style,
                               const wxValidator& validator,
                               const wxString& name)
{
    wxString label;
#if !defined(__WXMAC__) && !defined(__WXCOCOA__)
    wxUnusedVar(descriptiveLabel);
#else // defined(__WXMAC__) || defined(__WXCOCOA__)
    if (!descriptiveLabel.empty())
    {
        label = descriptiveLabel;
    }
    else
#endif
    {
        label = wxGetStockItemLabel(stock);
    }
    
    return wxStaticCast(this, wxButton)->Create(parent, id, label,
                                                pos, wxDefaultSize,
                                                style, validator, name);
}
#endif

#endif // wxUSE_BUTTON
