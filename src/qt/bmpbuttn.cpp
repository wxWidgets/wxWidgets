/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/bmpbuttn.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/bmpbuttn.h"

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
    // Show the initial bitmap and resize accordingly:
    if ( bitmap.IsOk() )
    {
        wxBitmapButtonBase::SetBitmapLabel(bitmap);

        // we need to adjust the size after setting the bitmap as it may be too
        // big for the default button size
        SetInitialSize(size);
    }
    return true;
}

