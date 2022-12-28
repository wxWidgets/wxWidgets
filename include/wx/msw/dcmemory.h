/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/dcmemory.h
// Purpose:     wxMemoryDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCMEMORY_H_
#define _WX_DCMEMORY_H_

#include "wx/dcmemory.h"
#include "wx/msw/dc.h"

class WXDLLIMPEXP_CORE wxMemoryDCImpl: public wxMSWDCImpl
{
public:
    wxMemoryDCImpl( wxMemoryDC *owner );
    wxMemoryDCImpl( wxMemoryDC *owner, wxBitmap& bitmap );
    wxMemoryDCImpl( wxMemoryDC *owner, wxDC *dc ); // Create compatible DC

    // override some base class virtuals
    virtual wxSize GetPPI() const override;
    virtual void SetFont(const wxFont& font) override;

    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height) override;
    virtual void DoGetSize(int* width, int* height) const override;
    virtual void DoSelect(const wxBitmap& bitmap) override;

    virtual wxBitmap DoGetAsBitmap(const wxRect* subrect) const override
    { return subrect == nullptr ? GetSelectedBitmap() : GetSelectedBitmap().GetSubBitmapOfHDC(*subrect, GetHDC() );}

protected:
    // create DC compatible with the given one or screen if dc == nullptr
    bool CreateCompatible(wxDC *dc);

    // initialize the newly created DC
    void Init();

    wxDECLARE_CLASS(wxMemoryDCImpl);
    wxDECLARE_NO_COPY_CLASS(wxMemoryDCImpl);
};

#endif
    // _WX_DCMEMORY_H_
