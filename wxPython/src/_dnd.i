/////////////////////////////////////////////////////////////////////////////
// Name:        _dnd.i
// Purpose:     SWIG definitions for the Drag-n-drop classes
//
// Author:      Robin Dunn
//
// Created:     31-October-1999
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
#ifndef __WXX11__

%newgroup

// flags for wxDropSource::DoDragDrop()
//
// NB: wxDrag_CopyOnly must be 0 (== False) and wxDrag_AllowMove must be 1
//     (== True) for compatibility with the old DoDragDrop(bool) method!
enum
{
    wxDrag_CopyOnly    = 0, // allow only copying
    wxDrag_AllowMove   = 1, // allow moving (copying is always allowed)
    wxDrag_DefaultMove = 3  // the default operation is move, not copy
};

// result of wxDropSource::DoDragDrop() call
enum wxDragResult
{
    wxDragError,    // error prevented the d&d operation from completing
    wxDragNone,     // drag target didn't accept the data
    wxDragCopy,     // the data was successfully copied
    wxDragMove,     // the data was successfully moved (MSW only)
    wxDragLink,     // operation is a drag-link
    wxDragCancel    // the operation was cancelled by user (not an error)
};

bool wxIsDragResultOk(wxDragResult res);

//---------------------------------------------------------------------------


// wxDropSource is the object you need to create (and call DoDragDrop on it)
// to initiate a drag-and-drop operation



%{
IMP_PYCALLBACK_BOOL_DR(wxPyDropSource, wxDropSource, GiveFeedback);
%}


%rename(DropSource) wxPyDropSource;
class wxPyDropSource {
public:
    %pythonAppend wxPyDropSource "self._setCallbackInfo(self, DropSource, 0)"
#ifndef __WXGTK__
     wxPyDropSource(wxWindow *win,
                    const wxCursor &copy = wxNullCursor,
                    const wxCursor &move = wxNullCursor,
                    const wxCursor &none = wxNullCursor);
#else
    wxPyDropSource(wxWindow *win,
                   const wxIcon& copy = wxNullIcon,
                   const wxIcon& move = wxNullIcon,
                   const wxIcon& none = wxNullIcon);
#endif

    void _setCallbackInfo(PyObject* self, PyObject* _class, int incref);
    ~wxPyDropSource();

    // set the data which is transfered by drag and drop
    void SetData(wxDataObject& data);

    wxDataObject *GetDataObject();

    // set the icon corresponding to given drag result
    void SetCursor(wxDragResult res, const wxCursor& cursor);

    wxDragResult DoDragDrop(int flags = wxDrag_CopyOnly);

    bool base_GiveFeedback(wxDragResult effect);
};


//---------------------------------------------------------------------------

// wxDropTarget should be associated with a window if it wants to be able to
// receive data via drag and drop.
//
// To use this class, you should derive from wxDropTarget and implement
// OnData() pure virtual method. You may also wish to override OnDrop() if you
// want to accept the data only inside some region of the window (this may
// avoid having to copy the data to this application which happens only when
// OnData() is called)


// Just a place holder for the type system.  The real base class for
// wxPython is wxPyDropTarget
// class wxDropTarget {
// public:
// };


%{
IMP_PYCALLBACK__(wxPyDropTarget, wxDropTarget, OnLeave);
IMP_PYCALLBACK_DR_2WXCDR(wxPyDropTarget, wxDropTarget, OnEnter);
IMP_PYCALLBACK_DR_2WXCDR(wxPyDropTarget, wxDropTarget, OnDragOver);
IMP_PYCALLBACK_DR_2WXCDR_pure(wxPyDropTarget, wxDropTarget, OnData);
IMP_PYCALLBACK_BOOL_INTINT(wxPyDropTarget, wxDropTarget, OnDrop);
%}


%rename(DropTarget) wxPyDropTarget;
class wxPyDropTarget // : public wxDropTarget
{
public:
    %pythonAppend wxPyDropTarget
       "self._setCallbackInfo(self, DropTarget)"
    %apply SWIGTYPE *DISOWN { wxDataObject *dataObject };

    wxPyDropTarget(wxDataObject *dataObject = NULL);
    void _setCallbackInfo(PyObject* self, PyObject* _class);

    ~wxPyDropTarget();

    // get/set the associated wxDataObject
    wxDataObject *GetDataObject();
    void SetDataObject(wxDataObject *dataObject);

    %clear wxDataObject *dataObject;

    wxDragResult base_OnEnter(wxCoord x, wxCoord y, wxDragResult def);
    wxDragResult base_OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    void base_OnLeave();
    bool base_OnDrop(wxCoord x, wxCoord y);

    // may be called *only* from inside OnData() and will fill m_dataObject
    // with the data from the drop source if it returns True
    bool GetData();

};


%pythoncode { PyDropTarget = DropTarget }

//---------------------------------------------------------------------------

// A simple wxDropTarget derived class for text data: you only need to
// override OnDropText() to get something working


%{
class wxPyTextDropTarget : public wxTextDropTarget {
public:
    wxPyTextDropTarget() {}

    DEC_PYCALLBACK_BOOL_INTINTSTR_pure(OnDropText);

    DEC_PYCALLBACK__(OnLeave);
    DEC_PYCALLBACK_DR_2WXCDR(OnEnter);
    DEC_PYCALLBACK_DR_2WXCDR(OnDragOver);
    DEC_PYCALLBACK_DR_2WXCDR(OnData);
    DEC_PYCALLBACK_BOOL_INTINT(OnDrop);

    PYPRIVATE;
};

IMP_PYCALLBACK_BOOL_INTINTSTR_pure(wxPyTextDropTarget, wxTextDropTarget, OnDropText);
IMP_PYCALLBACK__(wxPyTextDropTarget, wxTextDropTarget, OnLeave);
IMP_PYCALLBACK_DR_2WXCDR(wxPyTextDropTarget, wxTextDropTarget, OnEnter);
IMP_PYCALLBACK_DR_2WXCDR(wxPyTextDropTarget, wxTextDropTarget, OnDragOver);
IMP_PYCALLBACK_DR_2WXCDR(wxPyTextDropTarget, wxTextDropTarget, OnData);
IMP_PYCALLBACK_BOOL_INTINT(wxPyTextDropTarget, wxTextDropTarget, OnDrop);

%}

%rename(TextDropTarget) wxPyTextDropTarget;
class wxPyTextDropTarget : public wxPyDropTarget {
public:
    %pythonAppend wxPyTextDropTarget   "self._setCallbackInfo(self, TextDropTarget)"

    wxPyTextDropTarget();
    void _setCallbackInfo(PyObject* self, PyObject* _class);

    //bool OnDropText(wxCoord x, wxCoord y, const wxString& text) = 0;
    wxDragResult base_OnEnter(wxCoord x, wxCoord y, wxDragResult def);
    wxDragResult base_OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    void base_OnLeave();
    bool base_OnDrop(wxCoord x, wxCoord y);
    wxDragResult base_OnData(wxCoord x, wxCoord y, wxDragResult def);
};

//---------------------------------------------------------------------------

// A drop target which accepts files (dragged from File Manager or Explorer)


%{
class wxPyFileDropTarget : public wxFileDropTarget {
public:
    wxPyFileDropTarget() {}

    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);

    DEC_PYCALLBACK__(OnLeave);
    DEC_PYCALLBACK_DR_2WXCDR(OnEnter);
    DEC_PYCALLBACK_DR_2WXCDR(OnDragOver);
    DEC_PYCALLBACK_DR_2WXCDR(OnData);
    DEC_PYCALLBACK_BOOL_INTINT(OnDrop);

    PYPRIVATE;
};

bool wxPyFileDropTarget::OnDropFiles(wxCoord x, wxCoord y,
                                     const wxArrayString& filenames) {
    bool rval = false;
    bool blocked = wxPyBeginBlockThreads();
    if (wxPyCBH_findCallback(m_myInst, "OnDropFiles")) {
        PyObject* list = wxArrayString2PyList_helper(filenames);
        rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iiO)",x,y,list));
        Py_DECREF(list);
    }
    wxPyEndBlockThreads(blocked);
    return rval;
}



IMP_PYCALLBACK__(wxPyFileDropTarget, wxFileDropTarget, OnLeave);
IMP_PYCALLBACK_DR_2WXCDR(wxPyFileDropTarget, wxFileDropTarget, OnEnter);
IMP_PYCALLBACK_DR_2WXCDR(wxPyFileDropTarget, wxFileDropTarget, OnDragOver);
IMP_PYCALLBACK_DR_2WXCDR(wxPyFileDropTarget, wxFileDropTarget, OnData);
IMP_PYCALLBACK_BOOL_INTINT(wxPyFileDropTarget, wxFileDropTarget, OnDrop);

%}


%rename(FileDropTarget) wxPyFileDropTarget;
class wxPyFileDropTarget : public wxPyDropTarget
{
public:
    %pythonAppend wxPyFileDropTarget   "self._setCallbackInfo(self, FileDropTarget)"

    wxPyFileDropTarget();
    void _setCallbackInfo(PyObject* self, PyObject* _class);

//    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) = 0;
    wxDragResult base_OnEnter(wxCoord x, wxCoord y, wxDragResult def);
    wxDragResult base_OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    void base_OnLeave();
    bool base_OnDrop(wxCoord x, wxCoord y);
    wxDragResult base_OnData(wxCoord x, wxCoord y, wxDragResult def);
};


//---------------------------------------------------------------------------
%init %{
    wxPyPtrTypeMap_Add("wxDropSource",     "wxPyDropSource");
    wxPyPtrTypeMap_Add("wxDropTarget",     "wxPyDropTarget");
    wxPyPtrTypeMap_Add("wxTextDropTarget", "wxPyTextDropTarget");
    wxPyPtrTypeMap_Add("wxFileDropTarget", "wxPyFileDropTarget");
%}
//---------------------------------------------------------------------------

#endif
