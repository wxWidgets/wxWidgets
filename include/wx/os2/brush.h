/////////////////////////////////////////////////////////////////////////////
// Name:        wx/os2/brush.h
// Purpose:     wxBrush class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BRUSH_H_
#define _WX_BRUSH_H_

#include "wx/gdicmn.h"
#include "wx/gdiobj.h"
#include "wx/bitmap.h"

class WXDLLEXPORT wxBrush;

class WXDLLEXPORT wxBrushRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxBrush;
public:
    wxBrushRefData();
    wxBrushRefData(const wxBrushRefData& rData);
    virtual ~wxBrushRefData();

protected:
    int         m_nStyle;
    wxBitmap    m_vStipple ;
    wxColour    m_vColour;
    WXHBRUSH    m_hBrush; // in OS/2 GPI this will be the PS the pen is associated with
    AREABUNDLE  m_vBundle;
};

#define M_BRUSHDATA ((wxBrushRefData *)m_refData)

// Brush
class WXDLLEXPORT wxBrush: public wxBrushBase
{
    DECLARE_DYNAMIC_CLASS(wxBrush)

public:
    wxBrush();
    wxBrush(const wxColour& rCol, int nStyle = wxSOLID);
    wxBrush(const wxBitmap& rStipple);
    virtual ~wxBrush();

    inline bool operator == (const wxBrush& rBrush) const;
    inline bool operator != (const wxBrush& rBrush) const { return !(*this == rBrush); }

    virtual void SetColour(const wxColour& rColour);
    virtual void SetColour(unsigned char cRed, unsigned char cGreen, unsigned char cBrush);
    virtual void SetPS(HPS hPS);
    virtual void SetStyle(int nStyle)  ;
    virtual void SetStipple(const wxBitmap& rStipple);

    inline wxColour& GetColour(void) const { return (M_BRUSHDATA ? M_BRUSHDATA->m_vColour : wxNullColour); };
    virtual int      GetStyle(void) const { return (M_BRUSHDATA ? M_BRUSHDATA->m_nStyle : 0); };
    inline wxBitmap* GetStipple(void) const { return (M_BRUSHDATA ? & M_BRUSHDATA->m_vStipple : 0); };
    inline int       GetPS(void) const { return (M_BRUSHDATA ? M_BRUSHDATA->m_hBrush : 0); };

    inline virtual bool Ok() const { return IsOk(); }
    inline virtual bool IsOk(void) const { return (m_refData != NULL) ; }

    //
    // Implementation
    //

    //
    // Useful helper: create the brush resource
    //
    bool     RealizeResource(void);
    virtual WXHANDLE GetResourceHandle(void) const;
    bool     FreeResource(bool bForce = false);
    bool     IsFree(void) const;
    void     Unshare(void);
}; // end of CLASS wxBrush

#endif
    // _WX_BRUSH_H_
