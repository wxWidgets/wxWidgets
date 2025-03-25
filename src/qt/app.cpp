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
#include "wx/private/init.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include <QtCore/QStringList>
#include <QtWidgets/QApplication>
#include <QSurfaceFormat>

#include <algorithm>
#include <vector>

wxIMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler);

wxApp::wxApp()
{
    WXAppConstructed();
}

wxApp::~wxApp()
{
}

bool wxApp::Initialize( int& argc_, wxChar** argv_ )
{
    if ( !wxAppBase::Initialize( argc_, argv_ ))
        return false;

    wxConvCurrent = &wxConvUTF8;

    // Use SingleBuffer mode by default to reduce latency.
    QSurfaceFormat format;
    format.setSwapBehavior(QSurfaceFormat::SwapBehavior::SingleBuffer);
    QSurfaceFormat::setDefaultFormat(format);

    const int argcA = argc_;
    auto argvA = wxInitData::Get().argvA;

#ifdef __WINDOWS__
    // The arguments in argvA are allocated dynamically under Windows and need to be freed.
    // See notice below.
    std::vector<char*> qtConsumedArgs;
    for ( int i = 0; i < argcA; ++i )
    {
        qtConsumedArgs.push_back(argvA[i]); // We only need shallow copies of the pointers
    }
#endif // __WINDOWS__

    // Quoting the Qt documentation (QApplication ctor):
    // -------------------------------------------------
    // Warning: The data referred to by argc and argv must stay valid for the entire lifetime
    // of the QApplication object. In addition, argc must be greater than zero and argv must
    // contain at least one valid character string.

    m_qtApplication.reset(new QApplication(argc_, argvA));

    if ( m_qtApplication->platformName() == "xcb" )
        m_qtApplication->processEvents(); // Avoids SIGPIPE on X11 when debugging

    if ( argcA != argc_ )
    {
#ifdef __WINDOWS__
        // Notice that Qt removes command line arguments (from argvA) that it recognizes without
        // deleting them, so we need to free them here ourselves to avoid memory leaks.
        for ( int i = 0; i < argc_; ++i )
        {
            qtConsumedArgs.erase(std::remove(qtConsumedArgs.begin(), qtConsumedArgs.end(), argvA[i]),
                                 qtConsumedArgs.end());
        }

        for ( auto consumedArg : qtConsumedArgs )
        {
            free(consumedArg);
        }
#endif // __WINDOWS__

        // argc_ is already updated by Qt
        argv_[argc_] = nullptr;
    }
    //else: Qt didn't modify our parameters

    return true;
}
