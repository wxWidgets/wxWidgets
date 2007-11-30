/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/dcmemory.h
// Purpose:     wxMemoryDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCMEMORY_H_
#define _WX_DCMEMORY_H_

#include "wx/dcmemory.h"
#include "wx/msw/dc.h"

class WXDLLEXPORT wxMemoryDCImpl: public wxMSWDCImpl
{
public:
    wxMemoryDCImpl( wxMemoryDC *owner );
    wxMemoryDCImpl( wxMemoryDC *owner, wxBitmap& bitmap );
    wxMemoryDCImpl( wxMemoryDC *owner, wxDC *dc ); // Create compatible DC


protected:
    // override some base class virtuals
    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    virtual void DoGetSize(int* width, int* height) const;
    virtual void DoSelect(const wxBitmap& bitmap);

    virtual wxBitmap DoGetAsBitmap(const wxRect* subrect) const 
    { return subrect == NULL ? GetSelectedBitmap() : GetSelectedBitmap().GetSubBitmapOfHDC(*subrect, GetHDC() );}

    // create DC compatible with the given one or screen if dc == NULL
    bool CreateCompatible(wxDC *dc);

    // initialize the newly created DC
    void Init();

private:
    DECLARE_CLASS(wxMemoryDCImpl)
    DECLARE_NO_COPY_CLASS(wxMemoryDCImpl)
};

#endif
    // _WX_DCMEMORY_H_
