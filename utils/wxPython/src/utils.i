/////////////////////////////////////////////////////////////////////////////
// Name:        utils.i
// Purpose:     SWIG definitions of various utility classes
//
// Author:      Robin Dunn
//
// Created:     25-nov-1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module utils

%{
#include "helpers.h"
#include <wx/config.h>
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i


%{
#ifdef SEPARATE
    static wxString wxPyEmptyStr("");
#endif
%}


%{
    static PyObject* __EnumerationHelper(bool flag, wxString& str, long index) {
        PyObject* ret = PyTuple_New(3);
        if (ret) {
            PyTuple_SET_ITEM(ret, 0, PyInt_FromLong(flag));
            PyTuple_SET_ITEM(ret, 1, PyString_FromString(str));
            PyTuple_SET_ITEM(ret, 2, PyInt_FromLong(index));
        }
        return ret;
    }
%}

//---------------------------------------------------------------------------

class wxConfig {
public:
    wxConfig(const wxString& appName = wxPyEmptyStr,
             const wxString& vendorName = wxPyEmptyStr,
             const wxString& localFilename = wxPyEmptyStr,
             const wxString& globalFilename = wxPyEmptyStr,
             long style = 0);
    ~wxConfig();


    void DontCreateOnDemand();
    bool DeleteAll();  // This is supposed to have been fixed...
    bool DeleteEntry(const wxString& key, bool bDeleteGroupIfEmpty = TRUE);
    bool DeleteGroup(const wxString& key);
    bool Exists(wxString& strName);
    bool Flush(bool bCurrentOnly = FALSE);
    wxString GetAppName();


    // Each of these enumeration methods return a 3-tuple consisting of
    // the continue flag, the value string, and the index for the next call.
    %addmethods {
        PyObject* GetFirstGroup() {
            bool     cont;
            long     index = 0;
            wxString value;

            cont = self->GetFirstGroup(value, index);
            return __EnumerationHelper(cont, value, index);
        }

        PyObject* GetFirstEntry() {
            bool     cont;
            long     index = 0;
            wxString value;

            cont = self->GetFirstEntry(value, index);
            return __EnumerationHelper(cont, value, index);
        }

        PyObject* GetNextGroup(long index) {
            bool     cont;
            wxString value;

            cont = self->GetNextGroup(value, index);
            return __EnumerationHelper(cont, value, index);
        }

        PyObject* GetNextEntry(long index) {
            bool     cont;
            wxString value;

            cont = self->GetNextEntry(value, index);
            return __EnumerationHelper(cont, value, index);
        }
    }


    int GetNumberOfEntries(bool bRecursive = FALSE);
    int GetNumberOfGroups(bool bRecursive = FALSE);
    wxString GetPath();
    wxString GetVendorName();
    bool HasEntry(wxString& strName);
    bool HasGroup(const wxString& strName);
    bool IsExpandingEnvVars();
    bool IsRecordingDefaults();

    wxString Read(const wxString& key, const wxString& defaultVal = wxPyEmptyStr);
    %name(ReadInt)long Read(const wxString& key, long defaultVal = 0);
    %name(ReadFloat)double Read(const wxString& key, double defaultVal = 0.0);

    void SetExpandEnvVars (bool bDoIt = TRUE);
    void SetPath(const wxString& strPath);
    void SetRecordDefaults(bool bDoIt = TRUE);

    bool Write(const wxString& key, const wxString& value);
    %name(WriteInt)bool Write(const wxString& key, long value);
    %name(WriteFloat)bool Write(const wxString& key, double value);

};


//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.4  1999/04/30 03:29:19  RD
// wxPython 2.0b9, first phase (win32)
// Added gobs of stuff, see wxPython/README.txt for details
//
// Revision 1.3.4.1  1999/03/27 23:29:15  RD
//
// wxPython 2.0b8
//     Python thread support
//     various minor additions
//     various minor fixes
//
// Revision 1.3  1999/02/25 07:08:36  RD
//
// wxPython version 2.0b5
//
// Revision 1.2  1999/02/20 09:03:02  RD
// Added wxWindow_FromHWND(hWnd) for wxMSW to construct a wxWindow from a
// window handle.  If you can get the window handle into the python code,
// it should just work...  More news on this later.
//
// Added wxImageList, wxToolTip.
//
// Re-enabled wxConfig.DeleteAll() since it is reportedly fixed for the
// wxRegConfig class.
//
// As usual, some bug fixes, tweaks, etc.
//
// Revision 1.1  1998/12/15 20:41:23  RD
// Changed the import semantics from "from wxPython import *" to "from
// wxPython.wx import *"  This is for people who are worried about
// namespace pollution, they can use "from wxPython import wx" and then
// prefix all the wxPython identifiers with "wx."
//
// Added wxTaskbarIcon for wxMSW.
//
// Made the events work for wxGrid.
//
// Added wxConfig.
//
// Added wxMiniFrame for wxGTK, (untested.)
//
// Changed many of the args and return values that were pointers to gdi
// objects to references to reflect changes in the wxWindows API.
//
// Other assorted fixes and additions.
//
