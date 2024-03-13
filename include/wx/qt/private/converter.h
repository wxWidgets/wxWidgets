///////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/converter.h
// Purpose:     Converter utility classes and functions
// Author:      Peter Most, Kolya Kosenko
// Created:     02/28/10
// Copyright:   (c) Peter Most
//              (c) 2010 Kolya Kosenko
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CONVERTER_H_
#define _WX_QT_CONVERTER_H_

#include "wx/defs.h"

#include "wx/kbdstate.h"
#include "wx/gdicmn.h"
#include "wx/colour.h"

#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtGui/QColor>

// Rely on overloading and let the compiler pick the correct version, which makes
// them easier to use then to write wxQtConvertQtRectToWxRect() or wxQtConvertWxRectToQtRect()

inline wxPoint wxQtConvertPoint( const QPoint &point )
{
    return wxPoint( point.x(), point.y() );
}
inline QPoint wxQtConvertPoint( const wxPoint &point )
{
    return QPoint( point.x, point.y );
}

inline wxRect wxQtConvertRect( const QRect &rect )
{
    return wxRect( rect.x(), rect.y(), rect.width(), rect.height() );
}

inline QRect wxQtConvertRect( const wxRect &rect )
{
    return QRect( rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight() );
}

// TODO: Check whether QString::toStdString/QString::toStdWString might be faster

inline wxString wxQtConvertString( const QString &str )
{
    return wxString( str.toUtf8().data(), wxConvUTF8 );
}

inline QString wxQtConvertString( const wxString &str )
{
    return QString( str.utf8_str() );
}

inline wxColour wxQtConvertColour(const QColor &colour)
{
    return wxColour(colour.red(), colour.green(), colour.blue(), colour.alpha());
}

inline QColor wxQtConvertColour(const wxColour &colour)
{
    return QColor(colour.Red(), colour.Green(), colour.Blue(), colour.Alpha());
}

#if wxUSE_DATETIME

class WXDLLIMPEXP_FWD_BASE wxDateTime;
class QDate;
class QTime;

wxDateTime wxQtConvertDate(const QDate& date);
QDate wxQtConvertDate(const wxDateTime& date);

wxDateTime wxQtConvertTime(const QTime& Time);
QTime wxQtConvertTime(const wxDateTime& time);

#endif // wxUSE_DATETIME

inline wxSize wxQtConvertSize( const QSize  &size )
{
    return wxSize(size.width(), size.height());
}
inline QSize wxQtConvertSize( const wxSize &size )
{
    return QSize(size.GetWidth(), size.GetHeight());
}

Qt::Orientation wxQtConvertOrientation( long style, wxOrientation defaultOrientation );
wxOrientation wxQtConvertOrientation( Qt::Orientation );

wxKeyCode wxQtConvertKeyCode( int key, Qt::KeyboardModifiers modifiers );
void wxQtFillKeyboardModifiers( Qt::KeyboardModifiers modifiers, wxKeyboardState *state );
int wxQtConvertKeyCode( int keyCode, int modifiers, Qt::KeyboardModifiers &qtmodifiers );

#endif // _WX_QT_CONVERTER_H_

