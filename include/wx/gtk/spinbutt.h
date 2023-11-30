/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/spinbutt.h
// Purpose:     wxSpinButton class
// Author:      Robert Roebling
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_SPINBUTT_H_
#define _WX_GTK_SPINBUTT_H_

//-----------------------------------------------------------------------------
// wxSpinButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxSpinButton : public wxSpinButtonBase
{
public:
    wxSpinButton();
    wxSpinButton(wxWindow *parent,
                 wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_VERTICAL,
                 const wxString& name = wxSPIN_BUTTON_NAME)
    {
        Create(parent, id, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_VERTICAL,
                const wxString& name = wxSPIN_BUTTON_NAME);

    virtual int GetValue() const override;
    virtual void SetValue( int value ) override;
    virtual void SetRange( int minVal, int maxVal ) override;
    virtual int GetMin() const override;
    virtual int GetMax() const override;

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // implementation
    int m_pos;

protected:
    void GtkDisableEvents() const;
    void GtkEnableEvents() const;

    virtual wxSize DoGetBestSize() const override;
    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const override;

    virtual void DoEnable(bool enable) override;

private:
    typedef wxSpinButtonBase base_type;

    wxDECLARE_DYNAMIC_CLASS(wxSpinButton);
};

#endif // _WX_GTK_SPINBUTT_H_
