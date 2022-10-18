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
    wxMenuItem(wxMenu *parentMenu = nullptr,
               int id = wxID_SEPARATOR,
               const wxString& text = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = nullptr);
    virtual ~wxMenuItem();

    // implement base class virtuals
    virtual void SetItemLabel( const wxString& str ) override;
    virtual void Enable( bool enable = true ) override;
    virtual void Check( bool check = true ) override;
    virtual bool IsChecked() const override;
    void SetupBitmaps(wxWindow *win);

#if wxUSE_ACCEL
    virtual void AddExtraAccel(const wxAcceleratorEntry& accel) override;
    virtual void ClearExtraAccels() override;
#endif // wxUSE_ACCEL

    // implementation
    void SetMenuItem(GtkWidget *menuItem);
    GtkWidget *GetMenuItem() const { return m_menuItem; }
    void SetGtkLabel();

#if wxUSE_ACCEL
    void GTKSetExtraAccels();
#endif // wxUSE_ACCEL

private:
    GtkWidget *m_menuItem;  // GtkMenuItem

    wxDECLARE_DYNAMIC_CLASS(wxMenuItem);
};

#endif // _WX_GTKMENUITEM_H_
