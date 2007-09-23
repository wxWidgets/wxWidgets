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


#if wxUSE_NEW_DC
class WXDLLIMPEXP_CORE wxGTKMemoryImplDC : public wxGTKWindowImplDC, public wxMemoryImplDCBase
#else
#define wxGTKMemoryImplDC wxMemoryDC
class WXDLLIMPEXP_CORE wxMemoryDC : public wxWindowDC, public wxMemoryDCBase
#endif
{
public:

#if wxUSE_NEW_DC
    wxGTKMemoryImplDC( wxMemoryDC *owner );
    wxGTKMemoryImplDC( wxMemoryDC *owner, wxBitmap& bitmap );
    wxGTKMemoryImplDC( wxMemoryDC *owner, wxDC *dc );
#else
    wxMemoryDC();
    wxMemoryDC(wxBitmap& bitmap);
    wxMemoryDC( wxDC *dc );
#endif
    
    virtual ~wxGTKMemoryImplDC();

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

protected:
    // overridden from wxImplDC
    virtual void DoGetSize( int *width, int *height ) const;
    virtual wxBitmap DoGetAsBitmap(const wxRect *subrect) const;

public:    
    // overridden from wxMemoryImplDCBase
    virtual void DoSelect(const wxBitmap& bitmap);
    virtual const wxBitmap& DoGetSelectedBitmap() const;
    virtual wxBitmap& DoGetSelectedBitmap();

    wxBitmap  m_selected;
    
private:
    void Init();

    DECLARE_ABSTRACT_CLASS(wxGTKMemoryImplDC)
};

#endif // _WX_GTK_DCMEMORY_H_
