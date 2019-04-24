/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/datatransfer.h
// Purpose:     wxValidatorDataTransferImpl<> declarations
// Author:      Ali Kettab
// Created:     2018-06-13
// Copyright:   (c) 2018 Ali Kettab
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_VALIDATOR_DATATRANSFER_H_
#define _WX_PRIVATE_VALIDATOR_DATATRANSFER_H_

#include "wx/defs.h"

#ifndef HAVE_VARIADIC_MACROS
  #error "Variadic macros support required."
#endif // HAVE_VARIADIC_MACROS

#undef wxUSE_VALIDATOR_DATATRANSFER

#if !defined(HAVE_DECLTYPE) || (!defined(HAVE_TYPE_TRAITS) && \
                                !defined(HAVE_TR1_TYPE_TRAITS))
    #define wxUSE_VALIDATOR_DATATRANSFER 0
#else
    #define wxUSE_VALIDATOR_DATATRANSFER wxUSE_ANY
#endif

class WXDLLIMPEXP_FWD_BASE wxFileName;
class WXDLLIMPEXP_FWD_BASE wxDateTime;

//-----------------------------------------------------------------------------
//
// wxGenValidator[Simple|Composit]Type<> just delegate to wxValidatorDataTransferImpl<>
// to do the actual data transfer (via template specializations)
//
// How it works:
// -------------
// say you have a window 'MyWindow' which can transfer wxStrings and CustomData.
// the wxValidatorDataTransferImpl<> specialization for MyWindow would look like this:
//
// template<>
// struct wxValidatorDataTransferImpl<MyWindow>
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
struct wxValidatorDataTransferImpl;

//-----------------------------------------------------------------------------
// wxValidatorDataTransferImplementor() is/are just declaration(s) to help
// wxValidatorDataTransfer<W> figure out if the window W has any data transfer
// implementor or not
//-----------------------------------------------------------------------------

extern void* wxValidatorDataTransferImplementor(...);

#define wx_DECLARE_VALIDATOR_DATATRANSFER_IMPLEMENTOR(window)  \
  extern window* wxValidatorDataTransferImplementor(window*)

#define wxGET_VALIDATOR_DATATRANSFER_IMPLEMENTOR(window) \
  wxValidatorDataTransferImplementor(static_cast<window*>(NULL))

// Actually the same technique used to implement wxCALL_FOR_EACH (see wx/cpp.h)
// is used here to implement wxDECLARE_VALIDATOR_DATATRANSFER_IMPL, which makes
// it easy for us to declare specializations of wxValidatorDataTransferImpl<>
// in a clean way:
// e.g. wxDECLARE_VALIDATOR_DATATRANSFER_IMPL(MyWindow, Type1, Type2, ..., TypeN);
//
// Up to eight arguments (types) are supported.


#define wx_VALIDATOR_DATATRANSFER_IMPL_NARG(...) \
  wx_VALIDATOR_DATATRANSFER_IMPL_NARG_((__VA_ARGS__, wx_VALIDATOR_DATATRANSFER_IMPL_RSEQ_N()))
#define wx_VALIDATOR_DATATRANSFER_IMPL_NARG_(args) wx_VALIDATOR_DATATRANSFER_IMPL_ARG_N args
#define wx_VALIDATOR_DATATRANSFER_IMPL_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define wx_VALIDATOR_DATATRANSFER_IMPL_RSEQ_N() 8, 7, 6, 5, 4, 3, 2, 1, 0

#define wx_VALIDATOR_DATATRANSFER_IMPL_1_(args)   wx_VALIDATOR_DATATRANSFER_IMPL_1 args
#define wx_VALIDATOR_DATATRANSFER_IMPL_2_(args)   wx_VALIDATOR_DATATRANSFER_IMPL_2 args
#define wx_VALIDATOR_DATATRANSFER_IMPL_3_(args)   wx_VALIDATOR_DATATRANSFER_IMPL_3 args
#define wx_VALIDATOR_DATATRANSFER_IMPL_4_(args)   wx_VALIDATOR_DATATRANSFER_IMPL_4 args
#define wx_VALIDATOR_DATATRANSFER_IMPL_5_(args)   wx_VALIDATOR_DATATRANSFER_IMPL_5 args
#define wx_VALIDATOR_DATATRANSFER_IMPL_6_(args)   wx_VALIDATOR_DATATRANSFER_IMPL_6 args
#define wx_VALIDATOR_DATATRANSFER_IMPL_7_(args)   wx_VALIDATOR_DATATRANSFER_IMPL_7 args
#define wx_VALIDATOR_DATATRANSFER_IMPL_8_(args)   wx_VALIDATOR_DATATRANSFER_IMPL_8 args

#define wx_VALIDATOR_DATATRANSFER_IMPL_1(window, type)    \
    static bool To(window*, type*);                       \
    static bool From(window*, type*)

#define wx_VALIDATOR_DATATRANSFER_IMPL_2(window, type, ...)  \
    wx_VALIDATOR_DATATRANSFER_IMPL_1(window, type);          \
    wx_VALIDATOR_DATATRANSFER_IMPL_1_((window, __VA_ARGS__))

#define wx_VALIDATOR_DATATRANSFER_IMPL_3(window, type, ...)  \
    wx_VALIDATOR_DATATRANSFER_IMPL_1(window, type);          \
    wx_VALIDATOR_DATATRANSFER_IMPL_2_((window, __VA_ARGS__))

#define wx_VALIDATOR_DATATRANSFER_IMPL_4(window, type, ...)  \
    wx_VALIDATOR_DATATRANSFER_IMPL_1(window, type);          \
    wx_VALIDATOR_DATATRANSFER_IMPL_3_((window, __VA_ARGS__))

#define wx_VALIDATOR_DATATRANSFER_IMPL_5(window, type, ...)  \
    wx_VALIDATOR_DATATRANSFER_IMPL_1(window, type);          \
    wx_VALIDATOR_DATATRANSFER_IMPL_4_((window, __VA_ARGS__))

#define wx_VALIDATOR_DATATRANSFER_IMPL_6(window, type, ...)  \
    wx_VALIDATOR_DATATRANSFER_IMPL_1(window, type);          \
    wx_VALIDATOR_DATATRANSFER_IMPL_5_((window, __VA_ARGS__))

#define wx_VALIDATOR_DATATRANSFER_IMPL_7(window, type, ...)  \
    wx_VALIDATOR_DATATRANSFER_IMPL_1(window, type);          \
    wx_VALIDATOR_DATATRANSFER_IMPL_6_((window, __VA_ARGS__))

#define wx_VALIDATOR_DATATRANSFER_IMPL_8(window, type, ...)  \
    wx_VALIDATOR_DATATRANSFER_IMPL_1(window, type);          \
    wx_VALIDATOR_DATATRANSFER_IMPL_7_((window, __VA_ARGS__))

#define wx_VALIDATOR_DATATRANSFER_IMPL_(N, args) \
    wxCONCAT(wx_VALIDATOR_DATATRANSFER_IMPL_, N) args

#define wx_VALIDATOR_DATATRANSFER_IMPL(window, ...) \
    wx_VALIDATOR_DATATRANSFER_IMPL_(\
        wx_VALIDATOR_DATATRANSFER_IMPL_NARG(__VA_ARGS__), (window, __VA_ARGS__))

// This macro is for wxWidgets internal use only.
#define wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(window, ...)                    \
    class WXDLLIMPEXP_FWD_CORE window;                                         \
    wx_DECLARE_VALIDATOR_DATATRANSFER_IMPLEMENTOR(window);                     \
    template<>                                                                 \
    struct WXDLLIMPEXP_CORE wxValidatorDataTransferImpl<window>                \
    {                                                                          \
        wx_VALIDATOR_DATATRANSFER_IMPL(window, __VA_ARGS__);                   \
    }

// This macro is intended for client code usage.
#define wxDECLARE_VALIDATOR_DATATRANSFER_IMPL(window, ...)                     \
    wx_DECLARE_VALIDATOR_DATATRANSFER_IMPLEMENTOR(window);                     \
    template<>                                                                 \
    struct wxValidatorDataTransferImpl<window>                                 \
    {                                                                          \
        wx_VALIDATOR_DATATRANSFER_IMPL(window, __VA_ARGS__);                   \
    }

//=============================================================================
// declarations only from now on
//=============================================================================

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxButtonBase, wxString);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxCheckBoxBase, bool);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxCheckListBoxBase, wxArrayInt);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxChoiceBase, int, wxString);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxCollapsibleHeaderCtrlBase, bool);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxCollapsiblePaneBase, bool);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxColourPickerCtrl, wxColour);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxComboBox, int, wxString);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxFileDirPickerCtrlBase, wxString);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxFilePickerCtrl, wxFileName, wxString);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxDirPickerCtrl, wxFileName, wxString);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxFontPickerCtrl, wxFont);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxGaugeBase, int);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxListBoxBase, int, wxArrayInt);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxRadioBoxBase, int);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxScrollBarBase, int);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxSliderBase, int);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxSpinButtonBase, int);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxSpinCtrl, int);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxSpinCtrlDouble, double);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxSplitterWindow, int);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxStaticTextBase, wxString);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxTextCtrlBase, int, float, double, wxString, wxFileName);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxToggleButtonBase, bool);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxCalendarCtrlBase, wxDateTime);

//-----------------------------------------------------------------------------

wxDECLARE_VALIDATOR_DATATRANSFER_IMPL_(wxDateTimePickerCtrlBase, wxDateTime);

//-----------------------------------------------------------------------------

#endif // _WX_PRIVATE_VALIDATOR_DATATRANSFER_H_
