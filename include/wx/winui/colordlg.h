/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/colordlg.h
// Purpose:     wxColourDialog for wxWinUI
// Author:      wxWidgets development team
// Created:     2026-06-03
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_COLORDLG_H_
#define _WX_WINUI_COLORDLG_H_

#include "wx/dialog.h"
#include "wx/colourdata.h"

// ----------------------------------------------------------------------------
// wxColourDialog: dialog for choosing a colour (WinUI ColorPicker)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxColourDialog : public wxDialog
{
public:
    wxColourDialog() = default;
    wxColourDialog(wxWindow *parent, const wxColourData *data = nullptr)
    {
        Create(parent, data);
    }

    bool Create(wxWindow *parent, const wxColourData *data = nullptr);

    wxColourData& GetColourData() { return m_colourData; }

    // The WinUI picker is presented asynchronously; keep the requested title
    // independently from the hidden wxDialog owner used for wx lifecycle.
    void SetTitle(const wxString& title) override;
    wxString GetTitle() const override;

    int ShowModal() override;

protected:
    wxColourData m_colourData;
    wxString m_title;
    wxWindow* m_winuiParent = nullptr;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxColourDialog);
};

#endif // _WX_WINUI_COLORDLG_H_
