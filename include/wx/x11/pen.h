/////////////////////////////////////////////////////////////////////////////
// Name:        pen.h
// Purpose:     wxPen class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PEN_H_
#define _WX_PEN_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "pen.h"
#endif

#include "wx/gdicmn.h"
#include "wx/gdiobj.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxPen;
class wxColour;
class wxBitmap;

typedef char wxX11Dash;

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

class wxPen: public wxGDIObject
{
public:
    wxPen() { }
    
    wxPen( const wxColour &colour, int width = 1, int style = wxSOLID );
    wxPen( const wxBitmap &stipple, int width );
    ~wxPen();
    
    wxPen( const wxPen& pen ) { Ref(pen); }
    wxPen& operator = ( const wxPen& pen ) { Ref(pen); return *this; }
    
    bool Ok() const { return m_refData != NULL; }
    
    bool operator == ( const wxPen& pen ) const;
    bool operator != (const wxPen& pen) const { return !(*this == pen); }

    void SetColour( const wxColour &colour );
    void SetColour( int red, int green, int blue );
    void SetCap( int capStyle );
    void SetJoin( int joinStyle );
    void SetStyle( int style );
    void SetWidth( int width );
    void SetDashes( int number_of_dashes, const wxDash *dash );
    void SetStipple( wxBitmap *stipple );

    wxColour &GetColour() const;
    int GetCap() const;
    int GetJoin() const;
    int GetStyle() const;
    int GetWidth() const;
    int GetDashes(wxDash **ptr) const;
    int GetDashCount() const;
    wxDash* GetDash() const;
    wxBitmap* GetStipple() const;

private:    
    // ref counting code
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;
    
    DECLARE_DYNAMIC_CLASS(wxPen)
};

#endif
// _WX_PEN_H_
