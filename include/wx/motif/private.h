/////////////////////////////////////////////////////////////////////////////
// Name:        private.h
// Purpose:     Private declarations for wxMotif port
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_H_
#define _WX_PRIVATE_H_

#include "wx/defs.h"

class wxMouseEvent;
class wxKeyEvent;

// Put any private declarations here: native Motif types may be used because
// this header is included after Xm/Xm.h

// ----------------------------------------------------------------------------
// common callbacks
// ----------------------------------------------------------------------------

// All widgets should have this as their resize proc.
extern void wxWidgetResizeProc(Widget w, XConfigureEvent *event, String args[], int *num_args);

// For repainting arbitrary windows
void wxUniversalRepaintProc(Widget w, XtPointer WXUNUSED(c_data), XEvent *event, char *);

// ----------------------------------------------------------------------------
// we maintain a hash table which contains the mapping from Widget to wxWindow
// corresponding to the window for this widget
// ----------------------------------------------------------------------------

extern void wxDeleteWindowFromTable(Widget w);
extern wxWindow *wxGetWindowFromTable(Widget w);
extern bool wxAddWindowToTable(Widget w, wxWindow *win);

// ----------------------------------------------------------------------------
// key events related functions
// ----------------------------------------------------------------------------

extern char wxFindMnemonic(const char* s);

extern char * wxFindAccelerator (const char *s);
extern XmString wxFindAcceleratorText (const char *s);

extern int wxCharCodeXToWX(KeySym keySym);
extern KeySym wxCharCodeWXToX(int id);

// ----------------------------------------------------------------------------
// TranslateXXXEvent() functions - translate Motif event to wxWindow one
// ----------------------------------------------------------------------------
extern bool wxTranslateMouseEvent(wxMouseEvent& wxevent, wxWindow *win, Widget widget, XEvent *xevent);
extern bool wxTranslateKeyEvent(wxKeyEvent& wxevent, wxWindow *win, Widget widget, XEvent *xevent);

int wxGetBestMatchingPixel(Display *display, XColor *desiredColor, Colormap cmap);
Pixmap XCreateInsensitivePixmap( Display *display, Pixmap pixmap );

extern XColor g_itemColors[];
extern int wxComputeColours (Display *display, wxColour * back, wxColour * fore);

extern void wxDoChangeForegroundColour(WXWidget widget, wxColour& foregroundColour);
extern void wxDoChangeBackgroundColour(WXWidget widget, wxColour& backgroundColour, bool changeArmColour = FALSE);

#define wxNO_COLORS   0x00
#define wxBACK_COLORS 0x01
#define wxFORE_COLORS 0x02

extern XColor itemColors[5] ;

#define wxBACK_INDEX 0
#define wxFORE_INDEX 1
#define wxSELE_INDEX 2
#define wxTOPS_INDEX 3
#define wxBOTS_INDEX 4

// ----------------------------------------------------------------------------
// utility classes
// ----------------------------------------------------------------------------

// XmString made easy to use in wxWindows (and has an added benefit of
// cleaning up automatically)
class wxXmString
{
public:
    wxXmString(const wxString& str)
    {
        m_string = XmStringCreateLtoR((char *)str.c_str(),
                                      XmSTRING_DEFAULT_CHARSET);
    }

    ~wxXmString() { XmStringFree(m_string); }

    // semi-implicit conversion to XmString (shouldn't rely on implicit
    // conversion because many of Motif functions are macros)
    XmString operator()() const { return m_string; }

private:
    XmString m_string;
};

// ----------------------------------------------------------------------------
// macros to avoid casting WXFOO to Foo all the time
// ----------------------------------------------------------------------------

// argument is of type "wxWindow *"
#define GetWidget(w)    ((Widget)(w)->GetHandle())

#endif
    // _WX_PRIVATE_H_
