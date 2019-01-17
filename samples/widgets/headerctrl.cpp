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
    #include "wx/checkbox.h"
    #include "wx/sizer.h"
    #include "wx/statbox.h"
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
    // event handlers
    void OnStyleCheckBox(wxCommandEvent& evt);
    void OnResetButton(wxCommandEvent& evt);
    void OnUpdateUIResetButton(wxUpdateUIEvent& evt);

    // reset the header styles
    void Reset();
    // compose style flags based on selected styles
    long GetStyleFlags() const;

    // the control itself and the sizer it is in
    wxHeaderCtrlSimple *m_header;
    wxSizer *m_sizerHeader;
    // the check boxes for styles
    wxCheckBox *m_chkAllowReorder;
    wxCheckBox *m_chkAllowHide;
    wxCheckBox *m_chkBitmapOnRight;

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
                       "Header", HEADER_CTRL_FAMILY);

void HeaderCtrlWidgetsPage::CreateContent()
{
    // left pane
    wxStaticBox* box = new wxStaticBox(this, wxID_ANY, "&Set style");
    wxSizer* sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkAllowReorder = CreateCheckBoxAndAddToSizer(sizerLeft, "Allow &reorder");
    m_chkAllowHide = CreateCheckBoxAndAddToSizer(sizerLeft, "Alow &hide");
    m_chkBitmapOnRight = CreateCheckBoxAndAddToSizer(sizerLeft, "&Bitmap on right");

    sizerLeft->Add(5, 5, wxSizerFlags().Expand().Border(wxALL, 5)); // spacer

    wxButton* btnReset = new wxButton(this, wxID_ANY, "&Reset");
    sizerLeft->Add(btnReset, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // right pane
    m_sizerHeader = new wxStaticBoxSizer(wxVERTICAL, this, "Header");
    Reset();
    RecreateWidget();

    // the 2 panes compose the window
    wxSizer* sizerTop = new wxBoxSizer(wxHORIZONTAL);
    sizerTop->Add(sizerLeft, wxSizerFlags().Expand().DoubleBorder());
    sizerTop->Add(m_sizerHeader, wxSizerFlags(1).Expand().DoubleBorder());

    SetSizer(sizerTop);

    // Bind event handlers
    m_chkAllowReorder->Bind(wxEVT_CHECKBOX, &HeaderCtrlWidgetsPage::OnStyleCheckBox, this);
    m_chkAllowHide->Bind(wxEVT_CHECKBOX, &HeaderCtrlWidgetsPage::OnStyleCheckBox, this);
    m_chkBitmapOnRight->Bind(wxEVT_CHECKBOX, &HeaderCtrlWidgetsPage::OnStyleCheckBox, this);
    btnReset->Bind(wxEVT_BUTTON, &HeaderCtrlWidgetsPage::OnResetButton, this);
    btnReset->Bind(wxEVT_UPDATE_UI, &HeaderCtrlWidgetsPage::OnUpdateUIResetButton, this);
}

void HeaderCtrlWidgetsPage::RecreateWidget()
{
    m_sizerHeader->Clear(true /* delete windows */);

    long flags = GetAttrs().m_defaultFlags | GetStyleFlags();

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

void HeaderCtrlWidgetsPage::Reset()
{
    m_chkAllowReorder->SetValue((wxHD_DEFAULT_STYLE & wxHD_ALLOW_REORDER) != 0);
    m_chkAllowHide->SetValue((wxHD_DEFAULT_STYLE & wxHD_ALLOW_HIDE) != 0);
    m_chkBitmapOnRight->SetValue((wxHD_DEFAULT_STYLE & wxHD_BITMAP_ON_RIGHT) != 0);
}

long HeaderCtrlWidgetsPage::GetStyleFlags() const
{
    long flags = 0;

    if ( m_chkAllowReorder->IsChecked() )
        flags |= wxHD_ALLOW_REORDER;
    if ( m_chkAllowHide->IsChecked() )
        flags |= wxHD_ALLOW_HIDE;
    if ( m_chkBitmapOnRight->IsChecked() )
        flags |= wxHD_BITMAP_ON_RIGHT;

    return flags;
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void HeaderCtrlWidgetsPage::OnStyleCheckBox(wxCommandEvent& WXUNUSED(evt))
{
    RecreateWidget();
}

void HeaderCtrlWidgetsPage::OnResetButton(wxCommandEvent& WXUNUSED(evt))
{
    Reset();
    RecreateWidget();
}

void HeaderCtrlWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& evt)
{
    evt.Enable(GetStyleFlags() != wxHD_DEFAULT_STYLE);
}

#endif // wxUSE_HEADERCTRL
