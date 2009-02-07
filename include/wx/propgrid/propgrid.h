/////////////////////////////////////////////////////////////////////////////
// Name:        wx/propgrid/propgrid.h
// Purpose:     wxPropertyGrid
// Author:      Jaakko Salli
// Modified by:
// Created:     2004-09-25
// RCS-ID:      $Id$
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROPGRID_PROPGRID_H_
#define _WX_PROPGRID_PROPGRID_H_

#if wxUSE_PROPGRID

#include "wx/dcclient.h"
#include "wx/scrolwin.h"
#include "wx/tooltip.h"
#include "wx/datetime.h"

#include "wx/propgrid/property.h"
#include "wx/propgrid/propgridiface.h"


#ifndef SWIG
extern WXDLLIMPEXP_DATA_PROPGRID(const char) wxPropertyGridNameStr[];
#endif

class wxPGComboBox;

// -----------------------------------------------------------------------
// Global variables
// -----------------------------------------------------------------------

#ifndef SWIG

// This is required for sharing common global variables.
class WXDLLIMPEXP_PROPGRID wxPGGlobalVarsClass
{
public:

    wxPGGlobalVarsClass();
    ~wxPGGlobalVarsClass();

    // Used by advprops, but here to make things easier.
    wxString            m_pDefaultImageWildcard;

    // Map of editor class instances (keys are name string).
    wxPGHashMapS2P      m_mapEditorClasses;

#if wxUSE_VALIDATORS
    wxVector<wxValidator*>  m_arrValidators; // These wxValidators need to be freed
#endif

    wxPGHashMapS2P      m_dictPropertyClassInfo; // PropertyName -> ClassInfo

    wxPGChoices*        m_fontFamilyChoices;

    // Replace with your own to affect all properties using default renderer.
    wxPGCellRenderer*   m_defaultRenderer;

    wxPGChoices         m_boolChoices;

    wxVariant           m_vEmptyString;
    wxVariant           m_vZero;
    wxVariant           m_vMinusOne;
    wxVariant           m_vTrue;
    wxVariant           m_vFalse;

    // Cached constant strings
    wxPGCachedString    m_strstring;
    wxPGCachedString    m_strlong;
    wxPGCachedString    m_strbool;
    wxPGCachedString    m_strlist;

    wxPGCachedString    m_strMin;
    wxPGCachedString    m_strMax;
    wxPGCachedString    m_strUnits;
    wxPGCachedString    m_strInlineHelp;

    // If true then some things are automatically translated
    bool                m_autoGetTranslation;

    // > 0 if errors cannot or should not be shown in statusbar etc.
    int                 m_offline;

    int                 m_extraStyle;  // global extra style

#ifdef __WXDEBUG__
    int                 m_warnings;
#endif

    int HasExtraStyle( int style ) const { return (m_extraStyle & style); }
};

extern WXDLLIMPEXP_PROPGRID wxPGGlobalVarsClass* wxPGGlobalVars;

#define wxPGVariant_EmptyString     (wxPGGlobalVars->m_vEmptyString)
#define wxPGVariant_Zero            (wxPGGlobalVars->m_vZero)
#define wxPGVariant_MinusOne        (wxPGGlobalVars->m_vMinusOne)
#define wxPGVariant_True            (wxPGGlobalVars->m_vTrue)
#define wxPGVariant_False           (wxPGGlobalVars->m_vFalse)

#define wxPGVariant_Bool(A)     (A?wxPGVariant_True:wxPGVariant_False)

#endif // !SWIG

// -----------------------------------------------------------------------

/** @section propgrid_window_styles wxPropertyGrid Window Styles

    SetWindowStyleFlag method can be used to modify some of these at run-time.
    @{
*/
enum wxPG_WINDOW_STYLES
{

/** This will cause Sort() automatically after an item is added.
    When inserting a lot of items in this mode, it may make sense to
    use Freeze() before operations and Thaw() afterwards to increase
    performance.
*/
wxPG_AUTO_SORT                      = 0x00000010,

/** Categories are not initially shown (even if added).
    IMPORTANT NOTE: If you do not plan to use categories, then this
    style will waste resources.
    This flag can also be changed using wxPropertyGrid::EnableCategories method.
*/
wxPG_HIDE_CATEGORIES                = 0x00000020,

/* This style combines non-categoric mode and automatic sorting.
*/
wxPG_ALPHABETIC_MODE                = (wxPG_HIDE_CATEGORIES|wxPG_AUTO_SORT),

/** Modified values are shown in bold font. Changing this requires Refresh()
    to show changes.
*/
wxPG_BOLD_MODIFIED                  = 0x00000040,

/** When wxPropertyGrid is resized, splitter moves to the center. This
    behavior stops once the user manually moves the splitter.
*/
wxPG_SPLITTER_AUTO_CENTER           = 0x00000080,

/** Display tooltips for cell text that cannot be shown completely. If
    wxUSE_TOOLTIPS is 0, then this doesn't have any effect.
*/
wxPG_TOOLTIPS                       = 0x00000100,

/** Disables margin and hides all expand/collapse buttons that would appear
    outside the margin (for sub-properties). Toggling this style automatically
    expands all collapsed items.
*/
wxPG_HIDE_MARGIN                    = 0x00000200,

/** This style prevents user from moving the splitter.
*/
wxPG_STATIC_SPLITTER                = 0x00000400,

/** Combination of other styles that make it impossible for user to modify
    the layout.
*/
wxPG_STATIC_LAYOUT                  = (wxPG_HIDE_MARGIN|wxPG_STATIC_SPLITTER),

/** Disables wxTextCtrl based editors for properties which
    can be edited in another way.

    Equals calling wxPropertyGrid::LimitPropertyEditing for all added
    properties.
*/
wxPG_LIMITED_EDITING                = 0x00000800,

/** wxPropertyGridManager only: Show toolbar for mode and page selection. */
wxPG_TOOLBAR                        = 0x00001000,

/** wxPropertyGridManager only: Show adjustable text box showing description
    or help text, if available, for currently selected property.
*/
wxPG_DESCRIPTION                    = 0x00002000

};

enum wxPG_EX_WINDOW_STYLES
{

/**
    NOTE: wxPG_EX_xxx are extra window styles and must be set using
    SetExtraStyle() member function.

    Speeds up switching to wxPG_HIDE_CATEGORIES mode. Initially, if
    wxPG_HIDE_CATEGORIES is not defined, the non-categorized data storage is
    not activated, and switching the mode first time becomes somewhat slower.
    wxPG_EX_INIT_NOCAT activates the non-categorized data storage right away.
    IMPORTANT NOTE: If you do plan not switching to non-categoric mode, or if
    you don't plan to use categories at all, then using this style will result
    in waste of resources.

*/
wxPG_EX_INIT_NOCAT                  = 0x00001000,

/** Extended window style that sets wxPropertyGridManager toolbar to not
    use flat style.
*/
wxPG_EX_NO_FLAT_TOOLBAR             = 0x00002000,

/** Shows alphabetic/categoric mode buttons from toolbar.
*/
wxPG_EX_MODE_BUTTONS                = 0x00008000,

/** Show property help strings as tool tips instead as text on the status bar.
    You can set the help strings using SetPropertyHelpString member function.
*/
wxPG_EX_HELP_AS_TOOLTIPS            = 0x00010000,

/** Prevent TAB from focusing to wxButtons. This behavior was default
    in version 1.2.0 and earlier.
    NOTE! Tabbing to button doesn't work yet. Problem seems to be that on wxMSW
      atleast the button doesn't properly propagate key events (yes, I'm using
      wxWANTS_CHARS).
*/
//wxPG_EX_NO_TAB_TO_BUTTON            = 0x00020000,

/** Allows relying on native double-buffering.
*/
wxPG_EX_NATIVE_DOUBLE_BUFFERING         = 0x00080000,

/** Set this style to let user have ability to set values of properties to
    unspecified state. Same as setting wxPG_PROP_AUTO_UNSPECIFIED for
    all properties.
*/
wxPG_EX_AUTO_UNSPECIFIED_VALUES         = 0x00200000,

/**
    If this style is used, built-in attributes (such as wxPG_FLOAT_PRECISION
    and wxPG_STRING_PASSWORD) are not stored into property's attribute storage
    (thus they are not readable).

    Note that this option is global, and applies to all wxPG property
    containers.
*/
wxPG_EX_WRITEONLY_BUILTIN_ATTRIBUTES    = 0x00400000,

/** Hides page selection buttons from toolbar.
*/
wxPG_EX_HIDE_PAGE_BUTTONS               = 0x01000000

};

/** Combines various styles.
*/
#define wxPG_DEFAULT_STYLE          (0)

/** Combines various styles.
*/
#define wxPGMAN_DEFAULT_STYLE       (0)

/** @}
*/

// -----------------------------------------------------------------------

//
// Ids for sub-controls
// NB: It should not matter what these are.
#define wxPG_SUBID1                     2
#define wxPG_SUBID2                     3
#define wxPG_SUBID_TEMP1                4

// -----------------------------------------------------------------------

/** @class wxPGCommonValue

    wxPropertyGrid stores information about common values in these
    records.

    NB: Common value feature is not complete, and thus not mentioned in
        documentation.
*/
class WXDLLIMPEXP_PROPGRID wxPGCommonValue
{
public:

    wxPGCommonValue( const wxString& label, wxPGCellRenderer* renderer )
    {
        m_label = label;
        m_renderer = renderer;
        renderer->IncRef();
    }
    virtual ~wxPGCommonValue()
    {
        m_renderer->DecRef();
    }

    virtual wxString GetEditableText() const { return m_label; }
    const wxString& GetLabel() const { return m_label; }
    wxPGCellRenderer* GetRenderer() const { return m_renderer; }

protected:
    wxString            m_label;
    wxPGCellRenderer*   m_renderer;
};

// -----------------------------------------------------------------------

/** @section propgrid_vfbflags wxPropertyGrid Validation Failure Behavior Flags
    @{
*/

enum wxPG_VALIDATION_FAILURE_BEHAVIOR_FLAGS
{

/** Prevents user from leaving property unless value is valid. If this
    behavior flag is not used, then value change is instead cancelled.
*/
wxPG_VFB_STAY_IN_PROPERTY           = 0x01,

/** Calls wxBell() on validation failure.
*/
wxPG_VFB_BEEP                       = 0x02,

/** Cell with invalid value will be marked (with red colour).
*/
wxPG_VFB_MARK_CELL                  = 0x04,

/** Display customizable text message explaining the situation.
*/
wxPG_VFB_SHOW_MESSAGE               = 0x08,

/** Defaults. */
wxPG_VFB_DEFAULT                    = wxPG_VFB_STAY_IN_PROPERTY|wxPG_VFB_BEEP,

/** Only used internally. */
wxPG_VFB_UNDEFINED                  = 0x80

};

/** @}
*/

typedef wxByte wxPGVFBFlags;

/**
    wxPGValidationInfo

    Used to convey validation information to and from functions that
    actually perform validation. Mostly used in custom property
    classes.
*/
class WXDLLIMPEXP_PROPGRID wxPGValidationInfo
{
    friend class wxPropertyGrid;
public:
    /**
        @return Returns failure behavior which is a combination of
               @ref propgrid_vfbflags.
    */
    wxPGVFBFlags GetFailureBehavior() const
        { return m_failureBehavior; }

    /**
        Returns current failure message.
    */
    const wxString& GetFailureMessage() const
        { return m_failureMessage; }

    /**
        Returns reference to pending value.
    */
    const wxVariant& GetValue() const
    {
        wxASSERT(m_pValue);
        return *m_pValue;
    }

    /** Set validation failure behavior

        @param failureBehavior
            Mixture of @ref propgrid_vfbflags.
    */
    void SetFailureBehavior(wxPGVFBFlags failureBehavior)
        { m_failureBehavior = failureBehavior; }

    /**
        Set current failure message.
    */
    void SetFailureMessage(const wxString& message)
        { m_failureMessage = message; }

private:
    /** Value to be validated.
    */
    wxVariant*      m_pValue;

    /** Message displayed on validation failure.
    */
    wxString        m_failureMessage;

    /** Validation failure behavior. Use wxPG_VFB_XXX flags.
    */
    wxPGVFBFlags    m_failureBehavior;
};

// -----------------------------------------------------------------------

/** @section propgrid_pgactions wxPropertyGrid Action Identifiers

    These are used with wxPropertyGrid::AddActionTrigger() and
    wxPropertyGrid::ClearActionTriggers().
    @{
*/

enum wxPG_KEYBOARD_ACTIONS
{
    wxPG_ACTION_INVALID = 0,
    wxPG_ACTION_NEXT_PROPERTY,
    wxPG_ACTION_PREV_PROPERTY,
    wxPG_ACTION_EXPAND_PROPERTY,
    wxPG_ACTION_COLLAPSE_PROPERTY,
    wxPG_ACTION_CANCEL_EDIT,
    wxPG_ACTION_PRESS_BUTTON,  // Causes editor button (if any) to be pressed
    wxPG_ACTION_MAX
};

/** @}
*/

// -----------------------------------------------------------------------


// wxPropertyGrid::DoSelectProperty flags (selFlags)

// Focuses to created editor
#define wxPG_SEL_FOCUS      0x01
// Forces deletion and recreation of editor
#define wxPG_SEL_FORCE      0x02
// For example, doesn't cause EnsureVisible
#define wxPG_SEL_NONVISIBLE 0x04
// Do not validate editor's value before selecting
#define wxPG_SEL_NOVALIDATE 0x08
// Property being deselected is about to be deleted
#define wxPG_SEL_DELETING   0x10
// Property's values was set to unspecified by the user
#define wxPG_SEL_SETUNSPEC  0x20
// Property's event handler changed the value
#define wxPG_SEL_DIALOGVAL  0x40


// -----------------------------------------------------------------------

#ifndef SWIG

// Internal flags
#define wxPG_FL_INITIALIZED                 0x0001
// Set when creating editor controls if it was clicked on.
#define wxPG_FL_ACTIVATION_BY_CLICK         0x0002
#define wxPG_FL_DONT_CENTER_SPLITTER        0x0004
#define wxPG_FL_FOCUSED                     0x0008
#define wxPG_FL_MOUSE_CAPTURED              0x0010
#define wxPG_FL_MOUSE_INSIDE                0x0020
#define wxPG_FL_VALUE_MODIFIED              0x0040
// don't clear background of m_wndEditor
#define wxPG_FL_PRIMARY_FILLS_ENTIRE        0x0080
// currently active editor uses custom image
#define wxPG_FL_CUR_USES_CUSTOM_IMAGE       0x0100
// cell colours override selection colours for selected cell
#define wxPG_FL_CELL_OVERRIDES_SEL          0x0200
#define wxPG_FL_SCROLLED                    0x0400
// set when all added/inserted properties get hideable flag
#define wxPG_FL_ADDING_HIDEABLES            0x0800
// Disables showing help strings on statusbar.
#define wxPG_FL_NOSTATUSBARHELP             0x1000
// Marks that we created the state, so we have to destroy it too.
#define wxPG_FL_CREATEDSTATE                0x2000
// Set if scrollbar's existence was detected in last onresize.
#define wxPG_FL_SCROLLBAR_DETECTED          0x4000
// Set if wxPGMan requires redrawing of description text box.
#define wxPG_FL_DESC_REFRESH_REQUIRED       0x8000
// Set if contained in wxPropertyGridManager
#define wxPG_FL_IN_MANAGER                  0x00020000
// Set after wxPropertyGrid is shown in its initial good size
#define wxPG_FL_GOOD_SIZE_SET               0x00040000
// Set when in SelectProperty.
#define wxPG_FL_IN_SELECT_PROPERTY          0x00100000
// Set when help string is shown in status bar
#define wxPG_FL_STRING_IN_STATUSBAR         0x00200000
// Splitter position has been custom-set by the user
#define wxPG_FL_SPLITTER_PRE_SET            0x00400000
// Validation failed. Clear on modify event.
#define wxPG_FL_VALIDATION_FAILED           0x00800000
// Auto sort is enabled (for categorized mode)
#define wxPG_FL_CATMODE_AUTO_SORT           0x01000000
// Set after page has been inserted to manager
#define wxPG_MAN_FL_PAGE_INSERTED           0x02000000
// Active editor control is abnormally large
#define wxPG_FL_ABNORMAL_EDITOR             0x04000000
// Recursion guard for HandleCustomEditorEvent
#define wxPG_FL_IN_HANDLECUSTOMEDITOREVENT  0x08000000
#define wxPG_FL_VALUE_CHANGE_IN_EVENT       0x10000000
// Editor control width should not change on resize
#define wxPG_FL_FIXED_WIDTH_EDITOR          0x20000000
// Width of panel can be different than width of grid
#define wxPG_FL_HAS_VIRTUAL_WIDTH           0x40000000
// Prevents RecalculateVirtualSize re-entrancy
#define wxPG_FL_RECALCULATING_VIRTUAL_SIZE  0x80000000

#endif // #ifndef SWIG

#if !defined(__wxPG_SOURCE_FILE__) && !defined(SWIG)
    // Reduce compile time, but still include in user app
    #include "wx/propgrid/props.h"
#endif

// -----------------------------------------------------------------------

/** @class wxPropertyGrid

    wxPropertyGrid is a specialized grid for editing properties
    such as strings, numbers, flagsets, fonts, and colours. wxPropertySheet
    used to do the very same thing, but it hasn't been updated for a while
    and it is currently deprecated.

    Please note that most member functions are inherited and as such not
    documented on this page. This means you will probably also want to read
    wxPropertyGridInterface class reference.

    See also @ref overview_propgrid.

    @section propgrid_window_styles_ Window Styles

    See @ref propgrid_window_styles.

    @section propgrid_event_handling Event Handling

    To process input from a propertygrid control, use these event handler
    macros to direct input to member functions that take a wxPropertyGridEvent
    argument.

    @beginEventTable{wxPropertyGridEvent}
    @event{EVT_PG_SELECTED (id, func)}
        Respond to wxEVT_PG_SELECTED event, generated when property value
        has been changed by user.
    @event{EVT_PG_CHANGING(id, func)}
        Respond to wxEVT_PG_CHANGING event, generated when property value
        is about to be changed by user. Use wxPropertyGridEvent::GetValue()
        to take a peek at the pending value, and wxPropertyGridEvent::Veto()
        to prevent change from taking place, if necessary.
    @event{EVT_PG_HIGHLIGHTED(id, func)}
        Respond to wxEVT_PG_HIGHLIGHTED event, which occurs when mouse
        moves over a property. Event's property is NULL if hovered area does
        not belong to any property.
    @event{EVT_PG_RIGHT_CLICK(id, func)}
        Respond to wxEVT_PG_RIGHT_CLICK event, which occurs when property is
        clicked on with right mouse button.
    @event{EVT_PG_DOUBLE_CLICK(id, func)}
        Respond to wxEVT_PG_DOUBLE_CLICK event, which occurs when property is
        double-clicked onwith left mouse button.
    @event{EVT_PG_ITEM_COLLAPSED(id, func)}
        Respond to wxEVT_PG_ITEM_COLLAPSED event, generated when user collapses
        a property or category..
    @event{EVT_PG_ITEM_EXPANDED(id, func)}
        Respond to wxEVT_PG_ITEM_EXPANDED event, generated when user expands
        a property or category..
    @endEventTable

    @remarks

    - Use Freeze() and Thaw() respectively to disable and enable drawing. This
      will also delay sorting etc. miscellaneous calculations to the last
      possible moment.

    @library{wxpropgrid}
    @category{propgrid}
*/
class WXDLLIMPEXP_PROPGRID
    wxPropertyGrid : public wxScrolledWindow, public wxPropertyGridInterface
{
    friend class wxPropertyGridPageState;
    friend class wxPropertyGridInterface;
    friend class wxPropertyGridManager;
    friend class wxPGCanvas;

#ifndef SWIG
    DECLARE_DYNAMIC_CLASS(wxPropertyGrid)
#endif
public:

#ifdef SWIG
    %pythonAppend wxPropertyGrid {
        self._setOORInfo(self)
        self.DoDefaultTypeMappings()
        self.edited_objects = {}
        self.DoDefaultValueTypeMappings()
        if not hasattr(self.__class__,'_vt2setter'):
            self.__class__._vt2setter = {}
    }
    %pythonAppend wxPropertyGrid() ""

    wxPropertyGrid( wxWindow *parent, wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxPG_DEFAULT_STYLE,
                    const wxChar* name = wxPyPropertyGridNameStr );
    %RenameCtor(PrePropertyGrid,  wxPropertyGrid());
#else

    /**
        Two step constructor.

        Call Create when this constructor is called to build up the
        wxPropertyGrid
    */
    wxPropertyGrid();

    /** The default constructor. The styles to be used are styles valid for
        the wxWindow and wxScrolledWindow.

        @see @link wndflags Additional Window Styles @endlink
    */
    wxPropertyGrid( wxWindow *parent, wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxPG_DEFAULT_STYLE,
                    const wxString& name = wxPropertyGridNameStr );

    /** Destructor */
    virtual ~wxPropertyGrid();
#endif

    /** Adds given key combination to trigger given action.

        @param action
            Which action to trigger. See @link pgactions List of list of
            wxPropertyGrid actions@endlink.
    */
    void AddActionTrigger( int action, int keycode, int modifiers = 0 );

    /**
        This static function enables or disables automatic use of
        wxGetTranslation for following strings: wxEnumProperty list labels,
        wxFlagsProperty sub-property labels.

        Default is false.
    */
    static void AutoGetTranslation( bool enable );

    /**
        Changes value of a property, as if from an editor.

        Use this instead of SetPropertyValue() if you need the value to run
        through validation process, and also send the property change event.

        @return
        Returns true if value was successfully changed.
    */
    bool ChangePropertyValue( wxPGPropArg id, wxVariant newValue );

    /**
        Centers the splitter.

        If argument is true, automatic splitter centering is enabled (only
        applicapple if style wxPG_SPLITTER_AUTO_CENTER was defined).
    */
    void CenterSplitter( bool enable_auto_centering = false );

    /** Deletes all properties.
    */
    virtual void Clear();

    /** Clears action triggers for given action.
        @param action
            Which action to trigger. See @link pgactions List of list of
            wxPropertyGrid actions@endlink.
    */
    void ClearActionTriggers( int action );

    /** Forces updating the value of property from the editor control.

        Note that wxEVT_PG_CHANGING and wxEVT_PG_CHANGED are dispatched using
        ProcessEvent, meaning your event handlers will be called immediately.

        @return
        Returns true if anything was changed.
    */
    virtual bool CommitChangesFromEditor( wxUint32 flags = 0 );

    /**
        Two step creation.

        Whenever the control is created without any parameters, use Create to
        actually create it. Don't access the control's public methods before
        this is called @see @link wndflags Additional Window Styles@endlink
    */
    bool Create( wxWindow *parent, wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxPG_DEFAULT_STYLE,
                 const wxString& name = wxPropertyGridNameStr );

    /**
        Call when editor widget's contents is modified.

        For example, this is called when changes text in wxTextCtrl (used in
        wxStringProperty and wxIntProperty).

        @remarks
        This function should only be called by custom properties.

        @see wxPGProperty::OnEvent()
    */
    void EditorsValueWasModified() { m_iFlags |= wxPG_FL_VALUE_MODIFIED; }

    /** Reverse of EditorsValueWasModified().

        @remarks
        This function should only be called by custom properties.
    */
    void EditorsValueWasNotModified()
    {
        m_iFlags &= ~(wxPG_FL_VALUE_MODIFIED);
    }

    /**
        Enables or disables (shows/hides) categories according to parameter
        enable.
     */
    bool EnableCategories( bool enable );

    /** Scrolls and/or expands items to ensure that the given item is visible.
        Returns true if something was actually done.
    */
    bool EnsureVisible( wxPGPropArg id );

    /**
        Reduces column sizes to minimum possible that contents are still
        visibly (naturally some margin space will be applied as well).

        @return
        Minimum size for the grid to still display everything.

        @remarks
        Does not work well with wxPG_SPLITTER_AUTO_CENTER window style.

        This function only works properly if grid size prior to call was already
        fairly large.

        Note that you can also get calculated column widths by calling
        GetState->GetColumnWidth() immediately after this function returns.
    */
    wxSize FitColumns()
    {
        wxSize sz = m_pState->DoFitColumns();
        return sz;
    }

    /**
        Returns wxWindow that the properties are painted on, and which should
        be used as the parent for editor controls.
    */
    wxPanel* GetPanel() const
    {
        return m_canvas;
    }

    /** Returns current category caption background colour. */
    wxColour GetCaptionBackgroundColour() const { return m_colCapBack; }

    wxFont& GetCaptionFont() { return m_captionFont; }

    const wxFont& GetCaptionFont() const { return m_captionFont; }

    /** Returns current category caption text colour. */
    wxColour GetCaptionForegroundColour() const { return m_colCapFore; }

    /** Returns current cell background colour. */
    wxColour GetCellBackgroundColour() const { return m_colPropBack; }

    /** Returns current cell text colour when disabled. */
    wxColour GetCellDisabledTextColour() const { return m_colDisPropFore; }

    /** Returns current cell text colour. */
    wxColour GetCellTextColour() const { return m_colPropFore; }

    /**
        Returns number of columns currently on grid.
    */
    unsigned int GetColumnCount() const
    {
        return (unsigned int) m_pState->m_colWidths.size();
    }

    /** Returns colour of empty space below properties. */
    wxColour GetEmptySpaceColour() const { return m_colEmptySpace; }

    /** Returns height of highest characters of used font. */
    int GetFontHeight() const { return m_fontHeight; }

    /** Returns pointer to itself. Dummy function that enables same kind
        of code to use wxPropertyGrid and wxPropertyGridManager.
    */
    wxPropertyGrid* GetGrid() { return this; }

    /** Returns rectangle of custom paint image.
    */
    wxRect GetImageRect( wxPGProperty* p, int item ) const;

    /** Returns size of the custom paint image in front of property.
        If no argument is given, returns preferred size.
    */
    wxSize GetImageSize( wxPGProperty* p = NULL, int item = -1 ) const;

    //@{
    /** Returns last item which could be iterated using given flags.
        @param flags
        See @ref propgrid_iterator_flags.
    */
    wxPGProperty* GetLastItem( int flags = wxPG_ITERATE_DEFAULT )
    {
        return m_pState->GetLastItem(flags);
    }

    const wxPGProperty* GetLastItem( int flags = wxPG_ITERATE_DEFAULT ) const
    {
        return m_pState->GetLastItem(flags);
    }
    //@}

    /** Returns colour of lines between cells. */
    wxColour GetLineColour() const { return m_colLine; }

    /** Returns background colour of margin. */
    wxColour GetMarginColour() const { return m_colMargin; }

    /**
        Returns most up-to-date value of selected property. This will return
        value different from GetSelectedProperty()->GetValue() only when text
        editor is activate and string edited by user represents valid,
        uncommitted property value.
    */
    wxVariant GetUncommittedPropertyValue();

    /** Returns "root property". It does not have name, etc. and it is not
        visible. It is only useful for accessing its children.
    */
    wxPGProperty* GetRoot() const { return m_pState->m_properties; }

    /** Returns height of a single grid row (in pixels). */
    int GetRowHeight() const { return m_lineHeight; }

    /** Returns currently selected property. */
    wxPGProperty* GetSelectedProperty() const { return GetSelection(); }

    /** Returns currently selected property. */
    wxPGProperty* GetSelection() const
    {
        return m_selected;
    }

    /** Returns current selection background colour. */
    wxColour GetSelectionBackgroundColour() const { return m_colSelBack; }

    /** Returns current selection text colour. */
    wxColour GetSelectionForegroundColour() const { return m_colSelFore; }

    /** Returns current splitter x position. */
    int GetSplitterPosition() const
        { return m_pState->DoGetSplitterPosition(0); }

    /** Returns wxTextCtrl active in currently selected property, if any. Takes
        into account wxOwnerDrawnComboBox.
    */
    wxTextCtrl* GetEditorTextCtrl() const;

    wxPGValidationInfo& GetValidationInfo()
    {
        return m_validationInfo;
    }

    /** Returns current vertical spacing. */
    int GetVerticalSpacing() const { return (int)m_vspacing; }

    /** Returns true if editor's value was marked modified.
    */
    bool IsEditorsValueModified() const
        { return  ( m_iFlags & wxPG_FL_VALUE_MODIFIED ) ? true : false; }

    /** Returns information about arbitrary position in the grid.

        For wxPropertyGridHitTestResult definition, see
        wxPropertyGridPageState::HitTest().
    */
    wxPropertyGridHitTestResult HitTest( const wxPoint& pt ) const;

    /** Returns true if any property has been modified by the user. */
    bool IsAnyModified() const { return (m_pState->m_anyModified>0); }

    /**
        Returns true if updating is frozen (ie Freeze() called but not yet
        Thaw() ).
     */
    bool IsFrozen() const { return (m_frozen>0)?true:false; }

    /** Redraws given property.
    */
    virtual void RefreshProperty( wxPGProperty* p );

#ifndef SWIG
    /** Registers a new editor class.
        @return
        Pointer to the editor class instance that should be used.
    */
    static wxPGEditor* RegisterEditorClass( wxPGEditor* editor,
                                            bool noDefCheck = false );
#endif

    /** Resets all colours to the original system values.
    */
    void ResetColours();

    /**
        Selects a property.
        Editor widget is automatically created, but not focused unless focus is
        true. This will generate wxEVT_PG_SELECT event.
        @param id
            Property to select.
        @return
            True if selection finished successfully. Usually only fails if
            current value in editor is not valid.
        @see wxPropertyGrid::Unselect
    */
    bool SelectProperty( wxPGPropArg id, bool focus = false )
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)
        return DoSelectProperty(p,focus?wxPG_SEL_FOCUS:0);
    }

    /** Sets category caption background colour. */
    void SetCaptionBackgroundColour(const wxColour& col);

    /** Sets category caption text colour. */
    void SetCaptionTextColour(const wxColour& col);

    /** Sets default cell background colour - applies to property cells.
        Note that appearance of editor widgets may not be affected.
    */
    void SetCellBackgroundColour(const wxColour& col);

    /** Sets cell text colour for disabled properties.
    */
    void SetCellDisabledTextColour(const wxColour& col);

    /** Sets default cell text colour - applies to property name and value text.
        Note that appearance of editor widgets may not be affected.
    */
    void SetCellTextColour(const wxColour& col);

    /** Set number of columns (2 or more).
    */
    void SetColumnCount( int colCount )
    {
        m_pState->SetColumnCount(colCount);
        Refresh();
    }

    /**
        Sets the 'current' category - Append will add non-category properties
        under it.
    */
    void SetCurrentCategory( wxPGPropArg id )
    {
        wxPG_PROP_ARG_CALL_PROLOG()
        wxPropertyCategory* pc = wxDynamicCast(p, wxPropertyCategory);
        wxASSERT(pc);
        m_pState->m_currentCategory = pc;
    }

    /** Sets colour of empty space below properties. */
    void SetEmptySpaceColour(const wxColour& col);

    /** Sets colour of lines between cells. */
    void SetLineColour(const wxColour& col);

    /** Sets background colour of margin. */
    void SetMarginColour(const wxColour& col);

    /**
        Sets selection background colour - applies to selected property name
        background.
    */
    void SetSelectionBackgroundColour(const wxColour& col);

    /**
        Sets selection foreground colour - applies to selected property name
        text.
     */
    void SetSelectionTextColour(const wxColour& col);

    /** Sets x coordinate of the splitter.
        @remarks
        Splitter position cannot exceed grid size, and therefore setting it
        during form creation may fail as initial grid size is often smaller
        than desired splitter position, especially when sizers are being used.
    */
    void SetSplitterPosition( int newxpos, int col = 0 )
    {
        DoSetSplitterPosition_(newxpos, true, col);
        m_iFlags |= wxPG_FL_SPLITTER_PRE_SET;
    }

    /**
        Sets the property sorting function.

        @param sortFunction
            The sorting function to be used. It should return a value greater
            than 0 if position of p1 is after p2. So, for instance, when
            comparing property names, you can use following implementation:

            @code
                int MyPropertySortFunction(wxPropertyGrid* propGrid,
                                           wxPGProperty* p1,
                                           wxPGProperty* p2)
                {
                    return p1->GetBaseName().compare( p2->GetBaseName() );
                }
            @endcode

        @remarks
            Default property sort function sorts properties by their labels
            (case-insensitively).

        @see GetSortFunction, wxPropertyGridInterface::Sort,
             wxPropertyGridInterface::SortChildren
    */
    void SetSortFunction( wxPGSortCallback sortFunction )
    {
        m_sortFunction = sortFunction;
    }

    /**
        Returns the property sort function (default is @NULL).

        @see SetSortFunction
    */
    wxPGSortCallback GetSortFunction() const
    {
        return m_sortFunction;
    }

    /** Set virtual width for this particular page. Width -1 indicates that the
        virtual width should be disabled. */
    void SetVirtualWidth( int width );

    /**
        Moves splitter as left as possible, while still allowing all
        labels to be shown in full.

        @param privateChildrenToo
            If @false, will still allow private children to be cropped.
    */
    void SetSplitterLeft( bool privateChildrenToo = false )
    {
        m_pState->SetSplitterLeft(privateChildrenToo);
    }

    /** Sets vertical spacing. Can be 1, 2, or 3 - a value relative to font
        height. Value of 2 should be default on most platforms.
    */
    void SetVerticalSpacing( int vspacing )
    {
        m_vspacing = (unsigned char)vspacing;
        CalculateFontAndBitmapStuff( vspacing );
        if ( !m_pState->m_itemsAdded ) Refresh();
    }

    /** Shows an brief error message that is related to a property. */
    void ShowPropertyError( wxPGPropArg id, const wxString& msg )
    {
        wxPG_PROP_ARG_CALL_PROLOG()
        DoShowPropertyError(p, msg);
    }

    /////////////////////////////////////////////////////////////////
    //
    // Following methods do not need to be (currently) documented
    //
    /////////////////////////////////////////////////////////////////

    bool HasVirtualWidth() const
        { return (m_iFlags & wxPG_FL_HAS_VIRTUAL_WIDTH) ? true : false; }

    const wxPGCommonValue* GetCommonValue( unsigned int i ) const
    {
        return (wxPGCommonValue*) m_commonValues[i];
    }

    /** Returns number of common values.
    */
    unsigned int GetCommonValueCount() const
    {
        return (unsigned int) m_commonValues.size();
    }

    /** Returns label of given common value.
    */
    wxString GetCommonValueLabel( unsigned int i ) const
    {
        wxASSERT( GetCommonValue(i) );
        return GetCommonValue(i)->GetLabel();
    }

    /**
        Returns index of common value that will truly change value to
        unspecified.
    */
    int GetUnspecifiedCommonValue() const { return m_cvUnspecified; }

    /** Set index of common value that will truly change value to unspecified.
        Using -1 will set none to have such effect.
        Default is 0.
    */
    void SetUnspecifiedCommonValue( int index ) { m_cvUnspecified = index; }

    /**
        Shortcut for creating dialog-caller button. Used, for example, by
        wxFontProperty.
        @remarks
        This should only be called by properties.
    */
    wxWindow* GenerateEditorButton( const wxPoint& pos, const wxSize& sz );

    /** Fixes position of wxTextCtrl-like control (wxSpinCtrl usually
        fits as one). Call after control has been created (but before
        shown).
    */
    void FixPosForTextCtrl( wxWindow* ctrl,
                            const wxPoint& offset = wxPoint(0, 0) );

    /** Shortcut for creating text editor widget.
        @param pos
            Same as pos given for CreateEditor.
        @param sz
            Same as sz given for CreateEditor.
        @param value
            Initial text for wxTextCtrl.
        @param secondary
            If right-side control, such as button, also created, then create it
            first and pass it as this parameter.
        @param extraStyle
            Extra style flags to pass for wxTextCtrl.
        @remarks
        Note that this should generally be called only by new classes derived
        from wxPGProperty.
    */
    wxWindow* GenerateEditorTextCtrl( const wxPoint& pos,
                                      const wxSize& sz,
                                      const wxString& value,
                                      wxWindow* secondary,
                                      int extraStyle = 0,
                                      int maxLen = 0 );

    /* Generates both textctrl and button.
    */
    wxWindow* GenerateEditorTextCtrlAndButton( const wxPoint& pos,
        const wxSize& sz, wxWindow** psecondary, int limited_editing,
        wxPGProperty* property );

    /** Generates position for a widget editor dialog box.
        @param p
        Property for which dialog is positioned.
        @param sz
        Known or over-approximated size of the dialog.
        @return
        Position for dialog.
    */
    wxPoint GetGoodEditorDialogPosition( wxPGProperty* p,
                                         const wxSize& sz );

    // Converts escape sequences in src_str to newlines,
    // tabs, etc. and copies result to dst_str.
    static wxString& ExpandEscapeSequences( wxString& dst_str,
                                            wxString& src_str );

    // Converts newlines, tabs, etc. in src_str to escape
    // sequences, and copies result to dst_str.
    static wxString& CreateEscapeSequences( wxString& dst_str,
                                            wxString& src_str );

    /**
        Returns rectangle that fully contains properties between and including
        p1 and p2.
    */
    wxRect GetPropertyRect( const wxPGProperty* p1,
                            const wxPGProperty* p2 ) const;

    /** Returns pointer to current active primary editor control (NULL if none).
    */
    wxWindow* GetEditorControl() const;

    wxWindow* GetPrimaryEditor() const
    {
        return GetEditorControl();
    }

    /**
        Returns pointer to current active secondary editor control (NULL if
        none).
    */
    wxWindow* GetEditorControlSecondary() const
    {
        return m_wndEditor2;
    }

    /**
        Refreshes any active editor control.
    */
    void RefreshEditor();

    // Events from editor controls are forward to this function
    void HandleCustomEditorEvent( wxEvent &event );

#ifndef SWIG

    /**
        Generates contents for string dst based on the contents of
        wxArrayString src.

        Format will be "(preDelim)str1(postDelim) (preDelim)str2(postDelim) and
        so on. Set flags to 1 inorder to convert backslashes to double-back-
        slashes and "(preDelims)"'s to "(preDelims)".
    */
    static void ArrayStringToString( wxString& dst, const wxArrayString& src,
                                     wxChar preDelim, wxChar postDelim,
                                     int flags );

    // Mostly useful for page switching.
    void SwitchState( wxPropertyGridPageState* pNewState );

    long GetInternalFlags() const { return m_iFlags; }
    bool HasInternalFlag( long flag ) const
        { return (m_iFlags & flag) ? true : false; }
    void SetInternalFlag( long flag ) { m_iFlags |= flag; }
    void ClearInternalFlag( long flag ) { m_iFlags &= ~(flag); }
    void IncFrozen() { m_frozen++; }
    void DecFrozen() { m_frozen--; }

    void OnComboItemPaint( const wxPGComboBox* pCb,
                           int item,
                           wxDC* pDc,
                           wxRect& rect,
                           int flags );

    /** Standardized double-to-string conversion.
    */
    static void DoubleToString( wxString& target,
                                double value,
                                int precision,
                                bool removeZeroes,
                                wxString* precTemplate );

    /**
        Call this from wxPGProperty::OnEvent() to cause property value to be
        changed after the function returns (with true as return value).
        ValueChangeInEvent() must be used if you wish the application to be
        able to use wxEVT_PG_CHANGING to potentially veto the given value.
    */
    void ValueChangeInEvent( wxVariant variant )
    {
        m_changeInEventValue = variant;
        m_iFlags |= wxPG_FL_VALUE_CHANGE_IN_EVENT;
    }

    /** Returns true if given event is from first of an array of buttons
        (as can be in case when wxPGMultiButton is used).
    */
    bool IsMainButtonEvent( const wxEvent& event )
    {
        return (event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED)
                    && (m_wndSecId == event.GetId());
    }

    /** Pending value is expected to be passed in PerformValidation().
    */
    virtual bool DoPropertyChanged( wxPGProperty* p,
                                    unsigned int selFlags = 0 );

    /** Called when validation for given property fails.
        @param invalidValue
            Value which failed in validation.
        @return
            Return true if user is allowed to change to another property even
            if current has invalid value.
        @remarks
        To add your own validation failure behavior, override
        wxPropertyGrid::DoOnValidationFailure().
    */
    bool OnValidationFailure( wxPGProperty* property,
                              wxVariant& invalidValue );

    /** Called to indicate property and editor has valid value now.
    */
    void OnValidationFailureReset( wxPGProperty* property )
    {
        if ( property && property->HasFlag(wxPG_PROP_INVALID_VALUE) )
        {
            DoOnValidationFailureReset(property);
            property->ClearFlag(wxPG_PROP_INVALID_VALUE);
        }
        m_validationInfo.m_failureMessage.clear();
    }

    /** Override in derived class to display error messages in custom manner
        (these message usually only result from validation failure).
    */
    virtual void DoShowPropertyError( wxPGProperty* property,
                                      const wxString& msg );

    /** Override to customize property validation failure behavior.
        @param invalidValue
            Value which failed in validation.
        @return
            Return true if user is allowed to change to another property even
            if current has invalid value.
    */
    virtual bool DoOnValidationFailure( wxPGProperty* property,
                                        wxVariant& invalidValue );

    /** Override to customize resetting of property validation failure status.
        @remarks
        Property is guaranteed to have flag wxPG_PROP_INVALID_VALUE set.
    */
    virtual void DoOnValidationFailureReset( wxPGProperty* property );

    int GetSpacingY() const { return m_spacingy; }

    /**
        Must be called in wxPGEditor::CreateControls() if primary editor window
        is wxTextCtrl, just before textctrl is created.
        @param text
            Initial text value of created wxTextCtrl.
    */
    void SetupTextCtrlValue( const wxString text ) { m_prevTcValue = text; }

    /**
        Unfocuses or closes editor if one was open, but does not deselect
        property.
    */
    bool UnfocusEditor();

    virtual void SetWindowStyleFlag( long style );

protected:

    /**
        wxPropertyGridPageState used by the grid is created here.

        If grid is used in wxPropertyGridManager, there is no point overriding
        this - instead, set custom wxPropertyGridPage classes.
    */
    virtual wxPropertyGridPageState* CreateState() const;

    enum PerformValidationFlags
    {
        SendEvtChanging         = 0x0001,
        IsStandaloneValidation  = 0x0002   // Not called in response to event
    };

    /**
        Runs all validation functionality (includes sending wxEVT_PG_CHANGING).
        Returns true if all tests passed. Implement in derived class to
        add additional validation behavior.
    */
    virtual bool PerformValidation( wxPGProperty* p,
                                    wxVariant& pendingValue,
                                    int flags = SendEvtChanging );

public:

    // Control font changer helper.
    void SetCurControlBoldFont();

    wxPGCell& GetPropertyDefaultCell()
    {
        return m_propertyDefaultCell;
    }

    wxPGCell& GetCategoryDefaultCell()
    {
        return m_categoryDefaultCell;
    }

    //
    // Public methods for semi-public use
    // (not protected for optimization)
    //
    bool DoSelectProperty( wxPGProperty* p, unsigned int flags = 0 );

    // Overridden functions.
    virtual bool Destroy();
    // Returns property at given y coordinate (relative to grid's top left).
    wxPGProperty* GetItemAtY( int y ) const { return DoGetItemAtY(y); }

    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = (const wxRect *) NULL );
    virtual bool SetFont( const wxFont& font );
#if wxPG_SUPPORT_TOOLTIPS
    void SetToolTip( const wxString& tipString );
#endif
    virtual void Freeze();
    virtual void SetExtraStyle( long exStyle );
    virtual void Thaw();


protected:
    virtual wxSize DoGetBestSize() const;

#ifndef wxPG_ICON_WIDTH
    wxBitmap            *m_expandbmp, *m_collbmp;
#endif

    wxCursor            *m_cursorSizeWE;

    /** wxWindow pointers to editor control(s). */
    wxWindow            *m_wndEditor;
    wxWindow            *m_wndEditor2;

    /** wxPGCanvas instance. */
    wxPanel             *m_canvas;

#if wxPG_DOUBLE_BUFFER
    wxBitmap            *m_doubleBuffer;
#endif

    /** Local time ms when control was created. */
    wxLongLong          m_timeCreated;

    /** wxPGProperty::OnEvent can change value by setting this. */
    wxVariant           m_changeInEventValue;

    /** Id of m_wndEditor2, or its first child, if any. */
    int                 m_wndSecId;

    /** Extra Y spacing between the items. */
    int                 m_spacingy;

    /** Control client area width; updated on resize. */
    int                 m_width;

    /** Control client area height; updated on resize. */
    int                 m_height;

    /** Current non-client width (needed when auto-centering). */
    int                 m_ncWidth;

    /** Non-client width (auto-centering helper). */
    //int                 m_fWidth;

    /** Previously recorded scroll start position. */
    int                 m_prevVY;

    /**
        The gutter spacing in front and back of the image.
        This determines the amount of spacing in front of each item
     */
    int                 m_gutterWidth;

    /** Includes separator line. */
    int                 m_lineHeight;

    /** Gutter*2 + image width. */
    int                 m_marginWidth;

    // y spacing for expand/collapse button.
    int                 m_buttonSpacingY;

    /** Extra margin for expanded sub-group items. */
    int                 m_subgroup_extramargin;

    /**
        The image width of the [+] icon.

        This is also calculated in the gutter
     */
    int                 m_iconWidth;

#ifndef wxPG_ICON_WIDTH

    /**
        The image height of the [+] icon.

        This is calculated as minimal size and to align
     */
    int                 m_iconHeight;
#endif

    /** Current cursor id. */
    int                 m_curcursor;

    /**
        This captionFont is made equal to the font of the wxScrolledWindow.

        As extra the bold face is set on it when this is wanted by the user
        (see flags)
     */
    wxFont              m_captionFont;

    int                 m_fontHeight;  // Height of the font.

    /** m_splitterx when drag began. */
    int                 m_startingSplitterX;

    /**
        Index to splitter currently being dragged (0=one after the first
        column).
     */
    int                 m_draggedSplitter;

    /** Changed property, calculated in PerformValidation(). */
    wxPGProperty*       m_chgInfo_changedProperty;

    /**
        Lowest property for which editing happened, but which does not have
        aggregate parent
     */
    wxPGProperty*       m_chgInfo_baseChangedProperty;

    /** Changed property value, calculated in PerformValidation(). */
    wxVariant           m_chgInfo_pendingValue;

    /** Passed to SetValue. */
    wxVariant           m_chgInfo_valueList;

    /** Validation information. */
    wxPGValidationInfo  m_validationInfo;

    /** Actions and keys that trigger them. */
    wxPGHashMapI2I      m_actionTriggers;

    //
    // Temporary values
    //

    /** Bits are used to indicate which colours are customized. */
    unsigned short      m_coloursCustomized;

    /** x - m_splitterx. */
    signed char                 m_dragOffset;

    /** 0 = not dragging, 1 = drag just started, 2 = drag in progress */
    unsigned char       m_dragStatus;

    /** 0 = margin, 1 = label, 2 = value. */
    unsigned char       m_mouseSide;

    /** True when editor control is focused. */
    unsigned char       m_editorFocused;

    /** 1 if m_latsCaption is also the bottommost caption. */
    //unsigned char       m_lastCaptionBottomnest;

    /** Set to 1 when graphics frozen. */
    unsigned char       m_frozen;

    unsigned char       m_vspacing;

    // Used to track when Alt/Ctrl+Key was consumed.
    unsigned char       m_keyComboConsumed;

    /** 1 if in DoPropertyChanged() */
    unsigned char       m_inDoPropertyChanged;

    /** 1 if in CommitChangesFromEditor() */
    unsigned char       m_inCommitChangesFromEditor;

    /** 1 if in DoSelectProperty() */
    unsigned char       m_inDoSelectProperty;

    wxPGVFBFlags        m_permanentValidationFailureBehavior;  // Set by app

    /** Internal flags - see wxPG_FL_XXX constants. */
    wxUint32            m_iFlags;

    /** When drawing next time, clear this many item slots at the end. */
    int                 m_clearThisMany;

    /** Pointer to selected property. Note that this is duplicated in
        m_state for better transiency between pages so that the selected
        item can be retained.
    */
    wxPGProperty*       m_selected;

    // pointer to property that has mouse hovering
    wxPGProperty*       m_propHover;

    // EventObject for wxPropertyGridEvents
    wxWindow*           m_eventObject;

    // What (global) window is currently focused (needed to resolve event
    // handling mess).
    wxWindow*           m_curFocused;

    // wxPGTLWHandler
    wxEvtHandler*       m_tlwHandler;

    // Top level parent
    wxWindow*           m_tlp;

    // Sort function
    wxPGSortCallback    m_sortFunction;

    // y coordinate of property that mouse hovering
    int                 m_propHoverY;
    // Which column's editor is selected (usually 1)?
    int                 m_selColumn;

    // x relative to splitter (needed for resize).
    int                 m_ctrlXAdjust;

    // lines between cells
    wxColour            m_colLine;
    // property labels and values are written in this colour
    wxColour            m_colPropFore;
    // or with this colour when disabled
    wxColour            m_colDisPropFore;
    // background for m_colPropFore
    wxColour            m_colPropBack;
    // text color for captions
    wxColour            m_colCapFore;
    // background color for captions
    wxColour            m_colCapBack;
    // foreground for selected property
    wxColour            m_colSelFore;
    // background for selected property (actually use background color when
    // control out-of-focus)
    wxColour            m_colSelBack;
    // background colour for margin
    wxColour            m_colMargin;
    // background colour for empty space below the grid
    wxColour            m_colEmptySpace;

    // Default property colours
    wxPGCell            m_propertyDefaultCell;

    // Default property category
    wxPGCell            m_categoryDefaultCell;

    // Backup of selected property's cells
    wxVector<wxPGCell>  m_propCellsBackup;

    // NB: These *cannot* be moved to globals.

    // labels when properties use common values
    wxVector<wxPGCommonValue*>  m_commonValues;

    // Which cv selection really sets value to unspecified?
    int                 m_cvUnspecified;

    // Used to skip excess text editor events
    wxString            m_prevTcValue;

protected:

    // Sets some members to defaults (called constructors).
    void Init1();

    // Initializes some members (called by Create and complex constructor).
    void Init2();

    void OnPaint(wxPaintEvent &event );

    // main event receivers
    void OnMouseMove( wxMouseEvent &event );
    void OnMouseMoveBottom( wxMouseEvent &event );
    void OnMouseClick( wxMouseEvent &event );
    void OnMouseRightClick( wxMouseEvent &event );
    void OnMouseDoubleClick( wxMouseEvent &event );
    void OnMouseUp( wxMouseEvent &event );
    void OnKey( wxKeyEvent &event );
    void OnResize( wxSizeEvent &event );

    // event handlers
    bool HandleMouseMove( int x, unsigned int y, wxMouseEvent &event );
    bool HandleMouseClick( int x, unsigned int y, wxMouseEvent &event );
    bool HandleMouseRightClick( int x, unsigned int y, wxMouseEvent &event );
    bool HandleMouseDoubleClick( int x, unsigned int y, wxMouseEvent &event );
    bool HandleMouseUp( int x, unsigned int y, wxMouseEvent &event );
    void HandleKeyEvent( wxKeyEvent &event, bool fromChild );

    void OnMouseEntry( wxMouseEvent &event );

    void OnIdle( wxIdleEvent &event );
    void OnFocusEvent( wxFocusEvent &event );
    void OnChildFocusEvent( wxChildFocusEvent& event );

    bool OnMouseCommon( wxMouseEvent &event, int* px, int *py );
    bool OnMouseChildCommon( wxMouseEvent &event, int* px, int *py );

    // sub-control event handlers
    void OnMouseClickChild( wxMouseEvent &event );
    void OnMouseRightClickChild( wxMouseEvent &event );
    void OnMouseMoveChild( wxMouseEvent &event );
    void OnMouseUpChild( wxMouseEvent &event );
    void OnChildKeyDown( wxKeyEvent &event );

    void OnCaptureChange( wxMouseCaptureChangedEvent &event );

    void OnScrollEvent( wxScrollWinEvent &event );

    void OnSysColourChanged( wxSysColourChangedEvent &event );

protected:

    /**
        Adjust the centering of the bitmap icons (collapse / expand) when the
        caption font changes.

        They need to be centered in the middle of the font, so a bit of deltaY
        adjustment is needed. On entry, m_captionFont must be set to window
        font. It will be modified properly.
      */
    void CalculateFontAndBitmapStuff( int vspacing );

    wxRect GetEditorWidgetRect( wxPGProperty* p, int column ) const;

    void CorrectEditorWidgetSizeX();

    /** Called in RecalculateVirtualSize() to reposition control
        on virtual height changes.
    */
    void CorrectEditorWidgetPosY();

#ifdef __WXDEBUG__
    void _log_items();
    void OnScreenNote( const wxChar* format, ... );
#endif

    /** Deselect current selection, if any. Returns true if success
        (ie. validator did not intercept). */
    bool DoClearSelection();

    int DoDrawItems( wxDC& dc,
                     const wxRect* clipRect,
                     bool isBuffered ) const;

    /** Draws an expand/collapse (ie. +/-) button.
    */
    virtual void DrawExpanderButton( wxDC& dc, const wxRect& rect,
                                     wxPGProperty* property ) const;

    /** Draws items from topitemy to bottomitemy */
    void DrawItems( wxDC& dc, unsigned int topitemy, unsigned int bottomitemy,
                    const wxRect* clip_rect = (const wxRect*) NULL );

    void DrawItems( const wxPGProperty* p1, const wxPGProperty* p2 );

    void DrawItem( wxPGProperty* p )
    {
        DrawItems(p,p);
    }

    virtual void DrawItemAndChildren( wxPGProperty* p );

    /**
        Draws item, children, and consequtive parents as long as category is
        not met.
     */
    void DrawItemAndValueRelated( wxPGProperty* p );

    // Translate wxKeyEvent to wxPG_ACTION_XXX
    int KeyEventToActions(wxKeyEvent &event, int* pSecond) const;

    int KeyEventToAction(wxKeyEvent &event) const
    {
        return KeyEventToActions(event, NULL);
    }

    void ImprovedClientToScreen( int* px, int* py );

    // Returns True if editor control has focus
    bool IsEditorFocused() const;

    // Called by focus event handlers. newFocused is the window that becomes
    // focused.
    void HandleFocusChange( wxWindow* newFocused );

    /** Reloads all non-customized colours from system settings. */
    void RegainColours();

    bool DoEditorValidate();

    wxPGProperty* DoGetItemAtY( int y ) const;

    void DoSetSplitterPosition_( int newxpos,
                                 bool refresh = true,
                                 int splitterIndex = 0,
                                 bool allPages = false );

    void FreeEditors();

    virtual bool DoExpand( wxPGProperty* p, bool sendEvent = false );

    virtual bool DoCollapse( wxPGProperty* p, bool sendEvent = false );

    // Returns nearest paint visible property (such that will be painted unless
    // window is scrolled or resized). If given property is paint visible, then
    // it itself will be returned.
    wxPGProperty* GetNearestPaintVisible( wxPGProperty* p ) const;

    static void RegisterDefaultEditors();

    // Sets up basic event handling for child control
    void SetupChildEventHandling( wxWindow* wnd );

    void CustomSetCursor( int type, bool override = false );

    /**
        Repositions scrollbar and underlying panel according to changed virtual
        size.
    */
    void RecalculateVirtualSize( int forceXPos = -1 );

    void PrepareAfterItemsAdded();

    bool SendEvent( int eventType,
                    wxPGProperty* p,
                    wxVariant* pValue = NULL,
                    unsigned int selFlags = 0 );

    void SetFocusOnCanvas();

    bool DoHideProperty( wxPGProperty* p, bool hide, int flags );

private:

    bool ButtonTriggerKeyTest( int action, wxKeyEvent& event );

    DECLARE_EVENT_TABLE()
#endif // #ifndef SWIG
};

// -----------------------------------------------------------------------
//
// Bunch of inlines that need to resolved after all classes have been defined.
//

#ifndef SWIG
inline bool wxPropertyGridPageState::IsDisplayed() const
{
    return ( this == m_pPropGrid->GetState() );
}

inline unsigned int wxPropertyGridPageState::GetActualVirtualHeight() const
{
    return DoGetRoot()->GetChildrenHeight(GetGrid()->GetRowHeight());
}
#endif

inline int wxPGProperty::GetDisplayedCommonValueCount() const
{
    if ( HasFlag(wxPG_PROP_USES_COMMON_VALUE) )
    {
        wxPropertyGrid* pg = GetGrid();
        if ( pg )
            return (int) pg->GetCommonValueCount();
    }
    return 0;
}

inline void wxPGProperty::SetEditor( const wxString& editorName )
{
    m_customEditor = wxPropertyGridInterface::GetEditorByName(editorName);
}

inline void wxPGProperty::SetName( const wxString& newName )
{
    GetGrid()->SetPropertyName(this, newName);
}

inline bool wxPGProperty::Hide( bool hide, int flags )
{
    return GetGrid()->HideProperty(this, hide, flags);
}

inline bool wxPGProperty::SetMaxLength( int maxLen )
{
    return GetGrid()->SetPropertyMaxLength(this,maxLen);
}

// -----------------------------------------------------------------------

#define wxPG_BASE_EVT_PRE_ID     1775

#ifndef SWIG

wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_SELECTED, wxPropertyGridEvent )
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_CHANGING, wxPropertyGridEvent )
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_CHANGED, wxPropertyGridEvent )
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_HIGHLIGHTED, wxPropertyGridEvent )
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_RIGHT_CLICK, wxPropertyGridEvent )
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_PAGE_CHANGED, wxPropertyGridEvent )
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_ITEM_COLLAPSED, wxPropertyGridEvent )
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_ITEM_EXPANDED, wxPropertyGridEvent )
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_DOUBLE_CLICK, wxPropertyGridEvent )

#else
    enum {
        wxEVT_PG_SELECTED = wxPG_BASE_EVT_PRE_ID,
        wxEVT_PG_CHANGING,
        wxEVT_PG_CHANGED,
        wxEVT_PG_HIGHLIGHTED,
        wxEVT_PG_RIGHT_CLICK,
        wxEVT_PG_PAGE_CHANGED,
        wxEVT_PG_ITEM_COLLAPSED,
        wxEVT_PG_ITEM_EXPANDED,
        wxEVT_PG_DOUBLE_CLICK
    };
#endif


#define wxPG_BASE_EVT_TYPE       wxEVT_PG_SELECTED
#define wxPG_MAX_EVT_TYPE        (wxPG_BASE_EVT_TYPE+30)


#ifndef SWIG
typedef void (wxEvtHandler::*wxPropertyGridEventFunction)(wxPropertyGridEvent&);

#define EVT_PG_SELECTED(id, fn)              DECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_SELECTED, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), NULL ),
#define EVT_PG_CHANGING(id, fn)              DECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_CHANGING, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), NULL ),
#define EVT_PG_CHANGED(id, fn)               DECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_CHANGED, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), NULL ),
#define EVT_PG_HIGHLIGHTED(id, fn)           DECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_HIGHLIGHTED, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), NULL ),
#define EVT_PG_RIGHT_CLICK(id, fn)           DECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_RIGHT_CLICK, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), NULL ),
#define EVT_PG_DOUBLE_CLICK(id, fn)          DECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_DOUBLE_CLICK, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), NULL ),
#define EVT_PG_PAGE_CHANGED(id, fn)          DECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_PAGE_CHANGED, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), NULL ),
#define EVT_PG_ITEM_COLLAPSED(id, fn)        DECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_ITEM_COLLAPSED, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), NULL ),
#define EVT_PG_ITEM_EXPANDED(id, fn)         DECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_ITEM_EXPANDED, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), NULL ),

#define wxPropertyGridEventHandler(fn) \
    wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn )

#endif


/** @class wxPropertyGridEvent

    A propertygrid event holds information about events associated with
    wxPropertyGrid objects.

    @library{wxpropgrid}
    @category{propgrid}
*/
class WXDLLIMPEXP_PROPGRID wxPropertyGridEvent : public wxCommandEvent
{
public:

    /** Constructor. */
    wxPropertyGridEvent(wxEventType commandType=0, int id=0);
#ifndef SWIG
    /** Copy constructor. */
    wxPropertyGridEvent(const wxPropertyGridEvent& event);
#endif
    /** Destructor. */
    ~wxPropertyGridEvent();

    /** Copyer. */
    virtual wxEvent* Clone() const;

    wxPGProperty* GetMainParent() const
    {
        wxASSERT(m_property);
        return m_property->GetMainParent();
    }

    /** Returns id of associated property. */
    wxPGProperty* GetProperty() const
    {
        return m_property;
    }

    wxPGValidationInfo& GetValidationInfo()
    {
        wxASSERT(m_validationInfo);
        return *m_validationInfo;
    }

    /** Returns true if you can veto the action that the event is signaling.
    */
    bool CanVeto() const { return m_canVeto; }

    /**
        Call this from your event handler to veto action that the event is
        signaling.

        You can only veto a shutdown if wxPropertyGridEvent::CanVeto returns
        true.
        @remarks
        Currently only wxEVT_PG_CHANGING supports vetoing.
    */
    void Veto( bool veto = true ) { m_wasVetoed = veto; }

    /** Returns value that is about to be set for wxEVT_PG_CHANGING.
    */
    const wxVariant& GetValue() const
    {
        wxASSERT_MSG( m_validationInfo,
                      "Only call GetValue from a handler "
                      "of event type that supports it" );
        return m_validationInfo->GetValue();
    }

    /**
        Set override validation failure behavior.

        Only effective if Veto was also called, and only allowed if event type
        is wxEVT_PG_CHANGING.
    */
    void SetValidationFailureBehavior( wxPGVFBFlags flags )
    {
        wxASSERT( GetEventType() == wxEVT_PG_CHANGING );
        m_validationInfo->SetFailureBehavior( flags );
    }

    /** Sets custom failure message for this time only. Only applies if
        wxPG_VFB_SHOW_MESSAGE is set in validation failure flags.
    */
    void SetValidationFailureMessage( const wxString& message )
    {
        wxASSERT( GetEventType() == wxEVT_PG_CHANGING );
        m_validationInfo->SetFailureMessage( message );
    }

#ifndef SWIG
    wxPGVFBFlags GetValidationFailureBehavior() const
    {
        wxASSERT( GetEventType() == wxEVT_PG_CHANGING );
        return m_validationInfo->GetFailureBehavior();
    }

    void SetCanVeto( bool canVeto ) { m_canVeto = canVeto; }
    bool WasVetoed() const { return m_wasVetoed; }

    /** Changes the associated property. */
    void SetProperty( wxPGProperty* p ) { m_property = p; }

    void SetPropertyGrid( wxPropertyGrid* pg ) { m_pg = pg; }

    void SetupValidationInfo()
    {
        wxASSERT(m_pg);
        wxASSERT( GetEventType() == wxEVT_PG_CHANGING );
        m_validationInfo = &m_pg->GetValidationInfo();
    }

private:
    void Init();
    DECLARE_DYNAMIC_CLASS(wxPropertyGridEvent)

    wxPGProperty*       m_property;
    wxPropertyGrid*     m_pg;
    wxPGValidationInfo* m_validationInfo;

    bool                m_canVeto;
    bool                m_wasVetoed;

#endif
};


// -----------------------------------------------------------------------

/** @class wxPropertyGridPopulator
    @ingroup classes
    Allows populating wxPropertyGrid from arbitrary text source.
*/
class WXDLLIMPEXP_PROPGRID wxPropertyGridPopulator
{
public:
    /** Default constructor.
    */
    wxPropertyGridPopulator();

    /** Destructor. */
    virtual ~wxPropertyGridPopulator();

    void SetState( wxPropertyGridPageState* state );

    void SetGrid( wxPropertyGrid* pg );

    /** Appends a new property under bottommost parent.
        @param propClass
        Property class as string.
    */
    wxPGProperty* Add( const wxString& propClass,
                       const wxString& propLabel,
                       const wxString& propName,
                       const wxString* propValue,
                       wxPGChoices* pChoices = NULL );

    /**
        Pushes property to the back of parent array (ie it becomes bottommost
        parent), and starts scanning/adding children for it.

        When finished, parent array is returned to the original state.
    */
    void AddChildren( wxPGProperty* property );

    /** Adds attribute to the bottommost property.
        @param type
        Allowed values: "string", (same as string), "int", "bool". Empty string
          mean autodetect.
    */
    bool AddAttribute( const wxString& name,
                       const wxString& type,
                       const wxString& value );

    /** Called once in AddChildren.
    */
    virtual void DoScanForChildren() = 0;

    /**
        Returns id of parent property for which children can currently be
        added.
     */
    wxPGProperty* GetCurParent() const
    {
        return (wxPGProperty*) m_propHierarchy[m_propHierarchy.size()-1];
    }

    wxPropertyGridPageState* GetState() { return m_state; }
    const wxPropertyGridPageState* GetState() const { return m_state; }

    /** Like wxString::ToLong, except allows N% in addition of N.
    */
    static bool ToLongPCT( const wxString& s, long* pval, long max );

    /** Parses strings of format "choice1"[=value1] ... "choiceN"[=valueN] into
        wxPGChoices. Registers parsed result using idString (if not empty).
        Also, if choices with given id already registered, then don't parse but
        return those choices instead.
    */
    wxPGChoices ParseChoices( const wxString& choicesString,
                              const wxString& idString );

    /** Implement in derived class to do custom process when an error occurs.
        Default implementation uses wxLogError.
    */
    virtual void ProcessError( const wxString& msg );

protected:

    /** Used property grid. */
    wxPropertyGrid*         m_pg;

    /** Used property grid state. */
    wxPropertyGridPageState*    m_state;

    /** Tree-hierarchy of added properties (that can have children). */
    wxArrayPGProperty       m_propHierarchy;

    /** Hashmap for string-id to wxPGChoicesData mapping. */
    wxPGHashMapS2P          m_dictIdChoices;
};

// -----------------------------------------------------------------------

//
// Undefine macros that are not needed outside propertygrid sources
//
#ifndef __wxPG_SOURCE_FILE__
    #undef wxPG_FL_DESC_REFRESH_REQUIRED
    #undef wxPG_FL_SCROLLBAR_DETECTED
    #undef wxPG_FL_CREATEDSTATE
    #undef wxPG_FL_NOSTATUSBARHELP
    #undef wxPG_FL_SCROLLED
    #undef wxPG_FL_FOCUS_INSIDE_CHILD
    #undef wxPG_FL_FOCUS_INSIDE
    #undef wxPG_FL_MOUSE_INSIDE_CHILD
    #undef wxPG_FL_CUR_USES_CUSTOM_IMAGE
    #undef wxPG_FL_PRIMARY_FILLS_ENTIRE
    #undef wxPG_FL_VALUE_MODIFIED
    #undef wxPG_FL_MOUSE_INSIDE
    #undef wxPG_FL_FOCUSED
    #undef wxPG_FL_MOUSE_CAPTURED
    #undef wxPG_FL_INITIALIZED
    #undef wxPG_FL_ACTIVATION_BY_CLICK
    #undef wxPG_FL_DONT_CENTER_SPLITTER
    #undef wxPG_SUPPORT_TOOLTIPS
    #undef wxPG_DOUBLE_BUFFER
    #undef wxPG_ICON_WIDTH
    #undef wxPG_USE_RENDERER_NATIVE
// Following are needed by the manager headers
//    #undef const wxString&
#endif

// -----------------------------------------------------------------------

#endif

#endif // _WX_PROPGRID_PROPGRID_H_

