///////////////////////////////////////////////////////////////////////////////
// Name:        dataobj.h
// Purpose:     declaration of the wxDataObject class
// Author:      Julian Smart
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DATAOBJ_H_
#define _WX_DATAOBJ_H_

#ifdef __GNUG__
#pragma interface "dataobj.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/bitmap.h"

//-------------------------------------------------------------------------
// classes
//-------------------------------------------------------------------------

class WXDLLEXPORT wxDataObject;
class WXDLLEXPORT wxTextDataObject;
class WXDLLEXPORT wxBitmapDataObject;
class WXDLLEXPORT wxPrivateDataObject;
class WXDLLEXPORT wxFileDataObject;

//-------------------------------------------------------------------------
// wxDataObject
//-------------------------------------------------------------------------

class WXDLLEXPORT wxDataObject: public wxObject
{
  DECLARE_ABSTRACT_CLASS( wxDataObject )

public:

  wxDataObject() {}
  ~wxDataObject() {}

  virtual wxDataFormat GetFormat() const = 0;
  
  // implementation
};

// ----------------------------------------------------------------------------
// wxTextDataObject is a specialization of wxDataObject for text data
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTextDataObject : public wxDataObject
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

class WXDLLEXPORT wxFileDataObject : public wxDataObject
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

class WXDLLEXPORT wxBitmapDataObject : public wxDataObject
{
  DECLARE_DYNAMIC_CLASS( wxBitmapDataObject )

public:

  wxBitmapDataObject(void) {}
  
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

class WXDLLEXPORT wxPrivateDataObject : public wxDataObject
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
       //_WX_DATAOBJ_H_

