///////////////////////////////////////////////////////////////////////////////
// Name:        wx/uwp/menuitem.h
// Purpose:     wxMenuItem class
// Author:      Yann Clotioloman Yéo
// Modified by: 
// Created:     02/01/2022
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UWP_MENUITEM_H
#define _WX_UWP_MENUITEM_H

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/bitmap.h"

class WXDLLIMPEXP_CORE wxMenuItem : public wxMenuItemBase
#if wxUSE_OWNER_DRAWN
                             , public wxOwnerDrawn
#endif
{
public:
    // ctor & dtor
    wxMenuItem(wxMenu *parentMenu = NULL,
               int id = wxID_SEPARATOR,
               const wxString& name = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = NULL);

    virtual ~wxMenuItem();


private:

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxMenuItem);
};

#endif  //_WX_UWP_MENUITEM_H
