/////////////////////////////////////////////////////////////////////////////
// Name:        clipbrd.h
// Purpose:     Clipboard functionality
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
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

// These functions superceded by wxClipboard, but retained in order to implement
// wxClipboard, and for compatibility.
WXDLLEXPORT bool wxOpenClipboard(void);
WXDLLEXPORT bool wxClipboardOpen(void);
WXDLLEXPORT bool wxCloseClipboard(void);
WXDLLEXPORT bool wxEmptyClipboard(void);
WXDLLEXPORT bool wxIsClipboardFormatAvailable(wxDataFormat dataFormat);
WXDLLEXPORT bool wxSetClipboardData(wxDataFormat dataFormat, wxObject *obj, int width = 0, int height = 0);
WXDLLEXPORT wxObject* wxGetClipboardData(wxDataFormat dataFormat, long *len = NULL);
WXDLLEXPORT wxDataFormat wxEnumClipboardFormats(wxDataFormat dataFormat);
WXDLLEXPORT int  wxRegisterClipboardFormat(char *formatName);
WXDLLEXPORT bool wxGetClipboardFormatName(wxDataFormat dataFormat, char *formatName, int maxCount);

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
  virtual bool IsSupportedFormat( wxDataFormat format, const wxString &id = wxEmptyString );
  
  // fill data with data on the clipboard (if available)
  virtual bool GetData( wxDataObject *data );
  
  // clears wxTheClipboard and the system's clipboard if possible
  virtual void Clear();

 // implementation 
 
  bool              m_open;
  wxList            m_data;
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

#endif // wxUSE_CLIPBOARD
#endif
    // _WX_CLIPBRD_H_
