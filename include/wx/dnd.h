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

inline WXDLLEXPORT bool wxIsDragResultOk(wxDragResult res)
{
    return res == wxDragCopy || res == wxDragMove;
}

// ----------------------------------------------------------------------------
// wxDropSource is the object you need to create (and call DoDragDrop on it)
// to initiate a drag-and-drop operation
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDropSourceBase
{
public:
    wxDropSourceBase(const wxCursor &cursorCopy = wxNullCursor,
                     const wxCursor &cursorMove = wxNullCursor,
                     const wxCursor &cursorStop = wxNullCursor)
        : m_cursorCopy(cursorCopy),
          m_cursorMove(cursorMove),
          m_cursorStop(cursorStop)
        { m_data = (wxDataObject *)NULL; }
    virtual ~wxDropSourceBase() { }

    // set the data which is transfered by drag and drop
    void SetData(wxDataObject& data)
      { m_data = &data; }

    wxDataObject *GetDataObject()
      { return m_data; }

    // set the icon corresponding to given drag result
    void SetCursor(wxDragResult res, const wxCursor& cursor)
    {
        if ( res == wxDragCopy )
            m_cursorCopy = cursor;
        else if ( res == wxDragMove )
            m_cursorMove = cursor;
        else
            m_cursorStop = cursor;
    }

    // start drag action, see enum wxDragResult for return value description
    //
    // if bAllowMove is TRUE, data can be moved, if not - only copied
    virtual wxDragResult DoDragDrop(bool bAllowMove = FALSE) = 0;

    // override to give feedback depending on the current operation result
    // "effect" and return TRUE if you did something, FALSE to let the library
    // give the default feedback
    virtual bool GiveFeedback(wxDragResult WXUNUSED(effect)) { return FALSE; }

protected:
    const wxCursor& GetCursor(wxDragResult res) const
    {
        if ( res == wxDragCopy )
            return m_cursorCopy;
        else if ( res == wxDragMove )
            return m_cursorMove;
        else
            return m_cursorStop;
    }

    wxDataObject *m_data;

    // the cursors to use for feedback
    wxCursor      m_cursorCopy, m_cursorMove, m_cursorStop;
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

    // these functions are called when data is moved over position (x, y) and
    // may return either wxDragCopy, wxDragMove or wxDragNone depending on
    // what would happen if the data were dropped here.
    //
    // the last parameter is what would happen by default and is determined by
    // the platform-specific logic (for example, under Windows it's wxDragCopy
    // if Ctrl key is pressed and wxDragMove otherwise) except that it will
    // always be wxDragNone if the carried data is in an unsupported format.

    // called when the mouse enters the window (only once until OnLeave())
    virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def)
        { return OnDragOver(x, y, def); }

    // called when the mouse moves in the window - shouldn't take long to
    // execute or otherwise mouse movement would be too slow
    virtual wxDragResult OnDragOver(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                                    wxDragResult def)
        { return def; }

    // called when mouse leaves the window: might be used to remove the
    // feedback which was given in OnEnter()
    virtual void OnLeave() { }

    // this function is called when data is dropped at position (x, y) - if it
    // returns TRUE, OnData() will be called immediately afterwards which will
    // allow to retrieve the data dropped.
    virtual bool OnDrop(wxCoord x, wxCoord y) = 0;

    // called after OnDrop() returns TRUE: you will usually just call
    // GetData() from here and, probably, also refresh something to update the
    // new data and, finally, return the code indicating how did the operation
    // complete (returning default value in case of success and wxDragError on
    // failure is usually ok)
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def) = 0;

    // may be called *only* from inside OnData() and will fill m_dataObject
    // with the data from the drop source if it returns TRUE
    virtual bool GetData() = 0;

protected:
    wxDataObject *m_dataObject;
};

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

// ----------------------------------------------------------------------------
// standard wxDropTarget implementations (implemented in common/dobjcmn.cpp)
// ----------------------------------------------------------------------------

// A simple wxDropTarget derived class for text data: you only need to
// override OnDropText() to get something working
class WXDLLEXPORT wxTextDropTarget : public wxDropTarget
{
public:
    wxTextDropTarget();

    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& text) = 0;

    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);
};

// A drop target which accepts files (dragged from File Manager or Explorer)
class WXDLLEXPORT wxFileDropTarget : public wxDropTarget
{
public:
    wxFileDropTarget();

    // parameters are the number of files and the array of file names
    virtual bool OnDropFiles(wxCoord x, wxCoord y,
                             const wxArrayString& filenames) = 0;

    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);
};

#endif // wxUSE_DRAG_AND_DROP

#endif // _WX_DND_H_BASE_
