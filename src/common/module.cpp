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

IMPLEMENT_CLASS(wxModule, wxObject)

wxList wxModule::m_modules;

void wxModule::RegisterModule(wxModule* module)
{
  m_modules.Append(module);
}

// Collect up all module-derived classes, create an instance of each,
// and register them.
bool wxModule::RegisterModules(void)
{
    wxClassInfo* classInfo = wxClassInfo::first;
    while (classInfo)
    {
        if ((classInfo != (& (wxModule::classwxModule))) &&
            classInfo->IsKindOf(CLASSINFO(wxModule)))
        {
            wxModule* module = (wxModule*) classInfo->CreateObject();
            RegisterModule(module);
        }
        classInfo = classInfo->next;
    }
    return TRUE;
}

bool wxModule::InitializeModules(void)
{
  // Initialize user-defined modules
    for (wxNode *node = m_modules.First(); node; node = node->Next())
    {
      if (!((wxModule*)(node->Data()))->Init())
        return FALSE;
    }
    return TRUE;
}

void wxModule::CleanUpModules(void)
{
  // Cleanup user-defined modules
    for(wxNode* node = m_modules.Last(); node; node = node->Previous())
    {
      ((wxModule*)(node->Data()))->Exit();
      delete (wxModule*)(node->Data());
    }
    m_modules.Clear();
}

