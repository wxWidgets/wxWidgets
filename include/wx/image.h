/////////////////////////////////////////////////////////////////////////////
// Name:        image.h
// Purpose:     wxImage class
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGE_H_
#define _WX_IMAGE_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "image.h"
#endif

#include "wx/setup.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdicmn.h"
#include "wx/hashmap.h"

#if wxUSE_STREAMS
#  include "wx/stream.h"
#endif

#if wxUSE_IMAGE

// on some systems (Unixware 7.x) index is defined as a macro in the headers
// which breaks the compilation below
#undef index

#define wxIMAGE_OPTION_FILENAME wxString(_T("FileName"))

#define wxIMAGE_OPTION_RESOLUTION            wxString(_T("Resolution"))
#define wxIMAGE_OPTION_RESOLUTIONX           wxString(_T("ResolutionX"))
#define wxIMAGE_OPTION_RESOLUTIONY           wxString(_T("ResolutionY"))

#define wxIMAGE_OPTION_RESOLUTIONUNIT        wxString(_T("ResolutionUnit"))

// constants used with wxIMAGE_OPTION_RESOLUTIONUNIT
enum
{
    wxIMAGE_RESOLUTION_INCHES = 1,
    wxIMAGE_RESOLUTION_CM = 2
};

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxImageHandler;
class WXDLLEXPORT wxImage;
class WXDLLEXPORT wxPalette;

//-----------------------------------------------------------------------------
// wxImageHandler
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxImageHandler: public wxObject
{
public:
    wxImageHandler()
        : m_name(wxEmptyString), m_extension(wxEmptyString), m_mime(), m_type(0)
        { }

#if wxUSE_STREAMS
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=true, int index=-1 );
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=true );

    virtual int GetImageCount( wxInputStream& stream );

    bool CanRead( wxInputStream& stream ) { return CallDoCanRead(stream); }
    bool CanRead( const wxString& name );
#endif // wxUSE_STREAMS

    void SetName(const wxString& name) { m_name = name; }
    void SetExtension(const wxString& ext) { m_extension = ext; }
    void SetType(long type) { m_type = type; }
    void SetMimeType(const wxString& type) { m_mime = type; }
    wxString GetName() const { return m_name; }
    wxString GetExtension() const { return m_extension; }
    long GetType() const { return m_type; }
    wxString GetMimeType() const { return m_mime; }

protected:
#if wxUSE_STREAMS
    virtual bool DoCanRead( wxInputStream& stream ) = 0;

    // save the stream position, call DoCanRead() and restore the position
    bool CallDoCanRead(wxInputStream& stream);
#endif // wxUSE_STREAMS

    wxString  m_name;
    wxString  m_extension;
    wxString  m_mime;
    long      m_type;

private:
    DECLARE_CLASS(wxImageHandler)
};

//-----------------------------------------------------------------------------
// wxImageHistogram
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxImageHistogramEntry
{
public:
    wxImageHistogramEntry() { index = value = 0; }
    unsigned long index;
    unsigned long value;
};

WX_DECLARE_EXPORTED_HASH_MAP(unsigned long, wxImageHistogramEntry,
                             wxIntegerHash, wxIntegerEqual,
                             wxImageHistogramBase);

class WXDLLEXPORT wxImageHistogram : public wxImageHistogramBase
{
public:
    wxImageHistogram() : wxImageHistogramBase(256) { }

    // get the key in the histogram for the given RGB values
    static unsigned long MakeKey(unsigned char r,
                                 unsigned char g,
                                 unsigned char b)
    {
        return (r << 16) | (g << 8) | b;
    }

    // find first colour that is not used in the image and has higher
    // RGB values than RGB(startR, startG, startB)
    //
    // returns true and puts this colour in r, g, b (each of which may be NULL)
    // on success or returns false if there are no more free colours
    bool FindFirstUnusedColour(unsigned char *r,
                               unsigned char *g,
                               unsigned char *b,
                               unsigned char startR = 1,
                               unsigned char startG = 0,
                               unsigned char startB = 0 ) const;
};

//-----------------------------------------------------------------------------
// wxImage
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxImage: public wxObject
{
public:
    wxImage(){}
    wxImage( int width, int height, bool clear = true );
    wxImage( int width, int height, unsigned char* data, bool static_data = false );
    wxImage( int width, int height, unsigned char* data, unsigned char* alpha, bool static_data = false );
    wxImage( const wxString& name, long type = wxBITMAP_TYPE_ANY, int index = -1 );
    wxImage( const wxString& name, const wxString& mimetype, int index = -1 );

#if wxUSE_STREAMS
    wxImage( wxInputStream& stream, long type = wxBITMAP_TYPE_ANY, int index = -1 );
    wxImage( wxInputStream& stream, const wxString& mimetype, int index = -1 );
#endif // wxUSE_STREAMS

    wxImage( const wxImage& image );
    wxImage( const wxImage* image );

    bool Create( int width, int height, bool clear = true );
    bool Create( int width, int height, unsigned char* data, bool static_data = false );
    bool Create( int width, int height, unsigned char* data, unsigned char* alpha, bool static_data = false );
    void Destroy();

    // creates an identical copy of the image (the = operator
    // just raises the ref count)
    wxImage Copy() const;

    // return the new image with size width*height
    wxImage GetSubImage( const wxRect& ) const;

    // pastes image into this instance and takes care of
    // the mask colour and out of bounds problems
    void Paste( const wxImage &image, int x, int y );

    // return the new image with size width*height
    wxImage Scale( int width, int height ) const;

    wxImage ShrinkBy( int xFactor , int yFactor ) const ;

    // rescales the image in place
    wxImage& Rescale( int width, int height ) { return *this = Scale(width, height); }

    // Rotates the image about the given point, 'angle' radians.
    // Returns the rotated image, leaving this image intact.
    wxImage Rotate(double angle, const wxPoint & centre_of_rotation,
                   bool interpolating = true, wxPoint * offset_after_rotation = (wxPoint*) NULL) const;

    wxImage Rotate90( bool clockwise = true ) const;
    wxImage Mirror( bool horizontally = true ) const;

    // replace one colour with another
    void Replace( unsigned char r1, unsigned char g1, unsigned char b1,
                  unsigned char r2, unsigned char g2, unsigned char b2 );

    // convert to monochrome image (<r,g,b> will be replaced by white,
    // everything else by black)
    wxImage ConvertToMono( unsigned char r, unsigned char g, unsigned char b ) const;

    // these routines are slow but safe
    void SetRGB( int x, int y, unsigned char r, unsigned char g, unsigned char b );
    unsigned char GetRed( int x, int y ) const;
    unsigned char GetGreen( int x, int y ) const;
    unsigned char GetBlue( int x, int y ) const;

    void SetAlpha(int x, int y, unsigned char alpha);
    unsigned char GetAlpha(int x, int y) const;

    // find first colour that is not used in the image and has higher
    // RGB values than <startR,startG,startB>
    bool FindFirstUnusedColour( unsigned char *r, unsigned char *g, unsigned char *b,
                                unsigned char startR = 1, unsigned char startG = 0,
                                unsigned char startB = 0 ) const;
    // Set image's mask to the area of 'mask' that has <r,g,b> colour
    bool SetMaskFromImage(const wxImage & mask,
                          unsigned char mr, unsigned char mg, unsigned char mb);

    // converts image's alpha channel to mask, if it has any, does nothing
    // otherwise:
    bool ConvertAlphaToMask(unsigned char threshold = 128);

    // This method converts an image where the original alpha
    // information is only available as a shades of a colour
    // (actually shades of grey) typically when you draw anti-
    // aliased text into a bitmap. The DC drawinf routines
    // draw grey values on the black background although they
    // actually mean to draw white with differnt alpha values.
    // This method reverses it, assuming a black (!) background
    // and white text (actually only the red channel is read).
    // The method will then fill up the whole image with the
    // colour given.
    bool ConvertColourToAlpha( unsigned char r, unsigned char g, unsigned char b );

    static bool CanRead( const wxString& name );
    static int GetImageCount( const wxString& name, long type = wxBITMAP_TYPE_ANY );
    virtual bool LoadFile( const wxString& name, long type = wxBITMAP_TYPE_ANY, int index = -1 );
    virtual bool LoadFile( const wxString& name, const wxString& mimetype, int index = -1 );

#if wxUSE_STREAMS
    static bool CanRead( wxInputStream& stream );
    static int GetImageCount( wxInputStream& stream, long type = wxBITMAP_TYPE_ANY );
    virtual bool LoadFile( wxInputStream& stream, long type = wxBITMAP_TYPE_ANY, int index = -1 );
    virtual bool LoadFile( wxInputStream& stream, const wxString& mimetype, int index = -1 );
#endif

    virtual bool SaveFile( const wxString& name ) const;
    virtual bool SaveFile( const wxString& name, int type ) const;
    virtual bool SaveFile( const wxString& name, const wxString& mimetype ) const;

#if wxUSE_STREAMS
    virtual bool SaveFile( wxOutputStream& stream, int type ) const;
    virtual bool SaveFile( wxOutputStream& stream, const wxString& mimetype ) const;
#endif

    bool Ok() const;
    int GetWidth() const;
    int GetHeight() const;

    // these functions provide fastest access to wxImage data but should be
    // used carefully as no checks are done
    unsigned char *GetData() const;
    void SetData( unsigned char *data );
    void SetData( unsigned char *data, int new_width, int new_height );

    unsigned char *GetAlpha() const;    // may return NULL!
    bool HasAlpha() const { return GetAlpha() != NULL; }
    void SetAlpha(unsigned char *alpha = NULL);
    void InitAlpha();

    // Mask functions
    void SetMaskColour( unsigned char r, unsigned char g, unsigned char b );
    unsigned char GetMaskRed() const;
    unsigned char GetMaskGreen() const;
    unsigned char GetMaskBlue() const;
    void SetMask( bool mask = true );
    bool HasMask() const;

#if wxUSE_PALETTE
    // Palette functions
    bool HasPalette() const;
    const wxPalette& GetPalette() const;
    void SetPalette(const wxPalette& palette);
#endif // wxUSE_PALETTE

    // Option functions (arbitrary name/value mapping)
    void SetOption(const wxString& name, const wxString& value);
    void SetOption(const wxString& name, int value);
    wxString GetOption(const wxString& name) const;
    int GetOptionInt(const wxString& name) const;
    bool HasOption(const wxString& name) const;

    unsigned long CountColours( unsigned long stopafter = (unsigned long) -1 ) const;

    // Computes the histogram of the image and fills a hash table, indexed
    // with integer keys built as 0xRRGGBB, containing wxImageHistogramEntry
    // objects. Each of them contains an 'index' (useful to build a palette
    // with the image colours) and a 'value', which is the number of pixels
    // in the image with that colour.
    // Returned value: # of entries in the histogram
    unsigned long ComputeHistogram( wxImageHistogram &h ) const;

    wxImage& operator = (const wxImage& image)
    {
        if ( (*this) != image )
            Ref(image);
        return *this;
    }

    bool operator == (const wxImage& image) const
        { return m_refData == image.m_refData; }
    bool operator != (const wxImage& image) const
        { return m_refData != image.m_refData; }

    static wxList& GetHandlers() { return sm_handlers; }
    static void AddHandler( wxImageHandler *handler );
    static void InsertHandler( wxImageHandler *handler );
    static bool RemoveHandler( const wxString& name );
    static wxImageHandler *FindHandler( const wxString& name );
    static wxImageHandler *FindHandler( const wxString& extension, long imageType );
    static wxImageHandler *FindHandler( long imageType );
    static wxImageHandler *FindHandlerMime( const wxString& mimetype );

    static wxString GetImageExtWildcard();

    static void CleanUpHandlers();
    static void InitStandardHandlers();

protected:
    static wxList   sm_handlers;

private:
    friend class WXDLLEXPORT wxImageHandler;

    DECLARE_DYNAMIC_CLASS(wxImage)
};


extern void WXDLLEXPORT wxInitAllImageHandlers();

extern WXDLLEXPORT_DATA(wxImage)    wxNullImage;

//-----------------------------------------------------------------------------
// wxImage handlers
//-----------------------------------------------------------------------------

#include "wx/imagbmp.h"
#include "wx/imagpng.h"
#include "wx/imaggif.h"
#include "wx/imagpcx.h"
#include "wx/imagjpeg.h"
#include "wx/imagtiff.h"
#include "wx/imagpnm.h"
#include "wx/imagxpm.h"
#include "wx/imagiff.h"

#endif // wxUSE_IMAGE

#endif
  // _WX_IMAGE_H_

