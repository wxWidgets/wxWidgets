///////////////////////////////////////////////////////////////////////////////
// Name:        menuitem.h
// Purpose:     wxMenuItem class
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef __GTKMENUITEMH__
#define __GTKMENUITEMH__

#ifdef __GNUG__
#pragma interface
#endif

//-----------------------------------------------------------------------------
// wxMenuItem
//-----------------------------------------------------------------------------

class wxMenuItem : public wxMenuItemBase
{
public:
    wxMenuItem(wxMenu *parentMenu = (wxMenu *)NULL,
               int id = wxID_SEPARATOR,
               const wxString& text = wxEmptyString,
               const wxString& help = wxEmptyString,
               bool isCheckable = FALSE,
               wxMenu *subMenu = (wxMenu *)NULL);
    ~wxMenuItem();

    // implement base class virtuals
    virtual void SetText( const wxString& str );
    virtual void Enable( bool enable = TRUE );
    virtual void Check( bool check = TRUE );
    virtual bool IsChecked() const;

    // implementation
    void SetMenuItem(GtkWidget *menuItem) { m_menuItem = menuItem; }
    GtkWidget *GetMenuItem() const { return m_menuItem; }
    wxString GetFactoryPath() const;

    wxString GetHotKey() const { return m_hotKey; }

private:
    // DoSetText() transforms the accel mnemonics in our label from MSW/wxWin
    // style to GTK+ and is called from ctor and SetText()
    void DoSetText(const wxString& text);

    wxString  m_hotKey;

    GtkWidget *m_menuItem;  // GtkMenuItem

    DECLARE_DYNAMIC_CLASS(wxMenuItem)
};


#endif
        //__GTKMENUITEMH__
