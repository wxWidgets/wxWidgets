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

class wxDataFormat
{
public:
    // the clipboard formats under GDK are GdkAtoms
    typedef GdkAtom NativeFormat;
    
    wxDataFormat();
    wxDataFormat( wxDataFormatId type );
    wxDataFormat( const wxString &id );
    wxDataFormat( const wxChar *id );
    wxDataFormat( NativeFormat format );

    wxDataFormat& operator=(NativeFormat format)
        { SetId(format); return *this; }
    wxDataFormat& operator=(const wxDataFormat& format)
        { SetId(format); return *this; }

    // comparison (must have both versions)
    bool operator==(wxDataFormatId type) const
        { return m_type == (wxDataFormatId)type; }
    bool operator!=(wxDataFormatId type) const
        { return m_type != (wxDataFormatId)type; }
    bool operator==(NativeFormat format) const
        { return m_format == (NativeFormat)format; }
    bool operator!=(NativeFormat format) const
        { return m_format != (NativeFormat)format; }
    bool operator==(const wxDataFormat& format) const
        { return m_format == format.m_format; }
    bool operator!=(const wxDataFormat& format) const
        { return m_format != format.m_format; }

    // explicit and implicit conversions to NativeFormat which is one of
    // standard data types (implicit conversion is useful for preserving the
    // compatibility with old code)
    NativeFormat GetFormatId() const { return m_format; }
    operator NativeFormat() const { return m_format; }
    
    // this only works with standard ids
    void SetId( wxDataFormatId type );

    // this only works with standard ids
    void SetId( NativeFormat format );

    // string ids are used for custom types - this SetId() must be used for
    // application-specific formats
    wxString GetId() const;
    void SetId( const wxChar *id );

    // implementation
    wxDataFormatId GetType() const;

private:
    wxDataFormatId   m_type;
    NativeFormat     m_format;
    
    void PrepareFormats();
    void SetType( wxDataFormatId type );
};

//----------------------------------------------------------------------------
// wxDataObject
//----------------------------------------------------------------------------

class wxDataObject : public wxObject
{
public:
    wxDataObject();
    ~wxDataObject();
    
    virtual wxDataFormat GetPreferredFormat() const = 0;
    
        // get the number of formats we support: it is understood that if we
        // can accept data in some format, then we can render data in this
        // format as well, but the contrary is not necessarily true. For the
        // default value of the argument, all formats we support should be
        // returned, but if outputOnlyToo == FALSE, then we should only return
        // the formats which our SetData() understands
    virtual size_t GetFormatCount(bool outputOnlyToo = TRUE) const
        { return 1; }
	
        // return all formats in the provided array (of size GetFormatCount())
    virtual void GetAllFormats(wxDataFormat *formats,
                              bool outputOnlyToo = TRUE) const
        { formats[0] = GetPreferredFormat(); }
	
        // get the (total) size of data for the given format
    virtual size_t GetDataSize(const wxDataFormat& format) const = 0;
    
        // copy raw data (in the specified format) to provided pointer
    virtual bool GetDataHere(const wxDataFormat& format, void *buf) const = 0;
    
        // get data from the buffer (in the given format)
    virtual bool SetData(const wxDataFormat& format, const void *buf) = 0;

        // a simpler name which makes more sense for data objects supporting
        // only one format
    wxDataFormat GetFormat() const { return GetPreferredFormat(); }

    // old interface
        // decide if we support this format (can be either standard or custom
        // format) -- now uses GetAllFormats()
    virtual bool IsSupportedFormat(const wxDataFormat& format) const;

private:
    DECLARE_DYNAMIC_CLASS( wxDataObject )
};

//----------------------------------------------------------------------------
// wxTextDataObject is a specialization of wxDataObject for text data
//----------------------------------------------------------------------------

class wxTextDataObject : public wxDataObject
{
public:
    // ctors
    wxTextDataObject();
    wxTextDataObject(const wxString& strText);
    void Init(const wxString& strText) { m_strText = strText; }

    virtual wxDataFormat GetPreferredFormat() const
        { return wxDF_TEXT; }
    virtual bool IsSupportedFormat(const wxDataFormat& format) const
        { return format == wxDF_TEXT; }
	
    virtual size_t GetDataSize(const wxDataFormat& format) const;
    virtual bool GetDataHere(const wxDataFormat& format, void *buf) const;
    virtual bool SetData(const wxDataFormat& format, const void *buf);

    // additional helpers
    void SetText(const wxString& strText) { m_strText = strText; }
    wxString GetText() const { return m_strText; }

private:
    wxString  m_strText;
    
private:
    DECLARE_DYNAMIC_CLASS( wxTextDataObject )
};

//----------------------------------------------------------------------------
// wxFileDataObject is a specialization of wxDataObject for file names
//----------------------------------------------------------------------------

class wxFileDataObject : public wxDataObject
{
public:
    wxFileDataObject();
    
    void AddFile( const wxString &file );
    wxString GetFiles() const;
    
    virtual wxDataFormat GetPreferredFormat() const
        { return wxDF_FILENAME; }
    virtual bool IsSupportedFormat(const wxDataFormat& format) const
        { return format == wxDF_FILENAME; }
	
    virtual size_t GetDataSize(const wxDataFormat& format) const;
    virtual bool GetDataHere(const wxDataFormat& format, void *buf) const;
    virtual bool SetData(const wxDataFormat& format, const void *buf);

public:
    wxString  m_files;
  
private:
    DECLARE_DYNAMIC_CLASS( wxFileDataObject )
};

//----------------------------------------------------------------------------
// wxBitmapDataObject is a specialization of wxDataObject for bitmaps
//----------------------------------------------------------------------------

class wxBitmapDataObject : public wxDataObject
{
public:
    // ctors
    wxBitmapDataObject();
    wxBitmapDataObject(const wxBitmap& bitmap);

    // destr
    ~wxBitmapDataObject();
    
    // set/get our bitmap
    void SetBitmap(const wxBitmap& bitmap);
    const wxBitmap GetBitmap() const { return m_bitmap; }

    virtual wxDataFormat GetPreferredFormat() const
        { return wxDF_BITMAP; }
    virtual bool IsSupportedFormat(const wxDataFormat& format) const
        { return format == wxDF_BITMAP; }
	
    virtual size_t GetDataSize(const wxDataFormat& format) const;
    virtual bool GetDataHere(const wxDataFormat& format, void *buf) const;
    
    // sets PNG data
    virtual bool SetData(const wxDataFormat& format, const void *buf);

    // sets PNG data
    virtual void SetPngData(const void *buf, size_t size);
    
    void *GetData() 
        { return m_pngData; }
    
private: 
    wxBitmap    m_bitmap;
    size_t      m_pngSize;
    void       *m_pngData;
  
    void DoConvertToPng();
 
private: 
    DECLARE_DYNAMIC_CLASS( wxBitmapDataObject );
};

#endif
       //__GTKDNDH__

