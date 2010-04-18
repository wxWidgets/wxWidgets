/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/converter.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/gdicmn.h"
#include "wx/string.h"

#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtGui/QFont>

QRect wxQtConvertRect( const wxRect &rect )
    { return QRect( rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight() ); }



wxString wxQtConvertString( const QString &str )
    { return wxString( qPrintable( str )); }

QString  wxQtConvertString( const wxString &str )
    { return QString( str.c_str() ); }



QFont wxQtConvertFont( const wxFont &font )
    { return QFont(); }
