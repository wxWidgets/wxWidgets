/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/pen.h
// Purpose:     wxPen class
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: fixed operator=(), ==(), !=()
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PEN_H_
#define _WX_PEN_H_

#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

// ----------------------------------------------------------------------------
// Pen
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxPen : public wxGDIObject
{
public:
    wxPen() { }
    wxPen(const wxColour& col, int width = 1, int style = wxSOLID);
    wxPen(const wxBitmap& stipple, int width);
    virtual ~wxPen() { }

    bool operator==(const wxPen& pen) const;
    bool operator!=(const wxPen& pen) const { return !(*this == pen); }

    virtual bool Ok() const { return IsOk(); }
    virtual bool IsOk() const { return (m_refData != NULL); }

    // Override in order to recreate the pen
    void SetColour(const wxColour& col);
    void SetColour(unsigned char r, unsigned char g, unsigned char b);

    void SetWidth(int width);
    void SetStyle(int style);
    void SetStipple(const wxBitmap& stipple);
    void SetDashes(int nb_dashes, const wxDash *dash);
    void SetJoin(int join);
    void SetCap(int cap);

    wxColour& GetColour() const;
    int GetWidth() const;
    int GetStyle() const;
    int GetJoin() const;
    int GetCap() const;
    int GetDashes(wxDash** ptr) const;
    wxDash* GetDash() const;
    int GetDashCount() const;
    wxBitmap* GetStipple() const;

    // internal: wxGDIObject methods
    virtual bool RealizeResource();
    virtual bool FreeResource(bool force = false);
    virtual WXHANDLE GetResourceHandle() const;
    virtual bool IsFree() const;

protected:
    virtual wxObjectRefData* CreateRefData() const;
    virtual wxObjectRefData* CloneRefData(const wxObjectRefData* data) const;

    // same as FreeResource() + RealizeResource()
    bool Recreate();

    DECLARE_DYNAMIC_CLASS(wxPen)
};

#endif // _WX_PEN_H_
