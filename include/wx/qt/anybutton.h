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
    wxAnyButton() = default;

    // implementation
    // --------------

    virtual void SetLabel( const wxString &label ) override;
    virtual wxString GetLabel() const override;

    // implementation only
    void QtUpdateState();
    virtual int QtGetEventType() const = 0;

    QPushButton* GetQPushButton() const;

protected:
    virtual wxBitmap DoGetBitmap(State state) const override;
    virtual void DoSetBitmap(const wxBitmapBundle& bitmap, State which) override;

    void QtCreate(wxWindow *parent);
    void QtSetBitmap( const wxBitmapBundle &bitmap );

private:
    State QtGetCurrentState() const;

    typedef wxAnyButtonBase base_type;
    wxBitmapBundle m_bitmaps[State_Max];

    wxDECLARE_NO_COPY_CLASS(wxAnyButton);
};


#endif // _WX_QT_ANYBUTTON_H_
