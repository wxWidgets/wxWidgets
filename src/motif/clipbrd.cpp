/////////////////////////////////////////////////////////////////////////////
// Name:        clipbrd.cpp
// Purpose:     Clipboard functionality
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma implementation "clipbrd.h"
#endif

#include "wx/defs.h"

#if wxUSE_CLIPBOARD

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/bitmap.h"
#include "wx/utils.h"
#include "wx/metafile.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"

#include <Xm/Xm.h>
#include <Xm/CutPaste.h>

#include <string.h>

#if !USE_SHARED_LIBRARY
// IMPLEMENT_DYNAMIC_CLASS(wxClipboard, wxObject)
// IMPLEMENT_ABSTRACT_CLASS(wxClipboardClient, wxObject)
#endif

static bool gs_clipboardIsOpen = FALSE;

bool wxOpenClipboard()
{
    if (!gs_clipboardIsOpen)
    {
        gs_clipboardIsOpen = TRUE;
        return TRUE;
    }
    else
        return FALSE;
}

bool wxCloseClipboard()
{
    if (gs_clipboardIsOpen)
    {
        gs_clipboardIsOpen = FALSE;
        return TRUE;
    }
    else
        return FALSE;
}

bool wxEmptyClipboard()
{
    // No equivalent in Motif
    return TRUE;
}

bool wxClipboardOpen()
{
    return gs_clipboardIsOpen;
}

bool wxIsClipboardFormatAvailable(wxDataFormat dataFormat)
{
    // Only text is supported.
    if (dataFormat != wxDF_TEXT)
        return FALSE;

    unsigned long numBytes = 0;
    long privateId = 0;
    
    Window window = (Window) 0;
    if (wxTheApp->GetTopWindow())
        window = XtWindow( (Widget) wxTheApp->GetTopWindow()->GetTopWidget() );

    int success = XmClipboardRetrieve((Display*) wxGetDisplay(),
      window, "TEXT", (XtPointer) 0, 0, & numBytes, & privateId) ;

    // Assume only text is supported. If we have anything at all,
    // or the clipboard is locked so we're not sure, we say we support it.
    if (success == ClipboardNoData)
        return FALSE;
    else
        return TRUE;
}

bool wxSetClipboardData(wxDataFormat dataFormat, wxObject *obj, int width, int height)
{
    if (dataFormat != wxDF_TEXT)
        return FALSE;

    char* data = (char*) obj;

    XmString text = XmStringCreateSimple ("CLIPBOARD");
    Window window = (Window) 0;
    if (wxTheApp->GetTopWindow())
        window = XtWindow( (Widget) wxTheApp->GetTopWindow()->GetTopWidget() );

    long itemId = 0;
    int result = 0;

    while ((result =
      XmClipboardStartCopy((Display*) wxGetDisplay(),
       window, 
       text,
       XtLastTimestampProcessed((Display*) wxGetDisplay()),
       (Widget) 0,
       (XmCutPasteProc) 0,
       & itemId)) != ClipboardSuccess)

       ;

    XmStringFree (text);

    long dataId = 0;
    while ((result =
      XmClipboardCopy((Display*) wxGetDisplay(),
       window, 
       itemId,
       "TEXT",
       (XtPointer) data,
       strlen(data) + 1,
       0,
       & dataId)) != ClipboardSuccess)

       ;

    while (( result =
       XmClipboardEndCopy((Display*) wxGetDisplay(),
         window, itemId) ) != ClipboardSuccess)

       ;
    
    return TRUE;
}

wxObject *wxGetClipboardData(wxDataFormat dataFormat, long *len)
{
    if (dataFormat != wxDF_TEXT)
        return (wxObject*) NULL;

    bool done = FALSE;
    long id = 0;
    unsigned long numBytes = 0;
    int result = 0;
    Window window = (Window) 0;
    if (wxTheApp->GetTopWindow())
        window = XtWindow( (Widget) wxTheApp->GetTopWindow()->GetTopWidget() );

    int currentDataSize = 256;
    char* data = new char[currentDataSize];

    while (!done)
    {
        if (result == ClipboardTruncate)
        {
            delete[] data;
            currentDataSize = 2*currentDataSize;
            data = new char[currentDataSize];
	}
        result = XmClipboardRetrieve((Display*) wxGetDisplay(),
          window,
          "TEXT",
          (XtPointer) data,
          currentDataSize,
          &numBytes,
          &id);

        switch (result)
	  {
	  case ClipboardSuccess:
	    {
              if (len)
                *len = strlen(data) + 1;
              return (wxObject*) data;
              break;
	    }
	  case ClipboardTruncate:
	  case ClipboardLocked:
	    {
              break;
	    }
          default:
	  case ClipboardNoData:
	    {
              return (wxObject*) NULL;
              break;
	    }
	  }
          
    }

    return NULL;
}

wxDataFormat  wxEnumClipboardFormats(wxDataFormat dataFormat)
{
    // Only wxDF_TEXT supported
    if (dataFormat == wxDF_TEXT)
       return wxDF_TEXT;
    else
       return wxDF_INVALID;
}

wxDataFormat  wxRegisterClipboardFormat(char *formatName)
{
    // Not supported
    return (wxDataFormat) wxDF_INVALID;
}

bool wxGetClipboardFormatName(wxDataFormat dataFormat, char *formatName, int maxCount)
{
    // Only wxDF_TEXT supported
    if (dataFormat == wxDF_TEXT)
    {
       strcpy(formatName, "TEXT");
       return TRUE;
    }
    else
       return FALSE;
}

//-----------------------------------------------------------------------------
// wxClipboard
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboard,wxObject)

wxClipboard* wxTheClipboard = (wxClipboard*) NULL;

wxClipboard::wxClipboard()
{
    m_open = FALSE;
}

wxClipboard::~wxClipboard()
{
    Clear();  
}

void wxClipboard::Clear()
{
    wxNode* node = m_data.First();
    while (node)
    {
        wxDataObject* data = (wxDataObject*) node->Data();
        delete data;
        node = node->Next();
    }
    m_data.Clear();
}

bool wxClipboard::Open()
{
    wxCHECK_MSG( !m_open, FALSE, "clipboard already open" );
  
    m_open = TRUE;

    return wxOpenClipboard();
}

bool wxClipboard::SetData( wxDataObject *data )
{
    wxCHECK_MSG( data, FALSE, "data is invalid" );
    wxCHECK_MSG( m_open, FALSE, "clipboard not open" );

    Clear();

    return AddData( data );
}

bool wxClipboard::AddData( wxDataObject *data )
{
    wxCHECK_MSG( data, FALSE, "data is invalid" );
    wxCHECK_MSG( m_open, FALSE, "clipboard not open" );

    wxDataFormat::NativeFormat format = data->GetPreferredFormat().GetType();
    switch ( format )
    {
        case wxDF_TEXT:
        case wxDF_OEMTEXT:
        {
            wxTextDataObject* textDataObject = (wxTextDataObject*) data;
            wxString str(textDataObject->GetText());
            return wxSetClipboardData(format, (wxObject*) (const char*) str);
        }
#if 0
        case wxDF_BITMAP:
        case wxDF_DIB:
        {
            wxBitmapDataObject* bitmapDataObject = (wxBitmapDataObject*) data;
            wxBitmap bitmap(bitmapDataObject->GetBitmap());
            return wxSetClipboardData(data->GetType(), & bitmap);
            break;
        }
#endif // 0
    }
  
    return FALSE;
}

void wxClipboard::Close()
{
    wxCHECK_RET( m_open, "clipboard not open" );
    
    m_open = FALSE;
    wxCloseClipboard();
}

bool wxClipboard::IsSupported( const wxDataFormat& format)
{
    return wxIsClipboardFormatAvailable(format);
}

bool wxClipboard::GetData( wxDataObject& data )
{
    wxCHECK_MSG( m_open, FALSE, "clipboard not open" );
    
    wxDataFormat::NativeFormat format = data.GetPreferredFormat().GetType();
    switch ( format )
    {
        case wxDF_TEXT:
        case wxDF_OEMTEXT:
        {
            wxTextDataObject& textDataObject = (wxTextDataObject &) data;
            char* s = (char*) wxGetClipboardData(format);
            if (s)
            {
                textDataObject.SetText(s);
                delete[] s;
                return TRUE;
            }
            else
                return FALSE;
            break;
        }
/*
        case wxDF_BITMAP:
        case wxDF_DIB:
        {
            wxBitmapDataObject* bitmapDataObject = (wxBitmapDataObject*) data;
            wxBitmap* bitmap = (wxBitmap*) wxGetClipboardData(data->GetType());
            if (bitmap)
            {
                bitmapDataObject->SetBitmap(* bitmap);
                delete bitmap;
                return TRUE;
            }
            else
                return FALSE;
            break;
        }
*/
        default:
        {
            return FALSE;
        }
    }
    return FALSE;
}

#if 0

/*
* Old clipboard implementation by Matthew Flatt
*/

wxClipboard *wxTheClipboard = NULL;

void wxInitClipboard()
{
    if (!wxTheClipboard)
        wxTheClipboard = new wxClipboard;
}

wxClipboard::wxClipboard()
{
    clipOwner = NULL;
    cbString = NULL;
}

wxClipboard::~wxClipboard()
{
    if (clipOwner)
        clipOwner->BeingReplaced();
    if (cbString)
        delete[] cbString;
}

static int FormatStringToID(char *str)
{
    if (!strcmp(str, "TEXT"))
        return wxDF_TEXT;
    
    return wxRegisterClipboardFormat(str);
}

void wxClipboard::SetClipboardClient(wxClipboardClient *client, long time)
{
    bool got_selection;
    
    if (clipOwner)
        clipOwner->BeingReplaced();
    clipOwner = client;
    if (cbString) {
        delete[] cbString;
        cbString = NULL;
    }
    
    if (wxOpenClipboard()) {
        char **formats, *data;
        int i;
        int ftype;
        long size;
        
        formats = clipOwner->formats.ListToArray(FALSE);
        for (i = clipOwner->formats.Number(); i--; ) {
            ftype = FormatStringToID(formats[i]);
            data = clipOwner->GetData(formats[i], &size);
            if (!wxSetClipboardData(ftype, (wxObject *)data, size, 1)) {
                got_selection = FALSE;
                break;
            }
        }
        
        if (i < 0)
            got_selection = wxCloseClipboard();
    } else
        got_selection = FALSE;
    
    got_selection = FALSE; // Assume another process takes over
    
    if (!got_selection) {
        clipOwner->BeingReplaced();
        clipOwner = NULL;
    }
}

wxClipboardClient *wxClipboard::GetClipboardClient()
{
    return clipOwner;
}

void wxClipboard::SetClipboardString(char *str, long time)
{
    bool got_selection;
    
    if (clipOwner) {
        clipOwner->BeingReplaced();
        clipOwner = NULL;
    }
    if (cbString)
        delete[] cbString;
    
    cbString = str;
    
    if (wxOpenClipboard()) {
        if (!wxSetClipboardData(wxDF_TEXT, (wxObject *)str))
            got_selection = FALSE;
        else
            got_selection = wxCloseClipboard();
    } else
        got_selection = FALSE;
    
    got_selection = FALSE; // Assume another process takes over
    
    if (!got_selection) {
        delete[] cbString;
        cbString = NULL;
    }
}

char *wxClipboard::GetClipboardString(long time)
{
    char *str;
    long length;
    
    str = GetClipboardData("TEXT", &length, time);
    if (!str) {
        str = new char[1];
        *str = 0;
    }
    
    return str;
}

char *wxClipboard::GetClipboardData(char *format, long *length, long time)
{
    if (clipOwner)  {
        if (clipOwner->formats.Member(format))
            return clipOwner->GetData(format, length);
        else
            return NULL;
    } else if (cbString) {
        if (!strcmp(format, "TEXT"))
            return copystring(cbString);
        else
            return NULL;
    } else {
        if (wxOpenClipboard()) {
            receivedString = (char *)wxGetClipboardData(FormatStringToID(format), 
                length);
            wxCloseClipboard();
        } else
            receivedString = NULL;
        
        return receivedString;
    }
}
#endif

#endif // wxUSE_CLIPBOARD
