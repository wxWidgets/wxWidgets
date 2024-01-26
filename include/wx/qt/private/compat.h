///////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/private/compat.h
// Purpose:     Helpers for dealing with various Qt versions
// Author:      Vadim Zeitlin
// Created:     2022-10-21
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_PRIVATE_COMPAT_H_
#define _WX_QT_PRIVATE_COMPAT_H_

#include <QtGui/QFontMetrics>

// Hide the difference in name of QFontMetrics::width() in various Qt versions.
inline int
wxQtGetWidthFromMetrics(const QFontMetrics& metrics, const QString& string)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    return metrics.horizontalAdvance( string );
#else
    return metrics.width( string );
#endif
}

#endif // _WX_QT_PRIVATE_COMPAT_H_
