/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/clipbrd.h
// Purpose:     Clipboard functionality.
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CLIPBRD_H_
#define _WX_CLIPBRD_H_

#if wxUSE_CLIPBOARD

#include "wx/osx/core/cfref.h"

//-----------------------------------------------------------------------------
// wxClipboard
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxOSXDataSourceItem
{
public:
    virtual ~wxOSXDataSourceItem();

    virtual wxDataFormat::NativeFormat AvailableType(CFArrayRef types) const = 0;

    virtual bool GetData( const wxDataFormat& dataFormat, wxMemoryBuffer& target) = 0;

    virtual bool GetData( wxDataFormat::NativeFormat type, wxMemoryBuffer& target) = 0;

    virtual CFDataRef DoGetData(wxDataFormat::NativeFormat type) const = 0;
};

class WXDLLIMPEXP_CORE wxOSXDataSource
{
public:
    // the number of source items
    virtual size_t GetItemCount() const = 0;

    // get source item by index, needs to be deleted after use
    virtual const wxOSXDataSourceItem* GetItem(size_t pos) const = 0;

    // returns true if there is any data in this source conforming to dataFormat
    virtual bool IsSupported(const wxDataFormat &dataFormat);

    // returns true if there is any data in this source supported by dataobj
    virtual bool IsSupported(const wxDataObject &dataobj);

    // returns true if there is any data in this source of types
    virtual bool HasData(CFArrayRef types) const = 0;

};

class WXDLLIMPEXP_CORE wxOSXDataSinkItem
{
public:
    virtual ~wxOSXDataSinkItem();

    virtual void SetFilename(const wxString& filename);

    // translating from wx into native representation
    virtual void SetData(const wxDataFormat& format, const void *buf, size_t size) = 0;

    // translating from wx into native representation
    virtual void SetData(wxDataFormat::NativeFormat format, const void *buf, size_t size) = 0;

   // native implementation for setting data
    virtual void DoSetData(wxDataFormat::NativeFormat format, CFDataRef data) = 0;
};


class WXDLLIMPEXP_CORE wxOSXDataSink
{
public:
    // delete all created sink items
    virtual void Clear() = 0;

    // create a new sink item
    virtual wxOSXDataSinkItem* CreateItem() = 0;

    // flush the created sink items into the system sink representation
    virtual void Flush() = 0 ;
};

class WXDLLIMPEXP_CORE wxOSXPasteboard : public wxOSXDataSink, public wxOSXDataSource
{
public:
    wxOSXPasteboard(WXOSXPasteboard native);
    ~wxOSXPasteboard();

    // sink methods

    virtual wxOSXDataSinkItem* CreateItem() wxOVERRIDE;

    void Clear() wxOVERRIDE;

    void Flush() wxOVERRIDE;

    // source methods

    virtual size_t GetItemCount() const wxOVERRIDE;

    virtual const wxOSXDataSourceItem* GetItem(size_t pos) const wxOVERRIDE;

    virtual bool HasData(CFArrayRef types) const wxOVERRIDE;

    static wxOSXPasteboard* GetGeneralClipboard();
private:
    void DeleteSinkItems();
    
    WXOSXPasteboard m_pasteboard;
    wxVector<wxOSXDataSinkItem*> m_sinkItems;
};

class WXDLLIMPEXP_CORE wxClipboard : public wxClipboardBase
{
public:
    wxClipboard();
    virtual ~wxClipboard();

    // open the clipboard before SetData() and GetData()
    virtual bool Open();

    // close the clipboard after SetData() and GetData()
    virtual void Close();

    // query whether the clipboard is opened
    virtual bool IsOpened() const;

    // set the clipboard data. all other formats will be deleted.
    virtual bool SetData( wxDataObject *data );

    // add to the clipboard data.
    virtual bool AddData( wxDataObject *data );

    // ask if data in correct format is available
    virtual bool IsSupported( const wxDataFormat& format );

    // fill data with data on the clipboard (if available)
    virtual bool GetData( wxDataObject& data );

    // clears wxTheClipboard and the system's clipboard if possible
    virtual void Clear();

    // flushes the clipboard: this means that the data which is currently on
    // clipboard will stay available even after the application exits (possibly
    // eating memory), otherwise the clipboard will be emptied on exit
    virtual bool Flush();

private:
    wxDataObject     *m_data;
    bool              m_open;
    wxCFRef<PasteboardRef> m_pasteboard;

    wxDECLARE_DYNAMIC_CLASS(wxClipboard);
};

#endif // wxUSE_CLIPBOARD

#endif // _WX_CLIPBRD_H_
