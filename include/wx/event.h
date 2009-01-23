/////////////////////////////////////////////////////////////////////////////
// Name:        wx/event.h
// Purpose:     Event classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_EVENT_H_
#define _WX_EVENT_H_

#include "wx/defs.h"
#include "wx/cpp.h"
#include "wx/object.h"
#include "wx/clntdata.h"

#if wxUSE_GUI
    #include "wx/gdicmn.h"
    #include "wx/cursor.h"
    #include "wx/mousestate.h"
#endif

#include "wx/dynarray.h"
#include "wx/thread.h"
#include "wx/tracker.h"

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_BASE wxList;
class WXDLLIMPEXP_FWD_BASE wxEvent;
#if wxUSE_GUI
    class WXDLLIMPEXP_FWD_CORE wxDC;
    class WXDLLIMPEXP_FWD_CORE wxMenu;
    class WXDLLIMPEXP_FWD_CORE wxWindow;
    class WXDLLIMPEXP_FWD_CORE wxWindowBase;
#endif // wxUSE_GUI

// We operate with pointer to members of wxEvtHandler (such functions are used
// as event handlers in the event tables or as arguments to Connect()) but by
// default MSVC uses a restricted (but more efficient) representation of
// pointers to members which can't deal with multiple base classes. To avoid
// mysterious (as the compiler is not good enough to detect this and give a
// sensible error message) errors in the user code as soon as it defines
// classes inheriting from both wxEvtHandler (possibly indirectly, e.g. via
// wxWindow) and something else (including our own wxTrackable but not limited
// to it), we use the special MSVC keyword telling the compiler to use a more
// general pointer to member representation for the classes inheriting from
// wxEvtHandler.
#ifdef __VISUALC__
    #define wxMSVC_FWD_MULTIPLE_BASES __multiple_inheritance
#else
    #define wxMSVC_FWD_MULTIPLE_BASES
#endif

class WXDLLIMPEXP_FWD_BASE wxMSVC_FWD_MULTIPLE_BASES wxEvtHandler;
class wxEventConnectionRef;

// ----------------------------------------------------------------------------
// Event types
// ----------------------------------------------------------------------------

typedef int wxEventType;

#define wxEVT_ANY           ((wxEventType)-1)

// this is used to make the event table entry type safe, so that for an event
// handler only a function with proper parameter list can be given.
#define wxStaticCastEvent(type, val) static_cast<type>(val)

#define DECLARE_EVENT_TABLE_ENTRY(type, winid, idLast, fn, obj) \
    wxEventTableEntry(type, winid, idLast, wxNewEventFunctor(type, fn), obj)

#define DECLARE_EVENT_TABLE_TERMINATOR() \
    wxEventTableEntry(wxEVT_NULL, 0, 0, 0, 0)

// obsolete event declaration/definition macros, we don't need them any longer
// but we keep them for compatibility as it doesn't cost us anything anyhow
#define BEGIN_DECLARE_EVENT_TYPES()
#define END_DECLARE_EVENT_TYPES()
#define DECLARE_EXPORTED_EVENT_TYPE(expdecl, name, value) \
    extern expdecl const wxEventType name;
#define DECLARE_EVENT_TYPE(name, value) \
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_CORE, name, value)
#define DECLARE_LOCAL_EVENT_TYPE(name, value) \
    DECLARE_EXPORTED_EVENT_TYPE(wxEMPTY_PARAMETER_VALUE, name, value)
#define DEFINE_EVENT_TYPE(name) const wxEventType name = wxNewEventType();
#define DEFINE_LOCAL_EVENT_TYPE(name) DEFINE_EVENT_TYPE(name)

// generate a new unique event type
extern WXDLLIMPEXP_BASE wxEventType wxNewEventType();

// FIXME: currently the new events code is disabled because it creates too
//        many problems, it should be reenabled a.s.a.p. or removed
#undef wxEVENTS_COMPATIBILITY_2_8
#define wxEVENTS_COMPATIBILITY_2_8 1

// macros to create an event type depending on whether type safe events are
// enabled.

#if wxEVENTS_COMPATIBILITY_2_8
    #define wxDEFINE_EVENT( name, type ) \
        const wxEventType name( wxNewEventType() );

    #define wxDECLARE_EXPORTED_EVENT( expdecl, name, type ) \
        extern const expdecl wxEventType name;

    #define wxDEFINE_EVENT_ALIAS( name, type, value ) \
        const wxEventType name = value;

    #define wxDECLARE_EXPORTED_EVENT_ALIAS( expdecl, name, type ) \
        extern const expdecl wxEventType name;

    #define wxDECLARE_LOCAL_EVENT( name, type ) \
        wxDECLARE_EXPORTED_EVENT( wxEMPTY_PARAMETER_VALUE, name, type )

    #define wxEVENT_HANDLER_CAST( functype, func ) \
        ( wxObjectEventFunction )( wxEventFunction )wxStaticCastEvent( functype, &func )
#else
    #define wxDEFINE_EVENT( name, type ) \
        const wxTypedEventType< type > name( wxNewEventType() );

    #define wxDECLARE_EXPORTED_EVENT( expdecl, name, type ) \
        extern const expdecl wxTypedEventType< type > name;

    #define wxDEFINE_EVENT_ALIAS( name, type, value ) \
        const wxTypedEventType< type > name( value );

    #define wxDECLARE_EXPORTED_EVENT_ALIAS( expdecl, name, type ) \
        extern const expdecl wxTypedEventType< type > name;

    #define wxDECLARE_LOCAL_EVENT( name, type ) \
        wxDECLARE_EXPORTED_EVENT( wxEMPTY_PARAMETER_VALUE, name, type )

    #define wxEVENT_HANDLER_CAST( functype, func ) \
        ( &func )
#endif

// template which associates the correct event object with the event type

#if !wxEVENTS_COMPATIBILITY_2_8

template <typename Event>
class WXDLLIMPEXP_BASE wxTypedEventType
{
public:
    typedef Event CorrespondingEvent;

    wxTypedEventType(wxEventType type) { m_type = type; }

    // used for static event tables
    operator const wxEventType&() const { return m_type; }

private:
    wxEventType m_type;
};

#endif // !wxEVENTS_COMPATIBILITY_2_8

// These are needed for the functor definitions
typedef void (wxEvtHandler::*wxEventFunction)(wxEvent&);

// We had some trouble (specifically with eVC for ARM WinCE build) with using
// wxEventFunction in the past so we had introduced wxObjectEventFunction which
// used to be a typedef for a member of wxObject and not wxEvtHandler to work
// around this but as eVC is not really supported any longer we now only keep
// this for backwards compatibility and, despite its name, this is a typedef
// for wxEvtHandler member now -- but if we have the same problem with another
// compiler we can restore its old definition for it.
typedef wxEventFunction wxObjectEventFunction;


// the functors which will be stored in the static/dynamic tables
class WXDLLIMPEXP_BASE wxEventFunctor
{
public:
    virtual ~wxEventFunctor();

    // this operator is used to actually invoke the event handler
    virtual void operator()(wxEvtHandler *, wxEvent &) = 0;

    // this function tests whether this functor is matched, for the purpose of
    // finding it in an event table in Disconnect(), by the given func
    virtual bool Matches(const wxEventFunctor& func) const = 0;

    virtual wxEvtHandler *GetHandler() const { return NULL; }

    virtual wxObjectEventFunction GetMethod() const { return NULL; }
};

// A plain method functor
class WXDLLIMPEXP_BASE wxObjectEventFunctor : public wxEventFunctor
{
public:
    wxObjectEventFunctor(wxObjectEventFunction method, wxEvtHandler *handler)
    {
        m_handler = handler;
        m_method = method;
    }

    virtual void operator()(wxEvtHandler *handler, wxEvent& event)
    {
        wxEvtHandler * const realHandler = m_handler ? m_handler : handler;

        (realHandler->*m_method)(event);
    }

    virtual bool Matches(const wxEventFunctor& other) const
    {
        wxEvtHandler * const handler = other.GetHandler();

        return (m_handler == handler || !handler) &&
               (m_method == other.GetMethod());
    }

    virtual wxEvtHandler *GetHandler() const { return m_handler; }
    virtual wxObjectEventFunction GetMethod() const { return m_method; }

private:
    wxEvtHandler *m_handler;
    wxObjectEventFunction m_method;
};

// Create a functor for the legacy events: handler can be NULL and its default
// value is used by the event table macros

inline wxObjectEventFunctor *
wxNewEventFunctor(const wxEventType& WXUNUSED(evtType),
                  wxObjectEventFunction method,
                  wxEvtHandler *handler = NULL)
{
    return new wxObjectEventFunctor(method, handler);
}

inline wxObjectEventFunctor
wxConstructEventFunctor(const wxEventType& WXUNUSED(evtType),
                        wxObjectEventFunction method,
                        wxEvtHandler *handler)
{
    return wxObjectEventFunctor(method, handler);
}

#if !wxEVENTS_COMPATIBILITY_2_8

template <typename EventType>
class WXDLLIMPEXP_BASE wxEventFunctorFunction : public wxEventFunctor
{
public:
    wxEventFunctorFunction(void (*handler)(typename EventType::CorrespondingEvent &))
    {
        m_handler = handler;
    }

    virtual void operator()(wxEvtHandler *WXUNUSED(handler), wxEvent& event)
    {
        // Protect against wrong event i.e. wxMouseEvent evt(wxEVT_PAINT):
        wxASSERT( dynamic_cast< typename EventType::CorrespondingEvent * >( &event ) != NULL );

        // Will throw a std::bad_cast exception in release build:
        ( *m_handler )( dynamic_cast< typename EventType::CorrespondingEvent & >( event ));
    }

    virtual bool Matches( const wxEventFunctor &right ) const
    {
        wxEventFunctorFunction const &other = dynamic_cast< wxEventFunctorFunction const & >( right );

        return m_handler == other.m_handler || other.m_handler == NULL;
    }

private:
    void ( *m_handler )( typename EventType::CorrespondingEvent & );
};


template <typename EventType, typename Class, typename Derived>
class WXDLLIMPEXP_BASE wxEventFunctorMethod : public wxEventFunctor
{
public:
    wxEventFunctorMethod( void ( Class::*method )( typename EventType::CorrespondingEvent & ),
            Derived *handler )
    {
        m_handler = handler;
        m_method =  method;
    }

    virtual void operator () ( wxEvtHandler *handler, wxEvent &event )
    {
        // Compile-time type check 1: This requires Derived to derive from or
        // be of the same type as Class
        Class *realHandler = m_handler;

        if( m_handler == NULL )
        {
            // Verify that the handler does indeed derive from the class
            // containing the handler method
            wxASSERT( dynamic_cast< Class * >( handler) != NULL );

            realHandler = dynamic_cast< Class * >( handler );
        }

        // Protect against wrong event i.e. wxMouseEvent evt(wxEVT_PAINT):
        wxASSERT( dynamic_cast< typename EventType::CorrespondingEvent * >( &event ) != NULL );

        // Will throw a std::bad_cast exception in release build:
        ( realHandler->*m_method )( dynamic_cast< typename EventType::CorrespondingEvent & >( event ));
    }

    virtual bool Matches( const wxEventFunctor &right ) const
    {
        wxEventFunctorMethod const &other = dynamic_cast< wxEventFunctorMethod const & >( right );

        return (( m_handler == other.m_handler || other.m_handler == NULL ) &&
                ( m_method == other.m_method || other.m_method == NULL ));
    }

    virtual wxEvtHandler *GetHandler() const
    {
        // This makes sure Derived derives from wxEvtHandler (it is still
        // possible and even ok if Class does not derive from wxEvtHandler. In
        // this case Derived would end up using multiple inheritance: class
        // Derived : public wxEvtHandler, public Class { } where Class contains
        // the method to call, but wxEvtHandler contains the wxTrackable and
        // code for weak ref support
        return m_handler;
    }

    virtual wxObjectEventFunction GetMethod() const
    {
        return reinterpret_cast<wxObjectEventFunction>(m_method);
    }

private:
    Derived *m_handler;
    void (Class::*m_method)(typename EventType::CorrespondingEvent&);
};


template <typename EventType, typename Functor>
class WXDLLIMPEXP_BASE wxEventFunctorAdapter : public wxEventFunctor
{
public:
    wxEventFunctorAdapter( Functor &functor )
    {
        m_functor = functor;
    }

    virtual void operator () ( wxEvtHandler *WXUNUSED( handler ), wxEvent &event )
    {
        // Protect against wrong event i.e. wxMouseEvent evt(wxEVT_PAINT):
        wxASSERT( dynamic_cast< typename EventType::CorrespondingEvent * >( &event ) != NULL );

        // Will throw a std::bad_cast exception in release build:
        m_functor( dynamic_cast< typename EventType::CorrespondingEvent & >( event ));
    }

    virtual bool Matches( const wxEventFunctor &right ) const
    {
        wxEventFunctorAdapter const &other = dynamic_cast< wxEventFunctorAdapter const & >( right );

        return m_functor == other.m_functor;
    }

private:
    Functor m_functor;
};

//
// Create functors for the templatized events (needed in wxEvtHandler::Connect):
//

// Create a functor for functions:

template <typename EventType>
inline wxEventFunctorFunction<EventType> *
wxNewEventFunctor(const EventType &,
                  void (*function)(typename EventType::CorrespondingEvent&))
{
    return new wxEventFunctorFunction<EventType>(function);
}

// Create a functor for methods:

template <typename EventType, typename Class>
inline wxEventFunctorMethod<EventType, Class, Class> *
wxNewEventFunctor(const EventType &,
                  void (Class::*method)(typename EventType::CorrespondingEvent&))
{
    return new wxEventFunctorMethod<EventType, Class, Class>(method, NULL);
}

template <typename EventType, typename Class, typename Derived>
inline wxEventFunctorMethod<EventType, Class, Derived> *
wxNewEventFunctor(const EventType &,
                  void (Class::*method)(typename EventType::CorrespondingEvent &),
                  Derived *handler )
{
    return new wxEventFunctorMethod<EventType, Class, Derived>(method, handler);
}

// Create a functor for arbitrary functors (like boost::function):
template <typename EventType, typename Functor>
inline wxEventFunctorAdapter<EventType, Functor> *
wxNewEventFunctor(const EventType &,
                  Functor& functor )
{
    return new wxEventFunctorAdapter<EventType, Functor>(functor);
}

//
// Construct functors for the templatized events (needed in wxEvtHandler::Disconnect):
//

// Construct a functor for functions:

template <typename EventType>
inline wxEventFunctorFunction<EventType>
wxConstructEventFunctor(const EventType &,
                        void (*function)(typename EventType::CorrespondingEvent&))
{
    return wxEventFunctorFunction<EventType>(function);
}

// Construct a functor for methods:

template <typename EventType, typename Class>
inline wxEventFunctorMethod<EventType, Class, Class>
wxConstructEventFunctor(const EventType &,
                        void (Class::*method)(typename EventType::CorrespondingEvent&))
{
    return wxEventFunctorMethod<EventType, Class, Class>(method, NULL);
}

template <typename EventType, typename Class, typename Derived>
inline wxEventFunctorMethod<EventType, Class, Derived>
wxConstructEventFunctor(const EventType &,
                        void (Class::*method)(typename EventType::CorrespondingEvent&),
                        Derived *handler)
{
    return wxEventFunctorMethod<EventType, Class, Derived>(method, handler);
}

// Construct a functor for arbitrary functors (like boost:function):

template <typename EventType, typename Functor>
inline wxEventFunctorAdapter<EventType, Functor>
wxConstructEventFunctor(const EventType &,
                        Functor& functor)
{
    return wxEventFunctorAdapter<EventType, Functor>(functor);
}

#endif // !wxEVENTS_COMPATIBILITY_2_8

// many, but not all, standard event types

    // some generic events
extern WXDLLIMPEXP_BASE const wxEventType wxEVT_NULL;
extern WXDLLIMPEXP_BASE const wxEventType wxEVT_FIRST;
extern WXDLLIMPEXP_BASE const wxEventType wxEVT_USER_FIRST;

    // Need events declared to do this
class WXDLLIMPEXP_FWD_CORE wxCommandEvent;
class WXDLLIMPEXP_FWD_CORE wxMouseEvent;
class WXDLLIMPEXP_FWD_CORE wxFocusEvent;
class WXDLLIMPEXP_FWD_CORE wxChildFocusEvent;
class WXDLLIMPEXP_FWD_CORE wxKeyEvent;
class WXDLLIMPEXP_FWD_CORE wxNavigationKeyEvent;
class WXDLLIMPEXP_FWD_CORE wxSetCursorEvent;
class WXDLLIMPEXP_FWD_CORE wxScrollEvent;
class WXDLLIMPEXP_FWD_CORE wxSpinEvent;
class WXDLLIMPEXP_FWD_CORE wxScrollWinEvent;
class WXDLLIMPEXP_FWD_CORE wxSizeEvent;
class WXDLLIMPEXP_FWD_CORE wxMoveEvent;
class WXDLLIMPEXP_FWD_CORE wxCloseEvent;
class WXDLLIMPEXP_FWD_CORE wxActivateEvent;
class WXDLLIMPEXP_FWD_CORE wxWindowCreateEvent;
class WXDLLIMPEXP_FWD_CORE wxWindowDestroyEvent;
class WXDLLIMPEXP_FWD_CORE wxShowEvent;
class WXDLLIMPEXP_FWD_CORE wxIconizeEvent;
class WXDLLIMPEXP_FWD_CORE wxMaximizeEvent;
class WXDLLIMPEXP_FWD_CORE wxMouseCaptureChangedEvent;
class WXDLLIMPEXP_FWD_CORE wxMouseCaptureLostEvent;
class WXDLLIMPEXP_FWD_CORE wxPaintEvent;
class WXDLLIMPEXP_FWD_CORE wxEraseEvent;
class WXDLLIMPEXP_FWD_CORE wxNcPaintEvent;
class WXDLLIMPEXP_FWD_CORE wxMenuEvent;
class WXDLLIMPEXP_FWD_CORE wxContextMenuEvent;
class WXDLLIMPEXP_FWD_CORE wxSysColourChangedEvent;
class WXDLLIMPEXP_FWD_CORE wxDisplayChangedEvent;
class WXDLLIMPEXP_FWD_CORE wxQueryNewPaletteEvent;
class WXDLLIMPEXP_FWD_CORE wxPaletteChangedEvent;
class WXDLLIMPEXP_FWD_CORE wxJoystickEvent;
class WXDLLIMPEXP_FWD_CORE wxDropFilesEvent;
class WXDLLIMPEXP_FWD_CORE wxInitDialogEvent;
class WXDLLIMPEXP_FWD_CORE wxIdleEvent;
class WXDLLIMPEXP_FWD_CORE wxUpdateUIEvent;
class WXDLLIMPEXP_FWD_CORE wxClipboardTextEvent;
class WXDLLIMPEXP_FWD_CORE wxHelpEvent;


    // Command events
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_SLIDER_UPDATED, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEvent)

// wxEVT_COMMAND_SCROLLBAR_UPDATED is deprecated, use wxEVT_SCROLL... events
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_SCROLLBAR_UPDATED, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_VLBOX_SELECTED, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_TOOL_RCLICKED, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_TOOL_DROPDOWN_CLICKED, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_TOOL_ENTER, wxCommandEvent)

    // Mouse event types
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_LEFT_DOWN, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_LEFT_UP, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_MIDDLE_DOWN, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_MIDDLE_UP, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_RIGHT_DOWN, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_RIGHT_UP, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_MOTION, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_ENTER_WINDOW, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_LEAVE_WINDOW, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_LEFT_DCLICK, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_MIDDLE_DCLICK, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_RIGHT_DCLICK, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SET_FOCUS, wxFocusEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_KILL_FOCUS, wxFocusEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_CHILD_FOCUS, wxChildFocusEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_MOUSEWHEEL, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_AUX1_DOWN, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_AUX1_UP, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_AUX1_DCLICK, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_AUX2_DOWN, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_AUX2_UP, wxMouseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_AUX2_DCLICK, wxMouseEvent)

    // Character input event type
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_CHAR, wxKeyEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_CHAR_HOOK, wxKeyEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_NAVIGATION_KEY, wxNavigationKeyEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_KEY_DOWN, wxKeyEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_KEY_UP, wxKeyEvent)
#if wxUSE_HOTKEY
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_HOTKEY, wxKeyEvent)
#endif
    // Set cursor event
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SET_CURSOR, wxSetCursorEvent)

    // wxScrollBar and wxSlider event identifiers
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLL_TOP, wxScrollEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLL_BOTTOM, wxScrollEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLL_LINEUP, wxScrollEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLL_LINEDOWN, wxScrollEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLL_PAGEUP, wxScrollEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLL_PAGEDOWN, wxScrollEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLL_THUMBTRACK, wxScrollEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLL_THUMBRELEASE, wxScrollEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLL_CHANGED, wxScrollEvent)

// Due to a bug in older wx versions, wxSpinEvents were being sent with type of
// wxEVT_SCROLL_LINEUP, wxEVT_SCROLL_LINEDOWN and wxEVT_SCROLL_THUMBTRACK. But
// with the type-safe events in place, these event types are associated with
// wxScrollEvent. To allow handling of spin events, new event types have been
// defined in spinbutt.h/spinnbuttcmn.cpp. To maintain backward compatibility
// the spin event types are being initialized with the scroll event types.

#if wxUSE_SPINBTN

wxDECLARE_EXPORTED_EVENT_ALIAS( WXDLLIMPEXP_CORE, wxEVT_SPIN_UP,   wxSpinEvent )
wxDECLARE_EXPORTED_EVENT_ALIAS( WXDLLIMPEXP_CORE, wxEVT_SPIN_DOWN, wxSpinEvent )
wxDECLARE_EXPORTED_EVENT_ALIAS( WXDLLIMPEXP_CORE, wxEVT_SPIN,      wxSpinEvent )

#endif

    // Scroll events from wxWindow
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLLWIN_TOP, wxScrollWinEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLLWIN_BOTTOM, wxScrollWinEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLLWIN_LINEUP, wxScrollWinEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLLWIN_LINEDOWN, wxScrollWinEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLLWIN_PAGEUP, wxScrollWinEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLLWIN_PAGEDOWN, wxScrollWinEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLLWIN_THUMBTRACK, wxScrollWinEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SCROLLWIN_THUMBRELEASE, wxScrollWinEvent)

    // System events
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SIZE, wxSizeEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_MOVE, wxMoveEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_CLOSE_WINDOW, wxCloseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_END_SESSION, wxCloseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_QUERY_END_SESSION, wxCloseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_ACTIVATE_APP, wxActivateEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_ACTIVATE, wxActivateEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_CREATE, wxWindowCreateEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_DESTROY, wxWindowDestroyEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SHOW, wxShowEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_ICONIZE, wxIconizeEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_MAXIMIZE, wxMaximizeEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_MOUSE_CAPTURE_CHANGED, wxMouseCaptureChangedEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_MOUSE_CAPTURE_LOST, wxMouseCaptureLostEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_PAINT, wxPaintEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_ERASE_BACKGROUND, wxEraseEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_NC_PAINT, wxNcPaintEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_MENU_OPEN, wxMenuEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_MENU_CLOSE, wxMenuEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_MENU_HIGHLIGHT, wxMenuEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_CONTEXT_MENU, wxContextMenuEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SYS_COLOUR_CHANGED, wxSysColourChangedEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_DISPLAY_CHANGED, wxDisplayChangedEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_QUERY_NEW_PALETTE, wxQueryNewPaletteEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_PALETTE_CHANGED, wxPaletteChangedEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_JOY_BUTTON_DOWN, wxJoystickEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_JOY_BUTTON_UP, wxJoystickEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_JOY_MOVE, wxJoystickEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_JOY_ZMOVE, wxJoystickEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_DROP_FILES, wxDropFilesEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_INIT_DIALOG, wxInitDialogEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_BASE, wxEVT_IDLE, wxIdleEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_UPDATE_UI, wxUpdateUIEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_SIZING, wxSizeEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_MOVING, wxMoveEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_MOVE_START, wxMoveEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_MOVE_END, wxMoveEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_HIBERNATE, wxActivateEvent)

    // Clipboard events
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_TEXT_COPY, wxClipboardTextEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_TEXT_CUT, wxClipboardTextEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_TEXT_PASTE, wxClipboardTextEvent)

    // Generic command events
    // Note: a click is a higher-level event than button down/up
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_LEFT_CLICK, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_LEFT_DCLICK, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_RIGHT_CLICK, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_RIGHT_DCLICK, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_SET_FOCUS, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_KILL_FOCUS, wxCommandEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_ENTER, wxCommandEvent)

    // Help events
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_HELP, wxHelpEvent)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_DETAILED_HELP, wxHelpEvent)

// these 2 events are the same
#define wxEVT_COMMAND_TOOL_CLICKED wxEVT_COMMAND_MENU_SELECTED

// ----------------------------------------------------------------------------
// Compatibility
// ----------------------------------------------------------------------------

// this event is also used by wxComboBox and wxSpinCtrl which don't include
// wx/textctrl.h in all ports [yet], so declare it here as well
//
// still, any new code using it should include wx/textctrl.h explicitly
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEvent)

// the predefined constants for the number of times we propagate event
// upwards window child-parent chain
enum Propagation_state
{
    // don't propagate it at all
    wxEVENT_PROPAGATE_NONE = 0,

    // propagate it until it is processed
    wxEVENT_PROPAGATE_MAX = INT_MAX
};

/*
 * wxWidgets events, covering all interesting things that might happen
 * (button clicking, resizing, setting text in widgets, etc.).
 *
 * For each completely new event type, derive a new event class.
 * An event CLASS represents a C++ class defining a range of similar event TYPES;
 * examples are canvas events, panel item command events.
 * An event TYPE is a unique identifier for a particular system event,
 * such as a button press or a listbox deselection.
 *
 */

class WXDLLIMPEXP_BASE wxEvent : public wxObject
{
public:
    wxEvent(int winid = 0, wxEventType commandType = wxEVT_NULL );

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
    // is the analog of calling the base class version of a virtual function)
    void Skip(bool skip = true) { m_skipped = skip; }
    bool GetSkipped() const { return m_skipped; }

    // This function is used to create a copy of the event polymorphically and
    // all derived classes must implement it because otherwise wxPostEvent()
    // for them wouldn't work (it needs to do a copy of the event)
    virtual wxEvent *Clone() const = 0;

    // Implementation only: this test is explicitly anti OO and this function
    // exists only for optimization purposes.
    bool IsCommandEvent() const { return m_isCommandEvent; }

    // Determine if this event should be propagating to the parent window.
    bool ShouldPropagate() const
        { return m_propagationLevel != wxEVENT_PROPAGATE_NONE; }

    // Stop an event from propagating to its parent window, returns the old
    // propagation level value
    int StopPropagation()
    {
        int propagationLevel = m_propagationLevel;
        m_propagationLevel = wxEVENT_PROPAGATE_NONE;
        return propagationLevel;
    }

    // Resume the event propagation by restoring the propagation level
    // (returned by StopPropagation())
    void ResumePropagation(int propagationLevel)
    {
        m_propagationLevel = propagationLevel;
    }


    // This is for internal use only and is only called by
    // wxEvtHandler::ProcessEvent() to check whether it's the first time this
    // event is being processed
    bool WasProcessed()
    {
        if ( m_wasProcessed )
            return true;

        m_wasProcessed = true;

        return false;
    }

protected:
    wxObject*         m_eventObject;
    wxEventType       m_eventType;
    long              m_timeStamp;
    int               m_id;

public:
    // m_callbackUserData is for internal usage only
    wxObject*         m_callbackUserData;

protected:
    // the propagation level: while it is positive, we propagate the event to
    // the parent window (if any)
    //
    // this one doesn't have to be public, we don't have to worry about
    // backwards compatibility as it is new
    int               m_propagationLevel;

    bool              m_skipped;
    bool              m_isCommandEvent;

    // initially false but becomes true as soon as WasProcessed() is called for
    // the first time, as this is done only by ProcessEvent() it explains the
    // variable name: it becomes true after ProcessEvent() was called at least
    // once for this event
    bool m_wasProcessed;

protected:
    wxEvent(const wxEvent&);            // for implementing Clone()
    wxEvent& operator=(const wxEvent&); // for derived classes operator=()

private:
    // it needs to access our m_propagationLevel
    friend class WXDLLIMPEXP_FWD_BASE wxPropagateOnce;

    DECLARE_ABSTRACT_CLASS(wxEvent)
};

/*
 * Helper class to temporarily change an event not to propagate.
 */
class WXDLLIMPEXP_BASE wxPropagationDisabler
{
public:
    wxPropagationDisabler(wxEvent& event) : m_event(event)
    {
        m_propagationLevelOld = m_event.StopPropagation();
    }

    ~wxPropagationDisabler()
    {
        m_event.ResumePropagation(m_propagationLevelOld);
    }

private:
    wxEvent& m_event;
    int m_propagationLevelOld;

    DECLARE_NO_COPY_CLASS(wxPropagationDisabler)
};

/*
 * Another one to temporarily lower propagation level.
 */
class WXDLLIMPEXP_BASE wxPropagateOnce
{
public:
    wxPropagateOnce(wxEvent& event) : m_event(event)
    {
        wxASSERT_MSG( m_event.m_propagationLevel > 0,
                        _T("shouldn't be used unless ShouldPropagate()!") );

        m_event.m_propagationLevel--;
    }

    ~wxPropagateOnce()
    {
        m_event.m_propagationLevel++;
    }

private:
    wxEvent& m_event;

    DECLARE_NO_COPY_CLASS(wxPropagateOnce)
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
 wxEVT_COMMAND_TOGGLEBUTTON_CLICKED
*/

class WXDLLIMPEXP_CORE wxCommandEvent : public wxEvent
{
public:
    wxCommandEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);

    wxCommandEvent(const wxCommandEvent& event)
        : wxEvent(event),
          m_cmdString(event.m_cmdString),
          m_commandInt(event.m_commandInt),
          m_extraLong(event.m_extraLong),
          m_clientData(event.m_clientData),
          m_clientObject(event.m_clientObject)
        { }

    // Set/Get client data from controls
    void SetClientData(void* clientData) { m_clientData = clientData; }
    void *GetClientData() const { return m_clientData; }

    // Set/Get client object from controls
    void SetClientObject(wxClientData* clientObject) { m_clientObject = clientObject; }
    wxClientData *GetClientObject() const { return m_clientObject; }

    // Get listbox selection if single-choice
    int GetSelection() const { return m_commandInt; }

    // Set/Get listbox/choice selection string
    void SetString(const wxString& s) { m_cmdString = s; }
    wxString GetString() const;

    // Get checkbox value
    bool IsChecked() const { return m_commandInt != 0; }

    // true if the listbox event was a selection.
    bool IsSelection() const { return (m_extraLong != 0); }

    void SetExtraLong(long extraLong) { m_extraLong = extraLong; }
    long GetExtraLong() const { return m_extraLong; }

    void SetInt(int i) { m_commandInt = i; }
    int GetInt() const { return m_commandInt; }

    virtual wxEvent *Clone() const { return new wxCommandEvent(*this); }

protected:
    wxString          m_cmdString;     // String event argument
    int               m_commandInt;
    long              m_extraLong;     // Additional information (e.g. select/deselect)
    void*             m_clientData;    // Arbitrary client data
    wxClientData*     m_clientObject;  // Arbitrary client object

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxCommandEvent)
};

// this class adds a possibility to react (from the user) code to a control
// notification: allow or veto the operation being reported.
class WXDLLIMPEXP_CORE wxNotifyEvent  : public wxCommandEvent
{
public:
    wxNotifyEvent(wxEventType commandType = wxEVT_NULL, int winid = 0)
        : wxCommandEvent(commandType, winid)
        { m_bAllow = true; }

    wxNotifyEvent(const wxNotifyEvent& event)
        : wxCommandEvent(event)
        { m_bAllow = event.m_bAllow; }

    // veto the operation (usually it's allowed by default)
    void Veto() { m_bAllow = false; }

    // allow the operation if it was disabled by default
    void Allow() { m_bAllow = true; }

    // for implementation code only: is the operation allowed?
    bool IsAllowed() const { return m_bAllow; }

    virtual wxEvent *Clone() const { return new wxNotifyEvent(*this); }

private:
    bool m_bAllow;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxNotifyEvent)
};

// Scroll event class, derived form wxCommandEvent. wxScrollEvents are
// sent by wxSlider and wxScrollBar.
/*
 wxEVT_SCROLL_TOP
 wxEVT_SCROLL_BOTTOM
 wxEVT_SCROLL_LINEUP
 wxEVT_SCROLL_LINEDOWN
 wxEVT_SCROLL_PAGEUP
 wxEVT_SCROLL_PAGEDOWN
 wxEVT_SCROLL_THUMBTRACK
 wxEVT_SCROLL_THUMBRELEASE
 wxEVT_SCROLL_CHANGED
*/

class WXDLLIMPEXP_CORE wxScrollEvent : public wxCommandEvent
{
public:
    wxScrollEvent(wxEventType commandType = wxEVT_NULL,
                  int winid = 0, int pos = 0, int orient = 0);

    int GetOrientation() const { return (int) m_extraLong; }
    int GetPosition() const { return m_commandInt; }
    void SetOrientation(int orient) { m_extraLong = (long) orient; }
    void SetPosition(int pos) { m_commandInt = pos; }

    virtual wxEvent *Clone() const { return new wxScrollEvent(*this); }

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxScrollEvent)
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

class WXDLLIMPEXP_CORE wxScrollWinEvent : public wxEvent
{
public:
    wxScrollWinEvent(wxEventType commandType = wxEVT_NULL,
                     int pos = 0, int orient = 0);
    wxScrollWinEvent(const wxScrollWinEvent & event) : wxEvent(event)
        {    m_commandInt = event.m_commandInt;
            m_extraLong = event.m_extraLong;    }

    int GetOrientation() const { return (int) m_extraLong; }
    int GetPosition() const { return m_commandInt; }
    void SetOrientation(int orient) { m_extraLong = (long) orient; }
    void SetPosition(int pos) { m_commandInt = pos; }

    virtual wxEvent *Clone() const { return new wxScrollWinEvent(*this); }

protected:
    int               m_commandInt;
    long              m_extraLong;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxScrollWinEvent)
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

// the symbolic names for the mouse buttons
enum
{
    wxMOUSE_BTN_ANY     = -1,
    wxMOUSE_BTN_NONE    = 0,
    wxMOUSE_BTN_LEFT    = 1,
    wxMOUSE_BTN_MIDDLE  = 2,
    wxMOUSE_BTN_RIGHT   = 3,
    wxMOUSE_BTN_AUX1    = 4,
    wxMOUSE_BTN_AUX2    = 5,
    wxMOUSE_BTN_MAX
};

class WXDLLIMPEXP_CORE wxMouseEvent : public wxEvent,
                                      public wxMouseState
{
public:
    wxMouseEvent(wxEventType mouseType = wxEVT_NULL);
    wxMouseEvent(const wxMouseEvent& event)
        : wxEvent(event),
          wxMouseState(event)
    {
        Assign(event);
    }

    // Was it a button event? (*doesn't* mean: is any button *down*?)
    bool IsButton() const { return Button(wxMOUSE_BTN_ANY); }

    // Was it a down event from this (or any) button?
    bool ButtonDown(int but = wxMOUSE_BTN_ANY) const;

    // Was it a dclick event from this (or any) button?
    bool ButtonDClick(int but = wxMOUSE_BTN_ANY) const;

    // Was it a up event from this (or any) button?
    bool ButtonUp(int but = wxMOUSE_BTN_ANY) const;

    // Was the given button?
    bool Button(int but) const;

    // Was the given button in Down state?
    bool ButtonIsDown(int but) const;

    // Get the button which is changing state (wxMOUSE_BTN_NONE if none)
    int GetButton() const;

    // Find which event was just generated
    bool LeftDown() const { return (m_eventType == wxEVT_LEFT_DOWN); }
    bool MiddleDown() const { return (m_eventType == wxEVT_MIDDLE_DOWN); }
    bool RightDown() const { return (m_eventType == wxEVT_RIGHT_DOWN); }
    bool Aux1Down() const { return (m_eventType == wxEVT_AUX1_DOWN); }
    bool Aux2Down() const { return (m_eventType == wxEVT_AUX2_DOWN); }

    bool LeftUp() const { return (m_eventType == wxEVT_LEFT_UP); }
    bool MiddleUp() const { return (m_eventType == wxEVT_MIDDLE_UP); }
    bool RightUp() const { return (m_eventType == wxEVT_RIGHT_UP); }
    bool Aux1Up() const { return (m_eventType == wxEVT_AUX1_UP); }
    bool Aux2Up() const { return (m_eventType == wxEVT_AUX2_UP); }

    bool LeftDClick() const { return (m_eventType == wxEVT_LEFT_DCLICK); }
    bool MiddleDClick() const { return (m_eventType == wxEVT_MIDDLE_DCLICK); }
    bool RightDClick() const { return (m_eventType == wxEVT_RIGHT_DCLICK); }
    bool Aux1DClick() const { return (m_eventType == wxEVT_AUX1_UP); }
    bool Aux2DClick() const { return (m_eventType == wxEVT_AUX2_UP); }

    // Find the current state of the mouse buttons (regardless
    // of current event type)
    bool LeftIsDown() const { return m_leftDown; }
    bool MiddleIsDown() const { return m_middleDown; }
    bool RightIsDown() const { return m_rightDown; }
    bool Aux1IsDown() const { return m_aux1Down; }
    bool Aux2IsDown() const { return m_aux2Down; }

    // True if a button is down and the mouse is moving
    bool Dragging() const
    {
        return (m_eventType == wxEVT_MOTION) && ButtonIsDown(wxMOUSE_BTN_ANY);
    }

    // True if the mouse is moving, and no button is down
    bool Moving() const
    {
        return (m_eventType == wxEVT_MOTION) && !ButtonIsDown(wxMOUSE_BTN_ANY);
    }

    // True if the mouse is just entering the window
    bool Entering() const { return (m_eventType == wxEVT_ENTER_WINDOW); }

    // True if the mouse is just leaving the window
    bool Leaving() const { return (m_eventType == wxEVT_LEAVE_WINDOW); }

    // Returns the number of mouse clicks associated with this event.
    int GetClickCount() const { return m_clickCount; }


    // Find the position of the event
    void GetPosition(wxCoord *xpos, wxCoord *ypos) const
    {
        if (xpos)
            *xpos = m_x;
        if (ypos)
            *ypos = m_y;
    }

    void GetPosition(long *xpos, long *ypos) const
    {
        if (xpos)
            *xpos = (long)m_x;
        if (ypos)
            *ypos = (long)m_y;
    }

    // Find the position of the event
    wxPoint GetPosition() const { return wxPoint(m_x, m_y); }

    // Find the logical position of the event given the DC
    wxPoint GetLogicalPosition(const wxDC& dc) const;

    // Get X position
    wxCoord GetX() const { return m_x; }

    // Get Y position
    wxCoord GetY() const { return m_y; }

    // Get wheel rotation, positive or negative indicates direction of
    // rotation.  Current devices all send an event when rotation is equal to
    // +/-WheelDelta, but this allows for finer resolution devices to be
    // created in the future.  Because of this you shouldn't assume that one
    // event is equal to 1 line or whatever, but you should be able to either
    // do partial line scrolling or wait until +/-WheelDelta rotation values
    // have been accumulated before scrolling.
    int GetWheelRotation() const { return m_wheelRotation; }

    // Get wheel delta, normally 120.  This is the threshold for action to be
    // taken, and one such action (for example, scrolling one increment)
    // should occur for each delta.
    int GetWheelDelta() const { return m_wheelDelta; }

    // Gets the axis the wheel operation concerns, 0 being the y axis as on
    // most mouse wheels, 1 is the x axis for things like MightyMouse scrolls
    // or horizontal trackpad scrolling
    int GetWheelAxis() const { return m_wheelAxis; }

    // Returns the configured number of lines (or whatever) to be scrolled per
    // wheel action.  Defaults to one.
    int GetLinesPerAction() const { return m_linesPerAction; }

    // Is the system set to do page scrolling?
    bool IsPageScroll() const { return ((unsigned int)m_linesPerAction == UINT_MAX); }

    virtual wxEvent *Clone() const { return new wxMouseEvent(*this); }

    wxMouseEvent& operator=(const wxMouseEvent& event)
    {
        if (&event != this)
            Assign(event);
        return *this;
    }

public:
    wxCoord m_x, m_y;

    bool          m_leftDown;
    bool          m_middleDown;
    bool          m_rightDown;
    bool          m_aux1Down;
    bool          m_aux2Down;

    int           m_clickCount;

    int           m_wheelAxis;
    int           m_wheelRotation;
    int           m_wheelDelta;
    int           m_linesPerAction;

protected:
    void Assign(const wxMouseEvent& evt);

private:
    DECLARE_DYNAMIC_CLASS(wxMouseEvent)
};

// Cursor set event

/*
   wxEVT_SET_CURSOR
 */

class WXDLLIMPEXP_CORE wxSetCursorEvent : public wxEvent
{
public:
    wxSetCursorEvent(wxCoord x = 0, wxCoord y = 0)
        : wxEvent(0, wxEVT_SET_CURSOR),
          m_x(x), m_y(y), m_cursor()
        { }

    wxSetCursorEvent(const wxSetCursorEvent & event)
        : wxEvent(event),
          m_x(event.m_x),
          m_y(event.m_y),
          m_cursor(event.m_cursor)
        { }

    wxCoord GetX() const { return m_x; }
    wxCoord GetY() const { return m_y; }

    void SetCursor(const wxCursor& cursor) { m_cursor = cursor; }
    const wxCursor& GetCursor() const { return m_cursor; }
    bool HasCursor() const { return m_cursor.Ok(); }

    virtual wxEvent *Clone() const { return new wxSetCursorEvent(*this); }

private:
    wxCoord  m_x, m_y;
    wxCursor m_cursor;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxSetCursorEvent)
};

// Keyboard input event class

/*
 wxEVT_CHAR
 wxEVT_CHAR_HOOK
 wxEVT_KEY_DOWN
 wxEVT_KEY_UP
 wxEVT_HOTKEY
 */

class WXDLLIMPEXP_CORE wxKeyEvent : public wxEvent,
                                    public wxKeyboardState
{
public:
    wxKeyEvent(wxEventType keyType = wxEVT_NULL);
    wxKeyEvent(const wxKeyEvent& evt);

    // get the key code: an ASCII7 char or an element of wxKeyCode enum
    int GetKeyCode() const { return (int)m_keyCode; }

#if wxUSE_UNICODE
    // get the Unicode character corresponding to this key
    wxChar GetUnicodeKey() const { return m_uniChar; }
#endif // wxUSE_UNICODE

    // get the raw key code (platform-dependent)
    wxUint32 GetRawKeyCode() const { return m_rawCode; }

    // get the raw key flags (platform-dependent)
    wxUint32 GetRawKeyFlags() const { return m_rawFlags; }

    // Find the position of the event
    void GetPosition(wxCoord *xpos, wxCoord *ypos) const
    {
        if (xpos) *xpos = m_x;
        if (ypos) *ypos = m_y;
    }

    void GetPosition(long *xpos, long *ypos) const
    {
        if (xpos) *xpos = (long)m_x;
        if (ypos) *ypos = (long)m_y;
    }

    wxPoint GetPosition() const
        { return wxPoint(m_x, m_y); }

    // Get X position
    wxCoord GetX() const { return m_x; }

    // Get Y position
    wxCoord GetY() const { return m_y; }

    virtual wxEvent *Clone() const { return new wxKeyEvent(*this); }

    // we do need to copy wxKeyEvent sometimes (in wxTreeCtrl code, for
    // example)
    wxKeyEvent& operator=(const wxKeyEvent& evt)
    {
        if ( &evt != this )
        {
            wxEvent::operator=(evt);

            // Borland C++ 5.82 doesn't compile an explicit call to an
            // implicitly defined operator=() so need to do it this way:
            *static_cast<wxKeyboardState *>(this) = evt;

            m_x = evt.m_x;
            m_y = evt.m_y;

            m_keyCode = evt.m_keyCode;

            m_scanCode = evt.m_scanCode;
            m_rawCode = evt.m_rawCode;
            m_rawFlags = evt.m_rawFlags;
#if wxUSE_UNICODE
            m_uniChar = evt.m_uniChar;
#endif
        }
        return *this;
    }

public:
    wxCoord       m_x, m_y;

    long          m_keyCode;

    // FIXME: what is this for? relation to m_rawXXX?
    bool          m_scanCode;

#if wxUSE_UNICODE
    // This contains the full Unicode character
    // in a character events in Unicode mode
    wxChar        m_uniChar;
#endif

    // these fields contain the platform-specific information about
    // key that was pressed
    wxUint32      m_rawCode;
    wxUint32      m_rawFlags;

private:
    DECLARE_DYNAMIC_CLASS(wxKeyEvent)
};

// Size event class
/*
 wxEVT_SIZE
 */

class WXDLLIMPEXP_CORE wxSizeEvent : public wxEvent
{
public:
    wxSizeEvent() : wxEvent(0, wxEVT_SIZE)
        { }
    wxSizeEvent(const wxSize& sz, int winid = 0)
        : wxEvent(winid, wxEVT_SIZE),
          m_size(sz)
        { }
    wxSizeEvent(const wxSizeEvent & event)
        : wxEvent(event),
          m_size(event.m_size), m_rect(event.m_rect)
        { }
    wxSizeEvent(const wxRect& rect, int id = 0)
        : m_size(rect.GetSize()), m_rect(rect)
        { m_eventType = wxEVT_SIZING; m_id = id; }

    wxSize GetSize() const { return m_size; }
    wxRect GetRect() const { return m_rect; }
    void SetRect(const wxRect& rect) { m_rect = rect; }

    virtual wxEvent *Clone() const { return new wxSizeEvent(*this); }

public:
    // For internal usage only. Will be converted to protected members.
    wxSize m_size;
    wxRect m_rect; // Used for wxEVT_SIZING

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxSizeEvent)
};

// Move event class

/*
 wxEVT_MOVE
 */

class WXDLLIMPEXP_CORE wxMoveEvent : public wxEvent
{
public:
    wxMoveEvent()
        : wxEvent(0, wxEVT_MOVE)
        { }
    wxMoveEvent(const wxPoint& pos, int winid = 0)
        : wxEvent(winid, wxEVT_MOVE),
          m_pos(pos)
        { }
    wxMoveEvent(const wxMoveEvent& event)
        : wxEvent(event),
          m_pos(event.m_pos)
    { }
    wxMoveEvent(const wxRect& rect, int id = 0)
        : m_pos(rect.GetPosition()), m_rect(rect)
        { m_eventType = wxEVT_MOVING; m_id = id; }

    wxPoint GetPosition() const { return m_pos; }
    void SetPosition(const wxPoint& pos) { m_pos = pos; }
    wxRect GetRect() const { return m_rect; }
    void SetRect(const wxRect& rect) { m_rect = rect; }

    virtual wxEvent *Clone() const { return new wxMoveEvent(*this); }

protected:
    wxPoint m_pos;
    wxRect m_rect;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxMoveEvent)
};

// Paint event class
/*
 wxEVT_PAINT
 wxEVT_NC_PAINT
 wxEVT_PAINT_ICON
 */

#if defined(__WXDEBUG__) && (defined(__WXMSW__) || defined(__WXPM__))
    // see comments in src/msw|os2/dcclient.cpp where g_isPainting is defined
    extern WXDLLIMPEXP_CORE int g_isPainting;
#endif // debug

class WXDLLIMPEXP_CORE wxPaintEvent : public wxEvent
{
public:
    wxPaintEvent(int Id = 0)
        : wxEvent(Id, wxEVT_PAINT)
    {
#if defined(__WXDEBUG__) && (defined(__WXMSW__) || defined(__WXPM__))
        // set the internal flag for the duration of processing of WM_PAINT
        g_isPainting++;
#endif // debug
    }

    // default copy ctor and dtor are normally fine, we only need them to keep
    // g_isPainting updated in debug build
#if defined(__WXDEBUG__) && (defined(__WXMSW__) || defined(__WXPM__))
    wxPaintEvent(const wxPaintEvent& event)
            : wxEvent(event)
    {
        g_isPainting++;
    }

    virtual ~wxPaintEvent()
    {
        g_isPainting--;
    }
#endif // debug

    virtual wxEvent *Clone() const { return new wxPaintEvent(*this); }

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxPaintEvent)
};

class WXDLLIMPEXP_CORE wxNcPaintEvent : public wxEvent
{
public:
    wxNcPaintEvent(int winid = 0)
        : wxEvent(winid, wxEVT_NC_PAINT)
        { }

    virtual wxEvent *Clone() const { return new wxNcPaintEvent(*this); }

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxNcPaintEvent)
};

// Erase background event class
/*
 wxEVT_ERASE_BACKGROUND
 */

class WXDLLIMPEXP_CORE wxEraseEvent : public wxEvent
{
public:
    wxEraseEvent(int Id = 0, wxDC *dc = NULL)
        : wxEvent(Id, wxEVT_ERASE_BACKGROUND),
          m_dc(dc)
        { }

    wxEraseEvent(const wxEraseEvent& event)
        : wxEvent(event),
          m_dc(event.m_dc)
        { }

    wxDC *GetDC() const { return m_dc; }

    virtual wxEvent *Clone() const { return new wxEraseEvent(*this); }

protected:
    wxDC *m_dc;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxEraseEvent)
};

// Focus event class
/*
 wxEVT_SET_FOCUS
 wxEVT_KILL_FOCUS
 */

class WXDLLIMPEXP_CORE wxFocusEvent : public wxEvent
{
public:
    wxFocusEvent(wxEventType type = wxEVT_NULL, int winid = 0)
        : wxEvent(winid, type)
        { m_win = NULL; }

    wxFocusEvent(const wxFocusEvent& event)
        : wxEvent(event)
        { m_win = event.m_win; }

    // The window associated with this event is the window which had focus
    // before for SET event and the window which will have focus for the KILL
    // one. NB: it may be NULL in both cases!
    wxWindow *GetWindow() const { return m_win; }
    void SetWindow(wxWindow *win) { m_win = win; }

    virtual wxEvent *Clone() const { return new wxFocusEvent(*this); }

private:
    wxWindow *m_win;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxFocusEvent)
};

// wxChildFocusEvent notifies the parent that a child has got the focus: unlike
// wxFocusEvent it is propagated upwards the window chain
class WXDLLIMPEXP_CORE wxChildFocusEvent : public wxCommandEvent
{
public:
    wxChildFocusEvent(wxWindow *win = NULL);

    wxWindow *GetWindow() const { return (wxWindow *)GetEventObject(); }

    virtual wxEvent *Clone() const { return new wxChildFocusEvent(*this); }

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxChildFocusEvent)
};

// Activate event class
/*
 wxEVT_ACTIVATE
 wxEVT_ACTIVATE_APP
 wxEVT_HIBERNATE
 */

class WXDLLIMPEXP_CORE wxActivateEvent : public wxEvent
{
public:
    wxActivateEvent(wxEventType type = wxEVT_NULL, bool active = true, int Id = 0)
        : wxEvent(Id, type)
        { m_active = active; }
    wxActivateEvent(const wxActivateEvent& event)
        : wxEvent(event)
    { m_active = event.m_active; }

    bool GetActive() const { return m_active; }

    virtual wxEvent *Clone() const { return new wxActivateEvent(*this); }

private:
    bool m_active;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxActivateEvent)
};

// InitDialog event class
/*
 wxEVT_INIT_DIALOG
 */

class WXDLLIMPEXP_CORE wxInitDialogEvent : public wxEvent
{
public:
    wxInitDialogEvent(int Id = 0)
        : wxEvent(Id, wxEVT_INIT_DIALOG)
        { }

    virtual wxEvent *Clone() const { return new wxInitDialogEvent(*this); }

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxInitDialogEvent)
};

// Miscellaneous menu event class
/*
 wxEVT_MENU_OPEN,
 wxEVT_MENU_CLOSE,
 wxEVT_MENU_HIGHLIGHT,
*/

class WXDLLIMPEXP_CORE wxMenuEvent : public wxEvent
{
public:
    wxMenuEvent(wxEventType type = wxEVT_NULL, int winid = 0, wxMenu* menu = NULL)
        : wxEvent(winid, type)
        { m_menuId = winid; m_menu = menu; }
    wxMenuEvent(const wxMenuEvent & event)
        : wxEvent(event)
    { m_menuId = event.m_menuId; m_menu = event.m_menu; }

    // only for wxEVT_MENU_HIGHLIGHT
    int GetMenuId() const { return m_menuId; }

    // only for wxEVT_MENU_OPEN/CLOSE
    bool IsPopup() const { return m_menuId == wxID_ANY; }

    // only for wxEVT_MENU_OPEN/CLOSE
    wxMenu* GetMenu() const { return m_menu; }

    virtual wxEvent *Clone() const { return new wxMenuEvent(*this); }

private:
    int     m_menuId;
    wxMenu* m_menu;

    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxMenuEvent)
};

// Window close or session close event class
/*
 wxEVT_CLOSE_WINDOW,
 wxEVT_END_SESSION,
 wxEVT_QUERY_END_SESSION
 */

class WXDLLIMPEXP_CORE wxCloseEvent : public wxEvent
{
public:
    wxCloseEvent(wxEventType type = wxEVT_NULL, int winid = 0)
        : wxEvent(winid, type),
          m_loggingOff(true),
          m_veto(false),      // should be false by default
          m_canVeto(true) {}

    wxCloseEvent(const wxCloseEvent & event)
        : wxEvent(event),
        m_loggingOff(event.m_loggingOff),
        m_veto(event.m_veto),
        m_canVeto(event.m_canVeto) {}

    void SetLoggingOff(bool logOff) { m_loggingOff = logOff; }
    bool GetLoggingOff() const
    {
        // m_loggingOff flag is only used by wxEVT_[QUERY_]END_SESSION, it
        // doesn't make sense for wxEVT_CLOSE_WINDOW
        wxASSERT_MSG( m_eventType != wxEVT_CLOSE_WINDOW,
                      _T("this flag is for end session events only") );

        return m_loggingOff;
    }

    void Veto(bool veto = true)
    {
        // GetVeto() will return false anyhow...
        wxCHECK_RET( m_canVeto,
                     wxT("call to Veto() ignored (can't veto this event)") );

        m_veto = veto;
    }
    void SetCanVeto(bool canVeto) { m_canVeto = canVeto; }
    bool CanVeto() const { return m_canVeto; }
    bool GetVeto() const { return m_canVeto && m_veto; }

    virtual wxEvent *Clone() const { return new wxCloseEvent(*this); }

protected:
    bool m_loggingOff,
         m_veto,
         m_canVeto;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxCloseEvent)
};

/*
 wxEVT_SHOW
 */

class WXDLLIMPEXP_CORE wxShowEvent : public wxEvent
{
public:
    wxShowEvent(int winid = 0, bool show = false)
        : wxEvent(winid, wxEVT_SHOW)
        { m_show = show; }
    wxShowEvent(const wxShowEvent & event)
        : wxEvent(event)
    { m_show = event.m_show; }

    void SetShow(bool show) { m_show = show; }

    // return true if the window was shown, false if hidden
    bool IsShown() const { return m_show; }

#if WXWIN_COMPATIBILITY_2_8
    wxDEPRECATED( bool GetShow() const { return IsShown(); } )
#endif

    virtual wxEvent *Clone() const { return new wxShowEvent(*this); }

protected:
    bool m_show;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxShowEvent)
};

/*
 wxEVT_ICONIZE
 */

class WXDLLIMPEXP_CORE wxIconizeEvent : public wxEvent
{
public:
    wxIconizeEvent(int winid = 0, bool iconized = true)
        : wxEvent(winid, wxEVT_ICONIZE)
        { m_iconized = iconized; }
    wxIconizeEvent(const wxIconizeEvent & event)
        : wxEvent(event)
    { m_iconized = event.m_iconized; }

#if WXWIN_COMPATIBILITY_2_8
    wxDEPRECATED( bool Iconized() const { return IsIconized(); } )
#endif
    // return true if the frame was iconized, false if restored
    bool IsIconized() const { return m_iconized; }

    virtual wxEvent *Clone() const { return new wxIconizeEvent(*this); }

protected:
    bool m_iconized;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxIconizeEvent)
};
/*
 wxEVT_MAXIMIZE
 */

class WXDLLIMPEXP_CORE wxMaximizeEvent : public wxEvent
{
public:
    wxMaximizeEvent(int winid = 0)
        : wxEvent(winid, wxEVT_MAXIMIZE)
        { }

    virtual wxEvent *Clone() const { return new wxMaximizeEvent(*this); }

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxMaximizeEvent)
};

// Joystick event class
/*
 wxEVT_JOY_BUTTON_DOWN,
 wxEVT_JOY_BUTTON_UP,
 wxEVT_JOY_MOVE,
 wxEVT_JOY_ZMOVE
*/

// Which joystick? Same as Windows ids so no conversion necessary.
enum
{
    wxJOYSTICK1,
    wxJOYSTICK2
};

// Which button is down?
enum
{
    wxJOY_BUTTON_ANY = -1,
    wxJOY_BUTTON1    = 1,
    wxJOY_BUTTON2    = 2,
    wxJOY_BUTTON3    = 4,
    wxJOY_BUTTON4    = 8
};

class WXDLLIMPEXP_CORE wxJoystickEvent : public wxEvent
{
protected:
    wxPoint   m_pos;
    int       m_zPosition;
    int       m_buttonChange;   // Which button changed?
    int       m_buttonState;    // Which buttons are down?
    int       m_joyStick;       // Which joystick?

public:
    wxJoystickEvent(wxEventType type = wxEVT_NULL,
                    int state = 0,
                    int joystick = wxJOYSTICK1,
                    int change = 0)
        : wxEvent(0, type),
          m_pos(),
          m_zPosition(0),
          m_buttonChange(change),
          m_buttonState(state),
          m_joyStick(joystick)
    {
    }
    wxJoystickEvent(const wxJoystickEvent & event)
        : wxEvent(event),
          m_pos(event.m_pos),
          m_zPosition(event.m_zPosition),
          m_buttonChange(event.m_buttonChange),
          m_buttonState(event.m_buttonState),
          m_joyStick(event.m_joyStick)
    { }

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
    bool IsMove() const { return (GetEventType() == wxEVT_JOY_MOVE); }

    // Was it a zmove event?
    bool IsZMove() const { return (GetEventType() == wxEVT_JOY_ZMOVE); }

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

    virtual wxEvent *Clone() const { return new wxJoystickEvent(*this); }

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxJoystickEvent)
};

// Drop files event class
/*
 wxEVT_DROP_FILES
 */

class WXDLLIMPEXP_CORE wxDropFilesEvent : public wxEvent
{
public:
    int       m_noFiles;
    wxPoint   m_pos;
    wxString* m_files;

    wxDropFilesEvent(wxEventType type = wxEVT_NULL,
                     int noFiles = 0,
                     wxString *files = NULL)
        : wxEvent(0, type),
          m_noFiles(noFiles),
          m_pos(),
          m_files(files)
        { }

    // we need a copy ctor to avoid deleting m_files pointer twice
    wxDropFilesEvent(const wxDropFilesEvent& other)
        : wxEvent(other),
          m_noFiles(other.m_noFiles),
          m_pos(other.m_pos),
          m_files(NULL)
    {
        m_files = new wxString[m_noFiles];
        for ( int n = 0; n < m_noFiles; n++ )
        {
            m_files[n] = other.m_files[n];
        }
    }

    virtual ~wxDropFilesEvent()
    {
        delete [] m_files;
    }

    wxPoint GetPosition() const { return m_pos; }
    int GetNumberOfFiles() const { return m_noFiles; }
    wxString *GetFiles() const { return m_files; }

    virtual wxEvent *Clone() const { return new wxDropFilesEvent(*this); }

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxDropFilesEvent)
};

// Update UI event
/*
 wxEVT_UPDATE_UI
 */

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

class WXDLLIMPEXP_CORE wxUpdateUIEvent : public wxCommandEvent
{
public:
    wxUpdateUIEvent(wxWindowID commandId = 0)
        : wxCommandEvent(wxEVT_UPDATE_UI, commandId)
    {
        m_checked =
        m_enabled =
        m_shown =
        m_setEnabled =
        m_setShown =
        m_setText =
        m_setChecked = false;
    }
    wxUpdateUIEvent(const wxUpdateUIEvent & event)
        : wxCommandEvent(event),
          m_checked(event.m_checked),
          m_enabled(event.m_enabled),
          m_shown(event.m_shown),
          m_setEnabled(event.m_setEnabled),
          m_setShown(event.m_setShown),
          m_setText(event.m_setText),
          m_setChecked(event.m_setChecked),
          m_text(event.m_text)
    { }

    bool GetChecked() const { return m_checked; }
    bool GetEnabled() const { return m_enabled; }
    bool GetShown() const { return m_shown; }
    wxString GetText() const { return m_text; }
    bool GetSetText() const { return m_setText; }
    bool GetSetChecked() const { return m_setChecked; }
    bool GetSetEnabled() const { return m_setEnabled; }
    bool GetSetShown() const { return m_setShown; }

    void Check(bool check) { m_checked = check; m_setChecked = true; }
    void Enable(bool enable) { m_enabled = enable; m_setEnabled = true; }
    void Show(bool show) { m_shown = show; m_setShown = true; }
    void SetText(const wxString& text) { m_text = text; m_setText = true; }

    // Sets the interval between updates in milliseconds.
    // Set to -1 to disable updates, or to 0 to update as frequently as possible.
    static void SetUpdateInterval(long updateInterval) { sm_updateInterval = updateInterval; }

    // Returns the current interval between updates in milliseconds
    static long GetUpdateInterval() { return sm_updateInterval; }

    // Can we update this window?
    static bool CanUpdate(wxWindowBase *win);

    // Reset the update time to provide a delay until the next
    // time we should update
    static void ResetUpdateTime();

    // Specify how wxWidgets will send update events: to
    // all windows, or only to those which specify that they
    // will process the events.
    static void SetMode(wxUpdateUIMode mode) { sm_updateMode = mode; }

    // Returns the UI update mode
    static wxUpdateUIMode GetMode() { return sm_updateMode; }

    virtual wxEvent *Clone() const { return new wxUpdateUIEvent(*this); }

protected:
    bool          m_checked;
    bool          m_enabled;
    bool          m_shown;
    bool          m_setEnabled;
    bool          m_setShown;
    bool          m_setText;
    bool          m_setChecked;
    wxString      m_text;
#if wxUSE_LONGLONG
    static wxLongLong       sm_lastUpdate;
#endif
    static long             sm_updateInterval;
    static wxUpdateUIMode   sm_updateMode;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxUpdateUIEvent)
};

/*
 wxEVT_SYS_COLOUR_CHANGED
 */

// TODO: shouldn't all events record the window ID?
class WXDLLIMPEXP_CORE wxSysColourChangedEvent : public wxEvent
{
public:
    wxSysColourChangedEvent()
        : wxEvent(0, wxEVT_SYS_COLOUR_CHANGED)
        { }

    virtual wxEvent *Clone() const { return new wxSysColourChangedEvent(*this); }

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxSysColourChangedEvent)
};

/*
 wxEVT_MOUSE_CAPTURE_CHANGED
 The window losing the capture receives this message
 (even if it released the capture itself).
 */

class WXDLLIMPEXP_CORE wxMouseCaptureChangedEvent : public wxEvent
{
public:
    wxMouseCaptureChangedEvent(wxWindowID winid = 0, wxWindow* gainedCapture = NULL)
        : wxEvent(winid, wxEVT_MOUSE_CAPTURE_CHANGED),
          m_gainedCapture(gainedCapture)
        { }

    wxMouseCaptureChangedEvent(const wxMouseCaptureChangedEvent& event)
        : wxEvent(event),
          m_gainedCapture(event.m_gainedCapture)
        { }

    virtual wxEvent *Clone() const { return new wxMouseCaptureChangedEvent(*this); }

    wxWindow* GetCapturedWindow() const { return m_gainedCapture; }

private:
    wxWindow* m_gainedCapture;

    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxMouseCaptureChangedEvent)
};

/*
 wxEVT_MOUSE_CAPTURE_LOST
 The window losing the capture receives this message, unless it released it
 it itself or unless wxWindow::CaptureMouse was called on another window
 (and so capture will be restored when the new capturer releases it).
 */

class WXDLLIMPEXP_CORE wxMouseCaptureLostEvent : public wxEvent
{
public:
    wxMouseCaptureLostEvent(wxWindowID winid = 0)
        : wxEvent(winid, wxEVT_MOUSE_CAPTURE_LOST)
    {}

    wxMouseCaptureLostEvent(const wxMouseCaptureLostEvent& event)
        : wxEvent(event)
    {}

    virtual wxEvent *Clone() const { return new wxMouseCaptureLostEvent(*this); }

    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxMouseCaptureLostEvent)
};

/*
 wxEVT_DISPLAY_CHANGED
 */
class WXDLLIMPEXP_CORE wxDisplayChangedEvent : public wxEvent
{
private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxDisplayChangedEvent)

public:
    wxDisplayChangedEvent()
        : wxEvent(0, wxEVT_DISPLAY_CHANGED)
        { }

    virtual wxEvent *Clone() const { return new wxDisplayChangedEvent(*this); }
};

/*
 wxEVT_PALETTE_CHANGED
 */

class WXDLLIMPEXP_CORE wxPaletteChangedEvent : public wxEvent
{
public:
    wxPaletteChangedEvent(wxWindowID winid = 0)
        : wxEvent(winid, wxEVT_PALETTE_CHANGED),
          m_changedWindow(NULL)
        { }

    wxPaletteChangedEvent(const wxPaletteChangedEvent& event)
        : wxEvent(event),
          m_changedWindow(event.m_changedWindow)
        { }

    void SetChangedWindow(wxWindow* win) { m_changedWindow = win; }
    wxWindow* GetChangedWindow() const { return m_changedWindow; }

    virtual wxEvent *Clone() const { return new wxPaletteChangedEvent(*this); }

protected:
    wxWindow*     m_changedWindow;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxPaletteChangedEvent)
};

/*
 wxEVT_QUERY_NEW_PALETTE
 Indicates the window is getting keyboard focus and should re-do its palette.
 */

class WXDLLIMPEXP_CORE wxQueryNewPaletteEvent : public wxEvent
{
public:
    wxQueryNewPaletteEvent(wxWindowID winid = 0)
        : wxEvent(winid, wxEVT_QUERY_NEW_PALETTE),
          m_paletteRealized(false)
        { }
    wxQueryNewPaletteEvent(const wxQueryNewPaletteEvent & event)
        : wxEvent(event),
        m_paletteRealized(event.m_paletteRealized)
    { }

    // App sets this if it changes the palette.
    void SetPaletteRealized(bool realized) { m_paletteRealized = realized; }
    bool GetPaletteRealized() const { return m_paletteRealized; }

    virtual wxEvent *Clone() const { return new wxQueryNewPaletteEvent(*this); }

protected:
    bool m_paletteRealized;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxQueryNewPaletteEvent)
};

/*
 Event generated by dialog navigation keys
 wxEVT_NAVIGATION_KEY
 */
// NB: don't derive from command event to avoid being propagated to the parent
class WXDLLIMPEXP_CORE wxNavigationKeyEvent : public wxEvent
{
public:
    wxNavigationKeyEvent()
        : wxEvent(0, wxEVT_NAVIGATION_KEY),
          m_flags(IsForward | FromTab),    // defaults are for TAB
          m_focus(NULL)
        {
            m_propagationLevel = wxEVENT_PROPAGATE_NONE;
        }

    wxNavigationKeyEvent(const wxNavigationKeyEvent& event)
        : wxEvent(event),
          m_flags(event.m_flags),
          m_focus(event.m_focus)
        { }

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

    // Set to true under MSW if the event was generated using the tab key.
    // This is required for proper navogation over radio buttons
    bool IsFromTab() const
        { return (m_flags & FromTab) != 0; }
    void SetFromTab(bool bIs)
        { if ( bIs ) m_flags |= FromTab; else m_flags &= ~FromTab; }

    // the child which has the focus currently (may be NULL - use
    // wxWindow::FindFocus then)
    wxWindow* GetCurrentFocus() const { return m_focus; }
    void SetCurrentFocus(wxWindow *win) { m_focus = win; }

    // Set flags
    void SetFlags(long flags) { m_flags = flags; }

    virtual wxEvent *Clone() const { return new wxNavigationKeyEvent(*this); }

    enum
    {
        IsBackward = 0x0000,
        IsForward = 0x0001,
        WinChange = 0x0002,
        FromTab = 0x0004
    };

    long m_flags;
    wxWindow *m_focus;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxNavigationKeyEvent)
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

class WXDLLIMPEXP_CORE wxWindowCreateEvent : public wxCommandEvent
{
public:
    wxWindowCreateEvent(wxWindow *win = NULL);

    wxWindow *GetWindow() const { return (wxWindow *)GetEventObject(); }

    virtual wxEvent *Clone() const { return new wxWindowCreateEvent(*this); }

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxWindowCreateEvent)
};

class WXDLLIMPEXP_CORE wxWindowDestroyEvent : public wxCommandEvent
{
public:
    wxWindowDestroyEvent(wxWindow *win = NULL);

    wxWindow *GetWindow() const { return (wxWindow *)GetEventObject(); }

    virtual wxEvent *Clone() const { return new wxWindowDestroyEvent(*this); }

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxWindowDestroyEvent)
};

// A help event is sent when the user clicks on a window in context-help mode.
/*
 wxEVT_HELP
 wxEVT_DETAILED_HELP
*/

class WXDLLIMPEXP_CORE wxHelpEvent : public wxCommandEvent
{
public:
    // how was this help event generated?
    enum Origin
    {
        Origin_Unknown,    // unrecognized event source
        Origin_Keyboard,   // event generated from F1 key press
        Origin_HelpButton  // event from [?] button on the title bar (Windows)
    };

    wxHelpEvent(wxEventType type = wxEVT_NULL,
                wxWindowID winid = 0,
                const wxPoint& pt = wxDefaultPosition,
                Origin origin = Origin_Unknown)
        : wxCommandEvent(type, winid),
          m_pos(pt),
          m_origin(GuessOrigin(origin))
    { }
    wxHelpEvent(const wxHelpEvent & event)
        : wxCommandEvent(event),
          m_pos(event.m_pos),
          m_target(event.m_target),
          m_link(event.m_link),
          m_origin(event.m_origin)
    { }

    // Position of event (in screen coordinates)
    const wxPoint& GetPosition() const { return m_pos; }
    void SetPosition(const wxPoint& pos) { m_pos = pos; }

    // Optional link to further help
    const wxString& GetLink() const { return m_link; }
    void SetLink(const wxString& link) { m_link = link; }

    // Optional target to display help in. E.g. a window specification
    const wxString& GetTarget() const { return m_target; }
    void SetTarget(const wxString& target) { m_target = target; }

    virtual wxEvent *Clone() const { return new wxHelpEvent(*this); }

    // optional indication of the event source
    Origin GetOrigin() const { return m_origin; }
    void SetOrigin(Origin origin) { m_origin = origin; }

protected:
    wxPoint   m_pos;
    wxString  m_target;
    wxString  m_link;
    Origin    m_origin;

    // we can try to guess the event origin ourselves, even if none is
    // specified in the ctor
    static Origin GuessOrigin(Origin origin);

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxHelpEvent)
};

// A Clipboard Text event is sent when a window intercepts text copy/cut/paste
// message, i.e. the user has cut/copied/pasted data from/into a text control
// via ctrl-C/X/V, ctrl/shift-del/insert, a popup menu command, etc.
// NOTE : under windows these events are *NOT* generated automatically
// for a Rich Edit text control.
/*
wxEVT_COMMAND_TEXT_COPY
wxEVT_COMMAND_TEXT_CUT
wxEVT_COMMAND_TEXT_PASTE
*/

class WXDLLIMPEXP_CORE wxClipboardTextEvent : public wxCommandEvent
{
public:
    wxClipboardTextEvent(wxEventType type = wxEVT_NULL,
                     wxWindowID winid = 0)
        : wxCommandEvent(type, winid)
    { }
    wxClipboardTextEvent(const wxClipboardTextEvent & event)
        : wxCommandEvent(event)
    { }

    virtual wxEvent *Clone() const { return new wxClipboardTextEvent(*this); }

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxClipboardTextEvent)
};

// A Context event is sent when the user right clicks on a window or
// presses Shift-F10
// NOTE : Under windows this is a repackaged WM_CONTETXMENU message
//        Under other systems it may have to be generated from a right click event
/*
 wxEVT_CONTEXT_MENU
*/

class WXDLLIMPEXP_CORE wxContextMenuEvent : public wxCommandEvent
{
public:
    wxContextMenuEvent(wxEventType type = wxEVT_NULL,
                       wxWindowID winid = 0,
                       const wxPoint& pt = wxDefaultPosition)
        : wxCommandEvent(type, winid),
          m_pos(pt)
    { }
    wxContextMenuEvent(const wxContextMenuEvent & event)
        : wxCommandEvent(event),
        m_pos(event.m_pos)
    { }

    // Position of event (in screen coordinates)
    const wxPoint& GetPosition() const { return m_pos; }
    void SetPosition(const wxPoint& pos) { m_pos = pos; }

    virtual wxEvent *Clone() const { return new wxContextMenuEvent(*this); }

protected:
    wxPoint   m_pos;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxContextMenuEvent)
};

#endif // wxUSE_GUI

// Idle event
/*
 wxEVT_IDLE
 */

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

class WXDLLIMPEXP_BASE wxIdleEvent : public wxEvent
{
public:
    wxIdleEvent()
        : wxEvent(0, wxEVT_IDLE),
          m_requestMore(false)
        { }
    wxIdleEvent(const wxIdleEvent & event)
        : wxEvent(event),
          m_requestMore(event.m_requestMore)
    { }

    void RequestMore(bool needMore = true) { m_requestMore = needMore; }
    bool MoreRequested() const { return m_requestMore; }

    virtual wxEvent *Clone() const { return new wxIdleEvent(*this); }

    // Specify how wxWidgets will send idle events: to
    // all windows, or only to those which specify that they
    // will process the events.
    static void SetMode(wxIdleMode mode) { sm_idleMode = mode; }

    // Returns the idle event mode
    static wxIdleMode GetMode() { return sm_idleMode; }

protected:
    bool m_requestMore;
    static wxIdleMode sm_idleMode;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxIdleEvent)
};

/* TODO
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


// struct containing the members common to static and dynamic event tables
// entries
struct WXDLLIMPEXP_BASE wxEventTableEntryBase
{
    wxEventTableEntryBase(int winid, int idLast,
                          wxEventFunctor* fn, wxObject *data)
        : m_id(winid),
          m_lastId(idLast),
          m_fn(fn),
          m_callbackUserData(data)
    { }

    wxEventTableEntryBase( const wxEventTableEntryBase &entry )
        : m_id( entry.m_id ),
          m_lastId( entry.m_lastId ),
          m_fn( entry.m_fn ),
          m_callbackUserData( entry.m_callbackUserData )
    {
        // This is a 'hack' to ensure that only one instance tries to delete
        // the functor pointer. It is safe as long as the only place where the
        // copy constructor is being called is when the static event tables are
        // being initialized (a temporary instance is created and then this
        // constructor is called).

        const_cast< wxEventTableEntryBase & >( entry ).m_fn = NULL;
    }

    ~wxEventTableEntryBase()
    {
        delete m_fn;
    }

    // the range of ids for this entry: if m_lastId == wxID_ANY, the range
    // consists only of m_id, otherwise it is m_id..m_lastId inclusive
    int m_id,
        m_lastId;

    // function/method/functor to call
    wxEventFunctor* m_fn;

    // arbitrary user data associated with the callback
    wxObject* m_callbackUserData;

private:
    wxEventTableEntryBase &operator = ( const wxEventTableEntryBase & );
};

// an entry from a static event table
struct WXDLLIMPEXP_BASE wxEventTableEntry : public wxEventTableEntryBase
{
    wxEventTableEntry(const int& evType, int winid, int idLast,
                      wxEventFunctor* fn, wxObject *data)
        : wxEventTableEntryBase(winid, idLast, fn, data),
        m_eventType(evType)
    { }

    // the reference to event type: this allows us to not care about the
    // (undefined) order in which the event table entries and the event types
    // are initialized: initially the value of this reference might be
    // invalid, but by the time it is used for the first time, all global
    // objects will have been initialized (including the event type constants)
    // and so it will have the correct value when it is needed
    const int& m_eventType;

private:
    wxEventTableEntry &operator = ( const wxEventTableEntry & );
};

// an entry used in dynamic event table managed by wxEvtHandler::Connect()
struct WXDLLIMPEXP_BASE wxDynamicEventTableEntry : public wxEventTableEntryBase
{
    wxDynamicEventTableEntry(int evType, int winid, int idLast,
                             wxEventFunctor* fn, wxObject *data)
        : wxEventTableEntryBase(winid, idLast, fn, data),
          m_eventType(evType)
    { }

    // not a reference here as we can't keep a reference to a temporary int
    // created to wrap the constant value typically passed to Connect() - nor
    // do we need it
    int m_eventType;

private:
    wxDynamicEventTableEntry &operator = ( const wxDynamicEventTableEntry & );
};

// ----------------------------------------------------------------------------
// wxEventTable: an array of event entries terminated with {0, 0, 0, 0, 0}
// ----------------------------------------------------------------------------

struct WXDLLIMPEXP_BASE wxEventTable
{
    const wxEventTable *baseTable;    // base event table (next in chain)
    const wxEventTableEntry *entries; // bottom of entry array
};

// ----------------------------------------------------------------------------
// wxEventHashTable: a helper of wxEvtHandler to speed up wxEventTable lookups.
// ----------------------------------------------------------------------------

WX_DEFINE_ARRAY_PTR(const wxEventTableEntry*, wxEventTableEntryPointerArray);

class WXDLLIMPEXP_BASE wxEventHashTable
{
private:
    // Internal data structs
    struct EventTypeTable
    {
        wxEventType                   eventType;
        wxEventTableEntryPointerArray eventEntryTable;
    };
    typedef EventTypeTable* EventTypeTablePointer;

public:
    // Constructor, needs the event table it needs to hash later on.
    // Note: hashing of the event table is not done in the constructor as it
    //       can be that the event table is not yet full initialize, the hash
    //       will gets initialized when handling the first event look-up request.
    wxEventHashTable(const wxEventTable &table);
    // Destructor.
    ~wxEventHashTable();

    // Handle the given event, in other words search the event table hash
    // and call self->ProcessEvent() if a match was found.
    bool HandleEvent(wxEvent &event, wxEvtHandler *self);

    // Clear table
    void Clear();

#if defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING
    // Clear all tables
    static void ClearAll();
#endif // __WXDEBUG__ && wxUSE_MEMORY_TRACING

protected:
    // Init the hash table with the entries of the static event table.
    void InitHashTable();
    // Helper funtion of InitHashTable() to insert 1 entry into the hash table.
    void AddEntry(const wxEventTableEntry &entry);
    // Allocate and init with null pointers the base hash table.
    void AllocEventTypeTable(size_t size);
    // Grow the hash table in size and transfer all items currently
    // in the table to the correct location in the new table.
    void GrowEventTypeTable();

protected:
    const wxEventTable    &m_table;
    bool                   m_rebuildHash;

    size_t                 m_size;
    EventTypeTablePointer *m_eventTypeTable;

    static wxEventHashTable* sm_first;
    wxEventHashTable* m_previous;
    wxEventHashTable* m_next;

    DECLARE_NO_COPY_CLASS(wxEventHashTable)
};

// ----------------------------------------------------------------------------
// wxEvtHandler: the base class for all objects handling wxWidgets events
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxEvtHandler : public wxObject
                                    , public wxTrackable
{
public:
    wxEvtHandler();
    virtual ~wxEvtHandler();


    // Event handler chain
    // -------------------

    wxEvtHandler *GetNextHandler() const { return m_nextHandler; }
    wxEvtHandler *GetPreviousHandler() const { return m_previousHandler; }
    virtual void SetNextHandler(wxEvtHandler *handler) { m_nextHandler = handler; }
    virtual void SetPreviousHandler(wxEvtHandler *handler) { m_previousHandler = handler; }

    void SetEvtHandlerEnabled(bool enabled) { m_enabled = enabled; }
    bool GetEvtHandlerEnabled() const { return m_enabled; }

    void Unlink();
    bool IsUnlinked() const;



    // Event queuing and processing
    // ----------------------------


    // Process an event right now: this can only be called from the main
    // thread, use QueueEvent() for scheduling the events for
    // processing from other threads.
    virtual bool ProcessEvent(wxEvent& event);

    // Process an event by calling ProcessEvent and handling any exceptions
    // thrown by event handlers. It's mostly useful when processing wx events
    // when called from C code (e.g. in GTK+ callback) when the exception
    // wouldn't correctly propagate to wxEventLoop.
    bool SafelyProcessEvent(wxEvent& event);
        // NOTE: uses ProcessEvent()

    // Schedule the given event to be processed later. It takes ownership of
    // the event pointer, i.e. it will be deleted later. This is safe to call
    // from multiple threads although you still need to ensure that wxString
    // fields of the event object are deep copies and not use the same string
    // buffer as other wxString objects in this thread.
    virtual void QueueEvent(wxEvent *event);

    // Add an event to be processed later: notice that this function is not
    // safe to call from threads other than main, use QueueEvent()
    virtual void AddPendingEvent(const wxEvent& event)
    {
        // notice that the thread-safety problem comes from the fact that
        // Clone() doesn't make deep copies of wxString fields of wxEvent
        // object and so the same wxString could be used from both threads when
        // the event object is destroyed in this one -- QueueEvent() avoids
        // this problem as the event pointer is not used any more in this
        // thread at all after it is called.
        QueueEvent(event.Clone());
    }

    void ProcessPendingEvents();
        // NOTE: uses ProcessEvent()

#if wxUSE_THREADS
    bool ProcessThreadEvent(const wxEvent& event);
        // NOTE: uses AddPendingEvent()
#endif


    // Connecting and disconnecting
    // ----------------------------

    // Dynamic association of a member function handler with the event handler,
    // winid and event type
    void Connect(int winid,
                 int lastId,
                 wxEventType eventType,
                 wxObjectEventFunction func,
                 wxObject *userData = NULL,
                 wxEvtHandler *eventSink = NULL)
    {
        wxObjectEventFunctor *functor = wxNewEventFunctor( eventType, func, eventSink );

        Subscribe( winid, lastId, eventType, functor, userData );
    }

    // Convenience function: take just one id
    void Connect(int winid,
                 wxEventType eventType,
                 wxObjectEventFunction func,
                 wxObject *userData = NULL,
                 wxEvtHandler *eventSink = NULL)
        { Connect(winid, wxID_ANY, eventType, func, userData, eventSink); }

    // Even more convenient: without id (same as using id of wxID_ANY)
    void Connect(wxEventType eventType,
                 wxObjectEventFunction func,
                 wxObject *userData = NULL,
                 wxEvtHandler *eventSink = NULL)
        { Connect(wxID_ANY, wxID_ANY, eventType, func, userData, eventSink); }

    bool Disconnect(int winid,
                    int lastId,
                    wxEventType eventType,
                    wxObjectEventFunction func = NULL,
                    wxObject *userData = NULL,
                    wxEvtHandler *eventSink = NULL)
    {
        wxObjectEventFunctor functor = wxConstructEventFunctor( eventType, func, eventSink );

        return Unsubscribe( winid, lastId, eventType, functor, userData );
    }

    bool Disconnect(int winid = wxID_ANY,
                    wxEventType eventType = wxEVT_NULL,
                    wxObjectEventFunction func = NULL,
                    wxObject *userData = NULL,
                    wxEvtHandler *eventSink = NULL)
        { return Disconnect(winid, wxID_ANY, eventType, func, userData, eventSink); }

    bool Disconnect(wxEventType eventType,
                    wxObjectEventFunction func,
                    wxObject *userData = NULL,
                    wxEvtHandler *eventSink = NULL)
        { return Disconnect(wxID_ANY, eventType, func, userData, eventSink); }


#if !wxEVENTS_COMPATIBILITY_2_8
    //
    // Connect a function to an event:
    //
    template <typename EventType>
    void Connect(int winid,
                 int lastId,
                 const EventType &eventType,
                 void (*func)(typename EventType::CorrespondingEvent&),
                 wxObject* userData = NULL)
    {
        wxEventFunctorFunction< EventType > *functor = wxNewEventFunctor( eventType, func );

        Subscribe( winid, lastId, eventType, functor, userData );
    }

    template <typename EventType>
    void Connect( int winid,
            const EventType &eventType,
            void ( *func )( typename EventType::CorrespondingEvent & ),
            wxObject* userData = NULL )
        { Connect( winid, wxID_ANY, eventType, func, userData ); }

    template <typename EventType>
    void Connect( const EventType &eventType,
            void ( *func )( typename EventType::CorrespondingEvent & ),
            wxObject* userData = NULL )
        { Connect( wxID_ANY, wxID_ANY, eventType, func, userData ); }

    //
    // Connect a method to an event:
    //

    template <typename EventType, typename Class>
    void Connect( int winid,
            int lastId,
            const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL )
        {
            wxEventFunctorMethod< EventType, Class, Class > *functor =
                wxNewEventFunctor( eventType, func, static_cast< Class * const >( this ));

            Subscribe( winid, lastId, eventType, functor, userData );
        }

    template <typename EventType, typename Class>
    void Connect( int winid,
            const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL )
        { Connect( winid, wxID_ANY, eventType, func, userData ); }

    template <typename EventType, typename Class>
    void Connect( const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL )
        { Connect( wxID_ANY, wxID_ANY, eventType, func, userData ); }

    template <typename EventType, typename Class, typename Derived>
    void Connect( int winid,
            int lastId,
            const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL,
            Derived *eventSink = NULL )
        {
            wxEventFunctorMethod< EventType, Class, Derived > *functor =
                wxNewEventFunctor( eventType, func, eventSink );

            Subscribe( winid, lastId, eventType, functor, userData );
        }

    template <typename EventType, typename Class, typename Derived>
    void Connect( int winid,
            const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL,
            Derived *eventSink = NULL )
        { Connect( winid, wxID_ANY, eventType, func, userData, eventSink ); }

    template <typename EventType, typename Class, typename Derived>
    void Connect( const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL,
            Derived *eventSink = NULL )
        { Connect( wxID_ANY, wxID_ANY, eventType, func, userData, eventSink ); }

    template <typename Sender, typename EventType, typename Class, typename Derived>
    static void Connect( Sender *sender,
            int winid,
            int lastId,
            const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL,
            Derived *eventSink = NULL )
    {
        wxEventFunctorMethod< EventType, Class, Derived > *functor =
            wxNewEventFunctor( eventType, func, eventSink );

        sender->Subscribe( winid, lastId, eventType, functor, userData );
    }

    template <typename Sender, typename EventType, typename Class, typename Derived>
    static void Connect( Sender *sender,
            int winid,
            const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL,
            Derived *eventSink = NULL )
        { Connect( sender, winid, wxID_ANY, eventType, func, userData, eventSink ); }

    template <typename Sender, typename EventType, typename Class, typename Derived>
    static void Connect( Sender *sender,
            const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL,
            Derived *eventSink = NULL )
        { Connect( sender, wxID_ANY, wxID_ANY, eventType, func, userData, eventSink ); }

    //
    // Connect an arbitrary functor to an event:
    //

    template <typename EventType, typename Functor>
    void Connect( int winid,
            int lastId,
            const EventType &eventType,
            Functor &functor,
            wxObject* userData = NULL)
    {
        wxEventFunctorAdapter< EventType, Functor > *adapter =
            wxNewEventFunctor( eventType, functor );

        Subscribe( winid, lastId, eventType, adapter, userData );
    }
    template <typename EventType, typename Functor>
    void Connect( int winid,
            const EventType &eventType,
            Functor &functor,
            wxObject* userData = NULL)
        { Connect( winid, wxID_ANY, eventType, functor, userData ); }

    template <typename EventType, typename Functor>
    void Connect( const EventType &eventType,
            Functor &functor,
            wxObject* userData = NULL)
        { Connect( wxID_ANY, wxID_ANY, eventType, functor, userData ); }

    //
    // Disconnect a function from an event:
    //

    template <typename EventType>
    bool Disconnect( int winid,
        int lastId,
        const EventType &eventType,
        void ( *func )( typename EventType::CorrespondingEvent & ),
        wxObject* userData = NULL )
    {
        wxEventFunctorFunction< EventType > functor = wxConstructEventFunctor( eventType, func );

        return Unsubscribe( winid, lastId, eventType, functor, userData );
    }

    template <typename EventType>
    bool Disconnect( int winid,
            const EventType &eventType,
            void ( *func )( typename EventType::CorrespondingEvent & ),
            wxObject* userData = NULL )
        { return Disconnect( winid, wxID_ANY, eventType, func, userData ); }

    template <typename EventType>
    bool Disconnect( const EventType &eventType,
            void ( *func )( typename EventType::CorrespondingEvent & ),
            wxObject* userData = NULL )
        { return Disconnect( wxID_ANY, wxID_ANY, eventType, func, userData ); }

    //
    // Disconnect a method from an event:
    //

    template <typename EventType, typename Class>
    bool Disconnect( int winid,
            int lastId,
            const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL )
        {
            wxEventFunctorMethod< EventType, Class, Class > functor =
                wxConstructEventFunctor( eventType, func, static_cast< Class * const >( this ));

            return Unsubscribe( winid, lastId, eventType, functor, userData );
        }

    template <typename EventType, typename Class>
    bool Disconnect( int winid,
            const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL )
        { return Disconnect( winid, wxID_ANY, eventType, func, userData ); }

    template <typename EventType, typename Class>
    bool Disconnect( const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL )
        { return Disconnect( wxID_ANY, wxID_ANY, eventType, func, userData ); }

    template <typename EventType, typename Class, typename Derived>
    bool Disconnect( int winid,
            int lastId,
            const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL,
            Derived *eventSink = NULL )
        {
            wxEventFunctorMethod< EventType, Class, Derived > functor =
                wxConstructEventFunctor( eventType, func, eventSink );

            return Unsubscribe( winid, lastId, eventType, functor, userData );
        }

    template <typename EventType, typename Class, typename Derived>
    bool Disconnect( int winid,
            const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL,
            Derived *eventSink = NULL )
        { return Disconnect( winid, wxID_ANY, eventType, func, userData, eventSink ); }

    template <typename EventType, typename Class, typename Derived>
    bool Disconnect( const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL,
            Derived *eventSink = NULL )
        { return Disconnect( wxID_ANY, wxID_ANY, eventType, func, userData, eventSink ); }

    template <typename Sender, typename EventType, typename Class, typename Derived>
    static bool Disconnect( Sender *sender,
            int winid,
            int lastId,
            const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL,
            Derived *eventSink = NULL )
    {
        wxEventFunctorMethod< EventType, Class, Derived > functor =
            wxConstructEventFunctor( eventType, func, eventSink );

        return sender->Unsubscribe( winid, lastId, eventType, functor, userData );
    }

    template <typename Sender, typename EventType, typename Class, typename Derived>
    static bool Disconnect( Sender *sender,
            int winid,
            const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL,
            Derived *eventSink = NULL )
        { return Disconnect( sender, winid, wxID_ANY, eventType, func, userData, eventSink ); }

    template <typename Sender, typename EventType, typename Class, typename Derived>
    static bool Disconnect( Sender *sender,
            const EventType &eventType,
            void ( Class::*func )( typename EventType::CorrespondingEvent & ),
            wxObject *userData = NULL,
            Derived *eventSink = NULL )
        { return Disconnect( sender, wxID_ANY, wxID_ANY, eventType, func, userData, eventSink ); }

    //
    // Disconnect an arbitrary functor from an event:
    //

    template <typename EventType, typename Functor>
    bool Disconnect( int winid,
            int lastId,
            const EventType &eventType,
            Functor &functor,
            wxObject* userData = NULL)
    {
        wxEventFunctorAdapter< EventType, Functor > adapter =
            wxConstructEventFunctor( eventType, functor );

        return Unsubscribe( winid, lastId, eventType, adapter, userData );
    }

    template <typename EventType, typename Functor>
    bool Disconnect( int winid,
            const EventType &eventType,
            Functor &functor,
            wxObject* userData = NULL)
        { return Disconnect( winid, wxID_ANY, eventType, functor, userData ); }

    template <typename EventType, typename Functor>
    bool Disconnect( const EventType &eventType,
            Functor &functor,
            wxObject* userData = NULL)
        { return Disconnect( wxID_ANY, wxID_ANY, eventType, functor, userData ); }

#endif // !wxEVENTS_COMPATIBILITY_2_8


    wxList* GetDynamicEventTable() const { return m_dynamicEvents ; }

    // User data can be associated with each wxEvtHandler
    void SetClientObject( wxClientData *data ) { DoSetClientObject(data); }
    wxClientData *GetClientObject() const { return DoGetClientObject(); }

    void SetClientData( void *data ) { DoSetClientData(data); }
    void *GetClientData() const { return DoGetClientData(); }


    // implementation from now on
    // --------------------------

    // check if the given event table entry matches this event by id (the check
    // for the event type should be done by caller) and call the handler if it
    // does
    //
    // return true if the event was processed, false otherwise (no match or the
    // handler decided to skip the event)
    static bool ProcessEventIfMatchesId(const wxEventTableEntryBase& tableEntry,
                                        wxEvtHandler *handler,
                                        wxEvent& event);

    virtual bool SearchEventTable(wxEventTable& table, wxEvent& event);
    bool SearchDynamicEventTable( wxEvent& event );

    // Avoid problems at exit by cleaning up static hash table gracefully
    void ClearEventHashTable() { GetEventHashTable().Clear(); }
    void OnSinkDestroyed( wxEvtHandler *sink );


    // The method tries to process the event in this event handler.
    //
    // It is meant to be called from ProcessEvent() only and is not virtual,
    // additional event handlers can be hooked into the normal event processing
    // logic using TryValidator() hook.
    bool ProcessEventHere(wxEvent& event);


private:
    void Subscribe(int winid,
                   int lastId,
                   wxEventType eventType,
                   wxEventFunctor *func,
                   wxObject* userData);

    bool Unsubscribe(int winid,
                     int lastId,
                     wxEventType eventType,
                     const wxEventFunctor &func,
                     wxObject *userData);

    static const wxEventTableEntry sm_eventTableEntries[];

protected:
    // hooks for wxWindow used by ProcessEvent()
    // -----------------------------------------

    // This one is called before trying our own event table to allow plugging
    // in the validators.
    //
    // NB: This method is intentionally *not* inside wxUSE_VALIDATORS!
    //     It is part of wxBase which doesn't use validators and the code
    //     is compiled out when building wxBase w/o GUI classes, which affects
    //     binary compatibility and wxBase library can't be used by GUI
    //     ports.
    virtual bool TryValidator(wxEvent& WXUNUSED(event)) { return false; }

    // this one is called after failing to find the event handle in our own
    // table to give a chance to the other windows to process it
    //
    // base class implementation passes the event to wxTheApp
    virtual bool TryParent(wxEvent& event);


    static const wxEventTable sm_eventTable;
    virtual const wxEventTable *GetEventTable() const;

    static wxEventHashTable   sm_eventHashTable;
    virtual wxEventHashTable& GetEventHashTable() const;

    wxEvtHandler*       m_nextHandler;
    wxEvtHandler*       m_previousHandler;
    wxList*             m_dynamicEvents;
    wxList*             m_pendingEvents;

#if wxUSE_THREADS
    // critical section protecting m_pendingEvents
    wxCriticalSection m_pendingEventsLock;
#endif // wxUSE_THREADS

    // Is event handler enabled?
    bool                m_enabled;


    // The user data: either an object which will be deleted by the container
    // when it's deleted or some raw pointer which we do nothing with - only
    // one type of data can be used with the given window (i.e. you cannot set
    // the void data and then associate the container with wxClientData or vice
    // versa)
    union
    {
        wxClientData *m_clientObject;
        void         *m_clientData;
    };

    // what kind of data do we have?
    wxClientDataType m_clientDataType;

    // client data accessors
    virtual void DoSetClientObject( wxClientData *data );
    virtual wxClientData *DoGetClientObject() const;

    virtual void DoSetClientData( void *data );
    virtual void *DoGetClientData() const;

    // Search tracker objects for event connection with this sink
    wxEventConnectionRef *FindRefInTrackerList(wxEvtHandler *eventSink);

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxEvtHandler)
};

// ----------------------------------------------------------------------------
// wxEventConnectionRef represents all connections between two event handlers
// and enables automatic disconnect when an event handler sink goes out of
// scope. Each connection/disconnect increases/decreases ref count, and
// when it reaches zero the node goes out of scope.
// ----------------------------------------------------------------------------

class wxEventConnectionRef : public wxTrackerNode
{
public:
    wxEventConnectionRef() : m_src(NULL), m_sink(NULL), m_refCount(0) { }
    wxEventConnectionRef(wxEvtHandler *src, wxEvtHandler *sink)
        : m_src(src), m_sink(sink), m_refCount(1)
    {
        m_sink->AddNode(this);
    }

    // The sink is being destroyed
    virtual void OnObjectDestroy( )
    {
        if ( m_src )
            m_src->OnSinkDestroyed( m_sink );
        delete this;
    }

    virtual wxEventConnectionRef *ToEventConnection() { return this; }

    void IncRef() { m_refCount++; }
    void DecRef()
    {
        if ( !--m_refCount )
        {
            // The sink holds the only external pointer to this object
            if ( m_sink )
                m_sink->RemoveNode(this);
            delete this;
        }
    }

private:
    wxEvtHandler *m_src,
                 *m_sink;
    int m_refCount;

    friend class wxEvtHandler;

    DECLARE_NO_ASSIGN_CLASS(wxEventConnectionRef)
};

// Post a message to the given event handler which will be processed during the
// next event loop iteration.
//
// Notice that this one is not thread-safe, use wxQueueEvent()
inline void wxPostEvent(wxEvtHandler *dest, const wxEvent& event)
{
    wxCHECK_RET( dest, "need an object to post event to" );

    dest->AddPendingEvent(event);
}

// Wrapper around wxEvtHandler::QueueEvent(): adds an event for later
// processing, unlike wxPostEvent it is safe to use from different thread even
// for events with wxString members
inline void wxQueueEvent(wxEvtHandler *dest, wxEvent *event)
{
    wxCHECK_RET( dest, "need an object to queue event for" );

    dest->QueueEvent(event);
}

typedef void (wxEvtHandler::*wxEventFunction)(wxEvent&);
typedef void (wxEvtHandler::*wxIdleEventFunction)(wxIdleEvent&);

#define wxEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxEventFunction, func)
#define wxIdleEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxIdleEventFunction, func)

#if wxUSE_GUI

// ----------------------------------------------------------------------------
// wxEventBlocker: helper class to temporarily disable event handling for a window
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxEventBlocker : public wxEvtHandler
{
public:
    wxEventBlocker(wxWindow *win, wxEventType type = wxEVT_ANY);
    virtual ~wxEventBlocker();

    void Block(wxEventType type)
    {
        m_eventsToBlock.push_back(type);
    }

    virtual bool ProcessEvent(wxEvent& event);

protected:
    wxArrayInt m_eventsToBlock;
    wxWindow *m_window;

    DECLARE_NO_COPY_CLASS(wxEventBlocker)
};

typedef void (wxEvtHandler::*wxCommandEventFunction)(wxCommandEvent&);
typedef void (wxEvtHandler::*wxScrollEventFunction)(wxScrollEvent&);
typedef void (wxEvtHandler::*wxScrollWinEventFunction)(wxScrollWinEvent&);
typedef void (wxEvtHandler::*wxSizeEventFunction)(wxSizeEvent&);
typedef void (wxEvtHandler::*wxMoveEventFunction)(wxMoveEvent&);
typedef void (wxEvtHandler::*wxPaintEventFunction)(wxPaintEvent&);
typedef void (wxEvtHandler::*wxNcPaintEventFunction)(wxNcPaintEvent&);
typedef void (wxEvtHandler::*wxEraseEventFunction)(wxEraseEvent&);
typedef void (wxEvtHandler::*wxMouseEventFunction)(wxMouseEvent&);
typedef void (wxEvtHandler::*wxCharEventFunction)(wxKeyEvent&);
typedef void (wxEvtHandler::*wxFocusEventFunction)(wxFocusEvent&);
typedef void (wxEvtHandler::*wxChildFocusEventFunction)(wxChildFocusEvent&);
typedef void (wxEvtHandler::*wxActivateEventFunction)(wxActivateEvent&);
typedef void (wxEvtHandler::*wxMenuEventFunction)(wxMenuEvent&);
typedef void (wxEvtHandler::*wxJoystickEventFunction)(wxJoystickEvent&);
typedef void (wxEvtHandler::*wxDropFilesEventFunction)(wxDropFilesEvent&);
typedef void (wxEvtHandler::*wxInitDialogEventFunction)(wxInitDialogEvent&);
typedef void (wxEvtHandler::*wxSysColourChangedEventFunction)(wxSysColourChangedEvent&);
typedef void (wxEvtHandler::*wxDisplayChangedEventFunction)(wxDisplayChangedEvent&);
typedef void (wxEvtHandler::*wxUpdateUIEventFunction)(wxUpdateUIEvent&);
typedef void (wxEvtHandler::*wxCloseEventFunction)(wxCloseEvent&);
typedef void (wxEvtHandler::*wxShowEventFunction)(wxShowEvent&);
typedef void (wxEvtHandler::*wxIconizeEventFunction)(wxIconizeEvent&);
typedef void (wxEvtHandler::*wxMaximizeEventFunction)(wxMaximizeEvent&);
typedef void (wxEvtHandler::*wxNavigationKeyEventFunction)(wxNavigationKeyEvent&);
typedef void (wxEvtHandler::*wxPaletteChangedEventFunction)(wxPaletteChangedEvent&);
typedef void (wxEvtHandler::*wxQueryNewPaletteEventFunction)(wxQueryNewPaletteEvent&);
typedef void (wxEvtHandler::*wxWindowCreateEventFunction)(wxWindowCreateEvent&);
typedef void (wxEvtHandler::*wxWindowDestroyEventFunction)(wxWindowDestroyEvent&);
typedef void (wxEvtHandler::*wxSetCursorEventFunction)(wxSetCursorEvent&);
typedef void (wxEvtHandler::*wxNotifyEventFunction)(wxNotifyEvent&);
typedef void (wxEvtHandler::*wxHelpEventFunction)(wxHelpEvent&);
typedef void (wxEvtHandler::*wxContextMenuEventFunction)(wxContextMenuEvent&);
typedef void (wxEvtHandler::*wxMouseCaptureChangedEventFunction)(wxMouseCaptureChangedEvent&);
typedef void (wxEvtHandler::*wxMouseCaptureLostEventFunction)(wxMouseCaptureLostEvent&);
typedef void (wxEvtHandler::*wxClipboardTextEventFunction)(wxClipboardTextEvent&);


#define wxCommandEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxCommandEventFunction, func)
#define wxScrollEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxScrollEventFunction, func)
#define wxScrollWinEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxScrollWinEventFunction, func)
#define wxSizeEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxSizeEventFunction, func)
#define wxMoveEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxMoveEventFunction, func)
#define wxPaintEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxPaintEventFunction, func)
#define wxNcPaintEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxNcPaintEventFunction, func)
#define wxEraseEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxEraseEventFunction, func)
#define wxMouseEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxMouseEventFunction, func)
#define wxCharEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxCharEventFunction, func)
#define wxKeyEventHandler(func) wxCharEventHandler(func)
#define wxFocusEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxFocusEventFunction, func)
#define wxChildFocusEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxChildFocusEventFunction, func)
#define wxActivateEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxActivateEventFunction, func)
#define wxMenuEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxMenuEventFunction, func)
#define wxJoystickEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxJoystickEventFunction, func)
#define wxDropFilesEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxDropFilesEventFunction, func)
#define wxInitDialogEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxInitDialogEventFunction, func)
#define wxSysColourChangedEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxSysColourChangedEventFunction, func)
#define wxDisplayChangedEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxDisplayChangedEventFunction, func)
#define wxUpdateUIEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxUpdateUIEventFunction, func)
#define wxCloseEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxCloseEventFunction, func)
#define wxShowEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxShowEventFunction, func)
#define wxIconizeEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxIconizeEventFunction, func)
#define wxMaximizeEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxMaximizeEventFunction, func)
#define wxNavigationKeyEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxNavigationKeyEventFunction, func)
#define wxPaletteChangedEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxPaletteChangedEventFunction, func)
#define wxQueryNewPaletteEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxQueryNewPaletteEventFunction, func)
#define wxWindowCreateEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxWindowCreateEventFunction, func)
#define wxWindowDestroyEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxWindowDestroyEventFunction, func)
#define wxSetCursorEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxSetCursorEventFunction, func)
#define wxNotifyEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxNotifyEventFunction, func)
#define wxHelpEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxHelpEventFunction, func)
#define wxContextMenuEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxContextMenuEventFunction, func)
#define wxMouseCaptureChangedEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxMouseCaptureChangedEventFunction, func)
#define wxMouseCaptureLostEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxMouseCaptureLostEventFunction, func)
#define wxClipboardTextEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxClipboardTextEventFunction, func)

#endif // wxUSE_GUI

// N.B. In GNU-WIN32, you *have* to take the address of a member function
// (use &) or the compiler crashes...

#define DECLARE_EVENT_TABLE() \
    private: \
        static const wxEventTableEntry sm_eventTableEntries[]; \
    protected: \
        static const wxEventTable        sm_eventTable; \
        virtual const wxEventTable*      GetEventTable() const; \
        static wxEventHashTable          sm_eventHashTable; \
        virtual wxEventHashTable&        GetEventHashTable() const;

// N.B.: when building DLL with Borland C++ 5.5 compiler, you must initialize
//       sm_eventTable before using it in GetEventTable() or the compiler gives
//       E2233 (see http://groups.google.com/groups?selm=397dcc8a%241_2%40dnews)

#define BEGIN_EVENT_TABLE(theClass, baseClass) \
    const wxEventTable theClass::sm_eventTable = \
        { &baseClass::sm_eventTable, &theClass::sm_eventTableEntries[0] }; \
    const wxEventTable *theClass::GetEventTable() const \
        { return &theClass::sm_eventTable; } \
    wxEventHashTable theClass::sm_eventHashTable(theClass::sm_eventTable); \
    wxEventHashTable &theClass::GetEventHashTable() const \
        { return theClass::sm_eventHashTable; } \
    const wxEventTableEntry theClass::sm_eventTableEntries[] = { \

#define BEGIN_EVENT_TABLE_TEMPLATE1(theClass, baseClass, T1) \
    template<typename T1> \
    const wxEventTable theClass<T1>::sm_eventTable = \
        { &baseClass::sm_eventTable, &theClass<T1>::sm_eventTableEntries[0] }; \
    template<typename T1> \
    const wxEventTable *theClass<T1>::GetEventTable() const \
        { return &theClass<T1>::sm_eventTable; } \
    template<typename T1> \
    wxEventHashTable theClass<T1>::sm_eventHashTable(theClass<T1>::sm_eventTable); \
    template<typename T1> \
    wxEventHashTable &theClass<T1>::GetEventHashTable() const \
        { return theClass<T1>::sm_eventHashTable; } \
    template<typename T1> \
    const wxEventTableEntry theClass<T1>::sm_eventTableEntries[] = { \

#define BEGIN_EVENT_TABLE_TEMPLATE2(theClass, baseClass, T1, T2) \
    template<typename T1, typename T2> \
    const wxEventTable theClass<T1, T2>::sm_eventTable = \
        { &baseClass::sm_eventTable, &theClass<T1, T2>::sm_eventTableEntries[0] }; \
    template<typename T1, typename T2> \
    const wxEventTable *theClass<T1, T2>::GetEventTable() const \
        { return &theClass<T1, T2>::sm_eventTable; } \
    template<typename T1, typename T2> \
    wxEventHashTable theClass<T1, T2>::sm_eventHashTable(theClass<T1, T2>::sm_eventTable); \
    template<typename T1, typename T2> \
    wxEventHashTable &theClass<T1, T2>::GetEventHashTable() const \
        { return theClass<T1, T2>::sm_eventHashTable; } \
    template<typename T1, typename T2> \
    const wxEventTableEntry theClass<T1, T2>::sm_eventTableEntries[] = { \

#define BEGIN_EVENT_TABLE_TEMPLATE3(theClass, baseClass, T1, T2, T3) \
    template<typename T1, typename T2, typename T3> \
    const wxEventTable theClass<T1, T2, T3>::sm_eventTable = \
        { &baseClass::sm_eventTable, &theClass<T1, T2, T3>::sm_eventTableEntries[0] }; \
    template<typename T1, typename T2, typename T3> \
    const wxEventTable *theClass<T1, T2, T3>::GetEventTable() const \
        { return &theClass<T1, T2, T3>::sm_eventTable; } \
    template<typename T1, typename T2, typename T3> \
    wxEventHashTable theClass<T1, T2, T3>::sm_eventHashTable(theClass<T1, T2, T3>::sm_eventTable); \
    template<typename T1, typename T2, typename T3> \
    wxEventHashTable &theClass<T1, T2, T3>::GetEventHashTable() const \
        { return theClass<T1, T2, T3>::sm_eventHashTable; } \
    template<typename T1, typename T2, typename T3> \
    const wxEventTableEntry theClass<T1, T2, T3>::sm_eventTableEntries[] = { \

#define BEGIN_EVENT_TABLE_TEMPLATE4(theClass, baseClass, T1, T2, T3, T4) \
    template<typename T1, typename T2, typename T3, typename T4> \
    const wxEventTable theClass<T1, T2, T3, T4>::sm_eventTable = \
        { &baseClass::sm_eventTable, &theClass<T1, T2, T3, T4>::sm_eventTableEntries[0] }; \
    template<typename T1, typename T2, typename T3, typename T4> \
    const wxEventTable *theClass<T1, T2, T3, T4>::GetEventTable() const \
        { return &theClass<T1, T2, T3, T4>::sm_eventTable; } \
    template<typename T1, typename T2, typename T3, typename T4> \
    wxEventHashTable theClass<T1, T2, T3, T4>::sm_eventHashTable(theClass<T1, T2, T3, T4>::sm_eventTable); \
    template<typename T1, typename T2, typename T3, typename T4> \
    wxEventHashTable &theClass<T1, T2, T3, T4>::GetEventHashTable() const \
        { return theClass<T1, T2, T3, T4>::sm_eventHashTable; } \
    template<typename T1, typename T2, typename T3, typename T4> \
    const wxEventTableEntry theClass<T1, T2, T3, T4>::sm_eventTableEntries[] = { \

#define BEGIN_EVENT_TABLE_TEMPLATE5(theClass, baseClass, T1, T2, T3, T4, T5) \
    template<typename T1, typename T2, typename T3, typename T4, typename T5> \
    const wxEventTable theClass<T1, T2, T3, T4, T5>::sm_eventTable = \
        { &baseClass::sm_eventTable, &theClass<T1, T2, T3, T4, T5>::sm_eventTableEntries[0] }; \
    template<typename T1, typename T2, typename T3, typename T4, typename T5> \
    const wxEventTable *theClass<T1, T2, T3, T4, T5>::GetEventTable() const \
        { return &theClass<T1, T2, T3, T4, T5>::sm_eventTable; } \
    template<typename T1, typename T2, typename T3, typename T4, typename T5> \
    wxEventHashTable theClass<T1, T2, T3, T4, T5>::sm_eventHashTable(theClass<T1, T2, T3, T4, T5>::sm_eventTable); \
    template<typename T1, typename T2, typename T3, typename T4, typename T5> \
    wxEventHashTable &theClass<T1, T2, T3, T4, T5>::GetEventHashTable() const \
        { return theClass<T1, T2, T3, T4, T5>::sm_eventHashTable; } \
    template<typename T1, typename T2, typename T3, typename T4, typename T5> \
    const wxEventTableEntry theClass<T1, T2, T3, T4, T5>::sm_eventTableEntries[] = { \

#define BEGIN_EVENT_TABLE_TEMPLATE7(theClass, baseClass, T1, T2, T3, T4, T5, T6, T7) \
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
    const wxEventTable theClass<T1, T2, T3, T4, T5, T6, T7>::sm_eventTable = \
        { &baseClass::sm_eventTable, &theClass<T1, T2, T3, T4, T5, T6, T7>::sm_eventTableEntries[0] }; \
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
    const wxEventTable *theClass<T1, T2, T3, T4, T5, T6, T7>::GetEventTable() const \
        { return &theClass<T1, T2, T3, T4, T5, T6, T7>::sm_eventTable; } \
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
    wxEventHashTable theClass<T1, T2, T3, T4, T5, T6, T7>::sm_eventHashTable(theClass<T1, T2, T3, T4, T5, T6, T7>::sm_eventTable); \
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
    wxEventHashTable &theClass<T1, T2, T3, T4, T5, T6, T7>::GetEventHashTable() const \
        { return theClass<T1, T2, T3, T4, T5, T6, T7>::sm_eventHashTable; } \
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
    const wxEventTableEntry theClass<T1, T2, T3, T4, T5, T6, T7>::sm_eventTableEntries[] = { \

#define BEGIN_EVENT_TABLE_TEMPLATE8(theClass, baseClass, T1, T2, T3, T4, T5, T6, T7, T8) \
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
    const wxEventTable theClass<T1, T2, T3, T4, T5, T6, T7, T8>::sm_eventTable = \
        { &baseClass::sm_eventTable, &theClass<T1, T2, T3, T4, T5, T6, T7, T8>::sm_eventTableEntries[0] }; \
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
    const wxEventTable *theClass<T1, T2, T3, T4, T5, T6, T7, T8>::GetEventTable() const \
        { return &theClass<T1, T2, T3, T4, T5, T6, T7, T8>::sm_eventTable; } \
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
    wxEventHashTable theClass<T1, T2, T3, T4, T5, T6, T7, T8>::sm_eventHashTable(theClass<T1, T2, T3, T4, T5, T6, T7, T8>::sm_eventTable); \
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
    wxEventHashTable &theClass<T1, T2, T3, T4, T5, T6, T7, T8>::GetEventHashTable() const \
        { return theClass<T1, T2, T3, T4, T5, T6, T7, T8>::sm_eventHashTable; } \
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
    const wxEventTableEntry theClass<T1, T2, T3, T4, T5, T6, T7, T8>::sm_eventTableEntries[] = { \

#define END_EVENT_TABLE() DECLARE_EVENT_TABLE_TERMINATOR() };

/*
 * Event table macros
 */

// helpers for writing shorter code below: declare an event macro taking 2, 1
// or none ids (the missing ids default to wxID_ANY)
//
// macro arguments:
//  - evt one of wxEVT_XXX constants
//  - id1, id2 ids of the first/last id
//  - fn the function (should be cast to the right type)
#define wx__DECLARE_EVT2(evt, id1, id2, fn) \
    DECLARE_EVENT_TABLE_ENTRY(evt, id1, id2, fn, NULL),
#define wx__DECLARE_EVT1(evt, id, fn) \
    wx__DECLARE_EVT2(evt, id, wxID_ANY, fn)
#define wx__DECLARE_EVT0(evt, fn) \
    wx__DECLARE_EVT1(evt, wxID_ANY, fn)


// Generic events
#define EVT_CUSTOM(event, winid, func) \
    wx__DECLARE_EVT1(event, winid, wxEventHandler(func))
#define EVT_CUSTOM_RANGE(event, id1, id2, func) \
    wx__DECLARE_EVT2(event, id1, id2, wxEventHandler(func))

// EVT_COMMAND
#define EVT_COMMAND(winid, event, func) \
    wx__DECLARE_EVT1(event, winid, wxCommandEventHandler(func))

#define EVT_COMMAND_RANGE(id1, id2, event, func) \
    wx__DECLARE_EVT2(event, id1, id2, wxCommandEventHandler(func))

#define EVT_NOTIFY(event, winid, func) \
    wx__DECLARE_EVT1(event, winid, wxNotifyEventHandler(func))

#define EVT_NOTIFY_RANGE(event, id1, id2, func) \
    wx__DECLARE_EVT2(event, id1, id2, wxNotifyEventHandler(func))

// Miscellaneous
#define EVT_SIZE(func)  wx__DECLARE_EVT0(wxEVT_SIZE, wxSizeEventHandler(func))
#define EVT_SIZING(func)  wx__DECLARE_EVT0(wxEVT_SIZING, wxSizeEventHandler(func))
#define EVT_MOVE(func)  wx__DECLARE_EVT0(wxEVT_MOVE, wxMoveEventHandler(func))
#define EVT_MOVING(func)  wx__DECLARE_EVT0(wxEVT_MOVING, wxMoveEventHandler(func))
#define EVT_MOVE_START(func)  wx__DECLARE_EVT0(wxEVT_MOVE_START, wxMoveEventHandler(func))
#define EVT_MOVE_END(func)  wx__DECLARE_EVT0(wxEVT_MOVE_END, wxMoveEventHandler(func))
#define EVT_CLOSE(func)  wx__DECLARE_EVT0(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(func))
#define EVT_END_SESSION(func)  wx__DECLARE_EVT0(wxEVT_END_SESSION, wxCloseEventHandler(func))
#define EVT_QUERY_END_SESSION(func)  wx__DECLARE_EVT0(wxEVT_QUERY_END_SESSION, wxCloseEventHandler(func))
#define EVT_PAINT(func)  wx__DECLARE_EVT0(wxEVT_PAINT, wxPaintEventHandler(func))
#define EVT_NC_PAINT(func)  wx__DECLARE_EVT0(wxEVT_NC_PAINT, wxNcPaintEventHandler(func))
#define EVT_ERASE_BACKGROUND(func)  wx__DECLARE_EVT0(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(func))
#define EVT_CHAR(func)  wx__DECLARE_EVT0(wxEVT_CHAR, wxCharEventHandler(func))
#define EVT_KEY_DOWN(func)  wx__DECLARE_EVT0(wxEVT_KEY_DOWN, wxKeyEventHandler(func))
#define EVT_KEY_UP(func)  wx__DECLARE_EVT0(wxEVT_KEY_UP, wxKeyEventHandler(func))
#if wxUSE_HOTKEY
#define EVT_HOTKEY(winid, func)  wx__DECLARE_EVT1(wxEVT_HOTKEY, winid, wxCharEventHandler(func))
#endif
#define EVT_CHAR_HOOK(func)  wx__DECLARE_EVT0(wxEVT_CHAR_HOOK, wxCharEventHandler(func))
#define EVT_MENU_OPEN(func)  wx__DECLARE_EVT0(wxEVT_MENU_OPEN, wxMenuEventHandler(func))
#define EVT_MENU_CLOSE(func)  wx__DECLARE_EVT0(wxEVT_MENU_CLOSE, wxMenuEventHandler(func))
#define EVT_MENU_HIGHLIGHT(winid, func)  wx__DECLARE_EVT1(wxEVT_MENU_HIGHLIGHT, winid, wxMenuEventHandler(func))
#define EVT_MENU_HIGHLIGHT_ALL(func)  wx__DECLARE_EVT0(wxEVT_MENU_HIGHLIGHT, wxMenuEventHandler(func))
#define EVT_SET_FOCUS(func)  wx__DECLARE_EVT0(wxEVT_SET_FOCUS, wxFocusEventHandler(func))
#define EVT_KILL_FOCUS(func)  wx__DECLARE_EVT0(wxEVT_KILL_FOCUS, wxFocusEventHandler(func))
#define EVT_CHILD_FOCUS(func)  wx__DECLARE_EVT0(wxEVT_CHILD_FOCUS, wxChildFocusEventHandler(func))
#define EVT_ACTIVATE(func)  wx__DECLARE_EVT0(wxEVT_ACTIVATE, wxActivateEventHandler(func))
#define EVT_ACTIVATE_APP(func)  wx__DECLARE_EVT0(wxEVT_ACTIVATE_APP, wxActivateEventHandler(func))
#define EVT_HIBERNATE(func)  wx__DECLARE_EVT0(wxEVT_HIBERNATE, wxActivateEventHandler(func))
#define EVT_END_SESSION(func)  wx__DECLARE_EVT0(wxEVT_END_SESSION, wxCloseEventHandler(func))
#define EVT_QUERY_END_SESSION(func)  wx__DECLARE_EVT0(wxEVT_QUERY_END_SESSION, wxCloseEventHandler(func))
#define EVT_DROP_FILES(func)  wx__DECLARE_EVT0(wxEVT_DROP_FILES, wxDropFilesEventHandler(func))
#define EVT_INIT_DIALOG(func)  wx__DECLARE_EVT0(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(func))
#define EVT_SYS_COLOUR_CHANGED(func) wx__DECLARE_EVT0(wxEVT_SYS_COLOUR_CHANGED, wxSysColourChangedEventHandler(func))
#define EVT_DISPLAY_CHANGED(func)  wx__DECLARE_EVT0(wxEVT_DISPLAY_CHANGED, wxDisplayChangedEventHandler(func))
#define EVT_SHOW(func) wx__DECLARE_EVT0(wxEVT_SHOW, wxShowEventHandler(func))
#define EVT_MAXIMIZE(func) wx__DECLARE_EVT0(wxEVT_MAXIMIZE, wxMaximizeEventHandler(func))
#define EVT_ICONIZE(func) wx__DECLARE_EVT0(wxEVT_ICONIZE, wxIconizeEventHandler(func))
#define EVT_NAVIGATION_KEY(func) wx__DECLARE_EVT0(wxEVT_NAVIGATION_KEY, wxNavigationKeyEventHandler(func))
#define EVT_PALETTE_CHANGED(func) wx__DECLARE_EVT0(wxEVT_PALETTE_CHANGED, wxPaletteChangedEventHandler(func))
#define EVT_QUERY_NEW_PALETTE(func) wx__DECLARE_EVT0(wxEVT_QUERY_NEW_PALETTE, wxQueryNewPaletteEventHandler(func))
#define EVT_WINDOW_CREATE(func) wx__DECLARE_EVT0(wxEVT_CREATE, wxWindowCreateEventHandler(func))
#define EVT_WINDOW_DESTROY(func) wx__DECLARE_EVT0(wxEVT_DESTROY, wxWindowDestroyEventHandler(func))
#define EVT_SET_CURSOR(func) wx__DECLARE_EVT0(wxEVT_SET_CURSOR, wxSetCursorEventHandler(func))
#define EVT_MOUSE_CAPTURE_CHANGED(func) wx__DECLARE_EVT0(wxEVT_MOUSE_CAPTURE_CHANGED, wxMouseCaptureChangedEventHandler(func))
#define EVT_MOUSE_CAPTURE_LOST(func) wx__DECLARE_EVT0(wxEVT_MOUSE_CAPTURE_LOST, wxMouseCaptureLostEventHandler(func))

// Mouse events
#define EVT_LEFT_DOWN(func) wx__DECLARE_EVT0(wxEVT_LEFT_DOWN, wxMouseEventHandler(func))
#define EVT_LEFT_UP(func) wx__DECLARE_EVT0(wxEVT_LEFT_UP, wxMouseEventHandler(func))
#define EVT_MIDDLE_DOWN(func) wx__DECLARE_EVT0(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(func))
#define EVT_MIDDLE_UP(func) wx__DECLARE_EVT0(wxEVT_MIDDLE_UP, wxMouseEventHandler(func))
#define EVT_RIGHT_DOWN(func) wx__DECLARE_EVT0(wxEVT_RIGHT_DOWN, wxMouseEventHandler(func))
#define EVT_RIGHT_UP(func) wx__DECLARE_EVT0(wxEVT_RIGHT_UP, wxMouseEventHandler(func))
#define EVT_MOTION(func) wx__DECLARE_EVT0(wxEVT_MOTION, wxMouseEventHandler(func))
#define EVT_LEFT_DCLICK(func) wx__DECLARE_EVT0(wxEVT_LEFT_DCLICK, wxMouseEventHandler(func))
#define EVT_MIDDLE_DCLICK(func) wx__DECLARE_EVT0(wxEVT_MIDDLE_DCLICK, wxMouseEventHandler(func))
#define EVT_RIGHT_DCLICK(func) wx__DECLARE_EVT0(wxEVT_RIGHT_DCLICK, wxMouseEventHandler(func))
#define EVT_LEAVE_WINDOW(func) wx__DECLARE_EVT0(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(func))
#define EVT_ENTER_WINDOW(func) wx__DECLARE_EVT0(wxEVT_ENTER_WINDOW, wxMouseEventHandler(func))
#define EVT_MOUSEWHEEL(func) wx__DECLARE_EVT0(wxEVT_MOUSEWHEEL, wxMouseEventHandler(func))
#define EVT_MOUSE_AUX1_DOWN(func) wx__DECLARE_EVT0(wxEVT_AUX1_DOWN, wxMouseEventHandler(func))
#define EVT_MOUSE_AUX1_UP(func) wx__DECLARE_EVT0(wxEVT_AUX1_UP, wxMouseEventHandler(func))
#define EVT_MOUSE_AUX1_DCLICK(func) wx__DECLARE_EVT0(wxEVT_AUX1_DCLICK, wxMouseEventHandler(func))
#define EVT_MOUSE_AUX2_DOWN(func) wx__DECLARE_EVT0(wxEVT_AUX2_DOWN, wxMouseEventHandler(func))
#define EVT_MOUSE_AUX2_UP(func) wx__DECLARE_EVT0(wxEVT_AUX2_UP, wxMouseEventHandler(func))
#define EVT_MOUSE_AUX2_DCLICK(func) wx__DECLARE_EVT0(wxEVT_AUX2_DCLICK, wxMouseEventHandler(func))

// All mouse events
#define EVT_MOUSE_EVENTS(func) \
    EVT_LEFT_DOWN(func) \
    EVT_LEFT_UP(func) \
    EVT_LEFT_DCLICK(func) \
    EVT_MIDDLE_DOWN(func) \
    EVT_MIDDLE_UP(func) \
    EVT_MIDDLE_DCLICK(func) \
    EVT_RIGHT_DOWN(func) \
    EVT_RIGHT_UP(func) \
    EVT_RIGHT_DCLICK(func) \
    EVT_MOUSE_AUX1_DOWN(func) \
    EVT_MOUSE_AUX1_UP(func) \
    EVT_MOUSE_AUX1_DCLICK(func) \
    EVT_MOUSE_AUX2_DOWN(func) \
    EVT_MOUSE_AUX2_UP(func) \
    EVT_MOUSE_AUX2_DCLICK(func) \
    EVT_MOTION(func) \
    EVT_LEAVE_WINDOW(func) \
    EVT_ENTER_WINDOW(func) \
    EVT_MOUSEWHEEL(func)

// Scrolling from wxWindow (sent to wxScrolledWindow)
#define EVT_SCROLLWIN_TOP(func) wx__DECLARE_EVT0(wxEVT_SCROLLWIN_TOP, wxScrollWinEventHandler(func))
#define EVT_SCROLLWIN_BOTTOM(func) wx__DECLARE_EVT0(wxEVT_SCROLLWIN_BOTTOM, wxScrollWinEventHandler(func))
#define EVT_SCROLLWIN_LINEUP(func) wx__DECLARE_EVT0(wxEVT_SCROLLWIN_LINEUP, wxScrollWinEventHandler(func))
#define EVT_SCROLLWIN_LINEDOWN(func) wx__DECLARE_EVT0(wxEVT_SCROLLWIN_LINEDOWN, wxScrollWinEventHandler(func))
#define EVT_SCROLLWIN_PAGEUP(func) wx__DECLARE_EVT0(wxEVT_SCROLLWIN_PAGEUP, wxScrollWinEventHandler(func))
#define EVT_SCROLLWIN_PAGEDOWN(func) wx__DECLARE_EVT0(wxEVT_SCROLLWIN_PAGEDOWN, wxScrollWinEventHandler(func))
#define EVT_SCROLLWIN_THUMBTRACK(func) wx__DECLARE_EVT0(wxEVT_SCROLLWIN_THUMBTRACK, wxScrollWinEventHandler(func))
#define EVT_SCROLLWIN_THUMBRELEASE(func) wx__DECLARE_EVT0(wxEVT_SCROLLWIN_THUMBRELEASE, wxScrollWinEventHandler(func))

#define EVT_SCROLLWIN(func) \
    EVT_SCROLLWIN_TOP(func) \
    EVT_SCROLLWIN_BOTTOM(func) \
    EVT_SCROLLWIN_LINEUP(func) \
    EVT_SCROLLWIN_LINEDOWN(func) \
    EVT_SCROLLWIN_PAGEUP(func) \
    EVT_SCROLLWIN_PAGEDOWN(func) \
    EVT_SCROLLWIN_THUMBTRACK(func) \
    EVT_SCROLLWIN_THUMBRELEASE(func)

// Scrolling from wxSlider and wxScrollBar
#define EVT_SCROLL_TOP(func) wx__DECLARE_EVT0(wxEVT_SCROLL_TOP, wxScrollEventHandler(func))
#define EVT_SCROLL_BOTTOM(func) wx__DECLARE_EVT0(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(func))
#define EVT_SCROLL_LINEUP(func) wx__DECLARE_EVT0(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(func))
#define EVT_SCROLL_LINEDOWN(func) wx__DECLARE_EVT0(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(func))
#define EVT_SCROLL_PAGEUP(func) wx__DECLARE_EVT0(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(func))
#define EVT_SCROLL_PAGEDOWN(func) wx__DECLARE_EVT0(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(func))
#define EVT_SCROLL_THUMBTRACK(func) wx__DECLARE_EVT0(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(func))
#define EVT_SCROLL_THUMBRELEASE(func) wx__DECLARE_EVT0(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(func))
#define EVT_SCROLL_CHANGED(func) wx__DECLARE_EVT0(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(func))

#define EVT_SCROLL(func) \
    EVT_SCROLL_TOP(func) \
    EVT_SCROLL_BOTTOM(func) \
    EVT_SCROLL_LINEUP(func) \
    EVT_SCROLL_LINEDOWN(func) \
    EVT_SCROLL_PAGEUP(func) \
    EVT_SCROLL_PAGEDOWN(func) \
    EVT_SCROLL_THUMBTRACK(func) \
    EVT_SCROLL_THUMBRELEASE(func) \
    EVT_SCROLL_CHANGED(func)

// Scrolling from wxSlider and wxScrollBar, with an id
#define EVT_COMMAND_SCROLL_TOP(winid, func) wx__DECLARE_EVT1(wxEVT_SCROLL_TOP, winid, wxScrollEventHandler(func))
#define EVT_COMMAND_SCROLL_BOTTOM(winid, func) wx__DECLARE_EVT1(wxEVT_SCROLL_BOTTOM, winid, wxScrollEventHandler(func))
#define EVT_COMMAND_SCROLL_LINEUP(winid, func) wx__DECLARE_EVT1(wxEVT_SCROLL_LINEUP, winid, wxScrollEventHandler(func))
#define EVT_COMMAND_SCROLL_LINEDOWN(winid, func) wx__DECLARE_EVT1(wxEVT_SCROLL_LINEDOWN, winid, wxScrollEventHandler(func))
#define EVT_COMMAND_SCROLL_PAGEUP(winid, func) wx__DECLARE_EVT1(wxEVT_SCROLL_PAGEUP, winid, wxScrollEventHandler(func))
#define EVT_COMMAND_SCROLL_PAGEDOWN(winid, func) wx__DECLARE_EVT1(wxEVT_SCROLL_PAGEDOWN, winid, wxScrollEventHandler(func))
#define EVT_COMMAND_SCROLL_THUMBTRACK(winid, func) wx__DECLARE_EVT1(wxEVT_SCROLL_THUMBTRACK, winid, wxScrollEventHandler(func))
#define EVT_COMMAND_SCROLL_THUMBRELEASE(winid, func) wx__DECLARE_EVT1(wxEVT_SCROLL_THUMBRELEASE, winid, wxScrollEventHandler(func))
#define EVT_COMMAND_SCROLL_CHANGED(winid, func) wx__DECLARE_EVT1(wxEVT_SCROLL_CHANGED, winid, wxScrollEventHandler(func))

#define EVT_COMMAND_SCROLL(winid, func) \
    EVT_COMMAND_SCROLL_TOP(winid, func) \
    EVT_COMMAND_SCROLL_BOTTOM(winid, func) \
    EVT_COMMAND_SCROLL_LINEUP(winid, func) \
    EVT_COMMAND_SCROLL_LINEDOWN(winid, func) \
    EVT_COMMAND_SCROLL_PAGEUP(winid, func) \
    EVT_COMMAND_SCROLL_PAGEDOWN(winid, func) \
    EVT_COMMAND_SCROLL_THUMBTRACK(winid, func) \
    EVT_COMMAND_SCROLL_THUMBRELEASE(winid, func) \
    EVT_COMMAND_SCROLL_CHANGED(winid, func)

#if WXWIN_COMPATIBILITY_2_6
    // compatibility macros for the old name, deprecated in 2.8
    #define wxEVT_SCROLL_ENDSCROLL wxEVT_SCROLL_CHANGED
    #define EVT_COMMAND_SCROLL_ENDSCROLL EVT_COMMAND_SCROLL_CHANGED
    #define EVT_SCROLL_ENDSCROLL EVT_SCROLL_CHANGED
#endif // WXWIN_COMPATIBILITY_2_6

// Convenience macros for commonly-used commands
#define EVT_CHECKBOX(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_CHECKBOX_CLICKED, winid, wxCommandEventHandler(func))
#define EVT_CHOICE(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_CHOICE_SELECTED, winid, wxCommandEventHandler(func))
#define EVT_LISTBOX(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_LISTBOX_SELECTED, winid, wxCommandEventHandler(func))
#define EVT_LISTBOX_DCLICK(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, winid, wxCommandEventHandler(func))
#define EVT_MENU(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_MENU_SELECTED, winid, wxCommandEventHandler(func))
#define EVT_MENU_RANGE(id1, id2, func) wx__DECLARE_EVT2(wxEVT_COMMAND_MENU_SELECTED, id1, id2, wxCommandEventHandler(func))
#if defined(__SMARTPHONE__)
#  define EVT_BUTTON(winid, func) EVT_MENU(winid, func)
#else
#  define EVT_BUTTON(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_BUTTON_CLICKED, winid, wxCommandEventHandler(func))
#endif
#define EVT_SLIDER(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_SLIDER_UPDATED, winid, wxCommandEventHandler(func))
#define EVT_RADIOBOX(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_RADIOBOX_SELECTED, winid, wxCommandEventHandler(func))
#define EVT_RADIOBUTTON(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_RADIOBUTTON_SELECTED, winid, wxCommandEventHandler(func))
// EVT_SCROLLBAR is now obsolete since we use EVT_COMMAND_SCROLL... events
#define EVT_SCROLLBAR(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_SCROLLBAR_UPDATED, winid, wxCommandEventHandler(func))
#define EVT_VLBOX(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_VLBOX_SELECTED, winid, wxCommandEventHandler(func))
#define EVT_COMBOBOX(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_COMBOBOX_SELECTED, winid, wxCommandEventHandler(func))
#define EVT_TOOL(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_TOOL_CLICKED, winid, wxCommandEventHandler(func))
#define EVT_TOOL_DROPDOWN(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_TOOL_DROPDOWN_CLICKED, winid, wxCommandEventHandler(func))
#define EVT_TOOL_RANGE(id1, id2, func) wx__DECLARE_EVT2(wxEVT_COMMAND_TOOL_CLICKED, id1, id2, wxCommandEventHandler(func))
#define EVT_TOOL_RCLICKED(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_TOOL_RCLICKED, winid, wxCommandEventHandler(func))
#define EVT_TOOL_RCLICKED_RANGE(id1, id2, func) wx__DECLARE_EVT2(wxEVT_COMMAND_TOOL_RCLICKED, id1, id2, wxCommandEventHandler(func))
#define EVT_TOOL_ENTER(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_TOOL_ENTER, winid, wxCommandEventHandler(func))
#define EVT_CHECKLISTBOX(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, winid, wxCommandEventHandler(func))

// Generic command events
#define EVT_COMMAND_LEFT_CLICK(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_LEFT_CLICK, winid, wxCommandEventHandler(func))
#define EVT_COMMAND_LEFT_DCLICK(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_LEFT_DCLICK, winid, wxCommandEventHandler(func))
#define EVT_COMMAND_RIGHT_CLICK(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_RIGHT_CLICK, winid, wxCommandEventHandler(func))
#define EVT_COMMAND_RIGHT_DCLICK(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_RIGHT_DCLICK, winid, wxCommandEventHandler(func))
#define EVT_COMMAND_SET_FOCUS(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_SET_FOCUS, winid, wxCommandEventHandler(func))
#define EVT_COMMAND_KILL_FOCUS(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_KILL_FOCUS, winid, wxCommandEventHandler(func))
#define EVT_COMMAND_ENTER(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_ENTER, winid, wxCommandEventHandler(func))

// Joystick events

#define EVT_JOY_BUTTON_DOWN(func) wx__DECLARE_EVT0(wxEVT_JOY_BUTTON_DOWN, wxJoystickEventHandler(func))
#define EVT_JOY_BUTTON_UP(func) wx__DECLARE_EVT0(wxEVT_JOY_BUTTON_UP, wxJoystickEventHandler(func))
#define EVT_JOY_MOVE(func) wx__DECLARE_EVT0(wxEVT_JOY_MOVE, wxJoystickEventHandler(func))
#define EVT_JOY_ZMOVE(func) wx__DECLARE_EVT0(wxEVT_JOY_ZMOVE, wxJoystickEventHandler(func))

// All joystick events
#define EVT_JOYSTICK_EVENTS(func) \
    EVT_JOY_BUTTON_DOWN(func) \
    EVT_JOY_BUTTON_UP(func) \
    EVT_JOY_MOVE(func) \
    EVT_JOY_ZMOVE(func)

// Idle event
#define EVT_IDLE(func) wx__DECLARE_EVT0(wxEVT_IDLE, wxIdleEventHandler(func))

// Update UI event
#define EVT_UPDATE_UI(winid, func) wx__DECLARE_EVT1(wxEVT_UPDATE_UI, winid, wxUpdateUIEventHandler(func))
#define EVT_UPDATE_UI_RANGE(id1, id2, func) wx__DECLARE_EVT2(wxEVT_UPDATE_UI, id1, id2, wxUpdateUIEventHandler(func))

// Help events
#define EVT_HELP(winid, func) wx__DECLARE_EVT1(wxEVT_HELP, winid, wxHelpEventHandler(func))
#define EVT_HELP_RANGE(id1, id2, func) wx__DECLARE_EVT2(wxEVT_HELP, id1, id2, wxHelpEventHandler(func))
#define EVT_DETAILED_HELP(winid, func) wx__DECLARE_EVT1(wxEVT_DETAILED_HELP, winid, wxHelpEventHandler(func))
#define EVT_DETAILED_HELP_RANGE(id1, id2, func) wx__DECLARE_EVT2(wxEVT_DETAILED_HELP, id1, id2, wxHelpEventHandler(func))

// Context Menu Events
#define EVT_CONTEXT_MENU(func) wx__DECLARE_EVT0(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(func))
#define EVT_COMMAND_CONTEXT_MENU(winid, func) wx__DECLARE_EVT1(wxEVT_CONTEXT_MENU, winid, wxContextMenuEventHandler(func))

// Clipboard text Events
#define EVT_TEXT_CUT(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_TEXT_CUT, winid, wxClipboardTextEventHandler(func))
#define EVT_TEXT_COPY(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_TEXT_COPY, winid, wxClipboardTextEventHandler(func))
#define EVT_TEXT_PASTE(winid, func) wx__DECLARE_EVT1(wxEVT_COMMAND_TEXT_PASTE, winid, wxClipboardTextEventHandler(func))

// ----------------------------------------------------------------------------
// Global data
// ----------------------------------------------------------------------------

// list containing event handlers with pending events for them
//
// notice that each event handler should occur at most once in this list
extern WXDLLIMPEXP_BASE wxList *wxHandlersWithPendingEvents;
#if wxUSE_THREADS
    extern WXDLLIMPEXP_BASE wxCriticalSection *wxHandlersWithPendingEventsLocker;
#endif

// ----------------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------------

#if wxUSE_GUI

// Find a window with the focus, that is also a descendant of the given window.
// This is used to determine the window to initially send commands to.
WXDLLIMPEXP_CORE wxWindow* wxFindFocusDescendant(wxWindow* ancestor);

#endif // wxUSE_GUI

#endif // _WX_EVENT_H_
