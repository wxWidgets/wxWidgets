///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/ole/dataobj2.h
// Purpose:     second part of platform specific wxDataObject header -
//              declarations of predefined wxDataObjectSimple-derived classes
// Author:      Vadim Zeitlin
// Created:     21.10.99
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_OLE_DATAOBJ2_H
#define _WX_MSW_OLE_DATAOBJ2_H

// ----------------------------------------------------------------------------
// wxBitmapDataObject is a specialization of wxDataObject for bitmap data
//
// NB: in fact, under MSW we support CF_DIB (and not CF_BITMAP) clipboard
//     format and we also provide wxBitmapDataObject2 for CF_BITMAP (which is
//     rarely used). This is ugly, but I haven't found a solution for it yet.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBitmapDataObject : public wxBitmapDataObjectBase
{
public:
    // ctors
    wxBitmapDataObject(const wxBitmap& bitmap = wxNullBitmap)
        : wxBitmapDataObjectBase(bitmap)
        {
            SetFormat(wxDF_DIB);

            m_data = nullptr;
        }

    // implement base class pure virtuals
    virtual size_t GetDataSize() const override;
    virtual bool GetDataHere(void *buf) const override;
    virtual bool SetData(size_t len, const void *buf) override;

    virtual size_t GetDataSize(const wxDataFormat& WXUNUSED(format)) const override
        { return GetDataSize(); }
    virtual bool GetDataHere(const wxDataFormat& WXUNUSED(format),
                             void *buf) const override
        { return GetDataHere(buf); }
    virtual bool SetData(const wxDataFormat& WXUNUSED(format),
                         size_t len, const void *buf) override
        { return SetData(len, buf); }

private:
    // the DIB data
    void /* BITMAPINFO */ *m_data;

    wxDECLARE_NO_COPY_CLASS(wxBitmapDataObject);
};

// ----------------------------------------------------------------------------
// wxBitmapDataObject2 - a data object for CF_BITMAP
//
// FIXME did I already mention it was ugly?
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBitmapDataObject2 : public wxBitmapDataObjectBase
{
public:
    // ctors
    wxBitmapDataObject2(const wxBitmap& bitmap = wxNullBitmap)
        : wxBitmapDataObjectBase(bitmap)
        {
        }

    // implement base class pure virtuals
    virtual size_t GetDataSize() const override;
    virtual bool GetDataHere(void *buf) const override;
    virtual bool SetData(size_t len, const void *buf) override;

    virtual size_t GetDataSize(const wxDataFormat& WXUNUSED(format)) const override
        { return GetDataSize(); }
    virtual bool GetDataHere(const wxDataFormat& WXUNUSED(format),
                             void *buf) const override
        { return GetDataHere(buf); }
    virtual bool SetData(const wxDataFormat& WXUNUSED(format),
                         size_t len, const void *buf) override
        { return SetData(len, buf); }

private:
    wxDECLARE_NO_COPY_CLASS(wxBitmapDataObject2);
};

// ----------------------------------------------------------------------------
// wxFileDataObject - data object for CF_HDROP
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFileDataObject : public wxFileDataObjectBase
{
public:
    wxFileDataObject() = default;

    // implement base class pure virtuals
    virtual bool SetData(size_t len, const void *buf) override;
    virtual size_t GetDataSize() const override;
    virtual bool GetDataHere(void *pData) const override;
    virtual void AddFile(const wxString& file);

    virtual size_t GetDataSize(const wxDataFormat& WXUNUSED(format)) const override
        { return GetDataSize(); }
    virtual bool GetDataHere(const wxDataFormat& WXUNUSED(format),
                             void *buf) const override
        { return GetDataHere(buf); }
    virtual bool SetData(const wxDataFormat& WXUNUSED(format),
                         size_t len, const void *buf) override
        { return SetData(len, buf); }

private:
    wxDECLARE_NO_COPY_CLASS(wxFileDataObject);
};

// ----------------------------------------------------------------------------
// wxVirtualFileDataObject - data object for CFSTR_FILEDESCRIPTORW
// ----------------------------------------------------------------------------

#if wxUSE_OLE && wxUSE_STREAMS && wxUSE_DATETIME

#include "wx/datetime.h"
#include "wx/filefn.h"          // for wxFileOffset

class WXDLLIMPEXP_FWD_BASE wxInputStream;

#define wxHAS_VIRTUAL_FILE_DATA_OBJECT

class WXDLLIMPEXP_CORE wxVirtualFileDataObject : public wxDataObjectSimple
{
public:
    wxVirtualFileDataObject();
    virtual ~wxVirtualFileDataObject();

    // get the number of items, both files and directories
    size_t GetCount() const { return m_entries.size(); }

    // get the relative path of the item, using native path separators
    wxString GetName(size_t n) const;

    // check if the item is a directory
    bool IsDir(size_t n) const;

    // get the size of the item, if known, or return wxInvalidOffset
    wxFileOffset GetSize(size_t n) const;

    // get the item modification time if known or return invalid wxDateTime
    wxDateTime GetModificationTime(size_t n) const;

    // functions below retrieve the contents of the items from the data source
    // and can only be used immediately after getting the data, i.e. from
    // wxDropTarget::OnData() or right after wxClipboard::GetData()

    // return the stream for reading the item contents or null if it couldn't
    // be retrieved (which is always the case for directories)
    std::unique_ptr<wxInputStream> OpenStream(size_t n) const;

    // save the item to the given path, creating the directory for it if
    // necessary and overwriting the existing file, if any
    bool SaveTo(size_t n, const wxString& path) const;

    // save all items under the given directory and return the paths of the
    // items created directly in it, i.e. the same paths wxFileDataObject would
    // have returned for normal files, or an empty vector on error
    std::vector<wxString> SaveAll(const wxString& dir) const;

    // save all items to a new temporary directory, which is removed when this
    // object is destroyed or DeleteAllTempFiles() is called, and return the
    // same paths as SaveAll() or an empty vector on error
    std::vector<wxString> SaveAllToTempDir();

    // remove all temporary directories created by SaveAllToTempDir() before
    // this object is destroyed, return false if some couldn't be removed
    bool DeleteAllTempFiles();

    // implement base class virtual functions
    virtual bool SetData(size_t len, const void *buf) override;
    virtual bool SetData(const wxDataFormat& WXUNUSED(format),
                         size_t len, const void *buf) override
        { return SetData(len, buf); }
    virtual void MSWSetDataSource(IDataObject* source,
                                  const wxDataFormat& format) override;

private:
    struct Entry
    {
        wxString name;
        wxFileOffset size = wxInvalidOffset;
        wxDateTime mtime;       // invalid if unknown
        int index = 0;          // index of the item in the data source
        unsigned attrs = 0;     // FILE_ATTRIBUTE_XXX, 0 if unknown

        bool IsDir() const;
    };

    // Release the data source, if we have it.
    void ReleaseSource();

    // Common part of SaveTo() and SaveAll(), returns false on error.
    bool DoSaveTo(const Entry& entry, const wxString& path) const;


    std::vector<Entry> m_entries;

    IDataObject* m_source = nullptr;

    // Temporary directories created by SaveAllToTempDir().
    std::vector<wxString> m_tempDirs;

    wxDECLARE_NO_COPY_CLASS(wxVirtualFileDataObject);
};

#endif // wxUSE_OLE && wxUSE_STREAMS && wxUSE_DATETIME

// ----------------------------------------------------------------------------
// wxURLDataObject: data object for URLs
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxURLDataObject : public wxDataObjectComposite
{
public:
    // initialize with URL in ctor or use SetURL later
    wxURLDataObject(const wxString& url = wxEmptyString);

    // return the URL as string
    wxString GetURL() const;

    // Set a string as the URL in the data object
    void SetURL(const wxString& url);

    // override to set m_textFormat
    virtual bool SetData(const wxDataFormat& format,
                         size_t len,
                         const void *buf) override;

private:
    // last data object we got data in
    wxDataObjectSimple *m_dataObjectLast;

    wxDECLARE_NO_COPY_CLASS(wxURLDataObject);
};

#endif // _WX_MSW_OLE_DATAOBJ2_H
