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
"A brush is a drawing tool for filling in areas. It is used for
painting the background of rectangles, ellipses, etc. when drawing on
a `wx.DC`.  It has a colour and a style.", "

:warning: Do not create instances of wx.Brush before the `wx.App`
    object has been created because, depending on the platform,
    required internal data structures may not have been initialized
    yet.  Instead create your brushes in the app's OnInit or as they
    are needed for drawing.

:note: On monochrome displays all brushes are white, unless the colour
    really is black.

:see: `wx.BrushList`, `wx.DC`, `wx.DC.SetBrush`
");

MustHaveApp(wxBrush);

class wxBrush : public wxGDIObject {
public:
    DocCtorStr(
        wxBrush(const wxColour& colour, int style=wxSOLID),
        "Constructs a brush from a `wx.Colour` object and a style.",
        "The style parameter may be one of the following:

    ===================   =============================
    Style                 Meaning
    ===================   =============================
    wx.TRANSPARENT        Transparent (no fill).
    wx.SOLID              Solid.
    wx.STIPPLE            Uses a bitmap as a stipple.
    wx.BDIAGONAL_HATCH    Backward diagonal hatch.
    wx.CROSSDIAG_HATCH    Cross-diagonal hatch.
    wx.FDIAGONAL_HATCH    Forward diagonal hatch.
    wx.CROSS_HATCH        Cross hatch.
    wx.HORIZONTAL_HATCH   Horizontal hatch.
    wx.VERTICAL_HATCH     Vertical hatch.
    ===================   =============================

:see: `wx.BrushFromBitmap`
");

    DocCtorStrName(
        wxBrush(const wxBitmap& stippleBitmap),
        "Constructs a stippled brush using a bitmap.", "",
        BrushFromBitmap);

    ~wxBrush();


    DocDeclStr(
        virtual void , SetColour(const wxColour& col),
        "Set the brush's `wx.Colour`.", "");

    DocDeclStr(
        virtual void , SetStyle(int style),
        "Sets the style of the brush. See `__init__` for a listing of styles.", "");

    DocDeclStr(
        virtual void , SetStipple(const wxBitmap& stipple),
        "Sets the stipple `wx.Bitmap`.", "");


    DocDeclStr(
        wxColour , GetColour() const,
        "Returns the `wx.Colour` of the brush.", "");

    DocDeclStr(
        int , GetStyle() const,
        "Returns the style of the brush.  See `__init__` for a listing of
styles.", "");

    DocDeclStr(
        wxBitmap *, GetStipple() const,
        "Returns the stiple `wx.Bitmap` of the brush.  If the brush does not
have a wx.STIPPLE style, then the return value may be non-None but an
uninitialised bitmap (`wx.Bitmap.Ok` returns False).", "");

    
    DocDeclStr(
        bool , IsHatch() const,
        "Is the current style a hatch type?", "");
    

    DocDeclStr(
        bool , Ok(),
        "Returns True if the brush is initialised and valid.", "");


#ifdef __WXMAC__
    short MacGetTheme();
    void MacSetTheme(short macThemeBrush);
#endif

    %pythoncode { def __nonzero__(self): return self.Ok() }

    %property(Colour, GetColour, SetColour, doc="See `GetColour` and `SetColour`");
    %property(Stipple, GetStipple, SetStipple, doc="See `GetStipple` and `SetStipple`");
    %property(Style, GetStyle, SetStyle, doc="See `GetStyle` and `SetStyle`");

};

//---------------------------------------------------------------------------
