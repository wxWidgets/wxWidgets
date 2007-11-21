/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.h
// Purpose:     wxFontDialog class. Use generic version if no
//              platform-specific implementation.
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
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
    wxFontDialog(wxWindow *parent, const wxFontData& data);

    bool Create(wxWindow *parent, const wxFontData& data);

    int ShowModal();
    wxFontData& GetFontData() { return m_fontData; }

protected:
    wxWindow*   m_dialogParent;
    wxFontData  m_fontData;
};

#endif
    // _WX_FONTDLG_H_

