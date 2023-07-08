/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/fontdlg.h
// Purpose:     wxFontDialog class using fonts window services (10.2+).
// Author:      Ryan Norton
// Modified by:
// Created:     2004-09-25
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONTDLG_H_
#define _WX_FONTDLG_H_

#include "wx/dialog.h"

/*
 * Font dialog
 */

class WXDLLIMPEXP_CORE wxFontDialog : public wxDialog
{
public:
    wxFontDialog();
    wxFontDialog(wxWindow *parent);
    wxFontDialog(wxWindow *parent, const wxFontData& data);
    virtual ~wxFontDialog();

    bool Create(wxWindow *parent);
    bool Create(wxWindow *parent, const wxFontData& data);

    int ShowModal() override;
    wxFontData& GetFontData() { return m_fontData; }

protected:
    wxFontData m_fontData;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxFontDialog);
};

extern "C" int RunMixedFontDialog(wxFontDialog* dialog) ;

#endif
    // _WX_FONTDLG_H_
