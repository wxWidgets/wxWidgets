/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.h
// Purpose:
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __WX_DCMEMORY_H__
#define __WX_DCMEMORY_H__

#ifdef __GNUG__
#pragma interface "dcmemory.h"
#endif

#include "wx/defs.h"
#include "wx/dcclient.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxMemoryDC;

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxMemoryDC : public wxDC
{
public:
    wxMemoryDC();
    wxMemoryDC(wxDC *dc); // Create compatible DC
    ~wxMemoryDC();
    virtual void SelectObject(const wxBitmap& bitmap);

    // these get reimplemented for mono-bitmaps to behave
    // more like their Win32 couterparts. They now interpret
    // wxWHITE, wxWHITE_BRUSH and wxWHITE_PEN as drawing 0
    // and everything else as drawing 1.
    virtual void SetPen(const wxPen &pen);
    virtual void SetBrush(const wxBrush &brush);
    virtual void SetTextForeground(const wxColour &col);
    virtual void SetTextBackground(const wxColour &col);

    // implementation
    wxBitmap  m_selected;
    
    wxBitmap GetSelectedObject() const { return m_selected; }

private:
    DECLARE_DYNAMIC_CLASS(wxMemoryDC)
};

#endif
    // __WX_DCMEMORY_H__

