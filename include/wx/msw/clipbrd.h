/////////////////////////////////////////////////////////////////////////////
// Name:        clipbrd.h
// Purpose:     Clipboard functionality
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CLIPBRD_H_
#define _WX_CLIPBRD_H_

#ifdef __GNUG__
#pragma interface "clipbrd.h"
#endif

#include "wx/defs.h"
#include "wx/setup.h"

#if wxUSE_CLIPBOARD

#include "wx/list.h"
#include "wx/module.h"
#include "wx/dataobj.h"     // for wxDataFormat

// These functions superceded by wxClipboard, but retained in order to
// implement wxClipboard, and for compatibility.

// open/close the clipboard
WXDLLEXPORT bool wxOpenClipboard();
WXDLLEXPORT bool wxIsClipboardOpened();
#define wxClipboardOpen wxIsClipboardOpened
WXDLLEXPORT bool wxCloseClipboard();

// get/set data
WXDLLEXPORT bool wxEmptyClipboard();
WXDLLEXPORT bool wxSetClipboardData(wxDataFormat dataFormat,
                                    const void *data,
                                    int width = 0, int height = 0);
WXDLLEXPORT void* wxGetClipboardData(wxDataFormat dataFormat,
                                     long *len = NULL);

// clipboard formats
WXDLLEXPORT bool wxIsClipboardFormatAvailable(wxDataFormat dataFormat);
WXDLLEXPORT wxDataFormat wxEnumClipboardFormats(wxDataFormat dataFormat);
WXDLLEXPORT int  wxRegisterClipboardFormat(wxChar *formatName);
WXDLLEXPORT bool wxGetClipboardFormatName(wxDataFormat dataFormat,
                                          wxChar *formatName,
                                          int maxCount);

//-----------------------------------------------------------------------------
// wxClipboard
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxDataObject;
class WXDLLEXPORT wxClipboard : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxClipboard)

public:
    wxClipboard();
    ~wxClipboard();

    // open the clipboard before SetData() and GetData()
    virtual bool Open();

    // close the clipboard after SetData() and GetData()
    virtual void Close();

    // set the clipboard data. all other formats will be deleted.
    virtual bool SetData( wxDataObject *data );

    // add to the clipboard data.
    virtual bool AddData( wxDataObject *data );

    // ask if data in correct format is available
    virtual bool IsSupported( wxDataFormat format );

    // fill data with data on the clipboard (if available)
    virtual bool GetData( wxDataObject *data );

    // clears wxTheClipboard and the system's clipboard if possible
    virtual void Clear();
    
    /// X11 has two clipboards which get selected by this call. Empty on MSW.
    inline void UsePrimarySelection( bool WXUNUSED(primary) ) { }
    
};

// The global clipboard object
WXDLLEXPORT_DATA(extern wxClipboard*) wxTheClipboard;

//-----------------------------------------------------------------------------
// wxClipboardModule: module responsible for initializing the global clipboard
// object
//-----------------------------------------------------------------------------

class wxClipboardModule : public wxModule
{
    DECLARE_DYNAMIC_CLASS(wxClipboardModule)

public:
    wxClipboardModule() { }

    bool OnInit();
    void OnExit();
};

#endif // wxUSE_CLIPBOARD
#endif
    // _WX_CLIPBRD_H_
