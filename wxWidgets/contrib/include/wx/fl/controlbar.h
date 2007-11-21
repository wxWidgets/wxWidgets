/////////////////////////////////////////////////////////////////////////////
// Name:        controlbar.h
// Purpose:     Central header file for control-bar related classes
//
// Author:      Aleksandras Gluchovas <mailto:alex@soften.ktu.lt>
// Modified by:
// Created:     06/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __CONTROLBAR_G__
#define __CONTROLBAR_G__

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/pen.h"
#include "wx/window.h"
#include "wx/dynarray.h"
#include "wx/fl/fldefs.h"

#define WXCONTROLBAR_VERSION      1.3

// forward declarations

class  WXDLLIMPEXP_FL wxFrameLayout;

class  WXDLLIMPEXP_FL cbDockPane;
class  WXDLLIMPEXP_FL cbUpdatesManagerBase;
class  WXDLLIMPEXP_FL cbBarDimHandlerBase;
class  WXDLLIMPEXP_FL cbPluginBase;
class  WXDLLIMPEXP_FL cbPluginEvent;
class  WXDLLIMPEXP_FL cbPaneDrawPlugin;

class WXDLLIMPEXP_FL cbBarInfo;
class WXDLLIMPEXP_FL cbRowInfo;
class WXDLLIMPEXP_FL cbDimInfo;
class WXDLLIMPEXP_FL cbCommonPaneProperties;

typedef cbBarInfo* BarInfoPtrT;
typedef cbRowInfo* RowInfoPtrT;

WXFL_DEFINE_ARRAY_PTR( BarInfoPtrT, BarArrayT );
WXFL_DEFINE_ARRAY_PTR( RowInfoPtrT, RowArrayT );

// control bar states

#define wxCBAR_DOCKED_HORIZONTALLY 0
#define wxCBAR_DOCKED_VERTICALLY   1
#define wxCBAR_FLOATING            2
#define wxCBAR_HIDDEN              3

// the states are enumerated above
#define MAX_BAR_STATES             4

// control bar alignments

#if !defined(FL_ALIGN_TOP)

#define FL_ALIGN_TOP        0
#define FL_ALIGN_BOTTOM     1
#define FL_ALIGN_LEFT       2
#define FL_ALIGN_RIGHT      3

#endif

// one pane for each alignment
#define MAX_PANES      4

// masks for each pane

#define FL_ALIGN_TOP_PANE        0x0001
#define FL_ALIGN_BOTTOM_PANE   0x0002
#define FL_ALIGN_LEFT_PANE       0x0004
#define FL_ALIGN_RIGHT_PANE   0x0008

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

/*
Helper class, used for spying for unhandled mouse events on control bars
and forwarding them to the frame layout.
*/

class WXDLLIMPEXP_FL cbBarSpy : public wxEvtHandler
{
public:
    DECLARE_DYNAMIC_CLASS( cbBarSpy )

    wxFrameLayout* mpLayout;
    wxWindow*      mpBarWnd;

public:
        // Default constructor.

    cbBarSpy(void);

        // Constructor, taking a parent pane.

    cbBarSpy( wxFrameLayout* pPanel );

        // Sets the bar window.

    void SetBarWindow( wxWindow* pWnd );

        // Performs special event processing.

    virtual bool ProcessEvent(wxEvent& event);
};

/*
wxFrameLayout manages containment and docking of control bars,
which can be docked along the top, bottom, right, or left side of the
parent frame.
*/

class WXDLLIMPEXP_FL wxFrameLayout : public wxEvtHandler
{
public:
        // Default constructor, used only for serialization.

    wxFrameLayout();

        // Constructor, taking parent window, the (MDI) client of the parent if there
        // is one, and flag specifying whether to activate the layout.

    wxFrameLayout( wxWindow* pParentFrame,
                   wxWindow* pFrameClient = NULL,
                   bool      activateNow  = true );

        // Destructor. It does not destroy the bar windows.

    virtual ~wxFrameLayout();

        // Enables floating behaviour. By default floating of control bars is on.

    virtual void EnableFloating( bool enable = true );

        // Activate can be called after some other layout has been deactivated,
        // and this one must take over the current contents of the frame window.
        //
        // Effectively hooks itself to the frame window, re-displays all non-hidden
        // bar windows and repaints the decorations.

    virtual void Activate();

        // Deactivate unhooks itself from frame window, and hides all non-hidden windows.
        //
        // Note: two frame layouts should not be active at the same time in the
        // same frame window, since it would cause messy overlapping of bar windows
        // from both layouts.

    virtual void Deactivate();

        // Hides the bar windows, and also the client window if present.

    void HideBarWindows();

        // Destroys the bar windows.

    virtual void DestroyBarWindows();

        // Passes the client window (e.g. MDI client window) to be controlled by
        // frame layout, the size and position of which should be adjusted to be
        // surrounded by controlbar panes, whenever the frame is resized or the dimensions
        // of control panes change.

    void SetFrameClient( wxWindow* pFrameClient );

        // Returns the frame client, or NULL if not present.

    wxWindow* GetFrameClient();

        // Returns the parent frame.

    wxWindow& GetParentFrame() { return *mpFrame; }

        // Returns an array of panes. Used by update managers.

    cbDockPane** GetPanesArray() { return mPanes; }

        // Returns a pane for the given alignment. See pane alignment types.

    cbDockPane* GetPane( int alignment )

        { return mPanes[alignment]; }

        // Adds bar information to the frame layout. The appearance of the layout is not refreshed
        // immediately; RefreshNow() can be called if necessary.
        //
        // Notes: the argument pBarWnd can by NULL, resulting in bar decorations to be drawn
        // around the empty rectangle (filled with default background colour).
        // Argument dimInfo can be reused for adding any number of bars, since
        // it is not used directly - instead its members are copied. If the dimensions
        // handler is present, its instance is shared (reference counted). The dimension
        // handler should always be allocated on the heap.

        // pBarWnd is the window to be managed.

        // dimInfo contains dimension information.

        // alignment is a value such as FL_ALIGN_TOP.

        // rowNo is the vertical position or row in the pane (if in docked state).

        // columnPos is the horizontal position within the row in pixels (if in docked state).

        // name is a name by which the bar can be referred in layout customization dialogs.

        // If spyEvents is true, input events for the bar should be "spyed" in order
        // to forward unhandled mouse clicks to the frame layout, for example to enable
        // easy draggablity of toolbars just by clicking on their interior regions.
        // For widgets like text/tree control this value should be false,
        // since there's no certain way to detect  whether the event was actually handled.

        // state is the initial state, such as wxCBAR_DOCKED_HORIZONTALLY,
        // wxCBAR_FLOATING, wxCBAR_HIDDEN.

    virtual void AddBar( wxWindow*        pBarWnd,
                         const cbDimInfo&       dimInfo,

                         // defaults:
                         int alignment    = FL_ALIGN_TOP,
                         int rowNo        = 0,
                         int columnPos    = 0,
                         const wxString& name = wxT("bar"),
                         bool spyEvents    = false,
                         int state        = wxCBAR_DOCKED_HORIZONTALLY
                       );

        // ReddockBar can be used for repositioning existing bars. The given bar is first removed
        // from the pane it currently belongs to, and inserted into the pane, which "matches"
        // the given rectangular area. If pToPane is not NULL, the bar is docked to this given pane.
        // To dock a bar which is floating, use the wxFrameLayout::DockBar method.

    virtual bool RedockBar( cbBarInfo* pBar, const wxRect& shapeInParent,
                            cbDockPane* pToPane = NULL, bool updateNow = true );

        // Finds the bar in the framelayout, by name.

    cbBarInfo* FindBarByName( const wxString& name );

        // Finds the bar in the framelayout, by window.

    cbBarInfo* FindBarByWindow( const wxWindow* pWnd );

        // Gets an array of bars.

    BarArrayT& GetBars();

        // Changes the bar's docking state (see possible control bar states).

    void SetBarState( cbBarInfo* pBar, int newStatem, bool updateNow );

        // Toggles the bar between visible and hidden.

    void InverseVisibility( cbBarInfo* pBar );

        // Reflects changes in bar information structure visually.
        // For example, moves the bar, changes its dimension information,
        // or changes the pane to which it is docked.

    void ApplyBarProperties( cbBarInfo* pBar );

        // Removes the bar from the layout permanently, and hides its corresponding window if present.

    void RemoveBar( cbBarInfo* pBar );

        // Recalculates the layout of panes, and all bars/rows in each pane.

    virtual void RecalcLayout( bool repositionBarsNow = false );

        // Returns the client height.

    int     GetClientHeight();

        // Returns the client width.

    int     GetClientWidth();

        // Returns the client's rectangle.

    wxRect& GetClientRect()        { return mClntWndBounds;     }

        // Returns a reference to the updates manager.
        // Note: in future, the updates manager will become a normal plugin.

    cbUpdatesManagerBase& GetUpdatesManager();

        // Destroys the previous manager if any, and sets the new one.

    void SetUpdatesManager( cbUpdatesManagerBase* pUMgr );

        // Gets the pane properties for the given alignment.

    virtual void GetPaneProperties( cbCommonPaneProperties& props, int alignment = FL_ALIGN_TOP );

        // Sets the pane properties for the given alignment.
        // Note: changing properties of panes does not result immediate on-screen update.

    virtual void SetPaneProperties( const cbCommonPaneProperties& props,
                                    int paneMask = wxALL_PANES );

        // Sets the margins for the given panes.
        // The margins should go into cbCommonPaneProperties in the future.
        //
        // Note: this method should be called before any custom plugins are attached.

    virtual void SetMargins( int top, int bottom, int left, int right,
                             int paneMask = wxALL_PANES );

        // Sets the pane background colour.

    virtual void SetPaneBackground( const wxColour& colour );

        // Recalculates layout and performs on-screen update of all panes.

    void RefreshNow( bool recalcLayout = true );

        // Event handler for a size event.

    void OnSize       ( wxSizeEvent&  event );

        // Event handler for a left down button event.

    void OnLButtonDown( wxMouseEvent& event );

        // Event handler for a left doubleclick button event.

    void OnLDblClick  ( wxMouseEvent& event );

        // Event handler for a left button up event.

    void OnLButtonUp  ( wxMouseEvent& event );

        // Event handler for a right button down event.

    void OnRButtonDown( wxMouseEvent& event );

        // Event handler for a right button up event.

    void OnRButtonUp  ( wxMouseEvent& event );

        // Event handler for a mouse move event.

    void OnMouseMove  ( wxMouseEvent& event );

        // This function should be used instead of passing the event to the ProcessEvent method
        // of the top-level plugin directly. This method checks if events are currently
        // captured and ensures that plugin-event is routed correctly.

    virtual void FirePluginEvent( cbPluginEvent& event );

        // Captures user input events for the given plugin.
        // Input events are: mouse movement, mouse clicks, keyboard input.

    virtual void CaptureEventsForPlugin ( cbPluginBase* pPlugin );

        // Releases user input events for the given plugin.
        // Input events are: mouse movement, mouse clicks, keyboard input

    virtual void ReleaseEventsFromPlugin( cbPluginBase* pPlugin );

        // Called by plugins; also captures the mouse in the parent frame.

    void CaptureEventsForPane( cbDockPane* toPane );

        // Called by plugins; also releases mouse in the parent frame.

    void ReleaseEventsFromPane( cbDockPane* fromPane );

        // Returns the current top-level plugin (the one that receives events first,
        // except if input events are currently captured by some other plugin).

    virtual cbPluginBase& GetTopPlugin();

        // Hooking custom plugins to frame layout.
        //
        // Note: when hooking one plugin on top of the other,
        // use SetNextHandler or similar methods
        // of wxEvtHandler class to compose the chain of plugins,
        // than pass the left-most handler in this chain to
        // the above methods (assuming that events are delegated
        // from left-most towards right-most handler).
        //
        // This secenario is very inconvenient and "low-level",
        // so use the Add/Push/PopPlugin methods instead.

    virtual void SetTopPlugin( cbPluginBase* pPlugin );

        // Similar to wxWindow's "push/pop-event-handler" methods, execept
        // that the plugin is deleted upon "popping".

    virtual void PushPlugin( cbPluginBase* pPugin );

        // Similar to wxWindow's "push/pop-event-handler" methods, execept
        // that the plugin is deleted upon "popping".

    virtual void PopPlugin();

        // Pop all plugins.
    virtual void PopAllPlugins();

        // Adds the default plugins. These are cbPaneDrawPlugin, cbRowLayoutPlugin, cbBarDragPlugin,
        // cbAntiflickerPlugin, cbSimpleCustomizePlugin.
        //
        // This method is automatically invoked if no plugins were found upon
        // firing of the first plugin-event, i.e. when wxFrameLayout configures itself.

    virtual void PushDefaultPlugins();

        // An advanced methods for plugin configuration    using their
        // dynamic class information, for example CLASSINFO(pluginClass).

        // First checks if the plugin of the given class is already "hooked up".
        // If not, adds it to the top of the plugins chain.

    virtual void AddPlugin( wxClassInfo* pPlInfo, int paneMask = wxALL_PANES );

        // First checks if the plugin of the given class is already hooked.
        // If so, removes it, and then inserts it into the chain
        // before the plugin of the class given by pNextPlInfo.
        //
        // Note: this method is handy in some cases where the order
        // of the plugin-chain could be important, for example when one plugin overrides
        // some functionality of another already-hooked plugin,
        // so that the former plugin should be hooked before the one
        // whose functionality is being overridden.

    virtual void AddPluginBefore( wxClassInfo* pNextPlInfo, wxClassInfo* pPlInfo,
                                  int paneMask = wxALL_PANES );

        // Checks if the plugin of the given class is hooked, and removes
        // it if found.

    virtual void RemovePlugin( wxClassInfo* pPlInfo );

        // Finds a plugin with the given class, or returns NULL if a plugin of the given
        // class is not hooked.

    virtual cbPluginBase* FindPlugin( wxClassInfo* pPlInfo );

        // Returns true if there is a top plugin.

    bool HasTopPlugin();

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS( wxFrameLayout )

public: /* protected really, acessed only by plugins and serializers */

    friend class cbDockPane;
    friend class wxBarHandler;

    wxWindow*    mpFrame;           // parent frame
    wxWindow*    mpFrameClient;        // client window
    cbDockPane*  mPanes[MAX_PANES];    // panes in the panel

    // misc. cursors
    wxCursor*    mpHorizCursor;
    wxCursor*    mpVertCursor;
    wxCursor*    mpNormalCursor;
    wxCursor*    mpDragCursor;
    wxCursor*    mpNECursor; // no-entry cursor

    // pens for decoration and shades

    wxPen        mDarkPen;     // default wxSYS_COLOUR_3DSHADOW
    wxPen        mLightPen;  // default wxSYS_COLOUR_3DHILIGHT
    wxPen        mGrayPen;     // default wxSYS_COLOUR_3DFACE
    wxPen        mBlackPen;  // default wxColour(  0,  0,  0)
    wxPen        mBorderPen; // default wxSYS_COLOUR_3DFACE

    wxPen        mNullPen;   // transparent pen

        // pane to which the all mouse input is currently directed (caputred)

    cbDockPane*  mpPaneInFocus;

        // pane, from which mouse pointer had just left

    cbDockPane*  mpLRUPane;

        // bounds of client window in parent frame's coordinates

    wxRect       mClntWndBounds;
    wxRect       mPrevClntWndBounds;

    bool         mFloatingOn;
    wxPoint      mNextFloatedWndPos;
    wxSize       mFloatingPosStep;

        // current plugin (right-most) plugin which receives events first

    cbPluginBase* mpTopPlugin;

        // plugin, which currently has captured all input events, otherwise NULL

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

        // Called to apply the calculated layout to window objects.

    void PositionClientWindow();

        // Called to apply the calculated layout to window objects.

    void PositionPanes();

        // Creates the cursors.

    void CreateCursors();

        // Applies the calculated layout to a floating bar.

    void RepositionFloatedBar( cbBarInfo* pBar );

        // Applies the state to the window objects.

    void DoSetBarState( cbBarInfo* pBar );

        // The purpose of this function is unknown.

    bool LocateBar( cbBarInfo* pBarInfo,
                    cbRowInfo**  ppRow,
                    cbDockPane** ppPane );


        // Returns true if the position is within the given pane.

    bool HitTestPane( cbDockPane* pPane, int x, int y );

        // Returns the pane for which the rectangle hit test succeeds, giving
        // preference to the given pane if supplied.

    cbDockPane* HitTestPanes( const wxRect& rect, cbDockPane* pCurPane );

        // Returns the pane to which the given bar belongs.

    cbDockPane* GetBarPane( cbBarInfo* pBar );

        // Delegated from "bar-spy".
    void ForwardMouseEvent( wxMouseEvent& event,
                            cbDockPane*   pToPane,
                            int           eventType );

        // Routes the mouse event to the appropriate pane.

    void RouteMouseEvent( wxMouseEvent& event, int pluginEvtType );

        // Shows all floated windows.

    void ShowFloatedWindows( bool show );

        // Unhooks the layout from the frame.

    void UnhookFromFrame();

        // Hooks the layout up to the frame (pushes the layout onto the
        // frame's event handler stack).

    void HookUpToFrame();

        // Returns true if the platform allows reparenting. This may not return true
        // for all platforms. Reparenting allows control bars to be floated.

    bool CanReparent();

        // Reparents pChild to have parent pNewParent.

    void ReparentWindow( wxWindow* pChild, wxWindow* pNewParent );

        // Returns the previous client window rectangle.

    wxRect& GetPrevClientRect() { return mPrevClntWndBounds; }

        // Handles paint events, calling PaintPane for each pane.

    void OnPaint( wxPaintEvent& event );

        // Handles background erase events. Currently does nothing.

    void OnEraseBackground( wxEraseEvent& event );

        // Handles focus kill events. Currently does nothing.

    void OnKillFocus( wxFocusEvent& event );

        // Handles focus set events. Currently does nothing.

    void OnSetFocus( wxFocusEvent& event );

        // Handles activation events. Currently does nothing.

    void OnActivate( wxActivateEvent& event );

        // Handles idle events.

    void OnIdle( wxIdleEvent& event );

        // Returns a new cbGCUpdatesMgr object.

    virtual cbUpdatesManagerBase* CreateUpdatesManager();
};

/*
A structure that is present in each item of layout,
used by any particular updates-manager to store
auxiliary information to be used by its updating algorithm.
*/

class WXDLLIMPEXP_FL cbUpdateMgrData : public wxObject
{
    DECLARE_DYNAMIC_CLASS( cbUpdateMgrData )
public:
    wxRect mPrevBounds;      // previous state of layout item (in parent frame's coordinates)

    bool   mIsDirty;         // overrides result of current-against-previous bounds comparison,
                             // i.e. requires item to be updated, regardless of it's current area

    wxObject*  mpCustomData; // any custom data stored by specific updates mgr.

        // Default constructor. Is-dirty flag is set true initially.

    cbUpdateMgrData();

        // Store the item state.

    void StoreItemState( const wxRect& boundsInParent );

        // Set the dirty flag.

    void SetDirty( bool isDirty = true );

        // Set custom data.

    void SetCustomData( wxObject* pCustomData );

        // Returns the is-dirty flag.

    inline bool IsDirty() { return mIsDirty; }
};

/*
Abstract interface for bar-size handler classes.
These objects receive notifications whenever the docking
state of the bar is changed, thus they provide the possibility
to adjust the values in cbDimInfo::mSizes accordingly.
Specific handlers can be hooked up to specific types of bar.
*/

class WXDLLIMPEXP_FL cbBarDimHandlerBase : public wxObject
{
    DECLARE_ABSTRACT_CLASS( cbBarDimHandlerBase )

public:
    int mRefCount; // since one dim-handler can be assigned
                   // to multiple bars, it's instance is
                   // reference-counted
public:

        // Default constructor. The initial reference count is 0, since
        // the handler is not used until the first invocation of AddRef().

    cbBarDimHandlerBase();

        // Increments the reference count.

    void AddRef();

        // Decrements the reference count, and if the count is at zero,
        // delete 'this'.

    void RemoveRef();

        // Responds to "bar-state-changes" notifications.

    virtual void OnChangeBarState(cbBarInfo* pBar, int newState ) = 0;

        // Responds to bar resize notifications.

    virtual void OnResizeBar( cbBarInfo* pBar, const wxSize& given, wxSize& preferred ) = 0;
};

/*
Helper class used internally by the wxFrameLayout class.
Holds and manages information about bar dimensions.
*/

class WXDLLIMPEXP_FL cbDimInfo : public wxObject
{
    DECLARE_DYNAMIC_CLASS( cbDimInfo )
public:
    wxSize mSizes[MAX_BAR_STATES];  // preferred sizes for each possible bar state

    wxRect mBounds[MAX_BAR_STATES]; // saved positions and sizes for each
                                    // possible state, values contain (-1)s if
                                    // not initialized yet

    int    mLRUPane; // pane to which this bar was docked before it was floated
                     // (FL_ALIGN_TOP,FL_ALIGN_BOTTOM,..)

    // top/bottom gap, separates decorations
    // from the bar's actual window, filled
    // with frame's beckground color, default: 0

    int    mVertGap;

    // left/right gap, separates decorations
    // from the bar's actual wndow, filled
    // with frame's beckground colour, default: 0

    int    mHorizGap;    // NOTE:: gaps are given in frame's coord. orientation

    // true, if vertical/horizontal dimensions cannot be mannualy adjusted
    //       by user using resizing handles. If false, the frame-layout
    //       *automatically* places resizing handles among not-fixed bars

    bool   mIsFixed;

    cbBarDimHandlerBase* mpHandler; // NULL, if no handler present

public:

        // Default constructor.

    cbDimInfo(void);

        // Constructor.
        // isFixed is true if vertical/horizontal dimensions cannot be manually adjusted
        // by the user using resizing handles. If false, the frame-layout
        // automatically places resizing handles among bars that do are not fixed.

    cbDimInfo( cbBarDimHandlerBase* pDimHandler,
               bool                 isFixed         // (see comments on mIsFixed member)
             );

        // Constructor taking dimenstion information.
        //
        // dh_x, dh_y are the dimensions when docked horizontally.
        //
        // dv_x, dv_y are the dimensions when docked vertically.
        //
        // f_x, f_y are the dimensions when floating.
        //
        // For information on isFixed, see comments above.
        //
        // horizGap is the left/right gap, separating decorations
        // from the bar's actual wndow, filled with the frame's background colour.
        // The dimension is given in the frame's coordinates.
        //
        // vertGap is the top/bottom gap, separating decorations
        // from the bar's actual wndow, filled with the frame's background colour.
        // The dimension is given in the frame's coordinates.

    cbDimInfo( int dh_x, int dh_y,
               int dv_x, int dv_y,
               int f_x,  int f_y,

               bool isFixed  = true,
               int  horizGap = 6,
               int  vertGap  = 6,

               cbBarDimHandlerBase* pDimHandler = NULL
             );

        // Constructor.

    cbDimInfo( int x, int y,
               bool isFixed  = true,
               int  gap = 6,
               cbBarDimHandlerBase* pDimHandler = NULL
             );

        // Destructor. Destroys handler automatically, if present.

    ~cbDimInfo();

         // Assignment operator.

    const cbDimInfo& operator=( const cbDimInfo& other );

         // Returns the handler, if any.

    inline cbBarDimHandlerBase* GetDimHandler() { return mpHandler; }
};

// FIXME: this array definition compiles but probably doesn't do what was intended (GD)
WXFL_DEFINE_ARRAY_LONG(float, cbArrayFloat);

/*
Helper class used internally by the wxFrameLayout class.
Holds and manages information about bar rows.
*/

class cbRowInfo : public wxObject
{
    DECLARE_DYNAMIC_CLASS( cbRowInfo )
public:

    BarArrayT  mBars;  // row content

    // row flags (set up according to row-relations)

    bool    mHasUpperHandle;
    bool    mHasLowerHandle;
    bool    mHasOnlyFixedBars;
    int     mNotFixedBarsCnt;

    int        mRowWidth;
    int        mRowHeight;
    int        mRowY;

    // stores precalculated row's bounds in parent frame's coordinates
    wxRect mBoundsInParent;

    // info stored for updates-manager
    cbUpdateMgrData mUMgrData;

    cbRowInfo*    mpNext;
    cbRowInfo*    mpPrev;

    cbBarInfo*    mpExpandedBar; // NULL, if non of the bars is currently expanded

    cbArrayFloat  mSavedRatios;  // length-ratios bofore some of the bars was expanded

public:
        // Constructor.

    cbRowInfo(void);

        // Destructor.

    ~cbRowInfo();

        // Returns the first bar.

    inline cbBarInfo* GetFirstBar()

        { return mBars.GetCount() ? mBars[0] : NULL; }
};

/*
Helper class used internally by the wxFrameLayout class.
Holds and manages bar information.
*/

class cbBarInfo : public wxObject
{
    DECLARE_DYNAMIC_CLASS( cbBarInfo )
public:
    // textual name, by which this bar is referred in layout-customization dialogs
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

    // determines if this bar can float. The layout's setting as priority. For
    // example, if the layout's mFloatingOn is false, this setting is irrelevant
    // since nothing will float at all. If the layout's floating is on, use this
    // setting to prevent specific bars from floating. In other words, all bars
    // float by default and floating can be turned off on individual bars.
    bool          mFloatingOn;    // default: ON (which is also the layout's mFloatingOn default setting)

    cbDimInfo     mDimInfo;       // preferred sizes for each, control bar state

    int           mState;         // (see definition of controlbar states)

    int           mAlignment;     // alignment of the pane to which this
                                  // bar is currently placed

    int           mRowNo;         // row, into which this bar would be placed,
                                  // when in the docking state

    wxWindow*     mpBarWnd;          // the actual window object, NULL if no window
                                  // is attached to the control bar (possible!)

    double        mLenRatio;      // length ratio among not-fixed-size bars

    wxPoint       mPosIfFloated;  // stored last position when bar was in "floated" state
                                  // poistion is stored in parent-window's coordinates

    cbUpdateMgrData mUMgrData;    // info stored for updates-manager

    cbBarInfo*    mpNext;         // next. bar in the row
    cbBarInfo*    mpPrev;         // prev. bar in the row

public:
        // Constructor.

    cbBarInfo(void);

        // Destructor.

    ~cbBarInfo();

        // Returns true if this bar is fixed.

    inline bool IsFixed() const { return mDimInfo.mIsFixed; }

        // Returns true if this bar is expanded.

    inline bool IsExpanded() const { return this == mpRow->mpExpandedBar; }
};

/*
Used for storing the original bar's positions in the row, when the 'non-destructive-friction'
option is turned on.
*/

class cbBarShapeData : public wxObject
{
public:
    wxRect mBounds;
    double mLenRatio;
};

/*
Used for traversing through all bars of all rows in the pane.
*/

class wxBarIterator
{
    RowArrayT*  mpRows;
    cbRowInfo*  mpRow;
    cbBarInfo*  mpBar;

public:
        // Constructor, taking row array.

    wxBarIterator( RowArrayT& rows );

        // Resets the iterator to the start of the first row.

    void Reset();

        // Advances the iterator and returns true if a bar is available.

    bool Next();

        // Gets the current bar information.

    cbBarInfo& BarInfo();

        // Returns a reference to the currently traversed row.

    cbRowInfo& RowInfo();
};

/*
A structure holding configuration options,
which are usually the same for all panes in
a frame layout.
*/

class WXDLLIMPEXP_FL cbCommonPaneProperties : public wxObject
{
    DECLARE_DYNAMIC_CLASS( cbCommonPaneProperties )

    // look-and-feel configuration

    bool mRealTimeUpdatesOn;     // default: ON
    bool mOutOfPaneDragOn;       // default: ON
    bool mExactDockPredictionOn; // default: OFF
    bool mNonDestructFrictionOn; // default: OFF

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

        // Default constructor.

    cbCommonPaneProperties(void);

        // Copy constructor

    cbCommonPaneProperties(const cbCommonPaneProperties&);

        // Assignment operator

    cbCommonPaneProperties& operator=(const cbCommonPaneProperties&);
};

/*
This class manages containment and control of control bars
along one of the four edges of the parent frame.
*/

class cbDockPane : public wxObject
{
public:
    DECLARE_DYNAMIC_CLASS( cbDockPane )

    // look-and-feel configuration for this pane
    cbCommonPaneProperties mProps;

    // pane margins (in frame's coordinate-syst. orientation)

    int             mLeftMargin;     // default: 2 pixels
    int             mRightMargin;     // default: 2 pixels
    int             mTopMargin;         // default: 2 pixels
    int             mBottomMargin;   // default: 2 pixels

public:
    // position of the pane in frame's coordinates
    wxRect          mBoundsInParent;

    // pane width and height in pane's coordinates
    int             mPaneWidth;
    int             mPaneHeight;

    int                mAlignment;

    // info stored for updates-manager
    cbUpdateMgrData mUMgrData;

public: /* protected really */

    RowArrayT        mRows;
    wxFrameLayout*  mpLayout;         // back-ref

    // transient properties

    wxList          mRowShapeData;   // shapes of bars of recently modified row,
                                     // stored when in "non-destructive-friction" mode
    cbRowInfo*      mpStoredRow;     // row-info for which the shapes are stored

    friend class wxFrameLayout;

public: /* protected really (accessed only by plugins) */

        // Returns the row info for a row index. Internal function called by plugins.

    cbRowInfo* GetRow( int row );

        // Returns the row index for the given row info.  Internal function called by plugins.

    int GetRowIndex( cbRowInfo* pRow );

        // Returns the row at the given vertical position.
        // Returns -1 if the row is not present at given vertical position.
        // Internal function called by plugins.

    int     GetRowAt( int paneY );

        // Returns the row between the given vertical positions.
        // Returns -1 if the row is not present.
        // Internal function called by plugins.

    int     GetRowAt( int upperY, int lowerY );

        // Sets up flags in the row information structure, so that
        // they match the changed state of row items correctly.
        // Internal function called by plugins.

    void SyncRowFlags( cbRowInfo* pRow );

        // Returns true if the bar's dimension information indicates a fixed size.
        // Internal function called by plugins.

    bool IsFixedSize( cbBarInfo* pInfo );

        // Returns the number of bars whose size is not fixed.
        // Internal function called by plugins.

    int  GetNotFixedBarsCount( cbRowInfo* pRow );

        // Gets the vertical position at the given row.
        // Internal function called by plugins.

    int GetRowY( cbRowInfo* pRow );

        // Returns true if there are any variable-sized rows above this one.
        // Internal function called by plugins.

    bool HasNotFixedRowsAbove( cbRowInfo* pRow );

        // Returns true if there are any variable-sized rows below this one.
        // Internal function called by plugins.

    bool HasNotFixedRowsBelow( cbRowInfo* pRow );

        // Returns true if there are any variable-sized rows to the left of this one.
        // Internal function called by plugins.

    bool HasNotFixedBarsLeft ( cbBarInfo* pBar );

        // Returns true if there are any variable-sized rows to the right of this one.
        // Internal function called by plugins.

    bool HasNotFixedBarsRight( cbBarInfo* pBar );

        // Calculate lengths.
        // Internal function called by plugins.

    virtual void CalcLengthRatios( cbRowInfo* pInRow );

        // Generates a cbLayoutRowEvent event to recalculate row layouts.
        // Internal function called by plugins.

    virtual void RecalcRowLayout( cbRowInfo* pRow );

        // Expands the bar.
        // Internal function called by plugins.

    virtual void ExpandBar( cbBarInfo* pBar );

        // Contracts the bar.
        // Internal function called by plugins.

    virtual void ContractBar( cbBarInfo* pBar );

        // Sets up links between bars.
        // Internal function called by plugins.

    void InitLinksForRow( cbRowInfo* pRow );

        // Sets up links between bars.
        // Internal function called by plugins.

    void InitLinksForRows();

        // Coordinate translation between parent's frame and this pane.
        // Internal function called by plugins.

    void FrameToPane( int* x, int* y );

        // Coordinate translation between parent's frame and this pane.
        // Internal function called by plugins.

    void PaneToFrame( int* x, int* y );

        // Coordinate translation between parent's frame and this pane.
        // Internal function called by plugins.

    void FrameToPane( wxRect* pRect );

        // Coordinate translation between parent's frame and this pane.
        // Internal function called by plugins.

    void PaneToFrame( wxRect* pRect );

        // Returns true if pos is within the given rectangle.
        // Internal function called by plugins.

    inline bool HasPoint( const wxPoint& pos, int x, int y, int width, int height );

        // Returns the minimal row height for the given row.
        // Internal function called by plugins.

    int GetMinimalRowHeight( cbRowInfo* pRow );

        // Sets the row height for the given height. newHeight includes the height of row handles, if present.
        // Internal function called by plugins.

    void SetRowHeight( cbRowInfo* pRow, int newHeight );

        // Inserts the bar at the given row number.
        // Internal function called by plugins.

    void DoInsertBar( cbBarInfo* pBar, int rowNo );

public: /* protected really (accessed only by plugins) */

        // Generates a cbDrawBarDecorEvent and sends it to the layout to paint the bar decorations.
        // Internal function called by plugins.

    virtual void PaintBarDecorations( cbBarInfo* pBar, wxDC& dc );

        // Generates a cbDrawBarHandlesEvent and sends it to the layout to paint the bar handles.
        // Internal function called by plugins.

    virtual void PaintBarHandles( cbBarInfo* pBar, wxDC& dc );

        // Calls PaintBarDecorations and PaintBarHandles.
        // Internal function called by plugins.

    virtual void PaintBar( cbBarInfo* pBar, wxDC& dc );

        // Generates cbDrawRowHandlesEvent and cbDrawRowDecorEvent and sends them to the layout.
        // Internal function called by plugins.

    virtual void PaintRowHandles( cbRowInfo* pRow, wxDC& dc );

        // Generates cbDrawRowBkGroundEvent and sends it to the layout.
        // Internal function called by plugins.

    virtual void PaintRowBackground ( cbRowInfo* pRow, wxDC& dc );

        // Calls PaintBarDecorations for each row.
        // Internal function called by plugins.

    virtual void PaintRowDecorations( cbRowInfo* pRow, wxDC& dc );

        // Calls PaintRowBackground, PaintRowDecorations, PaintRowHandles.
        // Internal function called by plugins.

    virtual void PaintRow( cbRowInfo* pRow, wxDC& dc );

        // Generates cbDrawPaneBkGroundEvent and sends it to the layout.
        // Internal function called by plugins.

    virtual void PaintPaneBackground( wxDC& dc );

        // Generates cbDrawPaneDecorEvent and sends it to the layout.
        // Internal function called by plugins.

    virtual void PaintPaneDecorations( wxDC& dc );

        // Paints the pane background, the row background and decorations,
        // and finally the pane decorations.
        // Internal function called by plugins.

    virtual void PaintPane( wxDC& dc );

        // Generates a cbSizeBarWndEvent and sends it to the layout.
        // Internal function called by plugins.

    virtual void SizeBar( cbBarInfo* pBar );

        // Calls SizeBar for each bar in the row.
        // Internal function called by plugins.

    virtual void SizeRowObjects( cbRowInfo* pRow );

        // Calls SizeRowObjects for each row.
        // Internal function called by plugins.

    virtual void SizePaneObjects();

        // Generates cbStartDrawInAreaEvent and sends it to the layout.
        // Internal function called by plugins.

    virtual wxDC* StartDrawInArea ( const wxRect& area );

        // Generates cbFinishDrawInAreaEvent and sends it to the layout.
        // Internal function called by plugins.

    virtual void  FinishDrawInArea( const wxRect& area );

public: /* public members */

        // Default constructor.

    cbDockPane(void);

        // Constructor, taking alignment and layout panel.

    cbDockPane( int alignment, wxFrameLayout* pPanel );

        // Sets pane's margins in frame's coordinate orientations.

    void SetMargins( int top, int bottom, int left, int right );

        // Destructor.

    virtual ~cbDockPane();

        // Removes the bar from this pane. Does not destroy the bar.

    virtual void RemoveBar( cbBarInfo* pBar );

        // Inserts the bar into this pane. rect is given in the parent frame's coordinates.

    virtual void InsertBar( cbBarInfo* pBar, const wxRect& rect );

        // Inserts the bar into the given row, with dimensions and position
        // stored in pBarInfo->mBounds. Returns the node of inserted bar.

    virtual void InsertBar( cbBarInfo* pBar, cbRowInfo* pIntoRow );

        // Inserts bar and sets its position according to the preferred settings
        // given in pBarInfo.

    virtual void InsertBar( cbBarInfo* pBarInfo );

        // Removes the row from this pane. Does not destroy the row object.

    virtual void RemoveRow( cbRowInfo* pRow );

        // Inserts a row. Does not refresh the inserted row immediately.
        // If pBeforeRowNode is NULL, the row is appended to the end of pane's row list.

    virtual void InsertRow( cbRowInfo* pRow, cbRowInfo* pBeforeRow );

        // Sets pane's width in the pane's coordinates (including margins).

    void SetPaneWidth(int width);

        // Set the position and dimensions of the pane in the parent frame's coordinates.

    void SetBoundsInParent( const wxRect& rect );

        // Returns the bounds of the pane, in parent coordinates.

    inline wxRect& GetRealRect() { return mBoundsInParent; }

        // Returns an array of rows. Used by updates-managers.

    inline RowArrayT& GetRowList() { return mRows; }

        // Returns the first row.

    inline cbRowInfo* GetFirstRow()

        { return mRows.GetCount() ? mRows[0] : NULL; }

        // Returns true if the given bar is present in this pane.

    bool BarPresent( cbBarInfo* pBar );

        // Returns the height in the pane's coordinates.

    int GetPaneHeight();

        // Returns the alignment for this pane. The value is one of
        // FL_ALIGN_TOP, FL_ALIGN_BOTTOM, FL_ALIGN_LEFT, FL_ALIGN_RIGHT.

    int GetAlignment();

        // Returns true if the given mask matches the pane's mask.

    bool MatchesMask( int paneMask );

        // Returns true if the pane is aligned to the top or bottom.

    inline bool IsHorizontal()
    {
        return (mAlignment == FL_ALIGN_TOP ||
                mAlignment == FL_ALIGN_BOTTOM );
    }

        // Generates events to perform layout calculations.

    virtual void RecalcLayout();

        // Returns wxCBAR_DOCKED_HORIZONTALLY if the alignment is top or bottom,
        // or wxCBAR_DOCKED_VERTICALLY otherwise.

    virtual int GetDockingState();

        // Returns the result of hit-testing items in the pane.
        // See CB_HITTEST_RESULT enumerated type.
        // pos is the position in this pane's coordinates.

    virtual int HitTestPaneItems( const wxPoint& pos,
                                  cbRowInfo**    ppRow,
                                  cbBarInfo**    ppBar
                                );

        // Returns the bar's resize range.

    void GetBarResizeRange( cbBarInfo* pBar, int* from, int *till, bool forLeftHandle );

        // Returns the row's resize range.

    void GetRowResizeRange( cbRowInfo* pRow, int* from, int* till, bool forUpperHandle );

        // Finds the bar information by corresponding window.

    cbBarInfo* GetBarInfoByWindow( wxWindow* pBarWnd );

public: /* protected really (accessed only by plugins) */

        // Row/bar resizing related helper-method.

    void DrawVertHandle ( wxDC& dc, int x, int y, int height );

        // Row/bar resizing related helper-method.

    void DrawHorizHandle( wxDC& dc, int x, int y, int width  );

        // Row/bar resizing related helper-method.

    void ResizeRow( cbRowInfo* pRow, int ofs, bool forUpperHandle );

        // Row/bar resizing related helper-method.

    void ResizeBar( cbBarInfo* pBar, int ofs, bool forLeftHandle );

        // Returns row shape data.
        // cbBarShapeData objects will be added to the given pLst.
        // cbBarShapeData is used for storing the original bar's positions in the row,
        // when the 'non-destructive-friction' option is turned on.

    void GetRowShapeData( cbRowInfo* pRow, wxList* pLst );

        // Sets the shape data for the given row, using the data provided in pLst.
        // cbBarShapeData is used for storing the original bar's positions in the row,
        // when the 'non-destructive-friction' option is turned on.

    void SetRowShapeData( cbRowInfo* pRowNode, wxList* pLst );
};

/*
This class declares an abstract interface for optimized logic that should refresh
areas of frame layout that actually need to be updated. This should be extended in future
to implement a custom updating strategy.
*/

class WXDLLIMPEXP_FL cbUpdatesManagerBase : public wxObject
{
    DECLARE_ABSTRACT_CLASS( cbUpdatesManagerBase )

public: /* protected really, accessed by serializer (if any) */

    wxFrameLayout* mpLayout;

public:
        // Default constructor

    cbUpdatesManagerBase(void)
        : mpLayout( 0 ) {}

        // Constructor taking layout panel.

    cbUpdatesManagerBase( wxFrameLayout* pPanel )
        : mpLayout( pPanel ) {}

        // Destructor.

    virtual ~cbUpdatesManagerBase() {}

        // Sets the associated layout.

    void SetLayout( wxFrameLayout* pLayout ) { mpLayout = pLayout; }

        // This function receives a notification from the frame layout (in the order in which
        // they would usually be invoked). Custom updates-managers may utilize
        // these notifications to implement a more fine-grained updating strategy.

    virtual void OnStartChanges() = 0;

        // This function receives a notification from the frame layout (in the order in which
        // they would usually be invoked). Custom updates-managers may utilize
        // these notifications to implement a more fine-grained updating strategy.

    virtual void OnRowWillChange( cbRowInfo* WXUNUSED(pRow), cbDockPane* WXUNUSED(pInPane) ) {}

        // This function receives a notification from the frame layout (in the order in which
        // they would usually be invoked). Custom updates-managers may utilize
        // these notifications to implement a more fine-grained updating strategy.

    virtual void OnBarWillChange( cbBarInfo* WXUNUSED(pBar), cbRowInfo* WXUNUSED(pInRow), cbDockPane* WXUNUSED(pInPane) ) {}

        // This function receives a notification from the frame layout (in the order in which
        // they would usually be invoked). Custom updates-managers may utilize
        // these notifications to implement a more fine-grained updating strategy.

    virtual void OnPaneMarginsWillChange( cbDockPane* WXUNUSED(pPane) ) {}

        // This function receives a notification from the frame layout (in the order in which
        // they would usually be invoked). Custom updates-managers may utilize
        // these notifications to implement a more fine-grained updating strategy.

    virtual void OnPaneWillChange( cbDockPane* WXUNUSED(pPane) ) {}

        // This function receives a notification from the frame layout (in the order in which
        // they would usually be invoked). Custom updates-managers may utilize
        // these notifications to implement a more fine-grained updating strategy.

    virtual void OnFinishChanges() {}

        // Refreshes parts of the frame layout that need an update.

    virtual void UpdateNow() = 0;
};

/*
Base class for all control-bar plugin events.
This is not a dynamically-creatable class.
*/

class cbPluginEvent : public wxEvent
{
public:
        // NULL if event is not addressed to any specific pane.

    cbDockPane* mpPane;

        // Not used, but required.

    virtual wxEvent* Clone() const { return NULL; }

        // Constructor, taking event type and pane.

    cbPluginEvent( wxEventType eventType, cbDockPane* pPane )
        : mpPane( pPane )

        { m_eventType = eventType; }
};

// event types handled by plugins

extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_LEFT_DOWN;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_LEFT_UP;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_RIGHT_DOWN;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_RIGHT_UP;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_MOTION;

extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_LEFT_DCLICK;

extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_LAYOUT_ROW;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_RESIZE_ROW;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_LAYOUT_ROWS;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_INSERT_BAR;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_RESIZE_BAR;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_REMOVE_BAR;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_SIZE_BAR_WND;

extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_DRAW_BAR_DECOR;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_DRAW_ROW_DECOR;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_DRAW_PANE_DECOR;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_DRAW_BAR_HANDLES;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_DRAW_ROW_HANDLES;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_DRAW_ROW_BKGROUND;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_DRAW_PANE_BKGROUND;

extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_START_BAR_DRAGGING;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_DRAW_HINT_RECT;

extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_START_DRAW_IN_AREA;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_FINISH_DRAW_IN_AREA;

extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_CUSTOMIZE_BAR;
extern WXDLLIMPEXP_FL wxEventType cbEVT_PL_CUSTOMIZE_LAYOUT;

extern WXDLLIMPEXP_FL wxEventType wxCUSTOM_CB_PLUGIN_EVENTS_START_AT;

// Forward declarations, separated by categories.

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

// Definitions for for handler-methods.

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

// Macros for creating event table entries for plugin-events.

#define EVT_PL_LEFT_DOWN(func)   wxEventTableEntry( cbEVT_PL_LEFT_DOWN,   -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbLeftDownHandler   ) & func, (wxObject *) NULL ),
#define EVT_PL_LEFT_UP(func)     wxEventTableEntry( cbEVT_PL_LEFT_UP,     -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbLeftUpHandler     ) & func, (wxObject *) NULL ),
#define EVT_PL_RIGHT_DOWN(func)  wxEventTableEntry( cbEVT_PL_RIGHT_DOWN,  -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbRightDownHandler  ) & func, (wxObject *) NULL ),
#define EVT_PL_RIGHT_UP(func)    wxEventTableEntry( cbEVT_PL_RIGHT_UP,    -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbRightUpHandler    ) & func, (wxObject *) NULL ),
#define EVT_PL_MOTION(func)      wxEventTableEntry( cbEVT_PL_MOTION,      -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbMotionHandler     ) & func, (wxObject *) NULL ),
#define EVT_PL_LEFT_DCLICK(func) wxEventTableEntry( cbEVT_PL_LEFT_DCLICK, -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbLeftDClickHandler ) & func, (wxObject *) NULL ),

#define EVT_PL_LAYOUT_ROW(func)   wxEventTableEntry( cbEVT_PL_LAYOUT_ROW,   -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbLayoutRowHandler  ) & func, (wxObject *) NULL ),
#define EVT_PL_RESIZE_ROW(func)   wxEventTableEntry( cbEVT_PL_RESIZE_ROW,   -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbResizeRowHandler  ) & func, (wxObject *) NULL ),
#define EVT_PL_LAYOUT_ROWS(func)  wxEventTableEntry( cbEVT_PL_LAYOUT_ROWS,  -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbLayoutRowsHandler ) & func, (wxObject *) NULL ),
#define EVT_PL_INSERT_BAR(func)   wxEventTableEntry( cbEVT_PL_INSERT_BAR,   -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbInsertBarHandler  ) & func, (wxObject *) NULL ),
#define EVT_PL_RESIZE_BAR(func)   wxEventTableEntry( cbEVT_PL_RESIZE_BAR,   -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbResizeBarHandler  ) & func, (wxObject *) NULL ),
#define EVT_PL_REMOVE_BAR(func)   wxEventTableEntry( cbEVT_PL_REMOVE_BAR,   -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbRemoveBarHandler  ) & func, (wxObject *) NULL ),
#define EVT_PL_SIZE_BAR_WND(func) wxEventTableEntry( cbEVT_PL_SIZE_BAR_WND, -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbSizeBarWndHandler ) & func, (wxObject *) NULL ),

#define EVT_PL_DRAW_BAR_DECOR(func)     wxEventTableEntry( cbEVT_PL_DRAW_BAR_DECOR,     -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawBarDecorHandler    ) & func, (wxObject *) NULL ),
#define EVT_PL_DRAW_ROW_DECOR(func)     wxEventTableEntry( cbEVT_PL_DRAW_ROW_DECOR,     -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawRowDecorHandler    ) & func, (wxObject *) NULL ),
#define EVT_PL_DRAW_PANE_DECOR(func)    wxEventTableEntry( cbEVT_PL_DRAW_PANE_DECOR,    -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawPaneDecorHandler   ) & func, (wxObject *) NULL ),
#define EVT_PL_DRAW_BAR_HANDLES(func)   wxEventTableEntry( cbEVT_PL_DRAW_BAR_HANDLES,   -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawBarHandlesHandler  ) & func, (wxObject *) NULL ),
#define EVT_PL_DRAW_ROW_HANDLES(func)   wxEventTableEntry( cbEVT_PL_DRAW_ROW_HANDLES,   -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawRowHandlesHandler  ) & func, (wxObject *) NULL ),
#define EVT_PL_DRAW_ROW_BKGROUND(func)  wxEventTableEntry( cbEVT_PL_DRAW_ROW_BKGROUND,  -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawRowBkGroundHandler ) & func, (wxObject *) NULL ),
#define EVT_PL_DRAW_PANE_BKGROUND(func) wxEventTableEntry( cbEVT_PL_DRAW_PANE_BKGROUND, -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawPaneBkGroundHandler) & func, (wxObject *) NULL ),

#define EVT_PL_START_BAR_DRAGGING(func) wxEventTableEntry( cbEVT_PL_START_BAR_DRAGGING, -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbStartBarDraggingHandler) & func, (wxObject *) NULL ),
#define EVT_PL_DRAW_HINT_RECT(func)     wxEventTableEntry( cbEVT_PL_DRAW_HINT_RECT,     -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbDrawHintRectHandler    ) & func, (wxObject *) NULL ),


#define EVT_PL_START_DRAW_IN_AREA(func)  wxEventTableEntry( cbEVT_PL_START_DRAW_IN_AREA,  -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbStartDrawInAreaHandler)  & func, (wxObject *) NULL ),
#define EVT_PL_FINISH_DRAW_IN_AREA(func) wxEventTableEntry( cbEVT_PL_FINISH_DRAW_IN_AREA, -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbFinishDrawInAreaHandler) & func, (wxObject *) NULL ),

#define EVT_PL_CUSTOMIZE_BAR(func)       wxEventTableEntry( cbEVT_PL_CUSTOMIZE_BAR,       -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbCustomizeBarHandler)     & func, (wxObject *) NULL ),
#define EVT_PL_CUSTOMIZE_LAYOUT(func)    wxEventTableEntry( cbEVT_PL_CUSTOMIZE_LAYOUT,    -1, -1, (wxObjectEventFunction) (wxEventFunction) (cbCustomizeLayoutHandler)  & func, (wxObject *) NULL ),

/*
Abstract base class for all control-bar related plugins.
Note: pointer positions of mouse events sent to plugins
are always in the pane's coordinates (the pane to which
this plugin is hooked).
*/

class cbPluginBase : public wxEvtHandler
{
    DECLARE_ABSTRACT_CLASS( cbPluginBase )
public:
        // Back-reference to the frame layout.

    wxFrameLayout* mpLayout;

        // Specifies panes for which this plugin receives events
        // (see pane masks definitions).

    int            mPaneMask;

        // Is true when plugin is ready to handle events.

    bool           mIsReady;

public:
        // Default constructor.

    cbPluginBase()

        : mpLayout  ( 0 ),
          mPaneMask( wxALL_PANES ),
          mIsReady ( false )
    {}

        // Constructor taking layout panel and a mask.

    cbPluginBase( wxFrameLayout* pPanel, int paneMask = wxALL_PANES )

        : mpLayout  ( pPanel ),
          mPaneMask( paneMask ),
          mIsReady ( false )
    {}

        // Returns the pane mask.

    inline int GetPaneMask() { return mPaneMask; }

        // Destructor. Destroys the whole plugin chain of connected plugins.

    virtual ~cbPluginBase();

        // Override this method to do plugin-specific initialization.
        // At this point plugin is already attached to the frame layout,
        // and pane masks are set.

    virtual void OnInitPlugin() { mIsReady = true; }

        // Returns true if the plugin is ready to receive events.

    bool IsReady() { return mIsReady; }

        // Overridden to determine whether the target pane specified in the
        // event matches the pane mask of this plugin (specific plugins
        // do not override this method).

    virtual bool ProcessEvent(wxEvent& event);
};

/*
Class for mouse left down events.
*/

class cbLeftDownEvent : public cbPluginEvent
{
public:
    wxPoint mPos;

        // Constructor, taking mouse position and pane.

    cbLeftDownEvent( const wxPoint& pos, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_LEFT_DOWN, pPane ),
          mPos( pos )
    {}
};

/*
Class for mouse left up events.
*/

class cbLeftUpEvent : public cbPluginEvent
{
public:
    wxPoint mPos;

        // Constructor, taking mouse position and pane.

    cbLeftUpEvent( const wxPoint& pos, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_LEFT_UP, pPane ),
          mPos( pos )
    {}
};

/*
Class for mouse right down events.
*/

class cbRightDownEvent : public cbPluginEvent
{
public:
    wxPoint mPos;

        // Constructor, taking mouse position and pane.

    cbRightDownEvent( const wxPoint& pos, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_RIGHT_DOWN, pPane ),
          mPos( pos )
    {}
};

/*
Class for mouse right up events.
*/

class cbRightUpEvent : public cbPluginEvent
{
public:
    wxPoint mPos;

        // Constructor, taking mouse position and pane.

    cbRightUpEvent( const wxPoint& pos, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_RIGHT_UP, pPane ),
          mPos( pos )
    {}
};

/*
Class for mouse motion events.
*/

class cbMotionEvent : public cbPluginEvent
{
public:
    wxPoint mPos;

        // Constructor, taking mouse position and pane.

    cbMotionEvent( const wxPoint& pos, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_MOTION, pPane ),
          mPos( pos )
    {}
};

/*
Class for mouse left double click events.
*/

class cbLeftDClickEvent : public cbPluginEvent
{
public:
    wxPoint mPos;

        // Constructor, taking mouse position and pane.

    cbLeftDClickEvent( const wxPoint& pos, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_LEFT_DCLICK, pPane ),
          mPos( pos )
    {}
};

/*
Class for single row layout events.
*/

class cbLayoutRowEvent : public cbPluginEvent
{
public:
    cbRowInfo* mpRow;

        // Constructor, taking row information and pane.

    cbLayoutRowEvent( cbRowInfo* pRow, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_LAYOUT_ROW, pPane ),
          mpRow( pRow )
    {}
};

/*
Class for row resize events.
*/

class cbResizeRowEvent : public cbPluginEvent
{
public:
    cbRowInfo* mpRow;
    int        mHandleOfs;
    bool       mForUpperHandle;

        // Constructor, taking row information, two parameters of currently unknown use, and pane.

    cbResizeRowEvent( cbRowInfo* pRow, int handleOfs, bool forUpperHandle, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_RESIZE_ROW, pPane ),
          mpRow( pRow ),
          mHandleOfs( handleOfs ),
          mForUpperHandle( forUpperHandle )
    {}
};

/*
Class for multiple rows layout events.
*/

class cbLayoutRowsEvent : public cbPluginEvent
{
public:

        // Constructor, taking pane.

    cbLayoutRowsEvent( cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_LAYOUT_ROWS, pPane )
    {}
};

/*
Class for bar insertion events.
*/

class cbInsertBarEvent : public cbPluginEvent
{
public:
    cbBarInfo*  mpBar;
    cbRowInfo*  mpRow;

        // Constructor, taking bar information, row information, and pane.

    cbInsertBarEvent( cbBarInfo* pBar, cbRowInfo* pIntoRow, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_INSERT_BAR, pPane ),

          mpBar( pBar     ),
          mpRow( pIntoRow )
    {}
};

/*
Class for bar resize events.
*/

class cbResizeBarEvent : public cbPluginEvent
{
public:
    cbBarInfo* mpBar;
    cbRowInfo* mpRow;

        // Constructor, taking bar information, row information, and pane.

    cbResizeBarEvent( cbBarInfo* pBar, cbRowInfo* pRow, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_RESIZE_BAR, pPane ),
          mpBar( pBar ),
          mpRow( pRow )
    {}
};

/*
Class for bar removal events.
*/

class cbRemoveBarEvent : public cbPluginEvent
{
public:
    cbBarInfo* mpBar;

        // Constructor, taking bar information and pane.

    cbRemoveBarEvent( cbBarInfo* pBar, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_REMOVE_BAR, pPane ),
          mpBar( pBar )
    {}
};

/*
Class for bar window resize events.
*/

class cbSizeBarWndEvent : public cbPluginEvent
{
public:
    cbBarInfo* mpBar;
    wxRect     mBoundsInParent;

        // Constructor, taking bar information and pane.

    cbSizeBarWndEvent( cbBarInfo* pBar, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_SIZE_BAR_WND, pPane ),
          mpBar( pBar ),
          mBoundsInParent( pBar->mBoundsInParent )
    {}
};

/*
Class for bar decoration drawing events.
*/

class cbDrawBarDecorEvent : public cbPluginEvent
{
public:
    cbBarInfo* mpBar;
    wxDC*      mpDc;
    wxRect     mBoundsInParent;

        // Constructor, taking bar information, device context, and pane.

    cbDrawBarDecorEvent( cbBarInfo* pBar, wxDC& dc, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_DRAW_BAR_DECOR, pPane ),
          mpBar( pBar ),
          mpDc( &dc ),
          mBoundsInParent( pBar->mBoundsInParent )
    {}
};

/*
Class for row decoration drawing events.
*/

class cbDrawRowDecorEvent : public cbPluginEvent
{
public:
    cbRowInfo* mpRow;
    wxDC*      mpDc;

        // Constructor, taking row information, device context, and pane.

    cbDrawRowDecorEvent( cbRowInfo* pRow, wxDC& dc, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_DRAW_ROW_DECOR, pPane ),
          mpRow( pRow ),
          mpDc( &dc )
    {}
};

/*
Class for pane decoration drawing events.
*/

class cbDrawPaneDecorEvent : public cbPluginEvent
{
public:
    wxDC* mpDc;

        // Constructor, taking device context and pane.

    cbDrawPaneDecorEvent( wxDC& dc, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_DRAW_PANE_DECOR, pPane ),
          mpDc( &dc )
    {}
};

/*
Class for bar handles drawing events.
*/

class cbDrawBarHandlesEvent : public cbPluginEvent
{
public:
    cbBarInfo* mpBar;
    wxDC*   mpDc;

        // Constructor, taking bar information, device context, and pane.

    cbDrawBarHandlesEvent( cbBarInfo* pBar, wxDC& dc, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_DRAW_BAR_HANDLES, pPane ),
          mpBar( pBar ),
          mpDc( &dc )
    {}
};

/*
Class for row handles drawing events.
*/

class cbDrawRowHandlesEvent : public cbPluginEvent
{
public:
    cbRowInfo* mpRow;
    wxDC*      mpDc;

        // Constructor, taking row information, device context, and pane.

    cbDrawRowHandlesEvent( cbRowInfo* pRow, wxDC& dc, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_DRAW_ROW_HANDLES, pPane ),
          mpRow( pRow ),
          mpDc( &dc )
    {}
};

/*
Class for row background drawing events.
*/

class cbDrawRowBkGroundEvent : public cbPluginEvent
{
public:
    cbRowInfo* mpRow;
    wxDC*   mpDc;

        // Constructor, taking row information, device context, and pane.

    cbDrawRowBkGroundEvent( cbRowInfo* pRow, wxDC& dc, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_DRAW_ROW_BKGROUND, pPane ),
          mpRow( pRow ),
          mpDc( &dc )
    {}
};

/*
Class for pane background drawing events.
*/

class cbDrawPaneBkGroundEvent : public cbPluginEvent
{
public:
    wxDC* mpDc;

        // Constructor, taking device context and pane.

    cbDrawPaneBkGroundEvent( wxDC& dc, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_DRAW_PANE_BKGROUND, pPane ),
          mpDc( &dc )
    {}
};

/*
Class for start-bar-dragging events.
*/

class cbStartBarDraggingEvent : public cbPluginEvent
{
public:
    cbBarInfo* mpBar;
    wxPoint    mPos;  // is given in frame's coordinates

        // Constructor, taking bar information, mouse position, and pane.

    cbStartBarDraggingEvent( cbBarInfo* pBar, const wxPoint& pos, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_START_BAR_DRAGGING, pPane ),
          mpBar( pBar ),
          mPos( pos )
    {}
};

/*
Class for hint-rectangle drawing events.
*/

class cbDrawHintRectEvent : public cbPluginEvent
{
public:
    wxRect mRect;       // is given in frame's coordinates


    bool   mLastTime;  // indicates that this event finishes "session" of on-screen drawing,
                       // thus associated resources can be freed now
    bool   mEraseRect; // does not have any impact, if recangle is drawn using XOR-mask

    bool   mIsInClient;// in cleint area hint could be drawn differently,
                       // e.g. with fat/hatched border


        // Constructor, taking hint rectangle and three flags.

    cbDrawHintRectEvent( const wxRect& rect, bool isInClient, bool eraseRect, bool lastTime )

        : cbPluginEvent( cbEVT_PL_DRAW_HINT_RECT, 0 ),
          mRect      ( rect       ),
          mLastTime  ( lastTime   ),
          mEraseRect ( eraseRect  ),
          mIsInClient( isInClient )
    {}
};

/*
Class for start drawing in area events.
*/

class cbStartDrawInAreaEvent : public cbPluginEvent
{
public:
    wxRect mArea;
    wxDC** mppDc; // points to pointer, where the reference
                  // to the obtained buffer-context should be placed

        // Constructor, taking rectangular area, device context pointer to a pointer, and pane.

    cbStartDrawInAreaEvent( const wxRect& area, wxDC** ppDCForArea, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_START_DRAW_IN_AREA, pPane ),
          mArea( area ),
          mppDc( ppDCForArea )
    {}
};

/*
Class for finish drawing in area events.
*/

class cbFinishDrawInAreaEvent : public cbPluginEvent
{
public:
    wxRect mArea;

        // Constructor, taking rectangular area and pane.

    cbFinishDrawInAreaEvent( const wxRect& area, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_FINISH_DRAW_IN_AREA, pPane ),
          mArea( area )
    {}
};

/*
Class for bar customization events.
*/

class cbCustomizeBarEvent : public cbPluginEvent
{
public:
    wxPoint    mClickPos; // in parent frame's coordinates
    cbBarInfo* mpBar;

        // Constructor, taking bar information, mouse position, and pane.

    cbCustomizeBarEvent( cbBarInfo* pBar, const wxPoint& clickPos, cbDockPane* pPane )

        : cbPluginEvent( cbEVT_PL_CUSTOMIZE_BAR, pPane ),
          mClickPos( clickPos ),
          mpBar( pBar )
    {}
};

/*
Class for layout customization events.
*/

class cbCustomizeLayoutEvent : public cbPluginEvent
{
public:
    wxPoint mClickPos; // in parent frame's coordinates

        // Constructor, taking mouse position.

    cbCustomizeLayoutEvent( const wxPoint& clickPos )

        : cbPluginEvent( cbEVT_PL_CUSTOMIZE_LAYOUT, 0 ),
          mClickPos( clickPos )
    {}
};

#endif /* __CONTROLBAR_G__ */

