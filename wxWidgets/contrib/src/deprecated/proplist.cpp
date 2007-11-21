/////////////////////////////////////////////////////////////////////////////
// Name:        contrib/src/deprecated/proplist.cpp
// Purpose:     Property list classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/deprecated/setup.h"

#if wxUSE_PROPSHEET

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/font.h"
    #include "wx/button.h"
    #include "wx/bmpbuttn.h"
    #include "wx/textctrl.h"
    #include "wx/listbox.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/filedlg.h"
#endif

#include "wx/sizer.h"
#include "wx/module.h"
#include "wx/intl.h"
#include "wx/artprov.h"

#include "wx/colordlg.h"
#include "wx/deprecated/proplist.h"

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if !WXWIN_COMPATIBILITY_2_4
static inline wxChar* copystring(const wxChar* s)
    { return wxStrcpy(new wxChar[wxStrlen(s) + 1], s); }
#endif

// ----------------------------------------------------------------------------
// Property text edit control
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPropertyTextEdit, wxTextCtrl)

wxPropertyTextEdit::wxPropertyTextEdit(wxPropertyListView *v, wxWindow *parent,
    const wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size,
    long style, const wxString& name):
 wxTextCtrl(parent, id, value, pos, size, style, wxDefaultValidator, name)
{
  m_view = v;
}

void wxPropertyTextEdit::OnSetFocus()
{
}

void wxPropertyTextEdit::OnKillFocus()
{
}

// ----------------------------------------------------------------------------
// Property list view
// ----------------------------------------------------------------------------

bool wxPropertyListView::sm_dialogCancelled = false;

IMPLEMENT_DYNAMIC_CLASS(wxPropertyListView, wxPropertyView)

BEGIN_EVENT_TABLE(wxPropertyListView, wxPropertyView)
    EVT_BUTTON(wxID_OK,                 wxPropertyListView::OnOk)
    EVT_BUTTON(wxID_CANCEL,             wxPropertyListView::OnCancel)
    EVT_BUTTON(wxID_HELP,               wxPropertyListView::OnHelp)
    EVT_BUTTON(wxID_PROP_CROSS,         wxPropertyListView::OnCross)
    EVT_BUTTON(wxID_PROP_CHECK,         wxPropertyListView::OnCheck)
    EVT_BUTTON(wxID_PROP_EDIT,          wxPropertyListView::OnEdit)
    EVT_TEXT_ENTER(wxID_PROP_TEXT,      wxPropertyListView::OnText)
    EVT_LISTBOX(wxID_PROP_SELECT,       wxPropertyListView::OnPropertySelect)
    EVT_COMMAND(wxID_PROP_SELECT, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,
                                        wxPropertyListView::OnPropertyDoubleClick)
    EVT_LISTBOX(wxID_PROP_VALUE_SELECT, wxPropertyListView::OnValueListSelect)
END_EVENT_TABLE()

wxPropertyListView::wxPropertyListView(wxPanel *propPanel, long flags):wxPropertyView(flags)
{
  m_propertyScrollingList = NULL;
  m_valueList = NULL;
  m_valueText = NULL;
  m_editButton = NULL;
  m_confirmButton = NULL;
  m_cancelButton = NULL;
  m_propertyWindow = propPanel;
  m_managedWindow = NULL;

  m_windowCloseButton = NULL;
  m_windowCancelButton = NULL;
  m_windowHelpButton = NULL;

  m_detailedEditing = false;
}

wxPropertyListView::~wxPropertyListView()
{
}

void wxPropertyListView::ShowView(wxPropertySheet *ps, wxPanel *panel)
{
  m_propertySheet = ps;

  AssociatePanel(panel);
  CreateControls();

  UpdatePropertyList();
  panel->Layout();
}

// Update this view of the viewed object, called e.g. by
// the object itself.
bool wxPropertyListView::OnUpdateView()
{
  return true;
}

bool wxPropertyListView::UpdatePropertyList(bool clearEditArea)
{
  if (!m_propertyScrollingList || !m_propertySheet)
    return false;

  m_propertyScrollingList->Clear();
  if (clearEditArea)
  {
    m_valueList->Clear();
    m_valueText->SetValue(wxEmptyString);
  }
  wxNode *node = m_propertySheet->GetProperties().GetFirst();

  // Should sort them... later...
  while (node)
  {
    wxProperty *property = (wxProperty *)node->GetData();
    wxString stringValueRepr(property->GetValue().GetStringRepresentation());
    wxString paddedString(MakeNameValueString(property->GetName(), stringValueRepr));
    m_propertyScrollingList->Append(paddedString.GetData(), (void *)property);
    node = node->GetNext();
  }
  return true;
}

bool wxPropertyListView::UpdatePropertyDisplayInList(wxProperty *property)
{
  if (!m_propertyScrollingList || !m_propertySheet)
    return false;

#ifdef __WXMSW__
  int currentlySelected = m_propertyScrollingList->GetSelection();
#endif
// #ifdef __WXMSW__
  wxString stringValueRepr(property->GetValue().GetStringRepresentation());
  wxString paddedString(MakeNameValueString(property->GetName(), stringValueRepr));
  int sel = FindListIndexForProperty(property);

  if (sel > -1)
  {
    // Don't update the listbox unnecessarily because it can cause
    // ugly flashing.

    if (paddedString != m_propertyScrollingList->GetString(sel))
      m_propertyScrollingList->SetString(sel, paddedString.GetData());
  }
//#else
//  UpdatePropertyList(false);
//#endif

  // TODO: why is this necessary?
#ifdef __WXMSW__
  if (currentlySelected > -1)
    m_propertyScrollingList->SetSelection(currentlySelected);
#endif

  return true;
}

// Find the wxListBox index corresponding to this property
int wxPropertyListView::FindListIndexForProperty(wxProperty *property)
{
  int n = m_propertyScrollingList->GetCount();
  for (int i = 0; i < n; i++)
  {
    if (property == (wxProperty *)m_propertyScrollingList->wxListBox::GetClientData(i))
      return i;
  }
  return wxNOT_FOUND;
}

wxString wxPropertyListView::MakeNameValueString(wxString name, wxString value)
{
  wxString theString(name);

  int nameWidth = 25;
  int padWith = nameWidth - theString.length();
  if (padWith < 0)
    padWith = 0;

  if (GetFlags() & wxPROP_SHOWVALUES)
  {
    // Want to pad with spaces
    theString.Append( wxT(' '), padWith);
    theString += value;
  }

  return theString;
}

// Select and show string representation in validator the given
// property. NULL resets to show no property.
bool wxPropertyListView::ShowProperty(wxProperty *property, bool select)
{
  if (m_currentProperty)
  {
    EndShowingProperty(m_currentProperty);
    m_currentProperty = NULL;
  }

  m_valueList->Clear();
  m_valueText->SetValue(wxEmptyString);

  if (property)
  {
    m_currentProperty = property;
    BeginShowingProperty(property);
  }
  if (select)
  {
    int sel = FindListIndexForProperty(property);
    if (sel != wxNOT_FOUND)
      m_propertyScrollingList->SetSelection(sel);
  }
  return true;
}

// Find appropriate validator and load property into value controls
bool wxPropertyListView::BeginShowingProperty(wxProperty *property)
{
  m_currentValidator = FindPropertyValidator(property);
  if (!m_currentValidator)
    return false;

  if (!m_currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return false;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)m_currentValidator;

  listValidator->OnPrepareControls(property, this, m_propertyWindow);
  DisplayProperty(property);
  return true;
}

// Find appropriate validator and unload property from value controls
bool wxPropertyListView::EndShowingProperty(wxProperty *property)
{
  if (!m_currentValidator)
    return false;

  RetrieveProperty(property);

  if (!m_currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return false;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)m_currentValidator;

  listValidator->OnClearControls(property, this, m_propertyWindow);
  if (m_detailedEditing)
  {
    listValidator->OnClearDetailControls(property, this, m_propertyWindow);
    m_detailedEditing = false;
  }
  return true;
}

void wxPropertyListView::BeginDetailedEditing()
{
  if (!m_currentValidator)
    return;
  if (!m_currentProperty)
    return;
  if (m_detailedEditing)
    return;
  if (!m_currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return;
  if (!m_currentProperty->IsEnabled())
    return;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)m_currentValidator;

  if (listValidator->OnPrepareDetailControls(m_currentProperty, this, m_propertyWindow))
    m_detailedEditing = true;
}

void wxPropertyListView::EndDetailedEditing()
{
  if (!m_currentValidator)
    return;
  if (!m_currentProperty)
    return;

  RetrieveProperty(m_currentProperty);

  if (!m_currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)m_currentValidator;

  if (m_detailedEditing)
  {
    listValidator->OnClearDetailControls(m_currentProperty, this, m_propertyWindow);
    m_detailedEditing = false;
  }
}

bool wxPropertyListView::DisplayProperty(wxProperty *property)
{
  if (!m_currentValidator)
    return false;

  if (((m_currentValidator->GetFlags() & wxPROP_ALLOW_TEXT_EDITING) == 0) || !property->IsEnabled())
    m_valueText->SetEditable(false);
  else
    m_valueText->SetEditable(true);

  if (!m_currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return false;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)m_currentValidator;

  listValidator->OnDisplayValue(property, this, m_propertyWindow);
  return true;
}

bool wxPropertyListView::RetrieveProperty(wxProperty *property)
{
  if (!m_currentValidator)
    return false;
  if (!property->IsEnabled())
    return false;

  if (!m_currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return false;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)m_currentValidator;

  if (listValidator->OnCheckValue(property, this, m_propertyWindow))
  {
    if (listValidator->OnRetrieveValue(property, this, m_propertyWindow))
    {
      UpdatePropertyDisplayInList(property);
      OnPropertyChanged(property);
    }
  }
  else
  {
    // Revert to old value
    listValidator->OnDisplayValue(property, this, m_propertyWindow);
  }
  return true;
}


bool wxPropertyListView::EditProperty(wxProperty *WXUNUSED(property))
{
  return true;
}

// Called by the listbox callback
void wxPropertyListView::OnPropertySelect(wxCommandEvent& WXUNUSED(event))
{
  int sel = m_propertyScrollingList->GetSelection();
  if (sel != wxNOT_FOUND)
  {
    wxProperty *newSel = (wxProperty *)m_propertyScrollingList->wxListBox::GetClientData(sel);
    if (newSel && newSel != m_currentProperty)
    {
      ShowProperty(newSel, false);
    }
  }
}

bool wxPropertyListView::CreateControls()
{
    wxPanel *panel = (wxPanel *)m_propertyWindow;

    wxSize largeButtonSize( 70, 25 );
    wxSize smallButtonSize( 23, 23 );

    if (m_valueText)
        return true;

    if (!panel)
        return false;

    wxFont guiFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

#ifdef __WXMSW__
    wxFont *boringFont =
        wxTheFontList->FindOrCreateFont(guiFont.GetPointSize(), wxMODERN,
                                        wxNORMAL, wxNORMAL, false, _T("Courier New"));
#else
    wxFont *boringFont = wxTheFontList->FindOrCreateFont(guiFont.GetPointSize(), wxTELETYPE, wxNORMAL, wxNORMAL);
#endif

    // May need to be changed in future to eliminate clashes with app.
    // WHAT WAS THIS FOR?
//  panel->SetClientData((char *)this);

    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );

    // top row with optional buttons and input line

    wxBoxSizer *topsizer = new wxBoxSizer( wxHORIZONTAL );
    int buttonborder = 3;

    if (m_buttonFlags & wxPROP_BUTTON_CHECK_CROSS)
    {
        wxBitmap tickBitmap = wxArtProvider::GetBitmap(wxART_TICK_MARK);
        wxBitmap crossBitmap = wxArtProvider::GetBitmap(wxART_CROSS_MARK);

        if ( tickBitmap.Ok() && crossBitmap.Ok() )
        {
            m_confirmButton = new wxBitmapButton(panel, wxID_PROP_CHECK, tickBitmap, wxDefaultPosition, smallButtonSize );
            m_cancelButton = new wxBitmapButton(panel, wxID_PROP_CROSS, crossBitmap, wxDefaultPosition, smallButtonSize );
        }
        else
        {
            m_confirmButton = new wxButton(panel, wxID_PROP_CHECK, _T(":-)"), wxDefaultPosition, smallButtonSize );
            m_cancelButton = new wxButton(panel, wxID_PROP_CROSS, _T("X"), wxDefaultPosition, smallButtonSize );
        }

        topsizer->Add( m_confirmButton, 0, wxLEFT|wxTOP|wxBOTTOM | wxEXPAND, buttonborder );
        topsizer->Add( m_cancelButton, 0, wxLEFT|wxTOP|wxBOTTOM | wxEXPAND, buttonborder );
    }

    m_valueText = new wxPropertyTextEdit(this, panel, wxID_PROP_TEXT, wxEmptyString,
       wxDefaultPosition, wxSize(wxDefaultCoord, smallButtonSize.y), wxTE_PROCESS_ENTER);
    m_valueText->Disable();
    topsizer->Add( m_valueText, 1, wxALL | wxEXPAND, buttonborder );

    if (m_buttonFlags & wxPROP_PULLDOWN)
    {
        m_editButton = new wxButton(panel, wxID_PROP_EDIT, _T("..."),  wxDefaultPosition, smallButtonSize);
        m_editButton->Disable();
        topsizer->Add( m_editButton, 0, wxRIGHT|wxTOP|wxBOTTOM | wxEXPAND, buttonborder );
    }

    mainsizer->Add( topsizer, 0, wxEXPAND );

    // middle section with two list boxes

    m_middleSizer = new wxBoxSizer( wxVERTICAL );

    m_valueList = new wxListBox(panel, wxID_PROP_VALUE_SELECT, wxDefaultPosition, wxSize(wxDefaultCoord, 60));
    m_valueList->Show(false);

    m_propertyScrollingList = new wxListBox(panel, wxID_PROP_SELECT, wxDefaultPosition, wxSize(100, 100));
    m_propertyScrollingList->SetFont(* boringFont);
    m_middleSizer->Add( m_propertyScrollingList, 1, wxALL|wxEXPAND, buttonborder );

    mainsizer->Add( m_middleSizer, 1, wxEXPAND );

    // bottom row with buttons

    if ((m_buttonFlags & wxPROP_BUTTON_OK) ||
        (m_buttonFlags & wxPROP_BUTTON_CLOSE) ||
        (m_buttonFlags & wxPROP_BUTTON_CANCEL) ||
        (m_buttonFlags & wxPROP_BUTTON_HELP))
    {
        wxBoxSizer *bottomsizer = new wxBoxSizer( wxHORIZONTAL );
        buttonborder = 5;

        if (m_buttonFlags & wxPROP_BUTTON_OK)
        {
            m_windowCloseButton = new wxButton(panel, wxID_OK, _("OK"), wxDefaultPosition, largeButtonSize );
            m_windowCloseButton->SetDefault();
            m_windowCloseButton->SetFocus();
            bottomsizer->Add( m_windowCloseButton, 0, wxALL, buttonborder );
        }
        else if (m_buttonFlags & wxPROP_BUTTON_CLOSE)
        {
            m_windowCloseButton = new wxButton(panel, wxID_OK, _("Close"), wxDefaultPosition, largeButtonSize );
            bottomsizer->Add( m_windowCloseButton, 0, wxALL, buttonborder );
        }
        if (m_buttonFlags & wxPROP_BUTTON_CANCEL)
        {
            m_windowCancelButton = new wxButton(panel, wxID_CANCEL, _("Cancel"), wxDefaultPosition, largeButtonSize );
            bottomsizer->Add( m_windowCancelButton, 0, wxALL, buttonborder );
        }
        if (m_buttonFlags & wxPROP_BUTTON_HELP)
        {
            m_windowHelpButton = new wxButton(panel, wxID_HELP, _("Help"), wxDefaultPosition, largeButtonSize );
            bottomsizer->Add( m_windowHelpButton, 0, wxALL, buttonborder );
        }

        mainsizer->Add( bottomsizer, 0, wxALIGN_RIGHT | wxEXPAND );
    }

    panel->SetSizer( mainsizer );

    return true;
}

void wxPropertyListView::ShowTextControl(bool show)
{
  if (m_valueText)
    m_valueText->Show(show);
}

void wxPropertyListView::ShowListBoxControl(bool show)
{
    if (!m_valueList) return;

    m_valueList->Show(show);

    if (m_buttonFlags & wxPROP_DYNAMIC_VALUE_FIELD)
    {
        if (show)
            m_middleSizer->Prepend( m_valueList, 0, wxTOP|wxLEFT|wxRIGHT | wxEXPAND, 3 );
        else
            m_middleSizer->Remove( 0 );

        m_propertyWindow->Layout();
    }
}

void wxPropertyListView::EnableCheck(bool show)
{
  if (m_confirmButton)
    m_confirmButton->Enable(show);
}

void wxPropertyListView::EnableCross(bool show)
{
  if (m_cancelButton)
    m_cancelButton->Enable(show);
}

bool wxPropertyListView::OnClose()
{
  // Retrieve the value if any
  wxCommandEvent event;
  OnCheck(event);

  delete this;
  return true;
}

void wxPropertyListView::OnValueListSelect(wxCommandEvent& WXUNUSED(event))
{
  if (m_currentProperty && m_currentValidator)
  {
    if (!m_currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
      return;

    wxPropertyListValidator *listValidator = (wxPropertyListValidator *)m_currentValidator;

    listValidator->OnValueListSelect(m_currentProperty, this, m_propertyWindow);
  }
}

void wxPropertyListView::OnOk(wxCommandEvent& event)
{
  // Retrieve the value if any
  OnCheck(event);

  m_managedWindow->Close(true);
  sm_dialogCancelled = false;
}

void wxPropertyListView::OnCancel(wxCommandEvent& WXUNUSED(event))
{
//  SetReturnCode(wxID_CANCEL);
  m_managedWindow->Close(true);
  sm_dialogCancelled = true;
}

void wxPropertyListView::OnHelp(wxCommandEvent& WXUNUSED(event))
{
}

void wxPropertyListView::OnCheck(wxCommandEvent& WXUNUSED(event))
{
  if (m_currentProperty)
  {
    RetrieveProperty(m_currentProperty);
  }
}

void wxPropertyListView::OnCross(wxCommandEvent& WXUNUSED(event))
{
  if (m_currentProperty && m_currentValidator)
  {
    if (!m_currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
      return;

    wxPropertyListValidator *listValidator = (wxPropertyListValidator *)m_currentValidator;

    // Revert to old value
    listValidator->OnDisplayValue(m_currentProperty, this, m_propertyWindow);
  }
}

void wxPropertyListView::OnPropertyDoubleClick(wxCommandEvent& WXUNUSED(event))
{
  if (m_currentProperty && m_currentValidator)
  {
    if (!m_currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
      return;

    wxPropertyListValidator *listValidator = (wxPropertyListValidator *)m_currentValidator;

    // Revert to old value
    listValidator->OnDoubleClick(m_currentProperty, this, m_propertyWindow);
  }
}

void wxPropertyListView::OnEdit(wxCommandEvent& WXUNUSED(event))
{
  if (m_currentProperty && m_currentValidator)
  {
    if (!m_currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
      return;

    wxPropertyListValidator *listValidator = (wxPropertyListValidator *)m_currentValidator;

    listValidator->OnEdit(m_currentProperty, this, m_propertyWindow);
  }
}

void wxPropertyListView::OnText(wxCommandEvent& event)
{
  if (event.GetEventType() == wxEVT_COMMAND_TEXT_ENTER)
  {
    OnCheck(event);
  }
}

// ----------------------------------------------------------------------------
// Property dialog box
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPropertyListDialog, wxDialog)

BEGIN_EVENT_TABLE(wxPropertyListDialog, wxDialog)
    EVT_BUTTON(wxID_CANCEL,                wxPropertyListDialog::OnCancel)
    EVT_CLOSE(wxPropertyListDialog::OnCloseWindow)
END_EVENT_TABLE()

wxPropertyListDialog::wxPropertyListDialog(wxPropertyListView *v, wxWindow *parent,
    const wxString& title, const wxPoint& pos,
    const wxSize& size, long style, const wxString& name):
     wxDialog(parent, wxID_ANY, title, pos, size, style, name)
{
  m_view = v;
  m_view->AssociatePanel( ((wxPanel*)this) );
  m_view->SetManagedWindow(this);
  SetAutoLayout(true);
}

void wxPropertyListDialog::OnCloseWindow(wxCloseEvent& event)
{
  if (m_view)
  {
    SetReturnCode(wxID_CANCEL);
    m_view->OnClose();
    m_view = NULL;
    this->Destroy();
  }
  else
  {
    event.Veto();
  }
}

void wxPropertyListDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    SetReturnCode(wxID_CANCEL);
    this->Close();
}

void wxPropertyListDialog::OnDefaultAction(wxControl *WXUNUSED(item))
{
/*
  if (item == m_view->GetPropertyScrollingList())
    view->OnDoubleClick();
*/
}

// Extend event processing to search the view's event table
bool wxPropertyListDialog::ProcessEvent(wxEvent& event)
{
    if ( !m_view || ! m_view->ProcessEvent(event) )
        return wxEvtHandler::ProcessEvent(event);
    else
        return true;
}

// ----------------------------------------------------------------------------
// Property panel
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPropertyListPanel, wxPanel)

BEGIN_EVENT_TABLE(wxPropertyListPanel, wxPanel)
    EVT_SIZE(wxPropertyListPanel::OnSize)
END_EVENT_TABLE()

wxPropertyListPanel::~wxPropertyListPanel()
{
}

void wxPropertyListPanel::OnDefaultAction(wxControl *WXUNUSED(item))
{
/*
  if (item == view->GetPropertyScrollingList())
    view->OnDoubleClick();
*/
}

// Extend event processing to search the view's event table
bool wxPropertyListPanel::ProcessEvent(wxEvent& event)
{
    if ( !m_view || ! m_view->ProcessEvent(event) )
        return wxEvtHandler::ProcessEvent(event);
    else
        return true;
}

void wxPropertyListPanel::OnSize(wxSizeEvent& WXUNUSED(event))
{
    Layout();
}

// ----------------------------------------------------------------------------
// Property frame
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPropertyListFrame, wxFrame)

BEGIN_EVENT_TABLE(wxPropertyListFrame, wxFrame)
    EVT_CLOSE(wxPropertyListFrame::OnCloseWindow)
END_EVENT_TABLE()

void wxPropertyListFrame::OnCloseWindow(wxCloseEvent& event)
{
  if (m_view)
  {
    if (m_propertyPanel)
        m_propertyPanel->SetView(NULL);
    m_view->OnClose();
    m_view = NULL;
    this->Destroy();
  }
  else
  {
    event.Veto();
  }
}

wxPropertyListPanel *wxPropertyListFrame::OnCreatePanel(wxFrame *parent, wxPropertyListView *v)
{
  return new wxPropertyListPanel(v, parent);
}

bool wxPropertyListFrame::Initialize()
{
  m_propertyPanel = OnCreatePanel(this, m_view);
  if (m_propertyPanel)
  {
    m_view->AssociatePanel(m_propertyPanel);
    m_view->SetManagedWindow(this);
    m_propertyPanel->SetAutoLayout(true);
    return true;
  }
  else
    return false;
}

// ----------------------------------------------------------------------------
// Property list specific validator
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxPropertyListValidator, wxPropertyValidator)

bool wxPropertyListValidator::OnSelect(bool select, wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
//  view->GetValueText()->Show(true);
  if (select)
    OnDisplayValue(property, view, parentWindow);

  return true;
}

bool wxPropertyListValidator::OnValueListSelect(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  wxString s(view->GetValueList()->GetStringSelection());
  if ( !s.empty() )
  {
    view->GetValueText()->SetValue(s);
    view->RetrieveProperty(property);
  }
  return true;
}

bool wxPropertyListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
//  view->GetValueText()->Show(true);
  wxString str(property->GetValue().GetStringRepresentation());

  view->GetValueText()->SetValue(str);
  return true;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxPropertyListValidator::OnRetrieveValue(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (!view->GetValueText())
    return false;
  return false;
}

void wxPropertyListValidator::OnEdit(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (view->GetDetailedEditing())
    view->EndDetailedEditing();
  else
    view->BeginDetailedEditing();
}

bool wxPropertyListValidator::OnClearControls(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Disable();
  if (view->GetCancelButton())
    view->GetCancelButton()->Disable();
  if (view->GetEditButton())
    view->GetEditButton()->Disable();
  return true;
}

// ----------------------------------------------------------------------------
// Default validators
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxRealListValidator, wxPropertyListValidator)

///
/// Real number validator
///
bool wxRealListValidator::OnCheckValue(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *parentWindow)
{
  if (m_realMin == 0.0 && m_realMax == 0.0)
    return true;

  if (!view->GetValueText())
    return false;
  wxString value(view->GetValueText()->GetValue());

  float val = 0.0;
  if (!StringToFloat(WXSTRINGCAST value, &val))
  {
    wxChar buf[200];
    wxSprintf(buf, wxT("Value %s is not a valid real number!"), value.GetData());
    wxMessageBox(buf, wxT("Property value error"), wxOK | wxICON_EXCLAMATION, parentWindow);
    return false;
  }

  if (val < m_realMin || val > m_realMax)
  {
    wxChar buf[200];
    wxSprintf(buf, wxT("Value must be a real number between %.2f and %.2f!"), m_realMin, m_realMax);
    wxMessageBox(buf, wxT("Property value error"), wxOK | wxICON_EXCLAMATION, parentWindow);
    return false;
  }
  return true;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxRealListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (!view->GetValueText())
    return false;

  if (wxStrlen(view->GetValueText()->GetValue()) == 0)
    return false;

  wxString value(view->GetValueText()->GetValue());
  float f = (float)wxAtof(value.GetData());
  property->GetValue() = f;
  return true;
}

bool wxRealListValidator::OnPrepareControls(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable();
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable();
  if (view->GetEditButton())
    view->GetEditButton()->Disable();
  if (view->GetValueText())
    view->GetValueText()->Enable();
  return true;
}

///
/// Integer validator
///
IMPLEMENT_DYNAMIC_CLASS(wxIntegerListValidator, wxPropertyListValidator)

bool wxIntegerListValidator::OnCheckValue(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *parentWindow)
{
  if (m_integerMin == 0 && m_integerMax == 0)
    return true;

  if (!view->GetValueText())
    return false;
  wxString value(view->GetValueText()->GetValue());

  long val = 0;
  if (!StringToLong(WXSTRINGCAST value, &val))
  {
    wxChar buf[200];
    wxSprintf(buf, wxT("Value %s is not a valid integer!"), value.GetData());
    wxMessageBox(buf, wxT("Property value error"), wxOK | wxICON_EXCLAMATION, parentWindow);
    return false;
  }
  if (val < m_integerMin || val > m_integerMax)
  {
    wxChar buf[200];
    wxSprintf(buf, wxT("Value must be an integer between %ld and %ld!"), m_integerMin, m_integerMax);
    wxMessageBox(buf, wxT("Property value error"), wxOK | wxICON_EXCLAMATION, parentWindow);
    return false;
  }
  return true;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxIntegerListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (!view->GetValueText())
    return false;

  if (wxStrlen(view->GetValueText()->GetValue()) == 0)
    return false;

  wxString value(view->GetValueText()->GetValue());
  long val = (long)wxAtoi(value.GetData());
  property->GetValue() = (long)val;
  return true;
}

bool wxIntegerListValidator::OnPrepareControls(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable();
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable();
  if (view->GetEditButton())
    view->GetEditButton()->Disable();
  if (view->GetValueText())
    view->GetValueText()->Enable();
  return true;
}

///
/// boolean validator
///
IMPLEMENT_DYNAMIC_CLASS(wxBoolListValidator, wxPropertyListValidator)

bool wxBoolListValidator::OnCheckValue(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return false;
  wxString value(view->GetValueText()->GetValue());
  if (value != wxT("True") && value != wxT("False"))
  {
    wxMessageBox(wxT("Value must be True or False!"), wxT("Property value error"), wxOK | wxICON_EXCLAMATION, parentWindow);
    return false;
  }
  return true;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxBoolListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (!view->GetValueText())
    return false;

  if (wxStrlen(view->GetValueText()->GetValue()) == 0)
    return false;

  wxString value(view->GetValueText()->GetValue());
  bool boolValue = (value == wxT("True"));
  property->GetValue() = boolValue;
  return true;
}

bool wxBoolListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (!view->GetValueText())
    return false;
  wxString str(property->GetValue().GetStringRepresentation());

  view->GetValueText()->SetValue(str);

  if (view->GetValueList()->IsShown())
  {
    view->GetValueList()->SetStringSelection(str);
  }
  return true;
}

bool wxBoolListValidator::OnPrepareControls(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Disable();
  if (view->GetCancelButton())
    view->GetCancelButton()->Disable();
  if (view->GetEditButton())
    view->GetEditButton()->Enable();
  if (view->GetValueText())
    view->GetValueText()->Disable();
  return true;
}

bool wxBoolListValidator::OnPrepareDetailControls(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (view->GetValueList())
  {
    view->ShowListBoxControl(true);
    view->GetValueList()->Enable();

    view->GetValueList()->Append(wxT("True"));
    view->GetValueList()->Append(wxT("False"));
    wxChar *currentString = copystring(view->GetValueText()->GetValue());
    view->GetValueList()->SetStringSelection(currentString);
    delete[] currentString;
  }
  return true;
}

bool wxBoolListValidator::OnClearDetailControls(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (view->GetValueList())
  {
    view->GetValueList()->Clear();
    view->ShowListBoxControl(false);
    view->GetValueList()->Disable();
  }
  return true;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
bool wxBoolListValidator::OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (!view->GetValueText())
    return false;
  if (property->GetValue().BoolValue())
    property->GetValue() = false;
  else
    property->GetValue() = true;
  view->DisplayProperty(property);
  view->UpdatePropertyDisplayInList(property);
  view->OnPropertyChanged(property);
  return true;
}

///
/// String validator
///
IMPLEMENT_DYNAMIC_CLASS(wxStringListValidator, wxPropertyListValidator)

wxStringListValidator::wxStringListValidator(wxStringList *list, long flags):
  wxPropertyListValidator(flags)
{
  m_strings = list;
  // If no constraint, we just allow the string to be edited.
  if (!m_strings && ((m_validatorFlags & wxPROP_ALLOW_TEXT_EDITING) == 0))
    m_validatorFlags |= wxPROP_ALLOW_TEXT_EDITING;
}

bool wxStringListValidator::OnCheckValue(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!m_strings)
    return true;

  if (!view->GetValueText())
    return false;
  wxString value(view->GetValueText()->GetValue());

  if (!m_strings->Member(value.GetData()))
  {
    wxString str( wxT("Value ") );
    str += value.GetData();
    str += wxT(" is not valid.");
    wxMessageBox( str.GetData(), wxT("Property value error"), wxOK | wxICON_EXCLAMATION, parentWindow);
    return false;
  }
  return true;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxStringListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (!view->GetValueText())
    return false;
  wxString value(view->GetValueText()->GetValue());
  property->GetValue() = value ;
  return true;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxStringListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (!view->GetValueText())
    return false;
  wxString str(property->GetValue().GetStringRepresentation());
  view->GetValueText()->SetValue(str);
  if (m_strings && view->GetValueList() && view->GetValueList()->IsShown() && view->GetValueList()->GetCount() > 0)
  {
    view->GetValueList()->SetStringSelection(str);
  }
  return true;
}

bool wxStringListValidator::OnPrepareControls(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  // Unconstrained
  if (!m_strings)
  {
    if (view->GetEditButton())
      view->GetEditButton()->Disable();
    if (view->GetConfirmButton())
      view->GetConfirmButton()->Enable();
    if (view->GetCancelButton())
      view->GetCancelButton()->Enable();
    if (view->GetValueText())
      view->GetValueText()->Enable();
    return true;
  }

  // Constrained
  if (view->GetValueText())
    view->GetValueText()->Disable();

  if (view->GetEditButton())
    view->GetEditButton()->Enable();

  if (view->GetConfirmButton())
    view->GetConfirmButton()->Disable();
  if (view->GetCancelButton())
    view->GetCancelButton()->Disable();
  return true;
}

bool wxStringListValidator::OnPrepareDetailControls( wxProperty *property,
                                                     wxPropertyListView *view,
                                                     wxWindow *WXUNUSED(parentWindow) )
{
  if (view->GetValueList())
  {
    view->ShowListBoxControl(true);
    view->GetValueList()->Enable();
    wxStringList::Node  *node = m_strings->GetFirst();
    while (node)
    {
      wxChar *s = node->GetData();
      view->GetValueList()->Append(s);
      node = node->GetNext();
    }
    wxChar *currentString = property->GetValue().StringValue();
    view->GetValueList()->SetStringSelection(currentString);
  }
  return true;
}

bool wxStringListValidator::OnClearDetailControls(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (!m_strings)
  {
    return true;
  }

  if (view->GetValueList())
  {
    view->GetValueList()->Clear();
    view->ShowListBoxControl(false);
    view->GetValueList()->Disable();
  }
  return true;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
bool wxStringListValidator::OnDoubleClick( wxProperty *property,
                                           wxPropertyListView *view,
                                           wxWindow *WXUNUSED(parentWindow) )
{
  if (!view->GetValueText())
    return false;
  if (!m_strings)
    return false;

  wxStringList::Node    *node = m_strings->GetFirst();
  wxChar                *currentString = property->GetValue().StringValue();
  while (node)
  {
    wxChar *s = node->GetData();
    if (wxStrcmp(s, currentString) == 0)
    {
      wxChar *nextString;
      if (node->GetNext())
        nextString = node->GetNext()->GetData();
      else
        nextString = m_strings->GetFirst()->GetData();
      property->GetValue() = wxString(nextString);
      view->DisplayProperty(property);
      view->UpdatePropertyDisplayInList(property);
      view->OnPropertyChanged(property);
      return true;
    }
    else node = node->GetNext();
  }
  return true;
}

///
/// Filename validator
///
IMPLEMENT_DYNAMIC_CLASS(wxFilenameListValidator, wxPropertyListValidator)

wxFilenameListValidator::wxFilenameListValidator(wxString message , wxString wildcard, long flags):
  wxPropertyListValidator(flags), m_filenameWildCard(wildcard), m_filenameMessage(message)
{
}

wxFilenameListValidator::~wxFilenameListValidator()
{
}

bool wxFilenameListValidator::OnCheckValue(wxProperty *WXUNUSED(property), wxPropertyListView *WXUNUSED(view), wxWindow *WXUNUSED(parentWindow))
{
  return true;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxFilenameListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (!view->GetValueText())
    return false;
  wxString value(view->GetValueText()->GetValue());
  property->GetValue() = value ;
  return true;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxFilenameListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (!view->GetValueText())
    return false;
  wxString str(property->GetValue().GetStringRepresentation());
  view->GetValueText()->SetValue(str);
  return true;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
bool wxFilenameListValidator::OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return false;
  OnEdit(property, view, parentWindow);
  return true;
}

bool wxFilenameListValidator::OnPrepareControls(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable();
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable();
  if (view->GetEditButton())
    view->GetEditButton()->Enable();
  if (view->GetValueText())
    view->GetValueText()->Enable((GetFlags() & wxPROP_ALLOW_TEXT_EDITING) == wxPROP_ALLOW_TEXT_EDITING);
  return true;
}

void wxFilenameListValidator::OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return;

#if wxUSE_FILEDLG
    wxString s = wxFileSelector(
        m_filenameMessage.GetData(),
        wxPathOnly(property->GetValue().StringValue()),
        wxFileNameFromPath(property->GetValue().StringValue()),
        NULL,
        m_filenameWildCard.GetData(),
        0,
        parentWindow);
    if ( !s.empty() )
    {
        property->GetValue() = s;
        view->DisplayProperty(property);
        view->UpdatePropertyDisplayInList(property);
        view->OnPropertyChanged(property);
    }
#else
    wxUnusedVar(property);
    wxUnusedVar(view);
    wxUnusedVar(parentWindow);
#endif
}

///
/// Colour validator
///
IMPLEMENT_DYNAMIC_CLASS(wxColourListValidator, wxPropertyListValidator)

wxColourListValidator::wxColourListValidator(long flags):
  wxPropertyListValidator(flags)
{
}

wxColourListValidator::~wxColourListValidator()
{
}

bool wxColourListValidator::OnCheckValue(wxProperty *WXUNUSED(property), wxPropertyListView *WXUNUSED(view), wxWindow *WXUNUSED(parentWindow))
{
  return true;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxColourListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (!view->GetValueText())
    return false;
  wxString value(view->GetValueText()->GetValue());

  property->GetValue() = value ;
  return true;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxColourListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (!view->GetValueText())
    return false;
  wxString str(property->GetValue().GetStringRepresentation());
  view->GetValueText()->SetValue(str);
  return true;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
bool wxColourListValidator::OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return false;
  OnEdit(property, view, parentWindow);
  return true;
}

bool wxColourListValidator::OnPrepareControls(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
    if (view->GetConfirmButton())
        view->GetConfirmButton()->Enable();

    if (view->GetCancelButton())
        view->GetCancelButton()->Enable();

    if (view->GetEditButton())
        view->GetEditButton()->Enable();

    if (view->GetValueText())
        view->GetValueText()->Enable((GetFlags() & wxPROP_ALLOW_TEXT_EDITING) == wxPROP_ALLOW_TEXT_EDITING);

    return true;
}

void wxColourListValidator::OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return;

  wxChar *s = property->GetValue().StringValue();
  unsigned char r = 0;
  unsigned char g = 0;
  unsigned char b = 0;
  if (s)
  {
    r = (unsigned char)wxHexToDec(s);
    g = (unsigned char)wxHexToDec(s+2);
    b = (unsigned char)wxHexToDec(s+4);
  }

  wxColour col(r,g,b);

  wxColourData data;
  data.SetChooseFull(true);
  data.SetColour(col);

  for (int i = 0; i < 16; i++)
  {
    wxColour colour((unsigned char)(i*16),
                    (unsigned char)(i*16),
                    (unsigned char)(i*16));
    data.SetCustomColour(i, colour);
  }

  wxColourDialog dialog(parentWindow, &data);
  if (dialog.ShowModal() != wxID_CANCEL)
  {
    wxColourData retData = dialog.GetColourData();
    col = retData.GetColour();

    wxChar buf[7];
    wxDecToHex(col.Red(), buf);
    wxDecToHex(col.Green(), buf+2);
    wxDecToHex(col.Blue(), buf+4);

    property->GetValue() = wxString(buf);
    view->DisplayProperty(property);
    view->UpdatePropertyDisplayInList(property);
    view->OnPropertyChanged(property);
  }
}

///
/// List of strings validator. For this we need more user interface than
/// we get with a property list; so create a new dialog for editing the list.
///
IMPLEMENT_DYNAMIC_CLASS(wxListOfStringsListValidator, wxPropertyListValidator)

wxListOfStringsListValidator::wxListOfStringsListValidator(long flags):
  wxPropertyListValidator(flags)
{
}

bool wxListOfStringsListValidator::OnCheckValue(wxProperty *WXUNUSED(property), wxPropertyListView *WXUNUSED(view), wxWindow *WXUNUSED(parentWindow))
{
  // No constraints for an arbitrary, user-editable list of strings.
  return true;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself.
// In this case, the user cannot directly edit the string list.
bool wxListOfStringsListValidator::OnRetrieveValue(wxProperty *WXUNUSED(property), wxPropertyListView *WXUNUSED(view), wxWindow *WXUNUSED(parentWindow))
{
  return true;
}

bool wxListOfStringsListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (!view->GetValueText())
    return false;
  wxString str(property->GetValue().GetStringRepresentation());
  view->GetValueText()->SetValue(str);
  return true;
}

bool wxListOfStringsListValidator::OnPrepareControls(wxProperty *WXUNUSED(property), wxPropertyListView *view, wxWindow *WXUNUSED(parentWindow))
{
  if (view->GetEditButton())
    view->GetEditButton()->Enable();
  if (view->GetValueText())
    view->GetValueText()->Disable();

  if (view->GetConfirmButton())
    view->GetConfirmButton()->Disable();
  if (view->GetCancelButton())
    view->GetCancelButton()->Disable();
  return true;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
bool wxListOfStringsListValidator::OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  OnEdit(property, view, parentWindow);
  return true;
}

void wxListOfStringsListValidator::OnEdit( wxProperty *property,
                                           wxPropertyListView *view,
                                           wxWindow *parentWindow )
{
  // Convert property value to a list of strings for editing
  wxStringList *stringList = new wxStringList;

  wxPropertyValue *expr = property->GetValue().GetFirst();
  while (expr)
  {
    wxChar *s = expr->StringValue();
    if (s)
      stringList->Add(s);
    expr = expr->GetNext();
  }

  wxString title(wxT("Editing "));
  title += property->GetName();

  if (EditStringList(parentWindow, stringList, title.GetData()))
  {
    wxPropertyValue& oldValue = property->GetValue();
    oldValue.ClearList();
    wxStringList::Node  *node = stringList->GetFirst();
    while (node)
    {
      wxChar *s = node->GetData();
      oldValue.Append(new wxPropertyValue(s));

      node = node->GetNext();
    }

    view->DisplayProperty(property);
    view->UpdatePropertyDisplayInList(property);
    view->OnPropertyChanged(property);
  }
  delete stringList;
}

class wxPropertyStringListEditorDialog: public wxDialog
{
public:
    wxPropertyStringListEditorDialog(wxWindow *parent, const wxString& title,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
          long windowStyle = wxDEFAULT_DIALOG_STYLE, const wxString& name = wxT("stringEditorDialogBox")):
               wxDialog(parent, wxID_ANY, title, pos, size, windowStyle, name)
    {
        m_stringList = NULL;
        m_stringText = NULL;
        m_listBox = NULL;
        sm_dialogCancelled = false;
        m_currentSelection = -1;
    }
    ~wxPropertyStringListEditorDialog(void) {}
    void OnCloseWindow(wxCloseEvent& event);
    void SaveCurrentSelection(void);
    void ShowCurrentSelection(void);

    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnAdd(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnStrings(wxCommandEvent& event);
    void OnText(wxCommandEvent& event);

public:
    wxStringList*       m_stringList;
    wxListBox*          m_listBox;
    wxTextCtrl*         m_stringText;
    static bool         sm_dialogCancelled;
    int                 m_currentSelection;
DECLARE_EVENT_TABLE()
};

#define    wxID_PROP_SL_ADD            3000
#define    wxID_PROP_SL_DELETE            3001
#define    wxID_PROP_SL_STRINGS        3002
#define    wxID_PROP_SL_TEXT            3003

BEGIN_EVENT_TABLE(wxPropertyStringListEditorDialog, wxDialog)
    EVT_BUTTON(wxID_OK,                 wxPropertyStringListEditorDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL,                wxPropertyStringListEditorDialog::OnCancel)
    EVT_BUTTON(wxID_PROP_SL_ADD,        wxPropertyStringListEditorDialog::OnAdd)
    EVT_BUTTON(wxID_PROP_SL_DELETE,        wxPropertyStringListEditorDialog::OnDelete)
    EVT_LISTBOX(wxID_PROP_SL_STRINGS,    wxPropertyStringListEditorDialog::OnStrings)
    EVT_TEXT_ENTER(wxID_PROP_SL_TEXT,            wxPropertyStringListEditorDialog::OnText)
    EVT_CLOSE(wxPropertyStringListEditorDialog::OnCloseWindow)
END_EVENT_TABLE()

class wxPropertyStringListEditorText: public wxTextCtrl
{
 public:
  wxPropertyStringListEditorText(wxWindow *parent, wxWindowID id, const wxString& val,
      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long windowStyle = 0, const wxString& name = wxT("text")):
     wxTextCtrl(parent, id, val, pos, size, windowStyle, wxDefaultValidator, name)
  {
  }
  void OnKillFocus()
  {
    wxPropertyStringListEditorDialog *dialog = (wxPropertyStringListEditorDialog *)GetParent();
    dialog->SaveCurrentSelection();
  }
};

bool wxPropertyStringListEditorDialog::sm_dialogCancelled = false;

// Edit the string list.
bool wxListOfStringsListValidator::EditStringList(wxWindow *parent, wxStringList *stringList, const wxChar *title)
{
  int largeButtonWidth = 60;
  int largeButtonHeight = 25;

  wxBeginBusyCursor();
  wxPropertyStringListEditorDialog *dialog = new wxPropertyStringListEditorDialog(parent,
      title, wxPoint(10, 10), wxSize(400, 400));

  dialog->m_stringList = stringList;

  dialog->m_listBox = new wxListBox(dialog, wxID_PROP_SL_STRINGS,
    wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE);

  dialog->m_stringText = new wxPropertyStringListEditorText(dialog,
       wxID_PROP_SL_TEXT, wxEmptyString, wxPoint(5, 240),
       wxSize(300, wxDefaultCoord), wxTE_PROCESS_ENTER);
  dialog->m_stringText->Disable();

  wxButton *addButton = new wxButton(dialog, wxID_PROP_SL_ADD, wxT("Add"), wxDefaultPosition, wxSize(largeButtonWidth, largeButtonHeight));
  wxButton *deleteButton = new wxButton(dialog, wxID_PROP_SL_DELETE, wxT("Delete"), wxDefaultPosition, wxSize(largeButtonWidth, largeButtonHeight));
  wxButton *cancelButton = new wxButton(dialog, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxSize(largeButtonWidth, largeButtonHeight));
  wxButton *okButton = new wxButton(dialog, wxID_OK, wxT("OK"), wxDefaultPosition, wxSize(largeButtonWidth, largeButtonHeight));

#ifndef __WXGTK__
  okButton->SetDefault();
#endif

  wxBoxSizer *m_bottom_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_bottom_sizer->Add(addButton, 0, wxALL | wxALIGN_LEFT, 2 );
  m_bottom_sizer->Add(deleteButton, 0, wxALL | wxALIGN_LEFT, 2 );
  m_bottom_sizer->Add(1, 1, 1, wxEXPAND | wxALL);
  m_bottom_sizer->Add(cancelButton, 0, wxALL | wxALIGN_RIGHT, 2 );
  m_bottom_sizer->Add(okButton, 0, wxALL | wxALIGN_RIGHT, 2 );

  wxBoxSizer *m_sizer = new wxBoxSizer( wxVERTICAL );
  m_sizer->Add(dialog->m_listBox, 1, wxEXPAND | wxALL, 2 );
  m_sizer->Add(dialog->m_stringText, 0, wxEXPAND | wxALL, 2 );
  m_sizer->Add(m_bottom_sizer, 0, wxEXPAND | wxALL , 0 );

  dialog->SetSizer( m_sizer );
  m_sizer->SetSizeHints( dialog );

  wxStringList::Node    *node = stringList->GetFirst();
  while (node)
  {
    wxChar *str = node->GetData();
    // Save node as client data for each listbox item
    dialog->m_listBox->Append(str, (wxChar *)node);
    node = node->GetNext();
  }

  dialog->SetClientSize(310, 305);
  dialog->Layout();

  dialog->Centre(wxBOTH);
  wxEndBusyCursor();
  if (dialog->ShowModal() == wxID_CANCEL)
    return false;
  else
    return true;
}

/*
 * String list editor callbacks
 *
 */

void wxPropertyStringListEditorDialog::OnStrings(wxCommandEvent& WXUNUSED(event))
{
    int sel = m_listBox->GetSelection();
    if (sel != wxNOT_FOUND)
    {
        m_currentSelection = sel;

        ShowCurrentSelection();
    }
}

void wxPropertyStringListEditorDialog::OnDelete(wxCommandEvent& WXUNUSED(event))
{
    int sel = m_listBox->GetSelection();
    if (sel == wxNOT_FOUND)
        return;

    wxNode *node = (wxNode *)m_listBox->wxListBox::GetClientData(sel);
    if (!node)
        return;

    m_listBox->Delete(sel);
    delete[] (wxChar *)node->GetData();
    delete node;
    m_currentSelection = -1;
    m_stringText->SetValue(wxEmptyString);
}

void wxPropertyStringListEditorDialog::OnAdd(wxCommandEvent& WXUNUSED(event))
{
  SaveCurrentSelection();

  wxString initialText;
  wxNode *node = m_stringList->Add(initialText);
  m_listBox->Append(initialText, (void *)node);
  m_currentSelection = m_stringList->GetCount() - 1;
  m_listBox->SetSelection(m_currentSelection);
  ShowCurrentSelection();
  m_stringText->SetFocus();
}

void wxPropertyStringListEditorDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    SaveCurrentSelection();
    EndModal(wxID_OK);
//  Close(true);
    this->Destroy();
}

void wxPropertyStringListEditorDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    sm_dialogCancelled = true;
    EndModal(wxID_CANCEL);
//  Close(true);
    this->Destroy();
}

void wxPropertyStringListEditorDialog::OnText(wxCommandEvent& event)
{
    if (event.GetEventType() == wxEVT_COMMAND_TEXT_ENTER)
    {
        SaveCurrentSelection();
    }
}

void
wxPropertyStringListEditorDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    SaveCurrentSelection();

    Destroy();
}

void wxPropertyStringListEditorDialog::SaveCurrentSelection()
{
  if (m_currentSelection == -1)
    return;

  wxNode *node = (wxNode *)m_listBox->wxListBox::GetClientData(m_currentSelection);
  if (!node)
    return;

  wxString txt(m_stringText->GetValue());
  if (node->GetData())
    delete[] (wxChar *)node->GetData();
  node->SetData((wxObject *)wxStrdup(txt));

  m_listBox->SetString(m_currentSelection, (wxChar *)node->GetData());
}

void wxPropertyStringListEditorDialog::ShowCurrentSelection()
{
  if (m_currentSelection == -1)
  {
    m_stringText->SetValue(wxEmptyString);
    return;
  }
  wxNode *node = (wxNode *)m_listBox->wxListBox::GetClientData(m_currentSelection);
  wxChar *txt = (wxChar *)node->GetData();
  m_stringText->SetValue(txt);
  m_stringText->Enable();
}


#endif // wxUSE_PROPSHEET
