///////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/framemanager.h
// Purpose:     wxaui: wx advanced user interface - docking window manager
// Author:      Benjamin I. Williams
// Modified by: Malcolm MacLeod (mmacleod@webmail.co.za)
// Created:     2005-05-17
// RCS-ID:      $Id: framemanager.h 61581 2009-08-02 19:27:13Z MJM $
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
#include "wx/bitmap.h"

enum wxAuiManagerDock
{
    wxAUI_DOCK_NONE = 0,
    wxAUI_DOCK_TOP = 1,
    wxAUI_DOCK_RIGHT = 2,
    wxAUI_DOCK_BOTTOM = 3,
    wxAUI_DOCK_LEFT = 4,
    wxAUI_DOCK_CENTER = 5,
    wxAUI_DOCK_CENTRE = wxAUI_DOCK_CENTER
};

enum wxAuiManagerOption
{

    // The Notebook flags are also used as wxAuiNotbook style flags, and therefore must stay in the lower 16 bits
    // to avoid colliding standard windows styles
    wxAUI_MGR_NB_TOP                   = 1 << 0,
    wxAUI_MGR_NB_LEFT                  = 1 << 1,
    wxAUI_MGR_NB_RIGHT                 = 1 << 2,
    wxAUI_MGR_NB_BOTTOM                = 1 << 3,
    wxAUI_MGR_NB_TAB_MOVE              = 1 << 4,
    wxAUI_MGR_ALLOW_EXTERNAL_MOVE      = 1 << 5,
    wxAUI_MGR_NB_TAB_FIXED_WIDTH       = 1 << 6,
    wxAUI_MGR_NB_TAB_FIXED_HEIGHT      = wxAUI_MGR_NB_TAB_FIXED_WIDTH,
    wxAUI_MGR_NB_SCROLL_BUTTONS        = 1 << 7,
    wxAUI_MGR_NB_WINDOWLIST_BUTTON     = 1 << 8,
    wxAUI_MGR_NB_CLOSE_BUTTON          = 1 << 9,
    wxAUI_MGR_NB_CLOSE_ON_ACTIVE_TAB   = 1 << 10,
    wxAUI_MGR_NB_CLOSE_ON_ALL_TABS     = 1 << 11,
    wxAUI_MGR_NB_TAB_SPLIT             = 1 << 12,  // wxAuiNotebook only
    wxAUI_MGR_NB_ALLOW_NOTEBOOKS       = 1 << 13,

    // below the flags are only wxAuiManager flags
    wxAUI_MGR_ALLOW_FLOATING           = 1 << 16,
    wxAUI_MGR_ALLOW_ACTIVE_PANE        = 1 << 17,
    wxAUI_MGR_TRANSPARENT_DRAG         = 1 << 18,
    wxAUI_MGR_TRANSPARENT_HINT         = 1 << 19,
    wxAUI_MGR_VENETIAN_BLINDS_HINT     = 1 << 20,
    wxAUI_MGR_RECTANGLE_HINT           = 1 << 21,
    wxAUI_MGR_HINT_FADE                = 1 << 22,
    wxAUI_MGR_NO_VENETIAN_BLINDS_FADE  = 1 << 23,
    wxAUI_MGR_LIVE_RESIZE              = 1 << 24,
    wxAUI_MGR_MIDDLE_CLICK_CLOSE       = 1 << 25,

    wxAUI_MGR_DEFAULT = wxAUI_MGR_ALLOW_FLOATING |
                        wxAUI_MGR_TRANSPARENT_HINT |
                        wxAUI_MGR_HINT_FADE |
                        wxAUI_MGR_NO_VENETIAN_BLINDS_FADE |
                        wxAUI_MGR_NB_TOP |
                        wxAUI_MGR_NB_TAB_MOVE |
                        wxAUI_MGR_NB_TAB_SPLIT |
                        wxAUI_MGR_NB_SCROLL_BUTTONS |
                        wxAUI_MGR_NB_CLOSE_ON_ACTIVE_TAB |
                        wxAUI_MGR_MIDDLE_CLICK_CLOSE
};


enum wxAuiPaneDockArtSetting
{
    wxAUI_DOCKART_SASH_SIZE = 0,
    wxAUI_DOCKART_CAPTION_SIZE = 1,
    wxAUI_DOCKART_GRIPPER_SIZE = 2,
    wxAUI_DOCKART_PANE_BORDER_SIZE = 3,
    wxAUI_DOCKART_PANE_BUTTON_SIZE = 4,
    wxAUI_DOCKART_BACKGROUND_COLOUR = 5,
    wxAUI_DOCKART_SASH_COLOUR = 6,
    wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR = 7,
    wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR = 8,
    wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR = 9,
    wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR = 10,
    wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR = 11,
    wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR = 12,
    wxAUI_DOCKART_BORDER_COLOUR = 13,
    wxAUI_DOCKART_GRIPPER_COLOUR = 14,
    wxAUI_DOCKART_CAPTION_FONT = 15,
    wxAUI_DOCKART_GRADIENT_TYPE = 16
};

enum wxAuiPaneDockArtGradients
{
    wxAUI_GRADIENT_NONE = 0,
    wxAUI_GRADIENT_VERTICAL = 1,
    wxAUI_GRADIENT_HORIZONTAL = 2
};

enum wxAuiPaneButtonState
{
    wxAUI_BUTTON_STATE_NORMAL   = 0,
    wxAUI_BUTTON_STATE_HOVER    = 1 << 1,
    wxAUI_BUTTON_STATE_PRESSED  = 1 << 2,
    wxAUI_BUTTON_STATE_DISABLED = 1 << 3,
    wxAUI_BUTTON_STATE_HIDDEN   = 1 << 4,
    wxAUI_BUTTON_STATE_CHECKED  = 1 << 5
};

enum wxAuiButtonId
{
    wxAUI_BUTTON_CLOSE = 101,
    wxAUI_BUTTON_MAXIMIZE_RESTORE = 102,
    wxAUI_BUTTON_MINIMIZE = 103,
    wxAUI_BUTTON_PIN = 104,
    wxAUI_BUTTON_OPTIONS = 105,
    wxAUI_BUTTON_WINDOWLIST = 106,
    wxAUI_BUTTON_LEFT = 107,
    wxAUI_BUTTON_RIGHT = 108,
    wxAUI_BUTTON_UP = 109,
    wxAUI_BUTTON_DOWN = 110,
    wxAUI_BUTTON_CUSTOM1 = 201,
    wxAUI_BUTTON_CUSTOM2 = 202,
    wxAUI_BUTTON_CUSTOM3 = 203
};

enum wxAuiPaneInsertLevel
{
    wxAUI_INSERT_PANE = 0,
    wxAUI_INSERT_ROW = 1,
    wxAUI_INSERT_DOCK = 2
};




// forwards and array declarations
class wxAuiDockUIPart;
class wxAuiPaneButton;
class wxAuiPaneInfo;
class wxAuiDockInfo;
class wxAuiDockArt;
class wxAuiTabArt;
class wxAuiManagerEvent;
class wxAuiTabContainer;

#ifndef SWIG
WX_DECLARE_OBJARRAY(wxAuiTabContainer*, wxAuiTabContainerPointerArray);
WX_DECLARE_OBJARRAY(wxSizerItem*, wxAuiSizerItemPointerArray);
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxAuiDockInfo, wxAuiDockInfoArray, WXDLLIMPEXP_AUI);
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxAuiDockUIPart, wxAuiDockUIPartArray, WXDLLIMPEXP_AUI);
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxAuiPaneButton, wxAuiPaneButtonArray, WXDLLIMPEXP_AUI);
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxAuiPaneInfo, wxAuiPaneInfoArray, WXDLLIMPEXP_AUI);
WX_DEFINE_USER_EXPORTED_ARRAY_PTR(wxAuiPaneInfo*, wxAuiPaneInfoPtrArray, class WXDLLIMPEXP_AUI);
WX_DEFINE_USER_EXPORTED_ARRAY_PTR(wxAuiDockInfo*, wxAuiDockInfoPtrArray, class WXDLLIMPEXP_AUI);
#endif // SWIG

extern WXDLLIMPEXP_AUI wxAuiDockInfo wxAuiNullDockInfo;
extern WXDLLIMPEXP_AUI wxAuiPaneInfo wxAuiNullPaneInfo;



class WXDLLIMPEXP_AUI wxAuiPaneInfo
{
public:
    wxAuiPaneInfo();
    ~wxAuiPaneInfo() {}
#ifndef SWIG
    wxAuiPaneInfo(const wxAuiPaneInfo& c);
    wxAuiPaneInfo& operator=(const wxAuiPaneInfo& c);
#else // SWIG
    %typemap(out) wxAuiPaneInfo& { $result = $self; Py_INCREF($result); }
#endif // !SWIG


    // return a string serializing the state of this pane.
    wxString GetInfo() const;
    // load the pane state from a serialized string.
    void LoadInfo(wxString& info);

    // returns true if the pane contains a toolbar.
    bool IsToolbar() const { return HasFlag(optionToolbar); }

    // returns true if the wxAuiPaneInfo structure is valid. A pane structure is valid if it has an associated window.
    bool IsOk() const { return window != NULL; }
    // get/set the window associated with this pane
    wxWindow* GetWindow() const { return window; }
    wxAuiPaneInfo& Window(wxWindow* w)
    {
      wxAuiPaneInfo test(*this);
      test.window = w;
      wxCHECK_MSG(test.IsValid(), *this,
                  "window settings and pane settings are incompatible");
      this->window = w;
      return *this;
    }

    // get/set the floating frame window that holds the pane
    wxFrame* GetFrame() const { return frame; }
    wxAuiPaneInfo& Frame(wxFrame* f) { frame = f; return *this; }

    // get/set if the pane is resizable.
    // opposite of Is/SetFixed.
    bool IsResizable() const { return HasFlag(optionResizable); }
    wxAuiPaneInfo& Resizable(bool resizable = true) { return SetFlag(optionResizable, resizable); }
    // get/set whether the pane is not resizable.
    // opposite of Is/SetResizable.
    bool IsFixed() const { return !HasFlag(optionResizable); }
    wxAuiPaneInfo& Fixed() { return SetFlag(optionResizable, false); }

    // get/set if the pane is visible.
    bool IsShown() const { return !HasFlag(optionHidden); }
    wxAuiPaneInfo& Hide() { SetFlag(wxAuiPaneInfo::optionActiveNotebook,false); return SetFlag(optionHidden, true); }
    wxAuiPaneInfo& Show(bool show = true) { if (!show) SetFlag(wxAuiPaneInfo::optionActiveNotebook, false); return SetFlag(optionHidden, !show); }

    // get/set if the pane is floating.
    // opposite of IsDocked.
    bool IsFloating() const { return HasFlag(optionFloating); }
    wxAuiPaneInfo& Float() { SetFlag(wxAuiPaneInfo::optionActiveNotebook,false); return SetFlag(optionFloating, true); }
    // get/set if the pane is docked.
    // opposite of IsFloating.
    bool IsDocked() const { return !HasFlag(optionFloating); }
    wxAuiPaneInfo& Dock() { return SetFlag(optionFloating, false); }

    // get/set whether the pane can be docked at the center of the frame.
    bool IsCenterDockable() const { return HasFlag(optionCenterDockable); }
    wxAuiPaneInfo& CenterDockable(bool b = true) { return SetFlag(optionCenterDockable, b); }
    // get/set whether the pane can be docked at the top of the frame.
    bool IsTopDockable() const { return HasFlag(optionTopDockable); }
    wxAuiPaneInfo& TopDockable(bool b = true) { return SetFlag(optionTopDockable, b); }
    // get/set whether the pane can be docked at the bottom of the frame.
    bool IsBottomDockable() const { return HasFlag(optionBottomDockable); }
    wxAuiPaneInfo& BottomDockable(bool b = true) { return SetFlag(optionBottomDockable, b); }
    // get/set whether the pane can be docked at the left of the frame.
    bool IsLeftDockable() const { return HasFlag(optionLeftDockable); }
    wxAuiPaneInfo& LeftDockable(bool b = true) { return SetFlag(optionLeftDockable, b); }
    // get/set whether the pane can be docked at the right of the frame.
    bool IsRightDockable() const { return HasFlag(optionRightDockable); }
    wxAuiPaneInfo& RightDockable(bool b = true) { return SetFlag(optionRightDockable, b); }
    // convenience function to set all of the above at once.
    wxAuiPaneInfo& Dockable(bool b = true)
    {
        return TopDockable(b).BottomDockable(b).LeftDockable(b).RightDockable(b).CenterDockable(b);
    }
    bool IsDockable() const
    {
        return HasFlag(optionTopDockable | optionBottomDockable |
                        optionLeftDockable | optionRightDockable | optionCenterDockable);
    }


    // get/set if pane can be undocked and displayed as a floating window.
    bool IsFloatable() const { return HasFlag(optionFloatable); }
    wxAuiPaneInfo& Floatable(bool b = true) { return SetFlag(optionFloatable, b); }

    // get/set if pane can be undocked or moved to another dock position.
    bool IsMovable() const { return HasFlag(optionMovable); }
    wxAuiPaneInfo& Movable(bool b = true) { return SetFlag(optionMovable, b); }

    // get/set if pane is maximized.
    bool IsMaximized() const { return HasFlag(optionMaximized); }
    wxAuiPaneInfo& Maximize() { return SetFlag(optionMaximized, true); }
    wxAuiPaneInfo& Restore() { return SetFlag(optionMaximized, false); }

    // get/set whether a pane should be destroyed when closed, instead of the default behaviour of hiding a closed pane.
    bool HasDestroyOnClose() const { return HasFlag(optionDestroyOnClose); }
    wxAuiPaneInfo& DestroyOnClose(bool b = true) { return SetFlag(optionDestroyOnClose, b); }

    // get/set if pane displays a caption.
    bool HasCaption() const { return HasFlag(optionCaption); }
    wxAuiPaneInfo& CaptionVisible(bool visible = true) { return SetFlag(optionCaption, visible); }

    // get/set if pane displays a gripper.
    bool HasGripper() const { return HasFlag(optionGripper); }
    wxAuiPaneInfo& Gripper(bool visible = true) { return SetFlag(optionGripper, visible); }
    // get/set if pane displays a gripper at the top.
    bool HasGripperTop() const { return HasFlag(optionGripperTop); }
    wxAuiPaneInfo& GripperTop(bool attop = true) { return SetFlag(optionGripperTop, attop); }

    // get/set if pane displays a border.
    bool HasBorder() const { return HasFlag(optionPaneBorder); }
    wxAuiPaneInfo& PaneBorder(bool visible = true) { return SetFlag(optionPaneBorder, visible); }

    // get/set if pane displays a close button.
    bool HasCloseButton() const { return HasFlag(buttonClose); }
    wxAuiPaneInfo& CloseButton(bool visible = true) { return SetFlag(buttonClose, visible); }

    // get/set if pane displays a maximize button.
    bool HasMaximizeButton() const { return HasFlag(buttonMaximize); }
    wxAuiPaneInfo& MaximizeButton(bool visible = true) { return SetFlag(buttonMaximize, visible); }

    // get/set if pane displays a minimize button.
    bool HasMinimizeButton() const { return HasFlag(buttonMinimize); }
    wxAuiPaneInfo& MinimizeButton(bool visible = true) { return SetFlag(buttonMinimize, visible); }

    // get/set if pane displays a pin button, pin button is a button which is used to float a panel.
    bool HasPinButton() const { return HasFlag(buttonPin); }
    wxAuiPaneInfo& PinButton(bool visible = true) { return SetFlag(buttonPin, visible); }

    // get/set the name of the pane.
    wxString GetName() const { return name; }
    wxAuiPaneInfo& Name(const wxString& n) { name = n; return *this; }

    // get/set the caption of the pane.
    wxString GetCaption() const { return caption; }
    wxAuiPaneInfo& Caption(const wxString& c) { caption = c; return *this; }

    // get/set the tooltip of the pane.
    wxString GetToolTip() const { return m_tooltip; }
    wxAuiPaneInfo& ToolTip(const wxString& t) { m_tooltip = t; return *this; }

    // get/set the pane dock position.
    int GetDirection() const { return dock_direction; }
    wxAuiPaneInfo& Direction(int direction) { dock_direction = direction; return *this; }

    // Convenience functions to the above
    wxAuiPaneInfo& Left()   { dock_direction = wxAUI_DOCK_LEFT; return *this; }
    wxAuiPaneInfo& Right()  { dock_direction = wxAUI_DOCK_RIGHT; return *this; }
    wxAuiPaneInfo& Top()    { dock_direction = wxAUI_DOCK_TOP; return *this; }
    wxAuiPaneInfo& Bottom() { dock_direction = wxAUI_DOCK_BOTTOM; return *this; }
    wxAuiPaneInfo& Center() { dock_direction = wxAUI_DOCK_CENTER; return *this; }
    wxAuiPaneInfo& Centre() { dock_direction = wxAUI_DOCK_CENTRE; return *this; }

    // get/set the layer of the pane when docked.
    // The dock layer is similar to an onion, the inner-most layer being layer 0.
    // Each shell moving in the outward direction has a higher layer number.
    // This allows for more complex docking layout formation.
    int GetLayer() const { return dock_layer; }
    wxAuiPaneInfo& Layer(int layer) { dock_layer = layer; return *this; }

    // get/set the row of the pane when docked.
    int GetRow() const { return dock_row; }
    wxAuiPaneInfo& Row(int row) { dock_row = row; return *this; }

    // get/set the position of the pane when docked.
    int GetPosition() const { return dock_pos; }
    wxAuiPaneInfo& Position(int pos) { dock_pos = pos; return *this; }

    // get/set the tab position of the pane when in a notebook.
    int GetPage() const { return m_dock_page; }
    wxAuiPaneInfo& Page(int page) { m_dock_page = page; return *this; }

    // get/set the bitmap associated with this pane.
    const wxBitmap &GetIcon() const { return icon; }
    wxAuiPaneInfo& Icon(wxBitmap bitmap) { icon = bitmap; return *this; }

    // get/set the proportion of the pane.
    int GetProportion() const { return dock_proportion; }
    wxAuiPaneInfo& Proportion(int proportion) { dock_proportion = proportion; return *this; }

    // get/set the position of the pane when floating.
    wxPoint GetFloatingPosition() const { return floating_pos; }
    wxAuiPaneInfo& FloatingPosition(int x, int y) { floating_pos.x = x; floating_pos.y = y; return *this; }
    wxAuiPaneInfo& FloatingPosition(const wxPoint& pos) { floating_pos = pos; return *this; }

    // get/set the size of the pane when floating.
    wxSize GetFloatingSize() const { return floating_size; }
    wxAuiPaneInfo& FloatingSize(int x, int y) { floating_size.Set(x,y); return *this; }
    wxAuiPaneInfo& FloatingSize(const wxSize& size) { floating_size = size; return *this; }

    // get/set the ideal size of the pane.
    wxSize GetBestSize() const { return best_size; }
    wxAuiPaneInfo& BestSize(int x, int y) { best_size.Set(x,y); return *this; }
    wxAuiPaneInfo& BestSize(const wxSize& size) { best_size = size; return *this; }

    // get/set the minimum size of the pane.
    wxSize GetMinSize() const { return min_size; }
    wxAuiPaneInfo& MinSize(int x, int y) { min_size.Set(x,y); return *this; }
    wxAuiPaneInfo& MinSize(const wxSize& size) { min_size = size; return *this; }

    // get/set the maximum size of the pane.
    wxSize GetMaxSize() const { return max_size; };
    wxAuiPaneInfo& MaxSize(int x, int y) { max_size.Set(x,y); return *this; }
    wxAuiPaneInfo& MaxSize(const wxSize& size) { max_size = size; return *this; }

    // get/set the current rectangle (populated by wxAUI).
    wxRect GetRect() const { return rect; };
    wxAuiPaneInfo& Rect(const wxRect& r) { rect = r; return *this; }

    // get the array of buttons that are present on the pane.
    wxAuiPaneButtonArray& GetButtons() { return buttons; }

    // get/set whether the containing dock can have a resize sash.
    // when DockFixed is true no sash will be available.
    bool IsDockFixed() const { return HasFlag(optionDockFixed); }
    wxAuiPaneInfo& DockFixed(bool b = true) { return SetFlag(optionDockFixed, b); }

    // get/set whether the pane should always dock in a notebook, even if not stacked with another pane
    bool IsAlwaysDockInNotebook() const { return HasFlag(optionAlwaysDockInNotebook); }
    wxAuiPaneInfo &AlwaysDockInNotebook(bool b = true) { return SetFlag(optionAlwaysDockInNotebook, b); }
	
	// get/set wether the pane should be compacted when in a notebook
	// If this is set, the tab only shows its icon if it is not active.
	bool IsCompactTab() const { return HasFlag(optionCompactTab); }
	wxAuiPaneInfo &CompactTab(bool b = true) { return SetFlag(optionCompactTab, b); }

    // Move a pane over another one, creating a notebook if allowed.
    // The pane is set in the page immediatly after the targetted one
    wxAuiPaneInfo &MoveOver(const wxAuiPaneInfo &target);

    // get/set a property flag for this pane, used internally by other get/set functions.
    bool HasFlag(int flag) const
    {
        return (state & flag) != 0;
    }
    wxAuiPaneInfo& SetFlag(int flag, bool optionState)
    {
        wxAuiPaneInfo test(*this);
        if (optionState)
             test.state |= flag;
        else
            test.state &= ~flag;
        wxCHECK_MSG(test.IsValid(), *this,
                    "window settings and pane settings are incompatible");
        if (optionState)
             this->state |= flag;
        else
            this->state &= ~flag;
        return *this;
    }
    unsigned int GetFlags() const
    {
        return state;
    }

    wxAuiPaneInfo& Flags(unsigned int f)
    {
    state = f; return *this;
    }

    // adopt the default center pane settings for this pane.
    wxAuiPaneInfo& CentrePane() { return CenterPane(); }
    wxAuiPaneInfo& CenterPane()
    {
        state = 0;
        return Center().PaneBorder().Resizable();
    }

    // adopt the default pane settings for this pane.
    wxAuiPaneInfo& DefaultPane()
    {
       wxAuiPaneInfo test(*this);
       test.state |= optionTopDockable | optionBottomDockable |
                 optionLeftDockable | optionRightDockable |
                 optionFloatable | optionMovable | optionResizable |
                 optionCaption | optionPaneBorder | buttonClose;
        wxCHECK_MSG(test.IsValid(), *this, "window settings and pane settings are incompatible");
        this->state |= optionTopDockable | optionBottomDockable |
                 optionLeftDockable | optionRightDockable |
                 optionFloatable | optionMovable | optionResizable |
                 optionCaption | optionPaneBorder | buttonClose;
        return *this;
    }

    // adopt the toolbar pane settings for this pane.
    wxAuiPaneInfo& ToolbarPane()
    {
        DefaultPane();
        state |= (optionToolbar | optionGripper);
        state &= ~(optionResizable | optionCaption);
        if (dock_layer == 0)
            dock_layer = 10;
        return *this;
    }

    bool IsValid() const;
    bool IsActive() const { return HasFlag(wxAuiPaneInfo::optionActive); }
#ifdef SWIG
    %typemap(out) wxAuiPaneInfo& ;
#endif

    // Write the safe parts of a newly loaded PaneInfo structure "source" into "this"
    // used on loading perspectives etc.
    void SafeSet(wxAuiPaneInfo source)
    {
        // note source is not passed by reference so we can overwrite, to keep the
        // unsafe bits of "dest"
        source.window  = window;
        source.frame   = frame;
        source.buttons = buttons;
        source.icon    = icon;
        // now assign
        *this = source;
    }

public:

    // NOTE: You can add and subtract flags from this list,
    // but do not change the values of the flags, because
    // they are stored in a binary integer format in the
    // perspective string.  If you really need to change the
    // values around, you'll have to ensure backwards-compatibility
    // in the perspective loading code.
    enum wxAuiPaneState
    {
        optionFloating             = 1 << 0,
        optionHidden               = 1 << 1,
        optionLeftDockable         = 1 << 2,
        optionRightDockable        = 1 << 3,
        optionTopDockable          = 1 << 4,
        optionBottomDockable       = 1 << 5,
        optionFloatable            = 1 << 6,
        optionMovable              = 1 << 7,
        optionResizable            = 1 << 8,
        optionPaneBorder           = 1 << 9,
        optionCaption              = 1 << 10,
        optionGripper              = 1 << 11,
        optionDestroyOnClose       = 1 << 12,
        optionToolbar              = 1 << 13,
        optionActive               = 1 << 14,
        optionGripperTop           = 1 << 15,
        optionMaximized            = 1 << 16,
        optionDockFixed            = 1 << 17,
        optionActiveNotebook       = 1 << 18,
        optionAlwaysDockInNotebook = 1 << 19,
        optionCenterDockable       = 1 << 20,

        buttonClose                = 1 << 21,
        buttonMaximize             = 1 << 22,
        buttonMinimize             = 1 << 23,
        buttonPin                  = 1 << 24,

		savedActiveNotebookState   = 1 << 25, // used internally
		
        buttonCustom1              = 1 << 26,
        buttonCustom2              = 1 << 27,
        buttonCustom3              = 1 << 28,

		optionCompactTab           = 1 << 29,
        savedHiddenState           = 1 << 30, // used internally
        actionPane                 = 1 << 31  // used internally
    };

public:

    // All the variables in this section are present for backward compatibility
    // but should not be used anymore.
    // Use the accessors methods to get their value. These variables may
    // be removed or modified in the future.

    wxString name;        // name of the pane
    wxString caption;     // caption displayed on the window

    wxWindow* window;     // window that is in this pane
    wxFrame* frame;       // floating frame window that holds the pane
    unsigned int state;   // a combination of wxPaneState values

    int dock_direction;   // dock direction (top, bottom, left, right, center)
    int dock_layer;       // layer number (0 = innermost layer)
    int dock_row;         // row number on the docking bar (0 = first row)
    int dock_pos;         // position inside the row (0 = first position)

    wxBitmap icon;        // bitmap associated with the pane, shown on tab(if in a notebook) or on titlebar(if one is present)

    wxSize best_size;     // size that the layout engine will prefer
    wxSize min_size;      // minimum size the pane window can tolerate
    wxSize max_size;      // maximum size the pane window can tolerate

    wxPoint floating_pos; // position while floating
    wxSize floating_size; // size while floating

    int dock_proportion;          // proportion while docked
    wxAuiPaneButtonArray buttons; // buttons on the pane
    wxRect rect;                  // current rectangle (populated by wxAUI)

private:

    wxString m_tooltip;     // tooltip displayed when hovering over title/tab of window
    int      m_dock_page;   // tab position if we are in a notebook (0 = leftmost tab)


};



class WXDLLIMPEXP_FWD_AUI wxAuiFloatingFrame;
//temp: (MJM) - The following forward declaration is only temporarily necessary until dynamic notebook buttons and normal pane buttons can be made more compatible.
class WXDLLIMPEXP_FWD_AUI wxAuiTabContainerButton;

class WXDLLIMPEXP_AUI wxAuiManager : public wxEvtHandler
{
    friend class wxAuiFloatingFrame;

public:

    wxAuiManager(wxWindow* managedWnd = NULL, unsigned int flags = wxAUI_MGR_DEFAULT);
    virtual ~wxAuiManager();
    void UnInit();

    // get/set all style property flags for this manager
    void SetFlags(unsigned int flags);
    unsigned int GetFlags() const;

    // get/set a style property flag for this manager
    bool HasFlag(int flag) const;
    void SetFlag(int flag, bool optionState);

    wxWindow* GetManagedWindow() const;
    void SetManagedWindow(wxWindow* window);

    static wxAuiManager* GetManager(wxWindow* window);

    void SetArtProvider(wxAuiDockArt* artProvider);
    wxAuiDockArt* GetArtProvider() const;

    void SetTabArtProvider(wxAuiTabArt* artProvider);
    wxAuiTabArt* GetTabArtProvider() const;

    wxAuiPaneInfo& GetPane(wxWindow* window) const;
    wxAuiPaneInfo& GetPane(const wxString& name) const;
    wxAuiPaneInfo& GetPane(size_t paneIndex) const;
    wxAuiPaneInfoArray& GetAllPanes();
    size_t GetPaneCount() const;

    bool FindTab(wxWindow* page, wxAuiTabContainer** ctrl, int* idx);

    bool AddPane(wxWindow* window, const wxAuiPaneInfo& paneInfo);

    bool AddPane(wxWindow* window, const wxAuiPaneInfo& paneInfo, const wxPoint& dropPos);

    bool AddPane(wxWindow* window, int direction = wxLEFT, const wxString& caption = wxEmptyString);

    bool InsertPane(wxWindow* window, const wxAuiPaneInfo& insertLocation, int insertLevel = wxAUI_INSERT_PANE);

    bool DetachPane(wxWindow* window);

    void Update();

    wxString SavePaneInfo(wxAuiPaneInfo& pane);
    void LoadPaneInfo(wxString panePart, wxAuiPaneInfo& pane);
    wxString SavePerspective();
    bool LoadPerspective(const wxString& perspective, bool update = true);

    void SetDockSizeConstraint(double widthPct, double heightPct);
    void GetDockSizeConstraint(double* widthPct, double* heightPct) const;

    //Close pane, return true if close successful or false otherwise (event vetoed)
    bool ClosePane(wxAuiPaneInfo& paneInfo);
    void MaximizePane(wxAuiPaneInfo& paneInfo);
    void RestorePane(wxAuiPaneInfo& paneInfo);
    void RestoreMaximizedPane();

    int SetActivePane(wxWindow* activePane);
    int GetActivePane(wxWindow* focus) const;

    virtual wxAuiFloatingFrame* CreateFloatingFrame(wxWindow* parent, const wxAuiPaneInfo& p);
    virtual bool CanDockPanel(const wxAuiPaneInfo& p);

    void StartPaneDrag(wxWindow* paneWindow, const wxPoint& offset);

    wxRect CalculateHintRect(wxWindow* paneWindow, const wxPoint& pt, const wxPoint& offset);

    void DrawHintRect(wxWindow* paneWindow, const wxPoint& pt, const wxPoint& offset);

    virtual void ShowHint(const wxRect& rect);
    virtual void HideHint();

    void OnHintActivate(wxActivateEvent& event);


protected:
    //Layout helper functions.
    void DoFrameLayout();
    bool CanDockOver(const wxAuiPaneInfo & pane, const wxAuiPaneInfo & covered_pane);
    bool MustDockInNotebook(const wxAuiPaneInfo &pane) const;
	bool AreInSameNotebook(const wxAuiPaneInfo &pane0, const wxAuiPaneInfo &pane1);
    void LayoutAddPane(wxSizer* container, wxAuiDockInfo& dock, wxAuiPaneInfo& pane, wxAuiDockUIPartArray& uiparts, bool spacerOnly, bool allowtitlebar=true);
    void LayoutAddDock(wxSizer* container, wxAuiDockInfo& dock, wxAuiDockUIPartArray& uiParts, bool spacerOnly);
    void LayoutAddNotebook(wxAuiTabArt* tabArt, wxAuiTabContainer* notebookContainer, wxSizer* notebookSizer, wxAuiDockUIPart& part, wxAuiDockInfo& dock, wxAuiDockUIPartArray& uiparts, wxAuiTabContainerPointerArray& tabContainerRecalcList, wxAuiSizerItemPointerArray& tabContainerRecalcSizers, wxAuiPaneInfo* pane, int orient);
    wxSizer* LayoutAll(wxAuiPaneInfoArray& panes, wxAuiDockInfoArray& docks, wxAuiDockUIPartArray& uiParts, bool spacerOnly = false);

    //Functions for handling pane drag and drop.
    bool DoDrop(wxAuiDockInfoArray& docks, wxAuiPaneInfoArray& panes, wxAuiPaneInfo& drop, const wxPoint& pt, const wxPoint& actionOffset = wxPoint(0,0));
    bool DoDropExternal(wxAuiManager* otherMgr,wxWindow* otherWnd,wxAuiPaneInfo& drop, const wxPoint& screenPt, const wxPoint& actionOffset = wxPoint(0,0));
    virtual bool ProcessDockResult(wxAuiPaneInfo& target, const wxAuiPaneInfo& newPos);

    //Functions to handle rendering
    void Render(wxDC* dc);
    void Repaint(wxDC* dc = NULL);

    //Tooltip related functions
    void ShowToolTip(wxAuiPaneInfo* pane);
    void HideToolTip();

    void UpdateHintWindowConfig();

    wxAuiDockUIPart* HitTest(int x, int y);
    wxAuiDockUIPart* GetPanePart(wxWindow* pane);
    int GetDockPixelOffset(wxAuiPaneInfo& test);

    void ProcessMgrEvent(wxAuiManagerEvent& event);
    void UpdateButtonOnScreen(wxAuiDockUIPart* buttonUiPart, const wxMouseEvent& event);
    void GetPanePositionsAndSizes(wxAuiDockInfo& dock, wxArrayInt& positions, wxArrayInt& sizes);

    /// Ends a resize action, or for live update, resizes the sash
    bool DoEndResizeAction(wxMouseEvent& event);

    int GetNotebookFlags();

public:

    // public events (which can be invoked externally)
    void OnRender(wxAuiManagerEvent& evt);
    void OnPaneButton(wxAuiManagerEvent& evt);

protected:

    // protected events
    void OnFloatingPaneMoveStart(wxWindow* window);
    void OnFloatingPaneMoving(wxWindow* window, wxDirection dir );
    void OnFloatingPaneMoved(wxWindow* window, wxDirection dir);
    void OnFloatingPaneActivated(wxWindow* window);
    void OnFloatingPaneClosed(wxWindow* window, wxCloseEvent& evt);
    void OnFloatingPaneResized(wxWindow* window, const wxRect& rect);
    void OnPaint(wxPaintEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnSetCursor(wxSetCursorEvent& evt);
    void OnLeftDClick(wxMouseEvent& evt);
    void OnLeftDown(wxMouseEvent& evt);
    void OnLeftUp(wxMouseEvent& evt);
    void OnRightDown(wxMouseEvent& evt);
    void OnRightUp(wxMouseEvent& evt);
    void OnMiddleDown(wxMouseEvent& evt);
    void OnMiddleUp(wxMouseEvent& evt);
    void OnMotion(wxMouseEvent& evt);
    void OnCaptureLost(wxMouseCaptureLostEvent& evt);
    void OnLeaveWindow(wxMouseEvent& evt);
    void OnChildFocus(wxChildFocusEvent& evt);
    void OnHintFadeTimer(wxTimerEvent& evt);
    void OnFindManager(wxAuiManagerEvent& evt);


    enum
    {
        actionNone = 0,
        actionResize,
        actionClickButton,
        actionClickCaption,
        actionDragToolbarPane,
        actionDragFloatingPane,
        actionDragMovablePane
    };


    wxWindow* m_frame;               // the window being managed
    wxAuiDockArt* m_art;             // dock art object which does all drawing
    wxAuiTabArt* m_tab_art;          // tab art object which does all the drawing for notebooks.
    unsigned int m_flags;            // manager flags wxAUI_MGR_*

    wxAuiPaneInfoArray m_panes;      // array of panes structures
    wxAuiDockInfoArray m_docks;      // array of docks structures
    wxAuiDockUIPartArray m_uiParts;  // array of UI parts (captions, buttons, etc)

    int m_action;                    // current mouse action
    wxPoint m_actionStart;           // position where the action click started
    wxPoint m_actionOffset;          // offset from upper left of the item clicked
    wxRect* m_actionDeadZone;        // area of the screen in which movement should be temporarily ignored (up until the point the mouse has left this region) - this is used to prevent problems with e.g. differently sized tabs switching repeatedly on drag due to size differences
    wxAuiDockUIPart* m_actionPart;   // ptr to the part the action happened to
    wxWindow* m_actionWindow;        // action frame or window (NULL if none)
    wxRect m_actionHintRect;         // hint rectangle for the action
    wxRect m_lastRect;
    wxAuiDockUIPart* m_hoverButton;  // button uipart being hovered over
    //temp: (MJM) - The following variable is only temporarily necessary until dynamic notebook buttons and normal pane buttons can be made more compatible. Remove this as well as forward declaration further up when possible.
    wxAuiTabContainerButton* m_hoverButton2;// button uipart being hovered over
    wxPoint m_lastMouseMove;         // last mouse move position (see OnMotion)
    int  m_currentDragItem;
    bool m_skipping;
    bool m_hasMaximized;
    bool m_doingHintCalculation;     // set to true if we are in the process of calculating a hint.

    double m_dockConstraintX;        // 0.0 .. 1.0; max pct of window width a dock can consume
    double m_dockConstraintY;        // 0.0 .. 1.0; max pct of window height a dock can consume

    wxFrame* m_hintWnd;              // transparent hint window, if supported by platform
    wxTimer m_hintFadeTimer;         // transparent fade timer
    wxByte m_hintFadeAmt;            // transparent fade amount
    wxByte m_hintFadeMax;            // maximum value of hint fade
    wxRect m_lastHint;               // position of last drawn hint, so that we can avoid drawing the same hint repeatedly.

    void* m_reserved;

#ifndef SWIG
    DECLARE_EVENT_TABLE()
    DECLARE_CLASS(wxAuiManager)
#endif // SWIG
};



// event declarations/classes

class WXDLLIMPEXP_AUI wxAuiManagerEvent : public wxEvent
{
public:
    wxAuiManagerEvent(wxEventType type=wxEVT_NULL) : wxEvent(0, type)
    {
        manager = NULL;
        pane = NULL;
        target_pane = NULL;
        button = 0;
        veto_flag = false;
        canveto_flag = true;
        dc = NULL;
    }
#ifndef SWIG
    wxAuiManagerEvent(const wxAuiManagerEvent& c) : wxEvent(c)
    {
        manager = c.manager;
        pane = c.pane;
        target_pane = c.target_pane;
        button = c.button;
        veto_flag = c.veto_flag;
        canveto_flag = c.canveto_flag;
        dc = c.dc;
    }
#endif
    wxEvent *Clone() const { return new wxAuiManagerEvent(*this); }

    void SetManager(wxAuiManager* mgr) { manager = mgr; }
    void SetPane(wxAuiPaneInfo* p) { pane = p; }
    void SetTargetPane(wxAuiPaneInfo *p) { target_pane = p; }
    void SetButton(int b) { button = b; }
    void SetDC(wxDC* pdc) { dc = pdc; }

    wxAuiManager* GetManager() const { return manager; }
    wxAuiPaneInfo* GetPane() const { return pane; }
    wxAuiPaneInfo* GetTargetPane() const { return target_pane; }
    int GetButton() const { return button; }
    wxDC* GetDC() const { return dc; }

    void Allow(bool allow = true) { veto_flag = !allow; }
    bool IsAllowed() const { return !CanVeto(); }
    void Veto(bool veto = true) { veto_flag = veto; }
    bool GetVeto() const { return veto_flag; }
    void SetCanVeto(bool can_veto) { canveto_flag = can_veto; }
    bool CanVeto() const { return  canveto_flag && veto_flag; }

public:
    wxAuiManager  * manager;
    wxAuiPaneInfo * pane;
    wxAuiPaneInfo * target_pane;
    int  button;
    bool veto_flag;
    bool canveto_flag;
    wxDC* dc;

#ifndef SWIG
private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxAuiManagerEvent)
#endif
};


class WXDLLIMPEXP_AUI wxAuiDockInfo
{
public:
    wxAuiDockInfo()
    {
        dock_direction = 0;
        dock_layer = 0;
        dock_row = 0;
        size = 0;
        min_size = 0;
        resizable = true;
        fixed = false;
        toolbar = false;
        reserved1 = false;
    }

#ifndef SWIG
    wxAuiDockInfo(const wxAuiDockInfo& c)
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
        reserved1 = c.reserved1;
    }

    wxAuiDockInfo& operator=(const wxAuiDockInfo& c)
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
        reserved1 = c.reserved1;
        return *this;
    }
#endif // SWIG

    bool IsOk() const { return dock_direction != 0; }
    bool IsHorizontal() const { return dock_direction == wxAUI_DOCK_TOP ||
                             dock_direction == wxAUI_DOCK_BOTTOM; }
    bool IsVertical() const { return dock_direction == wxAUI_DOCK_LEFT ||
                             dock_direction == wxAUI_DOCK_RIGHT ||
                             dock_direction == wxAUI_DOCK_CENTER; }
public:
    wxAuiPaneInfoPtrArray panes; // array of panes
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
    bool reserved1;

    bool IsValid() const;
};


class WXDLLIMPEXP_AUI wxAuiDockUIPart
{
public:
    virtual ~wxAuiDockUIPart();
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
        typePaneButton,
        typePaneTab
    };

    int type;                            // ui part type (see enum above)
    int orientation;                     // orientation (either wxHORIZONTAL or wxVERTICAL)
    wxAuiDockInfo* dock;                 // which dock the item is associated with
    wxAuiPaneInfo* pane;                 // which pane the item is associated with
    wxAuiPaneButton* button;             // which pane button the item is associated with
    wxSizer* cont_sizer;                 // the part's containing sizer
    wxSizerItem* sizer_item;             // the sizer item of the part
    wxAuiTabContainer* m_tab_container;  // notebook pane is associated with (if any)
    wxRect rect;                         // client coord rectangle of the part itself
};


class WXDLLIMPEXP_AUI wxAuiPaneButton
{
public:
    int button_id;        // id of the button (e.g. buttonClose)
};



#ifndef SWIG

wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_AUI, wxEVT_AUI_PANE_BUTTON, wxAuiManagerEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_AUI, wxEVT_AUI_PANE_CLOSE, wxAuiManagerEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_AUI, wxEVT_AUI_PANE_MAXIMIZE, wxAuiManagerEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_AUI, wxEVT_AUI_PANE_RESTORE, wxAuiManagerEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_AUI, wxEVT_AUI_PANE_ACTIVATED, wxAuiManagerEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_AUI, wxEVT_AUI_PANE_DOCK_OVER, wxAuiManagerEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_AUI, wxEVT_AUI_RENDER, wxAuiManagerEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_AUI, wxEVT_AUI_FIND_MANAGER, wxAuiManagerEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_AUI, wxEVT_AUI_ALLOW_DND, wxAuiManagerEvent );


typedef void (wxEvtHandler::*wxAuiManagerEventFunction)(wxAuiManagerEvent&);

#define wxAuiManagerEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxAuiManagerEventFunction, func)

#define EVT_AUI_PANE_BUTTON(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_PANE_BUTTON, wxAuiManagerEventHandler(func))
#define EVT_AUI_PANE_CLOSE(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_PANE_CLOSE, wxAuiManagerEventHandler(func))
#define EVT_AUI_PANE_MAXIMIZE(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_PANE_MAXIMIZE, wxAuiManagerEventHandler(func))
#define EVT_AUI_PANE_RESTORE(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_PANE_RESTORE, wxAuiManagerEventHandler(func))
#define EVT_AUI_PANE_ACTIVATED(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_PANE_ACTIVATED, wxAuiManagerEventHandler(func))
#define EVT_AUI_PANE_DOCK_OVER(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_PANE_DOCK_OVER, wxAuiManagerEventHandler(func))
#define EVT_AUI_RENDER(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_RENDER, wxAuiManagerEventHandler(func))
#define EVT_AUI_FIND_MANAGER(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_FIND_MANAGER, wxAuiManagerEventHandler(func))
#define EVT_AUI_ALLOW_DND(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_ALLOW_DND, wxAuiManagerEventHandler(func))

#else

%constant wxEventType wxEVT_AUI_PANE_BUTTON;
%constant wxEventType wxEVT_AUI_PANE_CLOSE;
%constant wxEventType wxEVT_AUI_PANE_MAXIMIZE;
%constant wxEventType wxEVT_AUI_PANE_RESTORE;
%constant wxEventType wxEVT_AUI_PANE_ACTIVATED;
%constant wxEventType wxEVT_AUI_PANE_DOCK_OVER;
%constant wxEventType wxEVT_AUI_RENDER;
%constant wxEventType wxEVT_AUI_FIND_MANAGER;
%constant wxEventType wxEVT_AUI_ALLOW_DND;

%pythoncode {
    EVT_AUI_PANE_BUTTON = wx.PyEventBinder( wxEVT_AUI_PANE_BUTTON )
    EVT_AUI_PANE_CLOSE = wx.PyEventBinder( wxEVT_AUI_PANE_CLOSE )
    EVT_AUI_PANE_MAXIMIZE = wx.PyEventBinder( wxEVT_AUI_PANE_MAXIMIZE )
    EVT_AUI_PANE_RESTORE = wx.PyEventBinder( wxEVT_AUI_PANE_RESTORE )
    EVT_AUI_PANE_ACTIVATED = wx.PyEventBinder( wxEVT_AUI_PANE_ACTIVATED )
    EVT_AUI_PANE_DOCK_OVER = wx.PyEventBinder( wxEVT_AUI_PANE_DOCK_OVER )
    EVT_AUI_RENDER = wx.PyEventBinder( wxEVT_AUI_RENDER )
    EVT_AUI_FIND_MANAGER = wx.PyEventBinder( wxEVT_AUI_FIND_MANAGER )
    EVT_AUI_ALLOW_DND = wx.PyEventBinder( wxEVT_AUI_ALLOW_DND )
}
#endif // SWIG

#endif // wxUSE_AUI
#endif //_WX_FRAMEMANAGER_H_

