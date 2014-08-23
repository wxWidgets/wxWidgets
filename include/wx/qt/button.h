/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/button.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_BUTTON_H_
#define _WX_QT_BUTTON_H_

#include "wx/control.h"
#include "wx/button.h"
#include "wx/qt/pointer_qt.h"
#include "wx/qt/winevent_qt.h"
#include <QtWidgets/QPushButton>

class WXDLLIMPEXP_CORE wxQtPushButton : public wxQtEventSignalHandler< QPushButton, wxControl >
{

public:
    wxQtPushButton( wxWindow *parent, wxControl *handler );

    void SetToggleable();

    void SetValue( bool state );
    bool GetValue() const;

    void SetDefault();
    void SetLabel( const wxString &label );
    void SetBitmap( const wxBitmap &bitmap );

private:
    void OnButtonClicked( bool checked );
};

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

    bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxButtonNameStr);

    virtual wxWindow *SetDefault();
    virtual void SetLabel( const wxString &label );

    virtual QPushButton *GetHandle() const;

protected:
    wxQtPointer< wxQtPushButton > m_qtPushButton;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxButton);
};


#endif // _WX_QT_BUTTON_H_
