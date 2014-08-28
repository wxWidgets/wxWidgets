///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/tglbtn.h
// Purpose:     wxToggleButton for wxUniversal
// Author:      Vadim Zeitlin
// Modified by: David Bjorkevik
// Created:     16.05.06
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_TGLBTN_H_
#define _WX_UNIV_TGLBTN_H_

#include "wx/button.h"

// ----------------------------------------------------------------------------
// wxToggleButton: a push button
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxToggleButton: public wxButton
{
public:
    wxToggleButton();

    wxToggleButton(wxWindow *parent,
             wxWindowID id,
             const wxBitmap& bitmap,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxCheckBoxNameStr);

    wxToggleButton(wxWindow *parent,
             wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxCheckBoxNameStr);

    virtual ~wxToggleButton();

    virtual bool IsPressed() const wxOVERRIDE { return m_isPressed || m_value; }

    // wxToggleButton actions
    virtual void Toggle() wxOVERRIDE;
    virtual void Click() wxOVERRIDE;

    // Get/set the value
    void SetValue(bool state);
    bool GetValue() const { return m_value; }

protected:
    virtual wxBorder GetDefaultBorder() const wxOVERRIDE { return wxBORDER_NONE; }

    // the current value
    bool m_value;

private:
    // common part of all ctors
    void Init();

    DECLARE_DYNAMIC_CLASS(wxToggleButton)
};

#endif // _WX_UNIV_TGLBTN_H_
