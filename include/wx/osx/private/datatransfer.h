///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/private/datatransfer.h
// Purpose:     OS X specific data transfer implementation
// Author:      Stefan Csomor
// Created:     2019-03-29
// Copyright:   (c) 2019 Stefan Csomor <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_PRIVATE_DATATRANSFER_H_
#define _WX_OSX_PRIVATE_DATATRANSFER_H_

#include "wx/osx/private.h"
#include "wx/osx/dataform.h"

class WXDLLIMPEXP_FWD_CORE wxDataObject;

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
    wxOSXPasteboard(OSXPasteboard native);
    ~wxOSXPasteboard();

    // sink methods

    virtual wxOSXDataSinkItem* CreateItem() override;

    void Clear() override;

    void Flush() override;

    // source methods

    virtual size_t GetItemCount() const override;

    virtual const wxOSXDataSourceItem* GetItem(size_t pos) const override;

    virtual bool HasData(CFArrayRef types) const override;

    static wxOSXPasteboard* GetGeneralClipboard();
private:
    void DeleteSinkItems();

    OSXPasteboard m_pasteboard;
    wxVector<wxOSXDataSinkItem*> m_sinkItems;
};

#endif
