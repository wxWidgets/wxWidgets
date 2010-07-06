/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/statusbar.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_STATUSBAR_H_
#define _WX_QT_STATUSBAR_H_

#include "wx/qt/winevent_qt.h"

#include <QtGui/QLabel>
#include <QtGui/QStatusBar>

class WXDLLIMPEXP_CORE wxStatusBar : public wxStatusBarBase
{
public:
    wxStatusBar();
    ~wxStatusBar();
    wxStatusBar(wxWindow *parent, wxWindowID winid = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxStatusBarNameStr);
    bool Create(wxWindow *parent, wxWindowID winid = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxStatusBarNameStr);

    virtual bool GetFieldRect(int i, wxRect& rect) const;
    virtual void SetMinHeight(int height);
    virtual int GetBorderX() const;
    virtual int GetBorderY() const;
    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = (const wxRect *) NULL );

    virtual QStatusBar *GetHandle() const;
protected:
    virtual void DoUpdateStatusText(int number);

private:
    void Init();
    void UpdateFields();

    QPointer< QStatusBar > m_qtStatusBar;
    QList< QLabel* > m_qtPanes;

    DECLARE_DYNAMIC_CLASS( wxStatusBar )
};

class WXDLLIMPEXP_CORE wxQtStatusBar : public WindowEventForwarder< QStatusBar >
{
    public:
        wxQtStatusBar( wxStatusBar *statusBar, QWidget *parent );
        
    protected:
        virtual wxWindow *GetEventReceiver();
        virtual void resizeEvent ( QResizeEvent * event );
        
    private:
        wxStatusBar *m_statusBar;
        
};

#endif // _WX_QT_STATUSBAR_H_
