/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/valgen.cpp
// Purpose:     wxGenericValidator class
// Author:      Kevin Smith
// Modified by: Ali Kettab 2018-09-07
// Created:     Jan 22 1999
// Copyright:   (c) 1999 Kevin Smith
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_VALIDATORS

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/utils.h"
    #include "wx/intl.h"
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
    #include "wx/panel.h"
    #include "wx/slider.h"
    #include "wx/checklst.h"
#endif

#include "wx/calctrl.h"
#include "wx/collheaderctrl.h"
#include "wx/collpane.h"
#include "wx/clrpicker.h"
#include "wx/datectrl.h"
#include "wx/timectrl.h"
#include "wx/filepicker.h"
#include "wx/fontpicker.h"
#include "wx/spinctrl.h"
#include "wx/splitter.h"
#include "wx/spinbutt.h"
#include "wx/tglbtn.h"
#include "wx/filename.h"

#include "wx/valgen.h"

#if !wxUSE_DATATRANSFER


wxIMPLEMENT_CLASS(wxGenericValidator, wxValidator);

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

#if wxUSE_DATETIME

wxGenericValidator::wxGenericValidator(wxDateTime *val)
{
    Initialize();
    m_pDateTime = val;
}

#endif // wxUSE_DATETIME

wxGenericValidator::wxGenericValidator(wxFileName *val)
{
    Initialize();
    m_pFileName = val;
}

wxGenericValidator::wxGenericValidator(float *val)
{
    Initialize();
    m_pFloat = val;
}

wxGenericValidator::wxGenericValidator(double *val)
{
    Initialize();
    m_pDouble = val;
}

wxGenericValidator::wxGenericValidator(const wxGenericValidator& val)
    : wxValidator()
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
#if wxUSE_DATETIME
    m_pDateTime = val.m_pDateTime;
#endif // wxUSE_DATETIME
    m_pFileName = val.m_pFileName;
    m_pFloat = val.m_pFloat;
    m_pDouble = val.m_pDouble;

    return true;
}

// Called to transfer data to the window
bool wxGenericValidator::TransferToWindow(void)
{
    if ( !m_validatorWindow )
        return false;

    // bool controls
#if wxUSE_CHECKBOX
    if (wxDynamicCast(m_validatorWindow, wxCheckBox))
    {
        wxCheckBox* pControl = (wxCheckBox*) m_validatorWindow;
        if (m_pBool)
        {
            pControl->SetValue(*m_pBool);
            return true;
        }
    } else
#endif
#if wxUSE_RADIOBTN
    if (wxDynamicCast(m_validatorWindow, wxRadioButton))
    {
        wxRadioButton* pControl = (wxRadioButton*) m_validatorWindow;
        if (m_pBool)
        {
            pControl->SetValue(*m_pBool) ;
            return true;
        }
    } else
#endif

#if wxUSE_TOGGLEBTN
    if (wxDynamicCast(m_validatorWindow, wxToggleButton))
    {
        wxToggleButton * pControl = (wxToggleButton *) m_validatorWindow;
        if (m_pBool)
        {
            pControl->SetValue(*m_pBool);
            return true;
        }
    } else
#if (defined(__WXMAC__) || defined(__WXMSW__) || defined(__WXGTK20__)) && !defined(__WXUNIVERSAL__)
    if (wxDynamicCast(m_validatorWindow, wxBitmapToggleButton))
    {
        wxBitmapToggleButton * pControl = (wxBitmapToggleButton *) m_validatorWindow;
        if (m_pBool)
        {
            pControl->SetValue(*m_pBool);
            return true;
        }
    } else
#endif
#endif

    // int controls
#if wxUSE_GAUGE
    if (wxDynamicCast(m_validatorWindow, wxGauge))
    {
        wxGauge* pControl = (wxGauge*) m_validatorWindow;
        if (m_pInt)
        {
            pControl->SetValue(*m_pInt);
            return true;
        }
    } else
#endif
#if wxUSE_RADIOBOX
    if (wxDynamicCast(m_validatorWindow, wxRadioBox))
    {
        wxRadioBox* pControl = (wxRadioBox*) m_validatorWindow;
        if (m_pInt)
        {
            pControl->SetSelection(*m_pInt) ;
            return true;
        }
    } else
#endif
#if wxUSE_SCROLLBAR
    if (wxDynamicCast(m_validatorWindow, wxScrollBar))
    {
        wxScrollBar* pControl = (wxScrollBar*) m_validatorWindow;
        if (m_pInt)
        {
            pControl->SetThumbPosition(*m_pInt) ;
            return true;
        }
    } else
#endif
#if wxUSE_SPINCTRL && !defined(__WXMOTIF__)
    if (wxDynamicCast(m_validatorWindow, wxSpinCtrl))
    {
        wxSpinCtrl* pControl = (wxSpinCtrl*) m_validatorWindow;
        if (m_pInt)
        {
            pControl->SetValue(*m_pInt);
            return true;
        }
    } else
#endif
#if wxUSE_SPINBTN
    if (wxDynamicCast(m_validatorWindow, wxSpinButton))
    {
        wxSpinButton* pControl = (wxSpinButton*) m_validatorWindow;
        if (m_pInt)
        {
            pControl->SetValue(*m_pInt) ;
            return true;
        }
    } else
#endif
#if wxUSE_SLIDER
    if (wxDynamicCast(m_validatorWindow, wxSlider))
    {
        wxSlider* pControl = (wxSlider*) m_validatorWindow;
        if (m_pInt)
        {
            pControl->SetValue(*m_pInt) ;
            return true;
        }
    } else
#endif

    // date time controls
#if 0 // wxUSE_DATEPICKCTRL -- temporary fix for shared build linking
    if (wxDynamicCast(m_validatorWindow, wxDatePickerCtrl))
    {
        wxDatePickerCtrl* pControl = (wxDatePickerCtrl*) m_validatorWindow;
        if (m_pDateTime)
        {
            pControl->SetValue(*m_pDateTime) ;
            return true;
        }
    } else
#endif

    // string controls
#if wxUSE_BUTTON
    if (wxDynamicCast(m_validatorWindow, wxButton))
    {
        wxButton* pControl = (wxButton*) m_validatorWindow;
        if (m_pString)
        {
            pControl->SetLabel(*m_pString) ;
            return true;
        }
    } else
#endif
#if wxUSE_COMBOBOX
    if (wxDynamicCast(m_validatorWindow, wxComboBox))
    {
        wxComboBox* pControl = (wxComboBox*) m_validatorWindow;
        if (m_pInt)
        {
            pControl->SetSelection(*m_pInt) ;
            return true;
        }
        else if (m_pString)
        {
            if (pControl->FindString(* m_pString) != wxNOT_FOUND)
            {
                pControl->SetStringSelection(* m_pString);
            }
            if ((m_validatorWindow->GetWindowStyle() & wxCB_READONLY) == 0)
            {
                pControl->SetValue(* m_pString);
            }
            return true;
        }
    } else
#endif
#if wxUSE_CHOICE
    if (wxDynamicCast(m_validatorWindow, wxChoice))
    {
        wxChoice* pControl = (wxChoice*) m_validatorWindow;
        if (m_pInt)
        {
            pControl->SetSelection(*m_pInt) ;
            return true;
        }
        else if (m_pString)
        {
            if (pControl->FindString(* m_pString) != wxNOT_FOUND)
            {
                pControl->SetStringSelection(* m_pString);
            }
            return true;
        }
    } else
#endif
#if wxUSE_STATTEXT
    if (wxDynamicCast(m_validatorWindow, wxStaticText))
    {
        wxStaticText* pControl = (wxStaticText*) m_validatorWindow;
        if (m_pString)
        {
            pControl->SetLabel(*m_pString) ;
            return true;
        }
    } else
#endif
#if wxUSE_TEXTCTRL
    if (wxDynamicCast(m_validatorWindow, wxTextCtrl))
    {
        wxTextCtrl* pControl = (wxTextCtrl*) m_validatorWindow;
        if (m_pString)
        {
            pControl->SetValue(*m_pString) ;
            return true;
        }
        else if (m_pInt)
        {
            wxString str;
            str.Printf(wxT("%d"), *m_pInt);
            pControl->SetValue(str);
            return true;
        }
        else if (m_pFileName)
        {
            pControl->SetValue(m_pFileName->GetFullPath());
            return true;
        }
        else if (m_pFloat)
        {
            pControl->SetValue(wxString::Format(wxT("%g"), *m_pFloat));
            return true;
        }
        else if (m_pDouble)
        {
            pControl->SetValue(wxString::Format(wxT("%g"), *m_pDouble));
            return true;
        }
    } else
#endif

    // array controls
#if wxUSE_CHECKLISTBOX
    // NOTE: wxCheckListBox is a wxListBox, so wxCheckListBox MUST come first:
    if (wxDynamicCast(m_validatorWindow, wxCheckListBox))
    {
        wxCheckListBox* pControl = (wxCheckListBox*) m_validatorWindow;
        if (m_pArrayInt)
        {
            // clear all selections
            size_t i,
                   count = pControl->GetCount();
            for ( i = 0 ; i < count; i++ )
                pControl->Check(i, false);

            // select each item in our array
            count = m_pArrayInt->GetCount();
            for ( i = 0 ; i < count; i++ )
                pControl->Check(m_pArrayInt->Item(i));

            return true;
        }
        else
            return false;
    } else
#endif
#if wxUSE_LISTBOX
    if (wxDynamicCast(m_validatorWindow, wxListBox))
    {
        wxListBox* pControl = (wxListBox*) m_validatorWindow;
        if (m_pArrayInt)
        {
            // clear all selections
            size_t i,
                   count = pControl->GetCount();
            for ( i = 0 ; i < count; i++ )
                pControl->Deselect(i);

            // select each item in our array
            count = m_pArrayInt->GetCount();
            for ( i = 0 ; i < count; i++ )
                pControl->SetSelection(m_pArrayInt->Item(i));

            return true;
        }
    } else
#endif
    {   // to match the last 'else' above
    }

  // unrecognized control, or bad pointer
  return false;
}

// Called to transfer data from the window
bool wxGenericValidator::TransferFromWindow(void)
{
    if ( !m_validatorWindow )
        return false;

    // BOOL CONTROLS **************************************
#if wxUSE_CHECKBOX
    if (wxDynamicCast(m_validatorWindow, wxCheckBox))
    {
        wxCheckBox* pControl = (wxCheckBox*) m_validatorWindow;
        if (m_pBool)
        {
            *m_pBool = pControl->GetValue() ;
            return true;
        }
    } else
#endif
#if wxUSE_RADIOBTN
    if (wxDynamicCast(m_validatorWindow, wxRadioButton))
    {
        wxRadioButton* pControl = (wxRadioButton*) m_validatorWindow;
        if (m_pBool)
        {
            *m_pBool = pControl->GetValue() ;
            return true;
        }
    } else
#endif
#if wxUSE_TOGGLEBTN
    if (wxDynamicCast(m_validatorWindow, wxToggleButton))
    {
        wxToggleButton *pControl = (wxToggleButton *) m_validatorWindow;
        if (m_pBool)
        {
            *m_pBool = pControl->GetValue() ;
            return true;
        }
    } else
#if (defined(__WXMAC__) || defined(__WXMSW__) || defined(__WXGTK20__)) && !defined(__WXUNIVERSAL__)
    if (wxDynamicCast(m_validatorWindow, wxBitmapToggleButton))
    {
        wxBitmapToggleButton *pControl = (wxBitmapToggleButton *) m_validatorWindow;
        if (m_pBool)
        {
            *m_pBool = pControl->GetValue() ;
            return true;
        }
    } else
#endif
#endif

    // INT CONTROLS ***************************************
#if wxUSE_GAUGE
    if (wxDynamicCast(m_validatorWindow, wxGauge))
    {
        wxGauge* pControl = (wxGauge*) m_validatorWindow;
        if (m_pInt)
        {
            *m_pInt = pControl->GetValue() ;
            return true;
        }
    } else
#endif
#if wxUSE_RADIOBOX
    if (wxDynamicCast(m_validatorWindow, wxRadioBox))
    {
        wxRadioBox* pControl = (wxRadioBox*) m_validatorWindow;
        if (m_pInt)
        {
            *m_pInt = pControl->GetSelection() ;
            return true;
        }
    } else
#endif
#if wxUSE_SCROLLBAR
    if (wxDynamicCast(m_validatorWindow, wxScrollBar))
    {
        wxScrollBar* pControl = (wxScrollBar*) m_validatorWindow;
        if (m_pInt)
        {
            *m_pInt = pControl->GetThumbPosition() ;
            return true;
        }
    } else
#endif
#if wxUSE_SPINCTRL && !defined(__WXMOTIF__)
    if (wxDynamicCast(m_validatorWindow, wxSpinCtrl))
    {
        wxSpinCtrl* pControl = (wxSpinCtrl*) m_validatorWindow;
        if (m_pInt)
        {
            *m_pInt=pControl->GetValue();
            return true;
        }
    } else
#endif
#if wxUSE_SPINBTN
    if (wxDynamicCast(m_validatorWindow, wxSpinButton))
    {
        wxSpinButton* pControl = (wxSpinButton*) m_validatorWindow;
        if (m_pInt)
        {
            *m_pInt = pControl->GetValue() ;
            return true;
        }
    } else
#endif
#if wxUSE_SLIDER
    if (wxDynamicCast(m_validatorWindow, wxSlider))
    {
        wxSlider* pControl = (wxSlider*) m_validatorWindow;
        if (m_pInt)
        {
            *m_pInt = pControl->GetValue() ;
            return true;
        }
    } else
#endif

    // DATE TIME CONTROLS ************************************
#if 0 // wxUSE_DATEPICKCTRL -- temporary fix for shared build linking
    if (wxDynamicCast(m_validatorWindow, wxDatePickerCtrl))
    {
        wxDatePickerCtrl* pControl = (wxDatePickerCtrl*) m_validatorWindow;
        if (m_pDateTime)
        {
            *m_pDateTime = pControl->GetValue() ;
            return true;
        }
    } else
#endif

    // STRING CONTROLS ************************************
#if wxUSE_BUTTON
    if (wxDynamicCast(m_validatorWindow, wxButton))
    {
        wxButton* pControl = (wxButton*) m_validatorWindow;
        if (m_pString)
        {
            *m_pString = pControl->GetLabel() ;
            return true;
        }
    } else
#endif
#if wxUSE_COMBOBOX
    if (wxDynamicCast(m_validatorWindow, wxComboBox))
    {
        wxComboBox* pControl = (wxComboBox*) m_validatorWindow;
        if (m_pInt)
        {
            *m_pInt = pControl->GetSelection() ;
            return true;
        }
        else if (m_pString)
        {
            if (m_validatorWindow->GetWindowStyle() & wxCB_READONLY)
                *m_pString = pControl->GetStringSelection();
            else
                *m_pString = pControl->GetValue();
            return true;
        }
    } else
#endif
#if wxUSE_CHOICE
    if (wxDynamicCast(m_validatorWindow, wxChoice))
    {
        wxChoice* pControl = (wxChoice*) m_validatorWindow;
        if (m_pInt)
        {
            *m_pInt = pControl->GetSelection() ;
            return true;
        }
        else if (m_pString)
        {
            *m_pString = pControl->GetStringSelection();
            return true;
        }
    } else
#endif
#if wxUSE_STATTEXT
    if (wxDynamicCast(m_validatorWindow, wxStaticText))
    {
        wxStaticText* pControl = (wxStaticText*) m_validatorWindow;
        if (m_pString)
        {
            *m_pString = pControl->GetLabel() ;
            return true;
        }
    } else
#endif
#if wxUSE_TEXTCTRL
    if (wxDynamicCast(m_validatorWindow, wxTextCtrl))
    {
        wxTextCtrl* pControl = (wxTextCtrl*) m_validatorWindow;
        if (m_pString)
        {
            *m_pString = pControl->GetValue() ;
            return true;
        }
        else if (m_pInt)
        {
            *m_pInt = wxAtoi(pControl->GetValue());
            return true;
        }
        else if (m_pFileName)
        {
            m_pFileName->Assign(pControl->GetValue());
            return true;
        }
        else if (m_pFloat)
        {
            *m_pFloat = (float)wxAtof(pControl->GetValue());
            return true;
        }
        else if (m_pDouble)
        {
            *m_pDouble = wxAtof(pControl->GetValue());
            return true;
        }
    } else
#endif

    // ARRAY CONTROLS *************************************
#if wxUSE_CHECKLISTBOX
    // NOTE: wxCheckListBox isa wxListBox, so wxCheckListBox MUST come first:
    if (wxDynamicCast(m_validatorWindow, wxCheckListBox))
    {
        wxCheckListBox* pControl = (wxCheckListBox*) m_validatorWindow;
        if (m_pArrayInt)
        {
            // clear our array
            m_pArrayInt->Clear();

            // add each selected item to our array
            size_t i,
                   count = pControl->GetCount();
            for ( i = 0; i < count; i++ )
            {
                if (pControl->IsChecked(i))
                    m_pArrayInt->Add(i);
            }

            return true;
        }
        else
            return false;
    } else
#endif
#if wxUSE_LISTBOX
    if (wxDynamicCast(m_validatorWindow, wxListBox))
    {
        wxListBox* pControl = (wxListBox*) m_validatorWindow;
        if (m_pArrayInt)
        {
            // clear our array
            m_pArrayInt->Clear();

            // add each selected item to our array
            size_t i,
                   count = pControl->GetCount();
            for ( i = 0; i < count; i++ )
            {
                if (pControl->IsSelected(i))
                    m_pArrayInt->Add(i);
            }

            return true;
        }
    } else
#endif

    // unrecognized control, or bad pointer
        return false;

    return false;
}

/*
  Called by constructors to initialize ALL data members
*/
void wxGenericValidator::Initialize()
{
    m_pBool = NULL;
    m_pInt = NULL;
    m_pString = NULL;
    m_pArrayInt = NULL;
#if wxUSE_DATETIME
    m_pDateTime = NULL;
#endif // wxUSE_DATETIME
    m_pFileName = NULL;
    m_pFloat = NULL;
    m_pDouble = NULL;
}

#else // wxUSE_DATATRANSFER

wxIMPLEMENT_CLASS(wxGenericValidatorBase, wxValidator);

wxGenericValidatorBase::wxGenericValidatorBase(const wxGenericValidatorBase& val)
    : wxValidator(val), m_data(val.m_data)
{
}

//=============================================================================

#if wxUSE_BUTTON

bool wxValidatorDataTransferImpl<wxButtonBase>::To(wxButtonBase* btn, wxString* data)
{
    btn->SetLabel(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxButtonBase>::From(wxButtonBase* btn, wxString* data)
{
    *data = btn->GetLabel();
    return true;
}

#endif // wxUSE_BUTTON

//-----------------------------------------------------------------------------

#if wxUSE_CHECKBOX

bool wxValidatorDataTransferImpl<wxCheckBoxBase>::To(wxCheckBoxBase* ctrl, bool* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxCheckBoxBase>::From(wxCheckBoxBase* ctrl, bool* data)
{
    *data = ctrl->GetValue();
    return true;
}

#endif // wxUSE_CHECKBOX

//-----------------------------------------------------------------------------

#if wxUSE_CHECKLISTBOX

bool wxValidatorDataTransferImpl<wxCheckListBoxBase>::To(wxCheckListBoxBase* ctrl, wxArrayInt* arr)
{
    size_t i, count = ctrl->GetCount();

    for ( i = 0 ; i < count; ++i )
        ctrl->Check(i, false);

    count = arr->GetCount();
    for ( i = 0 ; i < count; ++i )
        ctrl->Check(arr->Item(i));

    return true;
}

bool wxValidatorDataTransferImpl<wxCheckListBoxBase>::From(wxCheckListBoxBase* ctrl, wxArrayInt* arr)
{
    arr->Clear();

    for ( size_t i = 0, count = ctrl->GetCount(); i < count; ++i )
    {
        if ( ctrl->IsChecked(i) )
            arr->Add(i);
    }

    return true;
}

#endif // wxUSE_CHECKLISTBOX

//-----------------------------------------------------------------------------

#if wxUSE_CHOICE

bool wxValidatorDataTransferImpl<wxChoiceBase>::To(wxChoiceBase* ctrl, int* data)
{
    ctrl->SetSelection(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxChoiceBase>::To(wxChoiceBase* ctrl, wxString* data)
{
    return ctrl->SetStringSelection(*data);
}

bool wxValidatorDataTransferImpl<wxChoiceBase>::From(wxChoiceBase* ctrl, int* data)
{
    *data = ctrl->GetSelection();
    return true;
}

bool wxValidatorDataTransferImpl<wxChoiceBase>::From(wxChoiceBase* ctrl, wxString* data)
{
    *data = ctrl->GetStringSelection();
    return true;
}

#endif // wxUSE_CHOICE

//-----------------------------------------------------------------------------

#if wxUSE_COLLPANE

bool wxValidatorDataTransferImpl<wxCollapsibleHeaderCtrlBase>::To(wxCollapsibleHeaderCtrlBase* ctrl, bool* value)
{
    ctrl->SetCollapsed(*value);
    return true;
}

bool wxValidatorDataTransferImpl<wxCollapsibleHeaderCtrlBase>::From(wxCollapsibleHeaderCtrlBase* ctrl, bool* value)
{
    *value = ctrl->IsCollapsed();
    return true;
}

//-----------------------------------------------------------------------------

bool wxValidatorDataTransferImpl<wxCollapsiblePaneBase>::To(wxCollapsiblePaneBase* ctrl, bool* data)
{
    ctrl->Collapse(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxCollapsiblePaneBase>::From(wxCollapsiblePaneBase* ctrl, bool* data)
{
    *data = ctrl->IsCollapsed();
    return true;
}

#endif // wxUSE_COLLPANE

//-----------------------------------------------------------------------------

#if wxUSE_COLOURPICKERCTRL

bool wxValidatorDataTransferImpl<wxColourPickerCtrl>::To(wxColourPickerCtrl* ctrl, wxColour* data)
{
    ctrl->SetColour(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxColourPickerCtrl>::From(wxColourPickerCtrl* ctrl, wxColour* data)
{
    *data = ctrl->GetColour();
    return true;
}

#endif // wxUSE_COLOURPICKERCTRL

//-----------------------------------------------------------------------------

#if wxUSE_COMBOBOX

bool wxValidatorDataTransferImpl<wxComboBox>::To(wxComboBox* ctrl, int* data)
{
    ctrl->SetSelection(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxComboBox>::From(wxComboBox* ctrl, int* data)
{
    *data = ctrl->GetSelection();
    return true;
}

bool wxValidatorDataTransferImpl<wxComboBox>::To(wxComboBox* ctrl, wxString* data)
{
    if ( !ctrl->SetStringSelection(*data) )
    {
        if ( (ctrl->GetWindowStyle() & wxCB_READONLY) )
            return false;

        ctrl->SetValue(*data);
    }

    return true;
}

bool wxValidatorDataTransferImpl<wxComboBox>::From(wxComboBox* ctrl, wxString* data)
{
    if ( ctrl->GetWindowStyle() & wxCB_READONLY )
        *data = ctrl->GetStringSelection();
    else
        *data = ctrl->GetValue();

    return true;
}

#endif // wxUSE_COMBOBOX

//-----------------------------------------------------------------------------

#if wxUSE_FILEPICKERCTRL || wxUSE_DIRPICKERCTRL

bool wxValidatorDataTransferImpl<wxFileDirPickerCtrlBase>::To(wxFileDirPickerCtrlBase* ctrl, wxString* data)
{
    ctrl->SetPath(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxFileDirPickerCtrlBase>::From(wxFileDirPickerCtrlBase* ctrl, wxString* data)
{
    *data = ctrl->GetPath();
    return true;
}

#endif // wxUSE_FILEPICKERCTRL || wxUSE_DIRPICKERCTRL

//-----------------------------------------------------------------------------

#if wxUSE_FILEPICKERCTRL

bool wxValidatorDataTransferImpl<wxFilePickerCtrl>::To(wxFilePickerCtrl* ctrl, wxFileName* data)
{
    ctrl->SetFileName(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxFilePickerCtrl>::From(wxFilePickerCtrl* ctrl, wxFileName* data)
{
    *data = ctrl->GetFileName();
    return true;
}

bool wxValidatorDataTransferImpl<wxFilePickerCtrl>::To(wxFilePickerCtrl* ctrl, wxString* data)
{
    return wxValidatorDataTransferImpl<wxFileDirPickerCtrlBase>::To(ctrl, data);
}

bool wxValidatorDataTransferImpl<wxFilePickerCtrl>::From(wxFilePickerCtrl* ctrl, wxString* data)
{
    return wxValidatorDataTransferImpl<wxFileDirPickerCtrlBase>::From(ctrl, data);
}

#endif // wxUSE_FILEPICKERCTRL

//-----------------------------------------------------------------------------

#if wxUSE_DIRPICKERCTRL

bool wxValidatorDataTransferImpl<wxDirPickerCtrl>::To(wxDirPickerCtrl* ctrl, wxFileName* data)
{
    ctrl->SetDirName(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxDirPickerCtrl>::From(wxDirPickerCtrl* ctrl, wxFileName* data)
{
    *data = ctrl->GetDirName();
    return true;
}

bool wxValidatorDataTransferImpl<wxDirPickerCtrl>::To(wxDirPickerCtrl* ctrl, wxString* data)
{
    return wxValidatorDataTransferImpl<wxFileDirPickerCtrlBase>::To(ctrl, data);
}


bool wxValidatorDataTransferImpl<wxDirPickerCtrl>::From(wxDirPickerCtrl* ctrl, wxString* data)
{
    return wxValidatorDataTransferImpl<wxFileDirPickerCtrlBase>::From(ctrl, data);
}

#endif // wxUSE_DIRPICKERCTRL

//-----------------------------------------------------------------------------

#if wxUSE_FONTPICKERCTRL

bool wxValidatorDataTransferImpl<wxFontPickerCtrl>::To(wxFontPickerCtrl* ctrl, wxFont* data)
{
    ctrl->SetSelectedFont(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxFontPickerCtrl>::From(wxFontPickerCtrl* ctrl, wxFont* data)
{
    *data = ctrl->GetSelectedFont();
    return true;
}

#endif // wxUSE_FONTPICKERCTRL

//-----------------------------------------------------------------------------

#if wxUSE_GAUGE

bool wxValidatorDataTransferImpl<wxGaugeBase>::To(wxGaugeBase* ctrl, int* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxGaugeBase>::From(wxGaugeBase* ctrl, int* data)
{
    *data = ctrl->GetValue();
    return true;
}

#endif // wxUSE_GAUGE

//-----------------------------------------------------------------------------

#if wxUSE_LISTBOX

bool wxValidatorDataTransferImpl<wxListBoxBase>::To(wxListBoxBase* ctrl, int* data)
{
    wxASSERT( (ctrl->GetWindowStyle() & wxLB_SINGLE) );

    ctrl->SetSelection(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxListBoxBase>::From(wxListBoxBase* ctrl, int* data)
{
    *data = ctrl->GetSelection();
    return true;
}

bool wxValidatorDataTransferImpl<wxListBoxBase>::To(wxListBoxBase* ctrl, wxArrayInt* arr)
{
    wxASSERT( (ctrl->GetWindowStyle() & wxLB_MULTIPLE) );

    size_t i, count = ctrl->GetCount();
    for ( i = 0 ; i < count; ++i )
        ctrl->Deselect(i);

    count = arr->GetCount();
    for ( i = 0 ; i < count; ++i )
        ctrl->SetSelection(arr->Item(i));

    return true;
}

bool wxValidatorDataTransferImpl<wxListBoxBase>::From(wxListBoxBase* ctrl, wxArrayInt* arr)
{
    arr->Clear();

    for ( size_t i = 0, count = ctrl->GetCount(); i < count; ++i )
    {
        if ( ctrl->IsSelected(i) )
            arr->Add(i);
    }

    return true;
}

#endif // wxUSE_LISTBOX

//-----------------------------------------------------------------------------

#if wxUSE_RADIOBOX

bool wxValidatorDataTransferImpl<wxRadioBoxBase>::To(wxRadioBoxBase* ctrl, int* data)
{
    ctrl->SetSelection(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxRadioBoxBase>::From(wxRadioBoxBase* ctrl, int* data)
{
    *data = ctrl->GetSelection();
    return true;
}

#endif // wxUSE_RADIOBOX

//-----------------------------------------------------------------------------

#if wxUSE_SCROLLBAR

bool wxValidatorDataTransferImpl<wxScrollBarBase>::To(wxScrollBarBase* ctrl, int* data)
{
    ctrl->SetThumbPosition(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxScrollBarBase>::From(wxScrollBarBase* ctrl, int* data)
{
    *data = ctrl->GetThumbPosition();
    return true;
}

#endif // wxUSE_SCROLLBAR

//-----------------------------------------------------------------------------

#if wxUSE_SLIDER

bool wxValidatorDataTransferImpl<wxSliderBase>::To(wxSliderBase* ctrl, int* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxSliderBase>::From(wxSliderBase* ctrl, int* data)
{
    *data = ctrl->GetValue();
    return true;
}

#endif // wxUSE_SLIDER

//-----------------------------------------------------------------------------

#if wxUSE_SPINBTN

bool wxValidatorDataTransferImpl<wxSpinButtonBase>::To(wxSpinButtonBase* ctrl, int* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxSpinButtonBase>::From(wxSpinButtonBase* ctrl, int* data)
{
    *data = ctrl->GetValue();
    return true;
}

#endif // wxUSE_SPINBTN

//-----------------------------------------------------------------------------

#if wxUSE_SPINCTRL

#if defined(wxHAS_NATIVE_SPINCTRL)
    #define WX_TRANSFER_DATA_FROM_SPINCTRL(ctrl, data) (*data = ctrl->GetValue())
#else
    #define WX_TRANSFER_DATA_FROM_SPINCTRL(ctrl, data) \
            (*data = ctrl->GetValue(wxSPINCTRL_GETVALUE_FIX))
#endif // defined(wxHAS_NATIVE_SPINCTRL)

#if defined(wxHAS_NATIVE_SPINCTRLDOUBLE)
    #define WX_TRANSFER_DATA_FROM_SPINCTRLDOUBLE(ctrl, data) (*data = ctrl->GetValue())
#else
    #define WX_TRANSFER_DATA_FROM_SPINCTRLDOUBLE(ctrl, data) \
            (*data = ctrl->GetValue(wxSPINCTRL_GETVALUE_FIX))
#endif // defined(wxHAS_NATIVE_SPINCTRLDOUBLE)

// wxSpinCtrl

bool wxValidatorDataTransferImpl<wxSpinCtrl>::To(wxSpinCtrl* ctrl, int* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxSpinCtrl>::From(wxSpinCtrl* ctrl, int* data)
{
    WX_TRANSFER_DATA_FROM_SPINCTRL(ctrl, data);
    return true;
}

// wxSpinCtrlDouble

bool wxValidatorDataTransferImpl<wxSpinCtrlDouble>::To(wxSpinCtrlDouble* ctrl, double* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxSpinCtrlDouble>::From(wxSpinCtrlDouble* ctrl, double* data)
{
    WX_TRANSFER_DATA_FROM_SPINCTRLDOUBLE(ctrl, data);
    return true;
}

#endif // wxUSE_SPINCTRL

//-----------------------------------------------------------------------------

#if wxUSE_SPLITTER

bool wxValidatorDataTransferImpl<wxSplitterWindow>::To(wxSplitterWindow* win, int* data)
{
    win->SetSashPosition(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxSplitterWindow>::From(wxSplitterWindow* win, int* data)
{
    *data = win->GetSashPosition();
    return true;
}

#endif // wxUSE_SPLITTER

//-----------------------------------------------------------------------------

#if wxUSE_STATTEXT

bool wxValidatorDataTransferImpl<wxStaticTextBase>::To(wxStaticTextBase* ctrl, wxString* data)
{
    ctrl->SetLabel(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxStaticTextBase>::From(wxStaticTextBase* ctrl, wxString* data)
{
    *data = ctrl->GetLabel();
    return true;
}

#endif // wxUSE_STATTEXT

//-----------------------------------------------------------------------------

#if wxUSE_TEXTCTRL

bool wxValidatorDataTransferImpl<wxTextCtrlBase>::To(wxTextCtrlBase* ctrl, wxString* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxTextCtrlBase>::From(wxTextCtrlBase* ctrl, wxString* data)
{
    *data = ctrl->GetValue();
    return true;
}

bool wxValidatorDataTransferImpl<wxTextCtrlBase>::To(wxTextCtrlBase* ctrl, int* data)
{
    const wxString str = wxString::Format("%d", *data);
    ctrl->SetValue(str);
    return true;
}

bool wxValidatorDataTransferImpl<wxTextCtrlBase>::From(wxTextCtrlBase* ctrl, int* data)
{
    *data = wxAtoi(ctrl->GetValue());
    return true;
}

bool wxValidatorDataTransferImpl<wxTextCtrlBase>::To(wxTextCtrlBase* ctrl, float* data)
{
    const wxString str = wxString::Format("%g", *data);
    ctrl->SetValue(str);
    return true;
}

bool wxValidatorDataTransferImpl<wxTextCtrlBase>::From(wxTextCtrlBase* ctrl, float* data)
{
    *data = static_cast<float>(wxAtof(ctrl->GetValue()));
    return true;
}

bool wxValidatorDataTransferImpl<wxTextCtrlBase>::To(wxTextCtrlBase* ctrl, double* data)
{
    const wxString str = wxString::Format("%g", *data);
    ctrl->SetValue(str);
    return true;
}

bool wxValidatorDataTransferImpl<wxTextCtrlBase>::From(wxTextCtrlBase* ctrl, double* data)
{
    *data = wxAtof(ctrl->GetValue());
    return true;
}

bool wxValidatorDataTransferImpl<wxTextCtrlBase>::To(wxTextCtrlBase* ctrl, wxFileName* data)
{
    ctrl->SetValue(data->GetFullPath());
    return true;
}

bool wxValidatorDataTransferImpl<wxTextCtrlBase>::From(wxTextCtrlBase* ctrl, wxFileName* data)
{
    *data = ctrl->GetValue();
    return true;
}

#endif // wxUSE_TEXTCTRL

//-----------------------------------------------------------------------------

#if wxUSE_TOGGLEBTN

bool wxValidatorDataTransferImpl<wxToggleButtonBase>::To(wxToggleButtonBase* btn, bool* data)
{
    btn->SetValue(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxToggleButtonBase>::From(wxToggleButtonBase* btn, bool* data)
{
    *data = btn->GetValue();
    return true;
}

#endif // wxUSE_TOGGLEBTN

//-----------------------------------------------------------------------------

#if wxUSE_CALENDARCTRL

bool wxValidatorDataTransferImpl<wxCalendarCtrlBase>::To(wxCalendarCtrlBase* ctrl, wxDateTime* data)
{
    return ctrl->SetDate(*data);
}

bool wxValidatorDataTransferImpl<wxCalendarCtrlBase>::From(wxCalendarCtrlBase* ctrl, wxDateTime* data)
{
    *data = ctrl->GetDate();
    return true;
}

#endif // wxUSE_CALENDARCTRL

//-----------------------------------------------------------------------------

#if wxUSE_DATEPICKCTRL || wxUSE_TIMEPICKCTRL

bool wxValidatorDataTransferImpl<wxDateTimePickerCtrlBase>::To(wxDateTimePickerCtrlBase* ctrl, wxDateTime* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxValidatorDataTransferImpl<wxDateTimePickerCtrlBase>::From(wxDateTimePickerCtrlBase* ctrl, wxDateTime* data)
{
    *data = ctrl->GetValue();
    return true;
}

#endif // wxUSE_DATEPICKCTRL || wxUSE_TIMEPICKCTRL

//-----------------------------------------------------------------------------

wxGenericValidatorBase* wxGenericValidatorBase::Convert(const wxAny& any) const
{
#if wxUSE_DATEPICKCTRL || wxUSE_TIMEPICKCTRL
    if ( wxDynamicCast(m_validatorWindow, wxDatePickerCtrl) ||
            wxDynamicCast(m_validatorWindow, wxTimePickerCtrl) )
    {
        if ( any.CheckType<wxDateTime>() )
            return new wxGenValidatorSimpleType<wxDateTimePickerCtrlBase, wxDateTime>
                    (static_cast<wxDateTime*>(m_data));

        return NULL;
    }
#endif // wxUSE_DATEPICKCTRL || wxUSE_TIMEPICKCTRL

#if wxUSE_CALENDARCTRL
    if ( wxDynamicCast(m_validatorWindow, wxCalendarCtrl) )
    {
        if ( any.CheckType<wxDateTime>() )
            return new wxGenValidatorSimpleType<wxCalendarCtrlBase, wxDateTime>
                    (static_cast<wxDateTime*>(m_data));

        return NULL;
    }
#endif // wxUSE_CALENDARCTRL

#if wxUSE_TOGGLEBTN
    if ( wxDynamicCast(m_validatorWindow, wxToggleButton) )
    {
        if ( any.CheckType<bool>() )
            return new wxGenValidatorSimpleType<wxToggleButtonBase, bool>
                    (static_cast<bool*>(m_data));

        return NULL;
    }
#endif // wxUSE_TOGGLEBTN

#if wxUSE_TEXTCTRL
    if ( wxDynamicCast(m_validatorWindow, wxTextCtrl) )
    {
        if ( any.CheckType<wxString>() )
            return new wxGenValidatorSimpleType<wxTextCtrlBase, wxString>
                    (static_cast<wxString*>(m_data));
        if ( any.CheckType<int>() )
            return new wxGenValidatorSimpleType<wxTextCtrlBase, int>
                    (static_cast<int*>(m_data));
        if ( any.CheckType<float>() )
            return new wxGenValidatorSimpleType<wxTextCtrlBase, float>
                    (static_cast<float*>(m_data));
        if ( any.CheckType<double>() )
            return new wxGenValidatorSimpleType<wxTextCtrlBase, double>
                    (static_cast<double*>(m_data));
        if ( any.CheckType<wxFileName>() )
            return new wxGenValidatorSimpleType<wxTextCtrlBase, wxFileName>
                    (static_cast<wxFileName*>(m_data));

        return NULL;
    }
#endif // wxUSE_TEXTCTRL

#if wxUSE_STATTEXT
    if ( wxDynamicCast(m_validatorWindow, wxStaticText) )
    {
        if ( any.CheckType<wxString>() )
            return new wxGenValidatorSimpleType<wxStaticTextBase, wxString>
                    (static_cast<wxString*>(m_data));

        return NULL;
    }
#endif // wxUSE_STATTEXT

#if wxUSE_SPLITTER
    if ( wxDynamicCast(m_validatorWindow, wxSplitterWindow) )
    {
        if ( any.CheckType<int>() )
            return new wxGenValidatorSimpleType<wxSplitterWindow, int>
                    (static_cast<int*>(m_data));

        return NULL;
    }
#endif // wxUSE_SPLITTER

#if wxUSE_SPINBTN
    if ( wxDynamicCast(m_validatorWindow, wxSpinCtrlDouble) )
    {
        if ( any.CheckType<double>() )
            return new wxGenValidatorSimpleType<wxSpinCtrlDouble, double>
                    (static_cast<double*>(m_data));

        return NULL;
    }

    if ( wxDynamicCast(m_validatorWindow, wxSpinCtrl) )
    {
        if ( any.CheckType<int>() )
            return new wxGenValidatorSimpleType<wxSpinCtrl, int>
                    (static_cast<int*>(m_data));

        return NULL;
    }

    if ( wxDynamicCast(m_validatorWindow, wxSpinButton) )
    {
        if ( any.CheckType<int>() )
            return new wxGenValidatorSimpleType<wxSpinButtonBase, int>
                    (static_cast<int*>(m_data));

        return NULL;
    }
#endif // wxUSE_SPINBTN

#if wxUSE_SLIDER
    if ( wxDynamicCast(m_validatorWindow, wxSlider) )
    {
        if ( any.CheckType<int>() )
            return new wxGenValidatorSimpleType<wxSliderBase, int>
                    (static_cast<int*>(m_data));

        return NULL;
    }
#endif // wxUSE_SLIDER

#if wxUSE_SCROLLBAR
    if ( wxDynamicCast(m_validatorWindow, wxScrollBar) )
    {
        if ( any.CheckType<int>() )
            return new wxGenValidatorSimpleType<wxScrollBarBase, int>
                    (static_cast<int*>(m_data));

        return NULL;
    }
#endif // wxUSE_SCROLLBAR

#if wxUSE_RADIOBOX
    if ( wxDynamicCast(m_validatorWindow, wxRadioBox) )
    {
        if ( any.CheckType<int>() )
            return new wxGenValidatorSimpleType<wxRadioBox, int>
                    (static_cast<int*>(m_data));

        return NULL;
    }
#endif // wxUSE_RADIOBOX

#if wxUSE_LISTBOX
    if ( wxDynamicCast(m_validatorWindow, wxListBox) )
    {
        if ( any.CheckType<int>() )
            return new wxGenValidatorSimpleType<wxListBoxBase, int>
                    (static_cast<int*>(m_data));
        if ( any.CheckType<wxArrayInt>() )
            return new wxGenValidatorSimpleType<wxListBoxBase, wxArrayInt>
                    (static_cast<wxArrayInt*>(m_data));

        return NULL;
    }
#endif // wxUSE_LISTBOX

#if wxUSE_GAUGE
    if ( wxDynamicCast(m_validatorWindow, wxGauge) )
    {
        if ( any.CheckType<int>() )
            return new wxGenValidatorSimpleType<wxGaugeBase, int>
                    (static_cast<int*>(m_data));

        return NULL;
    }
#endif // wxUSE_GAUGE

#if wxUSE_FONTPICKERCTRL
    if ( wxDynamicCast(m_validatorWindow, wxFontPickerCtrl) )
    {
        if ( any.CheckType<wxFont>() )
            return new wxGenValidatorSimpleType<wxFontPickerCtrl, wxFont>
                    (static_cast<wxFont*>(m_data));

        return NULL;
    }
#endif // wxUSE_FONTPICKERCTRL

#if wxUSE_DIRPICKERCTRL
    if ( wxDynamicCast(m_validatorWindow, wxDirPickerCtrl) )
    {
        if ( any.CheckType<wxString>() )
            return new wxGenValidatorSimpleType<wxDirPickerCtrl, wxString>
                    (static_cast<wxString*>(m_data));
        if ( any.CheckType<wxFileName>() )
            return new wxGenValidatorSimpleType<wxDirPickerCtrl, wxFileName>
                    (static_cast<wxFileName*>(m_data));

        return NULL;
    }
#endif // wxUSE_DIRPICKERCTRL

#if wxUSE_FILEPICKERCTRL
    if ( wxDynamicCast(m_validatorWindow, wxFilePickerCtrl) )
    {
        if ( any.CheckType<wxString>() )
            return new wxGenValidatorSimpleType<wxFilePickerCtrl, wxString>
                    (static_cast<wxString*>(m_data));
        if ( any.CheckType<wxFileName>() )
            return new wxGenValidatorSimpleType<wxFilePickerCtrl, wxFileName>
                    (static_cast<wxFileName*>(m_data));

        return NULL;
    }
#endif // wxUSE_FILEPICKERCTRL

#if wxUSE_COMBOBOX
    if ( wxDynamicCast(m_validatorWindow, wxComboBox) )
    {
        if ( any.CheckType<int>() )
            return new wxGenValidatorSimpleType<wxComboBox, int>
                    (static_cast<int*>(m_data));
        if ( any.CheckType<wxString>() )
            return new wxGenValidatorSimpleType<wxComboBox, wxString>
                    (static_cast<wxString*>(m_data));

        return NULL;
    }
#endif // wxUSE_COMBOBOX

#if wxUSE_COLOURPICKERCTRL
    if ( wxDynamicCast(m_validatorWindow, wxColourPickerCtrl) )
    {
        if ( any.CheckType<wxColour>() )
            return new wxGenValidatorSimpleType<wxColourPickerCtrl, wxColour>
                    (static_cast<wxColour*>(m_data));

        return NULL;
    }
#endif // wxUSE_COLOURPICKERCTRL

#if wxUSE_COLLPANE
    if ( wxDynamicCast(m_validatorWindow, wxCollapsiblePane) )
    {
        if ( any.CheckType<bool>() )
            return new wxGenValidatorSimpleType<wxCollapsiblePaneBase, bool>
                    (static_cast<bool*>(m_data));

        return NULL;
    }

    if ( wxDynamicCast(m_validatorWindow, wxCollapsibleHeaderCtrl) )
    {
        if ( any.CheckType<bool>() )
            return new wxGenValidatorSimpleType<wxCollapsibleHeaderCtrlBase, bool>
                    (static_cast<bool*>(m_data));

        return NULL;
    }
#endif // wxUSE_COLLPANE

#if wxUSE_CHOICE
    if ( wxDynamicCast(m_validatorWindow, wxChoice) )
    {
        if ( any.CheckType<int>() )
            return new wxGenValidatorSimpleType<wxChoiceBase, int>
                    (static_cast<int*>(m_data));
        if ( any.CheckType<wxString>() )
            return new wxGenValidatorSimpleType<wxChoiceBase, wxString>
                    (static_cast<wxString*>(m_data));

        return NULL;
    }
#endif // wxUSE_CHOICE

#if wxUSE_CHECKLISTBOX
    if ( wxDynamicCast(m_validatorWindow, wxCheckListBox) )
    {
        if ( any.CheckType<wxArrayInt>() )
            return new wxGenValidatorSimpleType<wxCheckListBoxBase, wxArrayInt>
                    (static_cast<wxArrayInt*>(m_data));

        return NULL;
    }
#endif // wxUSE_CHECKLISTBOX

#if wxUSE_CHECKBOX
    if ( wxDynamicCast(m_validatorWindow, wxCheckBox) )
    {
        if ( any.CheckType<bool>() )
            return new wxGenValidatorSimpleType<wxCheckBoxBase, bool>
                    (static_cast<bool*>(m_data));

        return NULL;
    }
#endif // wxUSE_CHECKBOX

#if wxUSE_BUTTON
    if ( wxDynamicCast(m_validatorWindow, wxButton) )
    {
        if ( any.CheckType<wxString>() )
            return new wxGenValidatorSimpleType<wxButtonBase, wxString>
                    (static_cast<wxString*>(m_data));

        return NULL;
    }
#endif // wxUSE_BUTTON

    return NULL;
}

#if defined(HAVE_STD_VARIANT)
wxIMPLEMENT_DYNAMIC_CLASS(wxMonoValidationEvent, wxEvent);

wxDEFINE_EVENT( wxEVT_SET_ALTERNATIVE, wxMonoValidationEvent );
wxDEFINE_EVENT( wxEVT_UNSET_ALTERNATIVE, wxMonoValidationEvent );
#endif // HAVE_STD_VARIANT

void wxSetGenericValidator(wxPanel* panel, const wxValidator& val)
{
    panel->SetValidator(val);
}

#endif // !wxUSE_DATATRANSFER

#endif // wxUSE_VALIDATORS
