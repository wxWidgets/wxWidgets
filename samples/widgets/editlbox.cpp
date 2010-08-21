/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        editlbox.cpp
// Purpose:     Part of the widgets sample showing wxEditableListbox
// Author:      Francesco Montorsi
// Created:     8/2/2009
// Id:          $Id$
// Copyright:   (c) 2009 Francesco Montorsi
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

#if wxUSE_EDITABLELISTBOX

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/combobox.h"
    #include "wx/listbox.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
    #include "wx/textctrl.h"
#endif

#include "wx/sizer.h"
#include "wx/editlbox.h"
#include "wx/listctrl.h"

#include "itemcontainer.h"
#include "widgets.h"

#include "icons/listbox.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    EditableListboxPage_Reset = wxID_HIGHEST,
    EditableListboxPage_Listbox,
    EditableListboxPage_ContainerTests
};

// ----------------------------------------------------------------------------
// EditableListboxWidgetsPage
// ----------------------------------------------------------------------------

class EditableListboxWidgetsPage : public WidgetsPage
{
public:
    EditableListboxWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);

    virtual wxControl *GetWidget() const { return m_lbox->GetListCtrl(); }
    virtual void RecreateWidget() { CreateLbox(); }

    // lazy creation of the content
    virtual void CreateContent();

protected:
    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnCheckBox(wxCommandEvent& event);

    // reset the listbox parameters
    void Reset();

    // (re)create the listbox
    void CreateLbox();

    // the checkboxes
    wxCheckBox *m_chkAllowNew,
               *m_chkAllowEdit,
               *m_chkAllowDelete,
               *m_chkAllowNoReorder;

    wxEditableListBox
                  *m_lbox;

    wxSizer *m_sizerLbox;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(EditableListboxWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(EditableListboxWidgetsPage, WidgetsPage)
    EVT_BUTTON(EditableListboxPage_Reset, EditableListboxWidgetsPage::OnButtonReset)
    EVT_CHECKBOX(wxID_ANY, EditableListboxWidgetsPage::OnCheckBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(EditableListboxWidgetsPage, wxT("EditableListbox"), GENERIC_CTRLS);

EditableListboxWidgetsPage::EditableListboxWidgetsPage(WidgetsBookCtrl *book,
                                                       wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, listbox_xpm)
{

}

void EditableListboxWidgetsPage::CreateContent()
{
    /*
       What we create here is a frame having 2 panes: style pane is the
       leftmost one and the pane containing the listbox itself to the right
    */
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY,
                                       wxT("&Set listbox parameters"));
    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkAllowNew = CreateCheckBoxAndAddToSizer(sizerLeft, wxT("Allow new items"));
    m_chkAllowEdit = CreateCheckBoxAndAddToSizer(sizerLeft, wxT("Allow editing items"));
    m_chkAllowDelete = CreateCheckBoxAndAddToSizer(sizerLeft, wxT("Allow deleting items"));
    m_chkAllowNoReorder = CreateCheckBoxAndAddToSizer(sizerLeft, wxT("Block user reordering"));

    wxButton *btn = new wxButton(this, EditableListboxPage_Reset, wxT("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
    m_lbox = new wxEditableListBox(this, EditableListboxPage_Listbox,
                                    _("Match these wildcards:"),
                                    wxDefaultPosition, wxDefaultSize, 0);
    sizerRight->Add(m_lbox, 1, wxGROW | wxALL, 5);
    sizerRight->SetMinSize(150, 0);
    m_sizerLbox = sizerRight; // save it to modify it later

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();

    SetSizer(sizerTop);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void EditableListboxWidgetsPage::Reset()
{
    m_chkAllowNew->SetValue(false);
    m_chkAllowEdit->SetValue(false);
    m_chkAllowDelete->SetValue(false);
    m_chkAllowNoReorder->SetValue(false);
}

void EditableListboxWidgetsPage::CreateLbox()
{
    int flags = 0;

    if ( m_chkAllowNew->GetValue() )
        flags |= wxEL_ALLOW_NEW;
    if ( m_chkAllowEdit->GetValue() )
        flags |= wxEL_ALLOW_EDIT;
    if ( m_chkAllowDelete->GetValue() )
        flags |= wxEL_ALLOW_DELETE;
    if ( m_chkAllowNoReorder->GetValue() )
        flags |= wxEL_NO_REORDER;

    wxArrayString items;
    if ( m_lbox )
    {
        m_lbox->GetStrings(items);
        m_sizerLbox->Detach( m_lbox );
        delete m_lbox;
    }

    m_lbox = new wxEditableListBox(this, EditableListboxPage_Listbox,
                                   _("Match these wildcards:"),
                                   wxDefaultPosition, wxDefaultSize,
                                   flags);

    m_lbox->SetStrings(items);
    m_sizerLbox->Add(m_lbox, 1, wxGROW | wxALL, 5);
    m_sizerLbox->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void EditableListboxWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateLbox();
}

void EditableListboxWidgetsPage::OnCheckBox(wxCommandEvent& WXUNUSED(event))
{
    CreateLbox();
}

#endif // wxUSE_EDITABLELISTBOX
