/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.h
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKDCMEMORYH__
#define __GTKDCMEMORYH__

#include "wx/defs.h"
#include "wx/dcclient.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMemoryDC;

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMemoryDC : public wxWindowDC
{
public:
    wxMemoryDC( const wxBitmap& bitmap = wxNullBitmap );
    wxMemoryDC( wxDC *dc ); // Create compatible DC
    virtual ~wxMemoryDC();
    virtual void SelectObject( const wxBitmap& bitmap );

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
    virtual wxBitmap GetSelectedBitmap() const { return m_selected; }        
    wxBitmap  m_selected;

protected:
    void DoGetSize( int *width, int *height ) const;
    virtual wxBitmap DoGetAsBitmap(const wxRect *subrect) const 
    { return subrect == NULL ? GetSelectedBitmap() : GetSelectedBitmap().GetSubBitmap(*subrect); }

    DECLARE_DYNAMIC_CLASS(wxMemoryDC)
};

#endif
    // __GTKDCMEMORYH__

