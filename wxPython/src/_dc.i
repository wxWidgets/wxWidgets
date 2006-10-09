/////////////////////////////////////////////////////////////////////////////
// Name:        _dc.i
// Purpose:     SWIG interface defs for wxDC and releated classes
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

%{
#include "wx/wxPython/pydrawxxx.h"
%}

// TODO: 1. wrappers for wxDrawObject and wxDC::DrawObject


//---------------------------------------------------------------------------

%typemap(in) (int points, wxPoint* points_array ) {
    $2 = wxPoint_LIST_helper($input, &$1);
    if ($2 == NULL) SWIG_fail;
}
%typemap(freearg) (int points, wxPoint* points_array ) {
    if ($2) delete [] $2;
}


//---------------------------------------------------------------------------
%newgroup;


DocStr(wxDC,
"A wx.DC is a device context onto which graphics and text can be
drawn. It is intended to represent a number of output devices in a
generic way, so a window can have a device context associated with it,
and a printer also has a device context. In this way, the same piece
of code may write to a number of different devices, if the device
context is used as a parameter.

Derived types of wxDC have documentation for specific features only,
so refer to this section for most device context information.

The wx.DC class is abstract and can not be instantiated, you must use
one of the derived classes instead.  Which one will depend on the
situation in which it is used.", "");

class wxDC : public wxObject {
public:
//    wxDC(); **** abstract base class, can't instantiate.
    ~wxDC();


    %pythoncode {
        %# These have been deprecated in wxWidgets.  Since they never
        %# really did anything to begin with, just make them be NOPs.
        def BeginDrawing(self):  pass
        def EndDrawing(self):  pass
    }
    

    
// TODO    virtual void DrawObject(wxDrawObject* drawobject);


    DocStr(
        FloodFill,
        "Flood fills the device context starting from the given point, using
the current brush colour, and using a style:

    - **wxFLOOD_SURFACE**: the flooding occurs until a colour other than
      the given colour is encountered.

    - **wxFLOOD_BORDER**: the area to be flooded is bounded by the given
      colour.

Returns False if the operation failed.

Note: The present implementation for non-Windows platforms may fail to
find colour borders if the pixels do not match the colour
exactly. However the function will still return true.", "");
    bool FloodFill(wxCoord x, wxCoord y, const wxColour& col, int style = wxFLOOD_SURFACE);
    %Rename(FloodFillPoint, bool, FloodFill(const wxPoint& pt, const wxColour& col, int style = wxFLOOD_SURFACE));

    // fill the area specified by rect with a radial gradient, starting from
    // initialColour in the centre of the cercle and fading to destColour.

    DocDeclStr(
        void , GradientFillConcentric(const wxRect& rect,
                                      const wxColour& initialColour, 
                                      const wxColour& destColour,
                                      const wxPoint& circleCenter),
        "Fill the area specified by rect with a radial gradient, starting from
initialColour in the center of the circle and fading to destColour on
the outside of the circle.  The circleCenter argument is the relative
coordinants of the center of the circle in the specified rect.

Note: Currently this function is very slow, don't use it for real-time
drawing.", "");
    

    DocDeclStr(
        void , GradientFillLinear(const wxRect& rect,
                                  const wxColour& initialColour, 
                                  const wxColour& destColour,
                                  wxDirection nDirection = wxEAST),
        "Fill the area specified by rect with a linear gradient, starting from
initialColour and eventually fading to destColour. The nDirection
parameter specifies the direction of the colour change, default is to
use initialColour on the left part of the rectangle and destColour on
the right side.", "");
    
   
    
    DocStr(
        GetPixel,
        "Gets the colour at the specified location on the DC.","");
    %extend {
        wxColour GetPixel(wxCoord x, wxCoord y) {
            wxColour col;
            self->GetPixel(x, y, &col);
            return col;
        }
        wxColour GetPixelPoint(const wxPoint& pt) {
            wxColour col;
            self->GetPixel(pt, &col);
            return col;
        }
    }

    
    DocStr(
        DrawLine,
        "Draws a line from the first point to the second. The current pen is
used for drawing the line. Note that the second point is *not* part of
the line and is not drawn by this function (this is consistent with
the behaviour of many other toolkits).", "");
    void DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);
    %Rename(DrawLinePoint, void, DrawLine(const wxPoint& pt1, const wxPoint& pt2));


    DocStr(
        CrossHair,
        "Displays a cross hair using the current pen. This is a vertical and
horizontal line the height and width of the window, centred on the
given point.", "");
    void CrossHair(wxCoord x, wxCoord y);
    %Rename(CrossHairPoint, void, CrossHair(const wxPoint& pt));


    DocStr(
        DrawArc,
        "Draws an arc of a circle, centred on the *center* point (xc, yc), from
the first point to the second. The current pen is used for the outline
and the current brush for filling the shape.

The arc is drawn in an anticlockwise direction from the start point to
the end point.", "");
    void DrawArc(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord xc, wxCoord yc);
    %Rename(DrawArcPoint, void, DrawArc(const wxPoint& pt1, const wxPoint& pt2, const wxPoint& center));


    DocStr(
        DrawCheckMark,
        "Draws a check mark inside the given rectangle.", "");
    void DrawCheckMark(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    %Rename(DrawCheckMarkRect, void, DrawCheckMark(const wxRect& rect));

    DocStr(
        DrawEllipticArc,
        "Draws an arc of an ellipse, with the given rectangle defining the
bounds of the ellipse. The current pen is used for drawing the arc and
the current brush is used for drawing the pie.

The *start* and *end* parameters specify the start and end of the arc
relative to the three-o'clock position from the center of the
rectangle. Angles are specified in degrees (360 is a complete
circle). Positive values mean counter-clockwise motion. If start is
equal to end, a complete ellipse will be drawn.", "");
    void DrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h, double start, double end);
    %Rename(DrawEllipticArcPointSize, void, DrawEllipticArc(const wxPoint& pt, const wxSize& sz, double start, double end));


    DocStr(
        DrawPoint,
        "Draws a point using the current pen.", "");
    void DrawPoint(wxCoord x, wxCoord y);
    %Rename(DrawPointPoint, void, DrawPoint(const wxPoint& pt));


    DocStr(
        DrawRectangle,
        "Draws a rectangle with the given top left corner, and with the given
size. The current pen is used for the outline and the current brush
for filling the shape.", "");
    void DrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    %Rename(DrawRectangleRect,void, DrawRectangle(const wxRect& rect));
    %Rename(DrawRectanglePointSize, void, DrawRectangle(const wxPoint& pt, const wxSize& sz));


    DocStr(
        DrawRoundedRectangle,
        "Draws a rectangle with the given top left corner, and with the given
size. The corners are quarter-circles using the given radius. The
current pen is used for the outline and the current brush for filling
the shape.

If radius is positive, the value is assumed to be the radius of the
rounded corner. If radius is negative, the absolute value is assumed
to be the proportion of the smallest dimension of the rectangle. This
means that the corner can be a sensible size relative to the size of
the rectangle, and also avoids the strange effects X produces when the
corners are too big for the rectangle.", "");
    void DrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius);
    %Rename(DrawRoundedRectangleRect, void, DrawRoundedRectangle(const wxRect& r, double radius));
    %Rename(DrawRoundedRectanglePointSize, void, DrawRoundedRectangle(const wxPoint& pt, const wxSize& sz, double radius));


    DocStr(
        DrawCircle,
        "Draws a circle with the given center point and radius.  The current
pen is used for the outline and the current brush for filling the
shape.", "

:see: `DrawEllipse`");
    void DrawCircle(wxCoord x, wxCoord y, wxCoord radius);
    %Rename(DrawCirclePoint, void, DrawCircle(const wxPoint& pt, wxCoord radius));


    DocStr(
        DrawEllipse,
        "Draws an ellipse contained in the specified rectangle. The current pen
is used for the outline and the current brush for filling the shape.", "

:see: `DrawCircle`");
    void DrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    %Rename(DrawEllipseRect, void, DrawEllipse(const wxRect& rect));
    %Rename(DrawEllipsePointSize, void, DrawEllipse(const wxPoint& pt, const wxSize& sz));


    DocStr(
        DrawIcon,
        "Draw an icon on the display (does nothing if the device context is
PostScript). This can be the simplest way of drawing bitmaps on a
window.", "");
    void DrawIcon(const wxIcon& icon, wxCoord x, wxCoord y);
    %Rename(DrawIconPoint, void, DrawIcon(const wxIcon& icon, const wxPoint& pt));


    DocStr(
        DrawBitmap,
        "Draw a bitmap on the device context at the specified point. If
*transparent* is true and the bitmap has a transparency mask, (or
alpha channel on the platforms that support it) then the bitmap will
be drawn transparently.", "

When drawing a mono-bitmap, the current text foreground colour will be
used to draw the foreground of the bitmap (all bits set to 1), and the
current text background colour to draw the background (all bits set to
0).

:see: `SetTextForeground`, `SetTextBackground` and `wx.MemoryDC`");
    void DrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask = false);
    %Rename(DrawBitmapPoint, void, DrawBitmap(const wxBitmap &bmp, const wxPoint& pt, bool useMask = false));


    DocStr(
        DrawText,
        "Draws a text string at the specified point, using the current text
font, and the current text foreground and background colours.

The coordinates refer to the top-left corner of the rectangle bounding
the string. See `GetTextExtent` for how to get the dimensions of a
text string, which can be used to position the text more precisely.

**NOTE**: under wxGTK the current logical function is used by this
function but it is ignored by wxMSW. Thus, you should avoid using
logical functions with this function in portable programs.", "

:see: `DrawRotatedText`");
    void DrawText(const wxString& text, wxCoord x, wxCoord y);
    %Rename(DrawTextPoint, void, DrawText(const wxString& text, const wxPoint& pt));


    DocStr(
        DrawRotatedText,
        "Draws the text rotated by *angle* degrees, if supported by the platform.

**NOTE**: Under Win9x only TrueType fonts can be drawn by this
function. In particular, a font different from ``wx.NORMAL_FONT``
should be used as the it is not normally a TrueType
font. ``wx.SWISS_FONT`` is an example of a font which is.","

:see: `DrawText`");
    void DrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle);
    %Rename(DrawRotatedTextPoint, void, DrawRotatedText(const wxString& text, const wxPoint& pt, double angle));


    DocDeclStr(
        bool , Blit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                    wxDC *source, wxCoord xsrc, wxCoord ysrc,
                    int rop = wxCOPY, bool useMask = false,
                    wxCoord xsrcMask = -1, wxCoord ysrcMask = -1),
        "Copy from a source DC to this DC.  Parameters specify the destination
coordinates, size of area to copy, source DC, source coordinates,
logical function, whether to use a bitmap mask, and mask source
position.", "

    :param xdest:       Destination device context x position.
    :param ydest:       Destination device context y position.
    :param width:       Width of source area to be copied.
    :param height:      Height of source area to be copied.
    :param source:      Source device context.
    :param xsrc:        Source device context x position.
    :param ysrc:        Source device context y position.
    :param rop:         Logical function to use: see `SetLogicalFunction`.
    :param useMask:     If true, Blit does a transparent blit using the mask
                        that is associated with the bitmap selected into the
                        source device context.
    :param xsrcMask:    Source x position on the mask. If both xsrcMask and
                        ysrcMask are -1, xsrc and ysrc will be assumed for
                        the mask source position.
    :param ysrcMask:    Source y position on the mask. 
");
    
    DocDeclStrName(
        bool , Blit(const wxPoint& destPt, const wxSize& sz,
                    wxDC *source, const wxPoint& srcPt,
                    int rop = wxCOPY, bool useMask = false,
                    const wxPoint& srcPtMask = wxDefaultPosition),
        "Copy from a source DC to this DC.  Parameters specify the destination
coordinates, size of area to copy, source DC, source coordinates,
logical function, whether to use a bitmap mask, and mask source
position.", "

    :param destPt:      Destination device context position.
    :param sz:          Size of source area to be copied.
    :param source:      Source device context.
    :param srcPt:       Source device context position.
    :param rop:         Logical function to use: see `SetLogicalFunction`.
    :param useMask:     If true, Blit does a transparent blit using the mask
                        that is associated with the bitmap selected into the
                        source device context.
    :param srcPtMask:   Source position on the mask. 
",
        BlitPointSize);
    

    DocStr(
        SetClippingRegion,
        "Sets the clipping region for this device context to the intersection
of the given region described by the parameters of this method and the
previously set clipping region. You should call `DestroyClippingRegion`
if you want to set the clipping region exactly to the region
specified.

The clipping region is an area to which drawing is
restricted. Possible uses for the clipping region are for clipping
text or for speeding up window redraws when only a known area of the
screen is damaged.", "

:see: `DestroyClippingRegion`, `wx.Region`");
    void SetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    %Rename(SetClippingRegionPointSize, void, SetClippingRegion(const wxPoint& pt, const wxSize& sz));
    %Rename(SetClippingRegionAsRegion, void, SetClippingRegion(const wxRegion& region));
    %Rename(SetClippingRect, void, SetClippingRegion(const wxRect& rect));


    
    DocDeclAStr(
        void , DrawLines(int points, wxPoint* points_array,
                         wxCoord xoffset = 0, wxCoord yoffset = 0),
        "DrawLines(self, List points, int xoffset=0, int yoffset=0)",
        "Draws lines using a sequence of `wx.Point` objects, adding the
optional offset coordinate. The current pen is used for drawing the
lines.", "");
    

    DocDeclAStr(
        void , DrawPolygon(int points, wxPoint* points_array,
                     wxCoord xoffset = 0, wxCoord yoffset = 0,
                           int fillStyle = wxODDEVEN_RULE),
        "DrawPolygon(self, List points, int xoffset=0, int yoffset=0,
    int fillStyle=ODDEVEN_RULE)",
        "Draws a filled polygon using a sequence of `wx.Point` objects, adding
the optional offset coordinate.  The last argument specifies the fill
rule: ``wx.ODDEVEN_RULE`` (the default) or ``wx.WINDING_RULE``.

The current pen is used for drawing the outline, and the current brush
for filling the shape. Using a transparent brush suppresses
filling. Note that wxWidgets automatically closes the first and last
points.", "");
    

    // TODO:  Figure out a good typemap for this...
    //        Convert the first 3 args from a sequence of sequences?
//     void DrawPolyPolygon(int n, int count[], wxPoint points[],
//                           wxCoord xoffset = 0, wxCoord yoffset = 0,
//                           int fillStyle = wxODDEVEN_RULE);

    
    DocDeclStr(
        void , DrawLabel(const wxString& text, const wxRect& rect,
                         int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                         int indexAccel = -1),
        "Draw *text* within the specified rectangle, abiding by the alignment
flags.  Will additionally emphasize the character at *indexAccel* if
it is not -1.", "

:see: `DrawImageLabel`");

    
    %extend {
        DocStr(DrawImageLabel,
               "Draw *text* and an image (which may be ``wx.NullBitmap`` to skip
drawing it) within the specified rectangle, abiding by the alignment
flags.  Will additionally emphasize the character at *indexAccel* if
it is not -1.  Returns the bounding rectangle.", "");
        wxRect DrawImageLabel(const wxString& text,
                              const wxBitmap& image,
                              const wxRect& rect,
                              int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                              int indexAccel = -1) {
            wxRect rv;
            self->DrawLabel(text, image, rect, alignment, indexAccel, &rv);
            return rv;
        }
    }



    DocDeclAStr(
        void , DrawSpline(int points, wxPoint* points_array),
        "DrawSpline(self, List points)",
        "Draws a spline between all given control points, (a list of `wx.Point`
objects) using the current pen. The spline is drawn using a series of
lines, using an algorithm taken from the X drawing program 'XFIG'.", "");
    



    // global DC operations
    // --------------------

    DocDeclStr(
        virtual void , Clear(),
        "Clears the device context using the current background brush.", "");
    

    DocDeclStr(
        virtual bool , StartDoc(const wxString& message),
        "Starts a document (only relevant when outputting to a
printer). *Message* is a message to show whilst printing.", "");
    
    DocDeclStr(
        virtual void , EndDoc(),
        "Ends a document (only relevant when outputting to a printer).", "");
    

    DocDeclStr(
        virtual void , StartPage(),
        "Starts a document page (only relevant when outputting to a printer).", "");
    
    DocDeclStr(
        virtual void , EndPage(),
        "Ends a document page (only relevant when outputting to a printer).", "");
    


    // set objects to use for drawing
    // ------------------------------

    DocDeclStr(
        virtual void , SetFont(const wxFont& font),
        "Sets the current font for the DC. It must be a valid font, in
particular you should not pass ``wx.NullFont`` to this method.","

:see: `wx.Font`");
    
    DocDeclStr(
        virtual void , SetPen(const wxPen& pen),
        "Sets the current pen for the DC.

If the argument is ``wx.NullPen``, the current pen is selected out of the
device context, and the original pen restored.", "

:see: `wx.Pen`");
    
    DocDeclStr(
        virtual void , SetBrush(const wxBrush& brush),
        "Sets the current brush for the DC.

If the argument is ``wx.NullBrush``, the current brush is selected out
of the device context, and the original brush restored, allowing the
current brush to be destroyed safely.","

:see: `wx.Brush`");
    
    DocDeclStr(
        virtual void , SetBackground(const wxBrush& brush),
        "Sets the current background brush for the DC.", "");
    
    DocDeclStr(
        virtual void , SetBackgroundMode(int mode),
        "*mode* may be one of ``wx.SOLID`` and ``wx.TRANSPARENT``. This setting
determines whether text will be drawn with a background colour or
not.", "");
    
    DocDeclStr(
        virtual void , SetPalette(const wxPalette& palette),
        "If this is a window DC or memory DC, assigns the given palette to the
window or bitmap associated with the DC. If the argument is
``wx.NullPalette``, the current palette is selected out of the device
context, and the original palette restored.", "

:see: `wx.Palette`");
    


    DocDeclStr(
        virtual void , DestroyClippingRegion(),
        "Destroys the current clipping region so that none of the DC is
clipped.", "

:see: `SetClippingRegion`");
    

    DocDeclAStr(
        void, GetClippingBox(wxCoord *OUTPUT, wxCoord *OUTPUT, wxCoord *OUTPUT, wxCoord *OUTPUT) const,
        "GetClippingBox() -> (x, y, width, height)",
        "Gets the rectangle surrounding the current clipping region.", "");

    %extend {
        DocStr(
            GetClippingRect,
            "Gets the rectangle surrounding the current clipping region.", "");
        wxRect GetClippingRect() {
            wxRect rect;
            self->GetClippingBox(rect);
            return rect;
        }
    }



    // text extent
    // -----------

    DocDeclStr(
        virtual wxCoord , GetCharHeight() const,
        "Gets the character height of the currently set font.", "");
    
    DocDeclStr(
        virtual wxCoord , GetCharWidth() const,
        "Gets the average character width of the currently set font.", "");
    


    DocDeclAStr(
        void, GetTextExtent(const wxString& string, wxCoord *OUTPUT, wxCoord *OUTPUT),
        "GetTextExtent(wxString string) -> (width, height)",
        "Get the width and height of the text using the current font. Only
works for single line strings.", "");

    DocDeclAStrName(
        void, GetTextExtent(const wxString& string,
                            wxCoord *OUTPUT, wxCoord *OUTPUT, wxCoord *OUTPUT, wxCoord* OUTPUT,
                            wxFont* font = NULL),
        "GetFullTextExtent(wxString string, Font font=None) ->\n   (width, height, descent, externalLeading)",
        "Get the width, height, decent and leading of the text using the
current or specified font. Only works for single line strings.", "",
        GetFullTextExtent);


    // works for single as well as multi-line strings
    DocDeclAStr(
        void, GetMultiLineTextExtent(const wxString& text,
                                     wxCoord *OUTPUT, wxCoord *OUTPUT, wxCoord *OUTPUT,
                                     wxFont *font = NULL),
        "GetMultiLineTextExtent(wxString string, Font font=None) ->\n   (width, height, lineHeight)",
        "Get the width, height, and line height of the text using the
current or specified font. Works for single as well as multi-line
strings.", "");

    
    %extend {
        DocAStr(GetPartialTextExtents,
                "GetPartialTextExtents(self, text) -> [widths]",
                "Returns a list of integers such that each value is the distance in
pixels from the begining of text to the coresponding character of
*text*. The generic version simply builds a running total of the widths
of each character using GetTextExtent, however if the various
platforms have a native API function that is faster or more accurate
than the generic implementation then it will be used instead.", "");
        wxArrayInt GetPartialTextExtents(const wxString& text) {
            wxArrayInt widths;
            self->GetPartialTextExtents(text, widths);
            return widths;
        }
    }
    

    // size and resolution
    // -------------------

    DocStr(
        GetSize,
        "This gets the horizontal and vertical resolution in device units. It
can be used to scale graphics to fit the page. For example, if *maxX*
and *maxY* represent the maximum horizontal and vertical 'pixel' values
used in your application, the following code will scale the graphic to
fit on the printer page::

      w, h = dc.GetSize()
      scaleX = maxX*1.0 / w
      scaleY = maxY*1.0 / h
      dc.SetUserScale(min(scaleX,scaleY),min(scaleX,scaleY))
", "");
    wxSize GetSize();
    DocDeclAName(
        void, GetSize( int *OUTPUT, int *OUTPUT ),
        "GetSizeTuple() -> (width, height)",
        GetSizeTuple);


    DocStr(GetSizeMM, "Get the DC size in milimeters.", "");
    wxSize GetSizeMM() const;
    DocDeclAName(
        void, GetSizeMM( int *OUTPUT, int *OUTPUT ) const,
        "GetSizeMMTuple() -> (width, height)",
        GetSizeMMTuple);



    // coordinates conversions
    // -----------------------

    DocDeclStr(
        wxCoord , DeviceToLogicalX(wxCoord x) const,
        "Convert device X coordinate to logical coordinate, using the current
mapping mode.", "");
    
    DocDeclStr(
        wxCoord , DeviceToLogicalY(wxCoord y) const,
        "Converts device Y coordinate to logical coordinate, using the current
mapping mode.", "");
    
    DocDeclStr(
        wxCoord , DeviceToLogicalXRel(wxCoord x) const,
        "Convert device X coordinate to relative logical coordinate, using the
current mapping mode but ignoring the x axis orientation. Use this
function for converting a width, for example.", "");
    
    DocDeclStr(
        wxCoord , DeviceToLogicalYRel(wxCoord y) const,
        "Convert device Y coordinate to relative logical coordinate, using the
current mapping mode but ignoring the y axis orientation. Use this
function for converting a height, for example.", "");
    
    DocDeclStr(
        wxCoord , LogicalToDeviceX(wxCoord x) const,
        "Converts logical X coordinate to device coordinate, using the current
mapping mode.", "");
    
    DocDeclStr(
        wxCoord , LogicalToDeviceY(wxCoord y) const,
        "Converts logical Y coordinate to device coordinate, using the current
mapping mode.", "");
    
    DocDeclStr(
        wxCoord , LogicalToDeviceXRel(wxCoord x) const,
        "Converts logical X coordinate to relative device coordinate, using the
current mapping mode but ignoring the x axis orientation. Use this for
converting a width, for example.", "");
    
    DocDeclStr(
        wxCoord , LogicalToDeviceYRel(wxCoord y) const,
        "Converts logical Y coordinate to relative device coordinate, using the
current mapping mode but ignoring the y axis orientation. Use this for
converting a height, for example.", "");
    

    
    // query DC capabilities
    // ---------------------

    virtual bool CanDrawBitmap() const;
    virtual bool CanGetTextExtent() const;


    DocDeclStr(
        virtual int , GetDepth() const,
        "Returns the colour depth of the DC.", "");
    

    DocDeclStr(
        virtual wxSize , GetPPI() const,
        "Resolution in pixels per inch", "");
    

    DocDeclStr(
        virtual bool , IsOk() const,
        "Returns true if the DC is ok to use.", "");
    %pythoncode { Ok = IsOk }
    


    DocDeclStr(
        int , GetBackgroundMode() const,
        "Returns the current background mode, either ``wx.SOLID`` or
``wx.TRANSPARENT``.","

:see: `SetBackgroundMode`");
    
    DocDeclStr(
        const wxBrush&  , GetBackground() const,
        "Gets the brush used for painting the background.","

:see: `SetBackground`");
    
    DocDeclStr(
        const wxBrush&  , GetBrush() const,
        "Gets the current brush", "");
    
    DocDeclStr(
        const wxFont&   , GetFont() const,
        "Gets the current font", "");
    
    DocDeclStr(
        const wxPen&    , GetPen() const,
        "Gets the current pen", "");
    
    DocDeclStr(
        const wxColour& , GetTextBackground() const,
        "Gets the current text background colour", "");
    
    DocDeclStr(
        const wxColour& , GetTextForeground() const,
        "Gets the current text foreground colour", "");
    

    DocDeclStr(
        virtual void , SetTextForeground(const wxColour& colour),
        "Sets the current text foreground colour for the DC.", "");
    
    DocDeclStr(
        virtual void , SetTextBackground(const wxColour& colour),
        "Sets the current text background colour for the DC.", "");
    

    DocDeclStr(
        int , GetMapMode() const,
        "Gets the current *mapping mode* for the device context ", "");
    
    DocDeclStr(
        virtual void , SetMapMode(int mode),
        "The *mapping mode* of the device context defines the unit of
measurement used to convert logical units to device units.  The
mapping mode can be one of the following:

    ================    =============================================
    wx.MM_TWIPS         Each logical unit is 1/20 of a point, or 1/1440
                        of an inch.
    wx.MM_POINTS        Each logical unit is a point, or 1/72 of an inch.
    wx.MM_METRIC        Each logical unit is 1 mm.
    wx.MM_LOMETRIC      Each logical unit is 1/10 of a mm.
    wx.MM_TEXT          Each logical unit is 1 pixel.
    ================    =============================================
","
Note that in X, text drawing isn't handled consistently with the
mapping mode; a font is always specified in point size. However,
setting the user scale (see `SetUserScale`) scales the text
appropriately. In Windows, scalable TrueType fonts are always used; in
X, results depend on availability of fonts, but usually a reasonable
match is found.

The coordinate origin is always at the top left of the screen/printer.

Drawing to a Windows printer device context uses the current mapping
mode, but mapping mode is currently ignored for PostScript output.
");
    


    DocDeclAStr(
        virtual void, GetUserScale(double *OUTPUT, double *OUTPUT) const,
        "GetUserScale(self) -> (xScale, yScale)",
        "Gets the current user scale factor (set by `SetUserScale`).", "");

    DocDeclStr(
        virtual void , SetUserScale(double x, double y),
        "Sets the user scaling factor, useful for applications which require
'zooming'.", "");
    


    DocDeclA(
        virtual void, GetLogicalScale(double *OUTPUT, double *OUTPUT),
        "GetLogicalScale() -> (xScale, yScale)");

    virtual void SetLogicalScale(double x, double y);


    wxPoint GetLogicalOrigin() const;
    DocDeclAName(
        void, GetLogicalOrigin(wxCoord *OUTPUT, wxCoord *OUTPUT) const,
        "GetLogicalOriginTuple() -> (x,y)",
        GetLogicalOriginTuple);

    virtual void SetLogicalOrigin(wxCoord x, wxCoord y);
    %extend {
        void SetLogicalOriginPoint(const wxPoint& point) {
            self->SetLogicalOrigin(point.x, point.y);
        }
    }

    
    wxPoint GetDeviceOrigin() const;
    DocDeclAName(
        void, GetDeviceOrigin(wxCoord *OUTPUT, wxCoord *OUTPUT) const,
        "GetDeviceOriginTuple() -> (x,y)",
        GetDeviceOriginTuple);

    virtual void SetDeviceOrigin(wxCoord x, wxCoord y);
    %extend {
        void SetDeviceOriginPoint(const wxPoint& point) {
            self->SetDeviceOrigin(point.x, point.y);
        }
    }

    DocDeclStr(
        virtual void , SetAxisOrientation(bool xLeftRight, bool yBottomUp),
        "Sets the x and y axis orientation (i.e., the direction from lowest to
highest values on the axis). The default orientation is the natural
orientation, e.g. x axis from left to right and y axis from bottom up.", "");


    DocDeclStr(
        int , GetLogicalFunction() const,
        "Gets the current logical function (set by `SetLogicalFunction`).", "");
    
    DocDeclStr(
        virtual void , SetLogicalFunction(int function),
        "Sets the current logical function for the device context. This
determines how a source pixel (from a pen or brush colour, or source
device context if using `Blit`) combines with a destination pixel in
the current device context.

The possible values and their meaning in terms of source and
destination pixel values are as follows:

    ================       ==========================
    wx.AND                 src AND dst
    wx.AND_INVERT          (NOT src) AND dst
    wx.AND_REVERSE         src AND (NOT dst)
    wx.CLEAR               0
    wx.COPY                src
    wx.EQUIV               (NOT src) XOR dst
    wx.INVERT              NOT dst
    wx.NAND                (NOT src) OR (NOT dst)
    wx.NOR                 (NOT src) AND (NOT dst)
    wx.NO_OP               dst
    wx.OR                  src OR dst
    wx.OR_INVERT           (NOT src) OR dst
    wx.OR_REVERSE          src OR (NOT dst)
    wx.SET                 1
    wx.SRC_INVERT          NOT src
    wx.XOR                 src XOR dst
    ================       ==========================

The default is wx.COPY, which simply draws with the current
colour. The others combine the current colour and the background using
a logical operation. wx.INVERT is commonly used for drawing rubber
bands or moving outlines, since drawing twice reverts to the original
colour.
", "");
    

    DocDeclStr(
        void , ComputeScaleAndOrigin(),
        "Performs all necessary computations for given platform and context
type after each change of scale and origin parameters. Usually called
automatically internally after such changes.
", "");
    

    
//     DocDeclStr(
//         virtual void , SetOptimization(bool optimize),
//         "If *optimize* is true this function sets optimization mode on. This
// currently means that under X, the device context will not try to set a
// pen or brush property if it is known to be set already. This approach
// can fall down if non-wxWidgets code is using the same device context
// or window, for example when the window is a panel on which the
// windowing system draws panel items. The wxWidgets device context
// 'memory' will now be out of step with reality.

// Setting optimization off, drawing, then setting it back on again, is a
// trick that must occasionally be employed.", "");
    
//     DocDeclStr(
//         virtual bool , GetOptimization(),
//         "Returns true if device context optimization is on. See
// `SetOptimization` for details.", "");
    
    %pythoncode {
        def SetOptimization(self, optimize):
            pass
        def GetOptimization(self):
            return False

        SetOptimization = wx._deprecated(SetOptimization)
        GetOptimization = wx._deprecated(GetOptimization)
   }

    
    // bounding box
    // ------------

    DocDeclStr(
        virtual void , CalcBoundingBox(wxCoord x, wxCoord y),
        "Adds the specified point to the bounding box which can be retrieved
with `MinX`, `MaxX` and `MinY`, `MaxY` or `GetBoundingBox` functions.", "");
    
    %extend {
        DocStr(CalcBoundingBoxPoint,
               "Adds the specified point to the bounding box which can be retrieved
with `MinX`, `MaxX` and `MinY`, `MaxY` or `GetBoundingBox` functions.","");
        void CalcBoundingBoxPoint(const wxPoint& point) {
            self->CalcBoundingBox(point.x, point.y);
        }
    }

    DocDeclStr(
        void , ResetBoundingBox(),
        "Resets the bounding box: after a call to this function, the bounding
box doesn't contain anything.", "");
    

    // Get the final bounding box of the PostScript or Metafile picture.
    DocDeclStr(
        wxCoord , MinX() const,
        "Gets the minimum horizontal extent used in drawing commands so far.", "");
    
    DocDeclStr(
        wxCoord , MaxX() const,
        "Gets the maximum horizontal extent used in drawing commands so far.", "");
    
    DocDeclStr(
        wxCoord , MinY() const,
        "Gets the minimum vertical extent used in drawing commands so far.", "");
    
    DocDeclStr(
        wxCoord , MaxY() const,
        "Gets the maximum vertical extent used in drawing commands so far.", "");
    


    DocAStr(GetBoundingBox,
            "GetBoundingBox() -> (x1,y1, x2,y2)",
            "Returns the min and max points used in drawing commands so far.", "");
    %extend {
        void GetBoundingBox(int* OUTPUT, int* OUTPUT, int* OUTPUT, int* OUTPUT);
        // See below for implementation
    }
    
    %pythoncode { def __nonzero__(self): return self.IsOk() };


#ifdef __WXMSW__
    long GetHDC();
#endif


    %extend { // See drawlist.cpp for impplementaion of these...
        PyObject* _DrawPointList(PyObject* pyCoords, PyObject* pyPens, PyObject* pyBrushes)
        {
            return wxPyDrawXXXList(*self, wxPyDrawXXXPoint, pyCoords, pyPens, pyBrushes);
        }

        PyObject* _DrawLineList(PyObject* pyCoords, PyObject* pyPens, PyObject* pyBrushes)
        {
            return wxPyDrawXXXList(*self, wxPyDrawXXXLine, pyCoords, pyPens, pyBrushes);
        }

        PyObject* _DrawRectangleList(PyObject* pyCoords, PyObject* pyPens, PyObject* pyBrushes)
        {
            return wxPyDrawXXXList(*self, wxPyDrawXXXRectangle, pyCoords, pyPens, pyBrushes);
        }

        PyObject* _DrawEllipseList(PyObject* pyCoords, PyObject* pyPens, PyObject* pyBrushes)
        {
            return wxPyDrawXXXList(*self, wxPyDrawXXXEllipse, pyCoords, pyPens, pyBrushes);
        }

        PyObject* _DrawPolygonList(PyObject* pyCoords, PyObject* pyPens, PyObject* pyBrushes)
        {
            return wxPyDrawXXXList(*self, wxPyDrawXXXPolygon, pyCoords, pyPens, pyBrushes);
        }

        PyObject* _DrawTextList(PyObject* textList, PyObject* pyPoints,
                                PyObject* foregroundList, PyObject* backgroundList) {
            return wxPyDrawTextList(*self, textList, pyPoints, foregroundList, backgroundList);
        }
    }

    %pythoncode {
    def DrawPointList(self, points, pens=None):
        """
        Draw a list of points as quickly as possible.

            :param points:  A sequence of 2-element sequences representing
                            each point to draw, (x,y).
            :param pens:    If None, then the current pen is used.  If a
                            single pen then it will be used for all points.  If
                            a list of pens then there should be one for each point
                            in points.
        """
        if pens is None:
           pens = []
        elif isinstance(pens, wx.Pen):
           pens = [pens]
        elif len(pens) != len(points):
           raise ValueError('points and pens must have same length')
        return self._DrawPointList(points, pens, [])


    def DrawLineList(self, lines, pens=None):
        """
        Draw a list of lines as quickly as possible.

            :param lines:  A sequence of 4-element sequences representing
                            each line to draw, (x1,y1, x2,y2).
            :param pens:    If None, then the current pen is used.  If a
                            single pen then it will be used for all lines.  If
                            a list of pens then there should be one for each line
                            in lines.
        """
        if pens is None:
           pens = []
        elif isinstance(pens, wx.Pen):
           pens = [pens]
        elif len(pens) != len(lines):
           raise ValueError('lines and pens must have same length')
        return self._DrawLineList(lines, pens, [])


    def DrawRectangleList(self, rectangles, pens=None, brushes=None):
        """
        Draw a list of rectangles as quickly as possible.

            :param rectangles:  A sequence of 4-element sequences representing
                            each rectangle to draw, (x,y, w,h).
            :param pens:    If None, then the current pen is used.  If a
                            single pen then it will be used for all rectangles.
                            If a list of pens then there should be one for each 
                            rectangle in rectangles.
            :param brushes: A brush or brushes to be used to fill the rectagles,
                            with similar semantics as the pens parameter.
        """
        if pens is None:
           pens = []
        elif isinstance(pens, wx.Pen):
           pens = [pens]
        elif len(pens) != len(rectangles):
           raise ValueError('rectangles and pens must have same length')
        if brushes is None:
           brushes = []
        elif isinstance(brushes, wx.Brush):
           brushes = [brushes]
        elif len(brushes) != len(rectangles):
           raise ValueError('rectangles and brushes must have same length')
        return self._DrawRectangleList(rectangles, pens, brushes)


    def DrawEllipseList(self, ellipses, pens=None, brushes=None):
        """
        Draw a list of ellipses as quickly as possible.

            :param ellipses: A sequence of 4-element sequences representing
                            each ellipse to draw, (x,y, w,h).
            :param pens:    If None, then the current pen is used.  If a
                            single pen then it will be used for all ellipses.
                            If a list of pens then there should be one for each 
                            ellipse in ellipses.
            :param brushes: A brush or brushes to be used to fill the ellipses,
                            with similar semantics as the pens parameter.
        """
        if pens is None:
           pens = []
        elif isinstance(pens, wx.Pen):
           pens = [pens]
        elif len(pens) != len(ellipses):
           raise ValueError('ellipses and pens must have same length')
        if brushes is None:
           brushes = []
        elif isinstance(brushes, wx.Brush):
           brushes = [brushes]
        elif len(brushes) != len(ellipses):
           raise ValueError('ellipses and brushes must have same length')
        return self._DrawEllipseList(ellipses, pens, brushes)


    def DrawPolygonList(self, polygons, pens=None, brushes=None):
        """
        Draw a list of polygons, each of which is a list of points.

            :param polygons: A sequence of sequences of sequences.
                             [[(x1,y1),(x2,y2),(x3,y3)...],
                             [(x1,y1),(x2,y2),(x3,y3)...]]
                              
            :param pens:    If None, then the current pen is used.  If a
                            single pen then it will be used for all polygons.
                            If a list of pens then there should be one for each 
                            polygon.
            :param brushes: A brush or brushes to be used to fill the polygons,
                            with similar semantics as the pens parameter.
        """
        if pens is None:
           pens = []
        elif isinstance(pens, wx.Pen):
           pens = [pens]
        elif len(pens) != len(polygons):
           raise ValueError('polygons and pens must have same length')
        if brushes is None:
           brushes = []
        elif isinstance(brushes, wx.Brush):
           brushes = [brushes]
        elif len(brushes) != len(polygons):
           raise ValueError('polygons and brushes must have same length')
        return self._DrawPolygonList(polygons, pens, brushes)


    def DrawTextList(self, textList, coords, foregrounds = None, backgrounds = None):
        """
        Draw a list of strings using a list of coordinants for positioning each string.

            :param textList:    A list of strings
            :param coords:      A list of (x,y) positions
            :param foregrounds: A list of `wx.Colour` objects to use for the
                                foregrounds of the strings.
            :param backgrounds: A list of `wx.Colour` objects to use for the
                                backgrounds of the strings.

        NOTE: Make sure you set Background mode to wx.Solid (DC.SetBackgroundMode)
              If you want backgrounds to do anything.
        """
        if type(textList) == type(''):
           textList = [textList]
        elif len(textList) != len(coords):
           raise ValueError('textlist and coords must have same length')
        if foregrounds is None:
           foregrounds = []
        elif isinstance(foregrounds, wx.Colour):
           foregrounds = [foregrounds]
        elif len(foregrounds) != len(coords):
           raise ValueError('foregrounds and coords must have same length')
        if backgrounds is None:
           backgrounds = []
        elif isinstance(backgrounds, wx.Colour):
           backgrounds = [backgrounds]
        elif len(backgrounds) != len(coords):
           raise ValueError('backgrounds and coords must have same length')
        return  self._DrawTextList(textList, coords, foregrounds, backgrounds)
    }

    %property(Background, GetBackground, SetBackground, doc="See `GetBackground` and `SetBackground`");
    %property(BackgroundMode, GetBackgroundMode, SetBackgroundMode, doc="See `GetBackgroundMode` and `SetBackgroundMode`");
    %property(BoundingBox, GetBoundingBox, doc="See `GetBoundingBox`");
    %property(Brush, GetBrush, SetBrush, doc="See `GetBrush` and `SetBrush`");
    %property(CharHeight, GetCharHeight, doc="See `GetCharHeight`");
    %property(CharWidth, GetCharWidth, doc="See `GetCharWidth`");
    %property(ClippingBox, GetClippingBox, doc="See `GetClippingBox`");
    %property(ClippingRect, GetClippingRect, SetClippingRect, doc="See `GetClippingRect` and `SetClippingRect`");
    %property(Depth, GetDepth, doc="See `GetDepth`");
    %property(DeviceOrigin, GetDeviceOrigin, SetDeviceOrigin, doc="See `GetDeviceOrigin` and `SetDeviceOrigin`");
    %property(Font, GetFont, SetFont, doc="See `GetFont` and `SetFont`");
    %property(FullTextExtent, GetFullTextExtent, doc="See `GetFullTextExtent`");
    %property(LogicalFunction, GetLogicalFunction, SetLogicalFunction, doc="See `GetLogicalFunction` and `SetLogicalFunction`");
    %property(LogicalOrigin, GetLogicalOrigin, SetLogicalOrigin, doc="See `GetLogicalOrigin` and `SetLogicalOrigin`");
    %property(LogicalScale, GetLogicalScale, SetLogicalScale, doc="See `GetLogicalScale` and `SetLogicalScale`");
    %property(MapMode, GetMapMode, SetMapMode, doc="See `GetMapMode` and `SetMapMode`");
    %property(MultiLineTextExtent, GetMultiLineTextExtent, doc="See `GetMultiLineTextExtent`");
    %property(Optimization, GetOptimization, SetOptimization, doc="See `GetOptimization` and `SetOptimization`");
    %property(PPI, GetPPI, doc="See `GetPPI`");
    %property(PartialTextExtents, GetPartialTextExtents, doc="See `GetPartialTextExtents`");
    %property(Pen, GetPen, SetPen, doc="See `GetPen` and `SetPen`");
    %property(Pixel, GetPixel, doc="See `GetPixel`");
    %property(PixelPoint, GetPixelPoint, doc="See `GetPixelPoint`");
    %property(Size, GetSize, doc="See `GetSize`");
    %property(SizeMM, GetSizeMM, doc="See `GetSizeMM`");
    %property(TextBackground, GetTextBackground, SetTextBackground, doc="See `GetTextBackground` and `SetTextBackground`");
    %property(TextExtent, GetTextExtent, doc="See `GetTextExtent`");
    %property(TextForeground, GetTextForeground, SetTextForeground, doc="See `GetTextForeground` and `SetTextForeground`");
    %property(UserScale, GetUserScale, SetUserScale, doc="See `GetUserScale` and `SetUserScale`");
};



%{
static void wxDC_GetBoundingBox(wxDC* dc, int* x1, int* y1, int* x2, int* y2) {
    *x1 = dc->MinX();
    *y1 = dc->MinY();
    *x2 = dc->MaxX();
    *y2 = dc->MaxY();
}
%}


//---------------------------------------------------------------------------
%newgroup

MustHaveApp(wxMemoryDC);

DocStr(wxMemoryDC,
"A memory device context provides a means to draw graphics onto a
bitmap. A bitmap must be selected into the new memory DC before it may
be used for anything. Typical usage is as follows::

    dc = wx.MemoryDC()
    dc.SelectObject(bitmap)
    # draw on the dc usign any of the Draw methods
    dc.SelectObject(wx.NullBitmap)
    # the bitmap now contains wahtever was drawn upon it

Note that the memory DC *must* be deleted (or the bitmap selected out
of it) before a bitmap can be reselected into another memory DC.
", "");

class wxMemoryDC : public wxDC {
public:
    DocCtorStr(
        wxMemoryDC(),
        "Constructs a new memory device context.

Use the Ok member to test whether the constructor was successful in
creating a usable device context. Don't forget to select a bitmap into
the DC before drawing on it.", "

:see: `MemoryDCFromDC`");

    DocCtorStrName(
        wxMemoryDC(wxDC* oldDC),
        "Creates a DC that is compatible with the oldDC.", "",
        MemoryDCFromDC);

    
    DocDeclStr(
        void , SelectObject(const wxBitmap& bitmap),
        "Selects the bitmap into the device context, to use as the memory
bitmap. Selecting the bitmap into a memory DC allows you to draw into
the DC, and therefore the bitmap, and also to use Blit to copy the
bitmap to a window.

If the argument is wx.NullBitmap (or some other uninitialised
`wx.Bitmap`) the current bitmap is selected out of the device context,
and the original bitmap restored, allowing the current bitmap to be
destroyed safely.", "");
    
};

//---------------------------------------------------------------------------
%newgroup


%{
#include <wx/dcbuffer.h>
%}

enum {
    wxBUFFER_VIRTUAL_AREA,
    wxBUFFER_CLIENT_AREA
};

MustHaveApp(wxBufferedDC);

DocStr(wxBufferedDC,
"This simple class provides a simple way to avoid flicker: when drawing
on it, everything is in fact first drawn on an in-memory buffer (a
`wx.Bitmap`) and then copied to the screen only once, when this object
is destroyed.

It can be used in the same way as any other device context.
wx.BufferedDC itself typically replaces `wx.ClientDC`, if you want to
use it in your EVT_PAINT handler, you should look at
`wx.BufferedPaintDC`.

Please note that GTK+ 2.0 and OS X provide double buffering themselves
natively so using this class on those platforms will normally result
in an unneeded level of buffering.
", "");

class wxBufferedDC : public wxMemoryDC
{
public:
    %pythonAppend wxBufferedDC
        "self.__dc = args[0] # save a ref so the other dc will not be deleted before self";
    
    %nokwargs wxBufferedDC;

    DocStr(
        wxBufferedDC,
        "Constructs a buffered DC.", "

    :param dc: The underlying DC: everything drawn to this object will
        be flushed to this DC when this object is destroyed. You may
        pass ``None`` in order to just initialize the buffer, and not
        flush it.

    :param buffer: If a `wx.Size` object is passed as the 2nd arg then
        it is the size of the bitmap that will be created internally
        and used for an implicit buffer. If the 2nd arg is a
        `wx.Bitmap` then it is the explicit buffer that will be
        used. Using an explicit buffer is the most efficient solution
        as the bitmap doesn't have to be recreated each time but it
        also requires more memory as the bitmap is never freed. The
        bitmap should have appropriate size, anything drawn outside of
        its bounds is clipped.  If wx.NullBitmap is used then a new
        buffer will be allocated that is the same size as the dc.

    :param style: The style parameter indicates whether the supplied buffer is
        intended to cover the entire virtual size of a `wx.ScrolledWindow` or
        if it only covers the client area.  Acceptable values are
        ``wx.BUFFER_VIRTUAL_AREA`` and ``wx.BUFFER_CLIENT_AREA``.

");
    wxBufferedDC( wxDC* dc,
                  const wxBitmap& buffer=wxNullBitmap,
                  int style = wxBUFFER_CLIENT_AREA );
    wxBufferedDC( wxDC* dc,
                  const wxSize& area,
                  int style = wxBUFFER_CLIENT_AREA );

    DocCtorStr(
        ~wxBufferedDC(),
        "Copies everything drawn on the DC so far to the underlying DC
associated with this object, if any.", "");
    
   
    DocDeclStr(
        void , UnMask(),
        "Blits the buffer to the dc, and detaches the dc from the buffer (so it
can be effectively used once only).  This is usually only called in
the destructor.", "");
    
};




MustHaveApp(wxBufferedPaintDC);

DocStr(wxBufferedPaintDC,
"This is a subclass of `wx.BufferedDC` which can be used inside of an
EVT_PAINT event handler. Just create an object of this class instead
of `wx.PaintDC` and that's all you have to do to (mostly) avoid
flicker. The only thing to watch out for is that if you are using this
class together with `wx.ScrolledWindow`, you probably do **not** want
to call `wx.Window.PrepareDC` on it as it already does this internally
for the real underlying `wx.PaintDC`.

If your window is already fully buffered in a `wx.Bitmap` then your
EVT_PAINT handler can be as simple as just creating a
``wx.BufferedPaintDC`` as it will `Blit` the buffer to the window
automatically when it is destroyed.  For example::

    def OnPaint(self, event):
        dc = wx.BufferedPaintDC(self, self.buffer)




", "");

class wxBufferedPaintDC : public wxBufferedDC
{
public:

    DocCtorStr(
        wxBufferedPaintDC( wxWindow *window,
                           const wxBitmap &buffer = wxNullBitmap,
                           int style = wxBUFFER_CLIENT_AREA),
        "Create a buffered paint DC.  As with `wx.BufferedDC`, you may either
provide the bitmap to be used for buffering or let this object create
one internally (in the latter case, the size of the client part of the
window is automatically used).

", "");

};


//---------------------------------------------------------------------------
%newgroup

MustHaveApp(wxScreenDC);

DocStr(wxScreenDC,
"A wxScreenDC can be used to paint anywhere on the screen. This should
normally be constructed as a temporary stack object; don't store a
wxScreenDC object.
", "");
class wxScreenDC : public wxDC {
public:
    wxScreenDC();

    DocDeclStrName(
        bool , StartDrawingOnTop(wxWindow* window),
        "Specify that the area of the screen to be drawn upon coincides with
the given window.

:see: `EndDrawingOnTop`", "",
        StartDrawingOnTopWin);
    

    DocDeclStr(
        bool , StartDrawingOnTop(wxRect* rect = NULL),
        "Specify that the area is the given rectangle, or the whole screen if
``None`` is passed.

:see: `EndDrawingOnTop`", "");
    

    DocDeclStr(
        bool , EndDrawingOnTop(),
        "Use this in conjunction with `StartDrawingOnTop` or
`StartDrawingOnTopWin` to ensure that drawing to the screen occurs on
top of existing windows. Without this, some window systems (such as X)
only allow drawing to take place underneath other windows.

You might use this pair of functions when implementing a drag feature,
for example as in the `wx.SplitterWindow` implementation.

These functions are probably obsolete since the X implementations
allow drawing directly on the screen now. However, the fact that this
function allows the screen to be refreshed afterwards may be useful
to some applications.", "");
    
};

//---------------------------------------------------------------------------
%newgroup

MustHaveApp(wxWindowDC);

DocStr(wxWindowDC,
       "A wx.WindowDC must be constructed if an application wishes to paint on
the whole area of a window (client and decorations). This should
normally be constructed as a temporary stack object; don't store a
wx.WindowDC object.","");
class wxWindowDC : public wxDC {
public:
    DocCtorStr(
        wxWindowDC(wxWindow* win),
        "Constructor. Pass the window on which you wish to paint.","");
};

//---------------------------------------------------------------------------
%newgroup

MustHaveApp(wxClientDC);

DocStr(wxClientDC,
"A wx.ClientDC must be constructed if an application wishes to paint on
the client area of a window from outside an EVT_PAINT event. This should
normally be constructed as a temporary stack object; don't store a
wx.ClientDC object long term.

To draw on a window from within an EVT_PAINT handler, construct a
`wx.PaintDC` object.

To draw on the whole window including decorations, construct a
`wx.WindowDC` object (Windows only).
", "");
class wxClientDC : public wxWindowDC {
public:
    DocCtorStr(
        wxClientDC(wxWindow* win),
        "Constructor. Pass the window on which you wish to paint.", "");
};

//---------------------------------------------------------------------------
%newgroup

MustHaveApp(wxPaintDC);

DocStr(wxPaintDC,
"A wx.PaintDC must be constructed if an application wishes to paint on
the client area of a window from within an EVT_PAINT event
handler. This should normally be constructed as a temporary stack
object; don't store a wx.PaintDC object. If you have an EVT_PAINT
handler, you **must** create a wx.PaintDC object within it even if you
don't actually use it.

Using wx.PaintDC within EVT_PAINT handlers is important because it
automatically sets the clipping area to the damaged area of the
window. Attempts to draw outside this area do not appear.

To draw on a window from outside EVT_PAINT handlers, construct a
`wx.ClientDC` object.
","");
class wxPaintDC : public wxClientDC {
public:
    DocCtorStr(
        wxPaintDC(wxWindow* win),
        "Constructor. Pass the window on which you wish to paint.", "");
};

//---------------------------------------------------------------------------
%newgroup

MustHaveApp(wxMirrorDC);

DocStr(wxMirrorDC,
"wx.MirrorDC is a simple wrapper class which is always associated with a
real `wx.DC` object and either forwards all of its operations to it
without changes (no mirroring takes place) or exchanges x and y
coordinates which makes it possible to reuse the same code to draw a
figure and its mirror -- i.e. reflection related to the diagonal line
x == y.", "");
class wxMirrorDC : public wxDC
{
public:
    DocCtorStr(
        wxMirrorDC(wxDC& dc, bool mirror),
        "Creates a mirrored DC associated with the real *dc*.  Everything drawn
on the wx.MirrorDC will appear on the *dc*, and will be mirrored if
*mirror* is True.","");
};

//---------------------------------------------------------------------------
%newgroup

%{
#include <wx/dcps.h>
%}

MustHaveApp(wxPostScriptDC);

DocStr(wxPostScriptDC,
"This is a `wx.DC` that can write to PostScript files on any platform.","");

class wxPostScriptDC : public wxDC {
public:
    DocCtorStr(
        wxPostScriptDC(const wxPrintData& printData),
        "Constructs a PostScript printer device context from a `wx.PrintData`
object.", "");

    wxPrintData& GetPrintData();
    void SetPrintData(const wxPrintData& data);

    DocDeclStr(
        static void , SetResolution(int ppi),
        "Set resolution (in pixels per inch) that will be used in PostScript
output. Default is 720ppi.", "");
    
    DocDeclStr(
        static int , GetResolution(),
        "Return resolution used in PostScript output.", "");

    %property(PrintData, GetPrintData, SetPrintData, doc="See `GetPrintData` and `SetPrintData`");
};

//---------------------------------------------------------------------------
%newgroup


MustHaveApp(wxMetaFile);
MustHaveApp(wxMetaFileDC);


#if defined(__WXMSW__) || defined(__WXMAC__)

%{
#include <wx/metafile.h>
%}

class wxMetaFile : public wxObject {
public:
    wxMetaFile(const wxString& filename = wxPyEmptyString);
    ~wxMetaFile();

    bool IsOk();
    %pythoncode { Ok = IsOk }
    bool SetClipboard(int width = 0, int height = 0);

    wxSize GetSize();
    int GetWidth();
    int GetHeight();

#ifdef __WXMSW__
    const wxString& GetFileName() const;
#endif
    
    %pythoncode { def __nonzero__(self): return self.IsOk() }
};

// bool wxMakeMetaFilePlaceable(const wxString& filename,
//                              int minX, int minY, int maxX, int maxY, float scale=1.0);


class wxMetaFileDC : public wxDC {
public:
    wxMetaFileDC(const wxString& filename = wxPyEmptyString,
                 int width = 0, int height = 0,
                 const wxString& description = wxPyEmptyString);
    wxMetaFile* Close();
};



#else  // Make some dummies for the other platforms

%{
class wxMetaFile : public wxObject {
public:
    wxMetaFile(const wxString&)
        { wxPyRaiseNotImplemented(); }
};

class wxMetaFileDC : public wxClientDC {
public:
    wxMetaFileDC(const wxString&, int, int, const wxString&)
        { wxPyRaiseNotImplemented(); }
};

%}

class wxMetaFile : public wxObject {
public:
    wxMetaFile(const wxString& filename = wxPyEmptyString);
};

class wxMetaFileDC : public wxDC {
public:
    wxMetaFileDC(const wxString& filename = wxPyEmptyString,
                 int width = 0, int height = 0,
                 const wxString& description = wxPyEmptyString);
};


#endif


//---------------------------------------------------------------------------

MustHaveApp(wxPrinterDC);

#if defined(__WXMSW__) || defined(__WXMAC__)

class  wxPrinterDC : public wxDC {
public:
    wxPrinterDC(const wxPrintData& printData);
};

#else
%{
class  wxPrinterDC : public wxClientDC {
public:
    wxPrinterDC(const wxPrintData&)
        { wxPyRaiseNotImplemented(); }

};
%}

class  wxPrinterDC : public wxDC {
public:
    wxPrinterDC(const wxPrintData& printData);
};
#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
