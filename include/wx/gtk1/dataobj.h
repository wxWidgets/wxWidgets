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

class wxDataObject;
class wxTextDataObject;
class wxBitmapDataObject;
class wxPrivateDataObject;
class wxFileDataObject;

//-------------------------------------------------------------------------
// wxDataObject
//-------------------------------------------------------------------------

class wxDataObject: public wxObject
{
  DECLARE_ABSTRACT_CLASS( wxDataObject )

public:

  wxDataObject() {}
  ~wxDataObject() {}

  virtual wxDataFormat GetFormat() const = 0;
  
  // implementation
  
  GdkAtom  m_formatAtom;
};

// ----------------------------------------------------------------------------
// wxTextDataObject is a specialization of wxDataObject for text data
// ----------------------------------------------------------------------------

class wxTextDataObject : public wxDataObject
{
  DECLARE_DYNAMIC_CLASS( wxTextDataObject )

public:

  wxTextDataObject() {}
  wxTextDataObject( const wxString& strText ) 
    : m_strText(strText) { }
  
  virtual wxDataFormat GetFormat() const
    { return wxDF_TEXT; }
    
  void SetText( const wxString& strText) 
    { m_strText = strText; }
    
  wxString GetText() const
    { return m_strText; }

private:
  wxString  m_strText;

};

// ----------------------------------------------------------------------------
// wxFileDataObject is a specialization of wxDataObject for file names
// ----------------------------------------------------------------------------

class wxFileDataObject : public wxDataObject
{
  DECLARE_DYNAMIC_CLASS( wxFileDataObject )

public:

  wxFileDataObject(void) {}
  
  virtual wxDataFormat GetFormat() const
    { return wxDF_FILENAME; }
    
  void AddFile( const wxString &file )
    { m_files += file; m_files += (char)0; }
    
  wxString GetFiles() const
    { return m_files; }
    
private:
  wxString  m_files;
  
};

// ----------------------------------------------------------------------------
// wxBitmapDataObject is a specialization of wxDataObject for bitmaps
// ----------------------------------------------------------------------------

class wxBitmapDataObject : public wxDataObject
{
  DECLARE_DYNAMIC_CLASS( wxBitmapDataObject )

public:

  wxBitmapDataObject(void) {}
  
  wxBitmapDataObject( const wxBitmap& bitmap ) { m_bitmap = bitmap; }
  
  virtual wxDataFormat GetFormat() const
    { return wxDF_BITMAP; }
    
  void SetBitmap( const wxBitmap &bitmap )
    { m_bitmap = bitmap; }
    
  wxBitmap GetBitmap() const
    { return m_bitmap; }
    
private:
  wxBitmap  m_bitmap;
};

// ----------------------------------------------------------------------------
// wxPrivateDataObject is a specialization of wxDataObject for app specific data
// ----------------------------------------------------------------------------

class wxPrivateDataObject : public wxDataObject
{
  DECLARE_DYNAMIC_CLASS( wxPrivateDataObject )

public:

  wxPrivateDataObject();
    
  ~wxPrivateDataObject();
  
  virtual wxDataFormat GetFormat() const
    { return wxDF_PRIVATE; }
    
  // the string ID identifies the format of clipboard or DnD data. a word
  // processor would e.g. add a wxTextDataObject and a wxPrivateDataObject
  // to the clipboard - the latter with the Id "WXWORD_FORMAT".
    
  void SetId( const wxString& id )
    { m_id = id; }
    
  wxString GetId() const
    { return m_id; }

  // will make internal copy
  void SetData( const char *data, size_t size );
    
  size_t GetDataSize() const
    { return m_size; }
    
  char* GetData() const
    { return m_data; }
    
private:
  size_t     m_size;
  char*      m_data;
  wxString   m_id;
};


#endif  
       //__GTKDNDH__

