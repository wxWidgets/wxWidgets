/////////////////////////////////////////////////////////////////////////////
// Name:        _event.i
// Purpose:     SWIG interface for common event classes and event binders
//
// Author:      Robin Dunn
//
// Created:     24-May-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
// Include some extra python code here
%pythoncode "_event_ex.py"

//---------------------------------------------------------------------------
%newgroup;

// the predefined constants for the number of times we propagate event
// upwards window child-parent chain
enum Propagation_state
{
    // don't propagate it at all
    wxEVENT_PROPAGATE_NONE = 0,

    // propagate it until it is processed
    wxEVENT_PROPAGATE_MAX = INT_MAX
};


//---------------------------------------------------------------------------
// Event types

wxEventType wxNewEventType();


%constant wxEventType wxEVT_NULL;
%constant wxEventType wxEVT_FIRST;
%constant wxEventType wxEVT_USER_FIRST;

%constant wxEventType wxEVT_COMMAND_BUTTON_CLICKED;
%constant wxEventType wxEVT_COMMAND_CHECKBOX_CLICKED;
%constant wxEventType wxEVT_COMMAND_CHOICE_SELECTED;
%constant wxEventType wxEVT_COMMAND_LISTBOX_SELECTED;
%constant wxEventType wxEVT_COMMAND_LISTBOX_DOUBLECLICKED;
%constant wxEventType wxEVT_COMMAND_CHECKLISTBOX_TOGGLED;

%constant wxEventType wxEVT_COMMAND_MENU_SELECTED;
%constant wxEventType wxEVT_COMMAND_TOOL_CLICKED;
%constant wxEventType wxEVT_COMMAND_SLIDER_UPDATED;
%constant wxEventType wxEVT_COMMAND_RADIOBOX_SELECTED;
%constant wxEventType wxEVT_COMMAND_RADIOBUTTON_SELECTED;

%constant wxEventType wxEVT_COMMAND_SCROLLBAR_UPDATED;
%constant wxEventType wxEVT_COMMAND_VLBOX_SELECTED;
%constant wxEventType wxEVT_COMMAND_COMBOBOX_SELECTED;
%constant wxEventType wxEVT_COMMAND_TOOL_RCLICKED;
%constant wxEventType wxEVT_COMMAND_TOOL_ENTER;

// Mouse event types
%constant wxEventType wxEVT_LEFT_DOWN;
%constant wxEventType wxEVT_LEFT_UP;
%constant wxEventType wxEVT_MIDDLE_DOWN;
%constant wxEventType wxEVT_MIDDLE_UP;
%constant wxEventType wxEVT_RIGHT_DOWN;
%constant wxEventType wxEVT_RIGHT_UP;
%constant wxEventType wxEVT_MOTION;
%constant wxEventType wxEVT_ENTER_WINDOW;
%constant wxEventType wxEVT_LEAVE_WINDOW;
%constant wxEventType wxEVT_LEFT_DCLICK;
%constant wxEventType wxEVT_MIDDLE_DCLICK;
%constant wxEventType wxEVT_RIGHT_DCLICK;
%constant wxEventType wxEVT_SET_FOCUS;
%constant wxEventType wxEVT_KILL_FOCUS;
%constant wxEventType wxEVT_CHILD_FOCUS;
%constant wxEventType wxEVT_MOUSEWHEEL;

// Non-client mouse events
%constant wxEventType wxEVT_NC_LEFT_DOWN;
%constant wxEventType wxEVT_NC_LEFT_UP;
%constant wxEventType wxEVT_NC_MIDDLE_DOWN;
%constant wxEventType wxEVT_NC_MIDDLE_UP;
%constant wxEventType wxEVT_NC_RIGHT_DOWN;
%constant wxEventType wxEVT_NC_RIGHT_UP;
%constant wxEventType wxEVT_NC_MOTION;
%constant wxEventType wxEVT_NC_ENTER_WINDOW;
%constant wxEventType wxEVT_NC_LEAVE_WINDOW;
%constant wxEventType wxEVT_NC_LEFT_DCLICK;
%constant wxEventType wxEVT_NC_MIDDLE_DCLICK;
%constant wxEventType wxEVT_NC_RIGHT_DCLICK;

// Character input event type
%constant wxEventType wxEVT_CHAR;
%constant wxEventType wxEVT_CHAR_HOOK;
%constant wxEventType wxEVT_NAVIGATION_KEY;
%constant wxEventType wxEVT_KEY_DOWN;
%constant wxEventType wxEVT_KEY_UP;
%constant wxEventType wxEVT_HOTKEY;
    
// Set cursor event
%constant wxEventType wxEVT_SET_CURSOR;

// wxScrollBar and wxSlider event identifiers
%constant wxEventType wxEVT_SCROLL_TOP;
%constant wxEventType wxEVT_SCROLL_BOTTOM;
%constant wxEventType wxEVT_SCROLL_LINEUP;
%constant wxEventType wxEVT_SCROLL_LINEDOWN;
%constant wxEventType wxEVT_SCROLL_PAGEUP;
%constant wxEventType wxEVT_SCROLL_PAGEDOWN;
%constant wxEventType wxEVT_SCROLL_THUMBTRACK;
%constant wxEventType wxEVT_SCROLL_THUMBRELEASE;
%constant wxEventType wxEVT_SCROLL_ENDSCROLL;

// Scroll events from wxWindow
%constant wxEventType wxEVT_SCROLLWIN_TOP;
%constant wxEventType wxEVT_SCROLLWIN_BOTTOM;
%constant wxEventType wxEVT_SCROLLWIN_LINEUP;
%constant wxEventType wxEVT_SCROLLWIN_LINEDOWN;
%constant wxEventType wxEVT_SCROLLWIN_PAGEUP;
%constant wxEventType wxEVT_SCROLLWIN_PAGEDOWN;
%constant wxEventType wxEVT_SCROLLWIN_THUMBTRACK;
%constant wxEventType wxEVT_SCROLLWIN_THUMBRELEASE;

// System events
%constant wxEventType wxEVT_SIZE;
%constant wxEventType wxEVT_MOVE;
%constant wxEventType wxEVT_CLOSE_WINDOW;
%constant wxEventType wxEVT_END_SESSION;
%constant wxEventType wxEVT_QUERY_END_SESSION;
%constant wxEventType wxEVT_ACTIVATE_APP;
%constant wxEventType wxEVT_POWER;
%constant wxEventType wxEVT_ACTIVATE;
%constant wxEventType wxEVT_CREATE;
%constant wxEventType wxEVT_DESTROY;
%constant wxEventType wxEVT_SHOW;
%constant wxEventType wxEVT_ICONIZE;
%constant wxEventType wxEVT_MAXIMIZE;
%constant wxEventType wxEVT_MOUSE_CAPTURE_CHANGED;
%constant wxEventType wxEVT_PAINT;
%constant wxEventType wxEVT_ERASE_BACKGROUND;
%constant wxEventType wxEVT_NC_PAINT;
%constant wxEventType wxEVT_PAINT_ICON;
%constant wxEventType wxEVT_MENU_OPEN;
%constant wxEventType wxEVT_MENU_CLOSE;
%constant wxEventType wxEVT_MENU_HIGHLIGHT;

%constant wxEventType wxEVT_CONTEXT_MENU;
%constant wxEventType wxEVT_SYS_COLOUR_CHANGED;
%constant wxEventType wxEVT_DISPLAY_CHANGED;
%constant wxEventType wxEVT_SETTING_CHANGED;
%constant wxEventType wxEVT_QUERY_NEW_PALETTE;
%constant wxEventType wxEVT_PALETTE_CHANGED;
%constant wxEventType wxEVT_DROP_FILES;
%constant wxEventType wxEVT_DRAW_ITEM;
%constant wxEventType wxEVT_MEASURE_ITEM;
%constant wxEventType wxEVT_COMPARE_ITEM;
%constant wxEventType wxEVT_INIT_DIALOG;
%constant wxEventType wxEVT_IDLE;
%constant wxEventType wxEVT_UPDATE_UI;
%constant wxEventType wxEVT_SIZING;
%constant wxEventType wxEVT_MOVING;

// Generic command events
// Note: a click is a higher-level event than button down/up
%constant wxEventType wxEVT_COMMAND_LEFT_CLICK;
%constant wxEventType wxEVT_COMMAND_LEFT_DCLICK;
%constant wxEventType wxEVT_COMMAND_RIGHT_CLICK;
%constant wxEventType wxEVT_COMMAND_RIGHT_DCLICK;
%constant wxEventType wxEVT_COMMAND_SET_FOCUS;
%constant wxEventType wxEVT_COMMAND_KILL_FOCUS;
%constant wxEventType wxEVT_COMMAND_ENTER;



%pythoncode {
%#
%# Create some event binders
EVT_SIZE = wx.PyEventBinder( wxEVT_SIZE )
EVT_SIZING = wx.PyEventBinder( wxEVT_SIZING )
EVT_MOVE = wx.PyEventBinder( wxEVT_MOVE )
EVT_MOVING = wx.PyEventBinder( wxEVT_MOVING )
EVT_CLOSE = wx.PyEventBinder( wxEVT_CLOSE_WINDOW )
EVT_END_SESSION = wx.PyEventBinder( wxEVT_END_SESSION )
EVT_QUERY_END_SESSION = wx.PyEventBinder( wxEVT_QUERY_END_SESSION )
EVT_PAINT = wx.PyEventBinder( wxEVT_PAINT )
EVT_NC_PAINT = wx.PyEventBinder( wxEVT_NC_PAINT )
EVT_ERASE_BACKGROUND = wx.PyEventBinder( wxEVT_ERASE_BACKGROUND )
EVT_CHAR = wx.PyEventBinder( wxEVT_CHAR )
EVT_KEY_DOWN = wx.PyEventBinder( wxEVT_KEY_DOWN )
EVT_KEY_UP = wx.PyEventBinder( wxEVT_KEY_UP )
EVT_HOTKEY = wx.PyEventBinder( wxEVT_HOTKEY, 1) 
EVT_CHAR_HOOK = wx.PyEventBinder( wxEVT_CHAR_HOOK )
EVT_MENU_OPEN = wx.PyEventBinder( wxEVT_MENU_OPEN )
EVT_MENU_CLOSE = wx.PyEventBinder( wxEVT_MENU_CLOSE )
EVT_MENU_HIGHLIGHT = wx.PyEventBinder( wxEVT_MENU_HIGHLIGHT, 1)
EVT_MENU_HIGHLIGHT_ALL = wx.PyEventBinder( wxEVT_MENU_HIGHLIGHT )
EVT_SET_FOCUS = wx.PyEventBinder( wxEVT_SET_FOCUS )
EVT_KILL_FOCUS = wx.PyEventBinder( wxEVT_KILL_FOCUS )
EVT_CHILD_FOCUS = wx.PyEventBinder( wxEVT_CHILD_FOCUS )
EVT_ACTIVATE = wx.PyEventBinder( wxEVT_ACTIVATE )
EVT_ACTIVATE_APP = wx.PyEventBinder( wxEVT_ACTIVATE_APP )
EVT_END_SESSION = wx.PyEventBinder( wxEVT_END_SESSION )
EVT_QUERY_END_SESSION = wx.PyEventBinder( wxEVT_QUERY_END_SESSION )
EVT_DROP_FILES = wx.PyEventBinder( wxEVT_DROP_FILES )
EVT_INIT_DIALOG = wx.PyEventBinder( wxEVT_INIT_DIALOG )
EVT_SYS_COLOUR_CHANGED = wx.PyEventBinder( wxEVT_SYS_COLOUR_CHANGED )
EVT_DISPLAY_CHANGED = wx.PyEventBinder( wxEVT_DISPLAY_CHANGED )
EVT_SHOW = wx.PyEventBinder( wxEVT_SHOW )
EVT_MAXIMIZE = wx.PyEventBinder( wxEVT_MAXIMIZE )
EVT_ICONIZE = wx.PyEventBinder( wxEVT_ICONIZE )
EVT_NAVIGATION_KEY = wx.PyEventBinder( wxEVT_NAVIGATION_KEY )
EVT_PALETTE_CHANGED = wx.PyEventBinder( wxEVT_PALETTE_CHANGED )
EVT_QUERY_NEW_PALETTE = wx.PyEventBinder( wxEVT_QUERY_NEW_PALETTE )
EVT_WINDOW_CREATE = wx.PyEventBinder( wxEVT_CREATE )
EVT_WINDOW_DESTROY = wx.PyEventBinder( wxEVT_DESTROY )
EVT_SET_CURSOR = wx.PyEventBinder( wxEVT_SET_CURSOR )
EVT_MOUSE_CAPTURE_CHANGED = wx.PyEventBinder( wxEVT_MOUSE_CAPTURE_CHANGED )

EVT_LEFT_DOWN = wx.PyEventBinder( wxEVT_LEFT_DOWN )
EVT_LEFT_UP = wx.PyEventBinder( wxEVT_LEFT_UP )
EVT_MIDDLE_DOWN = wx.PyEventBinder( wxEVT_MIDDLE_DOWN )
EVT_MIDDLE_UP = wx.PyEventBinder( wxEVT_MIDDLE_UP )
EVT_RIGHT_DOWN = wx.PyEventBinder( wxEVT_RIGHT_DOWN )
EVT_RIGHT_UP = wx.PyEventBinder( wxEVT_RIGHT_UP )
EVT_MOTION = wx.PyEventBinder( wxEVT_MOTION )
EVT_LEFT_DCLICK = wx.PyEventBinder( wxEVT_LEFT_DCLICK )
EVT_MIDDLE_DCLICK = wx.PyEventBinder( wxEVT_MIDDLE_DCLICK )
EVT_RIGHT_DCLICK = wx.PyEventBinder( wxEVT_RIGHT_DCLICK )
EVT_LEAVE_WINDOW = wx.PyEventBinder( wxEVT_LEAVE_WINDOW )
EVT_ENTER_WINDOW = wx.PyEventBinder( wxEVT_ENTER_WINDOW )
EVT_MOUSEWHEEL = wx.PyEventBinder( wxEVT_MOUSEWHEEL )

EVT_MOUSE_EVENTS = wx.PyEventBinder([ wxEVT_LEFT_DOWN,
                                     wxEVT_LEFT_UP,
                                     wxEVT_MIDDLE_DOWN,
                                     wxEVT_MIDDLE_UP,
                                     wxEVT_RIGHT_DOWN,
                                     wxEVT_RIGHT_UP,
                                     wxEVT_MOTION,
                                     wxEVT_LEFT_DCLICK,
                                     wxEVT_MIDDLE_DCLICK,
                                     wxEVT_RIGHT_DCLICK,
                                     wxEVT_ENTER_WINDOW,
                                     wxEVT_LEAVE_WINDOW,
                                     wxEVT_MOUSEWHEEL
                                     ])


%# Scrolling from wxWindow (sent to wxScrolledWindow)
EVT_SCROLLWIN = wx.PyEventBinder([ wxEVT_SCROLLWIN_TOP, 
                                  wxEVT_SCROLLWIN_BOTTOM,
                                  wxEVT_SCROLLWIN_LINEUP,
                                  wxEVT_SCROLLWIN_LINEDOWN,
                                  wxEVT_SCROLLWIN_PAGEUP, 
                                  wxEVT_SCROLLWIN_PAGEDOWN,
                                  wxEVT_SCROLLWIN_THUMBTRACK,
                                  wxEVT_SCROLLWIN_THUMBRELEASE,
                                  ])

EVT_SCROLLWIN_TOP = wx.PyEventBinder( wxEVT_SCROLLWIN_TOP )
EVT_SCROLLWIN_BOTTOM = wx.PyEventBinder( wxEVT_SCROLLWIN_BOTTOM )
EVT_SCROLLWIN_LINEUP = wx.PyEventBinder( wxEVT_SCROLLWIN_LINEUP )
EVT_SCROLLWIN_LINEDOWN = wx.PyEventBinder( wxEVT_SCROLLWIN_LINEDOWN )
EVT_SCROLLWIN_PAGEUP = wx.PyEventBinder( wxEVT_SCROLLWIN_PAGEUP )
EVT_SCROLLWIN_PAGEDOWN = wx.PyEventBinder( wxEVT_SCROLLWIN_PAGEDOWN )
EVT_SCROLLWIN_THUMBTRACK = wx.PyEventBinder( wxEVT_SCROLLWIN_THUMBTRACK )
EVT_SCROLLWIN_THUMBRELEASE = wx.PyEventBinder( wxEVT_SCROLLWIN_THUMBRELEASE )

%# Scrolling from wxSlider and wxScrollBar
EVT_SCROLL = wx.PyEventBinder([ wxEVT_SCROLL_TOP, 
                               wxEVT_SCROLL_BOTTOM, 
                               wxEVT_SCROLL_LINEUP, 
                               wxEVT_SCROLL_LINEDOWN, 
                               wxEVT_SCROLL_PAGEUP, 
                               wxEVT_SCROLL_PAGEDOWN, 
                               wxEVT_SCROLL_THUMBTRACK, 
                               wxEVT_SCROLL_THUMBRELEASE, 
                               wxEVT_SCROLL_ENDSCROLL,
                               ])

EVT_SCROLL_TOP = wx.PyEventBinder( wxEVT_SCROLL_TOP )
EVT_SCROLL_BOTTOM = wx.PyEventBinder( wxEVT_SCROLL_BOTTOM )
EVT_SCROLL_LINEUP = wx.PyEventBinder( wxEVT_SCROLL_LINEUP )
EVT_SCROLL_LINEDOWN = wx.PyEventBinder( wxEVT_SCROLL_LINEDOWN )
EVT_SCROLL_PAGEUP = wx.PyEventBinder( wxEVT_SCROLL_PAGEUP )
EVT_SCROLL_PAGEDOWN = wx.PyEventBinder( wxEVT_SCROLL_PAGEDOWN )
EVT_SCROLL_THUMBTRACK = wx.PyEventBinder( wxEVT_SCROLL_THUMBTRACK )
EVT_SCROLL_THUMBRELEASE = wx.PyEventBinder( wxEVT_SCROLL_THUMBRELEASE )
EVT_SCROLL_ENDSCROLL = wx.PyEventBinder( wxEVT_SCROLL_ENDSCROLL )

%# Scrolling from wxSlider and wxScrollBar, with an id
EVT_COMMAND_SCROLL = wx.PyEventBinder([ wxEVT_SCROLL_TOP, 
                                       wxEVT_SCROLL_BOTTOM, 
                                       wxEVT_SCROLL_LINEUP, 
                                       wxEVT_SCROLL_LINEDOWN, 
                                       wxEVT_SCROLL_PAGEUP, 
                                       wxEVT_SCROLL_PAGEDOWN, 
                                       wxEVT_SCROLL_THUMBTRACK, 
                                       wxEVT_SCROLL_THUMBRELEASE,
                                       wxEVT_SCROLL_ENDSCROLL,
                                       ], 1)

EVT_COMMAND_SCROLL_TOP = wx.PyEventBinder( wxEVT_SCROLL_TOP, 1)
EVT_COMMAND_SCROLL_BOTTOM = wx.PyEventBinder( wxEVT_SCROLL_BOTTOM, 1)
EVT_COMMAND_SCROLL_LINEUP = wx.PyEventBinder( wxEVT_SCROLL_LINEUP, 1)
EVT_COMMAND_SCROLL_LINEDOWN = wx.PyEventBinder( wxEVT_SCROLL_LINEDOWN, 1)
EVT_COMMAND_SCROLL_PAGEUP = wx.PyEventBinder( wxEVT_SCROLL_PAGEUP, 1)
EVT_COMMAND_SCROLL_PAGEDOWN = wx.PyEventBinder( wxEVT_SCROLL_PAGEDOWN, 1)
EVT_COMMAND_SCROLL_THUMBTRACK = wx.PyEventBinder( wxEVT_SCROLL_THUMBTRACK, 1)
EVT_COMMAND_SCROLL_THUMBRELEASE = wx.PyEventBinder( wxEVT_SCROLL_THUMBRELEASE, 1)
EVT_COMMAND_SCROLL_ENDSCROLL = wx.PyEventBinder( wxEVT_SCROLL_ENDSCROLL, 1)

EVT_BUTTON = wx.PyEventBinder( wxEVT_COMMAND_BUTTON_CLICKED, 1)
EVT_CHECKBOX = wx.PyEventBinder( wxEVT_COMMAND_CHECKBOX_CLICKED, 1)
EVT_CHOICE = wx.PyEventBinder( wxEVT_COMMAND_CHOICE_SELECTED, 1)
EVT_LISTBOX = wx.PyEventBinder( wxEVT_COMMAND_LISTBOX_SELECTED, 1)
EVT_LISTBOX_DCLICK = wx.PyEventBinder( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, 1)
EVT_MENU = wx.PyEventBinder( wxEVT_COMMAND_MENU_SELECTED, 1)
EVT_MENU_RANGE = wx.PyEventBinder( wxEVT_COMMAND_MENU_SELECTED, 2)
EVT_SLIDER = wx.PyEventBinder( wxEVT_COMMAND_SLIDER_UPDATED, 1)
EVT_RADIOBOX = wx.PyEventBinder( wxEVT_COMMAND_RADIOBOX_SELECTED, 1)
EVT_RADIOBUTTON = wx.PyEventBinder( wxEVT_COMMAND_RADIOBUTTON_SELECTED, 1)

EVT_SCROLLBAR = wx.PyEventBinder( wxEVT_COMMAND_SCROLLBAR_UPDATED, 1)
EVT_VLBOX = wx.PyEventBinder( wxEVT_COMMAND_VLBOX_SELECTED, 1)
EVT_COMBOBOX = wx.PyEventBinder( wxEVT_COMMAND_COMBOBOX_SELECTED, 1)
EVT_TOOL = wx.PyEventBinder( wxEVT_COMMAND_TOOL_CLICKED, 1)
EVT_TOOL_RANGE = wx.PyEventBinder( wxEVT_COMMAND_TOOL_CLICKED, 2)
EVT_TOOL_RCLICKED = wx.PyEventBinder( wxEVT_COMMAND_TOOL_RCLICKED, 1)
EVT_TOOL_RCLICKED_RANGE = wx.PyEventBinder( wxEVT_COMMAND_TOOL_RCLICKED, 2)
EVT_TOOL_ENTER = wx.PyEventBinder( wxEVT_COMMAND_TOOL_ENTER, 1)
EVT_CHECKLISTBOX = wx.PyEventBinder( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, 1)


EVT_COMMAND_LEFT_CLICK = wx.PyEventBinder( wxEVT_COMMAND_LEFT_CLICK, 1)
EVT_COMMAND_LEFT_DCLICK = wx.PyEventBinder( wxEVT_COMMAND_LEFT_DCLICK, 1)
EVT_COMMAND_RIGHT_CLICK = wx.PyEventBinder( wxEVT_COMMAND_RIGHT_CLICK, 1)
EVT_COMMAND_RIGHT_DCLICK = wx.PyEventBinder( wxEVT_COMMAND_RIGHT_DCLICK, 1)
EVT_COMMAND_SET_FOCUS = wx.PyEventBinder( wxEVT_COMMAND_SET_FOCUS, 1)
EVT_COMMAND_KILL_FOCUS = wx.PyEventBinder( wxEVT_COMMAND_KILL_FOCUS, 1)
EVT_COMMAND_ENTER = wx.PyEventBinder( wxEVT_COMMAND_ENTER, 1)

EVT_IDLE = wx.PyEventBinder( wxEVT_IDLE )

EVT_UPDATE_UI = wx.PyEventBinder( wxEVT_UPDATE_UI, 1)
EVT_UPDATE_UI_RANGE = wx.PyEventBinder( wxEVT_UPDATE_UI, 2)

EVT_CONTEXT_MENU = wx.PyEventBinder( wxEVT_CONTEXT_MENU )


}

//---------------------------------------------------------------------------
%newgroup;

class wxEvent : public wxObject {
public:
    // wxEvent(int winid = 0, wxEventType commandType = wxEVT_NULL);     // *** This class is now an ABC
    ~wxEvent();

    void SetEventType(wxEventType typ);
    wxEventType GetEventType() const;
    wxObject *GetEventObject() const;
    void SetEventObject(wxObject *obj);
    long GetTimestamp() const;
    void SetTimestamp(long ts = 0);
    int  GetId() const;
    void SetId(int Id);


    bool IsCommandEvent() const;

    DocDeclStr(
        void , Skip(bool skip = true),
        "Called by an event handler, it controls whether additional event
handlers bound to this event will be called after the current event
handler returns.  Skip(false) (the default setting) will prevent
additional event handlers from being called and control will be
returned to the sender of the event immediately after the current
handler has finished.  Skip(True) will cause the event processing
system to continue searching for a handler function for this event.
", "");
    
    bool GetSkipped() const;

    // Determine if this event should be propagating to the parent window.
    bool ShouldPropagate() const;

    // Stop an event from propagating to its parent window, returns the old
    // propagation level value
    int StopPropagation();

    // Resume the event propagation by restoring the propagation level
    // (returned by StopPropagation())
    void ResumePropagation(int propagationLevel);

    // this function is used to create a copy of the event polymorphically and
    // all derived classes must implement it because otherwise wxPostEvent()
    // for them wouldn't work (it needs to do a copy of the event)
    virtual wxEvent *Clone() /* =0*/;
};


//---------------------------------------------------------------------------
%newgroup;

// Helper class to temporarily change an event not to propagate.
class  wxPropagationDisabler
{
public:
    wxPropagationDisabler(wxEvent& event);
    ~wxPropagationDisabler();
};


// Another one to temporarily lower propagation level.
class  wxPropagateOnce
{
public:
    wxPropagateOnce(wxEvent& event);
    ~wxPropagateOnce();
};

//---------------------------------------------------------------------------
%newgroup;

class wxCommandEvent : public wxEvent
{
public:
    wxCommandEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);


//     // Set/Get client data from controls
//     void SetClientData(void* clientData) { m_clientData = clientData; }
//     void *GetClientData() const { return m_clientData; }

//     // Set/Get client object from controls
//     void SetClientObject(wxClientData* clientObject) { m_clientObject = clientObject; }
//     void *GetClientObject() const { return m_clientObject; }

    
    // Get listbox selection if single-choice
    int GetSelection() const;

    // Set/Get listbox/choice selection string
    void SetString(const wxString& s);
    wxString GetString() const;

    // Get checkbox value
    bool IsChecked() const;
    %pythoncode { Checked = IsChecked }
    
    // True if the listbox event was a selection.
    bool IsSelection() const;

    void SetExtraLong(long extraLong);
    long GetExtraLong() const;

    void SetInt(int i);
    long GetInt() const;

    virtual wxEvent *Clone() const;

};

//---------------------------------------------------------------------------
%newgroup;

// this class adds a possibility to react (from the user) code to a control
// notification: allow or veto the operation being reported.
class wxNotifyEvent : public wxCommandEvent
{
public:
    wxNotifyEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);

    // veto the operation (usually it's allowed by default)
    void Veto();

    // allow the operation if it was disabled by default
    void Allow();

    // for implementation code only: is the operation allowed?
    bool IsAllowed();
};


//---------------------------------------------------------------------------
%newgroup;

// Scroll event class, derived form wxCommandEvent. wxScrollEvents are
// sent by wxSlider and wxScrollBar.
class  wxScrollEvent : public wxCommandEvent
{
public:
    wxScrollEvent(wxEventType commandType = wxEVT_NULL,
                  int winid = 0, int pos = 0, int orient = 0);

    int GetOrientation() const;
    int GetPosition() const;
    void SetOrientation(int orient);
    void SetPosition(int pos);
};


//---------------------------------------------------------------------------
%newgroup;

// ScrollWin event class, derived fom wxEvent. wxScrollWinEvents
// are sent by wxWindow.
class wxScrollWinEvent : public wxEvent
{
public:
    wxScrollWinEvent(wxEventType commandType = wxEVT_NULL,
                     int pos = 0, int orient = 0);

    int GetOrientation() const;
    int GetPosition() const;
    void SetOrientation(int orient);
    void SetPosition(int pos);
};

//---------------------------------------------------------------------------
%newgroup;

// the symbolic names for the mouse buttons
enum
{
    wxMOUSE_BTN_ANY     = -1,
    wxMOUSE_BTN_NONE    = -1,
    wxMOUSE_BTN_LEFT    = 0,
    wxMOUSE_BTN_MIDDLE  = 1,
    wxMOUSE_BTN_RIGHT   = 2
};


// Mouse event class
class  wxMouseEvent : public wxEvent
{
public:
    wxMouseEvent(wxEventType mouseType = wxEVT_NULL);

    // Was it a button event? (*doesn't* mean: is any button *down*?)
    bool IsButton() const;

    // Was it a down event from this (or any) button?
    bool ButtonDown(int but = wxMOUSE_BTN_ANY) const;

    // Was it a dclick event from this (or any) button?
    bool ButtonDClick(int but = wxMOUSE_BTN_ANY) const;

    // Was it a up event from this (or any) button?
    bool ButtonUp(int but = wxMOUSE_BTN_ANY) const;

    // Was the given button changing state?
    bool Button(int but) const;

    // Was the given button in Down state?
    bool ButtonIsDown(int but) const;

    // Get the button which is changing state (wxMOUSE_BTN_NONE if none)
    int GetButton() const;

    // Find state of shift/control keys
    bool ControlDown() const;
    bool MetaDown() const;
    bool AltDown() const;
    bool ShiftDown() const;

    DocDeclStr(
        bool , CmdDown() const,
        "\"Cmd\" is a pseudo key which is the same as Control for PC and Unix
platforms but the special \"Apple\" (a.k.a as \"Command\") key on
Macs: it makes often sense to use it instead of, say, `ControlDown`
because Cmd key is used for the same thing under Mac as Ctrl
elsewhere. The Ctrl still exists, it's just not used for this
purpose. So for non-Mac platforms this is the same as `ControlDown`
and Macs this is the same as `MetaDown`.", "");
    
    // Find which event was just generated
    bool LeftDown() const;
    bool MiddleDown() const;
    bool RightDown() const;

    bool LeftUp() const;
    bool MiddleUp() const;
    bool RightUp() const;

    bool LeftDClick() const;
    bool MiddleDClick() const;
    bool RightDClick() const;

    // Find the current state of the mouse buttons (regardless
    // of current event type)
    bool LeftIsDown();
    bool MiddleIsDown();
    bool RightIsDown();

    // True if a button is down and the mouse is moving
    bool Dragging() const;

    // True if the mouse is moving, and no button is down
    bool Moving() const;

    // True if the mouse is just entering the window
    bool Entering() const;

    // True if the mouse is just leaving the window
    bool Leaving() const;


    DocStr(GetPosition,   // sets the docstring for both
           "Returns the position of the mouse in window coordinates when the event
happened.", "");
    wxPoint GetPosition();

    DocDeclAName(
        void, GetPosition(long *OUTPUT, long *OUTPUT),
        "GetPositionTuple() -> (x,y)",
        GetPositionTuple);
    
    // Find the logical position of the event given the DC
    wxPoint GetLogicalPosition(const wxDC& dc) const;

    // Get X position
    wxCoord GetX() const;

    // Get Y position
    wxCoord GetY() const;

    // Get wheel rotation, positive or negative indicates direction of
    // rotation.  Current devices all send an event when rotation is equal to
    // +/-WheelDelta, but this allows for finer resolution devices to be
    // created in the future.  Because of this you shouldn't assume that one
    // event is equal to 1 line or whatever, but you should be able to either
    // do partial line scrolling or wait until +/-WheelDelta rotation values
    // have been accumulated before scrolling.
    int GetWheelRotation() const;

    // Get wheel delta, normally 120.  This is the threshold for action to be
    // taken, and one such action (for example, scrolling one increment)
    // should occur for each delta.
    int GetWheelDelta() const;

    // Returns the configured number of lines (or whatever) to be scrolled per
    // wheel action.  Defaults to one.
    int GetLinesPerAction() const;

    // Is the system set to do page scrolling?
    bool IsPageScroll() const;

public:
    wxCoord m_x, m_y;

    bool          m_leftDown;
    bool          m_middleDown;
    bool          m_rightDown;

    bool          m_controlDown;
    bool          m_shiftDown;
    bool          m_altDown;
    bool          m_metaDown;

    int           m_wheelRotation;
    int           m_wheelDelta;
    int           m_linesPerAction;
};

//---------------------------------------------------------------------------
%newgroup;

// Cursor set event
class wxSetCursorEvent : public wxEvent
{
public:
    wxSetCursorEvent(wxCoord x = 0, wxCoord y = 0);

    wxCoord GetX() const;
    wxCoord GetY() const;

    void SetCursor(const wxCursor& cursor);
    const wxCursor& GetCursor() const;
    bool HasCursor() const;
};

//---------------------------------------------------------------------------
%newgroup;

// Keyboard input event class

class wxKeyEvent : public wxEvent
{
public:
    wxKeyEvent(wxEventType keyType = wxEVT_NULL);

    // Find state of shift/control keys
    bool ControlDown() const;
    bool MetaDown() const;
    bool AltDown() const;
    bool ShiftDown() const;
    
    DocDeclStr(
        bool , CmdDown() const,
        "\"Cmd\" is a pseudo key which is the same as Control for PC and Unix
platforms but the special \"Apple\" (a.k.a as \"Command\") key on
Macs: it makes often sense to use it instead of, say, `ControlDown`
because Cmd key is used for the same thing under Mac as Ctrl
elsewhere. The Ctrl still exists, it's just not used for this
purpose. So for non-Mac platforms this is the same as `ControlDown`
and Macs this is the same as `MetaDown`.", "");
   
    

    // exclude MetaDown() from HasModifiers() because NumLock under X is often
    // configured as mod2 modifier, yet the key events even when it is pressed
    // should be processed normally, not like Ctrl- or Alt-key
    bool HasModifiers() const;

    // get the key code: an ASCII7 char or an element of wxKeyCode enum
    int GetKeyCode() const;
    %pythoncode { KeyCode = GetKeyCode }

    %extend {
        int GetUnicodeKey() {
        %#if wxUSE_UNICODE
            return self->GetUnicodeKey();
        %#else
            return 0;
        %#endif
        }
    }
    %pythoncode { GetUniChar = GetUnicodeKey }
    
    
    // get the raw key code (platform-dependent)
    wxUint32 GetRawKeyCode() const;

    // get the raw key flags (platform-dependent)
    wxUint32 GetRawKeyFlags() const;

    
    DocStr(GetPosition,   // sets the docstring for both
           "Find the position of the event.", "");
    wxPoint GetPosition();

    DocDeclAName(
        void, GetPosition(long *OUTPUT, long *OUTPUT),
        "GetPositionTuple() -> (x,y)",
        GetPositionTuple);

    // Get X position
    wxCoord GetX() const;

    // Get Y position
    wxCoord GetY() const;

public:
    wxCoord       m_x, m_y;

    long          m_keyCode;

    bool          m_controlDown;
    bool          m_shiftDown;
    bool          m_altDown;
    bool          m_metaDown;
    bool          m_scanCode;

    // these fields contain the platform-specific information about
    // key that was pressed
    wxUint32      m_rawCode;
    wxUint32      m_rawFlags;
};

//---------------------------------------------------------------------------
%newgroup;

// Size event class
class wxSizeEvent : public wxEvent
{
public:
    wxSizeEvent(const wxSize& sz=wxDefaultSize, int winid = 0);

    wxSize GetSize() const;
    wxRect GetRect() const;
    void SetRect(wxRect rect);
    %extend {
        void SetSize(wxSize size) {
            self->m_size = size;
        }
    }

public:
    wxSize m_size;
    wxRect m_rect; // Used for wxEVT_SIZING
};


//---------------------------------------------------------------------------
%newgroup;

// Move event class
class wxMoveEvent : public wxEvent
{
public:
    wxMoveEvent(const wxPoint& pos=wxDefaultPosition, int winid = 0);

    wxPoint GetPosition() const;
    wxRect GetRect() const;
    void SetRect(const wxRect& rect);
    void SetPosition(const wxPoint& pos);

    %pythoncode {
        m_pos =  property(GetPosition, SetPosition)
        m_rect = property(GetRect, SetRect)
    }
};

//---------------------------------------------------------------------------
%newgroup;

// Paint event class
class wxPaintEvent : public wxEvent
{
public:
    wxPaintEvent(int Id = 0);
};


class wxNcPaintEvent : public wxEvent
{
public:
    wxNcPaintEvent(int winid = 0);
};

//---------------------------------------------------------------------------
%newgroup;

class wxEraseEvent : public wxEvent
{
public:
    wxEraseEvent(int Id = 0, wxDC *dc = (wxDC *) NULL);

    wxDC *GetDC() const;
};


//---------------------------------------------------------------------------
%newgroup;

class wxFocusEvent : public wxEvent
{
public:
    wxFocusEvent(wxEventType type = wxEVT_NULL, int winid = 0);

    // The window associated with this event is the window which had focus
    // before for SET event and the window which will have focus for the KILL
    // one. NB: it may be NULL in both cases!
    wxWindow *GetWindow() const;
    void SetWindow(wxWindow *win);
};

//---------------------------------------------------------------------------
%newgroup;

// wxChildFocusEvent notifies the parent that a child has got the focus: unlike
// wxFocusEvent it is propagated upwards the window chain
class wxChildFocusEvent : public wxCommandEvent
{
public:
    wxChildFocusEvent(wxWindow *win = NULL);

    wxWindow *GetWindow() const;
};

//---------------------------------------------------------------------------
%newgroup;

class wxActivateEvent : public wxEvent
{
public:
    wxActivateEvent(wxEventType type = wxEVT_NULL, bool active = true, int Id = 0);
    bool GetActive() const;
};


//---------------------------------------------------------------------------
%newgroup;

class wxInitDialogEvent : public wxEvent
{
public:
    wxInitDialogEvent(int Id = 0);
};


//---------------------------------------------------------------------------
%newgroup;

class wxMenuEvent : public wxEvent
{
public:
    wxMenuEvent(wxEventType type = wxEVT_NULL, int winid = 0, wxMenu* menu = NULL);
    
    // only for wxEVT_MENU_HIGHLIGHT
    int GetMenuId() const;

    // only for wxEVT_MENU_OPEN/CLOSE
    bool IsPopup() const;

    // only for wxEVT_MENU_OPEN/CLOSE
    wxMenu* GetMenu() const;
};

//---------------------------------------------------------------------------
%newgroup;

// Window close or session close event class
class wxCloseEvent : public wxEvent
{
public:
    wxCloseEvent(wxEventType type = wxEVT_NULL, int winid = 0);
    
    void SetLoggingOff(bool logOff);
    bool GetLoggingOff() const;

    void Veto(bool veto = true);
    void SetCanVeto(bool canVeto);
    
    bool CanVeto() const;
    bool GetVeto() const;
};


//---------------------------------------------------------------------------
%newgroup;

class wxShowEvent : public wxEvent
{
public:
    wxShowEvent(int winid = 0, bool show = false);
    
    void SetShow(bool show);
    bool GetShow() const;

};


//---------------------------------------------------------------------------
%newgroup;

class wxIconizeEvent: public wxEvent
{
public:
    wxIconizeEvent(int id = 0, bool iconized = true);
    bool Iconized();
};


//---------------------------------------------------------------------------
%newgroup;

class wxMaximizeEvent: public wxEvent
{
public:
    wxMaximizeEvent(int id = 0);
};

//---------------------------------------------------------------------------
%newgroup;

class wxDropFilesEvent: public wxEvent
{
public:
    wxPoint GetPosition();
    int GetNumberOfFiles();

    %extend {
        PyObject* GetFiles() {
            int         count = self->GetNumberOfFiles();
            wxString*   files = self->GetFiles();
            PyObject*   list  = PyList_New(count);

            if (!list) {
                PyErr_SetString(PyExc_MemoryError, "Can't allocate list of files!");
                return NULL;
            }

            for (int i=0; i<count; i++) {
                PyList_SetItem(list, i, wx2PyString(files[i]));
            }
            return list;
        }
    }
};



//---------------------------------------------------------------------------
%newgroup;


// Whether to always send update events to windows, or
// to only send update events to those with the
// wxWS_EX_PROCESS_UI_UPDATES style.

enum wxUpdateUIMode
{
        // Send UI update events to all windows
    wxUPDATE_UI_PROCESS_ALL,

        // Send UI update events to windows that have
        // the wxWS_EX_PROCESS_UI_UPDATES flag specified
    wxUPDATE_UI_PROCESS_SPECIFIED
};



class wxUpdateUIEvent : public wxCommandEvent
{
public:
    wxUpdateUIEvent(wxWindowID commandId = 0);
    
    bool GetChecked() const;
    bool GetEnabled() const;
    wxString GetText() const;
    bool GetSetText() const;
    bool GetSetChecked() const;
    bool GetSetEnabled() const;

    void Check(bool check);
    void Enable(bool enable);
    void SetText(const wxString& text);

    // Sets the interval between updates in milliseconds.
    // Set to -1 to disable updates, or to 0 to update as frequently as possible.
    static void SetUpdateInterval(long updateInterval);

    // Returns the current interval between updates in milliseconds
    static long GetUpdateInterval();

    // Can we update this window?
    static bool CanUpdate(wxWindow *win);

    // Reset the update time to provide a delay until the next
    // time we should update
    static void ResetUpdateTime();

    // Specify how wxWindows will send update events: to
    // all windows, or only to those which specify that they
    // will process the events.
    static void SetMode(wxUpdateUIMode mode);

    // Returns the UI update mode
    static wxUpdateUIMode GetMode();
};

//---------------------------------------------------------------------------
%newgroup;

class wxSysColourChangedEvent: public wxEvent {
public:
    wxSysColourChangedEvent();
};

//---------------------------------------------------------------------------
%newgroup;


//  wxEVT_MOUSE_CAPTURE_CHANGED
//  The window losing the capture receives this message
//  (even if it released the capture itself).
class wxMouseCaptureChangedEvent : public wxEvent
{
public:
    wxMouseCaptureChangedEvent(wxWindowID winid = 0, wxWindow* gainedCapture = NULL);

    wxWindow* GetCapturedWindow() const;
};

//---------------------------------------------------------------------------
%newgroup;

class wxDisplayChangedEvent : public wxEvent
{
public:
    wxDisplayChangedEvent();
};

//---------------------------------------------------------------------------
%newgroup;

class  wxPaletteChangedEvent : public wxEvent
{
public:
    wxPaletteChangedEvent(wxWindowID id = 0);

    void SetChangedWindow(wxWindow* win);
    wxWindow* GetChangedWindow();

};

//---------------------------------------------------------------------------
%newgroup;


//  wxEVT_QUERY_NEW_PALETTE
//  Indicates the window is getting keyboard focus and should re-do its palette.
class wxQueryNewPaletteEvent : public wxEvent
{
public:
    wxQueryNewPaletteEvent(wxWindowID winid = 0);

    // App sets this if it changes the palette.
    void SetPaletteRealized(bool realized);
    bool GetPaletteRealized() const;
};

//---------------------------------------------------------------------------
%newgroup;


//  Event generated by dialog navigation keys
//  wxEVT_NAVIGATION_KEY

class wxNavigationKeyEvent : public wxEvent
{
public:
    wxNavigationKeyEvent();
    
    // direction: forward (True) or backward (False)
    bool GetDirection() const;
    void SetDirection(bool forward);

    // it may be a window change event (MDI, notebook pages...) or a control
    // change event
    bool IsWindowChange() const;
    void SetWindowChange(bool ischange);

    // Set to true under MSW if the event was generated using the tab key.
    // This is required for proper navogation over radio buttons
    bool IsFromTab() const;
    void SetFromTab(bool bIs);

    void SetFlags(long flags);
    
    // the child which has the focus currently (may be NULL - use
    // wxWindow::FindFocus then)
    wxWindow* GetCurrentFocus() const;
    void SetCurrentFocus(wxWindow *win);

    enum {
        IsBackward,
        IsForward,
        WinChange,
        FromTab
    };
};


//---------------------------------------------------------------------------
%newgroup;

// Window creation/destruction events: the first is sent as soon as window is
// created (i.e. the underlying GUI object exists), but when the C++ object is
// fully initialized (so virtual functions may be called). The second,
// wxEVT_DESTROY, is sent right before the window is destroyed - again, it's
// still safe to call virtual functions at this moment

class wxWindowCreateEvent : public wxCommandEvent
{
public:
    wxWindowCreateEvent(wxWindow *win = NULL);
    wxWindow *GetWindow() const;
};

class wxWindowDestroyEvent : public wxCommandEvent
{
public:
    wxWindowDestroyEvent(wxWindow *win = NULL);
    wxWindow *GetWindow() const;
};


//---------------------------------------------------------------------------
%newgroup;

// A Context event is sent when the user right clicks on a window or
// presses Shift-F10
// NOTE : Under windows this is a repackaged WM_CONTETXMENU message
//        Under other systems it may have to be generated from a right click event
/*
 wxEVT_CONTEXT_MENU
*/

class wxContextMenuEvent : public wxCommandEvent
{
public:
    wxContextMenuEvent(wxEventType type = wxEVT_NULL,
                       wxWindowID winid = 0,
                       const wxPoint& pt = wxDefaultPosition);

    // Position of event (in screen coordinates)
    const wxPoint& GetPosition() const;
    void SetPosition(const wxPoint& pos);
};

//---------------------------------------------------------------------------
%newgroup;

// Whether to always send idle events to windows, or
// to only send update events to those with the
// wxWS_EX_PROCESS_IDLE style.

enum wxIdleMode
{
        // Send idle events to all windows
    wxIDLE_PROCESS_ALL,

        // Send idle events to windows that have
        // the wxWS_EX_PROCESS_IDLE flag specified
    wxIDLE_PROCESS_SPECIFIED
};


class wxIdleEvent : public wxEvent
{
public:
    wxIdleEvent();
    
    void RequestMore(bool needMore = true);
    bool MoreRequested() const;

    // Specify how wxWindows will send idle events: to
    // all windows, or only to those which specify that they
    // will process the events.
    static void SetMode(wxIdleMode mode);

    // Returns the idle event mode
    static wxIdleMode GetMode();

    // Can we send an idle event?
    static bool CanSend(wxWindow* win);
};

//---------------------------------------------------------------------------
%newgroup;

// These classes can be derived from in Python and passed through the event
// system without loosing anything.  They do this by keeping a reference to
// themselves and some special case handling in wxPyCallback::EventThunker.

class wxPyEvent : public wxEvent {
public:
    %pythonAppend wxPyEvent     "self.SetSelf(self)"
    wxPyEvent(int winid=0, wxEventType commandType = wxEVT_NULL );
    ~wxPyEvent();


    void SetSelf(PyObject* self);
    PyObject* GetSelf();
};


class wxPyCommandEvent : public wxCommandEvent {
public:
    %pythonAppend wxPyCommandEvent     "self.SetSelf(self)"
    wxPyCommandEvent(wxEventType commandType = wxEVT_NULL, int id=0);
    ~wxPyCommandEvent();

    void SetSelf(PyObject* self);
    PyObject* GetSelf();
};



//---------------------------------------------------------------------------


class wxDateEvent : public wxCommandEvent
{
public:
    wxDateEvent(wxWindow *win, const wxDateTime& dt, wxEventType type);

    const wxDateTime& GetDate() const;
    void SetDate(const wxDateTime &date);

};


%constant wxEventType wxEVT_DATE_CHANGED;

%pythoncode {
    EVT_DATE_CHANGED = wx.PyEventBinder( wxEVT_DATE_CHANGED, 1 )
}


//---------------------------------------------------------------------------
