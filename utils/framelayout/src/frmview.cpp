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
 
#ifdef __GNUG__
#pragma implementation "frmview.h"
// #pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "frmview.h"
#include "wx/utils.h"

/***** Implementation for class wxFrameView *****/

BEGIN_EVENT_TABLE( wxFrameView, wxEvtHandler )

	EVT_IDLE( wxFrameView::OnIdle )

END_EVENT_TABLE()

void wxFrameView::OnIdle( wxIdleEvent& event)
{
	event.Skip();

	if ( mDoToolUpdates )
	{
		int o;
		++o;

		// TBD::
	}
}

/*** public methods ***/

wxFrameView::wxFrameView()

	: mpLayout( NULL ),
	  mpFrameMgr( NULL )
{}

wxFrameView::~wxFrameView()
{
	if ( mpLayout ) delete mpLayout;
}

wxFrame* wxFrameView::GetParentFrame()
{
	return mpFrameMgr->GetParentFrame();
}

wxWindow* wxFrameView::GetClientWindow()
{
	return mpFrameMgr->GetClientWindow();
}

void wxFrameView::Activate()
{
	mpFrameMgr->ActivateView( this );
}

void wxFrameView::Deactivate()
{
	mpFrameMgr->DeactivateCurrentView();
}

void wxFrameView::CreateLayout()
{
	mpLayout = new wxFrameLayout( GetParentFrame(), mpFrameMgr->GetClientWindow(), FALSE );
}

wxFrameLayout* wxFrameView::GetLayout()
{
	return mpLayout;
}

void wxFrameView::SetToolUpdates( bool doToolUpdates )
{
	mDoToolUpdates = doToolUpdates;
}

void wxFrameView::SetLayout( wxFrameLayout* pLayout )
{
	if ( mpLayout ) delete mpLayout;

	mpLayout = pLayout;
}

wxFrameManager& wxFrameView::GetFrameManager()
{
	return *mpFrameMgr;
}

void wxFrameView::RegisterMenu( const wxString& topMenuName )
{
	mTopMenus.Add( topMenuName );
}

#if 0

/***** Implementation for class wxFrameViewSerializer *****/

// NOTE:: currently "stipple" property of the brush is not serialized

class wxFrameViewSerializer : public wxEvtHandlerSerializer
{
	DECLARE_SERIALIZER_CLASS( wxFrameViewSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );
};

IMPLEMENT_SERIALIZER_CLASS( wxFrameView, 
							wxFrameViewSerializer,
							wxFrameViewSerializer::Serialize,
							NO_CLASS_INIT )

void wxFrameViewSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	// wxFrameViewSerializer is a kind of wxEvtHandler - peform serialization of
	// the base class first

	info.SerializeInherited( pObj, store );

	wxFrameView* pView = (wxFrameView*)pObj;

	store.XchgObjPtr( (wxObject**) &pView->mpFrameMgr );
	store.XchgObjPtr( (wxObject**) &pView->mpLayout );
	store.XchgBool  ( pView->mDoToolUpdates );

	// serialize members in derived classes

	pView->OnSerialize( store );
}

#endif

/***** Implementation for class wxFrameManager *****/

void wxFrameManager::DoSerialize( wxObjectStorage& store )
{
#if 0
	store.AddInitialRef( mpFrameWnd );
	store.AddInitialRef( this );
	if ( mpClientWnd ) store.AddInitialRef( mpClientWnd );

	store.XchgObj( (wxObject*) &mViews );
	store.XchgInt( mActiveViewNo );

	store.Finalize(); // finish serialization
#endif 
}

void wxFrameManager::DestroyViews()
{
	DeactivateCurrentView();

	wxNode* pNode = mViews.First();

	while( pNode )
	{
		delete (wxFrameView*)pNode->Data();

		pNode = pNode->Next();
	}

	if ( mActiveViewNo != -1 && GetParentFrame() )

		GetParentFrame()->SetNextHandler( NULL );
}

int wxFrameManager::GetViewNo( wxFrameView* pView )
{
	wxNode* pNode = mViews.First();
	int n = 0;

	while( pNode )
	{
		if ( (wxFrameView*)pNode->Data() == pView )

			return n;

		++n;
		pNode = pNode->Next();
	}

	return -1;
}

void wxFrameManager::EnableMenusForView( wxFrameView* pView, bool enable )
{
	wxMenuBar* pMenuBar = GetParentFrame()->GetMenuBar();
	int count = pMenuBar->GetMenuCount();

	if ( !pMenuBar ) return;

	wxStringListNode* pNode = pView->mTopMenus.GetFirst();

	while( pNode )
	{
		for( int i = 0; i != count; ++i )
		{
			if ( pMenuBar->GetMenu(i)->GetTitle() == pNode->GetData() )

				pMenuBar->EnableTop( i, enable );
		}

		pNode = pNode->GetNext();
	}
}

void wxFrameManager::SyncAllMenus()
{
	wxNode* pNode = mViews.First();
	int i = 0;

	while( pNode )
	{
		if ( i != mActiveViewNo )

			EnableMenusForView( (wxFrameView*)pNode->GetData(), FALSE );

		pNode = pNode->Next();
	}

	EnableMenusForView( GetView( mActiveViewNo ), TRUE );
}

/*** public methods ***/

wxFrameManager::wxFrameManager()

	: mpFrameWnd( NULL  ),
	  mActiveViewNo( -1 ),
	  mpClientWnd( NULL )
{
}

wxFrameManager::~wxFrameManager()
{
	SaveViewsNow();
	DestroyViews();
}

void wxFrameManager::Init( wxWindow* pMainFrame, const wxString& settingsFile )
{
	mSettingsFile = settingsFile;
	mpFrameWnd    = pMainFrame;

	wxNode* pNode = mViews.First();

	while( pNode )
	{
		wxFrameView* pView = (wxFrameView*)pNode->Data();

		pView->OnInit();
		pView->OnInitMenus();

		pNode = pNode->Next();
	}

	if ( !ReloadViews() )
	{
		// if loading of settings file failed (e.g. was not found), 
		// do recreation of items in each view

		pNode = mViews.First();

		while( pNode )
		{
			wxFrameView* pView = (wxFrameView*)pNode->Data();

			pView->OnRecreate();

			pNode = pNode->Next();
		}
	}

	if ( mActiveViewNo >= mViews.Number() )
	
		mActiveViewNo = -1;

	ActivateView( GetView( ( mActiveViewNo == -1 ) ? 0 : mActiveViewNo ) );

	SyncAllMenus();
}

void wxFrameManager::AddView( wxFrameView* pFrmView )
{
	mViews.Append( pFrmView );

	pFrmView->mpFrameMgr = this; // back ref.
}

void wxFrameManager::RemoveView( wxFrameView* pFrmView )
{
	// TBD::
	wxASSERT(0);
}

int wxFrameManager::GetActiveViewNo()
{
	return mActiveViewNo;
}

wxFrameView* wxFrameManager::GetActiveView()
{
	wxNode* pNode = mViews.Nth( mActiveViewNo );

	if ( pNode ) return (wxFrameView*)pNode->Data();
			else return NULL;
}

wxNode* wxFrameManager::GetActiveViewNode()
{
	return mViews.Nth( mActiveViewNo );
}

wxFrame* wxFrameManager::GetParentFrame()
{
	return ((wxFrame*)mpFrameWnd);
}

wxWindow* wxFrameManager::GetParentWindow()
{
	return mpFrameWnd;
}

wxFrameView* wxFrameManager::GetView( int viewNo )
{
	wxNode* pNode = mViews.Nth( viewNo );

	if ( pNode ) return (wxFrameView*)pNode->Data();
			else return NULL;
}

void wxFrameManager::ActivateView( int viewNo )
{
	ActivateView( GetView( viewNo ) );
}

void wxFrameManager::ActivateView( wxFrameView* pFrmView )
{
	DeactivateCurrentView();

	mActiveViewNo = GetViewNo( pFrmView );

	if ( pFrmView->mpLayout )

		pFrmView->mpLayout->Activate();

	// FIXME:: we would have used PushEventHandler(),
	//         but wxFrame bypasses attached handlers when
	//         handling wxCommand events!

	GetParentFrame()->PushEventHandler( pFrmView );

	EnableMenusForView( pFrmView, TRUE );
}

void wxFrameManager::SetClinetWindow( wxWindow* pFrameClient )
{
	if ( mpClientWnd ) mpClientWnd->Destroy();

	mpClientWnd = pFrameClient;
}

wxWindow* wxFrameManager::GetClientWindow()
{
	if ( !mpClientWnd )

		mpClientWnd = new wxWindow( GetParentFrame(), -1 );

	return mpClientWnd;
}

void wxFrameManager::DeactivateCurrentView()
{
	if ( mActiveViewNo == -1 ) return;

	wxFrameView* pView = GetActiveView();

	// FOR NOW::
	wxASSERT( GetParentFrame()->GetEventHandler() == pView );

	GetParentFrame()->PopEventHandler();

	if ( pView->mpLayout )

		pView->mpLayout->Deactivate();

	EnableMenusForView( pView, FALSE );
}

void wxFrameManager::SaveViewsNow()
{
#if 0
	if ( mSettingsFile == "" ) return;

    wxIOStreamWrapper stm;
    stm.CreateForOutput( mSettingsFile );

    mStore.SetDataStream( stm );
    DoSerialize( mStore );
#endif
}

bool wxFrameManager::ReloadViews()
{
	return FALSE;

#if 0
	if ( mSettingsFile == "" || !wxFileExists( mSettingsFile ) ) 
		
		return FALSE;

	DestroyViews();

    wxIOStreamWrapper stm;
    stm.CreateForInput( mSettingsFile );

    mStore.SetDataStream( stm );
    DoSerialize( mStore );
#endif

	return TRUE;
}

bool wxFrameManager::ViewsAreLoaded()
{
	return ( mViews.Number() != 0 );
}