/////////////////////////////////////////////////////////////////////////////
// Name:        src/x11/clipbrd.cpp
// Purpose:     Clipboard functionality
// Author:      Robert Roebling
// Created:
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_CLIPBOARD

#include "wx/clipbrd.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/dataobj.h"
#endif

#include "wx/x11/private.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

#if !wxUSE_NANOX
Atom  g_clipboardAtom   = 0;
Atom  g_targetsAtom     = 0;
#endif

// avoid warnings about unused static variable (notice that we still use it
// even in release build if the compiler doesn't support variadic macros)
#if defined(__WXDEBUG__) || !defined(HAVE_VARIADIC_MACROS)

// the trace mask we use with wxLogTrace() - call
// wxLog::AddTraceMask(TRACE_CLIPBOARD) to enable the trace messages from here
// (there will be a *lot* of them!)
static const wxChar *TRACE_CLIPBOARD = wxT("clipboard");

#endif // __WXDEBUG__

// Atom for handle different format
// these atom not defined in Xatom.h
static Atom XA_CLIPBOARD;
static Atom XA_UTF8_STRING;
static Atom XA_TARGETS;
static Atom XA_MULTIPLE;
static Atom XA_IMAGE_BMP;
static Atom XA_IMAGE_JPG;
static Atom XA_IMAGE_TIFF;
static Atom XA_IMAGE_PNG;
static Atom XA_TEXT_URI_LIST;
static Atom XA_TEXT_URI;
static Atom XA_TEXT_PLAIN;
static Atom XA_TEXT;

static void InitX11Clipboard()
{
    static bool isInitialised = false;
    if ( !isInitialised )
    {
        Display* xdisplay = wxGlobalDisplay();

        // Get the clipboard atom
        XA_CLIPBOARD = XInternAtom(xdisplay, "CLIPBOARD", True);
        // Get UTF-8 string atom
        XA_UTF8_STRING = XInternAtom(xdisplay, "UTF8_STRING", True);
        // Get TAEGETS atom
        XA_TARGETS = XInternAtom(xdisplay, "TARGETS", True);
        XA_MULTIPLE = XInternAtom(xdisplay, "MULTIPLE", True);
        XA_IMAGE_BMP = XInternAtom(xdisplay, "image/bmp", True);
        XA_IMAGE_JPG = XInternAtom(xdisplay, "image/jpeg", True);
        XA_IMAGE_TIFF = XInternAtom(xdisplay, "image/tiff", True);
        XA_IMAGE_PNG = XInternAtom(xdisplay, "image/png", True);
        XA_TEXT_URI_LIST = XInternAtom(xdisplay, "text/uri-list", True);
        XA_TEXT_URI= XInternAtom(xdisplay, "text/uri", True);
        XA_TEXT_PLAIN = XInternAtom(xdisplay, "text/plain", True);
        XA_TEXT = XInternAtom(xdisplay, "Text", True);
        // already initialised
        isInitialised = true;
    }
}

//-----------------------------------------------------------------------------
// wxClipboard
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboard,wxObject)

wxClipboard::wxClipboard()
{
    m_open = false;

    m_ownsClipboard = false;
    m_ownsPrimarySelection = false;

    m_data = NULL;
    m_receivedData = NULL;

    /* we use m_targetsWidget to query what formats are available */

    /* we use m_clipboardWidget to get and to offer data */
#if !wxUSE_NANOX
    if (!g_clipboardAtom) g_clipboardAtom = XInternAtom( (Display*) wxGetDisplay(), "CLIPBOARD", False );
    if (!g_targetsAtom) g_targetsAtom = XInternAtom( (Display*) wxGetDisplay(), "TARGETS", False );
#endif

    m_formatSupported = false;
    m_targetRequested = 0;
}

wxClipboard::~wxClipboard()
{
    Clear();
}

void wxClipboard::Clear()
{
    if (m_data)
    {
#if wxUSE_THREADS
        /* disable GUI threads */
#endif

        wxDELETE(m_data);

#if wxUSE_THREADS
        /* re-enable GUI threads */
#endif
    }

    m_targetRequested = 0;
    m_formatSupported = false;
}

bool wxClipboard::Open()
{
    wxCHECK_MSG( !m_open, false, wxT("clipboard already open") );

    m_open = true;

    // initialize some atoms
    InitX11Clipboard();

    return true;
}

bool wxClipboard::SetData( wxDataObject *data )
{
    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );

    wxCHECK_MSG( data, false, wxT("data is invalid") );

    Clear();

    return AddData( data );
}

bool wxClipboard::AddData( wxDataObject *data )
{
#if wxUSE_NANOX
    return false;
#else
    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );

    wxCHECK_MSG( data, false, wxT("data is invalid") );

    /* we can only store one wxDataObject */
    Clear();

    m_data = data;

    wxTextDataObject* textdata = (wxTextDataObject*)data;

    Display* xdisplay = wxGlobalDisplay();
    int xscreen = DefaultScreen(xdisplay);
    Window window = RootWindow(xdisplay, xscreen);

    // Send the data to "clipboard".
    // "clipboard" means root window property
    XChangeProperty(xdisplay, window, XA_CLIPBOARD, XA_CLIPBOARD, 8, PropModeReplace,
                    (const unsigned char*)textdata->GetText().mb_str().data(),
                    textdata->GetTextLength());

    Window selectionOwner;

    // Set this window as the owner of the CLIPBOARD atom
    XSetSelectionOwner(xdisplay, XA_CLIPBOARD, window, CurrentTime);

    // Check if we accuired ownershop or not
    selectionOwner = XGetSelectionOwner(xdisplay, XA_CLIPBOARD);

    // Got ownership
    if ( selectionOwner == window )
    {
        return true;
    }
    else
    {
        return false;
    }

#endif
}

void wxClipboard::Close()
{
    wxCHECK_RET( m_open, wxT("clipboard not open") );

    m_open = false;
}

bool wxClipboard::IsOpened() const
{
    return m_open;
}

bool wxClipboard::IsSupported( const wxDataFormat& format )
{
    // TODO: this implementation only support copy/paste text for now.
    // remove the code that paste from gtk1 port.

    return format == wxDF_UNICODETEXT;
}

bool wxClipboard::GetData( wxDataObject& data )
{
    // prepare and find the root window,
    // the copied data stored in the root window as window property
    Display* xdisplay = wxGlobalDisplay();
    int xscreen = DefaultScreen(xdisplay);
    Window window = RootWindow(xdisplay, xscreen);

    // the window that hold the copied data.
    // this window just indicate whether there has data copied
    Window selectionOwner;

    // Get the owner of the clipboard selection
    selectionOwner = XGetSelectionOwner(xdisplay, XA_CLIPBOARD);

    // some variables that used to get the data in window property
    char *text = NULL;
    Atom type;
    unsigned char* textdata = NULL;
    int format, result;
    unsigned long len, bytesLeft, dummy;

    // there has something in the "clipboard"
    if ( selectionOwner != None )
    {
        XConvertSelection( xdisplay, XA_CLIPBOARD, XA_UTF8_STRING,
                XA_CLIPBOARD, window, CurrentTime);

        XGetWindowProperty(xdisplay, window, XA_CLIPBOARD, 0, 0, False, AnyPropertyType, &type,
                &format, &len, &bytesLeft, &textdata);

        if ( textdata )
        {
            XFree(textdata);
            textdata = NULL;
        }

        if ( bytesLeft )
        {
            // Fetch the data
            result = XGetWindowProperty(xdisplay,
                    window,
                    XA_CLIPBOARD, 0,
                    bytesLeft, False, AnyPropertyType,
                    &type, &format, &len, &dummy, &textdata);

            // if we got any data, copy it.
            if ( result == Success )
            {
                text = strdup((char*) textdata);
                XFree(textdata);
                XDeleteProperty(xdisplay, window, XA_CLIPBOARD);

                if ( data.IsSupported(wxDF_UNICODETEXT))
                {
                    data.SetData(wxDF_UNICODETEXT, len, text);
                }
                return true;
            }
        }
    }
    return false;
}

#endif
// wxUSE_CLIPBOARD
