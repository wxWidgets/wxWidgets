///////////////////////////////////////////////////////////////////////////////
// Name:        wx/dnd.h
// Purpose:     Drag and drop classes declarations
// Author:      Vadim Zeitlin, Robert Roebling
// Modified by:
// Created:     26.05.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows Team
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DND_H_BASE_
#define _WX_DND_H_BASE_

#include "wx/defs.h"

#if wxUSE_DRAG_AND_DROP

#include "wx/dataobj.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// result of wxDropSource::DoDragDrop() call
enum wxDragResult
{
    wxDragError,    // error prevented the d&d operation from completing
    wxDragNone,     // drag target didn't accept the data
    wxDragCopy,     // the data was successfully copied
    wxDragMove,     // the data was successfully moved (MSW only)
    wxDragCancel    // the operation was cancelled by user (not an error)
};

// ----------------------------------------------------------------------------
// wxDropSource is the object you need to create (and call DoDragDrop on it)
// to initiate a drag-and-drop operation
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDropSourceBase
{
public:
    wxDropSourceBase() { m_data = (wxDataObject *)NULL; }
    virtual ~wxDropSourceBase() { }

    // set the data which is transfered by drag and drop
    void SetData(wxDataObject& data) 
      { m_data = &data; }
      
    wxDataObject *GetDataObject()
      { return m_data; }

    // start drag action, see enum wxDragResult for return value description
    //
    // if bAllowMove is TRUE, data can be moved, if not - only copied
    virtual wxDragResult DoDragDrop(bool bAllowMove = FALSE) = 0;

    // override to give feedback depending on the current operation result
    // "effect"
    virtual bool GiveFeedback( wxDragResult WXUNUSED(effect),
                               bool WXUNUSED(bScrolling) )
    {
        return TRUE;
    }

protected:
    wxDataObject *m_data;
};

// ----------------------------------------------------------------------------
// wxDropTarget should be associated with a window if it wants to be able to
// receive data via drag and drop.
//
// To use this class, you should derive from wxDropTarget and implement
// OnData() pure virtual method. You may also wish to override OnDrop() if you
// want to accept the data only inside some region of the window (this may
// avoid having to copy the data to this application which happens only when
// OnData() is called)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDropTargetBase
{
public:
    // ctor takes a pointer to heap-allocated wxDataObject which will be owned
    // by wxDropTarget and deleted by it automatically. If you don't give it
    // here, you can use SetDataObject() later.
    wxDropTargetBase(wxDataObject *dataObject = (wxDataObject*)NULL)
        { m_dataObject = dataObject; }
    // dtor deletes our data object
    virtual ~wxDropTargetBase()
        { delete m_dataObject; }

    // get/set the associated wxDataObject
    wxDataObject *GetDataObject() const
        { return m_dataObject; }
    void SetDataObject(wxDataObject *dataObject)
        { if (m_dataObject) delete m_dataObject; 
	  m_dataObject = dataObject; }

    // called when mouse enters/leaves the window: might be used to give
    // some visual feedback to the user
    virtual void OnLeave() { }
    
    // this function is called when data enters over position (x, y) - if it
    // returns TRUE, the dragging icon can indicate that the window would
    // accept a drop here
    virtual bool OnEnter(wxCoord x, wxCoord y) = 0;

    // this function is called when data is move over position (x, y) - if it
    // returns TRUE, the dragging icon can indicate that the window would
    // accept a drop here
    virtual bool OnMove(wxCoord x, wxCoord y) = 0;
    
    // this function is called when data is dropped at position (x, y) - if it
    // returns TRUE, OnData() will be called immediately afterwards which will
    // allow to retrieve the data dropped.
    virtual bool OnDrop(wxCoord x, wxCoord y) = 0;

    // called after OnDrop() returns TRUE: you will usually just call
    // GetData() from here and, probably, also refresh something to update the
    // new data
    virtual bool OnData(wxCoord x, wxCoord y) = 0;

    // may be called *only* from inside OnData() and will fill m_dataObject
    // with the data from the drop source if it returns TRUE
    virtual bool GetData() = 0;

protected:
    wxDataObject *m_dataObject;
};

// ----------------------------------------------------------------------------
// the platform-specific headers also define standard wxDropTarget
// implementations wxTextDropTarget and wxFileDropTarget
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// include platform dependent class declarations
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/ole/dropsrc.h"
    #include "wx/msw/ole/droptgt.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/dnd.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/dnd.h"
#elif defined(__WXQT__)
    #include "wx/qt/dnd.h"
#elif defined(__WXMAC__)
    #include "wx/mac/dnd.h"
#elif defined(__WXPM__)
    #include "wx/os2/dnd.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/dnd.h"
#endif

#endif // wxUSE_DRAG_AND_DROP

#endif // _WX_DND_H_BASE_
