/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/anybutton.h
// Purpose:     wxQT wxAnyButton class declaration
// Author:      Mariano Reingart
// Copyright:   (c) 2014 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_ANYBUTTON_H_
#define _WX_QT_ANYBUTTON_H_

#include <QtWidgets/QPushButton>

//-----------------------------------------------------------------------------
// wxAnyButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxAnyButton : public wxAnyButtonBase
{
public:
    wxAnyButton()
    {
    }

    // implementation
    // --------------

    virtual void SetLabel( const wxString &label );
    virtual void DoSetBitmap(const wxBitmap& bitmap, State which);

    virtual QPushButton *GetHandle() const;

protected:

    wxQtPointer< QPushButton > m_qtPushButton;

    void QtSetBitmap( const wxBitmap &bitmap );

private:
    typedef wxAnyButtonBase base_type;

    wxDECLARE_NO_COPY_CLASS(wxAnyButton);
};


class WXDLLIMPEXP_CORE wxQtPushButton : public wxQtEventSignalHandler< QPushButton, wxAnyButton >
{

public:
    wxQtPushButton( wxWindow *parent, wxAnyButton *handler, wxEventType eventType );

private:
    void OnButtonClicked( bool checked );
    wxEventType m_eventType;
};


#endif // _WX_QT_ANYBUTTON_H_
