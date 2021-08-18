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
    m_InfoFrame = new wxFrame(flags.m_parent, wxID_ANY, wxString(),
                              wxDefaultPosition, wxDefaultSize,
                              wxSIMPLE_BORDER |
                              wxFRAME_TOOL_WINDOW |
                              wxSTAY_ON_TOP);

    wxPanel* const panel = new wxPanel(m_InfoFrame);

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
#if wxUSE_MARKUP
        title->SetLabelMarkup(flags.m_title);
#else
        title->SetLabelText(flags.m_title);
#endif

        sizer->Add(title, wxSizerFlags().DoubleBorder().Expand());
    }
    else
    {
        title = NULL;
    }

    // Vertically center the text in the window.
    sizer->AddStretchSpacer();

#if wxUSE_MARKUP
    m_text = new wxStaticTextWithMarkupSupport(panel, wxID_ANY, wxString(),
                                               wxDefaultPosition,
                                               wxDefaultSize,
                                               wxALIGN_CENTRE);
    if ( !flags.m_text.empty() )
        m_text->SetLabelMarkup(flags.m_text);
    else
        m_text->SetLabelText(flags.m_label);
#else
    m_text = new wxStaticText(panel, wxID_ANY, wxString());
    m_text->SetLabelText(flags.m_label);
#endif // wxUSE_MARKUP

    sizer->Add(m_text, wxSizerFlags().DoubleBorder().Centre());

    sizer->AddStretchSpacer();

    panel->SetSizer(sizer);

    if ( flags.m_foreground.IsOk() )
    {
        if ( title )
            title->SetForegroundColour(flags.m_foreground);
        m_text->SetForegroundColour(flags.m_foreground);
    }

    if ( flags.m_background.IsOk() )
        panel->SetBackgroundColour(flags.m_background);

    if ( flags.m_alpha != wxALPHA_OPAQUE )
        m_InfoFrame->SetTransparent(flags.m_alpha);

    m_InfoFrame->SetCursor(*wxHOURGLASS_CURSOR);

    // We need to accommodate our contents, but also impose some minimal size
    // to make the busy info frame more noticeable.
    wxSize size = panel->GetBestSize();
    size.IncTo(wxSize(400, 80));

    m_InfoFrame->SetClientSize(size);
    m_InfoFrame->Layout();

    m_InfoFrame->Centre(wxBOTH);
    m_InfoFrame->Show(true);
    m_InfoFrame->Refresh();
    m_InfoFrame->Update();
}

void wxBusyInfo::UpdateText(const wxString& str)
{
    m_text->SetLabelMarkup(str);
}

void wxBusyInfo::UpdateLabel(const wxString& str)
{
    m_text->SetLabelText(str);
}

wxBusyInfo::~wxBusyInfo()
{
    m_InfoFrame->Show(false);
    m_InfoFrame->Close();
}

#endif // wxUSE_BUSYINFO
