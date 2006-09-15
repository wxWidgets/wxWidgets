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


DocStr(wxDataFormat,
"A wx.DataFormat is an encapsulation of a platform-specific format
handle which is used by the system for the clipboard and drag and drop
operations. The applications are usually only interested in, for
example, pasting data from the clipboard only if the data is in a
format the program understands.  A data format is is used to uniquely
identify this format.",
"
On the system level, a data format is usually just a number, (which
may be the CLIPFORMAT under Windows or Atom under X11, for example.)

The standard format IDs are:

    ================    =====================================
    wx.DF_INVALID       An invalid format
    wx.DF_TEXT          Text format 
    wx.DF_BITMAP        A bitmap (wx.Bitmap)
    wx.DF_METAFILE      A metafile (wx.Metafile, Windows only)
    wx.DF_FILENAME      A list of filenames
    wx.DF_HTML          An HTML string. This is only valid on
                        Windows and non-unicode builds
    ================    =====================================

Besies the standard formats, the application may also use custom
formats which are identified by their names (strings) and not numeric
identifiers. Although internally custom format must be created (or
registered) first, you shouldn\'t care about it because it is done
automatically the first time the wxDataFormat object corresponding to
a given format name is created.

");

class wxDataFormat {
public:
    DocCtorStr(
        wxDataFormat( wxDataFormatId type ),
        "Constructs a data format object for one of the standard data formats
or an empty data object (use SetType or SetId later in this case)", "");

    DocCtorStrName(
        wxDataFormat(const wxString& format),
        "Constructs a data format object for a custom format identified by its
name.", "",
        CustomDataFormat);
    
    ~wxDataFormat();    

    
    %nokwargs operator==;
    %nokwargs operator!=;
    bool operator==(wxDataFormatId format) const;
    bool operator!=(wxDataFormatId format) const;
    bool operator==(const wxDataFormat& format) const;
    bool operator!=(const wxDataFormat& format) const;

    
    DocDeclStr(
        void , SetType(wxDataFormatId format),
        "Sets the format to the given value, which should be one of wx.DF_XXX
constants.", "");
    
    DocDeclStr(
        wxDataFormatId , GetType() const,
        "Returns the platform-specific number identifying the format.", "");
    

    DocDeclStr(
        wxString , GetId() const,
        "Returns the name of a custom format (this function will fail for a
standard format).", "");
    
    DocDeclStr(
        void , SetId(const wxString& format),
        "Sets the format to be the custom format identified by the given name.", "");    

    %property(Id, GetId, SetId, doc="See `GetId` and `SetId`");
    %property(Type, GetType, SetType, doc="See `GetType` and `SetType`");
};


%immutable;
const wxDataFormat wxFormatInvalid;
%mutable;

//---------------------------------------------------------------------------


DocStr(wxDataObject,
"A wx.DataObject represents data that can be copied to or from the
clipboard, or dragged and dropped. The important thing about
wx.DataObject is that this is a 'smart' piece of data unlike usual
'dumb' data containers such as memory buffers or files. Being 'smart'
here means that the data object itself should know what data formats
it supports and how to render itself in each of supported formats.

**NOTE**: This class is an abstract base class and can not be used
directly from Python.  If you need a custom type of data object then
you should instead derive from `wx.PyDataObjectSimple` or use
`wx.CustomDataObject`.
", "
Not surprisingly, being 'smart' comes at a price of added
complexity. This is reasonable for the situations when you really need
to support multiple formats, but may be annoying if you only want to
do something simple like cut and paste text.

To provide a solution for both cases, wxWidgets has two predefined
classes which derive from wx.DataObject: `wx.DataObjectSimple` and
`wx.DataObjectComposite`.  `wx.DataObjectSimple` is the simplest
wx.DataObject possible and only holds data in a single format (such as
text or bitmap) and `wx.DataObjectComposite` is the simplest way to
implement a wx.DataObject which supports multiple simultaneous formats
because it achievs this by simply holding several
`wx.DataObjectSimple` objects.

Please note that the easiest way to use drag and drop and the
clipboard with multiple formats is by using `wx.DataObjectComposite`,
but it is not the most efficient one as each `wx.DataObjectSimple`
would contain the whole data in its respective formats. Now imagine
that you want to paste 200 pages of text in your proprietary format,
as well as Word, RTF, HTML, Unicode and plain text to the clipboard
and even today's computers are in trouble. For this case, you will
have to derive from wx.DataObject directly and make it enumerate its
formats and provide the data in the requested format on
demand. (**TODO**: This is currently not possible from Python.  Make
it so.)

Note that the platform transfer mechanisms for the clipboard and drag
and drop, do not copy any data out of the source application until
another application actually requests the data. This is in contrast to
the 'feel' offered to the user of a program who would normally think
that the data resides in the clipboard after having pressed 'Copy' -
in reality it is only declared to be available.
");

// [other docs...]
// There are several predefined data object classes derived from
// wxDataObjectSimple: wxFileDataObject, wxTextDataObject and
// wxBitmapDataObject which can be used without change.

// You may also derive your own data object classes from
// wxCustomDataObject for user-defined types. The format of user-defined
// data is given as mime-type string literal, such as 'application/word'
// or 'image/png'. These strings are used as they are under Unix (so far
// only GTK) to identify a format and are translated into their Windows
// equivalent under Win32 (using the OLE IDataObject for data exchange to
// and from the clipboard and for drag and drop). Note that the format
// string translation under Windows is not yet finished.



class wxDataObject {      
public:
    enum Direction {
        Get  = 0x01,    // format is supported by GetDataHere()
        Set  = 0x02,    // format is supported by SetData()
        Both = 0x03     // format is supported by both (unused currently)
    };

    // wxDataObject();  // ***  It's an ABC.
    ~wxDataObject();

    DocDeclStr(
        virtual wxDataFormat , GetPreferredFormat(Direction dir = Get) const,
        "Returns the preferred format for either rendering the data (if dir is
Get, its default value) or for setting it. Usually this will be the
native format of the wx.DataObject.", "");
    

    DocDeclStr(
        virtual size_t , GetFormatCount(Direction dir = Get) const,
        "Returns the number of available formats for rendering or setting the
data.", "");
    

    DocDeclStr(
        bool , IsSupported(const wxDataFormat& format, Direction dir = Get) const,
        "Returns True if this format is supported.", "");
    

    DocDeclStr(
        virtual size_t , GetDataSize(const wxDataFormat& format) const,
        "Get the (total) size of data for the given format", "");
    


    %extend {
        DocAStr(GetAllFormats,
                "GetAllFormats(self, int dir=Get) -> [formats]",
                "Returns a list of all the wx.DataFormats that this dataobject supports
in the given direction.", "");
        PyObject* GetAllFormats(Direction dir = Get) {
            size_t count = self->GetFormatCount(dir);
            wxDataFormat* formats = new wxDataFormat[count];
            self->GetAllFormats(formats, dir);

            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject* list = PyList_New(count);
            for (size_t i=0; i<count; i++) {
                wxDataFormat* format = new wxDataFormat(formats[i]);
                PyObject* obj = wxPyConstructObject((void*)format, wxT("wxDataFormat"), true);
                PyList_SET_ITEM(list, i, obj); // PyList_SET_ITEM steals a reference
            }            
            wxPyEndBlockThreads(blocked);
            delete [] formats;
            return list;
        }
    }

    

    // copy raw data (in the specified format) to the provided buffer, return
    // True if data copied successfully, False otherwise
    // virtual bool GetDataHere(const wxDataFormat& format, void *buf) const;

    %extend {
        DocAStr(GetDataHere,
                "GetDataHere(self, DataFormat format) -> String",
                "Get the data bytes in the specified format, returns None on failure.", "
:todo: This should use the python buffer interface isntead...");
        PyObject* GetDataHere(const wxDataFormat& format) {
            PyObject* rval = NULL;
            size_t size = self->GetDataSize(format);            
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            if (size) {
                char* buf = new char[size];
                if (self->GetDataHere(format, buf)) 
                    rval = PyString_FromStringAndSize(buf, size);
                delete [] buf;
            }
            if (! rval) {
                rval = Py_None;
                Py_INCREF(rval);
            }
            wxPyEndBlockThreads(blocked);
            return rval;
        }
    }

    
    // get data from the buffer of specified length (in the given format),
    // return True if the data was read successfully, False otherwise
    //virtual bool SetData(const wxDataFormat& format,
    //                     size_t len, const void * buf);
    DocAStr(SetData,
            "SetData(self, DataFormat format, String data) -> bool",
            "Set the data in the specified format from the bytes in the the data string.
", "
:todo: This should use the python buffer interface isntead...");
    %extend {
        bool SetData(const wxDataFormat& format, PyObject* data) {
            bool rval;
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            if (PyString_Check(data)) {
                rval = self->SetData(format, PyString_Size(data), PyString_AsString(data));
            }
            else {
                // raise a TypeError if not a string
                PyErr_SetString(PyExc_TypeError, "String expected.");
                rval = false;
            }
            wxPyEndBlockThreads(blocked);
            return rval;
        }
    }
    
    %property(AllFormats, GetAllFormats, doc="See `GetAllFormats`");
    %property(DataHere, GetDataHere, doc="See `GetDataHere`");
    %property(DataSize, GetDataSize, doc="See `GetDataSize`");
    %property(FormatCount, GetFormatCount, doc="See `GetFormatCount`");
    %property(PreferredFormat, GetPreferredFormat, doc="See `GetPreferredFormat`");

};

    
//---------------------------------------------------------------------------


DocStr(wxDataObjectSimple,
"wx.DataObjectSimple is a `wx.DataObject` which only supports one
format.  This is the simplest possible `wx.DataObject` implementation.

This is still an \"abstract base class\" meaning that you can't use it
directly.  You either need to use one of the predefined base classes,
or derive your own class from `wx.PyDataObjectSimple`.
", "");

class wxDataObjectSimple : public wxDataObject {
public:
    DocCtorStr(
        wxDataObjectSimple(const wxDataFormat& format = wxFormatInvalid),
        "Constructor accepts the supported format (none by default) which may
also be set later with `SetFormat`.","");


    DocDeclStr(
        const wxDataFormat& , GetFormat(),
        "Returns the (one and only one) format supported by this object. It is
assumed that the format is supported in both directions.", "");
    
    DocDeclStr(
        void , SetFormat(const wxDataFormat& format),
        "Sets the supported format.", "");   

    DocDeclStr(
        virtual size_t , GetDataSize() const,
        "Get the size of our data.", "");


    
    %extend {
        DocAStr(GetDataHere,
                "GetDataHere(self) -> String",
                "Returns the data bytes from the data object as a string, returns None
on failure.  Must be implemented in the derived class if the object
supports rendering its data.", "");
        PyObject* GetDataHere() {
            PyObject* rval = NULL;
            size_t size = self->GetDataSize();            
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            if (size) {
                char* buf = new char[size];
                if (self->GetDataHere(buf)) 
                    rval = PyString_FromStringAndSize(buf, size);
                delete [] buf;
            }
            if (! rval) {
                rval = Py_None;
                Py_INCREF(rval);
            }
            wxPyEndBlockThreads(blocked);
            return rval;
        }
    }

    
    %extend {
        DocAStr(SetData,
                "SetData(self, String data) -> bool",
                "Copy the data value to the data object.  Must be implemented in the
derived class if the object supports setting its data.
", "");
        bool SetData(PyObject* data) {
            bool rval;
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            if (PyString_Check(data)) {
                rval = self->SetData(PyString_Size(data), PyString_AsString(data));
            }
            else {
                // raise a TypeError if not a string
                PyErr_SetString(PyExc_TypeError, "String expected.");
                rval = false;
            }
            wxPyEndBlockThreads(blocked);
            return rval;
        }
    }
    
    %property(Format, GetFormat, SetFormat, doc="See `GetFormat` and `SetFormat`");
};




%{  // Create a new class for wxPython to use
class wxPyDataObjectSimple : public wxDataObjectSimple {
public:
    wxPyDataObjectSimple(const wxDataFormat& format = wxFormatInvalid)
        : wxDataObjectSimple(format) {}

    DEC_PYCALLBACK_SIZET__const(GetDataSize);
    bool GetDataHere(void *buf) const;
    bool SetData(size_t len, const void *buf);
    PYPRIVATE;
};

IMP_PYCALLBACK_SIZET__const(wxPyDataObjectSimple, wxDataObjectSimple, GetDataSize);

bool wxPyDataObjectSimple::GetDataHere(void *buf) const {
    // We need to get the data for this object and write it to buf.  I think
    // the best way to do this for wxPython is to have the Python method
    // return either a string or None and then act appropriately with the
    // C++ version.

    bool rval = false;
    wxPyBlock_t blocked = wxPyBeginBlockThreads();
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
    wxPyEndBlockThreads(blocked);
    return rval;
}

bool wxPyDataObjectSimple::SetData(size_t len, const void *buf) {
    // For this one we simply need to make a string from buf and len
    // and send it to the Python method.
    bool rval = false;
    wxPyBlock_t blocked = wxPyBeginBlockThreads();
    if (wxPyCBH_findCallback(m_myInst, "SetData")) {
        PyObject* data = PyString_FromStringAndSize((char*)buf, len);
        rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", data));
        Py_DECREF(data);
    }
    wxPyEndBlockThreads(blocked);
    return rval;
}
%}



// Now define it for SWIG
DocStr(wxPyDataObjectSimple,
"wx.PyDataObjectSimple is a version of `wx.DataObjectSimple` that is
Python-aware and knows how to reflect calls to its C++ virtual methods
to methods in the Python derived class.  You should derive from this
class and overload `GetDataSize`, `GetDataHere` and `SetData` when you
need to create your own simple single-format type of `wx.DataObject`.
", "
Here is a simple example::

    class MyDataObject(wx.PyDataObjectSimple):
        def __init__(self):
            wx.PyDataObjectSimple.__init__(
                self, wx.CustomDataFormat('MyDOFormat'))
            self.data = ''

        def GetDataSize(self):
            return len(self.data)
        def GetDataHere(self):
            return self.data  # returns a string  
        def SetData(self, data):
            self.data = data
            return True

Note that there is already a `wx.CustomDataObject` class that behaves
very similarly to this example.  The value of creating your own
derived class like this is to be able to do additional things when the
data is requested or given via the clipboard or drag and drop
operation, such as generate the data value or decode it into needed
data structures.
");
class wxPyDataObjectSimple : public wxDataObjectSimple {
public:
    %pythonAppend wxPyDataObjectSimple   "self._setCallbackInfo(self, PyDataObjectSimple)"

    wxPyDataObjectSimple(const wxDataFormat& format = wxFormatInvalid);
    void _setCallbackInfo(PyObject* self, PyObject* _class);
};


//---------------------------------------------------------------------------


DocStr(wxDataObjectComposite,
"wx.DataObjectComposite is the simplest `wx.DataObject` derivation
which may be sued to support multiple formats. It contains several
'wx.DataObjectSimple` objects and supports any format supported by at
least one of them. Only one of these data objects is *preferred* (the
first one if not explicitly changed by using the second parameter of
`Add`) and its format determines the preferred format of the composite
data object as well.

See `wx.DataObject` documentation for the reasons why you might prefer
to use wx.DataObject directly instead of wx.DataObjectComposite for
efficiency reasons.
", "");

class wxDataObjectComposite : public wxDataObject {
public:
    wxDataObjectComposite();

    %disownarg( wxDataObjectSimple *dataObject );
    
    DocDeclStr(
        void , Add(wxDataObjectSimple *dataObject, bool preferred = false),
        "Adds the dataObject to the list of supported objects and it becomes
the preferred object if preferred is True.", "");
    
    %cleardisown( wxDataObjectSimple *dataObject );

    DocDeclStr(
        wxDataFormat , GetReceivedFormat() const,
        "Report the format passed to the `SetData` method.  This should be the
format of the data object within the composite that recieved data from
the clipboard or the DnD operation.  You can use this method to find
out what kind of data object was recieved.", "");
    
    %property(ReceivedFormat, GetReceivedFormat, doc="See `GetReceivedFormat`");
};

//---------------------------------------------------------------------------

DocStr(wxTextDataObject,
"wx.TextDataObject is a specialization of `wx.DataObject` for text
data. It can be used without change to paste data into the `wx.Clipboard`
or a `wx.DropSource`.

Alternativly, you may wish to derive a new class from the
`wx.PyTextDataObject` class for providing text on-demand in order to
minimize memory consumption when offering data in several formats,
such as plain text and RTF, because by default the text is stored in a
string in this class, but it might as well be generated on demand when
requested. For this, `GetTextLength` and `GetText` will have to be
overridden.", "");
class wxTextDataObject : public wxDataObjectSimple {
public:
    DocCtorStr(
        wxTextDataObject(const wxString& text = wxPyEmptyString),
        "Constructor, may be used to initialise the text (otherwise `SetText`
should be used later).", "");

    DocDeclStr(
        size_t , GetTextLength(),
        "Returns the data size.  By default, returns the size of the text data
set in the constructor or using `SetText`.  This can be overridden (via
`wx.PyTextDataObject`) to provide text size data on-demand. It is
recommended to return the text length plus 1 for a trailing zero, but
this is not strictly required.", "");
    
    DocDeclStr(
        wxString , GetText(),
        "Returns the text associated with the data object.", "");
    
    DocDeclStr(
        void , SetText(const wxString& text),
        "Sets the text associated with the data object. This method is called
when the data object receives the data and, by default, copies the
text into the member variable. If you want to process the text on the
fly you may wish to override this function (via
`wx.PyTextDataObject`.)", "");
    
    %property(Text, GetText, SetText, doc="See `GetText` and `SetText`");
    %property(TextLength, GetTextLength, doc="See `GetTextLength`");
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

DocStr(wxPyTextDataObject,
"wx.PyTextDataObject is a version of `wx.TextDataObject` that is
Python-aware and knows how to reflect calls to its C++ virtual methods
to methods in the Python derived class.  You should derive from this
class and overload `GetTextLength`, `GetText`, and `SetText` when you
want to be able to provide text on demand instead of preloading it
into the data object.", "");

class wxPyTextDataObject : public wxTextDataObject {
public:
    %pythonAppend wxPyTextDataObject   "self._setCallbackInfo(self, PyTextDataObject)"

    wxPyTextDataObject(const wxString& text = wxPyEmptyString);
    void _setCallbackInfo(PyObject* self, PyObject* _class);
};

//---------------------------------------------------------------------------

DocStr(wxBitmapDataObject,
"wx.BitmapDataObject is a specialization of wxDataObject for bitmap
data. It can be used without change to paste data into the `wx.Clipboard`
or a `wx.DropSource`.
", "
:see: `wx.PyBitmapDataObject` if you wish to override `GetBitmap` to increase efficiency.");

class wxBitmapDataObject : public wxDataObjectSimple {
public:
    DocCtorStr(
        wxBitmapDataObject(const wxBitmap& bitmap = wxNullBitmap),
        "Constructor, optionally passing a bitmap (otherwise use `SetBitmap`
later).", "");

    DocDeclStr(
        wxBitmap , GetBitmap() const,
        "Returns the bitmap associated with the data object.  You may wish to
override this method (by deriving from `wx.PyBitmapDataObject`) when
offering data on-demand, but this is not required by wxWidgets'
internals. Use this method to get data in bitmap form from the
`wx.Clipboard`.", "");
    
    DocDeclStr(
        void , SetBitmap(const wxBitmap& bitmap),
        "Sets the bitmap associated with the data object. This method is called
when the data object receives data. Usually there will be no reason to
override this function.", "");
    
    %property(Bitmap, GetBitmap, SetBitmap, doc="See `GetBitmap` and `SetBitmap`");
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
    wxPyBlock_t blocked = wxPyBeginBlockThreads();
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
    wxPyEndBlockThreads(blocked);
    return *rval;
}
 
void wxPyBitmapDataObject::SetBitmap(const wxBitmap& bitmap) {
    wxPyBlock_t blocked = wxPyBeginBlockThreads();
    if (wxPyCBH_findCallback(m_myInst, "SetBitmap")) {
        PyObject* bo = wxPyConstructObject((void*)&bitmap, wxT("wxBitmap"), false);
        wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", bo));
        Py_DECREF(bo);
    }
    wxPyEndBlockThreads(blocked);
}
%}




DocStr(wxPyBitmapDataObject,
"wx.PyBitmapDataObject is a version of `wx.BitmapDataObject` that is
Python-aware and knows how to reflect calls to its C++ virtual methods
to methods in the Python derived class. To be able to provide bitmap
data on demand derive from this class and overload `GetBitmap`.", "");

class wxPyBitmapDataObject : public wxBitmapDataObject {
public:
    %pythonAppend wxPyBitmapDataObject   "self._setCallbackInfo(self, PyBitmapDataObject)"

    wxPyBitmapDataObject(const wxBitmap& bitmap = wxNullBitmap);
    void _setCallbackInfo(PyObject* self, PyObject* _class);
};

//---------------------------------------------------------------------------


DocStr(wxFileDataObject,
"wx.FileDataObject is a specialization of `wx.DataObjectSimple` for
file names. The program works with it just as if it were a list of
absolute file names, but internally it uses the same format as
Explorer and other compatible programs under Windows or GNOME/KDE
filemanager under Unix which makes it possible to receive files from
them using this class.", "");

class wxFileDataObject : public wxDataObjectSimple
{
public:
    DocCtorStr(
        wxFileDataObject(),
        "", "");

    DocDeclAStr(
        const wxArrayString& , GetFilenames(),
        "GetFilenames(self) -> [names]",
        "Returns a list of file names.", "");
    
    DocDeclStr(
        void , AddFile(const wxString &filename),
        "Adds a file to the list of files represented by this data object.", "");
    
    %property(Filenames, GetFilenames, doc="See `GetFilenames`");
};

//---------------------------------------------------------------------------

DocStr(wxCustomDataObject,
"wx.CustomDataObject is a specialization of `wx.DataObjectSimple` for
some application-specific data in arbitrary format.  Python strings
are used for getting and setting data, but any picklable object can
easily be transfered via strings.  A copy of the data is stored in the
data object.", "");

class wxCustomDataObject : public wxDataObjectSimple {
public:
    %nokwargs wxCustomDataObject;
    wxCustomDataObject(const wxDataFormat& format);
    %extend {
        wxCustomDataObject(const wxString& formatName) {
            return new wxCustomDataObject(wxDataFormat(formatName));
        }
    }
    wxCustomDataObject();
    
    
    %extend {
        DocAStr(SetData,
                "SetData(self, String data) -> bool",
                "Copy the data value to the data object.", "");
        bool SetData(PyObject* data) {
            bool rval;
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            if (PyString_Check(data)) {
                rval = self->SetData(PyString_Size(data), PyString_AsString(data));
            }
            else {
                // raise a TypeError if not a string
                PyErr_SetString(PyExc_TypeError, "String expected.");
                rval = false;
            }
            wxPyEndBlockThreads(blocked);
            return rval;
        }
    }
    %pythoncode { TakeData = SetData }
    
    DocDeclStr(
        size_t , GetSize(),
        "Get the size of the data.", "");
    

    %extend {
        DocAStr(GetData,
                "GetData(self) -> String",
                "Returns the data bytes from the data object as a string.", "");
        PyObject* GetData() {
            PyObject* obj;
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            obj = PyString_FromStringAndSize((char*)self->GetData(), self->GetSize());
            wxPyEndBlockThreads(blocked);
            return obj;
        }
    }

    %property(Data, GetData, SetData, doc="See `GetData` and `SetData`");
    %property(Size, GetSize, doc="See `GetSize`");

};


//---------------------------------------------------------------------------

DocStr(wxURLDataObject,
"This data object holds a URL in a format that is compatible with some
browsers such that it is able to be dragged to or from them.", "");
class wxURLDataObject : public wxDataObject/*Composite*/ {
public:
    wxURLDataObject(const wxString& url = wxPyEmptyString);

    DocDeclStr(
        wxString , GetURL(),
        "Returns a string containing the current URL.", "");
    
    DocDeclStr(
        void , SetURL(const wxString& url),
        "Set the URL.", "");
    
};

//---------------------------------------------------------------------------

#if defined(__WXMSW__) || defined(__WXMAC__)

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
    wxMetafileDataObject() { wxPyRaiseNotImplemented(); }
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

