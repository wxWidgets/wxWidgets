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

#ifdef __GNUG__
#pragma interface "image.h"
#endif

#include "wx/setup.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdicmn.h"
#include "wx/bitmap.h"

#if wxUSE_STREAMS
#  include "wx/stream.h"
#endif

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxImageHandler;
#if wxUSE_LIBPNG
class WXDLLEXPORT wxPNGHandler;
#endif
#if wxUSE_LIBJPEG
class WXDLLEXPORT wxJPEGHandler;
#endif
class WXDLLEXPORT wxBMPHandler;
#if wxUSE_GIF
class WXDLLEXPORT wxGIFHandler;
#endif
#if wxUSE_PNM
class WXDLLEXPORT wxPNMHandler;
#endif
#if wxUSE_PCX
class WXDLLEXPORT wxPCXHandler;
#endif
class WXDLLEXPORT wxImage;

//-----------------------------------------------------------------------------
// wxImageHandler
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxImageHandler: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxImageHandler)

public:
  wxImageHandler() { m_name = ""; m_extension = ""; m_type = 0; }

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE );
  virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );

  virtual bool CanRead( wxInputStream& stream );
  virtual bool CanRead( const wxString& name );
#endif

  inline void SetName(const wxString& name) { m_name = name; }
  inline void SetExtension(const wxString& ext) { m_extension = ext; }
  inline void SetType(long type) { m_type = type; }
  inline void SetMimeType(const wxString& type) { m_mime = type; }
  inline wxString GetName() const { return m_name; }
  inline wxString GetExtension() const { return m_extension; }
  inline long GetType() const { return m_type; }
  inline wxString GetMimeType() const { return m_mime; }

protected:
  wxString  m_name;
  wxString  m_extension;
  wxString  m_mime;
  long      m_type;

};

//-----------------------------------------------------------------------------
// wxPNGHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBPNG
class WXDLLEXPORT wxPNGHandler: public wxImageHandler
{
  DECLARE_DYNAMIC_CLASS(wxPNGHandler)

public:

  inline wxPNGHandler()
  {
      m_name = "PNG file";
      m_extension = "png";
      m_type = wxBITMAP_TYPE_PNG;
      m_mime = "image/png";
  };

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE );
  virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
  virtual bool CanRead( wxInputStream& stream );
private:
   // hiding base class virtuals again!
  inline bool CanRead( const wxString& name ) { return(wxImageHandler::CanRead(name)); };
#endif
};
#endif

//-----------------------------------------------------------------------------
// wxJPEGHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBJPEG
class WXDLLEXPORT wxJPEGHandler: public wxImageHandler
{
  DECLARE_DYNAMIC_CLASS(wxJPEGHandler)

public:

  inline wxJPEGHandler()
  {
      m_name = "JPEG file";
      m_extension = "jpg";
      m_type = wxBITMAP_TYPE_JPEG;
      m_mime = "image/jpeg";
  };

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE );
  virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
  virtual bool CanRead( wxInputStream& stream );
private:
   // hiding base class virtuals again!
  inline bool CanRead( const wxString& name ) { return(wxImageHandler::CanRead(name)); };
#endif
};
#endif

//-----------------------------------------------------------------------------
// wxBMPHandler
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxBMPHandler: public wxImageHandler
{
  DECLARE_DYNAMIC_CLASS(wxBMPHandler)

public:

  inline wxBMPHandler()
  {
      m_name = "BMP file";
      m_extension = "bmp";
      m_type = wxBITMAP_TYPE_BMP;
      m_mime = "image/bmp";
  };

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE );
  virtual bool CanRead( wxInputStream& stream );
private:
   // hiding base class virtuals again!
  inline bool CanRead( const wxString& name ) { return(wxImageHandler::CanRead(name)); };
#endif
};

//-----------------------------------------------------------------------------
// wxGIFHandler
//-----------------------------------------------------------------------------

#if wxUSE_GIF

class WXDLLEXPORT wxGIFHandler : public wxImageHandler
{
  DECLARE_DYNAMIC_CLASS(wxGIFHandler)

public:

  inline wxGIFHandler()
  {
      m_name = "GIF file";
      m_extension = "gif";
      m_type = wxBITMAP_TYPE_GIF;
      m_mime = "image/gif";
  };

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE );
  virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
  virtual bool CanRead( wxInputStream& stream );
private:
   // hiding base class virtuals again!
  inline bool CanRead( const wxString& name ) { return(wxImageHandler::CanRead(name)); };
#endif
};
#endif

//-----------------------------------------------------------------------------
// wxPNMHandler
//-----------------------------------------------------------------------------

#if wxUSE_PNM
class WXDLLEXPORT wxPNMHandler : public wxImageHandler
{
  DECLARE_DYNAMIC_CLASS(wxPNMHandler)

public:

  inline wxPNMHandler()
  {
      m_name = "PNM file";
      m_extension = "pnm";
      m_type = wxBITMAP_TYPE_PNM;
      m_mime = "image/pnm";
  };

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE );
  virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
  virtual bool CanRead( wxInputStream& stream );
private:
   // hiding base class virtuals again!
  inline bool CanRead( const wxString& name ) { return(wxImageHandler::CanRead(name)); };
#endif
};
#endif

//-----------------------------------------------------------------------------
// wxPCXHandler
//-----------------------------------------------------------------------------

#if wxUSE_PCX
class WXDLLEXPORT wxPCXHandler : public wxImageHandler
{
  DECLARE_DYNAMIC_CLASS(wxPCXHandler)

public:

  inline wxPCXHandler()
  {
      m_name = "PCX file";
      m_extension = "pcx";
      m_type = wxBITMAP_TYPE_PCX;
      m_mime = "image/pcx";
  };

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE );
  virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
  virtual bool CanRead( wxInputStream& stream );
#endif
};
#endif

//-----------------------------------------------------------------------------
// wxImage
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxImage: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxImage)

  friend class WXDLLEXPORT wxImageHandler;

public:

  wxImage();
  wxImage( int width, int height );
  wxImage( const wxString& name, long type = wxBITMAP_TYPE_ANY );
  wxImage( wxInputStream& stream, long type = wxBITMAP_TYPE_ANY );
  wxImage( const wxString& name, const wxString& mimetype );
  wxImage( wxInputStream& stream, const wxString& mimetype );

  wxImage( const wxImage& image );
  wxImage( const wxImage* image );

  // these functions get implemented in /src/(platform)/bitmap.cpp
  wxImage( const wxBitmap &bitmap );
  operator wxBitmap() const { return ConvertToBitmap(); }
  wxBitmap ConvertToBitmap() const;

  void Create( int width, int height );
  void Destroy();

  // return the new image with size width*height
  wxImage GetSubImage( const wxRect& ) const;

  // return the new image with size width*height
  wxImage Scale( int width, int height ) const;

  // rescales the image in place
  void Rescale( int width, int height ) { *this = Scale(width, height); }

  // these routines are slow but safe
  void SetRGB( int x, int y, unsigned char r, unsigned char g, unsigned char b );
  unsigned char GetRed( int x, int y );
  unsigned char GetGreen( int x, int y );
  unsigned char GetBlue( int x, int y );

  virtual bool LoadFile( const wxString& name, long type = wxBITMAP_TYPE_ANY );
  virtual bool LoadFile( const wxString& name, const wxString& mimetype );

#if wxUSE_STREAMS
  virtual bool LoadFile( wxInputStream& stream, long type = wxBITMAP_TYPE_ANY );
  virtual bool LoadFile( wxInputStream& stream, const wxString& mimetype );
#endif

  virtual bool SaveFile( const wxString& name, int type );
  virtual bool SaveFile( const wxString& name, const wxString& mimetype );

#if wxUSE_STREAMS
  virtual bool SaveFile( wxOutputStream& stream, int type );
  virtual bool SaveFile( wxOutputStream& stream, const wxString& mimetype );
#endif

  bool Ok() const;
  int GetWidth() const;
  int GetHeight() const;

  char unsigned *GetData() const;
  void SetData( char unsigned *data );

  void SetMaskColour( unsigned char r, unsigned char g, unsigned char b );
  unsigned char GetMaskRed() const;
  unsigned char GetMaskGreen() const;
  unsigned char GetMaskBlue() const;
  void SetMask( bool mask = TRUE );
  bool HasMask() const;

  wxImage& operator = (const wxImage& image)
  {
    if ( (*this) != image )
      Ref(image);
    return *this;
  }

  bool operator == (const wxImage& image)
    { return m_refData == image.m_refData; }
  bool operator != (const wxImage& image)
    { return m_refData != image.m_refData; }

  static wxList& GetHandlers() { return sm_handlers; }
  static void AddHandler( wxImageHandler *handler );
  static void InsertHandler( wxImageHandler *handler );
  static bool RemoveHandler( const wxString& name );
  static wxImageHandler *FindHandler( const wxString& name );
  static wxImageHandler *FindHandler( const wxString& extension, long imageType );
  static wxImageHandler *FindHandler( long imageType );
  static wxImageHandler *FindHandlerMime( const wxString& mimetype );

  static void CleanUpHandlers();
  static void InitStandardHandlers();
  static void InitAllHandlers();

protected:

  static wxList sm_handlers;

};

#endif
  // _WX_IMAGE_H_

