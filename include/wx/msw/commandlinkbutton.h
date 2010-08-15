/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/commandlinkbutton.h
// Purpose:     wxCommandLinkButton class
// Author:      Rickard Westerlund
// Created:     2010-06-11
// RCS-ID:      $Id$
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COMMANDLINKBUTTON_H_
#define _WX_COMMANDLINKBUTTON_H_

#include "wx/defs.h"

// ----------------------------------------------------------------------------
// Command link button
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxCommandLinkButton : public wxGenericCommandLinkButton
{
public:
    wxCommandLinkButton () : wxGenericCommandLinkButton() { }

    wxCommandLinkButton(wxWindow *parent,
                        wxWindowID id,
                        const wxString& mainLabel = wxEmptyString,
                        const wxString& note = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxButtonNameStr)
        : wxGenericCommandLinkButton()
    {
        Create(parent, id, mainLabel, note, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& mainLabel = wxEmptyString,
                const wxString& note = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

    // overrides virtual methods in base class
    virtual void SetMainLabelAndNote(const wxString &mainLabel,
                                     const wxString &note);

    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const;

protected:
    // overrides virtual method in base class
    virtual wxSize DoGetBestSize() const;

    virtual bool HasNativeBitmap() const;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxCommandLinkButton);
};

#endif // _WX_COMMANDLINKBUTTON_H_

