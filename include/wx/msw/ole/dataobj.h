///////////////////////////////////////////////////////////////////////////////
// Name:        ole/dataobj.h
// Purpose:     declaration of the wxDataObject class
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     10.05.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_OLEDATAOBJ_H
#define   _WX_OLEDATAOBJ_H

// ----------------------------------------------------------------------------
// wxDataFormat identifies the single format of data
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDataFormat
{
public:
    // the clipboard formats under Win32 are UINTs
    typedef unsigned int NativeFormat;

    wxDataFormat(NativeFormat format = wxDF_INVALID) { m_format = format; }
    wxDataFormat& operator=(NativeFormat format)
        { m_format = format; return *this; }

    // defautl copy ctor/assignment operators ok

    // comparison (must have both versions)
    bool operator==(wxDataFormatId format) const
        { return m_format == (NativeFormat)format; }
    bool operator!=(wxDataFormatId format) const
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
    void SetId(wxDataFormatId format) { m_format = format; }

    // string ids are used for custom types - this SetId() must be used for
    // application-specific formats
    wxString GetId() const;
    void SetId(const wxChar *format);

private:
    // returns TRUE if the format is one of those defined in wxDataFormatId
    bool IsStandard() const { return m_format > 0 && m_format < wxDF_MAX; }

    NativeFormat m_format;
};

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------
struct IDataObject;

// ----------------------------------------------------------------------------
// wxDataObject is a "smart" and polymorphic piece of data.
//
// TODO it's currently "read-only" from COM point of view, i.e. we don't
//      support SetData. We don't support all advise functions neither (but
//      it's easy to do if we really want them)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDataObject
{
public:
    // ctor & dtor
    wxDataObject();
    virtual ~wxDataObject();

    // pure virtuals to override
        // get the best suited format for our data
    virtual wxDataFormat GetPreferredFormat() const = 0;
        // get the number of formats we support
    virtual size_t GetFormatCount() const
        { return 1; }
        // return all formats in the provided array (of size GetFormatCount())
    virtual void GetAllFormats(wxDataFormat *formats) const
        { formats[0] = GetPreferredFormat(); }
        // get the (total) size of data for the given format
    virtual size_t GetDataSize(const wxDataFormat& format) const = 0;
        // copy raw data (in the specified format) to provided pointer
    virtual void GetDataHere(const wxDataFormat& format, void *pBuf) const = 0;

    // accessors
        // retrieve IDataObject interface (for other OLE related classes)
    IDataObject *GetInterface() const { return m_pIDataObject; }
        // a simpler name which makes more sense for data objects supporting
        // only one format
    wxDataFormat GetFormat() const { return GetPreferredFormat(); }

    // old interface
        // decide if we support this format (can be either standard or custom
        // format) -- now uses GetAllFormats()
    virtual bool IsSupportedFormat(const wxDataFormat& format) const;

#ifdef __WXDEBUG__
    // function to return symbolic name of clipboard format (for debug messages)
    static const char *GetFormatName(wxDataFormat format);
#endif // Debug

private:
    IDataObject *m_pIDataObject; // pointer to the COM interface
};

// ----------------------------------------------------------------------------
// wxTextDataObject is a specialization of wxDataObject for text data
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTextDataObject : public wxDataObject
{
public:
    // ctors
    wxTextDataObject() { }
    wxTextDataObject(const wxString& strText) : m_strText(strText) { }
    void Init(const wxString& strText) { m_strText = strText; }

    // implement base class pure virtuals
    virtual wxDataFormat GetPreferredFormat() const
        { return wxDF_TEXT; }
    virtual bool IsSupportedFormat(const wxDataFormat& format) const
        { return format == wxDF_TEXT || format == wxDF_LOCALE; }
    virtual size_t GetDataSize(const wxDataFormat& format) const
        { return m_strText.Len() + 1; } // +1 for trailing '\0'of course
    virtual void GetDataHere(const wxDataFormat& format, void *pBuf) const
        { memcpy(pBuf, m_strText.c_str(), GetDataSize(format)); }

    // additional helpers
    void SetText(const wxString& strText) { m_strText = strText; }
    wxString GetText() const { return m_strText; }

private:
    wxString  m_strText;
};

// ----------------------------------------------------------------------------
// TODO: wx{Bitmap|Metafile|...}DataObject
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxBitmapDataObject is a specialization of wxDataObject for bitmap data
// ----------------------------------------------------------------------------

// TODO: implement OLE side of things. At present, it's just for clipboard
// use.
#include "wx/bitmap.h"

class WXDLLEXPORT wxBitmapDataObject : public wxDataObject
{
public:
    // ctors
    wxBitmapDataObject() { }
    wxBitmapDataObject(const wxBitmap& bitmap): m_bitmap(bitmap) { }

    // set/get our bitmap
    void SetBitmap(const wxBitmap& bitmap) { m_bitmap = bitmap; }
    const wxBitmap GetBitmap() const { return m_bitmap; }

    // implement base class pure virtuals
    virtual wxDataFormat GetPreferredFormat() const
        { return wxDF_BITMAP; }
    virtual bool IsSupportedFormat(const wxDataFormat& format) const
        { return format == wxDF_BITMAP; }
    virtual size_t GetDataSize(const wxDataFormat& format) const;
    virtual void GetDataHere(const wxDataFormat& format, void *pBuf) const;

private:
    wxBitmap m_bitmap;
};

// ----------------------------------------------------------------------------
// wxMetaFileDataObject: see metafile.h is a specialization of wxDataObject for bitmap data
// ----------------------------------------------------------------------------

// TODO: wxFileDataObject.

#endif  //_WX_OLEDATAOBJ_H

