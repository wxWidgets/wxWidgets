/////////////////////////////////////////////////////////////////////////////
// Name:        wx/event.h
// Purpose:     Event classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_EVENTH__
#define _WX_EVENTH__

#ifdef __GNUG__
    #pragma interface "event.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"

#if wxUSE_GUI
    #include "wx/gdicmn.h"
    #include "wx/cursor.h"
#endif

#include "wx/thread.h"

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxList;

#if wxUSE_GUI
    class WXDLLEXPORT wxClientData;
    class WXDLLEXPORT wxDC;
    class WXDLLEXPORT wxMenu;
    class WXDLLEXPORT wxWindow;
#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// Event types
// ----------------------------------------------------------------------------

typedef int wxEventType;

// in previous versions of wxWindows the event types used to be constants
// which created difficulties with custom/user event types definition
//
// starting from wxWindows 2.4 the event types are now dynamically assigned
// using wxNewEventType() which solves this problem, however at price of
// several incompatibilities:
//
//  a) event table macros declaration changed, it now uses wxEventTableEntry
//     ctor instead of initialisation from an agregate - the macro
//     DECLARE_EVENT_TABLE_ENTRY may be used to write code which can compile
//     with all versions of wxWindows
//
//  b) event types can't be used as switch() cases as they're not really
//     constant any more - there is no magic solution here, you just have to
//     change the switch()es to if()s
//
// if these are real problems for you, define WXWIN_COMPATIBILITY_EVENT_TYPES
// to get 100% old behaviour, however you won't be able to use the libraries
// using the new dynamic event type allocation in such case, so avoid it if
// possible.

#if WXWIN_COMPATIBILITY_EVENT_TYPES

#define DECLARE_EVENT_TABLE_ENTRY(type, id, idLast, fn, obj) \
    { type, id, idLast, fn, obj }

#define BEGIN_DECLARE_EVENT_TYPES() enum {
#define END_DECLARE_EVENT_TYPES() };
#define DECLARE_EVENT_TYPE(name, value) name = wxEVT_FIRST + value,
#define DEFINE_EVENT_TYPE(name)

#else // !WXWIN_COMPATIBILITY_EVENT_TYPES

#define DECLARE_EVENT_TABLE_ENTRY(type, id, idLast, fn, obj) \
    wxEventTableEntry(type, id, idLast, fn, obj)

#define BEGIN_DECLARE_EVENT_TYPES()
#define END_DECLARE_EVENT_TYPES()
#define DECLARE_EVENT_TYPE(name, value) \
    extern const wxEventType WXDLLEXPORT name;
#define DEFINE_EVENT_TYPE(name) const wxEventType name = wxNewEventType();

// generate a new unique event type
extern WXDLLEXPORT wxEventType wxNewEventType();

#endif // WXWIN_COMPATIBILITY_EVENT_TYPES/!WXWIN_COMPATIBILITY_EVENT_TYPES

BEGIN_DECLARE_EVENT_TYPES()

#if WXWIN_COMPATIBILITY_EVENT_TYPES
    wxEVT_NULL = 0,
    wxEVT_FIRST = 10000,
    wxEVT_USER_FIRST = wxEVT_FIRST + 2000,
#else // !WXWIN_COMPATIBILITY_EVENT_TYPES
    // it is important to still have these as constants to avoid
    // initialization order related problems
    const wxEventType wxEVT_NULL = 0;
    const wxEventType wxEVT_FIRST = 10000;
    const wxEventType wxEVT_USER_FIRST = wxEVT_FIRST + 2000;
#endif // WXWIN_COMPATIBILITY_EVENT_TYPES/!WXWIN_COMPATIBILITY_EVENT_TYPES

    DECLARE_EVENT_TYPE(wxEVT_COMMAND_BUTTON_CLICKED, 1)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_CHECKBOX_CLICKED, 2)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_CHOICE_SELECTED, 3)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LISTBOX_SELECTED, 4)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, 5)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, 6)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TEXT_UPDATED, 7)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TEXT_ENTER, 8)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_MENU_SELECTED, 9)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_SLIDER_UPDATED, 10)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_RADIOBOX_SELECTED, 11)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_RADIOBUTTON_SELECTED, 12)

    // wxEVT_COMMAND_SCROLLBAR_UPDATED is now obsolete since we use
    // wxEVT_SCROLL... events

    DECLARE_EVENT_TYPE(wxEVT_COMMAND_SCROLLBAR_UPDATED, 13)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_VLBOX_SELECTED, 14)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_COMBOBOX_SELECTED, 15)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TOOL_RCLICKED, 16)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TOOL_ENTER, 17)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_SPINCTRL_UPDATED, 18)

        // Sockets and timers send events, too
    DECLARE_EVENT_TYPE(wxEVT_SOCKET, 50)
    DECLARE_EVENT_TYPE(wxEVT_TIMER , 80)

        // Mouse event types
    DECLARE_EVENT_TYPE(wxEVT_LEFT_DOWN, 100)
    DECLARE_EVENT_TYPE(wxEVT_LEFT_UP, 101)
    DECLARE_EVENT_TYPE(wxEVT_MIDDLE_DOWN, 102)
    DECLARE_EVENT_TYPE(wxEVT_MIDDLE_UP, 103)
    DECLARE_EVENT_TYPE(wxEVT_RIGHT_DOWN, 104)
    DECLARE_EVENT_TYPE(wxEVT_RIGHT_UP, 105)
    DECLARE_EVENT_TYPE(wxEVT_MOTION, 106)
    DECLARE_EVENT_TYPE(wxEVT_ENTER_WINDOW, 107)
    DECLARE_EVENT_TYPE(wxEVT_LEAVE_WINDOW, 108)
    DECLARE_EVENT_TYPE(wxEVT_LEFT_DCLICK, 109)
    DECLARE_EVENT_TYPE(wxEVT_MIDDLE_DCLICK, 110)
    DECLARE_EVENT_TYPE(wxEVT_RIGHT_DCLICK, 111)
    DECLARE_EVENT_TYPE(wxEVT_SET_FOCUS, 112)
    DECLARE_EVENT_TYPE(wxEVT_KILL_FOCUS, 113)

        // Non-client mouse events
    DECLARE_EVENT_TYPE(wxEVT_NC_LEFT_DOWN, 200)
    DECLARE_EVENT_TYPE(wxEVT_NC_LEFT_UP, 201)
    DECLARE_EVENT_TYPE(wxEVT_NC_MIDDLE_DOWN, 202)
    DECLARE_EVENT_TYPE(wxEVT_NC_MIDDLE_UP, 203)
    DECLARE_EVENT_TYPE(wxEVT_NC_RIGHT_DOWN, 204)
    DECLARE_EVENT_TYPE(wxEVT_NC_RIGHT_UP, 205)
    DECLARE_EVENT_TYPE(wxEVT_NC_MOTION, 206)
    DECLARE_EVENT_TYPE(wxEVT_NC_ENTER_WINDOW, 207)
    DECLARE_EVENT_TYPE(wxEVT_NC_LEAVE_WINDOW, 208)
    DECLARE_EVENT_TYPE(wxEVT_NC_LEFT_DCLICK, 209)
    DECLARE_EVENT_TYPE(wxEVT_NC_MIDDLE_DCLICK, 210)
    DECLARE_EVENT_TYPE(wxEVT_NC_RIGHT_DCLICK, 211)

        // Character input event type
    DECLARE_EVENT_TYPE(wxEVT_CHAR, 212)
    DECLARE_EVENT_TYPE(wxEVT_CHAR_HOOK, 213)
    DECLARE_EVENT_TYPE(wxEVT_NAVIGATION_KEY, 214)
    DECLARE_EVENT_TYPE(wxEVT_KEY_DOWN, 215)
    DECLARE_EVENT_TYPE(wxEVT_KEY_UP, 216)

        // Set cursor event
    DECLARE_EVENT_TYPE(wxEVT_SET_CURSOR, 230)

        // wxScrollbar and wxSlider event identifiers
    DECLARE_EVENT_TYPE(wxEVT_SCROLL_TOP, 300)
    DECLARE_EVENT_TYPE(wxEVT_SCROLL_BOTTOM, 301)
    DECLARE_EVENT_TYPE(wxEVT_SCROLL_LINEUP, 302)
    DECLARE_EVENT_TYPE(wxEVT_SCROLL_LINEDOWN, 303)
    DECLARE_EVENT_TYPE(wxEVT_SCROLL_PAGEUP, 304)
    DECLARE_EVENT_TYPE(wxEVT_SCROLL_PAGEDOWN, 305)
    DECLARE_EVENT_TYPE(wxEVT_SCROLL_THUMBTRACK, 306)
    DECLARE_EVENT_TYPE(wxEVT_SCROLL_THUMBRELEASE, 307)

        // Scroll events from wxWindow
    DECLARE_EVENT_TYPE(wxEVT_SCROLLWIN_TOP, 320)
    DECLARE_EVENT_TYPE(wxEVT_SCROLLWIN_BOTTOM, 321)
    DECLARE_EVENT_TYPE(wxEVT_SCROLLWIN_LINEUP, 322)
    DECLARE_EVENT_TYPE(wxEVT_SCROLLWIN_LINEDOWN, 323)
    DECLARE_EVENT_TYPE(wxEVT_SCROLLWIN_PAGEUP, 324)
    DECLARE_EVENT_TYPE(wxEVT_SCROLLWIN_PAGEDOWN, 325)
    DECLARE_EVENT_TYPE(wxEVT_SCROLLWIN_THUMBTRACK, 326)
    DECLARE_EVENT_TYPE(wxEVT_SCROLLWIN_THUMBRELEASE, 327)

        // System events
    DECLARE_EVENT_TYPE(wxEVT_SIZE, 400)
    DECLARE_EVENT_TYPE(wxEVT_MOVE, 401)
    DECLARE_EVENT_TYPE(wxEVT_CLOSE_WINDOW, 402)
    DECLARE_EVENT_TYPE(wxEVT_END_SESSION, 403)
    DECLARE_EVENT_TYPE(wxEVT_QUERY_END_SESSION, 404)
    DECLARE_EVENT_TYPE(wxEVT_ACTIVATE_APP, 405)
    DECLARE_EVENT_TYPE(wxEVT_POWER, 406)
    DECLARE_EVENT_TYPE(wxEVT_ACTIVATE, 409)
    DECLARE_EVENT_TYPE(wxEVT_CREATE, 410)
    DECLARE_EVENT_TYPE(wxEVT_DESTROY, 411)
    DECLARE_EVENT_TYPE(wxEVT_SHOW, 412)
    DECLARE_EVENT_TYPE(wxEVT_ICONIZE, 413)
    DECLARE_EVENT_TYPE(wxEVT_MAXIMIZE, 414)
    DECLARE_EVENT_TYPE(wxEVT_MOUSE_CAPTURE_CHANGED, 415)
    DECLARE_EVENT_TYPE(wxEVT_PAINT, 416)
    DECLARE_EVENT_TYPE(wxEVT_ERASE_BACKGROUND, 417)
    DECLARE_EVENT_TYPE(wxEVT_NC_PAINT, 418)
    DECLARE_EVENT_TYPE(wxEVT_PAINT_ICON, 419)
    DECLARE_EVENT_TYPE(wxEVT_MENU_CHAR, 420)
    DECLARE_EVENT_TYPE(wxEVT_MENU_INIT, 421)
    DECLARE_EVENT_TYPE(wxEVT_MENU_HIGHLIGHT, 422)
    DECLARE_EVENT_TYPE(wxEVT_POPUP_MENU_INIT, 423)
    DECLARE_EVENT_TYPE(wxEVT_CONTEXT_MENU, 424)
    DECLARE_EVENT_TYPE(wxEVT_SYS_COLOUR_CHANGED, 425)
    DECLARE_EVENT_TYPE(wxEVT_SETTING_CHANGED, 426)
    DECLARE_EVENT_TYPE(wxEVT_QUERY_NEW_PALETTE, 427)
    DECLARE_EVENT_TYPE(wxEVT_PALETTE_CHANGED, 428)
    DECLARE_EVENT_TYPE(wxEVT_JOY_BUTTON_DOWN, 429)
    DECLARE_EVENT_TYPE(wxEVT_JOY_BUTTON_UP, 430)
    DECLARE_EVENT_TYPE(wxEVT_JOY_MOVE, 431)
    DECLARE_EVENT_TYPE(wxEVT_JOY_ZMOVE, 432)
    DECLARE_EVENT_TYPE(wxEVT_DROP_FILES, 433)
    DECLARE_EVENT_TYPE(wxEVT_DRAW_ITEM, 434)
    DECLARE_EVENT_TYPE(wxEVT_MEASURE_ITEM, 435)
    DECLARE_EVENT_TYPE(wxEVT_COMPARE_ITEM, 436)
    DECLARE_EVENT_TYPE(wxEVT_INIT_DIALOG, 437)
    DECLARE_EVENT_TYPE(wxEVT_IDLE, 438)
    DECLARE_EVENT_TYPE(wxEVT_UPDATE_UI, 439)

        // Generic command events
        // Note: a click is a higher-level event than button down/up
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LEFT_CLICK, 500)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LEFT_DCLICK, 501)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_RIGHT_CLICK, 502)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_RIGHT_DCLICK, 503)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_SET_FOCUS, 504)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_KILL_FOCUS, 505)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_ENTER, 506)

        // Help events
    DECLARE_EVENT_TYPE(wxEVT_HELP, 1050)
    DECLARE_EVENT_TYPE(wxEVT_DETAILED_HELP, 1051)

END_DECLARE_EVENT_TYPES()

// these 2 events are the same
#define wxEVT_COMMAND_TOOL_CLICKED wxEVT_COMMAND_MENU_SELECTED

// Compatibility

#if WXWIN_COMPATIBILITY

#define wxEVENT_TYPE_BUTTON_COMMAND             wxEVT_COMMAND_BUTTON_CLICKED
#define wxEVENT_TYPE_CHECKBOX_COMMAND           wxEVT_COMMAND_CHECKBOX_CLICKED
#define wxEVENT_TYPE_CHOICE_COMMAND             wxEVT_COMMAND_CHOICE_SELECTED
#define wxEVENT_TYPE_LISTBOX_COMMAND            wxEVT_COMMAND_LISTBOX_SELECTED
#define wxEVENT_TYPE_LISTBOX_DCLICK_COMMAND     wxEVT_COMMAND_LISTBOX_DOUBLECLICKED
#define wxEVENT_TYPE_TEXT_COMMAND               wxEVT_COMMAND_TEXT_UPDATED
#define wxEVENT_TYPE_MULTITEXT_COMMAND          wxEVT_COMMAND_TEXT_UPDATED
#define wxEVENT_TYPE_MENU_COMMAND               wxEVT_COMMAND_MENU_SELECTED
#define wxEVENT_TYPE_SLIDER_COMMAND             wxEVT_COMMAND_SLIDER_UPDATED
#define wxEVENT_TYPE_RADIOBOX_COMMAND           wxEVT_COMMAND_RADIOBOX_SELECTED
#define wxEVENT_TYPE_RADIOBUTTON_COMMAND        wxEVT_COMMAND_RADIOBUTTON_SELECTED
#define wxEVENT_TYPE_TEXT_ENTER_COMMAND         wxEVT_COMMAND_TEXT_ENTER
#define wxEVENT_TYPE_SET_FOCUS                  wxEVT_SET_FOCUS
#define wxEVENT_TYPE_KILL_FOCUS                 wxEVT_KILL_FOCUS
#define wxEVENT_TYPE_SCROLLBAR_COMMAND          wxEVT_COMMAND_SCROLLBAR_UPDATED
#define wxEVENT_TYPE_VIRT_LISTBOX_COMMAND       wxEVT_COMMAND_VLBOX_SELECTED
#define wxEVENT_TYPE_COMBOBOX_COMMAND           wxEVT_COMMAND_COMBOBOX_SELECTED

#define wxEVENT_TYPE_LEFT_DOWN                  wxEVT_LEFT_DOWN
#define wxEVENT_TYPE_LEFT_UP                    wxEVT_LEFT_UP
#define wxEVENT_TYPE_MIDDLE_DOWN                wxEVT_MIDDLE_DOWN
#define wxEVENT_TYPE_MIDDLE_UP                  wxEVT_MIDDLE_UP
#define wxEVENT_TYPE_RIGHT_DOWN                 wxEVT_RIGHT_DOWN
#define wxEVENT_TYPE_RIGHT_UP                   wxEVT_RIGHT_UP
#define wxEVENT_TYPE_MOTION                     wxEVT_MOTION
#define wxEVENT_TYPE_ENTER_WINDOW               wxEVT_ENTER_WINDOW
#define wxEVENT_TYPE_LEAVE_WINDOW               wxEVT_LEAVE_WINDOW
#define wxEVENT_TYPE_LEFT_DCLICK                wxEVT_LEFT_DCLICK
#define wxEVENT_TYPE_MIDDLE_DCLICK              wxEVT_MIDDLE_DCLICK
#define wxEVENT_TYPE_RIGHT_DCLICK               wxEVT_RIGHT_DCLICK
#define wxEVENT_TYPE_CHAR                       wxEVT_CHAR
#define wxEVENT_TYPE_SCROLL_TOP                 wxEVT_SCROLL_TOP
#define wxEVENT_TYPE_SCROLL_BOTTOM              wxEVT_SCROLL_BOTTOM
#define wxEVENT_TYPE_SCROLL_LINEUP              wxEVT_SCROLL_LINEUP
#define wxEVENT_TYPE_SCROLL_LINEDOWN            wxEVT_SCROLL_LINEDOWN
#define wxEVENT_TYPE_SCROLL_PAGEUP              wxEVT_SCROLL_PAGEUP
#define wxEVENT_TYPE_SCROLL_PAGEDOWN            wxEVT_SCROLL_PAGEDOWN
#define wxEVENT_TYPE_SCROLL_THUMBTRACK          wxEVT_SCROLL_THUMBTRACK

#endif // WXWIN_COMPATIBILITY

/*
 * wxWindows events, covering all interesting things that might happen
 * (button clicking, resizing, setting text in widgets, etc.).
 *
 * For each completely new event type, derive a new event class.
 * An event CLASS represents a C++ class defining a range of similar event TYPES;
 * examples are canvas events, panel item command events.
 * An event TYPE is a unique identifier for a particular system event,
 * such as a button press or a listbox deselection.
 *
 */

class WXDLLEXPORT wxEvent : public wxObject
{
    DECLARE_ABSTRACT_CLASS(wxEvent)

public:
    wxEvent(int id = 0);
    ~wxEvent() {}

    void SetEventType(wxEventType typ) { m_eventType = typ; }
    wxEventType GetEventType() const { return m_eventType; }
    wxObject *GetEventObject() const { return m_eventObject; }
    void SetEventObject(wxObject *obj) { m_eventObject = obj; }
    long GetTimestamp() const { return m_timeStamp; }
    void SetTimestamp(long ts = 0) { m_timeStamp = ts; }
    int GetId() const { return m_id; }
    void SetId(int Id) { m_id = Id; }

    // Can instruct event processor that we wish to ignore this event
    // (treat as if the event table entry had not been found): this must be done
    // to allow the event processing by the base classes (calling event.Skip()
    // is the analog of calling the base class verstion of a virtual function)
    void Skip(bool skip = TRUE) { m_skipped = skip; }
    bool GetSkipped() const { return m_skipped; };

    // implementation only: this test is explicitlty anti OO and this functions
    // exists only for optimization purposes
    bool IsCommandEvent() const { return m_isCommandEvent; }

    void CopyObject(wxObject& object_dest) const;

public:
    wxObject*         m_eventObject;
    wxEventType       m_eventType;
    long              m_timeStamp;
    int               m_id;
    wxObject*         m_callbackUserData;
    bool              m_skipped;

    // optimization: instead of using costly IsKindOf() we keep a flag telling
    // whether we're a command event (by far the most common case)
    bool              m_isCommandEvent;
};

#if wxUSE_GUI

// Item or menu event class
/*
 wxEVT_COMMAND_BUTTON_CLICKED
 wxEVT_COMMAND_CHECKBOX_CLICKED
 wxEVT_COMMAND_CHOICE_SELECTED
 wxEVT_COMMAND_LISTBOX_SELECTED
 wxEVT_COMMAND_LISTBOX_DOUBLECLICKED
 wxEVT_COMMAND_TEXT_UPDATED
 wxEVT_COMMAND_TEXT_ENTER
 wxEVT_COMMAND_MENU_SELECTED
 wxEVT_COMMAND_SLIDER_UPDATED
 wxEVT_COMMAND_RADIOBOX_SELECTED
 wxEVT_COMMAND_RADIOBUTTON_SELECTED
 wxEVT_COMMAND_SCROLLBAR_UPDATED
 wxEVT_COMMAND_VLBOX_SELECTED
 wxEVT_COMMAND_COMBOBOX_SELECTED
*/

class WXDLLEXPORT wxCommandEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxCommandEvent)

public:
    wxCommandEvent(wxEventType commandType = wxEVT_NULL, int id = 0);
    ~wxCommandEvent() {}

    /*
     * Accessors dependent on context
     *
     */

    // Set/Get client data from controls
    void SetClientData(void* clientData) { m_clientData = clientData; }
    void *GetClientData() const { return m_clientData; }

    // Set/Get client object from controls
    void SetClientObject(wxClientData* clientObject) { m_clientObject = clientObject; }
    void *GetClientObject() const { return m_clientObject; }

    // Get listbox selection if single-choice
    int GetSelection() const { return m_commandInt; }

    // Set/Get listbox/choice selection string
    void SetString(const wxString& s) { m_commandString = s; }
    wxString GetString() const { return m_commandString; }

    // Get checkbox value
    bool IsChecked() const { return m_commandInt != 0; }

    // TRUE if the listbox event was a selection.
    bool IsSelection() const { return (m_extraLong != 0); }

    void SetExtraLong(long extraLong) { m_extraLong = extraLong; }
    long GetExtraLong() const { return m_extraLong ; }

    void SetInt(int i) { m_commandInt = i; }
    long GetInt() const { return m_commandInt ; }

    void CopyObject(wxObject& obj) const;

#ifdef WXWIN_COMPATIBILITY_2
    bool Checked() const { return IsChecked(); }
#endif // WXWIN_COMPATIBILITY_2

public:
    wxString          m_commandString; // String event argument
    int               m_commandInt;
    long              m_extraLong;     // Additional information (e.g. select/deselect)
    void*             m_clientData;    // Arbitrary client data
    wxClientData*     m_clientObject;  // Arbitrary client object
};

// this class adds a possibility to react (from the user) code to a control
// notification: allow or veto the operation being reported.
class WXDLLEXPORT wxNotifyEvent  : public wxCommandEvent
{
public:
    wxNotifyEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
                : wxCommandEvent(commandType, id) { m_bAllow = TRUE; }

    // veto the operation (usually it's allowed by default)
    void Veto() { m_bAllow = FALSE; }

    // allow the operation if it was disabled by default
    void Allow() { m_bAllow = TRUE; }

    // for implementation code only: is the operation allowed?
    bool IsAllowed() const { return m_bAllow; }

    // probably useless: CopyObject() is used for deferred event
    // handling but wxNotifyEvent must be processed immediately
    void CopyObject(wxObject& obj) const;

private:
    bool m_bAllow;

    DECLARE_DYNAMIC_CLASS(wxNotifyEvent)
};

// Scroll event class, derived form wxCommandEvent. wxScrollEvents are
// sent by wxSlider and wxScrollbar.
/*
 wxEVT_SCROLL_TOP
 wxEVT_SCROLL_BOTTOM
 wxEVT_SCROLL_LINEUP
 wxEVT_SCROLL_LINEDOWN
 wxEVT_SCROLL_PAGEUP
 wxEVT_SCROLL_PAGEDOWN
 wxEVT_SCROLL_THUMBTRACK
 wxEVT_SCROLL_THUMBRELEASE
*/

class WXDLLEXPORT wxScrollEvent : public wxCommandEvent
{
    DECLARE_DYNAMIC_CLASS(wxScrollEvent)

public:
    wxScrollEvent(wxEventType commandType = wxEVT_NULL,
                  int id = 0, int pos = 0, int orient = 0);
    ~wxScrollEvent() {}

    /*
     * Accessors
     *
     */

    int GetOrientation() const { return (int) m_extraLong ; }
    int GetPosition() const { return m_commandInt ; }
    void SetOrientation(int orient) { m_extraLong = (long) orient; }
    void SetPosition(int pos) { m_commandInt = pos; }
};

// ScrollWin event class, derived fom wxEvent. wxScrollWinEvents
// are sent by wxWindow.
/*
 wxEVT_SCROLLWIN_TOP
 wxEVT_SCROLLWIN_BOTTOM
 wxEVT_SCROLLWIN_LINEUP
 wxEVT_SCROLLWIN_LINEDOWN
 wxEVT_SCROLLWIN_PAGEUP
 wxEVT_SCROLLWIN_PAGEDOWN
 wxEVT_SCROLLWIN_THUMBTRACK
 wxEVT_SCROLLWIN_THUMBRELEASE
*/

class WXDLLEXPORT wxScrollWinEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxScrollWinEvent)

public:
    wxScrollWinEvent(wxEventType commandType = wxEVT_NULL,
                     int pos = 0, int orient = 0);
    ~wxScrollWinEvent() {}

    /*
     * Accessors
     */

    int GetOrientation() const { return (int) m_extraLong ; }
    int GetPosition() const { return m_commandInt ; }
    void SetOrientation(int orient) { m_extraLong = (long) orient; }
    void SetPosition(int pos) { m_commandInt = pos; }

    void CopyObject(wxObject& object_dest) const;
public:
    int               m_commandInt;    // Additional information
    long              m_extraLong;
};

// Mouse event class

/*
 wxEVT_LEFT_DOWN
 wxEVT_LEFT_UP
 wxEVT_MIDDLE_DOWN
 wxEVT_MIDDLE_UP
 wxEVT_RIGHT_DOWN
 wxEVT_RIGHT_UP
 wxEVT_MOTION
 wxEVT_ENTER_WINDOW
 wxEVT_LEAVE_WINDOW
 wxEVT_LEFT_DCLICK
 wxEVT_MIDDLE_DCLICK
 wxEVT_RIGHT_DCLICK
 wxEVT_NC_LEFT_DOWN
 wxEVT_NC_LEFT_UP,
 wxEVT_NC_MIDDLE_DOWN,
 wxEVT_NC_MIDDLE_UP,
 wxEVT_NC_RIGHT_DOWN,
 wxEVT_NC_RIGHT_UP,
 wxEVT_NC_MOTION,
 wxEVT_NC_ENTER_WINDOW,
 wxEVT_NC_LEAVE_WINDOW,
 wxEVT_NC_LEFT_DCLICK,
 wxEVT_NC_MIDDLE_DCLICK,
 wxEVT_NC_RIGHT_DCLICK,
*/

class WXDLLEXPORT wxMouseEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxMouseEvent)

public:
    wxMouseEvent(wxEventType mouseType = wxEVT_NULL);

    // Was it a button event? (*doesn't* mean: is any button *down*?)
    bool IsButton() const { return Button(-1); }

    // Was it a down event from button 1, 2 or 3 or any?
    bool ButtonDown(int but = -1) const;

    // Was it a dclick event from button 1, 2 or 3 or any?
    bool ButtonDClick(int but = -1) const;

    // Was it a up event from button 1, 2 or 3 or any?
    bool ButtonUp(int but = -1) const;

    // Was the given button 1,2,3 or any changing state?
    bool Button(int but) const;

    // Was the given button 1,2,3 or any in Down state?
    bool ButtonIsDown(int but) const;

    // Find state of shift/control keys
    bool ControlDown() const { return m_controlDown; }
    bool MetaDown() const { return m_metaDown; }
    bool AltDown() const { return m_altDown; }
    bool ShiftDown() const { return m_shiftDown; }

    // Find which event was just generated
    bool LeftDown() const { return (m_eventType == wxEVT_LEFT_DOWN); }
    bool MiddleDown() const { return (m_eventType == wxEVT_MIDDLE_DOWN); }
    bool RightDown() const { return (m_eventType == wxEVT_RIGHT_DOWN); }

    bool LeftUp() const { return (m_eventType == wxEVT_LEFT_UP); }
    bool MiddleUp() const { return (m_eventType == wxEVT_MIDDLE_UP); }
    bool RightUp() const { return (m_eventType == wxEVT_RIGHT_UP); }

    bool LeftDClick() const { return (m_eventType == wxEVT_LEFT_DCLICK); }
    bool MiddleDClick() const { return (m_eventType == wxEVT_MIDDLE_DCLICK); }
    bool RightDClick() const { return (m_eventType == wxEVT_RIGHT_DCLICK); }

    // Find the current state of the mouse buttons (regardless
    // of current event type)
    bool LeftIsDown() const { return m_leftDown; }
    bool MiddleIsDown() const { return m_middleDown; }
    bool RightIsDown() const { return m_rightDown; }

    // True if a button is down and the mouse is moving
    bool Dragging() const
    {
        return ((m_eventType == wxEVT_MOTION) &&
                (LeftIsDown() || MiddleIsDown() || RightIsDown()));
    }

    // True if the mouse is moving, and no button is down
    bool Moving() const { return (m_eventType == wxEVT_MOTION); }

    // True if the mouse is just entering the window
    bool Entering() const { return (m_eventType == wxEVT_ENTER_WINDOW); }

    // True if the mouse is just leaving the window
    bool Leaving() const { return (m_eventType == wxEVT_LEAVE_WINDOW); }

    // Find the position of the event
    void GetPosition(wxCoord *xpos, wxCoord *ypos) const
    {
        if (xpos)
            *xpos = m_x;
        if (ypos)
            *ypos = m_y;
    }

#ifndef __WIN16__
    void GetPosition(long *xpos, long *ypos) const
    {
        if (xpos)
            *xpos = (long)m_x;
        if (ypos)
            *ypos = (long)m_y;
    }
#endif

    // Find the position of the event
    wxPoint GetPosition() const { return wxPoint(m_x, m_y); }

    // Find the logical position of the event given the DC
    wxPoint GetLogicalPosition(const wxDC& dc) const ;

    // Compatibility
#if WXWIN_COMPATIBILITY
    void Position(long *xpos, long *ypos) const
    {
        if (xpos)
            *xpos = (long)m_x;
        if (ypos)
            *ypos = (long)m_y;
    }

    void Position(float *xpos, float *ypos) const
    {
        *xpos = (float) m_x; *ypos = (float) m_y;
    }
#endif // WXWIN_COMPATIBILITY

    // Get X position
    wxCoord GetX() const { return m_x; }

    // Get Y position
    wxCoord GetY() const { return m_y; }

    void CopyObject(wxObject& obj) const;

public:
    wxCoord m_x, m_y;

    bool          m_leftDown;
    bool          m_middleDown;
    bool          m_rightDown;

    bool          m_controlDown;
    bool          m_shiftDown;
    bool          m_altDown;
    bool          m_metaDown;
};

// Cursor set event

/*
   wxEVT_SET_CURSOR
 */

class WXDLLEXPORT wxSetCursorEvent : public wxEvent
{
public:
    wxSetCursorEvent(wxCoord x, wxCoord y)
    {
        m_eventType = wxEVT_SET_CURSOR;

        m_x = x;
        m_y = y;
    }

    wxCoord GetX() const { return m_x; }
    wxCoord GetY() const { return m_y; }

    void SetCursor(const wxCursor& cursor) { m_cursor = cursor; }
    const wxCursor& GetCursor() const { return m_cursor; }
    bool HasCursor() const { return m_cursor.Ok(); }

private:
    wxCoord  m_x, m_y;
    wxCursor m_cursor;
};

// Keyboard input event class

/*
 wxEVT_CHAR
 wxEVT_CHAR_HOOK
 wxEVT_KEY_DOWN
 wxEVT_KEY_UP
 */

class WXDLLEXPORT wxKeyEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxKeyEvent)

public:
    wxKeyEvent(wxEventType keyType = wxEVT_NULL);

    // Find state of shift/control keys
    bool ControlDown() const { return m_controlDown; }
    bool MetaDown() const { return m_metaDown; }
    bool AltDown() const { return m_altDown; }
    bool ShiftDown() const { return m_shiftDown; }

    bool HasModifiers() const { return ControlDown() || AltDown() || MetaDown(); }

    // get the key code: an ASCII7 char or an element of wxKeyCode enum
    int GetKeyCode() const { return (int)m_keyCode; }

    // Find the position of the event
    void GetPosition(wxCoord *xpos, wxCoord *ypos) const
    {
        if (xpos) *xpos = m_x;
        if (ypos) *ypos = m_y;
    }

#ifndef __WIN16__
    void GetPosition(long *xpos, long *ypos) const
    {
        if (xpos) *xpos = (long)m_x;
        if (ypos) *ypos = (long)m_y;
    }
#endif

    wxPoint GetPosition() const
        { return wxPoint(m_x, m_y); }

    // Get X position
    wxCoord GetX() const { return m_x; }

    // Get Y position
    wxCoord GetY() const { return m_y; }

    void CopyObject(wxObject& obj) const;

    // deprecated
    long KeyCode() const { return m_keyCode; }

public:
    wxCoord       m_x, m_y;

    long          m_keyCode;

    bool          m_controlDown;
    bool          m_shiftDown;
    bool          m_altDown;
    bool          m_metaDown;
    bool          m_scanCode;
};

// Size event class
/*
 wxEVT_SIZE
 */

class WXDLLEXPORT wxSizeEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxSizeEvent)

public:
    wxSize m_size;

    wxSizeEvent() { m_eventType = wxEVT_SIZE; }
    wxSizeEvent(const wxSize& sz, int id = 0)
        : m_size(sz)
        { m_eventType = wxEVT_SIZE; m_id = id; }

    wxSize GetSize() const { return m_size; }

    void CopyObject(wxObject& obj) const;
};

// Move event class

/*
 wxEVT_MOVE
 */

class WXDLLEXPORT wxMoveEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxMoveEvent)

public:
    wxPoint m_pos;

    wxMoveEvent() { m_eventType = wxEVT_MOVE; }
    wxMoveEvent(const wxPoint& pos, int id = 0)
        : m_pos(pos)
        { m_eventType = wxEVT_MOVE; m_id = id; }

    wxPoint GetPosition() const { return m_pos; }

    void CopyObject(wxObject& obj) const;
};

// Paint event class
/*
 wxEVT_PAINT
 wxEVT_NC_PAINT
 wxEVT_PAINT_ICON
 */

#if defined(__WXDEBUG__) && (defined(__WXMSW__) || defined(__WXPM__))
    // see comments in src/msw|os2/dcclient.cpp where g_isPainting is defined
    extern WXDLLEXPORT int g_isPainting;
#endif // debug

class WXDLLEXPORT wxPaintEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxPaintEvent)

public:
    wxPaintEvent(int Id = 0)
    {
        m_eventType = wxEVT_PAINT;
        m_id = Id;

#if defined(__WXDEBUG__) && (defined(__WXMSW__) || defined(__WXPM__))
        // set the internal flag for the duration of processing of WM_PAINT
        g_isPainting++;
#endif // debug
    }

#if defined(__WXDEBUG__) && (defined(__WXMSW__) || defined(__WXPM__))
    ~wxPaintEvent()
    {
        g_isPainting--;
    }
#endif // debug
};

// Erase background event class
/*
 wxEVT_ERASE_BACKGROUND
 */

class WXDLLEXPORT wxEraseEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxEraseEvent)

public:
    wxDC *m_dc;

    wxEraseEvent(int Id = 0, wxDC *dc = (wxDC *) NULL)
        { m_eventType = wxEVT_ERASE_BACKGROUND; m_id = Id; m_dc = dc; }
    wxDC *GetDC() const { return m_dc; }

    void CopyObject(wxObject& obj) const;
};

// Focus event class
/*
 wxEVT_SET_FOCUS
 wxEVT_KILL_FOCUS
 */

class WXDLLEXPORT wxFocusEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxFocusEvent)

public:
    wxFocusEvent(wxEventType type = wxEVT_NULL, int Id = 0)
        { m_eventType = type; m_id = Id; }
};

// Activate event class
/*
 wxEVT_ACTIVATE
 wxEVT_ACTIVATE_APP
 */

class WXDLLEXPORT wxActivateEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxActivateEvent)

public:
    wxActivateEvent(wxEventType type = wxEVT_NULL, bool active = TRUE, int Id = 0)
        { m_eventType = type; m_active = active; m_id = Id; }
    bool GetActive() const { return m_active; }

    void CopyObject(wxObject& obj) const;

private:
    bool m_active;
};

// InitDialog event class
/*
 wxEVT_INIT_DIALOG
 */

class WXDLLEXPORT wxInitDialogEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxInitDialogEvent)

public:
    wxInitDialogEvent(int Id = 0)
        { m_eventType = wxEVT_INIT_DIALOG; m_id = Id; }
};

// Miscellaneous menu event class
/*
 wxEVT_MENU_CHAR,
 wxEVT_MENU_INIT,
 wxEVT_MENU_HIGHLIGHT,
 wxEVT_POPUP_MENU_INIT,
 wxEVT_CONTEXT_MENU,
*/

class WXDLLEXPORT wxMenuEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxMenuEvent)

public:
    wxMenuEvent(wxEventType type = wxEVT_NULL, int id = 0)
      { m_eventType = type; m_menuId = id; }

    int GetMenuId() const { return m_menuId; }

    void CopyObject(wxObject& obj) const;
private:
    int m_menuId;
};

// Window close or session close event class
/*
 wxEVT_CLOSE_WINDOW,
 wxEVT_END_SESSION,
 wxEVT_QUERY_END_SESSION
 */

class WXDLLEXPORT wxCloseEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxCloseEvent)

public:
    wxCloseEvent(wxEventType type = wxEVT_NULL, int id = 0)
    {
        m_eventType = type;
        m_loggingOff = TRUE;
        m_veto = FALSE;         // should be FALSE by default
        m_id = id;
#if WXWIN_COMPATIBILITY
        m_force = FALSE;
#endif // WXWIN_COMPATIBILITY
        m_canVeto = TRUE;
    }

    void SetLoggingOff(bool logOff) { m_loggingOff = logOff; }
    bool GetLoggingOff() const { return m_loggingOff; }

    void Veto(bool veto = TRUE)
    {
        // GetVeto() will return FALSE anyhow...
        wxCHECK_RET( m_canVeto,
                     wxT("call to Veto() ignored (can't veto this event)") );

        m_veto = veto;
    }
    void SetCanVeto(bool canVeto) { m_canVeto = canVeto; }
    // No more asserts here please, the one you put here was wrong.
    bool CanVeto() const { return m_canVeto; }
    bool GetVeto() const { return m_canVeto && m_veto; }

#if WXWIN_COMPATIBILITY
    // This is probably obsolete now, since we use CanVeto instead, in
    // both OnCloseWindow and OnQueryEndSession.
    // m_force == ! m_canVeto i.e., can't veto means we must force it to close.
    void SetForce(bool force) { m_force = force; }
    bool GetForce() const { return m_force; }
#endif

    void CopyObject(wxObject& obj) const;

protected:
    bool m_loggingOff;
    bool m_veto, m_canVeto;

#if WXWIN_COMPATIBILITY
    bool m_force;
#endif
};

/*
 wxEVT_SHOW
 */

class WXDLLEXPORT wxShowEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxShowEvent)

public:

    wxShowEvent(int id = 0, bool show = FALSE)
        { m_eventType = wxEVT_SHOW; m_id = id; m_show = show; }

    void SetShow(bool show) { m_show = show; }
    bool GetShow() const { return m_show; }

    void CopyObject(wxObject& obj) const;

protected:
    bool m_show;
};

/*
 wxEVT_ICONIZE
 */

class WXDLLEXPORT wxIconizeEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxIconizeEvent)

public:
    wxIconizeEvent(int id = 0)
        { m_eventType = wxEVT_ICONIZE; m_id = id; }
};

/*
 wxEVT_MAXIMIZE
 */

class WXDLLEXPORT wxMaximizeEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxMaximizeEvent)

public:
  wxMaximizeEvent(int id = 0)
      { m_eventType = wxEVT_MAXIMIZE; m_id = id; }
};

// Joystick event class
/*
 wxEVT_JOY_BUTTON_DOWN,
 wxEVT_JOY_BUTTON_UP,
 wxEVT_JOY_MOVE,
 wxEVT_JOY_ZMOVE
*/

// Which joystick? Same as Windows ids so no conversion necessary.
#define wxJOYSTICK1     0
#define wxJOYSTICK2     1

// Which button is down?
#define wxJOY_BUTTON1   1
#define wxJOY_BUTTON2   2
#define wxJOY_BUTTON3   4
#define wxJOY_BUTTON4   8
#define wxJOY_BUTTON_ANY -1

class WXDLLEXPORT wxJoystickEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxJoystickEvent)

public:
    wxPoint   m_pos;
    int       m_zPosition;
    int       m_buttonChange; // Which button changed?
    int       m_buttonState; // Which buttons are down?
    int       m_joyStick; // Which joystick?

    wxJoystickEvent(wxEventType type = wxEVT_NULL,
                    int state = 0,
                    int joystick = wxJOYSTICK1,
                    int change = 0)
    {
        m_eventType = type;
        m_buttonState = state;
        m_pos = wxPoint(0,0);
        m_zPosition = 0;
        m_joyStick = joystick;
        m_buttonChange = change;
    }

    wxPoint GetPosition() const { return m_pos; }
    int GetZPosition() const { return m_zPosition; }
    int GetButtonState() const { return m_buttonState; }
    int GetButtonChange() const { return m_buttonChange; }
    int GetJoystick() const { return m_joyStick; }

    void SetJoystick(int stick) { m_joyStick = stick; }
    void SetButtonState(int state) { m_buttonState = state; }
    void SetButtonChange(int change) { m_buttonChange = change; }
    void SetPosition(const wxPoint& pos) { m_pos = pos; }
    void SetZPosition(int zPos) { m_zPosition = zPos; }

    // Was it a button event? (*doesn't* mean: is any button *down*?)
    bool IsButton() const { return ((GetEventType() == wxEVT_JOY_BUTTON_DOWN) ||
            (GetEventType() == wxEVT_JOY_BUTTON_UP)); }

    // Was it a move event?
    bool IsMove() const { return (GetEventType() == wxEVT_JOY_MOVE) ; }

    // Was it a zmove event?
    bool IsZMove() const { return (GetEventType() == wxEVT_JOY_ZMOVE) ; }

    // Was it a down event from button 1, 2, 3, 4 or any?
    bool ButtonDown(int but = wxJOY_BUTTON_ANY) const
    { return ((GetEventType() == wxEVT_JOY_BUTTON_DOWN) &&
            ((but == wxJOY_BUTTON_ANY) || (but == m_buttonChange))); }

    // Was it a up event from button 1, 2, 3 or any?
    bool ButtonUp(int but = wxJOY_BUTTON_ANY) const
    { return ((GetEventType() == wxEVT_JOY_BUTTON_UP) &&
            ((but == wxJOY_BUTTON_ANY) || (but == m_buttonChange))); }

    // Was the given button 1,2,3,4 or any in Down state?
    bool ButtonIsDown(int but =  wxJOY_BUTTON_ANY) const
    { return (((but == wxJOY_BUTTON_ANY) && (m_buttonState != 0)) ||
            ((m_buttonState & but) == but)); }

    void CopyObject(wxObject& obj) const;
};

// Drop files event class
/*
 wxEVT_DROP_FILES
 */

class WXDLLEXPORT wxDropFilesEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxDropFilesEvent)

public:
    int       m_noFiles;
    wxPoint   m_pos;
    wxString* m_files;        // Memory (de)allocated by code calling ProcessEvent

    wxDropFilesEvent(wxEventType type = wxEVT_NULL,
                     int noFiles = 0,
                     wxString *files = (wxString *) NULL)
        { m_eventType = type; m_noFiles = noFiles; m_files = files; }

    wxPoint GetPosition() const { return m_pos; }
    int GetNumberOfFiles() const { return m_noFiles; }
    wxString *GetFiles() const { return m_files; }

    void CopyObject(wxObject& obj) const;
};

// Update UI event
/*
 wxEVT_UPDATE_UI
 */

class WXDLLEXPORT wxUpdateUIEvent : public wxCommandEvent
{
    DECLARE_DYNAMIC_CLASS(wxUpdateUIEvent)

public:
    wxUpdateUIEvent(wxWindowID commandId = 0)
    {
        m_eventType = wxEVT_UPDATE_UI;
        m_id = commandId;
        m_checked = FALSE;
        m_setChecked = FALSE;
        m_enabled = FALSE;
        m_setEnabled = FALSE;
        m_setText = FALSE;
        m_text = "";
    }

    bool GetChecked() const { return m_checked; }
    bool GetEnabled() const { return m_enabled; }
    wxString GetText() const { return m_text; }
    bool GetSetText() const { return m_setText; }
    bool GetSetChecked() const { return m_setChecked; }
    bool GetSetEnabled() const { return m_setEnabled; }

    void Check(bool check) { m_checked = check; m_setChecked = TRUE; }
    void Enable(bool enable) { m_enabled = enable; m_setEnabled = TRUE; }
    void SetText(const wxString& text) { m_text = text; m_setText = TRUE; }

    void CopyObject(wxObject& obj) const;

protected:
    bool          m_checked;
    bool          m_enabled;
    bool          m_setEnabled;
    bool          m_setText;
    bool          m_setChecked;
    wxString      m_text;
};

/*
 wxEVT_SYS_COLOUR_CHANGED
 */

// TODO: shouldn't all events record the window ID?
class WXDLLEXPORT wxSysColourChangedEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxSysColourChangedEvent)

public:
    wxSysColourChangedEvent()
        { m_eventType = wxEVT_SYS_COLOUR_CHANGED; }
};

/*
 wxEVT_PALETTE_CHANGED
 */

class WXDLLEXPORT wxPaletteChangedEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxPaletteChangedEvent)

public:
    wxPaletteChangedEvent(wxWindowID id = 0) : wxEvent(id)
    {
        m_eventType = wxEVT_PALETTE_CHANGED;
        m_changedWindow = (wxWindow *) NULL;
    }

    void SetChangedWindow(wxWindow* win) { m_changedWindow = win; }
    wxWindow* GetChangedWindow() const { return m_changedWindow; }

    void CopyObject(wxObject& obj) const;

protected:
    wxWindow*     m_changedWindow;
};

/*
 wxEVT_QUERY_NEW_PALETTE
 Indicates the window is getting keyboard focus and should re-do its palette.
 */

class WXDLLEXPORT wxQueryNewPaletteEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxQueryNewPaletteEvent)

public:
    wxQueryNewPaletteEvent(wxWindowID id = 0): wxEvent(id)
        { m_eventType = wxEVT_QUERY_NEW_PALETTE; m_paletteRealized = FALSE; }

    // App sets this if it changes the palette.
    void SetPaletteRealized(bool realized) { m_paletteRealized = realized; }
    bool GetPaletteRealized() const { return m_paletteRealized; }

    void CopyObject(wxObject& obj) const;

protected:
    bool m_paletteRealized;
};

/*
 Event generated by dialog navigation keys
 wxEVT_NAVIGATION_KEY
 */
// NB: don't derive from command event to avoid being propagated to the parent
class WXDLLEXPORT wxNavigationKeyEvent : public wxEvent
{
public:
    wxNavigationKeyEvent()
    {
        SetEventType(wxEVT_NAVIGATION_KEY);

        m_flags = IsForward | Propagate;    // defaults are for TAB
        m_focus = (wxWindow *)NULL;
    }

    // direction: forward (true) or backward (false)
    bool GetDirection() const
        { return (m_flags & IsForward) != 0; }
    void SetDirection(bool bForward)
        { if ( bForward ) m_flags |= IsForward; else m_flags &= ~IsForward; }

    // it may be a window change event (MDI, notebook pages...) or a control
    // change event
    bool IsWindowChange() const
        { return (m_flags & WinChange) != 0; }
    void SetWindowChange(bool bIs)
        { if ( bIs ) m_flags |= WinChange; else m_flags &= ~WinChange; }

    // some navigation events are meant to be propagated upwards (Windows
    // convention is to do this for TAB events) while others should always
    // cycle inside the panel/radiobox/whatever we're current inside
    bool ShouldPropagate() const
        { return (m_flags & Propagate) != 0; }
    void SetPropagate(bool bDoIt)
        { if ( bDoIt ) m_flags |= Propagate; else m_flags &= ~Propagate; }

    // the child which has the focus currently (may be NULL - use
    // wxWindow::FindFocus then)
    wxWindow* GetCurrentFocus() const { return m_focus; }
    void SetCurrentFocus(wxWindow *win) { m_focus = win; }

private:
    enum
    {
        IsForward = 0x0001,
        WinChange = 0x0002,
        Propagate = 0x0004
    };

    long m_flags;
    wxWindow *m_focus;

    DECLARE_DYNAMIC_CLASS(wxNavigationKeyEvent)
};

// Window creation/destruction events: the first is sent as soon as window is
// created (i.e. the underlying GUI object exists), but when the C++ object is
// fully initialized (so virtual functions may be called). The second,
// wxEVT_DESTROY, is sent right before the window is destroyed - again, it's
// still safe to call virtual functions at this moment
/*
 wxEVT_CREATE
 wxEVT_DESTROY
 */

class WXDLLEXPORT wxWindowCreateEvent : public wxCommandEvent
{
    DECLARE_DYNAMIC_CLASS(wxWindowCreateEvent)

public:
    wxWindowCreateEvent(wxWindow *win = NULL);

    wxWindow *GetWindow() const { return (wxWindow *)GetEventObject(); }
};

class WXDLLEXPORT wxWindowDestroyEvent : public wxCommandEvent
{
    DECLARE_DYNAMIC_CLASS(wxWindowDestroyEvent)

public:
    wxWindowDestroyEvent(wxWindow *win = NULL);

    wxWindow *GetWindow() const { return (wxWindow *)GetEventObject(); }
};

// A help event is sent when the user clicks on a window in context-help mode.
/*
 wxEVT_HELP
 wxEVT_DETAILED_HELP
*/

class WXDLLEXPORT wxHelpEvent : public wxCommandEvent
{
public:
    wxHelpEvent(wxEventType type = wxEVT_NULL,
                wxWindowID id = 0,
                const wxPoint& pt = wxDefaultPosition)
    {
        m_eventType = type;
        m_id = id;
        m_pos = pt;
    }

    // Position of event (in screen coordinates)
    const wxPoint& GetPosition() const { return m_pos; }
    void SetPosition(const wxPoint& pos) { m_pos = pos; }

    // Optional link to further help
    const wxString& GetLink() const { return m_link; }
    void SetLink(const wxString& link) { m_link = link; }

    // Optional target to display help in. E.g. a window specification
    const wxString& GetTarget() const { return m_target; }
    void SetTarget(const wxString& target) { m_target = target; }

protected:
    wxPoint   m_pos;
    wxString  m_target;
    wxString  m_link;

private:
    DECLARE_DYNAMIC_CLASS(wxHelpEvent)
};

#endif // wxUSE_GUI

// Idle event
/*
 wxEVT_IDLE
 */

class WXDLLEXPORT wxIdleEvent : public wxEvent
{
    DECLARE_DYNAMIC_CLASS(wxIdleEvent)

public:
    wxIdleEvent()
        { m_eventType = wxEVT_IDLE; m_requestMore = FALSE; }

    void RequestMore(bool needMore = TRUE) { m_requestMore = needMore; }
    bool MoreRequested() const { return m_requestMore; }

    void CopyObject(wxObject& obj) const;

protected:
    bool m_requestMore;
};

/* TODO
 wxEVT_POWER,
 wxEVT_MOUSE_CAPTURE_CHANGED,
 wxEVT_SETTING_CHANGED, // WM_WININICHANGE (NT) / WM_SETTINGCHANGE (Win95)
// wxEVT_FONT_CHANGED,  // WM_FONTCHANGE: roll into wxEVT_SETTING_CHANGED, but remember to propagate
                        // wxEVT_FONT_CHANGED to all other windows (maybe).
 wxEVT_DRAW_ITEM, // Leave these three as virtual functions in wxControl?? Platform-specific.
 wxEVT_MEASURE_ITEM,
 wxEVT_COMPARE_ITEM
*/

// ============================================================================
// event handler and related classes
// ============================================================================

typedef void (wxObject::*wxObjectEventFunction)(wxEvent&);

// we can't have ctors nor base struct in backwards compatibility mode or
// otherwise we won't be able to initialize the objects with an agregate, so
// we have to keep both versions
#if WXWIN_COMPATIBILITY_EVENT_TYPES

struct WXDLLEXPORT wxEventTableEntry
{
    // For some reason, this can't be wxEventType, or VC++ complains.
    int m_eventType;            // main event type
    int m_id;                   // control/menu/toolbar id
    int m_lastId;               // used for ranges of ids
    wxObjectEventFunction m_fn; // function to call: not wxEventFunction,
                                // because of dependency problems

    wxObject* m_callbackUserData;
};

#else // !WXWIN_COMPATIBILITY_EVENT_TYPES

// struct containing the members common to static and dynamic event tables
// entries
struct WXDLLEXPORT wxEventTableEntryBase
{
    wxEventTableEntryBase(int id, int idLast,
                          wxObjectEventFunction fn, wxObject *data)
    {
        m_id = id;
        m_lastId = idLast;
        m_fn = fn;
        m_callbackUserData = data;
    }

    // the range of ids for this entry: if m_lastId == -1, the range consists
    // only of m_id, otherwise it is m_id..m_lastId inclusive
    int m_id, m_lastId;

    // function to call: not wxEventFunction, because of dependency problems
    wxObjectEventFunction m_fn;

    // arbitrary user data asosciated with the callback
    wxObject* m_callbackUserData;
};

// an entry from a static event table
struct WXDLLEXPORT wxEventTableEntry : public wxEventTableEntryBase
{
    wxEventTableEntry(const int& evType, int id, int idLast,
                      wxObjectEventFunction fn, wxObject *data)
                 : wxEventTableEntryBase(id, idLast, fn, data),
                   m_eventType(evType)
    {
    }

    // the reference to event type: this allows us to not care about the
    // (undefined) order in which the event table entries and the event types
    // are initialized: initially the value of this reference might be
    // invalid, but by the time it is used for the first time, all global
    // objects will have been initialized (including the event type constants)
    // and so it will have the correct value when it is needed
    const int& m_eventType;
};

// an entry used in dynamic event table managed by wxEvtHandler::Connect()
struct WXDLLEXPORT wxDynamicEventTableEntry : public wxEventTableEntryBase
{
    wxDynamicEventTableEntry(int evType, int id, int idLast,
                             wxObjectEventFunction fn, wxObject *data)
        : wxEventTableEntryBase(id, idLast, fn, data)
    {
        m_eventType = evType;
    }

    // not a reference here as we can't keep a reference to a temporary int
    // created to wrap the constant value typically passed to Connect() - nor
    // do we need it
    int m_eventType;
};

#endif // !WXWIN_COMPATIBILITY_EVENT_TYPES

// ----------------------------------------------------------------------------
// wxEventTable: an array of event entries terminated with {0, 0, 0, 0, 0}
// ----------------------------------------------------------------------------
struct WXDLLEXPORT wxEventTable
{
    const wxEventTable *baseTable;    // base event table (next in chain)
    const wxEventTableEntry *entries; // bottom of entry array
};

// ----------------------------------------------------------------------------
// wxEvtHandler: the base class for all objects handling wxWindows events
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxEvtHandler : public wxObject
{
public:
    wxEvtHandler();
    virtual ~wxEvtHandler();

    wxEvtHandler *GetNextHandler() const { return m_nextHandler; }
    wxEvtHandler *GetPreviousHandler() const { return m_previousHandler; }
    void SetNextHandler(wxEvtHandler *handler) { m_nextHandler = handler; }
    void SetPreviousHandler(wxEvtHandler *handler) { m_previousHandler = handler; }

    void SetEvtHandlerEnabled(bool enabled) { m_enabled = enabled; }
    bool GetEvtHandlerEnabled() const { return m_enabled; }

    // process an event right now
    virtual bool ProcessEvent(wxEvent& event);

    // add an event to be processed later
    void AddPendingEvent(wxEvent& event);

    // process all pending events
    void ProcessPendingEvents();

    // add a
#if wxUSE_THREADS
    bool ProcessThreadEvent(wxEvent& event);
#endif

    // Dynamic association of a member function handler with the event handler,
    // id and event type
    void Connect( int id, int lastId, int eventType,
                  wxObjectEventFunction func,
                  wxObject *userData = (wxObject *) NULL );

    // Convenience function: take just one id
    void Connect( int id, int eventType,
                  wxObjectEventFunction func,
                  wxObject *userData = (wxObject *) NULL )
        { Connect(id, -1, eventType, func, userData); }

    bool Disconnect( int id, int lastId = -1, wxEventType eventType = wxEVT_NULL,
                  wxObjectEventFunction func = NULL,
                  wxObject *userData = (wxObject *) NULL );

    // Convenience function: take just one id
    bool Disconnect( int id, wxEventType eventType = wxEVT_NULL,
                  wxObjectEventFunction func = NULL,
                  wxObject *userData = (wxObject *) NULL )
        { return Disconnect(id, -1, eventType, func, userData); }

    // implementation from now on
    virtual bool SearchEventTable(wxEventTable& table, wxEvent& event);
    bool SearchDynamicEventTable( wxEvent& event );

#if wxUSE_THREADS
    void ClearEventLocker()
   {
#  if !defined(__VISAGECPP__)
      delete m_eventsLocker;
      m_eventsLocker = NULL;
#endif
   };
#endif

    // old stuff

#if WXWIN_COMPATIBILITY_2
    virtual void OnCommand(wxWindow& WXUNUSED(win),
                           wxCommandEvent& WXUNUSED(event))
    {
        wxFAIL_MSG(wxT("shouldn't be called any more"));
    }

    // Called if child control has no callback function
    virtual long Default()
        { return GetNextHandler() ? GetNextHandler()->Default() : 0; };
#endif // WXWIN_COMPATIBILITY_2

#if WXWIN_COMPATIBILITY
    virtual bool OnClose();
#endif

private:
    static const wxEventTableEntry sm_eventTableEntries[];

protected:
    static const wxEventTable sm_eventTable;

    virtual const wxEventTable *GetEventTable() const;

    wxEvtHandler*       m_nextHandler;
    wxEvtHandler*       m_previousHandler;
    wxList*             m_dynamicEvents;
    wxList*             m_pendingEvents;

#if wxUSE_THREADS
#if defined (__VISAGECPP__)
    wxCriticalSection   m_eventsLocker;
#  else
    wxCriticalSection*  m_eventsLocker;
#  endif
#endif

    // optimization: instead of using costly IsKindOf() to decide whether we're
    // a window (which is true in 99% of cases), use this flag
    bool                m_isWindow;

    // Is event handler enabled?
    bool                m_enabled;

private:
    DECLARE_DYNAMIC_CLASS(wxEvtHandler)
};

typedef void (wxEvtHandler::*wxEventFunction)(wxEvent&);
#if wxUSE_GUI
typedef void (wxEvtHandler::*wxCommandEventFunction)(wxCommandEvent&);
typedef void (wxEvtHandler::*wxScrollEventFunction)(wxScrollEvent&);
typedef void (wxEvtHandler::*wxScrollWinEventFunction)(wxScrollWinEvent&);
typedef void (wxEvtHandler::*wxSizeEventFunction)(wxSizeEvent&);
typedef void (wxEvtHandler::*wxMoveEventFunction)(wxMoveEvent&);
typedef void (wxEvtHandler::*wxPaintEventFunction)(wxPaintEvent&);
typedef void (wxEvtHandler::*wxEraseEventFunction)(wxEraseEvent&);
typedef void (wxEvtHandler::*wxMouseEventFunction)(wxMouseEvent&);
typedef void (wxEvtHandler::*wxCharEventFunction)(wxKeyEvent&);
typedef void (wxEvtHandler::*wxFocusEventFunction)(wxFocusEvent&);
typedef void (wxEvtHandler::*wxActivateEventFunction)(wxActivateEvent&);
typedef void (wxEvtHandler::*wxMenuEventFunction)(wxMenuEvent&);
typedef void (wxEvtHandler::*wxJoystickEventFunction)(wxJoystickEvent&);
typedef void (wxEvtHandler::*wxDropFilesEventFunction)(wxDropFilesEvent&);
typedef void (wxEvtHandler::*wxInitDialogEventFunction)(wxInitDialogEvent&);
typedef void (wxEvtHandler::*wxSysColourChangedFunction)(wxSysColourChangedEvent&);
typedef void (wxEvtHandler::*wxUpdateUIEventFunction)(wxUpdateUIEvent&);
typedef void (wxEvtHandler::*wxIdleEventFunction)(wxIdleEvent&);
typedef void (wxEvtHandler::*wxCloseEventFunction)(wxCloseEvent&);
typedef void (wxEvtHandler::*wxShowEventFunction)(wxShowEvent&);
typedef void (wxEvtHandler::*wxIconizeEventFunction)(wxShowEvent&);
typedef void (wxEvtHandler::*wxMaximizeEventFunction)(wxShowEvent&);
typedef void (wxEvtHandler::*wxNavigationKeyEventFunction)(wxNavigationKeyEvent&);
typedef void (wxEvtHandler::*wxPaletteChangedEventFunction)(wxPaletteChangedEvent&);
typedef void (wxEvtHandler::*wxQueryNewPaletteEventFunction)(wxQueryNewPaletteEvent&);
typedef void (wxEvtHandler::*wxWindowCreateEventFunction)(wxWindowCreateEvent&);
typedef void (wxEvtHandler::*wxWindowDestroyEventFunction)(wxWindowDestroyEvent&);
typedef void (wxEvtHandler::*wxSetCursorEventFunction)(wxSetCursorEvent&);
typedef void (wxEvtHandler::*wxNotifyEventFunction)(wxNotifyEvent&);
typedef void (wxEvtHandler::*wxHelpEventFunction)(wxHelpEvent&);
#endif // wxUSE_GUI

// N.B. In GNU-WIN32, you *have* to take the address of a member function
// (use &) or the compiler crashes...

#define DECLARE_EVENT_TABLE() \
    private: \
        static const wxEventTableEntry sm_eventTableEntries[]; \
    protected: \
        static const wxEventTable        sm_eventTable; \
        virtual const wxEventTable*        GetEventTable() const;

#define BEGIN_EVENT_TABLE(theClass, baseClass) \
    const wxEventTable *theClass::GetEventTable() const \
        { return &theClass::sm_eventTable; } \
    const wxEventTable theClass::sm_eventTable = \
        { &baseClass::sm_eventTable, &theClass::sm_eventTableEntries[0] }; \
    const wxEventTableEntry theClass::sm_eventTableEntries[] = { \

#define END_EVENT_TABLE() DECLARE_EVENT_TABLE_ENTRY( 0, 0, 0, 0, 0 ) };

/*
 * Event table macros
 */

// Generic events
#define EVT_CUSTOM(event, id, func) DECLARE_EVENT_TABLE_ENTRY( event, id, -1, (wxObjectEventFunction) (wxEventFunction) & func, (wxObject *) NULL ),
#define EVT_CUSTOM_RANGE(event, id1, id2, func) DECLARE_EVENT_TABLE_ENTRY( event, id1, id2, (wxObjectEventFunction) (wxEventFunction) & func, (wxObject *) NULL ),

// Miscellaneous
#define EVT_SIZE(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SIZE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxSizeEventFunction) & func, (wxObject *) NULL ),
#define EVT_MOVE(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_MOVE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMoveEventFunction) & func, (wxObject *) NULL ),
#define EVT_CLOSE(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_CLOSE_WINDOW, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxCloseEventFunction) & func, (wxObject *) NULL ),
#define EVT_END_SESSION(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_END_SESSION, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxCloseEventFunction) & func, (wxObject *) NULL ),
#define EVT_QUERY_END_SESSION(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_QUERY_END_SESSION, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxCloseEventFunction) & func, (wxObject *) NULL ),
#define EVT_PAINT(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_PAINT, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxPaintEventFunction) & func, (wxObject *) NULL ),
#define EVT_ERASE_BACKGROUND(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_ERASE_BACKGROUND, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxEraseEventFunction) & func, (wxObject *) NULL ),
#define EVT_CHAR(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_CHAR, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction) & func, (wxObject *) NULL ),
#define EVT_KEY_DOWN(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_KEY_DOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction) & func, (wxObject *) NULL ),
#define EVT_KEY_UP(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_KEY_UP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction) & func, (wxObject *) NULL ),
#define EVT_CHAR_HOOK(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_CHAR_HOOK, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction) & func, NULL ),
#define EVT_MENU_HIGHLIGHT(id, func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_MENU_HIGHLIGHT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxMenuEventFunction) & func, (wxObject *) NULL ),
#define EVT_MENU_HIGHLIGHT_ALL(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_MENU_HIGHLIGHT, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMenuEventFunction) & func, (wxObject *) NULL ),
#define EVT_SET_FOCUS(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SET_FOCUS, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxFocusEventFunction) & func, (wxObject *) NULL ),
#define EVT_KILL_FOCUS(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_KILL_FOCUS, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxFocusEventFunction) & func, (wxObject *) NULL ),
#define EVT_ACTIVATE(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_ACTIVATE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxActivateEventFunction) & func, (wxObject *) NULL ),
#define EVT_ACTIVATE_APP(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_ACTIVATE_APP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxActivateEventFunction) & func, (wxObject *) NULL ),
#define EVT_END_SESSION(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_END_SESSION, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxCloseEventFunction) & func, (wxObject *) NULL ),
#define EVT_QUERY_END_SESSION(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_QUERY_END_SESSION, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxCloseEventFunction) & func, (wxObject *) NULL ),
#define EVT_DROP_FILES(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_DROP_FILES, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxDropFilesEventFunction) & func, (wxObject *) NULL ),
#define EVT_INIT_DIALOG(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_INIT_DIALOG, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxInitDialogEventFunction) & func, (wxObject *) NULL ),
#define EVT_SYS_COLOUR_CHANGED(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SYS_COLOUR_CHANGED, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxSysColourChangedFunction) & func, (wxObject *) NULL ),
#define EVT_SHOW(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SHOW, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxShowEventFunction) & func, (wxObject *) NULL ),
#define EVT_MAXIMIZE(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_MAXIMIZE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMaximizeEventFunction) & func, (wxObject *) NULL ),
#define EVT_ICONIZE(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_ICONIZE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxIconizeEventFunction) & func, (wxObject *) NULL ),
#define EVT_NAVIGATION_KEY(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_NAVIGATION_KEY, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNavigationKeyEventFunction) & func, (wxObject *) NULL ),
#define EVT_PALETTE_CHANGED(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_PALETTE_CHANGED, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxPaletteChangedEventFunction) & func, (wxObject *) NULL ),
#define EVT_QUERY_NEW_PALETTE(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_QUERY_NEW_PALETTE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxQueryNewPaletteEventFunction) & func, (wxObject *) NULL ),
#define EVT_WINDOW_CREATE(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_CREATE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxWindowCreateEventFunction) & func, (wxObject *) NULL ),
#define EVT_WINDOW_DESTROY(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_DESTROY, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxWindowDestroyEventFunction) & func, (wxObject *) NULL ),
#define EVT_SET_CURSOR(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SET_CURSOR, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxSetCursorEventFunction) & func, (wxObject *) NULL ),

// Mouse events
#define EVT_LEFT_DOWN(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_LEFT_DOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),
#define EVT_LEFT_UP(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_LEFT_UP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),
#define EVT_MIDDLE_DOWN(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_MIDDLE_DOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),
#define EVT_MIDDLE_UP(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_MIDDLE_UP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),
#define EVT_RIGHT_DOWN(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_RIGHT_DOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),
#define EVT_RIGHT_UP(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_RIGHT_UP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),
#define EVT_MOTION(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_MOTION, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),
#define EVT_LEFT_DCLICK(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_LEFT_DCLICK, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),
#define EVT_MIDDLE_DCLICK(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_MIDDLE_DCLICK, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),
#define EVT_RIGHT_DCLICK(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_RIGHT_DCLICK, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),
#define EVT_LEAVE_WINDOW(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_LEAVE_WINDOW, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),
#define EVT_ENTER_WINDOW(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_ENTER_WINDOW, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),

// All mouse events
#define EVT_MOUSE_EVENTS(func) \
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_LEFT_DOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),\
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_LEFT_UP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),\
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_MIDDLE_DOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),\
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_MIDDLE_UP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),\
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_RIGHT_DOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),\
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_RIGHT_UP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),\
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_MOTION, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),\
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_LEFT_DCLICK, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),\
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_MIDDLE_DCLICK, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),\
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_RIGHT_DCLICK, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),\
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_ENTER_WINDOW, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),\
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_LEAVE_WINDOW, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxMouseEventFunction) & func, (wxObject *) NULL ),

// EVT_COMMAND
#define EVT_COMMAND(id, event, fn)  DECLARE_EVENT_TABLE_ENTRY( event, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_COMMAND_RANGE(id1, id2, event, fn)  DECLARE_EVENT_TABLE_ENTRY( event, id1, id2, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),

// Scrolling from wxWindow (sent to wxScrolledWindow)
#define EVT_SCROLLWIN(func) \
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_TOP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_BOTTOM, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_LINEUP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_LINEDOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_PAGEUP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_PAGEDOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_THUMBTRACK, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_THUMBRELEASE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),

#define EVT_SCROLLWIN_TOP(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_TOP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),
#define EVT_SCROLLWIN_BOTTOM(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_BOTTOM, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),
#define EVT_SCROLLWIN_LINEUP(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_LINEUP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),
#define EVT_SCROLLWIN_LINEDOWN(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_LINEDOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),
#define EVT_SCROLLWIN_PAGEUP(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_PAGEUP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),
#define EVT_SCROLLWIN_PAGEDOWN(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_PAGEDOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),
#define EVT_SCROLLWIN_THUMBTRACK(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_THUMBTRACK, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),
#define EVT_SCROLLWIN_THUMBRELEASE(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLLWIN_THUMBRELEASE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollWinEventFunction) & func, (wxObject *) NULL ),

// Scrolling from wxSlider and wxScrollBar
#define EVT_SCROLL(func) \
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_TOP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_BOTTOM, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_LINEUP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_LINEDOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_PAGEUP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_PAGEDOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_THUMBTRACK, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_THUMBRELEASE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),

#define EVT_SCROLL_TOP(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_TOP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),
#define EVT_SCROLL_BOTTOM(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_BOTTOM, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),
#define EVT_SCROLL_LINEUP(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_LINEUP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),
#define EVT_SCROLL_LINEDOWN(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_LINEDOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),
#define EVT_SCROLL_PAGEUP(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_PAGEUP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),
#define EVT_SCROLL_PAGEDOWN(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_PAGEDOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),
#define EVT_SCROLL_THUMBTRACK(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_THUMBTRACK, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),
#define EVT_SCROLL_THUMBRELEASE(func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_THUMBRELEASE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),

// Scrolling from wxSlider and wxScrollBar, with an id
#define EVT_COMMAND_SCROLL(id, func) \
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_TOP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_BOTTOM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_LINEUP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_LINEDOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_PAGEUP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_PAGEDOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_THUMBTRACK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),\
  DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_THUMBRELEASE, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),

#define EVT_COMMAND_SCROLL_TOP(id, func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_TOP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),
#define EVT_COMMAND_SCROLL_BOTTOM(id, func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_BOTTOM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),
#define EVT_COMMAND_SCROLL_LINEUP(id, func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_LINEUP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),
#define EVT_COMMAND_SCROLL_LINEDOWN(id, func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_LINEDOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),
#define EVT_COMMAND_SCROLL_PAGEUP(id, func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_PAGEUP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),
#define EVT_COMMAND_SCROLL_PAGEDOWN(id, func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_PAGEDOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),
#define EVT_COMMAND_SCROLL_THUMBTRACK(id, func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_THUMBTRACK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),
#define EVT_COMMAND_SCROLL_THUMBRELEASE(id, func) DECLARE_EVENT_TABLE_ENTRY( wxEVT_SCROLL_THUMBRELEASE, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxScrollEventFunction) & func, (wxObject *) NULL ),

// Convenience macros for commonly-used commands
#define EVT_BUTTON(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_BUTTON_CLICKED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_CHECKBOX(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_CHECKBOX_CLICKED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_CHOICE(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_CHOICE_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_LISTBOX(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LISTBOX_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_LISTBOX_DCLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_TEXT(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_TEXT_UPDATED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_TEXT_ENTER(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_TEXT_ENTER, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_MENU(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_MENU_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_MENU_RANGE(id1, id2, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_MENU_SELECTED, id1, id2, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_SLIDER(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_SLIDER_UPDATED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_RADIOBOX(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_RADIOBOX_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_RADIOBUTTON(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_RADIOBUTTON_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
// EVT_SCROLLBAR is now obsolete since we use EVT_COMMAND_SCROLL... events
#define EVT_SCROLLBAR(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_SCROLLBAR_UPDATED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_VLBOX(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_VLBOX_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_COMBOBOX(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_COMBOBOX_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_TOOL(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_TOOL_CLICKED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_TOOL_RANGE(id1, id2, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_TOOL_CLICKED, id1, id2, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_TOOL_RCLICKED(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_TOOL_RCLICKED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_TOOL_RCLICKED_RANGE(id1, id2, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_TOOL_RCLICKED, id1, id2, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_TOOL_ENTER(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_TOOL_ENTER, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_CHECKLISTBOX(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),

// Generic command events
#define EVT_COMMAND_LEFT_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LEFT_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_COMMAND_LEFT_DCLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LEFT_DCLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_COMMAND_RIGHT_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_RIGHT_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_COMMAND_RIGHT_DCLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_RIGHT_DCLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_COMMAND_SET_FOCUS(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_SET_FOCUS, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_COMMAND_KILL_FOCUS(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_KILL_FOCUS, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),
#define EVT_COMMAND_ENTER(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_ENTER, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL ),

// Joystick events
#define EVT_JOY_DOWN(func) \
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_JOY_BUTTON_DOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxJoystickEventFunction) & func, (wxObject *) NULL ),
#define EVT_JOY_UP(func) \
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_JOY_BUTTON_UP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxJoystickEventFunction) & func, (wxObject *) NULL ),
#define EVT_JOY_MOVE(func) \
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_JOY_MOVE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxJoystickEventFunction) & func, (wxObject *) NULL ),
#define EVT_JOY_ZMOVE(func) \
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_JOY_ZMOVE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxJoystickEventFunction) & func, (wxObject *) NULL ),

// All joystick events
#define EVT_JOYSTICK_EVENTS(func) \
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_JOY_BUTTON_DOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxJoystickEventFunction) & func, (wxObject *) NULL ),\
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_JOY_BUTTON_UP, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxJoystickEventFunction) & func, (wxObject *) NULL ),\
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_JOY_MOVE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxJoystickEventFunction) & func, (wxObject *) NULL ),\
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_JOY_ZMOVE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxJoystickEventFunction) & func, (wxObject *) NULL ),

// Idle event
#define EVT_IDLE(func) \
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_IDLE, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxIdleEventFunction) & func, (wxObject *) NULL ),

// Update UI event
#define EVT_UPDATE_UI(id, func) \
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_UPDATE_UI, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxUpdateUIEventFunction) & func, (wxObject *) NULL ),
#define EVT_UPDATE_UI_RANGE(id1, id2, func) \
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_UPDATE_UI, id1, id2, (wxObjectEventFunction)(wxEventFunction)(wxUpdateUIEventFunction)&func, (wxObject *) NULL ),

// Help events
#define EVT_HELP(id, func) \
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_HELP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxHelpEventFunction) & func, (wxObject *) NULL ),

#define EVT_HELP_RANGE(id1, id2, func) \
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_HELP, id1, id2, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxHelpEventFunction) & func, (wxObject *) NULL ),

#define EVT_DETAILED_HELP(id, func) \
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_DETAILED_HELP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxHelpEventFunction) & func, (wxObject *) NULL ),

#define EVT_DETAILED_HELP_RANGE(id1, id2, func) \
 DECLARE_EVENT_TABLE_ENTRY( wxEVT_DETAILED_HELP, id1, id2, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxHelpEventFunction) & func, (wxObject *) NULL ),

// ----------------------------------------------------------------------------
// Global data
// ----------------------------------------------------------------------------

// for pending event processing - notice that there is intentionally no
// WXDLLEXPORT here
extern wxList *wxPendingEvents;
#if wxUSE_THREADS
    extern wxCriticalSection *wxPendingEventsLocker;
#endif

// ----------------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------------

#if wxUSE_GUI

// Find a window with the focus, that is also a descendant of the given window.
// This is used to determine the window to initially send commands to.
wxWindow* wxFindFocusDescendant(wxWindow* ancestor);

#endif // wxUSE_GUI

#endif
        // _WX_EVENTH__
