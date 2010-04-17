/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/app.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/app.h"
#include "wx/apptrait.h"
#include "wx/qt/converter.h"

IMPLEMENT_DYNAMIC_CLASS( wxApp, wxAppBase )

wxApp::wxApp()
{
}


wxApp::~wxApp()
{
    delete m_qtApplication;
}

bool wxApp::Initialize( int &argc, wxChar **argv )
{
    if ( !wxAppBase::Initialize( argc, argv ))
        return false;

    // Use the previously stored argc, argv to prevent crashing.
    // (See: http://bugreports.qt.nokia.com/browse/QTBUG-7551)

    m_qtApplication = new QApplication( this->argc, this->argv, QApplication::GuiClient );

    return true;
}

#if wxUSE_GUI

wxGUIAppTraits::wxGUIAppTraits()
{
}

wxGUIAppTraits::~wxGUIAppTraits()
{
}

#endif // wxUSE_GUI
