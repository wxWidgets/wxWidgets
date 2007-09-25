/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/clipbrd.h
// Purpose:     Clipboard functionality.
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CLIPBRD_H_
#define _WX_CLIPBRD_H_

#if wxUSE_CLIPBOARD

class WXDLLIMPEXP_FWD_CORE wxDataObject;
struct wxDataIdToDataObject;

#include "wx/list.h"

WX_DECLARE_LIST(wxDataObject, wxDataObjectList);
WX_DECLARE_LIST(wxDataIdToDataObject, wxDataIdToDataObjectList);

WXDLLEXPORT bool wxOpenClipboard();
WXDLLEXPORT bool wxClipboardOpen();
WXDLLEXPORT bool wxCloseClipboard();
WXDLLEXPORT bool wxEmptyClipboard();
WXDLLEXPORT bool wxIsClipboardFormatAvailable(wxDataFormat dataFormat);
WXDLLEXPORT bool wxSetClipboardData(wxDataFormat dataFormat, wxObject *obj, int width = 0, int height = 0);
WXDLLEXPORT wxObject* wxGetClipboardData(wxDataFormat dataFormat, long *len = NULL);
WXDLLEXPORT wxDataFormat wxEnumClipboardFormats(wxDataFormat dataFormat);
WXDLLEXPORT wxDataFormat wxRegisterClipboardFormat(char *formatName);
WXDLLEXPORT bool wxGetClipboardFormatName(wxDataFormat dataFormat, char *formatName, int maxCount);

//-----------------------------------------------------------------------------
// wxClipboard
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxClipboard : public wxClipboardBase
{
public:
    wxClipboard();
    virtual ~wxClipboard();

    // open the clipboard before SetData() and GetData()
    virtual bool Open();

    // close the clipboard after SetData() and GetData()
    virtual void Close();

    // opened?
    virtual bool IsOpened() const { return m_open; }

    // replaces the data on the clipboard with data
    virtual bool SetData( wxDataObject *data );

    // adds data to the clipboard
    virtual bool AddData( wxDataObject *data );

    // format available on the clipboard ?
    virtual bool IsSupported( const wxDataFormat& format );

    // fill data with data on the clipboard (if available)
    virtual bool GetData( wxDataObject& data );

    // clears wxTheClipboard and the system's clipboard if possible
    virtual void Clear();

    // implementation from now on
    bool              m_open;
    wxDataObjectList  m_data;
    wxDataIdToDataObjectList m_idToObject;

private:
    DECLARE_DYNAMIC_CLASS(wxClipboard)
};

#endif // wxUSE_CLIPBOARD

#endif // _WX_CLIPBRD_H_
