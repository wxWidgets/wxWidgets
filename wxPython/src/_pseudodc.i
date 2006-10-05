/////////////////////////////////////////////////////////////////////////////
// Name:        
// Purpose:     Wraper definitions for the wxPseudoDC Class
// Author:      Paul Lanier 
// Modified by:
// Created:     05/25/06
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


%{
#include "wx/wxPython/pseudodc.h"
%}

%newgroup;

DocStr(wxPseudoDC,
"A PseudoDC is an object that can be used as if it were a `wx.DC`.  All
commands issued to the PseudoDC are stored in a list.  You can then
play these commands back to a real DC object using the DrawToDC
method.  Commands in the command list are indexed by ID.  You can use
this to clear the operations associated with a single ID and then
re-draw the object associated with that ID.", "");

class wxPseudoDC : public wxObject
{
public:
    DocCtorStr(
        wxPseudoDC(),
        "Constructs a new Pseudo device context for recording dc operations","");
    DocDeclStr(
        void , BeginDrawing(),
        "Allows for optimization of drawing code on platforms that need it.  On
other platforms this is just an empty function and is harmless.  To
take advantage of this postential optimization simply enclose each
group of calls to the drawing primitives within calls to
`BeginDrawing` and `EndDrawing`.", "");
    
    DocDeclStr(
        void , EndDrawing(),
        "Ends the group of drawing primitives started with `BeginDrawing`, and
invokes whatever optimization is available for this DC type on the
current platform.", "");
    DocCtorStr(
        ~wxPseudoDC(),
        "Frees memory used by the operation list of the pseudo dc","");
    DocDeclStr(
        void, RemoveAll(),
        "Removes all objects and operations from the recorded list.", "");
    DocDeclStr(
        int, GetLen(),
        "Returns the number of operations in the recorded list.", "");
    DocDeclStr(
        void, SetId(int id),
        "Sets the id to be associated with subsequent operations.", "");
    DocDeclStr(
        void, ClearId(int id),
        "Removes all operations associated with id so the object can be redrawn.", "");
    DocDeclStr(
        void, RemoveId(int id),
        "Remove the object node (and all operations) associated with an id.", "");
    DocDeclStr(
        void, TranslateId(int id, wxCoord dx, wxCoord dy),
        "Translate the operations of id by dx,dy.", "");
    DocDeclStr(
        void, SetIdGreyedOut(int id, bool greyout=true),
        "Set whether an object is drawn greyed out or not.", "");
    DocDeclStr(
        bool, GetIdGreyedOut(int id),
        "Get whether an object is drawn greyed out or not.", "");
    KeepGIL(FindObjects);
    DocDeclStr(
        PyObject*, FindObjects(wxCoord x, wxCoord y, 
                               wxCoord radius=1, const wxColor& bg=*wxWHITE),
        "Returns a list of all the id's that draw a pixel with color
not equal to bg within radius of (x,y).
Returns an empty list if nothing is found.  The list is in
reverse drawing order so list[0] is the top id.","");
    KeepGIL(FindObjectsByBBox);
    DocDeclStr(
        PyObject*, FindObjectsByBBox(wxCoord x, wxCoord y),
        "Returns a list of all the id's whose bounding boxes include (x,y).
Returns an empty list if nothing is found.  The list is in
reverse drawing order so list[0] is the top id.","");
    DocDeclStr(
        void, DrawIdToDC(int id, wxDC *dc),
        "Draw recorded operations of id to dc.", "");
    DocDeclStr(
        void, SetIdBounds(int id, wxRect& rect),
        "Set the bounding rect of a given object.  This will create 
an object node if one doesn't exist.", "");
    %extend {
        DocStr(
            GetIdBounds,
            "Returns the bounding rectangle previouly set with SetIdBounds.  If
no bounds have been set, it returns wx.Rect(0,0,0,0).", "");
        wxRect GetIdBounds(int id) {
            wxRect rect;
            self->GetIdBounds(id, rect);
            return rect;
        }
    }
    DocDeclStr(
        void, DrawToDCClipped(wxDC *dc, const wxRect& rect),
        "Draws the recorded operations to dc unless the operation is known to
be outside rect.", "");
    DocDeclStr(
        void, DrawToDCClippedRgn(wxDC *dc, const wxRegion& region),
        "Draws the recorded operations to dc unless the operation is known to
be outside rect.", "");
    DocDeclStr(
        void, DrawToDC(wxDC *dc),
        "Draws the recorded operations to dc.", "");
    
    //-------------------------------------------------------------------------
    // Methods Mirrored from wxDC
    //-------------------------------------------------------------------------
//
//    DocDeclStr(
//        void , SetFont(const wxFont& font),
//        "Sets the current font for the DC. It must be a valid font, in
//particular you should not pass ``wx.NullFont`` to this method.","
//
//:see: `wx.Font`");
//    
//    DocDeclStr(
//        void , SetPen(const wxPen& pen),
//        "Sets the current pen for the DC.
//
//If the argument is ``wx.NullPen``, the current pen is selected out of the
//device context, and the original pen restored.", "
//
//:see: `wx.Pen`");
//    
//    DocDeclStr(
//        void , SetBrush(const wxBrush& brush),
//        "Sets the current brush for the DC.
//
//If the argument is ``wx.NullBrush``, the current brush is selected out
//of the device context, and the original brush restored, allowing the
//current brush to be destroyed safely.","
//
//:see: `wx.Brush`");
//    
//    DocDeclStr(
//        void , SetBackground(const wxBrush& brush),
//        "Sets the current background brush for the DC.", "");
//    
//    DocDeclStr(
//        void , SetBackgroundMode(int mode),
//        "*mode* may be one of ``wx.SOLID`` and ``wx.TRANSPARENT``. This setting
//determines whether text will be drawn with a background colour or
//not.", "");
//    
//    DocDeclStr(
//        void , SetTextForeground(const wxColour& colour),
//        "Sets the current text foreground colour for the DC.", "");
//    
//    DocDeclStr(
//        void , SetTextBackground(const wxColour& colour),
//        "Sets the current text background colour for the DC.", "");
//    DocDeclStr(
//        void , DrawLabel(const wxString& text, const wxRect& rect,
//                         int alignment = wxALIGN_LEFT | wxALIGN_TOP,
//                         int indexAccel = -1),
//        "Draw *text* within the specified rectangle, abiding by the alignment
//flags.  Will additionally emphasize the character at *indexAccel* if
//it is not -1.", "
//
//:see: `DrawImageLabel`");
//    DocStr(
//        DrawText,
//        "Draws a text string at the specified point, using the current text
//font, and the current text foreground and background colours.
//
//The coordinates refer to the top-left corner of the rectangle bounding
//the string. See `GetTextExtent` for how to get the dimensions of a
//text string, which can be used to position the text more precisely.
//
//**NOTE**: under wxGTK the current logical function is used by this
//function but it is ignored by wxMSW. Thus, you should avoid using
//logical functions with this function in portable programs.", "
//
//:see: `DrawRotatedText`");
//    void DrawText(const wxString& text, wxCoord x, wxCoord y);
//    DocStr(
//        DrawLine,
//        "Draws a line from the first point to the second. The current pen is
//used for drawing the line. Note that the second point is *not* part of
//the line and is not drawn by this function (this is consistent with
//the behaviour of many other toolkits).", "");
//    void DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);
//    DocDeclStr(
//        void, DrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height),
//        "Draws a rectangle with the given top left corner, and with the given
//size. The current pen is used for the outline and the current brush
//for filling the shape.", "");
//    DocDeclStr(
//        void , Clear(),
//        "Clears the device context using the current background brush.", "");

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
    void FloodFill(wxCoord x, wxCoord y, const wxColour& col, int style = wxFLOOD_SURFACE);
    %Rename(FloodFillPoint, void, FloodFill(const wxPoint& pt, const wxColour& col, int style = wxFLOOD_SURFACE));

    
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
the string. See `wx.DC.GetTextExtent` for how to get the dimensions of
a text string, which can be used to position the text more precisely,
(you will need to use a real DC with GetTextExtent as wx.PseudoDC does
not implement it.)

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


//    DocDeclStr(
//        bool , Blit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
//                    wxDC *source, wxCoord xsrc, wxCoord ysrc,
//                    int rop = wxCOPY, bool useMask = false,
//                    wxCoord xsrcMask = -1, wxCoord ysrcMask = -1),
//        "Copy from a source DC to this DC.  Parameters specify the destination
//coordinates, size of area to copy, source DC, source coordinates,
//logical function, whether to use a bitmap mask, and mask source
//position.", "
//
//    :param xdest:       Destination device context x position.
//    :param ydest:       Destination device context y position.
//    :param width:       Width of source area to be copied.
//    :param height:      Height of source area to be copied.
//    :param source:      Source device context.
//    :param xsrc:        Source device context x position.
//    :param ysrc:        Source device context y position.
//    :param rop:         Logical function to use: see `SetLogicalFunction`.
//    :param useMask:     If true, Blit does a transparent blit using the mask
//                        that is associated with the bitmap selected into the
//                        source device context.
//    :param xsrcMask:    Source x position on the mask. If both xsrcMask and
//                        ysrcMask are -1, xsrc and ysrc will be assumed for
//                        the mask source position.
//    :param ysrcMask:    Source y position on the mask. 
//");
    
//    DocDeclStrName(
//        bool , Blit(const wxPoint& destPt, const wxSize& sz,
//                    wxDC *source, const wxPoint& srcPt,
//                    int rop = wxCOPY, bool useMask = false,
//                    const wxPoint& srcPtMask = wxDefaultPosition),
//        "Copy from a source DC to this DC.  Parameters specify the destination
//coordinates, size of area to copy, source DC, source coordinates,
//logical function, whether to use a bitmap mask, and mask source
//position.", "
//
//    :param destPt:      Destination device context position.
//    :param sz:          Size of source area to be copied.
//    :param source:      Source device context.
//    :param srcPt:       Source device context position.
//    :param rop:         Logical function to use: see `SetLogicalFunction`.
//    :param useMask:     If true, Blit does a transparent blit using the mask
//                        that is associated with the bitmap selected into the
//                        source device context.
//    :param srcPtMask:   Source position on the mask. 
//",
//        BlitPointSize);
    

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

    DocDeclStrName(
        void, DrawLabel(const wxString& text,
                              const wxBitmap& image,
                              const wxRect& rect,
                              int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                              int indexAccel = -1),
       "Draw *text* and an image (which may be ``wx.NullBitmap`` to skip
drawing it) within the specified rectangle, abiding by the alignment
flags.  Will additionally emphasize the character at *indexAccel* if
it is not -1.", "", DrawImageLabel);

    
    DocDeclAStr(
        void , DrawSpline(int points, wxPoint* points_array),
        "DrawSpline(self, List points)",
        "Draws a spline between all given control points, (a list of `wx.Point`
objects) using the current pen. The spline is drawn using a series of
lines, using an algorithm taken from the X drawing program 'XFIG'.", "");


    DocDeclStr(
        virtual void , Clear(),
        "Clears the device context using the current background brush.", "");
    

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
        virtual void , SetTextForeground(const wxColour& colour),
        "Sets the current text foreground colour for the DC.", "");
    
    DocDeclStr(
        virtual void , SetTextBackground(const wxColour& colour),
        "Sets the current text background colour for the DC.", "");
    
    DocDeclStr(
        virtual void , SetLogicalFunction(int function),
        "Sets the current logical function for the device context. This
determines how a source pixel (from a pen or brush colour, combines
with a destination pixel in the current device context.

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

    
    %property(IdBounds, GetIdBounds, SetIdBounds, doc="See `GetIdBounds` and `SetIdBounds`");
    %property(Len, GetLen, doc="See `GetLen`");
};
