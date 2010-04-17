///////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/converter.h
// Purpose:     Converter utility classes
// Author:      Peter Most
// Created:     02/28/10
// RCS-ID:      $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CONVERTER_H_
#define _WX_QT_CONVERTER_H_

#include <QtCore/QRect>
#include <QtCore/QString>

#include <QtGui/QFont>

class wxQtRect : public QRect
{
    public:
        wxQtRect( const wxRect &rect )
            : QRect( rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight() )
        { }
};

class wxQtString : public QString
{
public:
    wxQtString( const wxString &str )
        : QString( str.c_str() )
    { }
    
    wxQtString( const QString &str )
        : QString( str )
    { }
        
    operator wxString () const
    {
        return ( wxString( qPrintable( *this )));
    }
};

inline QFont wxFontToQFont( const wxFont & )
{
    return QFont();
}


#endif // _WX_QT_CONVERTER_H_

