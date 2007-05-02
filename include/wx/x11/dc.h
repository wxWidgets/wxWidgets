/////////////////////////////////////////////////////////////////////////////
// Name:        dc.h
// Purpose:     wxDC class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
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

class WXDLLEXPORT wxDC : public wxDCBase
{
public:
    wxDC();
    virtual ~wxDC() { }

    // implement base class pure virtuals
    // ----------------------------------

    virtual wxSize GetPPI() const;

protected:
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
        wxCoord width, wxCoord height);
    virtual void DoGetSizeMM(int* width, int* height) const;

    // implementation
    wxCoord XDEV2LOG(wxCoord x) const       { return DeviceToLogicalX(x); }
    wxCoord XDEV2LOGREL(wxCoord x) const    { return DeviceToLogicalXRel(x); }
    wxCoord YDEV2LOG(wxCoord y) const       { return DeviceToLogicalY(y); }
    wxCoord YDEV2LOGREL(wxCoord y) const    { return DeviceToLogicalYRel(y); }
    wxCoord XLOG2DEV(wxCoord x) const       { return LogicalToDeviceX(x); }
    wxCoord XLOG2DEVREL(wxCoord x) const    { return LogicalToDeviceXRel(x); }
    wxCoord YLOG2DEV(wxCoord y) const       { return LogicalToDeviceY(y); }
    wxCoord YLOG2DEVREL(wxCoord y) const    { return LogicalToDeviceYRel(y); }

private:
    DECLARE_ABSTRACT_CLASS(wxDC)
};

#endif
// _WX_DC_H_
