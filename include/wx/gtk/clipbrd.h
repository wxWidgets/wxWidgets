/////////////////////////////////////////////////////////////////////////////
// Name:        clipboard.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKCLIPBOARDH__
#define __GTKCLIPBOARDH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"

#if wxUSE_CLIPBOARD

#include "wx/object.h"
#include "wx/list.h"
#include "wx/dataobj.h"
#include "wx/control.h"
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

class wxClipboard : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxClipboard)

public:
    wxClipboard();
    ~wxClipboard();

    // open the clipboard before SetData() and GetData()
    virtual bool Open();

    // close the clipboard after SetData() and GetData()
    virtual void Close();

    // set the clipboard data. all other formats will be deleted.
    virtual bool SetData( wxDataObject *data );

    // add to the clipboard data.
    virtual bool AddData( wxDataObject *data );

    // ask if data in correct format is available
    virtual bool IsSupported( wxDataFormat format );

    // fill data with data on the clipboard (if available)
    virtual bool GetData( wxDataObject *data );

    // clears wxTheClipboard and the system's clipboard if possible
    virtual void Clear();

    // implementation

    bool              m_open;

    bool              m_ownsClipboard;
    bool              m_ownsPrimarySelection;

    wxDataBroker     *m_dataBroker;
    GtkWidget        *m_clipboardWidget;

    bool              m_formatSupported;
    GdkAtom           m_targetRequested;

    wxDataObject      *m_receivedData;
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

   // wxUSE_CLIPBOARD

#endif
    // __GTKCLIPBOARDH__
