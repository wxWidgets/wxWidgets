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

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxClipboard;

//-----------------------------------------------------------------------------
// global functions
//-----------------------------------------------------------------------------

void wxInitClipboard();
void wxDoneClipboard();

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern wxClipboard* wxTheClipboard;

//-----------------------------------------------------------------------------
// wxClipboard
//-----------------------------------------------------------------------------

class  wxClipboard: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxClipboard)

public:
 
  wxClipboard();
  ~wxClipboard();

  virtual void SetData( wxDataObject *data );
  virtual void *GetData( wxDataFormat format, size_t *length );
  virtual bool IsAvailable( wxDataFormat format );

 // implementation 
  
  wxDataObject  *m_data;
  char          *m_sentString, 
		*m_receivedString;
  void          *m_receivedTargets;
  size_t         m_receivedLength;
  GtkWidget     *m_clipboardWidget;
};

#endif
    // __GTKCLIPBOARDH__
