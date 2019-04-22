/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/datatransfer.h
// Purpose:     wxDataTransferImpl<> declarations
// Author:      Ali Kettab
// Created:     2018-06-13
// Copyright:   (c) 2018 Ali Kettab
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_DATATRANSFER_H_
#define _WX_PRIVATE_DATATRANSFER_H_

#include "wx/defs.h"

#ifndef HAVE_VARIADIC_MACROS
  #error "Variadic macros support required."
#endif // HAVE_VARIADIC_MACROS

#undef wxUSE_DATATRANSFER

#if !defined(HAVE_DECLTYPE) || (!defined(HAVE_TYPE_TRAITS) && \
                                !defined(HAVE_TR1_TYPE_TRAITS))
    #define wxUSE_DATATRANSFER 0
#else
    #define wxUSE_DATATRANSFER wxUSE_ANY
#endif

class WXDLLIMPEXP_FWD_BASE wxFileName;
class WXDLLIMPEXP_FWD_BASE wxDateTime;

//-----------------------------------------------------------------------------
//
// wxGenericValidator[Simple|Composit]Type<> just delegate to wxDataTransferImpl<>
// to do the actual data transfer (via template specializations)
//
// How it works:
// -------------
// say you have a window 'MyWindow' which can transfer wxStrings and CustomData.
// the wxDataTransferImpl<> specialization for MyWindow would look like this:
//
// template<>
// struct wxDataTransferImpl<MyWindow>
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
struct wxDataTransferImpl;

//-----------------------------------------------------------------------------
// wxDataTransferImplementor() is/are just declaration(s) to help wxDataTransfer<W>
// figure out if the window W has any data transfer implementor or not
//-----------------------------------------------------------------------------

extern void* wxDataTransferImplementor(...);

#define wx_DECLARE_DATA_TRANSFER_IMPLEMENTOR(window)  \
  extern window* wxDataTransferImplementor(window*)

#define wxDATA_TRANSFER_IMPLEMENTOR(window) \
  wxDataTransferImplementor(static_cast<window*>(NULL))

// actually the same technique used to implement wxCALL_FOR_EACH (see wx/cpp.h)
// is used here to implement wxDECLARE_DATA_TRANSFER_IMPL, which makes it easy 
// for us to declare specializations of wxDataTransferImpl<> in a clean way:
// e.g. wxDECLARE_DATA_TRANSFER_IMPL(MyWindow, Type1, Type2, ..., TypeN);
//
// Up to eight arguments (types) are supported.


#define wx_DATA_TRANSFER_IMPL_NARG(...) \
  wx_DATA_TRANSFER_IMPL_NARG_((__VA_ARGS__, wx_DATA_TRANSFER_IMPL_RSEQ_N()))
#define wx_DATA_TRANSFER_IMPL_NARG_(args) wx_DATA_TRANSFER_IMPL_ARG_N args
#define wx_DATA_TRANSFER_IMPL_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define wx_DATA_TRANSFER_IMPL_RSEQ_N() 8, 7, 6, 5, 4, 3, 2, 1, 0

#define wx_DATA_TRANSFER_IMPL_1_(args)   wx_DATA_TRANSFER_IMPL_1 args
#define wx_DATA_TRANSFER_IMPL_2_(args)   wx_DATA_TRANSFER_IMPL_2 args
#define wx_DATA_TRANSFER_IMPL_3_(args)   wx_DATA_TRANSFER_IMPL_3 args
#define wx_DATA_TRANSFER_IMPL_4_(args)   wx_DATA_TRANSFER_IMPL_4 args
#define wx_DATA_TRANSFER_IMPL_5_(args)   wx_DATA_TRANSFER_IMPL_5 args
#define wx_DATA_TRANSFER_IMPL_6_(args)   wx_DATA_TRANSFER_IMPL_6 args
#define wx_DATA_TRANSFER_IMPL_7_(args)   wx_DATA_TRANSFER_IMPL_7 args
#define wx_DATA_TRANSFER_IMPL_8_(args)   wx_DATA_TRANSFER_IMPL_8 args

#define wx_DATA_TRANSFER_IMPL_1(window, type)    \
    static bool To(window*, type*);              \
    static bool From(window*, type*)

#define wx_DATA_TRANSFER_IMPL_2(window, type, ...)  \
    wx_DATA_TRANSFER_IMPL_1(window, type);          \
    wx_DATA_TRANSFER_IMPL_1_((window, __VA_ARGS__))

#define wx_DATA_TRANSFER_IMPL_3(window, type, ...)  \
    wx_DATA_TRANSFER_IMPL_1(window, type);          \
    wx_DATA_TRANSFER_IMPL_2_((window, __VA_ARGS__))

#define wx_DATA_TRANSFER_IMPL_4(window, type, ...)  \
    wx_DATA_TRANSFER_IMPL_1(window, type);          \
    wx_DATA_TRANSFER_IMPL_3_((window, __VA_ARGS__))

#define wx_DATA_TRANSFER_IMPL_5(window, type, ...)  \
    wx_DATA_TRANSFER_IMPL_1(window, type);          \
    wx_DATA_TRANSFER_IMPL_4_((window, __VA_ARGS__))

#define wx_DATA_TRANSFER_IMPL_6(window, type, ...)  \
    wx_DATA_TRANSFER_IMPL_1(window, type);          \
    wx_DATA_TRANSFER_IMPL_5_((window, __VA_ARGS__))

#define wx_DATA_TRANSFER_IMPL_7(window, type, ...)  \
    wx_DATA_TRANSFER_IMPL_1(window, type);          \
    wx_DATA_TRANSFER_IMPL_6_((window, __VA_ARGS__))

#define wx_DATA_TRANSFER_IMPL_8(window, type, ...)  \
    wx_DATA_TRANSFER_IMPL_1(window, type);          \
    wx_DATA_TRANSFER_IMPL_7_((window, __VA_ARGS__))

#define wx_DATA_TRANSFER_IMPL_(N, args) \
    wxCONCAT(wx_DATA_TRANSFER_IMPL_, N) args

#define wx_DATA_TRANSFER_IMPL(window, ...) \
    wx_DATA_TRANSFER_IMPL_(wx_DATA_TRANSFER_IMPL_NARG(__VA_ARGS__), (window, __VA_ARGS__))

// This macro is for wxWidgets internal use only.
#define wxDECLARE_DATA_TRANSFER_IMPL_(window, ...)                             \
    class WXDLLIMPEXP_FWD_CORE window;                                         \
    wx_DECLARE_DATA_TRANSFER_IMPLEMENTOR(window);                              \
    template<>                                                                 \
    struct WXDLLIMPEXP_CORE wxDataTransferImpl<window>                         \
    {                                                                          \
        wx_DATA_TRANSFER_IMPL(window, __VA_ARGS__);                            \
    }

// This macro is intended for client code usage.
#define wxDECLARE_DATA_TRANSFER_IMPL(window, ...)                              \
    wx_DECLARE_DATA_TRANSFER_IMPLEMENTOR(window);                              \
    template<>                                                                 \
    struct wxDataTransferImpl<window>                                          \
    {                                                                          \
        wx_DATA_TRANSFER_IMPL(window, __VA_ARGS__);                            \
    }

//=============================================================================
// declarations only from now on
//=============================================================================

wxDECLARE_DATA_TRANSFER_IMPL_(wxButtonBase, wxString);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxCheckBoxBase, bool);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxCheckListBoxBase, wxArrayInt);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxChoiceBase, int, wxString);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxCollapsibleHeaderCtrlBase, bool);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxCollapsiblePaneBase, bool);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxColourPickerCtrl, wxColour);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxComboBox, int, wxString);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxFileDirPickerCtrlBase, wxString);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxFilePickerCtrl, wxFileName, wxString);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxDirPickerCtrl, wxFileName, wxString);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxFontPickerCtrl, wxFont);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxGaugeBase, int);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxListBoxBase, int, wxArrayInt);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxRadioBoxBase, int);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxScrollBarBase, int);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxSliderBase, int);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxSpinButtonBase, int);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxSpinCtrl, int);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxSpinCtrlDouble, double);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxSplitterWindow, int);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxStaticTextBase, wxString);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxTextCtrlBase, int, float, double, wxString, wxFileName);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxToggleButtonBase, bool);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxCalendarCtrlBase, wxDateTime);

//-----------------------------------------------------------------------------

wxDECLARE_DATA_TRANSFER_IMPL_(wxDateTimePickerCtrlBase, wxDateTime);

//-----------------------------------------------------------------------------

#endif // _WX_PRIVATE_DATATRANSFER_H_
