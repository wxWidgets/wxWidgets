///////////////////////////////////////////////////////////////////////////////
// Name:        dataobj.h
// Purpose:     declaration of the wxDataObject class
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin, Robert Roebling
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef __GTKDATAOBJECTH__
#define __GTKDATAOBJECTH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/bitmap.h"

//-------------------------------------------------------------------------
// classes
//-------------------------------------------------------------------------

class wxDataFormat;
class wxDataBroker;
class wxDataObject;
class wxTextDataObject;
class wxBitmapDataObject;
class wxPrivateDataObject;
class wxFileDataObject;

//-------------------------------------------------------------------------
// wxDataFormat
//-------------------------------------------------------------------------

enum wxDataType
{
  wxDF_INVALID =          0,
  wxDF_TEXT =             1,  /* CF_TEXT */
  wxDF_BITMAP =           2,  /* CF_BITMAP */
  wxDF_METAFILE =         3,  /* CF_METAFILEPICT */
  wxDF_SYLK =             4,
  wxDF_DIF =              5,
  wxDF_TIFF =             6,
  wxDF_OEMTEXT =          7,  /* CF_OEMTEXT */
  wxDF_DIB =              8,  /* CF_DIB */
  wxDF_PALETTE =          9,
  wxDF_PENDATA =          10,
  wxDF_RIFF =             11,
  wxDF_WAVE =             12,
  wxDF_UNICODETEXT =      13,
  wxDF_ENHMETAFILE =      14,
  wxDF_FILENAME =         15, /* CF_HDROP */
  wxDF_LOCALE =           16,
  wxDF_PRIVATE =          20
};

class wxDataFormat : public wxObject
{
  DECLARE_CLASS( wxDataFormat )
  
public:
  
  wxDataFormat();
  wxDataFormat( wxDataType type );
  wxDataFormat( const wxString &id );
  wxDataFormat( wxDataFormat &format );
  wxDataFormat( const GdkAtom atom );

  void SetType( wxDataType type );    
  wxDataType GetType() const;
  
  wxString GetId() const;
  void SetId( const wxString &id );
  
  GdkAtom GetAtom();
      
private:

  wxDataType  m_type;
  wxString    m_id;
  bool        m_hasAtom;
  GdkAtom     m_atom;
};

//-------------------------------------------------------------------------
// wxDataBroker handles data and ormat negotiation for clipboard and DnD
//-------------------------------------------------------------------------

class wxDataBroker : public wxObject
{
  DECLARE_CLASS( wxDataBroker )

public:

  /* constructor */
  wxDataBroker();
  
  /* add data object */
  void Add( wxDataObject *dataObject, bool preferred = FALSE );
  
private:  
  
  /* OLE implementation, the methods don't need to be overridden */
  
  /* get number of supported formats */
  virtual size_t GetFormatCount() const;
  
  /* return nth supported format */    
  virtual wxDataFormat &GetNthFormat( size_t nth ) const;
    
  /* return preferrd/best supported format */    
  virtual wxDataFormat &GetPreferredFormat() const;
    
  /* search through m_dataObjects, return TRUE if found */
  virtual bool IsSupportedFormat( wxDataFormat &format ) const;
  
  /* search through m_dataObjects and call child's GetSize() */
  virtual size_t GetSize( wxDataFormat& format ) const;
    
  /* search through m_dataObjects and call child's WriteData(dest) */
  virtual void WriteData( wxDataFormat& format, void *dest ) const;
  
  /* implementation */

public:
  
  wxList    m_dataObjects;
  size_t    m_preferred;
};

//----------------------------------------------------------------------------
// wxDataObject to be placed in wxDataBroker
//----------------------------------------------------------------------------

class wxDataObject : public wxObject
{
  DECLARE_DYNAMIC_CLASS( wxDataObject )
  
public:

  /* constructor */
  wxDataObject();
  
  /* destructor */
  ~wxDataObject();
  
  /* write data to dest */  
  virtual void WriteData( void *dest ) const = 0;
 
  /* get size of data */ 
  virtual size_t GetSize() const = 0;
  
  /* implementation */
  
  wxDataFormat &GetFormat();
  
  wxDataType GetFormatType() const;
  wxString   GetFormatId() const;
  GdkAtom    GetFormatAtom() const;
  
  wxDataFormat m_format;
};

//----------------------------------------------------------------------------
// wxTextDataObject is a specialization of wxDataObject for text data
//----------------------------------------------------------------------------

class wxTextDataObject : public wxDataObject
{
  DECLARE_DYNAMIC_CLASS( wxTextDataObject )

public:

  /* default constructor. call SetText() later or override
     WriteData() and GetSize() for working on-demand */
  wxTextDataObject();
  
  /* constructor */
  wxTextDataObject( const wxString& data );
  
  /* set current text data */
  void SetText( const wxString& data );
    
  /* get current text data */
  wxString GetText() const;

  /* by default calls WriteString() with string set by constructor or
     by SetText(). can be overridden for working on-demand */
  virtual void WriteData( void *dest ) const;
  
  /* by default, returns length of string as set by constructor or 
     by SetText(). can be overridden for working on-demand */
  virtual size_t GetSize() const;
  
  /* write string to dest */
  void WriteString( const wxString &str, void *dest ) const;
  
  /* implementation */

  wxString  m_data;
};

//----------------------------------------------------------------------------
// wxFileDataObject is a specialization of wxDataObject for file names
//----------------------------------------------------------------------------

class wxFileDataObject : public wxDataObject
{
  DECLARE_DYNAMIC_CLASS( wxFileDataObject )

public:

  /* default constructor */
  wxFileDataObject();
    
  /* add file name to list */
  void AddFile( const wxString &file );
    
  /* get all filename as one string. each file name is 0 terminated,
     the list is double zero terminated */
  wxString GetFiles() const;
    
  /* write list of filenames */
  virtual void WriteData( void *dest ) const;

  /* return length of list of filenames */  
  virtual size_t GetSize() const;
  
  /* implementation */

  wxString  m_files;
};

//----------------------------------------------------------------------------
// wxBitmapDataObject is a specialization of wxDataObject for bitmaps
//----------------------------------------------------------------------------

class wxBitmapDataObject : public wxDataObject
{
  DECLARE_DYNAMIC_CLASS( wxBitmapDataObject )

public:

  /* see wxTextDataObject for explanation */

  wxBitmapDataObject();
  wxBitmapDataObject( const wxBitmap& bitmap );
  
  void SetBitmap( const wxBitmap &bitmap );
  wxBitmap GetBitmap() const;
  
  virtual void WriteData( void *dest ) const;
  virtual size_t GetSize() const;
  
  void WriteBitmap( const wxBitmap &bitmap, void *dest ) const;
    
  // implementation

  wxBitmap  m_bitmap;
  
};

//----------------------------------------------------------------------------
// wxPrivateDataObject is a specialization of wxDataObject for app specific data
//----------------------------------------------------------------------------

class wxPrivateDataObject : public wxDataObject
{
  DECLARE_DYNAMIC_CLASS( wxPrivateDataObject )

public:

  /* see wxTextDataObject for explanation of functions */
  
  wxPrivateDataObject();
  ~wxPrivateDataObject();
  
  /* the string Id identifies the format of clipboard or DnD data. a word
   * processor would e.g. add a wxTextDataObject and a wxPrivateDataObject
   * to the clipboard - the latter with the Id "application/wxword", an
   * image manipulation program would put a wxBitmapDataObject and a
   * wxPrivateDataObject to the clipboard - the latter with "image/png". */
    
  void SetId( const wxString& id );
    
  /* get id */
  wxString GetId() const;

  /* set data. will make internal copy. */
  void SetData( const char *data, size_t size );
    
  /* returns pointer to data */
  char* GetData() const;
  
  virtual void WriteData( void *dest ) const;
  virtual size_t GetSize() const;
  
  void WriteData( const char *data, void *dest ) const;
    
  // implementation

  size_t     m_size;
  char*      m_data;
  wxString   m_id;
};


#endif  
       //__GTKDNDH__

