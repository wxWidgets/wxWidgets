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

  ~wxDropSource();
    
  /* set several dataobjects via wxDataBroker */
  void SetData( wxDataBroker *data );

  /* set one dataobject */
  void SetData( wxDataObject *data );

  /* start drag action */
  wxDragResult DoDragDrop( bool bAllowMove = FALSE );
    
  /* override to give feedback */
  virtual bool GiveFeedback( wxDragResult WXUNUSED(effect), bool WXUNUSED(bScrolling) ) { return TRUE; }
  
  /* GTK implementation */
      
  void RegisterWindow();
  void UnregisterWindow();
  
    GtkWidget     *m_widget;
    wxWindow      *m_window;
    wxDragResult   m_retValue;
    wxDataBroker  *m_data;
    
    wxCursor      m_defaultCursor;
    wxCursor      m_goaheadCursor;
    
    wxIcon        m_goIcon;
    wxIcon        m_stopIcon;
    
    bool          m_waiting;
};

#include "gtk/gtk.h"
#if (GTK_MINOR_VERSION > 0)

//-------------------------------------------------------------------------
// wxDropTarget
//-------------------------------------------------------------------------

class wxDropTarget: public wxObject
{
public:

  wxDropTarget();
  ~wxDropTarget();
    
  /* may be overridden to react to events */
  virtual void OnEnter();
  virtual void OnLeave();
  
  /* may be overridden to reject certain formats or drops
     on certain areas */
  virtual bool OnMove( int x, int y );
    
  /* has to be overridden to accept drop. call GetData() to
     indicate the format you request and get the data. */
  virtual bool OnDrop( int x, int y );

  /* called to query what formats are available */
  bool IsSupported( wxDataFormat format );
  
  /* fill data with data from the dragging source */
  bool GetData( wxDataObject *data );

// implementation
  
  void RegisterWidget( GtkWidget *widget );
  void UnregisterWidget( GtkWidget *widget );
    
  GdkDragContext     *m_dragContext;
  GtkWidget          *m_dragWidget;
  guint               m_dragTime;
  bool                m_firstMotion;     /* gdk has no "gdk_drag_enter" event */
  bool                m_waiting;         /* asynchronous process */
  bool                m_dataRetrieveSuccess;
  wxDataObject       *m_currentDataObject;
    
  void SetDragContext( GdkDragContext *dc ) { m_dragContext = dc; }
  void SetDragWidget( GtkWidget *w ) { m_dragWidget = w; }
  void SetDragTime( guint time ) { m_dragTime = time; }
};

//-------------------------------------------------------------------------
// wxTextDropTarget
//-------------------------------------------------------------------------

class wxTextDropTarget: public wxDropTarget
{
public:

  wxTextDropTarget() {}

  virtual bool OnMove( int x, int y );
  virtual bool OnDrop( int x, int y );
    
  /* you have to override OnDropData to get at the text */
  virtual bool OnDropText( int x, int y, const char *text ) = 0;
    
};

//-------------------------------------------------------------------------
// wxPrivateDropTarget
//-------------------------------------------------------------------------

class wxPrivateDropTarget: public wxDropTarget
{
public:

  /* sets id to "application/myprogram" where "myprogram" is the
     same as wxApp->GetAppName() */
  wxPrivateDropTarget();
  /* see SetId() below for explanation */
  wxPrivateDropTarget( const wxString &id );
  
  virtual bool OnMove( int x, int y );
  virtual bool OnDrop( int x, int y );
  
  /* you have to override OnDropData to get at the data */
  virtual bool OnDropData( int x, int y, void *data, size_t size ) = 0;
    
  /* the string ID identifies the format of clipboard or DnD data. a word
     processor would e.g. add a wxTextDataObject and a wxPrivateDataObject
     to the clipboard - the latter with the Id "application/wxword" or
     "image/png". */
  void SetId( const wxString& id ) { m_id = id; }
  wxString GetId() { return m_id; }
      
private:

    wxString   m_id;
};

//----------------------------------------------------------------------------
// A drop target which accepts files (dragged from File Manager or Explorer)
//----------------------------------------------------------------------------

class wxFileDropTarget: public wxDropTarget
{
public:
    
  wxFileDropTarget() {}
    
  virtual bool OnMove( int x, int y );
  virtual bool OnDrop( int x, int y );
  virtual void OnData( int x, int y );
  
  /* you have to override OnDropFiles to get at the file names */
  virtual bool OnDropFiles( int x, int y, size_t nFiles, const char * const aszFiles[] ) = 0;

};

#else

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

#endif


#endif

   // wxUSE_DRAG_AND_DROP

#endif  
       //__GTKDNDH__

