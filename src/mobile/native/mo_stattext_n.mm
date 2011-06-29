/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_stattext_n.mm
// Purpose:     wxMoStaticText class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mobile/native/stattext.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoStaticText, wxStaticText)


/// Default constructor.
wxMoStaticText::wxMoStaticText()
{
    Init();
}

void wxMoStaticText::Init()
{
    
}

/// Constructor.
wxMoStaticText::wxMoStaticText(wxWindow *parent,
                               wxWindowID id,
                               const wxString& label,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxString& name)
{
    Init();
    Create(parent, id, label, pos, size, style, name);
}

bool wxMoStaticText::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxString& label,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    return wxStaticText::Create(parent, id, label, pos, size, style, name);
}
