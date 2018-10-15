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
    
    if( style & wxBU_LEFT )
        m_qtPushButton->setStyleSheet( "Text-align:left" );
    if( style & wxBU_RIGHT )
        m_qtPushButton->setStyleSheet( "Text-align:right" );
    if( style & wxBU_TOP )
        m_qtPushButton->setStyleSheet( "Text-align:top" );
    if( style & wxBU_BOTTOM )
        m_qtPushButton->setStyleSheet( "Text-align:bottom" );
    if( style & ( wxBU_LEFT | wxBU_RIGHT ) )
        m_qtPushButton->setStyleSheet( "Text-align:center" );
    if( style & ( wxBU_TOP | wxBU_BOTTOM ) )
        m_qtPushButton->setStyleSheet( "Text-align:center" );
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
