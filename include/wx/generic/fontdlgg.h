/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlgg.h
// Purpose:     wxGenericFontDialog
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __FONTDLGH_G__
#define __FONTDLGH_G__

#ifdef __GNUG__
#pragma interface "fontdlgg.h"
#endif

#include "wx/setup.h"
#include "wx/gdicmn.h"
#include "wx/font.h"
#include "wx/dialog.h"
#include "wx/cmndata.h"

/*
 * FONT DIALOG
 */
 
class WXDLLEXPORT wxChoice;
class WXDLLEXPORT wxText;
class WXDLLEXPORT wxCheckBox;
class WXDLLEXPORT wxFontPreviewer;

#define wxID_FONT_UNDERLINE 3000
#define wxID_FONT_STYLE     3001
#define wxID_FONT_WEIGHT    3002
#define wxID_FONT_FAMILY    3003
#define wxID_FONT_COLOUR    3004
#define wxID_FONT_SIZE      3005

class WXDLLEXPORT wxGenericFontDialog: public wxDialog
{
 DECLARE_DYNAMIC_CLASS(wxGenericFontDialog)
 protected:
  wxFontData fontData;
  wxFont dialogFont;
  wxWindow *dialogParent;

  wxChoice *familyChoice;
  wxChoice *styleChoice;
  wxChoice *weightChoice;
  wxChoice *colourChoice;
  wxCheckBox *underLineCheckBox;
  wxChoice   *pointSizeChoice;
  wxFontPreviewer *m_previewer;
  bool       m_useEvents;

//  static bool fontDialogCancelled;
 public:
 
  wxGenericFontDialog(void);
  wxGenericFontDialog(wxWindow *parent, const wxFontData& data);
  ~wxGenericFontDialog(void);

  bool Create(wxWindow *parent, const wxFontData& data);

  int ShowModal(void);

  inline wxFontData& GetFontData(void) { return fontData; }

  // Internal functions
  void OnCloseWindow(wxCloseEvent& event);

  virtual void CreateWidgets(void);
  virtual void InitializeFont(void);
  
  void OnChangeFont(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

WXDLLEXPORT const wxChar *wxFontFamilyIntToString(int family);
WXDLLEXPORT const wxChar *wxFontWeightIntToString(int weight);
WXDLLEXPORT const wxChar *wxFontStyleIntToString(int style);
WXDLLEXPORT int wxFontFamilyStringToInt(wxChar *family);
WXDLLEXPORT int wxFontWeightStringToInt(wxChar *weight);
WXDLLEXPORT int wxFontStyleStringToInt(wxChar *style);

#endif
