/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.cpp
// Purpose:     wxFontDialog class. NOTE: you can use the generic class
//              if you wish, instead of implementing this.
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "fontdlg.h"
#endif

#include "wx/mac/fontdlg.h"
#include "wx/cmndata.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog)
#endif

/*
 * wxFontDialog
 */

wxFontDialog::wxFontDialog()
{
    m_dialogParent = NULL;
}

wxFontDialog::wxFontDialog(wxWindow *parent, const wxFontData&  data)
{
    Create(parent, data);
}

bool wxFontDialog::Create(wxWindow *parent, const wxFontData& data)
{
    m_dialogParent = parent;

    m_fontData = data;

    // TODO: you may need to do dialog creation here, unless it's
    // done in ShowModal.
    return TRUE;
}

int wxFontDialog::ShowModal()
{
    // TODO: show (maybe create) the dialog
    return wxID_CANCEL;
}

