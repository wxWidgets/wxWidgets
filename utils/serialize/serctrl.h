#ifndef __SERCTRL_H__
#define __SERCTRL_H__

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/serbase.h>
#include "serwnd.h"

DECLARE_ALIAS_SERIAL_CLASS(wxControl, wxWindow)
DECLARE_SERIAL_CLASS(wxButton, wxControl)
DECLARE_SERIAL_CLASS(wxSlider, wxControl)
DECLARE_SERIAL_CLASS(wxCheckBox, wxControl)
DECLARE_SERIAL_CLASS(wxChoice, wxControl)
DECLARE_SERIAL_CLASS(wxComboBox, wxControl)
DECLARE_SERIAL_CLASS(wxGauge, wxControl)
DECLARE_SERIAL_CLASS(wxListBox, wxControl)
DECLARE_SERIAL_CLASS(wxNotebook, wxControl)
DECLARE_SERIAL_CLASS(wxRadioBox, wxControl)
DECLARE_SERIAL_CLASS(wxStaticText, wxControl)

#endif
