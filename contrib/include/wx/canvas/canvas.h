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
    wxCanvasObject( int x, int y, int width, int height );
    
    virtual void Move( int x, int y );
    virtual void Render( int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );
    
    wxCanvas   *GetOwner()              { return m_owner; }
    void SetOwner( wxCanvas *owner )    { m_owner = owner; }
    
    bool        IsControl()     { return m_isControl; }
    bool        IsVector()      { return m_isVector; }
    bool        IsImage()       { return m_isImage; }
    inline int  GetX()          { return m_area.x; }
    inline int  GetY()          { return m_area.y; }
    inline int  GetWidth()      { return m_area.width; }
    inline int  GetHeight()     { return m_area.height; }

protected:
    wxCanvas   *m_owner;
    bool        m_isControl;
    bool        m_isVector;
    bool        m_isImage;
    wxRect      m_area;
    
    friend class wxCanvas;
};

//----------------------------------------------------------------------------
// wxCanvasImage
//----------------------------------------------------------------------------

class wxCanvasImage: public wxCanvasObject
{
public:
    wxCanvasImage( const wxImage &image, int x, int y );
    
    virtual void Render( int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );
    
private:
    wxImage     m_image;
};

//----------------------------------------------------------------------------
// wxCanvasControl
//----------------------------------------------------------------------------

class wxCanvasControl: public wxCanvasObject
{
public:
    wxCanvasControl( wxWindow *control );
    ~wxCanvasControl();
    
    virtual void Move( int x, int y );
    void UpdateSize();
    
private:
    wxWindow     *m_control;
};

//----------------------------------------------------------------------------
// wxCanvasText
//----------------------------------------------------------------------------

class wxCanvasText: public wxCanvasObject
{
public:
    wxCanvasText( const wxString &text, int x, int y, const wxString &foneFile, int size );
    ~wxCanvasText();
    
    virtual void Render( int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );
    
    void CreateBuffer();
    void SetRGB( unsigned char red, unsigned char green, unsigned char blue );
    void SetFlag( int flag );
    
private:
    wxString        m_text;
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
    virtual void Update( int x, int y, int width, int height );
    virtual void UpdateNow();
    
    virtual void Prepend( wxCanvasObject* obj );
    virtual void Append( wxCanvasObject* obj );
    virtual void Insert( size_t before, wxCanvasObject* obj );
    virtual void Remove( wxCanvasObject* obj );
    
    wxImage *GetBuffer()         { return &m_buffer; }
    bool NeedUpdate()            { return m_needUpdate; }
    
    void BlitBuffer( wxDC &dc );
    
private:
    wxImage        m_buffer;
    bool           m_needUpdate;
    wxList         m_updateRects;
    wxList         m_objects;
    unsigned char  m_green,m_red,m_blue;
    
    friend class wxCanvasObject;
    
private:
    void OnChar( wxKeyEvent &event );
    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnSize( wxSizeEvent &event );
    void OnIdle( wxIdleEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );

private:
    DECLARE_CLASS(wxCanvas)
    DECLARE_EVENT_TABLE()
};


#endif
    // WXCANVAS

