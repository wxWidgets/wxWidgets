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

#endif
    // _WX_PRIVATE_H_
