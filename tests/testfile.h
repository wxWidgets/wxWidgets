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

#include <ostream>

// define stream inserter for wxFileName to use it in CPPUNIT_ASSERT_EQUAL()
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
        m_name = wxFileName::CreateTempFileName(prefix);
        if ( !m_name.empty() )
        {
            if ( !wxRemoveFile(m_name) || !wxMkdir(m_name) )
                m_name.clear();
        }
    }

    ~TempDir() { Remove(); }

    bool IsOk() const
    {
        return !m_name.empty();
    }

    const wxString& GetName() const { return m_name; }

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

    wxDECLARE_NO_COPY_CLASS(TempDir);
};

#endif // _WX_TESTS_TEMPFILE_H_

