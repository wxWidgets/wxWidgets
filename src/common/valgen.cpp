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
  #include "wx/defs.h"
#endif

#if wxUSE_VALIDATORS

#ifndef WX_PRECOMP
  #include "wx/utils.h"
  #include "wx/intl.h"
  #include "wx/dynarray.h"
  #include "wx/choice.h"
  #include "wx/combobox.h"
  #include "wx/radiobox.h"
  #include "wx/radiobut.h"
  #include "wx/checkbox.h"
  #include "wx/scrolbar.h"
  #include "wx/gauge.h"
  #include "wx/stattext.h"
  #include "wx/textctrl.h"
  #include "wx/button.h"
  #include "wx/listbox.h"
  #include "wx/slider.h"
#endif

#if wxUSE_SPINCTRL && !defined(__WIN16__)
  #include "wx/spinctrl.h"
#endif
#if wxUSE_SPINBTN && !defined(__WIN16__)
  #include "wx/spinbutt.h"
#endif
#if wxUSE_CHECKLISTBOX && !defined(__WIN16__)
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
#if wxUSE_CHECKBOX
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxCheckBox)) )
    {
        wxCheckBox* pControl = (wxCheckBox*) m_validatorWindow;
        if (m_pBool)
	{
	    pControl->SetValue(*m_pBool);
	    return TRUE;
	}
    } else
#endif
#if wxUSE_RADIOBTN
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxRadioButton)) )
    {
        wxRadioButton* pControl = (wxRadioButton*) m_validatorWindow;
	if (m_pBool)
	{
	    pControl->SetValue(*m_pBool) ;
	    return TRUE;
	}
    } else
#endif

    // int controls
#if wxUSE_GAUGE
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxGauge)) )
    {
        wxGauge* pControl = (wxGauge*) m_validatorWindow;
	if (m_pInt)
	{
	    pControl->SetValue(*m_pInt);
	    return TRUE;
	}
    } else
#endif
#if wxUSE_RADIOBOX
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxRadioBox)) )
    {
        wxRadioBox* pControl = (wxRadioBox*) m_validatorWindow;
	if (m_pInt)
	{
	    pControl->SetSelection(*m_pInt) ;
	    return TRUE;
	}
    } else
#endif
#if wxUSE_SCROLLBAR
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxScrollBar)) )
    {
        wxScrollBar* pControl = (wxScrollBar*) m_validatorWindow;
	if (m_pInt)
	{
	    pControl->SetThumbPosition(*m_pInt) ;
	    return TRUE;
	}
    } else
#endif
#if wxUSE_SPINCTRL && !defined(__WIN16__)
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxSpinCtrl)) )
    {
        wxSpinCtrl* pControl = (wxSpinCtrl*) m_validatorWindow;
        if (m_pInt)
        {
            pControl->SetValue(*m_pInt);
            return TRUE;
        }
    } else
#endif
#if wxUSE_SPINBTN && !defined(__WIN16__)
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxSpinButton)) )
    {
        wxSpinButton* pControl = (wxSpinButton*) m_validatorWindow;
	if (m_pInt)
	{
	    pControl->SetValue(*m_pInt) ;
	    return TRUE;
	}
    } else
#endif
#if wxUSE_SLIDER
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxSlider)) )
    {
        wxSlider* pControl = (wxSlider*) m_validatorWindow;
        if (m_pInt)
        {
	    pControl->SetValue(*m_pInt) ;
	    return TRUE;
	}
    } else
#endif

  // string controls
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxButton)) )
    {
        wxButton* pControl = (wxButton*) m_validatorWindow;
	if (m_pString)
	{
	    pControl->SetLabel(*m_pString) ;
	    return TRUE;
	}
    } else
#if wxUSE_COMBOBOX
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxComboBox)) )
    {
        wxComboBox* pControl = (wxComboBox*) m_validatorWindow;
        if (m_pInt)
        {
            pControl->SetSelection(*m_pInt) ;
            return TRUE;
        }
        else if (m_pString)
        {
            if (pControl->FindString(* m_pString) > -1)
            {
                pControl->SetStringSelection(* m_pString);
            }
            return TRUE;
        }
    } else
#endif
#if wxUSE_CHOICE
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxChoice)) )
    {
        wxChoice* pControl = (wxChoice*) m_validatorWindow;
        if (m_pInt)
        {
            pControl->SetSelection(*m_pInt) ;
            return TRUE;
        }
        else if (m_pString)
        {
            if (pControl->FindString(* m_pString) > -1)
            {
                pControl->SetStringSelection(* m_pString);
            }
            return TRUE;
        }
    } else
#endif
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxStaticText)) )
    {
        wxStaticText* pControl = (wxStaticText*) m_validatorWindow;
        if (m_pString)
        {
            pControl->SetLabel(*m_pString) ;
            return TRUE;
        }
    } else 
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)) )
    {
        wxTextCtrl* pControl = (wxTextCtrl*) m_validatorWindow;
        if (m_pString)
        {
            pControl->SetValue(*m_pString) ;
            return TRUE;
        }
        else if (m_pInt)
        {
            wxString str;
            str.Printf(wxT("%d"), *m_pInt);
            pControl->SetValue(str);
            return TRUE;
        }
    } else
#if wxUSE_CHECKLISTBOX
#ifndef __WIN16__
  // array controls
  // NOTE: wxCheckListBox isa wxListBox, so wxCheckListBox
  // MUST come first:
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxCheckListBox)) )
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
  } else
#endif
#endif
#if wxUSE_LISTBOX
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxListBox)) )
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
    } else
#endif

  // unrecognized control, or bad pointer
    return FALSE;
  return FALSE;
}

// Called to transfer data from the window
bool wxGenericValidator::TransferFromWindow(void)
{
  if ( !m_validatorWindow )
    return FALSE;

  // bool controls
#if wxUSE_CHECKBOX
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxCheckBox)) )
  {
    wxCheckBox* pControl = (wxCheckBox*) m_validatorWindow;
	if (m_pBool)
    {
      *m_pBool = pControl->GetValue() ;
      return TRUE;
    }
  } else
#endif
#if wxUSE_RADIOBTN
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxRadioButton)) )
  {
    wxRadioButton* pControl = (wxRadioButton*) m_validatorWindow;
	if (m_pBool)
    {
      *m_pBool = pControl->GetValue() ;
      return TRUE;
    }
  } else
#endif
  // int controls
#if wxUSE_GAUGE
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxGauge)) )
  {
    wxGauge* pControl = (wxGauge*) m_validatorWindow;
	if (m_pInt)
    {
      *m_pInt = pControl->GetValue() ;
      return TRUE;
    }
  } else
#endif
#if wxUSE_RADIOBOX
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxRadioBox)) )
  {
    wxRadioBox* pControl = (wxRadioBox*) m_validatorWindow;
	if (m_pInt)
    {
      *m_pInt = pControl->GetSelection() ;
      return TRUE;
    }
  } else
#endif
#if wxUSE_SCROLLBAR
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxScrollBar)) )
  {
    wxScrollBar* pControl = (wxScrollBar*) m_validatorWindow;
	if (m_pInt)
    {
      *m_pInt = pControl->GetThumbPosition() ;
      return TRUE;
    }
  } else
#endif
#if wxUSE_SPINCTRL && !defined(__WIN16__)
    if (m_validatorWindow->IsKindOf(CLASSINFO(wxSpinCtrl)) )
    {
        wxSpinCtrl* pControl = (wxSpinCtrl*) m_validatorWindow;
        if (m_pInt)
        {
            *m_pInt=pControl->GetValue();
            return TRUE;
        }
    } else
#endif
#if wxUSE_SPINBTN && !defined(__WIN16__)
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxSpinButton)) )
  {
    wxSpinButton* pControl = (wxSpinButton*) m_validatorWindow;
	if (m_pInt)
    {
      *m_pInt = pControl->GetValue() ;
      return TRUE;
    }
  } else
#endif
#if wxUSE_SLIDER
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxSlider)) )
  {
    wxSlider* pControl = (wxSlider*) m_validatorWindow;
    if (m_pInt)
    {
      pControl->SetValue(*m_pInt) ;
      return TRUE;
    }
  } else
#endif
  // string controls
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxButton)) )
  {
    wxButton* pControl = (wxButton*) m_validatorWindow;
	if (m_pString)
    {
      *m_pString = pControl->GetLabel() ;
      return TRUE;
    }
  }
  else
#if wxUSE_COMBOBOX
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxComboBox)) )
  {
    wxComboBox* pControl = (wxComboBox*) m_validatorWindow;
	if (m_pString)
    {
      *m_pString = pControl->GetValue() ;
      return TRUE;
    }
    else if (m_pString)
    {
        *m_pString = pControl->GetStringSelection();
        return TRUE;
    }
  } else
#endif
#if wxUSE_CHOICE
 if (m_validatorWindow->IsKindOf(CLASSINFO(wxChoice)) )
  {
    wxChoice* pControl = (wxChoice*) m_validatorWindow;
	if (m_pInt)
    {
      *m_pInt = pControl->GetSelection() ;
      return TRUE;
    }
    else if (m_pString)
    {
        *m_pString = pControl->GetStringSelection();
        return TRUE;
    }
  } else
#endif
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxStaticText)) )
  {
    wxStaticText* pControl = (wxStaticText*) m_validatorWindow;
	if (m_pString)
    {
      *m_pString = pControl->GetLabel() ;
      return TRUE;
    }
  } else
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)) )
  {
    wxTextCtrl* pControl = (wxTextCtrl*) m_validatorWindow;
	if (m_pString)
    {
      *m_pString = pControl->GetValue() ;
      return TRUE;
    }
    else if (m_pInt)
    {
        *m_pInt = wxAtoi(pControl->GetValue());
        return TRUE;
    }
  } else
#if wxUSE_CHECKLISTBOX
#ifndef __WIN16__
  // array controls
  // NOTE: wxCheckListBox isa wxListBox, so wxCheckListBox
  // MUST come first:
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxCheckListBox)) )
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
  } else
#endif
#endif
#if wxUSE_LISTBOX
  if (m_validatorWindow->IsKindOf(CLASSINFO(wxListBox)) )
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
  } else
#endif

  // unrecognized control, or bad pointer
    return FALSE;
  return FALSE;
}

/*
  Called by constructors to initialize ALL data members
	Last change:  JAC  21 Jul 100    5:00 pm
*/
void wxGenericValidator::Initialize()
{
    m_pBool = 0;
    m_pInt = 0;
    m_pString = 0;
    m_pArrayInt = 0;
}

#endif
  // wxUSE_VALIDATORS

