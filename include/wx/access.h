/////////////////////////////////////////////////////////////////////////////
// Name:        wx/access.h
// Purpose:     Accessibility classes
// Author:      Julian Smart
// Modified by:
// Created:     2003-02-12
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ACCESSBASE_H_
#define _WX_ACCESSBASE_H_

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "accessbase.h"
#endif

// ----------------------------------------------------------------------------
// headers we have to include here
// ----------------------------------------------------------------------------

#include "wx/variant.h"

typedef enum
{
    wxACC_FAIL,
    wxACC_OK,
    wxACC_NOT_IMPLEMENTED,
    wxACC_NOT_SUPPORTED
} wxAccStatus;

// Child ids are integer identifiers from 1 up.
// So zero represents 'this' object.
#define wxACC_SELF 0

// Navigation constants

typedef enum
{
    wxNAVDIR_DOWN,
    // Navigate to the sibling object located below the starting object.

    wxNAVDIR_FIRSTCHILD,
    // Navigate to the first child of this object. When using this flag,
    // the lVal member of the varStart parameter must be CHILDID_SELF.

    wxNAVDIR_LASTCHILD,
    // Navigate to the last child of this object. When using this flag,
    // the lVal member of the varStart parameter must be CHILDID_SELF.

    wxNAVDIR_LEFT,
    // Navigate to the sibling object located to the left of the starting object.

    wxNAVDIR_NEXT,
    // Navigate to the next logical object, generally a sibling to the starting object.

    wxNAVDIR_PREVIOUS,
    // Navigate to the previous logical object, generally a sibling to the starting object.

    wxNAVDIR_RIGHT,
    // Navigate to the sibling object located to the right of the starting object.

    wxNAVDIR_UP
    // Navigate to the sibling object located above the starting object.
} wxNavDir;

// Role constants

typedef enum {  
    wxROLE_NONE,
        // No assigned role.
    wxROLE_SYSTEM_ALERT,
        // The object represents an alert or a condition that a user should
        // be notified about. This role is used only for objects that embody
        // an alert but are not associated with another user interface element
        // such as a message box, graphic, text, or sound. 
    wxROLE_SYSTEM_ANIMATION,
        // The object represents an animation control, which contains content
        // that changes over time, such as a control that displays a series of
        // bitmap frames, like a film strip. Animation controls are displayed
        // when files are copied, or when some other time-consuming task is performed. 
    wxROLE_SYSTEM_APPLICATION,
        // The object represents a main window for an application. 
    wxROLE_SYSTEM_BORDER,
        // The object represents a window border. The entire border is
        // represented by a single object, rather than by separate objects
        // for each side. 
    wxROLE_SYSTEM_BUTTONDROPDOWN,
        // The object represents a button that drops down a list of items. 
    wxROLE_SYSTEM_BUTTONDROPDOWNGRID,
        // The object represents a button that drops down a grid. 
    wxROLE_SYSTEM_BUTTONMENU,
        // The object represents a button that drops down a menu. 
    wxROLE_SYSTEM_CARET,
        // The object represents the system caret. 
    wxROLE_SYSTEM_CELL,
        // The object represents a cell within a table. 
    wxROLE_SYSTEM_CHARACTER,
        // The object represents a cartoon-like graphic object which is
        // displayed to provide help to users of an application. 
    wxROLE_SYSTEM_CHART,
        // The object represents a graphical image used to represent data. 
    wxROLE_SYSTEM_CHECKBUTTON,
        // The object represents a check box control, an option turned on
        // or off independently of other options. 
    wxROLE_SYSTEM_CLIENT,
        // The object represents a window's client area. 
    wxROLE_SYSTEM_CLOCK,
        // The object represents a control that displays time. 
    wxROLE_SYSTEM_COLUMN,
        // The object represents a column of cells within a table. 
    wxROLE_SYSTEM_COLUMNHEADER,
        // The object represents a column header, providing a visual label
        // for a column in a table. 
    wxROLE_SYSTEM_COMBOBOX,
        // The object represents a combo box; an edit control with an
        // associated list box that provides a set of predefined choices. 
    wxROLE_SYSTEM_CURSOR,
        // The object represents the system mouse pointer. 
    wxROLE_SYSTEM_DIAGRAM,
        // The object represents a graphical image used to diagram data. 
    wxROLE_SYSTEM_DIAL,
        // The object represents a dial or knob. This is also a read-only
        // object with like a speedometer. 
    wxROLE_SYSTEM_DIALOG,
        // The object represents a dialog box or message box. 
    wxROLE_SYSTEM_DOCUMENT,
        // The object represents a document window. A document window is
        // always contained within an application window. This role applies
        // only to multiple-document interface (MDI) windows and refers to
        // the object that contains the MDI title bar. 
    wxROLE_SYSTEM_DROPLIST,
        // The object represents a drop-down list box. It shows one item and
        // allows the user to display and select another from a list of
        // alternative values. 
    wxROLE_SYSTEM_EQUATION,
        // The object represents a mathematical equation. 
    wxROLE_SYSTEM_GRAPHIC,
        // The object represents a picture. 
    wxROLE_SYSTEM_GRIP,
        // The object represents a special mouse pointer, which allows a user
        // to manipulate user interface elements such as a window. For
        // example, a user clicks and drags a sizing grip in the lower-right
        // corner of a window to resize it. 
    wxROLE_SYSTEM_GROUPING,
        // The object logically groups other objects. There is not always a
        // parent-child relationship between the grouping object and the
        // objects it contains. 
    wxROLE_SYSTEM_HELPBALLOON,
        // The object displays help in the form of a ToolTip or help balloon. 
    wxROLE_SYSTEM_HOTKEYFIELD,
        // The object represents a hot-key field that allows the user to
        // enter a combination or sequence of keystrokes. 
    wxROLE_SYSTEM_INDICATOR,
        // The object represents an indicator such as a pointer graphic
        // that points to the current item. 
    wxROLE_SYSTEM_LINK,
        // The object represents a link to something else. This object might
        // look like text or a graphic, but it acts like a button. 
    wxROLE_SYSTEM_LIST,
        // The object represents a list box, allowing the user to select
        // one or more items. 
    wxROLE_SYSTEM_LISTITEM,
        // The object represents an item in a list box or the list portion
        // of a combo box, drop-down list box, or drop-down combo box. 
    wxROLE_SYSTEM_MENUBAR,
        // The object represents the menu bar, following (beneath) the title
        // bar of a window, from which menus are selected by the user. 
    wxROLE_SYSTEM_MENUITEM,
        // The object represents a menu item, which is an entry in a menu
        // that a user can choose to carry out a command, select an option,
        // or display another menu. Functionally, a menu item is equivalent
        // to a push button, radio button, check box, or menu. 
    wxROLE_SYSTEM_MENUPOPUP,
        // The object represents a menu, which presents a list of options
        // from which the user can make a selection to perform an action.
        // All menu types must have this role, including drop-down menus
        // that are displayed by selection from a menu bar, and shortcut
        // menus that are displayed when the right mouse button is clicked. 
    wxROLE_SYSTEM_OUTLINE,
        // The object represents an outline or tree structure, such as a
        // tree view control, which displays a hierarchical list and allows
        // the user to expand and collapse branches. 
    wxROLE_SYSTEM_OUTLINEITEM,
        // The object represents an item in an outline or tree structure. 
    wxROLE_SYSTEM_PAGETAB,
        // The object represents a page tab. The only child of a page tab
        // control is a wxROLE_SYSTEM_GROUPING object that contains the
        // contents of the associated page. 
    wxROLE_SYSTEM_PAGETABLIST,
        // The object represents a container of page tab controls. 
    wxROLE_SYSTEM_PANE,
        // The object represents a pane within a frame or document window.
        // Users can navigate between panes and within the contents of the
        // current pane, but cannot navigate between items in different panes.
        // Thus, panes represent a level of grouping lower than frame windows
        // or documents, but above individual controls. The user navigates
        // between panes by pressing TAB, F6, or CTRL+TAB, depending on
        // the context. 
    wxROLE_SYSTEM_PROGRESSBAR,
        // The object represents a progress bar, dynamically showing the user
        // the percent complete of an operation in progress. This control takes no user input. 
    wxROLE_SYSTEM_PROPERTYPAGE,
        // The object represents a property sheet. 
    wxROLE_SYSTEM_PUSHBUTTON,
        // The object represents a push button control. 
    wxROLE_SYSTEM_RADIOBUTTON,
        // The object represents an option button, also called a radio button.
        // It is one of a group of mutually exclusive options. All objects
        // sharing a single parent that have this attribute are assumed to
        // be part of single mutually exclusive group. Use
        // wxROLE_SYSTEM_GROUPING objects to divide them into separate groups. 
    wxROLE_SYSTEM_ROW,
        // The object represents a row of cells within a table. 
    wxROLE_SYSTEM_ROWHEADER,
        // The object represents a row header, that provides a visual label
        // for a table row. 
    wxROLE_SYSTEM_SCROLLBAR,
        // The object represents a vertical or horizontal scroll bar, which
        // is part of the client area or used in a control. 
    wxROLE_SYSTEM_SEPARATOR,
        // The object is used to visually divide a space into two regions,
        // such as a separator menu item or a bar that divides split panes
        // within a window. 
    wxROLE_SYSTEM_SLIDER,
        // The object represents a slider, which allows the user to adjust
        // a setting in given increments between minimum and maximum values. 
    wxROLE_SYSTEM_SOUND,
        // The object represents a system sound, which is associated with
        // various system events. 
    wxROLE_SYSTEM_SPINBUTTON,
        // The object represents a spin box, which is a control that allows
        // the user to increment or decrement the value displayed in a
        // separate "buddy" control associated with the spin box. 
    wxROLE_SYSTEM_STATICTEXT,
        // The object represents read-only text such as labels for other
        // controls or instructions in a dialog box. Static text cannot be
        // modified or selected. 
    wxROLE_SYSTEM_STATUSBAR,
        // The object represents a status bar, which is an area at the
        // bottom of a window that displays information about the current
        // operation, state of the application, or selected object. The
        // status bar has multiple fields, which display different kinds
        // of information. 
    wxROLE_SYSTEM_TABLE,
        // The object represents a table that contains rows and columns of
        // cells, and optionally, row headers and column headers. 
    wxROLE_SYSTEM_TEXT,
        // The object represents selectable text that allows edits or is read-only. 
    wxROLE_SYSTEM_TITLEBAR,
        // The object represents a title or caption bar for a window. 
    wxROLE_SYSTEM_TOOLBAR,
        // The object represents a toolbar, which is a grouping of controls
        // that provide easy access to frequently used features. 
    wxROLE_SYSTEM_TOOLTIP,
        // The object represents a ToolTip that provides helpful hints. 
    wxROLE_SYSTEM_WHITESPACE,
        // The object represents blank space between other objects. 
    wxROLE_SYSTEM_WINDOW
        // The object represents the window frame, which contains child objects
        // such as a title bar, client, and other objects contained in a window. 
} wxAccRole;

// Accessible states

#define wxACC_STATE_SYSTEM_ALERT_HIGH       0x0000001
/*
        Indicates important information to convey to the user
        immediately. For example, a battery level indicator that
        reaches a critical low level transitions to this state,
        in which case a blind access utility announces this
        information immediately to the user, and a screen
        magnification program scrolls the screen so the battery
        indicator is in view. This state is also appropriate for
        any prompt or operation that must be completed before
        the user can continue. 
*/

#define wxACC_STATE_SYSTEM_ALERT_MEDIUM     0x00000002
/*
        Indicates important information that is not conveyed to
        the user immediately. For example, when a battery level
        indicator is starting to reach a low level, it generates
        a medium-level alert. Blind access utilities then
        generate a sound to let the user know that important
        information is available, without actually interrupting
        the user's work. The user could then query the alert
        information at his or her leisure. 
*/

#define wxACC_STATE_SYSTEM_ALERT_LOW        0x00000004
/*
        Indicates low-priority information that is not important
        to the user. This state is used, for example, when Word
        changes the appearance of the TipWizard button on its
        toolbar to indicate that it has a hint for the user. 
*/

#define wxACC_STATE_SYSTEM_ANIMATED         0x00000008
/*
        The object's appearance changes rapidly or constantly.
        Graphics that are occasionally animated are described as
        wxROLE_SYSTEM_GRAPHIC with the State property set to
        This state is be used to indicate
        that the object's location is changing. 
*/

#define wxACC_STATE_SYSTEM_BUSY             0x00000010
/*
        The control cannot accept input at this time. 
*/

#define wxACC_STATE_SYSTEM_CHECKED          0x00000020
/*
        The object's check box is selected. 
*/

#define wxACC_STATE_SYSTEM_COLLAPSED        0x00000040
/*
        Children of this object that have the
        ROLE_SYSTEM_OUTLINEITEM role are hidden. 
*/

#define wxACC_STATE_SYSTEM_DEFAULT          0x00000080
/*
        The default button or menu item. 
*/

#define wxACC_STATE_SYSTEM_EXPANDED         0x00000100
/*
        Children of this object that have the
        ROLE_SYSTEM_OUTLINEITEM role are displayed. 
*/

#define wxACC_STATE_SYSTEM_EXTSELECTABLE    0x00000200
/*
        Indicates that an object extends its selection using
        SELFLAG_EXTENDSELECTION in the IAccessible::accSelect
        method. 
*/

#define wxACC_STATE_SYSTEM_FLOATING         0x00000400
/*
        The object is not clipped to the boundary of its parent
        object and does not move automatically when the parent
        moves. 
*/

#define wxACC_STATE_SYSTEM_FOCUSABLE        0x00000800
/*
        The object is on the active window and ready to receive
        keyboard focus. 
*/

#define wxACC_STATE_SYSTEM_FOCUSED          0x00001000
/*
        The object has the keyboard focus. Do not confuse object
        focus with object selection. For more information, see
        Selection and Focus Properties and Methods. 
*/

#define wxACC_STATE_SYSTEM_HOTTRACKED       0x00002000
/*
        The object is hot-tracked by the mouse, meaning that its
        appearance has changed to indicate that the mouse
        pointer is located over it. 
*/

#define wxACC_STATE_SYSTEM_INVISIBLE        0x00004000
/*
        Object is hidden or not visible. A list of files names
        in a list box contain several hundred names, but only a
        few are visible to the user. The rest are clipped by the
        parent and have wxACC_STATE_SYSTEM_INVISIBLE set. Objects that
        are never visible are set as wxACC_STATE_SYSTEM_OFFSCREEN.
        Note that an object is considered visible when the
        wxACC_STATE_SYSTEM_INVISIBLE flag is not set, and yet obscured
        by another application, so is not visible to the user.
        For example, an object is considered visible if it
        appears in the main window of an application even though
        it is obscured by a dialog. 
*/

#define wxACC_STATE_SYSTEM_MARQUEED         0x00008000
/*
        Indicates scrolling or moving text or graphics. 
*/

#define wxACC_STATE_SYSTEM_MIXED            0x00010000
/*
        Indicates that the state of a three-state check box or
        toolbar button is not determined. The check box is
        neither checked nor unchecked and is therefore in the
        third or mixed state. 
*/

#define wxACC_STATE_SYSTEM_MULTISELECTABLE  0x00020000
/*
        Object accepts multiple selected items,
        SELFLAG_ADDSELECTION for the IAccessible::accSelect
        method is valid. 
*/

#define wxACC_STATE_SYSTEM_OFFSCREEN        0x00040000
/*
        Indicates that the object has no on-screen
        representation. A sound or alert object has this state,
        or a hidden window that is never made visible. 
*/

#define wxACC_STATE_SYSTEM_PRESSED          0x00080000
/*
        The object is pressed. 
*/

#define wxACC_STATE_SYSTEM_PROTECTED        0x00100000
/*
        The object is a password-protected edit control. 
*/

#define wxACC_STATE_SYSTEM_READONLY         0x00200000
/*
        The object is read-only. 
*/

#define wxACC_STATE_SYSTEM_SELECTABLE       0x00400000
/*
        The object accepts selection. 
*/

#define wxACC_STATE_SYSTEM_SELECTED         0x00800000
/*
        The object is selected. 
*/

#define wxACC_STATE_SYSTEM_SELFVOICING      0x01000000
/*
        The object or child uses text-to-speech (TTS) for
        description purposes. A speech-based accessibility aid
        does not announce information when an object with this
        state has the focus because the object automatically
        announces information. 
*/

#define wxACC_STATE_SYSTEM_UNAVAILABLE      0x02000000
/*
        The object is unavailable. 
*/

// Selection flag

typedef enum
{
    wxACC_SEL_NONE            = 0,
    wxACC_SEL_TAKEFOCUS       = 1,
    wxACC_SEL_TAKESELECTION   = 2,
    wxACC_SEL_EXTENDSELECTION = 4,
    wxACC_SEL_ADDSELECTION    = 8,
    wxACC_SEL_REMOVESELECTION = 16
} wxAccSelectionFlags;

// ----------------------------------------------------------------------------
// wxAccessible
// All functions return an indication of success, failure, or not implemented.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAccessible;
class WXDLLEXPORT wxAccessibleBase : public wxObject
{
    DECLARE_NO_COPY_CLASS(wxAccessibleBase)

public:
    wxAccessibleBase(wxWindow* win): m_window(win) {};
    virtual ~wxAccessibleBase() {};

// Overridables

        // Can return either a child object, or an integer
        // representing the child element, starting from 1.
    virtual wxAccStatus HitTest(const wxPoint& WXUNUSED(pt), int* WXUNUSED(childId), wxAccessible** WXUNUSED(childObject))
         { return wxACC_NOT_IMPLEMENTED; }

        // Returns the rectangle for this object (id = 0) or a child element (id > 0).
    virtual wxAccStatus GetLocation(wxRect& WXUNUSED(rect), int WXUNUSED(elementId))
         { return wxACC_NOT_IMPLEMENTED; }

        // Navigates from fromId to toId/toObject.
    virtual wxAccStatus Navigate(wxNavDir WXUNUSED(navDir), int WXUNUSED(fromId),
                int* WXUNUSED(toId), wxAccessible** WXUNUSED(toObject))
         { return wxACC_NOT_IMPLEMENTED; }

        // Gets the name of the specified object.
    virtual wxAccStatus GetName(int WXUNUSED(childId), wxString* WXUNUSED(name))
         { return wxACC_NOT_IMPLEMENTED; }

        // Gets the number of children.
    virtual wxAccStatus GetChildCount(int* WXUNUSED(childId))
         { return wxACC_NOT_IMPLEMENTED; }

        // Gets the specified child (starting from 1).
        // If *child is NULL and return value is wxACC_OK,
        // this means that the child is a simple element and
        // not an accessible object.
    virtual wxAccStatus GetChild(int WXUNUSED(childId), wxAccessible** WXUNUSED(child))
         { return wxACC_NOT_IMPLEMENTED; }

        // Gets the parent, or NULL.
    virtual wxAccStatus GetParent(wxAccessible** WXUNUSED(parent))
         { return wxACC_NOT_IMPLEMENTED; }

        // Performs the default action. childId is 0 (the action for this object)
        // or > 0 (the action for a child).
        // Return wxACC_NOT_SUPPORTED if there is no default action for this
        // window (e.g. an edit control).
    virtual wxAccStatus DoDefaultAction(int WXUNUSED(childId))
         { return wxACC_NOT_IMPLEMENTED; }

        // Gets the default action for this object (0) or > 0 (the action for a child).
        // Return wxACC_OK even if there is no action. actionName is the action, or the empty
        // string if there is no action.
        // The retrieved string describes the action that is performed on an object,
        // not what the object does as a result. For example, a toolbar button that prints
        // a document has a default action of "Press" rather than "Prints the current document."
    virtual wxAccStatus GetDefaultAction(int WXUNUSED(childId), wxString* WXUNUSED(actionName))
         { return wxACC_NOT_IMPLEMENTED; }

        // Returns the description for this object or a child.
    virtual wxAccStatus GetDescription(int WXUNUSED(childId), wxString* WXUNUSED(description))
         { return wxACC_NOT_IMPLEMENTED; }

        // Returns help text for this object or a child, similar to tooltip text.
    virtual wxAccStatus GetHelpText(int WXUNUSED(childId), wxString* WXUNUSED(helpText))
         { return wxACC_NOT_IMPLEMENTED; }

        // Returns the keyboard shortcut for this object or child.
        // Return e.g. ALT+K
    virtual wxAccStatus GetKeyboardShortcut(int WXUNUSED(childId), wxString* WXUNUSED(shortcut))
         { return wxACC_NOT_IMPLEMENTED; }

        // Returns a role constant.
    virtual wxAccStatus GetRole(int WXUNUSED(childId), wxAccRole* WXUNUSED(role))
         { return wxACC_NOT_IMPLEMENTED; }

        // Returns a state constant.
    virtual wxAccStatus GetState(int WXUNUSED(childId), long* WXUNUSED(state))
         { return wxACC_NOT_IMPLEMENTED; }

        // Returns a localized string representing the value for the object
        // or child.
    virtual wxAccStatus GetValue(int WXUNUSED(childId), wxString* WXUNUSED(strValue))
         { return wxACC_NOT_IMPLEMENTED; }

        // Selects the object or child.
    virtual wxAccStatus Select(int WXUNUSED(childId), wxAccSelectionFlags WXUNUSED(selectFlags))
         { return wxACC_NOT_IMPLEMENTED; }

        // Gets the window with the keyboard focus.
        // If childId is 0 and child is NULL, no object in
        // this subhierarchy has the focus.
        // If this object has the focus, child should be 'this'.
    virtual wxAccStatus GetFocus(int* WXUNUSED(childId), wxAccessible** WXUNUSED(child))
         { return wxACC_NOT_IMPLEMENTED; }

        // Gets a variant representing the selected children
        // of this object.
        // Acceptable values:
        // - a null variant (IsNull() returns TRUE)
        // - a list variant (GetType() == wxT("list"))
        // - an integer representing the selected child element,
        //   or 0 if this object is selected (GetType() == wxT("long"))
        // - a "void*" pointer to a wxAccessible child object
    virtual wxAccStatus GetSelections(wxVariant* WXUNUSED(selections))
         { return wxACC_NOT_IMPLEMENTED; }

// Accessors

        // Returns the window associated with this object.

    wxWindow* GetWindow() { return m_window; }

        // Sets the window associated with this object.

    void SetWindow(wxWindow* window) { m_window = window; }

private:

// Data members

    wxWindow*   m_window;
};


/*

NOTES
Possibly have ways to access other accessible objects.
What if they're not explicitly implemented? Should be able to use
the accessible API to retrieve info about them. Probably
should use a quite different client API, to be able to
retrieve info from other apps too.
But try to avoid implementing this unless absolutely necessary.

 */

// ----------------------------------------------------------------------------
// now include the declaration of the real class
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/ole/access.h"
#elif defined(__WXMOTIF__)
    #include "wx/generic/access.h"
#elif defined(__WXMGL__)
    #include "wx/generic/access.h"
#elif defined(__WXGTK__)
    #include "wx/generic/access.h"
#elif defined(__WXX11__)
    #include "wx/generic/access.h"
#elif defined(__WXMAC__)
    #include "wx/generic/access.h"
#elif defined(__WXPM__)
    #include "wx/generic/access.h"
#endif

#endif
    // _WX_ACCESSBASE_H_

