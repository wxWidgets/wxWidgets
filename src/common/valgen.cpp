/////////////////////////////////////////////////////////////////////////////
// Name:        valgen.cpp
// Purpose:     wxGenericValidator class
// Author:      Kevin Smith
// Modified by:
// Created:     Jan 22 1999
// RCS-ID:      
// Copyright:   (c) 1999 Kevin Smith
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "valgen.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/wx.h"
#include "wx/dynarray.h"
#endif

#ifndef __WIN16__
#include "wx/spinbutt.h"
#include "wx/checklst.h"
#endif

#include "wx/valgen.h"

wxGenericValidator::wxGenericValidator(bool *val)
{
  Initialize();
  m_pBool = val;
}

wxGenericValidator::wxGenericValidator(int *val)
{
  Initialize();
  m_pInt = val;
}

wxGenericValidator::wxGenericValidator(wxString *val)
{
  Initialize();
  m_pString = val;
}

wxGenericValidator::wxGenericValidator(wxArrayInt *val)
{
  Initialize();
  m_pArrayInt = val;
}

wxGenericValidator::wxGenericValidator(const wxGenericValidator& val)
{
  Copy(val);
}

bool wxGenericValidator::Copy(const wxGenericValidator& val)
{
  wxValidator::Copy(val);

  m_pBool = val.m_pBool;
  m_pInt = val.m_pInt;
  m_pString = val.m_pString;
  m_pArrayInt = val.m_pArrayInt;

  return TRUE;
}

wxGenericValidator::~wxGenericValidator()
{
}

// Called to transfer data to the window
bool wxGenericValidator::TransferToWindow(void)
{
  if ( !m_validatorWindow )
    return FALSE;

  // bool controls
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxCheckBox)) )
  {
    wxCheckBox* pControl = (wxCheckBox*) m_validatorWindow;
	if (m_pBool)
    {
      pControl->SetValue(*m_pBool) ;
      return TRUE;
    }
  }
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxRadioButton)) )
  {
    wxRadioButton* pControl = (wxRadioButton*) m_validatorWindow;
	if (m_pBool)
    {
      pControl->SetValue(*m_pBool) ;
      return TRUE;
    }
  }
  // int controls
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxGauge)) )
  {
    wxGauge* pControl = (wxGauge*) m_validatorWindow;
	if (m_pInt)
    {
      pControl->SetValue(*m_pInt) ;
      return TRUE;
    }
  }
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxRadioBox)) )
  {
    wxRadioBox* pControl = (wxRadioBox*) m_validatorWindow;
	if (m_pInt)
    {
      pControl->SetSelection(*m_pInt) ;
      return TRUE;
    }
  }
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxScrollBar)) )
  {
    wxScrollBar* pControl = (wxScrollBar*) m_validatorWindow;
	if (m_pInt)
    {
      pControl->SetThumbPosition(*m_pInt) ;
      return TRUE;
    }
  }
#ifndef __WIN16__
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxSpinButton)) )
  {
    wxSpinButton* pControl = (wxSpinButton*) m_validatorWindow;
	if (m_pInt)
    {
      pControl->SetValue(*m_pInt) ;
      return TRUE;
    }
  }
#endif
  // string controls
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxButton)) )
  {
    wxButton* pControl = (wxButton*) m_validatorWindow;
	if (m_pString)
    {
      pControl->SetLabel(*m_pString) ;
      return TRUE;
    }
  }
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxComboBox)) )
  {
    wxComboBox* pControl = (wxComboBox*) m_validatorWindow;
	if (m_pString)
    {
      pControl->SetValue(*m_pString) ;
      return TRUE;
    }
  }
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxChoice)) )
  {
    wxChoice* pControl = (wxChoice*) m_validatorWindow;
	if (m_pInt)
    {
      pControl->SetSelection(*m_pInt) ;
      return TRUE;
    }
  }
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxStaticText)) )
  {
    wxStaticText* pControl = (wxStaticText*) m_validatorWindow;
	if (m_pString)
    {
      pControl->SetLabel(*m_pString) ;
      return TRUE;
    }
  }
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)) )
  {
    wxTextCtrl* pControl = (wxTextCtrl*) m_validatorWindow;
	if (m_pString)
    {
      pControl->SetValue(*m_pString) ;
      return TRUE;
    }
  }
#ifndef __WIN16__
  // array controls
  // NOTE: wxCheckListBox isa wxListBox, so wxCheckListBox
  // MUST come first:
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxCheckListBox)) )
  {
    wxCheckListBox* pControl = (wxCheckListBox*) m_validatorWindow;
	if (m_pArrayInt)
    {
      // clear all selections
      int i;
      for (i = 0 ; i < pControl->Number(); ++i)
        pControl->Check(i, FALSE);
      // select each item in our array
      unsigned u;
      for (u = 0; u < m_pArrayInt->Count(); ++u)
        pControl->Check(m_pArrayInt->Item(u));
      return TRUE;
    }
	else
		return FALSE;
  }
#endif
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxListBox)) )
  {
    wxListBox* pControl = (wxListBox*) m_validatorWindow;
	if (m_pArrayInt)
    {
      // clear all selections
      int i;
      for (i = 0 ; i < pControl->Number(); ++i)
        pControl->Deselect(i);
      // select each item in our array
      unsigned u;
      for (u = 0; u < m_pArrayInt->Count(); ++u)
        pControl->SetSelection(m_pArrayInt->Item(u));
      return TRUE;
    }
  }

  // unrecognized control, or bad pointer
  return FALSE;
}

// Called to transfer data to the window
bool wxGenericValidator::TransferFromWindow(void)
{
  if ( !m_validatorWindow )
    return FALSE;

  // bool controls
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxCheckBox)) )
  {
    wxCheckBox* pControl = (wxCheckBox*) m_validatorWindow;
	if (m_pBool)
    {
      *m_pBool = pControl->GetValue() ;
      return TRUE;
    }
  }
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxRadioButton)) )
  {
    wxRadioButton* pControl = (wxRadioButton*) m_validatorWindow;
	if (m_pBool)
    {
      *m_pBool = pControl->GetValue() ;
      return TRUE;
    }
  }
  // int controls
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxGauge)) )
  {
    wxGauge* pControl = (wxGauge*) m_validatorWindow;
	if (m_pInt)
    {
      *m_pInt = pControl->GetValue() ;
      return TRUE;
    }
  }
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxRadioBox)) )
  {
    wxRadioBox* pControl = (wxRadioBox*) m_validatorWindow;
	if (m_pInt)
    {
      *m_pInt = pControl->GetSelection() ;
      return TRUE;
    }
  }
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxScrollBar)) )
  {
    wxScrollBar* pControl = (wxScrollBar*) m_validatorWindow;
	if (m_pInt)
    {
      *m_pInt = pControl->GetThumbPosition() ;
      return TRUE;
    }
  }
#ifndef __WIN16__
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxSpinButton)) )
  {
    wxSpinButton* pControl = (wxSpinButton*) m_validatorWindow;
	if (m_pInt)
    {
      *m_pInt = pControl->GetValue() ;
      return TRUE;
    }
  }
#endif
  // string controls
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxButton)) )
  {
    wxButton* pControl = (wxButton*) m_validatorWindow;
	if (m_pString)
    {
      *m_pString = pControl->GetLabel() ;
      return TRUE;
    }
  }
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxComboBox)) )
  {
    wxComboBox* pControl = (wxComboBox*) m_validatorWindow;
	if (m_pString)
    {
      *m_pString = pControl->GetValue() ;
      return TRUE;
    }
  }
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxChoice)) )
  {
    wxChoice* pControl = (wxChoice*) m_validatorWindow;
	if (m_pInt)
    {
      *m_pInt = pControl->GetSelection() ;
      return TRUE;
    }
  }
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxStaticText)) )
  {
    wxStaticText* pControl = (wxStaticText*) m_validatorWindow;
	if (m_pString)
    {
      *m_pString = pControl->GetLabel() ;
      return TRUE;
    }
  }
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)) )
  {
    wxTextCtrl* pControl = (wxTextCtrl*) m_validatorWindow;
	if (m_pString)
    {
      *m_pString = pControl->GetValue() ;
      return TRUE;
    }
  }
#ifndef __WIN16__
  // array controls
  // NOTE: wxCheckListBox isa wxListBox, so wxCheckListBox
  // MUST come first:
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxCheckListBox)) )
  {
    wxCheckListBox* pControl = (wxCheckListBox*) m_validatorWindow;
	if (m_pArrayInt)
    {
      // clear our array
      m_pArrayInt->Clear();
      // add each selected item to our array
      int i;
      for (i = 0 ; i < pControl->Number(); ++i)
        if (pControl->IsChecked(i))
          m_pArrayInt->Add(i);
      return TRUE;
    }
	else
	  return FALSE;
  }
#endif
  else if (m_validatorWindow->IsKindOf(CLASSINFO(wxListBox)) )
  {
    wxListBox* pControl = (wxListBox*) m_validatorWindow;
	if (m_pArrayInt)
    {
      // clear our array
      m_pArrayInt->Clear();
      // add each selected item to our array
      int i;
      for (i = 0 ; i < pControl->Number(); ++i)
        if (pControl->Selected(i))
          m_pArrayInt->Add(i);
      return TRUE;
    }
  }

  // unrecognized control, or bad pointer
  return FALSE;
}

/*
  Called by constructors to initialize ALL data members
*/
void wxGenericValidator::Initialize()
{
  m_pBool = 0;
  m_pInt = 0;
  m_pString = 0;
  m_pArrayInt = 0;
}

