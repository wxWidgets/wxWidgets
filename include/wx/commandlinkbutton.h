/////////////////////////////////////////////////////////////////////////////
// Name:        wx/commandlinkbutton.h
// Purpose:     wxCommandLinkButtonBase and wxGenericCommandLinkButton classes
// Author:      Rickard Westerlund
// Created:     2010-06-11
// RCS-ID:      $Id$
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COMMANDLINKBUTTON_H_BASE_
#define _WX_COMMANDLINKBUTTON_H_BASE_

#if wxUSE_COMMANDLINKBUTTON

#include "wx/defs.h"

// ----------------------------------------------------------------------------
// Command link button base
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxCommandLinkButtonBase : public wxButton
{
public:
    wxCommandLinkButtonBase() : wxButton() { }

    wxCommandLinkButtonBase(wxWindow *parent,
                            wxWindowID id,
                            const wxString& mainLabel = wxEmptyString,
                            const wxString& note = wxEmptyString,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = 0,
                            const wxValidator& validator =
                                wxDefaultValidator,
                            const wxString& name = wxButtonNameStr)
        : wxButton(parent,
                   id,
                   mainLabel + '\n' + note,
                   pos,
                   size,
                   style,
                   validator,
                   name)
        { }

    virtual void SetLabel(const wxString& label)
    {
        SetMainLabelAndNote(label.BeforeFirst('\n'), label.AfterFirst('\n'));
    }

    virtual void SetMainLabelAndNote(const wxString &mainLabel,
                                     const wxString &note) = 0;

    virtual void SetMainLabel(const wxString &mainLabel)
    {
        SetMainLabelAndNote(mainLabel, GetNote());
    }

    virtual void SetNote(const wxString &note)
    {
        SetMainLabelAndNote(GetMainLabel(), note);
    }

    virtual wxString GetMainLabel() const
    {
        return GetLabel().BeforeFirst('\n');
    }

    virtual wxString GetNote() const
    {
        return GetLabel().AfterFirst('\n');
    }

protected:
    virtual bool HasNativeBitmap() const { return false; }

private:
    wxDECLARE_NO_COPY_CLASS(wxCommandLinkButtonBase);
};

// ----------------------------------------------------------------------------
// Generic command link button
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGenericCommandLinkButton
                      : public wxCommandLinkButtonBase
{
public:
    wxGenericCommandLinkButton() : wxCommandLinkButtonBase() { }


    wxGenericCommandLinkButton(wxWindow *parent,
                               wxWindowID id,
                               const wxString& mainLabel = wxEmptyString,
                               const wxString& note = wxEmptyString,
                               const wxPoint& pos = wxDefaultPosition,
                               const wxSize& size = wxDefaultSize,
                               long style = 0,
                               const wxValidator& validator = wxDefaultValidator,
                               const wxString& name = wxButtonNameStr)
        : wxCommandLinkButtonBase()
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

    virtual void SetMainLabelAndNote(const wxString &mainLabel,
                                     const wxString &note)
    {
        wxButton::SetLabel(mainLabel + '\n' + note);
    }

private:
    void SetDefaultBitmap();

    wxDECLARE_NO_COPY_CLASS(wxGenericCommandLinkButton);
};

#if defined(__WXMSW__)
    #include "wx/msw/commandlinkbutton.h"
#else
    class wxCommandLinkButton : public wxGenericCommandLinkButton
    {
    public:
        wxCommandLinkButton() : wxGenericCommandLinkButton() { }

        wxCommandLinkButton(wxWindow *parent,
                            wxWindowID id,
                            const wxString& mainLabel = wxEmptyString,
                            const wxString& note = wxEmptyString,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = 0,
                            const wxValidator& validator = wxDefaultValidator,
                            const wxString& name = wxButtonNameStr)
            : wxGenericCommandLinkButton(parent,
                                         id,
                                         mainLabel,
                                         note,
                                         pos,
                                         size,
                                         style,
                                         validator,
                                         name)
            { }

    private:
        wxDECLARE_NO_COPY_CLASS(wxCommandLinkButton);
    };
#endif // __WXMSW__/!__WXMSW__

#endif // wxUSE_COMMANDLINKBUTTON

#endif // _WX_COMMANDLINKBUTTON_H_BASE_

