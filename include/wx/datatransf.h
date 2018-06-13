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

// wxWindow-derived classes (W) need not to specialize wxDataTransfer<> if
// a base already has one and the derived just want to forward to its base
// implementation. if so, all you have to do is specialize this class.
// e.g.:
//
// template<>
// struct wxFwdDataTransfer<MyWindow>
// {
//     typedef MyWindowBase Base;
// };
//

template<class W>
struct wxFwdDataTransfer
{
    typedef void Base;
};

#define WX_FWD_DATA_TRANSFER(T, BASE) \
template<> \
struct wxFwdDataTransfer<T> \
{ \
    typedef BASE Base; \
}


// wxDataTransfer implements the actual data transfer.
//
// How it works:
//--------------
//  say you have a window 'MyWindow' which can transfer wxStrings and CustomData.
//  then wxDataTransfer<> specialization for MyWindow would look like this:
//
// template<>
// struct wxDataTransfer<MyWindow>
// {
//     static bool To(wxWindow* win, wxStrings* data)
//     { /*implementation*/ }
//     static bool From(wxWindow* win, wxStrings* data)
//     { /*implementation*/ }
//
//     static bool To(wxWindow* win, CustomData* data)
//     { /*implementation*/ }
//     static bool From(wxWindow* win, CustomData* data)
//     { /*implementation*/ }
// };
//

template<class W>
struct wxDataTransfer
{
    static bool To(wxWindow*, void*);
    static bool From(wxWindow*, void*);
};

#endif // wxUSE_VALIDATORS

#endif // _WX_DATATRANSFER_H_
