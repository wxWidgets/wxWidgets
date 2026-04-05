/////////////////////////////////////////////////////////////////////////////
// Name:        wx/x11/dc.h
// Purpose:     wxDC class
// Author:      Julian Smart
// Created:     17/09/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DC_H_
#define _WX_DC_H_

#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/icon.h"
#include "wx/font.h"
#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxX11DCImpl : public wxDCImpl
{
public:
    wxX11DCImpl( wxDC *owner );
    virtual ~wxX11DCImpl() = default;

    virtual wxSize GetPPI() const;

protected:
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
        wxCoord width, wxCoord height);
    virtual void DoGetSizeMM(int* width, int* height) const;

private:
    wxDECLARE_CLASS(wxX11DCImpl);
};

#endif
// _WX_DC_H_
