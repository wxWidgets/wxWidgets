/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/radiobut.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_RADIOBUT_H_
#define _WX_GTK_RADIOBUT_H_

//-----------------------------------------------------------------------------
// wxRadioButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxRadioButton: public wxRadioButtonBase
{
public:
    wxRadioButton() = default;
    wxRadioButton( wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxRadioButtonNameStr) )
    {
        Create( parent, id, label, pos, size, style, validator, name );
    }

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxASCII_STR(wxRadioButtonNameStr) );

    virtual void SetLabel(const wxString& label) override;
    virtual void SetValue(bool val) override;
    virtual bool GetValue() const override;

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

protected:
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

    virtual void DoApplyWidgetStyle(GtkRcStyle *style) override;
    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const override;

    virtual void DoEnable(bool enable) override;

private:
    typedef wxControl base_type;
    // Only used by wxRB_SINGLE
    GtkWidget* m_hiddenButton = nullptr;

    wxDECLARE_DYNAMIC_CLASS(wxRadioButton);
};

#endif // _WX_GTK_RADIOBUT_H_
