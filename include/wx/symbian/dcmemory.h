/////////////////////////////////////////////////////////////////////////////
// Name:        wx/symbian/dcmemory.h
// Purpose:     wxMemoryDC class
// Author:      Jordan Langholz
// Modified by:
// Created:     04/24/07
// RCS-ID:      $Id: dcmemory.h,v 1.2 2005/01/18 21:14:24 ABX Exp $
// Copyright:   (c) Jordan Langholz
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCMEMORY_H_
#define _WX_DCMEMORY_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "dcmemory.h"
#endif

#include "wx/dcclient.h"

class WXDLLEXPORT wxMemoryDC : public wxDC
{
public:
    wxMemoryDC();
    wxMemoryDC(wxDC *dc); // Create compatible DC

    virtual void SelectObject(const wxBitmap& bitmap);

protected:
    // override some base class virtuals
    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    virtual void DoGetSize(int* width, int* height) const;

    // create DC compatible with the given one or screen if dc == NULL
    bool CreateCompatible(wxDC *dc);

    // initialize the newly created DC
    void Init();

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMemoryDC)
};

#endif
    // _WX_DCMEMORY_H_
