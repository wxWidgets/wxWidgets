///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/button.h
// Purpose:     wxButton for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_BUTTON_H_
#define _WX_UNIV_BUTTON_H_

#ifdef __GNUG__
    #pragma interface "univbutton.h"
#endif

class WXDLLEXPORT wxInputHandler;

#include "wx/bitmap.h"

// ----------------------------------------------------------------------------
// the actions supported by this control
// ----------------------------------------------------------------------------

#define wxACTION_BUTTON_TOGGLE  _T("toggle")    // press/release the button
#define wxACTION_BUTTON_PRESS   _T("press")     // press the button
#define wxACTION_BUTTON_RELEASE _T("release")   // release the button
#define wxACTION_BUTTON_CLICK   _T("click")     // generate button click event

// ----------------------------------------------------------------------------
// wxButton: a push button
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxButton : public wxButtonBase
{
public:
    wxButton() { Init(); }
    wxButton(wxWindow *parent,
             wxWindowID id,
             const wxBitmap& bitmap,
             const wxString& label,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxButtonNameStr)
    {
        Init();

        Create(parent, id, bitmap, label, pos, size, style, validator, name);
    }

    wxButton(wxWindow *parent,
             wxWindowID id,
             const wxString& label,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxButtonNameStr)
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
                const wxString& name = wxButtonNameStr)
    {
        return Create(parent, id, wxNullBitmap, label,
                      pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& bitmap,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

    virtual ~wxButton();

    virtual void SetImageLabel(const wxBitmap& bitmap);
    virtual void SetImageMargins(wxCoord x, wxCoord y);
    virtual void SetDefault();

    virtual bool IsPressed() const { return m_isPressed; }
    virtual bool IsDefault() const { return m_isDefault; }

    // wxButton actions
    void Toggle();
    virtual void Press();
    virtual void Release();
    virtual void Click();

protected:
    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = -1,
                               const wxString& strArg = wxEmptyString);
    virtual wxSize DoGetBestClientSize() const;
    virtual void DoDraw(wxControlRenderer *renderer);

    virtual wxString GetInputHandlerType() const;
    virtual bool CanBeHighlighted() const { return TRUE; }

    // common part of all ctors
    void Init();

    // current state
    bool m_isPressed,
         m_isDefault;

    // the (optional) image to show and the margins around it
    wxBitmap m_bitmap;
    wxCoord  m_marginBmpX,
             m_marginBmpY;

private:
    DECLARE_DYNAMIC_CLASS(wxButton)
};

// ----------------------------------------------------------------------------
// wxStdButtonInputHandler: translates SPACE and ENTER keys and the left mouse
// click into button press/release actions
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStdButtonInputHandler : public wxStdInputHandler
{
public:
    wxStdButtonInputHandler(wxInputHandler *inphand);

    virtual bool HandleKey(wxControl *control,
                           const wxKeyEvent& event,
                           bool pressed);
    virtual bool HandleMouse(wxControl *control,
                             const wxMouseEvent& event);
    virtual bool HandleMouseMove(wxControl *control, const wxMouseEvent& event);
    virtual bool HandleFocus(wxControl *control, const wxFocusEvent& event);
    virtual bool HandleActivation(wxControl *control, bool activated);

private:
    // the window (button) which has capture or NULL and the flag telling if
    // the mouse is inside the button which captured it or not
    wxWindow *m_winCapture;
    bool      m_winHasMouse;
};

#endif // _WX_UNIV_BUTTON_H_

