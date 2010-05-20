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
    return wxPoint( point.x(), point.y() );
}

QPoint wxQtConvertPoint( const wxPoint &point )
{
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
    if ( date.isValid() )
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
    return wxSize(size.width(), size.height());
}

QSize  wxQtConvertSize( const wxSize &size )
{
    return QSize(size.GetWidth(), size.GetHeight());
}
