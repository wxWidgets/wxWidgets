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
    if ($1)
        _ptr = new wxPyInputStream($1);
    $result = wxPyConstructObject(_ptr, wxT("wxPyInputStream"), $owner);
}


//---------------------------------------------------------------------------
// Typemaps for wxOutputStream.  We only need in by reference and out by
// pointer in this one.


%typemap(in) wxOutputStream&  (wxPyOutputStream* temp, bool created) {
    if (wxPyConvertSwigPtr($input, (void **)&temp, wxT("wxPyOutputStream"))) {
        $1 = temp->m_wxos;
        created = false;
    } else {
        PyErr_Clear();  // clear the failure of the wxPyConvert above
        $1 = wxPyCBOutputStream_create($input, false);
        if ($1 == NULL) {
            PyErr_SetString(PyExc_TypeError, "Expected wx.OutputStream or Python file-like object.");
            SWIG_fail;
        }
        created = true;
    }
}
%typemap(freearg) wxOutputStream& { if (created$argnum) delete $1; }


%typemap(out) wxOutputStream* {
    wxPyOutputStream * _ptr = NULL;
    if ($1)
        _ptr = new wxPyOutputStream($1);
    $result = wxPyConstructObject(_ptr, wxT("wxPyOutputStream"), $owner);
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

    char Peek();
    char GetC();
    size_t LastRead();
    bool CanRead();
    bool Eof();
    bool Ungetch(char c);

    long SeekI(long pos, wxSeekMode mode = wxFromStart);
    long TellI();
};




%rename(OutputStream) wxPyOutputStream;
class wxPyOutputStream
{
public:
    %extend {
        wxPyOutputStream(PyObject* p) {
            wxOutputStream* wxis = wxPyCBOutputStream::create(p);
            if (wxis)
                return new wxPyOutputStream(wxis);
            else
                return NULL;
        }
    }
    ~wxPyOutputStream();

    void close();
    void flush();
    bool eof();
    void seek(int offset, int whence=0);
    int tell();

    void write(PyObject* data);
    //void writelines(wxStringArray& arr);

    void PutC(char c);
    size_t LastWrite();
    unsigned long SeekO(unsigned long pos, wxSeekMode mode = wxFromStart);
    unsigned long TellO();
};


//---------------------------------------------------------------------------
%init %{
    wxPyPtrTypeMap_Add("wxInputStream", "wxPyInputStream");
    wxPyPtrTypeMap_Add("wxOutputStream", "wxPyOutputStream");
%}
//---------------------------------------------------------------------------
