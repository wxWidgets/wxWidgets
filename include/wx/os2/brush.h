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

class WXDLLIMPEXP_FWD_CORE wxBrush;

class WXDLLEXPORT wxBrushRefData: public wxGDIRefData
{
    friend class WXDLLIMPEXP_FWD_CORE wxBrush;
public:
    wxBrushRefData();
    wxBrushRefData(const wxBrushRefData& rData);
    virtual ~wxBrushRefData();

    bool operator == (const wxBrushRefData& data) const
    {
        return (m_nStyle == data.m_nStyle &&
                m_vStipple.IsSameAs(data.m_vStipple) &&
                m_vColour == data.m_vColour);
    }

protected:
    wxBrushStyle m_nStyle;
    wxBitmap     m_vStipple;
    wxColour     m_vColour;
    WXHBRUSH     m_hBrush; // in OS/2 GPI this will be the PS the pen is associated with
    AREABUNDLE   m_vBundle;
};

#define M_BRUSHDATA ((wxBrushRefData *)m_refData)

// Brush
class WXDLLEXPORT wxBrush: public wxBrushBase
{
public:
    wxBrush();
    wxBrush(const wxColour& rCol, wxBrushStyle nStyle = wxBRUSHSTYLE_SOLID);
#if FUTURE_WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED_FUTURE( wxBrush(const wxColour& col, int style) );
#endif
    wxBrush(const wxBitmap& rStipple);
    virtual ~wxBrush();

    bool operator == (const wxBrush& rBrush) const;
    inline bool operator != (const wxBrush& rBrush) const { return !(*this == rBrush); }

    virtual void SetColour(const wxColour& rColour);
    virtual void SetColour(unsigned char cRed, unsigned char cGreen, unsigned char cBrush);
    virtual void SetPS(HPS hPS);
    virtual void SetStyle(wxBrushStyle nStyle);
    virtual void SetStipple(const wxBitmap& rStipple);

    inline wxColour& GetColour(void) const { return (M_BRUSHDATA ? M_BRUSHDATA->m_vColour : wxNullColour); };
    virtual wxBrushStyle GetStyle(void) const { return (M_BRUSHDATA ? M_BRUSHDATA->m_nStyle : wxBRUSHSTYLE_INVALID); };
    inline wxBitmap* GetStipple(void) const { return (M_BRUSHDATA ? & M_BRUSHDATA->m_vStipple : NULL); };
    inline int       GetPS(void) const { return (M_BRUSHDATA ? M_BRUSHDATA->m_hBrush : 0); };

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

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

private:
    DECLARE_DYNAMIC_CLASS(wxBrush)
}; // end of CLASS wxBrush

#endif
    // _WX_BRUSH_H_
