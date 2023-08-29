/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/clrpicker.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CLRPICKER_H_
#define _WX_QT_CLRPICKER_H_

#include "wx/generic/clrpickerg.h"

// TODO: A QtColorPicker is available from
// http://qt.nokia.com/products/appdev/add-on-products/catalog/4/Widgets/qtcolorpicker/
// How to integrate into wxWidgets:
//
// class WXDLLIMPEXP_CORE wxColourPickerWidget : public wxButton, public wxColourPickerWidgetBase

// TODO: For now we reuse the existing wxGenericColourButton but this should be
// changed to use the above mentioned color picker.

class WXDLLIMPEXP_CORE wxColourPickerWidget : public wxGenericColourButton
{
public:
    wxColourPickerWidget();
    wxColourPickerWidget(wxWindow *parent,
                   wxWindowID id,
                   const wxColour& initial = *wxBLACK,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxCLRBTN_DEFAULT_STYLE,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxColourPickerWidgetNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxColour& initial = *wxBLACK,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCLRBTN_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxColourPickerWidgetNameStr));

protected:
    virtual void UpdateColour() override;

private:
};

#endif // _WX_QT_CLRPICKER_H_
