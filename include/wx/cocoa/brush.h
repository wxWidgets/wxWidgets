/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/cocoa/brush.h
// Purpose:     wxBrush class
// Author:      David Elliott <dfe@cox.net>
// Modified by:
// Created:     2003/07/03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_BRUSH_H__
#define __WX_COCOA_BRUSH_H__

#include "wx/gdicmn.h"
#include "wx/gdiobj.h"
#include "wx/bitmap.h"

class WXDLLEXPORT wxBrush;

// ========================================================================
// wxBrush
// ========================================================================
class WXDLLEXPORT wxBrush: public wxGDIObject
{
    DECLARE_DYNAMIC_CLASS(wxBrush)
// ------------------------------------------------------------------------
// initialization
// ------------------------------------------------------------------------
public:
    wxBrush();
    wxBrush(const wxColour& col, int style = wxSOLID);
    wxBrush(const wxBitmap& stipple);
    wxBrush(const wxBrush& brush)
    :   wxGDIObject()
    {   Ref(brush); }
    ~wxBrush();

// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
    virtual void SetColour(const wxColour& col)  ;
    virtual void SetColour(unsigned char r, unsigned char g, unsigned char b)  ;
    virtual void SetStyle(int style)  ;
    virtual void SetStipple(const wxBitmap& stipple)  ;

    // assignment
    wxBrush& operator = (const wxBrush& brush)
    {   if (*this == brush) return (*this); Ref(brush); return *this; }

    // comparison
    bool operator == (const wxBrush& brush)
    {   return m_refData == brush.m_refData; }
    bool operator != (const wxBrush& brush)
    {   return m_refData != brush.m_refData; }

    // accessors
    wxColour GetColour() const;
    int GetStyle() const;
    wxBitmap *GetStipple() const;

    virtual bool Ok() const
    {   return (m_refData != NULL); }

    // wxObjectRefData
    wxObjectRefData *CreateRefData() const;
    wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;

    // wxCocoa
    WX_NSColor GetNSColor();
};

#endif // __WX_COCOA_BRUSH_H__
