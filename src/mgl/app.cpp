/////////////////////////////////////////////////////////////////////////////
// Name:        app.cpp
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "app.h"
#endif

#include "wx/app.h"
#include "wx/settings.h"
#include "wx/module.h"

#include <mgraph.hpp>

//-----------------------------------------------------------------------------
// Global data
//-----------------------------------------------------------------------------

// MGL's display DC:
MGLDevCtx *g_displayDC = NULL;

// ----

wxApp *wxTheApp = (wxApp *)  NULL;
wxAppInitializerFunction wxAppBase::m_appInitFn = (wxAppInitializerFunction) NULL;

// FIXME_MGL - whole file


extern bool g_isIdle;

bool g_mainThreadLocked = FALSE;

void wxExit()
{
    exit(0);
}

//-----------------------------------------------------------------------------
// wxYield
//-----------------------------------------------------------------------------

bool wxYield()
{
    return TRUE;
}

//-----------------------------------------------------------------------------
// wxWakeUpIdle
//-----------------------------------------------------------------------------

void wxWakeUpIdle()
{
}

//-----------------------------------------------------------------------------
// wxApp
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxApp,wxEvtHandler)

BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxApp::OnIdle)
END_EVENT_TABLE()


int wxEntry( int argc, char *argv[] )
{
    return 0;
}


// FIXME_MGL - this is temporary solution, will be removed
// once I have wxApp up and running
bool wxMGL_Initialize()
{
    wxBuffer = new wxChar[BUFSIZ + 512];

    wxClassInfo::InitializeClasses();
    wxSystemSettings::Init();
    wxTheColourDatabase = new wxColourDatabase( wxKEY_STRING );
    wxTheColourDatabase->Initialize();
    wxInitializeStockLists();
    wxInitializeStockObjects();
    wxModule::RegisterModules();
    if (!wxModule::InitializeModules()) return FALSE;
    return TRUE;
}

