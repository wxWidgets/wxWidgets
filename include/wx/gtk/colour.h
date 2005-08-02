/////////////////////////////////////////////////////////////////////////////
// Name:        colour.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKCOLOURH__
#define __GTKCOLOURH__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdiobj.h"
#include "wx/palette.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDC;
class WXDLLIMPEXP_CORE wxPaintDC;
class WXDLLIMPEXP_CORE wxBitmap;
class WXDLLIMPEXP_CORE wxWindow;

class WXDLLIMPEXP_CORE wxColour;

//-----------------------------------------------------------------------------
// wxColour
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxColour: public wxGDIObject
{
public:
    wxColour() { }

    // Construct from RGB
    wxColour( unsigned char red, unsigned char green, unsigned char blue );
    wxColour( unsigned long colRGB ) { Set(colRGB); }

    // Implicit conversion from the colour name
    wxColour( const wxString &colourName ) { InitFromName(colourName); }
    wxColour( const char *colourName ) { InitFromName( wxString::FromAscii(colourName) ); }
#if wxUSE_UNICODE
    wxColour( const wxChar *colourName ) { InitFromName( wxString(colourName) ); }
#endif


    wxColour( const wxColour& col )
        : wxGDIObject()
        { Ref(col); }
    wxColour& operator = ( const wxColour& col ) { Ref(col); return *this; }

    ~wxColour();

    bool Ok() const { return m_refData != NULL; }

    bool operator == ( const wxColour& col ) const;
    bool operator != ( const wxColour& col ) const { return !(*this == col); }

    void Set( unsigned char red, unsigned char green, unsigned char blue );
    void Set( unsigned long colRGB )
    {
        // We don't need to know sizeof(long) here because we assume that the three
        // least significant bytes contain the R, G and B values
        Set((unsigned char)colRGB,
            (unsigned char)(colRGB >> 8),
            (unsigned char)(colRGB >> 16));
    }

    unsigned char Red() const;
    unsigned char Green() const;
    unsigned char Blue() const;


    // Get colour from name or wxNullColour
    static wxColour CreateByName(const wxString& name);

    // Implementation part
    void CalcPixel( GdkColormap *cmap );
    int GetPixel() const;
    GdkColor *GetColor() const;

    void InitFromName(const wxString& colourName);

protected:
    // ref counting code
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;


private:
    DECLARE_DYNAMIC_CLASS(wxColour)
};

#endif // __GTKCOLOURH__
