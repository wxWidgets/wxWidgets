///////////////////////////////////////////////////////////////////////////////
// Name:        tests/testfile.h
// Purpose:     TestFile class
// Author:      Mike Wetherell
// Copyright:   (c) 2005 Mike Wetherell
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_TEMPFILE_H_
#define _WX_TESTS_TEMPFILE_H_

#include "wx/file.h"
#include "wx/filefn.h"
#include "wx/filename.h"
#include "wx/log.h"

#include <ostream>

// define stream inserter for wxFileName to use it in Catch assertions
inline std::ostream& operator<<(std::ostream& o, const wxFileName& fn)
{
    return o << fn.GetFullPath();
}

// ----------------------------------------------------------------------------
// TestFile: self deleting test file in temporary directory
// ----------------------------------------------------------------------------

class TestFile
{
public:
    // Ctor creates the file and initializes with the provided temporary data.
    explicit TestFile(const void* data = "Before", size_t size = 6)
    {
        wxFile file;
        m_name = wxFileName::CreateTempFileName(wxT("wxtest"), &file);
        file.Write(data, size);
    }

    ~TestFile() { if (wxFileExists(m_name)) wxRemoveFile(m_name); }
    wxString GetName() const { return m_name; }

private:
    wxString m_name;
};

// ----------------------------------------------------------------------------
// TempFile: just a self deleting file
// ----------------------------------------------------------------------------

class TempFile
{
public:
    explicit TempFile(const wxString& name = wxString()) : m_name(name) { }

    void Assign(const wxString& name) { m_name = name; }

    const wxString& GetName() const { return m_name; }

    ~TempFile()
    {
        if ( !m_name.empty() )
            wxRemoveFile(m_name);
    }

private:
    wxString m_name;

    wxDECLARE_NO_COPY_CLASS(TempFile);
};

// ----------------------------------------------------------------------------
// TempDir: self deleting directory.
//
// This helper creates a unique temporary directory and removes it recursively
// when it goes out of scope.
// ----------------------------------------------------------------------------

class TempDir
{
public:
    explicit TempDir(const wxString& prefix = wxT("wxtest"))
    {
        // Capture the error message, if any, to show it in RequireOk(), as
        // the errors are not shown by the test logger by default.
        wxLogBuffer logBuf;
        wxLog* const logOld = wxLog::SetActiveTarget(&logBuf);

        m_name = wxFileName::CreateTempDirName(prefix);

        if ( m_name.empty() )
        {
            m_error = logBuf.GetBuffer();
            m_error.Trim();
        }

        // Prevent the buffer contents from being shown when the old target is
        // restored, as this flushes the current one.
        logBuf.Clear();

        wxLog::SetActiveTarget(logOld);
    }

    ~TempDir() { Remove(); }

    bool IsOk() const
    {
        return !m_name.empty();
    }

    void RequireOk() const
    {
        if ( !IsOk() )
        {
            const wxString msg =
                m_error.empty() ? wxString("TempDir creation failed") : m_error;
            UNSCOPED_INFO(msg);
        }

        REQUIRE(IsOk());
    }

    const wxString& GetName() const { return m_name; }
    const wxString& GetError() const { return m_error; }

    bool Remove()
    {
        if ( m_name.empty() )
            return true;

        if ( !wxFileName::Rmdir(m_name, wxPATH_RMDIR_RECURSIVE) )
            return false;

        m_name.clear();
        return true;
    }

    void Dismiss() { m_name.clear(); }

private:
    wxString m_name;
    wxString m_error;

    wxDECLARE_NO_COPY_CLASS(TempDir);
};

#endif // _WX_TESTS_TEMPFILE_H_

