/////////////////////////////////////////////////////////////////////////////
// Name:        brush.h
// Purpose:     wxBrush class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BRUSH_H_
#define _WX_BRUSH_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "brush.h"
#endif

#include "wx/gdicmn.h"
#include "wx/gdiobj.h"
#include "wx/bitmap.h"

class WXDLLEXPORT wxBrush;

typedef enum
{
    kwxMacBrushColour ,
    kwxMacBrushTheme ,
    kwxMacBrushThemeBackground
} wxMacBrushKind ;

// Brush
class WXDLLEXPORT wxBrush: public wxGDIObject
{
    DECLARE_DYNAMIC_CLASS(wxBrush)

public:
    wxBrush();
    wxBrush(short macThemeBrush ) ;
    wxBrush(const wxColour& col, int style = wxSOLID);
    wxBrush(const wxBitmap& stipple);
    wxBrush(const wxBrush& brush)
        : wxGDIObject()
        { Ref(brush); }
    ~wxBrush();

    virtual void SetColour(const wxColour& col)  ;
    virtual void SetColour(unsigned char r, unsigned char g, unsigned char b)  ;
    virtual void SetStyle(int style)  ;
    virtual void SetStipple(const wxBitmap& stipple)  ;
    virtual void MacSetTheme(short macThemeBrush) ;
    virtual void MacSetThemeBackground(unsigned long macThemeBackground ,  WXRECTPTR extent) ;

    wxBrush& operator = (const wxBrush& brush)
    { if (*this == brush) return (*this); Ref(brush); return *this; }
    bool operator == (const wxBrush& brush)
    { return m_refData == brush.m_refData; }
    bool operator != (const wxBrush& brush)
    { return m_refData != brush.m_refData; }

    wxMacBrushKind MacGetBrushKind()  const ;

    unsigned long MacGetThemeBackground(WXRECTPTR extent)  const ;
    short MacGetTheme()  const ;
    wxColour& GetColour() const ;
    int GetStyle() const ;
    wxBitmap *GetStipple() const ;

    bool IsHatch() const
        { return (GetStyle()>=wxBDIAGONAL_HATCH) && (GetStyle()<=wxVERTICAL_HATCH); }

    virtual bool Ok() const { return (m_refData != NULL) ; }

// Implementation

    // Useful helper: create the brush resource
    bool RealizeResource();

    // When setting properties, we must make sure we're not changing
    // another object
    void Unshare();
};

#endif
    // _WX_BRUSH_H_
