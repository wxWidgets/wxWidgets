/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk1/colour.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKCOLOURH__
#define __GTKCOLOURH__

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

class WXDLLIMPEXP_CORE wxColour : public wxColourBase
{
public:
    // constructors
    // ------------

    // default
    wxColour() { }
    DEFINE_STD_WXCOLOUR_CONSTRUCTORS

    virtual ~wxColour();

    virtual bool FromString(const wxChar *str);

    bool Ok() const { return m_refData != NULL; }

    bool operator == ( const wxColour& col ) const;
    bool operator != ( const wxColour& col ) const { return !(*this == col); }

    unsigned char Red() const;
    unsigned char Green() const;
    unsigned char Blue() const;


    // Implementation part
    void CalcPixel( GdkColormap *cmap );
    int GetPixel() const;
    GdkColor *GetColor() const;

protected:
    // ref counting code
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;

    virtual void InitWith( unsigned char red, unsigned char green, unsigned char blue );

private:
    DECLARE_DYNAMIC_CLASS(wxColour)
};

#endif // __GTKCOLOURH__
