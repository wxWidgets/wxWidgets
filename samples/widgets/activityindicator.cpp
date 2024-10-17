/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        activityindicator.cpp
// Purpose:     Part of the widgets sample showing wxActivityIndicator
// Author:      Vadim Zeitlin
// Created:     2015-03-06
// Copyright:   (c) 2015 wxWindows team
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


#if wxUSE_ACTIVITYINDICATOR

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/sizer.h"
    #include "wx/statbox.h"
    #include "wx/stattext.h"
#endif

#include "wx/activityindicator.h"

#include "widgets.h"

#include "icons/activityindicator.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// Control IDs
enum
{
    ActivityIndicator_Start = wxID_HIGHEST,
    ActivityIndicator_Stop,
    ActivityIndicator_IsRunning
};

// ----------------------------------------------------------------------------
// ActivityIndicatorWidgetsPage
// ----------------------------------------------------------------------------

class ActivityIndicatorWidgetsPage : public WidgetsPage
{
public:
    ActivityIndicatorWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist)
        : WidgetsPage(book, imaglist, activityindicator_xpm)
    {
        m_indicator = nullptr;
        m_sizerIndicator = nullptr;
    }

    virtual wxWindow *GetWidget() const override { return m_indicator; }
    virtual void RecreateWidget() override;

    // lazy creation of the content
    virtual void CreateContent() override;

protected:
    void OnButtonStart(wxCommandEvent&) { m_indicator->Start(); }
    void OnButtonStop(wxCommandEvent&) { m_indicator->Stop(); }

    void OnUpdateIsRunning(wxUpdateUIEvent& event)
    {
        event.SetText(m_indicator && m_indicator->IsRunning()
                        ? "Indicator is running"
                        : "Indicator is stopped");
    }

    // the indicator itself and the sizer it is in
    wxActivityIndicator *m_indicator;
    wxStaticBoxSizer *m_sizerIndicator;

private:
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(ActivityIndicatorWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(ActivityIndicatorWidgetsPage, WidgetsPage)
    EVT_BUTTON(ActivityIndicator_Start,
               ActivityIndicatorWidgetsPage::OnButtonStart)
    EVT_BUTTON(ActivityIndicator_Stop,
               ActivityIndicatorWidgetsPage::OnButtonStop)

    EVT_UPDATE_UI(ActivityIndicator_IsRunning,
                  ActivityIndicatorWidgetsPage::OnUpdateIsRunning)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(ActivityIndicatorWidgetsPage,
                       "ActivityIndicator", NATIVE_CTRLS);

void ActivityIndicatorWidgetsPage::CreateContent()
{
    wxStaticBoxSizer* const sizerOper = new wxStaticBoxSizer(wxVERTICAL, this,
                                                    "&Operations");
    wxStaticBox* const sizerOperBox = sizerOper->GetStaticBox();

    sizerOper->Add(new wxButton(sizerOperBox, ActivityIndicator_Start, "&Start"),
                   wxSizerFlags().Expand().Border());
    sizerOper->Add(new wxButton(sizerOperBox, ActivityIndicator_Stop, "&Stop"),
                   wxSizerFlags().Expand().Border());

    sizerOper->Add(new wxStaticText(sizerOperBox, ActivityIndicator_IsRunning,
                                    "Indicator is initializing..."),
                   wxSizerFlags().Expand().Border());


    m_sizerIndicator = new wxStaticBoxSizer(wxHORIZONTAL, this,
                                            "Activity Indicator");
    RecreateWidget();


    wxSizer* const sizerTop = new wxBoxSizer(wxHORIZONTAL);
    sizerTop->Add(sizerOper, wxSizerFlags().Expand().DoubleBorder());
    sizerTop->Add(m_sizerIndicator, wxSizerFlags(1).Expand().DoubleBorder());

    SetSizer(sizerTop);
}

void ActivityIndicatorWidgetsPage::RecreateWidget()
{
    m_sizerIndicator->Clear(true /* delete windows */);

    m_indicator = new wxActivityIndicator(m_sizerIndicator->GetStaticBox(), wxID_ANY,
                                          wxDefaultPosition, wxDefaultSize,
                                          GetAttrs().m_defaultFlags);

    NotifyWidgetRecreation(m_indicator);

    m_sizerIndicator->AddStretchSpacer();
    m_sizerIndicator->Add(m_indicator, wxSizerFlags().Centre());
    m_sizerIndicator->AddStretchSpacer();
    m_sizerIndicator->Layout();
}

#endif // wxUSE_ACTIVITYINDICATOR
