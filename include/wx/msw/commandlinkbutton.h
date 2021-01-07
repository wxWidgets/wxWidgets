/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/commandlinkbutton.h
// Purpose:     wxCommandLinkButton class
// Author:      Rickard Westerlund
// Created:     2010-06-11
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_COMMANDLINKBUTTON_H_
#define _WX_MSW_COMMANDLINKBUTTON_H_

// ----------------------------------------------------------------------------
// Command link button for wxMSW
// ----------------------------------------------------------------------------

// Derive from the generic version to be able to fall back to it during
// run-time if the command link buttons are not supported by the system we're
// running under.

class WXDLLIMPEXP_ADV wxCommandLinkButton : public wxGenericCommandLinkButton
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
                        const wxString& name = wxASCII_STR(wxButtonNameStr))
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
                const wxString& name = wxASCII_STR(wxButtonNameStr));

    // overridden base class methods
    // -----------------------------

    // do the same thing as in the generic case here
    virtual void SetLabel(const wxString& label) wxOVERRIDE
    {
        SetMainLabelAndNote(label.BeforeFirst('\n'), label.AfterFirst('\n'));
    }

    virtual void SetMainLabelAndNote(const wxString& mainLabel,
                                     const wxString& note) wxOVERRIDE;

    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const wxOVERRIDE;

protected:
    virtual wxSize DoGetBestSize() const wxOVERRIDE;

    virtual bool HasNativeBitmap() const wxOVERRIDE;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxCommandLinkButton);
};

#endif // _WX_MSW_COMMANDLINKBUTTON_H_
