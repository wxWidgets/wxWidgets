/////////////////////////////////////////////////////////////////////////////
// Name:        brush.h
// Purpose:     wxBrush class
// Author:      Julian Smart, Robert Roebling
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BRUSH_H_
#define _WX_BRUSH_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "brush.h"
#endif

#include "wx/gdiobj.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxBrush;
class wxColour;
class wxBitmap;

//-----------------------------------------------------------------------------
// wxBrush
//-----------------------------------------------------------------------------

class wxBrush: public wxGDIObject
{
public:
    wxBrush() { }

    wxBrush( const wxColour &colour, int style = wxSOLID );
    wxBrush( const wxBitmap &stippleBitmap );
    ~wxBrush();

    wxBrush( const wxBrush &brush ) { Ref(brush); }
    wxBrush& operator = ( const wxBrush& brush ) { Ref(brush); return *this; }

    bool Ok() const { return m_refData != NULL; }

    bool operator == ( const wxBrush& brush ) const;
    bool operator != (const wxBrush& brush) const { return !(*this == brush); }

    int GetStyle() const;
    wxColour &GetColour() const;
    wxBitmap *GetStipple() const;

    bool IsHatch() const
        { return (GetStyle()>=wxBDIAGONAL_HATCH) && (GetStyle()<=wxVERTICAL_HATCH); }

    void SetColour( const wxColour& col );
    void SetColour( unsigned char r, unsigned char g, unsigned char b );
    void SetStyle( int style );
    void SetStipple( const wxBitmap& stipple );

private:
    // ref counting code
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;

    DECLARE_DYNAMIC_CLASS(wxBrush)
};

#endif
// _WX_BRUSH_H_
