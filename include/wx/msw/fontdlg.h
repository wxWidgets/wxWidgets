/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.h
// Purpose:     wxFontDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
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
 * FONT DIALOG
 */
 
class WXDLLEXPORT wxFontDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxFontDialog)

public:
    wxFontDialog(void);
    wxFontDialog(wxWindow *parent, wxFontData *data = NULL);

    bool Create(wxWindow *parent, wxFontData *data = NULL);

    int ShowModal(void);
    wxFontData& GetFontData(void) { return m_fontData; }

protected:
    wxWindow *m_dialogParent;
    wxFontData m_fontData;
};

#endif
    // _WX_FONTDLG_H_

