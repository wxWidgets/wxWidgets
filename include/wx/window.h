///////////////////////////////////////////////////////////////////////////////
// Name:        window.h
// Purpose:     wxWindowBase class - the interface of wxWindowBase
// Author:      Vadim Zeitlin
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINDOW_H_BASE_
#define _WX_WINDOW_H_BASE_

#ifdef __GNUG__
    #pragma interface "windowbase.h"
#endif

// ----------------------------------------------------------------------------
// headers which we must include here
// ----------------------------------------------------------------------------

#include "wx/event.h"           // the base class

#include "wx/list.h"            // defines wxWindowList

#include "wx/cursor.h"          // we have member variables of these classes
#include "wx/font.h"            // so we can't do without them
#include "wx/colour.h"
#include "wx/region.h"
#include "wx/accel.h"

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxClientData;
class WXDLLEXPORT wxControl;
class WXDLLEXPORT wxCursor;
class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxDropTarget;
class WXDLLEXPORT wxItemResource;
class WXDLLEXPORT wxLayoutConstraints;
class WXDLLEXPORT wxResourceTable;
class WXDLLEXPORT wxSizer;
class WXDLLEXPORT wxToolTip;
class WXDLLEXPORT wxValidator;
class WXDLLEXPORT wxWindowBase;
class WXDLLEXPORT wxWindow;

// ----------------------------------------------------------------------------
// (pseudo)template list classes
// ----------------------------------------------------------------------------

WX_DECLARE_LIST_3(wxWindow, wxWindowBase, wxWindowList, wxWindowListNode);

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern wxWindowList) wxTopLevelWindows;

// ----------------------------------------------------------------------------
// helper classes used by [SG]etClientObject/Data
//
// TODO move into a separate header?
// ----------------------------------------------------------------------------

class wxClientData
{
public:
    wxClientData() { }
    virtual ~wxClientData() { }
};

class wxStringClientData : public wxClientData
{
public:
    wxStringClientData() { }
    wxStringClientData( const wxString &data ) : m_data(data) { }
    void SetData( const wxString &data ) { m_data = data; }
    const wxString& GetData() const { return m_data; }

private:
    wxString  m_data;
};

// ----------------------------------------------------------------------------
// wxWindowBase is the base class for all GUI controls/widgets, this is the public
// interface of this class.
//
// Event handler: windows have themselves as their event handlers by default,
// but their event handlers could be set to another object entirely. This
// separation can reduce the amount of derivation required, and allow
// alteration of a window's functionality (e.g. by a resource editor that
// temporarily switches event handlers).
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxWindowBase : public wxEvtHandler
{
    DECLARE_ABSTRACT_CLASS(wxWindowBase);

public:
    // creating the window
    // -------------------

        // default ctor
    wxWindowBase() { InitBase(); }

        // pseudo ctor (can't be virtual, called from ctor)
    bool CreateBase(wxWindowBase *parent,
                    wxWindowID id,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxPanelNameStr);

    virtual ~wxWindowBase();

#if wxUSE_WX_RESOURCES
    // these functions are implemented in resource.cpp and resourc2.cpp
    virtual bool LoadFromResource(wxWindow *parent,
                                  const wxString& resourceName,
                                  const wxResourceTable *table = (const wxResourceTable *) NULL);
    virtual wxControl *CreateItem(const wxItemResource* childResource,
                                  const wxItemResource* parentResource,
                                  const wxResourceTable *table = (const wxResourceTable *) NULL);
#endif // wxUSE_WX_RESOURCES

    // deleting the window
    // -------------------

        // ask the window to close itself, return TRUE if the event handler
        // honoured our request
    bool Close( bool force = FALSE );

        // the following functions delete the C++ objects (the window itself
        // or its children) as well as the GUI windows and normally should
        // never be used directly

        // delete window unconditionally (dangerous!), returns TRUE if ok
    virtual bool Destroy();
        // delete all children of this window, returns TRUE if ok
    bool DestroyChildren();

        // is the window being deleted?
    bool IsBeingDeleted() const { return m_isBeingDeleted; }

    // window attributes
    // -----------------

        // the title (or label, see below) of the window: the text which the
        // window shows
    virtual void SetTitle( const wxString & WXUNUSED(title) ) { }
    virtual wxString GetTitle() const { return ""; }

        // label is just the same as the title (but for, e.g., buttons it
        // makes more sense to speak about labels)
    void SetLabel(const wxString& label) { SetTitle(label); }
    wxString GetLabel() const { return GetTitle(); }

        // the window name is used for ressource setting in X, it is not the
        // same as the window title/label
    virtual void SetName( const wxString &name ) { m_windowName = name; }
    virtual wxString GetName() const { return m_windowName; }

        // window id uniquely identifies the window among its siblings unless
        // it is -1 which means "don't care"
    void SetId( wxWindowID id ) { m_windowId = id; }
    wxWindowID GetId() const { return m_windowId; }

        // generate a control id for the controls which were not given one by
        // user
    static int NewControlId() { return --ms_lastControlId; }
        // get the id of the control following the one with the given
        // (autogenerated) id
    static int NextControlId(int id) { return id - 1; }
        // get the id of the control preceding the one with the given
        // (autogenerated) id
    static int PrevControlId(int id) { return id + 1; }

    // moving/resizing
    // ---------------

        // set the window size and/or position
    void SetSize( int x, int y, int width, int height,
                  int sizeFlags = wxSIZE_AUTO )
        {  DoSetSize(x, y, width, height, sizeFlags); }

    void SetSize( int width, int height )
        { DoSetSize( -1, -1, width, height, wxSIZE_USE_EXISTING ); }

    void SetSize( const wxSize& size )
        { SetSize( size.x, size.y); }

    void SetSize(const wxRect& rect, int sizeFlags = wxSIZE_AUTO)
        { DoSetSize(rect.x, rect.y, rect.width, rect.height, sizeFlags); }

    void Move( int x, int y )
        { DoSetSize( x, y, -1, -1, wxSIZE_USE_EXISTING ); }

    void Move(const wxPoint& pt)
        { Move(pt.x, pt.y); }

        // Z-order
    virtual void Raise() = 0;
    virtual void Lower() = 0;

        // client size is the size of area available for subwindows
    void SetClientSize( int width, int height )
        { DoSetClientSize(width, height); }

    void SetClientSize( const wxSize& size )
        { DoSetClientSize(size.x, size.y); }

    void SetClientSize(const wxRect& rect)
        { SetClientSize( rect.width, rect.height ); }

        // get the window position and/or size (pointers may be NULL)
    void GetPosition( int *x, int *y ) const { DoGetPosition(x, y); }
    wxPoint GetPosition() const
    {
        int w, h;
        DoGetPosition(&w, &h);

        return wxPoint(w, h);
    }

    void GetSize( int *w, int *h ) const { DoGetSize(w, h); }
    wxSize GetSize() const
    {
        int w, h;
        DoGetSize(& w, & h);
        return wxSize(w, h);
    }

    wxRect GetRect() const
    {
        int x, y, w, h;
        GetPosition(& x, & y);
        GetSize(& w, & h);

        return wxRect(x, y, w, h);
    }

    void GetClientSize( int *w, int *h ) const { DoGetClientSize(w, h); }
    wxSize GetClientSize() const
    {
        int w, h;
        DoGetClientSize(& w, & h);

        return wxSize(w, h);
    }

        // centre with respect to the the parent window
    void Centre( int direction = wxHORIZONTAL );
    void Center( int direction = wxHORIZONTAL ) { Centre(direction); }

        // set window size to wrap around its children
    virtual void Fit();

        // set min/max size of the window
    virtual void SetSizeHints( int minW, int minH,
                               int maxW = -1, int maxH = -1,
                               int incW = -1, int incH = -1 );

    // window state
    // ------------

        // returns TRUE if window was shown/hidden, FALSE if the nothing was
        // done (window was already shown/hidden)
    virtual bool Show( bool show = TRUE );
    bool Hide() { return Show(FALSE); }

        // returns TRUE if window was enabled/disabled, FALSE if nothing done
    virtual bool Enable( bool enable = TRUE );
    bool Disable() { return Enable(FALSE); }

    bool IsShown() const { return m_isShown; }
    bool IsEnabled() const { return m_isEnabled; }

        // get/set window style (setting style won't update the window and so
        // is only useful for internal usage)
    virtual void SetWindowStyleFlag( long style ) { m_windowStyle = style; }
    virtual long GetWindowStyleFlag() const { return m_windowStyle; }

        // just some (somewhat shorter) synonims
    void SetWindowStyle( long style ) { SetWindowStyleFlag(style); }
    long GetWindowStyle() const { return GetWindowStyleFlag(); }

    bool HasFlag(int flag) const { return (m_windowStyle & flag) != 0; }

    virtual bool IsRetained() const
        { return (m_windowStyle & wxRETAINED) != 0; }

        // make the window modal (all other windows unresponsive)
    virtual void MakeModal(bool modal = TRUE);

    // focus handling
    // --------------

        // set focus to this window
    virtual void SetFocus() = 0;

        // return the window which currently has the focus or NULL
    static wxWindow *FindFocus() /* = 0: implement in derived classes */;

        // can this window have focus?
    virtual bool AcceptsFocus() const { return IsShown() && IsEnabled(); }

    // parent/children relations
    // -------------------------

        // get the list of children
    const wxWindowList& GetChildren() const { return m_children; }
    wxWindowList& GetChildren() { return m_children; }

        // get the parent or the parent of the parent
    wxWindow *GetParent() const { return m_parent; }
    inline wxWindow *GetGrandParent() const;

        // is this window a top level one?
    bool IsTopLevel() const { return m_parent != 0; }

        // it doesn't really change parent, use ReParent() instead
    void SetParent( wxWindowBase *parent ) { m_parent = (wxWindow *)parent; }
        // change the real parent of this window, return TRUE if the parent
        // was changed, FALSE otherwise (error or newParent == oldParent)
    virtual bool Reparent( wxWindowBase *newParent );

        // find window among the descendants of this one either by id or by
        // name (return NULL if not found)
    wxWindow *FindWindow( long id );
    wxWindow *FindWindow( const wxString& name );

        // implementation mostly
    virtual void AddChild( wxWindowBase *child );
    virtual void RemoveChild( wxWindowBase *child );

    // event handler stuff
    // -------------------

        // get the current event handler
    wxEvtHandler *GetEventHandler() const { return m_eventHandler; }

        // replace the event handler (allows to completely subclass the
        // window)
    void SetEventHandler( wxEvtHandler *handler ) { m_eventHandler = handler; }

        // push/pop event handler: allows to chain a custom event handler to
        // alreasy existing ones
    void PushEventHandler( wxEvtHandler *handler );
    wxEvtHandler *PopEventHandler( bool deleteHandler = FALSE );

    // validators and client data
    // --------------------------

        // a window may have an associated validator which is used to control
        // user input
    virtual void SetValidator( const wxValidator &validator );
    virtual wxValidator *GetValidator() { return m_windowValidator; }

        // each window may have associated client data: either a pointer to
        // wxClientData object in which case it is managed by the window (i.e.
        // it will delete the data when it's destroyed) or an untyped pointer
        // which won't be deleted by the window
    virtual void SetClientObject( wxClientData *data )
    {
        if ( m_clientObject )
            delete m_clientObject;

        m_clientObject = data;
    }
    virtual wxClientData *GetClientObject() const { return m_clientObject; }

    virtual void SetClientData( void *data ) { m_clientData = data; }
    virtual void *GetClientData() const { return m_clientData; }

    // dialog oriented functions
    // -------------------------

        // validate the correctness of input, return TRUE if ok
    virtual bool Validate();

        // transfer data between internal and GUI representations
    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();

    virtual void InitDialog();

    // accelerators
    // ------------
    virtual void SetAcceleratorTable( const wxAcceleratorTable& accel )
        { m_acceleratorTable = accel; }
    wxAcceleratorTable *GetAcceleratorTable()
        { return &m_acceleratorTable; }

    // dialog units translations
    // -------------------------

    wxPoint ConvertPixelsToDialog( const wxPoint& pt );
    wxPoint ConvertDialogToPixels( const wxPoint& pt );
    wxSize ConvertPixelsToDialog( const wxSize& sz )
    {
        wxPoint pt(ConvertPixelsToDialog(wxPoint(sz.x, sz.y)));

        return wxSize(pt.x, pt.y);
    }

    wxSize ConvertDialogToPixels( const wxSize& sz )
    {
        wxPoint pt(ConvertDialogToPixels(wxPoint(sz.x, sz.y)));

        return wxSize(pt.x, pt.y);
    }

    // mouse functions
    // ---------------

        // move the mouse to the specified position
    virtual void WarpPointer(int x, int y) = 0;

        // start or end mouse capture
    virtual void CaptureMouse() = 0;
    virtual void ReleaseMouse() = 0;

    // painting the window
    // -------------------

        // mark the specified rectangle (or the whole window) as "dirty" so it
        // will be repainted
    virtual void Refresh( bool eraseBackground = TRUE,
                          const wxRect *rect = (const wxRect *) NULL ) = 0;
        // clear the window entirely
    virtual void Clear() = 0;

        // adjust DC for drawing on this window
    virtual void PrepareDC( wxDC & WXUNUSED(dc) ) { }

        // the update region of the window contains the areas which must be
        // repainted by the program
    const wxRegion& GetUpdateRegion() const { return m_updateRegion; }
    wxRegion& GetUpdateRegion() { return m_updateRegion; }

        // these functions verify whether the given point/rectangle belongs to
        // (or at least intersects with) the update region
    bool IsExposed( int x, int y ) const;
    bool IsExposed( int x, int y, int w, int h ) const;

    bool IsExposed( const wxPoint& pt ) const
        { return IsExposed(pt.x, pt.y); }
    bool IsExposed( const wxRect& rect ) const
        { return IsExposed(rect.x, rect.y, rect.width, rect.height); }

    // colours, fonts and cursors
    // --------------------------

        // set/retrieve the window colours (system defaults are used by
        // default): Set functions return TRUE if colour was changed
    virtual bool SetBackgroundColour( const wxColour &colour );
    virtual bool SetForegroundColour( const wxColour &colour );

    wxColour GetBackgroundColour() const { return m_backgroundColour; }
    wxColour GetForegroundColour() const { return m_foregroundColour; }

        // set/retrieve the cursor for this window (SetCursor() returns TRUE
        // if the cursor was really changed)
    virtual bool SetCursor( const wxCursor &cursor );
    const wxCursor& GetCursor() const { return m_cursor; }
    wxCursor& GetCursor() { return m_cursor; }

        // set/retrieve the font for the window (SetFont() returns TRUE if the
        // font really changed)
    virtual bool SetFont( const wxFont &font ) = 0;
    const wxFont& GetFont() const { return m_font; }
    wxFont& GetFont() { return m_font; }

        // get the (average) character size for the current font
    virtual int GetCharHeight() const = 0;
    virtual int GetCharWidth() const = 0;

        // get the width/height/... of the text using current or specified
        // font
    virtual void GetTextExtent(const wxString& string,
                               int *x, int *y,
                               int *descent = (int *) NULL,
                               int *externalLeading = (int *) NULL,
                               const wxFont *theFont = (const wxFont *) NULL)
                               const = 0;

        // translate to/from screen/client coordinates (pointers may be NULL)
    virtual void ClientToScreen( int *x, int *y ) const = 0;
    virtual void ScreenToClient( int *x, int *y ) const = 0;

    // misc
    // ----

    void UpdateWindowUI();

    virtual bool PopupMenu( wxMenu *menu, int x, int y ) = 0;

    // scrollbars
    // ----------

        // configure the window scrollbars
    virtual void SetScrollbar( int orient,
                               int pos,
                               int thumbVisible,
                               int range,
                               bool refresh = TRUE ) = 0;
    virtual void SetScrollPos( int orient, int pos, bool refresh = TRUE ) = 0;
    virtual int GetScrollPos( int orient ) const = 0;
    virtual int GetScrollThumb( int orient ) const = 0;
    virtual int GetScrollRange( int orient ) const = 0;

        // scroll window to the specified position
    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = (wxRect *) NULL ) = 0;

    // tooltips
    // --------
#if wxUSE_TOOLTIPS
        // the easiest way to set a tooltip for a window is to use this method
    void SetToolTip( const wxString &tip );
        // attach a tooltip to the window
    void SetToolTip( wxToolTip *tip ) { DoSetToolTip(tip); }
        // get the associated tooltip or NULL if none
    wxToolTip* GetToolTip() const { return m_tooltip; }
#endif // wxUSE_TOOLTIPS

    // drag and drop
    // -------------
#if wxUSE_DRAG_AND_DROP
        // set/retrieve the drop target associated with this window (may be
        // NULL; it's owned by the window and will be deleted by it)
    virtual void SetDropTarget( wxDropTarget *dropTarget ) = 0;
    virtual wxDropTarget *GetDropTarget() const { return m_dropTarget; }
#endif // wxUSE_DRAG_AND_DROP

    // constraints and sizers
    // ----------------------
#if wxUSE_CONSTRAINTS
        // set the constraints for this window or retrieve them (may be NULL)
    void SetConstraints( wxLayoutConstraints *constraints );
    wxLayoutConstraints *GetConstraints() const { return m_constraints; }

        // when using constraints, it makes sense to update children positions
        // automatically whenever the window is resized - this is done if
        // autoLayout is on
    void SetAutoLayout( bool autoLayout ) { m_autoLayout = autoLayout; }
    bool GetAutoLayout() const { return m_autoLayout; }

        // do layout the window and its children
    virtual bool Layout();

        // implementation only
    void UnsetConstraints(wxLayoutConstraints *c);
    wxWindowList *GetConstraintsInvolvedIn() const
        { return m_constraintsInvolvedIn; }
    void AddConstraintReference(wxWindowBase *otherWin);
    void RemoveConstraintReference(wxWindowBase *otherWin);
    void DeleteRelatedConstraints();
    void ResetConstraints();

        // these methods may be overriden for special layout algorithms
    virtual void SetConstraintSizes(bool recurse = TRUE);
    virtual bool LayoutPhase1(int *noChanges);
    virtual bool LayoutPhase2(int *noChanges);
    virtual bool DoPhase(int);

        // these methods are virtual but normally won't be overridden
    virtual void TransformSizerToActual(int *x, int *y) const ;
    virtual void SetSizeConstraint(int x, int y, int w, int h);
    virtual void MoveConstraint(int x, int y);
    virtual void GetSizeConstraint(int *w, int *h) const ;
    virtual void GetClientSizeConstraint(int *w, int *h) const ;
    virtual void GetPositionConstraint(int *x, int *y) const ;

        // sizers
        // TODO: what are they and how do they work??
    void SetSizer( wxSizer *sizer );
    wxSizer *GetSizer() const { return m_windowSizer; }

    void SetSizerParent( wxWindowBase *win ) { m_sizerParent = win; }
    wxWindowBase *GetSizerParent() const { return m_sizerParent; }

    virtual void SizerSetSize(int x, int y, int w, int h);
    virtual void SizerMove(int x, int y);
#endif // wxUSE_CONSTRAINTS

    // backward compatibility
    // ----------------------
#if WXWIN_COMPATIBILITY
    bool Enabled() const { return IsEnabled(); }

    void SetButtonFont(const wxFont& font) { SetFont(font); }
    void SetLabelFont(const wxFont& font) { SetFont(font); }
    wxFont& GetLabelFont() { return GetFont(); };
    wxFont& GetButtonFont() { return GetFont(); };
#endif // WXWIN_COMPATIBILITY

    // implementation
    // --------------

        // event handlers
    void OnSysColourChanged( wxSysColourChangedEvent& event );
    void OnInitDialog( wxInitDialogEvent &event );

        // get the haqndle of the window for the underlying window system: this
        // is only used for wxWin itself or for user code which wants to call
        // platform-specific APIs
    virtual WXWidget GetHandle() const = 0;

protected:
    // the window id - a number which uniquely identifies a window among
    // its siblings unless it is -1
    wxWindowID           m_windowId;

    // the parent window of this window (or NULL) and the list of the children
    // of this window
    wxWindow            *m_parent;
    wxWindowList         m_children;

    // the minimal allowed size for the window (no minimal size if variable(s)
    // contain(s) -1)
    int                  m_minWidth, m_minHeight, m_maxWidth, m_maxHeight;

    // event handler for this window: usually is just 'this' but may be
    // changed with SetEventHandler()
    wxEvtHandler        *m_eventHandler;

    // associated validator or NULL if none
    wxValidator         *m_windowValidator;

#if wxUSE_DRAG_AND_DROP
    wxDropTarget        *m_dropTarget;
#endif // wxUSE_DRAG_AND_DROP

    // visual window attributes
    wxCursor             m_cursor;
    wxFont               m_font;
    wxColour             m_backgroundColour, m_foregroundColour;

    // the region which should be repainted in response to paint event
    wxRegion             m_updateRegion;

    // the accelerator table for the window which translates key strokes into
    // command events
    wxAcceleratorTable   m_acceleratorTable;

    // user data associated with the window: either an object which will be
    // deleted by the window when it's deleted or some raw pointer which we do
    // nothing with
    wxClientData        *m_clientObject;
    void                *m_clientData;

    // the tooltip for this window (may be NULL)
#if wxUSE_TOOLTIPS
    wxToolTip           *m_tooltip;
#endif // wxUSE_TOOLTIPS

    // constraints and sizers
#if wxUSE_CONSTRAINTS
    // the constraints for this window or NULL
    wxLayoutConstraints *m_constraints;

    // constraints this window is involved in
    wxWindowList        *m_constraintsInvolvedIn;

    // top level and the parent sizers
    // TODO what's this and how does it work?)
    wxSizer             *m_windowSizer;
    wxWindowBase        *m_sizerParent;

    // Layout() window automatically when its size changes?
    bool                 m_autoLayout:1;
#endif // wxUSE_CONSTRAINTS

    // window state
    bool                 m_isShown:1;
    bool                 m_isEnabled:1;
    bool                 m_isBeingDeleted:1;

    // window attributes
    long                 m_windowStyle;
    wxString             m_windowName;

protected:
    // common part of all ctors: it is not virtual because it is called from
    // ctor
    void InitBase();

    // get the default size for the new window if no explicit size given
    // FIXME why 20 and not 30, 10 or ...?
    static int WidthDefault(int w) { return w == -1 ? 20 : w; }
    static int HeightDefault(int h) { return h == -1 ? 20 : h; }

    // more pure virtual functions
    // ---------------------------

    // NB: we must have DoSomething() function when Something() is an overloaded
    //     method: indeed, we can't just have "virtual Something()" in case when
    //     the function is overloaded because then we'd have to make virtual all
    //     the variants (otherwise only the virtual function may be called on a
    //     pointer to derived class according to C++ rules) which is, in
    //     general, absolutely not needed. So instead we implement all
    //     overloaded Something()s in terms of DoSomething() which will be the
    //     only one to be virtual.

    // retrieve the position/size of the window
    virtual void DoGetPosition( int *x, int *y ) const = 0;
    virtual void DoGetSize( int *width, int *height ) const = 0;
    virtual void DoGetClientSize( int *width, int *height ) const = 0;

    // this is the virtual function to be overriden in any derived class which
    // wants to change how SetSize() or Move() works - it is called by all
    // versions of these functions in the base class
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) = 0;

    // same as DoSetSize() for the client size
    virtual void DoSetClientSize(int width, int height) = 0;

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip );
#endif // wxUSE_TOOLTIPS

private:
    // contains the last id generated by NewControlId
    static int ms_lastControlId;

    DECLARE_NO_COPY_CLASS(wxWindowBase);
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// now include the declaration of wxWindow class
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/window.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/window.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/window.h"
#elif defined(__WXQT__)
    #include "wx/qt/window.h"
#elif defined(__WXMAC__)
    #include "wx/mac/window.h"
#endif

// ----------------------------------------------------------------------------
// inline functions which couldn't be declared in the class body because of
// forward dependencies
// ----------------------------------------------------------------------------

inline wxWindow *wxWindowBase::GetGrandParent() const
{
    return m_parent ? m_parent->GetParent() : (wxWindow *)NULL;
}

// ----------------------------------------------------------------------------
// global function
// ----------------------------------------------------------------------------

WXDLLEXPORT extern wxWindow* wxGetActiveWindow();
inline WXDLLEXPORT int NewControlId() { return wxWindowBase::NewControlId(); }

#endif
    // _WX_WINDOW_H_BASE_
