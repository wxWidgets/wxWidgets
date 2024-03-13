/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/fontdlg.h
// Purpose:     wxFontDialog class using fonts window services (10.2+).
// Author:      Ryan Norton
// Created:     2004-09-25
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONTDLG_H_
#define _WX_FONTDLG_H_

#include "wx/dialog.h"

class WXDLLIMPEXP_CORE wxFontDialog : public wxFontDialogBase
{
public:
    wxFontDialog() : wxFontDialogBase() { /* must be Create()d later */ }
    wxFontDialog(wxWindow *parent)
        : wxFontDialogBase(parent) { Create(parent); }
    wxFontDialog(wxWindow *parent, const wxFontData& data)
        : wxFontDialogBase(parent, data) { Create(parent, data); }
    virtual ~wxFontDialog();

    int ShowModal() override;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxFontDialog);
};

extern "C" int RunMixedFontDialog(wxFontDialog* dialog) ;

#endif
    // _WX_FONTDLG_H_
