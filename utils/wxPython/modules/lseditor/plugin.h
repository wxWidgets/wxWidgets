/////////////////////////////////////////////////////////////////////////////
// Name:        wxsplbase.h
// Purpose:     General interfaces for all plug-ins in wxStudio
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     11/04/1999
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:   	GNU General Public License wxWindows licence v2.0
/////////////////////////////////////////////////////////////////////////////

#ifndef __PLUGIN_G__
#define __PLUGIN_G__

#include "wxstldefs.h"
#include "wxsdefs.h"

class wxsPluginBase;
typedef  wxsPluginBase* wxsPluginBasePtrT;
#ifdef wxUSE_TEMPLATE_STL
typedef vector<wxsPluginBasePtrT> wxsPluginListT;
#else
typedef WXSTL_VECTOR_SHALLOW_COPY(wxsPluginBasePtrT) wxsPluginListT;
#endif


class wxsPluginManager : public wxObject
{
public:
    
    wxsPluginListT& GetPlugins();

	// allows to present plugin-specific features
	// as items in the menu-bar

	void RegisterMenuCommand( const string& itemName, 
							  const string& menuName,
							  int   id, 
	                          wxsPluginBase* forPlugin );

	// should be called by plugin, when it's being destroyed

	void UnregisterPlugin( wxsPluginBase* plugin );
};



// Used by create settings panel:
enum {
    WXS_SETTINGS_GLOBAL,
    WXS_SETTINGS_PROJECT
};


class wxsPluginBase : public wxObject
{
protected:
	wxsPluginManager* mpPluginMgr;

public:

	wxsPluginBase();
	virtual ~wxsPluginBase();

	virtual void InitPlugin() {}

	// utilities

	wxsPluginManager& GetPluginManager();
	void SetPluginManager( wxsPluginManager* mgr );

	// overridables

    // Current Types = UNKNOWN,EDITOR,CLASSBROWSER,FILEBROWSER,CLASSINFO,TOOL
	virtual WXS_PLUGIN_TYPE GetType() = 0;
    virtual string GetCategory() = 0;
    
    virtual string GetName() = 0;
    // will return a help panel
    virtual wxWindow* CreateSettingsPanel(wxWindow *parent, int type) {return NULL;}
	virtual wxsPluginBase* Clone() = 0;

	virtual string Command( const string& name, const string& args ) 
		
		{ return "NO_SUPPORTED"; }
};

// base clas for all plugins which are presented as windows

class wxsComponent : public wxsPluginBase 
{
public:
	virtual void Create( wxWindow* parent, wxWindowID id ) = 0;
	virtual wxWindow* GetWindow() = 0;
};

#endif 
	  // __PLUGIN_G__
