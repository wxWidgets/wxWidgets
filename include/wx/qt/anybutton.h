/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/anybutton.h
// Purpose:     wxQT wxAnyButton class declaration
// Author:      Mariano Reingart
// Copyright:   (c) 2014 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_ANYBUTTON_H_
#define _WX_QT_ANYBUTTON_H_

class QPushButton;

//-----------------------------------------------------------------------------
// wxAnyButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxAnyButton : public wxAnyButtonBase
{
public:
    wxAnyButton();

    // implementation
    // --------------

    virtual void SetLabel( const wxString &label ) wxOVERRIDE;

    virtual QWidget *GetHandle() const wxOVERRIDE;

    // implementation only
    void QtUpdateState();
    virtual int GetEventType() const = 0;

protected:
    virtual wxBitmap DoGetBitmap(State state) const wxOVERRIDE;
    virtual void DoSetBitmap(const wxBitmap& bitmap, State which) wxOVERRIDE;

    QPushButton *m_qtPushButton;

    void QtCreate(wxWindow *parent);
    void QtSetBitmap( const wxBitmap &bitmap );

private:
    State QtGetCurrentState() const;

    typedef wxAnyButtonBase base_type;
    wxBitmap  m_bitmaps[State_Max];

    wxDECLARE_NO_COPY_CLASS(wxAnyButton);
};


#endif // _WX_QT_ANYBUTTON_H_
