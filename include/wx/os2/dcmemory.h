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

class WXDLLEXPORT wxMemoryDC: public wxDC
{
    DECLARE_DYNAMIC_CLASS(wxMemoryDC)

public:
    wxMemoryDC(void);
    wxMemoryDC(wxDC* pDC); // Create compatible DC

    ~wxMemoryDC(void);
    virtual void SelectObject(const wxBitmap& rBitmap);
    virtual void DoGetSize( int* pWidth
                           ,int* pHeight
                          ) const;
}; // end of CLASS wxMemoryDC

#endif
    // _WX_DCMEMORY_H_
