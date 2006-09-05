///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.h
// Purpose:     Declaration of the wxDropTarget, wxDropSource class etc.
// Author:      Stefan Csomor
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DND_H_
#define _WX_DND_H_

#if wxUSE_DRAG_AND_DROP

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/string.h"
#include "wx/dataobj.h"
#include "wx/cursor.h"

//-------------------------------------------------------------------------
// classes
//-------------------------------------------------------------------------

class WXDLLEXPORT wxWindow;

class WXDLLEXPORT wxDropTarget;
class WXDLLEXPORT wxTextDropTarget;
class WXDLLEXPORT wxFileDropTarget;

class WXDLLEXPORT wxDropSource;

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// this macro may be used instead for wxDropSource ctor arguments: it will use
// the icon 'name' from an XPM file under GTK, but will expand to something
// else under MSW. If you don't use it, you will have to use #ifdef in the
// application code.
#define wxDROP_ICON(X)   wxCursor( (const char**) X##_xpm )

//-------------------------------------------------------------------------
// wxDropTarget
//-------------------------------------------------------------------------

class WXDLLEXPORT wxDropTarget: public wxDropTargetBase
{
  public:

    wxDropTarget(wxDataObject *dataObject = (wxDataObject*) NULL );

    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    virtual bool OnDrop(wxCoord x, wxCoord y);
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);
    virtual bool GetData();
    
    bool CurrentDragHasSupportedFormat() ;
    void SetCurrentDrag( void* drag ) { m_currentDrag = drag ; }
    void* GetCurrentDrag() { return m_currentDrag ; }
  protected :
    void* m_currentDrag ;
};

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

class WXDLLEXPORT wxDropSource: public wxDropSourceBase
{
public:
    // ctors: if you use default ctor you must call SetData() later!
    //
    // NB: the "wxWindow *win" parameter is unused and is here only for wxGTK
    //     compatibility, as well as both icon parameters
    wxDropSource( wxWindow *win = (wxWindow *)NULL,
                 const wxCursor &cursorCopy = wxNullCursor,
                 const wxCursor &cursorMove = wxNullCursor,
                 const wxCursor &cursorStop = wxNullCursor);

    /* constructor for setting one data object */
    wxDropSource( wxDataObject& data,
                  wxWindow *win,
                 const wxCursor &cursorCopy = wxNullCursor,
                 const wxCursor &cursorMove = wxNullCursor,
                 const wxCursor &cursorStop = wxNullCursor);

    virtual ~wxDropSource();

    // do it (call this in response to a mouse button press, for example)
    // params: if bAllowMove is false, data can be only copied
    virtual wxDragResult DoDragDrop(int flags = wxDrag_CopyOnly);

    wxWindow*     GetWindow() { return m_window ; }
    void SetCurrentDrag( void* drag ) { m_currentDrag = drag ; }
    void* GetCurrentDrag() { return m_currentDrag ; }
	bool			MacInstallDefaultCursor(wxDragResult effect) ;
  protected :
  	
    wxWindow        *m_window;
    void* m_currentDrag ;
};

#endif
      // D&D

#endif  
       //_WX_DND_H_

