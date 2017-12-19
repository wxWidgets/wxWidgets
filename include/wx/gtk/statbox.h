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
    wxStaticBox()
    {
        Init();
    }

    wxStaticBox( wxWindow *parent,
                 wxWindowID id,
                 const wxString &label,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = wxStaticBoxNameStr )
    {
        Init();

        Create( parent, id, label, pos, size, style, name );
    }

    wxStaticBox( wxWindow *parent,
                 wxWindowID id,
                 wxWindow* label,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = wxStaticBoxNameStr )
    {
        Init();

        Create( parent, id, label, pos, size, style, name );
    }

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxString &label,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = wxStaticBoxNameStr )
    {
        return DoCreate( parent, id, &label, NULL, pos, size, style, name );
    }

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 wxWindow* label,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = wxStaticBoxNameStr )
    {
        return DoCreate( parent, id, NULL, label, pos, size, style, name );
    }

    virtual void SetLabel( const wxString &label ) wxOVERRIDE;

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // implementation

    virtual bool GTKIsTransparentForMouse() const wxOVERRIDE { return true; }

    virtual void GetBordersForSizer(int *borderTop, int *borderOther) const wxOVERRIDE;

    virtual void AddChild( wxWindowBase *child ) wxOVERRIDE;

    virtual void WXDestroyWithoutChildren() wxOVERRIDE;

protected:
    // Common part of all ctors.
    void Init()
    {
        m_labelWin = NULL;
    }

    // Common implementation of both Create() overloads: exactly one of
    // labelStr and labelWin parameters must be non-null.
    bool DoCreate(wxWindow *parent,
                  wxWindowID id,
                  const wxString* labelStr,
                  wxWindow* labelWin,
                  const wxPoint& pos,
                  const wxSize& size,
                  long style,
                  const wxString& name);

    virtual bool GTKWidgetNeedsMnemonic() const wxOVERRIDE;
    virtual void GTKWidgetDoSetMnemonic(GtkWidget* w) wxOVERRIDE;

    void DoApplyWidgetStyle(GtkRcStyle *style) wxOVERRIDE;

    // If non-null, the window used as our label. This window is owned by the
    // static box and will be deleted when it is.
    wxWindow* m_labelWin;

    wxDECLARE_DYNAMIC_CLASS(wxStaticBox);
};

// Indicate that we have the ctor overload taking wxWindow as label.
#define wxHAS_WINDOW_LABEL_IN_STATIC_BOX

#endif // _WX_GTKSTATICBOX_H_
