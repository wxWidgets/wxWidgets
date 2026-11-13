/////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/dcmemory.h
// Purpose:     wxMemoryDCImpl class
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_DCMEMORY_H_
#define _WX_RETRO68_DCMEMORY_H_

#include "wx/retro68/dc.h"
#include "wx/bitmap.h"

class WXDLLIMPEXP_CORE wxMemoryDCImpl : public wxRetro68DCImpl
{
public:
    explicit wxMemoryDCImpl(wxMemoryDC *owner);
    wxMemoryDCImpl(wxMemoryDC *owner, wxBitmap& bitmap);
    wxMemoryDCImpl(wxMemoryDC *owner, wxDC *dc); // create compatible DC

    virtual ~wxMemoryDCImpl();

    // override wxMemoryDC-specific base class virtual methods
    virtual const wxBitmap& GetSelectedBitmap() const override
        { return m_bmp; }
    virtual wxBitmap& GetSelectedBitmap() override
        { return m_bmp; }

    virtual void DoSelect(const wxBitmap& bitmap) override;

private:
    void Init();

    // Release the currently selected bitmap, if any.
    void Deselect();

    wxBitmap m_bmp;

    wxDECLARE_ABSTRACT_CLASS(wxMemoryDCImpl);
};

#endif // _WX_RETRO68_DCMEMORY_H_
