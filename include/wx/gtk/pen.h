/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/pen.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKPENH__
#define __GTKPENH__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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

class WXDLLIMPEXP_CORE wxPen;

#if defined(__WXGTK127__) || defined(__WXGTK20__)
typedef    gint8 wxGTKDash;
#else
typedef    gchar wxGTKDash;
#endif

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPen: public wxGDIObject
{
public:
    wxPen() { }
    
    wxPen( const wxColour &colour, int width = 1, int style = wxSOLID );
    ~wxPen();
    
    wxPen( const wxPen& pen )
        : wxGDIObject()
        { Ref(pen); }
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
    
    wxColour &GetColour() const;
    int GetCap() const;
    int GetJoin() const;
    int GetStyle() const;
    int GetWidth() const;
    int GetDashes(wxDash **ptr) const;
    int GetDashCount() const;
    wxDash* GetDash() const;

private:    
    // ref counting code
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;
    
    DECLARE_DYNAMIC_CLASS(wxPen)
};

#endif // __GTKPENH__
