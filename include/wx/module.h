/////////////////////////////////////////////////////////////////////////////
// Name:        module.h
// Purpose:     Modules handling
// Author:      Wolfram Gloger/adapted by Guilhem Lavaux
// Modified by:
// Created:     04/11/98
// RCS-ID:      $Id$
// Copyright:   (c) Wolfram Gloger and Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MODULEH__
#define _WX_MODULEH__

#ifdef __GNUG__
#pragma interface "module.h"
#endif

#include "wx/object.h"
#include "wx/list.h"
#include "wx/setup.h"

class WXDLLEXPORT wxModule: public wxObject
{
public:
    wxModule(void) {}
    ~wxModule(void) {}

    // If returns FALSE, quits the application immediately.
    bool Init(void) { return OnInit(); }
    void Exit(void) { OnExit(); }

    // Override both of these
    virtual bool OnInit(void) = 0;
    virtual void OnExit(void) = 0;

    static void RegisterModule(wxModule* module);
    static bool RegisterModules(void);
    static bool InitializeModules(void);
    static void CleanUpModules(void);

protected:
    static wxList   m_modules;

DECLARE_CLASS(wxModule)
};

#endif

