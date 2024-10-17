/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/button.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
#endif // WX_PRECOMP

#include "wx/button.h"
#include "wx/stockitem.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>

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
    SetLabel( label.IsEmpty() && wxIsStockID( id ) ? wxGetStockLabel( id ) : label );

    return wxButtonBase::Create( parent, id, pos, size, style, validator, name );
}

wxWindow *wxButton::SetDefault()
{
    wxWindow *oldDefault = wxButtonBase::SetDefault();

    GetQPushButton()->setDefault( true );

    return oldDefault;

}

// ----------------------------------------------------------------------------
// authentication needed handling
// ----------------------------------------------------------------------------

bool wxButton::DoGetAuthNeeded() const
{
    return m_authNeeded;
}

void wxButton::DoSetAuthNeeded(bool show)
{
    QIcon icon;

    if ( show )
    {
        icon = QApplication::style()->standardIcon(QStyle::SP_VistaShield);
    }

    m_authNeeded = !icon.isNull();

    GetQPushButton()->setIcon(icon);
    InvalidateBestSize();
}

/* static */
wxSize wxButtonBase::GetDefaultSize(wxWindow* WXUNUSED(win))
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
