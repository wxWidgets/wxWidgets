/////////////////////////////////////////////////////////////////////////////
// Name:        event.cpp
// Purpose:     Event classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "event.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/app.h"
    #include "wx/list.h"

    #if wxUSE_GUI
        #include "wx/control.h"
        #include "wx/utils.h"
        #include "wx/dc.h"
    #endif // wxUSE_GUI
#endif

#include "wx/event.h"

#if wxUSE_GUI
    #include "wx/validate.h"
#endif // wxUSE_GUI


#if defined(__VISAGECPP__) && __IBMCPP__ >= 400
// must define these static for VA or else you get multiply defined symbols everywhere
const wxEventType wxEVT_NULL = 0;
const wxEventType wxEVT_FIRST = 10000;

const wxEventType wxEVT_COMMAND_BUTTON_CLICKED =            wxEVT_FIRST + 1;
const wxEventType wxEVT_COMMAND_CHECKBOX_CLICKED =          wxEVT_FIRST + 2;
const wxEventType wxEVT_COMMAND_CHOICE_SELECTED =           wxEVT_FIRST + 3;
const wxEventType wxEVT_COMMAND_LISTBOX_SELECTED =          wxEVT_FIRST + 4;
const wxEventType wxEVT_COMMAND_LISTBOX_DOUBLECLICKED =     wxEVT_FIRST + 5;
const wxEventType wxEVT_COMMAND_CHECKLISTBOX_TOGGLED =      wxEVT_FIRST + 6;
const wxEventType wxEVT_COMMAND_TEXT_UPDATED =              wxEVT_FIRST + 7;
const wxEventType wxEVT_COMMAND_TEXT_ENTER =                wxEVT_FIRST + 8;
const wxEventType wxEVT_COMMAND_MENU_SELECTED =             wxEVT_FIRST + 9;
const wxEventType wxEVT_COMMAND_TOOL_CLICKED =              wxEVT_COMMAND_MENU_SELECTED;
const wxEventType wxEVT_COMMAND_SLIDER_UPDATED =            wxEVT_FIRST + 10;
const wxEventType wxEVT_COMMAND_RADIOBOX_SELECTED =         wxEVT_FIRST + 11;
const wxEventType wxEVT_COMMAND_RADIOBUTTON_SELECTED =      wxEVT_FIRST + 12;
//const wxEventType wxEVT_COMMAND_SCROLLBAR_UPDATED is now obsolete since we use wxEVT_SCROLL... events
const wxEventType wxEVT_COMMAND_SCROLLBAR_UPDATED =         wxEVT_FIRST + 13;
const wxEventType wxEVT_COMMAND_VLBOX_SELECTED =            wxEVT_FIRST + 14;
const wxEventType wxEVT_COMMAND_COMBOBOX_SELECTED =         wxEVT_FIRST + 15;
const wxEventType wxEVT_COMMAND_TOOL_RCLICKED =             wxEVT_FIRST + 16;
const wxEventType wxEVT_COMMAND_TOOL_ENTER =                wxEVT_FIRST + 17;
const wxEventType wxEVT_COMMAND_SPINCTRL_UPDATED =          wxEVT_FIRST + 18;

/* Sockets send events, too */
const wxEventType wxEVT_SOCKET =                            wxEVT_FIRST + 50;
const wxEventType wxEVT_TIMER  =                            wxEVT_FIRST + 80;

/* Mouse event types */
const wxEventType wxEVT_LEFT_DOWN =                         wxEVT_FIRST + 100;
const wxEventType wxEVT_LEFT_UP =                           wxEVT_FIRST + 101;
const wxEventType wxEVT_MIDDLE_DOWN =                       wxEVT_FIRST + 102;
const wxEventType wxEVT_MIDDLE_UP =                         wxEVT_FIRST + 103;
const wxEventType wxEVT_RIGHT_DOWN =                        wxEVT_FIRST + 104;
const wxEventType wxEVT_RIGHT_UP =                          wxEVT_FIRST + 105;
const wxEventType wxEVT_MOTION =                            wxEVT_FIRST + 106;
const wxEventType wxEVT_ENTER_WINDOW =                      wxEVT_FIRST + 107;
const wxEventType wxEVT_LEAVE_WINDOW =                      wxEVT_FIRST + 108;
const wxEventType wxEVT_LEFT_DCLICK =                       wxEVT_FIRST + 109;
const wxEventType wxEVT_MIDDLE_DCLICK =                     wxEVT_FIRST + 110;
const wxEventType wxEVT_RIGHT_DCLICK =                      wxEVT_FIRST + 111;
const wxEventType wxEVT_SET_FOCUS =                         wxEVT_FIRST + 112;
const wxEventType wxEVT_KILL_FOCUS =                        wxEVT_FIRST + 113;

 /* Non-client mouse events */
const wxEventType wxEVT_NC_LEFT_DOWN =                      wxEVT_FIRST + 200;
const wxEventType wxEVT_NC_LEFT_UP =                        wxEVT_FIRST + 201;
const wxEventType wxEVT_NC_MIDDLE_DOWN =                    wxEVT_FIRST + 202;
const wxEventType wxEVT_NC_MIDDLE_UP =                      wxEVT_FIRST + 203;
const wxEventType wxEVT_NC_RIGHT_DOWN =                     wxEVT_FIRST + 204;
const wxEventType wxEVT_NC_RIGHT_UP =                       wxEVT_FIRST + 205;
const wxEventType wxEVT_NC_MOTION =                         wxEVT_FIRST + 206;
const wxEventType wxEVT_NC_ENTER_WINDOW =                   wxEVT_FIRST + 207;
const wxEventType wxEVT_NC_LEAVE_WINDOW =                   wxEVT_FIRST + 208;
const wxEventType wxEVT_NC_LEFT_DCLICK =                    wxEVT_FIRST + 209;
const wxEventType wxEVT_NC_MIDDLE_DCLICK =                  wxEVT_FIRST + 210;
const wxEventType wxEVT_NC_RIGHT_DCLICK =                   wxEVT_FIRST + 211;

/* Character input event type  */
const wxEventType wxEVT_CHAR =                              wxEVT_FIRST + 212;
const wxEventType wxEVT_CHAR_HOOK =                         wxEVT_FIRST + 213;
const wxEventType wxEVT_NAVIGATION_KEY =                    wxEVT_FIRST + 214;
const wxEventType wxEVT_KEY_DOWN =                          wxEVT_FIRST + 215;
const wxEventType wxEVT_KEY_UP =                            wxEVT_FIRST + 216;

 /*
  * wxScrollbar and wxSlider event identifiers
  */
const wxEventType wxEVT_SCROLL_TOP =                        wxEVT_FIRST + 300;
const wxEventType wxEVT_SCROLL_BOTTOM =                     wxEVT_FIRST + 301;
const wxEventType wxEVT_SCROLL_LINEUP =                     wxEVT_FIRST + 302;
const wxEventType wxEVT_SCROLL_LINEDOWN =                   wxEVT_FIRST + 303;
const wxEventType wxEVT_SCROLL_PAGEUP =                     wxEVT_FIRST + 304;
const wxEventType wxEVT_SCROLL_PAGEDOWN =                   wxEVT_FIRST + 305;
const wxEventType wxEVT_SCROLL_THUMBTRACK =                 wxEVT_FIRST + 306;

 /*
  * Scroll events from wxWindow
  */
const wxEventType wxEVT_SCROLLWIN_TOP =                     wxEVT_FIRST + 320;
const wxEventType wxEVT_SCROLLWIN_BOTTOM =                  wxEVT_FIRST + 321;
const wxEventType wxEVT_SCROLLWIN_LINEUP =                  wxEVT_FIRST + 322;
const wxEventType wxEVT_SCROLLWIN_LINEDOWN =                wxEVT_FIRST + 323;
const wxEventType wxEVT_SCROLLWIN_PAGEUP =                  wxEVT_FIRST + 324;
const wxEventType wxEVT_SCROLLWIN_PAGEDOWN =                wxEVT_FIRST + 325;
const wxEventType wxEVT_SCROLLWIN_THUMBTRACK =              wxEVT_FIRST + 326;

 /*
  * System events
  */
const wxEventType wxEVT_SIZE =                              wxEVT_FIRST + 400;
const wxEventType wxEVT_MOVE =                              wxEVT_FIRST + 401;
const wxEventType wxEVT_CLOSE_WINDOW =                      wxEVT_FIRST + 402;
const wxEventType wxEVT_END_SESSION =                       wxEVT_FIRST + 403;
const wxEventType wxEVT_QUERY_END_SESSION =                 wxEVT_FIRST + 404;
const wxEventType wxEVT_ACTIVATE_APP =                      wxEVT_FIRST + 405;
const wxEventType wxEVT_POWER =                             wxEVT_FIRST + 406;
const wxEventType wxEVT_ACTIVATE =                          wxEVT_FIRST + 409;
const wxEventType wxEVT_CREATE =                            wxEVT_FIRST + 410;
const wxEventType wxEVT_DESTROY =                           wxEVT_FIRST + 411;
const wxEventType wxEVT_SHOW =                              wxEVT_FIRST + 412;
const wxEventType wxEVT_ICONIZE =                           wxEVT_FIRST + 413;
const wxEventType wxEVT_MAXIMIZE =                          wxEVT_FIRST + 414;
const wxEventType wxEVT_MOUSE_CAPTURE_CHANGED =             wxEVT_FIRST + 415;
const wxEventType wxEVT_PAINT =                             wxEVT_FIRST + 416;
const wxEventType wxEVT_ERASE_BACKGROUND =                  wxEVT_FIRST + 417;
const wxEventType wxEVT_NC_PAINT =                          wxEVT_FIRST + 418;
const wxEventType wxEVT_PAINT_ICON =                        wxEVT_FIRST + 419;
const wxEventType wxEVT_MENU_CHAR =                         wxEVT_FIRST + 420;
const wxEventType wxEVT_MENU_INIT =                         wxEVT_FIRST + 421;
const wxEventType wxEVT_MENU_HIGHLIGHT =                    wxEVT_FIRST + 422;
const wxEventType wxEVT_POPUP_MENU_INIT =                   wxEVT_FIRST + 423;
const wxEventType wxEVT_CONTEXT_MENU =                      wxEVT_FIRST + 424;
const wxEventType wxEVT_SYS_COLOUR_CHANGED =                wxEVT_FIRST + 425;
const wxEventType wxEVT_SETTING_CHANGED =                   wxEVT_FIRST + 426;
const wxEventType wxEVT_QUERY_NEW_PALETTE =                 wxEVT_FIRST + 427;
const wxEventType wxEVT_PALETTE_CHANGED =                   wxEVT_FIRST + 428;
const wxEventType wxEVT_JOY_BUTTON_DOWN =                   wxEVT_FIRST + 429;
const wxEventType wxEVT_JOY_BUTTON_UP =                     wxEVT_FIRST + 430;
const wxEventType wxEVT_JOY_MOVE =                          wxEVT_FIRST + 431;
const wxEventType wxEVT_JOY_ZMOVE =                         wxEVT_FIRST + 432;
const wxEventType wxEVT_DROP_FILES =                        wxEVT_FIRST + 433;
const wxEventType wxEVT_DRAW_ITEM =                         wxEVT_FIRST + 434;
const wxEventType wxEVT_MEASURE_ITEM =                      wxEVT_FIRST + 435;
const wxEventType wxEVT_COMPARE_ITEM =                      wxEVT_FIRST + 436;
const wxEventType wxEVT_INIT_DIALOG =                       wxEVT_FIRST + 437;
const wxEventType wxEVT_IDLE =                              wxEVT_FIRST + 438;
const wxEventType wxEVT_UPDATE_UI =                         wxEVT_FIRST + 439;

 /* System misc. */
const wxEventType wxEVT_END_PROCESS =                       wxEVT_FIRST + 440;

 /* Dial up events */
const wxEventType wxEVT_DIALUP_CONNECTED =                  wxEVT_FIRST + 450;
const wxEventType wxEVT_DIALUP_DISCONNECTED =               wxEVT_FIRST + 451;

 /* Generic command events */
 /* Note: a click is a higher-level event than button down/up */
const wxEventType wxEVT_COMMAND_LEFT_CLICK =                wxEVT_FIRST + 500;
const wxEventType wxEVT_COMMAND_LEFT_DCLICK =               wxEVT_FIRST + 501;
const wxEventType wxEVT_COMMAND_RIGHT_CLICK =               wxEVT_FIRST + 502;
const wxEventType wxEVT_COMMAND_RIGHT_DCLICK =              wxEVT_FIRST + 503;
const wxEventType wxEVT_COMMAND_SET_FOCUS =                 wxEVT_FIRST + 504;
const wxEventType wxEVT_COMMAND_KILL_FOCUS =                wxEVT_FIRST + 505;
const wxEventType wxEVT_COMMAND_ENTER =                     wxEVT_FIRST + 506;

 /* Tree control event types */
const wxEventType wxEVT_COMMAND_TREE_BEGIN_DRAG =           wxEVT_FIRST + 600;
const wxEventType wxEVT_COMMAND_TREE_BEGIN_RDRAG =          wxEVT_FIRST + 601;
const wxEventType wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT =     wxEVT_FIRST + 602;
const wxEventType wxEVT_COMMAND_TREE_END_LABEL_EDIT =       wxEVT_FIRST + 603;
const wxEventType wxEVT_COMMAND_TREE_DELETE_ITEM =          wxEVT_FIRST + 604;
const wxEventType wxEVT_COMMAND_TREE_GET_INFO =             wxEVT_FIRST + 605;
const wxEventType wxEVT_COMMAND_TREE_SET_INFO =             wxEVT_FIRST + 606;
const wxEventType wxEVT_COMMAND_TREE_ITEM_EXPANDED =        wxEVT_FIRST + 607;
const wxEventType wxEVT_COMMAND_TREE_ITEM_EXPANDING =       wxEVT_FIRST + 608;
const wxEventType wxEVT_COMMAND_TREE_ITEM_COLLAPSED =       wxEVT_FIRST + 609;
const wxEventType wxEVT_COMMAND_TREE_ITEM_COLLAPSING =      wxEVT_FIRST + 610;
const wxEventType wxEVT_COMMAND_TREE_SEL_CHANGED =          wxEVT_FIRST + 611;
const wxEventType wxEVT_COMMAND_TREE_SEL_CHANGING =         wxEVT_FIRST + 612;
const wxEventType wxEVT_COMMAND_TREE_KEY_DOWN =             wxEVT_FIRST + 613;
const wxEventType wxEVT_COMMAND_TREE_ITEM_ACTIVATED =       wxEVT_FIRST + 614;
const wxEventType wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK =     wxEVT_FIRST + 615;
const wxEventType wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK =    wxEVT_FIRST + 616;

 /* List control event types */
const wxEventType wxEVT_COMMAND_LIST_BEGIN_DRAG =           wxEVT_FIRST + 700;
const wxEventType wxEVT_COMMAND_LIST_BEGIN_RDRAG =          wxEVT_FIRST + 701;
const wxEventType wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT =     wxEVT_FIRST + 702;
const wxEventType wxEVT_COMMAND_LIST_END_LABEL_EDIT =       wxEVT_FIRST + 703;
const wxEventType wxEVT_COMMAND_LIST_DELETE_ITEM =          wxEVT_FIRST + 704;
const wxEventType wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS =     wxEVT_FIRST + 705;
const wxEventType wxEVT_COMMAND_LIST_GET_INFO =             wxEVT_FIRST + 706;
const wxEventType wxEVT_COMMAND_LIST_SET_INFO =             wxEVT_FIRST + 707;
const wxEventType wxEVT_COMMAND_LIST_ITEM_SELECTED =        wxEVT_FIRST + 708;
const wxEventType wxEVT_COMMAND_LIST_ITEM_DESELECTED =      wxEVT_FIRST + 709;
const wxEventType wxEVT_COMMAND_LIST_KEY_DOWN =             wxEVT_FIRST + 710;
const wxEventType wxEVT_COMMAND_LIST_INSERT_ITEM =          wxEVT_FIRST + 711;
const wxEventType wxEVT_COMMAND_LIST_COL_CLICK =            wxEVT_FIRST + 712;
const wxEventType wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK =     wxEVT_FIRST + 713;
const wxEventType wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK =    wxEVT_FIRST + 714;
const wxEventType wxEVT_COMMAND_LIST_ITEM_ACTIVATED =       wxEVT_FIRST + 715;

 /* Tab and notebook control event types */
const wxEventType wxEVT_COMMAND_TAB_SEL_CHANGED =           wxEVT_FIRST + 800;
const wxEventType wxEVT_COMMAND_TAB_SEL_CHANGING =          wxEVT_FIRST + 801;
const wxEventType wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED =     wxEVT_FIRST + 802;
const wxEventType wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING =    wxEVT_FIRST + 803;

/* Splitter events */
const wxEventType wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED = wxEVT_FIRST + 850;
const wxEventType wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING = wxEVT_FIRST + 851;
const wxEventType wxEVT_COMMAND_SPLITTER_DOUBLECLICKED = wxEVT_FIRST + 852;
const wxEventType wxEVT_COMMAND_SPLITTER_UNSPLIT = wxEVT_FIRST + 853;

/* Wizard events */
const wxEventType wxEVT_WIZARD_PAGE_CHANGED = wxEVT_FIRST + 900;
const wxEventType wxEVT_WIZARD_PAGE_CHANGING = wxEVT_FIRST + 901;
const wxEventType wxEVT_WIZARD_CANCEL = wxEVT_FIRST + 902;

/* Calendar events */
const wxEventType wxEVT_CALENDAR_SEL_CHANGED = wxEVT_FIRST + 950;
const wxEventType wxEVT_CALENDAR_DAY_CHANGED = wxEVT_FIRST + 951;
const wxEventType wxEVT_CALENDAR_MONTH_CHANGED = wxEVT_FIRST + 952;
const wxEventType wxEVT_CALENDAR_YEAR_CHANGED = wxEVT_FIRST + 953;
const wxEventType wxEVT_CALENDAR_DOUBLECLICKED = wxEVT_FIRST + 954;
const wxEventType wxEVT_CALENDAR_WEEKDAY_CLICKED = wxEVT_FIRST + 955;

const wxEventType wxEVT_USER_FIRST = wxEVT_FIRST + 2000;
#endif

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

    IMPLEMENT_DYNAMIC_CLASS(wxEvtHandler, wxObject)
    IMPLEMENT_ABSTRACT_CLASS(wxEvent, wxObject)
    IMPLEMENT_DYNAMIC_CLASS(wxIdleEvent, wxEvent)

    #if wxUSE_GUI
        IMPLEMENT_DYNAMIC_CLASS(wxCommandEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxNotifyEvent, wxCommandEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxScrollEvent, wxCommandEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxScrollWinEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxMouseEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxKeyEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxSizeEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxPaintEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxEraseEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxMoveEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxFocusEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxCloseEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxShowEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxMaximizeEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxIconizeEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxMenuEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxJoystickEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxDropFilesEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxActivateEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxInitDialogEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxSysColourChangedEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxUpdateUIEvent, wxCommandEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxNavigationKeyEvent, wxCommandEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxPaletteChangedEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxQueryNewPaletteEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxWindowCreateEvent, wxEvent)
        IMPLEMENT_DYNAMIC_CLASS(wxWindowDestroyEvent, wxEvent)
    #endif // wxUSE_GUI

    const wxEventTable *wxEvtHandler::GetEventTable() const
        { return &wxEvtHandler::sm_eventTable; }

    const wxEventTable wxEvtHandler::sm_eventTable =
        { (const wxEventTable *)NULL, &wxEvtHandler::sm_eventTableEntries[0] };

    const wxEventTableEntry wxEvtHandler::sm_eventTableEntries[] =
        { { 0, 0, 0, (wxObjectEventFunction) NULL, (wxObject*) NULL } };


// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

// To put pending event handlers
wxList *wxPendingEvents = (wxList *)NULL;

#if wxUSE_THREADS
    // protects wxPendingEvents list
    wxCriticalSection *wxPendingEventsLocker = (wxCriticalSection *)NULL;
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxEvent
// ----------------------------------------------------------------------------

/*
 * General wxWindows events, covering
 * all interesting things that might happen (button clicking, resizing,
 * setting text in widgets, etc.).
 *
 * For each completely new event type, derive a new event class.
 *
 */

wxEvent::wxEvent(int theId)
{
    m_eventType = wxEVT_NULL;
    m_eventObject = (wxObject *) NULL;
    m_timeStamp = 0;
    m_id = theId;
    m_skipped = FALSE;
    m_callbackUserData = (wxObject *) NULL;
    m_isCommandEvent = FALSE;
}

void wxEvent::CopyObject(wxObject& object_dest) const
{
    wxEvent *obj = (wxEvent *)&object_dest;
    wxObject::CopyObject(object_dest);

    obj->m_eventType = m_eventType;
    obj->m_eventObject = m_eventObject;
    obj->m_timeStamp = m_timeStamp;
    obj->m_id = m_id;
    obj->m_skipped = m_skipped;
    obj->m_callbackUserData = m_callbackUserData;
    obj->m_isCommandEvent = m_isCommandEvent;
}

#if wxUSE_GUI

/*
 * Command events
 *
 */

wxCommandEvent::wxCommandEvent(wxEventType commandType, int theId)
{
    m_eventType = commandType;
    m_clientData = (char *) NULL;
    m_clientObject = (wxClientData *) NULL;
    m_extraLong = 0;
    m_commandInt = 0;
    m_id = theId;
    m_commandString = wxEmptyString;
    m_isCommandEvent = TRUE;
}

void wxCommandEvent::CopyObject(wxObject& obj_d) const
{
    wxCommandEvent *obj = (wxCommandEvent *)&obj_d;

    wxEvent::CopyObject(obj_d);

    obj->m_clientData    = m_clientData;
    obj->m_clientObject  = m_clientObject;
    obj->m_extraLong     = m_extraLong;
    obj->m_commandInt    = m_commandInt;
    obj->m_commandString = m_commandString;
}

/*
 * Notify events
 */

void wxNotifyEvent::CopyObject(wxObject& obj_d) const
{
    wxNotifyEvent *obj = (wxNotifyEvent *)&obj_d;

    wxEvent::CopyObject(obj_d);

    if (!m_bAllow) obj->Veto();
}

/*
 * Scroll events
 */

wxScrollEvent::wxScrollEvent(wxEventType commandType,
                             int id,
                             int pos,
                             int orient)
             : wxCommandEvent(commandType, id)
{
    m_extraLong = orient;
    m_commandInt = pos;
    m_isScrolling = TRUE;
}

void wxScrollEvent::CopyObject(wxObject& obj_d) const
{
    wxScrollEvent *obj = (wxScrollEvent*)&obj_d;

    wxCommandEvent::CopyObject(obj_d);

    obj->m_isScrolling  = m_isScrolling;
}

/*
 * ScrollWin events
 */

wxScrollWinEvent::wxScrollWinEvent(wxEventType commandType,
                                   int pos,
                                   int orient)
{
    m_eventType = commandType;
    m_extraLong = orient;
    m_commandInt = pos;
    m_isScrolling = TRUE;
}

void wxScrollWinEvent::CopyObject(wxObject& obj_d) const
{
    wxScrollWinEvent *obj = (wxScrollWinEvent*)&obj_d;

    wxEvent::CopyObject(obj_d);

    obj->m_extraLong    = m_extraLong;
    obj->m_commandInt   = m_commandInt;
    obj->m_isScrolling  = m_isScrolling;
}

/*
 * Mouse events
 *
 */

wxMouseEvent::wxMouseEvent(wxEventType commandType)
{
    m_eventType = commandType;
    m_metaDown = FALSE;
    m_altDown = FALSE;
    m_controlDown = FALSE;
    m_shiftDown = FALSE;
    m_leftDown = FALSE;
    m_rightDown = FALSE;
    m_middleDown = FALSE;
    m_x = 0;
    m_y = 0;
}

void wxMouseEvent::CopyObject(wxObject& obj_d) const
{
    wxMouseEvent *obj = (wxMouseEvent *)&obj_d;

    wxEvent::CopyObject(obj_d);

    obj->m_metaDown = m_metaDown;
    obj->m_altDown = m_altDown;
    obj->m_controlDown = m_controlDown;
    obj->m_shiftDown = m_shiftDown;
    obj->m_leftDown = m_leftDown;
    obj->m_rightDown = m_rightDown;
    obj->m_middleDown = m_middleDown;
    obj->m_x = m_x;
    obj->m_y = m_y;
}

// True if was a button dclick event (1 = left, 2 = middle, 3 = right)
// or any button dclick event (but = -1)
bool wxMouseEvent::ButtonDClick(int but) const
{
    switch (but)
    {
        case -1:
            return (LeftDClick() || MiddleDClick() || RightDClick());
        case 1:
            return LeftDClick();
        case 2:
            return MiddleDClick();
        case 3:
            return RightDClick();
        default:
            wxFAIL_MSG(wxT("invalid parameter in wxMouseEvent::ButtonDClick"));
    }

    return FALSE;
}

// True if was a button down event (1 = left, 2 = middle, 3 = right)
// or any button down event (but = -1)
bool wxMouseEvent::ButtonDown(int but) const
{
    switch (but)
    {
        case -1:
            return (LeftDown() || MiddleDown() || RightDown());
        case 1:
            return LeftDown();
        case 2:
            return MiddleDown();
        case 3:
            return RightDown();
        default:
            wxFAIL_MSG(wxT("invalid parameter in wxMouseEvent::ButtonDown"));
    }

    return FALSE;
}

// True if was a button up event (1 = left, 2 = middle, 3 = right)
// or any button up event (but = -1)
bool wxMouseEvent::ButtonUp(int but) const
{
    switch (but) {
        case -1:
            return (LeftUp() || MiddleUp() || RightUp());
        case 1:
            return LeftUp();
        case 2:
            return MiddleUp();
        case 3:
            return RightUp();
        default:
            wxFAIL_MSG(wxT("invalid parameter in wxMouseEvent::ButtonUp"));
    }

    return FALSE;
}

// True if the given button is currently changing state
bool wxMouseEvent::Button(int but) const
{
    switch (but) {
        case -1:
            return (ButtonUp(-1) || ButtonDown(-1) || ButtonDClick(-1));
        case 1:
            return (LeftDown() || LeftUp() || LeftDClick());
        case 2:
            return (MiddleDown() || MiddleUp() || MiddleDClick());
        case 3:
            return (RightDown() || RightUp() || RightDClick());
        default:
            wxFAIL_MSG(wxT("invalid parameter in wxMouseEvent::Button"));
    }

    return FALSE;
}

bool wxMouseEvent::ButtonIsDown(int but) const
{
    switch (but) {
        case -1:
            return (LeftIsDown() || MiddleIsDown() || RightIsDown());
        case 1:
            return LeftIsDown();
        case 2:
            return MiddleIsDown();
        case 3:
            return RightIsDown();
        default:
            wxFAIL_MSG(wxT("invalid parameter in wxMouseEvent::ButtonIsDown"));
    }

    return FALSE;
}

// Find the logical position of the event given the DC
wxPoint wxMouseEvent::GetLogicalPosition(const wxDC& dc) const
{
    wxPoint pt(dc.DeviceToLogicalX(m_x), dc.DeviceToLogicalY(m_y));
    return pt;
}


/*
 * Keyboard events
 *
 */

wxKeyEvent::wxKeyEvent(wxEventType type)
{
    m_eventType = type;
    m_shiftDown = FALSE;
    m_controlDown = FALSE;
    m_metaDown = FALSE;
    m_altDown = FALSE;
    m_keyCode = 0;
    m_scanCode = 0;
}

void wxKeyEvent::CopyObject(wxObject& obj_d) const
{
    wxKeyEvent *obj = (wxKeyEvent *)&obj_d;
    wxEvent::CopyObject(obj_d);

    obj->m_x = m_x;
    obj->m_y = m_y;
    obj->m_keyCode = m_keyCode;
    
    obj->m_shiftDown   = m_shiftDown;
    obj->m_controlDown = m_controlDown;
    obj->m_metaDown    = m_metaDown;
    obj->m_altDown     = m_altDown;
    obj->m_keyCode     = m_keyCode;
}


/*
 * Misc events
 */

void wxSizeEvent::CopyObject(wxObject& obj_d) const
{
    wxSizeEvent *obj = (wxSizeEvent *)&obj_d;
    wxEvent::CopyObject(obj_d);

    obj->m_size = m_size;
}

void wxMoveEvent::CopyObject(wxObject& obj_d) const
{
    wxMoveEvent *obj = (wxMoveEvent *)&obj_d;
    wxEvent::CopyObject(obj_d);

    obj->m_pos = m_pos;
}

void wxEraseEvent::CopyObject(wxObject& obj_d) const
{
    wxEraseEvent *obj = (wxEraseEvent *)&obj_d;
    wxEvent::CopyObject(obj_d);

    obj->m_dc = m_dc;
}

void wxActivateEvent::CopyObject(wxObject& obj_d) const
{
    wxActivateEvent *obj = (wxActivateEvent *)&obj_d;
    wxEvent::CopyObject(obj_d);

    obj->m_active = m_active;
}

void wxMenuEvent::CopyObject(wxObject& obj_d) const
{
    wxMenuEvent *obj = (wxMenuEvent *)&obj_d;
    wxEvent::CopyObject(obj_d);

    obj->m_menuId = m_menuId;
}

void wxCloseEvent::CopyObject(wxObject& obj_d) const
{
    wxCloseEvent *obj = (wxCloseEvent *)&obj_d;
    wxEvent::CopyObject(obj_d);

    obj->m_loggingOff = m_loggingOff;
    obj->m_veto = m_veto;
#if WXWIN_COMPATIBILITY
    obj->m_force = m_force;
#endif
    obj->m_canVeto = m_canVeto;
}

void wxShowEvent::CopyObject(wxObject& obj_d) const
{
    wxShowEvent *obj = (wxShowEvent *)&obj_d;
    wxEvent::CopyObject(obj_d);

    obj->m_show = m_show;
}

void wxJoystickEvent::CopyObject(wxObject& obj_d) const
{
    wxJoystickEvent *obj = (wxJoystickEvent *)&obj_d;
    wxEvent::CopyObject(obj_d);

    obj->m_pos = m_pos;
    obj->m_zPosition = m_zPosition;
    obj->m_buttonChange = m_buttonChange;
    obj->m_buttonState = m_buttonState;
    obj->m_joyStick = m_joyStick;
}

void wxDropFilesEvent::CopyObject(wxObject& obj_d) const
{
    wxDropFilesEvent *obj = (wxDropFilesEvent *)&obj_d;
    wxEvent::CopyObject(obj_d);

    obj->m_noFiles = m_noFiles;
    obj->m_pos = m_pos;
    // TODO: Problem with obj->m_files. It should be deallocated by the
    // destructor of the event.
}

void wxUpdateUIEvent::CopyObject(wxObject &obj_d) const
{
    wxUpdateUIEvent *obj = (wxUpdateUIEvent *)&obj_d;
    wxEvent::CopyObject(obj_d);

    obj->m_checked = m_checked;
    obj->m_enabled = m_enabled;
    obj->m_text = m_text;
    obj->m_setText = m_setText;
    obj->m_setChecked = m_setChecked;
    obj->m_setEnabled = m_setEnabled;
}

void wxPaletteChangedEvent::CopyObject(wxObject &obj_d) const
{
    wxPaletteChangedEvent *obj = (wxPaletteChangedEvent *)&obj_d;
    wxEvent::CopyObject(obj_d);

    obj->m_changedWindow = m_changedWindow;
}

void wxQueryNewPaletteEvent::CopyObject(wxObject& obj_d) const
{
    wxQueryNewPaletteEvent *obj = (wxQueryNewPaletteEvent *)&obj_d;
    wxEvent::CopyObject(obj_d);

    obj->m_paletteRealized = m_paletteRealized;
}

wxWindowCreateEvent::wxWindowCreateEvent(wxWindow *win)
                   : wxEvent()
{
    SetEventType(wxEVT_CREATE);
    SetEventObject(win);
}

wxWindowDestroyEvent::wxWindowDestroyEvent(wxWindow *win)
                    : wxEvent()
{
    SetEventType(wxEVT_DESTROY);
    SetEventObject(win);
}

#endif // wxUSE_GUI

void wxIdleEvent::CopyObject(wxObject& obj_d) const
{
    wxIdleEvent *obj = (wxIdleEvent *)&obj_d;
    wxEvent::CopyObject(obj_d);

    obj->m_requestMore = m_requestMore;
}

/*
 * Event handler
 */

wxEvtHandler::wxEvtHandler()
{
    m_nextHandler = (wxEvtHandler *) NULL;
    m_previousHandler = (wxEvtHandler *) NULL;
    m_enabled = TRUE;
    m_dynamicEvents = (wxList *) NULL;
    m_isWindow = FALSE;
    m_pendingEvents = (wxList *) NULL;
#if wxUSE_THREADS
#  if !defined(__VISAGECPP__)
    m_eventsLocker = new wxCriticalSection;
#  endif
#endif
}

wxEvtHandler::~wxEvtHandler()
{
    // Takes itself out of the list of handlers
    if (m_previousHandler)
        m_previousHandler->m_nextHandler = m_nextHandler;

    if (m_nextHandler)
        m_nextHandler->m_previousHandler = m_previousHandler;

    if (m_dynamicEvents)
    {
        wxNode *node = m_dynamicEvents->First();
        while (node)
        {
            wxEventTableEntry *entry = (wxEventTableEntry*)node->Data();
            if (entry->m_callbackUserData) delete entry->m_callbackUserData;
            delete entry;
            node = node->Next();
        }
        delete m_dynamicEvents;
    };

    delete m_pendingEvents;

#if wxUSE_THREADS
#  if !defined(__VISAGECPP__)
    delete m_eventsLocker;
#  endif
#endif
}

#if wxUSE_THREADS

bool wxEvtHandler::ProcessThreadEvent(wxEvent& event)
{
    // check that we are really in a child thread
    wxASSERT_MSG( !wxThread::IsMain(),
                  wxT("use ProcessEvent() in main thread") );

    AddPendingEvent(event);

    return TRUE;
}

#endif // wxUSE_THREADS

void wxEvtHandler::AddPendingEvent(wxEvent& event)
{
    // 1) Add event to list of pending events of this event handler

#if defined(__VISAGECPP__)
    wxENTER_CRIT_SECT( m_eventsLocker);
#else
    wxENTER_CRIT_SECT( *m_eventsLocker);
#endif

    if ( !m_pendingEvents )
      m_pendingEvents = new wxList;

    wxEvent *event2 = (wxEvent *)event.Clone();

    m_pendingEvents->Append(event2);

#if defined(__VISAGECPP__)
    wxLEAVE_CRIT_SECT( m_eventsLocker);
#else
    wxLEAVE_CRIT_SECT( *m_eventsLocker);
#endif

    // 2) Add this event handler to list of event handlers that
    //    have pending events.

    wxENTER_CRIT_SECT(*wxPendingEventsLocker);

    if ( !wxPendingEvents )
        wxPendingEvents = new wxList;
    wxPendingEvents->Append(this);

    wxLEAVE_CRIT_SECT(*wxPendingEventsLocker);
    
    // 3) Inform the system that new pending events are somwehere,
    //    and that these should be processed in idle time.
    wxWakeUpIdle();
}

void wxEvtHandler::ProcessPendingEvents()
{
#if defined(__VISAGECPP__)
    wxENTER_CRIT_SECT( m_eventsLocker);
#else
    wxENTER_CRIT_SECT( *m_eventsLocker);
#endif

    wxNode *node = m_pendingEvents->First();
    while ( node )
    {
        wxEvent *event = (wxEvent *)node->Data();
        delete node;

        // In ProcessEvent, new events might get added and
	    // we can safely leave the crtical section here.
#if defined(__VISAGECPP__)
        wxLEAVE_CRIT_SECT( m_eventsLocker);
#else
        wxLEAVE_CRIT_SECT( *m_eventsLocker);
#endif
        ProcessEvent(*event);
        delete event;
#if defined(__VISAGECPP__)
        wxENTER_CRIT_SECT( m_eventsLocker);
#else
        wxENTER_CRIT_SECT( *m_eventsLocker);
#endif

        node = m_pendingEvents->First();
    }

#if defined(__VISAGECPP__)
    wxLEAVE_CRIT_SECT( m_eventsLocker);
#else
    wxLEAVE_CRIT_SECT( *m_eventsLocker);
#endif
}

/*
 * Event table stuff
 */

bool wxEvtHandler::ProcessEvent(wxEvent& event)
{
#if wxUSE_GUI
    // check that our flag corresponds to reality
    wxASSERT( m_isWindow == IsKindOf(CLASSINFO(wxWindow)) );
#endif // wxUSE_GUI

    // An event handler can be enabled or disabled
    if ( GetEvtHandlerEnabled() )
    {
    
#if 0
/*
        What is this? When using GUI threads, a non main
        threads can send an event and process it itself.
        This breaks GTK's GUI threads, so please explain.
*/

        // Check whether we are in a child thread.
        if ( !wxThread::IsMain() )
          return ProcessThreadEvent(event);
#endif

        // Handle per-instance dynamic event tables first
        if ( m_dynamicEvents && SearchDynamicEventTable(event) )
            return TRUE;

        // Then static per-class event tables
        const wxEventTable *table = GetEventTable();

#if wxUSE_GUI && wxUSE_VALIDATORS
        // Try the associated validator first, if this is a window.
        // Problem: if the event handler of the window has been replaced,
        // this wxEvtHandler may no longer be a window.
        // Therefore validators won't be processed if the handler
        // has been replaced with SetEventHandler.
        // THIS CAN BE CURED if PushEventHandler is used instead of
        // SetEventHandler, and then processing will be passed down the
        // chain of event handlers.
        if (m_isWindow)
        {
            wxWindow *win = (wxWindow *)this;

            // Can only use the validator of the window which
            // is receiving the event
            if ( win == event.GetEventObject() )
            {
                wxValidator *validator = win->GetValidator();
                if ( validator && validator->ProcessEvent(event) )
                {
                    return TRUE;
                }
            }
        }
#endif

        // Search upwards through the inheritance hierarchy
        while (table)
        {
            if ( SearchEventTable((wxEventTable&)*table, event) )
                return TRUE;
            table = table->baseTable;
        }
    }

    // Try going down the event handler chain
    if ( GetNextHandler() )
    {
        if ( GetNextHandler()->ProcessEvent(event) )
            return TRUE;
    }

#if wxUSE_GUI
    // Carry on up the parent-child hierarchy,
    // but only if event is a command event: it wouldn't
    // make sense for a parent to receive a child's size event, for example
    if ( m_isWindow && event.IsCommandEvent() )
    {
        wxWindow *win = (wxWindow *)this;
        wxWindow *parent = win->GetParent();
        if (parent && !parent->IsBeingDeleted())
            return parent->GetEventHandler()->ProcessEvent(event);
    }
#endif // wxUSE_GUI

    // Last try - application object.
    if ( wxTheApp && (this != wxTheApp) )
    {
        // Special case: don't pass wxEVT_IDLE to wxApp, since it'll always
        // swallow it. wxEVT_IDLE is sent explicitly to wxApp so it will be
        // processed appropriately via SearchEventTable.
        if ( event.GetEventType() != wxEVT_IDLE )
        {
            if ( wxTheApp->ProcessEvent(event) )
                return TRUE;
        }
    }

    return FALSE;
}

bool wxEvtHandler::SearchEventTable(wxEventTable& table, wxEvent& event)
{
    int i = 0;
    int commandId = event.GetId();

    // BC++ doesn't like while (table.entries[i].m_fn)

#ifdef __SC__
    while (table.entries[i].m_fn != 0)
#else
    while (table.entries[i].m_fn != 0L)
#endif
    {
        if ((event.GetEventType() == table.entries[i].m_eventType) &&
                (table.entries[i].m_id == -1 || // Match, if event spec says any id will do (id == -1)
                 (table.entries[i].m_lastId == -1 && commandId == table.entries[i].m_id) ||
                 (table.entries[i].m_lastId != -1 &&
                  (commandId >= table.entries[i].m_id && commandId <= table.entries[i].m_lastId))))
        {
            event.Skip(FALSE);
            event.m_callbackUserData = table.entries[i].m_callbackUserData;

            (this->*((wxEventFunction) (table.entries[i].m_fn)))(event);

            if ( event.GetSkipped() )
                return FALSE;
            else
                return TRUE;
        }
        i++;
    }
    return FALSE;
}

void wxEvtHandler::Connect( int id, int lastId,
                            wxEventType eventType,
                            wxObjectEventFunction func,
                            wxObject *userData )
{
    wxEventTableEntry *entry = new wxEventTableEntry;
    entry->m_id = id;
    entry->m_lastId = lastId;
    entry->m_eventType = eventType;
    entry->m_fn = func;
    entry->m_callbackUserData = userData;

    if (!m_dynamicEvents)
        m_dynamicEvents = new wxList;

    m_dynamicEvents->Append( (wxObject*) entry );
}

bool wxEvtHandler::Disconnect( int id, int lastId, wxEventType eventType,
                  wxObjectEventFunction func,
                  wxObject *userData )
{
    if (!m_dynamicEvents)
        return FALSE;

    wxNode *node = m_dynamicEvents->First();
    while (node)
    {
        wxEventTableEntry *entry = (wxEventTableEntry*)node->Data();
        if ((entry->m_id == id) &&
            ((entry->m_lastId == lastId) || (lastId == -1)) &&
            ((entry->m_eventType == eventType) || (eventType == wxEVT_NULL)) &&
            ((entry->m_fn == func) || (func == (wxObjectEventFunction)NULL)) &&
            ((entry->m_callbackUserData == userData) || (userData == (wxObject*)NULL)))
        {
            if (entry->m_callbackUserData) delete entry->m_callbackUserData;
            m_dynamicEvents->DeleteNode( node );
            delete entry;
            return TRUE;
        }
        node = node->Next();
    }
    return FALSE;
}

bool wxEvtHandler::SearchDynamicEventTable( wxEvent& event )
{
    wxCHECK_MSG( m_dynamicEvents, FALSE,
                 wxT("caller should check that we have dynamic events") );

    int commandId = event.GetId();

    wxNode *node = m_dynamicEvents->First();
    while (node)
    {
        wxEventTableEntry *entry = (wxEventTableEntry*)node->Data();

        if (entry->m_fn)
        {
            // Match, if event spec says any id will do (id == -1)
            if ( (event.GetEventType() == entry->m_eventType) &&
                 (entry->m_id == -1 ||
                  (entry->m_lastId == -1 && commandId == entry->m_id) ||
                  (entry->m_lastId != -1 &&
                  (commandId >= entry->m_id && commandId <= entry->m_lastId))) )
            {
                event.Skip(FALSE);
                event.m_callbackUserData = entry->m_callbackUserData;

                (this->*((wxEventFunction) (entry->m_fn)))(event);

                if (event.GetSkipped())
                    return FALSE;
                else
                    return TRUE;
            }
        }
        node = node->Next();
    }
    return FALSE;
};

#if WXWIN_COMPATIBILITY
bool wxEvtHandler::OnClose()
{
    if (GetNextHandler())
        return GetNextHandler()->OnClose();
    else
        return FALSE;
}
#endif // WXWIN_COMPATIBILITY

#if wxUSE_GUI

// Find a window with the focus, that is also a descendant of the given window.
// This is used to determine the window to initially send commands to.
wxWindow* wxFindFocusDescendant(wxWindow* ancestor)
{
    // Process events starting with the window with the focus, if any.
    wxWindow* focusWin = wxWindow::FindFocus();
    wxWindow* win = focusWin;

    // Check if this is a descendant of this frame.
    // If not, win will be set to NULL.
    while (win)
    {
        if (win == ancestor)
            break;
        else
            win = win->GetParent();
    }
    if (win == (wxWindow*) NULL)
        focusWin = (wxWindow*) NULL;

    return focusWin;
}

#endif // wxUSE_GUI
