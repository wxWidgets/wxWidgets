/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.h
// Purpose:     wxFontDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_FONTDLG_H_
#define _WX_MSW_FONTDLG_H_

#ifdef __GNUG__
    #pragma interface "fontdlg.h"
#endif

#include "wx/dialog.h"
#include "wx/cmndata.h"

// ----------------------------------------------------------------------------
// wxFontDialog
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFontDialog : public wxDialog
{
public:
    wxFontDialog();
    wxFontDialog(wxWindow *parent, wxFontData *data = NULL);

    bool Create(wxWindow *parent, wxFontData *data = NULL);

    virtual int ShowModal();

    wxFontData& GetFontData() { return m_fontData; }

protected:
    wxFontData m_fontData;

    DECLARE_DYNAMIC_CLASS(wxFontDialog)
};

#endif
    // _WX_MSW_FONTDLG_H_

