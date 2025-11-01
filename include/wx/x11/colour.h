/////////////////////////////////////////////////////////////////////////////
// Name:        wx/x11/colour.h
// Purpose:     wxColourImpl class
// Author:      Julian Smart, Robert Roebling
// Created:     17/09/98
// Copyright:   (c) Julian Smart, Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLOUR_H_
#define _WX_COLOUR_H_

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdiobj.h"
#include "wx/palette.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxDC;
class WXDLLIMPEXP_FWD_CORE wxPaintDC;
class WXDLLIMPEXP_FWD_CORE wxBitmap;
class WXDLLIMPEXP_FWD_CORE wxWindow;

//-----------------------------------------------------------------------------
// wxColourImpl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxColourImpl : public wxColourBase
{
public:
    // constructors
    // ------------
    wxColourImpl() = default;

    bool operator==(const wxColourImpl& col) const;
    bool operator!=(const wxColourImpl& col) const { return !(*this == col); }

    unsigned char Red() const;
    unsigned char Green() const;
    unsigned char Blue() const;

    // Implementation part

    void CalcPixel( WXColormap cmap );
    unsigned long GetPixel() const;
    WXColor *GetColor() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    wxNODISCARD virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

    virtual void
    InitRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    virtual bool FromString(const wxString& str);
};

#endif // _WX_COLOUR_H_
