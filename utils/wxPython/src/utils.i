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
    wxString wxPyEmptyStr("");
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

%init %{

    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

%}

//---------------------------------------------------------------------------
