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

class WXDLLIMPEXP_CORE wxClipboard : public wxClipboardBase
{
public:
    wxClipboard();
    virtual ~wxClipboard();

    // open the clipboard before SetData() and GetData()
    virtual bool Open() wxOVERRIDE;

    // close the clipboard after SetData() and GetData()
    virtual void Close() wxOVERRIDE;

    // query whether the clipboard is opened
    virtual bool IsOpened() const wxOVERRIDE;

    // set the clipboard data. all other formats will be deleted.
    virtual bool SetData( wxDataObject *data ) wxOVERRIDE;

    // add to the clipboard data.
    virtual bool AddData( wxDataObject *data ) wxOVERRIDE;

    // ask if data in correct format is available
    virtual bool IsSupported( const wxDataFormat& format ) wxOVERRIDE;

    // fill data with data on the clipboard (if available)
    virtual bool GetData( wxDataObject& data ) wxOVERRIDE;

    // clears wxTheClipboard and the system's clipboard if possible
    virtual void Clear() wxOVERRIDE;

    // flushes the clipboard: this means that the data which is currently on
    // clipboard will stay available even after the application exits (possibly
    // eating memory), otherwise the clipboard will be emptied on exit
    virtual bool Flush() wxOVERRIDE;

private:
    wxDataObject     *m_data;
    bool              m_open;
    wxCFRef<PasteboardRef> m_pasteboard;

    wxDECLARE_DYNAMIC_CLASS(wxClipboard);
};

#endif // wxUSE_CLIPBOARD

#endif // _WX_CLIPBRD_H_
