/////////////////////////////////////////////////////////////////////////////
// Name:        streams.i
// Purpose:     SWIG definitions of the wxFileSystem family of classes
//
// Author:      Joerg Baumann
//
// Created:     25-Sept-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Joerg Baumann
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module streams

%{
#include "helpers.h"
#include <wx/stream.h>
#include <wx/list.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i

%pragma(python) code = "import wx"
%pragma(python) code = "import string"

//----------------------------------------------------------------------
// typemaps for wxInputStream
%typemap(python,in) wxInputStream *stream {
    if (PyInstance_Check($source)) {
        wxPyInputStream* ptr;
        if (SWIG_GetPtrObj($source, (void **) &ptr,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Expected _wxInputStream_p.");
            return NULL;
        }
        $target = ptr->wxi;
    } else {
        PyErr_SetString(PyExc_TypeError,"Expected _wxInputStream_p.");
        return NULL;
    }
}


// typemaps for wxInputStream
%typemap(python,out) wxInputStream* {
    wxPyInputStream * _ptr = NULL;

    if ($source) {
        _ptr = new wxPyInputStream($source);
    }
    if (_ptr) {
        char    swigptr[64];
        SWIG_MakePtr(swigptr, _ptr, "_wxPyInputStream_p");

        PyObject* classobj = PyDict_GetItemString(wxPython_dict, "wxInputStreamPtr");
        if (! classobj) {
            Py_INCREF(Py_None);
            $target = Py_None;
        } else {
            PyObject* arg = Py_BuildValue("(s)", swigptr);
            $target = PyInstance_New(classobj, arg, NULL);
            Py_DECREF(arg);

            // set ThisOwn
            PyObject* one = PyInt_FromLong(1);
            PyObject_SetAttrString($target, "thisown", one);
            Py_DECREF(one);
        }
    } else {
        Py_INCREF(Py_None);
        $target = Py_None;
    }
}

//----------------------------------------------------------------------

%{  // C++
// definitions of wxStringPtrList and wxPyInputStream
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(wxStringPtrList);


void wxPyInputStream::close() {
    /* do nothing */
}

void wxPyInputStream::flush() {
    /*do nothing*/
}

bool wxPyInputStream::eof() {
    if (wxi)
        return wxi->Eof();
    else
        return TRUE;
}

wxPyInputStream::~wxPyInputStream() {
    /*do nothing*/
}

wxString* wxPyInputStream::read(int size) {
    wxString* s = NULL;
    const int BUFSIZE = 1024;

    // check if we have a real wxInputStream to work with
    if (!wxi) {
        PyErr_SetString(PyExc_IOError,"no valid C-wxInputStream below");
        return NULL;
    }

    if (size < 0) {
        // init buffers
        char * buf = new char[BUFSIZE];
        if (!buf) {
            PyErr_NoMemory();
            return NULL;
        }

        s = new wxString();
        if (!s) {
            delete buf;
            PyErr_NoMemory();
            return NULL;
        }

        // read until EOF
        wxPy_BEGIN_ALLOW_THREADS;
        while (! wxi->Eof()) {
            wxi->Read(buf, BUFSIZE);
            //*s += wxString(buf, wxi->LastRead());
            s->Append(buf, wxi->LastRead());
        }
        delete buf;
        wxPy_END_ALLOW_THREADS;

        // error check
        if (wxi->LastError() == wxSTREAM_READ_ERROR) {
            delete s;
            PyErr_SetString(PyExc_IOError,"IOError in wxInputStream");
            return NULL;
        }

    } else {  // Read only size number of characters
        s = new wxString;
        if (!s) {
            PyErr_NoMemory();
            return NULL;
        }

        // read size bytes
        wxPy_BEGIN_ALLOW_THREADS;
        wxi->Read(s->GetWriteBuf(size+1), size);
        s->UngetWriteBuf(wxi->LastRead());
        wxPy_END_ALLOW_THREADS;

        // error check
        if (wxi->LastError() == wxSTREAM_READ_ERROR) {
            delete s;
            PyErr_SetString(PyExc_IOError,"IOError in wxInputStream");
            return NULL;
        }
    }
    return s;
}


wxString* wxPyInputStream::readline (int size) {
    // check if we have a real wxInputStream to work with
    if (!wxi) {
        PyErr_SetString(PyExc_IOError,"no valid C-wxInputStream below");
        return NULL;
    }

    // init buffer
    int i;
    char ch;
    wxString* s = new wxString;
    if (!s) {
        PyErr_NoMemory();
        return NULL;
    }

    // read until \n or byte limit reached
    wxPy_BEGIN_ALLOW_THREADS;
    for (i=ch=0; (ch != '\n') && (!wxi->Eof()) && ((size < 0) || (i < size)); i++) {
        *s += ch = wxi->GetC();
    }
    wxPy_END_ALLOW_THREADS;

    // errorcheck
    if (wxi->LastError() == wxSTREAM_READ_ERROR) {
        delete s;
        PyErr_SetString(PyExc_IOError,"IOError in wxInputStream");
        return NULL;
    }
    return s;
}


wxStringPtrList* wxPyInputStream::readlines (int sizehint) {
    // check if we have a real wxInputStream to work with
    if (!wxi) {
        PyErr_SetString(PyExc_IOError,"no valid C-wxInputStream below");
        return NULL;
    }

    // init list
    wxStringPtrList* l = new wxStringPtrList();
    if (!l) {
        PyErr_NoMemory();
        return NULL;
    }

    // read sizehint bytes or until EOF
    wxPy_BEGIN_ALLOW_THREADS;
    int i;
    for (i=0; (!wxi->Eof()) && ((sizehint < 0) || (i < sizehint));) {
        wxString* s = readline();
        if (s == NULL) {
            l->DeleteContents(TRUE);
            l->Clear();
            return NULL;
        }
        l->Append(s);
        i = i + s->Length();
    }
    wxPy_END_ALLOW_THREADS;

    // error check
    if (wxi->LastError() == wxSTREAM_READ_ERROR) {
        l->DeleteContents(TRUE);
        l->Clear();
        PyErr_SetString(PyExc_IOError,"IOError in wxInputStream");
        return NULL;
    }
    return l;
}


void wxPyInputStream::seek(int offset, int whence) {
    if (wxi)
        wxi->SeekI(offset, wxSeekMode(whence));
}

int wxPyInputStream::tell(){
    if (wxi)
        return wxi->TellI();
}



// wxInputStream which operates on a Python file-like object
class wxPyCBInputStream : public wxInputStream {
protected:
    PyObject* read;
    PyObject* seek;
    PyObject* tell;
    PyObject* py;

    virtual size_t OnSysRead(void *buffer, size_t bufsize) {
        if (bufsize == 0)
            return 0;

        bool doSave = wxPyRestoreThread();
        PyObject* arglist = Py_BuildValue("(i)", bufsize);
        PyObject* result = PyEval_CallObject(read, arglist);
        Py_DECREF(arglist);

        size_t o = 0;
        if ((result != NULL) && PyString_Check(result)) {
            o = PyString_Size(result);
            if (o == 0)
                m_lasterror = wxSTREAM_EOF;
            if (o > bufsize)
                o = bufsize;
            strncpy((char*)buffer, PyString_AsString(result), o);
            Py_DECREF(result);

        }
        else
            m_lasterror = wxSTREAM_READ_ERROR;
        wxPySaveThread(doSave);
        m_lastcount = o;
        return o;
    }

    virtual size_t OnSysWrite(const void *buffer, size_t bufsize){
        m_lasterror = wxSTREAM_WRITE_ERROR;
        return 0;
    }

    virtual off_t OnSysSeek(off_t off, wxSeekMode mode){
        bool doSave = wxPyRestoreThread();
        PyObject*arglist = Py_BuildValue("(ii)", off, mode);
        PyObject*result = PyEval_CallObject(seek, arglist);
        Py_DECREF(arglist);
        Py_XDECREF(result);
        wxPySaveThread(doSave);
        return OnSysTell();
    }

    virtual off_t OnSysTell() const{
        bool doSave = wxPyRestoreThread();
        PyObject* arglist = Py_BuildValue("()");
        PyObject* result = PyEval_CallObject(tell, arglist);
        Py_DECREF(arglist);
        off_t o = 0;
        if (result != NULL) {
            o = PyInt_AsLong(result);
            Py_DECREF(result);
        };
        wxPySaveThread(doSave);
        return o;
    }

    wxPyCBInputStream(PyObject *p, PyObject *r, PyObject *s, PyObject *t)
        : py(p), read(r), seek(s), tell(t)
        {}

public:
    ~wxPyCBInputStream() {
        bool doSave = wxPyRestoreThread();
        Py_XDECREF(py);
        Py_XDECREF(read);
        Py_XDECREF(seek);
        Py_XDECREF(tell);
        wxPySaveThread(doSave);
    }

    virtual size_t GetSize() {
        if (seek && tell) {
            off_t temp = OnSysTell();
            off_t ret = OnSysSeek(0, wxFromEnd);
            OnSysSeek(temp, wxFromStart);
            return ret;
        }
        else
            return 0;
    }

    static wxPyCBInputStream* create(PyObject *py) {
        PyObject* read;
        PyObject* seek;
        PyObject* tell;

        if (!PyInstance_Check(py) && !PyFile_Check(py)) {
            PyErr_SetString(PyExc_TypeError, "Not a file-like object");
            Py_XDECREF(py);
            return NULL;
        }
        read = getMethod(py, "read");
        seek = getMethod(py, "seek");
        tell = getMethod(py, "tell");

        if (!read) {
            PyErr_SetString(PyExc_TypeError, "Not a file-like object");
            Py_XDECREF(py);
            Py_XDECREF(read);
            Py_XDECREF(seek);
            Py_XDECREF(tell);
            return NULL;
        }
        return new wxPyCBInputStream(py, read, seek, tell);
    }

    static PyObject* getMethod(PyObject* py, char* name) {
        if (!PyObject_HasAttrString(py, name))
            return NULL;
        PyObject* o = PyObject_GetAttrString(py, name);
        if (!PyMethod_Check(o) && !PyCFunction_Check(o)) {
            Py_DECREF(o);
            return NULL;
        }
        return o;
    }

protected:

};

%}  // End of the C++
//----------------------------------------------------------------------


// block threads for wxPyInputStream  ****  WHY?
%except(python) {
    $function
}


// wxStringPtrList* to python list of strings typemap
%typemap(python, out) wxStringPtrList* {
    if ($source) {
        $target = PyList_New($source->GetCount());
        wxStringPtrList::Node *node = $source->GetFirst();
        for (int i=0; node; i++) {
            wxString *s = node->GetData();
            PyList_SetItem($target, i, PyString_FromStringAndSize(s->c_str(), s->Len()));
            node = node->GetNext();
            delete s;
        }
        delete $source;
    }
    else
        $target=0;
}


// transport exceptions via %target=0
%typemap(python, out) wxPyInputStream* {
    char _ptemp[128];
    if ($source) {
        SWIG_MakePtr(_ptemp, (char *) $source,"_wxPyInputStream_p");
        $target = Py_BuildValue("s",_ptemp);
    }
    else
        $target=0;
}


// transport exceptions via %target=0
%typemap(python, out) wxString* {
    if ($source) {
        $target = PyString_FromStringAndSize($source->c_str(), $source->Len());
        delete $source;
    }
    else
        $target=0;
}



%name(wxInputStream) class wxPyInputStream {
public:
    %addmethods {
        wxPyInputStream(PyObject* p) {
            wxInputStream* wxi = wxPyCBInputStream::create(p);
            if (wxi)
                return new wxPyInputStream(wxi);
            else
                return NULL;
        }
    }
    void close();
    void flush();
    bool eof();
    wxString* read(int size=-1);
    wxString* readline(int size=-1);
    wxStringPtrList* readlines(int sizehint=-1);
    void seek(int offset, int whence=0);
    int tell();
    /*
      bool isatty();
      int fileno();
      void truncate(int size=-1);
      void write(wxString data);
      void writelines(wxStringPtrList);
    */
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
        void write(const wxString& str) {
            self->Write(str.c_str(), str.Length());
        }
    }
};


// restore except and typemaps
%typemap(python,out) wxStringPtrList*;
%typemap(python,out) wxPyInputStream*;
%typemap(python, out) wxString* {
    $target = PyString_FromStringAndSize($source->c_str(), $source->Len());
}
%except(python) {
    wxPy_BEGIN_ALLOW_THREADS;
    $function
    wxPy_END_ALLOW_THREADS;
}


