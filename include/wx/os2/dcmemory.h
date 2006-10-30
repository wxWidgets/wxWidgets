/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.h
// Purpose:     wxMemoryDC class
// Author:      David Webster
// Modified by:
// Created:     09/09/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCMEMORY_H_
#define _WX_DCMEMORY_H_

#include "wx/dcclient.h"

class WXDLLEXPORT wxMemoryDC: public wxDC, public wxMemoryDCBase
{
public:
    wxMemoryDC() { CreateCompatible(NULL); Init(); }
    wxMemoryDC(wxBitmap& bitmap) { CreateCompatible(NULL); Init(); SelectObject(bitmap); }
    wxMemoryDC(wxDC* pDC); // Create compatible DC

protected:
    // override some base class virtuals
    virtual void DoGetSize( int* pWidth
                           ,int* pHeight
                          ) const;
    virtual void DoSelect(const wxBitmap& bitmap);

    // create DC compatible with the given one or screen if dc == NULL
    bool CreateCompatible(wxDC* pDC);

    // initialize the newly created DC
    void Init(void);
private:
    DECLARE_DYNAMIC_CLASS(wxMemoryDC)
}; // end of CLASS wxMemoryDC

#endif
    // _WX_DCMEMORY_H_
