/////////////////////////////////////////////////////////////////////////////
// Name:        _dataobj.i
// Purpose:     SWIG definitions for the Data Object classes
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

%{
#include <wx/dataobj.h>
%}

//---------------------------------------------------------------------------
%newgroup


enum wxDataFormatId
{
    wxDF_INVALID,
    wxDF_TEXT,
    wxDF_BITMAP,
    wxDF_METAFILE,
    wxDF_SYLK,
    wxDF_DIF,
    wxDF_TIFF,
    wxDF_OEMTEXT,
    wxDF_DIB,
    wxDF_PALETTE,
    wxDF_PENDATA,
    wxDF_RIFF,
    wxDF_WAVE,
    wxDF_UNICODETEXT,
    wxDF_ENHMETAFILE,
    wxDF_FILENAME,
    wxDF_LOCALE,
    wxDF_PRIVATE,
    wxDF_HTML,
    wxDF_MAX,
};


class wxDataFormat {
public:
    wxDataFormat( wxDataFormatId type );
    %name(CustomDataFormat) wxDataFormat(const wxString& format);
    
    ~wxDataFormat();    

    %nokwargs operator==;
    %nokwargs operator!=;
    bool operator==(wxDataFormatId format) const;
    bool operator!=(wxDataFormatId format) const;
    bool operator==(const wxDataFormat& format) const;
    bool operator!=(const wxDataFormat& format) const;

    void SetType(wxDataFormatId format);
    wxDataFormatId GetType() const;

    wxString GetId() const;
    void SetId(const wxString& format);
};


%immutable;
const wxDataFormat wxFormatInvalid;
%mutable;


//---------------------------------------------------------------------------


// wxDataObject represents a piece of data which knows which formats it
// supports and knows how to render itself in each of them - GetDataHere(),
// and how to restore data from the buffer (SetData()).
//
// Although this class may be used directly (i.e. custom classes may be
// derived from it), in many cases it might be simpler to use either
// wxDataObjectSimple or wxDataObjectComposite classes.
//
// A data object may be "read only", i.e. support only GetData() functions or
// "read-write", i.e. support both GetData() and SetData() (in principle, it
// might be "write only" too, but this is rare). Moreover, it doesn't have to
// support the same formats in Get() and Set() directions: for example, a data
// object containing JPEG image might accept BMPs in GetData() because JPEG
// image may be easily transformed into BMP but not in SetData(). Accordingly,
// all methods dealing with formats take an additional "direction" argument
// which is either SET or GET and which tells the function if the format needs
// to be supported by SetData() or GetDataHere().
class wxDataObject {      
public:
    enum Direction {
        Get  = 0x01,    // format is supported by GetDataHere()
        Set  = 0x02,    // format is supported by SetData()
        Both = 0x03     // format is supported by both (unused currently)
    };

    // wxDataObject();  // ***  It's an ABC.
    ~wxDataObject();

    // get the best suited format for rendering our data
    virtual wxDataFormat GetPreferredFormat(Direction dir = Get) const;

    // get the number of formats we support
    virtual size_t GetFormatCount(Direction dir = Get) const;

    // returns True if this format is supported
    bool IsSupported(const wxDataFormat& format, Direction dir = Get) const;

    // get the (total) size of data for the given format
    virtual size_t GetDataSize(const wxDataFormat& format) const;


    //-------------------------------------------------------------------
    // TODO:  Fix these three methods to do the right Pythonic things...
    //-------------------------------------------------------------------

    // return all formats in the provided array (of size GetFormatCount())
    virtual void GetAllFormats(wxDataFormat *formats,
                               Direction dir = Get) const;

    // copy raw data (in the specified format) to the provided buffer, return
    // True if data copied successfully, False otherwise
    virtual bool GetDataHere(const wxDataFormat& format, void *buf) const;

    // get data from the buffer of specified length (in the given format),
    // return True if the data was read successfully, False otherwise
    virtual bool SetData(const wxDataFormat& format,
                         size_t len, const void * buf);
};

    
//---------------------------------------------------------------------------



// wxDataObjectSimple is a wxDataObject which only supports one format (in
// both Get and Set directions, but you may return False from GetDataHere() or
// SetData() if one of them is not supported). This is the simplest possible
// wxDataObject implementation.
//
// This is still an "abstract base class" (although it doesn't have any pure
// virtual functions), to use it you should derive from it and implement
// GetDataSize(), GetDataHere() and SetData() functions because the base class
// versions don't do anything - they just return "not implemented".
//
// This class should be used when you provide data in only one format (no
// conversion to/from other formats), either a standard or a custom one.
// Otherwise, you should use wxDataObjectComposite or wxDataObject directly.
class wxDataObjectSimple : public wxDataObject {
public:
    wxDataObjectSimple(const wxDataFormat& format = wxFormatInvalid);

    // get/set the format we support
    const wxDataFormat& GetFormat();
    void SetFormat(const wxDataFormat& format);

};


%{  // Create a new class for wxPython to use
class wxPyDataObjectSimple : public wxDataObjectSimple {
public:
    wxPyDataObjectSimple(const wxDataFormat& format = wxFormatInvalid)
        : wxDataObjectSimple(format) {}

    DEC_PYCALLBACK_SIZET__const(GetDataSize);
    bool GetDataHere(void *buf) const;
    bool SetData(size_t len, const void *buf) const;
    PYPRIVATE;
};

IMP_PYCALLBACK_SIZET__const(wxPyDataObjectSimple, wxDataObjectSimple, GetDataSize);

bool wxPyDataObjectSimple::GetDataHere(void *buf) const {
    // We need to get the data for this object and write it to buf.  I think
    // the best way to do this for wxPython is to have the Python method
    // return either a string or None and then act appropriately with the
    // C++ version.

    bool rval = False;
    wxPyBeginBlockThreads();
    if (wxPyCBH_findCallback(m_myInst, "GetDataHere")) {
        PyObject* ro;
        ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));
        if (ro) {
            rval = (ro != Py_None && PyString_Check(ro));
            if (rval)
                memcpy(buf, PyString_AsString(ro), PyString_Size(ro));
            Py_DECREF(ro);
        }
    }
    wxPyEndBlockThreads();
    return rval;
}

bool wxPyDataObjectSimple::SetData(size_t len, const void *buf) const{
    // For this one we simply need to make a string from buf and len
    // and send it to the Python method.
    bool rval = False;
    wxPyBeginBlockThreads();
    if (wxPyCBH_findCallback(m_myInst, "SetData")) {
        PyObject* data = PyString_FromStringAndSize((char*)buf, len);
        rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", data));
        Py_DECREF(data);
    }
    wxPyEndBlockThreads();
    return rval;
}
%}



// Now define it for SWIG
class wxPyDataObjectSimple : public wxDataObjectSimple {
public:
    %addtofunc wxPyDataObjectSimple   "self._setCallbackInfo(self, PyDataObjectSimple)"

    wxPyDataObjectSimple(const wxDataFormat& format = wxFormatInvalid);
    void _setCallbackInfo(PyObject* self, PyObject* _class);
};


//---------------------------------------------------------------------------


// wxDataObjectComposite is the simplest way to implement wxDataObject
// supporting multiple formats. It contains several wxDataObjectSimple and
// supports all formats supported by any of them.
//
class wxDataObjectComposite : public wxDataObject {
public:
    wxDataObjectComposite();

    %addtofunc Add "args[1].thisown = 0"
    void Add(wxDataObjectSimple *dataObject, int preferred = False);
};

//---------------------------------------------------------------------------

// wxTextDataObject contains text data
class wxTextDataObject : public wxDataObjectSimple {
public:
    wxTextDataObject(const wxString& text = wxPyEmptyString);

    size_t GetTextLength();
    wxString GetText();
    void SetText(const wxString& text);
};



%{  // Create a new class for wxPython to use
class wxPyTextDataObject : public wxTextDataObject {
public:
    wxPyTextDataObject(const wxString& text = wxPyEmptyString)
        : wxTextDataObject(text) {}

    DEC_PYCALLBACK_SIZET__const(GetTextLength);
    DEC_PYCALLBACK_STRING__const(GetText);
    DEC_PYCALLBACK__STRING(SetText);
    PYPRIVATE;
};

IMP_PYCALLBACK_SIZET__const(wxPyTextDataObject, wxTextDataObject, GetTextLength);
IMP_PYCALLBACK_STRING__const(wxPyTextDataObject, wxTextDataObject, GetText);
IMP_PYCALLBACK__STRING(wxPyTextDataObject, wxTextDataObject, SetText);

%}


// Now define it for SWIG
class wxPyTextDataObject : public wxTextDataObject {
public:
    %addtofunc wxPyTextDataObject   "self._setCallbackInfo(self, PyTextDataObject)"

    wxPyTextDataObject(const wxString& text = wxPyEmptyString);
    void _setCallbackInfo(PyObject* self, PyObject* _class);
};

//---------------------------------------------------------------------------

// wxBitmapDataObject contains a bitmap
class wxBitmapDataObject : public wxDataObjectSimple {
public:
    wxBitmapDataObject(const wxBitmap& bitmap = wxNullBitmap);

    wxBitmap GetBitmap() const;
    void SetBitmap(const wxBitmap& bitmap);
};



%{  // Create a new class for wxPython to use
class wxPyBitmapDataObject : public wxBitmapDataObject {
public:
    wxPyBitmapDataObject(const wxBitmap& bitmap = wxNullBitmap)
        : wxBitmapDataObject(bitmap) {}

    wxBitmap GetBitmap() const;
    void SetBitmap(const wxBitmap& bitmap);
    PYPRIVATE;
};

wxBitmap wxPyBitmapDataObject::GetBitmap() const {
    wxBitmap* rval = &wxNullBitmap;
    wxPyBeginBlockThreads();
    if (wxPyCBH_findCallback(m_myInst, "GetBitmap")) {
        PyObject* ro;
        wxBitmap* ptr;
        ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));
        if (ro) {
            if (wxPyConvertSwigPtr(ro, (void **)&ptr, wxT("wxBitmap")))
                rval = ptr;
            Py_DECREF(ro);
        }
    }
    wxPyEndBlockThreads();
    return *rval;
}

void wxPyBitmapDataObject::SetBitmap(const wxBitmap& bitmap) {
    wxPyBeginBlockThreads();
    if (wxPyCBH_findCallback(m_myInst, "SetBitmap")) {
        PyObject* bo = wxPyConstructObject((void*)&bitmap, wxT("wxBitmap"), False);
        wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", bo));
        Py_DECREF(bo);
    }
    wxPyEndBlockThreads();
}
%}



// Now define it for SWIG
class wxPyBitmapDataObject : public wxBitmapDataObject {
public:
    %addtofunc wxPyBitmapDataObject   "self._setCallbackInfo(self, PyBitmapDataObject)"

    wxPyBitmapDataObject(const wxBitmap& bitmap = wxNullBitmap);
    void _setCallbackInfo(PyObject* self, PyObject* _class);
};

//---------------------------------------------------------------------------


// wxFileDataObject contains a list of filenames
class wxFileDataObject : public wxDataObjectSimple
{
public:
    wxFileDataObject();

    const wxArrayString& GetFilenames();
#ifdef __WXMSW__
    void AddFile(const wxString &filename);
#endif
};


//---------------------------------------------------------------------------

// wxCustomDataObject contains arbitrary untyped user data.
// It is understood that this data can be copied bitwise.
class wxCustomDataObject : public wxDataObjectSimple {
public:
    wxCustomDataObject(const wxDataFormat& format = wxFormatInvalid);

    //void TakeData(size_t size, void *data);
    //bool SetData(size_t size, const void *buf);
    %extend {
        void TakeData(PyObject* data) {
            if (PyString_Check(data)) {
                // for Python we just call SetData here since we always need it to make a copy.
                self->SetData(PyString_Size(data), PyString_AsString(data));
            }
            else {
                // raise a TypeError if not a string
                PyErr_SetString(PyExc_TypeError, "String expected.");
            }
        }
        bool SetData(PyObject* data) {
            if (PyString_Check(data)) {
                return self->SetData(PyString_Size(data), PyString_AsString(data));
            }
            else {
                // raise a TypeError if not a string
                PyErr_SetString(PyExc_TypeError, "String expected.");
                return False;
            }
        }
    }

    size_t GetSize();

    //void *GetData();
    %extend {
        PyObject* GetData() {
            return PyString_FromStringAndSize((char*)self->GetData(), self->GetSize());
        }
    }
};


// TODO: Implement wxPyCustomDataObject allowing GetSize, GetData and SetData
// to be overloaded.

//---------------------------------------------------------------------------

class wxURLDataObject : public wxDataObjectComposite {
public:
    wxURLDataObject();

    wxString GetURL();
    void SetURL(const wxString& url);
};

//---------------------------------------------------------------------------

#ifndef __WXGTK__

%{
#include <wx/metafile.h>
%}

class wxMetafileDataObject : public wxDataObjectSimple
{
public:
    wxMetafileDataObject();

    void SetMetafile(const wxMetafile& metafile);
    wxMetafile GetMetafile() const;
};


#else
%{
class wxMetafileDataObject : public wxDataObjectSimple
{
public:
    wxMetafileDataObject() { PyErr_SetNone(PyExc_NotImplementedError); }
};    
%}

class wxMetafileDataObject : public wxDataObjectSimple
{
public:
    wxMetafileDataObject();
};

#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

