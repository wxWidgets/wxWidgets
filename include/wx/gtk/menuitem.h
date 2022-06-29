///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/menuitem.h
// Purpose:     wxMenuItem class
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTKMENUITEM_H_
#define _WX_GTKMENUITEM_H_

//-----------------------------------------------------------------------------
// wxMenuItem
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMenuItem : public wxMenuItemBase
{
public:
    wxMenuItem(wxMenu *parentMenu = NULL,
               int id = wxID_SEPARATOR,
               const wxString& text = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = NULL);
    virtual ~wxMenuItem();

    // implement base class virtuals
    virtual void SetItemLabel( const wxString& str ) wxOVERRIDE;
    virtual void Enable( bool enable = true ) wxOVERRIDE;
    virtual void Check( bool check = true ) wxOVERRIDE;
    virtual bool IsChecked() const wxOVERRIDE;
    void SetupBitmaps(wxWindow *win);

#if wxUSE_ACCEL
    virtual void AddExtraAccel(const wxAcceleratorEntry& accel) wxOVERRIDE;
    virtual void ClearExtraAccels() wxOVERRIDE;
#endif // wxUSE_ACCEL

    // implementation
    void SetMenuItem(GtkWidget *menuItem);
    GtkWidget *GetMenuItem() const { return m_menuItem; }
    void SetGtkLabel();

#if wxUSE_ACCEL
    void GTKSetExtraAccels();
#endif // wxUSE_ACCEL

#if WXWIN_COMPATIBILITY_2_8
    // compatibility only, don't use in new code
    wxDEPRECATED_CONSTRUCTOR(
    wxMenuItem(wxMenu *parentMenu,
               int id,
               const wxString& text,
               const wxString& help,
               bool isCheckable,
               wxMenu *subMenu = NULL)
    );
#endif

private:
    GtkWidget *m_menuItem;  // GtkMenuItem

    wxDECLARE_DYNAMIC_CLASS(wxMenuItem);
};

#endif // _WX_GTKMENUITEM_H_
