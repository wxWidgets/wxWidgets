///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.h
// Purpose:     declaration of the wxDropTarget class
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin, Robert Roebling
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////


#ifndef __GTKDNDH__
#define __GTKDNDH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"

//-------------------------------------------------------------------------
// conditional compilation
//-------------------------------------------------------------------------

#if (GTK_MINOR_VERSION > 0)
#define NEW_GTK_DND_CODE
#endif

#if wxUSE_DRAG_AND_DROP

#include "wx/object.h"
#include "wx/string.h"
#include "wx/dataobj.h"
#include "wx/cursor.h"
#include "wx/icon.h"
#include "wx/gdicmn.h"

//-------------------------------------------------------------------------
// classes
//-------------------------------------------------------------------------

class wxWindow;

class wxDropTarget;
class wxTextDropTarget;
class wxFileDropTarget;
class wxPrivateDropTarget;

class wxDropSource;

//-------------------------------------------------------------------------
// wxDropTarget
//-------------------------------------------------------------------------

class wxDropTarget: public wxObject
{
  public:

    wxDropTarget();
    ~wxDropTarget();
    
    virtual void OnEnter() { }
    virtual void OnLeave() { }
    virtual void OnMouseMove( long WXUNUSED(x), long WXUNUSED(y) ) { }
    virtual bool OnDrop( long x, long y, const void *data, size_t size ) = 0;

    // Override these to indicate what kind of data you support: 
  
    virtual size_t GetFormatCount() const = 0;
    virtual wxDataFormat &GetFormat(size_t n) const;
  
  // implementation
  
    void RegisterWidget( GtkWidget *widget );
    void UnregisterWidget( GtkWidget *widget );
    
    wxDataFormat  *m_format;
};

//-------------------------------------------------------------------------
// wxTextDropTarget
//-------------------------------------------------------------------------

class wxTextDropTarget: public wxDropTarget
{
  public:

    wxTextDropTarget();
    virtual bool OnDrop( long x, long y, const void *data, size_t size );
    virtual bool OnDropText( long x, long y, const char *psz );
    
  protected:
  
    virtual size_t GetFormatCount() const;
};

//-------------------------------------------------------------------------
// wxPrivateDropTarget
//-------------------------------------------------------------------------

class wxPrivateDropTarget: public wxDropTarget
{
public:

  wxPrivateDropTarget();
  
  // you have to override OnDrop to get at the data
    
  // the string ID identifies the format of clipboard or DnD data. a word
  // processor would e.g. add a wxTextDataObject and a wxPrivateDataObject
  // to the clipboard - the latter with the Id "application/wxword" or
  // "image/png".
    
  void SetId( const wxString& id );
    
  wxString GetId()
      { return m_id; }

private:

  virtual size_t GetFormatCount() const;
    
  wxString   m_id;
};

//----------------------------------------------------------------------------
// A drop target which accepts files (dragged from File Manager or Explorer)
//----------------------------------------------------------------------------

class wxFileDropTarget: public wxDropTarget
{
  public:
    
    wxFileDropTarget();
    
    virtual bool OnDrop( long x, long y, const void *data, size_t size );
    virtual bool OnDropFiles( long x, long y, 
                              size_t nFiles, const char * const aszFiles[] );

  protected:
  
    virtual size_t GetFormatCount() const;
};

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

enum wxDragResult
{
  wxDragError,    // error prevented the d&d operation from completing
  wxDragNone,     // drag target didn't accept the data
  wxDragCopy,     // the data was successfully copied
  wxDragMove,     // the data was successfully moved (MSW only)
  wxDragCancel    // the operation was cancelled by user (not an error)
};

class wxDropSource: public wxObject
{
  public:

    /* constructor. set data later with SetData() */
    wxDropSource( wxWindow *win, const wxIcon &go = wxNullIcon, const wxIcon &stop = wxNullIcon );
    
    /* constructor for setting one data object */
    wxDropSource( wxDataObject *data, wxWindow *win, const wxIcon &go = wxNullIcon, const wxIcon &stop = wxNullIcon );
    
    /* constructor for setting several data objects via wxDataBroker */
    wxDropSource( wxDataBroker *data, wxWindow *win );
    
    ~wxDropSource(void);
    
    /* set one dataobject */
    void SetData( wxDataBroker *data );
    
    /* set severa dataobjects via wxDataBroker */
    void SetData( wxDataObject *data );
    
    /* start drag action */
    wxDragResult DoDragDrop( bool bAllowMove = FALSE );
    
    /* override to give feedback */
    virtual bool GiveFeedback( wxDragResult WXUNUSED(effect), bool WXUNUSED(bScrolling) ) { return TRUE; }
  
  /* GTK implementation */
      
    void RegisterWindow(void);
    void UnregisterWindow(void);
  
    GtkWidget     *m_widget;
    wxWindow      *m_window;
    wxDragResult   m_retValue;
    wxDataBroker  *m_data;
    
    wxCursor      m_defaultCursor;
    wxCursor      m_goaheadCursor;
    
    wxIcon        m_goIcon;
    wxIcon        m_stopIcon;
};

#endif

   // wxUSE_DRAG_AND_DROP

#endif  
       //__GTKDNDH__

