/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/checkbox.h
// Purpose:     wxCheckBox class
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHECKBOX_H_
#define _WX_CHECKBOX_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "checkbox.h"
#endif

// Checkbox item (single checkbox)
class WXDLLEXPORT wxCheckBox : public wxCheckBoxBase
{
public:
    wxCheckBox() { }
    wxCheckBox(wxWindow *parent,
               wxWindowID id,
               const wxString& label,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxCheckBoxNameStr)
    {
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

    virtual void SetValue(bool value);
    virtual bool GetValue() const;

    virtual void SetLabel(const wxString& label);

    virtual bool MSWCommand(WXUINT param, WXWORD id);
    virtual void Command(wxCommandEvent& event);

protected:
    virtual wxSize DoGetBestSize() const;

    virtual void DoSet3StateValue(wxCheckBoxState value);

    virtual wxCheckBoxState DoGet3StateValue() const;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxCheckBox)
};

#endif
    // _WX_CHECKBOX_H_
