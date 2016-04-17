/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        headerctrl.cpp
// Purpose:     Part of the widgets sample showing wxHeaderCtrl
// Author:      Vadim Zeitlin
// Created:     2016-04-17
// Copyright:   (c) 2016 wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_HEADERCTRL

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/sizer.h"
    #include "wx/stattext.h"
#endif

#include "wx/headerctrl.h"

#include "widgets.h"

#include "icons/header.xpm"

// ----------------------------------------------------------------------------
// HeaderCtrlWidgetsPage
// ----------------------------------------------------------------------------

class HeaderCtrlWidgetsPage : public WidgetsPage
{
public:
    HeaderCtrlWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist)
        : WidgetsPage(book, imaglist, header_xpm)
    {
        m_header = NULL;
        m_sizerHeader = NULL;
    }

    virtual wxWindow *GetWidget() const wxOVERRIDE { return m_header; }
    virtual void RecreateWidget() wxOVERRIDE;

    // lazy creation of the content
    virtual void CreateContent() wxOVERRIDE;

protected:
    // the control itself and the sizer it is in
    wxHeaderCtrlSimple *m_header;
    wxSizer *m_sizerHeader;

private:
    DECLARE_WIDGETS_PAGE(HeaderCtrlWidgetsPage)
};

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    #define HEADER_CTRL_FAMILY NATIVE_CTRLS
#else
    #define HEADER_CTRL_FAMILY GENERIC_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(HeaderCtrlWidgetsPage,
                       wxT("Header"), HEADER_CTRL_FAMILY);

void HeaderCtrlWidgetsPage::CreateContent()
{
    m_sizerHeader = new wxStaticBoxSizer(wxVERTICAL, this, "Header");
    RecreateWidget();

    wxSizer* const sizerTop = new wxBoxSizer(wxHORIZONTAL);
    sizerTop->Add(m_sizerHeader, wxSizerFlags(1).Expand().DoubleBorder());

    SetSizer(sizerTop);
}

void HeaderCtrlWidgetsPage::RecreateWidget()
{
    m_sizerHeader->Clear(true /* delete windows */);

    int flags = GetAttrs().m_defaultFlags;

    flags |= wxHD_DEFAULT_STYLE;

    m_header = new wxHeaderCtrlSimple(this, wxID_ANY,
                                      wxDefaultPosition, wxDefaultSize,
                                      flags);
    m_header->AppendColumn(wxHeaderColumnSimple("First", 100));
    m_header->AppendColumn(wxHeaderColumnSimple("Second", 200));

    m_sizerHeader->AddStretchSpacer();
    m_sizerHeader->Add(m_header, wxSizerFlags().Expand());
    m_sizerHeader->AddStretchSpacer();
    m_sizerHeader->Layout();
}

#endif // wxUSE_HEADERCTRL
