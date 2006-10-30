/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/pen.h
// Purpose:     wxPen class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PEN_H_
#define _WX_PEN_H_

#include "wx/gdiobj.h"
#include "wx/colour.h"
#include "wx/bitmap.h"

class WXDLLEXPORT wxPen;

class WXDLLEXPORT wxPenRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxPen;
public:
    wxPenRefData();
    wxPenRefData(const wxPenRefData& data);
    virtual ~wxPenRefData();

    wxPenRefData& operator=(const wxPenRefData& data);

    bool operator==(const wxPenRefData& data) const
    {
        // we intentionally don't compare m_hPen fields here
        return m_style == data.m_style &&
               m_width == data.m_width &&
               m_join == data.m_join &&
               m_cap == data.m_cap &&
               m_colour == data.m_colour &&
               (m_style != wxSTIPPLE || m_stipple.IsRefTo(&data.m_stipple)) &&
               (m_style != wxUSER_DASH ||
                (m_nbDash == data.m_nbDash &&
                    memcmp(m_dash, data.m_dash, m_nbDash*sizeof(wxDash)) == 0));
    }

protected:
  int           m_width;
  int           m_style;
  int           m_join ;
  int           m_cap ;
  wxBitmap      m_stipple ;
  int           m_nbDash ;
  wxDash *      m_dash ;
  wxColour      m_colour;
/* TODO: implementation
  WXHPEN        m_hPen;
*/
};

#define M_PENDATA ((wxPenRefData *)m_refData)

// Pen
class WXDLLEXPORT wxPen: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxPen)
public:
  wxPen();
  wxPen(const wxColour& col, int width = 1, int style = wxSOLID);
  wxPen(const wxBitmap& stipple, int width);
  virtual ~wxPen();

  inline bool operator == (const wxPen& pen) const
  {
    const wxPenRefData *penData = (wxPenRefData *)pen.m_refData;

    // an invalid pen is only equal to another invalid pen
    return m_refData ? penData && *M_PENDATA == *penData : !penData;
  }

  inline bool operator != (const wxPen& pen) const { return !(*this == pen); }

  virtual bool Ok() const { return IsOk(); }
  virtual bool IsOk() const { return (m_refData != NULL) ; }

  // Override in order to recreate the pen
  void SetColour(const wxColour& col) ;
  void SetColour(unsigned char r, unsigned char g, unsigned char b) ;

  void SetWidth(int width)  ;
  void SetStyle(int style)  ;
  void SetStipple(const wxBitmap& stipple)  ;
  void SetDashes(int nb_dashes, const wxDash *dash)  ;
  void SetJoin(int join)  ;
  void SetCap(int cap)  ;

  inline wxColour& GetColour() const { return (M_PENDATA ? M_PENDATA->m_colour : wxNullColour); };
  inline int GetWidth() const { return (M_PENDATA ? M_PENDATA->m_width : 0); };
  inline int GetStyle() const { return (M_PENDATA ? M_PENDATA->m_style : 0); };
  inline int GetJoin() const { return (M_PENDATA ? M_PENDATA->m_join : 0); };
  inline int GetCap() const { return (M_PENDATA ? M_PENDATA->m_cap : 0); };
  inline int GetDashes(wxDash **ptr) const {
     *ptr = (M_PENDATA ? M_PENDATA->m_dash : (wxDash*) NULL); return (M_PENDATA ? M_PENDATA->m_nbDash : 0);
  }

  inline wxBitmap *GetStipple() const { return (M_PENDATA ? (& M_PENDATA->m_stipple) : (wxBitmap*) NULL); };

// Implementation

  // Useful helper: create the brush resource
  bool RealizeResource();

  // When setting properties, we must make sure we're not changing
  // another object
  void Unshare();
};

#endif
    // _WX_PEN_H_
