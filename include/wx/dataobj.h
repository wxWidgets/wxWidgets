///////////////////////////////////////////////////////////////////////////////
// Name:        dataobj.h
// Purpose:     common data object classes
// Author:      Robert Roebling, Vadim Zeitlin
// Modified by:
// Created:     26.05.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows Team
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DATAOBJ_H_BASE_
#define _WX_DATAOBJ_H_BASE_

#if defined(__WXMSW__)
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

    #include "wx/msw/ole/dataobj.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/dataobj.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/dataobj.h"
#elif defined(__WXQT__)
    #include "wx/qt/dnd.h"
#elif defined(__WXMAC__)
    #include "wx/mac/dnd.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/dnd.h"
#endif

// ---------------------------------------------------------------------------
// wxPrivateDataObject is a specialization of wxDataObject for app specific
// data (of some given kind, derive directly from wxDataObject if you wish to
// efficiently support multiple formats)
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxPrivateDataObject : public wxDataObject
{
    DECLARE_DYNAMIC_CLASS( wxPrivateDataObject )

public:
    wxPrivateDataObject();
    virtual ~wxPrivateDataObject() { Free(); }

    // get the format object - it is used to decide whether we support the
    // given output format or not
    wxDataFormat& GetFormat() { return m_format; }

    // set data. will make copy of the data
    void SetData( const void *data, size_t size );

    // returns pointer to data
    void *GetData() const { return m_data; }

    // get the size of the data
    virtual size_t GetSize() const;

    // copy data to the given buffer
    virtual void WriteData( void *dest ) const;

    // these functions are provided for wxGTK compatibility, their usage is
    // deprecated - use GetFormat().SetId() instead
    void SetId( const wxString& id ) { m_format.SetId(id); }
    wxString GetId() const { return m_format.GetId(); }

    // implement the base class pure virtuals
    virtual wxDataFormat GetPreferredFormat() const
        { return m_format; }
    virtual bool IsSupportedFormat(wxDataFormat format) const
        { return m_format == format; }
    virtual size_t GetDataSize() const
        { return m_size; }
    virtual void GetDataHere(void *dest) const
        { WriteData(dest); }

protected:
    // the function which really copies the data - called by WriteData() above
    // and uses GetSize() to get the size of the data
    //
    // VZ: I really wonder why do we need it
    void WriteData( const void *data, void *dest ) const;

private:
    // free the data
    inline void Free();

    // the data
    size_t     m_size;
    void      *m_data;

    // the data format
    wxDataFormat m_format;
};


#endif
    // _WX_DATAOBJ_H_BASE_
