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

#include "wx/dc.h"
#include "wx/bitmap.h"

class WXDLLIMPEXP_CORE wxMemoryDC : public wxDC, public wxMemoryDCBase
{
public:
    wxMemoryDC() { Init(); }
    wxMemoryDC(wxBitmap& bitmap) { Init(); SelectObject(bitmap); }
    wxMemoryDC(wxDC *dc); // create compatible DC

    // implementation from now on:

    wxBitmap GetSelectedObject() const { return m_bmp; }

protected:
    virtual void DoSelect(const wxBitmap& bitmap);

private:
    void Init();

    wxBitmap m_bmp;

    DECLARE_DYNAMIC_CLASS(wxMemoryDC)
};

#endif // _WX_DFB_DCMEMORY_H_

