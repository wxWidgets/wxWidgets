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

#ifdef __GNUG__
#pragma implementation "module.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/module.h"
#include "wx/hash.h"
#include "wx/listimpl.cpp"

#ifdef __SALFORDC__
void wxwxModuleListNode::DeleteData()
{
    delete (_WX_LIST_ITEM_TYPE_wxModuleList *)GetData();
}
#else
WX_DEFINE_LIST(wxModuleList);
#endif

IMPLEMENT_CLASS(wxModule, wxObject)

wxModuleList wxModule::m_modules;

void wxModule::RegisterModule(wxModule* module)
{
    m_modules.Append(module);
}

// Collect up all module-derived classes, create an instance of each,
// and register them.
void wxModule::RegisterModules()
{
    wxNode *node;
    wxClassInfo* classInfo;

    wxClassInfo::sm_classTable->BeginFind();
    node = wxClassInfo::sm_classTable->Next();
    while (node)
    {
        classInfo = (wxClassInfo *)node->Data();
        if ( classInfo->IsKindOf(CLASSINFO(wxModule)) &&
            (classInfo != (& (wxModule::sm_classwxModule))) )
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
    wxModuleList::Node *node;
    for ( node = m_modules.GetFirst(); node; node = node->GetNext() )
    {
        if ( !node->GetData()->Init() )
        {
            // clean up already initialized modules - process in reverse order
            wxModuleList::Node *n;
            for ( n = node->GetPrevious(); n; n = n->GetPrevious() )
            {
                n->GetData()->OnExit();
            }

            return FALSE;
        }
    }

    return TRUE;
}

void wxModule::CleanUpModules()
{
    // Cleanup user-defined modules
    wxModuleList::Node *node;
    for ( node = m_modules.GetFirst(); node; node = node->GetNext() )
    {
        node->GetData()->Exit();
    }

    m_modules.DeleteContents(TRUE);
    m_modules.Clear();
}

