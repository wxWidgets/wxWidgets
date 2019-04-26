/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/fontdlg.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_FONTDLG_H_
#define _WX_QT_FONTDLG_H_

class QFontDialog;

class WXDLLIMPEXP_CORE wxFontDialog : public wxFontDialogBase
{
public:
    wxFontDialog() { }
    wxFontDialog(wxWindow *parent) { Create(parent); }
    wxFontDialog(wxWindow *parent, const wxFontData& data) { Create(parent, data); }

protected:
    bool DoCreate(wxWindow *parent) wxOVERRIDE;

private:

    wxFontData m_data;

    wxDECLARE_DYNAMIC_CLASS(wxFontDialog);
};

#endif // _WX_QT_FONTDLG_H_
