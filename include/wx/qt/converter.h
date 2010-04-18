///////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/converter.h
// Purpose:     Converter utility classes and functions
// Author:      Peter Most
// Created:     02/28/10
// RCS-ID:      $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CONVERTER_H_
#define _WX_QT_CONVERTER_H_

// Rely on overloading and let the compiler pick the correct version, which makes
// them easier to use then to write wxQtConvertQtRectToWxRect() or wxQtConvertWxRectToQtRect()

class wxRect;
class QRect;
wxRect wxQtConvertRect( const QRect &rect );
QRect  wxQtConvertRect( const wxRect &rect );

class wxString;
class QString;
wxString wxQtConvertString( const QString &str );
QString  wxQtConvertString( const wxString &str );

class wxFont;
class QFont;
QFont wxQtConvertFont( const wxFont &font );
#endif // _WX_QT_CONVERTER_H_

