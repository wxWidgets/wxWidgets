/////////////////////////////////////////////////////////////////////////////
// Name:        dialoged.cpp
// Purpose:     Main Dialog Editor implementation file
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dialoged.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "dialoged.h"

IMPLEMENT_APP(MyApp)

MyApp::MyApp(void)
{
}

wxResourceManager *theResourceManager = NULL;

#include "symbtabl.h"

bool MyApp::OnInit(void)
{
    theResourceManager = new wxResourceManager;
    theResourceManager->Initialize();

    theResourceManager->ShowResourceEditor(TRUE);
  
    if (argc > 1)
        theResourceManager->Load(argv[1]);

    SetTopWindow(theResourceManager->GetEditorFrame());

    return TRUE;
}

int MyApp::OnExit(void)
{
    delete theResourceManager;
    theResourceManager = NULL;
    return 0;
}
