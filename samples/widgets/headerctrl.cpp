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
    #include "wx/radiobox.h"
    #include "wx/sizer.h"
    #include "wx/statbox.h"
    #include "wx/stattext.h"
#endif

#include "wx/headerctrl.h"

#include "wx/artprov.h"

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
    void OnStyleCheckOrRadioBox(wxCommandEvent& evt);
    void OnResetButton(wxCommandEvent& evt);
    void OnUpdateUIResetButton(wxUpdateUIEvent& evt);

    // reset the header style
    void ResetHeaderStyle();
    // compose header style flags based on selections
    long GetHeaderStyleFlags() const;
    // reset column style
    void ResetColumnStyle(int col);
    // compose columnm style flags based on selections
    int GetColumnStyleFlags(int col) const;
    // get columnm alignment flags based on selection
    wxAlignment GetColumnAlignmentFlag(int col) const;

    // the control itself and the sizer it is in
    wxHeaderCtrlSimple *m_header;
    wxSizer *m_sizerHeader;
    // the check boxes for header styles
    wxCheckBox *m_chkAllowReorder;
    wxCheckBox *m_chkAllowHide;
    wxCheckBox *m_chkBitmapOnRight;
    // The check boxes for column styles
    struct
    {
        wxCheckBox *chkAllowResize;
        wxCheckBox *chkAllowReorder;
        wxCheckBox *chkAllowSort;
        wxCheckBox *chkAllowHide;
        wxCheckBox *chkWithBitmap;
        wxRadioBox *rbAlignments;
    } m_colSettings[2];

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

static const wxString gs_colAlignments[] = { "none", "left", "centre", "right" };
static const wxAlignment gs_colAlignFlags[] = { wxALIGN_NOT, wxALIGN_LEFT, wxALIGN_CENTRE, wxALIGN_RIGHT };
#define COL_WITH_BITMAP_DEFAULT     false
#define COL_ALIGNMENT_FLAG_DEFAULT  wxALIGN_NOT
#define COL_ALIGNMENT_INDEX_DEFAULT 0

void HeaderCtrlWidgetsPage::CreateContent()
{
    // left pane
    wxSizer *sizerLeft = new wxBoxSizer(wxVERTICAL);

    // header style
    wxSizer *sizerHeader = new wxStaticBoxSizer(wxVERTICAL, this, "&Header style");
    m_chkAllowReorder = CreateCheckBoxAndAddToSizer(sizerHeader, "Allow &reorder");
    m_chkAllowHide = CreateCheckBoxAndAddToSizer(sizerHeader, "Alow &hide");
    m_chkBitmapOnRight = CreateCheckBoxAndAddToSizer(sizerHeader, "&Bitmap on right");
    ResetHeaderStyle();
    sizerLeft->Add(sizerHeader, wxSizerFlags().Expand());

    // column flags
    for ( int i = 0; i < (int)WXSIZEOF(m_colSettings); i++ )
    {
        wxSizer* sizerCol = new wxStaticBoxSizer(wxVERTICAL, this, wxString::Format("Column %i style", i+1));
        m_colSettings[i].chkAllowResize = CreateCheckBoxAndAddToSizer(sizerCol, "Allow resize");
        m_colSettings[i].chkAllowReorder = CreateCheckBoxAndAddToSizer(sizerCol, "Allow reorder");
        m_colSettings[i].chkAllowSort = CreateCheckBoxAndAddToSizer(sizerCol, "Allow sort");
        m_colSettings[i].chkAllowHide = CreateCheckBoxAndAddToSizer(sizerCol, "Hidden");
        m_colSettings[i].chkWithBitmap = CreateCheckBoxAndAddToSizer(sizerCol, "With bitmap");
        m_colSettings[i].rbAlignments = new wxRadioBox(this, wxID_ANY, "Alignment",
               wxDefaultPosition, wxDefaultSize, WXSIZEOF(gs_colAlignments), gs_colAlignments,
               2, wxRA_SPECIFY_COLS);
        sizerCol->Add(m_colSettings[i].rbAlignments, wxSizerFlags().Expand().Border(wxALL, 5));
        ResetColumnStyle(i);
        sizerLeft->Add(sizerCol, wxSizerFlags().Expand().Border(wxTOP, 15));
    }

    sizerLeft->Add(5, 5, wxSizerFlags().Expand().Border(wxALL, 5)); // spacer
    wxButton* btnReset = new wxButton(this, wxID_ANY, "&Reset");
    sizerLeft->Add(btnReset, wxSizerFlags().CenterHorizontal().Border(wxALL, 15));

    // right pane
    m_sizerHeader = new wxStaticBoxSizer(wxVERTICAL, this, "Header");
    RecreateWidget();

    // the 2 panes compose the window
    wxSizer* sizerTop = new wxBoxSizer(wxHORIZONTAL);
    sizerTop->Add(sizerLeft, wxSizerFlags().Expand().DoubleBorder());
    sizerTop->Add(m_sizerHeader, wxSizerFlags(1).Expand().DoubleBorder());

    SetSizer(sizerTop);

    // Bind event handlers
    m_chkAllowReorder->Bind(wxEVT_CHECKBOX, &HeaderCtrlWidgetsPage::OnStyleCheckOrRadioBox, this);
    m_chkAllowHide->Bind(wxEVT_CHECKBOX, &HeaderCtrlWidgetsPage::OnStyleCheckOrRadioBox, this);
    m_chkBitmapOnRight->Bind(wxEVT_CHECKBOX, &HeaderCtrlWidgetsPage::OnStyleCheckOrRadioBox, this);
    for ( int i = 0; i < (int)WXSIZEOF(m_colSettings); i++ )
    {
        m_colSettings[i].chkAllowResize->Bind(wxEVT_CHECKBOX, &HeaderCtrlWidgetsPage::OnStyleCheckOrRadioBox, this);
        m_colSettings[i].chkAllowReorder->Bind(wxEVT_CHECKBOX, &HeaderCtrlWidgetsPage::OnStyleCheckOrRadioBox, this);
        m_colSettings[i].chkAllowSort->Bind(wxEVT_CHECKBOX, &HeaderCtrlWidgetsPage::OnStyleCheckOrRadioBox, this);
        m_colSettings[i].chkAllowHide->Bind(wxEVT_CHECKBOX, &HeaderCtrlWidgetsPage::OnStyleCheckOrRadioBox, this);
        m_colSettings[i].chkWithBitmap->Bind(wxEVT_CHECKBOX, &HeaderCtrlWidgetsPage::OnStyleCheckOrRadioBox, this);
        m_colSettings[i].rbAlignments->Bind(wxEVT_RADIOBOX, &HeaderCtrlWidgetsPage::OnStyleCheckOrRadioBox, this);
    }
    btnReset->Bind(wxEVT_BUTTON, &HeaderCtrlWidgetsPage::OnResetButton, this);
    btnReset->Bind(wxEVT_UPDATE_UI, &HeaderCtrlWidgetsPage::OnUpdateUIResetButton, this);
}

void HeaderCtrlWidgetsPage::RecreateWidget()
{
    m_sizerHeader->Clear(true /* delete windows */);

    long flags = GetAttrs().m_defaultFlags | GetHeaderStyleFlags();

    m_header = new wxHeaderCtrlSimple(this, wxID_ANY,
                                      wxDefaultPosition, wxDefaultSize,
                                      flags);
    wxASSERT(WXSIZEOF(m_colSettings) == 2);
    wxHeaderColumnSimple col1("First", 100, GetColumnAlignmentFlag(0), GetColumnStyleFlags(0));
    if ( m_colSettings[0].chkWithBitmap->IsChecked() )
    {
        col1.SetBitmap(wxArtProvider::GetIcon(wxART_ERROR, wxART_BUTTON));
    }
    m_header->AppendColumn(col1);
    wxHeaderColumnSimple col2("Second", 200, GetColumnAlignmentFlag(1), GetColumnStyleFlags(1));
    if ( m_colSettings[1].chkWithBitmap->IsChecked() )
    {
        col2.SetBitmap(wxArtProvider::GetIcon(wxART_QUESTION, wxART_BUTTON));
    }
    m_header->AppendColumn(col2);

    m_sizerHeader->AddStretchSpacer();
    m_sizerHeader->Add(m_header, wxSizerFlags().Expand());
    m_sizerHeader->AddStretchSpacer();
    m_sizerHeader->Layout();
}

void HeaderCtrlWidgetsPage::ResetHeaderStyle()
{
    m_chkAllowReorder->SetValue((wxHD_DEFAULT_STYLE & wxHD_ALLOW_REORDER) != 0);
    m_chkAllowHide->SetValue((wxHD_DEFAULT_STYLE & wxHD_ALLOW_HIDE) != 0);
    m_chkBitmapOnRight->SetValue((wxHD_DEFAULT_STYLE & wxHD_BITMAP_ON_RIGHT) != 0);
}

long HeaderCtrlWidgetsPage::GetHeaderStyleFlags() const
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

void HeaderCtrlWidgetsPage::ResetColumnStyle(int col)
{
    wxASSERT(col < (int)WXSIZEOF(m_colSettings));
    m_colSettings[col].chkAllowResize->SetValue((wxCOL_DEFAULT_FLAGS & wxCOL_RESIZABLE) != 0);
    m_colSettings[col].chkAllowReorder->SetValue((wxCOL_DEFAULT_FLAGS & wxCOL_REORDERABLE) != 0);
    m_colSettings[col].chkAllowSort->SetValue((wxCOL_DEFAULT_FLAGS & wxCOL_SORTABLE) != 0);
    m_colSettings[col].chkAllowHide->SetValue((wxCOL_DEFAULT_FLAGS & wxCOL_HIDDEN) != 0);
    m_colSettings[col].chkWithBitmap->SetValue(COL_WITH_BITMAP_DEFAULT);
    m_colSettings[col].rbAlignments->SetSelection(COL_ALIGNMENT_INDEX_DEFAULT);
}

int HeaderCtrlWidgetsPage::GetColumnStyleFlags(int col) const
{
    wxASSERT(col < (int)WXSIZEOF(m_colSettings));
    int flags = 0;

    if ( m_colSettings[col].chkAllowResize->IsChecked() )
        flags |= wxCOL_RESIZABLE;
    if ( m_colSettings[col].chkAllowReorder->IsChecked() )
        flags |= wxCOL_REORDERABLE;
    if ( m_colSettings[col].chkAllowSort->IsChecked() )
        flags |= wxCOL_SORTABLE;
    if ( m_colSettings[col].chkAllowHide->IsChecked() )
        flags |= wxCOL_HIDDEN;

    return flags;
}

wxAlignment HeaderCtrlWidgetsPage::GetColumnAlignmentFlag(int col) const
{
    wxASSERT(col < (int)WXSIZEOF(m_colSettings));
    wxASSERT(WXSIZEOF(gs_colAlignments) == WXSIZEOF(gs_colAlignFlags));
    int sel = m_colSettings[col].rbAlignments->GetSelection();
    return sel == wxNOT_FOUND ? COL_ALIGNMENT_FLAG_DEFAULT : gs_colAlignFlags[sel];
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void HeaderCtrlWidgetsPage::OnStyleCheckOrRadioBox(wxCommandEvent& WXUNUSED(evt))
{
    RecreateWidget();
}

void HeaderCtrlWidgetsPage::OnResetButton(wxCommandEvent& WXUNUSED(evt))
{
    ResetHeaderStyle();
    for ( int i = 0; i < (int)WXSIZEOF(m_colSettings); i++ )
    {
        ResetColumnStyle(i);
    }
    RecreateWidget();
}

void HeaderCtrlWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& evt)
{
    bool enable = GetHeaderStyleFlags() != wxHD_DEFAULT_STYLE;
    for ( int i = 0; !enable && i < (int)WXSIZEOF(m_colSettings); i++ )
    {
        enable = enable || GetColumnStyleFlags(i) != wxCOL_DEFAULT_FLAGS
                 || m_colSettings[i].chkWithBitmap->IsChecked() != COL_WITH_BITMAP_DEFAULT
                 || m_colSettings[i].rbAlignments->GetSelection() != COL_ALIGNMENT_INDEX_DEFAULT;
    }
    evt.Enable(enable);
}

#endif // wxUSE_HEADERCTRL
