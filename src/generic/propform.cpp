/////////////////////////////////////////////////////////////////////////////
// Name:        propform.cpp
// Purpose:     Property form classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
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

#if wxUSE_IOSTREAMH
#if defined(__WXMSW__) && !defined(__GNUWIN32__)
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#else
#include <strstream>
#endif

#include "wx/window.h"
#include "wx/utils.h"
#include "wx/list.h"
#include "wx/propform.h"

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

bool wxPropertyFormView::sm_dialogCancelled = FALSE;

wxPropertyFormView::wxPropertyFormView(wxWindow *propPanel, long flags):wxPropertyView(flags)
{
  m_propertyWindow = propPanel;
  m_managedWindow = NULL;

  m_windowCloseButton = NULL;
  m_windowCancelButton = NULL;
  m_windowHelpButton = NULL;

  m_detailedEditing = FALSE;
}

wxPropertyFormView::~wxPropertyFormView(void)
{
}

void wxPropertyFormView::ShowView(wxPropertySheet *ps, wxWindow *panel)
{
  m_propertySheet = ps;

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
  if (!m_propertySheet)
    return FALSE;
    
  wxNode *node = m_propertySheet->GetProperties().First();
  while (node)
  {
    wxProperty *prop = (wxProperty *)node->Data();
    wxPropertyValidator *validator = FindPropertyValidator(prop);
    if (validator && validator->IsKindOf(CLASSINFO(wxPropertyFormValidator)))
    {
      wxPropertyFormValidator *formValidator = (wxPropertyFormValidator *)validator;
      if (!formValidator->OnCheckValue(prop, this, m_propertyWindow))
        return FALSE;
    }
    node = node->Next();
  }
  return TRUE;
}

bool wxPropertyFormView::TransferToPropertySheet(void)
{
  if (!m_propertySheet)
    return FALSE;
    
  wxNode *node = m_propertySheet->GetProperties().First();
  while (node)
  {
    wxProperty *prop = (wxProperty *)node->Data();
    wxPropertyValidator *validator = FindPropertyValidator(prop);
    if (validator && validator->IsKindOf(CLASSINFO(wxPropertyFormValidator)))
    {
      wxPropertyFormValidator *formValidator = (wxPropertyFormValidator *)validator;
      formValidator->OnRetrieveValue(prop, this, m_propertyWindow);
    }
    node = node->Next();
  }
  return TRUE;
}

bool wxPropertyFormView::TransferToDialog(void)
{
  if (!m_propertySheet)
    return FALSE;
    
  wxNode *node = m_propertySheet->GetProperties().First();
  while (node)
  {
    wxProperty *prop = (wxProperty *)node->Data();
    wxPropertyValidator *validator = FindPropertyValidator(prop);
    if (validator && validator->IsKindOf(CLASSINFO(wxPropertyFormValidator)))
    {
      wxPropertyFormValidator *formValidator = (wxPropertyFormValidator *)validator;
      formValidator->OnDisplayValue(prop, this, m_propertyWindow);
    }
    node = node->Next();
  }
  return TRUE;
}

bool wxPropertyFormView::AssociateNames(void)
{
  if (!m_propertySheet || !m_propertyWindow)
    return FALSE;

  wxNode *node = m_propertyWindow->GetChildren().First();
  while (node)
  {
    wxWindow *win = (wxWindow *)node->Data();
    if (win->GetName() != "")
    {
      wxProperty *prop = m_propertySheet->GetProperty(win->GetName());
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

void wxPropertyFormView::OnOk(wxCommandEvent& WXUNUSED(event))
{
  // Retrieve the value if any
  if (!Check())
    return;
  
  sm_dialogCancelled = FALSE;
  TransferToPropertySheet();

  m_managedWindow->Close(TRUE);
}

void wxPropertyFormView::OnCancel(wxCommandEvent& WXUNUSED(event))
{
  sm_dialogCancelled = TRUE;

  m_managedWindow->Close(TRUE);
}

void wxPropertyFormView::OnHelp(wxCommandEvent& WXUNUSED(event))
{
}

void wxPropertyFormView::OnUpdate(wxCommandEvent& WXUNUSED(event))
{
    if (Check())
        TransferToPropertySheet();
}

void wxPropertyFormView::OnRevert(wxCommandEvent& WXUNUSED(event))
{
  TransferToDialog();
}

void wxPropertyFormView::OnCommand(wxWindow& win, wxCommandEvent& event)
{
  if (!m_propertySheet)
    return;
    
  if (win.GetName() == "")
    return;

  if (wxStrcmp(win.GetName(), _T("ok")) == 0)
    OnOk(event);
  else if (wxStrcmp(win.GetName(), _T("cancel")) == 0)
    OnCancel(event);
  else if (wxStrcmp(win.GetName(), _T("help")) == 0)
    OnHelp(event);
  else if (wxStrcmp(win.GetName(), _T("update")) == 0)
    OnUpdate(event);
  else if (wxStrcmp(win.GetName(), _T("revert")) == 0)
    OnRevert(event);
  else
  {
    // Find a validator to route the command to.
    wxNode *node = m_propertySheet->GetProperties().First();
    while (node)
    {
      wxProperty *prop = (wxProperty *)node->Data();
      if (prop->GetWindow() && (prop->GetWindow() == &win))
      {
        wxPropertyValidator *validator = FindPropertyValidator(prop);
        if (validator && validator->IsKindOf(CLASSINFO(wxPropertyFormValidator)))
        {
          wxPropertyFormValidator *formValidator = (wxPropertyFormValidator *)validator;
          formValidator->OnCommand(prop, this, m_propertyWindow, event);
          return;
        }
      }
      node = node->Next();
    }
  }
}

void wxPropertyFormView::OnDoubleClick(wxControl *item)
{
  if (!m_propertySheet)
    return;
    
  // Find a validator to route the command to.
  wxNode *node = m_propertySheet->GetProperties().First();
  while (node)
  {
    wxProperty *prop = (wxProperty *)node->Data();
    if (prop->GetWindow() && ((wxControl *)prop->GetWindow() == item))
    {
      wxPropertyValidator *validator = FindPropertyValidator(prop);
      if (validator && validator->IsKindOf(CLASSINFO(wxPropertyFormValidator)))
      {
        wxPropertyFormValidator *formValidator = (wxPropertyFormValidator *)validator;
        formValidator->OnDoubleClick(prop, this, m_propertyWindow);
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

BEGIN_EVENT_TABLE(wxPropertyFormDialog, wxDialog)
    EVT_CLOSE(wxPropertyFormDialog::OnCloseWindow)
END_EVENT_TABLE()

wxPropertyFormDialog::wxPropertyFormDialog(wxPropertyFormView *v, wxWindow *parent, const wxString& title,
	const wxPoint& pos, const wxSize& size, long style, const wxString& name):
     wxDialog(parent, -1, title, pos, size, style, name)
{
  m_view = v;
  m_view->AssociatePanel(this);
  m_view->SetManagedWindow(this);
//  SetAutoLayout(TRUE);
}

void wxPropertyFormDialog::OnCloseWindow(wxCloseEvent& event)
{
  if (m_view)
  {
    m_view->OnClose();
	m_view = NULL;
	this->Destroy();
  }
  else
    event.Veto();
}

void wxPropertyFormDialog::OnDefaultAction(wxControl *item)
{
  m_view->OnDoubleClick(item);
}

void wxPropertyFormDialog::OnCommand(wxWindow& win, wxCommandEvent& event)
{
  if ( m_view )
  	m_view->OnCommand(win, event);
}

// Extend event processing to search the view's event table
bool wxPropertyFormDialog::ProcessEvent(wxEvent& event)
{
	if ( !m_view || ! m_view->ProcessEvent(event) )
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
  m_view->OnDoubleClick(item);
}

void wxPropertyFormPanel::OnCommand(wxWindow& win, wxCommandEvent& event)
{
  m_view->OnCommand(win, event);
}

// Extend event processing to search the view's event table
bool wxPropertyFormPanel::ProcessEvent(wxEvent& event)
{
	if ( !m_view || ! m_view->ProcessEvent(event) )
		return wxEvtHandler::ProcessEvent(event);
	else
		return TRUE;
}

/*
 * Property frame
 */
 
IMPLEMENT_CLASS(wxPropertyFormFrame, wxFrame)

BEGIN_EVENT_TABLE(wxPropertyFormFrame, wxFrame)
    EVT_CLOSE(wxPropertyFormFrame::OnCloseWindow)
END_EVENT_TABLE()

void wxPropertyFormFrame::OnCloseWindow(wxCloseEvent& event)
{
  if (m_view && m_view->OnClose())
    this->Destroy();
  else
    event.Veto();
}

wxPanel *wxPropertyFormFrame::OnCreatePanel(wxFrame *parent, wxPropertyFormView *v)
{
  return new wxPropertyFormPanel(v, parent);
}

bool wxPropertyFormFrame::Initialize(void)
{
  m_propertyPanel = OnCreatePanel(this, m_view);
  if (m_propertyPanel)
  {
    m_view->AssociatePanel(m_propertyPanel);
    m_view->SetManagedWindow(this);
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
bool wxRealFormValidator::OnCheckValue( wxProperty *property, wxPropertyFormView *WXUNUSED(view), 
  wxWindow *parentWindow)
{
  if (m_realMin == 0.0 && m_realMax == 0.0)
    return TRUE;
    
  // The item used for viewing the real number: should be a text item.
  wxWindow *m_propertyWindow = property->GetWindow();
  if (!m_propertyWindow || !m_propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
    return FALSE;

  wxString value(((wxTextCtrl *)m_propertyWindow)->GetValue());

  float val = 0.0;
  if (!StringToFloat(WXSTRINGCAST value, &val))
  {
    wxChar buf[200];
    wxSprintf(buf, _T("Value %s is not a valid real number!"), (const wxChar *)value);
    wxMessageBox(buf, _T("Property value error"), wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  
  if (val < m_realMin || val > m_realMax)
  {
    wxChar buf[200];
    wxSprintf(buf, _T("Value must be a real number between %.2f and %.2f!"), m_realMin, m_realMax);
    wxMessageBox(buf, _T("Property value error"), wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  return TRUE;
}

bool wxRealFormValidator::OnRetrieveValue(wxProperty *property, wxPropertyFormView *WXUNUSED(view), 
  wxWindow *WXUNUSED(parentWindow) )
{
  // The item used for viewing the real number: should be a text item.
  wxWindow *m_propertyWindow = property->GetWindow();
  if (!m_propertyWindow || !m_propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
    return FALSE;

  wxString value(((wxTextCtrl *)m_propertyWindow)->GetValue());

  if (value.Length() == 0)
    return FALSE;
    
  float f = (float)wxAtof((const wxChar *)value);
  property->GetValue() = f;
  return TRUE;
}

bool wxRealFormValidator::OnDisplayValue(wxProperty *property, wxPropertyFormView *WXUNUSED(view), 
  wxWindow *WXUNUSED(parentWindow) )
{
  // The item used for viewing the real number: should be a text item.
  wxWindow *m_propertyWindow = property->GetWindow();
  if (!m_propertyWindow || !m_propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
    return FALSE;

  wxTextCtrl *textItem = (wxTextCtrl *)m_propertyWindow;
  textItem->SetValue(FloatToString(property->GetValue().RealValue()));
  return TRUE;
}

///
/// Integer validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxIntegerFormValidator, wxPropertyFormValidator)

bool wxIntegerFormValidator::OnCheckValue(wxProperty *property, wxPropertyFormView *WXUNUSED(view), 
  wxWindow *parentWindow)
{
  if (m_integerMin == 0.0 && m_integerMax == 0.0)
    return TRUE;
    
  // The item used for viewing the real number: should be a text item or a slider
  wxWindow *m_propertyWindow = property->GetWindow();
  if (!m_propertyWindow)
    return FALSE;

  long val = 0;

  if (m_propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
  {
    wxString value(((wxTextCtrl *)m_propertyWindow)->GetValue());

    if (!StringToLong(WXSTRINGCAST value, &val))
    {
      wxChar buf[200];
      wxSprintf(buf, _T("Value %s is not a valid integer!"), (const wxChar *)value);
      wxMessageBox(buf, _T("Property value error"), wxOK | wxICON_EXCLAMATION, parentWindow);
      return FALSE;
    }
  }
  else if (m_propertyWindow->IsKindOf(CLASSINFO(wxSlider)))
  {
    val = (long)((wxSlider *)m_propertyWindow)->GetValue();
  }
  else
    return FALSE;
    
  if (val < m_integerMin || val > m_integerMax)
  {
    char buf[200];
    sprintf(buf, "Value must be an integer between %ld and %ld!", m_integerMin, m_integerMax);
    wxMessageBox(buf, "Property value error", wxOK | wxICON_EXCLAMATION, parentWindow);
    return FALSE;
  }
  return TRUE;
}

bool wxIntegerFormValidator::OnRetrieveValue(wxProperty *property, wxPropertyFormView *WXUNUSED(view), 
  wxWindow *WXUNUSED(parentWindow))
{
  // The item used for viewing the real number: should be a text item or a slider
  wxWindow *m_propertyWindow = property->GetWindow();
  if (!m_propertyWindow)
    return FALSE;

  if (m_propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
  {
    wxString value(((wxTextCtrl *)m_propertyWindow)->GetValue());

    if (value.Length() == 0)
      return FALSE;
    
    long i = wxAtol((const wxChar *)value);
    property->GetValue() = i;
  }
  else if (m_propertyWindow->IsKindOf(CLASSINFO(wxSlider)))
  {
    property->GetValue() = (long)((wxSlider *)m_propertyWindow)->GetValue();
  }
  else
    return FALSE;
    
  return TRUE;
}

bool wxIntegerFormValidator::OnDisplayValue( wxProperty *property, wxPropertyFormView *WXUNUSED(view), 
  wxWindow *WXUNUSED(parentWindow))
{
  // The item used for viewing the real number: should be a text item or a slider
  wxWindow *m_propertyWindow = property->GetWindow();
  if (!m_propertyWindow)
    return FALSE;

  if (m_propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
  {
    wxTextCtrl *textItem = (wxTextCtrl *)m_propertyWindow;
    textItem->SetValue(LongToString(property->GetValue().IntegerValue()));
  }
  else if (m_propertyWindow->IsKindOf(CLASSINFO(wxSlider)))
  {
    ((wxSlider *)m_propertyWindow)->SetValue((int)property->GetValue().IntegerValue());
  }
  else
    return FALSE;
  return TRUE;
}

///
/// Boolean validator
/// 
IMPLEMENT_DYNAMIC_CLASS(wxBoolFormValidator, wxPropertyFormValidator)

bool wxBoolFormValidator::OnCheckValue(wxProperty *property, wxPropertyFormView *WXUNUSED(view), 
  wxWindow *WXUNUSED(parentWindow))
{
  // The item used for viewing the boolean: should be a checkbox
  wxWindow *m_propertyWindow = property->GetWindow();
  if (!m_propertyWindow || !m_propertyWindow->IsKindOf(CLASSINFO(wxCheckBox)))
    return FALSE;

  return TRUE;
}

bool wxBoolFormValidator::OnRetrieveValue(wxProperty *property, wxPropertyFormView *WXUNUSED(view), 
  wxWindow *WXUNUSED(parentWindow) )
{
  // The item used for viewing the boolean: should be a checkbox.
  wxWindow *m_propertyWindow = property->GetWindow();
  if (!m_propertyWindow || !m_propertyWindow->IsKindOf(CLASSINFO(wxCheckBox)))
    return FALSE;

  wxCheckBox *checkBox = (wxCheckBox *)m_propertyWindow;

  property->GetValue() = (bool)checkBox->GetValue();
  return TRUE;
}

bool wxBoolFormValidator::OnDisplayValue(wxProperty *property, wxPropertyFormView *WXUNUSED(view), 
  wxWindow *WXUNUSED(parentWindow))
{
  // The item used for viewing the boolean: should be a checkbox.
  wxWindow *m_propertyWindow = property->GetWindow();
  if (!m_propertyWindow || !m_propertyWindow->IsKindOf(CLASSINFO(wxCheckBox)))
    return FALSE;

  wxCheckBox *checkBox = (wxCheckBox *)m_propertyWindow;
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
  m_strings = list;
}

bool wxStringFormValidator::OnCheckValue(wxProperty *property, wxPropertyFormView *WXUNUSED(view), 
  wxWindow *parentWindow )
{
  if (!m_strings)
    return TRUE;

  // The item used for viewing the string: should be a text item, choice item or listbox.
  wxWindow *m_propertyWindow = property->GetWindow();
  if (!m_propertyWindow)
    return FALSE;
  if (m_propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
  {
    wxTextCtrl *text = (wxTextCtrl *)m_propertyWindow;
    if (!m_strings->Member(text->GetValue()))
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

bool wxStringFormValidator::OnRetrieveValue(wxProperty *property, wxPropertyFormView *WXUNUSED(view), 
  wxWindow *WXUNUSED(parentWindow) )
{
  // The item used for viewing the string: should be a text item, choice item or listbox.
  wxWindow *m_propertyWindow = property->GetWindow();
  if (!m_propertyWindow)
    return FALSE;
  if (m_propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
  {
    wxTextCtrl *text = (wxTextCtrl *)m_propertyWindow;
    property->GetValue() = text->GetValue();
  }
  else if (m_propertyWindow->IsKindOf(CLASSINFO(wxListBox)))
  {
    wxListBox *lbox = (wxListBox *)m_propertyWindow;
    if (lbox->GetSelection() > -1)
      property->GetValue() = lbox->GetStringSelection();
  }
/*
  else if (m_propertyWindow->IsKindOf(CLASSINFO(wxRadioBox)))
  {
    wxRadioBox *rbox = (wxRadioBox *)m_propertyWindow;
    int n = 0;
    if ((n = rbox->GetSelection()) > -1)
      property->GetValue() = rbox->GetString(n);
  }
*/
  else if (m_propertyWindow->IsKindOf(CLASSINFO(wxChoice)))
  {
    wxChoice *choice = (wxChoice *)m_propertyWindow;
    if (choice->GetSelection() > -1)
      property->GetValue() = choice->GetStringSelection();
  }
  else
    return FALSE;
  return TRUE;
}

bool wxStringFormValidator::OnDisplayValue(wxProperty *property, wxPropertyFormView *WXUNUSED(view), 
  wxWindow *WXUNUSED(parentWindow) )
{
  // The item used for viewing the string: should be a text item, choice item or listbox.
  wxWindow *m_propertyWindow = property->GetWindow();
  if (!m_propertyWindow)
    return FALSE;
  if (m_propertyWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
  {
    wxTextCtrl *text = (wxTextCtrl *)m_propertyWindow;
    text->SetValue(property->GetValue().StringValue());
  }
  else if (m_propertyWindow->IsKindOf(CLASSINFO(wxListBox)))
  {
    wxListBox *lbox = (wxListBox *)m_propertyWindow;
    if (lbox->Number() == 0 && m_strings)
    {
      // Try to initialize the listbox from 'strings'
      wxNode *node = m_strings->First();
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
  else if (m_propertyWindow->IsKindOf(CLASSINFO(wxRadioBox)))
  {
    wxRadioBox *rbox = (wxRadioBox *)m_propertyWindow;
    rbox->SetStringSelection(property->GetValue().StringValue());
  }
*/
  else if (m_propertyWindow->IsKindOf(CLASSINFO(wxChoice)))
  {
    wxChoice *choice = (wxChoice *)m_propertyWindow;
#ifndef __XVIEW__
    if (choice->Number() == 0 && m_strings)
    {
      // Try to initialize the choice item from 'strings'
      // XView doesn't allow this kind of thing.
      wxNode *node = m_strings->First();
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

