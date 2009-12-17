/////////////////////////////////////////////////////////////////////////////
// Name:        wx/symbian/dcclient.h
// Purpose:     wxClientDC class
// Author:      Jordan Langholz
// Modified by:
// Created:     04/24/07
// RCS-ID:      $Id$
// Copyright:   (c) Jordan Langholz
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCCLIENT_H_
#define _WX_DCCLIENT_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/dc.h"
#include "wx/dynarray.h"

// ----------------------------------------------------------------------------
// array types
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// DC classes
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxWindowDC : public wxDC
{
public:
    // default ctor
    wxWindowDC();

    // Create a DC corresponding to the whole window
    wxWindowDC(wxWindow *win);

protected:
    // initialize the newly created DC
    void InitDC();

    // override some base class virtuals
    virtual void DoGetSize(int *width, int *height) const;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxWindowDC)
};

class WXDLLEXPORT wxClientDC : public wxWindowDC
{
public:
    // default ctor
    wxClientDC();

    // Create a DC corresponding to the client area of the window
    wxClientDC(wxWindow *win);

    virtual ~wxClientDC();

protected:

    // override some base class virtuals
    virtual void DoGetSize(int *width, int *height) const;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxClientDC)
};

class WXDLLEXPORT wxPaintDC : public wxClientDC
{
public:
    wxPaintDC();

    // Create a DC corresponding for painting the window in OnPaint()
    wxPaintDC(wxWindow *win);

    virtual ~wxPaintDC();

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxPaintDC)
};

#endif
    // _WX_DCCLIENT_H_
