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
    wxAnyButton()
    {
    }

    // implementation
    // --------------

    virtual void SetLabel( const wxString &label );
    virtual void DoSetBitmap(State which);

    virtual QWidget *GetHandle() const;
    wxBitmap *GetStateBitmaps() { return m_bitmaps; };

protected:

    QPushButton *m_qtPushButton;

    void QtCreate(wxWindow *parent);
    void QtSetBitmap( const wxBitmap &bitmap );

private:
    typedef wxAnyButtonBase base_type;

    static bool IsStateValid(State state);
    wxBitmap DoGetBitmap(State state) const;

    // the bitmaps for the different state of the buttons, all of them may be
    // invalid and the button only shows a bitmap at all if State_Normal bitmap
    // is valid
    wxBitmap m_bitmaps[State_Max];

    
    wxDECLARE_NO_COPY_CLASS(wxAnyButton);
};


#endif // _WX_QT_ANYBUTTON_H_
