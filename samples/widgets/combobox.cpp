/////////////////////////////////////////////////////////////////////////////
// Program:     wxWindows Widgets Sample
// Name:        combobox.cpp
// Purpose:     Part of the widgets sample showing wxComboBox
// Author:      Vadim Zeitlin
// Created:     27.03.01
// Id:          $Id$
// Copyright:   (c) 2001 Vadim Zeitlin
// License:     wxWindows license
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

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"
    #include "wx/combobox.h"
#endif

#include "wx/sizer.h"

#include "widgets.h"

#include "icons/combobox.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    ComboPage_Reset = 100,
    ComboPage_Add,
    ComboPage_AddText,
    ComboPage_AddSeveral,
    ComboPage_AddMany,
    ComboPage_Clear,
    ComboPage_Change,
    ComboPage_ChangeText,
    ComboPage_Delete,
    ComboPage_DeleteText,
    ComboPage_DeleteSel,
    ComboPage_Combo
};

// kinds of comboboxes
enum
{
    ComboKind_Default,
    ComboKind_Simple,
    ComboKind_DropDown
};

// ----------------------------------------------------------------------------
// ComboboxWidgetsPage
// ----------------------------------------------------------------------------

class ComboboxWidgetsPage : public WidgetsPage
{
public:
    ComboboxWidgetsPage(wxNotebook *notebook, wxImageList *imaglist);

protected:
    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonChange(wxCommandEvent& event);
    void OnButtonDelete(wxCommandEvent& event);
    void OnButtonDeleteSel(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonAddSeveral(wxCommandEvent& event);
    void OnButtonAddMany(wxCommandEvent& event);

    void OnComboBox(wxCommandEvent& event);

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnUpdateUIAddSeveral(wxUpdateUIEvent& event);
    void OnUpdateUIClearButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event);
    void OnUpdateUIResetButton(wxUpdateUIEvent& event);

    // reset the combobox parameters
    void Reset();

    // (re)create the combobox
    void CreateCombo();

    // the controls
    // ------------

    // the sel mode radiobox
    wxRadioBox *m_radioKind;

    // the checkboxes for styles
    wxCheckBox *m_chkSort,
               *m_chkReadonly;

    // the combobox itself and the sizer it is in
    wxComboBox *m_combobox;
    wxSizer *m_sizerCombo;

    // the text entries for "Add/change string" and "Delete" buttons
    wxTextCtrl *m_textAdd,
               *m_textChange,
               *m_textDelete;

private:
    DECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(ComboboxWidgetsPage);
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ComboboxWidgetsPage, WidgetsPage)
    EVT_BUTTON(ComboPage_Reset, ComboboxWidgetsPage::OnButtonReset)
    EVT_BUTTON(ComboPage_Change, ComboboxWidgetsPage::OnButtonChange)
    EVT_BUTTON(ComboPage_Delete, ComboboxWidgetsPage::OnButtonDelete)
    EVT_BUTTON(ComboPage_DeleteSel, ComboboxWidgetsPage::OnButtonDeleteSel)
    EVT_BUTTON(ComboPage_Clear, ComboboxWidgetsPage::OnButtonClear)
    EVT_BUTTON(ComboPage_Add, ComboboxWidgetsPage::OnButtonAdd)
    EVT_BUTTON(ComboPage_AddSeveral, ComboboxWidgetsPage::OnButtonAddSeveral)
    EVT_BUTTON(ComboPage_AddMany, ComboboxWidgetsPage::OnButtonAddMany)

    EVT_TEXT_ENTER(ComboPage_AddText, ComboboxWidgetsPage::OnButtonAdd)
    EVT_TEXT_ENTER(ComboPage_DeleteText, ComboboxWidgetsPage::OnButtonDelete)

    EVT_UPDATE_UI(ComboPage_Reset, ComboboxWidgetsPage::OnUpdateUIResetButton)
    EVT_UPDATE_UI(ComboPage_AddSeveral, ComboboxWidgetsPage::OnUpdateUIAddSeveral)
    EVT_UPDATE_UI(ComboPage_Clear, ComboboxWidgetsPage::OnUpdateUIClearButton)
    EVT_UPDATE_UI(ComboPage_DeleteText, ComboboxWidgetsPage::OnUpdateUIClearButton)
    EVT_UPDATE_UI(ComboPage_Delete, ComboboxWidgetsPage::OnUpdateUIDeleteButton)
    EVT_UPDATE_UI(ComboPage_Change, ComboboxWidgetsPage::OnUpdateUIDeleteSelButton)
    EVT_UPDATE_UI(ComboPage_ChangeText, ComboboxWidgetsPage::OnUpdateUIDeleteSelButton)
    EVT_UPDATE_UI(ComboPage_DeleteSel, ComboboxWidgetsPage::OnUpdateUIDeleteSelButton)

    EVT_COMBOBOX(ComboPage_Combo, ComboboxWidgetsPage::OnComboBox)

    EVT_CHECKBOX(-1, ComboboxWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(-1, ComboboxWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(ComboboxWidgetsPage, _T("Combobox"));

ComboboxWidgetsPage::ComboboxWidgetsPage(wxNotebook *notebook,
                                       wxImageList *imaglist)
                  : WidgetsPage(notebook)
{
    imaglist->Add(wxBitmap(combobox_xpm));

    // init everything
    m_chkSort =
    m_chkReadonly = (wxCheckBox *)NULL;

    m_combobox = (wxComboBox *)NULL;
    m_sizerCombo = (wxSizer *)NULL;

    /*
       What we create here is a frame having 3 panes: style pane is the
       leftmost one, in the middle the pane with buttons allowing to perform
       miscellaneous combobox operations and the pane containing the combobox
       itself to the right
    */
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, -1, _T("&Set style"));

    // should be in sync with ComboKind_XXX values
    static const wxString kinds[] =
    {
        _T("default"),
        _T("simple"),
        _T("drop down"),
    };

    m_radioKind = new wxRadioBox(this, -1, _T("Combobox &kind:"),
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(kinds), kinds,
                                 1, wxRA_SPECIFY_COLS);

    m_chkSort = new wxCheckBox(this, -1, _T("&Sort items"));
    m_chkReadonly = new wxCheckBox(this, -1, _T("&Read only"));

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    sizerLeft->Add(m_chkSort, 0, wxLEFT | wxRIGHT, 5);
    sizerLeft->Add(m_chkReadonly, 0, wxLEFT | wxRIGHT, 5);
    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer
    sizerLeft->Add(m_radioKind, 0, wxGROW | wxALL, 5);

    wxButton *btn = new wxButton(this, ComboPage_Reset, _T("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, -1, _T("&Change combobox contents"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(this, ComboPage_Add, _T("&Add this string"));
    m_textAdd = new wxTextCtrl(this, ComboPage_AddText, _T("test item 0"));
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textAdd, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ComboPage_AddSeveral, _T("&Insert a few strings"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ComboPage_AddMany, _T("Add &many strings"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(this, ComboPage_Change, _T("C&hange current"));
    m_textChange = new wxTextCtrl(this, ComboPage_ChangeText, _T(""));
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textChange, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(this, ComboPage_Delete, _T("&Delete this item"));
    m_textDelete = new wxTextCtrl(this, ComboPage_DeleteText, _T(""));
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textDelete, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ComboPage_DeleteSel, _T("Delete &selection"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, ComboPage_Clear, _T("&Clear"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
    m_combobox = new wxComboBox(this, ComboPage_Combo, _T(""),
                                wxDefaultPosition, wxDefaultSize,
                                0, NULL,
                                0);
    sizerRight->Add(m_combobox, 1, wxGROW | wxALL, 5);
    sizerRight->SetMinSize(250, 0);
    m_sizerCombo = sizerRight; // save it to modify it later

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 1, wxGROW | wxALL, 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();

    SetAutoLayout(TRUE);
    SetSizer(sizerTop);

    sizerTop->Fit(this);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void ComboboxWidgetsPage::Reset()
{
    m_chkSort->SetValue(FALSE);
    m_chkReadonly->SetValue(FALSE);
}

void ComboboxWidgetsPage::CreateCombo()
{
    int flags = 0;

    if ( m_chkSort->GetValue() )
        flags |= wxCB_SORT;
    if ( m_chkReadonly->GetValue() )
        flags |= wxCB_READONLY;

    switch ( m_radioKind->GetSelection() )
    {
        default:
            wxFAIL_MSG( _T("unknown combo kind") );
            // fall through

        case ComboKind_Default:
            break;

        case ComboKind_Simple:
            flags |= wxCB_SIMPLE;
            break;

        case ComboKind_DropDown:
            flags = wxCB_DROPDOWN;
            break;
    }

    wxArrayString items;
    if ( m_combobox )
    {
        int count = m_combobox->GetCount();
        for ( int n = 0; n < count; n++ )
        {
            items.Add(m_combobox->GetString(n));
        }

        m_sizerCombo->Remove(m_combobox);
        delete m_combobox;
    }

    m_combobox = new wxComboBox(this, ComboPage_Combo, _T(""),
                                wxDefaultPosition, wxDefaultSize,
                                0, NULL,
                                flags);

    size_t count = items.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_combobox->Append(items[n]);
    }

    m_sizerCombo->Add(m_combobox, 1, wxGROW | wxALL, 5);
    m_sizerCombo->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void ComboboxWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateCombo();
}

void ComboboxWidgetsPage::OnButtonChange(wxCommandEvent& WXUNUSED(event))
{
    int sel = m_combobox->GetSelection();
    if ( sel != -1 )
    {
        m_combobox->SetString(sel, m_textChange->GetValue());
    }
}

void ComboboxWidgetsPage::OnButtonDelete(wxCommandEvent& WXUNUSED(event))
{
    unsigned long n;
    if ( !m_textDelete->GetValue().ToULong(&n) ||
            (n >= (unsigned)m_combobox->GetCount()) )
    {
        return;
    }

    m_combobox->Delete(n);
}

void ComboboxWidgetsPage::OnButtonDeleteSel(wxCommandEvent& WXUNUSED(event))
{
    int sel = m_combobox->GetSelection();
    if ( sel != -1 )
    {
        m_combobox->Delete(sel);
    }
}

void ComboboxWidgetsPage::OnButtonClear(wxCommandEvent& event)
{
    m_combobox->Clear();
}

void ComboboxWidgetsPage::OnButtonAdd(wxCommandEvent& event)
{
    static size_t s_item = 0;

    wxString s = m_textAdd->GetValue();
    if ( !m_textAdd->IsModified() )
    {
        // update the default string
        m_textAdd->SetValue(wxString::Format(_T("test item %u"), ++s_item));
    }

    m_combobox->Append(s);
}

void ComboboxWidgetsPage::OnButtonAddMany(wxCommandEvent& WXUNUSED(event))
{
    // "many" means 1000 here
    for ( size_t n = 0; n < 1000; n++ )
    {
        m_combobox->Append(wxString::Format(_T("item #%u"), n));
    }
}

void ComboboxWidgetsPage::OnButtonAddSeveral(wxCommandEvent& event)
{
    m_combobox->Append(_T("First"));
    m_combobox->Append(_T("another one"));
    m_combobox->Append(_T("and the last (very very very very very very very very very very long) one"));
}

void ComboboxWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    event.Enable( m_chkSort->GetValue() || m_chkReadonly->GetValue() );
}

void ComboboxWidgetsPage::OnUpdateUIDeleteButton(wxUpdateUIEvent& event)
{
    unsigned long n;
    event.Enable(m_textDelete->GetValue().ToULong(&n) &&
                    (n < (unsigned)m_combobox->GetCount()));
}

void ComboboxWidgetsPage::OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event)
{
    event.Enable(m_combobox->GetSelection() != -1);
}

void ComboboxWidgetsPage::OnUpdateUIClearButton(wxUpdateUIEvent& event)
{
    event.Enable(m_combobox->GetCount() != 0);
}

void ComboboxWidgetsPage::OnUpdateUIAddSeveral(wxUpdateUIEvent& event)
{
    event.Enable(!(m_combobox->GetWindowStyle() & wxCB_SORT));
}

void ComboboxWidgetsPage::OnComboBox(wxCommandEvent& event)
{
    int sel = event.GetInt();
    m_textDelete->SetValue(wxString::Format(_T("%ld"), sel));

    wxLogMessage(_T("Combobox item %d selected"), sel);
}

void ComboboxWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& event)
{
    CreateCombo();
}

