///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private.h
// Purpose:     wxGTK private macros, functions &c
// Author:      Vadim Zeitlin
// Modified by:
// Created:     12.03.02
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_H_
#define _WX_GTK_PRIVATE_H_

#include <gtk/gtk.h>

#include "wx/event.h"
#include "wx/gtk/private/string.h"

// fail all version tests if the GTK+ version is so ancient that it doesn't
// even have GTK_CHECK_VERSION
#ifndef GTK_CHECK_VERSION
    #define GTK_CHECK_VERSION(a, b, c) 0
#endif

#if wxUSE_UNICODE
    #define wxGTK_CONV(s) wxConvUTF8.cWX2MB((s))
    #define wxGTK_CONV_ENC(s, enc) wxGTK_CONV((s))
    #define wxGTK_CONV_FONT(s, font) wxGTK_CONV((s))
    #define wxGTK_CONV_SYS(s) wxGTK_CONV((s))

    #define wxGTK_CONV_BACK(s) wxConvUTF8.cMB2WX((s))
    #define wxGTK_CONV_BACK_ENC(s, enc) wxGTK_CONV_BACK(s)
    #define wxGTK_CONV_BACK_FONT(s, font) wxGTK_CONV_BACK(s)
    #define wxGTK_CONV_BACK_SYS(s) wxGTK_CONV_BACK(s)
#else
    #include "wx/font.h"

    // convert the text between the given encoding and UTF-8 used by wxGTK
    extern wxCharBuffer
    wxConvertToGTK(const wxString& s,
                   wxFontEncoding enc = wxFONTENCODING_SYSTEM);

    extern wxCharBuffer
    wxConvertFromGTK(const wxString& s,
                     wxFontEncoding enc = wxFONTENCODING_SYSTEM);

    // helper: use the encoding of the given font if it's valid
    inline wxCharBuffer wxConvertToGTK(const wxString& s, const wxFont& font)
    {
        return wxConvertToGTK(s, font.Ok() ? font.GetEncoding()
                                           : wxFONTENCODING_SYSTEM);
    }

    inline wxCharBuffer wxConvertFromGTK(const wxString& s, const wxFont& font)
    {
        return wxConvertFromGTK(s, font.Ok() ? font.GetEncoding()
                                             : wxFONTENCODING_SYSTEM);
    }

    // more helpers: allow passing GTK+ strings directly
    inline wxCharBuffer
    wxConvertFromGTK(const wxGtkString& gs,
                     wxFontEncoding enc = wxFONTENCODING_SYSTEM)
    {
        return wxConvertFromGTK(gs.c_str(), enc);
    }

    inline wxCharBuffer
    wxConvertFromGTK(const wxGtkString& gs, const wxFont& font)
    {
        return wxConvertFromGTK(gs.c_str(), font);
    }

    #define wxGTK_CONV(s) wxGTK_CONV_FONT((s), m_font)
    #define wxGTK_CONV_ENC(s, enc) wxConvertToGTK((s), (enc))
    #define wxGTK_CONV_FONT(s, font) wxConvertToGTK((s), (font))
    #define wxGTK_CONV_SYS(s) wxConvertToGTK((s))

    #define wxGTK_CONV_BACK(s) wxConvertFromGTK((s), m_font)
    #define wxGTK_CONV_BACK_ENC(s, enc) wxConvertFromGTK((s), (enc))
    #define wxGTK_CONV_BACK_FONT(s, font) wxConvertFromGTK((s), (font))
    #define wxGTK_CONV_BACK_SYS(s) wxConvertFromGTK((s))
#endif

// Some deprecated GTK+ prototypes we still use often
// FIXME: Don't use them if possible.
G_BEGIN_DECLS

// Should use gtk_image_new, but the mask seems to be handled different,
// and we need to migrate
GtkWidget* gtk_pixmap_new (GdkPixmap *pixmap,
                           GdkBitmap *mask);

// Deprecated since GTK+-1.3.7:
// Trivial wrapper around gtk_window_move, with some side effects we seem to rely on
void gtk_widget_set_uposition (GtkWidget *widget,
                               gint      x,
                               gint      y);

// We rely on the allow_shrink parameter in one place
void gtk_window_set_policy (GtkWindow *window,
                            gint       allow_shrink,
                            gint       allow_grow,
                            gint       auto_shrink);

G_END_DECLS

//-----------------------------------------------------------------------------
// Misc. functions
//-----------------------------------------------------------------------------

// Needed for implementing e.g. combobox on wxGTK within a modal dialog.
void wxAddGrab(wxWindow* window);
void wxRemoveGrab(wxWindow* window);

// Escapes string so that it is valid Pango markup XML string:
WXDLLIMPEXP_CORE wxString wxEscapeStringForPangoMarkup(const wxString& str);

// The declaration for gtk_icon_size_lookup was accidentally ifdefed out in
// GTK+ 2.1.0 which Sun seem to have shipped with some versions of JDS
// for Solaris 9 x86.
#ifdef NEED_GTK_ICON_SIZE_LOOKUP
extern "C" gboolean gtk_icon_size_lookup  (GtkIconSize  size,
                                           gint         *width,
                                           gint         *height);
#endif

#ifdef __WXGTK20__
#include <gdk/gdktypes.h>

// Returns stock accelerator modifier and key code for the given ID
WXDLLEXPORT bool wxGetStockGtkAccelerator(const char *id, GdkModifierType *mod, guint *key);
#endif

#endif // _WX_GTK_PRIVATE_H_

