/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.h
// Purpose:     wxFontDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __FONTDLGH__
#define __FONTDLGH__

#ifdef __GNUG__
#pragma interface "fontdlg.h"
#endif

#include "wx/dialog.h"
#include "wx/cmndata.h"

/*
 * FONT DIALOG
 */
 
class WXDLLEXPORT wxFontDialog: public wxDialog
{
 DECLARE_DYNAMIC_CLASS(wxFontDialog)
 protected:
  wxWindow *dialogParent;
  wxFontData fontData;
 public:
  wxFontDialog(void);
  wxFontDialog(wxWindow *parent, wxFontData *data = NULL);

  bool Create(wxWindow *parent, wxFontData *data = NULL);

  int ShowModal(void);
  wxFontData& GetFontData(void) { return fontData; }
};

#endif
    // __FONTDLGH__

