/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/dcmemory.h
// Purpose:     wxMemoryDC class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCMEMORY_H_
#define _WX_DCMEMORY_H_

#include "wx/dcclient.h"

class WXDLLEXPORT wxMemoryDC : public wxDC
{
public:
    wxMemoryDC( const wxBitmap& bitmap = wxNullBitmap );
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
