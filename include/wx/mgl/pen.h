/////////////////////////////////////////////////////////////////////////////
// Name:        pen.h
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __WX_PEN_H__
#define __WX_PEN_H__

#ifdef __GNUG__
#pragma interface "pen.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxBitmap;
class WXDLLEXPORT wxPen;

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPen: public wxGDIObject
{
public:
    wxPen();
    wxPen(const wxColour &colour, int width, int style);
    wxPen(const wxBitmap& stipple, int width);
    wxPen(const wxPen& pen);
    ~wxPen();
    wxPen& operator = (const wxPen& pen);
    bool operator == (const wxPen& pen) const;
    bool operator != (const wxPen& pen) const;

    void SetColour(const wxColour &colour);
    void SetColour(int red, int green, int blue);
    void SetCap(int capStyle);
    void SetJoin(int joinStyle);
    void SetStyle(int style);
    void SetWidth(int width);
    void SetDashes(int number_of_dashes, const wxDash *dash);
    void SetStipple(const wxBitmap& stipple);
    
    wxColour &GetColour() const;
    int GetCap() const;
    int GetJoin() const;
    int GetStyle() const;
    int GetWidth() const;
    int GetDashes(wxDash **ptr) const;
    int GetDashCount() const;
    wxDash* GetDash() const;
    wxBitmap *GetStipple() const;
    
    bool Ok() const;

    void Unshare();
    
    // implementation:
    void* GetPixPattern() const;

private:    
    DECLARE_DYNAMIC_CLASS(wxPen)
};

#endif // __WX_PEN_H__
