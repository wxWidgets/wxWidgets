/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/bmpbuttn.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/bmpbuttn.h"

wxBEGIN_EVENT_TABLE(wxBitmapButton, wxBitmapButtonBase)
wxEND_EVENT_TABLE()

wxBitmapButton::wxBitmapButton()
{
}


wxBitmapButton::wxBitmapButton(wxWindow *parent,
               wxWindowID id,
               const wxBitmap& bitmap,
               const wxPoint& pos,
               const wxSize& size,
               long style,
               const wxValidator& validator,
               const wxString& name )
{
    Create( parent, id, bitmap, pos, size, style, validator, name );
}


bool wxBitmapButton::Create(wxWindow *parent,
            wxWindowID id,
            const wxBitmap& bitmap,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name )
{
    if ( !wxBitmapButtonBase::Create( parent, id, pos, size, style, validator, name ))
        return false;

    return true;
}

