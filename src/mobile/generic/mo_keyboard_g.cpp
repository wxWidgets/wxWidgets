/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_keyboard_g.cpp
// Purpose:     wxMoAlphabeticKeyboard and other input-related classes
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
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
#include "wx/mobile/generic/simulator.h"

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

#if wxUSE_POPUPWIN
IMPLEMENT_DYNAMIC_CLASS(wxMoKeyboard, wxPopupWindow)
#else
IMPLEMENT_DYNAMIC_CLASS(wxMoKeyboard, wxFrame)
#endif

#if wxUSE_POPUPWIN
BEGIN_EVENT_TABLE(wxMoKeyboard, wxPopupWindow)
#else
BEGIN_EVENT_TABLE(wxMoKeyboard, wxFrame)
#endif
    EVT_SIZE(wxMoKeyboard::OnSize)
END_EVENT_TABLE()

wxMoKeyboard* wxMoKeyboard::sm_keyboard = NULL;

bool wxMoKeyboard::Create(wxWindow *parent)
{
#if wxUSE_POPUPWIN
    if ( !wxPopupWindow::Create(parent, wxBORDER_SIMPLE) )
        return false;
#else
    if ( !wxFrame::Create(parent, id, wxEmptyString, pos, size,
        style|wxBORDER_SIMPLE|wxSTAY_ON_TOP|wxFRAME_NO_TASKBAR, name) )
        return false;
#endif

    sm_keyboard = this;

    AddKeyboard(wxMOKEYBOARD_ALPHABETIC, new wxMoAlphabeticKeyboard(this, wxID_ANY));

    return true;
}

wxMoKeyboard::~wxMoKeyboard()
{
    sm_keyboard = NULL;
}

void wxMoKeyboard::Init()
{
    m_currentWindow = NULL;
}

void wxMoKeyboard::OnSize(wxSizeEvent& WXUNUSED(event))
{
    FitCurrentKeyboard();
}

// Shows the given keyboard by name - static function.
void wxMoKeyboard::ShowKeyboard(const wxString& name)
{
    if (!sm_keyboard)
    {
        wxWindow* parent = NULL;
        wxMoSimulatorScreenWindow* simulatorScreenWindow = wxMoSimulatorScreenWindow::FindSimulatorScreenWindow(NULL);
        if (simulatorScreenWindow)
            parent = wxGetTopLevelParent(simulatorScreenWindow);
        else
            parent = wxTheApp->GetTopWindow();

        sm_keyboard = new wxMoKeyboard(parent);
    }
    sm_keyboard->DoShowKeyboard(name);
}

// Hides the keyboard - static function.
void wxMoKeyboard::HideKeyboard()
{
    if (sm_keyboard)
        sm_keyboard->Hide();
}

// Destroys the keyboard - static function.
void wxMoKeyboard::DestroyKeyboard()
{
    if (sm_keyboard)
        sm_keyboard->Destroy();
}

void wxMoKeyboard::DoShowKeyboard(const wxString& name)
{
    wxWindow* win = GetWindowForName(name);
    if (win)
        DoShowKeyboard(win);
}

void wxMoKeyboard::DoShowKeyboard(wxWindow* win)
{
    if (GetCurrentKeyboard() == win && GetCurrentKeyboard()->IsShown())
    {
        Show(true);
        return;
    }

    if (GetCurrentKeyboard())
        GetCurrentKeyboard()->Show(false);

    SetCurrentKeyboard(win);
    FitCurrentKeyboard();
    GetCurrentKeyboard()->Show(true);

    Show(true);
}

wxWindow* wxMoKeyboard::GetWindowForName(const wxString& name) const
{
    size_t i;
    for (i = 0; i < m_windowNames.GetCount(); i++)
    {
        if (name == m_windowNames[i])
            return m_windows.Item(i)->GetData();
    }
    return NULL;
}

wxString wxMoKeyboard::GetNameForWindow(wxWindow* win) const
{
    size_t i = 0;
    wxWindowList::compatibility_iterator node;
    for (node = m_windows.GetFirst(); node; node = node->GetNext())
    {
        wxWindow* w = node->GetData();
        if (w == win)
            return m_windowNames[i];

        i ++;
    }
    return wxEmptyString;
}

void wxMoKeyboard::AddKeyboard(const wxString& name, wxWindow* window)
{
    m_windowNames.Add(name);
    m_windows.Append(window);
}

void wxMoKeyboard::FitCurrentKeyboard()
{
    wxMoSimulatorScreenWindow* simulatorScreenWindow = wxMoSimulatorScreenWindow::FindSimulatorScreenWindow(NULL);
    if (simulatorScreenWindow)
    {
        wxPoint simulatorPos = simulatorScreenWindow->ClientToScreen(wxPoint(0, 0));
        wxSize simulatorSize = simulatorScreenWindow->GetClientSize();
        
        if (GetCurrentKeyboard() && GetCurrentKeyboard()->GetSizer())
        {
            wxSize minSize = GetCurrentKeyboard()->GetSizer()->GetMinSize();
            SetSize(wxSize(simulatorSize.x, minSize.y));
            GetCurrentKeyboard()->SetSize(0, 0, simulatorSize.x, minSize.y);

            SetPosition(wxPoint(simulatorPos.x, simulatorPos.y + simulatorSize.y - GetSize().y));
        }
    }
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
    if ( !wxPanel::Create(parent, id, pos, size, style|wxBORDER_NONE, name) )
        return false;

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
    //sizer->AddSpacer(spacing);
    size_t i;
    for (i = 0; i < buttonLabels.GetCount(); i++)
    {
        wxMoKeyButton* button = new wxMoKeyButton(this, wxID_ANY, buttonLabels[i].Upper(), wxDefaultPosition, wxSize(20, 25));
        button->SetCharacter(buttonLabels[i].Lower());
        button->Connect(button->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wxMoKeyboardBase::OnButtonClick),
            NULL, this);
        sizer->Add(button, 0, wxALL, spacing);
        //sizer->AddSpacer(spacing);
    }
}

void wxMoKeyboardBase::OnButtonClick(wxCommandEvent& event)
{
    wxMoKeyButton* button = wxDynamicCast(event.GetEventObject(), wxMoKeyButton);
    if (button)
    {
        wxWindow* focusWin = wxFindFocusDescendant(wxMoSimulatorScreenWindow::FindSimulatorScreenWindow(button));
        //wxWindow* focusWin = wxFindFocusDescendant(wxMoApp::GetSimulatorFrame());
        wxTextCtrl* textCtrl = wxDynamicCast(focusWin, wxTextCtrl);
        if (textCtrl)
        {
            textCtrl->WriteText(button->GetCharacter());
        }
        else if (focusWin)
        {
            wxKeyEvent keyEvent(wxEVT_CHAR);
            keyEvent.SetEventObject(focusWin);
            keyEvent.m_keyCode = button->GetCharacter()[0];
            keyEvent.m_uniChar = button->GetCharacter()[0];
            keyEvent.SetId(focusWin->GetId());

            focusWin->GetEventHandler()->ProcessEvent(keyEvent);
        }

        //wxMessageBox(button->GetCharacter());
    }
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
    if ( !wxMoKeyboardBase::Create(parent, id, pos, size, style|wxBORDER_NONE, name) )
        return false;

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(topSizer);

    wxBoxSizer* row1Sizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(row1Sizer, 1, wxEXPAND);

    row1Sizer->AddStretchSpacer();
    wxArrayString row1Labels;
    row1Labels.Add(wxT("Q"));
    row1Labels.Add(wxT("W"));
    row1Labels.Add(wxT("E"));
    row1Labels.Add(wxT("R"));
    row1Labels.Add(wxT("T"));
    row1Labels.Add(wxT("Y"));
    row1Labels.Add(wxT("U"));
    row1Labels.Add(wxT("I"));
    row1Labels.Add(wxT("O"));
    row1Labels.Add(wxT("P"));
    AddButtons(row1Labels, row1Sizer);
    row1Sizer->AddStretchSpacer();
    
    wxBoxSizer* row2Sizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(row2Sizer, 1, wxEXPAND);

    row2Sizer->AddStretchSpacer();
    wxArrayString row2Labels;
    row2Labels.Add(wxT("A"));
    row2Labels.Add(wxT("S"));
    row2Labels.Add(wxT("D"));
    row2Labels.Add(wxT("F"));
    row2Labels.Add(wxT("G"));
    row2Labels.Add(wxT("H"));
    row2Labels.Add(wxT("J"));
    row2Labels.Add(wxT("K"));
    row2Labels.Add(wxT("L"));
    AddButtons(row2Labels, row2Sizer);
    row2Sizer->AddStretchSpacer();
    
    wxBoxSizer* row3Sizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(row3Sizer, 1, wxEXPAND);

    row3Sizer->AddStretchSpacer();
    wxArrayString row3Labels;
    row3Labels.Add(wxT("Z"));
    row3Labels.Add(wxT("X"));
    row3Labels.Add(wxT("C"));
    row3Labels.Add(wxT("V"));
    row3Labels.Add(wxT("B"));
    row3Labels.Add(wxT("N"));
    row3Labels.Add(wxT("M"));
    AddButtons(row3Labels, row3Sizer);
    row3Sizer->AddStretchSpacer();    

    return true;
}

wxMoAlphabeticKeyboard::~wxMoAlphabeticKeyboard()
{
}

void wxMoAlphabeticKeyboard::Init()
{
}
