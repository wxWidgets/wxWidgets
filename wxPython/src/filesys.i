/////////////////////////////////////////////////////////////////////////////
// Name:        filesys.i
// Purpose:     SWIG definitions of the wxFileSystem family of classes
//
// Author:      Joerg Baumann
//
// Created:     25-Sept-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Joerg Baumann
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module filesys

%{
#include "helpers.h"
#include <wx/filesys.h>
#include <wx/fs_inet.h>
#include <wx/fs_mem.h>
#include <wx/fs_zip.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import utils.i
%import image.i
%import streams.i

%pragma(python) code = "import wx"
%pragma(python) code = "import string"

//---------------------------------------------------------------------------

//  // typemaps for wxInputStream: Note wxFSFile object has to do the delete
//  // of wxInputStream *
//  %typemap(python,in) wxInputStream *stream {
//      if (PyInstance_Check($source)) {
//          wxPyInputStream* ptr;
//          if (SWIG_GetPtrObj($source, (void **) &ptr,"_wxPyInputStream_p")) {
//              PyErr_SetString(PyExc_TypeError,"Expected _wxInputStream_p.");
//              return NULL;
//          }
//          $target = ptr->wxi;
//      } else {
//          PyErr_SetString(PyExc_TypeError,"Expected _wxInputStream_p.");
//          return NULL;
//      }
//  }


//  // typemaps for wxInputStream: Note wxFSFile object has to do the delete
//  // of wxInputStream *
//  %typemap(python,out) wxInputStream* {
//      wxPyInputStream * _ptr = NULL;

//      if ($source) {
//          _ptr = new wxPyInputStream($source);
//      }
//      if (_ptr) {
//          char    swigptr[64];
//          SWIG_MakePtr(swigptr, _ptr, "_wxPyInputStream_p");

//          PyObject* classobj = PyDict_GetItemString(wxPython_dict, "wxInputStreamPtr");
//          if (! classobj) {
//              Py_INCREF(Py_None);
//              $target = Py_None;
//          } else {
//              PyObject* arg = Py_BuildValue("(s)", swigptr);
//              $target = PyInstance_New(classobj, arg, NULL);
//              Py_DECREF(arg);

//              // set ThisOwn
//              PyObject_SetAttrString($target, "thisown", PyInt_FromLong(1));
//          }
//      } else {
//          Py_INCREF(Py_None);
//          $target = Py_None;
//      }
//  }



class wxFSFile {
public:
    wxFSFile(wxInputStream *stream, const wxString& loc,
             const wxString& mimetype, const wxString& anchor,
             wxDateTime modif);

    wxInputStream *GetStream();
    const wxString& GetMimeType();
    const wxString& GetLocation();
    const wxString& GetAnchor();
    wxDateTime GetModificationTime();
};


// clear typemaps
%typemap(python,in)  wxInputStream *stream;
%typemap(python,out) wxInputStream *;


//---------------------------------------------------------------------------

%{
// wxPyFileSystemHandler will be the Python class wxFileSystemHandler and handling
// the callback functions
class wxPyFileSystemHandler : public wxFileSystemHandler {
public:
    wxPyFileSystemHandler() : wxFileSystemHandler() {}

    DEC_PYCALLBACK_BOOL_STRING_pure(CanOpen);
    DEC_PYCALLBACK_FSF_FSSTRING_pure(OpenFile);
    DEC_PYCALLBACK_STRING_STRINGINT_pure(FindFirst);
    DEC_PYCALLBACK_STRING__pure(FindNext);

    wxString GetProtocol(const wxString& location) {
        return wxFileSystemHandler::GetProtocol(location);
    }

    wxString GetLeftLocation(const wxString& location) {
        return wxFileSystemHandler::GetLeftLocation(location);
    }

    wxString GetAnchor(const wxString& location) {
        return wxFileSystemHandler::GetAnchor(location);
    }

    wxString GetRightLocation(const wxString& location) {
        return wxFileSystemHandler::GetRightLocation(location);
    }

    wxString GetMimeTypeFromExt(const wxString& location){
        return wxFileSystemHandler::GetMimeTypeFromExt(location);
    }

    PYPRIVATE;
};


IMP_PYCALLBACK_BOOL_STRING_pure(wxPyFileSystemHandler, wxFileSystemHandler, CanOpen);
IMP_PYCALLBACK_FSF_FSSTRING_pure(wxPyFileSystemHandler, wxFileSystemHandler, OpenFile);
IMP_PYCALLBACK_STRING_STRINGINT_pure(wxPyFileSystemHandler, wxFileSystemHandler, FindFirst);
IMP_PYCALLBACK_STRING__pure(wxPyFileSystemHandler, wxFileSystemHandler, FindNext);
%}


%name(wxCPPFileSystemHandler)class wxFileSystemHandler {
      wxFileSystemHandler();
}

%name(wxFileSystemHandler)class wxPyFileSystemHandler : public wxFileSystemHandler {
public:
    wxPyFileSystemHandler();

    void _setSelf(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setSelf(self, wxFileSystemHandler)"

    bool CanOpen(const wxString& location);
    wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
    wxString FindFirst(const wxString& spec, int flags = 0);
    wxString FindNext();

    wxString GetProtocol(const wxString& location);
    wxString GetLeftLocation(const wxString& location);
    wxString GetAnchor(const wxString& location);
    wxString GetRightLocation(const wxString& location) const;
    wxString GetMimeTypeFromExt(const wxString& location);
};

//---------------------------------------------------------------------------

class wxFileSystem {
public:
    wxFileSystem();

    void ChangePathTo(const wxString& location, bool is_dir = FALSE);
    wxString GetPath();

    wxFSFile* OpenFile(const wxString& location);

    wxString FindFirst(const wxString& spec, int flags = 0);
    wxString FindNext();

    static void AddHandler(wxFileSystemHandler *handler);
    static void CleanUpHandlers();
};

//---------------------------------------------------------------------------

class wxInternetFSHandler : public wxFileSystemHandler {
public:
    wxInternetFSHandler();
    bool CanOpen(const wxString& location);
    wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
};


//---------------------------------------------------------------------------
class wxZipFSHandler : public wxFileSystemHandler {
public:
    wxZipFSHandler();

    bool CanOpen(const wxString& location);
    wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
    wxString FindFirst(const wxString& spec, int flags = 0);
    wxString FindNext();
};

//---------------------------------------------------------------------------

class wxMemoryFSHandler : public wxFileSystemHandler {
public:
    wxMemoryFSHandler();

    // Remove file from memory FS and free occupied memory
    static void RemoveFile(const wxString& filename);

    bool CanOpen(const wxString& location);
    wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
    wxString FindFirst(const wxString& spec, int flags = 0);
    virtual wxString FindNext();
};


// getting the overloaded static AddFile method right
%inline %{
void __wxMemoryFSHandler_AddFile_wxImage(const wxString& filename,
                                         wxImage& image,
                                         long type) {
    wxMemoryFSHandler::AddFile(filename, image, type);
}

void __wxMemoryFSHandler_AddFile_wxBitmap(const wxString& filename,
                                          const wxBitmap& bitmap,
                                          long type) {
    wxMemoryFSHandler::AddFile(filename, bitmap, type);
}

//  void __wxMemoryFSHandler_AddFile_wxString(const wxString& filename,
//                                            const wxString& textdata) {
//      wxMemoryFSHandler::AddFile(filename, textdata);
//  }

void __wxMemoryFSHandler_AddFile_Data(const wxString& filename,
                                      PyObject* data) {

    wxMemoryFSHandler::AddFile(filename,
                               (void*)PyString_AsString(data),
                               (size_t)PyString_Size(data));
}
%}


// case switch for overloading
%pragma(python) code = "
import types
def wxMemoryFSHandler_AddFile(filename, a, b=''):
    if isinstance(a, wxImage):
        __wxMemoryFSHandler_AddFile_wxImage(filename, a, b)
    elif isinstance(a, wxBitmap):
        __wxMemoryFSHandler_AddFile_wxBitmap(filename, a, b)
    elif type(a) == types.StringType:
        #__wxMemoryFSHandler_AddFile_wxString(filename, a)
        __wxMemoryFSHandler_AddFile_Data(filename, a)
    else: raise TypeError, 'wxImage, wxBitmap or string expected'
"


//---------------------------------------------------------------------------
