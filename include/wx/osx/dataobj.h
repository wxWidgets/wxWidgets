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

    void Write(wxOSXDataSink *sink) const;
    bool Read(wxOSXDataSource *source);
    bool CanRead(wxOSXDataSource *source) const;

    void AddToPasteboard( void * pasteboardRef , wxIntPtr itemID );
    // returns true if the passed in format is present in the pasteboard
    // static bool IsFormatInPasteboard( void * pasteboardRef, const wxDataFormat &dataFormat );
    // returns true if any of the accepted formats of this dataobj is in the pasteboard
    //bool HasDataInPasteboard( void * pasteboardRef );
    //bool GetFromPasteboard( void * pasteboardRef );

#if wxOSX_USE_COCOA
    // adds all the native formats (in descending order of preference) this data object supports
    virtual void AddSupportedTypes( CFMutableArrayRef cfarray) const;
#endif
};

#endif // _WX_MAC_DATAOBJ_H_

