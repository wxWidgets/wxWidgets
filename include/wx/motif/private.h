/////////////////////////////////////////////////////////////////////////////
// Name:        private.h
// Purpose:     Private declarations
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_H_
#define _WX_PRIVATE_H_

#include "wx/defs.h"

class WXDLLEXPORT wxMouseEvent;
class WXDLLEXPORT wxKeyEvent;

/* Put any private declarations here.
 */

extern void wxWidgetResizeProc(Widget w, XConfigureEvent *event, String args[], int *num_args);

extern wxHashTable *wxWidgetHashTable;
extern void wxDeleteWindowFromTable(Widget w);
extern wxWindow *wxGetWindowFromTable(Widget w);
extern bool wxAddWindowToTable(Widget w, wxWindow *win);
extern char wxFindMnemonic(const char* s);
extern char * wxFindAccelerator (char *s);
extern XmString wxFindAcceleratorText (char *s);
extern int wxCharCodeXToWX(KeySym keySym);
extern KeySym wxCharCodeWXToX(int id);
bool wxTranslateMouseEvent(wxMouseEvent& wxevent, wxWindow *win, Widget widget, XEvent *xevent);
bool wxTranslateKeyEvent(wxKeyEvent& wxevent, wxWindow *win, Widget widget, XEvent *xevent);
int wxGetBestMatchingPixel(Display *display, XColor *desiredColor, Colormap cmap);
Pixmap XCreateInsensitivePixmap( Display *display, Pixmap pixmap );
extern XColor g_itemColors[];
extern int wxComputeColours (Display *display, wxColour * back, wxColour * fore);

#define	wxNO_COLORS   0x00
#define wxBACK_COLORS 0x01
#define wxFORE_COLORS 0x02

extern XColor itemColors[5] ;

#define wxBACK_INDEX 0
#define wxFORE_INDEX 1
#define wxSELE_INDEX 2
#define wxTOPS_INDEX 3
#define wxBOTS_INDEX 4

#endif
    // _WX_PRIVATE_H_
