/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/button.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
#endif // WX_PRECOMP

#include "wx/button.h"
#include "wx/stockitem.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QPushButton>

wxButton::wxButton()
{
}

wxButton::wxButton(wxWindow *parent, wxWindowID id,
       const wxString& label,
       const wxPoint& pos,
       const wxSize& size, long style,
       const wxValidator& validator,
       const wxString& name )
{
    Create( parent, id, label, pos, size, style, validator, name );
}

bool wxButton::Create(wxWindow *parent, wxWindowID id,
       const wxString& label,
       const wxPoint& pos,
       const wxSize& size, long style,
       const wxValidator& validator,
       const wxString& name )
{     
    QtCreate(parent);
    SetLabel( wxIsStockID( id ) ? wxGetStockLabel( id ) : label );

    QString alignment = "text-align: ";
    QString xalign = "center ";
    if ( style & wxBU_LEFT )
        xalign = "left ";
    else if ( style & wxBU_RIGHT )
        xalign = "right ";
    QString yalign = "center";
    if ( style & wxBU_TOP )
        yalign = "top";
    else if ( style & wxBU_BOTTOM )
        yalign = "bottom";
    m_qtPushButton->setStyleSheet( alignment + xalign + yalign );

    if( style & wxBORDER_NONE )
        m_qtPushButton->setFlat( true );

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

wxWindow *wxButton::SetDefault()
{
    wxWindow *oldDefault = wxButtonBase::SetDefault();

    m_qtPushButton->setDefault( true );

    return oldDefault;

}

/* static */
wxSize wxButtonBase::GetDefaultSize()
{
    static wxSize size = wxDefaultSize;
    if (size == wxDefaultSize)
    {
        // Default size of buttons should be same as size of stock
        // buttons as used in most GTK+ apps. (currently this is aproximate)
        QPushButton btn;
        size = wxQtConvertSize(btn.sizeHint());
    }
    return size;
}
