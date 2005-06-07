/////////////////////////////////////////////////////////////////////////////
// Name:        module.cpp
// Purpose:     Modules initialization/destruction
// Author:      Wolfram Gloger/adapted by Guilhem Lavaux
// Modified by:
// Created:     04/11/98
// RCS-ID:      $Id$
// Copyright:   (c) Wolfram Gloger and Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "module.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/module.h"
#include "wx/hash.h"
#include "wx/intl.h"
#include "wx/log.h"
#include "wx/listimpl.cpp"

WX_DEFINE_LIST(wxModuleList);

IMPLEMENT_CLASS(wxModule, wxObject)

wxModuleList wxModule::m_modules;

void wxModule::RegisterModule(wxModule* module)
{
    m_modules.Append(module);
}

void wxModule::UnregisterModule(wxModule* module)
{
    m_modules.DeleteObject(module);
    delete module;
}

// Collect up all module-derived classes, create an instance of each,
// and register them.
void wxModule::RegisterModules()
{
    wxHashTable::compatibility_iterator node;
    wxClassInfo* classInfo;

    wxClassInfo::sm_classTable->BeginFind();
    node = wxClassInfo::sm_classTable->Next();
    while (node)
    {
        classInfo = (wxClassInfo *)node->GetData();
        if ( classInfo->IsKindOf(CLASSINFO(wxModule)) &&
            (classInfo != (& (wxModule::ms_classInfo))) )
        {
            wxModule* module = (wxModule *)classInfo->CreateObject();
            RegisterModule(module);
        }
        node = wxClassInfo::sm_classTable->Next();
    }
}

bool wxModule::InitializeModules()
{
    // Initialize user-defined modules
    wxModuleList::compatibility_iterator node;
    for ( node = m_modules.GetFirst(); node; node = node->GetNext() )
    {
        wxModule *module = node->GetData();
        if ( !module->Init() )
        {
            wxLogError(_("Module \"%s\" initialization failed"),
                       module->GetClassInfo()->GetClassName());

            // clean up already initialized modules - process in reverse order
            wxModuleList::compatibility_iterator n;
            for ( n = node->GetPrevious(); n; n = n->GetPrevious() )
            {
                n->GetData()->OnExit();
            }

            return false;
        }
    }

    return true;
}

void wxModule::CleanUpModules()
{
    // Cleanup user-defined modules
    wxModuleList::compatibility_iterator node;
    for ( node = m_modules.GetFirst(); node; node = node->GetNext() )
    {
        node->GetData()->Exit();
    }

    WX_CLEAR_LIST(wxModuleList, m_modules);
}

