/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk1/tglbtn.h
// Purpose:     Declaration of the wxToggleButton class, which implements a
//              toggle button under wxGTK.
// Author:      John Norris, minor changes by Axel Schlueter
// Modified by:
// Created:     08.02.01
// Copyright:   (c) 2000 Johnny C. Norris II
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_TOGGLEBUTTON_H_
#define _WX_GTK_TOGGLEBUTTON_H_

#include "wx/bitmap.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxToggleButton;
class WXDLLIMPEXP_FWD_CORE wxToggleBitmapButton;

//-----------------------------------------------------------------------------
// wxToggleBitmapButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxToggleBitmapButton: public wxToggleButtonBase
{
public:
    // construction/destruction
    wxToggleBitmapButton() {}
    wxToggleBitmapButton(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmap& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxCheckBoxNameStr)
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    // Create the control
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxCheckBoxNameStr);

    // Get/set the value
    void SetValue(bool state);
    bool GetValue() const;

    // Set the label
    virtual void SetLabel(const wxString& label) { wxControl::SetLabel(label); }
    virtual void SetLabel(const wxBitmap& label);
    bool Enable(bool enable = TRUE);

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // implementation
    bool      m_blockEvent;
    wxBitmap  m_bitmap;

    void OnSetBitmap();
    void DoApplyWidgetStyle(GtkRcStyle *style);
    bool IsOwnGtkWindow(GdkWindow *window);

    virtual void OnInternalIdle();
    virtual wxSize DoGetBestSize() const;

private:
    wxDECLARE_DYNAMIC_CLASS(wxToggleBitmapButton);
};

wxDECLARE_DATA_TRANSFER_FWD(wxToggleBitmapButton, wxToggleButtonBase);

//-----------------------------------------------------------------------------
// wxToggleButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxToggleButton: public wxControl
{
public:
    // construction/destruction
    wxToggleButton() {}
    wxToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxCheckBoxNameStr)
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    // Create the control
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxCheckBoxNameStr);

    // Get/set the value
    void SetValue(bool state);
    bool GetValue() const;

    // Set the label
    void SetLabel(const wxString& label);
    bool Enable(bool enable = TRUE);

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // implementation
    bool m_blockEvent;

    void DoApplyWidgetStyle(GtkRcStyle *style);
    bool IsOwnGtkWindow(GdkWindow *window);

    virtual void OnInternalIdle();
    virtual wxSize DoGetBestSize() const;

private:
    wxDECLARE_DYNAMIC_CLASS(wxToggleButton);
};

#if wxUSE_VALIDATORS

// As wxToggleButton does not derive from wxToggleButtonBase, so we can't just use
// wxDECLARE_DATA_TRANSFER_FWD(wxToggleButton, wxToggleButtonBase);

template<>
struct WXDLLIMPEXP_CORE wxDataTransfer<wxToggleButton>
{
    static bool To(wxToggleButton* btn, bool* data)
    {
        btn->SetValue(*data);
        return true;
    }

    static bool From(wxToggleButton* btn, bool* data)
    {
        *data = btn->GetValue();
        return true;
    }
};

#endif // wxUSE_VALIDATORS

#endif // _WX_GTK_TOGGLEBUTTON_H_

