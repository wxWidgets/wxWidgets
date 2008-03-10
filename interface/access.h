/////////////////////////////////////////////////////////////////////////////
// Name:        access.h
// Purpose:     interface of wxAccessible
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxAccessible
    @wxheader{access.h}

    The wxAccessible class allows wxWidgets applications, and
    wxWidgets itself, to return extended information about user interface elements
    to client applications such as screen readers. This is the
    main way in which wxWidgets implements accessibility features.

    At present, only Microsoft Active Accessibility is supported
    by this class.

    To use this class, derive from wxAccessible, implement appropriate
    functions, and associate an object of the class with a
    window using wxWindow::SetAccessible.

    All functions return an indication of success, failure, or not implemented
    using values of the wxAccStatus enum type.

    If you return wxACC_NOT_IMPLEMENTED from any function, the system will try to
    implement the appropriate functionality. However this will not work with
    all functions.

    Most functions work with an @e object id, which can be zero to refer to
    'this' UI element, or greater than zero to refer to the nth child element.
    This allows you to specify elements that don't have a corresponding wxWindow or
    wxAccessible; for example, the sash of a splitter window.

    For details on the semantics of functions and types, please refer to the
    Microsoft Active Accessibility 1.2 documentation.

    This class is compiled into wxWidgets only if the wxUSE_ACCESSIBILITY setup
    symbol is set to 1.

    @library{wxcore}
    @category{FIXME}
*/
class wxAccessible : public wxObject
{
public:
    /**
        Constructor, taking an optional window. The object can be associated with
        a window later.
    */
    wxAccessible(wxWindow* win = NULL);

    /**
        Destructor.
    */
    ~wxAccessible();

    /**
        Performs the default action for the object. @a childId is 0 (the action for
        this object)
        or greater than 0 (the action for a child). Return wxACC_NOT_SUPPORTED if there
        is no default action for this window (e.g. an edit control).
    */
    virtual wxAccStatus DoDefaultAction(int childId);

    /**
        Gets the specified child (starting from 1). If @a child is @NULL and the return
        value is wxACC_OK,
        this means that the child is a simple element and not an accessible object.
    */
    virtual wxAccStatus GetChild(int childId, wxAccessible** child);

    /**
        Returns the number of children in @e childCount.
    */
    virtual wxAccStatus GetChildCount(int* childCount);

    /**
        Gets the default action for this object (0) or a child (greater than 0).
        Return wxACC_OK even if there is no action. @a actionName is the action, or the
        empty
        string if there is no action. The retrieved string describes the action that is
        performed on an object,
        not what the object does as a result. For example, a toolbar button that prints
        a document has a default action of "Press" rather than "Prints the current
        document."
    */
    virtual wxAccStatus GetDefaultAction(int childId,
                                         wxString* actionName);

    /**
        Returns the description for this object or a child.
    */
    virtual wxAccStatus GetDescription(int childId,
                                       wxString* description);

    /**
        Gets the window with the keyboard focus. If childId is 0 and child is @NULL, no
        object in
        this subhierarchy has the focus. If this object has the focus, child should be
        'this'.
    */
    virtual wxAccStatus GetFocus(int* childId, wxAccessible** child);

    /**
        Returns help text for this object or a child, similar to tooltip text.
    */
    virtual wxAccStatus GetHelpText(int childId, wxString* helpText);

    /**
        Returns the keyboard shortcut for this object or child.
        Return e.g. ALT+K.
    */
    virtual wxAccStatus GetKeyboardShortcut(int childId,
                                            wxString* shortcut);

    /**
        Returns the rectangle for this object (id is 0) or a child element (id is
        greater than 0).
        @a rect is in screen coordinates.
    */
    virtual wxAccStatus GetLocation(wxRect& rect, int elementId);

    /**
        Gets the name of the specified object.
    */
    virtual wxAccStatus GetName(int childId, wxString* name);

    /**
        Returns the parent of this object, or @NULL.
    */
    virtual wxAccStatus GetParent(wxAccessible** parent);

    /**
        Returns a role constant describing this object. See wxAccessible for a list
        of these roles.
    */
    virtual wxAccStatus GetRole(int childId, wxAccRole* role);

    /**
        Gets a variant representing the selected children
        of this object.
        Acceptable values are:
          a null variant (IsNull() returns TRUE)
          a list variant (GetType() == wxT("list"))
          an integer representing the selected child element,
        or 0 if this object is selected (GetType() == wxT("long"))
         a "void*" pointer to a wxAccessible child object
    */
    virtual wxAccStatus GetSelections(wxVariant* selections);

    /**
        Returns a state constant. See wxAccessible for a list
        of these states.
    */
    virtual wxAccStatus GetState(int childId, long* state);

    /**
        Returns a localized string representing the value for the object
        or child.
    */
    virtual wxAccStatus GetValue(int childId, wxString* strValue);

    /**
        Returns the window associated with this object.
    */
    wxWindow* GetWindow();

    /**
        Returns a status value and object id to indicate whether the given point was on
        this or
        a child object. Can return either a child object, or an integer
        representing the child element, starting from 1.
        @a pt is in screen coordinates.
    */
    virtual wxAccStatus HitTest(const wxPoint& pt, int* childId,
                                wxAccessible** childObject);

    /**
        Navigates from @a fromId to @e toId/@e toObject.
    */
    virtual wxAccStatus Navigate(wxNavDir navDir, int fromId,
                                 int* toId,
                                 wxAccessible** toObject);

    /**
        Allows the application to send an event when something changes in an accessible
        object.
    */
    virtual static void NotifyEvent(int eventType, wxWindow* window,
                                    wxAccObject objectType,
                                    int objectType);

    /**
        Selects the object or child. See wxAccessible for a list
        of the selection actions.
    */
    virtual wxAccStatus Select(int childId,
                               wxAccSelectionFlags selectFlags);

    /**
        Sets the window associated with this object.
    */
    void SetWindow(wxWindow* window);
};

