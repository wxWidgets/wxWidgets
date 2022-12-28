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
#include "wx/scopedarray.h"

// for store the formats that in a wxDataObject
typedef wxScopedArray<wxDataFormat> wxDataFormatScopedArray;

// ----------------------------------------------
// The mechanism of clipboard under X11 platform:
// ----------------------------------------------

// X11 platform doesn't has a global clipboard like Windows by default. So the
// first problem is the application doing the pasting has to first know where
// to get the data from.

// It solved by X server: The copied data is store in host program(src
// program) it self. When other program(dest program) want to paste data that
// copied in the src program. It will send an paste request, in x11, it will
// send an event which type is SelectionRequest by XConvertSelection, then X
// server will try to find a program that could handle SelectionRequest event.
// In src program, when recieve a SelectionRequest event, it will set the
// copied data to dest program's window property. More specific, the dest
// program is called "requestor", the src program could find which window ask
// for the data, through the event member : "event.xselection.requestor".
// Esentially, the pasting application asks for a list of available formats,
// and then picks the one it deems most suitable.

// -------------------------
// X11 clipboard background:
// -------------------------

// -----
// Atoms
// -----

// Atom is a 4 byte integer ID that used to identify Properties and
// Selections. Properties and Selection have name. But use Atom is more
// efficiency, because it just needs to pass and compare integer ID rather
// than a character string.

// XInterAtom() can gets the atom numer corresponding to a string.
// ie: Atom a = XInternAtom(xdisplay, "CLIPBOARD", True);
// XGetAtomName gets the string corresponding ot the atom number.
// ie: string clip = XGetAtomName(disp, a);
// The content of clip will be "CLIPBOARD".

// ----------
// Properties
// ----------

// Each window has a list of properties. A property is a collection of named,
// typed data. Atom is their name. This is mean the property has an 4 byte
// integer ID and a ASCII string name that store in the correspongding Atom.

//Window have predefined properties, and users can define custom property.

// ----------
// Selections
// ----------

// Selection IS property, just renamed it because it used for exchange data
// between applications.

// Under x11, there have tow useful selections: PRIMARY and CLIPBOARD. PRIMARY
// is for highlight/middle click, CLIPBOARD is for Ctrl+C/Ctrl+V.

// For compability to other wx ports, here use CLIPBOARD Atom.

// ------------------------------------
// X11 clipboard implementation details
// ------------------------------------

// For x11, the first thing is define XA_CLIPBOARD atoem. Although
// X11/Xatom.h have some predefined Atoms, but for some unknown reasons,
// XA_CLIPBOARD is not an predefined Atom.
// ie. XA_CLIPBOARD = XInternAtom(xdisplay, "CLIPBOARD", True);

// The copied data is store in this Atom/Selection.

// -----
// Paste
// -----

// For paste data. First, we need to know which window owns the XA_CLIPBOAD
// selection: win = XGetSelectionOwner(xdisplay, XA_CLIPBOARD);

// If find a window that owns XA_CLIPBOARD, convert the selection to a prop
// atom: XConvertSelection(xdisplay, XA_CLIPBOARD, XA_STRING, XA_CLIPBOARD,
// win, CurrentTime);

// Then get the data in owner's selection through XGetWIndowProperty().

// ----
// Copy
// ----

// If we want copy data to clipboard. We must own the XA_CLIPBOARD selection
// through XSetSelectionOwner().

// But the data is still host by src program. When src program recieve
// SelectionRequest event type. It set the data to requestor's window
// property, through XCHangeProperty(xdisplay, requestor, ...). The second
// parameter is the requests window. Requestor could find through XEvent.

// --------------------
// wxX11 implementation
// --------------------

// According the descirption in above. The implementation of clipboard should
// accomlish these things: set wxDataObject to clipboard, get data from
// clipboard and store it to a wxDataObject.

// -----------------
// SetData function:
// -----------------

// In SetData, due to x11 program does not send data to a gloabal clipboard,
// so the program hold the data, when the program recieve a SelectionRequest
// event, the program set the data to requestor's window property. So in the
// implementation of SetData, it hold the wxDataObject that need to be paste.
// And set XA_CLIPBOARD selection owner.

// -----------------------------------
// wxClipboardHandleSelectionRequest()
// -----------------------------------

// Add a new function void wxClipboardHandleSelectionRequest(XEvent event), it
// called by wxApp::ProcessXEvent(), when other program want to paste data.
// wxApp will check the wxClipboard whether it has data that conforms with
// the format. If has, set the data to requestor's window property.

// The GetData function
// GetData function retieve the data in the owner of XA_CLIPBOARD.



//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

#if !wxUSE_NANOX
Atom  g_clipboardAtom   = 0;
Atom  g_targetsAtom     = 0;
#endif

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

// get the data from XA_CLIPBOARD with specific Atom type.
// currently the 3rd parameter is always XA_CLIPBOARD.
// this is because XA_PRIMARY is not often used.
// the 4th parameter is the format that requestor want get.
unsigned char *GetClipboardDataByFormat(Display* disp, Window win, Atom clipbrdType, Atom target,
                                         unsigned long *length)
{
    // some variables that used to get the data in window property
    unsigned char *clipbrddata = nullptr;
    int read_bytes = 1024;
    Atom type;
    int format, result;
    unsigned long bytes_after;

    XConvertSelection(disp, XA_CLIPBOARD, target, XA_CLIPBOARD, win, CurrentTime);

    do
    {
        if ( clipbrddata != nullptr )
        {
            XFree(clipbrddata);
            clipbrddata = nullptr;
        }

        result = XGetWindowProperty(disp, win, clipbrdType, 0, read_bytes, False, AnyPropertyType,
                                &type, &format, length, &bytes_after, &clipbrddata);
        read_bytes *= 2;
    } while ( bytes_after != 0 );

    // if we got any data, copy it.
    if ( result == Success && clipbrddata )
        return clipbrddata;

    return nullptr;
}

// get the data for a specific wxDataFormat that stored as a property in Root window
void GetClipboardData(Display* disp, Window win, wxDataObject &data, wxDataFormat dfFormat)
{
    unsigned char *clipbrdData = nullptr;

    // some variables that used to get the data in window property
    unsigned long len;

    switch ( dfFormat )
    {
        case wxDF_INVALID:
        {
            return;
        }
        case wxDF_BITMAP:
        {
            wxVector<Atom> atomVector;
            atomVector.push_back(XA_IMAGE_BMP);
            atomVector.push_back(XA_IMAGE_JPG);
            atomVector.push_back(XA_IMAGE_TIFF);
            atomVector.push_back(XA_IMAGE_PNG);

            // check the four atoms in clipboard, try to find whether there has data
            // stored in one of these atom.
            for ( unsigned i = 0; i < atomVector.size(); i++ )
            {

                clipbrdData  = GetClipboardDataByFormat(disp, win, XA_CLIPBOARD,
                                                        atomVector.at(i), &len);
                if ( clipbrdData != nullptr )
                    break;
            }
            // if we got any data, copy it.
            if ( clipbrdData )
            {
                data.SetData(dfFormat, len, (char*)clipbrdData);
            }
            break;
        }
        default:
        {
            clipbrdData  = GetClipboardDataByFormat(disp, win, XA_CLIPBOARD, XA_UTF8_STRING, &len);
            // if we got any data, copy it.
            if ( clipbrdData  )
            {
                data.SetData(dfFormat, len, (char*)clipbrdData);
            }
        }
    }
}

// This function is used to response the paste request.
// It convert the stored data into a acceptable format by destination
// program and send an acknowledgement.
// In x11, the "copied" data stored by original window,
// when a paste request arrived, then the original window will send the
// data to destination.
extern "C" void wxClipboardHandleSelectionRequest(XEvent event)
{
    if( event.type != SelectionRequest )
        return;

    //Extract the relavent data
    Atom target      = event.xselectionrequest.target;
    Atom property    = event.xselectionrequest.property;
    Window requestor = event.xselectionrequest.requestor;
    Time timestamp   = event.xselectionrequest.time;
    Display* disp    = event.xselection.display;

    // A selection request has arrived, we should know these values:
    // Selection atom
    // Target atom: the format that requetor want to;
    // Property atom: ;
    // Requestor: the window that want to paste data.

    //Replies to the application requesting a pasting are XEvenst
    //sent via XSendEvent
    XEvent response;

    //Start by constructing a refusal request.
    response.xselection.type = SelectionNotify;
    //s.xselection.serial     - filled in by server
    //s.xselection.send_event - filled in by server
    //s.xselection.display    - filled in by server
    response.xselection.requestor = requestor;
    response.xselection.selection = XA_CLIPBOARD;
    response.xselection.target    = target;
    response.xselection.property  = None;   //This means refusal
    response.xselection.time      = timestamp;

    // get formats count in the wxDataObject
    // for each data format, search it in x11 selection
    // and store it to wxDataObject
    wxDataObject* data = wxTheClipboard->m_data;
    size_t count = data->GetFormatCount();
    wxDataFormatScopedArray dfarr(count);
    data->GetAllFormats(dfarr.get());

    // retrieve the data with specific image Atom.
    wxVector<Atom> atomVector;
    atomVector.push_back(XA_IMAGE_BMP);
    atomVector.push_back(XA_IMAGE_JPG);
    atomVector.push_back(XA_IMAGE_TIFF);
    atomVector.push_back(XA_IMAGE_PNG);

    wxDataFormat dfFormat = wxDF_UNICODETEXT;

    for ( unsigned i = 0; i <= atomVector.size(); i++ )
    {
        if ( target == atomVector.at(i) )
        {
            dfFormat = wxDF_BITMAP;
            break;
        }
    }

    // Tell the requestor what we can provide
    if ( target == XA_TARGETS )
    {
        Atom possibleTargets[] =
        {
            XA_UTF8_STRING,
            XA_IMAGE_BMP,
            XA_IMAGE_JPG,
            XA_IMAGE_TIFF,
            XA_IMAGE_PNG,
            XA_TEXT_URI_LIST,
            XA_TEXT_URI,
            XA_TEXT_PLAIN,
            XA_TEXT,
            XA_STRING,
        };

        XChangeProperty(disp, requestor, property, XA_ATOM, 32, PropModeReplace,
                        (unsigned char *)possibleTargets, 2);
    }
    // the requested target is in possibleTargets
    // TODO, when finish the event process issue, improve the check.
    else if ( target == XA_UTF8_STRING )
    {
        size_t size = data->GetDataSize(dfFormat);
        wxCharTypeBuffer<unsigned char> buf(size);
        data->GetDataHere(dfFormat, buf.data());
        XChangeProperty(disp, requestor, XA_CLIPBOARD, target, 8, PropModeReplace,
                        buf.data(), size);
    }
    else
    {
        // we could not provide the data with a target that requestor want.
        return;
    }

    //Reply
    XSendEvent(disp, event.xselectionrequest.requestor, True, 0, &response);
}

//-----------------------------------------------------------------------------
// wxClipboard
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxClipboard,wxObject);

wxClipboard::wxClipboard()
{
    m_open = false;

    m_ownsClipboard = false;
    m_ownsPrimarySelection = false;

    m_data = nullptr;
    m_receivedData = nullptr;

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

    // in x11, the "copied data" hold by the program itself.
    // so here just use m_data to hold the "copied data"
    // use wxApp->ProcessXEvent to check whether there has
    // SelectionRequest event arrived. If the event arrived,
    // check the request format, if wx program has the request
    // format, reply the data.
    // Reply the data means fill up the data in requestor's
    // window property.
    // See HandleSelectionRequest for more details
    m_data = data;

    // prepare and find the root window,
    // the copied data stored in the root window as window property
    Display* xdisplay = wxGlobalDisplay();
    int xscreen = DefaultScreen(xdisplay);
    Window window = RootWindow(xdisplay, xscreen);

    size_t size = m_data->GetDataSize(wxDF_UNICODETEXT);
    wxCharTypeBuffer<unsigned char> buf(size);
    m_data->GetDataHere(wxDF_UNICODETEXT, buf.data());

    XChangeProperty(xdisplay, window, XA_CLIPBOARD, XA_STRING, 8, PropModeReplace,
                    buf.data(), size);

    XSetSelectionOwner(xdisplay, XA_CLIPBOARD, window, CurrentTime);
    XFlush(xdisplay);
    return true;
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

    return format == wxDF_UNICODETEXT;
}

bool wxClipboard::GetData( wxDataObject& data )
{
    // get formats count in the wxDataObject
    // for each data format, search it in x11 selection
    // and store it to wxDataObject
    size_t count = data.GetFormatCount();
    wxDataFormatScopedArray dfarr(count);
    data.GetAllFormats(dfarr.get());

    // prepare and find the root window,
    // the copied data stored in the root window as window property
    Display* xdisplay = wxGlobalDisplay();
    int xscreen = DefaultScreen(xdisplay);
    Window window = RootWindow(xdisplay, xscreen);

    // retrieve the data in each format.
    for( size_t i = 0; i < count; ++i )
    {
        GetClipboardData(xdisplay, window, data, dfarr[i]);
    }
    return true;
}


#endif
// wxUSE_CLIPBOARD
