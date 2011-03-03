/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/button_qt.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_BUTTON_QT_H_
#define _WX_QT_BUTTON_QT_H_

#include "wx/control.h"
#include "wx/qt/winevent_qt.h"
#include <QtGui/QPushButton>

class WXDLLIMPEXP_CORE wxQtPushButton : public wxQtEventSignalHandler< QPushButton, wxControl >
{
    Q_OBJECT

public:
    wxQtPushButton( wxWindow *parent, wxControl *handler );

    void SetToggleable();

    void SetValue( bool state );
    bool GetValue() const;

    void SetDefault();
    void SetLabel( const wxString &label );
    void SetBitmap( const wxBitmap &bitmap );

private Q_SLOTS:
    void OnButtonClicked( bool checked );
};
#endif // _WX_QT_BUTTON_QT_H_
