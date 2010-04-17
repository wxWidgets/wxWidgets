/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/colordlg.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/colordlg.h"

wxColourDialog::wxColourDialog()
{
}

wxColourDialog::wxColourDialog(wxWindow *parent, wxColourData *data )
{
}


bool wxColourDialog::Create(wxWindow *parent, wxColourData *data )
{
    return false;
}


wxColourData &wxColourDialog::GetColourData()
{
    static wxColourData data;
    
    return data;
}


int wxColourDialog::ShowModal()
{
    return 0;
}


QColorDialog *wxColourDialog::GetHandle() const
{
    return NULL;
}

