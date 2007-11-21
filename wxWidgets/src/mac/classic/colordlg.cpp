/////////////////////////////////////////////////////////////////////////////
// Name:        colordlg.cpp
// Purpose:     wxColourDialog class. NOTE: you can use the generic class
//              if you wish, instead of implementing this.
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/mac/colordlg.h"

IMPLEMENT_DYNAMIC_CLASS(wxColourDialog, wxDialog)

#include "wx/mac/private.h"
#ifndef __DARWIN__
#include <ColorPicker.h>
#endif

/*
 * wxColourDialog
 */

wxColourDialog::wxColourDialog()
{
    m_dialogParent = NULL;
}

wxColourDialog::wxColourDialog(wxWindow *parent, wxColourData *data)
{
    Create(parent, data);
}

bool wxColourDialog::Create(wxWindow *parent, wxColourData *data)
{
    m_dialogParent = parent;
  
    if (data)
        m_colourData = *data;
    return TRUE;
}

int wxColourDialog::ShowModal()
{
    Point where ;
    RGBColor currentColor = *((RGBColor*)m_colourData.m_dataColour.GetPixel()) , newColor ;
    
    where.h = where.v = -1;

    if (GetColor( where, "\pSelect a new palette color.", &currentColor, &newColor ))
    {
        m_colourData.m_dataColour.Set( (WXCOLORREF*) &newColor ) ;
        return wxID_OK;
    }
    else
    {
        return wxID_CANCEL;
    }

    return wxID_CANCEL;
}

