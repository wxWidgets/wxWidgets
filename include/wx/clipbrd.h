/////////////////////////////////////////////////////////////////////////////
// Name:        wx/clipbrd.h
// Purpose:     wxClipboad class and clipboard functions
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.10.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows Team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CLIPBRD_H_BASE_
#define _WX_CLIPBRD_H_BASE_

#include "wx/defs.h"

#if wxUSE_CLIPBOARD

#include "wx/object.h"
#include "wx/wxchar.h"

class WXDLLEXPORT wxDataFormat;
class WXDLLEXPORT wxDataObject;

// ----------------------------------------------------------------------------
// wxClipboard represents the system clipboard. Normally, you should use
// wxTheClipboard which is a global pointer to the (unique) clipboard.
//
// Clipboard can be used to copy data to/paste data from. It works together
// with wxDataObject.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxClipboardBase : public wxObject
{
public:
    // open the clipboard before Add/SetData() and GetData()
    virtual bool Open();

    // close the clipboard after Add/SetData() and GetData()
    virtual void Close();

    // add to the clipboard data
    //
    // NB: the clipboard owns the pointer and will delete it, so data must be
    //     allocated on the heap
    virtual bool AddData( wxDataObject *data );

    // set the clipboard data, this is the same as Clear() followed by
    // AddData()
    virtual bool SetData( wxDataObject *data );

    // ask if data in correct format is available
    virtual bool IsSupported( const wxDataFormat& format );

    // fill data with data on the clipboard (if available)
    virtual bool GetData( wxDataObject& data );

    // clears wxTheClipboard and the system's clipboard if possible
    virtual void Clear();

    // flushes the clipboard: this means that the data which is currently on
    // clipboard will stay available even after the application exits (possibly
    // eating memory), otherwise the clipboard will be emptied on exit
    virtual bool Flush() { return FALSE; }

    // X11 has two clipboards which get selected by this call. Empty on MSW.
    virtual void UsePrimarySelection( bool WXUNUSED(primary) = FALSE ) { }

#ifdef WXWIN_COMPATIBILITY_2
    // deprecated version
    bool GetData(wxDataObject *data)
    {
        wxCHECK_MSG(data, FALSE, wxT("NULL pointer in wxClipboard"));

        return GetData(*data);
    }
#endif // WXWIN_COMPATIBILITY_2
};

// ----------------------------------------------------------------------------
// include platform-specific class declaration
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/clipbrd.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/clipbrd.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/clipbrd.h"
#elif defined(__WXQT__)
    #include "wx/gtk/clipbrd.h"
#elif defined(__WXMAC__)
    #include "wx/mac/clipbrd.h"
#elif defined(__WXPM__)
    #include "wx/os2/clipbrd.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/clipbrd.h"
#endif

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// The global clipboard object
WXDLLEXPORT_DATA(extern wxClipboard *) wxTheClipboard;

// ----------------------------------------------------------------------------
// wxClipboardModule: module responsible for initializing the global clipboard
// object
//
// NB: IMPLEMENT_DYNAMIC_CLASS() for it is in common/appcmn.cpp
// ----------------------------------------------------------------------------

class wxClipboardModule : public wxModule
{
public:
    bool OnInit()
        { wxTheClipboard = new wxClipboard; }
    void OnExit()
        { delete wxTheClipboard; wxTheClipboard = (wxClipboard *)NULL; }

private:
    DECLARE_DYNAMIC_CLASS(wxClipboardModule)
};

#endif // wxUSE_CLIPBOARD

#endif // _WX_CLIPBRD_H_BASE_
