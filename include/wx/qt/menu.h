/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/menu.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_MENU_H_
#define _WX_QT_MENU_H_

#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>

class WXDLLIMPEXP_CORE wxMenu : public wxMenuBase
{
public:
    wxMenu(long style = 0);
    wxMenu(const wxString& title, long style = 0);

    virtual QMenu *GetHandle() const;

protected:
    virtual wxMenuItem *DoAppend(wxMenuItem *item);
    virtual wxMenuItem *DoInsert(size_t pos, wxMenuItem *item);
    virtual wxMenuItem *DoRemove(wxMenuItem *item);

private:
    QMenu *m_qtMenu;

    wxDECLARE_DYNAMIC_CLASS(wxMenu);
};



class WXDLLIMPEXP_CORE wxMenuBar : public wxMenuBarBase
{
public:
    wxMenuBar();
    wxMenuBar(long style);
    wxMenuBar(size_t n, wxMenu *menus[], const wxString titles[], long style = 0);

    virtual bool Append(wxMenu *menu, const wxString& title);
    virtual bool Insert(size_t pos, wxMenu *menu, const wxString& title);
    virtual wxMenu *Remove(size_t pos);

    virtual void EnableTop(size_t pos, bool enable);

    virtual void SetMenuLabel(size_t pos, const wxString& label);
    virtual wxString GetMenuLabel(size_t pos) const;

    virtual QMenuBar *GetHandle() const;

    virtual void Attach(wxFrame *frame);
    virtual void Detach();

private:
    QMenuBar *m_qtMenuBar;

    wxDECLARE_DYNAMIC_CLASS(wxMenuBar);
};

#endif // _WX_QT_MENU_H_
