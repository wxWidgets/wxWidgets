///////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/framemanager.h
// Purpose:     wxaui: wx advanced user interface - docking window manager
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2005-05-17
// RCS-ID:      $Id$
// Copyright:   (C) Copyright 2005, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FRAMEMANAGER_H_
#define _WX_FRAMEMANAGER_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

#if wxUSE_AUI

#include "wx/dynarray.h"
#include "wx/gdicmn.h"
#include "wx/window.h"
#include "wx/timer.h"
#include "wx/sizer.h"

enum wxFrameManagerDock
{
    wxAUI_DOCK_NONE = 0,
    wxAUI_DOCK_TOP = 1,
    wxAUI_DOCK_RIGHT = 2,
    wxAUI_DOCK_BOTTOM = 3,
    wxAUI_DOCK_LEFT = 4,
    wxAUI_DOCK_CENTER = 5,
    wxAUI_DOCK_CENTRE = wxAUI_DOCK_CENTER
};

enum wxFrameManagerOption
{
    wxAUI_MGR_ALLOW_FLOATING        = 1 << 0,
    wxAUI_MGR_ALLOW_ACTIVE_PANE     = 1 << 1,
    wxAUI_MGR_TRANSPARENT_DRAG      = 1 << 2,
    wxAUI_MGR_TRANSPARENT_HINT      = 1 << 3,
    wxAUI_MGR_TRANSPARENT_HINT_FADE = 1 << 4,
    // The venetian blind effect is ONLY used when the wxAUI_MGR_TRANSPARENT_HINT has been used, but
    // at runtime we determine we cannot use transparency (because, for instance, the OS does not support it).
    // setting this flag drops back in such circumstances (only) to the behaviour without wxAUI_MGR_TRANSPARENT_HINT
    wxAUI_MGR_DISABLE_VENETIAN_BLINDS = 1 << 5,
    wxAUI_MGR_DISABLE_VENETIAN_BLINDS_FADE = 1 << 6,

    wxAUI_MGR_DEFAULT = wxAUI_MGR_ALLOW_FLOATING |
                        wxAUI_MGR_TRANSPARENT_HINT |
                        wxAUI_MGR_TRANSPARENT_HINT_FADE |
                        wxAUI_MGR_DISABLE_VENETIAN_BLINDS_FADE
};

enum wxPaneDockArtSetting
{
    wxAUI_ART_SASH_SIZE = 0,
    wxAUI_ART_CAPTION_SIZE = 1,
    wxAUI_ART_GRIPPER_SIZE = 2,
    wxAUI_ART_PANE_BORDER_SIZE = 3,
    wxAUI_ART_PANE_BUTTON_SIZE = 4,
    wxAUI_ART_BACKGROUND_COLOUR = 5,
    wxAUI_ART_SASH_COLOUR = 6,
    wxAUI_ART_ACTIVE_CAPTION_COLOUR = 7,
    wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR = 8,
    wxAUI_ART_INACTIVE_CAPTION_COLOUR = 9,
    wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR = 10,
    wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR = 11,
    wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR = 12,
    wxAUI_ART_BORDER_COLOUR = 13,
    wxAUI_ART_GRIPPER_COLOUR = 14,
    wxAUI_ART_CAPTION_FONT = 15,
    wxAUI_ART_GRADIENT_TYPE = 16
};

enum wxPaneDockArtGradients
{
    wxAUI_GRADIENT_NONE = 0,
    wxAUI_GRADIENT_VERTICAL = 1,
    wxAUI_GRADIENT_HORIZONTAL = 2
};

enum wxPaneButtonState
{
    wxAUI_BUTTON_STATE_NORMAL = 0,
    wxAUI_BUTTON_STATE_HOVER = 1,
    wxAUI_BUTTON_STATE_PRESSED = 2
};

enum wxPaneInsertLevel
{
    wxAUI_INSERT_PANE = 0,
    wxAUI_INSERT_ROW = 1,
    wxAUI_INSERT_DOCK = 2
};



// forwards and array declarations
class wxDockUIPart;
class wxPaneButton;
class wxPaneInfo;
class wxDockInfo;
class wxDockArt;
class wxFrameManagerEvent;

#ifndef SWIG
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxDockInfo, wxDockInfoArray, WXDLLIMPEXP_AUI);
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxDockUIPart, wxDockUIPartArray, WXDLLIMPEXP_AUI);
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxPaneButton, wxPaneButtonArray, WXDLLIMPEXP_AUI);
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxPaneInfo, wxPaneInfoArray, WXDLLIMPEXP_AUI);
WX_DEFINE_ARRAY_PTR(wxPaneInfo*, wxPaneInfoPtrArray);
WX_DEFINE_ARRAY_PTR(wxDockInfo*, wxDockInfoPtrArray);
#endif // SWIG

extern WXDLLIMPEXP_AUI wxDockInfo wxNullDockInfo;
extern WXDLLIMPEXP_AUI wxPaneInfo wxNullPaneInfo;



class WXDLLIMPEXP_AUI wxPaneInfo
{
public:

    wxPaneInfo()
    {
        window = NULL;
        frame = NULL;
        state = 0;
        dock_direction = wxAUI_DOCK_LEFT;
        dock_layer = 0;
        dock_row = 0;
        dock_pos = 0;
        floating_pos = wxDefaultPosition;
        floating_size = wxDefaultSize;
        best_size = wxDefaultSize;
        min_size = wxDefaultSize;
        max_size = wxDefaultSize;
        dock_proportion = 0;

        DefaultPane();
    }

    ~wxPaneInfo() {}
    
#ifndef SWIG
    wxPaneInfo(const wxPaneInfo& c)
    {
        name = c.name;
        caption = c.caption;
        window = c.window;
        frame = c.frame;
        state = c.state;
        dock_direction = c.dock_direction;
        dock_layer = c.dock_layer;
        dock_row = c.dock_row;
        dock_pos = c.dock_pos;
        best_size = c.best_size;
        min_size = c.min_size;
        max_size = c.max_size;
        floating_pos = c.floating_pos;
        floating_size = c.floating_size;
        dock_proportion = c.dock_proportion;
        buttons = c.buttons;
        rect = c.rect;
    }

    wxPaneInfo& operator=(const wxPaneInfo& c)
    {
        name = c.name;
        caption = c.caption;
        window = c.window;
        frame = c.frame;
        state = c.state;
        dock_direction = c.dock_direction;
        dock_layer = c.dock_layer;
        dock_row = c.dock_row;
        dock_pos = c.dock_pos;
        best_size = c.best_size;
        min_size = c.min_size;
        max_size = c.max_size;
        floating_pos = c.floating_pos;
        floating_size = c.floating_size;
        dock_proportion = c.dock_proportion;
        buttons = c.buttons;
        rect = c.rect;
        return *this;
    }
#endif // SWIG

    // Write the safe parts of a newly loaded PaneInfo structure "source" into "this"
    // used on loading perspectives etc.
    void SafeSet(wxPaneInfo source)
    {
        // note source is not passed by reference so we can overwrite, to keep the
        // unsafe bits of "dest"
        source.window = window;
        source.frame = frame;
        source.buttons = buttons;
        // now assign
        *this = source;
    }

    bool IsOk() const { return (window != NULL) ? true : false; }
    bool IsFixed() const { return !HasFlag(optionResizable); }
    bool IsResizable() const { return HasFlag(optionResizable); }
    bool IsShown() const { return !HasFlag(optionHidden); }
    bool IsFloating() const { return HasFlag(optionFloating); }
    bool IsDocked() const { return !HasFlag(optionFloating); }
    bool IsToolbar() const { return HasFlag(optionToolbar); }
    bool IsTopDockable() const { return HasFlag(optionTopDockable); }
    bool IsBottomDockable() const { return HasFlag(optionBottomDockable); }
    bool IsLeftDockable() const { return HasFlag(optionLeftDockable); }
    bool IsRightDockable() const { return HasFlag(optionRightDockable); }
    bool IsFloatable() const { return HasFlag(optionFloatable); }
    bool IsMovable() const { return HasFlag(optionMovable); }
    bool HasCaption() const { return HasFlag(optionCaption); }
    bool HasGripper() const { return HasFlag(optionGripper); }
    bool HasBorder() const { return HasFlag(optionPaneBorder); }
    bool HasCloseButton() const { return HasFlag(buttonClose); }
    bool HasMaximizeButton() const { return HasFlag(buttonMaximize); }
    bool HasMinimizeButton() const { return HasFlag(buttonMinimize); }
    bool HasPinButton() const { return HasFlag(buttonPin); }
    bool HasGripperTop() const { return HasFlag(optionGripperTop); }

#ifdef SWIG
    %typemap(out) wxPaneInfo& { $result = $self; Py_INCREF($result); }
#endif
    wxPaneInfo& Window(wxWindow* w) { window = w; return *this; }
    wxPaneInfo& Name(const wxString& n) { name = n; return *this; }
    wxPaneInfo& Caption(const wxString& c) { caption = c; return *this; }
    wxPaneInfo& Left() { dock_direction = wxAUI_DOCK_LEFT; return *this; }
    wxPaneInfo& Right() { dock_direction = wxAUI_DOCK_RIGHT; return *this; }
    wxPaneInfo& Top() { dock_direction = wxAUI_DOCK_TOP; return *this; }
    wxPaneInfo& Bottom() { dock_direction = wxAUI_DOCK_BOTTOM; return *this; }
    wxPaneInfo& Center() { dock_direction = wxAUI_DOCK_CENTER; return *this; }
    wxPaneInfo& Centre() { dock_direction = wxAUI_DOCK_CENTRE; return *this; }
    wxPaneInfo& Direction(int direction) { dock_direction = direction; return *this; }
    wxPaneInfo& Layer(int layer) { dock_layer = layer; return *this; }
    wxPaneInfo& Row(int row) { dock_row = row; return *this; }
    wxPaneInfo& Position(int pos) { dock_pos = pos; return *this; }
    wxPaneInfo& BestSize(const wxSize& size) { best_size = size; return *this; }
    wxPaneInfo& MinSize(const wxSize& size) { min_size = size; return *this; }
    wxPaneInfo& MaxSize(const wxSize& size) { max_size = size; return *this; }
    wxPaneInfo& BestSize(int x, int y) { best_size.Set(x,y); return *this; }
    wxPaneInfo& MinSize(int x, int y) { min_size.Set(x,y); return *this; }
    wxPaneInfo& MaxSize(int x, int y) { max_size.Set(x,y); return *this; }
    wxPaneInfo& FloatingPosition(const wxPoint& pos) { floating_pos = pos; return *this; }
    wxPaneInfo& FloatingPosition(int x, int y) { floating_pos.x = x; floating_pos.y = y; return *this; }
    wxPaneInfo& FloatingSize(const wxSize& size) { floating_size = size; return *this; }
    wxPaneInfo& FloatingSize(int x, int y) { floating_size.Set(x,y); return *this; }
    wxPaneInfo& Fixed() { return SetFlag(optionResizable, false); }
    wxPaneInfo& Resizable(bool resizable = true) { return SetFlag(optionResizable, resizable); }
    wxPaneInfo& Dock() { return SetFlag(optionFloating, false); }
    wxPaneInfo& Float() { return SetFlag(optionFloating, true); }
    wxPaneInfo& Hide() { return SetFlag(optionHidden, true); }
    wxPaneInfo& Show(bool show = true) { return SetFlag(optionHidden, !show); }
    wxPaneInfo& CaptionVisible(bool visible = true) { return SetFlag(optionCaption, visible); }
    wxPaneInfo& PaneBorder(bool visible = true) { return SetFlag(optionPaneBorder, visible); }
    wxPaneInfo& Gripper(bool visible = true) { return SetFlag(optionGripper, visible); }
    wxPaneInfo& GripperTop(bool attop = true) { return SetFlag(optionGripperTop, attop); }
    wxPaneInfo& CloseButton(bool visible = true) { return SetFlag(buttonClose, visible); }
    wxPaneInfo& MaximizeButton(bool visible = true) { return SetFlag(buttonMaximize, visible); }
    wxPaneInfo& MinimizeButton(bool visible = true) { return SetFlag(buttonMinimize, visible); }
    wxPaneInfo& PinButton(bool visible = true) { return SetFlag(buttonPin, visible); }
    wxPaneInfo& DestroyOnClose(bool b = true) { return SetFlag(optionDestroyOnClose, b); }
    wxPaneInfo& TopDockable(bool b = true) { return SetFlag(optionTopDockable, b); }
    wxPaneInfo& BottomDockable(bool b = true) { return SetFlag(optionBottomDockable, b); }
    wxPaneInfo& LeftDockable(bool b = true) { return SetFlag(optionLeftDockable, b); }
    wxPaneInfo& RightDockable(bool b = true) { return SetFlag(optionRightDockable, b); }
    wxPaneInfo& Floatable(bool b = true) { return SetFlag(optionFloatable, b); }
    wxPaneInfo& Movable(bool b = true) { return SetFlag(optionMovable, b); }
    wxPaneInfo& Dockable(bool b = true)
    {
        return TopDockable(b).BottomDockable(b).LeftDockable(b).RightDockable(b);
    }

    wxPaneInfo& DefaultPane()
    {
        state |= optionTopDockable | optionBottomDockable |
                 optionLeftDockable | optionRightDockable |
                 optionFloatable | optionMovable | optionResizable |
                 optionCaption | optionPaneBorder | buttonClose;
        return *this;
    }

    wxPaneInfo& CentrePane() { return CenterPane(); }
    wxPaneInfo& CenterPane()
    {
        state = 0;
        return Center().PaneBorder().Resizable();
    }

    wxPaneInfo& ToolbarPane()
    {
        DefaultPane();
        state |= (optionToolbar | optionGripper);
        state &= ~(optionResizable | optionCaption);
        if (dock_layer == 0)
            dock_layer = 10;
        return *this;
    }

    wxPaneInfo& SetFlag(unsigned int flag, bool option_state)
    {
        if (option_state)
            state |= flag;
             else
            state &= ~flag;
        return *this;
    }

    bool HasFlag(unsigned int flag) const
    {
        return (state & flag) ? true:false;
    }

#ifdef SWIG
    %typemap(out) wxPaneInfo& ;
#endif
    
public:

    enum wxPaneState
    {
        optionFloating        = 1 << 0,
        optionHidden          = 1 << 1,
        optionLeftDockable    = 1 << 2,
        optionRightDockable   = 1 << 3,
        optionTopDockable     = 1 << 4,
        optionBottomDockable  = 1 << 5,
        optionFloatable       = 1 << 6,
        optionMovable         = 1 << 7,
        optionResizable       = 1 << 8,
        optionPaneBorder      = 1 << 9,
        optionCaption         = 1 << 10,
        optionGripper         = 1 << 11,
        optionDestroyOnClose  = 1 << 12,
        optionToolbar         = 1 << 13,
        optionActive          = 1 << 14,
        optionGripperTop      = 1 << 15,

        buttonClose           = 1 << 24,
        buttonMaximize        = 1 << 25,
        buttonMinimize        = 1 << 26,
        buttonPin             = 1 << 27,
        buttonCustom1         = 1 << 28,
        buttonCustom2         = 1 << 29,
        buttonCustom3         = 1 << 30,
        actionPane            = 1 << 31  // used internally
    };

public:
    wxString name;        // name of the pane
    wxString caption;     // caption displayed on the window

    wxWindow* window;     // window that is in this pane
    wxFrame* frame;       // floating frame window that holds the pane
    unsigned int state;   // a combination of wxPaneState values

    int dock_direction;   // dock direction (top, bottom, left, right, center)
    int dock_layer;       // layer number (0 = innermost layer)
    int dock_row;         // row number on the docking bar (0 = first row)
    int dock_pos;         // position inside the row (0 = first position)

    wxSize best_size;     // size that the layout engine will prefer
    wxSize min_size;      // minimum size the pane window can tolerate
    wxSize max_size;      // maximum size the pane window can tolerate

    wxPoint floating_pos; // position while floating
    wxSize floating_size; // size while floating
    int dock_proportion;  // proportion while docked

    wxPaneButtonArray buttons; // buttons on the pane

    wxRect rect;              // current rectangle (populated by wxAUI)
};





class WXDLLIMPEXP_AUI wxFrameManager : public wxEvtHandler
{
friend class wxFloatingPane;

public:

    wxFrameManager(wxWindow* managed_wnd = NULL,
                   unsigned int flags = wxAUI_MGR_DEFAULT);
    virtual ~wxFrameManager();
    void UnInit();

    void SetFlags(unsigned int flags);
    unsigned int GetFlags() const;

    void SetManagedWindow(wxWindow* managed_wnd);
    wxWindow* GetManagedWindow() const;

#ifdef SWIG
    %disownarg( wxDockArt* art_provider );
#endif
    void SetArtProvider(wxDockArt* art_provider);
    wxDockArt* GetArtProvider() const;

    wxPaneInfo& GetPane(wxWindow* window);
    wxPaneInfo& GetPane(const wxString& name);
    wxPaneInfoArray& GetAllPanes();

    bool AddPane(wxWindow* window,
                 const wxPaneInfo& pane_info);
                 
    bool AddPane(wxWindow* window,
                 const wxPaneInfo& pane_info,
                 const wxPoint& drop_pos);

    bool AddPane(wxWindow* window,
                 int direction = wxLEFT,
                 const wxString& caption = wxEmptyString);

    bool InsertPane(wxWindow* window,
                 const wxPaneInfo& insert_location,
                 int insert_level = wxAUI_INSERT_PANE);

    bool DetachPane(wxWindow* window);

    wxString SavePaneInfo(wxPaneInfo& pane);
    void LoadPaneInfo(wxString pane_part, wxPaneInfo &pane);

    wxString SavePerspective();

    bool LoadPerspective(const wxString& perspective,
                 bool update = true);

    void Update();


public:

    void DrawHintRect(wxWindow* pane_window,
                       const wxPoint& pt,
                       const wxPoint& offset);
    virtual void ShowHint(const wxRect& rect);
    virtual void HideHint();

public:

    // deprecated -- please use SetManagedWindow() and
    // and GetManagedWindow() instead
    
    wxDEPRECATED( void SetFrame(wxFrame* frame) );
    wxDEPRECATED( wxFrame* GetFrame() const );
    
protected:


    
    void DoFrameLayout();

    void LayoutAddPane(wxSizer* container,
                       wxDockInfo& dock,
                       wxPaneInfo& pane,
                       wxDockUIPartArray& uiparts,
                       bool spacer_only);

    void LayoutAddDock(wxSizer* container,
                       wxDockInfo& dock,
                       wxDockUIPartArray& uiparts,
                       bool spacer_only);

    wxSizer* LayoutAll(wxPaneInfoArray& panes,
                       wxDockInfoArray& docks,
                       wxDockUIPartArray& uiparts,
                       bool spacer_only = false);

    virtual bool ProcessDockResult(wxPaneInfo& target,
                                   const wxPaneInfo& new_pos);

    bool DoDrop(wxDockInfoArray& docks,
                wxPaneInfoArray& panes,
                wxPaneInfo& drop,
                const wxPoint& pt,
                const wxPoint& action_offset = wxPoint(0,0));

    wxPaneInfo& LookupPane(wxWindow* window);
    wxPaneInfo& LookupPane(const wxString& name);
    wxDockUIPart* HitTest(int x, int y);
    wxDockUIPart* GetPanePart(wxWindow* pane);
    int GetDockPixelOffset(wxPaneInfo& test);
    void OnFloatingPaneMoveStart(wxWindow* window);
    void OnFloatingPaneMoving(wxWindow* window, wxDirection dir );
    void OnFloatingPaneMoved(wxWindow* window, wxDirection dir);
    void OnFloatingPaneActivated(wxWindow* window);
    void OnFloatingPaneClosed(wxWindow* window, wxCloseEvent& evt);
    void OnFloatingPaneResized(wxWindow* window, const wxSize& size);
    void Render(wxDC* dc);
    void Repaint(wxDC* dc = NULL);
    void ProcessMgrEvent(wxFrameManagerEvent& event);
    void UpdateButtonOnScreen(wxDockUIPart* button_ui_part,
                              const wxMouseEvent& event);
    void GetPanePositionsAndSizes(wxDockInfo& dock,
                              wxArrayInt& positions,
                              wxArrayInt& sizes);


public:

    // public events (which can be invoked externally)
    void OnRender(wxFrameManagerEvent& evt);
    void OnPaneButton(wxFrameManagerEvent& evt);

protected:

    // protected events
    void OnPaint(wxPaintEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnSetCursor(wxSetCursorEvent& evt);
    void OnLeftDown(wxMouseEvent& evt);
    void OnLeftUp(wxMouseEvent& evt);
    void OnMotion(wxMouseEvent& evt);
    void OnLeaveWindow(wxMouseEvent& evt);
    void OnChildFocus(wxChildFocusEvent& evt);
    void OnHintFadeTimer(wxTimerEvent& evt);

protected:

    enum
    {
        actionNone = 0,
        actionResize,
        actionClickButton,
        actionClickCaption,
        actionDragToolbarPane,
        actionDragFloatingPane
    };

protected:

    wxWindow* m_frame;           // the window being managed
    wxDockArt* m_art;            // dock art object which does all drawing
    unsigned int m_flags;        // manager flags wxAUI_MGR_*

    wxPaneInfoArray m_panes;     // array of panes structures
    wxDockInfoArray m_docks;     // array of docks structures
    wxDockUIPartArray m_uiparts; // array of UI parts (captions, buttons, etc)

    int m_action;                // current mouse action
    wxPoint m_action_start;      // position where the action click started
    wxPoint m_action_offset;     // offset from upper left of the item clicked
    wxDockUIPart* m_action_part; // ptr to the part the action happened to
    wxWindow* m_action_window;   // action frame or window (NULL if none)
    wxRect m_action_hintrect;    // hint rectangle for the action
    wxDockUIPart* m_hover_button;// button uipart being hovered over
    wxRect m_last_hint;          // last hint rectangle
    wxPoint m_last_mouse_move;   // last mouse move position (see OnMotion)

    wxFrame* m_hint_wnd;         // transparent hint window, if supported by platform
    wxTimer m_hint_fadetimer;    // transparent fade timer
    wxByte m_hint_fadeamt;       // transparent fade amount
    wxByte m_hint_fademax;       // maximum value of hint fade

#ifndef SWIG
    DECLARE_EVENT_TABLE()
#endif // SWIG
};



// event declarations/classes

class WXDLLIMPEXP_AUI wxFrameManagerEvent : public wxEvent
{
public:
    wxFrameManagerEvent(wxEventType type=wxEVT_NULL) : wxEvent(0, type)
    {
        pane = NULL;
        button = 0;
        veto_flag = false;
        canveto_flag = true;
        dc = NULL;
    }
#ifndef SWIG
    wxFrameManagerEvent(const wxFrameManagerEvent& c) : wxEvent(c)
    {
        pane = c.pane;
        button = c.button;
        veto_flag = c.veto_flag;
        canveto_flag = c.canveto_flag;
        dc = c.dc;
    }
#endif
    wxEvent *Clone() const { return new wxFrameManagerEvent(*this); }

    void SetPane(wxPaneInfo* p) { pane = p; }
    void SetButton(int b) { button = b; }
    void SetDC(wxDC* pdc) { dc = pdc; }
 
    wxPaneInfo* GetPane() { return pane; }
    int GetButton() { return button; }
    wxDC* GetDC() { return dc; }
    
    void Veto(bool veto = true) { veto_flag = veto; }
    bool GetVeto() const { return veto_flag; }
    void SetCanVeto(bool can_veto) { canveto_flag = can_veto; }
    bool CanVeto() const { return  canveto_flag && veto_flag; }
    
public:
    wxPaneInfo* pane;
    int button;
    bool veto_flag;
    bool canveto_flag;
    wxDC* dc;

#ifndef SWIG
private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxFrameManagerEvent)
#endif
};


class WXDLLIMPEXP_AUI wxDockInfo
{
public:
    wxDockInfo()
    {
        dock_direction = 0;
        dock_layer = 0;
        dock_row = 0;
        size = 0;
        min_size = 0;
        resizable = true;
        fixed = false;
        toolbar = false;
    }

#ifndef SWIG
    wxDockInfo(const wxDockInfo& c)
    {
        dock_direction = c.dock_direction;
        dock_layer = c.dock_layer;
        dock_row = c.dock_row;
        size = c.size;
        min_size = c.min_size;
        resizable = c.resizable;
        fixed = c.fixed;
        toolbar = c.toolbar;
        panes = c.panes;
        rect = c.rect;
    }

    wxDockInfo& operator=(const wxDockInfo& c)
    {
        dock_direction = c.dock_direction;
        dock_layer = c.dock_layer;
        dock_row = c.dock_row;
        size = c.size;
        min_size = c.min_size;
        resizable = c.resizable;
        fixed = c.fixed;
        toolbar = c.toolbar;
        panes = c.panes;
        rect = c.rect;
        return *this;
    }
#endif // SWIG

    bool IsOk() const { return (dock_direction != 0) ? true : false; }
    bool IsHorizontal() const { return (dock_direction == wxAUI_DOCK_TOP ||
                             dock_direction == wxAUI_DOCK_BOTTOM) ? true:false; }
    bool IsVertical() const { return (dock_direction == wxAUI_DOCK_LEFT ||
                             dock_direction == wxAUI_DOCK_RIGHT ||
                             dock_direction == wxAUI_DOCK_CENTER) ? true:false; }
public:
    wxPaneInfoPtrArray panes; // array of panes
    wxRect rect;              // current rectangle
    int dock_direction;       // dock direction (top, bottom, left, right, center)
    int dock_layer;           // layer number (0 = innermost layer)
    int dock_row;             // row number on the docking bar (0 = first row)
    int size;                 // size of the dock
    int min_size;             // minimum size of a dock (0 if there is no min)
    bool resizable;           // flag indicating whether the dock is resizable
    bool toolbar;             // flag indicating dock contains only toolbars
    bool fixed;               // flag indicating that the dock operates on
                              // absolute coordinates as opposed to proportional
};


class WXDLLIMPEXP_AUI wxDockUIPart
{
public:
    enum
    {
        typeCaption,
        typeGripper,
        typeDock,
        typeDockSizer,
        typePane,
        typePaneSizer,
        typeBackground,
        typePaneBorder,
        typePaneButton
    };

    int type;                // ui part type (see enum above)
    int orientation;         // orientation (either wxHORIZONTAL or wxVERTICAL)
    wxDockInfo* dock;        // which dock the item is associated with
    wxPaneInfo* pane;        // which pane the item is associated with
    wxPaneButton* button;    // which pane button the item is associated with
    wxSizer* cont_sizer;     // the part's containing sizer
    wxSizerItem* sizer_item; // the sizer item of the part
    wxRect rect;             // client coord rectangle of the part itself
};


class WXDLLIMPEXP_AUI wxPaneButton
{
public:
    int button_id;        // id of the button (e.g. buttonClose)
};



#ifndef SWIG
// wx event machinery

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_AUI_PANEBUTTON, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_AUI_PANECLOSE, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_AUI_RENDER, 0)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxFrameManagerEventFunction)(wxFrameManagerEvent&);

#define wxFrameManagerEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxFrameManagerEventFunction, &func)

#define EVT_AUI_PANEBUTTON(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_PANEBUTTON, wxFrameManagerEventHandler(func))
#define EVT_AUI_PANECLOSE(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_PANECLOSE, wxFrameManagerEventHandler(func))
#define EVT_AUI_RENDER(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_RENDER, wxFrameManagerEventHandler(func))

#else

%constant wxEventType wxEVT_AUI_PANEBUTTON;
%constant wxEventType wxEVT_AUI_PANECLOSE;
%constant wxEventType wxEVT_AUI_RENDER;

%pythoncode {
    EVT_AUI_PANEBUTTON = wx.PyEventBinder( wxEVT_AUI_PANEBUTTON )
    EVT_AUI_PANECLOSE = wx.PyEventBinder( wxEVT_AUI_PANECLOSE )
    EVT_AUI_RENDER = wx.PyEventBinder( wxEVT_AUI_RENDER )
}
#endif // SWIG

#endif // wxUSE_AUI
#endif //_WX_FRAMEMANAGER_H_

