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

/*
 * TODO
 *
 *  1. support for programs with multiple object files
 *  2. support for programs under utils and demos, not only samples
 */

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
#include <wx/cmdline.h>

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

class MakeGenApp : public wxApp
{
public:
    MakeGenApp() { m_quiet = FALSE; }

    virtual bool OnInit();

    virtual int OnRun();

private:
    bool GenerateMakefile(const wxString& filename);

    wxString m_progname,        // the name of the sample
             m_dirname,         // directory with the template files
             m_outdir;          // directory to output files to

    bool m_quiet;               // don't give non essential messages
};

IMPLEMENT_APP(MakeGenApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MakeGenApp
// ----------------------------------------------------------------------------

bool MakeGenApp::GenerateMakefile(const wxString& filename)
{
    wxTextFile fileIn(m_dirname + filename);
    if ( !fileIn.Open() )
    {
        wxLogError(_T("Makefile '%s' couldn't be generated."), filename.c_str());

        return FALSE;
    }

    wxString fileOutName;
    fileOutName << m_outdir << _T('/') << filename;
    wxFFile fileOut(fileOutName, "w");
    if ( !fileOut.IsOpened() )
    {
        wxLogError(_T("Makefile '%s' couldn't be generated."), filename.c_str());

        return FALSE;
    }

    wxLogVerbose(_T("Generating '%s' for '%s'..."),
                 fileOutName.c_str(), m_progname.c_str());

    size_t count = fileIn.GetLineCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxString line = fileIn[n];

        line.Replace(_T("#DATE"), wxDateTime::Now().FormatISODate());
        line.Replace(_T("#NAME"), m_progname);

        fileOut.Write(line + _T('\n'));
    }

    return TRUE;
}

// parse the cmd line
bool MakeGenApp::OnInit()
{
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, _T("h"), _T("help"),    _T("give this usage message"),
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, _T("v"), _T("verbose"), _T("be more verbose") },
        { wxCMD_LINE_SWITCH, _T("q"), _T("quiet"),   _T("be quiet") },

        { wxCMD_LINE_OPTION, _T("i"), _T("input"),   _T("directory with template files") },

        { wxCMD_LINE_PARAM,  NULL, NULL,     _T("output_directory") },

        { wxCMD_LINE_NONE }
    };

    wxCmdLineParser parser(cmdLineDesc, argc, argv);
    parser.SetLogo(_T("MakeGen: a makefile generator for wxWindows\n"
                      "Copyright (c) 2000 Vadim Zeitlin"));

    if ( parser.Parse() != 0 )
    {
        // failed to parse the cmd line or help was requested (and given)
        return FALSE;
    }

    (void)parser.Found(_T("i"), &m_dirname);
    if ( parser.Found(_T("q")) )
    {
        m_quiet = TRUE;

        wxLog::GetActiveTarget()->SetVerbose(FALSE);
    }
    else if ( parser.Found(_T("v")) )
    {
        wxLog::GetActiveTarget()->SetVerbose();
    }

    m_outdir = parser.GetParam();

#ifdef __WINDOWS__
    m_outdir.Replace(_T("\\"), _T("/"));
#endif

    if ( !!m_outdir && m_outdir.Last() == _T('/') )
    {
        m_outdir.Truncate(m_outdir.length() - 1);
    }

    m_progname = m_outdir.AfterLast(_T('/'));

    if ( !m_progname )
    {
        wxLogError(_T("Output directory should be specified."));

        parser.Usage();

        return FALSE;
    }

    return TRUE;
}

int MakeGenApp::OnRun()
{
    if ( !m_dirname )
    {
        m_dirname = wxGetenv(_T("MAKEGEN_PATH"));
        if ( !m_dirname )
        {
            m_dirname = wxGetCwd();
        }
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
    bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
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
                     n, m_outdir.c_str());
    }
    else if ( !m_quiet )
    {
        wxLogWarning(_T("No makefiles found: either set MAKEGEN_PATH variable "
                        "or run the program from its directory"));
    }

    return 0;
}
