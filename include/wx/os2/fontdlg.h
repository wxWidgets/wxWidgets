/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.h
// Purpose:     wxFontDialog class. Use generic version if no
//              platform-specific implementation.
// Author:      David Webster
// Modified by:
// Created:     10/06/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONTDLG_H_
#define _WX_FONTDLG_H_

#include "wx/dialog.h"
#include "wx/cmndata.h"

/*
 * Font dialog
 */

class WXDLLEXPORT wxFontDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxFontDialog)
public:
    wxFontDialog();
    wxFontDialog(wxWindow *parent, wxFontData *data = NULL);

    bool Create(wxWindow *parent, wxFontData *data = NULL);

    int ShowModal();
    wxFontData& GetFontData() { return m_fontData; }

//EK protected:
    wxWindow*   m_dialogParent;
    wxFontData  m_fontData;
};

#endif
    // _WX_FONTDLG_H_

