///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/menuitem.h
// Purpose:     wxMenuItem class for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.05.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_MENUITEM_H_
#define _WX_UNIV_MENUITEM_H_

#ifdef __GNUG__
    #pragma interface "univmenuitem.h"
#endif

// ----------------------------------------------------------------------------
// wxMenuItem implements wxMenuItemBase
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMenuItem : public wxMenuItemBase
{
public:
    // ctor & dtor
    wxMenuItem(wxMenu *parentMenu = (wxMenu *)NULL,
               int id = wxID_SEPARATOR,
               const wxString& name = wxEmptyString,
               const wxString& help = wxEmptyString,
               bool isCheckable = FALSE,
               wxMenu *subMenu = (wxMenu *)NULL);
    virtual ~wxMenuItem();

    // override base class virtuals to update the item appearance on screen
    virtual void SetText(const wxString& text);
    virtual void SetCheckable(bool checkable);

    virtual void Enable(bool enable = TRUE);
    virtual void Check(bool check = TRUE);
    virtual bool IsChecked() const;

private:
    DECLARE_DYNAMIC_CLASS(wxMenuItem)
};

#endif // _WX_UNIV_MENUITEM_H_

