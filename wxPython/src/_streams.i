/////////////////////////////////////////////////////////////////////////////
// Name:        _streams.i
// Purpose:     SWIG typemaps and wrappers for wxInputStream
//
// Author:      Robin Dunn
//
// Created:     25-Sept-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
#include "wx/wxPython/pyistream.h"
%}

//---------------------------------------------------------------------------
%newgroup


// Typemaps for wxInputStream
//
// We assume that input params taking a wxInputStream& will *not* take
// ownership of the stream and so we manage it in the typemaps. On the other
// hand, when a paramter expects a wxInputStream* then it does take ownership
// (such as wxFSFile) and so the typemap will make a copy of the stream object
// to give to it.
 
%typemap(in) wxInputStream&  (wxPyInputStream* temp, bool created) {
    if (wxPyConvertSwigPtr($input, (void **)&temp, wxT("wxPyInputStream"))) {
        $1 = temp->m_wxis;
        created = false;
    } else {
        PyErr_Clear();  // clear the failure of the wxPyConvert above
        $1 = wxPyCBInputStream_create($input, false);
        if ($1 == NULL) {
            PyErr_SetString(PyExc_TypeError, "Expected wx.InputStream or Python file-like object.");
            SWIG_fail;
        }
        created = true;
    }
}
%typemap(freearg) wxInputStream& { if (created$argnum) delete $1; }


%typemap(in) wxInputStream*  (wxPyInputStream* temp) {
    if (wxPyConvertSwigPtr($input, (void **)&temp, wxT("wxPyInputStream"))) {
        $1 = wxPyCBInputStream_copy((wxPyCBInputStream*)temp->m_wxis);
    } else {
        PyErr_Clear();  // clear the failure of the wxPyConvert above
        $1 = wxPyCBInputStream_create($input, true);
        if ($1 == NULL) {
            PyErr_SetString(PyExc_TypeError, "Expected wx.InputStream or Python file-like object.");
            SWIG_fail;
        }
    }
}



%typemap(out) wxInputStream* {
    wxPyInputStream * _ptr = NULL;

    if ($1) {
        _ptr = new wxPyInputStream($1);
    }
    $result = wxPyConstructObject(_ptr, wxT("wxPyInputStream"), $owner);
}


//---------------------------------------------------------------------------

enum wxSeekMode
{
  wxFromStart,
  wxFromCurrent,
  wxFromEnd
};


%rename(InputStream) wxPyInputStream;
class wxPyInputStream
{
public:
    %extend {
        wxPyInputStream(PyObject* p) {
            wxInputStream* wxis = wxPyCBInputStream::create(p);
            if (wxis)
                return new wxPyInputStream(wxis);
            else
                return NULL;
        }
    }
    ~wxPyInputStream();
    
    void close();
    void flush();
    bool eof();
    PyObject* read(int size=-1);
    PyObject* readline(int size=-1);
    PyObject* readlines(int sizehint=-1);
    void seek(int offset, int whence=0);
    int tell();

    /*
      bool isatty();
      int fileno();
      void truncate(int size=-1);
      void write(wxString data);
      void writelines(wxStringPtrList);
    */

    char Peek();
    char GetC();
    size_t LastRead();
    bool CanRead();
    bool Eof();
    bool Ungetch(char c);

    long SeekI(long pos, wxSeekMode mode = wxFromStart);
    long TellI();
};



// TODO:  make a more fully implemented file interface...
class wxOutputStream {
public:
    /*
      void close();
      void flush();
      wxString* read(int size=-1);
      wxString* readline(int size=-1);
      wxStringPtrList* readlines(int sizehint=-1);
      void seek(int offset, int whence=0);
      int tell();
      bool isatty();
      int fileno();
      void truncate(int size=-1);
      void write(wxString data);
      void writelines(wxStringPtrList);
    */

    %extend {
        void write(PyObject* obj) {
            // We use only strings for the streams, not unicode
            PyObject* str = PyObject_Str(obj);
            if (! str) {
                PyErr_SetString(PyExc_TypeError, "Unable to convert to string");
                return;
            }
            self->Write(PyString_AS_STRING(str),
                        PyString_GET_SIZE(str));
            Py_DECREF(str);
        }
    }
};


//---------------------------------------------------------------------------
%init %{
    wxPyPtrTypeMap_Add("wxInputStream", "wxPyInputStream");
%}
//---------------------------------------------------------------------------
