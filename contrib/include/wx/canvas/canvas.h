/////////////////////////////////////////////////////////////////////////////
// Name:        canvas.h
// Author:      Robert Roebling
// Created:     XX/XX/XX
// Copyright:   2000 (c) Robert Roebling
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WXCANVAS_H__
#define __WXCANVAS_H__

#ifdef __GNUG__
    #pragma interface "canvas.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/txtstrm.h"
#include "wx/geometry.h"
#include "wx/matrix.h"
#include "bbox.h"


//----------------------------------------------------------------------------
// decls
//----------------------------------------------------------------------------

class wxCanvas;
class wxCanvasAdmin;

//----------------------------------------------------------------------------
// wxCanvasObject
//----------------------------------------------------------------------------
enum DRAGMODE {DRAG_RECTANGLE,DRAG_ONTOP,DRAG_REDRAW};

//:defenition
// wxCanvasObject is the base class for  Canvas Objects.
// All Objects for drawing one the canvas are derived from this class.
// It supports dragging and moving methods that can be used in derived
// classes for defining several ways of dragging.
// Also it is possible to plug in events handlers to do this for all derived classes at once.
//
// wxCanvasObjects have themselves as their event handlers by default,
// but their event handlers could be set to another object entirely. This
// separation can reduce the amount of derivation required, and allow
// alteration of a  wxCanvasObject functionality
class wxCanvasObject: public wxEvtHandler
{
public:

    wxCanvasObject();

    //If the position (x,y) is within the object this return a pointer to the object
    //Normally this function needs to be defined for each derived wxCanvasObject.
    //The default is a simple bounding box test.
    virtual wxCanvasObject* IsHitWorld( double x, double y, double margin = 0 );

    //render this object to the canvas (either on its buffer or directly on the canvas)
    //this depends on the wxDC that is set for the active canvas.
    virtual void Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height );

    //x position in world coordinates of the object
    virtual double  GetPosX()=0;
    //y position in world coordinates of the object
    virtual double  GetPosY()=0;

    //set position in world coordinates for the object
    virtual void  SetPosXY( double x, double y)=0;

    //absolute moving the object to position x,y in world coordinates
    //also does an update of the old and new area
    virtual void MoveAbsolute( double x, double y );

    //relative moving the object to position x,y in world coordinates
    //also does an update of the old and new area
    virtual void MoveRelative( double x, double y );

    //relative translate the object to position x,y in world coordinates
    //does NOT update the old and new area
    //this function must be defined for each derived object,
    //it is used internally for dragging and moving objects.
    virtual void TransLate( double x, double y )=0;

    //choose one of the three diffrenet drag methods |
    //DRAG_RECTANGLE = as a rectangle when drag is in progress |
    //DRAG_ONTOP = only redraw the object when dragging |
    //DRAG_REDRAW = redraw the damaged areas when dragging
    void SetDragMode(DRAGMODE mode) { m_dragmode=mode; };

    //return the dragmode
    DRAGMODE GetDragMode() { return m_dragmode; };

    //called when starting a drag
    virtual void DragStart();
    //called when dragging is in progress
    virtual void DragRelative( double x, double y);
    //called when dragging is ended
    virtual void DragEnd();

    //return the object if it is part of the given object or
    //if the given object is part of a group within this object.
    //For group objects this means recursively search for it.
    virtual wxCanvasObject* Contains( wxCanvasObject* obj );

    //calculate the boundingbox in world coordinates
    virtual void CalcBoundingBox()=0;

    //write the object as SVG (scalable vector grafhics
    virtual void WriteSVG( wxTextOutputStream &stream );

    //get the administrator for the object,
    //this will give access to the canvas's where it is displayed upon.
    //It is used to render the object to the active canvas.
    //Conversion from world to Device coordinates and visa versa is
    //done using the Active canvas at that moment.
    wxCanvasAdmin *GetAdmin()              { return m_admin; }

    //set the administrator
    virtual void SetAdmin( wxCanvasAdmin *admin )  { m_admin = admin; }

    //is this a control type of canvas object
    bool        IsControl()     { return m_isControl; }
    //is this a vector type of canvas object
    bool        IsVector()      { return m_isVector; }
    //is this an Image type of canvas object
    bool        IsImage()       { return m_isImage; }

    //get minimum X of the boundingbox in world coordinates
    inline double  GetXMin()     { return m_bbox.GetMinX(); }
    //get minimum Y of the boundingbox in world coordinates
    inline double  GetYMin()     { return m_bbox.GetMinY(); }
    //get maximum X of the boundingbox in world coordinates
    inline double  GetXMax()     { return m_bbox.GetMaxX(); }
    //get maximum Y of the boundingbox in world coordinates
    inline double  GetYMax()     { return m_bbox.GetMaxY(); }

    //get boundingbox
    inline wxBoundingBox GetBbox() { return m_bbox; }

    //redirect all mouse events for the canvas to this object
    void CaptureMouse();
    //release the mouse capture for this object
    void ReleaseMouse();
    //is the mouse captured for this object
    bool IsCapturedMouse();

    //set if this object will visible (be rendered or not)
    inline void SetVisible(bool visible) { m_visible=visible; }
    //get visibility
    inline bool GetVisible() {return m_visible; }

    //can the object be dragged
    inline void SetDraggable(bool drag) { m_dragable=drag; }
    //get if the object can be dragged
    inline bool GetDraggable() {return m_dragable; }

    //get absolute area in the device coordinates where the object
    //its boundingbox in world coordinates is first translated using the matrix.
    wxRect GetAbsoluteArea(const wxTransformMatrix& cworld);

    //get currently used eventhandler (always the first in the list)
    wxEvtHandler *GetEventHandler() const { return m_eventHandler; }

    //process an event for the object, starting with the first eventhandler
    // in the list.
    bool ProcessCanvasObjectEvent(wxEvent& event);

    // push/pop event handler: allows to chain a custom event handler to
    // already existing ones
    void PushEventHandler( wxEvtHandler *handler );

    //remove first eventhandler in the list (one will be always stay in)
    wxEvtHandler *PopEventHandler( bool deleteHandler = FALSE );
    //append an eventhandler to the list, this event handler will be called
    //if the other skipped the event to process.
    void AppendEventHandler(wxEvtHandler *handler);
    //remove last event handler in the list (one will always stay in)
    wxEvtHandler *RemoveLastEventHandler(bool deleteHandler);

protected:

    //administator for rendering and accessing the canvas's
    wxCanvasAdmin* m_admin;

    //active event handler, default the object itself
    wxEvtHandler* m_eventHandler;

    bool m_isControl:1;
    bool m_isVector:1;
    bool m_isImage:1;
    bool m_visible:1;
    bool m_dragable:1;
    DRAGMODE m_dragmode:2;

    //boundingbox in world coordinates
    wxBoundingBox m_bbox;

    //used for dragging
    wxBitmap m_atnewpos;
};

//:defenition
// wxCanvasObjectGroup is a container for wxCanvas derived Objects.
// It renders itself by calling the render methods of the wxCanvasObjects it contains.
// It can have nested groups also, in the same way as the other wxCanvasObjects it already contains.
// The group has a matrix to position/rotate/scale the group.
class wxCanvasObjectGroup: public wxCanvasObject
{
public:
    wxCanvasObjectGroup(double x, double y);
    virtual ~wxCanvasObjectGroup();

    void SetAdmin(wxCanvasAdmin* admin);

    //prepend a wxCanvasObject to this group
    virtual void Prepend( wxCanvasObject* obj );
    //append a wxCanvasObject to this group
    virtual void Append( wxCanvasObject* obj );
    //insert a wxCanvasObject to this group
    virtual void Insert( size_t before, wxCanvasObject* obj );
    //remove the given object from the group
    virtual void Remove( wxCanvasObject* obj );

    //those this group contain the given object.
    //in case of nested groups also search in there to the lowwest level.
    virtual wxCanvasObject* Contains( wxCanvasObject* obj );

    //returns index of the given wxCanvasObject in this group
    virtual int IndexOf( wxCanvasObject* obj );

    double  GetPosX() { return lworld.GetValue(2,0); }
    double  GetPosY() { return lworld.GetValue(2,1); }
    void    SetPosXY( double x, double y) {lworld.SetValue(2,0,x);lworld.SetValue(2,1,y);CalcBoundingBox();};

    void TransLate( double x, double y );

    void CalcBoundingBox();
    //remove all wxCanvasObjects from the group (flag for deletion of the objectsalso)
    void DeleteContents( bool );
    virtual void Render(wxTransformMatrix* cworld,int x, int y, int width, int height );
    virtual void WriteSVG( wxTextOutputStream &stream );

    //recursive call the IsHitWorld on all contained objects, the first
    //one that is hit will be returned
    wxCanvasObject* IsHitWorld( double x, double y, double margin = 0 );

    //recursive calls for contained objects to set eventhandlers,
    //and also sets its own eventhandler
    void PushEventHandler( wxEvtHandler *handler );
    //recursive calls for contained objects to set eventhandlers,
    //and also sets its own eventhandler
    wxEvtHandler *PopEventHandler( bool deleteHandler = FALSE );
    //recursive calls for contained objects to set eventhandlers,
    //and also sets its own eventhandler
    void AppendEventHandler(wxEvtHandler *handler);
    //recursive calls for contained objects to set eventhandlers,
    //and also sets its own eventhandler
    wxEvtHandler *RemoveLastEventHandler(bool deleteHandler);

protected:

    //to position the object
    wxTransformMatrix lworld;

    wxList        m_objects;

    friend class wxCanvas;
};

//:defenition
// wxCanvasObjectRef is a reference to any wxCanvasObject derived class.
// It does not duplicate the referenced object.
// It has a matrix to reposition/rotate/scale the object it references.
// The position/matrix of the referenced Object is accumulated with the one here.
class wxCanvasObjectRef: public wxCanvasObject
{
public:
    wxCanvasObjectRef(double x, double y,wxCanvasObject* obj);

    //set rotation for the reference
    void SetRotation(double rotation);

    //set scale in x and y ( > zero)
    void SetScale( double scalex, double scaley );

    void SetAdmin(wxCanvasAdmin* admin);

    double  GetPosX() { return lworld.GetValue(2,0); }
    double  GetPosY() { return lworld.GetValue(2,1); }
    void    SetPosXY( double x, double y) {lworld.SetValue(2,0,x);lworld.SetValue(2,1,y);CalcBoundingBox();};

    void TransLate( double x, double y );
    void CalcBoundingBox();
    virtual void Render(wxTransformMatrix* cworld,int x, int y, int width, int height );
    virtual void WriteSVG( wxTextOutputStream &stream );

    //return this object if one of the objects it references is hit
    wxCanvasObject* IsHitWorld( double x, double y, double margin = 0 );
    virtual wxCanvasObject* Contains( wxCanvasObject* obj );

    //recursive calls for contained objects to set eventhandlers,
    //and also sets its own eventhandler
    void PushEventHandler( wxEvtHandler *handler );
    //recursive calls for contained objects to set eventhandlers,
    //and also sets its own eventhandler
    wxEvtHandler *PopEventHandler( bool deleteHandler = FALSE );
    //recursive calls for contained objects to set eventhandlers,
    //and also sets its own eventhandler
    void AppendEventHandler(wxEvtHandler *handler);
    //recursive calls for contained objects to set eventhandlers,
    //and also sets its own eventhandler
    wxEvtHandler *RemoveLastEventHandler(bool deleteHandler);

protected:

    //to position the object
    wxTransformMatrix lworld;

    //reference to another wxCanvasObject
    wxCanvasObject* m_obj;
};

//:defenition
// wxCanvasRect
class wxCanvasRect: public wxCanvasObject
{
public:
    wxCanvasRect( double x, double y, double w, double h , double radius=0 );
    void SetBrush( const wxBrush& brush)  { m_brush = brush; };
    void SetPen( const wxPen& pen)        { m_pen = pen; CalcBoundingBox(); };

    double  GetPosX() { return m_x; }
    double  GetPosY() { return m_y; }
    void    SetPosXY( double x, double y) {m_x=x; m_y=y; CalcBoundingBox();};

    void TransLate( double x, double y );
    void CalcBoundingBox();

    virtual void Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );

private:
    wxPen         m_pen;
    wxBrush       m_brush;

    double        m_x;
    double        m_y;
    double        m_width;
    double        m_height;
    double        m_radius;
};

//----------------------------------------------------------------------------
// wxCanvasCircle
//----------------------------------------------------------------------------
class wxCanvasCircle: public wxCanvasObject
{
public:
    wxCanvasCircle( double x, double y, double radius );
    void SetBrush( const wxBrush& brush)  { m_brush = brush; };
    void SetPen( const wxPen& pen)        { m_pen = pen; CalcBoundingBox(); };

    double  GetPosX() { return m_x; }
    double  GetPosY() { return m_y; }
    void    SetPosXY( double x, double y) {m_x=x; m_y=y;CalcBoundingBox(); };

    void TransLate( double x, double y );

    void CalcBoundingBox();

    virtual void Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );

    wxCanvasObject* IsHitWorld( double x, double y, double margin = 0 );

private:
    wxPen         m_pen;
    wxBrush       m_brush;

    double        m_x;
    double        m_y;
    double        m_radius;
};

//:defenition
// wxCanvasEllipse
class wxCanvasEllipse: public wxCanvasObject
{
public:
    wxCanvasEllipse( double x, double y, double width, double height );
    void SetBrush( const wxBrush& brush)  { m_brush = brush; };
    void SetPen( const wxPen& pen)        { m_pen = pen; CalcBoundingBox(); };

    double  GetPosX() { return m_x; }
    double  GetPosY() { return m_y; }
    void    SetPosXY( double x, double y) {m_x=x; m_y=y; CalcBoundingBox();};

    void TransLate( double x, double y );

    void CalcBoundingBox();

    virtual void Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );

    wxCanvasObject* IsHitWorld( double x, double y, double margin = 0 );

private:
    wxPen         m_pen;
    wxBrush       m_brush;

    double        m_x;
    double        m_y;
    double        m_width;
    double        m_height;
};

//:defenition
// wxCanvasEllipticArc
class wxCanvasEllipticArc: public wxCanvasObject
{
public:
    wxCanvasEllipticArc( double x, double y, double width, double height, double start, double end );
    void SetBrush( const wxBrush& brush)  { m_brush = brush; };
    void SetPen( const wxPen& pen)        { m_pen = pen; CalcBoundingBox(); };

    double  GetPosX() { return m_x; }
    double  GetPosY() { return m_y; }
    void    SetPosXY( double x, double y) {m_x=x; m_y=y; CalcBoundingBox();};

    void TransLate( double x, double y );
    void CalcBoundingBox();

    virtual void Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );

    wxCanvasObject* IsHitWorld( double x, double y, double margin = 0 );

private:
    wxPen         m_pen;
    wxBrush       m_brush;

    double        m_x;
    double        m_y;
    double        m_width;
    double        m_height;
    double        m_start;
    double        m_end;
};

//:defenition
// wxCanvasLine
class wxCanvasLine: public wxCanvasObject
{
public:
    wxCanvasLine( double x1, double y1, double x2, double y2 );
    void SetPen( const wxPen& pen)    { m_pen = pen; CalcBoundingBox(); };


    double  GetPosX() { return m_x1; }
    double  GetPosY() { return m_y1; }
    void    SetPosXY( double x, double y) {m_x1=x; m_y1=y; CalcBoundingBox();};

    void TransLate( double x, double y );

    void CalcBoundingBox();

    virtual void Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );

    wxCanvasObject* IsHitWorld( double x, double y, double margin = 0 );

private:
    wxPen         m_pen;

    double        m_x1;
    double        m_y1;
    double        m_x2;
    double        m_y2;
};

//:defenition
// wxCanvasImage
class wxCanvasImage: public wxCanvasObject
{
public:
    wxCanvasImage( const wxImage &image, double x, double y, double w, double h );

    double  GetPosX() { return m_x; }
    double  GetPosY() { return m_y; }
    void    SetPosXY( double x, double y);

    void TransLate( double x, double y );

    void CalcBoundingBox();

    virtual void Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );

private:
    double      m_x;
    double      m_y;
    double      m_width;
    double      m_height;

    wxImage     m_image;
    int         m_orgw,m_orgh;
    
    // cache
    wxBitmap    m_cBitmap;
    wxImage     m_cImage;
    int         m_cW;
    int         m_cH;
    double      m_cR;
};

//----------------------------------------------------------------------------
// wxCanvasControl
//----------------------------------------------------------------------------

class wxCanvasControl: public wxCanvasObject
{
public:
    wxCanvasControl( wxWindow *control );
    ~wxCanvasControl();

    double  GetPosX();
    double  GetPosY();
    void    SetPosXY( double x, double y);

    void TransLate( double x, double y );
    void MoveRelative( double x, double y );

    void CalcBoundingBox();

private:
    wxWindow     *m_control;
};

//:defenition
// wxCanvasText
class wxCanvasText: public wxCanvasObject
{
public:
    wxCanvasText( const wxString &text, double x, double y, const wxString &foneFile, int size );
    ~wxCanvasText();

    double  GetPosX() { return m_x; }
    double  GetPosY() { return m_y; }
    void    SetPosXY( double x, double y) {m_x=x; m_y=y;CalcBoundingBox(); };

    void TransLate( double x, double y );

    void CalcBoundingBox();

    virtual void Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );

    void SetRGB( unsigned char red, unsigned char green, unsigned char blue );
    void SetFlag( int flag );
    int GetFlag()              { return m_flag; }

private:
    wxString        m_text;
    double          m_x;
    double          m_y;
    unsigned char  *m_alpha;
    void           *m_faceData;
    int             m_flag;
    int             m_red;
    int             m_green;
    int             m_blue;
    wxString        m_fontFileName;
    int             m_size;
};

//:defenition
// wxCanvas is used to display a wxCanvasGroupObject, which contains wxCanvasObject derived
// drawable objects. The group to draw is called the root.
// All objects are defined in world coordinates, relative to its parent (e.g. nested groups)
// There are methods to convert from world to device coordinates and visa versa.
// Rendering a draw is normally started on the root, it to a buffer, afterwords
// an update of the damaged parts will blitted from the buffer to the screen.
// This is done in Idle time, but can also be forced.
// World coordinates can be with the Y axis going up are down.
// The area of the drawing in world coordinates that is visible on the canvas
// can be set. Parts of this area can be zoomed into resulting in scroll bars
// to be displayed.
class wxCanvas: public wxScrolledWindow
{
public:
    // constructors and destructors
    wxCanvas( wxCanvasAdmin* admin ,wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle );
    virtual ~wxCanvas();

    //background colour for the canvas
    virtual  void SetColour( const wxColour& background );

    //update the area given in device coordinates
    virtual void Update( int x, int y, int width, int height, bool blit = TRUE );

    //blit all updated areas now to the screen, else it will happen in idle time.
    //Use this to support dragging for instance, becuase in such cases idle time
    //will take to long.
    virtual void UpdateNow();

    //prevent canvas activety
    virtual void Freeze();
    //allow canvas activety
    virtual void Thaw();

    //get the buffer that is used for rendering in general
    inline wxBitmap *GetBuffer() { return &m_buffer; }
    //get the DC that is used for rendering
    inline wxDC *GetDC()   { return m_renderDC; }
    //set the DC that is used for rendering
    inline void  SetDC(wxDC* dc)   { m_renderDC=dc; }

    inline int GetBufferWidth()  { return m_buffer.GetWidth(); }
    inline int GetBufferHeight() { return m_buffer.GetHeight(); }

    //updating is needed for the canvas if the buffer did change
    bool NeedUpdate()            { return m_needUpdate; }
    bool IsFrozen()              { return m_frozen; }

    //blit damaged areas in the buffer to the screen
    void BlitBuffer( wxDC &dc );

    //redirect events to this canvas object
    void SetCaptureMouse( wxCanvasObject *obj );
    //are events redirected, if so return the object else NULL
    inline wxCanvasObject* GetCaptured() { return m_captureMouse;}

    //set the root group where the objects for this canvas are stored
    void SetRoot(wxCanvasObjectGroup* aroot){m_root=aroot;}

    //get root group that is displayed on the canvas
    wxCanvasObjectGroup* GetRoot(){return m_root;}

    //scroll the window in device coordinates
    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = (wxRect *) NULL );

    //get y axis orientation
    virtual bool GetYaxis() { return FALSE; }
    
    //get the visible part in world coordinates
    virtual double GetMinX() const;
    virtual double GetMinY() const;
    virtual double GetMaxX() const;
    virtual double GetMaxY() const;

    //convert from window to virtual coordinates
    virtual double DeviceToLogicalX(int x) const;
    virtual double DeviceToLogicalY(int y) const;
    virtual double DeviceToLogicalXRel(int x) const;
    virtual double DeviceToLogicalYRel(int y) const;
    virtual int LogicalToDeviceX(double x) const;
    virtual int LogicalToDeviceY(double y) const;
    virtual int LogicalToDeviceXRel(double x) const;
    virtual int LogicalToDeviceYRel(double y) const;

protected:
    wxBitmap         m_buffer;

    //always available and m_buffer selected
    wxDC*            m_renderDC;
    
    bool             m_needUpdate;
    wxList           m_updateRects;
    wxCanvasObjectGroup* m_root;

    wxColour         m_background;
    bool             m_frozen;
    wxCanvasObject  *m_lastMouse;
    wxCanvasObject  *m_captureMouse;

    int              m_oldDeviceX,m_oldDeviceY;

    wxCanvasAdmin*   m_admin;
    
private:
    int              m_bufferX,m_bufferY;

protected:
    void OnMouse( wxMouseEvent &event );
    void OnPaint( wxPaintEvent &event );
    void OnSize( wxSizeEvent &event );
    void OnIdle( wxIdleEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );
    void OnEraseBackground( wxEraseEvent &event );

private:
    DECLARE_CLASS(wxCanvas)
    DECLARE_EVENT_TABLE()
};



class wxVectorCanvas: public wxCanvas
{
public:
    // constructors and destructors
    wxVectorCanvas( wxCanvasAdmin* admin ,wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle );

    //scroll the window in device coordinates
    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = (wxRect *) NULL );

    //set if the Yaxis goes up or down
    void SetYaxis(bool up) { m_yaxis=up; }

    //get currently used Yaxis setting
    virtual bool GetYaxis() { return m_yaxis; }

    //to set the total area in world coordinates that can be scrolled.
    // when totaly zoomed out (SetMappingScroll same size as given here),
    // this will be the area displayed.
    // To display all of a drawing, set this here to the boundingbox of the root group
    // of the canvas.
    void SetScroll(double vx1,double vy1,double vx2,double vy2);

    //given the virtual size to be displayed, the mappingmatrix will be calculated
    //in such a manner that it fits (same ratio in width and height) to the window size.
    //The window size is used to intitialize the mapping.
    //The virtual size is just an indication, it will be ajusted to fit in the client window ratio.
    //When border is set an extra margin is added so that the drawing will fit nicely.
    // To display all of a drawing, set this here to the boundingbox of the root group
    // of the canvas.
    void SetMappingScroll(double vx1,double vy1,double vx2,double vy2,bool border);

    //matrix for calculating the virtual coordinate given a screen coordinate
    wxTransformMatrix   GetInverseMappingMatrix();

    //matrix for calculating the screen coordinate given a virtual coordinate
    wxTransformMatrix   GetMappingMatrix();

    //get minimum X of the visible part in world coordinates
    virtual double GetMinX() const;
    virtual double GetMinY() const;
    virtual double GetMaxX() const;
    virtual double GetMaxY() const;
    
    //convert from window to virtual coordinates and back
    virtual double DeviceToLogicalX(int x) const;
    virtual double DeviceToLogicalY(int y) const;
    virtual double DeviceToLogicalXRel(int x) const;
    virtual double DeviceToLogicalYRel(int y) const;
    virtual int LogicalToDeviceX(double x) const;
    virtual int LogicalToDeviceY(double y) const;
    virtual int LogicalToDeviceXRel(double x) const;
    virtual int LogicalToDeviceYRel(double y) const;

protected:
    // up or down
    bool m_yaxis;

    // holds the matrix for mapping from virtual to screen coordinates
    wxTransformMatrix m_mapping_matrix;

    // holds the inverse of the mapping matrix
    wxTransformMatrix m_inverse_mapping;

    //virtual coordinates of total drawing
    double m_virtm_minX, m_virtm_minY, m_virtm_maxX, m_virtm_maxY;

    // virtual coordinates box
    double m_virt_minX, m_virt_minY, m_virt_maxX, m_virt_maxY;

    // bounding box
    double m_minX, m_minY, m_maxX, m_maxY;

    //are scroll bars active?
    bool m_scrolled;

private:
    void OnScroll(wxScrollWinEvent& event);
    void OnChar( wxKeyEvent &event );
    void OnSize( wxSizeEvent &event );

private:
    DECLARE_CLASS(wxVectorCanvas)
    DECLARE_EVENT_TABLE()
};



//:defenition
//Contains a list of wxCanvas Objects that will be maintained through this class.
//Each wxCanvasObject can be displayed on several wxCanvas Objects at the same time.
//The active wxCanvas is used to render and convert coordinates from world to device.
//So it is important to set the active wxCanvas based on the wxCanvas that has the focus
//or is scrolled etc. This is normally done within wxCanvas when appropriate.
class wxCanvasAdmin
{
public:
    // constructors and destructors
    wxCanvasAdmin();
    virtual ~wxCanvasAdmin();

    //convert from window to virtual coordinates
    double DeviceToLogicalX(int x) const;
    //convert from window to virtual coordinates
    double DeviceToLogicalY(int y) const;
    //convert from window to virtual coordinates relatif
    double DeviceToLogicalXRel(int x) const;
    //convert from window to virtual coordinates relatif
    double DeviceToLogicalYRel(int y) const;
    //convert from virtual to window coordinates
    int LogicalToDeviceX(double x) const;
    //convert from virtual to window coordinates
    int LogicalToDeviceY(double y) const;
    //convert from virtual to window coordinates relatif
    int LogicalToDeviceXRel(double x) const;
    //convert from virtual to window coordinates relatif
    int LogicalToDeviceYRel(double y) const;

    //update in the buffer off all canvases, the area given in world coordinates
    virtual void Update(wxCanvasObject* obj, double x, double y, double width, double height);

    //blit all updated areas now to the screen, else it will happen in idle time.
    //Use this to support dragging for instance, becuase in such cases idle time
    //will take to long.
    virtual void UpdateNow();

    //append another canvas
    virtual void Append( wxCanvas* canvas );

    //remove a canvas
    virtual void Remove( wxCanvas* canvas );

    //set the given canvas as active (for rendering, coordinate conversion etc.)
    void SetActive(wxCanvas* activate);

    //get active canvas
    inline wxCanvas* GetActive() {return m_active;};

private:
    wxList m_canvaslist;
    wxCanvas* m_active;
};

#endif
    // WXCANVAS

