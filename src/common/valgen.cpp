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

bool wxDataTransferImpl<wxButtonBase>::To(wxButtonBase* btn, wxString* data)
{
    btn->SetLabel(*data);
    return true;
}

bool wxDataTransferImpl<wxButtonBase>::From(wxButtonBase* btn, wxString* data)
{
    *data = btn->GetLabel();
    return true;
}

#endif // wxUSE_BUTTON

//-----------------------------------------------------------------------------

#if wxUSE_CHECKBOX

bool wxDataTransferImpl<wxCheckBoxBase>::To(wxCheckBoxBase* ctrl, bool* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxDataTransferImpl<wxCheckBoxBase>::From(wxCheckBoxBase* ctrl, bool* data)
{
    *data = ctrl->GetValue();
    return true;
}

#endif // wxUSE_CHECKBOX

//-----------------------------------------------------------------------------

#if wxUSE_CHECKLISTBOX

bool wxDataTransferImpl<wxCheckListBoxBase>::To(wxCheckListBoxBase* ctrl, wxArrayInt* arr)
{
    size_t i, count = ctrl->GetCount();

    for ( i = 0 ; i < count; ++i )
        ctrl->Check(i, false);

    count = arr->GetCount();
    for ( i = 0 ; i < count; ++i )
        ctrl->Check(arr->Item(i));

    return true;
}

bool wxDataTransferImpl<wxCheckListBoxBase>::From(wxCheckListBoxBase* ctrl, wxArrayInt* arr)
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

bool wxDataTransferImpl<wxChoiceBase>::To(wxChoiceBase* ctrl, int* data)
{
    ctrl->SetSelection(*data);
    return true;
}

bool wxDataTransferImpl<wxChoiceBase>::To(wxChoiceBase* ctrl, wxString* data)
{
    return ctrl->SetStringSelection(*data);
}

bool wxDataTransferImpl<wxChoiceBase>::From(wxChoiceBase* ctrl, int* data)
{
    *data = ctrl->GetSelection();
    return true;
}

bool wxDataTransferImpl<wxChoiceBase>::From(wxChoiceBase* ctrl, wxString* data)
{
    *data = ctrl->GetStringSelection();
    return true;
}

#endif // wxUSE_CHOICE

//-----------------------------------------------------------------------------

#if wxUSE_COLLPANE

bool wxDataTransferImpl<wxCollapsibleHeaderCtrlBase>::To(wxCollapsibleHeaderCtrlBase* ctrl, bool* value)
{
    ctrl->SetCollapsed(*value);
    return true;
}

bool wxDataTransferImpl<wxCollapsibleHeaderCtrlBase>::From(wxCollapsibleHeaderCtrlBase* ctrl, bool* value)
{
    *value = ctrl->IsCollapsed();
    return true;
}

//-----------------------------------------------------------------------------

bool wxDataTransferImpl<wxCollapsiblePaneBase>::To(wxCollapsiblePaneBase* ctrl, bool* data)
{
    ctrl->Collapse(*data);
    return true;
}

bool wxDataTransferImpl<wxCollapsiblePaneBase>::From(wxCollapsiblePaneBase* ctrl, bool* data)
{
    *data = ctrl->IsCollapsed();
    return true;
}

#endif // wxUSE_COLLPANE

//-----------------------------------------------------------------------------

#if wxUSE_COLOURPICKERCTRL

bool wxDataTransferImpl<wxColourPickerCtrl>::To(wxColourPickerCtrl* ctrl, wxColour* data)
{
    ctrl->SetColour(*data);
    return true;
}

bool wxDataTransferImpl<wxColourPickerCtrl>::From(wxColourPickerCtrl* ctrl, wxColour* data)
{
    *data = ctrl->GetColour();
    return true;
}

#endif // wxUSE_COLOURPICKERCTRL

//-----------------------------------------------------------------------------

#if wxUSE_COMBOBOX

bool wxDataTransferImpl<wxComboBox>::To(wxComboBox* ctrl, int* data)
{
    ctrl->SetSelection(*data);
    return true;
}

bool wxDataTransferImpl<wxComboBox>::From(wxComboBox* ctrl, int* data)
{
    *data = ctrl->GetSelection();
    return true;
}

bool wxDataTransferImpl<wxComboBox>::To(wxComboBox* ctrl, wxString* data)
{
    if ( !ctrl->SetStringSelection(*data) )
    {
        if ( (ctrl->GetWindowStyle() & wxCB_READONLY) )
            return false;

        ctrl->SetValue(*data);
    }

    return true;
}

bool wxDataTransferImpl<wxComboBox>::From(wxComboBox* ctrl, wxString* data)
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

bool wxDataTransferImpl<wxFileDirPickerCtrlBase>::To(wxFileDirPickerCtrlBase* ctrl, wxString* data)
{
    ctrl->SetPath(*data);
    return true;
}

bool wxDataTransferImpl<wxFileDirPickerCtrlBase>::From(wxFileDirPickerCtrlBase* ctrl, wxString* data)
{
    *data = ctrl->GetPath();
    return true;
}

#endif // wxUSE_FILEPICKERCTRL || wxUSE_DIRPICKERCTRL

//-----------------------------------------------------------------------------

#if wxUSE_FILEPICKERCTRL

bool wxDataTransferImpl<wxFilePickerCtrl>::To(wxFilePickerCtrl* ctrl, wxFileName* data)
{
    ctrl->SetFileName(*data);
    return true;
}

bool wxDataTransferImpl<wxFilePickerCtrl>::From(wxFilePickerCtrl* ctrl, wxFileName* data)
{
    *data = ctrl->GetFileName();
    return true;
}

bool wxDataTransferImpl<wxFilePickerCtrl>::To(wxFilePickerCtrl* ctrl, wxString* data)
{
    return wxDataTransferImpl<wxFileDirPickerCtrlBase>::To(ctrl, data);
}

bool wxDataTransferImpl<wxFilePickerCtrl>::From(wxFilePickerCtrl* ctrl, wxString* data)
{
    return wxDataTransferImpl<wxFileDirPickerCtrlBase>::From(ctrl, data);
}

#endif // wxUSE_FILEPICKERCTRL

//-----------------------------------------------------------------------------

#if wxUSE_DIRPICKERCTRL

bool wxDataTransferImpl<wxDirPickerCtrl>::To(wxDirPickerCtrl* ctrl, wxFileName* data)
{
    ctrl->SetDirName(*data);
    return true;
}

bool wxDataTransferImpl<wxDirPickerCtrl>::From(wxDirPickerCtrl* ctrl, wxFileName* data)
{
    *data = ctrl->GetDirName();
    return true;
}

bool wxDataTransferImpl<wxDirPickerCtrl>::To(wxDirPickerCtrl* ctrl, wxString* data)
{
    return wxDataTransferImpl<wxFileDirPickerCtrlBase>::To(ctrl, data);
}


bool wxDataTransferImpl<wxDirPickerCtrl>::From(wxDirPickerCtrl* ctrl, wxString* data)
{
    return wxDataTransferImpl<wxFileDirPickerCtrlBase>::From(ctrl, data);
}

#endif // wxUSE_DIRPICKERCTRL

//-----------------------------------------------------------------------------

#if wxUSE_FONTPICKERCTRL

bool wxDataTransferImpl<wxFontPickerCtrl>::To(wxFontPickerCtrl* ctrl, wxFont* data)
{
    ctrl->SetSelectedFont(*data);
    return true;
}

bool wxDataTransferImpl<wxFontPickerCtrl>::From(wxFontPickerCtrl* ctrl, wxFont* data)
{
    *data = ctrl->GetSelectedFont();
    return true;
}

#endif // wxUSE_FONTPICKERCTRL

//-----------------------------------------------------------------------------

#if wxUSE_GAUGE

bool wxDataTransferImpl<wxGaugeBase>::To(wxGaugeBase* ctrl, int* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxDataTransferImpl<wxGaugeBase>::From(wxGaugeBase* ctrl, int* data)
{
    *data = ctrl->GetValue();
    return true;
}

#endif // wxUSE_GAUGE

//-----------------------------------------------------------------------------

#if wxUSE_LISTBOX

bool wxDataTransferImpl<wxListBoxBase>::To(wxListBoxBase* ctrl, int* data)
{
    wxASSERT( (ctrl->GetWindowStyle() & wxLB_SINGLE) );

    ctrl->SetSelection(*data);
    return true;
}

bool wxDataTransferImpl<wxListBoxBase>::From(wxListBoxBase* ctrl, int* data)
{
    *data = ctrl->GetSelection();
    return true;
}

bool wxDataTransferImpl<wxListBoxBase>::To(wxListBoxBase* ctrl, wxArrayInt* arr)
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

bool wxDataTransferImpl<wxListBoxBase>::From(wxListBoxBase* ctrl, wxArrayInt* arr)
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

bool wxDataTransferImpl<wxRadioBoxBase>::To(wxRadioBoxBase* ctrl, int* data)
{
    ctrl->SetSelection(*data);
    return true;
}

bool wxDataTransferImpl<wxRadioBoxBase>::From(wxRadioBoxBase* ctrl, int* data)
{
    *data = ctrl->GetSelection();
    return true;
}

#endif // wxUSE_RADIOBOX

//-----------------------------------------------------------------------------

#if wxUSE_SCROLLBAR

bool wxDataTransferImpl<wxScrollBarBase>::To(wxScrollBarBase* ctrl, int* data)
{
    ctrl->SetThumbPosition(*data);
    return true;
}

bool wxDataTransferImpl<wxScrollBarBase>::From(wxScrollBarBase* ctrl, int* data)
{
    *data = ctrl->GetThumbPosition();
    return true;
}

#endif // wxUSE_SCROLLBAR

//-----------------------------------------------------------------------------

#if wxUSE_SLIDER

bool wxDataTransferImpl<wxSliderBase>::To(wxSliderBase* ctrl, int* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxDataTransferImpl<wxSliderBase>::From(wxSliderBase* ctrl, int* data)
{
    *data = ctrl->GetValue();
    return true;
}

#endif // wxUSE_SLIDER

//-----------------------------------------------------------------------------

#if wxUSE_SPINBTN

bool wxDataTransferImpl<wxSpinButtonBase>::To(wxSpinButtonBase* ctrl, int* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxDataTransferImpl<wxSpinButtonBase>::From(wxSpinButtonBase* ctrl, int* data)
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

bool wxDataTransferImpl<wxSpinCtrl>::To(wxSpinCtrl* ctrl, int* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxDataTransferImpl<wxSpinCtrl>::From(wxSpinCtrl* ctrl, int* data)
{
    WX_TRANSFER_DATA_FROM_SPINCTRL(ctrl, data);
    return true;
}

// wxSpinCtrlDouble

bool wxDataTransferImpl<wxSpinCtrlDouble>::To(wxSpinCtrlDouble* ctrl, double* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxDataTransferImpl<wxSpinCtrlDouble>::From(wxSpinCtrlDouble* ctrl, double* data)
{
    WX_TRANSFER_DATA_FROM_SPINCTRLDOUBLE(ctrl, data);
    return true;
}

#endif // wxUSE_SPINCTRL

//-----------------------------------------------------------------------------

#if wxUSE_SPLITTER

bool wxDataTransferImpl<wxSplitterWindow>::To(wxSplitterWindow* win, int* data)
{
    win->SetSashPosition(*data);
    return true;
}

bool wxDataTransferImpl<wxSplitterWindow>::From(wxSplitterWindow* win, int* data)
{
    *data = win->GetSashPosition();
    return true;
}

#endif // wxUSE_SPLITTER

//-----------------------------------------------------------------------------

#if wxUSE_STATTEXT

bool wxDataTransferImpl<wxStaticTextBase>::To(wxStaticTextBase* ctrl, wxString* data)
{
    ctrl->SetLabel(*data);
    return true;
}

bool wxDataTransferImpl<wxStaticTextBase>::From(wxStaticTextBase* ctrl, wxString* data)
{
    *data = ctrl->GetLabel();
    return true;
}

#endif // wxUSE_STATTEXT

//-----------------------------------------------------------------------------

#if wxUSE_TEXTCTRL

bool wxDataTransferImpl<wxTextCtrlBase>::To(wxTextCtrlBase* ctrl, wxString* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxDataTransferImpl<wxTextCtrlBase>::From(wxTextCtrlBase* ctrl, wxString* data)
{
    *data = ctrl->GetValue();
    return true;
}

bool wxDataTransferImpl<wxTextCtrlBase>::To(wxTextCtrlBase* ctrl, int* data)
{
    const wxString str = wxString::Format("%d", *data);
    ctrl->SetValue(str);
    return true;
}

bool wxDataTransferImpl<wxTextCtrlBase>::From(wxTextCtrlBase* ctrl, int* data)
{
    *data = wxAtoi(ctrl->GetValue());
    return true;
}

bool wxDataTransferImpl<wxTextCtrlBase>::To(wxTextCtrlBase* ctrl, float* data)
{
    const wxString str = wxString::Format("%g", *data);
    ctrl->SetValue(str);
    return true;
}

bool wxDataTransferImpl<wxTextCtrlBase>::From(wxTextCtrlBase* ctrl, float* data)
{
    *data = static_cast<float>(wxAtof(ctrl->GetValue()));
    return true;
}

bool wxDataTransferImpl<wxTextCtrlBase>::To(wxTextCtrlBase* ctrl, double* data)
{
    const wxString str = wxString::Format("%g", *data);
    ctrl->SetValue(str);
    return true;
}

bool wxDataTransferImpl<wxTextCtrlBase>::From(wxTextCtrlBase* ctrl, double* data)
{
    *data = wxAtof(ctrl->GetValue());
    return true;
}

bool wxDataTransferImpl<wxTextCtrlBase>::To(wxTextCtrlBase* ctrl, wxFileName* data)
{
    ctrl->SetValue(data->GetFullPath());
    return true;
}

bool wxDataTransferImpl<wxTextCtrlBase>::From(wxTextCtrlBase* ctrl, wxFileName* data)
{
    *data = ctrl->GetValue();
    return true;
}

#endif // wxUSE_TEXTCTRL

//-----------------------------------------------------------------------------

#if wxUSE_TOGGLEBTN

bool wxDataTransferImpl<wxToggleButtonBase>::To(wxToggleButtonBase* btn, bool* data)
{
    btn->SetValue(*data);
    return true;
}

bool wxDataTransferImpl<wxToggleButtonBase>::From(wxToggleButtonBase* btn, bool* data)
{
    *data = btn->GetValue();
    return true;
}

#endif // wxUSE_TOGGLEBTN

//-----------------------------------------------------------------------------

#if wxUSE_CALENDARCTRL

bool wxDataTransferImpl<wxCalendarCtrlBase>::To(wxCalendarCtrlBase* ctrl, wxDateTime* data)
{
    return ctrl->SetDate(*data);
}

bool wxDataTransferImpl<wxCalendarCtrlBase>::From(wxCalendarCtrlBase* ctrl, wxDateTime* data)
{
    *data = ctrl->GetDate();
    return true;
}

#endif // wxUSE_CALENDARCTRL

//-----------------------------------------------------------------------------

#if wxUSE_DATEPICKCTRL || wxUSE_TIMEPICKCTRL

bool wxDataTransferImpl<wxDateTimePickerCtrlBase>::To(wxDateTimePickerCtrlBase* ctrl, wxDateTime* data)
{
    ctrl->SetValue(*data);
    return true;
}

bool wxDataTransferImpl<wxDateTimePickerCtrlBase>::From(wxDateTimePickerCtrlBase* ctrl, wxDateTime* data)
{
    *data = ctrl->GetValue();
    return true;
}

#endif // wxUSE_DATEPICKCTRL || wxUSE_TIMEPICKCTRL

//-----------------------------------------------------------------------------

wxGenericValidatorBase* wxGenericValidatorBase::Convert(bool* data) const
{
#if wxUSE_TOGGLEBTN
    if ( wxDynamicCast(m_validatorWindow, wxToggleButton) )
        return new wxGenericValidatorSimpleType<wxToggleButtonBase, bool>(data);
#endif // wxUSE_TOGGLEBTN

#if wxUSE_COLLPANE
    if ( wxDynamicCast(m_validatorWindow, wxCollapsiblePane) )
        return new wxGenericValidatorSimpleType<wxCollapsiblePaneBase, bool>(data);

    if ( wxDynamicCast(m_validatorWindow, wxCollapsibleHeaderCtrl) )
        return new wxGenericValidatorSimpleType<wxCollapsibleHeaderCtrlBase, bool>(data);
#endif // wxUSE_COLLPANE

#if wxUSE_CHECKBOX
    if ( wxDynamicCast(m_validatorWindow, wxCheckBox) )
        return new wxGenericValidatorSimpleType<wxCheckBoxBase, bool>(data);
#endif // wxUSE_CHECKBOX

    return NULL;
}

wxGenericValidatorBase* wxGenericValidatorBase::Convert(int* data) const
{
#if wxUSE_TEXTCTRL
    if ( wxDynamicCast(m_validatorWindow, wxTextCtrl) )
        return new wxGenericValidatorSimpleType<wxTextCtrlBase, int>(data);
#endif // wxUSE_TEXTCTRL

#if wxUSE_SPLITTER
    if ( wxDynamicCast(m_validatorWindow, wxSplitterWindow) )
        return new wxGenericValidatorSimpleType<wxSplitterWindow, int>(data);
#endif // wxUSE_SPLITTER

#if wxUSE_SPINBTN
    if ( wxDynamicCast(m_validatorWindow, wxSpinCtrl) )
        return new wxGenericValidatorSimpleType<wxSpinCtrl, int>(data);

    if ( wxDynamicCast(m_validatorWindow, wxSpinButton) )
        return new wxGenericValidatorSimpleType<wxSpinButtonBase, int>(data);
#endif // wxUSE_SPINBTN

#if wxUSE_SLIDER
    if ( wxDynamicCast(m_validatorWindow, wxSlider) )
        return new wxGenericValidatorSimpleType<wxSliderBase, int>(data);
#endif // wxUSE_SLIDER

#if wxUSE_SCROLLBAR
    if ( wxDynamicCast(m_validatorWindow, wxScrollBar) )
        return new wxGenericValidatorSimpleType<wxScrollBarBase, int>(data);
#endif // wxUSE_SCROLLBAR

#if wxUSE_RADIOBOX
    if ( wxDynamicCast(m_validatorWindow, wxRadioBox) )
        return new wxGenericValidatorSimpleType<wxRadioBox, int>(data);
#endif // wxUSE_RADIOBOX

#if wxUSE_LISTBOX
    if ( wxDynamicCast(m_validatorWindow, wxListBox) )
        return new wxGenericValidatorSimpleType<wxListBoxBase, int>(data);
#endif // wxUSE_LISTBOX

#if wxUSE_GAUGE
    if ( wxDynamicCast(m_validatorWindow, wxGauge) )
        return new wxGenericValidatorSimpleType<wxGaugeBase, int>(data);
#endif // wxUSE_GAUGE

#if wxUSE_COMBOBOX
    if ( wxDynamicCast(m_validatorWindow, wxComboBox) )
        return new wxGenericValidatorSimpleType<wxComboBox, int>(data);
#endif // wxUSE_COMBOBOX

#if wxUSE_CHOICE
    if ( wxDynamicCast(m_validatorWindow, wxChoice) )
        return new wxGenericValidatorSimpleType<wxChoiceBase, int>(data);
#endif // wxUSE_CHOICE

    return NULL;
}

wxGenericValidatorBase* wxGenericValidatorBase::Convert(float* data) const
{
#if wxUSE_TEXTCTRL
    if ( wxDynamicCast(m_validatorWindow, wxTextCtrl) )
        return new wxGenericValidatorSimpleType<wxTextCtrlBase, float>(data);
#endif // wxUSE_TEXTCTRL

    return NULL;
}

wxGenericValidatorBase* wxGenericValidatorBase::Convert(double* data) const
{
#if wxUSE_TEXTCTRL
    if ( wxDynamicCast(m_validatorWindow, wxTextCtrl) )
        return new wxGenericValidatorSimpleType<wxTextCtrlBase, double>(data);
#endif // wxUSE_TEXTCTRL

#if wxUSE_SPINBTN
    if ( wxDynamicCast(m_validatorWindow, wxSpinCtrlDouble) )
        return new wxGenericValidatorSimpleType<wxSpinCtrlDouble, double>(data);
#endif // wxUSE_SPINBTN

    return NULL;
}

wxGenericValidatorBase* wxGenericValidatorBase::Convert(wxString* data) const
{
#if wxUSE_TEXTCTRL
    if ( wxDynamicCast(m_validatorWindow, wxTextCtrl) )
        return new wxGenericValidatorSimpleType<wxTextCtrlBase, wxString>(data);
#endif // wxUSE_TEXTCTRL

#if wxUSE_STATTEXT
    if ( wxDynamicCast(m_validatorWindow, wxStaticText) )
        return new wxGenericValidatorSimpleType<wxStaticTextBase, wxString>(data);
#endif // wxUSE_STATTEXT

#if wxUSE_DIRPICKERCTRL
    if ( wxDynamicCast(m_validatorWindow, wxDirPickerCtrl) )
        return new wxGenericValidatorSimpleType<wxDirPickerCtrl, wxString>(data);
#endif // wxUSE_DIRPICKERCTRL

#if wxUSE_FILEPICKERCTRL
    if ( wxDynamicCast(m_validatorWindow, wxFilePickerCtrl) )
        return new wxGenericValidatorSimpleType<wxFilePickerCtrl, wxString>(data);
#endif // wxUSE_FILEPICKERCTRL

#if wxUSE_COMBOBOX
    if ( wxDynamicCast(m_validatorWindow, wxComboBox) )
        return new wxGenericValidatorSimpleType<wxComboBox, wxString>(data);
#endif // wxUSE_COMBOBOX

#if wxUSE_CHOICE
    if ( wxDynamicCast(m_validatorWindow, wxChoice) )
        return new wxGenericValidatorSimpleType<wxChoiceBase, wxString>(data);
#endif // wxUSE_CHOICE

#if wxUSE_BUTTON
    if ( wxDynamicCast(m_validatorWindow, wxButton) )
        return new wxGenericValidatorSimpleType<wxButtonBase, wxString>(data);
#endif // wxUSE_BUTTON

    return NULL;
}

wxGenericValidatorBase* wxGenericValidatorBase::Convert(wxArrayInt* data) const
{
#if wxUSE_LISTBOX
    if ( wxDynamicCast(m_validatorWindow, wxListBox) )
        return new wxGenericValidatorSimpleType<wxListBoxBase, wxArrayInt>(data);
#endif // wxUSE_LISTBOX

#if wxUSE_CHECKLISTBOX
    if ( wxDynamicCast(m_validatorWindow, wxCheckListBox) )
        return new wxGenericValidatorSimpleType<wxCheckListBoxBase, wxArrayInt>(data);
#endif // wxUSE_CHECKLISTBOX

    return NULL;
}

wxGenericValidatorBase* wxGenericValidatorBase::Convert(wxDateTime* data) const
{
#if wxUSE_DATEPICKCTRL || wxUSE_TIMEPICKCTRL
    if ( wxDynamicCast(m_validatorWindow, wxDatePickerCtrl) ||
            wxDynamicCast(m_validatorWindow, wxTimePickerCtrl) )
    {
        return new
            wxGenericValidatorSimpleType<wxDateTimePickerCtrlBase, wxDateTime>(data);
    }
#endif // wxUSE_DATEPICKCTRL || wxUSE_TIMEPICKCTRL

#if wxUSE_CALENDARCTRL
    if ( wxDynamicCast(m_validatorWindow, wxCalendarCtrl) )
        return new wxGenericValidatorSimpleType<wxCalendarCtrlBase, wxDateTime>(data);
#endif // wxUSE_CALENDARCTRL

    return NULL;
}

wxGenericValidatorBase* wxGenericValidatorBase::Convert(wxFileName* data) const
{
#if wxUSE_TEXTCTRL
    if ( wxDynamicCast(m_validatorWindow, wxTextCtrl) )
        return new wxGenericValidatorSimpleType<wxTextCtrlBase, wxFileName>(data);
#endif // wxUSE_TEXTCTRL

#if wxUSE_DIRPICKERCTRL
    if ( wxDynamicCast(m_validatorWindow, wxDirPickerCtrl) )
        return new wxGenericValidatorSimpleType<wxDirPickerCtrl, wxFileName>(data);
#endif // wxUSE_DIRPICKERCTRL

#if wxUSE_FILEPICKERCTRL
    if ( wxDynamicCast(m_validatorWindow, wxFilePickerCtrl) )
        return new wxGenericValidatorSimpleType<wxFilePickerCtrl, wxFileName>(data);
#endif // wxUSE_FILEPICKERCTRL

    return NULL;
}

wxGenericValidatorBase* wxGenericValidatorBase::Convert(wxColour* data) const
{
#if wxUSE_COLOURPICKERCTRL
    if ( wxDynamicCast(m_validatorWindow, wxColourPickerCtrl) )
        return new wxGenericValidatorSimpleType<wxColourPickerCtrl, wxColour>(data);
#endif // wxUSE_COLOURPICKERCTRL

    return NULL;
}

wxGenericValidatorBase* wxGenericValidatorBase::Convert(wxFont* data) const
{
#if wxUSE_FONTPICKERCTRL
    if ( wxDynamicCast(m_validatorWindow, wxFontPickerCtrl) )
        return new wxGenericValidatorSimpleType<wxFontPickerCtrl, wxFont>(data);
#endif // wxUSE_FONTPICKERCTRL

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
