/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     02/01/99
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __FRMVIEW_G__
#define __FRMVIEW_G__

#include "wx/module.h"

#if 0
#include "objstore.h"
#endif

class wxObjectStorage;

#include "controlbar.h"

class wxFrameManager;

class wxFrameView : public wxEvtHandler
{
protected:
	wxStringList    mTopMenus;
	wxFrameLayout*  mpLayout;
	wxFrameManager* mpFrameMgr;
	bool            mDoToolUpdates;

	friend class wxFrameManager;
	friend class wxFrameViewSerializer;	

protected:
	void OnIdle( wxIdleEvent& event);
	
public:
	wxFrameView();
	~wxFrameView();

	virtual void Activate();
	virtual void Deactivate();

	wxFrame* GetParentFrame();
	wxWindow* GetClientWindow();

	wxFrameManager& GetFrameManager();

	void RegisterMenu( const wxString& topMenuName );

	void CreateLayout();
	wxFrameLayout* GetLayout();
	void SetLayout( wxFrameLayout* pLayout );
	void SetToolUpdates( bool doToolUpdates = TRUE );


	// hooks for specific frame-views

	virtual void OnInit() {}

	virtual void OnSerialize( wxObjectStorage& store ) {}
	virtual void OnActiveate() {}
	virtual void OnDeactivate() {}

	// imp. is mandatory
	virtual void OnRecreate() {}
	virtual void OnInitMenus() {}

	DECLARE_EVENT_TABLE()
};

class wxFrame;

class wxFrameManager : wxObject
{
protected:
	wxList       mViews;
	wxWindow*    mpFrameWnd;
	int          mActiveViewNo;
	wxWindow*    mpClientWnd;

#if 0
	wxObjectStorage mStore;
#endif

	wxString        mSettingsFile;

protected:
	void DoSerialize( wxObjectStorage& store );
	void DestroyViews();
	int GetViewNo( wxFrameView* pView );
	void EnableMenusForView( wxFrameView* pView, bool enable );
	void SyncAllMenus();

public:
	wxFrameManager();
	~wxFrameManager();

	// if file name is empty, views are are not saved/loaded

	virtual void Init( wxWindow* pMainFrame, const wxString& settingsFile = "" );

	// synonyms
	wxFrame* GetParentFrame();
	wxWindow* GetParentWindow();

	int GetActiveViewNo();
	wxFrameView* GetActiveView();
	wxNode* GetActiveViewNode();

	wxFrameView* GetView( int viewNo );

	void SetClinetWindow( wxWindow* pFrameClient ); 
	wxWindow* GetClientWindow();

	void AddView( wxFrameView* pFrmView );
	void RemoveView( wxFrameView* pFrmView );

	void ActivateView( int viewNo );
	void ActivateView( wxFrameView* pFrmView );
	void DeactivateCurrentView();

	wxObjectStorage& GetObjectStore();

	void SaveViewsNow();
	bool ReloadViews();

	bool ViewsAreLoaded();
};

#endif