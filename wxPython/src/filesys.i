/////////////////////////////////////////////////////////////////////////////
// Name:        filesys.i
// Purpose:     SWIG definitions of the wxFileSystem family of classes
//
// Author:      Joerg Baumann and Robin Dunn
//
// Created:     25-Sept-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Joerg Baumann
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module filesys

%{
#include "helpers.h"
#include "pyistream.h"
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

//---------------------------------------------------------------------------


class wxFSFile : public wxObject {
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

    wxString GetMimeTypeFromExt(const wxString& location) {
        return wxFileSystemHandler::GetMimeTypeFromExt(location);
    }

    PYPRIVATE;
};


IMP_PYCALLBACK_BOOL_STRING_pure(wxPyFileSystemHandler, wxFileSystemHandler, CanOpen);
IMP_PYCALLBACK_FSF_FSSTRING_pure(wxPyFileSystemHandler, wxFileSystemHandler, OpenFile);
IMP_PYCALLBACK_STRING_STRINGINT_pure(wxPyFileSystemHandler, wxFileSystemHandler, FindFirst);
IMP_PYCALLBACK_STRING__pure(wxPyFileSystemHandler, wxFileSystemHandler, FindNext);
%}


%name(wxCPPFileSystemHandler)class wxFileSystemHandler : public wxObject {
    wxFileSystemHandler();
}

%name(wxFileSystemHandler)class wxPyFileSystemHandler : public wxFileSystemHandler {
public:
    wxPyFileSystemHandler();

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxFileSystemHandler)"

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

class wxFileSystem : public wxObject {
public:
    wxFileSystem();

    void ChangePathTo(const wxString& location, bool is_dir = FALSE);
    wxString GetPath();

    wxFSFile* OpenFile(const wxString& location);

    wxString FindFirst(const wxString& spec, int flags = 0);
    wxString FindNext();

    static void AddHandler(wxFileSystemHandler *handler);
    static void CleanUpHandlers();

    // Returns the file URL for a native path
    static wxString FileNameToURL(const wxString& filename);

    // Returns the native path for a file URL
    //static wxFileName URLToFileName(const wxString& url);  *** See below
};


// Returns the native path for a file URL
wxString wxFileSystem_URLToFileName(const wxString& url);
%{
    wxString wxFileSystem_URLToFileName(const wxString& url) {
        wxFileName fname = wxFileSystem::URLToFileName(url);
        return fname.GetFullPath();
    }
%}

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

void __wxMemoryFSHandler_AddFile_Data(const wxString& filename,
                                      PyObject* data) {

    wxMemoryFSHandler::AddFile(filename,
                               // TODO:  Verify data type
                               (void*)PyString_AsString(data),
                               (size_t)PyString_Size(data));
}
%}


// case switch for overloading
%pragma(python) code = "
import types
def wxMemoryFSHandler_AddFile(filename, a, b=''):
    if wx.wxPy_isinstance(a, (wxImage, wxImagePtr)):
        __wxMemoryFSHandler_AddFile_wxImage(filename, a, b)
    elif wx.wxPy_isinstance(a, (wxBitmap, wxBitmapPtr)):
        __wxMemoryFSHandler_AddFile_wxBitmap(filename, a, b)
    elif type(a) == types.StringType:
        #__wxMemoryFSHandler_AddFile_wxString(filename, a)
        __wxMemoryFSHandler_AddFile_Data(filename, a)
    else: raise TypeError, 'wxImage, wxBitmap or string expected'
"


//---------------------------------------------------------------------------

%init %{
    wxPyPtrTypeMap_Add("wxFileSystemHandler", "wxPyFileSystemHandler");
%}

//---------------------------------------------------------------------------
