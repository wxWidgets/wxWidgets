/////////////////////////////////////////////////////////////////////////////
// Name:        utils/makegen/makegen.cpp
// Purpose:     a tool to generate the makefiles for samples
// Author:      Vadim Zeitlin
// Modified by:
// Created:     03.01.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

#if wxUSE_GUI
    #error "This is a console mode program and must be linked with wxBase."
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/string.h>
#include <wx/file.h>
#include <wx/ffile.h>
#include <wx/app.h>
#include <wx/log.h>
#include <wx/dir.h>
#include <wx/textfile.h>
#include <wx/datetime.h>

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

class MakeGenApp : public wxApp
{
public:
    virtual bool OnInit();

    virtual int OnRun();

private:
    void Usage();               // give the usage message

    bool GenerateMakefile(const wxString& filename);

    wxString m_sampleName,      // the name of the sample
             m_dirname,         // directory with the template files
             m_outputDir;       // directory to output files to
};

IMPLEMENT_APP(MakeGenApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MakeGenApp
// ----------------------------------------------------------------------------

void MakeGenApp::Usage()
{
    wxLogError(_T("Usage: %s [-o output_dir] sample_name"), argv[0]);
}

bool MakeGenApp::GenerateMakefile(const wxString& filename)
{
    wxTextFile fileIn(m_dirname + filename);
    if ( !fileIn.Open() )
    {
        wxLogError(_T("Makefile '%s' couldn't be generated."), filename.c_str());

        return FALSE;
    }

    wxFFile fileOut(m_outputDir + filename, "w");
    if ( !fileOut.IsOpened() )
    {
        wxLogError(_T("Makefile '%s' couldn't be generated."), filename.c_str());

        return FALSE;
    }

    size_t count = fileIn.GetLineCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxString line = fileIn[n];

        line.Replace(_T("#DATE"), wxDateTime::Now().FormatISODate());
        line.Replace(_T("#NAME"), m_sampleName);

        fileOut.Write(line + _T('\n'));
    }

    return TRUE;
}

bool MakeGenApp::OnInit()
{
    // parse the cmd line
    if ( (argc == 1) || (argc == 3) ||
         (argv[1][0] == _T('-') && argv[1][1] != _T('o')) ||
         (argc == 2 && argv[1][0] == _T('-')) )
    {
        Usage();

        return FALSE;
    }

    m_sampleName = argv[1];
    if ( m_sampleName[0u] == _T('-') )
    {
        m_outputDir = argv[2];
        if ( !wxEndsWithPathSeparator(m_outputDir) )
        {
            m_outputDir += _T('/');
        }

        m_sampleName = argv[3];
    }

    return TRUE;
}

int MakeGenApp::OnRun()
{
    m_dirname = wxGetenv(_T("MAKEGEN_PATH"));
    if ( !m_dirname )
    {
        m_dirname = wxGetCwd();
    }

    if ( !wxEndsWithPathSeparator(m_dirname) )
    {
        m_dirname += _T('/');
    }

    m_dirname += _T("templates");

    wxDir dir(m_dirname);

    m_dirname += _T('/');

    if ( !dir.IsOpened() )
    {
        wxLogError(_T("Aborting generating the makefiles."));

        return 1;
    }

    wxString filename;
    size_t n = 0;
    bool cont = dir.GetFirst(&filename, _T("?akefile.*"), wxDIR_FILES);
    while ( cont )
    {
        n++;

        if ( !GenerateMakefile(filename) )
        {
            wxLogError(_T("Error during makefile generation, aborting."));

            return 2;
        }

        cont = dir.GetNext(&filename);
    }

    if ( n )
    {
        wxLogVerbose(_T("Successfully generated %u makefiles in '%s'."),
                     n, m_outputDir.c_str());
    }
    else
    {
        wxLogWarning(_T("No makefiles found: either set MAKEGEN_PATH variable "
                        "or run the program from its directory"));
    }

    return 0;
}
