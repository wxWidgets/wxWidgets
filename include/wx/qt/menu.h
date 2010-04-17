/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/menu.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_MENU_H_
#define _WX_QT_MENU_H_

class WXDLLIMPEXP_CORE wxMenu : public wxMenuBase
{
public:
    wxMenu(long style = 0);
    wxMenu(const wxString& title, long style = 0);

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMenu)
};



class WXDLLIMPEXP_CORE wxMenuBar : public wxMenuBarBase
{
public:
    wxMenuBar();
    wxMenuBar(long style);
    wxMenuBar(size_t n, wxMenu *menus[], const wxString titles[], long style = 0);

    virtual void EnableTop(size_t pos, bool enable);

    virtual void SetMenuLabel(size_t pos, const wxString& label);
    virtual wxString GetMenuLabel(size_t pos) const;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMenuBar)
};

#endif // _WX_QT_MENU_H_
