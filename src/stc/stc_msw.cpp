/////////////////////////////////////////////////////////////////////////////
// Name:        src/stc/stc_msw.cpp
// Purpose:     wxMSW implementation of wxStyledTextCtrl
// Author:      Maarten Bent
// Created:     2026-11-18
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STC

#include "wx/stc/stc.h"

#ifndef WX_PRECOMP
    #include "wx/module.h"
    #include "wx/msw/private.h"
#endif // WX_PRECOMP

#include "Scintilla.h"

bool wxStyledTextCtrl::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    static bool s_registered = false;
    if (!s_registered)
    {
        if (!Scintilla_RegisterClasses(wxGetInstance()))
            return false;
        s_registered = true;
    }

    style &= ~(wxVSCROLL | wxHSCROLL);
    style |= wxWANTS_CHARS | wxCLIP_CHILDREN;

    if (!CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) ||
        !MSWCreateControl(L"Scintilla", wxEmptyString, pos, size))
    {
        return false;
    }

    m_sciFn = reinterpret_cast<void*>(
        ::SendMessage(GetHwnd(), SCI_GETDIRECTFUNCTION, 0, 0));
    m_sciPtr = ::SendMessage(GetHwnd(), SCI_GETDIRECTPOINTER, 0, 0);

    SetDefaultAttributes();

    return true;
}

wxStyledTextCtrl::~wxStyledTextCtrl()
{
}

bool wxStyledTextCtrl::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM* result)
{
    auto* hdr = reinterpret_cast<NMHDR*>(lParam);
    if (hdr && hdr->hwndFrom == GetHwnd())
    {
        NotifyParent(reinterpret_cast<SCNotification*>(lParam));
        *result = 0;
        return true;
    }

    return wxControl::MSWOnNotify(idCtrl, lParam, result);
}

bool wxStyledTextCtrl::MSWCommand(WXUINT param, WXWORD id)
{
    if (param == SCEN_CHANGE)
    {
        NotifyChange();
        return true;
    }

    return wxControl::MSWCommand(param, id);
}

WXLRESULT wxStyledTextCtrl::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    switch (nMsg)
    {
        // Prevent wxWindowMSW::SetFocusFromKbd from selecting all text when
        // the frame activates or gains focus.
        case EM_SETSEL:
            return 0;

        // Let Scintilla handle its built-in popup menu rather than wxWindow
        // consuming the message.
        case WM_CONTEXTMENU:
        // Let Scintilla update its layout and scrollbar ranges on resize.
        case WM_SIZE:
            return MSWDefWindowProc(nMsg, wParam, lParam);

        default:
            break;
    }

    return wxControl::MSWWindowProc(nMsg, wParam, lParam);
}

class wxSTCModule : public wxModule
{
public:
    virtual bool OnInit() override { return true; }
    virtual void OnExit() override
    {
        Scintilla_ReleaseResources();
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxSTCModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxSTCModule, wxModule);

#endif // wxUSE_STC
