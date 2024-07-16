/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/statusbar.h
// Author:      Peter Most, Javier Torres, Mariano Reingart, Sean D'Epagnier
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_STATUSBAR_H_
#define _WX_QT_STATUSBAR_H_

#include "wx/statusbr.h"

class QStatusBar;

class WXDLLIMPEXP_CORE wxStatusBar : public wxStatusBarBase
{
public:
    wxStatusBar() = default;
    wxStatusBar(wxWindow *parent, wxWindowID winid = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxStatusBarNameStr));

    bool Create(wxWindow *parent, wxWindowID winid = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxStatusBarNameStr));

    virtual void SetStatusWidths(int n, const int widths_field[]) override;
    virtual bool GetFieldRect(int i, wxRect& rect) const override;
    virtual void SetMinHeight(int height) override;
    virtual int GetBorderX() const override;
    virtual int GetBorderY() const override;

    QStatusBar* GetQStatusBar() const;

protected:
    virtual void DoUpdateStatusText(int number) override;

private:
    void CreateFieldsIfNeeded();

    std::vector<QWidget*> m_qtPanes;

    wxDECLARE_DYNAMIC_CLASS(wxStatusBar);
};


#endif // _WX_QT_STATUSBAR_H_
