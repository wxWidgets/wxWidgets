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

    // flushes the clipboard: not available under GTK
    virtual bool Flush() { return FALSE; }
    
    /// If primary == TRUE, use primary selection in all further ops,
    /// primary=FALSE resets it.
    inline void UsePrimarySelection(bool primary = TRUE) { m_usePrimary = primary; }
    
  // implementation
    bool              m_open;
    bool              m_ownsClipboard;
    bool              m_ownsPrimarySelection;
    wxDataObject     *m_data;

    GtkWidget        *m_clipboardWidget;  /* for getting and offering data */
    GtkWidget        *m_targetsWidget;    /* for getting list of supported formats */
    bool              m_waiting;          /* querying data or formats is asynchronous */

    bool              m_formatSupported;
    GdkAtom           m_targetRequested;
    bool              m_usePrimary;
    wxDataObject      *m_receivedData;
    
private:
    DECLARE_DYNAMIC_CLASS(wxClipboard)
};

//-----------------------------------------------------------------------------
// wxClipboardModule
//-----------------------------------------------------------------------------

class wxClipboardModule: public wxModule
{
public:
    wxClipboardModule() {}
    bool OnInit();
    void OnExit();
    
private:
    DECLARE_DYNAMIC_CLASS(wxClipboardModule)
};

#endif

   // wxUSE_CLIPBOARD

#endif
    // __GTKCLIPBOARDH__
