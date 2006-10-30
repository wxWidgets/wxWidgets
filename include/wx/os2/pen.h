/////////////////////////////////////////////////////////////////////////////
// Name:        wx/os2/pen.h
// Purpose:     wxPen class
// Author:      David Webster
// Modified by:
// Created:     10/10/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PEN_H_
#define _WX_PEN_H_

#include "wx/gdiobj.h"
#include "wx/bitmap.h"

typedef long wxPMDash;

class WXDLLEXPORT wxPen;

class WXDLLEXPORT wxPenRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxPen;
public:
    wxPenRefData();
    wxPenRefData(const wxPenRefData& rData);
    virtual ~wxPenRefData();

    bool operator==(const wxPenRefData& data) const
    {
        // we intentionally don't compare m_hPen fields here
        return m_nStyle == data.m_nStyle &&
               m_nWidth == data.m_nWidth &&
               m_nJoin == data.m_nJoin &&
               m_nCap == data.m_nCap &&
               m_vColour == data.m_vColour &&
               (m_style != wxSTIPPLE || m_stipple.IsRefTo(&data.m_stipple)) &&
               (m_style != wxUSER_DASH ||
                (m_dash == data.m_dash &&
                    memcmp(m_dash, data.m_dash, m_nbDash*sizeof(wxDash)) == 0));
    }

protected:
    int                             m_nWidth;
    int                             m_nStyle;
    int                             m_nJoin;
    int                             m_nCap;
    wxBitmap                        m_vStipple;
    int                             m_nbDash;
    wxDash *                        m_dash;
    wxColour                        m_vColour;
    WXHPEN                          m_hPen;// in OS/2 GPI this will be the PS the pen is associated with
};

#define M_PENDATA ((wxPenRefData *)m_refData)

// Pen
class WXDLLEXPORT wxPen : public wxGDIObject
{
    DECLARE_DYNAMIC_CLASS(wxPen)
public:
    wxPen();
    wxPen( const wxColour& rColour
          ,int             nWidth = 1
          ,int             nStyle = wxSOLID
         );
    wxPen( const wxBitmap& rStipple
          ,int             nWidth
         );
    virtual ~wxPen();

    inline bool   operator == (const wxPen& rPen) const
    {
        const wxPenRefData *penData = (wxPenRefData *)pen.m_refData;

        // an invalid pen is only equal to another invalid pen
        return m_refData ? penData && *M_PENDATA == *penData : !penData;
    }

    inline bool   operator != (const wxPen& rPen) const
        { return !(*this == rPen); }

    virtual bool Ok() const { return IsOk(); }
    virtual bool IsOk(void) const { return (m_refData != NULL); }

    //
    // Override in order to recreate the pen
    //
    void SetColour(const wxColour& rColour);
    void SetColour(unsigned char cRed, unsigned char cGreen, unsigned char cBlue);

    void SetWidth(int nWidth);
    void SetStyle(int nStyle);
    void SetStipple(const wxBitmap& rStipple);
    void SetDashes( int           nNbDashes
                   ,const wxDash* pDash
                  );
    void SetJoin(int nJoin);
    void SetCap(int nCap);
    void SetPS(HPS hPS);

    inline wxColour& GetColour(void) const { return (M_PENDATA ? M_PENDATA->m_vColour : wxNullColour); };
    inline int       GetWidth(void) const { return (M_PENDATA ? M_PENDATA->m_nWidth : 0); };
    inline int       GetStyle(void) const { return (M_PENDATA ? M_PENDATA->m_nStyle : 0); };
    inline int       GetJoin(void) const { return (M_PENDATA ? M_PENDATA->m_nJoin : 0); };
    inline int       GetCap(void) const { return (M_PENDATA ? M_PENDATA->m_nCap : 0); };
    inline int       GetPS(void) const { return (M_PENDATA ? M_PENDATA->m_hPen : 0); };
    inline int       GetDashes(wxDash **ptr) const
    {
        *ptr = (M_PENDATA ? (wxDash*)M_PENDATA->m_dash : (wxDash*) NULL);
        return (M_PENDATA ? M_PENDATA->m_nbDash : 0);
    }
    inline wxDash*   GetDash() const { return (M_PENDATA ? (wxDash*)M_PENDATA->m_dash : (wxDash*)NULL); };
    inline int       GetDashCount() const { return (M_PENDATA ? M_PENDATA->m_nbDash : 0); };

    inline wxBitmap* GetStipple(void) const { return (M_PENDATA ? (& M_PENDATA->m_vStipple) : (wxBitmap*) NULL); };

    //
    // Implementation
    //

    //
    // Useful helper: create the brush resource
    //
    bool     RealizeResource(void);
    bool     FreeResource(bool bForce = false);
    virtual WXHANDLE GetResourceHandle(void) const;
    bool     IsFree(void) const;
    void     Unshare(void);

private:
    LINEBUNDLE                     m_vLineBundle;
    AREABUNDLE                     m_vAreaBundle;
}; // end of CLASS wxPen

extern int wx2os2PenStyle(int nWxStyle);

#endif
    // _WX_PEN_H_
