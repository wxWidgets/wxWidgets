/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk1/radiobut.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKRADIOBUTTONH__
#define __GTKRADIOBUTTONH__

//-----------------------------------------------------------------------------
// wxRadioButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxRadioButton: public wxRadioButtonBase
{
public:
    wxRadioButton() { }
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

    virtual void SetLabel(const wxString& label);
    virtual void SetValue(bool val);
    virtual bool GetValue() const;
    virtual bool Enable( bool enable = TRUE );

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // implementation

    virtual bool IsRadioButton() const { return TRUE; }

    void DoApplyWidgetStyle(GtkRcStyle *style);
    bool IsOwnGtkWindow( GdkWindow *window );
    void OnInternalIdle();

    bool m_blockEvent;

protected:
    virtual wxSize DoGetBestSize() const;

private:
    wxDECLARE_DYNAMIC_CLASS(wxRadioButton);
};

#endif // __GTKRADIOBUTTONH__
