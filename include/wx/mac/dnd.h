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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "dnd.h"
#endif

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
#define wxDROP_ICON(name)   wxICON(name)

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
    /* constructor. set data later with SetData() */
    wxDropSource( wxWindow *win = (wxWindow *)NULL,
                  const wxIcon &copy = wxNullIcon,
                  const wxIcon &move = wxNullIcon,
                  const wxIcon &none = wxNullIcon);

    /* constructor for setting one data object */
    wxDropSource( wxDataObject& data,
                  wxWindow *win,
                  const wxIcon &copy = wxNullIcon,
                  const wxIcon &move = wxNullIcon,
                  const wxIcon &none = wxNullIcon);


    ~wxDropSource();

    /* start drag action */
    virtual wxDragResult DoDragDrop(int flags = wxDrag_CopyOnly);
    
    wxWindow*     GetWindow() { return m_window ; }
    void SetCurrentDrag( void* drag ) { m_currentDrag = drag ; }
    void* GetCurrentDrag() { return m_currentDrag ; }
  protected :
    wxWindow        *m_window;
    void* m_currentDrag ;
};

#endif
      // D&D

#endif  
       //_WX_DND_H_

