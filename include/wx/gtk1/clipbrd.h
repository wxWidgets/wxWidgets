/////////////////////////////////////////////////////////////////////////////
// Name:        clipboard.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKCLIPBOARDH__
#define __GTKCLIPBOARDH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"
#include "wx/dnd.h"      // for wxDataObject
#include "wx/module.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxClipboard;
class wxClipboardModule;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern wxClipboard* wxTheClipboard;

//-----------------------------------------------------------------------------
// wxClipboard
//-----------------------------------------------------------------------------

class wxClipboard: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxClipboard)

public:
 
  wxClipboard();
  ~wxClipboard();

  virtual void SetData( wxDataObject *data );
  
  virtual bool IsSupportedFormat( wxDataFormat format );
  virtual bool ObtainData( wxDataFormat format );
  
  // call these after ObtainData()
  virtual size_t GetDataSize() const;
  virtual void GetDataHere( void *data ) const;
  
  // clears wxTheClipboard and the system's clipboard if possible
  virtual void Clear();

 // implementation 
  
  wxDataObject     *m_data;
  char             *m_sentString, 
		   *m_receivedString;
  void             *m_receivedTargets;
  GtkWidget        *m_clipboardWidget;
  
  bool              m_formatSupported;
  GdkAtom           m_targetRequested;

  size_t            m_receivedSize;
  char              *m_receivedData;
};

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


#endif
    // __GTKCLIPBOARDH__
