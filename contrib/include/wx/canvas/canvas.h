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

class wxCanvas;

// WDR: class declarations

//----------------------------------------------------------------------------
// wxCanvasObject
//----------------------------------------------------------------------------

class wxCanvasObject: public wxEvtHandler
{
public:
    wxCanvasObject();

    // Area occupied by object. Used for clipping, intersection,
    // mouse enter etc. Screen coordinates
    void SetArea( int x, int y, int width, int height );
    void SetArea( wxRect rect );

    // These are for screen output only therefore use
    // int as coordinates.
    virtual bool IsHit( int x, int y, int margin = 0 );
    virtual void Render( int clip_x, int clip_y, int clip_width, int clip_height );

    // use doubles later
    virtual void Move( int x, int y );

    // Once we have world coordinates in doubles, this will get
    // called for every object if the world coordinate system
    // changes (zooming).
    virtual void Recreate();

    // Later...
    virtual void WriteSVG( wxTextOutputStream &stream );

    wxCanvas *GetOwner()              { return m_owner; }
    void SetOwner( wxCanvas *owner )  { m_owner = owner; }

    bool        IsControl()     { return m_isControl; }
    bool        IsVector()      { return m_isVector; }
    bool        IsImage()       { return m_isImage; }
    inline int  GetX()          { return m_area.x; }
    inline int  GetY()          { return m_area.y; }
    inline int  GetWidth()      { return m_area.width; }
    inline int  GetHeight()     { return m_area.height; }

    void CaptureMouse();
    void ReleaseMouse();
    bool IsCapturedMouse();

protected:
    wxCanvas   *m_owner;
    bool        m_isControl;
    bool        m_isVector;
    bool        m_isImage;
    wxRect      m_area;

    friend class wxCanvas;
};

//----------------------------------------------------------------------------
// wxCanvasRect
//----------------------------------------------------------------------------

class wxCanvasRect: public wxCanvasObject
{
public:
    wxCanvasRect( double x, double y, double w, double h,
                  unsigned char red, unsigned char green, unsigned char blue );

    virtual void Recreate();

    virtual void Render( int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );

private:
    double        m_x;
    double        m_y;
    double        m_width;
    double        m_height;

    unsigned char m_red;
    unsigned char m_green;
    unsigned char m_blue;
};

//----------------------------------------------------------------------------
// wxCanvasLine
//----------------------------------------------------------------------------

class wxCanvasLine: public wxCanvasObject
{
public:
    wxCanvasLine( double x1, double y1, double x2, double y2,
                  unsigned char red, unsigned char green, unsigned char blue );

    virtual void Recreate();

    virtual void Render( int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );
    
private:
    double        m_x1;
    double        m_y1;
    double        m_x2;
    double        m_y2;

    unsigned char m_red;
    unsigned char m_green;
    unsigned char m_blue;
};

//----------------------------------------------------------------------------
// wxCanvasImage
//----------------------------------------------------------------------------

class wxCanvasImage: public wxCanvasObject
{
public:
    wxCanvasImage( const wxImage &image, double x, double y, double w, double h );
    
    virtual void Recreate();
    
    virtual void Render( int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );
    
private:
    double      m_x;
    double      m_y;
    double      m_width;
    double      m_height;
    
    wxImage     m_image;
    wxImage     m_tmp;
};

//----------------------------------------------------------------------------
// wxCanvasControl
//----------------------------------------------------------------------------

class wxCanvasControl: public wxCanvasObject
{
public:
    wxCanvasControl( wxWindow *control );
    ~wxCanvasControl();
    
    virtual void Recreate();
    
    virtual void Move( int x, int y );
    
private:
    wxWindow     *m_control;
};

//----------------------------------------------------------------------------
// wxCanvasText
//----------------------------------------------------------------------------

class wxCanvasText: public wxCanvasObject
{
public:
    wxCanvasText( const wxString &text, double x, double y, const wxString &foneFile, int size );
    ~wxCanvasText();

    void Recreate();

    virtual void Render( int clip_x, int clip_y, int clip_width, int clip_height );
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

//----------------------------------------------------------------------------
// wxCanvas
//----------------------------------------------------------------------------

class wxCanvas: public wxScrolledWindow
{
public:
    // constructors and destructors
    wxCanvas( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle );
    virtual ~wxCanvas();

    virtual void SetArea( int width, int height );
    virtual void SetColour( unsigned char red, unsigned char green, unsigned char blue );
    virtual void Update( int x, int y, int width, int height, bool blit = TRUE );
    virtual void UpdateNow();

    virtual void Freeze();
    virtual void Thaw();

    virtual void Prepend( wxCanvasObject* obj );
    virtual void Append( wxCanvasObject* obj );
    virtual void Insert( size_t before, wxCanvasObject* obj );
    virtual void Remove( wxCanvasObject* obj );

    // override these to change your coordiate system ...
    virtual int GetDeviceX( double x );
    virtual int GetDeviceY( double y );
    virtual int GetDeviceWidth( double width );
    virtual int GetDeviceHeight( double height );

    // ... and call this to tell all objets to recreate then
    virtual void Recreate();

    inline wxImage *GetBuffer()  { return &m_buffer; }
    inline int GetBufferX()      { return m_bufferX; }
    inline int GetBufferY()      { return m_bufferY; }
    inline int GetBufferWidth()  { return m_buffer.GetWidth(); }
    inline int GetBufferHeight() { return m_buffer.GetHeight(); }
    
    bool NeedUpdate()            { return m_needUpdate; }
    bool IsFrozen()              { return m_frozen; }

    void BlitBuffer( wxDC &dc );

    void SetCaptureMouse( wxCanvasObject *obj );

    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = (wxRect *) NULL );

private:
    wxImage          m_buffer;
    int              m_bufferX;
    int              m_bufferY;
    bool             m_needUpdate;
    wxList           m_updateRects;
    wxList           m_objects;
    unsigned char    m_green,m_red,m_blue;
    bool             m_frozen;
    bool             m_requestNewBuffer;
    wxCanvasObject  *m_lastMouse;
    wxCanvasObject  *m_captureMouse;

    friend class wxCanvasObject;

private:
    void OnChar( wxKeyEvent &event );
    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnSize( wxSizeEvent &event );
    void OnIdle( wxIdleEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );
    void OnEraseBackground( wxEraseEvent &event );

private:
    DECLARE_CLASS(wxCanvas)
    DECLARE_EVENT_TABLE()
};


#endif
    // WXCANVAS

