/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     27/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "cbstore.h"
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

#include "cbstore.h"

/***** Implementation for class wxFrameLayoutSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( wxFrameLayout, 
							wxFrameLayoutSerializer,
							wxFrameLayoutSerializer::Serialize,
							wxFrameLayoutSerializer::Initialize )

void wxFrameLayoutSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	// wxFrameLayout is a "kind of" wxEvtHandler - perform
	// serialization of the base class first

	info.SerializeInherited( pObj, store );

	wxFrameLayout* pLayout = (wxFrameLayout*)pObj;

	store.XchgObjPtr( (wxObject**) &pLayout->mpFrame       );
	store.XchgObjPtr( (wxObject**) &pLayout->mpFrameClient );

	for( int i = 0; i != MAX_PANES; ++i )
	
		store.XchgObjPtr( (wxObject**) &(pLayout->mPanes[i]) );

	// plugins are serialized _after_ panes

	store.XchgObjPtr( (wxObject**) &(pLayout->mpTopPlugin) );

	// and the rest will follow...

	store.XchgObjArray( pLayout->mAllBars );

	store.XchgObjList( pLayout->mBarSpyList );

	store.XchgObjList( pLayout->mFloatedFrames );

	store.XchgObjPtr( (wxObject**) &(pLayout->mpUpdatesMgr) );

	store.XchgBool( pLayout->mFloatingOn );

	store.XchgWxPoint( pLayout->mNextFloatedWndPos );

	store.XchgWxSize( pLayout->mFloatingPosStep );

	store.XchgObj( (wxObject*) &pLayout->mDarkPen   );
	store.XchgObj( (wxObject*) &pLayout->mLightPen  );
	store.XchgObj( (wxObject*) &pLayout->mGrayPen   );
	store.XchgObj( (wxObject*) &pLayout->mBlackPen  );
	store.XchgObj( (wxObject*) &pLayout->mBorderPen );
}

void wxFrameLayoutSerializer::Initialize( wxObject* pObj )
{
	wxFrameLayout* pLayout = (wxFrameLayout*)pObj;

	// wxFrameLayout is a "kind of" wxEvtHandler - perform
	// wxEvtHandler-specific initialization first

	info.InitializeInherited( pObj );

	//pLayout->RecalcLayout( TRUE );
}

/***** Implementation for class wxFrameLayoutSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( cbBarSpy, 
							cbBarSpySerializer,
							cbBarSpySerializer::Serialize,
							cbBarSpySerializer::Initialize )

void cbBarSpySerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	// cbBarSpy is a "kind of" wxEvtHandler - perform
	// serialization of the base class first

	info.SerializeInherited( pObj, store );

	cbBarSpy* pSpy = (cbBarSpy*)pObj;

	store.XchgObjPtr( (wxObject**) &(pSpy->mpLayout) );
	store.XchgObjPtr( (wxObject**) &(pSpy->mpBarWnd) );
}

void cbBarSpySerializer::Initialize( wxObject* pObj )
{
	// cbBarSpySerializer is a "kind of" wxEvtHandler - perform
	// wxEvtHandler-specific initialization first

	info.InitializeInherited( pObj );

	cbBarSpy* pSpy = (cbBarSpy*)pObj;

	// is done by wxEventHandler's serializer already!

	//pSpy->mpBarWnd->PushEventHandler( pSpy );
}

/***** Implementation for class cbBarDimHandlerBaseSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( cbBarDimHandlerBase, 
							cbBarDimHandlerBaseSerializer,
							cbBarDimHandlerBaseSerializer::Serialize,
							NO_CLASS_INIT )

void cbBarDimHandlerBaseSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	cbBarDimHandlerBase* pHandler = (cbBarDimHandlerBase*)pObj;

	store.XchgInt( pHandler->mRefCount );
}

/***** Implementation for class cbDimInfoSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( cbDimInfo, 
							cbDimInfoSerializer,
							cbDimInfoSerializer::Serialize,
							NO_CLASS_INIT )

void cbDimInfoSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	cbDimInfo* pInfo = (cbDimInfo*)pObj;

	int i = 0;

	for( i = 0; i != MAX_BAR_STATES; ++i )
	
		store.XchgWxSize( pInfo->mSizes[i] );

	for( i = 0; i != MAX_BAR_STATES; ++i )
	
		store.XchgWxRect( pInfo->mBounds[i] );

	store.XchgInt   ( pInfo->mLRUPane  );
	store.XchgInt   ( pInfo->mHorizGap );
	store.XchgInt   ( pInfo->mVertGap  );
	
	store.XchgBool  ( pInfo->mIsFixed );
	store.XchgObjPtr( (wxObject**) &(pInfo->mpHandler) );
}

/***** Implementation for class cbRowInfoSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( cbRowInfo, 
							cbRowInfoSerializer,
							cbRowInfoSerializer::Serialize,
							NO_CLASS_INIT )

void cbRowInfoSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	cbRowInfo* pInfo = (cbRowInfo*)pObj;

	store.XchgObjArray( pInfo->mBars );

	store.XchgLongArray( pInfo->mSavedRatios );

	store.XchgObjPtr( (wxObject**) &pInfo->mpNext );
	store.XchgObjPtr( (wxObject**) &pInfo->mpPrev );
	store.XchgObjPtr( (wxObject**) &pInfo->mpExpandedBar );

	store.XchgBool  ( pInfo->mHasUpperHandle   );
	store.XchgBool  ( pInfo->mHasLowerHandle   );
	store.XchgBool  ( pInfo->mHasOnlyFixedBars );
	store.XchgInt   ( pInfo->mNotFixedBarsCnt  );

	// other properties of the row are transient, since
	// they are reclaculated each time the frame is resized/activated
}

/***** Implementation for class cbBarInfoSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( cbBarInfo, 
							cbBarInfoSerializer,
							cbBarInfoSerializer::Serialize,
							NO_CLASS_INIT )

void cbBarInfoSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	cbBarInfo* pInfo = (cbBarInfo*)pObj;

	store.XchgWxStr ( pInfo->mName );

	store.XchgWxRect( pInfo->mBounds );

	store.XchgObjPtr( (wxObject**) &(pInfo->mpRow) );

	store.XchgBool  ( pInfo->mHasLeftHandle  );
	store.XchgBool  ( pInfo->mHasRightHandle );

	store.XchgObj   ( (wxObject*) &(pInfo->mDimInfo ) );

	store.XchgInt   ( pInfo->mState     );
	store.XchgInt   ( pInfo->mAlignment );
	store.XchgInt   ( pInfo->mRowNo     );

	store.XchgObjPtr( (wxObject**) &(pInfo->mpBarWnd) );

	store.XchgDouble( pInfo->mLenRatio );

	store.XchgWxPoint( pInfo->mPosIfFloated );

	store.XchgObjPtr( (wxObject**) &(pInfo->mpNext) );
	store.XchgObjPtr( (wxObject**) &(pInfo->mpPrev) );

	// other properties of the bar are transient, since
	// they are reclaculated each time the frame is resized/activated
}

/***** Implementation for class cbCommonPanePropertiesSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( cbCommonPaneProperties, 
							cbCommonPanePropertiesSerializer,
							cbCommonPanePropertiesSerializer::Serialize,
							NO_CLASS_INIT )

void cbCommonPanePropertiesSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	cbCommonPaneProperties* pProps = (cbCommonPaneProperties*)pObj;
	
	store.XchgBool ( pProps->mRealTimeUpdatesOn     );
	store.XchgBool ( pProps->mOutOfPaneDragOn       );
	store.XchgBool ( pProps->mExactDockPredictionOn );
	store.XchgBool ( pProps->mNonDestructFirctionOn );

	store.XchgBool ( pProps->mShow3DPaneBorderOn );

	store.XchgBool ( pProps->mBarFloatingOn      );
	store.XchgBool ( pProps->mRowProportionsOn   );
	store.XchgBool ( pProps->mColProportionsOn   );
	store.XchgBool ( pProps->mBarCollapseIconsOn );
	store.XchgBool ( pProps->mBarDragHintsOn     );

	store.XchgWxSize( pProps->mMinCBarDim );

	store.XchgInt( pProps->mResizeHandleSize );
}

/***** Implementation for class *****/

IMPLEMENT_SERIALIZER_CLASS( cbDockPane, 
							cbDockPaneSerializer,
							cbDockPaneSerializer::Serialize,
							NO_CLASS_INIT )

void cbDockPaneSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	cbDockPane* pPane = (cbDockPane*)pObj;

	store.XchgObj( (wxObject*) &(pPane->mProps) );

	store.XchgInt( pPane->mLeftMargin   );
	store.XchgInt( pPane->mRightMargin  );
	store.XchgInt( pPane->mTopMargin    );
	store.XchgInt( pPane->mBottomMargin );

	store.XchgInt( pPane->mAlignment );

	store.XchgObjArray( pPane->mRows );
	store.XchgObjPtr  ( (wxObject**) &(pPane->mpLayout) );
}

/***** Implementation for class cbUpdatesManagerBaseSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( cbUpdatesManagerBase, 
							cbUpdatesManagerBaseSerializer,
							cbUpdatesManagerBaseSerializer::Serialize,
							NO_CLASS_INIT )

void cbUpdatesManagerBaseSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	cbUpdatesManagerBase* pMgr = (cbUpdatesManagerBase*)pObj;

	// only back-reference to layout "engine"
	store.XchgObjPtr( (wxObject**) &(pMgr->mpLayout) );
}

/***** Implementation for class cbPluginBaseSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( cbPluginBase, 
							cbPluginBaseSerializer,
							cbPluginBaseSerializer::Serialize,
							cbPluginBaseSerializer::Initialize )

void cbPluginBaseSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	// plugin is "a kind" of wxEvtHandler - perform
	// serialization of the base class first

	info.SerializeInherited( pObj, store );

 	cbPluginBase* pPlugin = (cbPluginBase*)pObj;

	store.XchgObjPtr( (wxObject**) &(pPlugin->mpLayout) );

	store.XchgInt( pPlugin->mPaneMask );
}

void cbPluginBaseSerializer::Initialize( wxObject* pObj )
{
	// plugins need extra-initialization, after they are
	// attached to the frame-layout and pane mask is set

	( (cbPluginBase*)pObj )->OnInitPlugin();
}

/***** Implementation for class cbRowDragPluginSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( cbRowDragPlugin, 
							cbRowDragPluginSerializer,
							cbRowDragPluginSerializer::Serialize,
							cbRowDragPluginSerializer::Initialize )

void cbRowDragPluginSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	// plugin is "a kind" of cbPluginBaseSerializer - perform
	// serialization of the base class first

	info.SerializeInherited( pObj, store );

 	cbRowDragPlugin* pPlugin = (cbRowDragPlugin*)pObj;

	store.XchgInt( pPlugin->mSvTopMargin );
	store.XchgInt( pPlugin->mSvBottomMargin );
	store.XchgInt( pPlugin->mSvLeftMargin );
	store.XchgInt( pPlugin->mSvRightMargin );

	store.XchgObjList( pPlugin->mHiddenBars );
}

void cbRowDragPluginSerializer::Initialize( wxObject* pObj )
{
	// plugins need extra-initialization, after they are
	// attached to the frame-layout and pane mask is set

	( (cbPluginBase*)pObj )->OnInitPlugin();
}

/***** Implementation for class cbHiddenBarInfoSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( cbHiddenBarInfo, 
							cbHiddenBarInfoSerializer,
							cbHiddenBarInfoSerializer::Serialize,
							NO_CLASS_INIT )

void cbHiddenBarInfoSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
 	cbHiddenBarInfo* pInfo = (cbHiddenBarInfo*)pObj;

	store.XchgObjPtr( (wxObject**) &(pInfo->mpBar) );
	store.XchgInt( pInfo->mRowNo );
	store.XchgInt( pInfo->mIconNo );
	store.XchgInt( pInfo->mAlignment );
}

/***** Implementation for class cbFloatedBarWindowSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( cbFloatedBarWindow, 
							cbFloatedBarWindowSerializer,
							cbFloatedBarWindowSerializer::Serialize,
							cbFloatedBarWindowSerializer::Initialize )

static wxString __gTmpFrameTitle;

void cbFloatedBarWindowSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
 	cbFloatedBarWindow* pWnd = (cbFloatedBarWindow*)pObj;

	if ( store.IsLoading() == FALSE )

		__gTmpFrameTitle = pWnd->GetTitle();

	store.XchgWxStr( __gTmpFrameTitle );

	// cbFloatedBarWindow is "a kind" of wxWindow - perform
	// serialization of the base class first

	wxWindowSerializer::DoSerialize( pObj, store, 
									(wndCreationFn)cbFloatedBarWindowSerializer::CreateFloatedBarWindowFn );

	store.XchgObjPtr( (wxObject**) &(pWnd->mpBar)       );
	store.XchgObjPtr( (wxObject**) &(pWnd->mpLayout)    );
	store.XchgObjPtr( (wxObject**) &(pWnd->mpClientWnd) );
}

void cbFloatedBarWindowSerializer::CreateFloatedBarWindowFn( cbFloatedBarWindow* fbar, wxWindow* parent, const wxWindowID id, 
															 const wxPoint& pos, const wxSize& size, long style , 
													         const wxString& name )
{
	fbar->Create( parent, id, __gTmpFrameTitle, pos, size, style );
}

void cbFloatedBarWindowSerializer::Initialize( wxObject* pObj )
{
	// FOR NOW:: nothing
}

/***** Implementation for class wxNewBitmapButtonSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( wxNewBitmapButton, 
							wxNewBitmapButtonSerializer,
							wxNewBitmapButtonSerializer::Serialize,
							wxNewBitmapButtonSerializer::Initialize )

void wxNewBitmapButtonSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
 	wxNewBitmapButton* pBtn = (wxNewBitmapButton*)pObj;

	store.XchgInt ( pBtn->mTextToLabelGap );
	store.XchgInt ( pBtn->mMarginX );
	store.XchgInt ( pBtn->mMarginY );
	store.XchgInt ( pBtn->mTextAlignment );
	store.XchgBool( pBtn->mIsFlat );
	store.XchgBool( pBtn->mIsSticky );

	store.XchgWxStr( pBtn->mLabelText );
	store.XchgWxStr( pBtn->mImageFileName );
	store.XchgInt  ( pBtn->mImageFileType );

	store.XchgInt( pBtn->mFiredEventType );

	// cbFloatedBarWindow is "a kind" of wxWindow - perform
	// serialization of the base class 

	wxWindowSerializer::DoSerialize( pObj, store, 
									(wndCreationFn)wxNewBitmapButtonSerializer::CreateNewBmpBtnWindowFn );
}

void wxNewBitmapButtonSerializer::CreateNewBmpBtnWindowFn( wxNewBitmapButton* btn, wxWindow* parent, const wxWindowID id, 
														   const wxPoint& pos, const wxSize& size, long style , 
													       const wxString& name )
{
	btn->Create( parent, id, pos, size, style, name );

	//btn->Reshape();
	btn->mIsCreated = FALSE;
	btn->Reshape();
}

void wxNewBitmapButtonSerializer::Initialize( wxObject* pObj )
{
	// FOR NOW:: nothing
	wxNewBitmapButton* pBtn = (wxNewBitmapButton*)pObj;

	//pBtn->Reshape();
}

/***** Implementation for class wxDynamicToolBarSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( wxDynamicToolBar, 
							wxDynamicToolBarSerializer,
							wxDynamicToolBarSerializer::Serialize,
							wxDynamicToolBarSerializer::Initialize )

void wxDynamicToolBarSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	// cbFloatedBarWindow is "a kind" of wxWindow - perform
	// serialization of the base class first

	wxWindowSerializer::DoSerialize( pObj, store, 
									(wndCreationFn)wxDynamicToolBarSerializer::CreateDynTBarWindowFn );

 	wxDynamicToolBar* pTBar = (wxDynamicToolBar*)pObj;

	store.XchgObjArray( pTBar->mTools );
}

void wxDynamicToolBarSerializer::CreateDynTBarWindowFn( wxDynamicToolBar* tbar, wxWindow* parent, const wxWindowID id, 
														const wxPoint& pos, const wxSize& size, long style , 
													    const wxString& name )
{
	tbar->Create( parent, id, pos, size, style );
}

void wxDynamicToolBarSerializer::Initialize( wxObject* pObj )
{
	// FOR NOW:: nothing
}

/***** Implementation for class wxDynToolInfoSerializer *****/

IMPLEMENT_SERIALIZER_CLASS( wxDynToolInfo, 
							wxDynToolInfoSerializer,
							wxDynToolInfoSerializer::Serialize,
							NO_CLASS_INIT )

void wxDynToolInfoSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	// cbFloatedBarWindow is "a kind" of wxWindow - perform
	// serialization of the base class first

 	wxDynToolInfo* pInfo = (wxDynToolInfo*)pObj;

	store.XchgWxRect( pInfo->mRect );
	store.XchgBool  ( pInfo->mIsSeparator );

	store.XchgObjPtr( (wxObject**) &pInfo->mpToolWnd );
	store.XchgInt   ( pInfo->mIndex );
	store.XchgWxSize( pInfo->mRealSize );
}

#include "objstore.h"     // tabbed window is serialiable

/***** Implementation for class wxTabbedWindowSerializer ****/

IMPLEMENT_SERIALIZER_CLASS( wxTabbedWindow, 
							wxTabbedWindowSerializer,
							wxTabbedWindowSerializer::Serialize,
							wxTabbedWindowSerializer::Initialize)

void wxTabbedWindowSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	wxTabbedWindow* pWnd = (wxTabbedWindow*)pObj;

	// we're kind of window - serialize it first

	if ( store.IsLoading() )

		// FOR NOW::workaround for the mistery

		pWnd->mpTabScroll = (wxScrollBar*)(-1);

	wxWindowSerializer::DoSerialize( pObj, store, 
									(wndCreationFn)wxWindowSerializer::CreateWindowFn,
									FALSE );

	store.XchgObjList( pWnd->mTabs );

	store.XchgInt( pWnd->mActiveTab   );
	store.XchgInt( pWnd->mTitleHeight );
	store.XchgInt( pWnd->mLayoutType  );
	store.XchgInt( pWnd->mTitleHeight );

	store.XchgObj( (wxObject*) &(pWnd->mWhitePen) );
	store.XchgObj( (wxObject*) &(pWnd->mGrayPen)  );
	store.XchgObj( (wxObject*) &(pWnd->mDarkPen)  );
	store.XchgObj( (wxObject*) &(pWnd->mBlackPen) );

	store.XchgObjPtr( (wxObject**) &(pWnd->mpTabScroll  ) );
	store.XchgObjPtr( (wxObject**) &(pWnd->mpHorizScroll) );
	store.XchgObjPtr( (wxObject**) &(pWnd->mpVertScroll ) );

	store.XchgInt( pWnd->mVertGap );
	store.XchgInt( pWnd->mHorizGap );
	store.XchgInt( pWnd->mTitleVertGap );
	store.XchgInt( pWnd->mTitleHorizGap );
	store.XchgInt( pWnd->mImageTextGap );
	store.XchgInt( pWnd->mFirstTitleGap );
	store.XchgInt( pWnd->mBorderOnlyWidth );
}

void wxTabbedWindowSerializer::Initialize( wxObject* pObj )
{
	wxTabbedWindow* pWnd = (wxTabbedWindow*)pObj;

	pWnd->RecalcLayout(TRUE);
}

/***** Implementation for class twTabInfoSerializer ****/

IMPLEMENT_SERIALIZER_CLASS( twTabInfo, 
							twTabInfoSerializer,
							twTabInfoSerializer::Serialize,
							NO_CLASS_INIT )

void twTabInfoSerializer::Serialize( wxObject* pObj, wxObjectStorage& store )
{
	twTabInfo* pInfo = (twTabInfo*)pObj;

	store.XchgObjPtr( (wxObject**) &(pInfo->mpContent) );

	// NOTE:: wxSize is NOT a dynamic class unfortunately ...

	store.XchgWxSize( pInfo->mDims );

	store.XchgWxStr ( pInfo->mText );
	store.XchgWxStr( pInfo->mImageFile );

	store.XchgLong( pInfo->mImageType );

	if ( store.IsLoading() && wxFileExists( pInfo->mImageFile ) )
	
		 pInfo->mBitMap.LoadFile(  pInfo->mImageFile, pInfo->mImageType );
}

