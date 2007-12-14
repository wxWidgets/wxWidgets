/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dfb/dcmemory.h
// Purpose:     wxMemoryDC class declaration
// Created:     2006-08-10
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DFB_DCMEMORY_H_
#define _WX_DFB_DCMEMORY_H_

#include "wx/dfb/dc.h"
#include "wx/bitmap.h"

class WXDLLIMPEXP_CORE wxMemoryDCImpl : public wxDFBDCImpl
{
public:
    wxMemoryDCImpl(wxMemoryDC *owner)
        : wxDFBDCImpl(owner)
    {
        Init();
    }

    wxMemoryDCImpl(wxMemoryDC *owner, wxBitmap& bitmap)
        : wxDFBDCImpl(owner)
    {
        Init();
        DoSelect(bitmap);
    }

    wxMemoryDCImpl(wxMemoryDC *owner, wxDC *dc); // create compatible DC

    // override wxMemoryDC-specific base class virtual methods
    virtual const wxBitmap& GetSelectedBitmap() const { return m_bmp; }
    virtual wxBitmap& GetSelectedBitmap() { return m_bmp; }
    virtual void DoSelect(const wxBitmap& bitmap);

private:
    void Init();

    wxBitmap m_bmp;

    DECLARE_DYNAMIC_CLASS(wxMemoryDCImpl)
};

#endif // _WX_DFB_DCMEMORY_H_

