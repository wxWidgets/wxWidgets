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
     on certain areas. always returns TRUE by default
     indicating that you'd accept the data from the drag. */
  virtual bool OnMove( long x, long y );

  /* has to be overridden to accept a drop event. call
     IsSupported() to ask which formats are available
     and then call RequestData() to indicate the format
     you request. */
  virtual bool OnDrop( long x, long y );

  /* this gets called once the data has actually arrived. get
     it with GetData(). this has to be overridden. */
  virtual bool OnData( long x, long y );

  /* called from within OnDrop() to request a certain format
     from the drop event. */
  bool RequestData( wxDataFormat format );

  /* called to query what formats are available */
  bool IsSupported( wxDataFormat format );

  /* fill data with data from the dragging source */
  bool GetData( wxDataObject *data );

// implementation

  void RegisterWidget( GtkWidget *widget );
  void UnregisterWidget( GtkWidget *widget );

  GdkDragContext     *m_dragContext;
  GtkWidget          *m_dragWidget;
  GtkSelectionData   *m_dragData;
  guint               m_dragTime;
  bool                m_firstMotion;     /* gdk has no "gdk_drag_enter" event */

  void SetDragContext( GdkDragContext *dc ) { m_dragContext = dc; }
  void SetDragWidget( GtkWidget *w ) { m_dragWidget = w; }
  void SetDragData( GtkSelectionData *sd ) { m_dragData = sd; }
  void SetDragTime( guint time ) { m_dragTime = time; }
};

//-------------------------------------------------------------------------
// wxTextDropTarget
//-------------------------------------------------------------------------

class wxTextDropTarget: public wxDropTarget
{
public:

  wxTextDropTarget() {}

  virtual bool OnMove( long x, long y );
  virtual bool OnDrop( long x, long y );
  virtual bool OnData( long x, long y );

  /* you have to override OnDropData to get at the text */
  virtual bool OnDropText( long x, long y, const wxChar *text ) = 0;

};

//-------------------------------------------------------------------------
// wxPrivateDropTarget
//-------------------------------------------------------------------------

/*
class wxPrivateDropTarget: public wxDropTarget
{
public:

  wxPrivateDropTarget();
  wxPrivateDropTarget( const wxString &id );

  virtual bool OnMove( long x, long y );
  virtual bool OnDrop( long x, long y );
  virtual bool OnData( long x, long y );

  virtual bool OnDropData( long x, long y, void *data, size_t size ) = 0;

  void SetId( const wxString& id ) { m_id = id; }
  wxString GetId() { return m_id; }

private:

    wxString   m_id;
};
*/

//----------------------------------------------------------------------------
// A drop target which accepts files (dragged from File Manager or Explorer)
//----------------------------------------------------------------------------

class wxFileDropTarget: public wxDropTarget
{
public:

  wxFileDropTarget() {}

  virtual bool OnMove( long x, long y );
  virtual bool OnDrop( long x, long y );
  virtual bool OnData( long x, long y );

  virtual bool OnDropFiles( long x, long y, size_t nFiles, const wxChar * const aszFiles[] ) = 0;
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

  wxDropSource( wxDataObject& data, wxWindow *win, const wxIcon &go = wxNullIcon, const wxIcon &stop = wxNullIcon );
  ~wxDropSource();

  void SetData( wxDataObject& data );

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
    wxDataObject  *m_data;

    wxCursor      m_defaultCursor;
    wxCursor      m_goaheadCursor;

    wxIcon        m_goIcon;
    wxIcon        m_stopIcon;

    bool          m_waiting;
};

#endif

   // wxUSE_DRAG_AND_DROP

#endif
       //__GTKDNDH__

