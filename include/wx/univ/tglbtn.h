///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/button.h
// Purpose:     wxToggleButton for wxUniversal
// Author:      Vadim Zeitlin
// Modified by: David Bjorkevik
// Created:     16.05.06
// RCS-ID:      $Id$
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_TGLBTN_H_
#define _WX_UNIV_TGLBTN_H_

class WXDLLEXPORT wxInputHandler;

#include "wx/bitmap.h"
#include "wx/checkbox.h"

// ----------------------------------------------------------------------------
// the actions supported by this control
// ----------------------------------------------------------------------------

#define wxACTION_BUTTON_TOGGLE  _T("toggle")    // press/release the button
#define wxACTION_BUTTON_PRESS   _T("press")     // press the button
#define wxACTION_BUTTON_RELEASE _T("release")   // release the button
#define wxACTION_BUTTON_CLICK   _T("click")     // generate button click event

// ----------------------------------------------------------------------------
// wxToggleButton: a push button
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxToggleButton: public wxControl
{
public:
    wxToggleButton() { Init(); }
    wxToggleButton(wxWindow *parent,
             wxWindowID id,
             const wxBitmap& bitmap,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxCheckBoxNameStr)
    {
        Init();

        Create(parent, id, bitmap, label, pos, size, style, validator, name);
    }
    
    wxToggleButton(wxWindow *parent,
             wxWindowID id,
             const wxString& label = wxEmptyString,
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
                const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxCheckBoxNameStr)
    {
        return Create(parent, id, wxNullBitmap, label,
                      pos, size, style, validator, name);
    }
    
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& bitmap,
                const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxCheckBoxNameStr);

    virtual ~wxToggleButton();

    virtual void SetImageLabel(const wxBitmap& bitmap);
    virtual void SetImageMargins(wxCoord x, wxCoord y);

    virtual bool IsPressed() const { return m_isPressed || m_value; }

    // wxToggleButton actions
    void Toggle();
    virtual void Press();
    virtual void Release();
    virtual void Click();

    // Get/set the value
    void SetValue(bool state);
    bool GetValue() const { return m_value; }

    // returns the default button size for this platform
    static wxSize GetDefaultSize();

protected:
    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = -1,
                               const wxString& strArg = wxEmptyString);
    virtual wxSize DoGetBestClientSize() const;
    
    virtual bool DoDrawBackground(wxDC& dc);
    virtual void DoDraw(wxControlRenderer *renderer);

    virtual bool CanBeHighlighted() const { return true; }

    // common part of all ctors
    void Init();

    // current state - is the user currently pressing the button
    bool m_isPressed;
    
    // the current value
    bool m_value;

    // the (optional) image to show and the margins around it
    wxBitmap m_bitmap;
    wxCoord  m_marginBmpX,
             m_marginBmpY;

private:
    DECLARE_DYNAMIC_CLASS(wxToggleButton)
};
// wxStdToggleButtonInputHandler: translates SPACE and ENTER keys and the left mouse
// click into button press/release actions
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStdToggleButtonInputHandler : public wxStdInputHandler
{
public:
    wxStdToggleButtonInputHandler(wxInputHandler *inphand);

    virtual bool HandleKey(wxInputConsumer *consumer,
                           const wxKeyEvent& event,
                           bool pressed);
    virtual bool HandleMouse(wxInputConsumer *consumer,
                             const wxMouseEvent& event);
    virtual bool HandleMouseMove(wxInputConsumer *consumer, const wxMouseEvent& event);
    virtual bool HandleFocus(wxInputConsumer *consumer, const wxFocusEvent& event);
    virtual bool HandleActivation(wxInputConsumer *consumer, bool activated);

private:
    // the window (button) which has capture or NULL and the flag telling if
    // the mouse is inside the button which captured it or not
    wxWindow *m_winCapture;
    bool      m_winHasMouse;
};

#endif // _WX_UNIV_TGLBTN_H_

