///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.h
// Purpose:     declaration of the wxDropTarget class
// Author:      Robert Roebling
// RCS-ID:      
// Copyright:   (c) 1998 Vadim Zeitlin, Robert Roebling
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////


#ifndef __GTKDNDH__
#define __GTKDNDH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/cursor.h"

//-------------------------------------------------------------------------
// classes
//-------------------------------------------------------------------------

class wxWindow;

class wxDropTarget;
class wxTextDropTarget;
class wxDragSource;
class wxTextDragSource;

//-------------------------------------------------------------------------
// wxDropTarget
//-------------------------------------------------------------------------

class wxDropTarget: wxObject
{
  public:

    wxDropTarget();
    ~wxDropTarget();
    virtual void OnEnter() { }
    virtual void OnLeave() { }
    virtual bool OnDrop( long x, long y, const void *pData ) = 0;

  public:

    void Drop( GdkEvent *event, int x, int y );
    virtual void RegisterWidget( GtkWidget *widget ) = 0;
    void UnregisterWidget( GtkWidget *widget );
};

//-------------------------------------------------------------------------
// wxTextDropTarget
//-------------------------------------------------------------------------

class wxTextDropTarget: public wxDropTarget
{
  public:

    wxTextDropTarget() {};
    virtual bool OnDrop( long x, long y, const void *pData );
    virtual bool OnDropText( long x, long y, const char *psz );
    virtual void RegisterWidget( GtkWidget *widget );
};

//-------------------------------------------------------------------------
// wxDragSource
//-------------------------------------------------------------------------

class wxDragSource: public wxObject
{
  public:

    wxDragSource( wxWindow *win );
    ~wxDragSource(void);
    void SetData( char *data, long size );
    void Start( int x, int y );

  public:

    void ConnectWindow(void);
    void UnconnectWindow(void);
    virtual void RegisterWindow(void) = 0;
    void UnregisterWindow(void);
  
    GtkWidget   *m_widget;
    wxWindow    *m_window;
    char        *m_data;
    long         m_size;
    wxCursor     m_defaultCursor;
    wxCursor     m_goaheadCursor;
};

//-------------------------------------------------------------------------
// wxTextDragSource
//-------------------------------------------------------------------------

class wxTextDragSource: public wxDragSource
{
  public:

    wxTextDragSource( wxWindow *win ) : wxDragSource(win) {};
    void SetTextData( const wxString &text );
    void RegisterWindow(void);
    
  private:
  
    wxString m_tmp;
};

#endif  
       //__GTKDNDH__

