/////////////////////////////////////////////////////////////////////////////
// Name:        brush.h
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __WX_BRUSH_H__
#define __WX_BRUSH_H__

#ifdef __GNUG__
#pragma interface "brush.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdiobj.h"
#include "wx/bitmap.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxBitmap;
class WXDLLEXPORT wxBrush;

//-----------------------------------------------------------------------------
// wxBrush
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxBrush: public wxGDIObject
{
public:
    wxBrush();
    wxBrush(const wxColour &colour, int style);
    wxBrush(const wxBitmap &stippleBitmap);
    wxBrush(const wxBrush &brush);
    ~wxBrush();
    wxBrush& operator = (const wxBrush& brush);
    bool operator == (const wxBrush& brush) const;
    bool operator != (const wxBrush& brush) const;
    bool Ok() const;

    int GetStyle() const;
    wxColour &GetColour() const;
    wxBitmap *GetStipple() const;

    void SetColour(const wxColour& col);
    void SetColour(unsigned char r, unsigned char g, unsigned char b);
    void SetStyle(int style);
    void SetStipple(const wxBitmap& stipple);
    
    // implementation:

    void Unshare();
    
    void* GetMaskPattern() const;
    void* GetPixPattern() const;

private:
    DECLARE_DYNAMIC_CLASS(wxBrush)
};

#endif // __WX_BRUSH_H__
