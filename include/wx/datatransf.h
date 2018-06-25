/////////////////////////////////////////////////////////////////////////////
// Name:        wx/datatransf.h
// Purpose:     .
// Author:      Ali Kettab
// Created:     2018-06-13
// Copyright:   (c) 2018 Ali Kettab
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DATATRANSFER_H_
#define _WX_DATATRANSFER_H_

#if wxUSE_VALIDATORS

//-----------------------------------------------------------------------------
//
// wxGenericValidatorType<> just delegates to wxDataTransfer<> to do the actual
// data transfer (via template specializations)
//
// How it works:
// -------------
// say you have a window 'MyWindow' which can transfer wxStrings and CustomData.
// then wxDataTransfer<> specialization for MyWindow would look like this:
//
// template<>
// struct wxDataTransfer<MyWindow>
// {
//     static bool To(MyWindow* win, wxString* data)
//     { /*implementation*/ }
//     static bool From(MyWindow* win, wxString* data)
//     { /*implementation*/ }
//
//     static bool To(MyWindow* win, CustomData* data)
//     { /*implementation*/ }
//     static bool From(MyWindow* win, CustomData* data)
//     { /*implementation*/ }
// };
//
//-----------------------------------------------------------------------------

template<class W>
struct wxDataTransfer
{
    static bool To(wxWindow*, void*);
    static bool From(wxWindow*, void*);
};


//-----------------------------------------------------------------------------
//
// wxDataTransfer<> might be implemented in terms of base class, in which case
// derived-classes no longer required to reimplement wxDataTransfer<> on their
// own. Instead, it's enough to specialize wxFwdDataTransfer<> and define the
// base which already implements wxDataTransfer<> like so:
//
// template<>
// struct wxFwdDataTransfer<MyWindow>
// {
//     typedef MyWindowBase Base;
// };
//
// or preferably:
//
// wxDECLARE_DATA_TRANSFER_FWD(MyWindow, MyWindowBase);
//
// ----------------------------------------------------------------------------
//
// N.B. CHAINED FORWARDING is not supported in current implementation.
//      i.e. if MySpecialWindow derives from MyWindow and MyWindow just forwards
//           to MyWindowBase, then this declaration won't work:
//
//      wxDECLARE_DATA_TRANSFER_FWD(MySpecialWindow, MyWindow);
//
//      you have to use this instead:
//
//      wxDECLARE_DATA_TRANSFER_FWD(MySpecialWindow, MyWindowBase);
//
//-----------------------------------------------------------------------------

template<class W>
struct wxFwdDataTransfer
{
    typedef void Base;
};


//-----------------------------------------------------------------------------
// Helper macros
//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_FWD(T, BASE) \
template<> \
struct wxFwdDataTransfer<T> \
{ \
    typedef BASE Base; \
}

//=============================================================================
// implementation only from now on
//=============================================================================

// NOTE:
// -----
// I am not sure which is best:
//
// 1) leave these wxDECLARE_DATA_TRANSFER_XXX as they are, in which case we can
//    leave out all these WXDLLIMPEXP_XXX. as all these declarations are also
//    just inline implementations.
//
// 2) Add a bunch of wxDEFINE_DATA_TRANSFER_XXX macros and make these
//    wxDECLARE_DATA_TRANSFER_XXX pure declarations only. in that case, the
//    wxDEFINE_DATA_TRANSFER_XXX should be put in the implementation file where
//    the class XXX in question reside.
//

#define wxDECLARE_DATA_TRANSFER_BUTTON()					\
template<> 													\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxButtonBase> 		\
{ 															\
    static bool To(wxButtonBase* btn, wxString* data) 		\
    { 														\
        btn->SetLabel(*data); 								\
        return true; 										\
    } 														\
															\
    static bool From(wxButtonBase* btn, wxString* data) 	\
    { 														\
        *data = btn->GetLabel(); 							\
        return true; 										\
    } 														\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_CHECKBOX()					\
template<>													\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxCheckBoxBase>		\
{															\
    static bool To(wxCheckBoxBase* ctrl, bool* data)		\
    {														\
        ctrl->SetValue(*data);								\
        return true;										\
    }														\
    														\
    static bool From(wxCheckBoxBase* ctrl, bool* data)		\
    {														\
        *data = ctrl->GetValue();							\
        return true;										\
    }														\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_CHECKLISTBOX() 							\
template<> 																\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxCheckListBoxBase> 				\
{ 																		\
    static bool To(wxCheckListBoxBase* ctrl, wxArrayInt* arr) 			\
    { 																	\
        size_t i, count = ctrl->GetCount(); 							\
																		\
        for ( i = 0 ; i < count; ++i ) 									\
            ctrl->Check(i, false); 										\
																		\
        count = arr->GetCount(); 										\
        for ( i = 0 ; i < count; ++i ) 									\
            ctrl->Check(arr->Item(i)); 									\
																		\
        return true; 													\
    } 																	\
																		\
    static bool From(wxCheckListBoxBase* ctrl, wxArrayInt* arr) 		\
    { 																	\
        arr->Clear(); 													\
																		\
        for ( size_t i = 0, count = ctrl->GetCount(); i < count; ++i ) 	\
        { 																\
            if ( ctrl->IsChecked(i) ) 									\
                arr->Add(i); 											\
        } 																\
																		\
        return true; 													\
    } 																	\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_CHOICE()					\
template<> 													\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxChoiceBase> 		\
{ 															\
    static bool To(wxChoiceBase* ctrl, int* data) 			\
    { 														\
        ctrl->SetSelection(*data); 							\
        return true; 										\
    } 														\
															\
    static bool To(wxChoiceBase* ctrl, wxString* data) 		\
    { 														\
        return ctrl->SetStringSelection(*data); 			\
    } 														\
															\
    static bool From(wxChoiceBase* ctrl, int* data) 		\
    { 														\
        *data = ctrl->GetSelection(); 						\
        return true; 										\
    } 														\
															\
    static bool From(wxChoiceBase* ctrl, wxString* data) 	\
    { 														\
        *data = ctrl->GetStringSelection(); 				\
        return true; 										\
    } 														\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_COLLAPSIBLEHEADER()						\
template<>																\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxCollapsibleHeaderCtrlBase>		\
{																		\
    static bool To(wxCollapsibleHeaderCtrlBase* ctrl, bool* value)		\
    {																	\
        ctrl->SetCollapsed(*value);										\
        return true;													\
    }																	\
    																	\
    static bool From(wxCollapsibleHeaderCtrlBase* ctrl, bool* value)	\
    {																	\
        *value = ctrl->IsCollapsed();									\
        return true;													\
    }																	\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_COLLAPSIBLEPANE()					\
template<>															\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxCollapsiblePaneBase>		\
{																	\
    static bool To(wxCollapsiblePaneBase* ctrl, bool* data)			\
    {																\
        ctrl->Collapse(*data);										\
        return true;												\
    }																\
    																\
    static bool From(wxCollapsiblePaneBase* ctrl, bool* data)		\
    {																\
        *data = ctrl->IsCollapsed();								\
        return true;												\
    }																\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_COLOURPICKER()						\
template<>															\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxColourPickerCtrl>			\
{																	\
    static bool To(wxColourPickerCtrl* ctrl, wxColour* data)		\
    {																\
        ctrl->SetColour(*data);										\
        return true;												\
    }																\
    																\
    static bool From(wxColourPickerCtrl* ctrl, wxColour* data)		\
    {																\
        *data = ctrl->GetColour();									\
        return true;												\
    }																\
}


//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_COMBOBOX()						\
template<>														\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxComboBox>				\
{																\
    static bool To(wxComboBox* ctrl, int* data)					\
    {															\
        ctrl->SetSelection(*data);								\
        return true;											\
    }															\
    															\
    static bool To(wxComboBox* ctrl, wxString* data)			\
    {															\
        if ( !ctrl->SetStringSelection(*data) )					\
        {														\
            if ( (ctrl->GetWindowStyle() & wxCB_READONLY) )		\
                return false;									\
            													\
            ctrl->SetValue(*data);								\
        }														\
        														\
        return true;											\
    }															\
    															\
    static bool From(wxComboBox* ctrl, int* data)				\
    {															\
        *data = ctrl->GetSelection();							\
        return true;											\
    }															\
    															\
    static bool From(wxComboBox* ctrl, wxString* data)			\
    {															\
        if ( ctrl->GetWindowStyle() & wxCB_READONLY )			\
            *data = ctrl->GetStringSelection();					\
        else													\
            *data = ctrl->GetValue();							\
        														\
        return true;											\
    }															\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_FILEDIRPICKER()						\
template<>															\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxFileDirPickerCtrlBase>		\
{																	\
    static bool To(wxFileDirPickerCtrlBase* ctrl, wxString* data)	\
    {																\
        ctrl->SetPath(*data);										\
        return true;												\
    }																\
    																\
    static bool From(wxFileDirPickerCtrlBase* ctrl, wxString* data)	\
    {																\
        *data = ctrl->GetPath();									\
        return true;												\
    }																\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_FILEPICKER()					\
template<>														\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxFilePickerCtrl>		\
{																\
    static bool To(wxFilePickerCtrl* ctrl, wxFileName* data)	\
    {															\
        ctrl->SetFileName(*data);								\
        return true;											\
    }															\
    															\
    static bool From(wxFilePickerCtrl* ctrl, wxFileName* data)	\
    {															\
        *data = ctrl->GetFileName();							\
        return true;											\
    }															\
    															\
    static bool To(wxFilePickerCtrl* ctrl, wxString* data)		\
    {															\
        return wxDataTransfer<wxFileDirPickerCtrlBase>::To(ctrl, data); \
    }															\
    															\
    static bool From(wxFilePickerCtrl* ctrl, wxString* data)	\
    {															\
        return wxDataTransfer<wxFileDirPickerCtrlBase>::From(ctrl, data); \
    }															\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_DIRPICKER()						\
template<>														\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxDirPickerCtrl>			\
{																\
    static bool To(wxDirPickerCtrl* ctrl, wxFileName* data)		\
    {															\
        ctrl->SetDirName(*data);								\
        return true;											\
    }															\
    															\
    static bool From(wxDirPickerCtrl* ctrl, wxFileName* data)	\
    {															\
        *data = ctrl->GetDirName();								\
        return true;											\
    }															\
    															\
    static bool To(wxDirPickerCtrl* ctrl, wxString* data)		\
    {															\
        return wxDataTransfer<wxFileDirPickerCtrlBase>::To(ctrl, data); \
    }															\
    															\
    static bool From(wxDirPickerCtrl* ctrl, wxString* data)		\
    {															\
        return wxDataTransfer<wxFileDirPickerCtrlBase>::From(ctrl, data); \
    }															\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_FONTPICKER()				\
template<>													\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxFontPickerCtrl>	\
{															\
    static bool To(wxFontPickerCtrl* ctrl, wxFont* data)	\
    {														\
        ctrl->SetSelectedFont(*data);						\
        return true;										\
    }														\
    														\
    static bool From(wxFontPickerCtrl* ctrl, wxFont* data)	\
    {														\
        *data = ctrl->GetSelectedFont();					\
        return true;										\
    }														\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_GAUGE()					\
template<>												\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxGaugeBase>		\
{														\
    static bool To(wxGaugeBase* ctrl, int* data)		\
    {													\
        ctrl->SetValue(*data);							\
        return true;									\
    }													\
    													\
    static bool From(wxGaugeBase* ctrl, int* data)		\
    {													\
        *data = ctrl->GetValue();						\
        return true;									\
    }													\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_LISTBOX() 						\
template<>														\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxListBoxBase>			\
{																\
    static bool To(wxListBoxBase* ctrl, int* data)				\
    {															\
        wxASSERT( (ctrl->GetWindowStyle() & wxLB_SINGLE) );		\
        														\
        ctrl->SetSelection(*data);								\
        return true;											\
    }															\
    															\
    static bool To(wxListBoxBase* ctrl, wxArrayInt* arr)		\
    {															\
        wxASSERT( (ctrl->GetWindowStyle() & wxLB_MULTIPLE) );	\
        														\
        size_t i, count = ctrl->GetCount();						\
        for ( i = 0 ; i < count; ++i )							\
            ctrl->Deselect(i);									\
        														\
        count = arr->GetCount();								\
        for ( i = 0 ; i < count; ++i )							\
            ctrl->SetSelection(arr->Item(i));					\
        														\
        return true;											\
    }															\
    															\
    static bool From(wxListBoxBase* ctrl, int* data)			\
    {															\
        *data = ctrl->GetSelection();							\
        return true;											\
    }															\
    															\
    static bool From(wxListBoxBase* ctrl, wxArrayInt* arr)		\
    {															\
        arr->Clear();											\
        														\
        for ( size_t i = 0, count = ctrl->GetCount(); i < count; ++i ) \
        {														\
            if ( ctrl->IsSelected(i) )							\
                arr->Add(i);									\
        }														\
 																\
        return true;											\
    }															\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_RADIOBOX()				\
template<>												\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxRadioBoxBase>	\
{														\
    static bool To(wxRadioBoxBase* ctrl, int* data)		\
    {													\
        ctrl->SetSelection(*data);						\
        return true;									\
    }													\
    													\
    static bool From(wxRadioBoxBase* ctrl, int* data)	\
    {													\
        *data = ctrl->GetSelection();					\
        return true;									\
    }													\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_SCROLLBAR()					\
template<>													\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxScrollBarBase>		\
{															\
    static bool To(wxScrollBarBase* ctrl, int* data)		\
    {														\
        ctrl->SetThumbPosition(*data);						\
        return true;										\
    }														\
    														\
    static bool From(wxScrollBarBase* ctrl, int* data)		\
    {														\
        *data = ctrl->GetThumbPosition();					\
        return true;										\
    }														\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_SLIDER()				\
template<>												\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxSliderBase>	\
{														\
    static bool To(wxSliderBase* ctrl, int* data)		\
    {													\
        ctrl->SetValue(*data);							\
        return true;									\
    }													\
    													\
    static bool From(wxSliderBase* ctrl, int* data)		\
    {													\
        *data = ctrl->GetValue();						\
        return true;									\
    }													\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_SPINBUTTON()				\
template<>													\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxSpinButtonBase>	\
{															\
    static bool To(wxSpinButtonBase* ctrl, int* data)		\
    {														\
        ctrl->SetValue(*data);								\
        return true;										\
    }														\
    														\
    static bool From(wxSpinButtonBase* ctrl, int* data)		\
    {														\
        *data = ctrl->GetValue();							\
        return true;										\
    }														\
}

//-----------------------------------------------------------------------------

// if (wxHAS_NATIVE_SPINCTRL)
#define WX_TRANSFER_DATA_FROM_SPINCTRL_true(ctrl, data) (*data = ctrl->GetValue())
// else
#define WX_TRANSFER_DATA_FROM_SPINCTRL_false(ctrl, data) \
			(*data = ctrl->GetValue(wxSPINCTRL_GETVALUE_FIX))

#define WX_TRANSFER_DATA_FROM_SPINCTRL(ctrl, data, is_native) \
		WX_TRANSFER_DATA_FROM_SPINCTRL_ ## is_native (ctrl, data)


#define wxDECLARE_DATA_TRANSFER_SPINCTRL(is_native)	\
template<>											\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxSpinCtrl>	\
{													\
    static bool To(wxSpinCtrl* ctrl, int* data)		\
    {												\
        ctrl->SetValue(*data);						\
        return true;								\
    }												\
    												\
    static bool From(wxSpinCtrl* ctrl, int* data)	\
    {												\
        WX_TRANSFER_DATA_FROM_SPINCTRL(ctrl, data, is_native);	\
        return true;								\
    }												\
}

//-----------------------------------------------------------------------------

// if (wxHAS_NATIVE_SPINCTRLDOUBLE)
#define WX_TRANSFER_DATA_FROM_SPINCTRLDOUBLE_true(ctrl, data) (*data = ctrl->GetValue())
// else
#define WX_TRANSFER_DATA_FROM_SPINCTRLDOUBLE_false(ctrl, data) \
			(*data = ctrl->GetValue(wxSPINCTRL_GETVALUE_FIX))

#define WX_TRANSFER_DATA_FROM_SPINCTRLDOUBLE(ctrl, data, is_native) \
		WX_TRANSFER_DATA_FROM_SPINCTRLDOUBLE_ ## is_native (ctrl, data)


#define wxDECLARE_DATA_TRANSFER_SPINCTRLDOUBLE(is_native)	\
template<>													\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxSpinCtrlDouble>	\
{															\
    static bool To(wxSpinCtrlDouble* ctrl, double* data)	\
    {														\
        ctrl->SetValue(*data);								\
        return true;										\
    }														\
    														\
    static bool From(wxSpinCtrlDouble* ctrl, double* data)	\
    {														\
        WX_TRANSFER_DATA_FROM_SPINCTRLDOUBLE(ctrl, data, is_native); \
        return true;										\
    }														\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_SPLITTERWINDOW() 			\
template<>													\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxSplitterWindow>	\
{															\
    static bool To(wxSplitterWindow* win, int* data)		\
    {														\
        win->SetSashPosition(*data);						\
        return true;										\
    }														\
    														\
    static bool From(wxSplitterWindow* win, int* data)		\
    {														\
        *data = win->GetSashPosition();						\
        return true;										\
    }														\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_STATICTEXT() 					\
template<>														\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxStaticTextBase>		\
{																\
    static bool To(wxStaticTextBase* ctrl, wxString* data)		\
    {															\
        ctrl->SetLabel(*data);									\
        return true;											\
    }															\
    															\
    static bool From(wxStaticTextBase* ctrl, wxString* data)	\
    {															\
        *data = ctrl->GetLabel();								\
        return true;											\
    }															\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_TEXTCTRL() 					\
template<>													\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxTextCtrlBase>		\
{															\
    static bool To(wxTextCtrlBase* ctrl, wxString* data)	\
    {														\
        ctrl->SetValue(*data);								\
        return true;										\
    }														\
    														\
    static bool To(wxTextCtrlBase* ctrl, int* data)			\
    {														\
        const wxString str = wxString::Format("%d", *data);	\
        ctrl->SetValue(str);								\
        return true;										\
    }														\
    														\
    static bool To(wxTextCtrlBase* ctrl, float* data)		\
    {														\
        const wxString str = wxString::Format("%g", *data);	\
        ctrl->SetValue(str);								\
        return true;										\
    }														\
    														\
    static bool To(wxTextCtrlBase* ctrl, double* data)		\
    {														\
        const wxString str = wxString::Format("%g", *data);	\
        ctrl->SetValue(str);								\
        return true;										\
    }														\
    														\
    static bool To(wxTextCtrlBase* ctrl, wxFileName* data)	\
    {														\
        ctrl->SetValue(data->GetFullPath());				\
        return true;										\
    }														\
    														\
    static bool From(wxTextCtrlBase* ctrl, wxString* data)	\
    {														\
        *data = ctrl->GetValue();							\
        return true;										\
    }														\
    														\
    static bool From(wxTextCtrlBase* ctrl, int* data)		\
    {														\
        *data = wxAtoi(ctrl->GetValue());					\
        return true;										\
    }														\
    														\
    static bool From(wxTextCtrlBase* ctrl, float* data)		\
    {														\
        *data = static_cast<float>(wxAtof(ctrl->GetValue()));	\
        return true;										\
    }														\
    														\
    static bool From(wxTextCtrlBase* ctrl, double* data)	\
    {														\
        *data = wxAtof(ctrl->GetValue());					\
        return true;										\
    }														\
    														\
    static bool From(wxTextCtrlBase* ctrl, wxFileName* data)	\
    {														\
        *data = ctrl->GetValue();							\
        return true;										\
    }														\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_TOGGLEBUTTON() 				\
template<>													\
struct WXDLLIMPEXP_CORE wxDataTransfer<wxToggleButtonBase>	\
{															\
    static bool To(wxToggleButtonBase* btn, bool* data)		\
    {														\
        btn->SetValue(*data);								\
        return true;										\
    }														\
    														\
    static bool From(wxToggleButtonBase* btn, bool* data)	\
    {														\
        *data = btn->GetValue();							\
        return true;										\
    }														\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_CALENDAR() 							\
template<>															\
struct WXDLLIMPEXP_ADV wxDataTransfer<wxCalendarCtrlBase>			\
{																	\
    static bool To(wxCalendarCtrlBase* ctrl, wxDateTime* data)		\
    {																\
        return ctrl->SetDate(*data);								\
    }																\
    																\
    static bool From(wxCalendarCtrlBase* ctrl, wxDateTime* data)	\
    {																\
        *data = ctrl->GetDate();									\
        return true;												\
    }																\
}

//-----------------------------------------------------------------------------

#define wxDECLARE_DATA_TRANSFER_DATETIMEPICKER()						\
template<>																\
struct WXDLLIMPEXP_ADV wxDataTransfer<wxDateTimePickerCtrlBase>			\
{																		\
    static bool To(wxDateTimePickerCtrlBase* ctrl, wxDateTime* data)	\
    {																	\
        ctrl->SetValue(*data);											\
        return true;													\
    }																	\
    																	\
    static bool From(wxDateTimePickerCtrlBase* ctrl, wxDateTime* data)	\
    {																	\
        *data = ctrl->GetValue();										\
        return true;													\
    }																	\
}

//-----------------------------------------------------------------------------

#else // !wxUSE_VALIDATORS

#define WX_NO_DATA_TRANSFER ((void)0)


#define wxDECLARE_DATA_TRANSFER_FWD(T, BASE) WX_NO_DATA_TRANSFER


#define wxDECLARE_DATA_TRANSFER_BUTTON() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_CHECKBOX() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_CHECKLISTBOX() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_CHOICE() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_COLLAPSIBLEHEADER() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_COLLAPSIBLEPANE() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_COLOURPICKER() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_COMBOBOX() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_FILEDIRPICKER() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_FILEPICKER() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_DIRPICKER() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_FONTPICKER() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_GAUGE() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_LISTBOX() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_RADIOBOX() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_SCROLLBAR() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_SLIDER() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_SPINBUTTON() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_SPINCTRL(is_native) WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_SPINCTRLDOUBLE(is_native) WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_SPLITTERWINDOW() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_STATICTEXT() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_TEXTCTRL() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_TOGGLEBUTTON() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_CALENDAR() WX_NO_DATA_TRANSFER
#define wxDECLARE_DATA_TRANSFER_DATETIMEPICKER() WX_NO_DATA_TRANSFER

#endif // wxUSE_VALIDATORS

#endif // _WX_DATATRANSFER_H_
