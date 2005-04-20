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
%constant wxEventType wxEVT_HIBERNATE;


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
EVT_HIBERNATE = wx.PyEventBinder( wxEVT_HIBERNATE )     
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

%# Scrolling from wx.Slider and wx.ScrollBar
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

%# Scrolling from wx.Slider and wx.ScrollBar, with an id
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

DocStr(wxEvent,
"An event is a structure holding information about an event passed to a
callback or member function. wx.Event is an abstract base class for
other event classes", "");

class wxEvent : public wxObject {
public:
    // wxEvent(int winid = 0, wxEventType commandType = wxEVT_NULL);     // *** This class is now an ABC
    ~wxEvent();

    DocDeclStr(
        void , SetEventType(wxEventType typ),
        "Sets the specific type of the event.", "");
    
    DocDeclStr(
        wxEventType , GetEventType() const,
        "Returns the identifier of the given event type, such as
``wxEVT_COMMAND_BUTTON_CLICKED``.", "");
    
    DocDeclStr(
        wxObject *, GetEventObject() const,
        "Returns the object (usually a window) associated with the event, if
any.", "");
    
    DocDeclStr(
        void , SetEventObject(wxObject *obj),
        "Sets the originating object, or in other words, obj is normally the
object that is sending the event.", "");
    
    long GetTimestamp() const;
    void SetTimestamp(long ts = 0);
    
    DocDeclStr(
        int  , GetId() const,
        "Returns the identifier associated with this event, such as a button
command id.", "");
    
    DocDeclStr(
        void , SetId(int Id),
        "Set's the ID for the event.  This is usually the ID of the window that
is sending the event, but it can also be a command id from a menu
item, etc.", "");
    


    DocDeclStr(
        bool , IsCommandEvent() const,
        "Returns true if the event is or is derived from `wx.CommandEvent` else
it returns false. Note: Exists only for optimization purposes.", "");
    

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
    
    DocDeclStr(
        bool , GetSkipped() const,
        "Returns true if the event handler should be skipped, false otherwise.
:see: `Skip`", "");
    

    DocDeclStr(
        bool , ShouldPropagate() const,
        "Test if this event should be propagated to the parent window or not,
i.e. if the propagation level is currently greater than 0.", "");
    

    // Stop an event from propagating to its parent window, returns the old
    // propagation level value
    DocDeclStr(
        int , StopPropagation(),
        "Stop the event from propagating to its parent window.  Returns the old
propagation level value which may be later passed to
`ResumePropagation` to allow propagating the event again.", "");
    

    DocDeclStr(
        void , ResumePropagation(int propagationLevel),
        "Resume the event propagation by restoring the propagation level.  (For
example, you can use the value returned by an earlier call to
`StopPropagation`.)
", "");
    

    // this function is used to create a copy of the event polymorphically and
    // all derived classes must implement it because otherwise wxPostEvent()
    // for them wouldn't work (it needs to do a copy of the event)
    virtual wxEvent *Clone() /* =0*/;
};


//---------------------------------------------------------------------------
%newgroup;

DocStr(wxPropagationDisabler,
"Helper class to temporarily change an event not to propagate.  Simply
create an instance of this class and then whe it is destroyed the
propogation of the event will be restored.", "");
class  wxPropagationDisabler
{
public:
    wxPropagationDisabler(wxEvent& event);
    ~wxPropagationDisabler();
};


DocStr( wxPropagateOnce,
"A helper class that will temporarily lower propagation level of an
event.  Simply create an instance of this class and then whe it is
destroyed the propogation of the event will be restored.", "");
class  wxPropagateOnce
{
public:
    wxPropagateOnce(wxEvent& event);
    ~wxPropagateOnce();
};

//---------------------------------------------------------------------------
%newgroup;

DocStr(wxCommandEvent,
"This event class contains information about command events, which
originate from a variety of simple controls, as well as menus and
toolbars.", "");

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

    
    DocDeclStr(
        int , GetSelection() const,
        "Returns item index for a listbox or choice selection event (not valid
for a deselection).", "");
    

    void SetString(const wxString& s);
    DocDeclStr(
        wxString , GetString() const,
        "Returns item string for a listbox or choice selection event (not valid
for a deselection).", "");
    

    DocDeclStr(
        bool , IsChecked() const,
        "This method can be used with checkbox and menu events: for the
checkboxes, the method returns true for a selection event and false
for a deselection one. For the menu events, this method indicates if
the menu item just has become checked or unchecked (and thus only
makes sense for checkable menu items).", "");
    
    %pythoncode { Checked = IsChecked }
    
    DocDeclStr(
        bool , IsSelection() const,
        "For a listbox or similar event, returns true if it is a selection,
false if it is a deselection.", "");
    

    void SetExtraLong(long extraLong);
    DocDeclStr(
        long , GetExtraLong() const,
        "Returns extra information dependant on the event objects type. If the event
comes from a listbox selection, it is a boolean determining whether the event
was a selection (true) or a deselection (false). A listbox deselection only
occurs for multiple-selection boxes, and in this case the index and string
values are indeterminate and the listbox must be examined by the application.", "");
    

    void SetInt(int i);
    DocDeclStr(
        long , GetInt() const,
        "Returns the integer identifier corresponding to a listbox, choice or radiobox
selection (only if the event was a selection, not a deselection), or a boolean
value representing the value of a checkbox.", "");
    

    virtual wxEvent *Clone() const;

};

//---------------------------------------------------------------------------
%newgroup;

DocStr(wxNotifyEvent,
"An instance of this class (or one of its derived classes) is sent from
a control when the control's state is being changed and the control
allows that change to be prevented from happening.  The event handler
can call `Veto` or `Allow` to tell the control what to do.", "");

class wxNotifyEvent : public wxCommandEvent
{
public:
    wxNotifyEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);

    DocDeclStr(
        void , Veto(),
        "Prevents the change announced by this event from happening.

It is in general a good idea to notify the user about the reasons for
vetoing the change because otherwise the applications behaviour (which
just refuses to do what the user wants) might be quite surprising.", "");
    

    DocDeclStr(
        void , Allow(),
        "This is the opposite of `Veto`: it explicitly allows the event to be
processed. For most events it is not necessary to call this method as
the events are allowed anyhow but some are forbidden by default (this
will be mentioned in the corresponding event description).", "");
    

    DocDeclStr(
        bool , IsAllowed(),
        "Returns true if the change is allowed (`Veto` hasn't been called) or
false otherwise (if it was).", "");
    
};


//---------------------------------------------------------------------------
%newgroup;

DocStr(wxScrollEvent,
"A scroll event holds information about events sent from stand-alone
scrollbars and sliders. Note that scrolled windows do not send
instnaces of this event class, but send the `wx.ScrollWinEvent`
instead.", "

Events
-------
    =======================     ==========================================
    EVT_SCROLL                  Used to bind all scroll events
    EVT_SCROLL_TOP              scroll-to-top events (minimum position)
    EVT_SCROLL_BOTTOM           scroll-to-bottom events (maximum position)
    EVT_SCROLL_LINEUP           line up events
    EVT_SCROLL_LINEDOWN         line down events
    EVT_SCROLL_PAGEUP           page up events
    EVT_SCROLL_PAGEDOWN         page down events
    EVT_SCROLL_THUMBTRACK       thumbtrack events (frequent events sent
                                as the user drags the 'thumb')
    EVT_SCROLL_THUMBRELEASE     thumb release events.
    EVT_SCROLL_ENDSCROLL        End of scrolling
    =======================     ==========================================

Note
------
    The EVT_SCROLL_THUMBRELEASE event is only emitted when actually
    dragging the thumb using the mouse and releasing it (This
    EVT_SCROLL_THUMBRELEASE event is also followed by an
    EVT_SCROLL_ENDSCROLL event).

    The EVT_SCROLL_ENDSCROLL event also occurs when using the keyboard
    to change the thumb position, and when clicking next to the thumb
    (In all these cases the EVT_SCROLL_THUMBRELEASE event does not
    happen).

    In short, the EVT_SCROLL_ENDSCROLL event is triggered when
    scrolling/ moving has finished. The only exception (unfortunately)
    is that changing the thumb position using the mousewheel does give
    a EVT_SCROLL_THUMBRELEASE event but NOT an EVT_SCROLL_ENDSCROLL
    event.
");

class  wxScrollEvent : public wxCommandEvent
{
public:
    DocCtorStr(
        wxScrollEvent(wxEventType commandType = wxEVT_NULL,
                      int winid = 0, int pos = 0, int orient = 0),
        "", "");

    DocDeclStr(
        int , GetOrientation() const,
        "Returns wx.HORIZONTAL or wx.VERTICAL, depending on the orientation of
the scrollbar.", "");
    
    DocDeclStr(
        int , GetPosition() const,
        "Returns the position of the scrollbar.", "");
    
    void SetOrientation(int orient);
    void SetPosition(int pos);
};


//---------------------------------------------------------------------------
%newgroup;

DocStr(wxScrollWinEvent,
"A wx.ScrollWinEvent holds information about scrolling and is sent from
scrolling windows.", "

Events
-------
    ==========================     ==========================================
    EVT_SCROLLWIN                  Used to bind all scrolled window scroll events
    EVT_SCROLLWIN_TOP              scroll-to-top events (minimum position)
    EVT_SCROLLWIN_BOTTOM           scroll-to-bottom events (maximum position)
    EVT_SCROLLWIN_LINEUP           line up events
    EVT_SCROLLWIN_LINEDOWN         line down events
    EVT_SCROLLWIN_PAGEUP           page up events
    EVT_SCROLLWIN_PAGEDOWN         page down events
    EVT_SCROLLWIN_THUMBTRACK       thumbtrack events (frequent events sent
                                   as the user drags the 'thumb')
    EVT_SCROLLWIN_THUMBRELEASE     thumb release events.
    EVT_SCROLLWIN_ENDSCROLL        End of scrolling
    ==========================     ==========================================

");

class wxScrollWinEvent : public wxEvent
{
public:
    wxScrollWinEvent(wxEventType commandType = wxEVT_NULL,
                     int pos = 0, int orient = 0);

    DocDeclStr(
        int , GetOrientation() const,
        "Returns wx.HORIZONTAL or wx.VERTICAL, depending on the orientation of
the scrollbar.", "");
    
    DocDeclStr(
        int , GetPosition() const,
        "Returns the position of the scrollbar for the thumb track and release
events. Note that this field can't be used for the other events, you
need to query the window itself for the current position in that case.", "");
    
    void SetOrientation(int orient);
    void SetPosition(int pos);
};

//---------------------------------------------------------------------------
%newgroup;


DocStr(wxMouseEvent,
"This event class contains information about the events generated by
the mouse: they include mouse buttons press and release events and
mouse move events.

All mouse events involving the buttons use ``wx.MOUSE_BTN_LEFT`` for
the left mouse button, ``wx.MOUSE_BTN_MIDDLE`` for the middle one and
``wx.MOUSE_BTN_RIGHT`` for the right one. Note that not all mice have
a middle button so a portable application should avoid relying on the
events from it.

Note the difference between methods like `LeftDown` and `LeftIsDown`:
the former returns true when the event corresponds to the left mouse
button click while the latter returns true if the left mouse button is
currently being pressed. For example, when the user is dragging the
mouse you can use `LeftIsDown` to test whether the left mouse button
is (still) depressed. Also, by convention, if `LeftDown` returns true,
`LeftIsDown` will also return true in wxWidgets whatever the
underlying GUI behaviour is (which is platform-dependent). The same
applies, of course, to other mouse buttons as well.", "

Events
-------
    ==================     ==============================================
    EVT_LEFT_DOWN          Left mouse button down event. The handler
                           of this event should normally call
                           event.Skip() to allow the default processing
                           to take place as otherwise the window under
                           mouse wouldn't get the focus.
    EVT_LEFT_UP            Left mouse button up event
    EVT_LEFT_DCLICK        Left mouse button double click event
    EVT_MIDDLE_DOWN        Middle mouse button down event
    EVT_MIDDLE_UP          Middle mouse button up event
    EVT_MIDDLE_DCLICK      Middle mouse button double click event
    EVT_RIGHT_DOWN         Right mouse button down event
    EVT_RIGHT_UP           Right mouse button up event
    EVT_RIGHT_DCLICK       Right mouse button double click event
    EVT_MOTION             Event sent when the mouse is moving
    EVT_ENTER_WINDOW       Event sent when the mouse enters the
                           boundaries of a window.
    EVT_LEAVE_WINDOW       Sent when the mouse leaves the window's bounds
    EVT_MOUSEWHEEL         Mouse scroll wheel event
    EVT_MOUSE_EVENTS       Binds all mouse events at once.
    ==================     ==============================================

");


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
    DocCtorStr(
        wxMouseEvent(wxEventType mouseType = wxEVT_NULL),
"Constructs a wx.MouseEvent.  Valid event types are:

    * wxEVT_ENTER_WINDOW
    * wxEVT_LEAVE_WINDOW
    * wxEVT_LEFT_DOWN
    * wxEVT_LEFT_UP
    * wxEVT_LEFT_DCLICK
    * wxEVT_MIDDLE_DOWN
    * wxEVT_MIDDLE_UP
    * wxEVT_MIDDLE_DCLICK
    * wxEVT_RIGHT_DOWN
    * wxEVT_RIGHT_UP
    * wxEVT_RIGHT_DCLICK
    * wxEVT_MOTION
    * wxEVT_MOUSEWHEEL ", "");


    DocDeclStr(
        bool , IsButton() const,
        "Returns true if the event was a mouse button event (not necessarily a
button down event - that may be tested using `ButtonDown`).", "");
    

    DocDeclStr(
        bool , ButtonDown(int but = wxMOUSE_BTN_ANY) const,
        "If the argument is omitted, this returns true if the event was any mouse
button down event. Otherwise the argument specifies which button-down
event shold be checked for (see `Button` for the possible values).", "");
    

    DocDeclStr(
        bool , ButtonDClick(int but = wxMOUSE_BTN_ANY) const,
        "If the argument is omitted, this returns true if the event was any
mouse double click event. Otherwise the argument specifies which
double click event to check for (see `Button` for the possible
values).", "");
    

    DocDeclStr(
        bool , ButtonUp(int but = wxMOUSE_BTN_ANY) const,
        "If the argument is omitted, this returns true if the event was any
mouse button up event. Otherwise the argument specifies which
button up event to check for (see `Button` for the possible values).", "");
    

    DocDeclStr(
        bool , Button(int button) const,
        "Returns true if the identified mouse button is changing state. Valid
values of button are:

     ====================      =====================================
     wx.MOUSE_BTN_LEFT         check if left button was pressed
     wx.MOUSE_BTN_MIDDLE       check if middle button was pressed
     wx.MOUSE_BTN_RIGHT        check if right button was pressed
     wx.MOUSE_BTN_ANY          check if any button was pressed
     ====================      =====================================
", "");
    

    // Was the given button in Down state?
    bool ButtonIsDown(int but) const;

    DocDeclStr(
        int , GetButton() const,
        "Returns the mouse button which generated this event or
wx.MOUSE_BTN_NONE if no button is involved (for mouse move, enter or
leave event, for example). Otherwise wx.MOUSE_BTN_LEFT is returned for
the left button down, up and double click events, wx.MOUSE_BTN_MIDDLE
and wx.MOUSE_BTN_RIGHT for the same events for the middle and the
right buttons respectively.", "");
    

    DocDeclStr(
        bool , ControlDown() const,
        "Returns true if the control key was down at the time of the event.", "");
    
    DocDeclStr(
        bool , MetaDown() const,
        "Returns true if the Meta key was down at the time of the event.", "");
    
    
    DocDeclStr(
        bool , AltDown() const,
        "Returns true if the Alt key was down at the time of the event.", "");
    
    DocDeclStr(
        bool , ShiftDown() const,
        "Returns true if the Shift key was down at the time of the event.", "");
    

    DocDeclStr(
        bool , CmdDown() const,
        "\"Cmd\" is a pseudo key which is the same as Control for PC and Unix
platforms but the special \"Apple\" (a.k.a as \"Command\") key on
Macs: it often makes sense to use it instead of, say, `ControlDown`
because Cmd key is used for the same thing under Mac as Ctrl
elsewhere. The Ctrl key still exists, it's just not used for this
purpose. So for non-Mac platforms this is the same as `ControlDown`
and Macs this is the same as `MetaDown`.", "");
    

    DocDeclStr(
        bool , LeftDown() const,
        "Returns true if the left mouse button state changed to down.", "");
    
    DocDeclStr(
        bool , MiddleDown() const,
        "Returns true if the middle mouse button state changed to down.", "");
    
    DocDeclStr(
        bool , RightDown() const,
        "Returns true if the right mouse button state changed to down.", "");
    

    
    DocDeclStr(
        bool , LeftUp() const,
        "Returns true if the left mouse button state changed to up.", "");
    
    DocDeclStr(
        bool , MiddleUp() const,
        "Returns true if the middle mouse button state changed to up.", "");
    
    DocDeclStr(
        bool , RightUp() const,
        "Returns true if the right mouse button state changed to up.", "");
    


    
    DocDeclStr(
        bool , LeftDClick() const,
        "Returns true if the event was a left button double click.", "");
    
    DocDeclStr(
        bool , MiddleDClick() const,
        "Returns true if the event was a middle button double click.", "");
    
    DocDeclStr(
        bool , RightDClick() const,
        "Returns true if the event was a right button double click.", "");
    

    
    DocDeclStr(
        bool , LeftIsDown(),
        "Returns true if the left mouse button is currently down, independent
of the current event type.

Please notice that it is not the same as LeftDown which returns true
if the left mouse button was just pressed. Rather, it describes the
state of the mouse button before the event happened.

This event is usually used in the mouse event handlers which process
\"move mouse\" messages to determine whether the user is (still)
dragging the mouse.", "");
    
    DocDeclStr(
        bool , MiddleIsDown(),
        "Returns true if the middle mouse button is currently down, independent
of the current event type.", "");
    
    DocDeclStr(
        bool , RightIsDown(),
        "Returns true if the right mouse button is currently down, independent
of the current event type.", "");
    

    
    DocDeclStr(
        bool , Dragging() const,
        "Returns true if this was a dragging event (motion while a button is
depressed).", "");
    

    DocDeclStr(
        bool , Moving() const,
        "Returns true if this was a motion event and no mouse buttons were
pressed. If any mouse button is held pressed, then this method returns
false and Dragging returns true.", "");
    

    DocDeclStr(
        bool , Entering() const,
        "Returns true if the mouse was entering the window.", "");
    

    DocDeclStr(
        bool , Leaving() const,
        "Returns true if the mouse was leaving the window.", "");
    


    DocStr(GetPosition,   // sets the docstring for both
           "Returns the pixel position of the mouse in window coordinates when the
event happened.", "");
    wxPoint GetPosition();

    DocDeclAName(
        void, GetPosition(long *OUTPUT, long *OUTPUT),
        "GetPositionTuple() -> (x,y)",
        GetPositionTuple);
    
    DocDeclStr(
        wxPoint , GetLogicalPosition(const wxDC& dc) const,
        "Returns the logical mouse position in pixels (i.e. translated
according to the translation set for the DC, which usually indicates
that the window has been scrolled).", "");
    

    DocDeclStr(
        wxCoord , GetX() const,
        "Returns X coordinate of the physical mouse event position.", "");
    
    DocDeclStr(
        wxCoord , GetY() const,
        "Returns Y coordinate of the physical mouse event position.", "");
    

    DocDeclStr(
        int , GetWheelRotation() const,
        "Get wheel rotation, positive or negative indicates direction of
rotation. Current devices all send an event when rotation is equal to
+/-WheelDelta, but this allows for finer resolution devices to be
created in the future. Because of this you shouldn't assume that one
event is equal to 1 line or whatever, but you should be able to either
do partial line scrolling or wait until +/-WheelDelta rotation values
have been accumulated before scrolling.", "");
    

    DocDeclStr(
        int , GetWheelDelta() const,
        "Get wheel delta, normally 120. This is the threshold for action to be
taken, and one such action (for example, scrolling one increment)
should occur for each delta.", "");
    

    DocDeclStr(
        int , GetLinesPerAction() const,
        "Returns the configured number of lines (or whatever) to be scrolled
per wheel action. Defaults to three.", "");
    

    DocDeclStr(
        bool , IsPageScroll() const,
        "Returns true if the system has been setup to do page scrolling with
the mouse wheel instead of line scrolling.", "");
    

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
