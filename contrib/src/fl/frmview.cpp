/////////////////////////////////////////////////////////////////////////////
// Name:        frmview.cpp
// Purpose:     wxFrameView implementation. NOT USED IN FL.
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     02/01/99
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/fl/frmview.h"
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
    mpLayout = new wxFrameLayout( GetParentFrame(), mpFrameMgr->GetClientWindow(), false );
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

void wxFrameManager::DoSerialize( wxObjectStorage& WXUNUSED(store) )
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

    wxObjectList::compatibility_iterator pNode = mViews.GetFirst();

    while ( pNode )
    {
        delete (wxFrameView*)pNode->GetData();

        pNode = pNode->GetNext();
    }

    if ( mActiveViewNo != -1 && GetParentFrame() )

        GetParentFrame()->SetNextHandler( NULL );
}

int wxFrameManager::GetViewNo( wxFrameView* pView )
{
    wxObjectList::compatibility_iterator pNode = mViews.GetFirst();
    int n = 0;

    while ( pNode )
    {
        if ( (wxFrameView*)pNode->GetData() == pView )

            return n;

        ++n;
        pNode = pNode->GetNext();
    }

    return -1;
}

void wxFrameManager::EnableMenusForView( wxFrameView* pView, bool enable )
{
    wxMenuBar* pMenuBar = GetParentFrame()->GetMenuBar();

    if ( !pMenuBar )
        return;

    int count = pMenuBar->GetMenuCount();



    wxStringList::compatibility_iterator pNode = pView->mTopMenus.GetFirst();

    int i;
    while ( pNode )
    {
        for ( i = 0; i != count; ++i )
        {
            if ( pMenuBar->GetMenu(i)->GetTitle() == pNode->GetData() )
                pMenuBar->EnableTop( i, enable );
        }

        pNode = pNode->GetNext();
    }
}

void wxFrameManager::SyncAllMenus()
{
    wxObjectList::compatibility_iterator pNode = mViews.GetFirst();
    int i = 0;

    while ( pNode )
    {
        if ( i != mActiveViewNo )

            EnableMenusForView( (wxFrameView*)pNode->GetData(), false );

        pNode = pNode->GetNext();
    }

    EnableMenusForView( GetView( mActiveViewNo ), true );
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

    wxObjectList::compatibility_iterator pNode = mViews.GetFirst();

    while ( pNode )
    {
        wxFrameView* pView = (wxFrameView*)pNode->GetData();

        pView->OnInit();
        pView->OnInitMenus();

        pNode = pNode->GetNext();
    }

    if ( !ReloadViews() )
    {
        // if loading of settings file failed (e.g. was not found), 
        // do recreation of items in each view

        pNode = mViews.GetFirst();

        while ( pNode )
        {
            wxFrameView* pView = (wxFrameView*)pNode->GetData();

            pView->OnRecreate();

            pNode = pNode->GetNext();
        }
    }

    if ( mActiveViewNo >= (int)mViews.GetCount() )
        mActiveViewNo = -1;

    ActivateView( GetView( ( mActiveViewNo == -1 ) ? 0 : mActiveViewNo ) );

    SyncAllMenus();
}

void wxFrameManager::AddView( wxFrameView* pFrmView )
{
    mViews.Append( pFrmView );

    pFrmView->mpFrameMgr = this; // back ref.
}

void wxFrameManager::RemoveView( wxFrameView* WXUNUSED(pFrmView) )
{
    // TBD::
    wxFAIL_MSG( _T("wxFrameManager::RemoveView() has not been implemented yet.") );
}

int wxFrameManager::GetActiveViewNo()
{
    return mActiveViewNo;
}

wxFrameView* wxFrameManager::GetActiveView()
{
    wxObjectList::compatibility_iterator pNode = mViews.Item( mActiveViewNo );

    if ( pNode ) return (wxFrameView*)pNode->GetData();
            else return NULL;
}

wxObjectList::compatibility_iterator wxFrameManager::GetActiveViewNode()
{
    return mViews.Item( mActiveViewNo );
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
    wxObjectList::compatibility_iterator pNode = mViews.Item( viewNo );

    if ( pNode ) return (wxFrameView*)pNode->GetData();
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

    EnableMenusForView( pFrmView, true );
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
    if ( mActiveViewNo == -1 )
        return;

    wxFrameView* pView = GetActiveView();

    // FOR NOW::
    wxASSERT( GetParentFrame()->GetEventHandler() == pView );

    GetParentFrame()->PopEventHandler();

    if ( pView->mpLayout )
        pView->mpLayout->Deactivate();

    EnableMenusForView( pView, false );
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
    return false;

    // TBD: ????
#if 0
    if ( mSettingsFile == "" || !wxFileExists( mSettingsFile ) ) 
        return false;

    DestroyViews();

    wxIOStreamWrapper stm;
    stm.CreateForInput( mSettingsFile );

    mStore.SetDataStream( stm );
    DoSerialize( mStore );

    return true;
#endif
}

bool wxFrameManager::ViewsAreLoaded()
{
    return ( mViews.GetCount() != 0 );
}

