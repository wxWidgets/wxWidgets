/////////////////////////////////////////////////////////////////////////////
// Name:        life/dialogs.cpp
// Purpose:     Life! dialogs
// Author:      Guillermo Rodriguez Garcia, <guille@iies.es>
// Modified by:
// Created:     Jan/2000
// Copyright:   (c) 2000, Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ==========================================================================
// headers, declarations, constants
// ==========================================================================

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/statline.h"
#include "wx/minifram.h"
#include "wx/settings.h"

#include "dialogs.h"
#include "life.h"
#include "game.h"


// --------------------------------------------------------------------------
// resources
// --------------------------------------------------------------------------

#include "bitmaps/life.xpm"

// sample configurations
#include "samples.inc"

// --------------------------------------------------------------------------
// constants
// --------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // listbox in samples dialog
    ID_LISTBOX
};

// --------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// --------------------------------------------------------------------------

// Event tables
wxBEGIN_EVENT_TABLE(LifeSamplesDialog, wxDialog)
    EVT_LISTBOX (ID_LISTBOX, LifeSamplesDialog::OnListBox)
wxEND_EVENT_TABLE()


// ==========================================================================
// implementation
// ==========================================================================

// --------------------------------------------------------------------------
// LifeSamplesDialog
// --------------------------------------------------------------------------

LifeSamplesDialog::LifeSamplesDialog(wxWindow *parent)
                 : wxDialog(parent, wxID_ANY, _("Sample games"),
                            wxDefaultPosition, wxDefaultSize)
{
    m_value = 0;

    wxSize listSize = wxDefaultSize;
    bool isPDA = wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA;

    // Screens are generally horizontal in orientation,
    // but PDAs are generally vertical.
    bool screenIsHorizontal = true;
    if (isPDA &&
        wxSystemSettings::GetMetric(wxSYS_SCREEN_X) < wxSystemSettings::GetMetric(wxSYS_SCREEN_Y))
    {
        listSize = wxSize(wxDefaultCoord, 50);
        screenIsHorizontal = false;
    }

    // create and populate the list of available samples
    m_list = new wxListBox( this, ID_LISTBOX,
        wxDefaultPosition,
        listSize,
        0, nullptr,
        wxLB_SINGLE | wxLB_NEEDED_SB | wxLB_HSCROLL );

    for (unsigned i = 0; i < (sizeof(g_patterns) / sizeof(LifePattern)); i++)
        m_list->Append(g_patterns[i].m_name);

    // descriptions
    wxStaticBox *statbox = new wxStaticBox( this, wxID_ANY, _("Description"));
    m_life   = new Life();
    m_life->SetPattern(g_patterns[0]);
    m_canvas = new LifeCanvas( this, m_life, false );
    m_text   = new wxTextCtrl( this, wxID_ANY,
        g_patterns[0].m_description,
        wxDefaultPosition,
        wxSize(300, 60),
        wxTE_MULTILINE | wxTE_READONLY);

    // layout components

    wxStaticBoxSizer *sizer1 = new wxStaticBoxSizer( statbox, wxVERTICAL );
    sizer1->Add( m_canvas, 2, wxGROW | wxALL, 5);
    sizer1->Add( m_text, 1, wxGROW | wxALL, 5 );

    wxBoxSizer *sizer2 = new wxBoxSizer( screenIsHorizontal ? wxHORIZONTAL : wxVERTICAL );
    sizer2->Add( m_list, 0, wxGROW | wxALL, 5 );
    sizer2->Add( sizer1, 1, wxGROW | wxALL, 5 );

    wxBoxSizer *sizer3 = new wxBoxSizer( wxVERTICAL );
    sizer3->Add( CreateTextSizer(_("Select a configuration")), 0, wxALL|wxCENTRE, isPDA ? 2 : 10 );
#if wxUSE_STATLINE
    if (!isPDA)
        sizer3->Add( new wxStaticLine(this, wxID_ANY), 0, wxGROW | wxLEFT | wxRIGHT, 10 );
#endif // wxUSE_STATLINE
    sizer3->Add( sizer2, 1, wxGROW | wxALL, 5 );

    wxSizer *sizerBtns = CreateButtonSizer(wxOK|wxCANCEL);
    if ( sizerBtns )
    {
        sizer3->Add(sizerBtns, wxSizerFlags().Expand().Border());
    }

    // activate
    SetSizer(sizer3);
    sizer3->SetSizeHints(this);
    sizer3->Fit(this);
    Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

LifeSamplesDialog::~LifeSamplesDialog()
{
    m_canvas->Destroy();
}

const LifePattern& LifeSamplesDialog::GetPattern()
{
    return g_patterns[m_value];
}

void LifeSamplesDialog::OnListBox(wxCommandEvent& event)
{
    int sel = event.GetSelection();

    if (sel != -1)
    {
        m_value = m_list->GetSelection();
        m_text->SetValue(g_patterns[ sel ].m_description);
        m_life->SetPattern(g_patterns[ sel ]);

        // these values shouldn't be hardcoded...
        if ((size_t)sel < (sizeof(g_patterns) / sizeof(LifePattern)) - 3)
            m_canvas->SetCellSize(8);
        else
            m_canvas->SetCellSize(2);
    }
}

// --------------------------------------------------------------------------
// LifeAboutDialog
// --------------------------------------------------------------------------

LifeAboutDialog::LifeAboutDialog(wxWindow *parent)
               : wxDialog(parent, wxID_ANY, _("About Life!"),
                          wxDefaultPosition, wxDefaultSize)
{
    // logo
    wxStaticBitmap *sbmp = new wxStaticBitmap(this, wxID_ANY, wxBitmap(life_xpm));

    // layout components
    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    sizer->Add( sbmp, 0, wxCENTRE | wxALL, 10 );
#if wxUSE_STATLINE
    sizer->Add( new wxStaticLine(this, wxID_ANY), 0, wxGROW | wxLEFT | wxRIGHT, 5 );
#endif // wxUSE_STATLINE
    sizer->Add( CreateTextSizer(_("Life! version 2.2 for wxWidgets\n\n\
(c) 2000 Guillermo Rodriguez Garcia\n\n\
<guille@iies.es>\n\n\
Portions of the code are based in XLife;\n\
XLife is (c) 1989 by Jon Bennett et al.")),
                                  0, wxCENTRE | wxRIGHT|wxLEFT|wxTOP, 20 );

    // buttons if any
    wxSizer *sizerBtns = CreateButtonSizer(wxOK);
    if ( sizerBtns )
    {
        sizer->Add(sizerBtns, wxSizerFlags().Expand().Border());
    }

    // activate
    SetSizer(sizer);
    sizer->SetSizeHints(this);
    sizer->Fit(this);
    Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}
