///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/checkbox.h
// Purpose:     wxCheckBox declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07.09.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_CHECKBOX_H_
#define _WX_UNIV_CHECKBOX_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "univcheckbox.h"
#endif

#include "wx/button.h" // for wxStdButtonInputHandler

// ----------------------------------------------------------------------------
// the actions supported by wxCheckBox
// ----------------------------------------------------------------------------

#define wxACTION_CHECKBOX_CHECK   _T("check")   // SetValue(true)
#define wxACTION_CHECKBOX_CLEAR   _T("clear")   // SetValue(false)
#define wxACTION_CHECKBOX_TOGGLE  _T("toggle")  // toggle the check state

// additionally it accepts wxACTION_BUTTON_PRESS and RELEASE

// ----------------------------------------------------------------------------
// wxCheckBox
// ----------------------------------------------------------------------------

// X11 headers may define this
#ifdef Status
#undef Status
#endif

class WXDLLEXPORT wxCheckBox : public wxCheckBoxBase
{
public:
    // checkbox constants
    enum State
    {
        State_Normal,
        State_Pressed,
        State_Disabled,
        State_Current,
        State_Max
    };

    enum Status
    {
        Status_Checked,
        Status_Unchecked,
        Status_3rdState,
        Status_Max
    };

    // constructors
    wxCheckBox() { Init(); }

    wxCheckBox(wxWindow *parent,
               wxWindowID id,
               const wxString& label,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxCheckBoxNameStr)
    {
        Init();

        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxCheckBoxNameStr);

    // implement the checkbox interface
    virtual void SetValue(bool value);
    virtual bool GetValue() const;

    // set/get the bitmaps to use for the checkbox indicator
    void SetBitmap(const wxBitmap& bmp, State state, Status status);
    virtual wxBitmap GetBitmap(State state, Status status) const;

    // wxCheckBox actions
    void Toggle();
    virtual void Press();
    virtual void Release();
    virtual void ChangeValue(bool value);

    // overridden base class virtuals
    virtual bool IsPressed() const { return m_isPressed; }

protected:
    virtual void DoSet3StateValue(wxCheckBoxState WXUNUSED(state));
    virtual wxCheckBoxState DoGet3StateValue() const;

    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = -1,
                               const wxString& strArg = wxEmptyString);
    virtual void DoDraw(wxControlRenderer *renderer);
    virtual wxSize DoGetBestClientSize() const;

    virtual bool CanBeHighlighted() const { return true; }

    // get the size of the bitmap using either the current one or the default
    // one (query renderer then)
    virtual wxSize GetBitmapSize() const;

    // common part of all ctors
    void Init();

    // send command event notifying about the checkbox state change
    virtual void SendEvent();

    // called when the checkbox becomes checked - radio button hook
    virtual void OnCheck();

    // get the state corresponding to the flags (combination of wxCONTROL_XXX)
    wxCheckBox::State GetState(int flags) const;

    // directly access the bitmaps array without trying to find a valid bitmap
    // to use as GetBitmap() does
    wxBitmap DoGetBitmap(State state, Status status) const
        { return m_bitmaps[state][status]; }

    // get the current status
    Status GetStatus() const { return m_status; }

private:
    // the current check status
    Status m_status;

    // the bitmaps to use for the different states
    wxBitmap m_bitmaps[State_Max][Status_Max];

    // is the checkbox currently pressed?
    bool m_isPressed;

    DECLARE_DYNAMIC_CLASS(wxCheckBox)
};

// ----------------------------------------------------------------------------
// wxStdCheckboxInputHandler: handles the mouse events for the check and radio
// boxes (handling the keyboard input is simple, but its handling differs a
// lot between GTK and MSW, so a new class should be derived for this)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStdCheckboxInputHandler : public wxStdButtonInputHandler
{
public:
    wxStdCheckboxInputHandler(wxInputHandler *inphand);

    // we have to override this one as wxStdButtonInputHandler version works
    // only with the buttons
    virtual bool HandleActivation(wxInputConsumer *consumer, bool activated);
};

#endif // _WX_UNIV_CHECKBOX_H_
