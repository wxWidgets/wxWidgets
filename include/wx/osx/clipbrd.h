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
    
    virtual CFDataRef GetData(wxDataFormat::NativeFormat type) const = 0;

};

class WXDLLIMPEXP_CORE wxOSXDataSource
{
public:
    virtual size_t GetItemCount() const = 0;

    virtual bool IsSupported(const wxDataFormat &dataFormat);

    virtual bool IsSupported(const wxDataObject &dataobj);

    virtual bool HasData(CFArrayRef types) const = 0;

    virtual CFDataRef GetData(wxDataFormat::NativeFormat type) const = 0;

    virtual const wxOSXDataSourceItem* GetItem(size_t pos) const = 0;
};

class WXDLLIMPEXP_CORE wxOSXDataSinkItem
{
public:
    virtual ~wxOSXDataSinkItem();

    virtual void AddFilename(const char* utf8Dnormpath);

    virtual void AddData(wxDataFormat::NativeFormat format, CFDataRef data) = 0;
};


class WXDLLIMPEXP_CORE wxOSXDataSink
{
public:
    virtual void Clear() = 0;

    virtual wxOSXDataSinkItem* CreateItem() = 0;

    virtual void Flush() = 0 ;
};

class WXDLLIMPEXP_CORE wxOSXPasteboard : public wxOSXDataSink, public wxOSXDataSource
{
public:
    wxOSXPasteboard(OSXPasteboard native);

    // sink methods

    virtual wxOSXDataSinkItem* CreateItem() wxOVERRIDE;
    void Clear() wxOVERRIDE;
    void Flush() wxOVERRIDE;

    // source methods

    virtual size_t GetItemCount() const wxOVERRIDE;
    virtual bool HasData(CFArrayRef types) const wxOVERRIDE;
    virtual CFDataRef GetData(wxDataFormat::NativeFormat type) const wxOVERRIDE;

    virtual const wxOSXDataSourceItem* GetItem(size_t pos) const wxOVERRIDE;

    static wxOSXPasteboard* GetGeneralClipboard();
private:
    OSXPasteboard m_pasteboard;
    wxVector<wxOSXDataSinkItem*> m_items;
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
