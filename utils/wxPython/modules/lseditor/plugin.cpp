/////////////////////////////////////////////////////////////////////////////
// Name:        wxsplbase.cpp
// Purpose:     General interfaces for all plug-ins in wxStudio
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     11/04/1999
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandars Gluchovas
// Licence:   	GNU General Public License wxWindows licence v2.0
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "plugin.h"

/***** Implementation for class wxStudioPluginManager *****/

void wxsPluginManager::RegisterMenuCommand( const wxString& itemName, 
											const wxString& menuName,
											int   id, 
											wxsPluginBase* forPlugin )
{
	// TBD::
}

void wxsPluginManager::UnregisterPlugin( wxsPluginBase* plugin )
{
	// TBD::
}

/***** Implementation for class wxStudioPluginBase *****/

wxsPluginBase::wxsPluginBase()
{}

wxsPluginBase::~wxsPluginBase()
{}


wxsPluginManager& wxsPluginBase::GetPluginManager()
{
	wxASSERT( mpPluginMgr );
	return *mpPluginMgr;
}

void wxsPluginBase::SetPluginManager( wxsPluginManager* pMgr )
{
	mpPluginMgr = pMgr;
}
