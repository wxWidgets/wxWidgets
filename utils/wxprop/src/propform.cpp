/////////////////////////////////////////////////////////////////////////////
// Name:        propform.cpp
// Purpose:     Property form classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "propform.h"
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

#if defined(__WINDOWS__) && !defined(__GNUWIN32__)
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#include "wx/window.h"
#include "wx/utils.h"
#include "wx/list.h"
#include "propform.h"

/*
 * Property view
 */

IMPLEMENT_DYNAMIC_CLASS(wxPropertyFormView, wxPropertyView)

BEGIN_EVENT_TABLE(wxPropertyFormView, wxPropertyView)
	EVT_BUTTON(wxID_OK, 		wxPropertyFormView::OnOk)
	EVT_BUTTON(wxID_CANCEL, 	wxPropertyFormView::OnCancel)
	EVT_BUTTON(wxID_HELP, 		wxPropertyFormView::OnHelp)
	EVT_BUTTON(wxID_PROP_REVERT,	wxPropertyFormView::OnRevert)
	EVT_BUTTON(wxID_PROP_UPDATE,	wxPropertyFormView::OnUpdate)
END_EVENT_TABLE()

bool wxPropertyFormView::dialogCancelled = FALSE;

wxPropertyFormView::wxPropertyFormView(wxWindow *propPanel, long flags):wxPropertyView(flags)
{
  propertyWindow = propPanel;
  managedWindow = NULL;

  windowCloseButton = NULL;
  windowCancelButton = NULL;
  windowHelpButton = NULL;

  detailedEditing = FALSE;
}

wxPropertyFormView::~wxPropertyFormView(void)
{
}

void wxPropertyFormView::ShowView(wxPropertySheet *ps, wxWindow *panel)
{
  propertySheet = ps;
  
  AssociatePanel(panel);
//  CreateControls();
//  UpdatePropertyList();
}

// Update this view of the viewed object, called e.g. by
// the object itself.
bool wxPropertyFormView::OnUpdateView(void)
{
  return TRUE;
}

bool wxPropertyFormView::Check(void)
{
  if (!propertySheet)
    return FALSE;
    
  wxNode *node = propertySheet->GetProperties().First();
  while (node)
  {
    wxProperty *prop = (wxProperty *)node->Data();
    wxPropertyValidator *validator = FindPropertyValidator(prop);
    if (validator && validator->IsKindOf(CLASSINFO(wxPropertyFormValidator)))
    {
      wxPropertyFormValidator *formValidator = (wxPropertyFormValidator *)validator;
      if (!formValidator->OnCheckValue(prop, this, propertyWindow))
        return FALSE;
    }
    node = node->Next();
  }
  return TRUE;
}

bool wxPropertyFormView::TransferToPropertySheet(void)
{
  if (!propertySheet)
    return FALSE;
    
  wxNode *node = propertySheet->GetProperties().First();
  while (node)
  {
    wxProperty *prop = (wxProperty *)node->Data();
    wxPropertyValidator *validator = FindPropertyValidator(prop);
    if (validator && validator->IsKindOf(CLASSINFO(wxPropertyFormValidator)))
    {
      wxPropertyFormValidator *formValidator = (wxPropertyFormValidator *)validator;
      formValidator->OnRetrieveValue(prop, this, propertyWindow);
    }
    node = node->Next();
  }
  return TRUE;
}

bool wxPropertyFormView::TransferToDialog(void)
{
  if (!propertySheet)
    return FALSE;
    
  wxNode *node = propertySheet->GetProperties().First();
  while (node)
  {
    wxProperty *prop = (wxProperty *)node->Data();
    wxPropertyValidator *validator = FindPropertyValidator(prop);
    if (validator && validator->IsKindOf(CLASSINFO(wxPropertyFormValidator)))
    {
      wxPropertyFormValidator *formValidator = (wxPropertyFormValidator *)validator;
      formValidator->OnDisplayValue(prop, this, propertyWindow);
    }
    node = node->Next();
  }
  return TRUE;
}

bool wxPropertyFormView::AssociateNames(void)
{
  if (!propertySheet || !propertyWindow)
    return FALSE;

  wxNode *node = propertyWindow->GetChildren()->First();
  while (node)
  {
    wxWindow *win = (wxWindow *)node->Data();
    if (win->GetName() != "")
    {
      wxProperty *prop = propertySheet->GetProperty(win->GetName());
      if (prop)
        prop->SetWindow(win);
    }
    node = node->Next();
  }
  return TRUE;
}


bool wxPropertyFormView::OnClose(void)
{
  delete this;
  return TRUE;
}

void wxPropertyFormView::OnOk(wxCommandEvent& event)
{
  // Retrieve the value if any
  if (!Check())
    return;
  
  dialogCancelled = FALSE;

  managedWindow->Close(TRUE);
}

void wxPropertyFormView::OnCancel(wxCommandEvent& event)
{
  dialogCancelled = TRUE;
  
  managedWindow->Close(TRUE);
}

void wxPropertyFormView::OnHelp(wxCommandEvent& event)
{
}

void wxPropertyFormView::OnUpdate(wxCommandEvent& event)
{
  TransferToPropertySheet();
}

void wxPropertyFormView::OnRevert(wxCommandEvent& event)
{
  TransferToDialog();
}

void wxPropertyFormView::OnCommand(wxWindow& win, wxCommandEvent& event)
{
  if (!propertySheet)
    return;
    
  if (win.GetName() == "")
    return;

  if (strcmp(win.GetName(), "ok") == 0)
    OnOk(event);
  else if (strcmp(win.GetName(), "cancel") == 0)
    OnCancel(event);
  else if (strcmp(win.GetName(), "help") == 0)
    OnHelp(event);
  else if (strcmp(win.GetName(), "update") == 0)
    OnUpdate(event);
  else if (strcmp(win.GetName(), "revert") == 0)
    OnRevert(event);
  else
  {
    // Find a validator to route the command to.
    wxNode *node = propertySheet->GetProperties().First();
    while (node)
    {
      wxProperty *prop = (wxProperty *)node->Data();
      if (prop->GetWindow() && (prop->GetWindow() == &win))
      {
        wxPropertyValidator *validator = FindPropertyValidator(prop);
        if (validator && validator->IsKindOf(CLASSINFO(wxPropertyFormValidator)))
        {
          wxPropertyFormValidator *formValidator = (wxPropertyFormValidator *)validator;
          formValidator->OnCommand(prop, this, propertyWindow, event);
          return;
        }
      }
      node = node->Next();
    }
  }
}

void wxPropertyFormView::OnDoubleClick(wxControl *item)
{
  if (!propertySheet)
    return;
    
  // Find a validator to route the command to.
  wxNode *node = propertySheet->GetProperties().First();
  while (node)
  {
    wxProperty *prop = (wxProperty *)node->Data();
    if (prop->GetWindow() && ((wxControl *)prop->GetWindow() == item))
    {
      wxPropertyValidator *validator = FindPropertyValidator(prop);
      if (validator && validator->IsKindOf(CLASSINFO(wxPropertyFormValidator)))
      {
        wxPropertyFormValidator *formValidator = (wxPropertyFormValidator *)validator;
        formValidator->OnDoubleClick(prop, this, propertyWindow);
        return;
      }
    }
    node = node->Next();
  }
}

/*
 * Property form dialog box
 */
 
IMPLEMENT_CLASS(wxPropertyFormDialog, wxDialog)

wxPropertyFormDialog::wxPropertyFormDialog(wxPropertyFormView *v, wxWindow *parent, const wxString& title,
	const wxPoint& pos, const wxSize& size, long style, const wxString& name):
     wxDialog(parent, -1, title, pos, size, style, name)
{
  view = v;
  view->AssociatePanel(this);
  view->SetManagedWindow(this);
//  SetAutoLayout(TRUE);
}

bool wxPropertyFormDialog::OnClose(void)
{
  if (view)
  {
    view->OnClose();
	view = NULL;
	return TRUE;
  }
  else
    return FALSE;
}

void wxPropertyFormDialog::OnDefaultAction(wxControl *item)
{
  view->OnDoubleClick(item);
}

void wxPropertyFormDialog::OnCommand(wxWindow& win, wxCommandEvent& event)
{
  if ( view )
  	view->OnCommand(win, event);
}

// Extend event processing to search the view's event table
bool wxPropertyFormDialog::ProcessEvent(wxEvent& event)
{
	if ( !view || ! view->ProcessEvent(event) )
		return wxEvtHandler::ProcessEvent(event);
	else
		return TRUE;
}


/*
 * Property form panel
 */
 
IMPLEMENT_CLASS(wxPropertyFormPanel, wxPanel)

void wxPropertyFormPanel::OnDefaultAction(wxControl *item)
{
  view->OnDoubleClick(item);
}

void wxPropertyFormPanel::OnCommand(wxWindow& win, wxCommandEvent& event)
{
  view->OnCommand(win, event);
}

// Extend event processing to search the view's event table
bool wxPropertyFormPanel::ProcessEvent(wxEvent& event)
{
	if ( !view || ! view->ProcessEvent(event) )
		return wxEvtHandler::ProcessEvent(event);
	else
		return TRUE;
}

/*
 * Property frame
 */
 
IMPLEMENT_CLASS(wxPropertyFormFrame, wxFrame)

bool wxPropertyFormFrame::OnClose(void)
{
  if (view)
    return view->OnClose();
  else
    return FALSE;
}

wxPanel *wxPropertyFormFrame::OnCreatePanel(wxFrame *parent, wxPropertyFormView *v)
{
  return new wxPropertyFormPanel(v, parent);
}

bool wxPropertyFormFrame::Initialize(void)
{
  propertyPanel = OnCreatePanel(this, view);
  if (propertyPanel)
  {
    view->AssociatePanel(propertyPanel);
    view->SetManagedWindow(this);
//    propertyPanel->SetAutoLayout(TRUE);
    return TRUE;
  }
  else
    return FALSE;
}

 /*
  * Property form specific validator
  */
  
IMPLEMENT_ABSTRACT_CLASS(wxPropertyFormValidator, wxPropertyValidator)


/*
 * Default validators
 */

IMPLEMENT_DYNAMIC_CLASS(wxRealFormValidator, wxPropertyFormValidator)

///
/// Real number form validator
/// 
bool wxRealFormValidator::OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  if (realMin == 0.0 && realMax == 0.0)
    return TRUE;
    
  // The item used for viewing the real number: should be a text item.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow || !propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
    return FALSE;

  wxString value(((wxTextCtrl *)propertyWindow)->GetValue());

  float val = 0.0;
  if (!StringToFloat(WXSTRINGCAST value, &val))
  {
    char buf[200];
    sprintf(buf, "Value %s is not a valid real number!", (const char *)value);
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

bool wxRealFormValidator::OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the real number: should be a text item.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow || !propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
    return FALSE;

  wxString value(((wxTextCtrl *)propertyWindow)->GetValue());

  if (value.Length() == 0)
    return FALSE;
    
  float f = (float)atof((const char *)value);
  property->GetValue() = f;
  return TRUE;
}

bool wxRealFormValidator::OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the real number: should be a text item.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow || !propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
    return FALSE;

  wxTextCtrl *textItem = (wxTextCtrl *)propertyWindow;
  textItem->SetValue(FloatToString(property->GetValue().RealValue()));
  return TRUE;
}

///
/// Integer validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxIntegerFormValidator, wxPropertyFormValidator)

bool wxIntegerFormValidator::OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  if (integerMin == 0.0 && integerMax == 0.0)
    return TRUE;
    
  // The item used for viewing the real number: should be a text item or a slider
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow)
    return FALSE;

  long val = 0;

  if (propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
  {
    wxString value(((wxTextCtrl *)propertyWindow)->GetValue());

    if (!StringToLong(WXSTRINGCAST value, &val))
    {
      char buf[200];
      sprintf(buf, "Value %s is not a valid integer!", (const char *)value);
      wxMessageBox(buf, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
      return FALSE;
    }
  }
  else if (propertyWindow->IsKindOf(CLASSINFO(wxSlider)))
  {
    val = (long)((wxSlider *)propertyWindow)->GetValue();
  }
  else
    return FALSE;
    
  if (val < integerMin || val > integerMax)
  {
    char buf[200];
    sprintf(buf, "Value must be an integer between %ld and %ld!", integerMin, integerMax);
    wxMessageBox(buf, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  return TRUE;
}

bool wxIntegerFormValidator::OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the real number: should be a text item or a slider
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow)
    return FALSE;

  if (propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
  {
    wxString value(((wxTextCtrl *)propertyWindow)->GetValue());

    if (value.Length() == 0)
      return FALSE;
    
    long i = atol((const char *)value);
    property->GetValue() = i;
  }
  else if (propertyWindow->IsKindOf(CLASSINFO(wxSlider)))
  {
    property->GetValue() = (long)((wxSlider *)propertyWindow)->GetValue();
  }
  else
    return FALSE;
    
  return TRUE;
}

bool wxIntegerFormValidator::OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the real number: should be a text item or a slider
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow)
    return FALSE;

  if (propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
  {
    wxTextCtrl *textItem = (wxTextCtrl *)propertyWindow;
    textItem->SetValue(LongToString(property->GetValue().IntegerValue()));
  }
  else if (propertyWindow->IsKindOf(CLASSINFO(wxSlider)))
  {
    ((wxSlider *)propertyWindow)->SetValue((int)property->GetValue().IntegerValue());
  }
  else
    return FALSE;
  return TRUE;
}

///
/// Boolean validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxBoolFormValidator, wxPropertyFormValidator)

bool wxBoolFormValidator::OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the boolean: should be a checkbox
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow || !propertyWindow->IsKindOf(CLASSINFO(wxCheckBox)))
    return FALSE;

  return TRUE;
}

bool wxBoolFormValidator::OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the boolean: should be a checkbox.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow || !propertyWindow->IsKindOf(CLASSINFO(wxCheckBox)))
    return FALSE;

  wxCheckBox *checkBox = (wxCheckBox *)propertyWindow;
  
  property->GetValue() = (bool)checkBox->GetValue();
  return TRUE;
}

bool wxBoolFormValidator::OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the boolean: should be a checkbox.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow || !propertyWindow->IsKindOf(CLASSINFO(wxCheckBox)))
    return FALSE;

  wxCheckBox *checkBox = (wxCheckBox *)propertyWindow;
  checkBox->SetValue((bool)property->GetValue().BoolValue());
  return TRUE;
}

///
/// String validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxStringFormValidator, wxPropertyFormValidator)

wxStringFormValidator::wxStringFormValidator(wxStringList *list, long flags):
  wxPropertyFormValidator(flags)
{
  strings = list;
}

bool wxStringFormValidator::OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  if (!strings)
    return TRUE;

  // The item used for viewing the string: should be a text item, choice item or listbox.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow)
    return FALSE;
  if (propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
  {
    wxTextCtrl *text = (wxTextCtrl *)propertyWindow;
    if (!strings->Member(text->GetValue()))
    {
      wxString s("Value ");
      s += text->GetValue();
      s += " is not valid.";
      wxMessageBox(s, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
      return FALSE;
    }
  }
  else
  {
    // Any other item constrains the string value,
    // so we don't have to check it.
  }
  return TRUE;
}

bool wxStringFormValidator::OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the string: should be a text item, choice item or listbox.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow)
    return FALSE;
  if (propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
  {
    wxTextCtrl *text = (wxTextCtrl *)propertyWindow;
    property->GetValue() = text->GetValue();
  }
  else if (propertyWindow->IsKindOf(CLASSINFO(wxListBox)))
  {
    wxListBox *lbox = (wxListBox *)propertyWindow;
    if (lbox->GetSelection() > -1)
      property->GetValue() = lbox->GetStringSelection();
  }
/*
  else if (propertyWindow->IsKindOf(CLASSINFO(wxRadioBox)))
  {
    wxRadioBox *rbox = (wxRadioBox *)propertyWindow;
    int n = 0;
    if ((n = rbox->GetSelection()) > -1)
      property->GetValue() = rbox->GetString(n);
  }
*/
  else if (propertyWindow->IsKindOf(CLASSINFO(wxChoice)))
  {
    wxChoice *choice = (wxChoice *)propertyWindow;
    if (choice->GetSelection() > -1)
      property->GetValue() = choice->GetStringSelection();
  }
  else
    return FALSE;
  return TRUE;
}

bool wxStringFormValidator::OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow)
{
  // The item used for viewing the string: should be a text item, choice item or listbox.
  wxWindow *propertyWindow = property->GetWindow();
  if (!propertyWindow)
    return FALSE;
  if (propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
  {
    wxTextCtrl *text = (wxTextCtrl *)propertyWindow;
    text->SetValue(property->GetValue().StringValue());
  }
  else if (propertyWindow->IsKindOf(CLASSINFO(wxListBox)))
  {
    wxListBox *lbox = (wxListBox *)propertyWindow;
    if (lbox->Number() == 0 && strings)
    {
      // Try to initialize the listbox from 'strings'
      wxNode *node = strings->First();
      while (node)
      {
        char *s = (char *)node->Data();
        lbox->Append(s);
        node = node->Next();
      }
    }
    lbox->SetStringSelection(property->GetValue().StringValue());
  }
/*
  else if (propertyWindow->IsKindOf(CLASSINFO(wxRadioBox)))
  {
    wxRadioBox *rbox = (wxRadioBox *)propertyWindow;
    rbox->SetStringSelection(property->GetValue().StringValue());
  }
*/
  else if (propertyWindow->IsKindOf(CLASSINFO(wxChoice)))
  {
    wxChoice *choice = (wxChoice *)propertyWindow;
#ifndef __XVIEW__
    if (choice->Number() == 0 && strings)
    {
      // Try to initialize the choice item from 'strings'
      // XView doesn't allow this kind of thing.
      wxNode *node = strings->First();
      while (node)
      {
        char *s = (char *)node->Data();
        choice->Append(s);
        node = node->Next();
      }
    }
#endif
    choice->SetStringSelection(property->GetValue().StringValue());
  }
  else
    return FALSE;
  return TRUE;
}

