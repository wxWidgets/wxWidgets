/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.cpp
// Purpose:     wxFontDialog class. NOTE: you can use the generic class
//              if you wish, instead of implementing this.
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "fontdlg.h"
#endif

#include "wx/mac/fontdlg.h"
#include "wx/cmndata.h"

IMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog)

/*
 * wxFontDialog
 */

wxFontDialog::wxFontDialog()
{
    m_dialogParent = NULL;
}

wxFontDialog::wxFontDialog(wxWindow *parent, wxFontData *data)
{
    Create(parent, data);
}

bool wxFontDialog::Create(wxWindow *parent, wxFontData *data)
{
    m_dialogParent = parent;

    if (data)
        m_fontData = *data;

    // TODO: you may need to do dialog creation here, unless it's
    // done in ShowModal.
    return TRUE;
}

int wxFontDialog::ShowModal()
{
    // TODO: show (maybe create) the dialog
    return wxID_CANCEL;
}

