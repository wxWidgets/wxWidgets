/////////////////////////////////////////////////////////////////////////////
// Name:        pseudodc.h
// Purpose:     wxPseudoDC class
// Author:      Paul Lanier
// Modified by:
// Created:     05/25/06
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_PSUEDO_DC_H_BASE_
#define _WX_PSUEDO_DC_H_BASE_

//----------------------------------------------------------------------------
// Base class for all pdcOp classes
//----------------------------------------------------------------------------
class pdcOp
{
    public:
        // Constructor and Destructor
        pdcOp() {}
        virtual ~pdcOp() {}

        // Virtual Drawing Methods
        virtual void DrawToDC(wxDC *dc, bool grey=false)=0;
        virtual void Translate(wxCoord WXUNUSED(dx), wxCoord WXUNUSED(dy)) {}
        virtual void CacheGrey() {}
};

//----------------------------------------------------------------------------
// declare a list class for list of pdcOps
//----------------------------------------------------------------------------
WX_DECLARE_LIST(pdcOp, pdcOpList);


//----------------------------------------------------------------------------
// Helper functions used for drawing greyed out versions of objects
//----------------------------------------------------------------------------
wxColour &MakeColourGrey(const wxColour &c);
wxBrush &GetGreyBrush(wxBrush &brush);
wxPen &GetGreyPen(wxPen &pen);
wxIcon &GetGreyIcon(wxIcon &icon);
wxBitmap &GetGreyBitmap(wxBitmap &bmp);

//----------------------------------------------------------------------------
// Classes derived from pdcOp
// There is one class for each method mirrored from wxDC to wxPseudoDC
//----------------------------------------------------------------------------
class pdcSetFontOp : public pdcOp
{
    public:
        pdcSetFontOp(const wxFont& font) 
            {m_font=font;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) {dc->SetFont(m_font);}
    protected:
        wxFont m_font;
};

class pdcSetBrushOp : public pdcOp
{
    public:
        pdcSetBrushOp(const wxBrush& brush) 
            {m_greybrush=m_brush=brush;} 
        virtual void DrawToDC(wxDC *dc, bool grey=false) 
        {
            if (!grey) dc->SetBrush(m_brush);
            else dc->SetBrush(m_greybrush);
        }
        virtual void CacheGrey() {m_greybrush=GetGreyBrush(m_brush);}
    protected:
        wxBrush m_brush;
        wxBrush m_greybrush;
};

class pdcSetBackgroundOp : public pdcOp
{
    public:
        pdcSetBackgroundOp(const wxBrush& brush) 
            {m_greybrush=m_brush=brush;} 
        virtual void DrawToDC(wxDC *dc, bool grey=false)
        {
            if (!grey) dc->SetBackground(m_brush);
            else dc->SetBackground(m_greybrush);
        }
        virtual void CacheGrey() {m_greybrush=GetGreyBrush(m_brush);}
    protected:
        wxBrush m_brush;
        wxBrush m_greybrush;
};

class pdcSetPenOp : public pdcOp
{
    public:
        pdcSetPenOp(const wxPen& pen) 
            {m_greypen=m_pen=pen;}
        virtual void DrawToDC(wxDC *dc, bool grey=false)
        {
            if (!grey) dc->SetPen(m_pen);
            else dc->SetPen(m_greypen);
        }
        virtual void CacheGrey() {m_greypen=GetGreyPen(m_pen);}
    protected:
        wxPen m_pen;
        wxPen m_greypen;
};

class pdcSetTextBackgroundOp : public pdcOp
{
    public:
        pdcSetTextBackgroundOp(const wxColour& colour) 
            {m_colour=colour;}
        virtual void DrawToDC(wxDC *dc, bool grey=false) 
        {
            if (!grey) dc->SetTextBackground(m_colour);
            else dc->SetTextBackground(MakeColourGrey(m_colour));
        }
    protected:
        wxColour m_colour;
};

class pdcSetTextForegroundOp : public pdcOp
{
    public:
        pdcSetTextForegroundOp(const wxColour& colour) 
            {m_colour=colour;}
        virtual void DrawToDC(wxDC *dc, bool grey=false)
        {
            if (!grey) dc->SetTextForeground(m_colour);
            else dc->SetTextForeground(MakeColourGrey(m_colour));
        }
    protected:
        wxColour m_colour;
};

class pdcDrawRectangleOp : public pdcOp
{
    public:
        pdcDrawRectangleOp(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
            {m_x=x; m_y=y; m_w=w; m_h=h;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) {dc->DrawRectangle(m_x,m_y,m_w,m_h);}
        virtual void Translate(wxCoord dx, wxCoord dy) 
            {m_x+=dx;m_y+=dy;}
    protected:
        wxCoord m_x,m_y,m_w,m_h;
};

class pdcDrawLineOp : public pdcOp
{
    public:
        pdcDrawLineOp(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
            {m_x1=x1; m_y1=y1; m_x2=x2; m_y2=y2;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) {dc->DrawLine(m_x1,m_y1,m_x2,m_y2);}
        virtual void Translate(wxCoord dx, wxCoord dy) 
            {m_x1+=dx; m_y1+=dy; m_x2+=dx; m_y2+=dy;}
    protected:
        wxCoord m_x1,m_y1,m_x2,m_y2;
};

class pdcSetBackgroundModeOp : public pdcOp
{
    public:
        pdcSetBackgroundModeOp(int mode) {m_mode=mode;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) {dc->SetBackgroundMode(m_mode);}
    protected:
        int m_mode;
};

class pdcDrawTextOp : public pdcOp
{
    public:
        pdcDrawTextOp(const wxString& text, wxCoord x, wxCoord y)
            {m_text=text; m_x=x; m_y=y;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) {dc->DrawText(m_text, m_x, m_y);}
        virtual void Translate(wxCoord dx, wxCoord dy) 
            {m_x+=dx; m_y+=dy;}
    protected:
        wxString m_text;
        wxCoord m_x, m_y;
};

class pdcClearOp : public pdcOp
{
    public:
        pdcClearOp() {}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) {dc->Clear();}
};

class pdcBeginDrawingOp : public pdcOp
{
    public:
        pdcBeginDrawingOp() {}
        virtual void DrawToDC(wxDC *WXUNUSED(dc), bool WXUNUSED(grey)=false) {}
};

class pdcEndDrawingOp : public pdcOp
{
    public:
        pdcEndDrawingOp() {}
        virtual void DrawToDC(wxDC *WXUNUSED(dc), bool WXUNUSED(grey)=false) {}
};

class pdcFloodFillOp : public pdcOp
{
    public:
        pdcFloodFillOp(wxCoord x, wxCoord y, const wxColour& col,
                   int style) {m_x=x; m_y=y; m_col=col; m_style=style;}
        virtual void DrawToDC(wxDC *dc, bool grey=false) 
        {
            if (!grey) dc->FloodFill(m_x,m_y,m_col,m_style);
            else dc->FloodFill(m_x,m_y,MakeColourGrey(m_col),m_style);
        }
        virtual void Translate(wxCoord dx, wxCoord dy) 
            {m_x+=dx; m_y+=dy;}
    protected:
        wxCoord m_x,m_y;
        wxColour m_col;
        int m_style;
};

class pdcCrossHairOp : public pdcOp
{
    public:
        pdcCrossHairOp(wxCoord x, wxCoord y) {m_x=x; m_y=y;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) {dc->CrossHair(m_x,m_y);}
        virtual void Translate(wxCoord dx, wxCoord dy) 
            {m_x+=dx; m_y+=dy;}
    protected:
        wxCoord m_x,m_y;
};

class pdcDrawArcOp : public pdcOp
{
    public:
        pdcDrawArcOp(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2,
                         wxCoord xc, wxCoord yc) 
            {m_x1=x1; m_y1=y1; m_x2=x2; m_y2=y2; m_xc=xc; m_yc=yc;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) 
            {dc->DrawArc(m_x1,m_y1,m_x2,m_y2,m_xc,m_yc);}
        virtual void Translate(wxCoord dx, wxCoord dy) 
            {m_x1+=dx; m_x2+=dx; m_y1+=dy; m_y2+=dy;}
    protected:
        wxCoord m_x1,m_x2,m_xc;
        wxCoord m_y1,m_y2,m_yc;
};

class pdcDrawCheckMarkOp : public pdcOp
{
    public:
        pdcDrawCheckMarkOp(wxCoord x, wxCoord y,
                       wxCoord width, wxCoord height) 
            {m_x=x; m_y=y; m_w=width; m_h=height;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) 
            {dc->DrawCheckMark(m_x,m_y,m_w,m_h);}
        virtual void Translate(wxCoord dx, wxCoord dy) 
            {m_x+=dx; m_y+=dy;}
    protected:
        wxCoord m_x,m_y,m_w,m_h;
};

class pdcDrawEllipticArcOp : public pdcOp
{
    public:
        pdcDrawEllipticArcOp(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                         double sa, double ea) 
            {m_x=x; m_y=y; m_w=w; m_h=h; m_sa=sa; m_ea=ea;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) 
            {dc->DrawEllipticArc(m_x,m_y,m_w,m_h,m_sa,m_ea);}
        virtual void Translate(wxCoord dx, wxCoord dy) 
            {m_x+=dx; m_y+=dy;}
    protected:
        wxCoord m_x,m_y,m_w,m_h;
        double m_sa,m_ea;
};

class pdcDrawPointOp : public pdcOp
{
    public:
        pdcDrawPointOp(wxCoord x, wxCoord y) 
            {m_x=x; m_y=y;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) {dc->DrawPoint(m_x,m_y);}
        virtual void Translate(wxCoord dx, wxCoord dy) 
            {m_x+=dx; m_y+=dy;}
    protected:
        wxCoord m_x,m_y;
};

class pdcDrawRoundedRectangleOp : public pdcOp
{
    public:
        pdcDrawRoundedRectangleOp(wxCoord x, wxCoord y, wxCoord width, 
                                  wxCoord height, double radius) 
            {m_x=x; m_y=y; m_w=width; m_h=height; m_r=radius;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) 
            {dc->DrawRoundedRectangle(m_x,m_y,m_w,m_h,m_r);}
        virtual void Translate(wxCoord dx, wxCoord dy) 
            {m_x+=dx; m_y+=dy;}
    protected:
        wxCoord m_x,m_y,m_w,m_h;
        double m_r;
};

class pdcDrawEllipseOp : public pdcOp
{
    public:
        pdcDrawEllipseOp(wxCoord x, wxCoord y, wxCoord width, wxCoord height) 
            {m_x=x; m_y=y; m_w=width; m_h=height;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) {dc->DrawEllipse(m_x,m_y,m_w,m_h);}
        virtual void Translate(wxCoord dx, wxCoord dy) 
            {m_x+=dx; m_y+=dy;}
    protected:
        wxCoord m_x,m_y,m_w,m_h;
};

class pdcDrawIconOp : public pdcOp
{
    public:
        pdcDrawIconOp(const wxIcon& icon, wxCoord x, wxCoord y) 
            {m_icon=icon; m_x=x; m_y=y;}
        virtual void DrawToDC(wxDC *dc, bool grey=false) 
        {
            if (grey) dc->DrawIcon(m_greyicon,m_x,m_y);
            else dc->DrawIcon(m_icon,m_x,m_y);
        }
        virtual void CacheGrey() {m_greyicon=GetGreyIcon(m_icon);}
        virtual void Translate(wxCoord dx, wxCoord dy) 
            {m_x+=dx; m_y+=dy;}
    protected:
        wxIcon m_icon;
        wxIcon m_greyicon;
        wxCoord m_x,m_y;
};

class pdcDrawLinesOp : public pdcOp
{
    public:
        pdcDrawLinesOp(int n, wxPoint points[],
               wxCoord xoffset = 0, wxCoord yoffset = 0);
        virtual ~pdcDrawLinesOp();
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) 
            {dc->DrawLines(m_n,m_points,m_xoffset,m_yoffset);}
        virtual void Translate(wxCoord dx, wxCoord dy)
        { 
            for(int i=0; i<m_n; i++)
            {
                m_points[i].x+=dx; 
                m_points[i].y+=dy;
            }
        }
    protected:
        int m_n;
        wxPoint *m_points;
        wxCoord m_xoffset,m_yoffset;
};

class pdcDrawPolygonOp : public pdcOp
{
    public:
        pdcDrawPolygonOp(int n, wxPoint points[],
                     wxCoord xoffset = 0, wxCoord yoffset = 0,
                     int fillStyle = wxODDEVEN_RULE);
        virtual ~pdcDrawPolygonOp();
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) 
            {dc->DrawPolygon(m_n,m_points,m_xoffset,m_yoffset,m_fillStyle);}
        
        virtual void Translate(wxCoord dx, wxCoord dy)
        { 
            for(int i=0; i<m_n; i++)
            {
                m_points[i].x+=dx; 
                m_points[i].y+=dy;
            }
        }
    protected:
        int m_n;
        wxPoint *m_points;
        wxCoord m_xoffset,m_yoffset;
        int m_fillStyle;
};

class pdcDrawPolyPolygonOp : public pdcOp
{
    public:
        pdcDrawPolyPolygonOp(int n, int count[], wxPoint points[],
                         wxCoord xoffset = 0, wxCoord yoffset = 0,
                         int fillStyle = wxODDEVEN_RULE);
        virtual ~pdcDrawPolyPolygonOp();
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) 
            {dc->DrawPolyPolygon(m_n,m_count,m_points,
                    m_xoffset,m_yoffset,m_fillStyle);}
        virtual void Translate(wxCoord dx, wxCoord dy)
        { 
            for(int i=0; i<m_totaln; i++)
            {
                m_points[i].x += dx; 
                m_points[i].y += dy;
            }
        }
    protected:
        int m_n;
        int m_totaln;
        int *m_count;
        wxPoint *m_points;
        wxCoord m_xoffset, m_yoffset;
        int m_fillStyle;
};

class pdcDrawRotatedTextOp : public pdcOp
{
    public:
        pdcDrawRotatedTextOp(const wxString& text, wxCoord x, wxCoord y, double angle) 
            {m_text=text; m_x=x; m_y=y; m_angle=angle;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) 
            {dc->DrawRotatedText(m_text,m_x,m_y,m_angle);}
        virtual void Translate(wxCoord dx, wxCoord dy) 
            {m_x+=dx; m_y+=dy;}
    protected:
        wxString m_text;
        wxCoord m_x,m_y;
        double m_angle;
};

class pdcDrawBitmapOp : public pdcOp
{
    public:
        pdcDrawBitmapOp(const wxBitmap &bmp, wxCoord x, wxCoord y,
                        bool useMask = false) 
            {m_bmp=bmp; m_x=x; m_y=y; m_useMask=useMask;}
        virtual void DrawToDC(wxDC *dc, bool grey=false) 
        {
            if (grey) dc->DrawBitmap(m_greybmp,m_x,m_y,m_useMask);
            else dc->DrawBitmap(m_bmp,m_x,m_y,m_useMask);
        }
        virtual void CacheGrey() {m_greybmp=GetGreyBitmap(m_bmp);}
        virtual void Translate(wxCoord dx, wxCoord dy) 
            {m_x+=dx; m_y+=dy;}
    protected:
        wxBitmap m_bmp;
        wxBitmap m_greybmp;
        wxCoord m_x,m_y;
        bool m_useMask;
};

class pdcDrawLabelOp : public pdcOp
{
    public:
        pdcDrawLabelOp(const wxString& text,
                           const wxBitmap& image,
                           const wxRect& rect,
                           int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                           int indexAccel = -1)
            {m_text=text; m_image=image; m_rect=rect; 
             m_align=alignment; m_iAccel=indexAccel;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) 
            {dc->DrawLabel(m_text,m_image,m_rect,m_align,m_iAccel);}
        virtual void Translate(wxCoord dx, wxCoord dy) 
            {m_rect.x+=dx; m_rect.y+=dy;}
    protected:
        wxString m_text;
        wxBitmap m_image;
        wxRect m_rect;
        int m_align;
        int m_iAccel;
};

#if wxUSE_SPLINES
class pdcDrawSplineOp : public pdcOp
{
    public:
        pdcDrawSplineOp(int n, wxPoint points[]);
        virtual ~pdcDrawSplineOp();
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) {dc->DrawSpline(m_n,m_points);}
        virtual void Translate(wxCoord dx, wxCoord dy)
        {
            int i;
            for(i=0; i<m_n; i++)
                m_points[i].x+=dx; m_points[i].y+=dy;
        }
    protected:
        wxPoint *m_points;
        int m_n;
};
#endif // wxUSE_SPLINES

#if wxUSE_PALETTE
class pdcSetPaletteOp : public pdcOp
{
    public:
        pdcSetPaletteOp(const wxPalette& palette) {m_palette=palette;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) {dc->SetPalette(m_palette);}
    protected:
        wxPalette m_palette;
};
#endif // wxUSE_PALETTE

class pdcSetLogicalFunctionOp : public pdcOp
{
    public:
        pdcSetLogicalFunctionOp(int function) {m_function=function;}
        virtual void DrawToDC(wxDC *dc, bool WXUNUSED(grey)=false) {dc->SetLogicalFunction(m_function);}
    protected:
        int m_function;
};

//----------------------------------------------------------------------------
// pdcObject type to contain list of operations for each real (Python) object
//----------------------------------------------------------------------------
class pdcObject
{
    public:
        pdcObject(int id) 
            {m_id=id; m_bounded=false; m_oplist.DeleteContents(true);
             m_greyedout=false;}

        virtual ~pdcObject() {m_oplist.Clear();}
        
        // Protected Member Access
        void SetId(int id) {m_id=id;}
        int  GetId() {return m_id;}
        void   SetBounds(wxRect& rect) {m_bounds=rect; m_bounded=true;}
        wxRect GetBounds() {return m_bounds;}
        void SetBounded(bool bounded) {m_bounded=bounded;}
        bool IsBounded() {return m_bounded;}
        void SetGreyedOut(bool greyout=true);
        bool GetGreyedOut() {return m_greyedout;}
    
        // Op List Management Methods
        void Clear() {m_oplist.Clear();}
        void AddOp(pdcOp *op) 
        {
            m_oplist.Append(op);
            if (m_greyedout) op->CacheGrey();
        }
        int  GetLen() {return m_oplist.GetCount();}
        virtual void Translate(wxCoord dx, wxCoord dy);
        
        // Drawing Method
        virtual void DrawToDC(wxDC *dc);
    protected:
        int m_id; // id of object (associates this pdcObject
                  //               with a Python object with same id)
        wxRect m_bounds;  // bounding rect of this object
        bool m_bounded;   // true if bounds is valid, false by default
        pdcOpList m_oplist; // list of operations for this object
        bool m_greyedout; // if true then draw this object in greys only
};


//----------------------------------------------------------------------------
// Declare a wxList to hold all the objects.  List order reflects drawing
// order (Z order) and is the same order as objects are added to the list
//----------------------------------------------------------------------------
class pdcObjectList;
WX_DECLARE_LIST(pdcObject, pdcObjectList);


// ----------------------------------------------------------------------------
// wxPseudoDC class
// ----------------------------------------------------------------------------
// This is the actual PseudoDC class
// This class stores a list of recorded dc operations in m_list
// and plays them back to a real dc using DrawToDC or DrawToDCClipped.
// Drawing methods are mirrored from wxDC but add nodes to m_list 
// instead of doing any real drawing.
// ----------------------------------------------------------------------------
class wxPseudoDC : public wxObject
{
public:
    wxPseudoDC() 
        {m_currId=-1; m_lastObjNode=NULL; m_objectlist.DeleteContents(true);}
    ~wxPseudoDC();
    // ------------------------------------------------------------------------
    // List managment methods
    // 
    void RemoveAll();
    int GetLen();
    
    // ------------------------------------------------------------------------
    // methods for managing operations by ID
    // 
    // Set the Id for all subsequent operations (until SetId is called again)
    void SetId(int id) {m_currId = id;}
    // Remove all the operations associated with an id so it can be redrawn
    void ClearId(int id);
    // Remove the object node (and all operations) associated with an id
    void RemoveId(int id);
    // Set the bounding rect of a given object
    // This will create an object node if one doesn't exist
    void SetIdBounds(int id, wxRect& rect);
    void GetIdBounds(int id, wxRect& rect);
    // Translate all the operations for this id
    void TranslateId(int id, wxCoord dx, wxCoord dy);
    // Grey-out an object
    void SetIdGreyedOut(int id, bool greyout=true);
    bool GetIdGreyedOut(int id);
    // Find Objects at a point.  Returns Python list of id's
    // sorted in reverse drawing order (result[0] is top object)
    // This version looks at drawn pixels
    PyObject *FindObjects(wxCoord x, wxCoord y, 
                          wxCoord radius=1, const wxColor& bg=*wxWHITE);
    // This version only looks at bounding boxes
    PyObject *FindObjectsByBBox(wxCoord x, wxCoord y);

    // ------------------------------------------------------------------------
    // Playback Methods
    //
    // draw to dc but skip objects known to be outside of rect
    // This is a coarse level of clipping to speed things up 
    // when lots of objects are off screen and doesn't affect the dc level 
    // clipping
    void DrawToDCClipped(wxDC *dc, const wxRect& rect);
        void DrawToDCClippedRgn(wxDC *dc, const wxRegion& region);
    // draw to dc with no clipping (well the dc will still clip)
    void DrawToDC(wxDC *dc);
    // draw a single object to the dc
    void DrawIdToDC(int id, wxDC *dc);

    // ------------------------------------------------------------------------
    // Hit Detection Methods
    //
    // returns list of object with a drawn pixel within radius pixels of (x,y)
    // the list is in reverse draw order so last drawn is first in list
    // PyObject *HitTest(wxCoord x, wxCoord y, double radius)
    // returns list of objects whose bounding boxes include (x,y)
    // PyObject *HitTestBB(wxCoord x, wxCoord y)
    
        
    // ------------------------------------------------------------------------
    // Methods mirrored from wxDC
    //
    void FloodFill(wxCoord x, wxCoord y, const wxColour& col,
                   int style = wxFLOOD_SURFACE)
        {AddToList(new pdcFloodFillOp(x,y,col,style));}
    void FloodFill(const wxPoint& pt, const wxColour& col,
                   int style = wxFLOOD_SURFACE)
        { FloodFill(pt.x, pt.y, col, style); }

    void DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
        {AddToList(new pdcDrawLineOp(x1, y1, x2, y2));}
    void DrawLine(const wxPoint& pt1, const wxPoint& pt2)
        { DrawLine(pt1.x, pt1.y, pt2.x, pt2.y); }

    void CrossHair(wxCoord x, wxCoord y)
        {AddToList(new pdcCrossHairOp(x,y));}
    void CrossHair(const wxPoint& pt)
        { CrossHair(pt.x, pt.y); }

    void DrawArc(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2,
                 wxCoord xc, wxCoord yc)
        {AddToList(new pdcDrawArcOp(x1,y1,x2,y2,xc,yc));}
    void DrawArc(const wxPoint& pt1, const wxPoint& pt2, const wxPoint& centre)
        { DrawArc(pt1.x, pt1.y, pt2.x, pt2.y, centre.x, centre.y); }

    void DrawCheckMark(wxCoord x, wxCoord y,
                       wxCoord width, wxCoord height)
        {AddToList(new pdcDrawCheckMarkOp(x,y,width,height));}
    void DrawCheckMark(const wxRect& rect)
        { DrawCheckMark(rect.x, rect.y, rect.width, rect.height); }

    void DrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                         double sa, double ea)
        {AddToList(new pdcDrawEllipticArcOp(x,y,w,h,sa,ea));}
    void DrawEllipticArc(const wxPoint& pt, const wxSize& sz,
                         double sa, double ea)
        { DrawEllipticArc(pt.x, pt.y, sz.x, sz.y, sa, ea); }

    void DrawPoint(wxCoord x, wxCoord y)
        {AddToList(new pdcDrawPointOp(x,y));}
    void DrawPoint(const wxPoint& pt)
        { DrawPoint(pt.x, pt.y); }

    void DrawPolygon(int n, wxPoint points[],
                     wxCoord xoffset = 0, wxCoord yoffset = 0,
                     int fillStyle = wxODDEVEN_RULE)
        {AddToList(new pdcDrawPolygonOp(n,points,xoffset,yoffset,fillStyle));}

    void DrawPolyPolygon(int n, int count[], wxPoint points[],
                         wxCoord xoffset = 0, wxCoord yoffset = 0,
                         int fillStyle = wxODDEVEN_RULE)
        {AddToList(new pdcDrawPolyPolygonOp(n,count,points,xoffset,yoffset,fillStyle));}

    void DrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
        {AddToList(new pdcDrawRectangleOp(x, y, width, height));}
    void DrawRectangle(const wxPoint& pt, const wxSize& sz)
        { DrawRectangle(pt.x, pt.y, sz.x, sz.y); }
    void DrawRectangle(const wxRect& rect)
        { DrawRectangle(rect.x, rect.y, rect.width, rect.height); }

    void DrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height,
                              double radius)
        {AddToList(new pdcDrawRoundedRectangleOp(x,y,width,height,radius));}
    void DrawRoundedRectangle(const wxPoint& pt, const wxSize& sz,
                             double radius)
        { DrawRoundedRectangle(pt.x, pt.y, sz.x, sz.y, radius); }
    void DrawRoundedRectangle(const wxRect& r, double radius)
        { DrawRoundedRectangle(r.x, r.y, r.width, r.height, radius); }

    void DrawCircle(wxCoord x, wxCoord y, wxCoord radius)
        { DrawEllipse(x - radius, y - radius, 2*radius, 2*radius); }
    void DrawCircle(const wxPoint& pt, wxCoord radius)
        { DrawCircle(pt.x, pt.y, radius); }

    void DrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
        {AddToList(new pdcDrawEllipseOp(x,y,width,height));}
    void DrawEllipse(const wxPoint& pt, const wxSize& sz)
        { DrawEllipse(pt.x, pt.y, sz.x, sz.y); }
    void DrawEllipse(const wxRect& rect)
        { DrawEllipse(rect.x, rect.y, rect.width, rect.height); }

    void DrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
        {AddToList(new pdcDrawIconOp(icon,x,y));}
    void DrawIcon(const wxIcon& icon, const wxPoint& pt)
        { DrawIcon(icon, pt.x, pt.y); }

    void DrawLines(int n, wxPoint points[],
               wxCoord xoffset = 0, wxCoord yoffset = 0)
        {AddToList(new pdcDrawLinesOp(n,points,xoffset,yoffset));}

    void DrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                    bool useMask = false)
        {AddToList(new pdcDrawBitmapOp(bmp,x,y,useMask));}
    void DrawBitmap(const wxBitmap &bmp, const wxPoint& pt,
                    bool useMask = false)
        { DrawBitmap(bmp, pt.x, pt.y, useMask); }

    void DrawText(const wxString& text, wxCoord x, wxCoord y)
        {AddToList(new pdcDrawTextOp(text, x, y));}
    void DrawText(const wxString& text, const wxPoint& pt)
        { DrawText(text, pt.x, pt.y); }

    void DrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle)
        {AddToList(new pdcDrawRotatedTextOp(text,x,y,angle));}
    void DrawRotatedText(const wxString& text, const wxPoint& pt, double angle)
        { DrawRotatedText(text, pt.x, pt.y, angle); }

    // this version puts both optional bitmap and the text into the given
    // rectangle and aligns is as specified by alignment parameter; it also
    // will emphasize the character with the given index if it is != -1 
    void DrawLabel(const wxString& text,
                           const wxBitmap& image,
                           const wxRect& rect,
                           int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                           int indexAccel = -1)
        {AddToList(new pdcDrawLabelOp(text,image,rect,alignment,indexAccel));}

    void DrawLabel(const wxString& text, const wxRect& rect,
                   int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                   int indexAccel = -1)
        { DrawLabel(text, wxNullBitmap, rect, alignment, indexAccel); }

/*?????? I don't think that the source dc would stick around
    void Blit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
              wxDC *source, wxCoord xsrc, wxCoord ysrc,
              int rop = wxCOPY, bool useMask = false, wxCoord xsrcMask = wxDefaultCoord, wxCoord ysrcMask = wxDefaultCoord)
                {AddToList(new pdcBlitOp(xdest,ydest,width,height,source,xsrc,
                                         ysrc,rop,useMask,xsrcMask,ysrcMask));}
    void Blit(const wxPoint& destPt, const wxSize& sz,
              wxDC *source, const wxPoint& srcPt,
              int rop = wxCOPY, bool useMask = false, const wxPoint& srcPtMask = wxDefaultPosition)
    {
        Blit(destPt.x, destPt.y, sz.x, sz.y, source, srcPt.x, srcPt.y, 
             rop, useMask, srcPtMask.x, srcPtMask.y);
    }
??????*/

#if wxUSE_SPLINES
    void DrawSpline(int n, wxPoint points[])
        {AddToList(new pdcDrawSplineOp(n,points));}
#endif // wxUSE_SPLINES

#if wxUSE_PALETTE
    void SetPalette(const wxPalette& palette)
        {AddToList(new pdcSetPaletteOp(palette));}
#endif // wxUSE_PALETTE

    void SetLogicalFunction(int function)
        {AddToList(new pdcSetLogicalFunctionOp(function));}
    void SetFont(const wxFont& font) 
        {AddToList(new pdcSetFontOp(font));}
    void SetPen(const wxPen& pen)
        {AddToList(new pdcSetPenOp(pen));}
    void SetBrush(const wxBrush& brush)
        {AddToList(new pdcSetBrushOp(brush));}
    void SetBackground(const wxBrush& brush)
        {AddToList(new pdcSetBackgroundOp(brush));}
    void SetBackgroundMode(int mode)
        {AddToList(new pdcSetBackgroundModeOp(mode));}
    void SetTextBackground(const wxColour& colour)
        {AddToList(new pdcSetTextBackgroundOp(colour));}
    void SetTextForeground(const wxColour& colour)
        {AddToList(new pdcSetTextForegroundOp(colour));}

    void Clear()
        {AddToList(new pdcClearOp());}
    void BeginDrawing()
        {AddToList(new pdcBeginDrawingOp());}
    void EndDrawing()
        {AddToList(new pdcEndDrawingOp());}

protected:
    // ------------------------------------------------------------------------
    // protected helper methods
    void AddToList(pdcOp *newOp);
    pdcObjectList::Node *FindObjNode(int id, bool create=false);
    
    // ------------------------------------------------------------------------
    // Data members
    // 
    int m_currId; // id to use for operations done on the PseudoDC
    pdcObjectList::Node *m_lastObjNode; // used to find last used object quickly
    pdcObjectList m_objectlist; // list of objects
};

#endif

