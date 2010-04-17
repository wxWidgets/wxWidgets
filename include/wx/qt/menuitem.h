/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/menuitem.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_MENUITEM_H_
#define _WX_QT_MENUITEM_H_

class WXDLLIMPEXP_FWD_CORE wxBitmap;

class WXDLLIMPEXP_CORE wxMenuItem : public wxMenuItemBase
{
public:
    wxMenuItem(wxMenu *parentMenu = NULL,
               int id = wxID_SEPARATOR,
               const wxString& text = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = NULL);

    void SetBitmap(const wxBitmap& bitmap);
    const wxBitmap& GetBitmap() const;

};

#endif // _WX_QT_MENUITEM_H_
