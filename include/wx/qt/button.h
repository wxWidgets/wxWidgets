/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/button.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_BUTTON_H_
#define _WX_QT_BUTTON_H_

#include "wx/qt/winevent_qt.h"
#include "wx/button.h"
#include "wx/qt/pointer_qt.h"
#include <QtGui/QPushButton>

class WXDLLIMPEXP_CORE wxButton : public wxButtonBase
{
public:
    wxButton();
    wxButton(wxWindow *parent, wxWindowID id,
           const wxString& label = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxButtonNameStr);

    ~wxButton();

    bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxButtonNameStr);

    virtual wxWindow *SetDefault();

    virtual QPushButton *GetHandle() const;

private:
    wxQtPointer< QPushButton > m_qtPushButton;
};


class WXDLLIMPEXP_CORE wxQtButton : public QPushButton, public wxQtSignalForwarder< wxButton >
{
    Q_OBJECT

public:
    wxQtButton( wxButton *button, const QString &text, QWidget *parent );

private Q_SLOTS:
    void OnButtonClicked( bool checked );
};

#endif // _WX_QT_BUTTON_H_
