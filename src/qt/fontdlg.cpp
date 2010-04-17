/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/fontdlg.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/fontdlg.h"

wxFontDialog::wxFontDialog()
{
}

wxFontDialog::wxFontDialog(wxWindow *parent)
{
}

wxFontDialog::wxFontDialog(wxWindow *parent, const wxFontData& data)
{
}


QFontDialog *wxFontDialog::GetHandle() const
{
    return NULL;
}

