/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Central header file for control-bar related classes
//
// Author:      Aleksandras Gluchovas <mailto:alex@soften.ktu.lt>
// Modified by:
// Created:     06/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __CONTROLBAR_G__
#define __CONTROLBAR_G__

#ifdef __GNUG__
#pragma interface "controlbar.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/window.h"
#include "wx/dynarray.h"

#define WXCONTROLBAR_VERSION      1.3

// forward declarations

class  wxFrameLayout;

class  cbDockPane;
class  cbUpdatesManagerBase;
class  cbBarDimHandlerBase;
class  cbPluginBase;
class  cbPluginEvent;
class  cbPaneDrawPlugin;

class cbBarInfo;
class cbRowInfo;
class cbDimInfo;
class cbCommonPaneProperties;

typedef cbBarInfo* BarInfoPtrT;
typedef cbRowInfo* RowInfoPtrT;

WX_DEFINE_ARRAY( BarInfoPtrT, BarArrayT );
WX_DEFINE_ARRAY( RowInfoPtrT, RowArrayT );

// control bar states

#define wxCBAR_DOCKED_HORIZONTALLY 0
#define wxCBAR_DOCKED_VERTICALLY   1
#define wxCBAR_FLOATING            2
#define wxCBAR_HIDDEN              3

// the states are enumerated above
#define MAX_BAR_STATES             4

// control bar alignments

#if !defined(wxTOP)

#define wxTOP	   0
#define wxBOTTOM   1
#define wxLEFT	   2
#define wxRIGHT    3

#endif

// one pane for each alignment
#define MAX_PANES      4

// masks for each pane

#define wxTOP_PANE 	   0x0001
#define wxBOTTOM_PANE  0x0002
#define wxLEFT_PANE	   0x0004
#define wxRIGHT_PANE   0x0008

#define wxALL_PANES    0x000F

// enumeration of hittest results, see cbDockPane::HitTestPaneItems(..) 

enum CB_HITTEST_RESULT
{
	CB_NO_ITEMS_HITTED,

	CB_UPPER_ROW_HANDLE_HITTED,
	CB_LOWER_ROW_HANDLE_HITTED,
	CB_LEFT_BAR_HANDLE_HITTED,
	CB_RIGHT_BAR_HANDLE_HITTED,
	CB_BAR_CONTENT_HITTED
};

// FIXME:: somehow in debug v. originall wxASSERT's are not compiled in...

//#undef  wxASSERT
//#define wxASSERT(x) if ( !(x) ) throw;

// helper class, used for spying for not-handled mouse events on control-bars
// and forwarding them to the frame layout

class cbBarSpy : public wxEvtHandler 
{
public:
	DECLARE_DYNAMIC_CLASS( cbBarSpy )

	wxFrameLayout* mpLayout;
	wxWindow*      mpBarWnd;

public:
	cbBarSpy(void);

	cbBarSpy( wxFrameLayout* pPanel );

	void SetBarWindow( wxWindow* pWnd );

	// overriden

	virtual bool ProcessEvent(wxEvent& event);
};

/* wxFrameLayout manages containment and docking of control bars. 
 * which can be docked along top, bottom, righ, or left side of the 
 * parent frame
 */

class wxFrameLayout : public wxEvtHandler
{
public:
	DECLARE_DYNAMIC_CLASS( wxFrameLayout )

public: /* protected really, acessed only by plugins and serializers */

	friend class cbDockPane;
	friend class wxBarHandler;

	wxWindow*    mpFrame;           // parent frame
	wxWindow*    mpFrameClient;	    // client window
	cbDockPane*  mPanes[MAX_PANES];	// panes in the panel

	// misc. cursors
	wxCursor*    mpHorizCursor;
	wxCursor*    mpVertCursor;
	wxCursor*    mpNormalCursor;
	wxCursor*    mpDragCursor;
	wxCursor*    mpNECursor; // no-entry cursor

	// pens for decoration and shades

	wxPen        mDarkPen;	 // default wxColour(128,128,128)
	wxPen        mLightPen;  // default white
	wxPen        mGrayPen;	 // default wxColour(192,192,192)
	wxPen        mBlackPen;  // default wxColour(  0,  0,  0)
	wxPen        mBorderPen; // default wxColour(128,192,192)

	wxPen        mNullPen;   // transparent pen

	// pane to which the all mouse input is currently directed (caputred)

	cbDockPane*  mpPaneInFocus;

	// pane, from which mouse pointer had just leaft

	cbDockPane*  mpLRUPane;	   

	// bounds of client window in parent frame's coordinates

	wxRect       mClntWndBounds;
	wxRect       mPrevClntWndBounds;

	bool         mFloatingOn;
	wxPoint      mNextFloatedWndPos;
	wxSize       mFloatingPosStep;

	// current plugin (right-most) plugin which receives events first

	cbPluginBase* mpTopPlugin;

	// plugin, which currently has caputred all input events, otherwise NULL 

	cbPluginBase* mpCaputesInput;

	// list of event handlers which are "pushed" onto each bar, to catch 
	// mouse events which are not handled by bars, and froward them to the ,
	// frome-layout and further to plugins

	wxList        mBarSpyList;

	// list of top-most frames which contain floated bars

	wxList        mFloatedFrames;

	// linked list of references to all bars (docked/floated/hidden)

	BarArrayT    mAllBars;

	// FOR NOW:: dirty stuff...
	bool         mClientWndRefreshPending;
	bool         mRecalcPending;
	bool         mCheckFocusWhenIdle;

public: /* protected really (accessed only by plugins) */

	// refrence to custom updates manager 
	cbUpdatesManagerBase* mpUpdatesMgr;

	// called to set calculated layout to window objects
	void PositionClientWindow();
	void PositionPanes();
	void CreateCursors();

	void RepositionFloatedBar( cbBarInfo* pBar );
	void DoSetBarState( cbBarInfo* pBar );

	bool LocateBar( cbBarInfo* pBarInfo, 
					cbRowInfo**  ppRow,
					cbDockPane** ppPane );


	bool HitTestPane( cbDockPane* pPane, int x, int y );
	cbDockPane* HitTestPanes( const wxRect& rect, cbDockPane* pCurPane );

	// returns panes, to which the given bar belongs

	cbDockPane* GetBarPane( cbBarInfo* pBar );

	// delegated from "bar-spy"
	void ForwardMouseEvent( wxMouseEvent& event, 
							cbDockPane*   pToPane,
							int           eventType );

	void RouteMouseEvent( wxMouseEvent& event, int pluginEvtType );

	void ShowFloatedWindows( bool show );

	void UnhookFromFrame();
	void HookUpToFrame();

	// NOTE:: reparenting of windows may NOT work on all platforms
	//        (reparenting allows control-bars to be floated)

	bool CanReparent();
	void ReparentWindow( wxWindow* pChild, wxWindow* pNewParent );

	wxRect& GetPrevClientRect() { return mPrevClntWndBounds; }

	void OnPaint( wxPaintEvent& event );
	void OnEraseBackground( wxEraseEvent& event );
	void OnKillFocus( wxFocusEvent& event );
	void OnSetFocus( wxFocusEvent& event );
	void OnActivate( wxActivateEvent& event );
	void OnIdle( wxIdleEvent& event );

	// factory method
	virtual cbUpdatesManagerBase* CreateUpdatesManager();

public: /* public members */

	wxFrameLayout(void); // used only while serializing

	wxFrameLayout( wxWindow* pParentFrame, 
				   wxWindow* pFrameClient = NULL, 
				   bool      activateNow  = TRUE );

	// (doesn't destory bar windows)
	virtual ~wxFrameLayout();

	// (by default floating of control-bars is ON)
	virtual void EnableFloating( bool enable = TRUE );

	// Can be called after some other layout has been deactivated,
	// and this one must "take over" the current contents of frame window.
	//
	// Effectivelly hooks itself to the frame window, re-displays all not-hidden 
	// bar-windows and repaints decorations

	virtual void Activate();

	// unhooks itself from frame window, and hides all not-hidden windows
	//
	// NOTE:: two frame-layouts should not be active at the same time in the
	//        same frame window, it would cause messy overlapping of bar windows 
	//        from both layouts

	virtual void Deactivate();

	// also hides the client window if presents

	void HideBarWindows();

	virtual void DestroyBarWindows();

	// passes the client window (e.g. MDI-client frame) to be controled by
	// frame layout, the size and position of which should be adjusted to be 
	// surrounded by controlbar panes, whenever frame is resized, or dimesnions 
	// of control panes change

	void SetFrameClient( wxWindow* pFrameClient );

	wxWindow* GetFrameClient();

	wxWindow& GetParentFrame() { return *mpFrame; }

	// used by updates-managers
	cbDockPane** GetPanesArray() { return mPanes; }

	// see pane alignment types 
	cbDockPane* GetPane( int alignment )

		{ return mPanes[alignment]; }

	// Adds bar information to frame-layout, appearence of layout is not refreshed
	// immediatelly, RefreshNow() can be called if necessary.
	//
	// NOTES:: argument pBarWnd can by NULL, resulting bar decorations to be drawn
	//         around the empty rectangle (filled with default background colour).
	//         Argument dimInfo, can be re-used for adding any number of bars, since
	//         it is not used directly, instead it's members are copied. If dimensions- 
	//         handler is present, it's instance shared (reference counted). Dimension 
	//         handler should always be allocated on the heap!)

	virtual void AddBar( wxWindow*        pBarWnd, 
					 	 cbDimInfo&       dimInfo,

						 // defaults:

						 int alignment    = wxTOP,
						 int rowNo        = 0,      // vert. position - row in the pane (if docked state)
						 int columnPos    = 0,      // horiz. position in the row in pixels (if docked state)
						 const wxString& name="bar",// name, by which the bar could be referred
						                            // in layout costumization dialogs

						 bool spyEvents    = FALSE,	// if TRUE - input events for the bar should 
													// be "spyed" in order to forward not-handled 
													// mouse clicks to frame layout (e.g. to enable 
													// easy-draggablity of toolbars just by clicking 
													// on their interior regions). For widgets like 
													// text/tree control this value should be FALSE 
													// (since there's _no_ certain way to detect 
													//  whether the event was actually handled...)

						 int state        = wxCBAR_DOCKED_HORIZONTALLY // e.g. wxCBAR_FLOATING
																	   // or wxCBAR_HIDDEN
					   );

	// can be used for repositioning already existing bars. The given bar is first removed
	// from the pane it currently belongs to, and inserted into the pane, which "matches"
	// the given recantular area. If pToPane is not NULL, bar is docked to this given pane
	
	// to dock the bar which is floated, use wxFrameLayout::DockBar(..) method

	virtual bool RedockBar( cbBarInfo* pBar, const wxRect& shapeInParent, 
							cbDockPane* pToPane = NULL, bool updateNow = TRUE );

	// methods for access and modification of bars in frame layout

	cbBarInfo* FindBarByName( const wxString& name );

	BarArrayT& GetBars();

	// changes bar's docking state (see possible control bar states)

	void SetBarState( cbBarInfo* pBar, int newStatem, bool updateNow );

	// reflects changes in bar information structure visually
	// (e.g. moves bar, changes it's dimension info, pane to which it is docked)

	void ApplyBarProperties( cbBarInfo* pBar );

	// removes bar from layout permanently, hides it's corresponding window if present

	void RemoveBar( cbBarInfo* pBar );

	// recalcualtes layout of panes, and all bars/rows in each pane

	virtual void RecalcLayout( bool repositionBarsNow = FALSE );

	int     GetClientHeight();
	int     GetClientWidth();
	wxRect& GetClientRect()		{ return mClntWndBounds;     }

	// NOTE:: in future ubdates-manager will become a normal plugin
	
	cbUpdatesManagerBase& GetUpdatesManager();
	
	// destroys the previous manager if any, set the new one

	void SetUpdatesManager( cbUpdatesManagerBase* pUMgr );

	// NOTE:: changing properties of panes, does not result immediate on-screen update

	virtual void GetPaneProperties( cbCommonPaneProperties& props, int alignment = wxTOP );

	virtual void SetPaneProperties( const cbCommonPaneProperties& props, 
		                            int paneMask = wxALL_PANES );

	// TODO:: margins should go into cbCommonPaneProperties in the future
	//
	// NOTE:: this method should be called before any custom plugins are attached

	virtual void SetMargins( int top, int bottom, int left, int right,
		                     int paneMask = wxALL_PANES );

	virtual void SetPaneBackground( const wxColour& colour );

	// recalculates layoute and performs on-screen update of all panes

	void RefreshNow( bool recalcLayout = TRUE );

	// event handlers

	void OnSize       ( wxSizeEvent&  event );
	void OnLButtonDown( wxMouseEvent& event );
	void OnLDblClick  ( wxMouseEvent& event );
	void OnLButtonUp  ( wxMouseEvent& event );
	void OnRButtonDown( wxMouseEvent& event );
	void OnRButtonUp  ( wxMouseEvent& event );
	void OnMouseMove  ( wxMouseEvent& event );

	/*** plugin-related methods ***/

	// should be used, instead of passing the event to ProcessEvent(..) method
	// of the top-plugin directly. This method checks if events are currently 
	// captured and ensures that plugin-event is routed correctly.

	virtual void FirePluginEvent( cbPluginEvent& event );
	
	// captures/releases user-input event's for the given plugin
	// Input events are: mouse movement, mouse clicks, keyboard input

	virtual void CaptureEventsForPlugin ( cbPluginBase* pPlugin );
	virtual void ReleaseEventsFromPlugin( cbPluginBase* pPlugin );

	// called by plugins ( also captures/releases mouse in parent frame)
	void CaptureEventsForPane( cbDockPane* toPane );
	void ReleaseEventsFromPane( cbDockPane* fromPane );

	// returns current top-level plugin (the one which receives events first,
	// with an exception if input-events are currently captured by some other plugin)
	
	virtual cbPluginBase& GetTopPlugin();

	// hooking custom plugins to frame layout
	//
	// NOTE:: when hooking one plugin on top of the other -
	//        use SetNextHandler(..) or similar methods
	//		  of wxEvtHandler class to compose the chain of plugins,
	//        than pass the left-most handler in this chain to
	//        the above methods (assuming that events are delegated
	//        from left-most towards right-most handler)
	//
	//        NOTE2:: this secenario is very inconvenient and "low-level",
	//                use Add/Push/PopPlugin methods instead

	virtual void SetTopPlugin( cbPluginBase* pPlugin );

	// similar to wxWindow's "push/pop-event-handler" methods, execept
	// that plugin is *deleted* upon "popping"

	virtual void PushPlugin( cbPluginBase* pPugin );
	virtual void PopPlugin();

	virtual void PopAllPlugins();

	// default plugins are : cbPaneDrawPlugin, cbRowLayoutPlugin, cbBarDragPlugin,
	// 						 cbAntiflickerPlugin, cbSimpleCustomizePlugin
	//
	// this method is automatically invoked, if no plugins were found upon 
	// fireing of the first plugin-event, i.e. wxFrameLayout *CONFIGURES* itself

	virtual void PushDefaultPlugins();

	/* "Advanced" methods for plugin-configuration	using their */
	/* dynamic class information (e.g. CLASSINFO(pluginClass) ) */

	// first checks if plugin of the given class is already "hooked up", 
	// if not, adds it to the top of plugins chain

	virtual void AddPlugin( wxClassInfo* pPlInfo, int paneMask = wxALL_PANES );

	// first checks if plugin of the givne class already hooked,
	// if so, removes it, and then inserts it to the chain
	// before plugin of the class given by "pNextPlInfo"
	//
	// NOTE:: this method is "handy" in some cases, where the order
	//        of plugin-chain could be important, e.g. one plugin overrides
	//        some functionallity of the other already hooked plugin,
	//        thefore the former should be hooked before the one
	//        who's functionality is being overriden

	virtual void AddPluginBefore( wxClassInfo* pNextPlInfo, wxClassInfo* pPlInfo, 
								  int paneMask = wxALL_PANES );

	// checks if plugin of the given class is hooked, removes
	// it if found
	//
	// @param pPlInfo class information structure for the plugin
	// @note  
	// @see wxFrameLayout::Method


	virtual void RemovePlugin( wxClassInfo* pPlInfo );

	// returns NULL, if plugin of the given class is not hooked

	virtual cbPluginBase* FindPlugin( wxClassInfo* pPlInfo );

	bool HasTopPlugin();

	DECLARE_EVENT_TABLE();
};

/* structure, which is present in each item of layout,
 * it used by any specific updates-manager to store
 * auxilary information to be used by it's specific 
 * updating algorithm 
 */

class cbUpdateMgrData : public wxObject
{
	DECLARE_DYNAMIC_CLASS( cbUpdateMgrData )
public:
	wxRect mPrevBounds;      // previous state of layout item (in parent frame's coordinates)

	bool   mIsDirty;         // overrides result of current-against-previouse bounds comparison,
	                         // i.e. requires item to be updated, regardless of it's current area
	
	wxObject*  mpCustomData; // any custom data stored by specific updates mgr.

	cbUpdateMgrData();   // is-dirty flag is set TRUE initially

	void StoreItemState( const wxRect& boundsInParent );

	void SetDirty( bool isDirty = TRUE );

	void SetCustomData( wxObject* pCustomData );

	inline bool IsDirty() { return mIsDirty; }
};

/* Abstract inteface for bar-size handler classes.
 * These objects receive notifications, whenever the docking 
 * state of the bar is changed, thus they have a possibility 
 * to adjust the values in cbDimInfo::mSizes accordingly.
 * Specific handlers can be hooked to specific types of bars.
 */

class cbBarDimHandlerBase : public wxObject
{
	DECLARE_ABSTRACT_CLASS( cbBarDimHandlerBase )

public:
	int mRefCount; // since one dim-handler can be asigned
	               // to multiple bars, it's instance is
				   // refernce-counted
public:

	// inital refernce count is 0, since handler is not used, until the
	// first invocation of AddRef()

	cbBarDimHandlerBase();

	void AddRef();
	void RemoveRef();

	// "bar-state-changes" notification
	virtual void OnChangeBarState(cbBarInfo* pBar, int newState ) = 0;
	virtual void OnResizeBar( cbBarInfo* pBar, const wxSize& given, wxSize& preferred ) = 0;
};

/* helper classes (used internally by wxFrameLayout class) */

// holds and manages information about bar demensions

class cbDimInfo : public wxObject
{
	DECLARE_DYNAMIC_CLASS( cbDimInfo )
public:
	wxSize mSizes[MAX_BAR_STATES];  // preferred sizes for each possible bar state

	wxRect mBounds[MAX_BAR_STATES]; // saved positions and sizes for each
									// possible state, values contain (-1)s if
								    // not initialized yet

	int    mLRUPane; // pane to which this bar was docked before it was floated
	                 // (wxTOP,wxBOTTOM,..)

	// top/bottom gap, separates decorations
	// from the bar's actual window, filled
	// with frame's beckground color, default: 0

	int    mVertGap;	

	// left/right gap, separates decorations
	// from the bar's actual window, filled
	// with frame's beckground colour, default: 0

	int    mHorizGap;	// NOTE:: gaps are given in frame's coord. orientation

	// TRUE, if vertical/horizotal dimensions cannot be mannualy adjusted
	//       by user using resizing handles. If FALSE, the frame-layout
	//       *automatically* places resizing handles among not-fixed bars

	bool   mIsFixed;

	cbBarDimHandlerBase* mpHandler; // NULL, if no handler present 

public:

	cbDimInfo(void);

	cbDimInfo( cbBarDimHandlerBase* pDimHandler,
			   bool                 isFixed		 // (see comments on mIsFixed member)
			 );

	cbDimInfo( int dh_x, int dh_y, // dims when docked horizontally
			   int dv_x, int dv_y, // dims when docked vertically
			   int f_x,  int f_y,  // dims when floating

			   bool isFixed  = TRUE,// (see comments on mIsFixed member)
			   int  horizGap = 6,   // (see comments on mHorizGap member)
			   int  vertGap  = 6,   // -/-

			   cbBarDimHandlerBase* pDimHandler = NULL
			 );

	const cbDimInfo& operator=( cbDimInfo& other );

	// destroys handler automatically, if present
	~cbDimInfo();

	inline cbBarDimHandlerBase* GetDimHandler() { return mpHandler; } 
};

WX_DEFINE_ARRAY(float, cbArrayFloat);

class cbRowInfo : public wxObject
{
	DECLARE_DYNAMIC_CLASS( cbRowInfo )
public:

	BarArrayT  mBars;  // row content

	// row flags (set up according to row-relations)

	bool	mHasUpperHandle;
	bool	mHasLowerHandle;
	bool    mHasOnlyFixedBars;
	int     mNotFixedBarsCnt;

	int		mRowWidth;
	int		mRowHeight;
	int		mRowY;

	// stores precalculated row's bounds in parent frame's coordinates
	wxRect mBoundsInParent;

	// info stored for updates-manager
	cbUpdateMgrData mUMgrData;

	cbRowInfo*    mpNext;
	cbRowInfo*    mpPrev;

	cbBarInfo*    mpExpandedBar; // NULL, if non of the bars is currently expanded

	cbArrayFloat  mSavedRatios;  // length-ratios bofore some of the bars was expanded

public:
	cbRowInfo(void);

	~cbRowInfo();

	// convenience method

	inline cbBarInfo* GetFirstBar() 

		{ return mBars.GetCount() ? mBars[0] : NULL; }
};

class cbBarInfo : public wxObject
{
	DECLARE_DYNAMIC_CLASS( cbBarInfo )
public:
	// textual name, by which this bar is refered in layout-costumization dialogs
	wxString      mName;

	// stores bar's bounds in pane's coordinates
	wxRect        mBounds;             

	// stores precalculated bar's bounds in parent frame's coordinates
	wxRect        mBoundsInParent;     

	// back-ref to the row, which contains this bar
	cbRowInfo*    mpRow;               

	// are set up according to the types of the surrounding bars in the row
	bool          mHasLeftHandle;
	bool          mHasRightHandle;

	cbDimInfo     mDimInfo;       // preferred sizes for each, control bar state
								  
	int           mState;         // (see definition of controlbar states)

	int           mAlignment;     // alignment of the pane to which this
	                              // bar is currently placed

	int           mRowNo;         // row, into which this bar would be placed,
	                              // when in the docking state

	wxWindow*     mpBarWnd;		  // the actual window object, NULL if no window
	                              // is attached to the control bar (possible!)

	double        mLenRatio;      // length ratio among not-fixed-size bars

	wxPoint       mPosIfFloated;  // stored last position when bar was in "floated" state
								  // poistion is stored in parent-window's coordinates
	
	cbUpdateMgrData mUMgrData;    // info stored for updates-manager

	cbBarInfo*    mpNext;         // next. bar in the row
	cbBarInfo*    mpPrev;         // prev. bar in the row

public:
	cbBarInfo(void);

	~cbBarInfo();

	inline bool IsFixed() const { return mDimInfo.mIsFixed; }

	inline bool IsExpanded() const { return this == mpRow->mpExpandedBar; }
};

// used for storing original bar's postions in the row, when the "non-destructive-friction"
// option is turned ON

class cbBarShapeData : public wxObject
{
public:
	wxRect mBounds;
	double mLenRatio;
};

// used for traversing through all bars of all rows in the pane

class wxBarIterator
{
	RowArrayT*  mpRows;
	cbRowInfo*  mpRow;
	cbBarInfo*  mpBar;

public:
	wxBarIterator( RowArrayT& rows );

	void Reset();
	bool Next(); // TRUE, if next bar is available
	
	cbBarInfo& BarInfo();

	// returns reference to currently traversed row
	cbRowInfo& RowInfo();
};

/* structure holds configuration options, 
 * which are usually the same for all panes in
 * frame layout
 */

class cbCommonPaneProperties : public wxObject
{
	DECLARE_DYNAMIC_CLASS( cbCommonPaneProperties )

	// look-and-feel configuration

	bool mRealTimeUpdatesOn;     // default: ON
	bool mOutOfPaneDragOn;       // default: ON
	bool mExactDockPredictionOn; // default: OFF
	bool mNonDestructFirctionOn; // default: OFF

	bool mShow3DPaneBorderOn;    // default: ON

	// FOR NOW:: the below properties are reserved for the "future"

	bool mBarFloatingOn;         // default: OFF
	bool mRowProportionsOn;      // default: OFF
	bool mColProportionsOn;      // default: ON
	bool mBarCollapseIconsOn;    // default: OFF
	bool mBarDragHintsOn;        // default: OFF

	// minimal dimensions for not-fixed bars in this pane (16x16 default)

	wxSize mMinCBarDim; 

	// width/height of resizing sash
	
	int    mResizeHandleSize;

	cbCommonPaneProperties(void);
};

/* class manages containment and control of control-bars 
 * along one of the four edges of the parent frame
 */

class cbDockPane : public wxObject
{
public:
	DECLARE_DYNAMIC_CLASS( cbDockPane )

	// look-and-feel configuration for this pane
	cbCommonPaneProperties mProps;

	// pane margins (in frame's coordinate-syst. orientation)

	int             mLeftMargin;	 // default: 2 pixels
	int             mRightMargin;	 // default: 2 pixels
	int             mTopMargin;		 // default: 2 pixels
	int             mBottomMargin;   // default: 2 pixels
	                           
public:
	// position of the pane in frame's coordinates
	wxRect          mBoundsInParent; 

	// pane width and height in pane's coordinates
	int             mPaneWidth;
	int             mPaneHeight;

	int				mAlignment;

	// info stored for updates-manager
	cbUpdateMgrData mUMgrData;

public: /* protected really */

	RowArrayT	    mRows;
	wxFrameLayout*  mpLayout;		 // back-ref

	// transient properties

	wxList          mRowShapeData;   // shapes of bars of recently modified row, 
								     // stored when in "non-destructive-firction" mode
	cbRowInfo*      mpStoredRow;     // row-info for which the shapes are stored

	friend class wxFrameLayout;

public: /* protected really (accessed only by plugins) */

	cbRowInfo* GetRow( int row );

	int GetRowIndex( cbRowInfo* pRow );

	// return -1, if row is not present at given vertical position
	int     GetRowAt( int paneY );
	int     GetRowAt( int upperY, int lowerY );

	// re-setups flags in the row-information structure, so that 
	// the would match the changed state of row-items correctly 
	void SyncRowFlags( cbRowInfo* pRow );

	// layout "AI" helpers:

	bool IsFixedSize( cbBarInfo* pInfo );
	int  GetNotFixedBarsCount( cbRowInfo* pRow );

	int GetRowWidth( wxList* pRow );

	int GetRowY( cbRowInfo* pRow );

	bool HasNotFixedRowsAbove( cbRowInfo* pRow );
	bool HasNotFixedRowsBelow( cbRowInfo* pRow );
	bool HasNotFixedBarsLeft ( cbBarInfo* pBar );
	bool HasNotFixedBarsRight( cbBarInfo* pBar );

	virtual void CalcLenghtRatios( cbRowInfo* pInRow );
	virtual void RecalcRowLayout( cbRowInfo* pRow );

	virtual void ExpandBar( cbBarInfo* pBar );
	virtual void ContractBar( cbBarInfo* pBar );

	void InitLinksForRow( cbRowInfo* pRow );
	void InitLinksForRows();

	// coordinate translation between parent's frame and this pane 

	void FrameToPane( long* x, long* y );
	void PaneToFrame( long* x, long* y );
	void FrameToPane( wxRect* pRect );
	void PaneToFrame( wxRect* pRect );

	inline bool HasPoint( const wxPoint& pos, int x, int y, int width, int height );

	int GetMinimalRowHeight( cbRowInfo* pRow );

	// given row height includes height of row handles, if present
	void SetRowHeight( cbRowInfo* pRow, int newHeight );

	void DoInsertBar( cbBarInfo* pBar, int rowNo );

public: /* protected really (accessed only by plugins) */

	// methods for incramental on-screen refreshing of the pane 
	// (simply, they are wrappers around corresponding plugin-events)

	virtual void PaintBarDecorations( cbBarInfo* pBar, wxDC& dc );
	virtual void PaintBarHandles( cbBarInfo* pBar, wxDC& dc );
	virtual void PaintBar( cbBarInfo* pBar, wxDC& dc );
	virtual void PaintRowHandles( cbRowInfo* pRow, wxDC& dc );
	virtual void PaintRowBackground ( cbRowInfo* pRow, wxDC& dc );
	virtual void PaintRowDecorations( cbRowInfo* pRow, wxDC& dc );
	virtual void PaintRow( cbRowInfo* pRow, wxDC& dc );
	virtual void PaintPaneBackground( wxDC& dc );
	virtual void PaintPaneDecorations( wxDC& dc );
	virtual void PaintPane( wxDC& dc );
	virtual void SizeBar( cbBarInfo* pBar );
	virtual void SizeRowObjects( cbRowInfo* pRow );
	virtual void SizePaneObjects();

	virtual wxDC* StartDrawInArea ( const wxRect& area );
	virtual void  FinishDrawInArea( const wxRect& area );

public: /* public members */

	cbDockPane(void);

	cbDockPane( int alignment, wxFrameLayout* pPanel );

	// sets pane's margins in frame's coordinate orientations
	void SetMargins( int top, int bottom, int left, int right );

	virtual ~cbDockPane();

	// does not destroys the info bar , only removes it's reference
	// from this pane

	virtual void RemoveBar( cbBarInfo* pBar );

	// rect given in the parent frame's coordinates
	
	virtual void InsertBar( cbBarInfo* pBar, const wxRect& atRect );

	// inserts bar into the given row, with dimensions and position
	// stored in pBarInfo->mBounds. Returns the node of inserted bar

	virtual void InsertBar( cbBarInfo* pBar, cbRowInfo* pIntoRow );

	// inserts bar, sets its position according to the preferred settings 
	// given in (*pBarInfo) structure

	virtual void InsertBar( cbBarInfo* pBarInfo );

	// does not destroy the row object, only removes the corresponding
	// node from this pane
	virtual void RemoveRow( cbRowInfo* pRow );
	
	// does not refresh the inserted row immediatelly,
	// if pBeforeRowNode arg. is NULL, row is appended to the end of pane's row list
	virtual void InsertRow( cbRowInfo* pRow, cbRowInfo* pBeforeRow );

	// sets pane's width in pane's coordinates (including margins)
	void SetPaneWidth(int width);

	// set the position and dims. of the pane in parent frame's coordinates
	void SetBoundsInParent( const wxRect& rect );

	inline wxRect& GetRealRect() { return mBoundsInParent; }

	// used by upadates-managers
	inline RowArrayT& GetRowList() { return mRows; }

	// convenience method

	inline cbRowInfo* GetFirstRow() 
	
		{ return mRows.GetCount() ? mRows[0] : NULL; }

	// TRUE, if the given bar node presents in this pane

	bool BarPresent( cbBarInfo* pBar );

	// retuns height, in pane's coordinates
	int GetPaneHeight();

	int GetAlignment();

	bool MatchesMask( int paneMask );

	inline bool IsHorizontal()
	{
		return (mAlignment == wxTOP ||
			    mAlignment == wxBOTTOM );
	}

	virtual void RecalcLayout();

	virtual int GetDockingState();

	// returns result of hit-testing items in the pane,
	// see CB_HITTEST_RESULTS enumeration

	virtual int HitTestPaneItems( const wxPoint& pos,   // position in pane's coorinates
								  cbRowInfo**    ppRow,
								  cbBarInfo**    ppBar
								);

	void GetBarResizeRange( cbBarInfo* pBar, int* from, int *till, bool forLeftHandle );
	void GetRowResizeRange( cbRowInfo* pRow, int* from, int* till, bool forUpperHandle );

	cbBarInfo* GetBarInfoByWindow( wxWindow* pBarWnd );

public: /* protected really (accessed only by plugins) */

	// row/bar resizing related helper-methods

	void DrawVertHandle ( wxDC& dc, int x, int y, int height );
	void DrawHorizHandle( wxDC& dc, int x, int y, int width  );

	void ResizeRow( cbRowInfo* pRow, int ofs, bool forUpperHandle );
	void ResizeBar( cbBarInfo* pBar, int ofs, bool forLeftHandle );

	// cbBarShapeData objects will be placed to given pLst (see comments on cbBarShapeData)

	void GetRowShapeData( cbRowInfo* pRow, wxList* pLst );

	// sets the shape to the given row, using the data provided in pLst
	void SetRowShapeData( cbRowInfo* pRowNode, wxList* pLst );
};

/*
 * class declares abstract interface for optimized logic, which should refresh
 * areas of frame layout - that actually need to be updated. Should be extanded,
 * to implemnet custom updating strategy
 */

class cbUpdatesManagerBase : public wxObject
{
	DECLARE_ABSTRACT_CLASS( cbUpdatesManagerBase )

public: /* protected really, accessed by serializer (if any) */

	wxFrameLayout* mpLayout;

public:
	cbUpdatesManagerBase(void)
		: mpLayout( 0 ) {}

	cbUpdatesManagerBase( wxFrameLayout* pPanel )
		: mpLayout( pPanel ) {}

	void SetLayout( wxFrameLayout* pLayout ) { mpLayout = pLayout; }

	// notificiactions received from frame-layout (in the order, in which
	// they usually would be invoked). Custom updates-managers may utilize
	// these notifications to implement more "fine-grained" updating strategy

	virtual void OnStartChanges() = 0;

	virtual void OnRowWillChange( cbRowInfo* pRow, cbDockPane* pInPane ) {}
	virtual void OnBarWillChange( cbBarInfo* pBar, cbRowInfo* pInRow, cbDockPane* pInPane ) {}
	virtual void OnPaneMarginsWillChange( cbDockPane* pPane ) {}
	virtual void OnPaneWillChange( cbDockPane* pPane ) {}

	virtual void OnFinishChanges() {}

	// refreshes parts of the frame layout, which need an update
	virtual void UpdateNow() = 0;
};

/*------------------------------------------------------------
 * "API" for developing custom plugins of Frame Layout Engine
 * TODO:: documentation
 *------------------------------------------------------------
 */

// base class for all control-bar plugin events

class cbPluginEvent : public wxEvent
{
	// NOTE:: plugin-event does not need to be a dynamic class

public:
	cbDockPane* mpPane; // NULL, if event is not addressed to any specific pane

	/* OLD STUFF::
	// FOR NOW FOR NOW:: all-in-on plugin event structure 
	wxNode* mpObjNode;
	wxNode* mpObjNodeAux;
	wxPoint mPos;
	wxSize  mSize;
	wxDC*   mpDC;
	bool    mAuxBoolVal;
	*/

	cbPluginEvent( int eventType, cbDockPane* pPane )
		: mpPane( pPane )
		  
		{ m_eventType = eventType; }
};

// event types handled by plugins

#define cbEVT_PL_LEFT_DOWN           0
#define cbEVT_PL_LEFT_UP	         1
#define cbEVT_PL_RIGHT_DOWN          2
#define cbEVT_PL_RIGHT_UP	         3
#define cbEVT_PL_MOTION	             4

#define cbEVT_PL_LEFT_DCLICK         5

#define cbEVT_PL_LAYOUT_ROW          6 
#define cbEVT_PL_RESIZE_ROW          7
#define cbEVT_PL_LAYOUT_ROWS         8
#define cbEVT_PL_INSERT_BAR          9
#define cbEVT_PL_RESIZE_BAR          10
#define cbEVT_PL_REMOVE_BAR          11
#define cbEVT_PL_SIZE_BAR_WND        12

#define cbEVT_PL_DRAW_BAR_DECOR      13
#define cbEVT_PL_DRAW_ROW_DECOR      14
#define cbEVT_PL_DRAW_PANE_DECOR     15
#define cbEVT_PL_DRAW_BAR_HANDLES    16
#define cbEVT_PL_DRAW_ROW_HANDLES    17
#define cbEVT_PL_DRAW_ROW_BKGROUND   18
#define cbEVT_PL_DRAW_PANE_BKGROUND  19

#define cbEVT_PL_START_BAR_DRAGGING  20
#define cbEVT_PL_DRAW_HINT_RECT      21

#define cbEVT_PL_START_DRAW_IN_AREA  22
#define cbEVT_PL_FINISH_DRAW_IN_AREA 23

#define cbEVT_PL_CUSTOMIZE_BAR       24
#define cbEVT_PL_CUSTOMIZE_LAYOUT    25

#define wxCUSTOM_CB_PLUGIN_EVENTS_START_AT 100

// forward decls, separated by categories

class cbLeftDownEvent;
class cbLeftUpEvent;
class cbRightDownEvent;
class cbRightUpEvent;
class cbMotionEvent;
class cbLeftDClickEvent;

class cbLayoutRowEvent;
class cbResizeRowEvent;
class cbLayoutRowsEvent;
class cbInsertBarEvent;
class cbResizeBarEvent;
class cbRemoveBarEvent;
class cbSizeBarWndEvent;

class cbDrawBarDecorEvent;
class cbDrawRowDecorEvent;
class cbDrawPaneDecorEvent;
class cbDrawBarHandlesEvent;
class cbDrawRowHandlesEvent;
class cbDrawRowBkGroundEvent;
class cbDrawPaneBkGroundEvent;

class cbStartBarDraggingEvent;
class cbDrawHintRectEvent;

class cbStartDrawInAreaEvent;
class cbFinishDrawInAreaEvent;

class cbCustomizeBarEvent;
class cbCustomizeLayoutEvent;

// defs. for handler-methods
 
typedef void (wxEvtHandler::*cbLeftDownHandler        )(cbLeftDownEvent&);
typedef void (wxEvtHandler::*cbLeftUpHandler          )(cbLeftUpEvent&);
typedef void (wxEvtHandler::*cbRightDownHandler       )(cbRightDownEvent&);
typedef void (wxEvtHandler::*cbRightUpHandler         )(cbRightUpEvent&);
typedef void (wxEvtHandler::*cbMotionHandler          )(cbMotionEvent&);
typedef void (wxEvtHandler::*cbLeftDClickHandler      )(cbLeftDClickEvent&);

typedef void (wxEvtHandler::*cbLayoutRowHandler       )(cbLayoutRowEvent&);
typedef void (wxEvtHandler::*cbResizeRowHandler       )(cbResizeRowEvent&);
typedef void (wxEvtHandler::*cbLayoutRowsHandler      )(cbLayoutRowsEvent&);
typedef void (wxEvtHandler::*cbInsertBarHandler       )(cbInsertBarEvent&);
typedef void (wxEvtHandler::*cbResizeBarHandler       )(cbResizeBarEvent&);
typedef void (wxEvtHandler::*cbRemoveBarHandler       )(cbRemoveBarEvent&);
typedef void (wxEvtHandler::*cbSizeBarWndHandler      )(cbSizeBarWndEvent&);

typedef void (wxEvtHandler::*cbDrawBarDecorHandler    )(cbDrawBarDecorEvent&);
typedef void (wxEvtHandler::*cbDrawRowDecorHandler    )(cbDrawRowDecorEvent&);
typedef void (wxEvtHandler::*cbDrawPaneDecorHandler   )(cbDrawPaneDecorEvent&);
typedef void (wxEvtHandler::*cbDrawBarHandlesHandler  )(cbDrawBarHandlesEvent&);
typedef void (wxEvtHandler::*cbDrawRowHandlesHandler  )(cbDrawRowHandlesEvent&);
typedef void (wxEvtHandler::*cbDrawRowBkGroundHandler )(cbDrawRowBkGroundEvent&);
typedef void (wxEvtHandler::*cbDrawPaneBkGroundHandler)(cbDrawPaneBkGroundEvent&);

typedef void (wxEvtHandler::*cbStartBarDraggingHandler )(cbStartBarDraggingEvent&);
typedef void (wxEvtHandler::*cbDrawHintRectHandler     )(cbDrawHintRectEvent&);

typedef void (wxEvtHandler::*cbStartDrawInAreaHandler )(cbStartDrawInAreaEvent&);
typedef void (wxEvtHandler::*cbFinishDrawInAreaHandler)(cbFinishDrawInAreaEvent&);

typedef void (wxEvtHandler::*cbCustomizeBarHandler    )(cbCustomizeBarEvent&);
typedef void (wxEvtHandler::*cbCustomizeLayoutHandler )(cbCustomizeLayoutEvent&);

// macros for creating event table entries for plugin-events

#define EVT_PL_LEFT_DOWN(func)		     { cbEVT_PL_LEFT_DOWN,		     -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbLeftDownHandler        ) & func },
#define EVT_PL_LEFT_UP(func)		     { cbEVT_PL_LEFT_UP,             -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbLeftUpHandler          ) & func },
#define EVT_PL_RIGHT_DOWN(func)		     { cbEVT_PL_RIGHT_DOWN,	         -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbRightDownHandler       ) & func },
#define EVT_PL_RIGHT_UP(func)		     { cbEVT_PL_RIGHT_UP,		     -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbRightUpHandler         ) & func },
#define EVT_PL_MOTION(func)			     { cbEVT_PL_MOTION,		         -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbMotionHandler          ) & func },
#define EVT_PL_LEFT_DCLICK(func)	     { cbEVT_PL_LEFT_DCLICK,		 -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbLeftDClickHandler      ) & func },
																															                         
#define EVT_PL_LAYOUT_ROW(func)		     { cbEVT_PL_LAYOUT_ROW,	         -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbLayoutRowHandler       ) & func },
#define EVT_PL_RESIZE_ROW(func)		     { cbEVT_PL_RESIZE_ROW,	         -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbResizeRowHandler       ) & func },
#define EVT_PL_LAYOUT_ROWS(func)	     { cbEVT_PL_LAYOUT_ROWS,	     -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbLayoutRowsHandler      ) & func },
#define EVT_PL_INSERT_BAR(func)		     { cbEVT_PL_INSERT_BAR,	         -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbInsertBarHandler       ) & func },
#define EVT_PL_RESIZE_BAR(func)		     { cbEVT_PL_RESIZE_BAR,	         -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbResizeBarHandler       ) & func },
#define EVT_PL_REMOVE_BAR(func)		     { cbEVT_PL_REMOVE_BAR,	         -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbRemoveBarHandler       ) & func },
#define EVT_PL_SIZE_BAR_WND(func)	     { cbEVT_PL_SIZE_BAR_WND,	     -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbSizeBarWndHandler      ) & func },
																															                         
#define EVT_PL_DRAW_BAR_DECOR(func)      { cbEVT_PL_DRAW_BAR_DECOR,      -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawBarDecorHandler    ) & func },
#define EVT_PL_DRAW_ROW_DECOR(func)      { cbEVT_PL_DRAW_ROW_DECOR,      -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawRowDecorHandler    ) & func },
#define EVT_PL_DRAW_PANE_DECOR(func)     { cbEVT_PL_DRAW_PANE_DECOR,     -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawPaneDecorHandler   ) & func },
#define EVT_PL_DRAW_BAR_HANDLES(func)    { cbEVT_PL_DRAW_BAR_HANDLES,    -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawBarHandlesHandler  ) & func },
#define EVT_PL_DRAW_ROW_HANDLES(func)    { cbEVT_PL_DRAW_ROW_HANDLES,    -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawRowHandlesHandler  ) & func },
#define EVT_PL_DRAW_ROW_BKGROUND(func)   { cbEVT_PL_DRAW_ROW_BKGROUND,   -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawRowBkGroundHandler ) & func },
#define EVT_PL_DRAW_PANE_BKGROUND(func)  { cbEVT_PL_DRAW_PANE_BKGROUND,  -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawPaneBkGroundHandler) & func },
																															                         
#define EVT_PL_START_BAR_DRAGGING(func)  { cbEVT_PL_START_BAR_DRAGGING,  -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbStartBarDraggingHandler) & func },
#define EVT_PL_DRAW_HINT_RECT(func)      { cbEVT_PL_DRAW_HINT_RECT,      -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawHintRectHandler    ) & func },

#define EVT_PL_START_DRAW_IN_AREA(func)  { cbEVT_PL_START_DRAW_IN_AREA,  -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbStartDrawInAreaHandler)  & func },
#define EVT_PL_FINISH_DRAW_IN_AREA(func) { cbEVT_PL_FINISH_DRAW_IN_AREA, -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbFinishDrawInAreaHandler) & func },

#define EVT_PL_CUSTOMIZE_BAR(func)       { cbEVT_PL_CUSTOMIZE_BAR,       -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbCustomizeBarHandler)     & func },
#define EVT_PL_CUSTOMIZE_LAYOUT(func)    { cbEVT_PL_CUSTOMIZE_LAYOUT,    -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbCustomizeLayoutHandler)  & func },

/* 
 * abstract base class for all control-bar related plugins 
 */

class cbPluginBase : public wxEvtHandler
{
	DECLARE_ABSTRACT_CLASS( cbPluginBase )
public:

	wxFrameLayout* mpLayout; // back-reference to the frame layout 

	// specifies panes, for which this plugin receives events
	// (see pane masks definitions)
	int            mPaneMask; 

	bool           mIsReady; // is TRUE, when plugin is ready to handle events

public:
	cbPluginBase(void) 

		: mpLayout  ( 0 ),
		  mPaneMask( wxALL_PANES ),
		  mIsReady ( FALSE )
	{}

	cbPluginBase( wxFrameLayout* pPanel, int paneMask = wxALL_PANES ) 

		: mpLayout  ( pPanel ),
		  mPaneMask( paneMask ),
		  mIsReady ( FALSE )
	{}

	inline int GetPaneMask() { return mPaneMask; }

	// NOTE:: pointer positions of mouse-events sent to plugins
	//        are always in pane's coordinates (pane's to which
	//        this plugin is hooked)

	// destroys the whole plugin chain of connected plagins
	virtual ~cbPluginBase();

	// override this method to do plugin-specific initialization
	// (at this point plugin is already attached to the frame layout,
	//  and pane masks are set)
	virtual void OnInitPlugin() { mIsReady = TRUE; }

	bool IsReady() { return mIsReady; }

	// overriden, to determine whether the target pane specified in the
	// event, matches the pane mask of this plugin (specific plugins
	// do not override this method)

	virtual bool ProcessEvent(wxEvent& event);
};

/*** event classes, for each corresponding event type (24 currnetly...uhh) ***/

// mouse-events category

class cbLeftDownEvent : public cbPluginEvent
{
public:
	wxPoint mPos;

	cbLeftDownEvent( const wxPoint& pos, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_LEFT_DOWN, pPane ),
		  mPos( pos )
	{}
};

class cbLeftUpEvent : public cbPluginEvent
{
public:
	wxPoint mPos;

	cbLeftUpEvent( const wxPoint& pos, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_LEFT_UP, pPane ),
		  mPos( pos )
	{}
};

class cbRightDownEvent : public cbPluginEvent
{
public:
	wxPoint mPos;

	cbRightDownEvent( const wxPoint& pos, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_RIGHT_DOWN, pPane ),
		  mPos( pos )
	{}
};

class cbRightUpEvent : public cbPluginEvent
{
public:
	wxPoint mPos;

	cbRightUpEvent( const wxPoint& pos, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_RIGHT_UP, pPane ),
		  mPos( pos )
	{}
};

class cbMotionEvent : public cbPluginEvent
{
public:
	wxPoint mPos;

	cbMotionEvent( const wxPoint& pos, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_MOTION, pPane ),
		  mPos( pos )
	{}
};

class cbLeftDClickEvent : public cbPluginEvent
{
public:
	wxPoint mPos;

	cbLeftDClickEvent( const wxPoint& pos, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_LEFT_DCLICK, pPane ),
		  mPos( pos )
	{}
};

// bar/row events category

class cbLayoutRowEvent : public cbPluginEvent
{
public:
	cbRowInfo* mpRow;

	cbLayoutRowEvent( cbRowInfo* pRow, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_LAYOUT_ROW, pPane ),
		  mpRow( pRow )
	{}
};

class cbResizeRowEvent : public cbPluginEvent
{
public:
	cbRowInfo* mpRow;
	int        mHandleOfs;
	bool       mForUpperHandle;

	cbResizeRowEvent( cbRowInfo* pRow, int handleOfs, bool forUpperHandle, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_RESIZE_ROW, pPane ),
		  mpRow( pRow ),
		  mHandleOfs( handleOfs ),
		  mForUpperHandle( forUpperHandle )
	{}
};

class cbLayoutRowsEvent : public cbPluginEvent
{
public:

	cbLayoutRowsEvent( cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_LAYOUT_ROWS, pPane )
	{}
};

class cbInsertBarEvent : public cbPluginEvent
{
public:
	cbBarInfo*  mpBar;
	cbRowInfo*  mpRow;

	cbInsertBarEvent( cbBarInfo* pBar, cbRowInfo* pIntoRow, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_INSERT_BAR, pPane ),

		  mpBar( pBar     ),
		  mpRow( pIntoRow )
	{}
};

class cbResizeBarEvent : public cbPluginEvent
{
public:
	cbBarInfo* mpBar;
	cbRowInfo* mpRow;

	cbResizeBarEvent( cbBarInfo* pBar, cbRowInfo* pRow, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_RESIZE_BAR, pPane ),
		  mpBar( pBar ),
		  mpRow( pRow )
	{}
};

class cbRemoveBarEvent : public cbPluginEvent
{
public:
	cbBarInfo* mpBar;

	cbRemoveBarEvent( cbBarInfo* pBar, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_REMOVE_BAR, pPane ),
		  mpBar( pBar )
	{}
};

class cbSizeBarWndEvent : public cbPluginEvent
{
public:
	cbBarInfo* mpBar;
	wxRect     mBoundsInParent;

	cbSizeBarWndEvent( cbBarInfo* pBar, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_SIZE_BAR_WND, pPane ),
		  mpBar( pBar ),
		  mBoundsInParent( pBar->mBoundsInParent )
	{}
};

class cbDrawBarDecorEvent : public cbPluginEvent
{
public:
	cbBarInfo* mpBar;
	wxDC*      mpDc;
	wxRect     mBoundsInParent;

	cbDrawBarDecorEvent( cbBarInfo* pBar, wxDC& dc, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_DRAW_BAR_DECOR, pPane ),
		  mpBar( pBar ),
		  mpDc( &dc ),
		  mBoundsInParent( pBar->mBoundsInParent )
	{}
};

class cbDrawRowDecorEvent : public cbPluginEvent
{
public:
	cbRowInfo* mpRow;
	wxDC*      mpDc;

	cbDrawRowDecorEvent( cbRowInfo* pRow, wxDC& dc, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_DRAW_ROW_DECOR, pPane ),
		  mpRow( pRow ),
		  mpDc( &dc )
	{}
};

class cbDrawPaneDecorEvent : public cbPluginEvent
{
public:
	wxDC* mpDc;

	cbDrawPaneDecorEvent( wxDC& dc, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_DRAW_PANE_DECOR, pPane ),
		  mpDc( &dc )
	{}
};

class cbDrawBarHandlesEvent : public cbPluginEvent
{
public:
	cbBarInfo* mpBar;
	wxDC*   mpDc;

	cbDrawBarHandlesEvent( cbBarInfo* pBar, wxDC& dc, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_DRAW_BAR_HANDLES, pPane ),
		  mpBar( pBar ),
		  mpDc( &dc )
	{}
};

class cbDrawRowHandlesEvent : public cbPluginEvent
{
public:
	cbRowInfo* mpRow;
	wxDC*      mpDc;

	cbDrawRowHandlesEvent( cbRowInfo* pRow, wxDC& dc, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_DRAW_ROW_HANDLES, pPane ),
		  mpRow( pRow ),
		  mpDc( &dc )
	{}
};

class cbDrawRowBkGroundEvent : public cbPluginEvent
{
public:
	cbRowInfo* mpRow;
	wxDC*   mpDc;

	cbDrawRowBkGroundEvent( cbRowInfo* pRow, wxDC& dc, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_DRAW_ROW_BKGROUND, pPane ),
		  mpRow( pRow ),
		  mpDc( &dc )
	{}
};

class cbDrawPaneBkGroundEvent : public cbPluginEvent
{
public:
	wxDC* mpDc;

	cbDrawPaneBkGroundEvent( wxDC& dc, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_DRAW_PANE_BKGROUND, pPane ),
		  mpDc( &dc )
	{}
};

class cbStartBarDraggingEvent : public cbPluginEvent
{
public:
	cbBarInfo* mpBar;
	wxPoint    mPos;  // is given in frame's coordinates

	cbStartBarDraggingEvent( cbBarInfo* pBar, const wxPoint& pos, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_START_BAR_DRAGGING, pPane ),
		  mpBar( pBar ),
		  mPos( pos )
	{}
};

class cbDrawHintRectEvent : public cbPluginEvent
{
public:
	wxRect mRect;	   // is given in frame's coordinates


	bool   mLastTime;  // indicates that this event finishes "session" of on-screen drawing,
					   // thus associated resources can be freed now
	bool   mEraseRect; // does not have any impact, if recangle is drawn using XOR-mask

	bool   mIsInClient;// in cleint area hint could be drawn differently,
	                   // e.g. with fat/hatched border


	cbDrawHintRectEvent( const wxRect& rect, bool isInClient, bool eraseRect, bool lastTime )

		: cbPluginEvent( cbEVT_PL_DRAW_HINT_RECT, 0 ),
		  mRect      ( rect       ),
		  mLastTime  ( lastTime   ),
		  mEraseRect ( eraseRect  ),
		  mIsInClient( isInClient )
	{}
};

class cbStartDrawInAreaEvent : public cbPluginEvent
{
public:
	wxRect mArea;
	wxDC** mppDc; // points to pointer, where the reference
	              // to the obtained buffer-context should be placed

	cbStartDrawInAreaEvent( const wxRect& area, wxDC** ppDCForArea, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_START_DRAW_IN_AREA, pPane ),
		  mArea( area ),
		  mppDc( ppDCForArea )
	{}
};

class cbFinishDrawInAreaEvent : public cbPluginEvent
{
public:
	wxRect mArea;

	cbFinishDrawInAreaEvent( const wxRect& area, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_FINISH_DRAW_IN_AREA, pPane ),
		  mArea( area )
	{}
};

class cbCustomizeBarEvent : public cbPluginEvent
{
public:
	wxPoint    mClickPos; // in parent frame's coordinates
	cbBarInfo* mpBar;

	cbCustomizeBarEvent( cbBarInfo* pBar, const wxPoint& clickPos, cbDockPane* pPane )

		: cbPluginEvent( cbEVT_PL_CUSTOMIZE_BAR, pPane ),
		  mClickPos( clickPos ),
		  mpBar( pBar )
	{}
};

class cbCustomizeLayoutEvent : public cbPluginEvent
{
public:
	wxPoint mClickPos; // in parent frame's coordinates

	cbCustomizeLayoutEvent( const wxPoint& clickPos )

		: cbPluginEvent( cbEVT_PL_CUSTOMIZE_LAYOUT, 0 ),
		  mClickPos( clickPos )
	{}
};


#endif
