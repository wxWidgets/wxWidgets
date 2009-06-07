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
    wxAUI_MGR_ALLOW_FLOATING           = 1 << 0,
    wxAUI_MGR_ALLOW_ACTIVE_PANE        = 1 << 1,
    wxAUI_MGR_TRANSPARENT_DRAG         = 1 << 2,
    wxAUI_MGR_TRANSPARENT_HINT         = 1 << 3,
    wxAUI_MGR_VENETIAN_BLINDS_HINT     = 1 << 4,
    wxAUI_MGR_RECTANGLE_HINT           = 1 << 5,
    wxAUI_MGR_HINT_FADE                = 1 << 6,
    wxAUI_MGR_NO_VENETIAN_BLINDS_FADE  = 1 << 7,
    wxAUI_MGR_LIVE_RESIZE              = 1 << 8,

    wxAUI_MGR_DEFAULT = wxAUI_MGR_ALLOW_FLOATING |
                        wxAUI_MGR_TRANSPARENT_HINT |
                        wxAUI_MGR_HINT_FADE |
                        wxAUI_MGR_NO_VENETIAN_BLINDS_FADE
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
class wxAuiManagerEvent;

#ifndef SWIG
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

    wxAuiPaneInfo()
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

        SetDefaultPane();
    }

    ~wxAuiPaneInfo() {}

#ifndef SWIG
    wxAuiPaneInfo(const wxAuiPaneInfo& c)
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

    wxAuiPaneInfo& operator=(const wxAuiPaneInfo& c)
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
#else // SWIG
    %typemap(out) wxAuiPaneInfo& { $result = $self; Py_INCREF($result); }
#endif // !SWIG

#if WXWIN_COMPATIBILITY_2_8
    wxDEPRECATED( wxAuiPaneInfo& Window(wxWindow* w) { window = w; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& Name(const wxString& n) { name = n; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& Caption(const wxString& c) { caption = c; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& Left() { dock_direction = wxAUI_DOCK_LEFT; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& Right() { dock_direction = wxAUI_DOCK_RIGHT; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& Top() { dock_direction = wxAUI_DOCK_TOP; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& Bottom() { dock_direction = wxAUI_DOCK_BOTTOM; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& Center() { dock_direction = wxAUI_DOCK_CENTER; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& Centre() { dock_direction = wxAUI_DOCK_CENTRE; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& Direction(int direction) { dock_direction = direction; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& Layer(int layer) { dock_layer = layer; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& Row(int row) { dock_row = row; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& Position(int pos) { dock_pos = pos; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& BestSize(const wxSize& size) { best_size = size; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& MinSize(const wxSize& size) { min_size = size; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& MaxSize(const wxSize& size) { max_size = size; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& BestSize(int x, int y) { best_size.Set(x,y); return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& MinSize(int x, int y) { min_size.Set(x,y); return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& MaxSize(int x, int y) { max_size.Set(x,y); return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& FloatingPosition(const wxPoint& pos) { floating_pos = pos; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& FloatingPosition(int x, int y) { floating_pos.x = x; floating_pos.y = y; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& FloatingSize(const wxSize& size) { floating_size = size; return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& FloatingSize(int x, int y) { floating_size.Set(x,y); return *this; } );
    wxDEPRECATED( wxAuiPaneInfo& Fixed() { return SetFlag(optionResizable, false); } );
    wxDEPRECATED( wxAuiPaneInfo& Resizable(bool resizable = true) { return SetFlag(optionResizable, resizable); } );
    wxDEPRECATED( wxAuiPaneInfo& CaptionVisible(bool visible = true) { return SetFlag(optionCaption, visible); } );
    wxDEPRECATED( wxAuiPaneInfo& PaneBorder(bool visible = true) { return SetFlag(optionPaneBorder, visible); } );
    wxDEPRECATED( wxAuiPaneInfo& Gripper(bool visible = true) { return SetFlag(optionGripper, visible); } );
    wxDEPRECATED( wxAuiPaneInfo& GripperTop(bool attop = true) { return SetFlag(optionGripperTop, attop); } );
    wxDEPRECATED( wxAuiPaneInfo& CloseButton(bool visible = true) { return SetFlag(buttonClose, visible); } );
    wxDEPRECATED( wxAuiPaneInfo& MaximizeButton(bool visible = true) { return SetFlag(buttonMaximize, visible); } );
    wxDEPRECATED( wxAuiPaneInfo& MinimizeButton(bool visible = true) { return SetFlag(buttonMinimize, visible); } );
    wxDEPRECATED( wxAuiPaneInfo& PinButton(bool visible = true) { return SetFlag(buttonPin, visible); } );
    wxDEPRECATED( wxAuiPaneInfo& DestroyOnClose(bool b = true) { return SetFlag(optionDestroyOnClose, b); } );
    wxDEPRECATED( wxAuiPaneInfo& TopDockable(bool b = true) { return SetFlag(optionTopDockable, b); } );
    wxDEPRECATED( wxAuiPaneInfo& BottomDockable(bool b = true) { return SetFlag(optionBottomDockable, b); } );
    wxDEPRECATED( wxAuiPaneInfo& LeftDockable(bool b = true) { return SetFlag(optionLeftDockable, b); } );
    wxDEPRECATED( wxAuiPaneInfo& RightDockable(bool b = true) { return SetFlag(optionRightDockable, b); } );
    wxDEPRECATED( wxAuiPaneInfo& Floatable(bool b = true) { return SetFlag(optionFloatable, b); } );
    wxDEPRECATED( wxAuiPaneInfo& Movable(bool b = true) { return SetFlag(optionMovable, b); } );
    wxDEPRECATED( wxAuiPaneInfo& DockFixed(bool b = true) { return SetFlag(optionDockFixed, b); } );
    wxDEPRECATED( wxAuiPaneInfo& Dockable(bool b = true) { return SetTopDockable(b).SetBottomDockable(b).SetLeftDockable(b).SetRightDockable(b); } );
    wxDEPRECATED( wxAuiPaneInfo& CentrePane() { return SetCenterPane(); } );
    wxDEPRECATED( wxAuiPaneInfo& CenterPane()
    {
        state = 0;
        return SetDirectionCenter().SetBorder().SetResizable();
    } );
    wxDEPRECATED( wxAuiPaneInfo& DefaultPane()
    {
        state |= optionTopDockable | optionBottomDockable |
                 optionLeftDockable | optionRightDockable |
                 optionFloatable | optionMovable | optionResizable |
                 optionCaption | optionPaneBorder | buttonClose;
        return *this;
    } );
    wxDEPRECATED( wxAuiPaneInfo& ToolbarPane()
    {
        SetDefaultPane();
        state |= (optionToolbar | optionGripper);
        state &= ~(optionResizable | optionCaption);
        if (dock_layer == 0)
            dock_layer = 10;
        return *this;
    } );
#else // !WXWIN_COMPATIBILITY_2_8
#endif // WXWIN_COMPATIBILITY_2_8/!WXWIN_COMPATIBILITY_2_8
    // returns true if the pane contains a toolbar.
    bool IsToolbar() const { return HasFlag(optionToolbar); }

    // returns true if the wxAuiPaneInfo structure is valid. A pane structure is valid if it has an associated window.
    bool IsOk() const { return window != NULL; }
    // associate a window with this pane
    wxAuiPaneInfo& SetWindow(wxWindow* w) { window = w; return *this; }

    // get/set if the pane is resizable.
    // opposite of Is/SetFixed.
    bool IsResizable() const { return HasFlag(optionResizable); }
    wxAuiPaneInfo& SetResizable(bool resizable = true) { return SetFlag(optionResizable, resizable); }
    // get/set whether the pane is not resizable.
    // opposite of Is/SetResizable.
    bool IsFixed() const { return !HasFlag(optionResizable); }
    wxAuiPaneInfo& SetFixed() { return SetFlag(optionResizable, false); }

    // get/set if the pane is visible.
    bool IsShown() const { return !HasFlag(optionHidden); }
    wxAuiPaneInfo& Hide() { return SetFlag(optionHidden, true); }
    wxAuiPaneInfo& Show(bool show = true) { return SetFlag(optionHidden, !show); }

    // get/set if the pane is floating.
    // opposite of IsDocked.
    bool IsFloating() const { return HasFlag(optionFloating); }
    wxAuiPaneInfo& Float() { return SetFlag(optionFloating, true); }
    // get/set if the pane is docked.
    // opposite of IsFloating.
    bool IsDocked() const { return !HasFlag(optionFloating); }
    wxAuiPaneInfo& Dock() { return SetFlag(optionFloating, false); }

    // get/set whether the pane can be docked at the top of the frame.
    bool IsTopDockable() const { return HasFlag(optionTopDockable); }
    wxAuiPaneInfo& SetTopDockable(bool b = true) { return SetFlag(optionTopDockable, b); }
    // get/set whether the pane can be docked at the bottom of the frame.
    bool IsBottomDockable() const { return HasFlag(optionBottomDockable); }
    wxAuiPaneInfo& SetBottomDockable(bool b = true) { return SetFlag(optionBottomDockable, b); }
    // get/set whether the pane can be docked at the left of the frame.
    bool IsLeftDockable() const { return HasFlag(optionLeftDockable); }
    wxAuiPaneInfo& SetLeftDockable(bool b = true) { return SetFlag(optionLeftDockable, b); }
    // get/set whether the pane can be docked at the right of the frame.
    bool IsRightDockable() const { return HasFlag(optionRightDockable); }
    wxAuiPaneInfo& SetRightDockable(bool b = true) { return SetFlag(optionRightDockable, b); }
    // convenience function to set all of the above at once.
    wxAuiPaneInfo& SetDockable(bool b = true)
    {
        return SetTopDockable(b).SetBottomDockable(b).SetLeftDockable(b).SetRightDockable(b);
    }

    // get/set if pane can be undocked and displayed as a floating window.
    bool IsFloatable() const { return HasFlag(optionFloatable); }
    wxAuiPaneInfo& SetFloatable(bool b = true) { return SetFlag(optionFloatable, b); }

    // get/set if pane can be undocked or moved to another dock position.
    bool IsMovable() const { return HasFlag(optionMovable); }
    wxAuiPaneInfo& SetMovable(bool b = true) { return SetFlag(optionMovable, b); }

    // get/set if pane is maximized.
    bool IsMaximized() const { return HasFlag(optionMaximized); }
    wxAuiPaneInfo& Maximize() { return SetFlag(optionMaximized, true); }
    wxAuiPaneInfo& Restore() { return SetFlag(optionMaximized, false); }

    // get/set whether a pane should be destroyed when closed, instead of the default behaviour of hiding a closed pane.
    bool IsDestroyOnClose() const { return HasFlag(optionDestroyOnClose); }
    wxAuiPaneInfo& SetDestroyOnClose(bool b = true) { return SetFlag(optionDestroyOnClose, b); }

    // get/set if pane displays a caption.
    bool HasCaption() const { return HasFlag(optionCaption); }
    wxAuiPaneInfo& SetCaptionVisible(bool visible = true) { return SetFlag(optionCaption, visible); }

    // get/set if pane displays a gripper.
    bool HasGripper() const { return HasFlag(optionGripper); }
    wxAuiPaneInfo& SetGripper(bool visible = true) { return SetFlag(optionGripper, visible); }
    // get/set if pane displays a gripper at the top.
    bool HasGripperTop() const { return HasFlag(optionGripperTop); }
    wxAuiPaneInfo& SetGripperTop(bool attop = true) { return SetFlag(optionGripperTop, attop); }

    // get/set if pane displays a border.
    bool HasBorder() const { return HasFlag(optionPaneBorder); }
    wxAuiPaneInfo& SetBorder(bool visible = true) { return SetFlag(optionPaneBorder, visible); }

    // get/set if pane displays a close button.
    bool HasCloseButton() const { return HasFlag(buttonClose); }
    wxAuiPaneInfo& SetCloseButton(bool visible = true) { return SetFlag(buttonClose, visible); }

    // get/set if pane displays a maximize button.
    bool HasMaximizeButton() const { return HasFlag(buttonMaximize); }
    wxAuiPaneInfo& SetMaximizeButton(bool visible = true) { return SetFlag(buttonMaximize, visible); }

    // get/set if pane displays a minimize button.
    bool HasMinimizeButton() const { return HasFlag(buttonMinimize); }
    wxAuiPaneInfo& SetMinimizeButton(bool visible = true) { return SetFlag(buttonMinimize, visible); }

    // get/set if pane displays a pin button, pin button is a button which is used to float a panel.
    bool HasPinButton() const { return HasFlag(buttonPin); }
    wxAuiPaneInfo& SetPinButton(bool visible = true) { return SetFlag(buttonPin, visible); }

    // get/set the name of the pane.
    wxAuiPaneInfo& SetName(const wxString& n) { name = n; return *this; }
    wxString GetName() { return name; }

    // get/set the caption of the pane.
    wxAuiPaneInfo& SetCaption(const wxString& c) { caption = c; return *this; }
    wxString GetCaption() { return caption; }

    // get/set the pane dock position.
    wxAuiPaneInfo& SetDirection(int direction) { dock_direction = direction; return *this; }
    int GetDirection() { return dock_direction; }
    // Convenience functions to the above
    wxAuiPaneInfo& SetDirectionLeft() { dock_direction = wxAUI_DOCK_LEFT; return *this; }
    wxAuiPaneInfo& SetDirectionRight() { dock_direction = wxAUI_DOCK_RIGHT; return *this; }
    wxAuiPaneInfo& SetDirectionTop() { dock_direction = wxAUI_DOCK_TOP; return *this; }
    wxAuiPaneInfo& SetDirectionBottom() { dock_direction = wxAUI_DOCK_BOTTOM; return *this; }
    wxAuiPaneInfo& SetDirectionCenter() { dock_direction = wxAUI_DOCK_CENTER; return *this; }
    wxAuiPaneInfo& SetDirectionCentre() { dock_direction = wxAUI_DOCK_CENTRE; return *this; }

    // get/set the layer of the pane when docked.
    // The dock layer is similar to an onion, the inner-most layer being layer 0.
    // Each shell moving in the outward direction has a higher layer number.
    // This allows for more complex docking layout formation.
    wxAuiPaneInfo& SetLayer(int layer) { dock_layer = layer; return *this; }
    int GetLayer() { return dock_layer; }

    // get/set the row of the pane when docked.
    wxAuiPaneInfo& SetRow(int row) { dock_row = row; return *this; }
    int GetRow() { return dock_row; }

    // get/set the position of the pane when docked.
    wxAuiPaneInfo& SetPosition(int pos) { dock_pos = pos; return *this; }
    int GetPosition() { return dock_pos; }

    // get/set the position of the pane when floating.
    wxAuiPaneInfo& SetFloatingPosition(int x, int y) { floating_pos.x = x; floating_pos.y = y; return *this; }
    wxAuiPaneInfo& SetFloatingPosition(const wxPoint& pos) { floating_pos = pos; return *this; }
    wxPoint GetFloatingPosition() { return floating_pos; }

    // get/set the size of the pane when floating.
    wxAuiPaneInfo& SetFloatingSize(int x, int y) { floating_size.Set(x,y); return *this; }
    wxAuiPaneInfo& SetFloatingSize(const wxSize& size) { floating_size = size; return *this; }
    wxSize GetFloatingSize() { return floating_size; }

    // get/set the ideal size of the pane.
    wxAuiPaneInfo& SetBestSize(int x, int y) { best_size.Set(x,y); return *this; }
    wxAuiPaneInfo& SetBestSize(const wxSize& size) { best_size = size; return *this; }
    wxSize GetBestSize() { return best_size; }

    // get/set the minimum size of the pane.
    wxAuiPaneInfo& SetMinSize(int x, int y) { min_size.Set(x,y); return *this; }
    wxAuiPaneInfo& SetMinSize(const wxSize& size) { min_size = size; return *this; }
    wxSize GetMinSize() { return min_size; }

    // get/set the maximum size of the pane.
    wxAuiPaneInfo& SetMaxSize(int x, int y) { max_size.Set(x,y); return *this; }
    wxAuiPaneInfo& SetMaxSize(const wxSize& size) { max_size = size; return *this; }
    wxSize GetMaxSize() { return max_size; };

    // get/set whether the containing dock can have a resize sash.
    // when DockFixed is true no sash will be available.
    wxAuiPaneInfo& SetDockFixed(bool b = true) { return SetFlag(optionDockFixed, b); }
    bool IsDockFixed() { return HasFlag(optionDockFixed); }

    // get/set a property flag for this pane, used internally by other get/set functions.
    wxAuiPaneInfo& SetFlag(int flag, bool option_state)
    {
        if (option_state)
            state |= flag;
        else
            state &= ~flag;
        return *this;
    }
    bool HasFlag(int flag) const
    {
        return (state & flag) != 0;
    }

    // adopt the default center pane settings for this pane.
    wxAuiPaneInfo& SetCentrePane() { return SetCenterPane(); }
    wxAuiPaneInfo& SetCenterPane()
    {
        state = 0;
        return SetDirectionCenter().SetBorder().SetResizable();
    }

    // adopt the default pane settings for this pane.
    wxAuiPaneInfo& SetDefaultPane()
    {
        state |= optionTopDockable | optionBottomDockable |
                 optionLeftDockable | optionRightDockable |
                 optionFloatable | optionMovable | optionResizable |
                 optionCaption | optionPaneBorder | buttonClose;
        return *this;
    }

    // adopt the toolbar pane settings for this pane.
    wxAuiPaneInfo& SetToolbarPane()
    {
        SetDefaultPane();
        state |= (optionToolbar | optionGripper);
        state &= ~(optionResizable | optionCaption);
        if (dock_layer == 0)
            dock_layer = 10;
        return *this;
    }

#ifdef SWIG
    %typemap(out) wxAuiPaneInfo& ;
#endif

    // Write the safe parts of a newly loaded PaneInfo structure "source" into "this"
    // used on loading perspectives etc.
    void SafeSet(wxAuiPaneInfo source)
    {
        // note source is not passed by reference so we can overwrite, to keep the
        // unsafe bits of "dest"
        source.window = window;
        source.frame = frame;
        source.buttons = buttons;
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
        optionMaximized       = 1 << 16,
        optionDockFixed       = 1 << 17,

        buttonClose           = 1 << 21,
        buttonMaximize        = 1 << 22,
        buttonMinimize        = 1 << 23,
        buttonPin             = 1 << 24,

        buttonCustom1         = 1 << 26,
        buttonCustom2         = 1 << 27,
        buttonCustom3         = 1 << 28,

        savedHiddenState      = 1 << 30, // used internally
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

    wxAuiPaneButtonArray buttons; // buttons on the pane

    wxRect rect;              // current rectangle (populated by wxAUI)
};



class WXDLLIMPEXP_FWD_AUI wxAuiFloatingFrame;

class WXDLLIMPEXP_AUI wxAuiManager : public wxEvtHandler
{
    friend class wxAuiFloatingFrame;

public:

    wxAuiManager(wxWindow* managed_wnd = NULL,
                   unsigned int flags = wxAUI_MGR_DEFAULT);
    virtual ~wxAuiManager();
    void UnInit();

    void SetFlags(unsigned int flags);
    unsigned int GetFlags() const;

    void SetManagedWindow(wxWindow* managed_wnd);
    wxWindow* GetManagedWindow() const;

    static wxAuiManager* GetManager(wxWindow* window);

    void SetArtProvider(wxAuiDockArt* art_provider);
    wxAuiDockArt* GetArtProvider() const;

    wxAuiPaneInfo& GetPane(wxWindow* window);
    wxAuiPaneInfo& GetPane(const wxString& name);
    wxAuiPaneInfoArray& GetAllPanes();

    bool AddPane(wxWindow* window,
                 const wxAuiPaneInfo& pane_info);

    bool AddPane(wxWindow* window,
                 const wxAuiPaneInfo& pane_info,
                 const wxPoint& drop_pos);

    bool AddPane(wxWindow* window,
                 int direction = wxLEFT,
                 const wxString& caption = wxEmptyString);

    bool InsertPane(wxWindow* window,
                 const wxAuiPaneInfo& insert_location,
                 int insert_level = wxAUI_INSERT_PANE);

    bool DetachPane(wxWindow* window);

    void Update();

    wxString SavePaneInfo(wxAuiPaneInfo& pane);
    void LoadPaneInfo(wxString pane_part, wxAuiPaneInfo &pane);
    wxString SavePerspective();
    bool LoadPerspective(const wxString& perspective, bool update = true);

    void SetDockSizeConstraint(double width_pct, double height_pct);
    void GetDockSizeConstraint(double* width_pct, double* height_pct) const;

    void ClosePane(wxAuiPaneInfo& pane_info);
    void MaximizePane(wxAuiPaneInfo& pane_info);
    void RestorePane(wxAuiPaneInfo& pane_info);
    void RestoreMaximizedPane();

public:

    virtual wxAuiFloatingFrame* CreateFloatingFrame(wxWindow* parent, const wxAuiPaneInfo& p);
    virtual bool CanDockPanel(const wxAuiPaneInfo & p);

    void StartPaneDrag(
                 wxWindow* pane_window,
                 const wxPoint& offset);

    wxRect CalculateHintRect(
                 wxWindow* pane_window,
                 const wxPoint& pt,
                 const wxPoint& offset);

    void DrawHintRect(
                 wxWindow* pane_window,
                 const wxPoint& pt,
                 const wxPoint& offset);

    virtual void ShowHint(const wxRect& rect);
    virtual void HideHint();

    void OnHintActivate(wxActivateEvent& event);

public:

    // deprecated -- please use SetManagedWindow() and
    // and GetManagedWindow() instead

    wxDEPRECATED( void SetFrame(wxFrame* frame) );
    wxDEPRECATED( wxFrame* GetFrame() const );

protected:

    void UpdateHintWindowConfig();

    void DoFrameLayout();

    void LayoutAddPane(wxSizer* container,
                       wxAuiDockInfo& dock,
                       wxAuiPaneInfo& pane,
                       wxAuiDockUIPartArray& uiparts,
                       bool spacer_only);

    void LayoutAddDock(wxSizer* container,
                       wxAuiDockInfo& dock,
                       wxAuiDockUIPartArray& uiparts,
                       bool spacer_only);

    wxSizer* LayoutAll(wxAuiPaneInfoArray& panes,
                       wxAuiDockInfoArray& docks,
                       wxAuiDockUIPartArray& uiparts,
                       bool spacer_only = false);

    virtual bool ProcessDockResult(wxAuiPaneInfo& target,
                                   const wxAuiPaneInfo& new_pos);

    bool DoDrop(wxAuiDockInfoArray& docks,
                wxAuiPaneInfoArray& panes,
                wxAuiPaneInfo& drop,
                const wxPoint& pt,
                const wxPoint& action_offset = wxPoint(0,0));

    wxAuiDockUIPart* HitTest(int x, int y);
    wxAuiDockUIPart* GetPanePart(wxWindow* pane);
    int GetDockPixelOffset(wxAuiPaneInfo& test);
    void OnFloatingPaneMoveStart(wxWindow* window);
    void OnFloatingPaneMoving(wxWindow* window, wxDirection dir );
    void OnFloatingPaneMoved(wxWindow* window, wxDirection dir);
    void OnFloatingPaneActivated(wxWindow* window);
    void OnFloatingPaneClosed(wxWindow* window, wxCloseEvent& evt);
    void OnFloatingPaneResized(wxWindow* window, const wxSize& size);
    void Render(wxDC* dc);
    void Repaint(wxDC* dc = NULL);
    void ProcessMgrEvent(wxAuiManagerEvent& event);
    void UpdateButtonOnScreen(wxAuiDockUIPart* button_ui_part,
                              const wxMouseEvent& event);
    void GetPanePositionsAndSizes(wxAuiDockInfo& dock,
                              wxArrayInt& positions,
                              wxArrayInt& sizes);

    /// Ends a resize action, or for live update, resizes the sash
    bool DoEndResizeAction(wxMouseEvent& event);

public:

    // public events (which can be invoked externally)
    void OnRender(wxAuiManagerEvent& evt);
    void OnPaneButton(wxAuiManagerEvent& evt);

protected:

    // protected events
    void OnPaint(wxPaintEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnSetCursor(wxSetCursorEvent& evt);
    void OnLeftDown(wxMouseEvent& evt);
    void OnLeftUp(wxMouseEvent& evt);
    void OnMotion(wxMouseEvent& evt);
    void OnCaptureLost(wxMouseCaptureLostEvent& evt);
    void OnLeaveWindow(wxMouseEvent& evt);
    void OnChildFocus(wxChildFocusEvent& evt);
    void OnHintFadeTimer(wxTimerEvent& evt);
    void OnFindManager(wxAuiManagerEvent& evt);

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
    wxAuiDockArt* m_art;            // dock art object which does all drawing
    unsigned int m_flags;        // manager flags wxAUI_MGR_*

    wxAuiPaneInfoArray m_panes;     // array of panes structures
    wxAuiDockInfoArray m_docks;     // array of docks structures
    wxAuiDockUIPartArray m_uiparts; // array of UI parts (captions, buttons, etc)

    int m_action;                // current mouse action
    wxPoint m_action_start;      // position where the action click started
    wxPoint m_action_offset;     // offset from upper left of the item clicked
    wxAuiDockUIPart* m_action_part; // ptr to the part the action happened to
    wxWindow* m_action_window;   // action frame or window (NULL if none)
    wxRect m_action_hintrect;    // hint rectangle for the action
    wxRect m_last_rect;
    wxAuiDockUIPart* m_hover_button;// button uipart being hovered over
    wxRect m_last_hint;          // last hint rectangle
    wxPoint m_last_mouse_move;   // last mouse move position (see OnMotion)
    int  m_currentDragItem;
    bool m_skipping;
    bool m_has_maximized;

    double m_dock_constraint_x;  // 0.0 .. 1.0; max pct of window width a dock can consume
    double m_dock_constraint_y;  // 0.0 .. 1.0; max pct of window height a dock can consume

    wxFrame* m_hint_wnd;         // transparent hint window, if supported by platform
    wxTimer m_hint_fadetimer;    // transparent fade timer
    wxByte m_hint_fadeamt;       // transparent fade amount
    wxByte m_hint_fademax;       // maximum value of hint fade

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
        button = c.button;
        veto_flag = c.veto_flag;
        canveto_flag = c.canveto_flag;
        dc = c.dc;
    }
#endif
    wxEvent *Clone() const { return new wxAuiManagerEvent(*this); }

    void SetManager(wxAuiManager* mgr) { manager = mgr; }
    void SetPane(wxAuiPaneInfo* p) { pane = p; }
    void SetButton(int b) { button = b; }
    void SetDC(wxDC* pdc) { dc = pdc; }

    wxAuiManager* GetManager() const { return manager; }
    wxAuiPaneInfo* GetPane() const { return pane; }
    int GetButton() const { return button; }
    wxDC* GetDC() const { return dc; }

    void Veto(bool veto = true) { veto_flag = veto; }
    bool GetVeto() const { return veto_flag; }
    void SetCanVeto(bool can_veto) { canveto_flag = can_veto; }
    bool CanVeto() const { return  canveto_flag && veto_flag; }

public:
    wxAuiManager* manager;
    wxAuiPaneInfo* pane;
    int button;
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
};


class WXDLLIMPEXP_AUI wxAuiDockUIPart
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
    wxAuiDockInfo* dock;        // which dock the item is associated with
    wxAuiPaneInfo* pane;        // which pane the item is associated with
    wxAuiPaneButton* button;    // which pane button the item is associated with
    wxSizer* cont_sizer;     // the part's containing sizer
    wxSizerItem* sizer_item; // the sizer item of the part
    wxRect rect;             // client coord rectangle of the part itself
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
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_AUI, wxEVT_AUI_RENDER, wxAuiManagerEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_AUI, wxEVT_AUI_FIND_MANAGER, wxAuiManagerEvent );

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
#define EVT_AUI_RENDER(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_RENDER, wxAuiManagerEventHandler(func))
#define EVT_AUI_FIND_MANAGER(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_FIND_MANAGER, wxAuiManagerEventHandler(func))

#else

%constant wxEventType wxEVT_AUI_PANE_BUTTON;
%constant wxEventType wxEVT_AUI_PANE_CLOSE;
%constant wxEventType wxEVT_AUI_PANE_MAXIMIZE;
%constant wxEventType wxEVT_AUI_PANE_RESTORE;
%constant wxEventType wxEVT_AUI_RENDER;
%constant wxEventType wxEVT_AUI_FIND_MANAGER;

%pythoncode {
    EVT_AUI_PANE_BUTTON = wx.PyEventBinder( wxEVT_AUI_PANE_BUTTON )
    EVT_AUI_PANE_CLOSE = wx.PyEventBinder( wxEVT_AUI_PANE_CLOSE )
    EVT_AUI_PANE_MAXIMIZE = wx.PyEventBinder( wxEVT_AUI_PANE_MAXIMIZE )
    EVT_AUI_PANE_RESTORE = wx.PyEventBinder( wxEVT_AUI_PANE_RESTORE )
    EVT_AUI_RENDER = wx.PyEventBinder( wxEVT_AUI_RENDER )
    EVT_AUI_FIND_MANAGER = wx.PyEventBinder( wxEVT_AUI_FIND_MANAGER )
}
#endif // SWIG

#endif // wxUSE_AUI
#endif //_WX_FRAMEMANAGER_H_

