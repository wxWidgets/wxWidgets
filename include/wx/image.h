/////////////////////////////////////////////////////////////////////////////
// Name:        image.h
// Purpose:     wxImage class
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:   	wxWindows licence
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

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxImageHandler;
#ifdef wxUSE_LIBPNG
class WXDLLEXPORT wxPNGHandler;
#endif
class WXDLLEXPORT wxBMPHandler;
class WXDLLEXPORT wxImage;

class WXDLLEXPORT wxBitmap;

//-----------------------------------------------------------------------------
// wxImageHandler
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxImageHandler: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxImageHandler)
  
public:
  wxImageHandler() { m_name = ""; m_extension = ""; m_type = 0; }

  virtual bool LoadFile( wxImage *image, const wxString& name );
  virtual bool SaveFile( wxImage *image, const wxString& name );

  inline void SetName(const wxString& name) { m_name = name; }
  inline void SetExtension(const wxString& ext) { m_extension = ext; }
  inline void SetType(long type) { m_type = type; }
  inline wxString GetName() const { return m_name; }
  inline wxString GetExtension() const { return m_extension; }
  inline long GetType() const { return m_type; }
  
protected:
  wxString  m_name;
  wxString  m_extension;
  long      m_type;
  
};

//-----------------------------------------------------------------------------
// wxPNGHandler
//-----------------------------------------------------------------------------

#ifdef wxUSE_LIBPNG
class WXDLLEXPORT wxPNGHandler: public wxImageHandler
{
  DECLARE_DYNAMIC_CLASS(wxPNGHandler)
  
public:

  inline wxPNGHandler()
  {
      m_name = "PNG file";
      m_extension = "png";
      m_type = wxBITMAP_TYPE_PNG;
  };

  virtual bool LoadFile( wxImage *image, const wxString& name );
  virtual bool SaveFile( wxImage *image, const wxString& name );
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
  };

  virtual bool LoadFile( wxImage *image, const wxString& name );
};

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
  wxImage( const wxString& name, long type = wxBITMAP_TYPE_PNG );
  
  wxImage( const wxImage& image );
  wxImage( const wxImage* image );
  
  // these functions get implemented in /src/(platform)/bitmap.cpp 
  wxImage( const wxBitmap &bitmap );
  wxBitmap ConvertToBitmap() const;

  void Create( int width, int height );
  void Destroy();
  
  wxImage Scale( int width, int height );
  
  // these routines are slow but safe  
  void SetRGB( int x, int y, unsigned char r, unsigned char g, unsigned char b );
  unsigned char GetRed( int x, int y );
  unsigned char GetGreen( int x, int y );
  unsigned char GetBlue( int x, int y );
  
  virtual bool LoadFile( const wxString& name, long type = wxBITMAP_TYPE_PNG );
  virtual bool SaveFile( const wxString& name, int type );

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

  inline wxImage& operator = (const wxImage& image) 
    { if (*this == image) return (*this); Ref(image); return *this; }
  inline bool operator == (const wxImage& image) 
    { return m_refData == image.m_refData; }
  inline bool operator != (const wxImage& image) 
    { return m_refData != image.m_refData; }

  static inline wxList& GetHandlers() { return sm_handlers; }
  static void AddHandler( wxImageHandler *handler );
  static void InsertHandler( wxImageHandler *handler );
  static bool RemoveHandler( const wxString& name );
  static wxImageHandler *FindHandler( const wxString& name );
  static wxImageHandler *FindHandler( const wxString& extension, long imageType );
  static wxImageHandler *FindHandler( long imageType );
  
  static void CleanUpHandlers();
  static void InitStandardHandlers();

protected:

  static wxList sm_handlers;
  
};

#endif
  // _WX_IMAGE_H_
