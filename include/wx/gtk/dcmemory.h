/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.h
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_DCMEMORY_H_
#define _WX_GTK_DCMEMORY_H_

#include "wx/dcclient.h"

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMemoryDC : public wxWindowDC, public wxMemoryDCBase
{
public:
    wxMemoryDC() : wxWindowDC() { Init(); }
    wxMemoryDC(wxBitmap& bitmap) : wxWindowDC() { Init(); SelectObject(bitmap); }
    wxMemoryDC( wxDC *dc ); // Create compatible DC
    virtual ~wxMemoryDC();

    // these get reimplemented for mono-bitmaps to behave
    // more like their Win32 couterparts. They now interpret
    // wxWHITE, wxWHITE_BRUSH and wxWHITE_PEN as drawing 0
    // and everything else as drawing 1.
    virtual void SetPen( const wxPen &pen );
    virtual void SetBrush( const wxBrush &brush );
    virtual void SetBackground( const wxBrush &brush );
    virtual void SetTextForeground( const wxColour &col );
    virtual void SetTextBackground( const wxColour &col );

    // implementation
    wxBitmap GetSelectedBitmap() const { return m_selected; }
    wxBitmap  m_selected;

protected:
    void DoGetSize( int *width, int *height ) const;
    virtual void DoSelect(const wxBitmap& bitmap);
    virtual wxBitmap DoGetAsBitmap(const wxRect *subrect) const
    {
        wxBitmap bmp = GetSelectedBitmap();
        return subrect ? bmp.GetSubBitmap(*subrect) : bmp;
    }

private:
    void Init();

    DECLARE_DYNAMIC_CLASS(wxMemoryDC)
};

#endif // _WX_GTK_DCMEMORY_H_
