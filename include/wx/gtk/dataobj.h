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
// wxDataFormat (internal)
//-------------------------------------------------------------------------

class wxDataFormat : public wxObject
{
public:
    wxDataFormat();
    wxDataFormat( wxDataFormatId type );
    wxDataFormat( const wxString &id );
    wxDataFormat( const wxChar *id );
    wxDataFormat( const wxDataFormat &format );
    wxDataFormat( const GdkAtom atom );

    void SetType( wxDataFormatId type );
    wxDataFormatId GetType() const;

    /* the string Id identifies the format of clipboard or DnD data. a word
     * processor would e.g. add a wxTextDataObject and a wxPrivateDataObject
     * to the clipboard - the latter with the Id "application/wxword", an
     * image manipulation program would put a wxBitmapDataObject and a
     * wxPrivateDataObject to the clipboard - the latter with "image/png". */

    wxString GetId() const;
    void SetId( const wxChar *id );

    GdkAtom GetAtom();
    void SetAtom(GdkAtom atom) { m_hasAtom = TRUE; m_atom = atom; }

    // implicit conversion to wxDataFormatId
    operator wxDataFormatId() const { return m_type; }

    bool operator==(wxDataFormatId type) const { return m_type == type; }
    bool operator!=(wxDataFormatId type) const { return m_type != type; }

private:
    wxDataFormatId  m_type;
    wxString    m_id;
    bool        m_hasAtom;
    GdkAtom     m_atom;
    
    void PrepareFormats();

private:
    DECLARE_CLASS( wxDataFormat )
};

//-------------------------------------------------------------------------
// wxDataBroker (internal)
//-------------------------------------------------------------------------

class wxDataBroker : public wxObject
{
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
    virtual wxDataFormatId GetPreferredFormat() const;

    /* search through m_dataObjects, return TRUE if found */
    virtual bool IsSupportedFormat( wxDataFormat &format ) const;

    /* search through m_dataObjects and call child's GetSize() */
    virtual size_t GetSize( wxDataFormat& format ) const;

    /* search through m_dataObjects and call child's WriteData(dest) */
    virtual void WriteData( wxDataFormat& format, void *dest ) const;

public:
    /* implementation */
    wxList    m_dataObjects;
    size_t    m_preferred;
  
private:
    DECLARE_CLASS( wxDataBroker )
};

//----------------------------------------------------------------------------
// wxDataObject to be placed in wxDataBroker
//----------------------------------------------------------------------------

class wxDataObject : public wxObject
{
public:
    /* constructor */
    wxDataObject();

    /* destructor */
    ~wxDataObject();

    /* write data to dest */
    virtual void WriteData( void *dest ) const = 0;

    /* get size of data */
    virtual size_t GetSize() const = 0;

public:
    /* implementation */
    wxDataFormat m_format;
    
    wxDataFormat &GetFormat();

    wxDataFormatId GetFormatType() const;
    wxString   GetFormatId() const;
    GdkAtom    GetFormatAtom() const;

private:
    DECLARE_DYNAMIC_CLASS( wxDataObject )
};

//----------------------------------------------------------------------------
// wxTextDataObject is a specialization of wxDataObject for text data
//----------------------------------------------------------------------------

class wxTextDataObject : public wxDataObject
{
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

public:
    /* implementation */
    wxString  m_data;

private:
    DECLARE_DYNAMIC_CLASS( wxTextDataObject )
};

//----------------------------------------------------------------------------
// wxFileDataObject is a specialization of wxDataObject for file names
//----------------------------------------------------------------------------

class wxFileDataObject : public wxDataObject
{
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

public:
    /* implementation */
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
    /* see wxTextDataObject for explanation */
    wxBitmapDataObject();
    wxBitmapDataObject( const wxBitmap& bitmap );
    ~wxBitmapDataObject();

    void SetBitmap( const wxBitmap &bitmap );
    wxBitmap GetBitmap() const;

    virtual void WriteData( void *dest ) const;
    virtual size_t GetSize() const;
    void *GetData() const { return (void*)m_pngData; }

    void WriteBitmap( const wxBitmap &bitmap, void *dest ) const;
    
    void SetPngData( const char *pngData, size_t pngSize );

private: 
    wxBitmap    m_bitmap;
    size_t      m_pngSize;
    char       *m_pngData;
  
    void DoConvertToPng();
 
private: 
    DECLARE_DYNAMIC_CLASS( wxBitmapDataObject );
};

#endif
       //__GTKDNDH__

