///////////////////////////////////////////////////////////////////////////////
// Name:        msw/ole/dataobj.h
// Purpose:     declaration of the wxDataObject class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.05.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_MSW_OLE_DATAOBJ_H
#define   _WX_MSW_OLE_DATAOBJ_H

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------

struct IDataObject;

// ----------------------------------------------------------------------------
// wxDataObject is a "smart" and polymorphic piece of data.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDataObject
{
public:
    // ctor & dtor
    wxDataObject();
    virtual ~wxDataObject();

    // pure virtuals to override
        // get the best suited format for rendering our data
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

    // implementation only from now on
    // -------------------------------

    // tell the object that it should be now owned by IDataObject - i.e. when
    // it is deleted, it should delete us as well
    void SetAutoDelete();

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
    virtual bool GetDataHere(const wxDataFormat& format, void *buf) const
        { memcpy(buf, m_strText.c_str(), GetDataSize(format)); return TRUE; }
    virtual bool SetData(const wxDataFormat& format, const void *buf)
        { m_strText = (const wxChar *)buf; return TRUE; }

    // additional helpers
    void SetText(const wxString& strText) { m_strText = strText; }
    wxString GetText() const { return m_strText; }

private:
    wxString  m_strText;
};

// ----------------------------------------------------------------------------
// wxBitmapDataObject is a specialization of wxDataObject for bitmap data
// ----------------------------------------------------------------------------

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
    virtual wxDataFormat GetPreferredFormat() const { return wxDF_BITMAP; }
    virtual size_t GetFormatCount(bool outputOnlyToo = TRUE) const;
    virtual void GetAllFormats(wxDataFormat *formats,
                               bool outputOnlyToo = TRUE) const;
    virtual size_t GetDataSize(const wxDataFormat& format) const;
    virtual bool GetDataHere(const wxDataFormat& format, void *buf) const;
    virtual bool SetData(const wxDataFormat& format, const void *buf);

private:
    wxBitmap m_bitmap;
};

// ----------------------------------------------------------------------------
// wxMetaFileDataObject: see metafile.h is a specialization of wxDataObject for bitmap data
// ----------------------------------------------------------------------------

// TODO: wxFileDataObject.

#endif  //_WX_MSW_OLE_DATAOBJ_H

