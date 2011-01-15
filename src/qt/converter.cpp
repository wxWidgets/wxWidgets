/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/converter.cpp
// Author:      Peter Most, Kolya Kosenko
// Id:          $Id$
// Copyright:   (c) Peter Most
//              (c) 2010 Kolya Kosenko
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/gdicmn.h"
#include "wx/gdicmn.h"

#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtGui/QFont>
#include <QtCore/QSize>

#if wxUSE_DATETIME
    #include "wx/datetime.h"
    #include <QtCore/QDate>
#endif // wxUSE_DATETIME

wxPoint wxQtConvertPoint( const QPoint &point )
{
    if (point.isNull())
        return wxDefaultPosition;

    return wxPoint( point.x(), point.y() );
}

QPoint wxQtConvertPoint( const wxPoint &point )
{
    if (point == wxDefaultPosition)
        return QPoint();

    return QPoint( point.x, point.y );
}


QRect wxQtConvertRect( const wxRect &rect )
{
    return QRect( rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight() );
}

wxRect wxQtConvertRect( const QRect &rect )
{
    return wxRect( rect.x(), rect.y(), rect.width(), rect.height() );
}

// TODO: Check whether QString::toStdString/QString::toStdWString might be faster

wxString wxQtConvertString( const QString &str )
{
    return wxString( qPrintable( str ));
}

QString  wxQtConvertString( const wxString &str )
{
    return QString( str.c_str() );
}




#if wxUSE_DATETIME

wxDateTime wxQtConvertDate(const QDate& date)
{
    if ( date.isNull() )
        return wxDateTime(date.day(),
            static_cast<wxDateTime::Month>(date.month() - 1),
            date.year(), 0, 0, 0, 0);
    else
        return wxDateTime();
}

QDate wxQtConvertDate(const wxDateTime& date)
{
    if ( date.IsValid() )
        return QDate(date.GetYear(), date.GetMonth() + 1, date.GetDay());
    else
        return QDate();
}

#endif // wxUSE_DATETIME

wxSize wxQtConvertSize( const QSize  &size )
{
    if (size.isNull())
        return wxDefaultSize;

    return wxSize(size.width(), size.height());
}

QSize  wxQtConvertSize( const wxSize &size )
{
    if (size == wxDefaultSize)
        return QSize();

    return QSize(size.GetWidth(), size.GetHeight());
}

Qt::Orientation wxQtConvertOrientation( long style, wxOrientation defaultOrientation )
{
    if (( style & ( wxHORIZONTAL | wxVERTICAL )) == 0 )
        style |= defaultOrientation;

    switch ( style & ( wxHORIZONTAL | wxVERTICAL ))
    {
        case wxHORIZONTAL:
            return Qt::Horizontal;

        case wxVERTICAL:
            return Qt::Vertical;
    }
    wxFAIL_MSG( "Unreachable code!" );
    return static_cast< Qt::Orientation >( -1 );
}


wxOrientation wxQtConvertOrientation( Qt::Orientation qtOrientation )
{
    switch ( qtOrientation )
    {
        case Qt::Horizontal:
            return wxHORIZONTAL;

        case Qt::Vertical:
            return wxVERTICAL;
    }
    wxFAIL_MSG( "Unreachable code!" );
    return static_cast< wxOrientation >( -1 );
}

