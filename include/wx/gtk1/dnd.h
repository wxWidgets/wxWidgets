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
    wxDropTarget( wxDataObject *data );
    ~wxDropTarget();

    /* may be overridden to react to events */
    virtual bool OnEnter( int x, int y );
  
    virtual void OnLeave();

    /* may be overridden to reject certain formats or drops
       on certain areas. always returns TRUE by default
       indicating that you'd accept the data from the drag. */
    virtual bool OnMove( int x, int y );

    /* has to be overridden to accept a drop event. call
       IsSupported() to ask which formats are available
       and then call RequestData() to indicate the format
       you request. */
    virtual bool OnDrop( int x, int y );

    /* this gets called once the data has actually arrived.
       it will call GetData() to fill up its wxDataObject */
    virtual bool OnData( int x, int y );

    /* fill data with data from the dragging source */
    bool GetData();

// implementation

    GdkAtom GetMatchingPair();

    void RegisterWidget( GtkWidget *widget );
    void UnregisterWidget( GtkWidget *widget );

    wxDataObject       *m_data;
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

