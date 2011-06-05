/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/keyboard.h
// Purpose:     Virtual keyboard classes
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_KEYBOARD_H_
#define _WX_MOBILE_GENERIC_KEYBOARD_H_

#include "wx/bitmap.h"
#include "wx/frame.h"
#include "wx/panel.h"
#include "wx/mobile/generic/button.h"

#if wxUSE_POPUPWIN
#include "wx/popupwin.h"
#endif

#define wxMOKEYBOARD_ALPHABETIC   wxT("Alphabetic")

/**
    @class wxMoKeyboard

    This class implements an alphanumeric keyboard control. It is not directly
    exposed as part of the wxMobile API.

    @category{wxMobileImplementation}
*/

#if wxUSE_POPUPWIN
class WXDLLEXPORT wxMoKeyboard: public wxPopupWindow
#else
class WXDLLEXPORT wxMoKeyboard: public wxFrame
#endif
{
public:
    /// Default constructor.
    wxMoKeyboard() { Init(); }

    /// Constructor taking a text label.
    wxMoKeyboard(wxWindow *parent)
    {
        Init();
        Create(parent);
    }

    /// Creation function.
    bool Create(wxWindow *parent);

    virtual ~wxMoKeyboard();

    void Init();

    /// Shows the given keyboard by name - static function.
    static void ShowKeyboard(const wxString& name);

    /// Hides the keyboard - static function.
    static void HideKeyboard();

    /// Destroys the keyboard - static function.
    static void DestroyKeyboard();

    /// Is the keyboard showing?
    static bool IsKeyboardShowing() { return sm_keyboard && sm_keyboard->IsShown(); }

    /// Get the keyboard instance
    static wxMoKeyboard* GetKeyboard() { return sm_keyboard; }

    /// Shows the given keyboard by name
    void DoShowKeyboard(const wxString& name);

    /// Shows the given keyboard by window
    void DoShowKeyboard(wxWindow* win);

    /// Sets the current keyboard
    void SetCurrentKeyboard(wxWindow* win) { m_currentWindow = win; }

    /// Returns the current keyboard
    wxWindow* GetCurrentKeyboard() const { return m_currentWindow; }

    /// Finds a keyboard window by name
    wxWindow* GetWindowForName(const wxString& name) const;

    /// Finds a keyboard name by window
    wxString GetNameForWindow(wxWindow* win) const;

    /// Adds a keyboard to the list of  available keyboards
    void AddKeyboard(const wxString& name, wxWindow* window);

    /// Fits the current keyboard to the size of the keyboard frame
    void FitCurrentKeyboard();

protected:

    void OnSize(wxSizeEvent& event);

    wxWindowList    m_windows;
    wxArrayString   m_windowNames;

    wxWindow*       m_currentWindow;

    static wxMoKeyboard* sm_keyboard;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoKeyboard)
    DECLARE_EVENT_TABLE()
};

/**
    @class wxMoKeyboardBase

    This class implements some functionality common to all keyboard panels.

    @category{wxMobileImplementation}
*/

class WXDLLEXPORT wxMoKeyboardBase: public wxPanel
{
public:
    /// Default constructor.
    wxMoKeyboardBase() { Init(); }

    /// Constructor taking a text label.
    wxMoKeyboardBase(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = wxT("KeyboardPanel"))
    {
        Init();
        Create(parent, id, pos, size, style, name);
    }

    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxT("KeyboardPanel"));

    virtual ~wxMoKeyboardBase();

    void Init();

    void AddButtons(const wxArrayString& buttonLabels, wxSizer* sizer, int spacing=5);

    void OnButtonClick(wxCommandEvent& event);

    virtual bool AcceptsFocus() const { return false; }

protected:

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoKeyboardBase)
    DECLARE_EVENT_TABLE()
};

/**
    @class wxMoAlphabeticKeyboard

    This class implements an alphanumeric keyboard control. It is not directly
    exposed as part of the wxMobile API.

    @category{wxMobileImplementation}
*/

class WXDLLEXPORT wxMoAlphabeticKeyboard: public wxMoKeyboardBase
{
public:
    /// Default constructor.
    wxMoAlphabeticKeyboard() { Init(); }

    /// Constructor taking a text label.
    wxMoAlphabeticKeyboard(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = wxT("KeyboardPanel"))
    {
        Init();
        Create(parent, id, pos, size, style, name);
    }

    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxT("KeyboardPanel"));

    virtual ~wxMoAlphabeticKeyboard();

    void Init();

protected:

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoAlphabeticKeyboard)
    DECLARE_EVENT_TABLE()
};

/**
    @class wxMoKeyButton

    This shows a key button on the virtual keyboard.

    @category{wxMobileImplementation}
*/

class WXDLLEXPORT wxMoKeyButton: public wxMoButton
{
public:
    /// Default constructor.
    wxMoKeyButton() { Init(); }

    /// Constructor taking a text label.
    wxMoKeyButton(wxWindow *parent,
             wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxButtonNameStr)
    {
        Init();
        Create(parent, id, label, pos, size, style, validator, name);
    }

    virtual ~wxMoKeyButton();

    void Init();

    // Eventually this could be an array of strings, with the first being
    // the main one, and the rest being international variations.
    void SetCharacter(const wxString& character) { m_character = character; }
    wxString GetCharacter() const { return m_character; }

    virtual bool AcceptsFocus() const { return false; }


protected:

    wxString m_character;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoKeyButton)
    //DECLARE_EVENT_TABLE()
};

#endif
    // _WX_MOBILE_GENERIC_KEYBOARD_H_
