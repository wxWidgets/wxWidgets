/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/menu.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_MENU_H_
#define _WX_QT_MENU_H_

class QMenu;
class QMenuBar;

class WXDLLIMPEXP_CORE wxMenu : public wxMenuBase
{
public:
    explicit wxMenu(long style = 0);
    explicit wxMenu(const wxString& title, long style = 0);

    virtual QMenu *GetHandle() const;

protected:
    virtual wxMenuItem *DoAppend(wxMenuItem *item) override;
    virtual wxMenuItem *DoInsert(size_t pos, wxMenuItem *item) override;
    virtual wxMenuItem *DoRemove(wxMenuItem *item) override;

private:
    QMenu *m_qtMenu;

    wxDECLARE_DYNAMIC_CLASS(wxMenu);
};



class WXDLLIMPEXP_CORE wxMenuBar : public wxMenuBarBase
{
public:
    wxMenuBar();
    explicit wxMenuBar(long style);
    wxMenuBar(size_t n, wxMenu *menus[], const wxString titles[], long style = 0);

    virtual bool Append(wxMenu *menu, const wxString& title) override;
    virtual bool Insert(size_t pos, wxMenu *menu, const wxString& title) override;
    virtual wxMenu *Remove(size_t pos) override;

    virtual void EnableTop(size_t pos, bool enable) override;
    virtual bool IsEnabledTop(size_t pos) const override;

    virtual void SetMenuLabel(size_t pos, const wxString& label) override;
    virtual wxString GetMenuLabel(size_t pos) const override;

    QMenuBar* GetQMenuBar() const;

    virtual void Attach(wxFrame *frame) override;
    virtual void Detach() override;

private:
    wxDECLARE_DYNAMIC_CLASS(wxMenuBar);
};

#endif // _WX_QT_MENU_H_
