/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/checkbox.h
// Purpose:     wxCheckBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHECKBOX_H_
#define _WX_CHECKBOX_H_

#ifdef __GNUG__
#pragma interface "checkbox.h"
#endif

#if wxUSE_CHECKBOX

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxCheckBoxNameStr;

// Checkbox item (single checkbox)
class WXDLLEXPORT wxBitmap;
class WXDLLEXPORT wxCheckBox : public wxControl
{
DECLARE_DYNAMIC_CLASS(wxCheckBox)

public:
    wxCheckBox() { }
    wxCheckBox(wxWindow *parent, wxWindowID id, const wxString& label,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxCheckBoxNameStr)
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id, const wxString& label,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxCheckBoxNameStr);

    virtual void SetValue(bool value);
    virtual bool GetValue() const ;

    virtual bool MSWCommand(WXUINT param, WXWORD id);
    virtual void SetLabel(const wxString& label);
    virtual void Command(wxCommandEvent& event);

protected:
    virtual wxSize DoGetBestSize() const;
};

class WXDLLEXPORT wxBitmapCheckBox: public wxCheckBox
{
DECLARE_DYNAMIC_CLASS(wxBitmapCheckBox)

public:
    int checkWidth ;
    int checkHeight ;

    wxBitmapCheckBox() { checkWidth = -1; checkHeight = -1; }
    wxBitmapCheckBox(wxWindow *parent, wxWindowID id, const wxBitmap *label,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxCheckBoxNameStr)
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id, const wxBitmap *bitmap,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxCheckBoxNameStr);

    virtual void SetLabel(const wxBitmap& bitmap);
};

#endif // wxUSE_CHECKBOX

#endif
    // _WX_CHECKBOX_H_
