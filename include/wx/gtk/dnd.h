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

#ifdef wxUSE_DRAG_AND_DROP

#include "wx/object.h"
#include "wx/string.h"
#include "wx/dataobj.h"
#include "wx/cursor.h"

//-------------------------------------------------------------------------
// conditional compilation
//-------------------------------------------------------------------------

#if (GTK_MINOR_VERSION == 1)
#if (GTK_MICRO_VERSION >= 3)
#define NEW_GTK_DND_CODE
#endif
#endif

//-------------------------------------------------------------------------
// classes
//-------------------------------------------------------------------------

class wxWindow;

class wxDropTarget;
class wxTextDropTarget;
class wxFileDropTarget;

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
    virtual bool OnDrop( long x, long y, const void *data, size_t size ) = 0;

    // Override these to indicate what kind of data you support: 
  
    virtual size_t GetFormatCount() const = 0;
    virtual wxDataFormat GetFormat(size_t n) const = 0;
  
  // implementation
  
    void RegisterWidget( GtkWidget *widget );
    void UnregisterWidget( GtkWidget *widget );
};

//-------------------------------------------------------------------------
// wxTextDropTarget
//-------------------------------------------------------------------------

class wxTextDropTarget: public wxDropTarget
{
  public:

    wxTextDropTarget() {};
    virtual bool OnDrop( long x, long y, const void *data, size_t size );
    virtual bool OnDropText( long x, long y, const char *psz );
    
  protected:
  
    virtual size_t GetFormatCount() const;
    virtual wxDataFormat GetFormat(size_t n) const;
};

// ----------------------------------------------------------------------------
// A drop target which accepts files (dragged from File Manager or Explorer)
// ----------------------------------------------------------------------------

class wxFileDropTarget: public wxDropTarget
{
  public:
    
    wxFileDropTarget() {};
    
    virtual bool OnDrop( long x, long y, const void *data, size_t size );
    virtual bool OnDropFiles( long x, long y, 
                              size_t nFiles, const char * const aszFiles[] );

  protected:
  
    virtual size_t GetFormatCount() const;
    virtual wxDataFormat GetFormat(size_t n) const;
};

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

enum wxDragResult
{
  wxDragError,    // error prevented the d&d operation from completing
  wxDragNone,     // drag target didn't accept the data
  wxDragCopy,     // the data was successfully copied
  wxDragMove,     // the data was successfully moved
  wxDragCancel    // the operation was cancelled by user (not an error)
};

class wxDropSource: public wxObject
{
  public:

    wxDropSource( wxWindow *win );
    wxDropSource( wxDataObject &data, wxWindow *win );
    
    ~wxDropSource(void);
    
    void SetData( wxDataObject &data  );
    wxDragResult DoDragDrop( bool bAllowMove = FALSE );
    
    virtual bool GiveFeedback( wxDragResult WXUNUSED(effect), bool WXUNUSED(bScrolling) ) { return TRUE; };
  
  // implementation
      
    void RegisterWindow(void);
    void UnregisterWindow(void);
  
    GtkWidget     *m_widget;
    wxWindow      *m_window;
    wxDragResult   m_retValue;
    wxDataObject  *m_data;
    
    wxCursor      m_defaultCursor;
    wxCursor      m_goaheadCursor;
};

#endif

   // wxUSE_DRAG_AND_DROP

#endif  
       //__GTKDNDH__

