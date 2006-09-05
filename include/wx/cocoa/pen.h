/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/pen.h
// Purpose:     wxPen class
// Author:      David Elliott
// Modified by:
// Created:     2003/08/02 (stubs from 22.03.2003)
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_PEN_H__
#define __WX_COCOA_PEN_H__

#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

class WXDLLEXPORT wxColour;
class WXDLLEXPORT wxBitmap;

// ========================================================================
// wxPen
// ========================================================================
class WXDLLEXPORT wxPen: public wxGDIObject
{
    DECLARE_DYNAMIC_CLASS(wxPen)
public:
    wxPen();
    wxPen(const wxColour& col, int width = 1, int style = wxSOLID);
    wxPen(const wxBitmap& stipple, int width);
    virtual ~wxPen();

    // wxObjectRefData
    wxObjectRefData *CreateRefData() const;
    wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;

    inline bool operator == (const wxPen& pen) const
    {   return m_refData == pen.m_refData; }
    inline bool operator != (const wxPen& pen) const
    {   return m_refData != pen.m_refData; }

    virtual bool Ok() const { return (m_refData != NULL) ; }

    void SetColour(const wxColour& col) ;
    void SetColour(unsigned char r, unsigned char g, unsigned char b)  ;

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
    int GetDashes(wxDash **ptr) const;
    wxBitmap *GetStipple() const;

    WX_NSColor GetNSColor();
    int GetCocoaLineDash(const float **pattern);
};

#endif // __WX_COCOA_PEN_H__
