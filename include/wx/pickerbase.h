/////////////////////////////////////////////////////////////////////////////
// Name:        wx/pickerbase.h
// Purpose:     wxPickerBase definition
// Author:      Francesco Montorsi (based on Vadim Zeitlin's code)
// Created:     14/4/2006
// Copyright:   (c) Vadim Zeitlin, Francesco Montorsi
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PICKERBASE_H_BASE_
#define _WX_PICKERBASE_H_BASE_

#include "wx/control.h"
#include "wx/sizer.h"
#include "wx/containr.h"

class WXDLLIMPEXP_FWD_CORE wxTextCtrl;
class WXDLLIMPEXP_FWD_CORE wxToolTip;

extern WXDLLIMPEXP_DATA_CORE(const char) wxButtonNameStr[];

// ----------------------------------------------------------------------------
// wxPickerBase is the base class for the picker controls which support
// a wxPB_USE_TEXTCTRL style; i.e. for those pickers which can use an auxiliary
// text control next to the 'real' picker.
//
// The wxTextPickerHelper class manages enabled/disabled state of the text control,
// its sizing and positioning.
// ----------------------------------------------------------------------------

#define wxPB_USE_TEXTCTRL           0x0002
#define wxPB_SMALL                  0x8000

class WXDLLIMPEXP_CORE wxPickerBase : public wxNavigationEnabled<wxControl>
{
public:
    wxPickerBase() = default;
    virtual ~wxPickerBase() = default;


    // if present, intercepts wxPB_USE_TEXTCTRL style and creates the text control
    // The 3rd argument is the initial wxString to display in the text control
    bool CreateBase(wxWindow *parent,
                    wxWindowID id,
                    const wxString& text = wxEmptyString,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxASCII_STR(wxButtonNameStr));

public:     // public API

    // margin between the text control and the picker
    void SetInternalMargin(int newmargin)
        { GetTextCtrlItem()->SetBorder(newmargin); m_sizer->Layout(); }
    int GetInternalMargin() const
        { return GetTextCtrlItem()->GetBorder(); }

    // proportion of the text control
    void SetTextCtrlProportion(int prop)
        { GetTextCtrlItem()->SetProportion(prop); m_sizer->Layout(); }
    int GetTextCtrlProportion() const
        { return GetTextCtrlItem()->GetProportion(); }

    // proportion of the picker control
    void SetPickerCtrlProportion(int prop)
        { GetPickerCtrlItem()->SetProportion(prop); m_sizer->Layout(); }
    int GetPickerCtrlProportion() const
        { return GetPickerCtrlItem()->GetProportion(); }

    bool IsTextCtrlGrowable() const
        { return (GetTextCtrlItem()->GetFlag() & wxGROW) != 0; }
    void SetTextCtrlGrowable(bool grow = true)
    {
        DoSetGrowableFlagFor(GetTextCtrlItem(), grow);
    }

    bool IsPickerCtrlGrowable() const
        { return (GetPickerCtrlItem()->GetFlag() & wxGROW) != 0; }
    void SetPickerCtrlGrowable(bool grow = true)
    {
        DoSetGrowableFlagFor(GetPickerCtrlItem(), grow);
    }

    bool HasTextCtrl() const
        { return m_text != nullptr; }
    wxTextCtrl *GetTextCtrl()
        { return m_text; }
    wxControl *GetPickerCtrl()
        { return m_picker; }

    // It's not clear why did these functions ever existed, but they can't be
    // used, both controls are, and can be, managed only by the picker itself.
#ifdef WXWIN_COMPATIBILITY_3_2
    wxDEPRECATED_MSG("must not be used, text control is managed internally")
    void SetTextCtrl(wxTextCtrl* text)
        { m_text = text; }
    wxDEPRECATED_MSG("must not be used, picker control is managed internally")
    void SetPickerCtrl(wxControl* picker)
        { m_picker = picker; }
#endif // WXWIN_COMPATIBILITY_3_2

    // methods that derived class must/may override
    virtual void UpdatePickerFromTextCtrl() = 0;
    virtual void UpdateTextCtrlFromPicker() = 0;

protected:
    // overridden base class methods
#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip(wxToolTip *tip) override;
#endif // wxUSE_TOOLTIPS


    // event handlers
    void OnTextCtrlUpdate(wxCommandEvent &);
    void OnTextCtrlKillFocus(wxFocusEvent &);

    // returns the set of styles for the attached wxTextCtrl
    // from given wxPickerBase's styles
    virtual long GetTextCtrlStyle(long style) const
        { return (style & wxWINDOW_STYLE_MASK); }

    // returns the set of styles for the m_picker
    virtual long GetPickerStyle(long style) const
        { return (style & wxWINDOW_STYLE_MASK); }


    wxSizerItem *GetPickerCtrlItem() const
    {
        if (this->HasTextCtrl())
            return m_sizer->GetItem((size_t)1);
        return m_sizer->GetItem((size_t)0);
    }

    wxSizerItem *GetTextCtrlItem() const
    {
        wxASSERT(this->HasTextCtrl());
        return m_sizer->GetItem((size_t)0);
    }

#if WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED_MSG("useless and will be removed in the future")
    int GetDefaultPickerCtrlFlag() const
    {
        return wxALIGN_CENTER_VERTICAL;
    }

    wxDEPRECATED_MSG("useless and will be removed in the future")
    int GetDefaultTextCtrlFlag() const
    {
        // Cast to avoid warnings about mixing elements of different enums.
        return wxALIGN_CENTER_VERTICAL | static_cast<int>(wxRIGHT);
    }
#endif // WXWIN_COMPATIBILITY_3_0

    void PostCreation();

protected:
    wxTextCtrl *m_text = nullptr;     // can be null
    wxControl *m_picker = nullptr;
    wxBoxSizer *m_sizer = nullptr;

private:
    // Common implementation of Set{Text,Picker}CtrlGrowable().
    void DoSetGrowableFlagFor(wxSizerItem* item, bool grow);

    wxDECLARE_ABSTRACT_CLASS(wxPickerBase);
};


#endif
    // _WX_PICKERBASE_H_BASE_
