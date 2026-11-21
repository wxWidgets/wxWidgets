/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        buttonbox.cpp
// Purpose:     Part of the widgets sample showing wxButtonBox
// Author:      Wlodzimierz ABX Skiba, Assisted-by: Claude Sonnet 5.5
// Created:     12.11.26
// Copyright:   (c) 2026 Wlodzimierz ABX Skiba
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

#include "wx/wxprec.h"

#if wxUSE_BUTTONBOX

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"
    #include "wx/sizer.h"
    #include "wx/stattext.h"
    #include "wx/statbox.h"
    #include "wx/textctrl.h"
#endif

#include "wx/artprov.h"
#include "wx/buttonbox.h"
#include "wx/spinctrl.h"

#include "widgets.h"
#include "icons.h"


// ----------------------------------------------------------------------------
// ButtonBoxWidgetsPage
// ----------------------------------------------------------------------------

class ButtonBoxWidgetsPage : public WidgetsPage
{
public:
    ButtonBoxWidgetsPage(WidgetsBookCtrl *book,
                         wxVector<wxBitmapBundle>& imaglist)
        : WidgetsPage(book, imaglist, WidgetsIcon::ButtonBox)
    {
        m_radioMode = nullptr;
        m_chkBitmaps = nullptr;
        m_chkLabels = nullptr;
        m_textLabel = nullptr;
        m_spinIndex = nullptr;
        m_spinWidth = nullptr;
        m_sizerBox = nullptr;
        m_box = nullptr;
    }

    virtual void CreateContent() override;
    virtual wxWindow *GetWidget() const override { return m_box; }
    virtual void RecreateWidget() override;

private:
    void Reset();
    long GetModeStyle() const;
    void RecreateBox();
    void FillBox();
    void AddSegment(size_t pos, const wxString& label);
    wxBitmapBundle GetNextBitmap() const;
    int GetIndex() const { return m_spinIndex->GetValue(); }

    void OnReset(wxCommandEvent& event);
    void OnButtonBox(wxCommandEvent& event);

    wxRadioBox *m_radioMode;
    wxCheckBox *m_chkBitmaps,
               *m_chkLabels;
    wxTextCtrl *m_textLabel;
    wxSpinCtrl *m_spinIndex,
               *m_spinWidth;

    wxSizer *m_sizerBox;
    wxButtonBox *m_box;

    DECLARE_WIDGETS_PAGE(ButtonBoxWidgetsPage)
};

// ============================================================================
// implementation
// ============================================================================

#if defined(wxHAS_NATIVE_BUTTONBOX)
    #define FAMILY_CTRLS NATIVE_CTRLS
#else
    #define FAMILY_CTRLS GENERIC_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(ButtonBoxWidgetsPage, "ButtonBox",
                       FAMILY_CTRLS | WITH_ITEMS_CTRLS
                       );

namespace
{

enum
{
    ButtonBoxPage_Reset = wxID_HIGHEST + 1,
    ButtonBoxPage_Append,
    ButtonBoxPage_Insert,
    ButtonBoxPage_Delete,
    ButtonBoxPage_Clear,
    ButtonBoxPage_Select,
    ButtonBoxPage_Deselect,
    ButtonBoxPage_ToggleEnabled,
    ButtonBoxPage_SetLabel,
    ButtonBoxPage_SetWidth
};

const wxString g_modes[] =
{
    "select one",
    "select any",
    "momentary"
};

} // anonymous namespace

void ButtonBoxWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane: configuration
    wxSizer *sizerLeft = new wxBoxSizer(wxVERTICAL);

    m_radioMode = new wxRadioBox(this, wxID_ANY, "Selection &mode",
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(g_modes), g_modes, 1);
    sizerLeft->Add(m_radioMode, wxSizerFlags().Expand().Border(wxBOTTOM));

    m_chkLabels = CreateCheckBoxAndAddToSizer(sizerLeft, "Use &labels");
    m_chkLabels->SetValue(true);
    m_chkBitmaps = CreateCheckBoxAndAddToSizer(sizerLeft, "Use &bitmaps");

    sizerLeft->AddSpacer(10);
    sizerLeft->Add(new wxButton(this, ButtonBoxPage_Reset, "&Reset"),
                   wxSizerFlags().CenterHorizontal());

    // middle pane: operations on segments
    wxStaticBoxSizer *sizerOps = new wxStaticBoxSizer(wxVERTICAL, this, "&Segments");
    wxWindow* const box = sizerOps->GetStaticBox();

    wxFlexGridSizer *sizerParams = new wxFlexGridSizer(2, wxSize(5, 5));
    sizerParams->Add(new wxStaticText(box, wxID_ANY, "Index:"),
                     wxSizerFlags().CentreVertical());
    m_spinIndex = new wxSpinCtrl(box, wxID_ANY, "1", wxDefaultPosition,
                                 wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 1);
    sizerParams->Add(m_spinIndex);

    sizerParams->Add(new wxStaticText(box, wxID_ANY, "Label:"),
                     wxSizerFlags().CentreVertical());
    m_textLabel = new wxTextCtrl(box, wxID_ANY, "New");
    sizerParams->Add(m_textLabel);

    sizerParams->Add(new wxStaticText(box, wxID_ANY, "Width:"),
                     wxSizerFlags().CentreVertical());
    m_spinWidth = new wxSpinCtrl(box, wxID_ANY, "0", wxDefaultPosition,
                                 wxDefaultSize, wxSP_ARROW_KEYS, 0, 500, 0);
    m_spinWidth->SetToolTip("0 means automatic width");
    sizerParams->Add(m_spinWidth);
    sizerOps->Add(sizerParams, wxSizerFlags().Border());

    static const struct
    {
        int id;
        const char *label;
    } ops[] =
    {
        { ButtonBoxPage_Append,        "&Append" },
        { ButtonBoxPage_Insert,        "&Insert at index" },
        { ButtonBoxPage_Delete,        "&Delete index" },
        { ButtonBoxPage_Clear,         "&Clear all" },
        { ButtonBoxPage_Select,        "&Select index" },
        { ButtonBoxPage_Deselect,      "D&eselect index" },
        { ButtonBoxPage_ToggleEnabled, "Toggle e&nabled" },
        { ButtonBoxPage_SetLabel,      "Set l&abel" },
        { ButtonBoxPage_SetWidth,      "Set &width" },
    };

    for ( size_t n = 0; n < WXSIZEOF(ops); ++n )
    {
        sizerOps->Add(new wxButton(box, ops[n].id, ops[n].label),
                      wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM));
    }

    // right pane: the control itself (native under macOS, generic elsewhere)
    m_sizerBox = new wxBoxSizer(wxVERTICAL);

    m_box = new wxButtonBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                            GetModeStyle());
    m_sizerBox->AddStretchSpacer();
    m_sizerBox->Add(m_box, wxSizerFlags().Centre().Border());
    m_sizerBox->AddStretchSpacer(2);

    FillBox();

    sizerTop->Add(sizerLeft, wxSizerFlags().Expand().Border(wxALL, 10));
    sizerTop->Add(sizerOps, wxSizerFlags().Expand().Border(wxALL, 10));
    sizerTop->Add(m_sizerBox, wxSizerFlags(1).Expand().Border(wxALL, 10));

    SetSizer(sizerTop);

    // events
    Bind(wxEVT_BUTTON, &ButtonBoxWidgetsPage::OnReset, this, ButtonBoxPage_Reset);
    Bind(wxEVT_BUTTONBOX, &ButtonBoxWidgetsPage::OnButtonBox, this);
    Bind(wxEVT_RADIOBOX, [this](wxCommandEvent&) { RecreateBox(); },
         m_radioMode->GetId());
    Bind(wxEVT_CHECKBOX, [this](wxCommandEvent&) { RecreateBox(); },
         m_chkLabels->GetId());
    Bind(wxEVT_CHECKBOX, [this](wxCommandEvent&) { RecreateBox(); },
         m_chkBitmaps->GetId());

    Bind(wxEVT_BUTTON, [this](wxCommandEvent&)
    {
        AddSegment(m_box->GetCount(), m_textLabel->GetValue());
    }, ButtonBoxPage_Append);

    Bind(wxEVT_BUTTON, [this](wxCommandEvent&)
    {
        const size_t pos = GetIndex();
        if ( pos <= m_box->GetCount() )
            AddSegment(pos, m_textLabel->GetValue());
    }, ButtonBoxPage_Insert);

    Bind(wxEVT_BUTTON, [this](wxCommandEvent&)
    {
        const size_t n = GetIndex();
        if ( n < m_box->GetCount() )
            m_box->Delete(n);
    }, ButtonBoxPage_Delete);

    Bind(wxEVT_BUTTON, [this](wxCommandEvent&)
    {
        m_box->Clear();
    }, ButtonBoxPage_Clear);

    Bind(wxEVT_BUTTON, [this](wxCommandEvent&)
    {
        const size_t n = GetIndex();
        if ( n < m_box->GetCount() )
            m_box->SetSelected(n, true);
    }, ButtonBoxPage_Select);

    Bind(wxEVT_BUTTON, [this](wxCommandEvent&)
    {
        const size_t n = GetIndex();
        if ( n < m_box->GetCount() )
            m_box->SetSelected(n, false);
    }, ButtonBoxPage_Deselect);

    Bind(wxEVT_BUTTON, [this](wxCommandEvent&)
    {
        const size_t n = GetIndex();
        if ( n < m_box->GetCount() )
            m_box->EnableItem(n, !m_box->IsItemEnabled(n));
    }, ButtonBoxPage_ToggleEnabled);

    Bind(wxEVT_BUTTON, [this](wxCommandEvent&)
    {
        const size_t n = GetIndex();
        if ( n < m_box->GetCount() )
            m_box->SetItemLabel(n, m_textLabel->GetValue());
    }, ButtonBoxPage_SetLabel);

    Bind(wxEVT_BUTTON, [this](wxCommandEvent&)
    {
        const size_t n = GetIndex();
        const int w = m_spinWidth->GetValue();
        if ( n < m_box->GetCount() )
            m_box->SetItemWidth(n, w > 0 ? w : wxDefaultCoord);
    }, ButtonBoxPage_SetWidth);

    Reset();
}

void ButtonBoxWidgetsPage::Reset()
{
    m_radioMode->SetSelection(0);
    m_chkLabels->SetValue(true);
    m_chkBitmaps->SetValue(false);
    m_textLabel->SetValue("New");
    m_spinIndex->SetValue(1);
    m_spinWidth->SetValue(0);
}

long ButtonBoxWidgetsPage::GetModeStyle() const
{
    switch ( m_radioMode ? m_radioMode->GetSelection() : 0 )
    {
        case 1:  return wxBB_SELECT_ANY;
        case 2:  return wxBB_MOMENTARY;
        default: return wxBB_SELECT_ONE;
    }
}

wxBitmapBundle ButtonBoxWidgetsPage::GetNextBitmap() const
{
    static const wxArtID ids[] =
        { wxART_GO_BACK, wxART_GO_UP, wxART_GO_FORWARD, wxART_GO_DOWN };

    static size_t s_next = 0;
    return wxArtProvider::GetBitmapBundle(ids[s_next++ % WXSIZEOF(ids)],
                                          wxART_BUTTON);
}

void ButtonBoxWidgetsPage::AddSegment(size_t pos, const wxString& label)
{
    m_box->Insert(pos,
                  m_chkLabels->GetValue() ? label : wxString(),
                  m_chkBitmaps->GetValue() ? GetNextBitmap() : wxBitmapBundle());
    m_box->SetItemToolTip(pos, wxString::Format("Segment \"%s\"", label));
}

void ButtonBoxWidgetsPage::FillBox()
{
    static const char* const labels[] = { "Left", "Center", "Right" };

    for ( size_t n = 0; n < WXSIZEOF(labels); ++n )
        AddSegment(n, labels[n]);
}

void ButtonBoxWidgetsPage::RecreateBox()
{
    // The selection mode is a creation-time style, so the control has to be
    // replaced when it changes (the same is done for other style flags in the
    // other pages of this sample).
    wxButtonBox *box = new wxButtonBox(this, wxID_ANY, wxDefaultPosition,
                                       wxDefaultSize, GetModeStyle());
    m_sizerBox->Replace(m_box, box);
    delete m_box;
    m_box = box;

    FillBox();

    m_sizerBox->Layout();
}

void ButtonBoxWidgetsPage::RecreateWidget()
{
    RecreateBox();
}

void ButtonBoxWidgetsPage::OnReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();
    RecreateBox();
}

void ButtonBoxWidgetsPage::OnButtonBox(wxCommandEvent& event)
{
    wxLogMessage("wxButtonBox: segment %d (\"%s\") %s",
                 event.GetInt(),
                 event.GetString(),
                 event.GetExtraLong() ? "selected/clicked" : "deselected");
}

#endif // wxUSE_BUTTONBOX
