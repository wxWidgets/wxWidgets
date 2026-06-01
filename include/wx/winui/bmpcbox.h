/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/bmpcbox.h
// Purpose:     wxWinUI wxBitmapComboBox (WinUI ComboBox with image items)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_BMPCBOX_H_
#define _WX_WINUI_BMPCBOX_H_

#include "wx/combobox.h"

class WXDLLIMPEXP_CORE wxBitmapComboBox : public wxComboBox,
                                          public wxBitmapComboBoxBase
{
public:
    wxBitmapComboBox() = default;

    wxBitmapComboBox(wxWindow *parent,
                     wxWindowID id = wxID_ANY,
                     const wxString& value = wxEmptyString,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     int n = 0,
                     const wxString choices[] = nullptr,
                     long style = 0,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxASCII_STR(wxBitmapComboBoxNameStr))
    {
        Create(parent, id, value, pos, size, n, choices, style, validator, name);
    }

    wxBitmapComboBox(wxWindow *parent,
                     wxWindowID id,
                     const wxString& value,
                     const wxPoint& pos,
                     const wxSize& size,
                     const wxArrayString& choices,
                     long style,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxASCII_STR(wxBitmapComboBoxNameStr))
    {
        Create(parent, id, value, pos, size, choices, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                int n,
                const wxString choices[],
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxBitmapComboBoxNameStr));
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxBitmapComboBoxNameStr));

    void SetItemBitmap(unsigned int n, const wxBitmapBundle& bitmap) override;

    int Append(const wxString& item, const wxBitmapBundle& bitmap = wxBitmapBundle());
    int Append(const wxString& item, const wxBitmapBundle& bitmap, void *clientData);
    int Append(const wxString& item, const wxBitmapBundle& bitmap, wxClientData *clientData);

    int Insert(const wxString& item, const wxBitmapBundle& bitmap, unsigned int pos);
    int Insert(const wxString& item, const wxBitmapBundle& bitmap,
               unsigned int pos, void *clientData);
    int Insert(const wxString& item, const wxBitmapBundle& bitmap,
               unsigned int pos, wxClientData *clientData);

protected:
    wxItemContainer* GetItemContainer() override { return this; }
    wxWindow* GetControl() override { return this; }

    wxBitmap WinUIGetItemBitmap(unsigned int n) const override;

    void DoClear() override;
    void DoDeleteOneItem(unsigned int n) override;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxBitmapComboBox);
};

#endif // _WX_WINUI_BMPCBOX_H_
