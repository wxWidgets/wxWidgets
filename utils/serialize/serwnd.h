/////////////////////////////////////////////////////////////////////////////
// Name:        serwnd.h
// Purpose:     Serialization: wxWindow classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     July 1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _SERIALIZE_SERWND_H_
#define _SERIALIZE_SERWND_H_

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/window.h>
#include <wx/serbase.h>

class WXSERIAL(wxWindow) : public WXSERIAL(wxObject)
{
  DECLARE_DYNAMIC_CLASS( wxWindow_Serialize )
 public:
  WXSERIAL(wxWindow)() { }
  virtual ~WXSERIAL(wxWindow)() { };

  void StoreObject(wxObjectOutputStream& s);
  void LoadObject(wxObjectInputStream& s);

 public:
  int m_x, m_y, m_w, m_h;
  bool m_shown, m_auto_layout;
  wxWindowID m_id;
  wxString m_name, m_title, m_label;
  wxWindow *m_parent;
  wxValidator *m_validator;
  wxColour m_bg_colour, m_fg_colour;
  long m_style;
};

DECLARE_SERIAL_CLASS(wxIndividualLayoutConstraint, wxObject)
DECLARE_SERIAL_CLASS(wxLayoutConstraints, wxObject)
DECLARE_ALIAS_SERIAL_CLASS(wxValidator, wxObject)
DECLARE_SERIAL_CLASS(wxFrame, wxWindow)
DECLARE_SERIAL_CLASS(wxPanel, wxWindow)
DECLARE_SERIAL_CLASS(wxDialog, wxWindow)
DECLARE_SERIAL_CLASS(wxMenuBar, wxWindow)
DECLARE_SERIAL_CLASS(wxMenuItem, wxObject)
DECLARE_SERIAL_CLASS(wxMenu, wxObject)
DECLARE_SERIAL_CLASS(wxMDIParentFrame, wxFrame)
DECLARE_SERIAL_CLASS(wxMDIChildFrame, wxFrame)
DECLARE_SERIAL_CLASS(wxMDIClientWindow, wxFrame)

#endif
