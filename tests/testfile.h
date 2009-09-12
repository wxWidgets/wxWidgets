///////////////////////////////////////////////////////////////////////////////
// Name:        tests/testfile.h
// Purpose:     TestFile class
// Author:      Mike Wetherell
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Mike Wetherell
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_TEMPFILE_H_
#define _WX_TESTS_TEMPFILE_H_

#include "wx/filefn.h"
#include "wx/filename.h"

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

#endif // _WX_TESTS_TEMPFILE_H_

