/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk1/statbox.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKSTATICBOXH__
#define __GTKSTATICBOXH__

//-----------------------------------------------------------------------------
// wxStaticBox
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxStaticBox : public wxStaticBoxBase
{
public:
    wxStaticBox();
    wxStaticBox( wxWindow *parent,
                 wxWindowID id,
                 const wxString &label,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = wxASCII_STR(wxStaticBoxNameStr) );
    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxString &label,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = wxASCII_STR(wxStaticBoxNameStr) );

    virtual void SetLabel( const wxString &label );

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // implementation

    virtual bool IsTransparentForMouse() const { return TRUE; }

protected:
    void DoApplyWidgetStyle(GtkRcStyle *style);

private:
    wxDECLARE_DYNAMIC_CLASS(wxStaticBox);
};

#endif // __GTKSTATICBOXH__
