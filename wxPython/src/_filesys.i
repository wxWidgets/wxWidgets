/////////////////////////////////////////////////////////////////////////////
// Name:        _filesys.i
// Purpose:     SWIG definitions of the wxFileSystem family of classes
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


class wxFSFile : public wxObject
{
public:
    %typemap(out) wxFSFile*;    // turn off this typemap

    wxFSFile(wxInputStream *stream, const wxString& loc,
             const wxString& mimetype, const wxString& anchor,
             wxDateTime modif);

    // Turn it back on.
    %typemap(out) wxFSFile* { $result = wxPyMake_wxObject($1, $owner); }

    ~wxFSFile();

    wxInputStream *GetStream();
    void DetachStream();
    
    const wxString& GetMimeType();
    const wxString& GetLocation();
    const wxString& GetAnchor();
    wxDateTime GetModificationTime();

    %property(Anchor, GetAnchor, doc="See `GetAnchor`");
    %property(Location, GetLocation, doc="See `GetLocation`");
    %property(MimeType, GetMimeType, doc="See `GetMimeType`");
    %property(ModificationTime, GetModificationTime, doc="See `GetModificationTime`");
    %property(Stream, GetStream, doc="See `GetStream`");
    
};


//---------------------------------------------------------------------------

%{
class wxPyFileSystemHandler : public wxFileSystemHandler
{
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



%rename(CPPFileSystemHandler) wxFileSystemHandler;
class wxFileSystemHandler //: public wxObject
{
public:
    //wxFileSystemHandler();
    ~wxFileSystemHandler();
};



%rename(FileSystemHandler) wxPyFileSystemHandler;
class wxPyFileSystemHandler : public wxFileSystemHandler
{
public:
    %pythonAppend    wxPyFileSystemHandler "self._setCallbackInfo(self, FileSystemHandler)";

    wxPyFileSystemHandler();

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    bool CanOpen(const wxString& location);
    %newobject OpenFile;
    wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
    wxString FindFirst(const wxString& spec, int flags = 0);
    wxString FindNext();

    wxString GetProtocol(const wxString& location);
    wxString GetLeftLocation(const wxString& location);
    wxString GetAnchor(const wxString& location);
    wxString GetRightLocation(const wxString& location);
    wxString GetMimeTypeFromExt(const wxString& location);
    
    %property(Anchor, GetAnchor, doc="See `GetAnchor`");
    %property(LeftLocation, GetLeftLocation, doc="See `GetLeftLocation`");
    %property(MimeTypeFromExt, GetMimeTypeFromExt, doc="See `GetMimeTypeFromExt`");
    %property(Protocol, GetProtocol, doc="See `GetProtocol`");
    %property(RightLocation, GetRightLocation, doc="See `GetRightLocation`");
};


// //---------------------------------------------------------------------------


class wxFileSystem : public wxObject {
public:
    // turn off this typemap
    %typemap(out) wxFileSystem*;    

    wxFileSystem();
    ~wxFileSystem();

    // Turn it back on again
    %typemap(out) wxFileSystem* { $result = wxPyMake_wxObject($1, $owner); }

    void ChangePathTo(const wxString& location, bool is_dir = false);
    wxString GetPath();

    %newobject OpenFile;
    wxFSFile* OpenFile(const wxString& location);

    wxString FindFirst(const wxString& spec, int flags = 0);
    wxString FindNext();

    %disownarg(wxFileSystemHandler *handler);
    static void AddHandler(wxFileSystemHandler *handler);
    %cleardisown(wxFileSystemHandler *handler);

    // Removes FS handler
    %newobject RemoveHandler;
    static wxFileSystemHandler* RemoveHandler(wxFileSystemHandler *handler);

    static void CleanUpHandlers();

    // Returns the file URL for a native path
    static wxString FileNameToURL(const wxString& filename);

    // Returns the native path for a file URL
    //static wxFileName URLToFileName(const wxString& url);
    %extend {
        static wxString URLToFileName(const wxString& url) {
            wxFileName fname = wxFileSystem::URLToFileName(url);
            return fname.GetFullPath();
        }
    }

    %property(Path, GetPath, doc="See `GetPath`");
};



//---------------------------------------------------------------------------

class wxInternetFSHandler : public wxFileSystemHandler {
public:
    wxInternetFSHandler();
    bool CanOpen(const wxString& location);
    %newobject OpenFile;
    wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
};


//---------------------------------------------------------------------------

class wxZipFSHandler : public wxFileSystemHandler {
public:
    wxZipFSHandler();

    bool CanOpen(const wxString& location);
    %newobject OpenFile;
    wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
    wxString FindFirst(const wxString& spec, int flags = 0);
    wxString FindNext();
};

//---------------------------------------------------------------------------

// TODO: Use SWIG's overloading feature to fix this mess?

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
        if (! PyString_Check(data)) {
            wxPyBLOCK_THREADS(PyErr_SetString(PyExc_TypeError,
                                              "Expected string object"));
            return;
        }

        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        void*  ptr = (void*)PyString_AsString(data);
        size_t size = PyString_Size(data);
        wxPyEndBlockThreads(blocked);

        wxMemoryFSHandler::AddFile(filename, ptr, size);
    }
%}


// case switch for overloading
%pythoncode {
def MemoryFSHandler_AddFile(filename, dataItem, imgType=-1):
    """
    Add 'file' to the memory filesystem.  The dataItem parameter can
    either be a `wx.Bitmap`, `wx.Image` or a string that can contain
    arbitrary data.  If a bitmap or image is used then the imgType
    parameter should specify what kind of image file it should be
    written as, wx.BITMAP_TYPE_PNG, etc.
    """
    if isinstance(dataItem, wx.Image):
        __wxMemoryFSHandler_AddFile_wxImage(filename, dataItem, imgType)
    elif isinstance(dataItem, wx.Bitmap):
        __wxMemoryFSHandler_AddFile_wxBitmap(filename, dataItem, imgType)
    elif type(dataItem) == str:
        __wxMemoryFSHandler_AddFile_Data(filename, dataItem)
    else:
        raise TypeError, 'wx.Image, wx.Bitmap or string expected'
}


class wxMemoryFSHandler : public wxFileSystemHandler {
public:
    wxMemoryFSHandler();

    // Remove file from memory FS and free occupied memory
    static void RemoveFile(const wxString& filename);

    // Add a file to the memory FS
    %pythoncode { AddFile = staticmethod(MemoryFSHandler_AddFile) }

    bool CanOpen(const wxString& location);
    %newobject OpenFile;
    wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
    wxString FindFirst(const wxString& spec, int flags = 0);
    virtual wxString FindNext();
};


//---------------------------------------------------------------------------
%init %{
    wxPyPtrTypeMap_Add("wxFileSystemHandler", "wxPyFileSystemHandler");
%}
//---------------------------------------------------------------------------
