/////////////////////////////////////////////////////////////////////////////
// Name:        dialogs.cpp
// Purpose:     Life! dialogs
// Author:      Guillermo Rodriguez Garcia, <guille@iies.es>
// Modified by:
// Created:     Jan/2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000, Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ==========================================================================
// declarations
// ==========================================================================

// --------------------------------------------------------------------------
// headers
// --------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "dialogs.h"
#endif

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/statline.h"
#include "wx/spinctrl.h"

#include "dialogs.h"
#include "life.h"
#include "game.h"

// --------------------------------------------------------------------------
// constants
// --------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // listbox in samples dialog
    ID_LISTBOX = 2001
};

// sample configurations
#include "samples.inc"

// --------------------------------------------------------------------------
// event tables and other macros for wxWindows
// --------------------------------------------------------------------------

// Event tables
BEGIN_EVENT_TABLE(LifeNewGameDialog, wxDialog)
    EVT_BUTTON  (wxID_OK,    LifeNewGameDialog::OnOK)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(LifeSamplesDialog, wxDialog)
    EVT_LISTBOX (ID_LISTBOX, LifeSamplesDialog::OnListBox)
END_EVENT_TABLE()


// ==========================================================================
// implementation
// ==========================================================================

// --------------------------------------------------------------------------
// LifeNewGameDialog
// --------------------------------------------------------------------------

LifeNewGameDialog::LifeNewGameDialog(wxWindow *parent, int *w, int *h)
                 : wxDialog(parent, -1,
                            _("New game"),
                            wxDefaultPosition,
                            wxDefaultSize,
                            wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL)
{
    m_w = w;
    m_h = h;

    // spin ctrls
    m_spinctrlw = new wxSpinCtrl( this, -1 );
    m_spinctrlw->SetValue(*m_w);
    m_spinctrlw->SetRange(LIFE_MIN, LIFE_MAX);

    m_spinctrlh = new wxSpinCtrl( this, -1 );
    m_spinctrlh->SetValue(*m_h);
    m_spinctrlh->SetRange(LIFE_MIN, LIFE_MAX);

    // component layout
    wxBoxSizer *inputsizer1 = new wxBoxSizer( wxHORIZONTAL );
    inputsizer1->Add( new wxStaticText(this, -1, _("Width")), 1, wxCENTRE | wxLEFT, 20);
    inputsizer1->Add( m_spinctrlw, 2, wxCENTRE | wxLEFT | wxRIGHT, 20 );

    wxBoxSizer *inputsizer2 = new wxBoxSizer( wxHORIZONTAL );
    inputsizer2->Add( new wxStaticText(this, -1, _("Height")), 1, wxCENTRE | wxLEFT, 20);
    inputsizer2->Add( m_spinctrlh, 2, wxCENTRE | wxLEFT | wxRIGHT, 20 );

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
    topsizer->Add( CreateTextSizer(_("Enter board dimensions")), 0, wxALL, 10 );
    topsizer->Add( new wxStaticLine(this, -1), 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    topsizer->Add( inputsizer1, 1, wxGROW | wxLEFT | wxRIGHT, 5 );
    topsizer->Add( inputsizer2, 1, wxGROW | wxLEFT | wxRIGHT, 5 );
    topsizer->Add( new wxStaticLine(this, -1), 0, wxGROW | wxLEFT | wxRIGHT | wxTOP, 10);
    topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxCENTRE | wxALL, 10);

    // activate
    SetSizer(topsizer);
    SetAutoLayout(TRUE);
    topsizer->SetSizeHints(this);
    topsizer->Fit(this);
    Centre(wxBOTH);
}

void LifeNewGameDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    *m_w = m_spinctrlw->GetValue();
    *m_h = m_spinctrlh->GetValue();

    EndModal(wxID_OK);
}

// --------------------------------------------------------------------------
// LifeSamplesDialog
// --------------------------------------------------------------------------

LifeSamplesDialog::LifeSamplesDialog(wxWindow *parent)
                 : wxDialog(parent, -1,
                            _("Sample games"),
                            wxDefaultPosition,
                            wxDefaultSize,
                            wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL)
{
    m_value = 0;
   
    // create and populate the list of available samples
    m_list = new wxListBox( this, ID_LISTBOX,
        wxDefaultPosition,
        wxDefaultSize,
        0, NULL,
        wxLB_SINGLE | wxLB_NEEDED_SB | wxLB_HSCROLL );

    for (unsigned i = 0; i < (sizeof(g_shapes) / sizeof(LifeShape)); i++)
        m_list->Append(g_shapes[i].m_name);

    // descriptions
    wxStaticBox *statbox = new wxStaticBox( this, -1, _("Description"));
    m_life   = new Life( 16, 16 );
    m_life->SetShape(g_shapes[0]);
    m_canvas = new LifeCanvas( this, m_life, FALSE );
    m_text   = new wxTextCtrl( this, -1,
        g_shapes[0].m_desc,
        wxDefaultPosition,
        wxSize(300, 60),
        wxTE_MULTILINE | wxTE_READONLY);

    // layout components
    wxStaticBoxSizer *sizer1 = new wxStaticBoxSizer( statbox, wxVERTICAL );
    sizer1->Add( m_canvas, 2, wxGROW | wxCENTRE | wxALL, 5);
    sizer1->Add( m_text, 1, wxGROW | wxCENTRE | wxALL, 5 );

    wxBoxSizer *sizer2 = new wxBoxSizer( wxHORIZONTAL );
    sizer2->Add( m_list, 0, wxGROW | wxCENTRE | wxALL, 5 );
    sizer2->Add( sizer1, 1, wxGROW | wxCENTRE | wxALL, 5 );

    wxBoxSizer *sizer3 = new wxBoxSizer( wxVERTICAL );
    sizer3->Add( CreateTextSizer(_("Select one configuration")), 0, wxALL, 10 );
    sizer3->Add( new wxStaticLine(this, -1), 0, wxGROW | wxLEFT | wxRIGHT, 10 );
    sizer3->Add( sizer2, 1, wxGROW | wxCENTRE | wxALL, 5 );
    sizer3->Add( new wxStaticLine(this, -1), 0, wxGROW | wxLEFT | wxRIGHT, 10 );
    sizer3->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxCENTRE | wxALL, 10 );

    // activate
    SetSizer(sizer3);
    SetAutoLayout(TRUE);
    sizer3->SetSizeHints(this);
    sizer3->Fit(this);
    Centre(wxBOTH);
}

LifeSamplesDialog::~LifeSamplesDialog()
{
    m_canvas->Destroy();
    delete m_life;
}

int LifeSamplesDialog::GetValue()
{
    return m_value;
}

void LifeSamplesDialog::OnListBox(wxCommandEvent& event)
{
    if (event.GetSelection() != -1)
    {
        m_value = m_list->GetSelection();
        m_text->SetValue(g_shapes[ event.GetSelection() ].m_desc);
        m_life->SetShape(g_shapes[ event.GetSelection() ]);

        m_canvas->DrawEverything(TRUE);     // force redraw everything
        m_canvas->Refresh(FALSE);           // do not erase background
    }
}

