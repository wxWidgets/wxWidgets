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
                const wxString& name = wxButtonNameStr);

    virtual ~wxButton();

    virtual void SetDefault();

    virtual bool IsPressed() const { return m_isPressed; }
    virtual bool IsDefault() const { return m_isDefault; }

    // wxButton actions
    void Press();
    void Release();
    void Toggle();
    void Click();

protected:
    virtual wxInputHandler *CreateInputHandler() const;
    virtual bool PerformAction(const wxControlAction& action);
    virtual wxSize DoGetBestSize() const;
    virtual void DoDraw(wxControlRenderer *renderer);

    // common part of all ctors
    void Init();

    bool m_isPressed,
         m_isDefault;

private:
    DECLARE_DYNAMIC_CLASS(wxButton)
};

#endif
    // _WX_BUTTON_H_

