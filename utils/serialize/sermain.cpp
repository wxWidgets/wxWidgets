/////////////////////////////////////////////////////////////////////////////
// Name:        sermain.cpp
// Purpose:     Serialization: main
// Author:      Guilhem Lavaux
// Modified by:
// Created:     July 1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "serbase.h"
#endif

#include <wx/dynlib.h>
#include <wx/serbase.h>

#include "sercore.h"
#include "serwnd.h"
#include "sergdi.h"
#include "serctrl.h"

IMPLEMENT_DYNAMIC_CLASS(wxObject_Serialize, wxObject)

#define REGISTER_CLASS(classname) \
lib->RegisterClass(CLASSINFO(classname##_Serialize), #classname "_Serialize")

WXDLL_ENTRY_FUNCTION()
{
  wxClassLibrary *lib = new wxClassLibrary;

  REGISTER_CLASS(wxList);

  REGISTER_CLASS(wxWindow);
  REGISTER_CLASS(wxIndividualLayoutConstraint);
  REGISTER_CLASS(wxLayoutConstraints);
  REGISTER_CLASS(wxFrame);
//  REGISTER_CLASS(wxPanel);
//  REGISTER_CLASS(wxDialog);
  REGISTER_CLASS(wxMenu);
  REGISTER_CLASS(wxMenuItem);
  REGISTER_CLASS(wxMenuBar);

  REGISTER_CLASS(wxGDIObject);
  REGISTER_CLASS(wxBitmap);
  REGISTER_CLASS(wxColour);
  REGISTER_CLASS(wxFont);
  REGISTER_CLASS(wxPen);
  REGISTER_CLASS(wxBrush);
  REGISTER_CLASS(wxPenList);
  REGISTER_CLASS(wxBrushList);
  REGISTER_CLASS(wxFontList);
  REGISTER_CLASS(wxColourDatabase);
  REGISTER_CLASS(wxBitmapList);

  REGISTER_CLASS(wxControl);
  REGISTER_CLASS(wxSlider);
  REGISTER_CLASS(wxCheckBox);
  REGISTER_CLASS(wxChoice);
  REGISTER_CLASS(wxGauge);
  REGISTER_CLASS(wxListBox);
  REGISTER_CLASS(wxButton);
  REGISTER_CLASS(wxStaticText);
  REGISTER_CLASS(wxRadioBox);
  REGISTER_CLASS(wxComboBox);
  REGISTER_CLASS(wxNotebook);

  return lib;
}

WXDLL_EXIT_FUNCTION(lib)
{
  delete lib;
}
