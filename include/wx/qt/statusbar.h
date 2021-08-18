/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/statusbar.h
// Author:      Peter Most, Javier Torres, Mariano Reingart, Sean D'Epagnier
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_STATUSBAR_H_
#define _WX_QT_STATUSBAR_H_

#include "wx/statusbr.h"

class QLabel;
class QStatusBar;

template < class T > class QList;

class WXDLLIMPEXP_CORE wxStatusBar : public wxStatusBarBase
{
public:
    wxStatusBar();
    wxStatusBar(wxWindow *parent, wxWindowID winid = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxStatusBarNameStr));

    bool Create(wxWindow *parent, wxWindowID winid = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxStatusBarNameStr));

    virtual bool GetFieldRect(int i, wxRect& rect) const wxOVERRIDE;
    virtual void SetMinHeight(int height) wxOVERRIDE;
    virtual int GetBorderX() const wxOVERRIDE;
    virtual int GetBorderY() const wxOVERRIDE;
    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = (const wxRect *) NULL ) wxOVERRIDE;

    QStatusBar *GetQStatusBar() const { return m_qtStatusBar; }
    QWidget *GetHandle() const wxOVERRIDE;

protected:
    virtual void DoUpdateStatusText(int number) wxOVERRIDE;

private:
    void Init();
    void UpdateFields();

    QStatusBar *m_qtStatusBar;
    wxVector<QLabel*> m_qtPanes;

    wxDECLARE_DYNAMIC_CLASS(wxStatusBar);
};


#endif // _WX_QT_STATUSBAR_H_
