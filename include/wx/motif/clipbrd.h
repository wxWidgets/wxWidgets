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

#include "wx/defs.h"

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

class WXDLLEXPORT wxDataObject;
class WXDLLEXPORT wxClipboard: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxClipboard)

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

  /// If primary == TRUE, use primary selection in all further ops,
  /// primary=FALSE resets it.
  inline void UsePrimarySelection(bool primary = TRUE) { m_usePrimary = primary; }

 // implementation

  bool              m_open;
  wxList            m_data;
  bool              m_usePrimary;
};

/* The clipboard */
WXDLLEXPORT_DATA(extern wxClipboard*) wxTheClipboard;

//-----------------------------------------------------------------------------
// wxClipboardModule
//-----------------------------------------------------------------------------

class wxClipboardModule: public wxModule
{
  DECLARE_DYNAMIC_CLASS(wxClipboardModule)

public:
    wxClipboardModule() {}
    bool OnInit();
    void OnExit();
};

// This is the old, 1.68 implementation
#if 0

/* A clipboard client holds data belonging to the clipboard.
   For plain text, a client is not necessary. */
class WXDLLEXPORT wxClipboardClient : public wxObject
{
  DECLARE_ABSTRACT_CLASS(wxClipboardClient)

 public:
  /* This list should be filled in with strings indicating the formats
     this client can provide. Almost all clients will provide "TEXT(".
     Format names should be 4 characters long, so things will work
     out on the Macintosh */
  wxStringList formats;

  /* This method is called when the client is losing the selection. */
  virtual void BeingReplaced() = 0;

  /* This method is called when someone wants the data this client is
     supplying to the clipboard. "format" is a string indicating the
     format of the data - one of the strings from the "formats"
     list. "*size" should be filled with the size of the resulting
     data. In the case of text, "*size" does not count the
     NULL terminator. */
  virtual char *GetData(char *format, long *size) = 0;
};

/* ONE instance of this class: */
class WXDLLEXPORT wxClipboard : public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxClipboard)

 public:
  wxClipboardClient *clipOwner;
  char *cbString, *sentString, *receivedString;
  void *receivedTargets;
  long receivedLength;

  wxClipboard();
  ~wxClipboard();

  /* Set the clipboard data owner. "time" comes from the event record. */
  void SetClipboardClient(wxClipboardClient *, long time);

  /* Set the clipboard string; does not require a client. */
  void SetClipboardString(char *, long time);

  /* Get data from the clipboard in the format "TEXT(". */
  char *GetClipboardString(long time);

  /* Get data from the clipboard */
  char *GetClipboardData(char *format, long *length, long time);

  /* Get the clipboard client directly. Will be NULL if clipboard data
     is a string, or if some other application owns the clipboard.
     This can be useful for shortcutting data translation, if the
     clipboard user can check for a specific client. (This is used
     by the wxMediaEdit class.) */
  wxClipboardClient *GetClipboardClient();
};

/* Initialize wxTheClipboard. Can be called repeatedly */
void WXDLLEXPORT wxInitClipboard();

/* The clipboard */
WXDLLEXPORT_DATA(extern wxClipboard*) wxTheClipboard;

#endif
  // Old clipboard class

#endif // wxUSE_CLIPBOARD

#endif
    // _WX_CLIPBRD_H_
