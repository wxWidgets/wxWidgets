/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_keyboard_n.mm
// Purpose:     wxMoAlphabeticKeyboard and other input-related classes
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

/*

TODO:

  - make it a popup window that doesn't grab focus
  - have special key-like buttons that also don't grab focus; also
    can toggle.
  - add data to buttons for the text to insert; wire up caps lock/symbol action
  - caps
  - numeric keyboard
  - symbols
  - eventually, choice of international characters as per real UI when lingering on the key
  - API for showing/dismissing keyboard. See how Cocoa Touch actually does automatic
    keyboard showing. In OnIdle, may need to cast to a base control class that has appropriate
    properties for controlling this.
  - Set all button sizes according to current orientation, and relayout.
  - scroll to show the currently edited control. How is this normally done?

  If current focus is text control, insert character directly.
  Otherwise, send an ascii keyboard event.

  */

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mobile/keyboard.h"
#include "wx/mobile/utils.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/textctrl.h"
    #include "wx/brush.h"
    #include "wx/panel.h"
    #include "wx/sizer.h"
    #include "wx/bmpbuttn.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
    #include "wx/dcclient.h"
    #include "wx/toplevel.h"
    #include "wx/msgdlg.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxMoKeyboard, wxFrame)

BEGIN_EVENT_TABLE(wxMoKeyboard, wxFrame)
    EVT_SIZE(wxMoKeyboard::OnSize)
END_EVENT_TABLE()

wxMoKeyboard* wxMoKeyboard::sm_keyboard = NULL;

bool wxMoKeyboard::Create(wxWindow *parent)
{
    // FIXME stub
    
    return true;
}

wxMoKeyboard::~wxMoKeyboard()
{
    // FIXME stub
}

void wxMoKeyboard::Init()
{
    // FIXME stub
}

void wxMoKeyboard::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME stub
}

// Shows the given keyboard by name - static function.
void wxMoKeyboard::ShowKeyboard(const wxString& name)
{
    // FIXME stub
}

// Hides the keyboard - static function.
void wxMoKeyboard::HideKeyboard()
{
    // FIXME stub
}

// Destroys the keyboard - static function.
void wxMoKeyboard::DestroyKeyboard()
{
    // FIXME stub
}

void wxMoKeyboard::DoShowKeyboard(const wxString& name)
{
    // FIXME stub
}

void wxMoKeyboard::DoShowKeyboard(wxWindow* win)
{
    // FIXME stub
}

wxWindow* wxMoKeyboard::GetWindowForName(const wxString& name) const
{
    // FIXME stub
    
    return NULL;
}

wxString wxMoKeyboard::GetNameForWindow(wxWindow* win) const
{
    // FIXME stub

    return wxEmptyString;
}

void wxMoKeyboard::AddKeyboard(const wxString& name, wxWindow* window)
{
    // FIXME stub
}

void wxMoKeyboard::FitCurrentKeyboard()
{
    // FIXME stub
}

// Key button
IMPLEMENT_DYNAMIC_CLASS(wxMoKeyButton, wxMoButton)

wxMoKeyButton::~wxMoKeyButton()
{
}

void wxMoKeyButton::Init()
{
}

// Keyboard panel base
IMPLEMENT_DYNAMIC_CLASS(wxMoKeyboardBase, wxPanel)

BEGIN_EVENT_TABLE(wxMoKeyboardBase, wxPanel)
END_EVENT_TABLE()

bool wxMoKeyboardBase::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    // FIXME stub
    
    return true;
}

wxMoKeyboardBase::~wxMoKeyboardBase()
{
}

void wxMoKeyboardBase::Init()
{
}

void wxMoKeyboardBase::AddButtons(const wxArrayString& buttonLabels, wxSizer* sizer, int spacing)
{
    // FIXME stub
}

void wxMoKeyboardBase::OnButtonClick(wxCommandEvent& event)
{
    // FIXME stub
}

// An alphanumeric keyboard
IMPLEMENT_DYNAMIC_CLASS(wxMoAlphabeticKeyboard, wxMoKeyboardBase)

BEGIN_EVENT_TABLE(wxMoAlphabeticKeyboard, wxMoKeyboardBase)
END_EVENT_TABLE()

bool wxMoAlphabeticKeyboard::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    // FIXME stub  

    return true;
}

wxMoAlphabeticKeyboard::~wxMoAlphabeticKeyboard()
{
}

void wxMoAlphabeticKeyboard::Init()
{
}
