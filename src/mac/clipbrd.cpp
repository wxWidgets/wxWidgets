/////////////////////////////////////////////////////////////////////////////
// Name:        clipbrd.cpp
// Purpose:     Clipboard functionality
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma implementation "clipbrd.h"
#endif

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/bitmap.h"
#include "wx/utils.h"
#include "wx/metafile.h"
#include "wx/clipbrd.h"

#include <string.h>

bool wxOpenClipboard()
{
    return TRUE;
}

bool wxCloseClipboard()
{
    return FALSE;
}

bool wxEmptyClipboard()
{
		ZeroScrap() ;
    return FALSE;
}

bool wxClipboardOpen()
{
    // TODO
    return FALSE;
}

bool wxIsClipboardFormatAvailable(int dataFormat)
{
    // TODO
    return FALSE;
}

bool wxSetClipboardData(int dataFormat, wxObject *obj, int width, int height)
{
    // TODO
    return FALSE;
}

wxObject *wxGetClipboardData(int dataFormat, long *len)
{
    // TODO
    return NULL;
}

int  wxEnumClipboardFormats(int dataFormat)
{
    // TODO
    return 0;
}

int  wxRegisterClipboardFormat(char *formatName)
{
    // TODO
    return 0;
}

bool wxGetClipboardFormatName(int dataFormat, char *formatName, int maxCount)
{
    // TODO
    return FALSE;
}

/*
 * Generalized clipboard implementation by Matthew Flatt
 */

wxClipboard::wxClipboard()
{
    m_clearOnExit = FALSE;
}

wxClipboard::~wxClipboard()
{
    if ( m_clearOnExit )
    {
        Clear();
    }
}

void wxClipboard::Clear()
{
}

bool wxClipboard::Flush()
{
    return FALSE;
}

bool wxClipboard::Open()
{
    return wxOpenClipboard();
}

bool wxClipboard::IsOpened() const
{
    return wxIsClipboardOpened();
}

static int FormatStringToID(char *str)
{
  if (!strcmp(str, "TEXT"))
    return wxDF_TEXT;

  return wxRegisterClipboardFormat(str);
}

bool wxClipboard::SetData( wxDataObject *data )
{
    (void)wxEmptyClipboard();

    if ( data )
        return AddData(data);
    else
        return TRUE;
}

bool wxClipboard::AddData( wxDataObject *data )
{
    wxCHECK_MSG( data, FALSE, wxT("data is invalid") );

#if wxUSE_DATAOBJ
    wxCHECK_MSG( wxIsClipboardOpened(), FALSE, wxT("clipboard not open") );

    wxDataFormat format = data->GetFormat();

    switch ( format )
    {
        case wxDF_TEXT:
        case wxDF_OEMTEXT:
        {
            wxTextDataObject* textDataObject = (wxTextDataObject*) data;
            wxString str(textDataObject->GetText());
            return wxSetClipboardData(format, str.c_str());
        }

        case wxDF_BITMAP:
        case wxDF_DIB:
        {
            wxBitmapDataObject* bitmapDataObject = (wxBitmapDataObject*) data;
            wxBitmap bitmap(bitmapDataObject->GetBitmap());
            return wxSetClipboardData(data->GetFormat(), &bitmap);
        }

#if wxUSE_METAFILE
        case wxDF_METAFILE:
        {
            wxMetafileDataObject* metaFileDataObject = 
                (wxMetafileDataObject*) data;
            wxMetafile metaFile = metaFileDataObject->GetMetafile();
            return wxSetClipboardData(wxDF_METAFILE, &metaFile,
                                      metaFileDataObject->GetWidth(),
                                      metaFileDataObject->GetHeight());
        }
#endif // wxUSE_METAFILE

        default:
            return wxSetClipboardData(data);
    }
#else // !wxUSE_DATAOBJ
    return FALSE;
#endif 
}

void wxClipboard::Close()
{
    wxCloseClipboard();
}

bool wxClipboard::IsSupported( wxDataFormat format )
{
    return wxIsClipboardFormatAvailable(format);
}

bool wxClipboard::GetData( wxDataObject& data )
{
#if wxUSE_DATAOBJ
    wxCHECK_MSG( wxIsClipboardOpened(), FALSE, wxT("clipboard not open") );

    wxDataFormat format = data.GetFormat();
    switch ( format )
    {
        case wxDF_TEXT:
        case wxDF_OEMTEXT:
        {
            wxTextDataObject& textDataObject = (wxTextDataObject &)data;
            char* s = (char*)wxGetClipboardData(format);
            if ( !s )
                return FALSE;

            textDataObject.SetText(s);
            delete [] s;

            return TRUE;
        }

        case wxDF_BITMAP:
        case wxDF_DIB:
        {
            wxBitmapDataObject& bitmapDataObject = (wxBitmapDataObject &)data;
            wxBitmap* bitmap = (wxBitmap *)wxGetClipboardData(data->GetFormat());
            if ( !bitmap )
                return FALSE;

            bitmapDataObject.SetBitmap(*bitmap);
            delete bitmap;

            return TRUE;
        }
#if wxUSE_METAFILE
        case wxDF_METAFILE:
        {
            wxMetafileDataObject& metaFileDataObject = (wxMetafileDataObject &)data;
            wxMetafile* metaFile = (wxMetafile *)wxGetClipboardData(wxDF_METAFILE);
            if ( !metaFile )
                return FALSE;

            metaFileDataObject.SetMetafile(*metaFile);
            delete metaFile;

            return TRUE;
        }
#endif // wxUSE_METAFILE
    }
#else // !wxUSE_DATAOBJ
    wxFAIL_MSG( wxT("no clipboard implementation") );
#endif
    return FALSE;
}
/*
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
*/

