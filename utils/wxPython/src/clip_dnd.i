/////////////////////////////////////////////////////////////////////////////
// Name:        clip_dnd.i
// Purpose:     SWIG definitions for the Clipboard and Drag-n-drop classes
//
// Author:      Robin Dunn
//
// Created:     31-October-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1999 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module clip_dnd

%{
#include "helpers.h"
#include <wx/dataobj.h>
#include <wx/clipbrd.h>
#include <wx/dnd.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import gdi.i


%pragma(python) code = "import wx"

//----------------------------------------------------------------------


enum wxDataFormatId
{
    wxDF_INVALID =          0,
    wxDF_TEXT =             1,  /* CF_TEXT */
    wxDF_BITMAP =           2,  /* CF_BITMAP */
    wxDF_METAFILE =         3,  /* CF_METAFILEPICT */
    wxDF_SYLK =             4,
    wxDF_DIF =              5,
    wxDF_TIFF =             6,
    wxDF_OEMTEXT =          7,  /* CF_OEMTEXT */
    wxDF_DIB =              8,  /* CF_DIB */
    wxDF_PALETTE =          9,
    wxDF_PENDATA =          10,
    wxDF_RIFF =             11,
    wxDF_WAVE =             12,
    wxDF_UNICODETEXT =      13,
    wxDF_ENHMETAFILE =      14,
    wxDF_FILENAME =         15, /* CF_HDROP */
    wxDF_LOCALE =           16,
    wxDF_PRIVATE =          20,
    wxDF_MAX
};

//----------------------------------------------------------------------

class wxDataFormat {
public:
    wxDataFormat( wxDataFormatId type );
    ~wxDataFormat();

    void SetType(wxDataFormatId format);
    wxDataFormatId GetType() const;

    wxString GetId() const;
    void SetId(const char *format);
};

%new wxDataFormat* wxCustomDataFormat(const wxString &id);
%{  // An alternate constructor...
    wxDataFormat* wxCustomDataFormat(const wxString &id) {
        return new wxDataFormat(id);
    }
%}


%{
wxDataFormat wxPyFormatInvalid;
%}
%readonly
%name(wxFormatInvalid) wxDataFormat wxPyFormatInvalid;
%readwrite


//----------------------------------------------------------------------



class wxDataObject {      //  An abstract base class
public:
    enum Direction {
        Get  = 0x01,    // format is supported by GetDataHere()
        Set  = 0x02,    // format is supported by SetData()
        Both = 0x03     // format is supported by both (unused currently)
    };

    ~wxDataObject();

    wxDataFormat GetPreferredFormat(Direction dir = wxDataObject::Get);
    size_t GetFormatCount(Direction dir = wxDataObject::Get);
    void GetAllFormats(wxDataFormat *formats,
                       Direction dir = wxDataObject::Get);
    size_t GetDataSize(const wxDataFormat& format);
    bool GetDataHere(const wxDataFormat& format, void *buf);
    bool SetData(const wxDataFormat& format,
                 size_t len, const void * buf);
    bool IsSupportedFormat(const wxDataFormat& format);
};

//----------------------------------------------------------------------

class wxDataObjectSimple : public wxDataObject {
public:
    wxDataObjectSimple(const wxDataFormat& format = wxPyFormatInvalid);

    const wxDataFormat& GetFormat();
    void SetFormat(const wxDataFormat& format);

};


%{  // Create a new class for wxPython to use
class wxPyDataObjectSimple : public wxDataObjectSimple {
public:
    wxPyDataObjectSimple(const wxDataFormat& format = wxPyFormatInvalid)
        : wxDataObjectSimple(format) {}

    DEC_PYCALLBACK_SIZET_(GetDataSize);
    bool GetDataHere(void *buf);
    bool SetData(size_t len, const void *buf);
    PYPRIVATE;
};

IMP_PYCALLBACK_SIZET_(wxPyDataObjectSimple, wxDataObjectSimple, GetDataSize);

bool wxPyDataObjectSimple::GetDataHere(void *buf) {
    // We need to get the data for this object and write it to buf.  I think
    // the best way to do this for wxPython is to have the Python method
    // return either a string or None and then act appropriately with the
    // C++ version.

    bool rval = FALSE;
    bool doSave = wxPyRestoreThread();
    if (m_myInst.findCallback("GetDataHere")) {
        PyObject* ro;
        ro = m_myInst.callCallbackObj(Py_BuildValue("()"));
        if (ro) {
            rval = (ro != Py_None && PyString_Check(ro));
            if (rval)
                memcpy(buf, PyString_AsString(ro), PyString_Size(ro));
            Py_DECREF(ro);
        }
    }
    wxPySaveThread(doSave);
    return rval;
}

bool wxPyDataObjectSimple::SetData(size_t len, const void *buf) {
    // For this one we simply need to make a string from buf and len
    // and send it to the Python method.
    bool rval = FALSE;
    bool doSave = wxPyRestoreThread();
    if (m_myInst.findCallback("SetData")) {
        PyObject* data = PyString_FromStringAndSize((char*)buf, len);
        rval = m_myInst.callCallback(Py_BuildValue("(O)", data));
        Py_DECREF(data);
    }
    wxPySaveThread(doSave);
    return rval;
}
%}



// Now define it for SWIG
class wxPyDataObjectSimple : public wxDataObjectSimple {
public:
    wxPyDataObjectSimple(const wxDataFormat& format = wxPyFormatInvalid);
    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"
};

//----------------------------------------------------------------------

class wxDataObjectComposite : public wxDataObject {
public:
    wxDataObjectComposite();

    void Add(wxDataObjectSimple *dataObject, int preferred = FALSE);
    %pragma(python) addtomethod = "Add:_args[0].thisown = 0"

};


//----------------------------------------------------------------------

class wxTextDataObject : public wxDataObjectSimple {
public:
    wxTextDataObject(const wxString& text = wxEmptyString);

    size_t GetTextLength();
    wxString GetText();
    void SetText(const wxString& text);
};



%{  // Create a new class for wxPython to use
class wxPyTextDataObject : public wxTextDataObject {
public:
    wxPyTextDataObject(const wxString& text = wxEmptyString)
        : wxTextDataObject(text) {}

    DEC_PYCALLBACK_SIZET_(GetTextLength);
    DEC_PYCALLBACK_STRING_(GetText);
    DEC_PYCALLBACK__STRING(SetText);
    PYPRIVATE;
};

IMP_PYCALLBACK_SIZET_(wxPyTextDataObject, wxTextDataObject, GetTextLength);
IMP_PYCALLBACK_STRING_(wxPyTextDataObject, wxTextDataObject, GetText);
IMP_PYCALLBACK__STRING(wxPyTextDataObject, wxTextDataObject, SetText);

%}


// Now define it for SWIG
class wxPyTextDataObject : public wxTextDataObject {
public:
    wxPyTextDataObject(const wxString& text = wxEmptyString);
    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"
};

//----------------------------------------------------------------------

class wxBitmapDataObject : public wxDataObjectSimple {
public:
    wxBitmapDataObject(const wxBitmap& bitmap = wxNullBitmap);

    wxBitmap GetBitmap();
    void SetBitmap(const wxBitmap& bitmap);
};



%{  // Create a new class for wxPython to use
class wxPyBitmapDataObject : public wxBitmapDataObject {
public:
    wxPyBitmapDataObject(const wxBitmap& bitmap = wxNullBitmap)
        : wxBitmapDataObject(bitmap) {}

    wxBitmap GetBitmap();
    void SetBitmap(const wxBitmap& bitmap);
    PYPRIVATE;
};

wxBitmap wxPyBitmapDataObject::GetBitmap() {
    wxBitmap* rval = &wxNullBitmap;
    bool doSave = wxPyRestoreThread();
    if (m_myInst.findCallback("GetBitmap")) {
        PyObject* ro;
        wxBitmap* ptr;
        ro = m_myInst.callCallbackObj(Py_BuildValue("()"));
        if (ro) {
            if (!SWIG_GetPtrObj(ro, (void **)&ptr, "_wxBitmap_p"))
                rval = ptr;
            Py_DECREF(ro);
        }
    }
    wxPySaveThread(doSave);
    return *rval;
}

void wxPyBitmapDataObject::SetBitmap(const wxBitmap& bitmap) {
    bool doSave = wxPyRestoreThread();
    if (m_myInst.findCallback("SetBitmap")) {
        m_myInst.callCallback(Py_BuildValue("(O)",
                              wxPyConstructObject((void*)&bitmap, "wxBitmap")));
    }
    wxPySaveThread(doSave);
}
%}



// Now define it for SWIG
class wxPyBitmapDataObject : public wxBitmapDataObject {
public:
    wxPyBitmapDataObject(const wxBitmap& bitmap = wxNullBitmap);
    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"
};


//----------------------------------------------------------------------

class wxFileDataObject : public wxDataObjectSimple
{
public:
    wxFileDataObject();

    //const wxArrayString& GetFilenames();
    %addmethods {
        PyObject* GetFilenames() {
            const wxArrayString& strings = self->GetFilenames();
            PyObject* list = PyList_New(0);
            for (size_t x=0; x<strings.GetCount(); x++)
                PyList_Append(list, PyString_FromString(strings[x]));
            return list;
        }
    }

    //void AddFile(const wxString &filename);
};


//----------------------------------------------------------------------

class wxCustomDataObject : public wxDataObjectSimple {
public:
    wxCustomDataObject(const wxDataFormat& format = wxPyFormatInvalid);

    //void TakeData(size_t size, void *data);
    //bool SetData(size_t size, const void *buf);
    %addmethods {
        void TakeData(PyObject* data) {
            if (PyString_Check(data)) {
                self->SetData(PyString_Size(data), PyString_AsString(data));
            }
        }
        bool SetData(PyObject* data) {
            if (PyString_Check(data)) {
                return self->SetData(PyString_Size(data), PyString_AsString(data));
            }
            return FALSE;
        }
    }

    size_t GetSize();
    //void *GetData();
    %addmethods {
        PyObject* GetData() {
            return PyString_FromStringAndSize((char*)self->GetData(), self->GetSize());
        }
    }


};



//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

class wxClipboard {
public:
    wxClipboard();

    bool Open();
    void Close();
    bool IsOpened() const;

    bool AddData( wxDataObject *data );
    %pragma(python) addtomethod = "AddData:_args[0].thisown = 0"
    bool SetData( wxDataObject *data );
    %pragma(python) addtomethod = "SetData:_args[0].thisown = 0"

    bool IsSupported( const wxDataFormat& format );
    bool GetData( wxDataObject& data );
    void Clear();
    bool Flush();
    void UsePrimarySelection( int primary = FALSE );
};

%{
    // See below in the init function...
    wxClipboard* wxPyTheClipboard;
%}
%readonly
%name(wxTheClipboard) wxClipboard* wxPyTheClipboard;
%readwrite

//----------------------------------------------------------------------
//----------------------------------------------------------------------

enum wxDragResult
{
    wxDragError,    // error prevented the d&d operation from completing
    wxDragNone,     // drag target didn't accept the data
    wxDragCopy,     // the data was successfully copied
    wxDragMove,     // the data was successfully moved (MSW only)
    wxDragCancel    // the operation was cancelled by user (not an error)
};

bool wxIsDragResultOk(wxDragResult res);

//----------------------------------------------------------------------
%{
class wxPyDropSource : public wxDropSource {
public:
#ifdef __WXMSW__
    wxPyDropSource(wxWindow *win = NULL,
                   const wxCursor &cursorCopy = wxNullCursor,
                   const wxCursor &cursorMove = wxNullCursor,
                   const wxCursor &cursorStop = wxNullCursor)
        : wxDropSource(win, cursorCopy, cursorMove, cursorStop) {}
#else
    wxPyDropSource(wxWindow *win = NULL,
                   const wxIcon &go = wxNullIcon)
        : wxDropSource(win, go) {}
#endif
    ~wxPyDropSource() { }

    DEC_PYCALLBACK_BOOL_DR(GiveFeedback);
    PYPRIVATE;
};

IMP_PYCALLBACK_BOOL_DR(wxPyDropSource, wxDropSource, GiveFeedback);

%}


%name(wxDropSource) class wxPyDropSource {
public:
#ifdef __WXMSW__
    wxPyDropSource(wxWindow *win = NULL,
                 const wxCursor &cursorCopy = wxNullCursor,
                 const wxCursor &cursorMove = wxNullCursor,
                 const wxCursor &cursorStop = wxNullCursor);
#else
    wxPyDropSource(wxWindow *win = NULL,
                   const wxIcon &go = wxNullIcon);
#endif

    void _setSelf(PyObject* self, int incref);
    %pragma(python) addtomethod = "__init__:self._setSelf(self, 0)"
    ~wxPyDropSource();

    void SetData(wxDataObject& data);
    wxDataObject *GetDataObject();
    void SetCursor(wxDragResult res, const wxCursor& cursor);
    wxDragResult DoDragDrop(int bAllowMove = FALSE);

    bool base_GiveFeedback(wxDragResult effect);
};

//----------------------------------------------------------------------

// Just a place holder for the type system.  The real base class for
// wxPython is wxPyDropTarget
class wxDropTarget {
public:
};


%{
class wxPyDropTarget : public wxDropTarget {
public:
    wxPyDropTarget(wxDataObject *dataObject = NULL)
        : wxDropTarget(dataObject) {}

//      DEC_PYCALLBACK_SIZET_(GetFormatCount);
//      DEC_PYCALLBACK_DATAFMT_SIZET(GetFormat);

    DEC_PYCALLBACK__(OnLeave);
    DEC_PYCALLBACK_DR_2WXCDR(OnEnter);
    DEC_PYCALLBACK_DR_2WXCDR(OnDragOver);
    DEC_PYCALLBACK_DR_2WXCDR_pure(OnData);
    DEC_PYCALLBACK_BOOL_INTINT(OnDrop);

    PYPRIVATE;
};

//  IMP_PYCALLBACK_SIZET_(wxPyDropTarget, wxDropTarget, GetFormatCount);
//  IMP__PYCALLBACK_DATAFMT_SIZET(wxPyDropTarget, wxDropTarget, GetFormat);

IMP_PYCALLBACK__(wxPyDropTarget, wxDropTarget, OnLeave);
IMP_PYCALLBACK_DR_2WXCDR(wxPyDropTarget, wxDropTarget, OnEnter);
IMP_PYCALLBACK_DR_2WXCDR(wxPyDropTarget, wxDropTarget, OnDragOver);
IMP_PYCALLBACK_DR_2WXCDR_pure(wxPyDropTarget, wxDropTarget, OnData);
IMP_PYCALLBACK_BOOL_INTINT(wxPyDropTarget, wxDropTarget, OnDrop);

%}


class wxPyDropTarget : public wxDropTarget {
public:
    wxPyDropTarget(wxDataObject *dataObject = NULL);
    %pragma(python) addtomethod = "__init__:if _args:_args[0].thisown = 0"
    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

    ~wxPyDropTarget();

    wxDataObject *GetDataObject();
    void SetDataObject(wxDataObject *dataObject);
    %pragma(python) addtomethod = "SetDataObject:if _args:_args[0].thisown = 0"

//      size_t base_GetFormatCount();
//      wxDataFormat base_GetFormat(size_t n);

    wxDragResult base_OnEnter(wxCoord x, wxCoord y, wxDragResult def);
    wxDragResult base_OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    void base_OnLeave();
    bool base_OnDrop(wxCoord x, wxCoord y);
    //wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def) = 0;

    // **** not sure about this one
    bool GetData();

};


//----------------------------------------------------------------------

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

%name(wxTextDropTarget) class wxPyTextDropTarget : public wxPyDropTarget {
public:
    wxPyTextDropTarget();
    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

    //bool OnDropText(wxCoord x, wxCoord y, const wxString& text) = 0;
    wxDragResult base_OnEnter(wxCoord x, wxCoord y, wxDragResult def);
    wxDragResult base_OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    void base_OnLeave();
    bool base_OnDrop(wxCoord x, wxCoord y);
    wxDragResult base_OnData(wxCoord x, wxCoord y, wxDragResult def);
};

//----------------------------------------------------------------------
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
    bool rval = FALSE;
    bool doSave = wxPyRestoreThread();
    PyObject* list = PyList_New(0);
    for (size_t i=0; i<filenames.GetCount(); i++) {
        PyObject* str = PyString_FromString(filenames[i].c_str());
        PyList_Append(list, str);
    }
    if (m_myInst.findCallback("OnDropFiles"))
        rval = m_myInst.callCallback(Py_BuildValue("(iiO)",x,y,list));
    Py_DECREF(list);
    wxPySaveThread(doSave);
    return rval;
}



IMP_PYCALLBACK__(wxPyFileDropTarget, wxFileDropTarget, OnLeave);
IMP_PYCALLBACK_DR_2WXCDR(wxPyFileDropTarget, wxFileDropTarget, OnEnter);
IMP_PYCALLBACK_DR_2WXCDR(wxPyFileDropTarget, wxFileDropTarget, OnDragOver);
IMP_PYCALLBACK_DR_2WXCDR(wxPyFileDropTarget, wxFileDropTarget, OnData);
IMP_PYCALLBACK_BOOL_INTINT(wxPyFileDropTarget, wxFileDropTarget, OnDrop);

%}


%name(wxFileDropTarget) class wxPyFileDropTarget : public wxPyDropTarget
{
public:
    wxPyFileDropTarget();
    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

//    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) = 0;
    wxDragResult base_OnEnter(wxCoord x, wxCoord y, wxDragResult def);
    wxDragResult base_OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    void base_OnLeave();
    bool base_OnDrop(wxCoord x, wxCoord y);
    wxDragResult base_OnData(wxCoord x, wxCoord y, wxDragResult def);
};

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

%init %{

    wxPyTheClipboard = wxTheClipboard;

%}

//----------------------------------------------------------------------

