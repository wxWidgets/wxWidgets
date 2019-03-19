///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/dataobj.h
// Purpose:     declaration of the wxDataObject
// Author:      Stefan Csomor (adapted from Robert Roebling's gtk port)
// Modified by:
// Created:     10/21/99
// Copyright:   (c) 1998, 1999 Vadim Zeitlin, Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAC_DATAOBJ_H_
#define _WX_MAC_DATAOBJ_H_

class WXDLLIMPEXP_CORE wxOSXDataSink;
class WXDLLIMPEXP_CORE wxOSXDataSource;

class WXDLLIMPEXP_CORE wxDataObject : public wxDataObjectBase
{
public:
    wxDataObject();
    virtual ~wxDataObject() { }

    virtual bool IsSupportedFormat( const wxDataFormat& format, Direction dir = Get ) const;

    void WriteToSink(wxOSXDataSink *sink) const;
    bool ReadFromSource(wxOSXDataSource *source);
    bool CanReadFromSource(wxOSXDataSource *source) const;

#if wxOSX_USE_COCOA
    // adds all the native formats (in descending order of preference) this data object supports
    virtual void AddSupportedTypes( CFMutableArrayRef cfarray) const;
#endif
};

#endif // _WX_MAC_DATAOBJ_H_

