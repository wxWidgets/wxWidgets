///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/button.h
// Purpose:     wxButton for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.08.00
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_BUTTON_H_
#define _WX_UNIV_BUTTON_H_

class WXDLLIMPEXP_FWD_CORE wxInputHandler;

#include "wx/bitmap.h"

// ----------------------------------------------------------------------------
// the actions supported by this control
// ----------------------------------------------------------------------------

#define wxACTION_BUTTON_TOGGLE  wxT("toggle")    // press/release the button
#define wxACTION_BUTTON_PRESS   wxT("press")     // press the button
#define wxACTION_BUTTON_RELEASE wxT("release")   // release the button
#define wxACTION_BUTTON_CLICK   wxT("click")     // generate button click event

// ----------------------------------------------------------------------------
// wxButton: a push button
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxButton : public wxButtonBase
{
public:
    wxButton() { Init(); }
    wxButton(wxWindow *parent,
             wxWindowID id,
             const wxBitmap& bitmap,
             const wxString& label = wxEmptyString,
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

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxEmptyString,
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
                const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

    virtual ~wxButton();

    virtual wxWindow *SetDefault() wxOVERRIDE;

    virtual bool IsPressed() const wxOVERRIDE { return m_isPressed; }
    virtual bool IsDefault() const wxOVERRIDE { return m_isDefault; }

    // wxButton actions
    virtual void Toggle();
    virtual void Press();
    virtual void Release();
    virtual void Click();

    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = -1,
                               const wxString& strArg = wxEmptyString) wxOVERRIDE;

    virtual bool CanBeHighlighted() const wxOVERRIDE { return true; }

    static wxInputHandler *GetStdInputHandler(wxInputHandler *handlerDef);
    virtual wxInputHandler *DoGetStdInputHandler(wxInputHandler *handlerDef) wxOVERRIDE
    {
        return GetStdInputHandler(handlerDef);
    }


protected:
    virtual wxSize DoGetBestClientSize() const wxOVERRIDE;

    virtual bool DoDrawBackground(wxDC& dc) wxOVERRIDE;
    virtual void DoDraw(wxControlRenderer *renderer) wxOVERRIDE;

    virtual void DoSetBitmap(const wxBitmap& bitmap, State which) wxOVERRIDE;
    virtual void DoSetBitmapMargins(wxCoord x, wxCoord y) wxOVERRIDE;

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

#endif // _WX_UNIV_BUTTON_H_

