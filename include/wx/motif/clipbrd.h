/////////////////////////////////////////////////////////////////////////////
// Name:        clipbrd.h
// Purpose:     Clipboard functionality.
//              Note: this functionality is under review, and
//              is derived from wxWindows 1.xx code. Please contact
//              the wxWindows developers for further information.
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CLIPBRD_H_
#define _WX_CLIPBRD_H_

#ifdef __GNUG__
#pragma interface "clipbrd.h"
#endif

#if wxUSE_CLIPBOARD

#include "wx/dataobj.h"

#include "wx/module.h"

bool WXDLLEXPORT wxOpenClipboard();
bool WXDLLEXPORT wxClipboardOpen();
bool WXDLLEXPORT wxCloseClipboard();
bool WXDLLEXPORT wxEmptyClipboard();
bool WXDLLEXPORT wxIsClipboardFormatAvailable(wxDataFormat dataFormat);
bool WXDLLEXPORT wxSetClipboardData(wxDataFormat dataFormat, wxObject *obj, int width = 0, int height = 0);
wxObject* WXDLLEXPORT wxGetClipboardData(wxDataFormat dataFormat, long *len = NULL);
wxDataFormat WXDLLEXPORT wxEnumClipboardFormats(wxDataFormat dataFormat);
wxDataFormat WXDLLEXPORT wxRegisterClipboardFormat(char *formatName);
bool WXDLLEXPORT wxGetClipboardFormatName(wxDataFormat dataFormat, char *formatName, int maxCount);

//-----------------------------------------------------------------------------
// wxClipboard
//-----------------------------------------------------------------------------

class wxClipboard : public wxClipboardBase
{
public:
    wxClipboard();
    ~wxClipboard();

    // open the clipboard before SetData() and GetData()
    virtual bool Open();

    // close the clipboard after SetData() and GetData()
    virtual void Close();

    // can be called several times
    virtual bool SetData( wxDataObject *data );

    // format available on the clipboard ?
    // supply ID if private format, the same as wxPrivateDataObject::SetId()
    virtual bool IsSupported( wxDataFormat format );

    // fill data with data on the clipboard (if available)
    virtual bool GetData( wxDataObject *data );

    // clears wxTheClipboard and the system's clipboard if possible
    virtual void Clear();

    // implementation from now on

    bool              m_open;
    wxList            m_data;
    bool              m_usePrimary;

private:
    DECLARE_DYNAMIC_CLASS(wxClipboard)
};

/* The clipboard */
WXDLLEXPORT_DATA(extern wxClipboard*) wxTheClipboard;

#endif // wxUSE_CLIPBOARD

#endif
    // _WX_CLIPBRD_H_
