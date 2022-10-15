/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/bmpcbox.h
// Purpose:     wxBitmapComboBox
// Author:      Jaakko Salli
// Created:     2008-04-06
// Copyright:   (c) 2008 Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_BMPCBOX_H_
#define _WX_MSW_BMPCBOX_H_


#include "wx/combobox.h"


// ----------------------------------------------------------------------------
// wxBitmapComboBox: a wxComboBox that allows images to be shown
// in front of string items.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxBitmapComboBox : public wxComboBox,
                                         public wxBitmapComboBoxBase
{
public:
    // ctors and such
    wxBitmapComboBox() : wxComboBox(), wxBitmapComboBoxBase()
    {
        Init();
    }

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
        : wxComboBox(),
          wxBitmapComboBoxBase()
    {
        Init();

        (void)Create(parent, id, value, pos, size, n,
                     choices, style, validator, name);
    }

    wxBitmapComboBox(wxWindow *parent,
                     wxWindowID id,
                     const wxString& value,
                     const wxPoint& pos,
                     const wxSize& size,
                     const wxArrayString& choices,
                     long style,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxASCII_STR(wxBitmapComboBoxNameStr));

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

    virtual ~wxBitmapComboBox();

    // Sets the image for the given item.
    virtual void SetItemBitmap(unsigned int n, const wxBitmapBundle& bitmap) override;

    virtual bool SetFont(const wxFont& font) override;

    // Adds item with image to the end of the combo box.
    int Append(const wxString& item, const wxBitmapBundle& bitmap = wxBitmapBundle());
    int Append(const wxString& item, const wxBitmapBundle& bitmap, void *clientData);
    int Append(const wxString& item, const wxBitmapBundle& bitmap, wxClientData *clientData);

    // Inserts item with image into the list before pos. Not valid for wxCB_SORT
    // styles, use Append instead.
    int Insert(const wxString& item, const wxBitmapBundle& bitmap, unsigned int pos);
    int Insert(const wxString& item, const wxBitmapBundle& bitmap,
               unsigned int pos, void *clientData);
    int Insert(const wxString& item, const wxBitmapBundle& bitmap,
               unsigned int pos, wxClientData *clientData);

protected:

    WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const override;
    virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *item) override;
    virtual bool MSWOnMeasure(WXMEASUREITEMSTRUCT *item) override;
    virtual void MSWUpdateFontOnDPIChange(const wxSize& newDPI) override;

    // Event handlers
    void OnSize(wxSizeEvent& event);

    virtual wxItemContainer* GetItemContainer() override { return this; }
    virtual wxWindow* GetControl() override { return this; }

    // wxItemContainer implementation
    virtual int DoInsertItems(const wxArrayStringsAdapter & items,
                              unsigned int pos,
                              void **clientData, wxClientDataType type) override;
    virtual void DoClear() override;
    virtual void DoDeleteOneItem(unsigned int n) override;

    virtual bool OnAddBitmap(const wxBitmapBundle& bitmap) override;
    virtual wxSize DoGetBestSize() const override;
    void RecreateControl();

private:
    void Init();

    bool m_inResize;

    wxDECLARE_EVENT_TABLE();

    wxDECLARE_DYNAMIC_CLASS(wxBitmapComboBox);
};

#endif // _WX_MSW_BMPCBOX_H_
