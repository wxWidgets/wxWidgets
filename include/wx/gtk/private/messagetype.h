///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/messagetype.h
// Purpose:     translate between wx and GtkMessageType
// Author:      Vadim Zeitlin
// Created:     2009-09-27
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _GTK_PRIVATE_MSGTYPE_H_
#define _GTK_PRIVATE_MSGTYPE_H_

#include <gtk/gtk.h>

#include "wx/gtk/private/gtk2-compat.h"

namespace wxGTKImpl
{

// Convert the given wx style to GtkMessageType, return true if succeeded or
// false if failed.
inline bool ConvertMessageTypeFromWX(int style, GtkMessageType *type)
{
#ifdef __WXGTK210__
    if ( wx_is_at_least_gtk2(10) && (style & wxICON_NONE))
        *type = GTK_MESSAGE_OTHER;
    else
#endif // __WXGTK210__
    if (style & wxICON_EXCLAMATION)
        *type = GTK_MESSAGE_WARNING;
    else if (style & wxICON_ERROR)
        *type = GTK_MESSAGE_ERROR;
    else if (style & wxICON_INFORMATION)
        *type = GTK_MESSAGE_INFO;
    else if (style & wxICON_QUESTION)
        *type = GTK_MESSAGE_QUESTION;
    else
        return false;

    return true;
}

} // namespace wxGTKImpl

#endif // _GTK_PRIVATE_MSGTYPE_H_

