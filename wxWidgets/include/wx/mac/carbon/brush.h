/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/brush.h
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
class WXDLLEXPORT wxBrush: public wxBrushBase
{
    DECLARE_DYNAMIC_CLASS(wxBrush)

public:
    wxBrush();
    wxBrush(short macThemeBrush ) ;
    wxBrush(const wxColour& col, int style = wxSOLID);
    wxBrush(const wxBitmap& stipple);
    virtual ~wxBrush();

    virtual void SetColour(const wxColour& col) ;
    virtual void SetColour(unsigned char r, unsigned char g, unsigned char b) ;
    virtual void SetStyle(int style)  ;
    virtual void SetStipple(const wxBitmap& stipple)  ;
    virtual void MacSetTheme(short macThemeBrush) ;
    virtual void MacSetThemeBackground(unsigned long macThemeBackground ,  WXRECTPTR extent) ;

    bool operator == (const wxBrush& brush) const;
    bool operator != (const wxBrush& brush) const
        { return !(*this == brush); }

    wxMacBrushKind MacGetBrushKind()  const ;

    unsigned long MacGetThemeBackground(WXRECTPTR extent)  const ;
    short MacGetTheme()  const ;
    wxColour& GetColour() const ;
    virtual int GetStyle() const ;
    wxBitmap *GetStipple() const ;

    virtual bool Ok() const { return IsOk(); }
    virtual bool IsOk() const { return (m_refData != NULL) ; }

// Implementation

    // Useful helper: create the brush resource
    bool RealizeResource();

    // When setting properties, we must make sure we're not changing
    // another object
    void Unshare();
};

#endif
    // _WX_BRUSH_H_
