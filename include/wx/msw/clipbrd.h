/////////////////////////////////////////////////////////////////////////////
// Name:        clipbrd.h
// Purpose:     Clipboard functionality
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __CLIPBRDH__
#define __CLIPBRDH__

#ifdef __GNUG__
#pragma interface "clipbrd.h"
#endif

#include "wx/defs.h"
#include "wx/setup.h"

#if USE_CLIPBOARD

#include "wx/list.h"

bool WXDLLEXPORT wxOpenClipboard(void);
bool WXDLLEXPORT wxClipboardOpen(void);
bool WXDLLEXPORT wxCloseClipboard(void);
bool WXDLLEXPORT wxEmptyClipboard(void);
bool WXDLLEXPORT wxIsClipboardFormatAvailable(int dataFormat);
bool WXDLLEXPORT wxSetClipboardData(int dataFormat, wxObject *obj, int width = 0, int height = 0);
wxObject* WXDLLEXPORT wxGetClipboardData(int dataFormat, long *len = NULL);
int  WXDLLEXPORT wxEnumClipboardFormats(int dataFormat);
int  WXDLLEXPORT wxRegisterClipboardFormat(char *formatName);
bool WXDLLEXPORT wxGetClipboardFormatName(int dataFormat, char *formatName, int maxCount);

/* The following is Matthew Flatt's implementation of the MSW
 * side of generic clipboard functionality.
 */

/* A clipboard client holds data belonging to the clipboard.
   For plain text, a client is not necessary. */
class WXDLLEXPORT wxClipboardClient : public wxObject
{
  DECLARE_ABSTRACT_CLASS(wxClipboardClient)

 public:
  /* This list should be filled in with strings indicating the formats
     this client can provide. Almost all clients will provide "TEXT".
     Format names should be 4 characters long, so things will work
     out on the Macintosh */
  wxStringList formats;

  /* This method is called when the client is losing the selection. */
  virtual void BeingReplaced(void) = 0;

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
#ifdef __XVIEW__
  long sel_owner;
#endif

  wxClipboard();
  ~wxClipboard();

  /* Set the clipboard data owner. "time" comes from the event record. */
  void SetClipboardClient(wxClipboardClient *, long time);

  /* Set the clipboard string; does not require a client. */
  void SetClipboardString(char *, long time);

  /* Get data from the clipboard in the format "TEXT". */
  char *GetClipboardString(long time);

  /* Get data from the clipboard */
  char *GetClipboardData(char *format, long *length, long time);

  /* Get the clipboard client directly. Will be NULL if clipboard data
     is a string, or if some other application owns the clipboard. 
     This can be useful for shortcutting data translation, if the
     clipboard user can check for a specific client. (This is used
     by the wxMediaEdit class.) */
  wxClipboardClient *GetClipboardClient(void);
};

/* Initialize wxTheClipboard. Can be called repeatedly */
void WXDLLEXPORT wxInitClipboard(void);

/* The clipboard */
extern wxClipboard* WXDLLEXPORT wxTheClipboard;

#endif // USE_CLIPBOARD
#endif
    // __CLIPBRDH__
