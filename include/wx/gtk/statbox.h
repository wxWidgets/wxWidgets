/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/statbox.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTKSTATICBOX_H_
#define _WX_GTKSTATICBOX_H_

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
                 const wxString &name = wxStaticBoxNameStr );
    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxString &label,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = wxStaticBoxNameStr );

    virtual void SetLabel( const wxString &label ) wxOVERRIDE;

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // implementation

    virtual bool GTKIsTransparentForMouse() const wxOVERRIDE { return true; }

    virtual void GetBordersForSizer(int *borderTop, int *borderOther) const wxOVERRIDE;

    virtual void AddChild( wxWindowBase *child ) wxOVERRIDE;

protected:
    virtual bool GTKWidgetNeedsMnemonic() const wxOVERRIDE;
    virtual void GTKWidgetDoSetMnemonic(GtkWidget* w) wxOVERRIDE;

    void DoApplyWidgetStyle(GtkRcStyle *style) wxOVERRIDE;

    DECLARE_DYNAMIC_CLASS(wxStaticBox)
};

#endif // _WX_GTKSTATICBOX_H_
