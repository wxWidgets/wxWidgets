/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/anybutton.cpp
// Author:      Mariano Reingart
// Copyright:   (c) 2014 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef wxHAS_ANY_BUTTON

#ifndef WX_PRECOMP
    #include "wx/anybutton.h"
#endif

#include <QtWidgets/QPushButton>

#include "wx/bitmap.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/winevent.h"

class wxQtPushButton : public wxQtEventSignalHandler< QPushButton, wxAnyButton >
{

public:
    wxQtPushButton( wxWindow *parent, wxAnyButton *handler);

private:
    void clicked(bool);
};

wxQtPushButton::wxQtPushButton(wxWindow *parent, wxAnyButton *handler)
    : wxQtEventSignalHandler< QPushButton, wxAnyButton >( parent, handler )
{
    connect(this, &QPushButton::clicked, this, &wxQtPushButton::clicked);
}

void wxQtPushButton::clicked( bool WXUNUSED(checked) )
{
    wxAnyButton *handler = GetHandler();
    if ( handler )
    {
        wxCommandEvent event( wxEVT_BUTTON, handler->GetId() );
        EmitEvent( event );
    }
}

wxAnyButton::wxAnyButton() :
    m_qtPushButton(NULL)
{
}


void wxAnyButton::QtCreate(wxWindow *parent)
{
    // create the default push button (used in button and bmp button)
    m_qtPushButton = new wxQtPushButton( parent, this );
}

void wxAnyButton::QtSetBitmap( const wxBitmap &bitmap )
{
    // load the bitmap and resize the button:
    QPixmap *pixmap = bitmap.GetHandle();
    if ( pixmap != NULL )
    {
        m_qtPushButton->setIcon(QIcon(*pixmap));
        m_qtPushButton->setIconSize(pixmap->rect().size());
    }

    m_bitmap = bitmap;
}

void wxAnyButton::SetLabel( const wxString &label )
{
    m_qtPushButton->setText( wxQtConvertString( label ));
}

QWidget *wxAnyButton::GetHandle() const
{
    return m_qtPushButton;
}

wxBitmap wxAnyButton::DoGetBitmap(State state) const
{
    return state == State_Normal ? m_bitmap : wxNullBitmap;
}

void wxAnyButton::DoSetBitmap(const wxBitmap& bitmap, State which)
{
    switch ( which )
    {
        case State_Normal:
            QtSetBitmap(bitmap);
            InvalidateBestSize();
            break;

        case State_Pressed:
            wxMISSING_IMPLEMENTATION( wxSTRINGIZE( State_Pressed ));
            break;

        case State_Current:
            wxMISSING_IMPLEMENTATION( wxSTRINGIZE( State_Current ));
            break;

        case State_Focused:
            wxMISSING_IMPLEMENTATION( wxSTRINGIZE( State_Focused ));
            break;

        case State_Disabled:
            wxMISSING_IMPLEMENTATION( wxSTRINGIZE( State_Disabled ));
            break;

        case State_Max:
            wxMISSING_IMPLEMENTATION( wxSTRINGIZE( State_Max ));

    }
}

#endif // wxHAS_ANY_BUTTON
