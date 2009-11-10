///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/gdkconv.h
// Purpose:     Helper functions for converting between GDK and wx types
// Author:      Vadim Zeitlin
// Created:     2009-11-10
// RCS-ID:      $Id: wxhead.h,v 1.11 2009-06-29 10:23:04 zeitlin Exp $
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _GTK_PRIVATE_GDKCONV_H_
#define _GTK_PRIVATE_GDKCONV_H_

namespace wxGTKImpl
{

inline wxRect wxRectFromGDKRect(const GdkRectangle *r)
{
    return wxRect(r->x, r->y, r->width, r->height);
}

} // namespace wxGTKImpl

#endif // _GTK_PRIVATE_GDKCONV_H_

