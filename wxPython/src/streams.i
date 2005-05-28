/////////////////////////////////////////////////////////////////////////////
// Name:        streams.i
// Purpose:     SWIG definitions of the wxFileSystem family of classes
//
// Author:      Joerg Baumann and Robin Dunn
//
// Created:     25-Sept-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Joerg Baumann
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module streams

%{
#include "helpers.h"
#include "pyistream.h"
#include <wx/stream.h>
#include <wx/list.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i

%pragma(python) code = "import wx"

//----------------------------------------------------------------------
// typemaps for wxInputStream


%typemap(python,in) wxInputStream *  (wxPyInputStream* temp, bool created) {
    if (SWIG_GetPtrObj($source, (void **) &temp, "_wxPyInputStream_p") == 0) {
        $target = temp->m_wxis;
        created = FALSE;
    } else {
        $target = wxPyCBInputStream_create($source, FALSE);
        if ($target == NULL) {
            PyErr_SetString(PyExc_TypeError,"Expected _wxInputStream_p or Python file-like object.");
            return NULL;
        }
        created = TRUE;
    }
}

%typemap(python, freearg) wxInputStream * {
    if (created)
        delete $source;
}


// typemaps for wxInputStream
%typemap(python,out) wxInputStream* {
    wxPyInputStream * _ptr = NULL;

    if ($source) {
        _ptr = new wxPyInputStream($source);
    }
    $target = wxPyConstructObject(_ptr, wxT("wxInputStream"), TRUE);
}

//----------------------------------------------------------------------


//  // wxStringPtrList* to python list of strings typemap
//  %typemap(python, out) wxStringPtrList* {
//      if ($source) {
//          $target = PyList_New($source->GetCount());
//          wxStringPtrList::Node *node = $source->GetFirst();
//          for (int i=0; node; i++) {
//              wxString *s = node->GetData();
//  #if wxUSE_UNICODE
//              PyList_SetItem($target, i, PyUnicode_FromUnicode(s->c_str(), s->Len()));
//  #else
//              PyList_SetItem($target, i, PyString_FromStringAndSize(s->c_str(), s->Len()));
//  #endif
//              node = node->GetNext();
//              delete s;
//          }
//          delete $source;
//      }
//      else
//          $target=0;
//  }

enum wxSeekMode
{
  wxFromStart,
  wxFromCurrent,
  wxFromEnd
};



%name(wxInputStream) class wxPyInputStream {
public:
    %addmethods {
        wxPyInputStream(PyObject* p) {
            wxInputStream* wxis = wxPyCBInputStream::create(p);
            if (wxis)
                return new wxPyInputStream(wxis);
            else
                return NULL;
        }
    }
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
}



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

    %addmethods {
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


// restore except and typemaps
%typemap(python,out) wxStringPtrList*;
%typemap(python,out) wxPyInputStream*;


//----------------------------------------------------------------------

%init %{
    wxPyPtrTypeMap_Add("wxInputStream", "wxPyInputStream");
%}

//----------------------------------------------------------------------
