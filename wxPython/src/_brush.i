/////////////////////////////////////////////////////////////////////////////
// Name:        _brush.i
// Purpose:     SWIG interface for wxPen
//
// Author:      Robin Dunn
//
// Created:     7-July-1997
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

DocStr(wxBrush,
       "A brush is a drawing tool for filling in areas. It is used for painting the\n"
       "background of rectangles, ellipses, etc. It has a colour and a style.");

class wxBrush : public wxGDIObject {
public:
    DocStr(wxBrush, "Constructs a brush from a colour object and style.");
    wxBrush(const wxColour& colour, int style=wxSOLID);
    ~wxBrush();
    
    
    virtual void SetColour(const wxColour& col);
    virtual void SetStyle(int style);
    virtual void SetStipple(const wxBitmap& stipple);

    wxColour GetColour() const;
    int GetStyle() const;
    wxBitmap *GetStipple() const;

    bool Ok();

#ifdef __WXMAC__
    short GetMacTheme();
    void SetMacTheme(short macThemeBrush);
#endif

    %pythoncode { def __nonzero__(self): return self.Ok() }
};

//---------------------------------------------------------------------------
