/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.h
// Purpose:     wxFontDialog class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONTDLG_H_
#define _WX_FONTDLG_H_

#ifdef __GNUG__
#pragma interface "fontdlg.h"
#endif

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

protected:
    wxWindow*   m_dialogParent;
    wxFontData  m_fontData;
};

#endif
    // _WX_FONTDLG_H_

