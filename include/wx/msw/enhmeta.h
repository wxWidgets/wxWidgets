///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/enhmeta.h
// Purpose:     wxEnhMetaFile class for Win32
// Author:      Vadim Zeitlin
// Modified by:
// Created:     13.01.00
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_ENHMETA_H_
#define _WX_MSW_ENHMETA_H_

#include "wx/defs.h"

#if wxUSE_ENH_METAFILE

#include "wx/dc.h"
#include "wx/gdiobj.h"

#if wxUSE_DATAOBJ
    #include "wx/dataobj.h"
#endif

// ----------------------------------------------------------------------------
// wxEnhMetaFile: encapsulation of Win32 HENHMETAFILE
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxEnhMetaFile : public wxGDIObject
{
public:
    wxEnhMetaFile(const wxString& file = wxEmptyString) : m_filename(file)
        { Init(); }
    wxEnhMetaFile(const wxEnhMetaFile& metafile) : wxGDIObject()
        { Init(); Assign(metafile); }
    wxEnhMetaFile& operator=(const wxEnhMetaFile& metafile)
        { Free(); Assign(metafile); return *this; }

    virtual ~wxEnhMetaFile()
        { Free(); }

    // display the picture stored in the metafile on the given DC
    bool Play(wxDC *dc, wxRect *rectBound = nullptr);

    // accessors
    virtual bool IsOk() const override { return m_hMF != nullptr; }

    wxSize GetSize() const;
    int GetWidth() const { return GetSize().x; }
    int GetHeight() const { return GetSize().y; }

    const wxString& GetFileName() const { return m_filename; }

    // copy the metafile to the clipboard: the width and height parameters are
    // for backwards compatibility (with wxMetaFile) only, they are ignored by
    // this method
    bool SetClipboard(int width = 0, int height = 0);

    // Detach the HENHMETAFILE from this object, i.e. don't delete the handle
    // in the dtor -- the caller is now responsible for doing this, e.g. using
    // Free() method below.
    WXHANDLE Detach() { WXHANDLE h = m_hMF; m_hMF = nullptr; return h; }

    // Destroy the given HENHMETAFILE object.
    static void Free(WXHANDLE handle);

    // implementation
    WXHANDLE GetHENHMETAFILE() const { return m_hMF; }
    void SetHENHMETAFILE(WXHANDLE hMF) { Free(); m_hMF = hMF; }

protected:
    void Init();
    void Free() { Free(m_hMF); }
    void Assign(const wxEnhMetaFile& mf);

    // we don't use these functions (but probably should) but have to implement
    // them as they're pure virtual in the base class
    virtual wxGDIRefData *CreateGDIRefData() const override;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

private:
    wxString m_filename;
    WXHANDLE m_hMF;

    wxDECLARE_DYNAMIC_CLASS(wxEnhMetaFile);
};

// ----------------------------------------------------------------------------
// wxEnhMetaFileDC: allows to create a wxEnhMetaFile
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxEnhMetaFileDC : public wxDC
{
public:
    // the ctor parameters specify the filename (empty for memory metafiles),
    // the metafile picture size and the optional description/comment
    wxEnhMetaFileDC(const wxString& filename = wxEmptyString,
                    int width = 0, int height = 0,
                    const wxString& description = wxEmptyString);

    // as above, but takes reference DC as first argument to take resolution,
    // size, font metrics etc. from
    explicit
    wxEnhMetaFileDC(const wxDC& referenceDC,
                    const wxString& filename = wxEmptyString,
                    int width = 0, int height = 0,
                    const wxString& description = wxEmptyString);

    // obtain a pointer to the new metafile (caller should delete it)
    wxEnhMetaFile *Close();

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxEnhMetaFileDC);
};

#if wxUSE_DATAOBJ

// ----------------------------------------------------------------------------
// wxEnhMetaFileDataObject is a specialization of wxDataObject for enh metafile
// ----------------------------------------------------------------------------

// notice that we want to support both CF_METAFILEPICT and CF_ENHMETAFILE and
// so we derive from wxDataObject and not from wxDataObjectSimple
class WXDLLIMPEXP_CORE wxEnhMetaFileDataObject : public wxDataObject
{
public:
    // ctors
    wxEnhMetaFileDataObject() { }
    wxEnhMetaFileDataObject(const wxEnhMetaFile& metafile)
        : m_metafile(metafile) { }

    // virtual functions which you may override if you want to provide data on
    // demand only - otherwise, the trivial default versions will be used
    virtual void SetMetafile(const wxEnhMetaFile& metafile)
        { m_metafile = metafile; }
    virtual wxEnhMetaFile GetMetafile() const
        { return m_metafile; }

    // implement base class pure virtuals
    virtual wxDataFormat GetPreferredFormat(Direction dir) const override;
    virtual size_t GetFormatCount(Direction dir) const override;
    virtual void GetAllFormats(wxDataFormat *formats, Direction dir) const override;
    virtual size_t GetDataSize(const wxDataFormat& format) const override;
    virtual bool GetDataHere(const wxDataFormat& format, void *buf) const override;
    virtual bool SetData(const wxDataFormat& format, size_t len,
                         const void *buf) override;

protected:
    wxEnhMetaFile m_metafile;

    wxDECLARE_NO_COPY_CLASS(wxEnhMetaFileDataObject);
};


// ----------------------------------------------------------------------------
// wxEnhMetaFileSimpleDataObject does derive from wxDataObjectSimple which
// makes it more convenient to use (it can be used with wxDataObjectComposite)
// at the price of not supporting any more CF_METAFILEPICT but only
// CF_ENHMETAFILE
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxEnhMetaFileSimpleDataObject : public wxDataObjectSimple
{
public:
    // ctors
    wxEnhMetaFileSimpleDataObject() : wxDataObjectSimple(wxDF_ENHMETAFILE) { }
    wxEnhMetaFileSimpleDataObject(const wxEnhMetaFile& metafile)
        : wxDataObjectSimple(wxDF_ENHMETAFILE), m_metafile(metafile) { }

    // virtual functions which you may override if you want to provide data on
    // demand only - otherwise, the trivial default versions will be used
    virtual void SetEnhMetafile(const wxEnhMetaFile& metafile)
        { m_metafile = metafile; }
    virtual wxEnhMetaFile GetEnhMetafile() const
        { return m_metafile; }

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

protected:
    wxEnhMetaFile m_metafile;

    wxDECLARE_NO_COPY_CLASS(wxEnhMetaFileSimpleDataObject);
};

#endif // wxUSE_DATAOBJ

#endif // wxUSE_ENH_METAFILE

#endif // _WX_MSW_ENHMETA_H_
