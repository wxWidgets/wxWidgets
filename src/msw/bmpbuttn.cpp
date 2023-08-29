/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/bmpbuttn.cpp
// Purpose:     wxBitmapButton
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_BMPBUTTON

#include "wx/bmpbuttn.h"

// ----------------------------------------------------------------------------
// wxBitmapButton implementation
// ----------------------------------------------------------------------------

/*
TODO PROPERTIES :

long "style" , wxBU_AUTODRAW
bool "default" , 0
bitmap "selected" ,
bitmap "focus" ,
bitmap "disabled" ,
*/

bool wxBitmapButton::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxBitmapBundle& bitmap,
                            const wxPoint& pos,
                            const wxSize& size, long style,
                            const wxValidator& validator,
                            const wxString& name)
{
    if ( !wxBitmapButtonBase::Create(parent, id, pos, size, style,
                                     validator, name) )
        return false;

    if ( bitmap.IsOk() )
        SetBitmapLabel(bitmap);

    if ( !size.IsFullySpecified() )
    {
        // As our bitmap has just changed, our best size has changed as well so
        // reset the initial size using the new value.
        SetInitialSize(size);
    }

    return true;
}

#endif // wxUSE_BMPBUTTON
