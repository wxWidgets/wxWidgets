/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/app.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/app.h"
#include "wx/apptrait.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include <QtCore/QStringList>
#include <QtWidgets/QApplication>

#ifdef _WIN32
    #include "wx/dynlib.h"
    #include <Shlwapi.h>
#endif // _WIN32

wxIMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler);

wxApp::wxApp()
{
    m_qtArgc = 0;

    WXAppConstructed();
}


wxApp::~wxApp()
{
    // Delete command line arguments
    for ( int i = 0; i < m_qtArgc; ++i )
    {
        free(m_qtArgv[i]);
    }
}

bool wxApp::Initialize( int &argc, wxChar **argv )
{
    if ( !wxAppBase::Initialize( argc, argv ))
        return false;

    wxConvCurrent = &wxConvUTF8;

    // (See: http://bugreports.qt.nokia.com/browse/QTBUG-7551)
    // Need to store argc, argv. The argc, argv from wxAppBase are
    // being initialized *after* Initialize();

    // TODO: Check whether new/strdup etc. can be replaced with std::vector<>.

    // Clone and store arguments
    m_qtArgv.reset(new char* [argc + 1]);
    for ( int i = 0; i < argc; i++ )
    {
        m_qtArgv[i] = wxStrdupA(wxConvUTF8.cWX2MB(argv[i]));
    }
    m_qtArgv[argc] = nullptr;
    m_qtArgc = argc;

    m_qtApplication.reset(new QApplication(m_qtArgc, m_qtArgv.get()));

    // Use the args returned by Qt as it may have deleted (processed) some of them
    // Using QApplication::arguments() forces argument processing
    QStringList qtArgs = m_qtApplication->arguments();
    if ( qtArgs.size() != argc )
    {
        /* As per Qt 4.6: Here, qtArgc and qtArgv have been modified and can
         * be used to replace our args (with Qt-flags removed). Also, they can be
         * deleted as they are internally kept by Qt in a list after calling arguments().
         * However, there isn't any guarantee of that in the docs, so we keep arguments
         * ourselves and only delete then after the QApplication is deleted */

        // Qt changed the arguments
        delete [] argv;
        argv = new wxChar *[qtArgs.size() + 1];
        for ( int i = 0; i < qtArgs.size(); i++ )
        {
            argv[i] = wxStrdupW( wxConvUTF8.cMB2WX( qtArgs[i].toUtf8().data() ) );
        }

        argc = m_qtApplication->arguments().size();
        argv[argc] = nullptr;
    }

    return true;
}

// Compatibility for MSW wxFileDialog/wxDirDialog implementations
#ifdef _WIN32

// ----------------------------------------------------------------------------
// system DLL versions
// ----------------------------------------------------------------------------

namespace
{

    // helper function: retrieve the DLL version by using DllGetVersion(), returns
    // 0 if the DLL doesn't export such function
    int CallDllGetVersion(wxDynamicLibrary& dll)
    {
        // now check if the function is available during run-time
        wxDYNLIB_FUNCTION(DLLGETVERSIONPROC, DllGetVersion, dll);
        if (!pfnDllGetVersion)
            return 0;

        DLLVERSIONINFO dvi;
        dvi.cbSize = sizeof(dvi);

        HRESULT hr = (*pfnDllGetVersion)(&dvi);
        if (FAILED(hr))
        {
            wxLogApiError(wxT("DllGetVersion"), hr);

            return 0;
        }

        return 100 * dvi.dwMajorVersion + dvi.dwMinorVersion;
    }

} // anonymous namespace

/* static */
int wxApp::GetComCtl32Version()
{
    // cache the result
    //
    // NB: this is MT-ok as in the worst case we'd compute s_verComCtl32 twice,
    //     but as its value should be the same both times it doesn't matter
    static int s_verComCtl32 = -1;

    if (s_verComCtl32 == -1)
    {
        // Test for Wine first because its comctl32.dll always returns 581 from
        // its DllGetVersion() even though it supports the functionality of
        // much later versions too.
        wxVersionInfo verWine;
        if (wxIsRunningUnderWine(&verWine))
        {
            // Not sure which version of Wine implements comctl32.dll v6
            // functionality, but 5 seems to have it already.
            if (verWine.GetMajor() >= 5)
                s_verComCtl32 = 610;
        }
    }

    if (s_verComCtl32 == -1)
    {
        // we're prepared to handle the errors
        wxLogNull noLog;

        // we don't want to load comctl32.dll, it should be already loaded but,
        // depending on the OS version and the presence of the manifest, it can
        // be either v5 or v6 and instead of trying to guess it just get the
        // handle of the already loaded version
        wxLoadedDLL dllComCtl32(wxT("comctl32.dll"));
        if (!dllComCtl32.IsLoaded())
        {
            s_verComCtl32 = 0;
            return 0;
        }

        // try DllGetVersion() for recent DLLs
        s_verComCtl32 = CallDllGetVersion(dllComCtl32);

        // if DllGetVersion() is unavailable either during compile or
        // run-time, try to guess the version otherwise
        if (!s_verComCtl32)
        {
            // InitCommonControlsEx is unique to 4.70 and later
            void* pfn = dllComCtl32.GetSymbol(wxT("InitCommonControlsEx"));
            if (!pfn)
            {
                // not found, must be 4.00
                s_verComCtl32 = 400;
            }
            else // 4.70+
            {
                // many symbols appeared in comctl32 4.71, could use any of
                // them except may be DllInstall()
                pfn = dllComCtl32.GetSymbol(wxT("InitializeFlatSB"));
                if (!pfn)
                {
                    // not found, must be 4.70
                    s_verComCtl32 = 470;
                }
                else
                {
                    // found, must be 4.71 or later
                    s_verComCtl32 = 471;
                }
            }
        }
    }

    return s_verComCtl32;
}

#endif // _WIN32
