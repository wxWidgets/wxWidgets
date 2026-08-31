/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/busyinfo.cpp
// Purpose:     Information window when app is busy
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_BUSYINFO

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/busyinfo.h"
    #include "wx/stattext.h"
    #include "wx/panel.h"
    #include "wx/frame.h"
    #include "wx/sizer.h"
    #include "wx/statbmp.h"
    #include "wx/utils.h"
#endif

#include "wx/busyinfo.h"

// wxStaticText currently supports markup only in wxGTK and wxOSX/Cocoa, so use
// the generic version for markup support in the other ports.
#if wxUSE_MARKUP && !(defined(__WXGTK__) || defined(__WXOSX_COCOA__))
    #include "wx/generic/stattextg.h"

    #define wxStaticTextWithMarkupSupport wxGenericStaticText
#else
    #define wxStaticTextWithMarkupSupport wxStaticText
#endif

void wxBusyInfo::Init(const wxBusyInfoFlags& flags)
{
    wxFrame * const infoFrame =
        new wxFrame(flags.m_parent, wxID_ANY, wxString(),
                    wxDefaultPosition, wxDefaultSize,
                    wxSIMPLE_BORDER |
                    wxFRAME_TOOL_WINDOW |
                    wxSTAY_ON_TOP);
    m_InfoFrame = infoFrame;

    wxPanel* const panel = new wxPanel(infoFrame);

    wxBoxSizer* const sizer = new wxBoxSizer(wxVERTICAL);

    if ( flags.m_icon.IsOk() )
    {
        sizer->Add(new wxStaticBitmap(panel, wxID_ANY, flags.m_icon),
                   wxSizerFlags().DoubleBorder().Centre());
    }

    wxControl* title;
    if ( !flags.m_title.empty() )
    {
        title = new wxStaticTextWithMarkupSupport(panel, wxID_ANY, wxString(),
                                                  wxDefaultPosition,
                                                  wxDefaultSize,
                                                  wxALIGN_CENTRE);
        title->SetFont(title->GetFont().Scaled(2));

#ifdef __WXGTK__
        // This bad hack is needed to fix layout under GTK: the font sent above
        // is not taken into account for the size calculation until the window
        // is shown but we need the correct size when computing the best size
        // below, as otherwise we would make the entire frame too small and
        // when the correct size is used for the actual layout later, the title
        // control would take too much space pushing the text below it outside
        // of the window bounds.
        //
        // So preemptively make it about as big as it's going to be to prevent
        // this from happening.
        title->SetMinSize(2*title->GetBestSize());
#endif // __WXGTK__

#if wxUSE_MARKUP
        title->SetLabelMarkup(flags.m_title);
#else
        title->SetLabelText(flags.m_title);
#endif

        sizer->Add(title, wxSizerFlags().DoubleBorder().Expand());
    }
    else
    {
        title = nullptr;
    }

    // Vertically center the text in the window.
    sizer->AddStretchSpacer();

#if wxUSE_MARKUP
    wxControl * const text =
        new wxStaticTextWithMarkupSupport(panel, wxID_ANY, wxString(),
                                          wxDefaultPosition,
                                          wxDefaultSize,
                                          wxALIGN_CENTRE);
    m_text = text;
    if ( !flags.m_text.empty() )
        text->SetLabelMarkup(flags.m_text);
    else
        text->SetLabelText(flags.m_label);
#else
    wxControl * const text =
        new wxStaticText(panel, wxID_ANY, wxString());
    m_text = text;
    text->SetLabelText(flags.m_label);
#endif // wxUSE_MARKUP

    sizer->Add(text, wxSizerFlags().DoubleBorder().Centre());

    sizer->AddStretchSpacer();

    panel->SetSizer(sizer);

    if ( flags.m_foreground.IsOk() )
    {
        if ( title )
            title->SetForegroundColour(flags.m_foreground);
        text->SetForegroundColour(flags.m_foreground);
    }

    if ( flags.m_background.IsOk() )
        panel->SetBackgroundColour(flags.m_background);

    if ( flags.m_alpha != wxALPHA_OPAQUE )
        infoFrame->SetTransparent(flags.m_alpha);

    infoFrame->SetCursor(*wxHOURGLASS_CURSOR);

    // We need to accommodate our contents, but also impose some minimal size
    // to make the busy info frame more noticeable.
    wxSize size = panel->GetBestSize();
    size.IncTo(wxSize(400, 80));

    infoFrame->SetClientSize(size);
    infoFrame->Layout();

    infoFrame->Centre(wxBOTH);
#if defined(__WXWINUI__) && wxUSE_WINUI3
    // Busy information is feedback, not a new interaction surface. Keep the
    // user's active control while still showing the transient TLW.
    infoFrame->ShowWithoutActivating();
#else
    infoFrame->Show(true);
#endif
    infoFrame->Refresh();
    infoFrame->Update();
}

void wxBusyInfo::UpdateText(const wxString& str)
{
    wxControl * const text = m_text.get();
    if ( !text )
        return;

#if wxUSE_MARKUP
    text->SetLabelMarkup(str);
#else // !wxUSE_MARKUP
    text->SetLabelText(str);
#endif // wxUSE_MARKUP/!wxUSE_MARKUP

    RefitAfterTextUpdate();
}

void wxBusyInfo::UpdateLabel(const wxString& str)
{
    wxControl * const text = m_text.get();
    if ( !text )
        return;

    text->SetLabelText(str);
    RefitAfterTextUpdate();
}

void wxBusyInfo::RefitAfterTextUpdate()
{
    wxFrame * const infoFrame = m_InfoFrame.get();
    wxControl * const text = m_text.get();
    if ( !infoFrame || !text )
        return;

    text->InvalidateBestSize();

    wxWindow * const panel = text->GetParent();
    if ( !panel )
        return;

    // Realized WinUI templates can make the updated label larger than its
    // initial estimate. Refit grow-only: long updates are never cropped and a
    // later short update cannot make this transient jump back and forth.
    panel->InvalidateBestSize();
    wxSize size = panel->GetBestSize();
    size.IncTo(wxSize(400, 80));
    size.IncTo(infoFrame->GetClientSize());
    infoFrame->SetClientSize(size);
    panel->Layout();
    infoFrame->Layout();
    infoFrame->Refresh();
    infoFrame->Update();
}

wxBusyInfo::~wxBusyInfo()
{
    wxFrame * const infoFrame = m_InfoFrame.get();

    // Release both trackers before requesting destruction: the frame may be
    // destroyed synchronously by a port or may already have gone away with
    // its owner.
    m_text.Release();
    m_InfoFrame.Release();

    if ( infoFrame )
    {
        infoFrame->Hide();
        infoFrame->Destroy();
    }
}

#endif // wxUSE_BUSYINFO
