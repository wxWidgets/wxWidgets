/////////////////////////////////////////////////////////////////////////////
// Name:        pen.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKPENH__
#define __GTKPENH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxPen;

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

class wxPen: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxPen)

  public:
  
    wxPen(void);
    wxPen( const wxColour &colour, int width, int style );
    wxPen( const wxPen& pen );
    wxPen( const wxPen* pen );
    ~wxPen(void);
    wxPen& operator = ( const wxPen& pen );
    bool operator == ( const wxPen& pen );
    bool operator != ( const wxPen& pen );
    
    void SetColour( const wxColour &colour );
    void SetColour( int red, int green, int blue );
    void SetCap( int capStyle );
    void SetJoin( int joinStyle );
    void SetStyle( int style );
    void SetWidth( int width );
    wxColour &GetColour(void) const;
    int GetCap(void) const;
    int GetJoin(void) const;
    int GetStyle(void) const;
    int GetWidth(void) const;
    bool Ok(void) const;
    
    void Unshare(void);
    
    // no data :-)
};

#endif // __GTKPENH__
