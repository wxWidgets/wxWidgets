/////////////////////////////////////////////////////////////////////////////
// Name:        proplist.cpp
// Purpose:     Property list classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "proplist.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if defined(__WXMSW__) && !defined(__GNUWIN32__)
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#include "wx/window.h"
#include "wx/utils.h"
#include "wx/list.h"
#include "wx/colordlg.h"
#include "proplist.h"

/*
 * Property text edit control
 */
 
IMPLEMENT_CLASS(wxPropertyTextEdit, wxTextCtrl)

wxPropertyTextEdit::wxPropertyTextEdit(wxPropertyListView *v, wxWindow *parent,
    const wxWindowID id, const wxString& value,
	const wxPoint& pos, const wxSize& size,
    long style, const wxString& name):
 wxTextCtrl(parent, id, value, pos, size, style, wxDefaultValidator, name)
{
  view = v;
}

void wxPropertyTextEdit::OnSetFocus(void)
{
}

void wxPropertyTextEdit::OnKillFocus(void)
{
}

/*
 * Property list view
 */

IMPLEMENT_DYNAMIC_CLASS(wxPropertyListView, wxPropertyView)

BEGIN_EVENT_TABLE(wxPropertyListView, wxPropertyView)
	EVT_BUTTON(wxID_OK, 		wxPropertyListView::OnOk)
	EVT_BUTTON(wxID_CANCEL, 	wxPropertyListView::OnCancel)
	EVT_BUTTON(wxID_HELP, 		wxPropertyListView::OnHelp)
	EVT_BUTTON(wxID_PROP_CROSS,	wxPropertyListView::OnCross)
	EVT_BUTTON(wxID_PROP_CHECK,	wxPropertyListView::OnCheck)
	EVT_BUTTON(wxID_PROP_EDIT,	wxPropertyListView::OnEdit)
	EVT_TEXT_ENTER(wxID_PROP_TEXT,	wxPropertyListView::OnText)
	EVT_LISTBOX(wxID_PROP_SELECT,	wxPropertyListView::OnPropertySelect)
    EVT_COMMAND(wxID_PROP_SELECT, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxPropertyListView::OnPropertyDoubleClick)
	EVT_TEXT(wxID_PROP_VALUE_SELECT,	wxPropertyListView::OnValueListSelect)
END_EVENT_TABLE()

bool wxPropertyListView::dialogCancelled = FALSE;
wxBitmap *wxPropertyListView::tickBitmap = NULL;
wxBitmap *wxPropertyListView::crossBitmap = NULL;

wxPropertyListView::wxPropertyListView(wxPanel *propPanel, long flags):wxPropertyView(flags)
{
  propertyScrollingList = NULL;
  valueList = NULL;
  valueText = NULL;
  editButton = NULL;
  confirmButton = NULL;
  cancelButton = NULL;
  propertyWindow = propPanel;
  managedWindow = NULL;

  windowCloseButton = NULL;
  windowCancelButton = NULL;
  windowHelpButton = NULL;

  detailedEditing = FALSE;
}

wxPropertyListView::~wxPropertyListView(void)
{
/*
  if (tickBitmap)
    delete tickBitmap;
  if (crossBitmap)
    delete crossBitmap;
*/
}

void wxPropertyListView::ShowView(wxPropertySheet *ps, wxPanel *panel)
{
  propertySheet = ps;
  
  AssociatePanel(panel);
  CreateControls();

  UpdatePropertyList();
  panel->Layout();
}

// Update this view of the viewed object, called e.g. by
// the object itself.
bool wxPropertyListView::OnUpdateView(void)
{
  return TRUE;
}

bool wxPropertyListView::UpdatePropertyList(bool clearEditArea)
{
  if (!propertyScrollingList || !propertySheet)
    return FALSE;

  propertyScrollingList->Clear();
  if (clearEditArea)
  {
    valueList->Clear();
    valueText->SetValue("");
  }
  wxNode *node = propertySheet->GetProperties().First();

  // Should sort them... later...
  while (node)
  {
    wxProperty *property = (wxProperty *)node->Data();
    wxString stringValueRepr(property->GetValue().GetStringRepresentation());
    wxString paddedString(MakeNameValueString(property->GetName(), stringValueRepr));

    propertyScrollingList->Append(paddedString.GetData(), (char *)property);
    node = node->Next();
  }
  return TRUE;
}

bool wxPropertyListView::UpdatePropertyDisplayInList(wxProperty *property)
{
  if (!propertyScrollingList || !propertySheet)
    return FALSE;

  int currentlySelected = propertyScrollingList->GetSelection();
// #ifdef __WXMSW__
  wxString stringValueRepr(property->GetValue().GetStringRepresentation());
  wxString paddedString(MakeNameValueString(property->GetName(), stringValueRepr));
  int sel = FindListIndexForProperty(property);

  if (sel > -1)
  {
    // Don't update the listbox unnecessarily because it can cause
    // ugly flashing.
    
    if (paddedString != propertyScrollingList->GetString(sel))
      propertyScrollingList->SetString(sel, paddedString.GetData());
  }
//#else
//  UpdatePropertyList(FALSE);
//#endif

  if (currentlySelected > -1)
    propertyScrollingList->SetSelection(currentlySelected);

  return TRUE;
}

// Find the wxListBox index corresponding to this property
int wxPropertyListView::FindListIndexForProperty(wxProperty *property)
{
  int n = propertyScrollingList->Number();
  for (int i = 0; i < n; i++)
  {
    if (property == (wxProperty *)propertyScrollingList->wxListBox::GetClientData(i))
      return i;
  }
  return -1;
}

wxString wxPropertyListView::MakeNameValueString(wxString name, wxString value)
{
  wxString theString(name);

  int nameWidth = 25;
  int padWith = nameWidth - theString.Length();
  if (padWith < 0)
    padWith = 0;

  if (GetFlags() & wxPROP_SHOWVALUES)
  {
    // Want to pad with spaces
    theString.Append(' ', padWith);
    theString += value;
  }

  return theString;
}

// Select and show string representation in validator the given
// property. NULL resets to show no property.
bool wxPropertyListView::ShowProperty(wxProperty *property, bool select)
{
  if (currentProperty)
  {
    EndShowingProperty(currentProperty);
    currentProperty = NULL;
  }

  valueList->Clear();
  valueText->SetValue("");

  if (property)
  {
    currentProperty = property;
    BeginShowingProperty(property);
  }
  if (select)
  {
    int sel = FindListIndexForProperty(property);
    if (sel > -1)
      propertyScrollingList->SetSelection(sel);
  }
  return TRUE;
}

// Find appropriate validator and load property into value controls
bool wxPropertyListView::BeginShowingProperty(wxProperty *property)
{
  currentValidator = FindPropertyValidator(property);
  if (!currentValidator)
    return FALSE;

  if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return FALSE;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

  listValidator->OnPrepareControls(property, this, propertyWindow);
  DisplayProperty(property);
  return TRUE;
}

// Find appropriate validator and unload property from value controls
bool wxPropertyListView::EndShowingProperty(wxProperty *property)
{
  if (!currentValidator)
    return FALSE;

  RetrieveProperty(property);

  if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return FALSE;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

  listValidator->OnClearControls(property, this, propertyWindow);
  if (detailedEditing)
  {
    listValidator->OnClearDetailControls(property, this, propertyWindow);
    detailedEditing = FALSE;
  }
  return TRUE;
}

void wxPropertyListView::BeginDetailedEditing(void)
{
  if (!currentValidator)
    return;
  if (!currentProperty)
    return;
  if (detailedEditing)
    return;
  if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return;
  if (!currentProperty->IsEnabled())
    return;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

  if (listValidator->OnPrepareDetailControls(currentProperty, this, propertyWindow))
    detailedEditing = TRUE;
}

void wxPropertyListView::EndDetailedEditing(void)
{
  if (!currentValidator)
    return;
  if (!currentProperty)
    return;

  RetrieveProperty(currentProperty);

  if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

  if (detailedEditing)
  {
    listValidator->OnClearDetailControls(currentProperty, this, propertyWindow);
    detailedEditing = FALSE;
  }
}

bool wxPropertyListView::DisplayProperty(wxProperty *property)
{
  if (!currentValidator)
    return FALSE;

  if (((currentValidator->GetFlags() & wxPROP_ALLOW_TEXT_EDITING) == 0) || !property->IsEnabled())
    valueText->SetEditable(FALSE);
  else
    valueText->SetEditable(TRUE);

  if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return FALSE;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

  listValidator->OnDisplayValue(property, this, propertyWindow);
  return TRUE;
}

bool wxPropertyListView::RetrieveProperty(wxProperty *property)
{
  if (!currentValidator)
    return FALSE;
  if (!property->IsEnabled())
    return FALSE;

  if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
    return FALSE;

  wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

  if (listValidator->OnCheckValue(property, this, propertyWindow))
  {
    if (listValidator->OnRetrieveValue(property, this, propertyWindow))
    {
      UpdatePropertyDisplayInList(property);
      OnPropertyChanged(property);
    }
  }
  else
  {
    // Revert to old value
    listValidator->OnDisplayValue(property, this, propertyWindow);
  }
  return TRUE;
}


bool wxPropertyListView::EditProperty(wxProperty *WXUNUSED(property))
{
  return TRUE;
}

// Called by the listbox callback
void wxPropertyListView::OnPropertySelect(wxCommandEvent& WXUNUSED(event))
{
  int sel = propertyScrollingList->GetSelection();
  if (sel > -1)
  {
    wxProperty *newSel = (wxProperty *)propertyScrollingList->wxListBox::GetClientData(sel);
    if (newSel && newSel != currentProperty)
    {
      ShowProperty(newSel, FALSE);
    }
  }
}

bool wxPropertyListView::CreateControls(void)
{
  wxPanel *panel = (wxPanel *)propertyWindow;

  int largeButtonWidth = 60;
  int largeButtonHeight = 25;

  int smallButtonWidth = 25;
  int smallButtonHeight = 20;

  // XView must be allowed to choose its own sized buttons
#ifdef __XVIEW__
  largeButtonWidth = -1;
  largeButtonHeight = -1;

  smallButtonWidth = -1;
  smallButtonHeight = -1;
#endif
  
  if (valueText)
    return TRUE;
    
  if (!panel)
    return FALSE;

  wxWindow *leftMostWindow = panel;
  wxWindow *topMostWindow = panel;
  wxWindow *rightMostWindow = panel;

  wxSystemSettings settings;
  wxFont guiFont = settings.GetSystemFont(wxSYS_DEFAULT_GUI_FONT);

#ifdef __WXMSW__
  wxFont *boringFont = wxTheFontList->FindOrCreateFont(guiFont.GetPointSize(), wxDEFAULT, wxNORMAL, wxNORMAL, FALSE, "Courier New");
#else
  wxFont *boringFont = wxTheFontList->FindOrCreateFont(guiFont.GetPointSize(), wxMODERN, wxNORMAL, wxNORMAL);
#endif

  // May need to be changed in future to eliminate clashes with app.
  panel->SetClientData((char *)this);

  // These buttons are at the bottom of the window, but create them now
  // so the constraints are evaluated in the correct order
  if (buttonFlags & wxPROP_BUTTON_OK)
  {
    windowCloseButton = new wxButton(panel, wxID_OK, "OK",
     wxPoint(-1, -1), wxSize(largeButtonWidth, largeButtonHeight));
    windowCloseButton->SetDefault();
    windowCloseButton->SetFocus();
  }
  else if (buttonFlags & wxPROP_BUTTON_CLOSE)
  {
    windowCloseButton = new wxButton(panel, wxID_OK, "Close",
     wxPoint(-1, -1), wxSize(largeButtonWidth, largeButtonHeight));
  }
  if (buttonFlags & wxPROP_BUTTON_CANCEL)
  {
    windowCancelButton = new wxButton(panel, wxID_CANCEL, "Cancel",
     wxPoint(-1, -1), wxSize(largeButtonWidth, largeButtonHeight));
  }
  if (buttonFlags & wxPROP_BUTTON_HELP)
  {
    windowHelpButton = new wxButton(panel, wxID_HELP, "Help",
     wxPoint(-1, -1), wxSize(largeButtonWidth, largeButtonHeight));
  }

  if (windowCloseButton)
  {
    wxLayoutConstraints *c1 = new wxLayoutConstraints;

    c1->left.SameAs       (panel, wxLeft, 2);
    c1->bottom.SameAs        (panel, wxBottom, 2);
    c1->width.AsIs();
    c1->height.AsIs();
    windowCloseButton->SetConstraints(c1);
    leftMostWindow = windowCloseButton;
  }
  if (windowCancelButton)
  {
    wxLayoutConstraints *c2 = new wxLayoutConstraints;

    c2->right.SameAs       (panel, wxRight, 2);
    c2->bottom.SameAs          (panel, wxBottom, 2);
    c2->width.AsIs();
    c2->height.AsIs();
    windowCancelButton->SetConstraints(c2);
    leftMostWindow = windowCancelButton;
  }
  if (windowHelpButton)
  {
    wxLayoutConstraints *c2 = new wxLayoutConstraints;
    if (leftMostWindow == panel)
      c2->left.SameAs       (panel, wxLeft, 2);
    else
      c2->left.RightOf      (leftMostWindow, 2);
      
    c2->bottom.SameAs          (panel, wxBottom, 2);
    c2->width.AsIs();
    c2->height.AsIs();
    windowHelpButton->SetConstraints(c2);
    leftMostWindow = windowHelpButton;
  }

  if (buttonFlags & wxPROP_BUTTON_CHECK_CROSS)
  {
/*
    if (!tickBitmap)
    {
#ifdef __WXMSW__
      tickBitmap = new wxBitmap("tick_bmp", wxBITMAP_TYPE_RESOURCE);
      crossBitmap =  new wxBitmap("cross_bmp", wxBITMAP_TYPE_RESOURCE);
      if (!tickBitmap || !crossBitmap || !tickBitmap->Ok() || !crossBitmap->Ok())
      {
        if (tickBitmap)
          delete tickBitmap;
        if (crossBitmap)
          delete crossBitmap;
        tickBitmap = NULL;
        crossBitmap = NULL;
      }
#endif
    }
*/
/*
    if (tickBitmap && crossBitmap)
    {
      confirmButton = new wxBitmapButton(panel, wxID_PROP_CHECK, tickBitmap,
       wxPoint(-1, -1), wxSize(smallButtonWidth-5, smallButtonHeight-5));
      cancelButton = new wxBitmapButton(panel, wxID_PROP_CROSS, crossBitmap,
       wxPoint(-1, -1), wxSize(smallButtonWidth-5, smallButtonHeight-5));
    }
    else
*/
    {
      confirmButton = new wxButton(panel, wxID_PROP_CHECK, ":-)",
       wxPoint(-1, -1), wxSize(smallButtonWidth, smallButtonHeight));
      cancelButton = new wxButton(panel, wxID_PROP_CROSS, "X",
       wxPoint(-1, -1), wxSize(smallButtonWidth, smallButtonHeight));
    }

    wxLayoutConstraints *c = new wxLayoutConstraints;
    c->left.SameAs         (panel, wxLeft, 2);
/*
    if (windowCloseButton)
      c->top.Below         (windowCloseButton, 2);
    else
*/
      c->top.SameAs        (panel, wxTop, 2);

    c->width.AsIs();
    c->height.AsIs();

    cancelButton->SetConstraints(c);

    c = new wxLayoutConstraints;
    c->left.RightOf        (cancelButton, 2);
    c->top.SameAs          (cancelButton, wxTop, 0);
    c->width.AsIs();
    c->height.AsIs();

    confirmButton->SetConstraints(c);

    cancelButton->Enable(FALSE);
    confirmButton->Enable(FALSE);
  }

  if (buttonFlags & wxPROP_PULLDOWN)
  {
    editButton = new wxButton(panel, wxID_PROP_EDIT, "...",
     wxPoint(-1, -1), wxSize(smallButtonWidth, smallButtonHeight));
    editButton->Enable(FALSE);
    wxLayoutConstraints *c = new wxLayoutConstraints;

/*
    if (windowCloseButton)
      c->top.Below           (windowCloseButton, 2);
    else
*/
      c->top.SameAs          (panel, wxTop, 2);

    c->right.SameAs          (panel, wxRight, 2);
    c->width.AsIs();
    c->height.AsIs();
    editButton->SetConstraints(c);
  }

  valueText = new wxPropertyTextEdit(this, panel, wxID_PROP_TEXT, "", wxPoint(-1, -1), wxSize(-1, -1), wxPROCESS_ENTER);
  valueText->Enable(FALSE);
  
  wxLayoutConstraints *c = new wxLayoutConstraints;

  if (cancelButton)
    c->left.RightOf        (confirmButton, 2);
  else
    c->left.SameAs         (panel, wxLeft, 2);
/*
  if (windowCloseButton)
    c->top.Below           (windowCloseButton, 2);
  else
*/
    c->top.SameAs          (panel, wxTop, 2);

  if (editButton)
    c->right.LeftOf        (editButton, 2);
  else
    c->right.SameAs        (panel, wxRight, 2);
  c->height.AsIs();

  valueText->SetConstraints(c);

  valueList = new wxListBox(panel, wxID_PROP_VALUE_SELECT, wxPoint(-1, -1), wxSize(-1, 60));
  valueList->Show(FALSE);

  c = new wxLayoutConstraints;

  c->left.SameAs         (panel, wxLeft, 2);
  c->top.Below           (valueText, 2);
  c->right.SameAs        (panel, wxRight, 2);
  c->height.Absolute(60);

  valueList->SetConstraints(c);

  propertyScrollingList = new wxListBox(panel, wxID_PROP_SELECT,
    wxPoint(-1, -1), wxSize(300, 300));
  propertyScrollingList->SetFont(boringFont);

  c = new wxLayoutConstraints;

  c->left.SameAs         (panel, wxLeft, 2);

  if (buttonFlags & wxPROP_DYNAMIC_VALUE_FIELD)
    c->top.Below         (valueText, 2);
  else
    c->top.Below         (valueList, 2);

  c->right.SameAs        (panel, wxRight, 2);

  if (windowCloseButton)
    c->bottom.Above       (windowCloseButton, -2);
  else
    c->bottom.SameAs       (panel, wxBottom, 2);

  propertyScrollingList->SetConstraints(c);


  // Note: if this is called now, it causes a GPF.
  // Why?
//  panel->Layout();

  return TRUE;
}

void wxPropertyListView::ShowTextControl(bool show)
{
  if (valueText)
    valueText->Show(show);
}

void wxPropertyListView::ShowListBoxControl(bool show)
{
  if (valueList)
  {
    valueList->Show(show);
    if (buttonFlags & wxPROP_DYNAMIC_VALUE_FIELD)
    {
      wxLayoutConstraints *constraints = propertyScrollingList->GetConstraints();
      if (constraints)
      {
        if (show)
          constraints->top.Below(valueList, 2);
        else
          constraints->top.Below(valueText, 2);
        propertyWindow->Layout();
      }
    }
  }
}

void wxPropertyListView::EnableCheck(bool show)
{
  if (confirmButton)
    confirmButton->Enable(show);
}

void wxPropertyListView::EnableCross(bool show)
{
  if (cancelButton)
    cancelButton->Enable(show);
}

bool wxPropertyListView::OnClose(void)
{
  // Retrieve the value if any
  wxCommandEvent event;
  OnCheck(event);
  
  delete this;
  return TRUE;
}

void wxPropertyListView::OnValueListSelect(wxCommandEvent& WXUNUSED(event))
{
  if (currentProperty && currentValidator)
  {
    if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
      return;

    wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

    listValidator->OnValueListSelect(currentProperty, this, propertyWindow);
  }
}

void wxPropertyListView::OnOk(wxCommandEvent& event)
{
  // Retrieve the value if any
  OnCheck(event);
  
  managedWindow->Close(TRUE);
}

void wxPropertyListView::OnCancel(wxCommandEvent& WXUNUSED(event))
{
//  SetReturnCode(wxID_CANCEL);
  managedWindow->Close(TRUE);
  dialogCancelled = TRUE;
}

void wxPropertyListView::OnHelp(wxCommandEvent& WXUNUSED(event))
{
}

void wxPropertyListView::OnCheck(wxCommandEvent& WXUNUSED(event))
{
  if (currentProperty)
  {
    RetrieveProperty(currentProperty);
  }
}

void wxPropertyListView::OnCross(wxCommandEvent& WXUNUSED(event))
{
  if (currentProperty && currentValidator)
  {
    if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
      return;

    wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

    // Revert to old value
    listValidator->OnDisplayValue(currentProperty, this, propertyWindow);
  }
}

void wxPropertyListView::OnPropertyDoubleClick(wxCommandEvent& WXUNUSED(event))
{
  if (currentProperty && currentValidator)
  {
    if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
      return;

    wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

    // Revert to old value
    listValidator->OnDoubleClick(currentProperty, this, propertyWindow);
  }
}

void wxPropertyListView::OnEdit(wxCommandEvent& WXUNUSED(event))
{
  if (currentProperty && currentValidator)
  {
    if (!currentValidator->IsKindOf(CLASSINFO(wxPropertyListValidator)))
      return;

    wxPropertyListValidator *listValidator = (wxPropertyListValidator *)currentValidator;

    listValidator->OnEdit(currentProperty, this, propertyWindow);
  }
}

void wxPropertyListView::OnText(wxCommandEvent& event)
{
  if (event.GetEventType() == wxEVT_COMMAND_TEXT_ENTER)
  {
    OnCheck(event);
  }
}

/*
 * Property dialog box
 */
 
IMPLEMENT_CLASS(wxPropertyListDialog, wxDialog)

BEGIN_EVENT_TABLE(wxPropertyListDialog, wxDialog)
    EVT_BUTTON(wxID_CANCEL,				wxPropertyListDialog::OnCancel)
END_EVENT_TABLE()

wxPropertyListDialog::wxPropertyListDialog(wxPropertyListView *v, wxWindow *parent,
	const wxString& title, const wxPoint& pos,
    const wxSize& size, long style, const wxString& name):
     wxDialog(parent, -1, title, pos, size, style, name)
{
  view = v;
  view->AssociatePanel( ((wxPanel*)this) );
  view->SetManagedWindow(this);
  SetAutoLayout(TRUE);
}

bool wxPropertyListDialog::OnClose(void)
{
  if (view)
  {
	SetReturnCode(wxID_CANCEL);
    view->OnClose();
	view = NULL;
	return TRUE;
  }
  else
    return FALSE;
}

void wxPropertyListDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	SetReturnCode(wxID_CANCEL);
    this->Close();
}

void wxPropertyListDialog::OnDefaultAction(wxControl *item)
{
/*
  if (item == view->GetPropertyScrollingList())
    view->OnDoubleClick();
*/
}

// Extend event processing to search the view's event table
bool wxPropertyListDialog::ProcessEvent(wxEvent& event)
{
	if ( !view || ! view->ProcessEvent(event) )
		return wxEvtHandler::ProcessEvent(event);
	else
		return TRUE;
}

/*
 * Property panel
 */
 
IMPLEMENT_CLASS(wxPropertyListPanel, wxPanel)

BEGIN_EVENT_TABLE(wxPropertyListPanel, wxPanel)
    EVT_SIZE(wxPropertyListPanel::OnSize)
END_EVENT_TABLE()

void wxPropertyListPanel::OnDefaultAction(wxControl *item)
{
/*
  if (item == view->GetPropertyScrollingList())
    view->OnDoubleClick();
*/
}

// Extend event processing to search the view's event table
bool wxPropertyListPanel::ProcessEvent(wxEvent& event)
{
	if ( !view || ! view->ProcessEvent(event) )
		return wxEvtHandler::ProcessEvent(event);
	else
		return TRUE;
}

void wxPropertyListPanel::OnSize(wxSizeEvent& WXUNUSED(event))
{
    Layout();
}

/*
 * Property frame
 */
 
IMPLEMENT_CLASS(wxPropertyListFrame, wxFrame)

bool wxPropertyListFrame::OnClose(void)
{
  if (view)
  {
    if (propertyPanel)
        propertyPanel->SetView(NULL);
    view->OnClose();
    view = NULL;
    return TRUE;
  }
  else
    return FALSE;
}

wxPropertyListPanel *wxPropertyListFrame::OnCreatePanel(wxFrame *parent, wxPropertyListView *v)
{
  return new wxPropertyListPanel(v, parent);
}

bool wxPropertyListFrame::Initialize(void)
{
  propertyPanel = OnCreatePanel(this, view);
  if (propertyPanel)
  {
    view->AssociatePanel(propertyPanel);
    view->SetManagedWindow(this);
    propertyPanel->SetAutoLayout(TRUE);
    return TRUE;
  }
  else
    return FALSE;
}

 /*
  * Property list specific validator
  */
  
IMPLEMENT_ABSTRACT_CLASS(wxPropertyListValidator, wxPropertyValidator)

bool wxPropertyListValidator::OnSelect(bool select, wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
//  view->GetValueText()->Show(TRUE);
  if (select)
    OnDisplayValue(property, view, parentWindow);

  return TRUE;
}

bool wxPropertyListValidator::OnValueListSelect(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  wxString s(view->GetValueList()->GetStringSelection());
  if (s != "")
  {
    view->GetValueText()->SetValue(s);
    view->RetrieveProperty(property);
  }
  return TRUE;
}

bool wxPropertyListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
//  view->GetValueText()->Show(TRUE);
  wxString str(property->GetValue().GetStringRepresentation());

  view->GetValueText()->SetValue(str.GetData());
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxPropertyListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  return FALSE;
}

void wxPropertyListValidator::OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetDetailedEditing())
    view->EndDetailedEditing();
  else
    view->BeginDetailedEditing();
}

bool wxPropertyListValidator::OnClearControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(FALSE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(FALSE);
  if (view->GetEditButton())
    view->GetEditButton()->Enable(FALSE);
  return TRUE;
}

/*
 * Default validators
 */

IMPLEMENT_DYNAMIC_CLASS(wxRealListValidator, wxPropertyListValidator)

///
/// Real number validator
/// 
bool wxRealListValidator::OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (realMin == 0.0 && realMax == 0.0)
    return TRUE;
    
  if (!view->GetValueText())
    return FALSE;
  wxString value(view->GetValueText()->GetValue());

  float val = 0.0;
  if (!StringToFloat(WXSTRINGCAST value, &val))
  {
    char buf[200];
    sprintf(buf, "Value %s is not a valid real number!", value.GetData());
    wxMessageBox(buf, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  
  if (val < realMin || val > realMax)
  {
    char buf[200];
    sprintf(buf, "Value must be a real number between %.2f and %.2f!", realMin, realMax);
    wxMessageBox(buf, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxRealListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;

  if (strlen(view->GetValueText()->GetValue()) == 0)
    return FALSE;
    
  wxString value(view->GetValueText()->GetValue());
  float f = (float)atof(value.GetData());
  property->GetValue() = f;
  return TRUE;
}

bool wxRealListValidator::OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(TRUE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(TRUE);
  if (view->GetEditButton())
    view->GetEditButton()->Enable(FALSE);
  if (view->GetValueText())
    view->GetValueText()->Enable(TRUE);
  return TRUE;
}

///
/// Integer validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxIntegerListValidator, wxPropertyListValidator)

bool wxIntegerListValidator::OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (integerMin == 0 && integerMax == 0)
    return TRUE;
    
  if (!view->GetValueText())
    return FALSE;
  wxString value(view->GetValueText()->GetValue());

  long val = 0;
  if (!StringToLong(WXSTRINGCAST value, &val))
  {
    char buf[200];
    sprintf(buf, "Value %s is not a valid integer!", value.GetData());
    wxMessageBox(buf, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  if (val < integerMin || val > integerMax)
  {
    char buf[200];
    sprintf(buf, "Value must be an integer between %ld and %ld!", integerMin, integerMax);
    wxMessageBox(buf, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxIntegerListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;

  if (strlen(view->GetValueText()->GetValue()) == 0)
    return FALSE;
    
  wxString value(view->GetValueText()->GetValue());
  long val = (long)atoi(value.GetData());
  property->GetValue() = (long)val;
  return TRUE;
}

bool wxIntegerListValidator::OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(TRUE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(TRUE);
  if (view->GetEditButton())
    view->GetEditButton()->Enable(FALSE);
  if (view->GetValueText())
    view->GetValueText()->Enable(TRUE);
  return TRUE;
}

///
/// boolean validator
///
IMPLEMENT_DYNAMIC_CLASS(wxBoolListValidator, wxPropertyListValidator)

bool wxBoolListValidator::OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString value(view->GetValueText()->GetValue());
  if (value != "True" && value != "False")
  {
    wxMessageBox("Value must be True or False!", "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxBoolListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;

  if (strlen(view->GetValueText()->GetValue()) == 0)
    return FALSE;
    
  wxString value(view->GetValueText()->GetValue());
  bool boolValue = FALSE;
  if (value == "True")
    boolValue = TRUE;
  else
    boolValue = FALSE;
  property->GetValue() = (bool)boolValue;
  return TRUE;
}

bool wxBoolListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString str(property->GetValue().GetStringRepresentation());

  view->GetValueText()->SetValue(str.GetData());
  view->GetValueList()->SetStringSelection(str.GetData());
  return TRUE;
}

bool wxBoolListValidator::OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(FALSE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(FALSE);
  if (view->GetEditButton())
    view->GetEditButton()->Enable(TRUE);
  if (view->GetValueText())
    view->GetValueText()->Enable(FALSE);
  return TRUE;
}

bool wxBoolListValidator::OnPrepareDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetValueList())
  {
    view->ShowListBoxControl(TRUE);
    view->GetValueList()->Enable(TRUE);
    
    view->GetValueList()->Append("True");
    view->GetValueList()->Append("False");
    char *currentString = copystring(view->GetValueText()->GetValue());
    view->GetValueList()->SetStringSelection(currentString);
    delete[] currentString;
  }
  return TRUE;
}

bool wxBoolListValidator::OnClearDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetValueList())
  {
    view->GetValueList()->Clear();
    view->ShowListBoxControl(FALSE);
    view->GetValueList()->Enable(FALSE);
  }
  return TRUE;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
bool wxBoolListValidator::OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  if (property->GetValue().BoolValue())
    property->GetValue() = (bool)FALSE;
  else
    property->GetValue() = (bool)TRUE;
  view->DisplayProperty(property);
  view->UpdatePropertyDisplayInList(property);
  view->OnPropertyChanged(property);
  return TRUE;
}

///
/// String validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxStringListValidator, wxPropertyListValidator)

wxStringListValidator::wxStringListValidator(wxStringList *list, long flags):
  wxPropertyListValidator(flags)
{
  strings = list;
  // If no constraint, we just allow the string to be edited.
  if (!strings && ((validatorFlags & wxPROP_ALLOW_TEXT_EDITING) == 0))
    validatorFlags |= wxPROP_ALLOW_TEXT_EDITING;
}

bool wxStringListValidator::OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!strings)
    return TRUE;

  if (!view->GetValueText())
    return FALSE;
  wxString value(view->GetValueText()->GetValue());

  if (!strings->Member(value.GetData()))
  {
    wxString s("Value ");
    s += value.GetData();
    s += " is not valid.";
    wxMessageBox(s.GetData(), "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxStringListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString value(view->GetValueText()->GetValue());
  property->GetValue() = value ;
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxStringListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString str(property->GetValue().GetStringRepresentation());
  view->GetValueText()->SetValue(str.GetData());
  if (strings)
  {
    view->GetValueList()->SetStringSelection(str.GetData());
  }
  return TRUE;
}

bool wxStringListValidator::OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  // Unconstrained
  if (!strings)
  {
    if (view->GetEditButton())
      view->GetEditButton()->Enable(FALSE);
    if (view->GetConfirmButton())
      view->GetConfirmButton()->Enable(TRUE);
    if (view->GetCancelButton())
      view->GetCancelButton()->Enable(TRUE);
    if (view->GetValueText())
      view->GetValueText()->Enable(TRUE);
    return TRUE;
  }
  
  // Constrained
  if (view->GetValueText())
    view->GetValueText()->Enable(FALSE);

  if (view->GetEditButton())
    view->GetEditButton()->Enable(TRUE);

  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(FALSE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(FALSE);
  return TRUE;
}

bool wxStringListValidator::OnPrepareDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetValueList())
  {
    view->ShowListBoxControl(TRUE);
    view->GetValueList()->Enable(TRUE);
    wxNode *node = strings->First();
    while (node)
    {
      char *s = (char *)node->Data();
      view->GetValueList()->Append(s);
      node = node->Next();
    }
    char *currentString = property->GetValue().StringValue();
    view->GetValueList()->SetStringSelection(currentString);
  }
  return TRUE;
}

bool wxStringListValidator::OnClearDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!strings)
  {
    return TRUE;
  }

  if (view->GetValueList())
  {
    view->GetValueList()->Clear();
    view->ShowListBoxControl(FALSE);
    view->GetValueList()->Enable(FALSE);
  }
  return TRUE;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
bool wxStringListValidator::OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  if (!strings)
    return FALSE;

  wxNode *node = strings->First();
  char *currentString = property->GetValue().StringValue();
  while (node)
  {
    char *s = (char *)node->Data();
    if (strcmp(s, currentString) == 0)
    {
      char *nextString = NULL;
      if (node->Next())
        nextString = (char *)node->Next()->Data();
      else
        nextString = (char *)strings->First()->Data();
      property->GetValue() = wxString(nextString);
      view->DisplayProperty(property);
      view->UpdatePropertyDisplayInList(property);
      view->OnPropertyChanged(property);
      return TRUE;
    }
    else node = node->Next();
  }
  return TRUE;
}

///
/// Filename validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxFilenameListValidator, wxPropertyListValidator)

wxFilenameListValidator::wxFilenameListValidator(wxString message , wxString wildcard, long flags):
  wxPropertyListValidator(flags), filenameWildCard(wildcard), filenameMessage(message)
{
}

wxFilenameListValidator::~wxFilenameListValidator(void)
{
}

bool wxFilenameListValidator::OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxFilenameListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString value(view->GetValueText()->GetValue());
  property->GetValue() = value ;
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxFilenameListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString str(property->GetValue().GetStringRepresentation());
  view->GetValueText()->SetValue(str);
  return TRUE;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
bool wxFilenameListValidator::OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  OnEdit(property, view, parentWindow);
  return TRUE;
}

bool wxFilenameListValidator::OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(TRUE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(TRUE);
  if (view->GetEditButton())
    view->GetEditButton()->Enable(TRUE);
  if (view->GetValueText())
    view->GetValueText()->Enable((GetFlags() & wxPROP_ALLOW_TEXT_EDITING) == wxPROP_ALLOW_TEXT_EDITING);
  return TRUE;
}

void wxFilenameListValidator::OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return;

  char *s = wxFileSelector(
     filenameMessage.GetData(),
     wxPathOnly(property->GetValue().StringValue()),
     wxFileNameFromPath(property->GetValue().StringValue()),
     NULL,
     filenameWildCard.GetData(),
     0,
     parentWindow);
  if (s)
  {
    property->GetValue() = wxString(s);
    view->DisplayProperty(property);
    view->UpdatePropertyDisplayInList(property);
    view->OnPropertyChanged(property);
  }
}

///
/// Colour validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxColourListValidator, wxPropertyListValidator)

wxColourListValidator::wxColourListValidator(long flags):
  wxPropertyListValidator(flags)
{
}

wxColourListValidator::~wxColourListValidator(void)
{
}

bool wxColourListValidator::OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxColourListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString value(view->GetValueText()->GetValue());
  
  property->GetValue() = value ;
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself
bool wxColourListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString str(property->GetValue().GetStringRepresentation());
  view->GetValueText()->SetValue(str);
  return TRUE;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
bool wxColourListValidator::OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  OnEdit(property, view, parentWindow);
  return TRUE;
}

bool wxColourListValidator::OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(TRUE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(TRUE);
  if (view->GetEditButton())
    view->GetEditButton()->Enable(TRUE);
  if (view->GetValueText())
    view->GetValueText()->Enable((GetFlags() & wxPROP_ALLOW_TEXT_EDITING) == wxPROP_ALLOW_TEXT_EDITING);
  return TRUE;
}

void wxColourListValidator::OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return;
    
  char *s = property->GetValue().StringValue();
  int r = 0;
  int g = 0;
  int b = 0;
  if (s)
  {
    r = wxHexToDec(s);
    g = wxHexToDec(s+2);
    b = wxHexToDec(s+4);
  }
  
  wxColour col(r,g,b);
  
  wxColourData data;
  data.SetChooseFull(TRUE);
  data.SetColour(col);
  
  for (int i = 0; i < 16; i++)
  {
    wxColour colour(i*16, i*16, i*16);
    data.SetCustomColour(i, colour);
  }
      
  wxColourDialog dialog(parentWindow, &data);
  if (dialog.ShowModal() != wxID_CANCEL)
  {
    wxColourData retData = dialog.GetColourData();
    col = retData.GetColour();
    
    char buf[7];
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

bool wxListOfStringsListValidator::OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  // No constraints for an arbitrary, user-editable list of strings.
  return TRUE;
}

// Called when TICK is pressed or focus is lost or view wants to update
// the property list.
// Does the transferance from the property editing area to the property itself.
// In this case, the user cannot directly edit the string list.
bool wxListOfStringsListValidator::OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  return TRUE;
}

bool wxListOfStringsListValidator::OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (!view->GetValueText())
    return FALSE;
  wxString str(property->GetValue().GetStringRepresentation());
  view->GetValueText()->SetValue(str.GetData());
  return TRUE;
}

bool wxListOfStringsListValidator::OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  if (view->GetEditButton())
    view->GetEditButton()->Enable(TRUE);
  if (view->GetValueText())
    view->GetValueText()->Enable(FALSE);

  if (view->GetConfirmButton())
    view->GetConfirmButton()->Enable(FALSE);
  if (view->GetCancelButton())
    view->GetCancelButton()->Enable(FALSE);
  return TRUE;
}

// Called when the property is double clicked. Extra functionality can be provided,
// cycling through possible values.
bool wxListOfStringsListValidator::OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  OnEdit(property, view, parentWindow);
  return TRUE;
}

void wxListOfStringsListValidator::OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow)
{
  // Convert property value to a list of strings for editing
  wxStringList *stringList = new wxStringList;
  
  wxPropertyValue *expr = property->GetValue().GetFirst();
  while (expr)
  {
    char *s = expr->StringValue();
    if (s)
      stringList->Add(s);
    expr = expr->GetNext();
  }
  
  wxString title("Editing ");
  title += property->GetName();
  
  if (EditStringList(parentWindow, stringList, title.GetData()))
  {
    wxPropertyValue& oldValue = property->GetValue();
    oldValue.ClearList();
    wxNode *node = stringList->First();
    while (node)
    {
      char *s = (char *)node->Data();
      oldValue.Append(new wxPropertyValue(s));
      
      node = node->Next();
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
    wxStringList *stringList;
    wxListBox *listBox;
    wxTextCtrl *stringText;
    static bool dialogCancelled;
    int currentSelection;
    wxPropertyStringListEditorDialog(wxWindow *parent, const wxString& title,
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      	long windowStyle = wxDEFAULT_DIALOG_STYLE, const wxString& name = "stringEditorDialogBox"):
       		wxDialog(parent, -1, title, pos, size, windowStyle, name)
    {
      stringList = NULL;
      stringText = NULL;
      listBox = NULL;
      dialogCancelled = FALSE;
      currentSelection = -1;
    }
    ~wxPropertyStringListEditorDialog(void) {}
    bool OnClose(void);
    void SaveCurrentSelection(void);
    void ShowCurrentSelection(void);

	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnAdd(wxCommandEvent& event);
	void OnDelete(wxCommandEvent& event);
	void OnStrings(wxCommandEvent& event);
	void OnText(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

#define	wxID_PROP_SL_ADD			3000
#define	wxID_PROP_SL_DELETE			3001
#define	wxID_PROP_SL_STRINGS		3002
#define	wxID_PROP_SL_TEXT			3003

BEGIN_EVENT_TABLE(wxPropertyStringListEditorDialog, wxDialog)
	EVT_BUTTON(wxID_OK, 				wxPropertyStringListEditorDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL,				wxPropertyStringListEditorDialog::OnCancel)
    EVT_BUTTON(wxID_PROP_SL_ADD,		wxPropertyStringListEditorDialog::OnAdd)
    EVT_BUTTON(wxID_PROP_SL_DELETE,		wxPropertyStringListEditorDialog::OnDelete)
    EVT_LISTBOX(wxID_PROP_SL_STRINGS,	wxPropertyStringListEditorDialog::OnStrings)
    EVT_TEXT(wxID_PROP_SL_TEXT,			wxPropertyStringListEditorDialog::OnText)
END_EVENT_TABLE()

class wxPropertyStringListEditorText: public wxTextCtrl
{
 public:
  wxPropertyStringListEditorText(wxWindow *parent, wxWindowID id, const wxString& val,
  	const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long windowStyle = 0, const wxString& name = "text"):
     wxTextCtrl(parent, id, val, pos, size, windowStyle, wxDefaultValidator, name)
  {
  }
  void OnKillFocus(void)
  {
    wxPropertyStringListEditorDialog *dialog = (wxPropertyStringListEditorDialog *)GetParent();
    dialog->SaveCurrentSelection();
  }
};

bool wxPropertyStringListEditorDialog::dialogCancelled = FALSE;

// Edit the string list.
bool wxListOfStringsListValidator::EditStringList(wxWindow *parent, wxStringList *stringList, const char *title)
{
  wxBeginBusyCursor();
  wxPropertyStringListEditorDialog *dialog = new wxPropertyStringListEditorDialog(parent,
  	title, wxPoint(10, 10), wxSize(400, 400), wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL);
  
  dialog->stringList = stringList;
  
  wxButton *okButton = new wxButton(dialog, wxID_OK, "OK", wxPoint(5, 5));
  wxButton *cancelButton = new wxButton(dialog, wxID_CANCEL, "Cancel", wxPoint(40, 5));

//  wxButton *helpButton = new wxButton(dialog, (wxFunction)StringListEditorHelpProc, "Help");
//  helpButton->SetClientData((char *)this);

  dialog->listBox = new wxListBox(dialog, wxID_PROP_SL_STRINGS,
    wxPoint(5, 30), wxSize(300, 200), 0, NULL, wxLB_SINGLE);

  dialog->stringText = new wxPropertyStringListEditorText(dialog,
  wxID_PROP_SL_TEXT, "", wxPoint(5, 240),
       wxSize(300, -1), wxPROCESS_ENTER);
  dialog->stringText->Enable(FALSE);

  wxButton *addButton = new wxButton(dialog, wxID_PROP_SL_ADD, "Add", wxPoint(5, 280));
  wxButton *deleteButton = new wxButton(dialog, wxID_PROP_SL_DELETE, "Delete", wxPoint(40, 280));

  wxNode *node = stringList->First();
  while (node)
  {
    char *str = (char *)node->Data();
    // Save node as client data for each listbox item
    dialog->listBox->Append(str, (char *)node);
    node = node->Next();
  }

  dialog->SetClientSize(310, 305);

  dialog->Centre(wxBOTH);
  wxEndBusyCursor();
  if (dialog->ShowModal() == wxID_CANCEL)
	return FALSE;
  else
	return TRUE;
}

/*
 * String list editor callbacks
 *
 */

void wxPropertyStringListEditorDialog::OnStrings(wxCommandEvent& WXUNUSED(event))
{
  int sel = listBox->GetSelection();
  if (sel > -1)
  {
    currentSelection = sel;

    ShowCurrentSelection();
  }
}

void wxPropertyStringListEditorDialog::OnDelete(wxCommandEvent& WXUNUSED(event))
{
  int sel = listBox->GetSelection();
  if (sel == -1)
    return;
    
  wxNode *node = (wxNode *)listBox->wxListBox::GetClientData(sel);
  if (!node)
    return;
    
  listBox->Delete(sel);
  delete[] (char *)node->Data();
  delete node;
  currentSelection = -1;
  stringText->SetValue("");
}

void wxPropertyStringListEditorDialog::OnAdd(wxCommandEvent& WXUNUSED(event))
{
  SaveCurrentSelection();
  
  char *initialText = "";
  wxNode *node = stringList->Add(initialText);
  listBox->Append(initialText, (char *)node);
  currentSelection = stringList->Number() - 1;
  listBox->SetSelection(currentSelection);
  ShowCurrentSelection();
  stringText->SetFocus();
}

void wxPropertyStringListEditorDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
  SaveCurrentSelection();
  EndModal(wxID_OK);
  Close(TRUE);
}

void wxPropertyStringListEditorDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
  dialogCancelled = TRUE;
  EndModal(wxID_CANCEL);
  Close(TRUE);
}

void wxPropertyStringListEditorDialog::OnText(wxCommandEvent& event)
{
  if (event.GetEventType() == wxEVENT_TYPE_TEXT_ENTER_COMMAND)
  {
    SaveCurrentSelection();
  }
}

bool wxPropertyStringListEditorDialog::OnClose(void)
{
  SaveCurrentSelection();
  return TRUE;
}

void wxPropertyStringListEditorDialog::SaveCurrentSelection(void)
{
  if (currentSelection == -1)
    return;
    
  wxNode *node = (wxNode *)listBox->wxListBox::GetClientData(currentSelection);
  if (!node)
    return;
    
  wxString txt(stringText->GetValue());
  if (node->Data())
    delete[] (char *)node->Data();
  node->SetData((wxObject *)copystring(txt));
  
  listBox->SetString(currentSelection, (char *)node->Data());
}

void wxPropertyStringListEditorDialog::ShowCurrentSelection(void)
{
  if (currentSelection == -1)
  {
    stringText->SetValue("");
    return;
  }
  wxNode *node = (wxNode *)listBox->wxListBox::GetClientData(currentSelection);
  char *txt = (char *)node->Data();
  stringText->SetValue(txt);
  stringText->Enable(TRUE);
}
