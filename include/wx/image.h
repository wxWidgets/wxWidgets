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
class WXDLLEXPORT wxImage;

//-----------------------------------------------------------------------------
// wxImageHandler
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxImageHandler: public wxObject
{
  DECLARE_CLASS(wxImageHandler)

public:
  wxImageHandler() { m_name = ""; m_extension = ""; m_type = 0; }

#if wxUSE_STREAMS
  virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE, int index=0 );
  virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );

  virtual int GetImageCount( wxInputStream& stream );

  bool CanRead( wxInputStream& stream ) { return DoCanRead(stream); }
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
#endif // wxUSE_STREAMS

  wxString  m_name;
  wxString  m_extension;
  wxString  m_mime;
  long      m_type;
};


//-----------------------------------------------------------------------------
// wxImage
//-----------------------------------------------------------------------------


// GRG: Dic/99
class WXDLLEXPORT wxHNode
{
public:
    unsigned long index;
    unsigned long value;
};


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
  wxImage& Rescale( int width, int height ) { return *this = Scale(width, height); }

  // these routines are slow but safe
  void SetRGB( int x, int y, unsigned char r, unsigned char g, unsigned char b );
  unsigned char GetRed( int x, int y );
  unsigned char GetGreen( int x, int y );
  unsigned char GetBlue( int x, int y );

  static bool CanRead( const wxString& name );
  virtual bool LoadFile( const wxString& name, long type = wxBITMAP_TYPE_ANY );
  virtual bool LoadFile( const wxString& name, const wxString& mimetype );

#if wxUSE_STREAMS
  static bool CanRead( wxInputStream& stream );
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

  // GRG: Dic/99
  unsigned long CountColours( unsigned long stopafter = -1 );
  unsigned long ComputeHistogram( wxHashTable &h );


protected:

  static wxList sm_handlers;

};


extern void WXDLLEXPORT wxInitAllImageHandlers();


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

#endif
  // _WX_IMAGE_H_

