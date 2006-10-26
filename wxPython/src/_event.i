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

%{
#if ! wxUSE_HOTKEY
#define wxEVT_HOTKEY -9999
#endif
%}

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
%constant wxEventType wxEVT_SCROLL_CHANGED;
%pythoncode { wxEVT_SCROLL_ENDSCROLL = wxEVT_SCROLL_CHANGED }


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
%constant wxEventType wxEVT_ACTIVATE;
%constant wxEventType wxEVT_CREATE;
%constant wxEventType wxEVT_DESTROY;
%constant wxEventType wxEVT_SHOW;
%constant wxEventType wxEVT_ICONIZE;
%constant wxEventType wxEVT_MAXIMIZE;
%constant wxEventType wxEVT_MOUSE_CAPTURE_CHANGED;
%constant wxEventType wxEVT_MOUSE_CAPTURE_LOST;
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

%constant wxEventType wxEVT_COMMAND_TEXT_COPY;
%constant wxEventType wxEVT_COMMAND_TEXT_CUT;
%constant wxEventType wxEVT_COMMAND_TEXT_PASTE;


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
EVT_MOUSE_CAPTURE_LOST = wx.PyEventBinder( wxEVT_MOUSE_CAPTURE_LOST )         

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
                               wxEVT_SCROLL_CHANGED,
                               ])

EVT_SCROLL_TOP = wx.PyEventBinder( wxEVT_SCROLL_TOP )
EVT_SCROLL_BOTTOM = wx.PyEventBinder( wxEVT_SCROLL_BOTTOM )
EVT_SCROLL_LINEUP = wx.PyEventBinder( wxEVT_SCROLL_LINEUP )
EVT_SCROLL_LINEDOWN = wx.PyEventBinder( wxEVT_SCROLL_LINEDOWN )
EVT_SCROLL_PAGEUP = wx.PyEventBinder( wxEVT_SCROLL_PAGEUP )
EVT_SCROLL_PAGEDOWN = wx.PyEventBinder( wxEVT_SCROLL_PAGEDOWN )
EVT_SCROLL_THUMBTRACK = wx.PyEventBinder( wxEVT_SCROLL_THUMBTRACK )
EVT_SCROLL_THUMBRELEASE = wx.PyEventBinder( wxEVT_SCROLL_THUMBRELEASE )
EVT_SCROLL_CHANGED = wx.PyEventBinder( wxEVT_SCROLL_CHANGED )
EVT_SCROLL_ENDSCROLL = EVT_SCROLL_CHANGED

%# Scrolling from wx.Slider and wx.ScrollBar, with an id
EVT_COMMAND_SCROLL = wx.PyEventBinder([ wxEVT_SCROLL_TOP,
                                       wxEVT_SCROLL_BOTTOM,
                                       wxEVT_SCROLL_LINEUP,
                                       wxEVT_SCROLL_LINEDOWN,
                                       wxEVT_SCROLL_PAGEUP,
                                       wxEVT_SCROLL_PAGEDOWN,
                                       wxEVT_SCROLL_THUMBTRACK,
                                       wxEVT_SCROLL_THUMBRELEASE,
                                       wxEVT_SCROLL_CHANGED,
                                       ], 1)

EVT_COMMAND_SCROLL_TOP = wx.PyEventBinder( wxEVT_SCROLL_TOP, 1)
EVT_COMMAND_SCROLL_BOTTOM = wx.PyEventBinder( wxEVT_SCROLL_BOTTOM, 1)
EVT_COMMAND_SCROLL_LINEUP = wx.PyEventBinder( wxEVT_SCROLL_LINEUP, 1)
EVT_COMMAND_SCROLL_LINEDOWN = wx.PyEventBinder( wxEVT_SCROLL_LINEDOWN, 1)
EVT_COMMAND_SCROLL_PAGEUP = wx.PyEventBinder( wxEVT_SCROLL_PAGEUP, 1)
EVT_COMMAND_SCROLL_PAGEDOWN = wx.PyEventBinder( wxEVT_SCROLL_PAGEDOWN, 1)
EVT_COMMAND_SCROLL_THUMBTRACK = wx.PyEventBinder( wxEVT_SCROLL_THUMBTRACK, 1)
EVT_COMMAND_SCROLL_THUMBRELEASE = wx.PyEventBinder( wxEVT_SCROLL_THUMBRELEASE, 1)
EVT_COMMAND_SCROLL_CHANGED = wx.PyEventBinder( wxEVT_SCROLL_CHANGED, 1)
EVT_COMMAND_SCROLL_ENDSCROLL = EVT_COMMAND_SCROLL_CHANGED

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

EVT_TEXT_CUT   =  wx.PyEventBinder( wxEVT_COMMAND_TEXT_CUT )
EVT_TEXT_COPY  =  wx.PyEventBinder( wxEVT_COMMAND_TEXT_COPY )
EVT_TEXT_PASTE =  wx.PyEventBinder( wxEVT_COMMAND_TEXT_PASTE )

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
        "This method can be used inside an event handler to control whether
further event handlers bound to this event will be called after the
current one returns. Without Skip() (or equivalently if Skip(False) is
used), the event will not be processed any more. If Skip(True) is
called, the event processing system continues searching for a further
handler function for this event, even though it has been processed
already in the current handler.", "");

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

    %property(EventObject, GetEventObject, SetEventObject, doc="See `GetEventObject` and `SetEventObject`");
    %property(EventType, GetEventType, SetEventType, doc="See `GetEventType` and `SetEventType`");
    %property(Id, GetId, SetId, doc="See `GetId` and `SetId`");
    %property(Skipped, GetSkipped, doc="See `GetSkipped`");
    %property(Timestamp, GetTimestamp, SetTimestamp, doc="See `GetTimestamp` and `SetTimestamp`");
    
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
        "Returns extra information dependant on the event objects type. If the
event comes from a listbox selection, it is a boolean determining
whether the event was a selection (true) or a deselection (false). A
listbox deselection only occurs for multiple-selection boxes, and in
this case the index and string values are indeterminate and the
listbox must be examined by the application.", "");


    void SetInt(int i);
    DocDeclStr(
        int , GetInt() const,
        "Returns the integer identifier corresponding to a listbox, choice or
radiobox selection (only if the event was a selection, not a
deselection), or a boolean value representing the value of a checkbox.", "");


     %extend {
        DocStr(GetClientData,
               "Returns the client data object for a listbox or choice selection event, (if any.)", "");
        PyObject* GetClientData() {
            wxPyClientData* data = (wxPyClientData*)self->GetClientObject();
            if (data) {
                Py_INCREF(data->m_obj);
                return data->m_obj;
            } else {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }

        DocStr(SetClientData,
               "Associate the given client data with the item at position n.", "");
        void SetClientData(PyObject* clientData) {
            wxPyClientData* data = new wxPyClientData(clientData);
            self->SetClientObject(data);
        }
    }
    %pythoncode {
         GetClientObject = GetClientData
         SetClientObject = SetClientData
    }

    virtual wxEvent *Clone() const;

    %property(ClientData, GetClientData, SetClientData, doc="See `GetClientData` and `SetClientData`");
    %property(ClientObject, GetClientObject, SetClientObject, doc="See `GetClientObject` and `SetClientObject`");
    %property(ExtraLong, GetExtraLong, SetExtraLong, doc="See `GetExtraLong` and `SetExtraLong`");
    %property(Int, GetInt, SetInt, doc="See `GetInt` and `SetInt`");
    %property(Selection, GetSelection, doc="See `GetSelection`");
    %property(String, GetString, SetString, doc="See `GetString` and `SetString`");
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
    EVT_SCROLL_THUMBRELEASE     thumb release events
    EVT_SCROLL_CHANGED          End of scrolling
    =======================     ==========================================

Note
------
    The EVT_SCROLL_THUMBRELEASE event is only emitted when actually
    dragging the thumb using the mouse and releasing it (This
    EVT_SCROLL_THUMBRELEASE event is also followed by an
    EVT_SCROLL_CHANGED event).

    The EVT_SCROLL_CHANGED event also occurs when using the keyboard
    to change the thumb position, and when clicking next to the thumb
    (In all these cases the EVT_SCROLL_THUMBRELEASE event does not
    happen).

    In short, the EVT_SCROLL_CHANGED event is triggered when
    scrolling/ moving has finished. The only exception (unfortunately)
    is that changing the thumb position using the mousewheel does give
    a EVT_SCROLL_THUMBRELEASE event but NOT an EVT_SCROLL_CHANGED
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
    
    %property(Orientation, GetOrientation, SetOrientation, doc="See `GetOrientation` and `SetOrientation`");
    %property(Position, GetPosition, SetPosition, doc="See `GetPosition` and `SetPosition`");
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
    EVT_SCROLLWIN_THUMBRELEASE     thumb release events
    EVT_SCROLLWIN_CHANGED          End of scrolling
    ==========================     ==========================================

:see: `wx.ScrollEvent`
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

    %property(Orientation, GetOrientation, SetOrientation, doc="See `GetOrientation` and `SetOrientation`");
    %property(Position, GetPosition, SetPosition, doc="See `GetPosition` and `SetPosition`");
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
    // turn off this typemap
    %typemap(out) wxMouseEvent*;    
    
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

    // Turn it back on again
    %typemap(out) wxMouseEvent* { $result = wxPyMake_wxObject($1, $owner); }

    DocDeclStr(
        bool , IsButton() const,
        "Returns true if the event was a mouse button event (not necessarily a
button down event - that may be tested using `ButtonDown`).", "");


    DocDeclStr(
        bool , ButtonDown(int but = wxMOUSE_BTN_ANY) const,
        "If the argument is omitted, this returns true if the event was any
mouse button down event. Otherwise the argument specifies which
button-down event shold be checked for (see `Button` for the possible
values).", "");


    DocDeclStr(
        bool , ButtonDClick(int but = wxMOUSE_BTN_ANY) const,
        "If the argument is omitted, this returns true if the event was any
mouse double click event. Otherwise the argument specifies which
double click event to check for (see `Button` for the possible
values).", "");


    DocDeclStr(
        bool , ButtonUp(int but = wxMOUSE_BTN_ANY) const,
        "If the argument is omitted, this returns true if the event was any
mouse button up event. Otherwise the argument specifies which button
up event to check for (see `Button` for the possible values).", "");


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
Macs. It often makes sense to use it instead of, say, `ControlDown`
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

    %property(Button, GetButton, doc="See `GetButton`");
    %property(LinesPerAction, GetLinesPerAction, doc="See `GetLinesPerAction`");
    %property(LogicalPosition, GetLogicalPosition, doc="See `GetLogicalPosition`");
    %property(Position, GetPosition, doc="See `GetPosition`");
    %property(WheelDelta, GetWheelDelta, doc="See `GetWheelDelta`");
    %property(WheelRotation, GetWheelRotation, doc="See `GetWheelRotation`");
    %property(X, GetX, doc="See `GetX`");
    %property(Y, GetY, doc="See `GetY`");
};

//---------------------------------------------------------------------------
%newgroup;

// Cursor set event

DocStr(wxSetCursorEvent,
"A SetCursorEvent is generated when the mouse cursor is about to be set
as a result of mouse motion. This event gives the application the
chance to perform specific mouse cursor processing based on the
current position of the mouse within the window. Use the `SetCursor`
method to specify the cursor you want to be displayed.", "");

class wxSetCursorEvent : public wxEvent
{
public:
    DocCtorStr(
        wxSetCursorEvent(wxCoord x = 0, wxCoord y = 0),
        "Construct a new `wx.SetCursorEvent`.", "");

    DocDeclStr(
        wxCoord , GetX() const,
        "Returns the X coordinate of the mouse in client coordinates.", "");

    DocDeclStr(
        wxCoord , GetY() const,
        "Returns the Y coordinate of the mouse in client coordinates.", "");


    DocDeclStr(
        void , SetCursor(const wxCursor& cursor),
        "Sets the cursor associated with this event.", "");

    DocDeclStr(
        const wxCursor& , GetCursor() const,
        "Returns a reference to the cursor specified by this event.", "");

    DocDeclStr(
        bool , HasCursor() const,
        "Returns true if the cursor specified by this event is a valid cursor.", "");

    %property(Cursor, GetCursor, SetCursor, doc="See `GetCursor` and `SetCursor`");
    %property(X, GetX, doc="See `GetX`");
    %property(Y, GetY, doc="See `GetY`");
};

//---------------------------------------------------------------------------
%newgroup;

// Keyboard input event class

DocStr(wxKeyEvent,
"This event class contains information about keypress and character
events.  These events are only sent to the widget that currently has
the keyboard focus.

Notice that there are three different kinds of keyboard events in
wxWidgets: key down and up events and char events. The difference
between the first two is clear - the first corresponds to a key press
and the second to a key release - otherwise they are identical. Just
note that if the key is maintained in a pressed state you will
typically get a lot of (automatically generated) down events but only
one up so it is wrong to assume that there is one up event
corresponding to each down one.

Both key events provide untranslated key codes while the char event
carries the translated one. The untranslated code for alphanumeric
keys is always an upper case value. For the other keys it is one of
WXK_XXX values from the keycodes table. The translated key is, in
general, the character the user expects to appear as the result of the
key combination when typing the text into a text entry zone, for
example.

A few examples to clarify this (all assume that CAPS LOCK is unpressed
and the standard US keyboard): when the 'A' key is pressed, the key
down event key code is equal to ASCII A == 65. But the char event key
code is ASCII a == 97. On the other hand, if you press both SHIFT and
'A' keys simultaneously , the key code in key down event will still be
just 'A' while the char event key code parameter will now be 'A' as
well.

Although in this simple case it is clear that the correct key code
could be found in the key down event handler by checking the value
returned by `ShiftDown`, in general you should use EVT_CHAR for this
as for non alphanumeric keys or non-US keyboard layouts the
translation is keyboard-layout dependent and can only be done properly
by the system itself.

Another kind of translation is done when the control key is pressed:
for example, for CTRL-A key press the key down event still carries the
same key code 'A' as usual but the char event will have key code of 1,
the ASCII value of this key combination.

You may discover how the other keys on your system behave
interactively by running the KeyEvents sample in the wxPython demo and
pressing some keys while the blue box at the top has the keyboard
focus.

**Note**: If a key down event is caught and the event handler does not
call event.Skip() then the coresponding char event will not
happen. This is by design and enables the programs that handle both
types of events to be a bit simpler.

**Note for Windows programmers**: The key and char events in wxWidgets
are similar to but slightly different from Windows WM_KEYDOWN and
WM_CHAR events. In particular, Alt-x combination will generate a char
event in wxWidgets (unless it is used as an accelerator).

**Tip**: be sure to call event.Skip() for events that you don't
process in key event function, otherwise menu shortcuts may cease to
work under Windows.
", "

Events
------
    ==================     ==============================================
    wx.EVT_KEY_DOWN        Sent when a keyboard key has been pressed
    wx.EVT_KEY_UP          Sent when a keyboard key has been released
    wx.EVT_CHAR            Sent for translated character events.
    ==================     ==============================================


Keycode Table
-------------
    ===========  ==============  ========  ====================  =================
    WXK_BACK     WXK_EXECUTE     WXK_F1    WXK_NUMPAD_SPACE      WXK_WINDOWS_LEFT
    WXK_TAB      WXK_SNAPSHOT    WXK_F2    WXK_NUMPAD_TAB        WXK_WINDOWS_RIGHT
    WXK_RETURN   WXK_INSERT      WXK_F3    WXK_NUMPAD_ENTER      WXK_WINDOWS_MENU
    WXK_ESCAPE   WXK_HELP        WXK_F4    WXK_NUMPAD_F1         WXK_SPECIAL1
    WXK_SPACE    WXK_NUMPAD0     WXK_F5    WXK_NUMPAD_F2         WXK_SPECIAL2
    WXK_DELETE   WXK_NUMPAD1     WXK_F6    WXK_NUMPAD_F3         WXK_SPECIAL3
    WXK_LBUTTON  WXK_NUMPAD2     WXK_F7    WXK_NUMPAD_F4         WXK_SPECIAL4
    WXK_RBUTTON  WXK_NUMPAD3     WXK_F8    WXK_NUMPAD_HOME       WXK_SPECIAL5
    WXK_CANCEL   WXK_NUMPAD4     WXK_F9    WXK_NUMPAD_LEFT       WXK_SPECIAL6
    WXK_MBUTTON  WXK_NUMPAD5     WXK_F10   WXK_NUMPAD_UP         WXK_SPECIAL7
    WXK_CLEAR    WXK_NUMPAD6     WXK_F11   WXK_NUMPAD_RIGHT      WXK_SPECIAL8
    WXK_SHIFT    WXK_NUMPAD7     WXK_F12   WXK_NUMPAD_DOWN       WXK_SPECIAL9
    WXK_ALT      WXK_NUMPAD8     WXK_F13   WXK_NUMPAD_PRIOR      WXK_SPECIAL10
    WXK_CONTROL  WXK_NUMPAD9     WXK_F14   WXK_NUMPAD_PAGEUP     WXK_SPECIAL11
    WXK_MENU     WXK_MULTIPLY    WXK_F15   WXK_NUMPAD_NEXT       WXK_SPECIAL12
    WXK_PAUSE    WXK_ADD         WXK_F16   WXK_NUMPAD_PAGEDOWN   WXK_SPECIAL13
    WXK_CAPITAL  WXK_SEPARATOR   WXK_F17   WXK_NUMPAD_END        WXK_SPECIAL14
    WXK_PRIOR    WXK_SUBTRACT    WXK_F18   WXK_NUMPAD_BEGIN      WXK_SPECIAL15
    WXK_NEXT     WXK_DECIMAL     WXK_F19   WXK_NUMPAD_INSERT     WXK_SPECIAL16
    WXK_END      WXK_DIVIDE      WXK_F20   WXK_NUMPAD_DELETE     WXK_SPECIAL17
    WXK_HOME     WXK_NUMLOCK     WXK_F21   WXK_NUMPAD_EQUAL      WXK_SPECIAL18
    WXK_LEFT     WXK_SCROLL      WXK_F22   WXK_NUMPAD_MULTIPLY   WXK_SPECIAL19
    WXK_UP       WXK_PAGEUP      WXK_F23   WXK_NUMPAD_ADD        WXK_SPECIAL20
    WXK_RIGHT    WXK_PAGEDOWN    WXK_F24   WXK_NUMPAD_SEPARATOR
    WXK_DOWN                               WXK_NUMPAD_SUBTRACT
    WXK_SELECT                             WXK_NUMPAD_DECIMAL
    WXK_PRINT                              WXK_NUMPAD_DIVIDE
    ===========  ==============  ========  ====================  =================
");

class wxKeyEvent : public wxEvent
{
public:
    DocCtorStr(
        wxKeyEvent(wxEventType eventType = wxEVT_NULL),
        "Construct a new `wx.KeyEvent`.  Valid event types are:
    * ", "");


    DocDeclStr(
        int, GetModifiers() const,
        "Returns a bitmask of the current modifier settings.  Can be used to
check if the key event has exactly the given modifiers without having
to explicitly check that the other modifiers are not down.  For
example::

    if event.GetModifers() == wx.MOD_CONTROL:
        DoSomething()
", "");

    DocDeclStr(
        bool , ControlDown() const,
        "Returns ``True`` if the Control key was down at the time of the event.", "");

    DocDeclStr(
        bool , MetaDown() const,
        "Returns ``True`` if the Meta key was down at the time of the event.", "");

    DocDeclStr(
        bool , AltDown() const,
        "Returns ``True`` if the Alt key was down at the time of the event.", "");

    DocDeclStr(
        bool , ShiftDown() const,
        "Returns ``True`` if the Shift key was down at the time of the event.", "");


    DocDeclStr(
        bool , CmdDown() const,
        "\"Cmd\" is a pseudo key which is the same as Control for PC and Unix
platforms but the special \"Apple\" (a.k.a as \"Command\") key on
Macs. It makes often sense to use it instead of, say, `ControlDown`
because Cmd key is used for the same thing under Mac as Ctrl
elsewhere. The Ctrl still exists, it's just not used for this
purpose. So for non-Mac platforms this is the same as `ControlDown`
and Macs this is the same as `MetaDown`.", "");



    DocDeclStr(
        bool , HasModifiers() const,
        "Returns true if either CTRL or ALT keys was down at the time of the
key event. Note that this function does not take into account neither
SHIFT nor META key states (the reason for ignoring the latter is that
it is common for NUMLOCK key to be configured as META under X but the
key presses even while NUMLOCK is on should be still processed
normally).", "");


    DocDeclStr(
        int , GetKeyCode() const,
        "Returns the virtual key code. ASCII events return normal ASCII values,
while non-ASCII events return values such as WXK_LEFT for the left
cursor key. See `wx.KeyEvent` for a full list of the virtual key
codes.

Note that in Unicode build, the returned value is meaningful only if
the user entered a character that can be represented in current
locale's default charset. You can obtain the corresponding Unicode
character using `GetUnicodeKey`.", "");
    
//    %pythoncode { KeyCode = GetKeyCode }  this will be hidden by the property


    %extend {
        DocStr(
            GetUnicodeKey,
            "Returns the Unicode character corresponding to this key event.  This
function is only meaningfule in a Unicode build of wxPython.", "");
        int GetUnicodeKey() {
        %#if wxUSE_UNICODE
            return self->GetUnicodeKey();
        %#else
            return 0;
        %#endif
        }
    }
    %pythoncode { GetUniChar = GetUnicodeKey }

    %extend {
        DocStr(
            SetUnicodeKey,
            "Set the Unicode value of the key event, but only if this is a Unicode
build of wxPython.", "");
        void SetUnicodeKey(int uniChar) {
            %#if wxUSE_UNICODE
                self->m_uniChar = uniChar;
            %#endif
        }
    }

    DocDeclStr(
        wxUint32 , GetRawKeyCode() const,
        "Returns the raw key code for this event. This is a platform-dependent
scan code which should only be used in advanced
applications. Currently the raw key codes are not supported by all
ports.", "");


    DocDeclStr(
        wxUint32 , GetRawKeyFlags() const,
        "Returns the low level key flags for this event. The flags are
platform-dependent and should only be used in advanced applications.
Currently the raw key flags are not supported by all ports.", "");



    DocStr(GetPosition,   // sets the docstring for both
           "Find the position of the event, if applicable.", "");
    wxPoint GetPosition();

    DocDeclAName(
        void, GetPosition(long *OUTPUT, long *OUTPUT),
        "GetPositionTuple() -> (x,y)",
        GetPositionTuple);


    DocDeclStr(
        wxCoord , GetX() const,
        "Returns the X position (in client coordinates) of the event, if
applicable.", "");


    DocDeclStr(
        wxCoord , GetY() const,
        "Returns the Y position (in client coordinates) of the event, if
applicable.", "");


public:
    wxCoord       m_x, m_y;

    long          m_keyCode;

    bool          m_controlDown;
    bool          m_shiftDown;
    bool          m_altDown;
    bool          m_metaDown;
    bool          m_scanCode;

    wxUint32      m_rawCode;
    wxUint32      m_rawFlags;

    %property(KeyCode, GetKeyCode, doc="See `GetKeyCode`");
    %property(Modifiers, GetModifiers, doc="See `GetModifiers`");
    %property(Position, GetPosition, doc="See `GetPosition`");
    %property(RawKeyCode, GetRawKeyCode, doc="See `GetRawKeyCode`");
    %property(RawKeyFlags, GetRawKeyFlags, doc="See `GetRawKeyFlags`");
    %property(UnicodeKey, GetUnicodeKey, SetUnicodeKey, doc="See `GetUnicodeKey` and `SetUnicodeKey`");
    %property(X, GetX, doc="See `GetX`");
    %property(Y, GetY, doc="See `GetY`");
};

//---------------------------------------------------------------------------
%newgroup;

// Size event class

DocStr(wxSizeEvent,
"A size event holds information about size change events.  The EVT_SIZE
handler function will be called when the window it is bound to has
been resized.

Note that the size passed is of the whole window: call
`wx.Window.GetClientSize` for the area which may be used by the
application.

When a window is resized, usually only a small part of the window is
damaged and and that area is all that is in the update region for the
next paint event. However, if your drawing depends on the size of the
window, you may need to clear the DC explicitly and repaint the whole
window. In which case, you may need to call `wx.Window.Refresh` to
invalidate the entire window.
", "");

class wxSizeEvent : public wxEvent
{
public:
    DocCtorStr(
        wxSizeEvent(const wxSize& sz=wxDefaultSize, int winid = 0),
        "Construct a new ``wx.SizeEvent``.", "");

    DocDeclStr(
        wxSize , GetSize() const,
        "Returns the entire size of the window generating the size change
event.", "");

    wxRect GetRect() const;
    void SetRect(wxRect rect);

    %extend {
        void SetSize(wxSize size) {
            self->m_size = size;
        }
    }

    wxSize m_size;
    wxRect m_rect;
    
    %property(Rect, GetRect, SetRect, doc="See `GetRect` and `SetRect`");
    %property(Size, GetSize, SetSize, doc="See `GetSize` and `SetSize`");
};


//---------------------------------------------------------------------------
%newgroup;

DocStr(wxMoveEvent,
"This event object is sent for EVT_MOVE event bindings when a window is
moved to a new position.", "");

class wxMoveEvent : public wxEvent
{
public:
    DocCtorStr(
        wxMoveEvent(const wxPoint& pos=wxDefaultPosition, int winid = 0),
        "Constructor.", "");

    DocDeclStr(
        wxPoint , GetPosition() const,
        "Returns the position of the window generating the move change event.", "");

    wxRect GetRect() const;
    void SetRect(const wxRect& rect);
    void SetPosition(const wxPoint& pos);

    %pythoncode {
        m_pos =  property(GetPosition, SetPosition)
        m_rect = property(GetRect, SetRect)
    }

    %property(Position, GetPosition, SetPosition, doc="See `GetPosition` and `SetPosition`");
    %property(Rect, GetRect, SetRect, doc="See `GetRect` and `SetRect`");
    
};

//---------------------------------------------------------------------------
%newgroup;

DocStr(wxPaintEvent,
"A paint event is sent when a window's contents needs to be repainted.
Note that in an EVT_PAINT handler the application must *always* create
a `wx.PaintDC` object, even if you do not use it.  Otherwise MS
Windows assumes that the window has not been painted yet and will send
the event again, causing endless refreshes.

You can optimize painting by retrieving the rectangles that have been
damaged using `wx.Window.GetUpdateRegion` and/or `wx.RegionIterator`,
and only repainting these rectangles. The rectangles are in terms of
the client area, and are unscrolled, so you will need to do some
calculations using the current view position to obtain logical,
scrolled units.
", "");

class wxPaintEvent : public wxEvent
{
public:
    DocCtorStr(
        wxPaintEvent(int Id = 0),
        "", "");
};


class wxNcPaintEvent : public wxEvent
{
public:
    wxNcPaintEvent(int winid = 0);
};

//---------------------------------------------------------------------------
%newgroup;

DocStr(wxEraseEvent,
"An erase event is sent whenever the background of a window needs to be
repainted.  To intercept this event use the EVT_ERASE_BACKGROUND event
binder.  On some platforms, such as GTK+, this event is simulated
(simply generated just before the paint event) and may cause flicker.

To paint a custom background use the `GetDC` method and use the returned
device context if it is not ``None``, otherwise create a temporary
`wx.ClientDC` and draw on that.
", "");

class wxEraseEvent : public wxEvent
{
public:
    DocCtorStr(
        wxEraseEvent(int Id = 0, wxDC *dc = NULL),
        "Constructor", "");

    DocDeclStr(
        wxDC *, GetDC() const,
        "Returns the device context the event handler should draw upon.  If
``None`` is returned then create a temporary `wx.ClientDC` and use
that instead.", "");

    %property(DC, GetDC, doc="See `GetDC`"); 
};


//---------------------------------------------------------------------------
%newgroup;

DocStr(wxFocusEvent,
"A focus event is sent when a window's focus changes. The window losing
focus receives an EVT_KILL_FOCUS event while the window gaining it
gets an EVT_SET_FOCUS event.

Notice that the set focus event happens both when the user gives focus
to the window (whether using the mouse or keyboard) and when it is
done from the program itself using `wx.Window.SetFocus`.
", "");

class wxFocusEvent : public wxEvent
{
public:
    DocCtorStr(
        wxFocusEvent(wxEventType type = wxEVT_NULL, int winid = 0),
        "Constructor", "");

    DocDeclStr(
        wxWindow *, GetWindow() const,
        "Returns the other window associated with this event, that is the
window which had the focus before for the EVT_SET_FOCUS event and the
window which is going to receive focus for the wxEVT_KILL_FOCUS event.

Warning: the window returned may be None!", "");

    void SetWindow(wxWindow *win);

    %property(Window, GetWindow, SetWindow, doc="See `GetWindow` and `SetWindow`");
};

//---------------------------------------------------------------------------
%newgroup;

DocStr(wxChildFocusEvent,
"A child focus event is sent to a (parent-)window when one of its child
windows gains focus, so that the window could restore the focus back
to its corresponding child if it loses it now and regains later.

Notice that child window is the direct child of the window receiving
the event, and so may not be the actual widget recieving focus if it
is further down the containment heirarchy.  Use `wx.Window.FindFocus`
to get the widget that is actually receiving focus.", "");


class wxChildFocusEvent : public wxCommandEvent
{
public:
    DocCtorStr(
        wxChildFocusEvent(wxWindow *win = NULL),
        "Constructor", "");

    DocDeclStr(
        wxWindow *, GetWindow() const,
        "The window, or (grand)parent of the window which has just received the
focus.", "");

    %property(Window, GetWindow, doc="See `GetWindow`");
};

//---------------------------------------------------------------------------
%newgroup;

DocStr(wxActivateEvent,
"An activate event is sent when a top-level window or the entire
application is being activated or deactivated.

A top-level window (a dialog or frame) receives an activate event when
is being activated or deactivated. This is indicated visually by the
title bar changing colour, and a subwindow gaining the keyboard focus.
An application is activated or deactivated when one of its frames
becomes activated, or a frame becomes inactivate resulting in all
application frames being inactive.

Please note that usually you should call event.Skip() in your handlers
for these events so the default handlers will still be called, as not
doing so can result in strange effects.
", "

Events
-------
    =======================     ==========================================
    wx.EVT_ACTIVATE             Sent to a frame when it has been activated
                                or deactivated.
    wx.EVT_ACTIVATE_APP         Sent to the app object when its activation
                                state changes.
    =======================     ==========================================
");


class wxActivateEvent : public wxEvent
{
public:
    DocCtorStr(
        wxActivateEvent(wxEventType type = wxEVT_NULL, bool active = true, int Id = 0),
        "Constructor", "");

    DocDeclStr(
        bool , GetActive() const,
        "Returns true if the application or window is being activated, false
otherwise.", "");

    %property(Active, GetActive, doc="See `GetActive`");
};


//---------------------------------------------------------------------------
%newgroup;

DocStr(wxInitDialogEvent,
"A wx.InitDialogEvent is sent as a dialog is being initialised, or for
any window when `wx.Window.InitDialog` is called.  Handlers for this
event can transfer data to the window, or anything else that should be
done before the user begins editing the form. The default handler
calls `wx.Window.TransferDataToWindow`.", "

Events
-------
    =======================     ==========================================
    wx.EVT_INIT_DIALOG          Binder for init dialog events.
    =======================     ==========================================
");

class wxInitDialogEvent : public wxEvent
{
public:
    DocCtorStr(
        wxInitDialogEvent(int Id = 0),
        "Constructor", "");
};


//---------------------------------------------------------------------------
%newgroup;

DocStr(wxMenuEvent,
"This class is used for a variety of menu-related events.  Note that
these do not include menu command events, which are handled by sending
`wx.CommandEvent` objects.

The default handler for wx.EVT_MENU_HIGHLIGHT displays menu item help
text in the first field of the status bar.", "

Events
-------
    =========================   ==========================================
    wx.EVT_MENU_OPEN            A menu is about to be opened. On Windows,
                                this is only sent once for each navigation
                                of the menubar (up until all menus have closed).
    wx.EVT_MENU_CLOSE           A menu has been just closed.
    wx.EVT_MENU_HIGHLIGHT       Sent when the menu item with the specified id
                                has been highlighted: used by default to show
                                help prompts in the `wx.Frame`'s status bar.
    wx.EVT_MENU_HIGHLIGHT_ALL   Can be used to catch the menu highlight
                                event for all menu items.
    =========================   ==========================================
");

class wxMenuEvent : public wxEvent
{
public:
    DocCtorStr(
        wxMenuEvent(wxEventType type = wxEVT_NULL, int winid = 0, wxMenu* menu = NULL),
        "Constructor", "");

    DocDeclStr(
        int , GetMenuId() const,
        "Returns the menu identifier associated with the event. This method
should be only used with the HIGHLIGHT events.", "");


    DocDeclStr(
        bool , IsPopup() const,
        "Returns ``True`` if the menu which is being opened or closed is a
popup menu, ``False`` if it is a normal one.  This method should only
be used with the OPEN and CLOSE events.", "");


    DocDeclStr(
        wxMenu* , GetMenu() const,
        "Returns the menu which is being opened or closed. This method should
only be used with the OPEN and CLOSE events.", "");

    %property(Menu, GetMenu, doc="See `GetMenu`");
    %property(MenuId, GetMenuId, doc="See `GetMenuId`");
};

//---------------------------------------------------------------------------
%newgroup;

DocStr(wxCloseEvent,
"This event class contains information about window and session close
events.

The handler function for EVT_CLOSE is called when the user has tried
to close a a frame or dialog box using the window manager controls or
the system menu. It can also be invoked by the application itself
programmatically, for example by calling the `wx.Window.Close`
function.

You should check whether the application is forcing the deletion of
the window using `CanVeto`. If it returns ``False``, you must destroy
the window using `wx.Window.Destroy`. If the return value is ``True``,
it is up to you whether you respond by destroying the window or not.
For example you may wish to display a message dialog prompting to save
files or to cancel the close.

If you don't destroy the window, you should call `Veto` to let the
calling code know that you did not destroy the window. This allows the
`wx.Window.Close` function to return ``True`` or ``False`` depending
on whether the close instruction was honored or not.", "

Events
-------
    =========================   ==========================================
    wx.EVT_CLOSE                This event is sent to a top-level window
                                when it has been requested to close either
                                from the user or programatically.
    wx.EVT_QUERY_END_SESSION    This event is sent to the `wx.App` when the
                                system is about to be logged off, giving the
                                app a chance to veto the shutdown.
    wx.EVT_END_SESSION          This event is sent to the `wx.App` when the
                                system is being logged off.
    =========================   ==========================================
");

class wxCloseEvent : public wxEvent
{
public:
    DocCtorStr(
        wxCloseEvent(wxEventType type = wxEVT_NULL, int winid = 0),
        "Constructor.", "");

    DocDeclStr(
        void , SetLoggingOff(bool logOff),
        "Sets the 'logging off' flag.", "");

    DocDeclStr(
        bool , GetLoggingOff() const,
        "Returns ``True`` if the user is logging off or ``False`` if the
system is shutting down. This method can only be called for end
session and query end session events, it doesn't make sense for close
window event.", "");


    DocDeclStr(
        void , Veto(bool veto = true),
        "Call this from your event handler to veto a system shutdown or to
signal to the calling application that a window close did not happen.

You can only veto a shutdown or close if `CanVeto` returns true.", "");

    bool GetVeto() const;

    DocDeclStr(
        void , SetCanVeto(bool canVeto),
        "Sets the 'can veto' flag.", "");

    DocDeclStr(
        bool , CanVeto() const,
        "Returns true if you can veto a system shutdown or a window close
event. Vetoing a window close event is not possible if the calling
code wishes to force the application to exit, and so this function
must be called to check this.", "");

    %property(LoggingOff, GetLoggingOff, SetLoggingOff, doc="See `GetLoggingOff` and `SetLoggingOff`");
};


//---------------------------------------------------------------------------
%newgroup;

DocStr(wxShowEvent,
"An EVT_SHOW event is sent when a window is shown or hidden.", "");

class wxShowEvent : public wxEvent
{
public:
    wxShowEvent(int winid = 0, bool show = false);

    void SetShow(bool show);
    bool GetShow() const;

    %property(Show, GetShow, SetShow, doc="See `GetShow` and `SetShow`");
};


//---------------------------------------------------------------------------
%newgroup;

DocStr(wxIconizeEvent,
"An EVT_ICONIZE event is sent when a frame is iconized (minimized) or
restored.", "");

class wxIconizeEvent: public wxEvent
{
public:
    wxIconizeEvent(int id = 0, bool iconized = true);

    DocDeclStr(
        bool , Iconized(),
        "Returns ``True`` if the frame has been iconized, ``False`` if it has
been restored.", "");

};


//---------------------------------------------------------------------------
%newgroup;

DocStr(wxMaximizeEvent,
"An EVT_MAXIMIZE event is sent when a frame is maximized or restored.", "");

class wxMaximizeEvent: public wxEvent
{
public:
    wxMaximizeEvent(int id = 0);
};

//---------------------------------------------------------------------------
%newgroup;

DocStr(wxDropFilesEvent,
"This class is used for drop files events, that is, when files have
been dropped onto the window. This functionality is only available
under Windows. The window must have previously been enabled for
dropping by calling `wx.Window.DragAcceptFiles`.

Important note: this is a separate implementation to the more general
drag and drop implementation using `wx.FileDropTarget`, and etc. This
implementation uses the older, Windows message-based approach of
dropping files.

Use wx.EVT_DROP_FILES to bind an event handler to receive file drop
events.
", "");

class wxDropFilesEvent: public wxEvent
{
public:

    // TODO:  wrap the ctor!


    DocDeclStr(
        wxPoint , GetPosition(),
        "Returns the position at which the files were dropped.", "");

    DocDeclStr(
        int , GetNumberOfFiles(),
        "Returns the number of files dropped.", "");


    %extend {
        DocStr(
            GetFiles,
            "Returns a list of the filenames that were dropped.", "");

        PyObject* GetFiles() {
            int         count = self->GetNumberOfFiles();
            wxString*   files = self->GetFiles();
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject*   list  = PyList_New(count);

            if (!list) {
                PyErr_SetString(PyExc_MemoryError, "Can't allocate list of files!");
                wxPyEndBlockThreads(blocked);
                return NULL;
            }

            for (int i=0; i<count; i++) {
                PyList_SetItem(list, i, wx2PyString(files[i]));
            }
            wxPyEndBlockThreads(blocked);
            return list;
        }
    }

    %property(Files, GetFiles, doc="See `GetFiles`");
    %property(NumberOfFiles, GetNumberOfFiles, doc="See `GetNumberOfFiles`");
    %property(Position, GetPosition, doc="See `GetPosition`");
};



//---------------------------------------------------------------------------
%newgroup;


enum wxUpdateUIMode
{
    wxUPDATE_UI_PROCESS_ALL,
    wxUPDATE_UI_PROCESS_SPECIFIED
};


DocStr(wxUpdateUIEvent,
"This class is used for EVT_UPDATE_UI pseudo-events which are sent by
wxWidgets to give an application the chance to update various user
interface elements.

Without update UI events, an application has to work hard to
check/uncheck, enable/disable, and set the text for elements such as
menu items and toolbar buttons. The code for doing this has to be
mixed up with the code that is invoked when an action is invoked for a
menu item or button.

With update UI events, you define an event handler to look at the
state of the application and change UI elements accordingly. wxWidgets
will call your handler functions in idle time, so you don't have to
worry where to call this code. In addition to being a clearer and more
declarative method, it also means you don't have to worry whether
you're updating a toolbar or menubar identifier. The same handler can
update a menu item and toolbar button, if the ID values are the same.

Instead of directly manipulating the menu or button, you call
functions in the event object, such as `Check`. wxWidgets will
determine whether such a call has been made, and which UI element to
update.

These events will work for popup menus as well as menubars. Just
before a menu is popped up, `wx.Menu.UpdateUI` is called to process
any UI events for the window that owns the menu.

If you find that the overhead of UI update processing is affecting
your application, you can do one or both of the following:

   1. Call `wx.UpdateUIEvent.SetMode` with a value of
      wx.UPDATE_UI_PROCESS_SPECIFIED, and set the extra style
      wx.WS_EX_PROCESS_UPDATE_EVENTS for every window that should
      receive update events. No other windows will receive update
      events.

   2. Call `wx.UpdateUIEvent.SetUpdateInterval` with a millisecond
      value to set the delay between updates. You may need to call
      `wx.Window.UpdateWindowUI` at critical points, for example when
      a dialog is about to be shown, in case the user sees a slight
      delay before windows are updated.

Note that although events are sent in idle time, defining a EVT_IDLE
handler for a window does not affect this because the events are sent
from an internal idle handler.

wxWidgets tries to optimize update events on some platforms. On
Windows and GTK+, events for menubar items are only sent when the menu
is about to be shown, and not in idle time.
", "");

class wxUpdateUIEvent : public wxCommandEvent
{
public:
    DocCtorStr(
        wxUpdateUIEvent(wxWindowID commandId = 0),
        "Constructor", "");

    DocDeclStr(
        bool , GetChecked() const,
        "Returns ``True`` if the UI element should be checked.", "");

    DocDeclStr(
        bool , GetEnabled() const,
        "Returns ``True`` if the UI element should be enabled.", "");

    DocDeclStr(
        bool , GetShown() const,
        "Returns ``True`` if the UI element should be shown.", "");

    DocDeclStr(
        wxString , GetText() const,
        "Returns the text that should be set for the UI element.", "");

    DocDeclStr(
        bool , GetSetText() const,
        "Returns ``True`` if the application has called `SetText`. For
wxWidgets internal use only.", "");

    DocDeclStr(
        bool , GetSetChecked() const,
        "Returns ``True`` if the application has called `Check`. For wxWidgets
internal use only.", "");

    DocDeclStr(
        bool , GetSetEnabled() const,
        "Returns ``True`` if the application has called `Enable`. For wxWidgets
internal use only.", "");

    DocDeclStr(
        bool , GetSetShown() const,
        "Returns ``True`` if the application has called `Show`. For wxWidgets
internal use only.", "");


    DocDeclStr(
        void , Check(bool check),
        "Check or uncheck the UI element.", "");

    DocDeclStr(
        void , Enable(bool enable),
        "Enable or disable the UI element.", "");

    DocDeclStr(
        void , Show(bool show),
        "Show or hide the UI element.", "");


    DocDeclStr(
        void , SetText(const wxString& text),
        "Sets the text for this UI element.", "");



    DocDeclStr(
        static void , SetUpdateInterval(long updateInterval),
        "Sets the interval between updates in milliseconds. Set to -1 to
disable updates, or to 0 to update as frequently as possible. The
default is 0.

Use this to reduce the overhead of UI update events if your
application has a lot of windows. If you set the value to -1 or
greater than 0, you may also need to call `wx.Window.UpdateWindowUI`
at appropriate points in your application, such as when a dialog is
about to be shown.", "");


    DocDeclStr(
        static long , GetUpdateInterval(),
        "Returns the current interval between updates in milliseconds. -1
disables updates, 0 updates as frequently as possible.", "");


    DocDeclStr(
        static bool , CanUpdate(wxWindow *win),
        "Returns ``True`` if it is appropriate to update (send UI update events
to) this window.

This function looks at the mode used (see `wx.UpdateUIEvent.SetMode`),
the wx.WS_EX_PROCESS_UPDATE_EVENTS flag in window, the time update
events were last sent in idle time, and the update interval, to
determine whether events should be sent to this window now. By default
this will always return true because the update mode is initially
wx.UPDATE_UI_PROCESS_ALL and the interval is set to 0; so update
events will be sent as often as possible. You can reduce the frequency
that events are sent by changing the mode and/or setting an update
interval.
", "");


    DocDeclStr(
        static void , ResetUpdateTime(),
        "Used internally to reset the last-updated time to the current time. It
is assumed that update events are normally sent in idle time, so this
is called at the end of idle processing.", "");


    DocDeclStr(
        static void , SetMode(wxUpdateUIMode mode),
        "Specify how wxWidgets will send update events: to all windows, or only
to those which specify that they will process the events.

The mode may be one of the following values:

    =============================   ==========================================
    wxUPDATE_UI_PROCESS_ALL         Send UI update events to all windows.  This
                                    is the default setting.
    wxUPDATE_UI_PROCESS_SPECIFIED   Send UI update events only to windows that
                                    have the wx.WS_EX_PROCESS_UI_UPDATES extra
                                    style set.
    =============================   ==========================================
", "");


    DocDeclStr(
        static wxUpdateUIMode , GetMode(),
        "Returns a value specifying how wxWidgets will send update events: to
all windows, or only to those which specify that they will process the
events.", "");

    %property(Checked, GetChecked, Check, doc="See `GetChecked`");
    %property(Enabled, GetEnabled, Enable, doc="See `GetEnabled`");
    %property(Shown, GetShown, Show, doc="See `GetShown`");
    %property(Text, GetText, SetText, doc="See `GetText` and `SetText`");
};

//---------------------------------------------------------------------------
%newgroup;

DocStr(wxSysColourChangedEvent,
"This class is used for EVT_SYS_COLOUR_CHANGED, which are generated
when the user changes the colour settings using the control
panel. This is only applicable under Windows.

The default event handler for this event propagates the event to child
windows, since Windows only sends the events to top-level windows. If
intercepting this event for a top-level window, remember to call
`Skip` so the the base class handler will still be executed, or to
pass the event on to the window's children explicitly.
", "");

class wxSysColourChangedEvent: public wxEvent {
public:
    DocCtorStr(
        wxSysColourChangedEvent(),
        "Constructor", "");
};

//---------------------------------------------------------------------------
%newgroup;


DocStr(wxMouseCaptureChangedEvent,
"An mouse capture changed event (EVT_MOUSE_CAPTURE_CHANGED) is sent to
a window that loses its mouse capture. This is called even if
`wx.Window.ReleaseMouse` was called by the application code. Handling
this event allows an application to cater for unexpected capture
releases which might otherwise confuse mouse handling code.

This event is implemented under Windows only.", "");

class wxMouseCaptureChangedEvent : public wxEvent
{
public:
    DocCtorStr(
        wxMouseCaptureChangedEvent(wxWindowID winid = 0, wxWindow* gainedCapture = NULL),
        "Constructor", "");

    DocDeclStr(
        wxWindow* , GetCapturedWindow() const,
        "Returns the window that gained the capture, or ``None`` if it was a
non-wxWidgets window.", "");

    %property(CapturedWindow, GetCapturedWindow, doc="See `GetCapturedWindow`");
};

//---------------------------------------------------------------------------
%newgroup;

DocStr(wxMouseCaptureLostEvent,
"A mouse capture lost event is sent to a window that obtained mouse
capture, which was subsequently loss due to \"external\" event, for
example when a dialog box is shown or if another application captures
the mouse.

If this happens, this event is sent to all windows that are on the
capture stack (i.e. a window that called `wx.Window.CaptureMouse`, but
didn't call `wx.Window.ReleaseMouse` yet). The event is *not* sent
if the capture changes because of a call to CaptureMouse or
ReleaseMouse.

This event is currently emitted under Windows only.
", "");

class wxMouseCaptureLostEvent : public wxEvent
{
public:
    wxMouseCaptureLostEvent(wxWindowID winid = 0);
};

//---------------------------------------------------------------------------
%newgroup;

DocStr(wxDisplayChangedEvent,
"An EVT_DISPLAY_CHANGED event is sent to all windows when the display
resolution has changed.

This event is implemented under Windows only.", "");

class wxDisplayChangedEvent : public wxEvent
{
public:
    DocCtorStr(
        wxDisplayChangedEvent(),
        "", "");
};

//---------------------------------------------------------------------------
%newgroup;

DocStr(wxPaletteChangedEvent,
"An EVT_PALETTE_CHANGED event is sent when the system palette has
changed, thereby giving each window a chance to redo their own to
match.

This event is implemented under Windows only.", "");

class  wxPaletteChangedEvent : public wxEvent
{
public:
    wxPaletteChangedEvent(wxWindowID id = 0);

    void SetChangedWindow(wxWindow* win);
    wxWindow* GetChangedWindow();

    %property(ChangedWindow, GetChangedWindow, SetChangedWindow, doc="See `GetChangedWindow` and `SetChangedWindow`");
};

//---------------------------------------------------------------------------
%newgroup;

DocStr(wxQueryNewPaletteEvent,
       "An EVT_QUERY_NEW_PALETE event indicates the window is getting keyboard
focus and should re-do its palette.

This event is implemented under Windows only.", "");

class wxQueryNewPaletteEvent : public wxEvent
{
public:
    DocCtorStr(
        wxQueryNewPaletteEvent(wxWindowID winid = 0),
        "Constructor.", "");

    DocDeclStr(
        void , SetPaletteRealized(bool realized),
        "App should set this if it changes the palette.", "");

    bool GetPaletteRealized() const;

    %property(PaletteRealized, GetPaletteRealized, SetPaletteRealized, doc="See `GetPaletteRealized` and `SetPaletteRealized`");    
};

//---------------------------------------------------------------------------
%newgroup;



DocStr(wxNavigationKeyEvent,
"EVT_NAVIGATION_KEY events are used to control moving the focus between
widgets, otherwise known as tab-traversal.  You woudl normally not
catch navigation events in applications as there are already
appropriate handlers in `wx.Dialog` and `wx.Panel`, but you may find
it useful to send navigation events in certain situations to change
the focus in certain ways, although it's probably easier to just call
`wx.Window.Navigate`.", "");

class wxNavigationKeyEvent : public wxEvent
{
public:
    DocCtorStr(
        wxNavigationKeyEvent(),
        "", "");

    DocDeclStr(
        bool , GetDirection() const,
        "Returns ``True`` if the direction is forward, ``False`` otherwise.", "");

    DocDeclStr(
        void , SetDirection(bool forward),
        "Specify the direction that the navigation should take.  Usually the
difference between using Tab and Shift-Tab.", "");


    DocDeclStr(
        bool , IsWindowChange() const,
        "Returns ``True`` if window change is allowed.", "");

    DocDeclStr(
        void , SetWindowChange(bool ischange),
        "Specify if the navigation should be able to change parent windows.
For example, changing notebook pages, etc. This is usually implemented
by using Control-Tab.", "");


    DocDeclStr(
        bool , IsFromTab() const,
        "Returns ``True`` if the navigation event is originated from the Tab
key.", "");

    DocDeclStr(
        void , SetFromTab(bool bIs),
        "Set to true under MSW if the event was generated using the tab key.
This is required for proper navogation over radio buttons.", "");


    DocDeclStr(
        void , SetFlags(long flags),
        "Set the navigation flags to a combination of the following:

    * wx.NavigationKeyEvent.IsBackward
    * wx.NavigationKeyEvent.IsForward
    * wx.NavigationKeyEvent.WinChange
    * wx.NavigationKeyEvent.FromTab
", "");


    DocDeclStr(
        wxWindow* , GetCurrentFocus() const,
        "Returns the child window which currenty has the focus.  May be
``None``.", "");

    DocDeclStr(
        void , SetCurrentFocus(wxWindow *win),
        "Set the window that has the focus.", "");


    enum {
        IsBackward,
        IsForward,
        WinChange,
        FromTab
    };

    %property(CurrentFocus, GetCurrentFocus, SetCurrentFocus, doc="See `GetCurrentFocus` and `SetCurrentFocus`");
    %property(Direction, GetDirection, SetDirection, doc="See `GetDirection` and `SetDirection`");
    
};

//---------------------------------------------------------------------------
%newgroup;


DocStr(wxWindowCreateEvent,
       "The EVT_WINDOW_CREATE event is sent as soon as the window object (the
underlying GUI object) exists.", "");

class wxWindowCreateEvent : public wxCommandEvent
{
public:
    wxWindowCreateEvent(wxWindow *win = NULL);

    DocDeclStr(
        wxWindow *, GetWindow() const,
        "Returns the window that this event refers to.", "");

    %property(Window, GetWindow, doc="See `GetWindow`");
};


DocStr(wxWindowDestroyEvent,
       "The EVT_WINDOW_DESTROY event is sent from the `wx.Window` destructor
when the GUI window is destroyed.

When a class derived from `wx.Window` is destroyed its destructor will
have already run by the time this event is sent. Therefore this event
will not usually be received at all by the window itself.  Since it is
received after the destructor has run, an object should not try to
handle its own wx.WindowDestroyEvent, but it can be used to get
notification of the destruction of another window.", "");
class wxWindowDestroyEvent : public wxCommandEvent
{
public:
    wxWindowDestroyEvent(wxWindow *win = NULL);

    DocDeclStr(
        wxWindow *, GetWindow() const,
        "Returns the window that this event refers to.", "");

    %property(Window, GetWindow, doc="See `GetWindow`");
};


//---------------------------------------------------------------------------
%newgroup;


DocStr(wxContextMenuEvent,
"This class is used for context menu events (EVT_CONTECT_MENU,) sent to
give the application a chance to show a context (popup) menu.", "");

class wxContextMenuEvent : public wxCommandEvent
{
public:
    DocCtorStr(
        wxContextMenuEvent(wxEventType type = wxEVT_NULL,
                           wxWindowID winid = 0,
                           const wxPoint& pt = wxDefaultPosition),
        "Constructor.", "");

    // Position of event (in screen coordinates)
    DocDeclStr(
        const wxPoint& , GetPosition() const,
        "Returns the position (in screen coordinants) at which the menu should
be shown.", "");


    DocDeclStr(
        void , SetPosition(const wxPoint& pos),
        "Sets the position at which the menu should be shown.", "");

    %property(Position, GetPosition, SetPosition, doc="See `GetPosition` and `SetPosition`");
};

//---------------------------------------------------------------------------
%newgroup;

enum wxIdleMode
{
    wxIDLE_PROCESS_ALL,
    wxIDLE_PROCESS_SPECIFIED
};


DocStr(wxIdleEvent,
"This class is used for EVT_IDLE events, which are generated and sent
when the application *becomes* idle.  In other words, the when the
event queue becomes empty then idle events are sent to all windows (by
default) and as long as none of them call `RequestMore` then there are
no more idle events until after the system event queue has some normal
events and then becomes empty again.

By default, idle events are sent to all windows. If this is causing a
significant overhead in your application, you can call
`wx.IdleEvent.SetMode` with the value wx.IDLE_PROCESS_SPECIFIED, and
set the wx.WS_EX_PROCESS_IDLE extra window style for every window
which should receive idle events.  Then idle events will only be sent
to those windows and not to any others.", "");

class wxIdleEvent : public wxEvent
{
public:
    DocCtorStr(
        wxIdleEvent(),
        "Constructor", "");


    DocDeclStr(
        void , RequestMore(bool needMore = true),
        "Tells wxWidgets that more processing is required. This function can be
called by an EVT_IDLE handler for a window to indicate that the
application should forward the EVT_IDLE event once more to the
application windows. If no window calls this function during its
EVT_IDLE handler, then the application will remain in a passive event
loop until a new event is posted to the application by the windowing
system.", "");

    DocDeclStr(
        bool , MoreRequested() const,
        "Returns ``True`` if the OnIdle function processing this event
requested more processing time.", "");


    DocDeclStr(
        static void , SetMode(wxIdleMode mode),
        "Static method for specifying how wxWidgets will send idle events: to
all windows, or only to those which specify that they will process the
events.

The mode can be one of the following values:

    =========================   ========================================
    wx.IDLE_PROCESS_ALL         Send idle events to all windows
    wx.IDLE_PROCESS_SPECIFIED   Send idle events only to windows that have
                                the wx.WS_EX_PROCESS_IDLE extra style
                                flag set.
    =========================   ========================================
", "");


    DocDeclStr(
        static wxIdleMode , GetMode(),
        "Static method returning a value specifying how wxWidgets will send
idle events: to all windows, or only to those which specify that they
will process the events.", "");


    DocDeclStr(
        static bool , CanSend(wxWindow* win),
        "Returns ``True`` if it is appropriate to send idle events to this
window.

This function looks at the mode used (see `wx.IdleEvent.SetMode`), and
the wx.WS_EX_PROCESS_IDLE style in window to determine whether idle
events should be sent to this window now. By default this will always
return ``True`` because the update mode is initially
wx.IDLE_PROCESS_ALL. You can change the mode to only send idle events
to windows with the wx.WS_EX_PROCESS_IDLE extra window style set.", "");

};

//---------------------------------------------------------------------------
%newgroup;


DocStr(wxClipboardTextEvent,
"A Clipboard Text event is sent when a window intercepts a text
copy/cut/paste message, i.e. the user has cut/copied/pasted data
from/into a text control via ctrl-C/X/V, ctrl/shift-del/insert, a
popup menu command, etc.  NOTE : under windows these events are *NOT*
generated automatically for a Rich Edit text control.", "");

class wxClipboardTextEvent : public wxCommandEvent
{
public:
    wxClipboardTextEvent(wxEventType type = wxEVT_NULL,
                         wxWindowID winid = 0);
};


//---------------------------------------------------------------------------
%newgroup;

// These classes can be derived from in Python and passed through the event
// system without loosing anything.  They do this by keeping a reference to
// themselves and some special case handling in wxPyCallback::EventThunker.

DocStr(wxPyEvent,
"wx.PyEvent can be used as a base class for implementing custom event
types in Python.  You should derived from this class instead of
`wx.Event` because this class is Python-aware and is able to transport
its Python bits safely through the wxWidgets event system and have
them still be there when the event handler is invoked.

:see: `wx.PyCommandEvent`
", "");

class wxPyEvent : public wxEvent {
public:
    %pythonAppend wxPyEvent     "self._SetSelf(self)"
    DocCtorStr(
        wxPyEvent(int winid=0, wxEventType eventType = wxEVT_NULL ),
        "", "");

    ~wxPyEvent();


    %Rename(_SetSelf, void , SetSelf(PyObject* self));
    %Rename(_GetSelf, PyObject* , GetSelf());
};



DocStr(wxPyCommandEvent,
"wx.PyCommandEvent can be used as a base class for implementing custom
event types in Python, where the event shoudl travel up to parent
windows looking for a handler.  You should derived from this class
instead of `wx.CommandEvent` because this class is Python-aware and is
able to transport its Python bits safely through the wxWidgets event
system and have them still be there when the event handler is invoked.

:see: `wx.PyEvent`
", "");

class wxPyCommandEvent : public wxCommandEvent {
public:
    %pythonAppend wxPyCommandEvent     "self._SetSelf(self)"

    DocCtorStr(
        wxPyCommandEvent(wxEventType eventType = wxEVT_NULL, int id=0),
        "", "");

    ~wxPyCommandEvent();

    %Rename(_SetSelf, void , SetSelf(PyObject* self));
    %Rename(_GetSelf, PyObject* , GetSelf());
};



//---------------------------------------------------------------------------


DocStr(wxDateEvent,
"This event class holds information about a date change event and is
used together with `wx.DatePickerCtrl`. It also serves as a base class
for `wx.calendar.CalendarEvent`.  Bind these event types with
EVT_DATE_CHANGED.", "");

class wxDateEvent : public wxCommandEvent
{
public:
    DocCtorStr(
        wxDateEvent(wxWindow *win, const wxDateTime& dt, wxEventType type),
        "", "");

    DocDeclStr(
        const wxDateTime& , GetDate() const,
        "Returns the date.", "");

    DocDeclStr(
        void , SetDate(const wxDateTime &date),
        "Sets the date carried by the event, normally only used by the library
internally.", "");

    %property(Date, GetDate, SetDate, doc="See `GetDate` and `SetDate`");

};


%constant wxEventType wxEVT_DATE_CHANGED;

%pythoncode {
    EVT_DATE_CHANGED = wx.PyEventBinder( wxEVT_DATE_CHANGED, 1 )
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
