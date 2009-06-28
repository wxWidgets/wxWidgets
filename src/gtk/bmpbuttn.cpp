/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/bmpbuttn.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_BMPBUTTON

#include "wx/bmpbuttn.h"

IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton,wxButton)

bool wxBitmapButton::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxBitmap& bitmap,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxValidator& validator,
                            const wxString& name)
{
    // we use wxBU_NOTEXT to let the base class Create() know that we are not
    // going to show the label -- this is a hack, but like this it can support
    // bitmaps with all GTK+ versions, not just 2.6+ which support both labels
    // and bitmaps
    //
    // and we also use wxBU_EXACTFIT to avoid being resized up to the standard
    // button size as this doesn't make sense for bitmap buttons which are not
    // standard anyhow
    if ( !wxBitmapButtonBase::Create(parent, id, pos, size, style,
                                     validator, name) )
        return false;

    if ( bitmap.IsOk() )
    {
        SetBitmapLabel(bitmap);

        // we need to adjust the size after setting the bitmap as it may be too
        // big for the default button size
        SetInitialSize(size);
    }

    return true;
}
#endif // wxUSE_BMPBUTTON
