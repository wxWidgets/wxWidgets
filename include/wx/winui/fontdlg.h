/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/fontdlg.h
// Purpose:     wxFontDialog for wxWinUI
// Author:      wxWidgets development team
// Created:     2026-11-22
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_FONTDLG_H_
#define _WX_WINUI_FONTDLG_H_

class wxWinUIFontDialogImpl;

// ----------------------------------------------------------------------------
// wxFontDialog: the font dialog of Windows, made of WinUI controls
// ----------------------------------------------------------------------------

// It has the layout and the behaviour of the common dialog shown by wxMSW:
// the fonts, the styles of the chosen one and the sizes in three lists, each
// under a text field, the effects, a sample and the script. The fonts and
// their styles are the ones of DirectWrite, which is what WinUI draws text
// with.
class WXDLLIMPEXP_CORE wxFontDialog : public wxFontDialogBase
{
public:
    wxFontDialog() = default;
    wxFontDialog(wxWindow *parent)
        : wxFontDialogBase(parent) { Create(parent); }
    wxFontDialog(wxWindow *parent, const wxFontData& data)
        : wxFontDialogBase(parent, data) { Create(parent, data); }

    ~wxFontDialog() override;

protected:
    bool DoCreate(wxWindow *parent) override;

private:
    wxWinUIFontDialogImpl *m_impl = nullptr;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxFontDialog);
};

#endif // _WX_WINUI_FONTDLG_H_
