///////////////////////////////////////////////////////////////////////////////
// Name:        tests/testfile.h
// Purpose:     TestFile class
// Author:      Mike Wetherell
// Copyright:   (c) 2005 Mike Wetherell
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_TEMPFILE_H_
#define _WX_TESTS_TEMPFILE_H_

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
    TestFile()
    {
        wxFile file;
        m_name = wxFileName::CreateTempFileName(wxT("wxtest"), &file);
        file.Write("Before", 6);
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

#endif // _WX_TESTS_TEMPFILE_H_

