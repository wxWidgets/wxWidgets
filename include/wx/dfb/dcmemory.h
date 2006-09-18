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

class WXDLLIMPEXP_CORE wxMemoryDC : public wxDC
{
public:
    wxMemoryDC();
    wxMemoryDC(wxDC *dc); // create compatible DC

    virtual void SelectObject(const wxBitmap& bitmap);

    // implementation from now on:

    wxBitmap GetSelectedObject() const { return m_bmp; }

private:
    wxBitmap m_bmp;

    DECLARE_DYNAMIC_CLASS(wxMemoryDC)
};

#endif // _WX_DFB_DCMEMORY_H_

