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
#include "wx/utils.h"

#include <atomic>
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
    static unsigned long ReserveNameId()
    {
        // Keep each TempDir construction on a distinct name sequence.
        static std::atomic<unsigned long> s_nextId(0);
        return s_nextId.fetch_add(1) + 1;
    }

    static wxString GetCandidateName(const wxString& prefix,
                                     unsigned long nameId, unsigned int num)
    {
        wxString dir;
        wxString name;
        wxString ext;
        wxFileName::SplitPath(prefix, &dir, &name, &ext);

        if ( dir.empty() )
            dir = wxFileName::GetTempDir();

        if ( name.empty() )
            name = wxT("wxtest");

        if ( !ext.empty() )
            name << "." << ext;

        const wxString candidateName = wxString::Format(
            "%s%lx-%lx-%03u", name, wxGetProcessId(), nameId, num);
        return wxFileName(dir, candidateName).GetFullPath();
    }

    explicit TempDir(const wxString& prefix = wxT("wxtest"))
        : TempDir(prefix, ReserveNameId())
    {
    }

    TempDir(const wxString& prefix, unsigned long nameId)
    {
        // Make directory creation itself reserve the name.  Creating a temp
        // file and replacing it with a directory opens a race after deletion.
        static const unsigned int maxTries = 1000;
        for ( unsigned int n = 0; n < maxTries; ++n )
        {
            m_name = GetCandidateName(prefix, nameId, n);

            unsigned long err;
            {
                wxLogNull noLog;
                if ( wxMkdir(m_name) )
                {
                    m_error.clear();
                    return;
                }

                // Capture the failure before Exists() below can change it.
                err = wxSysErrorCode();
            }

            SetMkdirError(m_name, err);

            if ( wxFileName::Exists(m_name) )
                continue;

            m_name.clear();
            return;
        }

        m_name.clear();
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
    void SetMkdirError(const wxString& path, unsigned long err)
    {
        m_error = wxString::Format("wxMkdir failed for \"%s\": %s",
                                   path,
                                   wxSysErrorMsgStr(err));
    }

    wxString m_name;
    wxString m_error;

    wxDECLARE_NO_COPY_CLASS(TempDir);
};

#endif // _WX_TESTS_TEMPFILE_H_

