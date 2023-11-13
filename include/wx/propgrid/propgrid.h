/////////////////////////////////////////////////////////////////////////////
// Name:        wx/propgrid/propgrid.h
// Purpose:     wxPropertyGrid
// Author:      Jaakko Salli
// Created:     2004-09-25
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROPGRID_PROPGRID_H_
#define _WX_PROPGRID_PROPGRID_H_

#include "wx/defs.h"

#if wxUSE_PROPGRID

#include "wx/scrolwin.h"
#include "wx/recguard.h"
#include "wx/time.h" // needed for wxMilliClock_t

#include "wx/propgrid/property.h"
#include "wx/propgrid/propgridiface.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifndef SWIG
extern WXDLLIMPEXP_DATA_PROPGRID(const char) wxPropertyGridNameStr[];
#endif

class wxPGComboBox;

#if wxUSE_STATUSBAR
class WXDLLIMPEXP_FWD_CORE wxStatusBar;
#endif
class WXDLLIMPEXP_FWD_CORE wxTextCtrl;

// -----------------------------------------------------------------------
// Global variables
// -----------------------------------------------------------------------

// This is required for sharing common global variables.
class WXDLLIMPEXP_PROPGRID wxPGGlobalVarsClass
{
public:

    wxPGGlobalVarsClass();
    ~wxPGGlobalVarsClass();

#if wxUSE_THREADS
    // Critical section for handling the globals. Generally it is not needed
    // since GUI code is supposed to be in single thread. However,
    // we do want the user to be able to convey wxPropertyGridEvents to other
    // threads.
    wxCriticalSection   m_critSect;
#endif

    // Used by advprops, but here to make things easier.
    wxString            m_pDefaultImageWildcard;

    // Map of editor class instances (keys are name string).
    std::unordered_map<wxString, wxPGEditor*> m_mapEditorClasses;

#if wxUSE_VALIDATORS
    std::vector<wxValidator*> m_arrValidators; // These wxValidators need to be freed
#endif

    wxPGChoices*        m_fontFamilyChoices;

    // Replace with your own to affect all properties using default renderer.
    wxPGCellRenderer*   m_defaultRenderer;

    wxPGChoices         m_boolChoices;

#if WXWIN_COMPATIBILITY_3_2
    // Some shared variants
    wxDEPRECATED_BUT_USED_INTERNALLY(const wxVariant     m_vEmptyString;)
    wxDEPRECATED_BUT_USED_INTERNALLY(const wxVariant     m_vZero;)
    wxDEPRECATED_BUT_USED_INTERNALLY(const wxVariant     m_vMinusOne;)
    wxDEPRECATED_BUT_USED_INTERNALLY(const wxVariant     m_vTrue;)
    wxDEPRECATED_BUT_USED_INTERNALLY(const wxVariant     m_vFalse;)

    // Cached constant strings
    wxDEPRECATED_BUT_USED_INTERNALLY(const wxString      m_strstring;)
    wxDEPRECATED_BUT_USED_INTERNALLY(const wxString      m_strlong;)
    wxDEPRECATED_BUT_USED_INTERNALLY(const wxString      m_strbool;)
    wxDEPRECATED_BUT_USED_INTERNALLY(const wxString      m_strlist;)

    wxDEPRECATED_BUT_USED_INTERNALLY(const wxString      m_strDefaultValue;)
    wxDEPRECATED_BUT_USED_INTERNALLY(const wxString      m_strMin;)
    wxDEPRECATED_BUT_USED_INTERNALLY(const wxString      m_strMax;)
    wxDEPRECATED_BUT_USED_INTERNALLY(const wxString      m_strUnits;)
    wxDEPRECATED_BUT_USED_INTERNALLY(const wxString      m_strHint;)
#endif // WXWIN_COMPATIBILITY_3_2

    // If true then some things are automatically translated
    bool                m_autoGetTranslation;

    // > 0 if errors cannot or should not be shown in statusbar etc.
    int                 m_offline;

    int                 m_extraStyle;  // global extra style

    int                 m_warnings;

    int HasExtraStyle( int style ) const { return (m_extraStyle & style); }
};

// Internal class providing access to the global wxPGGlobalVars instance.
class WXDLLIMPEXP_PROPGRID wxPGGlobalVarsPtr
{
public:
    wxPGGlobalVarsClass* operator->() const;
    bool operator!() const;
    explicit operator bool() const;
};

extern WXDLLIMPEXP_DATA_PROPGRID(wxPGGlobalVarsPtr) wxPGGlobalVars;

#if WXWIN_COMPATIBILITY_3_2
#ifdef wxPG_MUST_DEPRECATE_MACRO_NAME
#pragma deprecated(wxPGVariant_EmptyString)
#pragma deprecated(wxPGVariant_Zero)
#pragma deprecated(wxPGVariant_MinusOne)
#pragma deprecated(wxPGVariant_True)
#pragma deprecated(wxPGVariant_False)
#pragma deprecated(wxPGVariant_Bool)
#endif
#define wxPGVariant_EmptyString wxPG_DEPRECATED_MACRO_VALUE(wxVariant(wxString()),\
    "wxPGVariant_EmptyString is deprecated. Use wxVariant(wxString()) instead.")
#define wxPGVariant_Zero wxPG_DEPRECATED_MACRO_VALUE(wxVariant(0L),\
    "wxPGVariant_Zero is deprecated. Use wxVariant(0L) instead.")
#define wxPGVariant_MinusOne wxPG_DEPRECATED_MACRO_VALUE(wxVariant(-1L),\
    "wxPGVariant_MinusOne is deprecated. Use wxVariant(-1L) instead.")
#define wxPGVariant_True wxPG_DEPRECATED_MACRO_VALUE(wxVariant(true),\
    "wxPGVariant_True is deprecated. Use wxVariant(true) instead.")
#define wxPGVariant_False wxPG_DEPRECATED_MACRO_VALUE(wxVariant(false),\
    "wxPGVariant_False is deprecated. Use wxVariant(false) instead.")
#define wxPGVariant_Bool(A) wxPG_DEPRECATED_MACRO_VALUE(wxVariant(A),\
    "wxPGVariant_Bool is deprecated. Use wxVariant(A) instead.")
#endif // WXWIN_COMPATIBILITY_3_2

// When wxPG is loaded dynamically after the application is already running
// then the built-in module system won't pick this one up.  Add it manually.
WXDLLIMPEXP_PROPGRID void wxPGInitResourceModule();

// -----------------------------------------------------------------------

// SetWindowStyleFlag method can be used to modify some of these at run-time.
enum wxPG_WINDOW_STYLES
{

// This will cause Sort() automatically after an item is added.
// When inserting a lot of items in this mode, it may make sense to
// use Freeze() before operations and Thaw() afterwards to increase
// performance.
wxPG_AUTO_SORT                      = 0x00000010,

// Categories are not initially shown (even if added).
// IMPORTANT NOTE: If you do not plan to use categories, then this
// style will waste resources.
// This flag can also be changed using wxPropertyGrid::EnableCategories method.
wxPG_HIDE_CATEGORIES                = 0x00000020,

// This style combines non-categoric mode and automatic sorting.
wxPG_ALPHABETIC_MODE                = (wxPG_HIDE_CATEGORIES|wxPG_AUTO_SORT),

// Modified values are shown in bold font. Changing this requires Refresh()
// to show changes.
wxPG_BOLD_MODIFIED                  = 0x00000040,

// When wxPropertyGrid is resized, splitter moves to the center. This
// behaviour stops once the user manually moves the splitter.
wxPG_SPLITTER_AUTO_CENTER           = 0x00000080,

// Display tooltips for cell text that cannot be shown completely. If
// wxUSE_TOOLTIPS is 0, then this doesn't have any effect.
wxPG_TOOLTIPS                       = 0x00000100,

// Disables margin and hides all expand/collapse buttons that would appear
// outside the margin (for sub-properties). Toggling this style automatically
// expands all collapsed items.
wxPG_HIDE_MARGIN                    = 0x00000200,

// This style prevents user from moving the splitter.
wxPG_STATIC_SPLITTER                = 0x00000400,

// Combination of other styles that make it impossible for user to modify
// the layout.
wxPG_STATIC_LAYOUT                  = (wxPG_HIDE_MARGIN|wxPG_STATIC_SPLITTER),

// Disables wxTextCtrl based editors for properties which
// can be edited in another way.
// Equals calling wxPropertyGrid::LimitPropertyEditing for all added
// properties.
wxPG_LIMITED_EDITING                = 0x00000800,

// wxPropertyGridManager only: Show toolbar for mode and page selection.
wxPG_TOOLBAR                        = 0x00001000,

// wxPropertyGridManager only: Show adjustable text box showing description
// or help text, if available, for currently selected property.
wxPG_DESCRIPTION                    = 0x00002000,

// wxPropertyGridManager only: don't show an internal border around the
// property grid. Recommended if you use a header.
wxPG_NO_INTERNAL_BORDER             = 0x00004000,

// A mask which can be used to filter (out) all styles.
wxPG_WINDOW_STYLE_MASK = wxPG_AUTO_SORT|wxPG_HIDE_CATEGORIES|wxPG_BOLD_MODIFIED|
                         wxPG_SPLITTER_AUTO_CENTER|wxPG_TOOLTIPS|wxPG_HIDE_MARGIN|
                         wxPG_STATIC_SPLITTER|wxPG_LIMITED_EDITING|wxPG_TOOLBAR|
                         wxPG_DESCRIPTION|wxPG_NO_INTERNAL_BORDER
};


// NOTE: wxPG_EX_xxx are extra window styles and must be set using
// SetExtraStyle() member function.
enum wxPG_EX_WINDOW_STYLES
{
// Speeds up switching to wxPG_HIDE_CATEGORIES mode. Initially, if
// wxPG_HIDE_CATEGORIES is not defined, the non-categorized data storage is
// not activated, and switching the mode first time becomes somewhat slower.
// wxPG_EX_INIT_NOCAT activates the non-categorized data storage right away.
// NOTE: If you do plan not switching to non-categoric mode, or if
// you don't plan to use categories at all, then using this style will result
// in waste of resources.
wxPG_EX_INIT_NOCAT                  = 0x00001000,

// Extended window style that sets wxPropertyGridManager toolbar to not
// use flat style.
wxPG_EX_NO_FLAT_TOOLBAR             = 0x00002000,

// Shows alphabetic/categoric mode buttons from toolbar.
wxPG_EX_MODE_BUTTONS                = 0x00008000,

// Show property help strings as tool tips instead as text on the status bar.
// You can set the help strings using SetPropertyHelpString member function.
wxPG_EX_HELP_AS_TOOLTIPS            = 0x00010000,

// Prevent TAB from focusing to wxButtons. This behaviour was default
// in version 1.2.0 and earlier.
// NOTE! Tabbing to button doesn't work yet. Problem seems to be that on wxMSW
// at least the button doesn't properly propagate key events (yes, I'm using
// wxWANTS_CHARS).
//wxPG_EX_NO_TAB_TO_BUTTON            = 0x00020000,

// Allows relying on native double-buffering.
wxPG_EX_NATIVE_DOUBLE_BUFFERING         = 0x00080000,

// Set this style to let user have ability to set values of properties to
// unspecified state. Same as setting wxPG_PROP_AUTO_UNSPECIFIED for
// all properties.
wxPG_EX_AUTO_UNSPECIFIED_VALUES         = 0x00200000,

// If this style is used, built-in attributes (such as wxPG_FLOAT_PRECISION
// and wxPG_STRING_PASSWORD) are not stored into property's attribute storage
// (thus they are not readable).
// Note that this option is global, and applies to all wxPG property
// containers.
wxPG_EX_WRITEONLY_BUILTIN_ATTRIBUTES    = 0x00400000,

// Hides page selection buttons from toolbar.
wxPG_EX_HIDE_PAGE_BUTTONS               = 0x01000000,

// Allows multiple properties to be selected by user (by pressing SHIFT
// when clicking on a property, or by dragging with left mouse button
// down).
// You can get array of selected properties with
// wxPropertyGridInterface::GetSelectedProperties(). In multiple selection
// mode wxPropertyGridInterface::GetSelection() returns
// property which has editor active (usually the first one
// selected). Other useful member functions are ClearSelection(),
// AddToSelection() and RemoveFromSelection().
wxPG_EX_MULTIPLE_SELECTION              = 0x02000000,

// This enables top-level window tracking which allows wxPropertyGrid to
// notify the application of last-minute property value changes by user.
// This style is not enabled by default because it may cause crashes when
// wxPropertyGrid is used in with wxAUI or similar system.
// Note: If you are not in fact using any system that may change
// wxPropertyGrid's top-level parent window on its own, then you
// are recommended to enable this style.
wxPG_EX_ENABLE_TLP_TRACKING             = 0x04000000,

// Don't show divider above toolbar, on Windows.
wxPG_EX_NO_TOOLBAR_DIVIDER              = 0x08000000,

// Show a separator below the toolbar.
wxPG_EX_TOOLBAR_SEPARATOR               = 0x10000000,

// Allows to take focus on the entire area (on canvas)
// even if wxPropertyGrid is not a standalone control.
wxPG_EX_ALWAYS_ALLOW_FOCUS              = 0x00100000,


// A mask which can be used to filter (out) all extra styles applicable to wxPropertyGrid.
wxPG_EX_WINDOW_PG_STYLE_MASK = wxPG_EX_INIT_NOCAT|wxPG_EX_HELP_AS_TOOLTIPS|wxPG_EX_NATIVE_DOUBLE_BUFFERING|
                               wxPG_EX_AUTO_UNSPECIFIED_VALUES|wxPG_EX_WRITEONLY_BUILTIN_ATTRIBUTES|
                               wxPG_EX_MULTIPLE_SELECTION|wxPG_EX_ENABLE_TLP_TRACKING|wxPG_EX_ALWAYS_ALLOW_FOCUS,

// A mask which can be used to filter (out) all extra styles applicable to wxPropertyGridManager.
wxPG_EX_WINDOW_PGMAN_STYLE_MASK = wxPG_EX_NO_FLAT_TOOLBAR|wxPG_EX_MODE_BUTTONS|wxPG_EX_HIDE_PAGE_BUTTONS|
                                  wxPG_EX_NO_TOOLBAR_DIVIDER|wxPG_EX_TOOLBAR_SEPARATOR,

// A mask which can be used to filter (out) all extra styles.
wxPG_EX_WINDOW_STYLE_MASK = wxPG_EX_WINDOW_PG_STYLE_MASK|wxPG_EX_WINDOW_PGMAN_STYLE_MASK
};

// Combines various styles.
constexpr long wxPG_DEFAULT_STYLE = 0L;

// Combines various styles.
constexpr long wxPGMAN_DEFAULT_STYLE = 0L;

// -----------------------------------------------------------------------

// wxPropertyGrid stores information about common values in these
// records.
// NB: Common value feature is not complete, and thus not mentioned in
// documentation.
class WXDLLIMPEXP_PROPGRID wxPGCommonValue
{
public:

    wxPGCommonValue( const wxString& label, wxPGCellRenderer* renderer )
        : m_label(label)
        , m_renderer(renderer)
    {
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

// Used to convey validation information to and from functions that
// actually perform validation. Mostly used in custom property
// classes.
class WXDLLIMPEXP_PROPGRID wxPGValidationInfo
{
    friend class wxPropertyGrid;
public:
    wxPGValidationInfo()
        : m_failureBehavior(wxPGVFBFlags::Null)
        , m_isFailing(false)
    {
    }

    ~wxPGValidationInfo() = default;

    // Returns failure behaviour which is a combination of
    // wxPGVFBFlags::XXX flags.
    wxPGVFBFlags GetFailureBehavior() const
    {
        return m_failureBehavior;
    }

    // Returns current failure message.
    const wxString& GetFailureMessage() const
    {
        return m_failureMessage;
    }

    // Returns reference to pending value.
    const wxVariant& GetValue() const
    {
        return m_value;
    }

    // Set validation failure behaviour
    // failureBehavior - Mixture of wxPGVFBFlags::XXX flags.
#if WXWIN_COMPATIBILITY_3_2
    wxDEPRECATED_MSG("use SetFailureBehavior with wxPGVFBFlags argument")
    void SetFailureBehavior(int failureBehavior)
    {
        SetFailureBehavior(static_cast<wxPGVFBFlags>(failureBehavior));
    }
#endif // WXWIN_COMPATIBILITY_3_2
    void SetFailureBehavior(wxPGVFBFlags failureBehavior)
    {
        m_failureBehavior = failureBehavior;
    }

    // Set current failure message.
    void SetFailureMessage(const wxString& message)
    {
        m_failureMessage = message;
    }

private:
    void SetValue(const wxVariant& value)
    {
        m_value = value;
    }

    void ClearFailureMessage()
    {
        m_failureMessage.clear();
    }

    void SetFailing(bool isFailing)
    {
        m_isFailing = isFailing;
    }

    bool IsFailing() const
    {
        return m_isFailing;
    }

    // Value to be validated.
    wxVariant       m_value;

    // Message displayed on validation failure.
    wxString        m_failureMessage;

    // Validation failure behaviour. Use wxPGVFBFlags::XXX flags.
    wxPGVFBFlags    m_failureBehavior;

    // True when validation is currently failing.
    bool            m_isFailing;
};

// -----------------------------------------------------------------------

// These are used with wxPropertyGrid::AddActionTrigger() and
// wxPropertyGrid::ClearActionTriggers().
enum class wxPGKeyboardActions
{
#if WXWIN_COMPATIBILITY_3_2
    Invalid = 0,
#else
    Invalid,
#endif

    // Select the next property.
    NextProperty,

    // Select the previous property.
    PrevProperty,

    // Expand the selected property, if it has child items.
    ExpandProperty,

    // Collapse the selected property, if it has child items.
    CollapseProperty,

    // Cancel and undo any editing done in the currently active property
    // editor.
    CancelEdit,

    // Move focus to the editor control of the currently selected
    // property.
    Edit,

    // Causes editor's button (if any) to be pressed.
    PressButton,
};

#if WXWIN_COMPATIBILITY_3_2
wxDEPRECATED_MSG("use wxPGKeyboardActions::Invalid instead")
constexpr wxPGKeyboardActions wxPG_ACTION_INVALID { wxPGKeyboardActions::Invalid };
wxDEPRECATED_MSG("use wxPGKeyboardActions::NextProperty instead")
constexpr wxPGKeyboardActions wxPG_ACTION_NEXT_PROPERTY { wxPGKeyboardActions::NextProperty };
wxDEPRECATED_MSG("use wxPGKeyboardActions::PrevProperty instead")
constexpr wxPGKeyboardActions wxPG_ACTION_PREV_PROPERTY { wxPGKeyboardActions::PrevProperty };
wxDEPRECATED_MSG("use wxPGKeyboardActions::ExpandProperty instead")
constexpr wxPGKeyboardActions wxPG_ACTION_EXPAND_PROPERTY { wxPGKeyboardActions::ExpandProperty };
wxDEPRECATED_MSG("use wxPGKeyboardActions::CollapseProperty instead")
constexpr wxPGKeyboardActions wxPG_ACTION_COLLAPSE_PROPERTY { wxPGKeyboardActions::CollapseProperty };
wxDEPRECATED_MSG("use wxPGKeyboardActions::CancelEdit instead")
constexpr wxPGKeyboardActions wxPG_ACTION_CANCEL_EDIT { wxPGKeyboardActions::CancelEdit };
wxDEPRECATED_MSG("use wxPGKeyboardActions::Edit instead")
constexpr wxPGKeyboardActions wxPG_ACTION_EDIT { wxPGKeyboardActions::Edit };
wxDEPRECATED_MSG("use wxPGKeyboardActions::PressButton instead")
constexpr wxPGKeyboardActions wxPG_ACTION_PRESS_BUTTON { wxPGKeyboardActions::PressButton };
#endif // WXWIN_COMPATIBILITY_3_2

// -----------------------------------------------------------------------

// -----------------------------------------------------------------------

// wxPropertyGrid is a specialized grid for editing properties
// such as strings, numbers, flagsets, fonts, and colours. wxPropertySheet
// used to do the very same thing, but it hasn't been updated for a while
// and it is currently deprecated.
// Please note that most member functions are inherited and as such not
// documented here. This means you will probably also want to read
// wxPropertyGridInterface class reference.
// To process input from a propertygrid control, use these event handler
// macros to direct input to member functions that take a wxPropertyGridEvent
// argument.
//  EVT_PG_SELECTED (id, func)
//     Respond to wxEVT_PG_SELECTED event, generated when a property selection
//     has been changed, either by user action or by indirect program
//     function. For instance, collapsing a parent property programmatically
//     causes any selected child property to become unselected, and may
//     therefore cause this event to be generated.
//  EVT_PG_CHANGING(id, func)
//     Respond to wxEVT_PG_CHANGING event, generated when property value
//     is about to be changed by user. Use wxPropertyGridEvent::GetValue()
//     to take a peek at the pending value, and wxPropertyGridEvent::Veto()
//     to prevent change from taking place, if necessary.
//  EVT_PG_HIGHLIGHTED(id, func)
//     Respond to wxEVT_PG_HIGHLIGHTED event, which occurs when mouse
//     moves over a property. Event's property is null if hovered area does
//     not belong to any property.
//  EVT_PG_RIGHT_CLICK(id, func)
//     Respond to wxEVT_PG_RIGHT_CLICK event, which occurs when property is
//     clicked on with right mouse button.
//  EVT_PG_DOUBLE_CLICK(id, func)
//     Respond to wxEVT_PG_DOUBLE_CLICK event, which occurs when property is
//     double-clicked onwith left mouse button.
//  EVT_PG_ITEM_COLLAPSED(id, func)
//     Respond to wxEVT_PG_ITEM_COLLAPSED event, generated when user collapses
//     a property or category..
//  EVT_PG_ITEM_EXPANDED(id, func)
//     Respond to wxEVT_PG_ITEM_EXPANDED event, generated when user expands
//     a property or category..
//  EVT_PG_LABEL_EDIT_BEGIN(id, func)
//     Respond to wxEVT_PG_LABEL_EDIT_BEGIN event, generated when is about to
//     begin editing a property label. You can veto this event to prevent the
//     action.
//  EVT_PG_LABEL_EDIT_ENDING(id, func)
//     Respond to wxEVT_PG_LABEL_EDIT_ENDING event, generated when is about to
//     end editing of a property label. You can veto this event to prevent the
//     action.
//  EVT_PG_COL_BEGIN_DRAG(id, func)
//     Respond to wxEVT_PG_COL_BEGIN_DRAG event, generated when user
//     starts resizing a column - can be vetoed.
//  EVT_PG_COL_DRAGGING,(id, func)
//     Respond to wxEVT_PG_COL_DRAGGING, event, generated when a
//     column resize by user is in progress. This event is also generated
//     when user double-clicks the splitter in order to recenter
//     it.
//  EVT_PG_COL_END_DRAG(id, func)
//     Respond to wxEVT_PG_COL_END_DRAG event, generated after column
//     resize by user has finished.
// Use Freeze() and Thaw() respectively to disable and enable drawing. This
// will also delay sorting etc. miscellaneous calculations to the last
// possible moment.
class WXDLLIMPEXP_PROPGRID wxPropertyGrid : public wxScrolled<wxControl>,
                                            public wxPropertyGridInterface
{
    friend class wxPropertyGridEvent;
    friend class wxPropertyGridPageState;
    friend class wxPropertyGridInterface;
    friend class wxPropertyGridManager;
    friend class wxPGHeaderCtrl;

    wxDECLARE_DYNAMIC_CLASS(wxPropertyGrid);
public:

#ifndef SWIG
    // Two step constructor.
    // Call Create when this constructor is called to build up the
    // wxPropertyGrid
    wxPropertyGrid();
#endif

    // The default constructor. The styles to be used are styles valid for
    // the wxWindow.
    wxPropertyGrid( wxWindow *parent, wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxPG_DEFAULT_STYLE,
                    const wxString& name = wxASCII_STR(wxPropertyGridNameStr) );

    // Destructor
    virtual ~wxPropertyGrid();

    // Adds given key combination to trigger given action.
    // Here is a sample code to make Enter key press move focus to
    // the next property.
    //   propGrid->AddActionTrigger(wxPGKeyboardActions::NextProperty, WXK_RETURN);
    //   propGrid->DedicateKey(WXK_RETURN);
    // action - Which action to trigger. See @ref propgrid_keyboard_actions.
    // keycode - Which keycode triggers the action.
    // modifiers - Which key event modifiers, in addition to keycode, are needed to
    //   trigger the action.
#if WXWIN_COMPATIBILITY_3_2
    wxDEPRECATED_MSG("use AddActionTrigger with 'action' argument as wxPGKeyboardActions")
    void AddActionTrigger(int action, int keycode, int modifiers)
    {
        AddActionTrigger(static_cast<wxPGKeyboardActions>(action), keycode, modifiers);
    }
#endif // WXWIN_COMPATIBILITY_3_2
    void AddActionTrigger(wxPGKeyboardActions action, int keycode, int modifiers = 0);

    // Dedicates a specific keycode to wxPropertyGrid. This means that such
    // key presses will not be redirected to editor controls.
    // Using this function allows, for example, navigation between
    // properties using arrow keys even when the focus is in the editor
    // control.
    void DedicateKey( int keycode )
    {
        m_dedicatedKeys.insert(keycode);
    }

    // This static function enables or disables automatic use of
    // wxGetTranslation for following strings: wxEnumProperty list labels,
    // wxFlagsProperty child property labels.
    // Default is false.
    static void AutoGetTranslation( bool enable );

    // Changes value of a property, as if from an editor.
    // Use this instead of SetPropertyValue() if you need the value to run
    // through validation process, and also send the property change event.
    // Returns true if value was successfully changed.
    bool ChangePropertyValue( wxPGPropArg id, wxVariant newValue );

    // Centers the splitter.
    // enableAutoResizing - If true, automatic column resizing is enabled
    //   (only applicable if window style wxPG_SPLITTER_AUTO_CENTER is used).
    void CenterSplitter( bool enableAutoResizing = false );

    // Deletes all properties.
    virtual void Clear() override;

    // Clears action triggers for given action.
#if WXWIN_COMPATIBILITY_3_2
    wxDEPRECATED_MSG("use ClearActionTriggers with wxPGKeyboardActions argument")
    void ClearActionTriggers(int action)
    {
        ClearActionTriggers(static_cast<wxPGKeyboardActions>(action));
    }
#endif // WXWIN_COMPATIBILITY_3_2
    void ClearActionTriggers(wxPGKeyboardActions action);

    // Forces updating the value of property from the editor control.
    // Note that wxEVT_PG_CHANGING and wxEVT_PG_CHANGED are dispatched using
    // ProcessEvent, meaning your event handlers will be called immediately.
    // Returns true if anything was changed.
#if WXWIN_COMPATIBILITY_3_2
    wxDEPRECATED_MSG("use CommitChangesFromEditor with wxPGSelectPropertyFlags argument")
    virtual bool CommitChangesFromEditor(wxUint32 flags)
    {
        return CommitChangesFromEditor(static_cast<wxPGSelectPropertyFlags>(flags));
    }
#endif // WXWIN_COMPATIBILITY_3_2
    virtual bool CommitChangesFromEditor(wxPGSelectPropertyFlags flags = wxPGSelectPropertyFlags::Null);

    // Two step creation.
    // Whenever the control is created without any parameters, use Create to
    // actually create it. Don't access the control's public methods before
    // this is called @see @link wndflags Additional Window Styles@endlink
    bool Create( wxWindow *parent, wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxPG_DEFAULT_STYLE,
                 const wxString& name = wxASCII_STR(wxPropertyGridNameStr) );

    // Call when editor widget's contents is modified.
    // For example, this is called when changes text in wxTextCtrl (used in
    // wxStringProperty and wxIntProperty).
    // This function should only be called by custom properties.
    void EditorsValueWasModified() { m_iFlags |= wxPG_FL_VALUE_MODIFIED; }

    // Reverse of EditorsValueWasModified().
    // This function should only be called by custom properties.
    void EditorsValueWasNotModified()
    {
        m_iFlags &= ~(wxPG_FL_VALUE_MODIFIED);
    }

    // Enables or disables (shows/hides) categories according to parameter
    // enable.
    bool EnableCategories( bool enable );

    // Scrolls and/or expands items to ensure that the given item is visible.
    // Returns true if something was actually done.
    bool EnsureVisible( wxPGPropArg id );

    // Reduces column sizes to minimum possible that contents are still
    // visibly (naturally some margin space will be applied as well).
    // Returns minimum size for the grid to still display everything.
    // Does not work well with wxPG_SPLITTER_AUTO_CENTER window style.
    // This function only works properly if grid size prior to call was already
    // fairly large.
    // Note that you can also get calculated column widths by calling
    // GetState->GetColumnWidth() immediately after this function returns.
    wxSize FitColumns()
    {
        return m_pState->DoFitColumns();
    }

    // Returns wxWindow that the properties are painted on, and which should
    // be used as the parent for editor controls.
    wxWindow* GetPanel()
    {
        return this;
    }

    // Returns current category caption background colour.
    wxColour GetCaptionBackgroundColour() const { return m_colCapBack; }

    wxFont& GetCaptionFont() { return m_captionFont; }

    // Returns current category caption font.
    const wxFont& GetCaptionFont() const { return m_captionFont; }

    // Returns current category caption text colour.
    wxColour GetCaptionForegroundColour() const { return m_colCapFore; }

    // Returns current cell background colour.
    wxColour GetCellBackgroundColour() const { return m_colPropBack; }

    // Returns current cell text colour when disabled.
    wxColour GetCellDisabledTextColour() const { return m_colDisPropFore; }

    // Returns current cell text colour.
    wxColour GetCellTextColour() const { return m_colPropFore; }

    // Returns number of columns currently on grid.
    unsigned int GetColumnCount() const
    {
        return m_pState->GetColumnCount();
    }

    // Returns colour of empty space below properties.
    wxColour GetEmptySpaceColour() const { return m_colEmptySpace; }

    // Returns height of highest characters of used font.
    int GetFontHeight() const { return m_fontHeight; }

    // Returns pointer to itself. Dummy function that enables same kind
    // of code to use wxPropertyGrid and wxPropertyGridManager.
    wxPropertyGrid* GetGrid() { return this; }

    // Returns rectangle of custom paint image.
    wxRect GetImageRect( wxPGProperty* p, int item ) const;

    // Returns size of the custom paint image in front of property.
    // If no argument is given (p is null), returns preferred size.
    wxSize GetImageSize( wxPGProperty* p = nullptr, int item = -1 ) const;

    // Returns last item which could be iterated using given flags.
    wxPGProperty* GetLastItem( int flags = wxPG_ITERATE_DEFAULT )
    {
        return m_pState->GetLastItem(flags);
    }

    const wxPGProperty* GetLastItem( int flags = wxPG_ITERATE_DEFAULT ) const
    {
        return m_pState->GetLastItem(flags);
    }

    // Returns colour of lines between cells.
    wxColour GetLineColour() const { return m_colLine; }

    // Returns background colour of margin.
    wxColour GetMarginColour() const { return m_colMargin; }

    // Returns margin width.
    int GetMarginWidth() const { return m_marginWidth; }

    // Returns most up-to-date value of selected property. This will return
    // value different from GetSelectedProperty()->GetValue() only when text
    // editor is activate and string edited by user represents valid,
    // uncommitted property value.
    wxVariant GetUncommittedPropertyValue();

    // Returns "root property". It does not have name, etc. and it is not
    // visible. It is only useful for accessing its children.
    wxPGProperty* GetRoot() const { return m_pState->m_properties; }

    // Returns height of a single grid row (in pixels).
    int GetRowHeight() const { return m_lineHeight; }

    // Returns currently selected property.
    wxPGProperty* GetSelectedProperty() const { return GetSelection(); }

    // Returns current selection background colour.
    wxColour GetSelectionBackgroundColour() const { return m_colSelBack; }

    // Returns current selection text colour.
    wxColour GetSelectionForegroundColour() const { return m_colSelFore; }

    // Returns current splitter x position.
    int GetSplitterPosition( unsigned int splitterIndex = 0 ) const
    {
        return m_pState->DoGetSplitterPosition(splitterIndex);
    }

    // Returns wxTextCtrl active in currently selected property, if any. Takes
    // into account wxOwnerDrawnComboBox.
    wxTextCtrl* GetEditorTextCtrl() const;

    wxPGValidationInfo& GetValidationInfo()
    {
        return m_validationInfo;
    }

    // Returns current vertical spacing.
    int GetVerticalSpacing() const { return (int)m_vspacing; }

    // Returns true if a property editor control has focus.
    bool IsEditorFocused() const;

    // Returns true if editor's value was marked modified.
    bool IsEditorsValueModified() const
        { return  ( m_iFlags & wxPG_FL_VALUE_MODIFIED ) != 0; }

    // Returns information about arbitrary position in the grid.
    // pt - Coordinates in the virtual grid space. You may need to use
    //   wxScrolled<T>::CalcScrolledPosition() for translating
    //   wxPropertyGrid client coordinates into something this member
    //   function can use.
    wxPropertyGridHitTestResult HitTest( const wxPoint& pt ) const;

    // Returns true if any property has been modified by the user.
    bool IsAnyModified() const
#if WXWIN_COMPATIBILITY_3_0
         { return m_pState->m_anyModified != (unsigned char)false; }
#else
         { return m_pState->m_anyModified; }
#endif

    // It is recommended that you call this function any time your code causes
    // wxPropertyGrid's top-level parent to change. wxPropertyGrid's OnIdle()
    // handler should be able to detect most changes, but it is not perfect.
    // newTLP - New top-level parent that is about to be set. Old top-level parent
    //   window should still exist as the current one.
    // This function is automatically called from wxPropertyGrid::
    // Reparent() and wxPropertyGridManager::Reparent(). You only
    // need to use it if you reparent wxPropertyGrid indirectly.
    void OnTLPChanging( wxWindow* newTLP );

    // Redraws given property.
    virtual void RefreshProperty( wxPGProperty* p ) override;

    // Registers a new editor class.
    // Returns pointer to the editor class instance that should be used.
    static wxPGEditor* RegisterEditorClass( wxPGEditor* editor,
                                            bool noDefCheck = false )
    {
        return DoRegisterEditorClass(editor, wxString(), noDefCheck);
    }

    static wxPGEditor* DoRegisterEditorClass( wxPGEditor* editorClass,
                                              const wxString& editorName,
                                              bool noDefCheck = false );

    // Resets all colours to the original system values.
    void ResetColours();

    // Resets column sizes and splitter positions, based on proportions.
    // enableAutoResizing - If true, automatic column resizing is enabled
    // (only applicable if window style wxPG_SPLITTER_AUTO_CENTER is used).
    void ResetColumnSizes( bool enableAutoResizing = false );

    // Selects a property.
    // Editor widget is automatically created, but not focused unless focus is
    // true.
    // id - Property to select.
    // Returns true if selection finished successfully. Usually only fails if
    // current value in editor is not valid.
    // This function clears any previous selection.
    bool SelectProperty( wxPGPropArg id, bool focus = false );

    // Set entire new selection from given list of properties.
    void SetSelection( const wxArrayPGProperty& newSelection )
    {
        DoSetSelection( newSelection, wxPGSelectPropertyFlags::DontSendEvent );
    }

    // Adds given property into selection. If wxPG_EX_MULTIPLE_SELECTION
    // extra style is not used, then this has same effect as
    // calling SelectProperty().
    // Multiple selection is not supported for categories. This
    // means that if you have properties selected, you cannot
    // add category to selection, and also if you have category
    // selected, you cannot add other properties to selection.
    // This member function will fail silently in these cases,
    // even returning true.
    bool AddToSelection( wxPGPropArg id )
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)
        return DoAddToSelection(p, wxPGSelectPropertyFlags::DontSendEvent);
    }

    // Removes given property from selection. If property is not selected,
    // an assertion failure will occur.
    bool RemoveFromSelection( wxPGPropArg id )
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)
        return DoRemoveFromSelection(p, wxPGSelectPropertyFlags::DontSendEvent);
    }

    // Makes given column editable by user.
    // editable - Using false here will disable column from being editable.
    void MakeColumnEditable( unsigned int column, bool editable = true );

    // Creates label editor wxTextCtrl for given column, for property
    // that is currently selected. When multiple selection is
    // enabled, this applies to whatever property GetSelection()
    // returns.
    // colIndex - Which column's label to edit. Note that you should not
    //   use value 1, which is reserved for property value
    //   column.
    void BeginLabelEdit( unsigned int column = 0 )
    {
        DoBeginLabelEdit(column, wxPGSelectPropertyFlags::DontSendEvent);
    }

    // Destroys label editor wxTextCtrl, if any.
    // commit - Use true (default) to store edited label text in
    //   property cell data.
    void EndLabelEdit( bool commit = true )
    {
        DoEndLabelEdit(commit, wxPGSelectPropertyFlags::DontSendEvent);
    }

    // Returns currently active label editor, nullptr if none.
    wxTextCtrl* GetLabelEditor() const
    {
        return m_labelEditor;
    }

    // Sets category caption background colour.
    void SetCaptionBackgroundColour(const wxColour& col);

    // Sets category caption text colour.
    void SetCaptionTextColour(const wxColour& col);

    // Sets default cell background colour - applies to property cells.
    // Note that appearance of editor widgets may not be affected.
    void SetCellBackgroundColour(const wxColour& col);

    // Sets cell text colour for disabled properties.
    void SetCellDisabledTextColour(const wxColour& col);

    // Sets default cell text colour - applies to property name and value text.
    // Note that appearance of editor widgets may not be affected.
    void SetCellTextColour(const wxColour& col);

    // Set number of columns (2 or more).
    void SetColumnCount( int colCount )
    {
        m_pState->SetColumnCount(colCount);
        Refresh();
    }

    // Sets the 'current' category - Append will add non-category properties
    // under it.
    void SetCurrentCategory( wxPGPropArg id )
    {
        wxPG_PROP_ARG_CALL_PROLOG()
        wxPropertyCategory* pc = wxDynamicCast(p, wxPropertyCategory);
        wxASSERT(pc);
        m_pState->m_currentCategory = pc;
    }

    // Sets colour of empty space below properties.
    void SetEmptySpaceColour(const wxColour& col);

    // Sets colour of lines between cells.
    void SetLineColour(const wxColour& col);

    // Sets background colour of margin.
    void SetMarginColour(const wxColour& col);

    // Sets selection background colour - applies to selected property name
    // background.
    void SetSelectionBackgroundColour(const wxColour& col);

    // Sets selection foreground colour - applies to selected property name
    // text.
    void SetSelectionTextColour(const wxColour& col);

    // Sets x coordinate of the splitter.
    // Splitter position cannot exceed grid size, and therefore setting it
    // during form creation may fail as initial grid size is often smaller
    // than desired splitter position, especially when sizers are being used.
    void SetSplitterPosition( int newXPos, int col = 0 )
    {
        DoSetSplitter(newXPos, col, wxPGSplitterPositionFlags::Refresh);
    }

    // Sets the property sorting function.
    // sortFunction - The sorting function to be used. It should return a value greater
    //   than 0 if position of p1 is after p2. So, for instance, when
    //   comparing property names, you can use following implementation:
    //   int MyPropertySortFunction(wxPropertyGrid* propGrid,
    //                              wxPGProperty* p1,
    //                              wxPGProperty* p2)
    //   {
    //       return p1->GetBaseName().compare( p2->GetBaseName() );
    //   }
    //   Default property sort function sorts properties by their labels
    //   (case-insensitively).
    void SetSortFunction( wxPGSortCallback sortFunction )
    {
        m_sortFunction = sortFunction;
    }

    // Returns the property sort function (default is null).
    wxPGSortCallback GetSortFunction() const
    {
        return m_sortFunction;
    }

    // Sets appearance of value cells representing an unspecified property
    // value. Default appearance is blank.
    // If you set the unspecified value to have any
    // textual representation, then that will override
    // "InlineHelp" attribute.
    void SetUnspecifiedValueAppearance( const wxPGCell& cell )
    {
        m_unspecifiedAppearance = m_propertyDefaultCell;
        m_unspecifiedAppearance.MergeFrom(cell);
    }

    // Returns current appearance of unspecified value cells.
    const wxPGCell& GetUnspecifiedValueAppearance() const
    {
        return m_unspecifiedAppearance;
    }

    // Returns (visual) text representation of the unspecified
    // property value.
    // argFlags - For internal use only.
    wxString GetUnspecifiedValueText( int argFlags = 0 ) const;

    // Set virtual width for this particular page. Width -1 indicates that the
    // virtual width should be disabled.
    void SetVirtualWidth( int width );

    // Moves splitter as left as possible, while still allowing all
    // labels to be shown in full.
    // privateChildrenToo - If false, will still allow private children to be cropped.
    void SetSplitterLeft( bool privateChildrenToo = false )
    {
        m_pState->SetSplitterLeft(privateChildrenToo);
    }

    // Sets vertical spacing. Can be 1, 2, or 3 - a value relative to font
    // height. Value of 2 should be default on most platforms.
    void SetVerticalSpacing( int vspacing )
    {
        m_vspacing = (unsigned char)vspacing;
        CalculateFontAndBitmapStuff( vspacing );
        if ( !m_pState->m_itemsAdded ) Refresh();
    }

    // Shows an brief error message that is related to a property.
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
        { return (m_iFlags & wxPG_FL_HAS_VIRTUAL_WIDTH) != 0; }

    const wxPGCommonValue* GetCommonValue( unsigned int i ) const
    {
        wxCHECK_MSG( i < m_commonValues.size(), nullptr, "Invalid item index" );
        return m_commonValues[i];
    }

    // Returns number of common values.
    unsigned int GetCommonValueCount() const
    {
        return (unsigned int) m_commonValues.size();
    }

    // Returns label of given common value.
    wxString GetCommonValueLabel( unsigned int i ) const
    {
        wxCHECK_MSG( i < m_commonValues.size(), wxString(), "Invalid item index" );
        return m_commonValues[i]->GetLabel();
    }

    // Returns index of common value that will truly change value to
    // unspecified.
    int GetUnspecifiedCommonValue() const { return m_cvUnspecified; }

    // Set index of common value that will truly change value to unspecified.
    // Using -1 will set none to have such effect.
    // Default is 0.
    void SetUnspecifiedCommonValue( int index ) { m_cvUnspecified = index; }

    // Shortcut for creating dialog-caller button. Used, for example, by
    // wxFontProperty.
    // This should only be called by properties.
    wxWindow* GenerateEditorButton( const wxPoint& pos, const wxSize& sz );

    // Fixes position of wxTextCtrl-like control (wxSpinCtrl usually
    // fits as one). Call after control has been created (but before
    // shown).
    void FixPosForTextCtrl( wxWindow* ctrl,
                            unsigned int forColumn = 1,
                            const wxPoint& offset = wxPoint(0, 0) );

    // Shortcut for creating text editor widget.
    // pos - Same as pos given for CreateEditor.
    // sz - Same as sz given for CreateEditor.
    // value - Initial text for wxTextCtrl.
    // secondary - If right-side control, such as button, also created,
    //   then create it first and pass it as this parameter.
    // extraStyle - Extra style flags to pass for wxTextCtrl.
    // Note that this should generally be called only by new classes derived
    // from wxPGProperty.
    wxWindow* GenerateEditorTextCtrl( const wxPoint& pos,
                                      const wxSize& sz,
                                      const wxString& value,
                                      wxWindow* secondary,
                                      int extraStyle = 0,
                                      int maxLen = 0,
                                      unsigned int forColumn = 1 );

    // Generates both textctrl and button.
    wxWindow* GenerateEditorTextCtrlAndButton( const wxPoint& pos,
        const wxSize& sz, wxWindow** psecondary, int limited_editing,
        wxPGProperty* property );

    // Generates position for a widget editor dialog box.
    // p - Property for which dialog is positioned.
    // sz - Known or over-approximated size of the dialog.
    // Returns position for dialog.
    wxPoint GetGoodEditorDialogPosition( wxPGProperty* p,
                                         const wxSize& sz );

    // Converts escape sequences in src_str to newlines,
    // tabs, etc. and copies result to dst_str.
    static wxString& ExpandEscapeSequences( wxString& dst_str,
                                            const wxString& src_str );

    // Converts newlines, tabs, etc. in src_str to escape
    // sequences, and copies result to dst_str.
    static wxString& CreateEscapeSequences( wxString& dst_str,
                                            const wxString& src_str );

    // Checks system screen design used for laying out various dialogs.
    static bool IsSmallScreen();

    // Returns rescaled bitmap
    static wxBitmap RescaleBitmap(const wxBitmap& srcBmp, double scaleX, double scaleY);

    // Returns rectangle that fully contains properties between and including
    // p1 and p2. Rectangle is in virtual scrolled window coordinates.
    wxRect GetPropertyRect( const wxPGProperty* p1,
                            const wxPGProperty* p2 ) const;

    // Returns pointer to current active primary editor control (nullptr if none).
    wxWindow* GetEditorControl() const;

    wxWindow* GetPrimaryEditor() const
    {
        return GetEditorControl();
    }

    // Returns pointer to current active secondary editor control (nullptr if
    // none).
    wxWindow* GetEditorControlSecondary() const
    {
        return m_wndEditor2;
    }

    // Refreshes any active editor control.
    void RefreshEditor();

    // Events from editor controls are forward to this function
    bool HandleCustomEditorEvent( wxEvent &event );

    // Mostly useful for page switching.
    void SwitchState( wxPropertyGridPageState* pNewState );

    // Internal flags
    enum wxPG_INTERNAL_FLAGS : long
    {
        wxPG_FL_INITIALIZED           = 0x0001,
        // Set when creating editor controls if it was clicked on.
        wxPG_FL_ACTIVATION_BY_CLICK   = 0x0002,
        wxPG_FL_FOCUSED               = 0x0004,
        wxPG_FL_MOUSE_CAPTURED        = 0x0008,
        wxPG_FL_MOUSE_INSIDE          = 0x0010,
        wxPG_FL_VALUE_MODIFIED        = 0x0020,
        // don't clear background of m_wndEditor
        wxPG_FL_PRIMARY_FILLS_ENTIRE  = 0x0040,
        // currently active editor uses custom image
        wxPG_FL_CUR_USES_CUSTOM_IMAGE = 0x0080,
        // cell colours override selection colours for selected cell
        wxPG_FL_CELL_OVERRIDES_SEL    = 0x0100,
        wxPG_FL_SCROLLED              = 0x0200,
        // set when all added/inserted properties get hideable flag
        wxPG_FL_ADDING_HIDEABLES      = 0x0400,
        // Disables showing help strings on statusbar.
        wxPG_FL_NOSTATUSBARHELP       = 0x0800,
        // Marks that we created the state, so we have to destroy it too.
        wxPG_FL_CREATEDSTATE          = 0x1000,
        // Set if contained in wxPropertyGridManager
        wxPG_FL_IN_MANAGER            = 0x2000,
        // Set after wxPropertyGrid is shown in its initial good size
        wxPG_FL_GOOD_SIZE_SET         = 0x4000,
        // Set when in SelectProperty.
        wxPG_FL_IN_SELECT_PROPERTY    = 0x8000,
        // Set when help string is shown in status bar
        wxPG_FL_STRING_IN_STATUSBAR        = 0x00010000,
        // Auto sort is enabled (for categorized mode)
        wxPG_FL_CATMODE_AUTO_SORT          = 0x00020000,
        // Active editor control is abnormally large
        wxPG_FL_ABNORMAL_EDITOR            = 0x00040000,
        // Recursion guard for HandleCustomEditorEvent
        wxPG_FL_IN_HANDLECUSTOMEDITOREVENT = 0x00080000,
        wxPG_FL_VALUE_CHANGE_IN_EVENT      = 0x00100000,
        // Editor control width should not change on resize
        wxPG_FL_FIXED_WIDTH_EDITOR         = 0x00200000,
        // Width of panel can be different from width of grid
        wxPG_FL_HAS_VIRTUAL_WIDTH          = 0x00400000,
        // Prevents RecalculateVirtualSize re-entrancy
        wxPG_FL_RECALCULATING_VIRTUAL_SIZE = 0x00800000
    };

    long GetInternalFlags() const { return m_iFlags; }
    bool HasInternalFlag( long flag ) const
        { return (m_iFlags & flag) != 0; }
    void SetInternalFlag( long flag ) { m_iFlags |= flag; }
    void ClearInternalFlag( long flag ) { m_iFlags &= ~(flag); }

    void OnComboItemPaint( const wxPGComboBox* pCb,
                           int item,
                           wxDC* pDc,
                           wxRect& rect,
                           int flags );

#if WXWIN_COMPATIBILITY_3_0
    // Standardized double-to-string conversion.
    static const wxString& DoubleToString( wxString& target,
                                           double value,
                                           int precision,
                                           bool removeZeroes,
                                           wxString* precTemplate = nullptr );
#endif // WXWIN_COMPATIBILITY_3_0

    // Call this from wxPGProperty::OnEvent() to cause property value to be
    // changed after the function returns (with true as return value).
    // ValueChangeInEvent() must be used if you wish the application to be
    // able to use wxEVT_PG_CHANGING to potentially veto the given value.
    void ValueChangeInEvent( const wxVariant& variant )
    {
        m_changeInEventValue = variant;
        m_iFlags |= wxPG_FL_VALUE_CHANGE_IN_EVENT;
    }

    // You can use this member function, for instance, to detect in
    // wxPGProperty::OnEvent() if wxPGProperty::SetValueInEvent() was
    // already called in wxPGEditor::OnEvent(). It really only detects
    // if was value was changed using wxPGProperty::SetValueInEvent(), which
    // is usually used when a 'picker' dialog is displayed. If value was
    // written by "normal means" in wxPGProperty::StringToValue() or
    // IntToValue(), then this function will return false (on the other hand,
    // wxPGProperty::OnEvent() is not even called in those cases).
    bool WasValueChangedInEvent() const
    {
        return (m_iFlags & wxPG_FL_VALUE_CHANGE_IN_EVENT) != 0;
    }

    // Returns true if given event is from first of an array of buttons
    // (as can be in case when wxPGMultiButton is used).
    bool IsMainButtonEvent( const wxEvent& event ) const
    {
        return (event.GetEventType() == wxEVT_BUTTON)
                    && (m_wndSecId == event.GetId());
    }

    // Pending value is expected to be passed in PerformValidation().
    virtual bool DoPropertyChanged( wxPGProperty* p,
                                    wxPGSelectPropertyFlags selFlags = wxPGSelectPropertyFlags::Null );

    // Called when validation for given property fails.
    // invalidValue - Value which failed in validation.
    // Returns true if user is allowed to change to another property even
    // if current has invalid value.
    // To add your own validation failure behaviour, override
    // wxPropertyGrid::DoOnValidationFailure().
    bool OnValidationFailure( wxPGProperty* property,
                              wxVariant& invalidValue );

    // Called to indicate property and editor has valid value now.
    void OnValidationFailureReset( wxPGProperty* property )
    {
        if ( property && property->HasFlag(wxPG_PROP_INVALID_VALUE) )
        {
            DoOnValidationFailureReset(property);
            property->ClearFlag(wxPG_PROP_INVALID_VALUE);
        }
        m_validationInfo.ClearFailureMessage();
    }

    // Override in derived class to display error messages in custom manner
    // (these message usually only result from validation failure).
    // If you implement this, then you also need to implement
    // DoHidePropertyError() - possibly to do nothing, if error
    // does not need hiding (e.g. it was logged or shown in a
    // message box).
    virtual void DoShowPropertyError( wxPGProperty* property,
                                      const wxString& msg );

    // Override in derived class to hide an error displayed by
    // DoShowPropertyError().
    virtual void DoHidePropertyError( wxPGProperty* property );

#if wxUSE_STATUSBAR
    // Return wxStatusBar that is used by this wxPropertyGrid. You can
    // reimplement this member function in derived class to override
    // the default behaviour of using the top-level wxFrame's status
    // bar, if any.
    virtual wxStatusBar* GetStatusBar();
#endif

    // Override to customize property validation failure behaviour.
    // invalidValue - Value which failed in validation.
    // Returns true if user is allowed to change to another property even
    // if current has invalid value.
    virtual bool DoOnValidationFailure( wxPGProperty* property,
                                        wxVariant& invalidValue );

    // Override to customize resetting of property validation failure status.
    // Property is guaranteed to have flag wxPG_PROP_INVALID_VALUE set.
    virtual void DoOnValidationFailureReset( wxPGProperty* property );

    int GetSpacingY() const { return m_spacingy; }

    // Must be called in wxPGEditor::CreateControls() if primary editor window
    // is wxTextCtrl, just before textctrl is created.
    // text - Initial text value of created wxTextCtrl.
    void SetupTextCtrlValue( const wxString& text ) { m_prevTcValue = text; }

    // Unfocuses or closes editor if one was open, but does not deselect
    // property.
    bool UnfocusEditor();

    virtual void SetWindowStyleFlag( long style ) override;

    void DrawItems( const wxPGProperty* p1, const wxPGProperty* p2 );

    void DrawItem( wxPGProperty* p )
    {
        DrawItems(p,p);
    }

    virtual void DrawItemAndChildren( wxPGProperty* p );

    // Draws item, children, and consecutive parents as long as category is
    // not met.
    void DrawItemAndValueRelated( wxPGProperty* p );

protected:

    // wxPropertyGridPageState used by the grid is created here.
    // If grid is used in wxPropertyGridManager, there is no point overriding
    // this - instead, set custom wxPropertyGridPage classes.
    virtual wxPropertyGridPageState* CreateState() const;

    enum PerformValidationFlags
    {
        SendEvtChanging         = 0x0001,
        IsStandaloneValidation  = 0x0002   // Not called in response to event
    };

    // Runs all validation functionality (includes sending wxEVT_PG_CHANGING).
    // Returns true if all tests passed. Implement in derived class to
    // add additional validation behaviour.
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

    // Public methods for semi-public use
    bool DoSelectProperty( wxPGProperty* p, wxPGSelectPropertyFlags flags = wxPGSelectPropertyFlags::Null );

    // Overridden functions.
    virtual bool Destroy() override;
    // Returns property at given y coordinate (relative to grid's top left).
    wxPGProperty* GetItemAtY( int y ) const { return DoGetItemAtY(y); }

    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = (const wxRect *) nullptr ) override;
    virtual bool SetFont( const wxFont& font ) override;
    virtual void SetExtraStyle( long exStyle ) override;
    virtual bool Reparent( wxWindowBase *newParent ) override;
    virtual void ScrollWindow(int dx, int dy, const wxRect* rect) override;
    virtual void SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY,
                               int noUnitsX, int noUnitsY,
                               int xPos, int yPos, bool noRefresh) override;
protected:
    virtual void DoThaw() override;

    virtual wxSize DoGetBestSize() const override;
    virtual void DoEnable(bool enable) override;

    wxCursor            m_cursorSizeWE;

    // wxWindow pointers to editor control(s).
    wxWindow            *m_wndEditor;
    wxWindow            *m_wndEditor2;
    // Actual positions of the editors within the cell.
    wxPoint             m_wndEditorPosRel;
    wxPoint             m_wndEditor2PosRel;

    wxBitmap            *m_doubleBuffer;

    // Local time ms when control was created.
    wxMilliClock_t      m_timeCreated;

    // wxPGProperty::OnEvent can change value by setting this.
    wxVariant           m_changeInEventValue;

    // Id of m_wndEditor2, or its first child, if any.
    int                 m_wndSecId;

    // Extra Y spacing between the items.
    int                 m_spacingy;

    // Control client area width; updated on resize.
    int                 m_width;

    // Control client area height; updated on resize.
    int                 m_height;

    // Current non-client width (needed when auto-centering).
    int                 m_ncWidth;

    // The gutter spacing in front and back of the image.
    // This determines the amount of spacing in front of each item
    int                 m_gutterWidth;

    // Includes separator line.
    int                 m_lineHeight;

    // Gutter*2 + image width.
    int                 m_marginWidth;

    // y spacing for expand/collapse button.
    int                 m_buttonSpacingY;

    // Extra margin for expanded sub-group items.
    int                 m_subgroup_extramargin;

    // The image width of the [+] icon.
    // This is also calculated in the gutter
    int                 m_iconWidth;

    // The image height of the [+] icon.
    // This is calculated as minimal size and to align
    int                 m_iconHeight;

    // Current cursor id.
    int                 m_curcursor;

    // Caption font. Same as normal font plus bold style.
    wxFont              m_captionFont;

    int                 m_fontHeight;  // Height of the font.

    // m_splitterx when drag began.
    int                 m_startingSplitterX;

    // Index to splitter currently being dragged (0=one after the first
    // column).
    int                 m_draggedSplitter;

    // Changed property, calculated in PerformValidation().
    wxPGProperty*       m_chgInfo_changedProperty;

    // Lowest property for which editing happened, but which does not have
    // aggregate parent
    wxPGProperty*       m_chgInfo_baseChangedProperty;

    // Changed property value, calculated in PerformValidation().
    wxVariant           m_chgInfo_pendingValue;

    // Passed to SetValue.
    wxVariant           m_chgInfo_valueList;

    // Validation information.
    wxPGValidationInfo  m_validationInfo;

    // Actions and keys that trigger them.
    std::unordered_map<int, std::pair<wxPGKeyboardActions, wxPGKeyboardActions>> m_actionTriggers;

    // Appearance of currently active editor.
    wxPGCell            m_editorAppearance;

    // Appearance of a unspecified value cell.
    wxPGCell            m_unspecifiedAppearance;

    // List of properties to be deleted/removed in idle event handler.
    std::set<wxPGProperty*>  m_deletedProperties;
    std::set<wxPGProperty*>  m_removedProperties;

#if !WXWIN_COMPATIBILITY_3_0
    // List of editors and their event handlers to be deleted in idle event handler.
    std::vector<wxObject*> m_deletedEditorObjects;
#endif

    // List of key codes that will not be handed over to editor controls.
    std::unordered_set<int> m_dedicatedKeys;

    //
    // Temporary values
    //

    // Bits are used to indicate which colours are customized.
    unsigned short      m_coloursCustomized;

    // x - m_splitterx.
    signed char                 m_dragOffset;

    // 0 = not dragging, 1 = drag just started, 2 = drag in progress
    unsigned char       m_dragStatus;

#if WXWIN_COMPATIBILITY_3_0
    // True when editor control is focused.
    unsigned char       m_editorFocused;
#else
    bool                m_editorFocused;
#endif

    unsigned char       m_vspacing;

    // 1 if in DoPropertyChanged()
    bool                m_inDoPropertyChanged;

    // 1 if in CommitChangesFromEditor()
    bool                m_inCommitChangesFromEditor;

    // 1 if in DoSelectProperty()
    bool                m_inDoSelectProperty;

    bool                m_inOnValidationFailure;

    wxPGVFBFlags        m_permanentValidationFailureBehavior;  // Set by app

    // DoEditorValidate() recursion guard
    wxRecursionGuardFlag    m_validatingEditor;

    // Internal flags - see wxPG_FL_XXX constants.
    wxUint32            m_iFlags;

    // Mouse is hovering over this column (index), -1 for margin
    int                 m_colHover;

    // pointer to property that has mouse hovering
    wxPGProperty*       m_propHover;

    // Active label editor and its actual position within the cell
    wxTextCtrl*         m_labelEditor;
    wxPoint             m_labelEditorPosRel;

    // For which property the label editor is active
    wxPGProperty*       m_labelEditorProperty;

    // EventObject for wxPropertyGridEvents
    wxWindow*           m_eventObject;

    // What (global) window is currently focused (needed to resolve event
    // handling mess).
    wxWindow*           m_curFocused;

    // Event currently being sent - nullptr if none at the moment
    wxPropertyGridEvent*    m_processedEvent;

    // Last known top-level parent
    wxWindow*           m_tlp;

    // Last closed top-level parent
    wxWindow*           m_tlpClosed;

    // Local time ms when tlp was closed.
    wxMilliClock_t      m_tlpClosedTime;

    // Sort function
    wxPGSortCallback    m_sortFunction;

    // y coordinate of property that mouse hovering
    int                 m_propHoverY;

    // Which column's editor is selected (usually 1)?
    unsigned int        m_selColumn;
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
    std::vector<wxPGCell> m_propCellsBackup;

    // NB: These *cannot* be moved to globals.

    // labels when properties use common values
    std::vector<wxPGCommonValue*> m_commonValues;

    // array of live events
    std::vector<wxPropertyGridEvent*> m_liveEvents;

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

    void OnDPIChanged(wxDPIChangedEvent& event);

    void OnTLPClose( wxCloseEvent& event );

protected:

    bool AddToSelectionFromInputEvent( wxPGProperty* prop,
                                       unsigned int colIndex,
                                       wxMouseEvent* event = nullptr,
                                       wxPGSelectPropertyFlags selFlags = wxPGSelectPropertyFlags::Null );

    // Adjust the centering of the bitmap icons (collapse / expand) when the
    // caption font changes.
    // They need to be centered in the middle of the font, so a bit of deltaY
    // adjustment is needed. On entry, m_captionFont must be set to window
    // font. It will be modified properly.
    void CalculateFontAndBitmapStuff( int vspacing );

    wxRect GetEditorWidgetRect( wxPGProperty* p, int column ) const;

    void CorrectEditorWidgetSizeX();

    // Called in RecalculateVirtualSize() to reposition control
    // on virtual height changes.
    void CorrectEditorWidgetPosY();

#if WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED_MSG("use two-argument function DoDrawItems(dc,rect)")
    int DoDrawItems( wxDC& dc,
                     const wxRect* itemsRect,
                     bool isBuffered ) const
    {
        return DoDrawItemsBase(dc, itemsRect, isBuffered);
    }

    int DoDrawItems( wxDC& dc,
                     const wxRect* itemsRect ) const
    {
        return DoDrawItemsBase(dc, itemsRect, true);
    }

    int DoDrawItemsBase( wxDC& dc,
                     const wxRect* itemsRect,
                     bool isBuffered ) const;
#else
    int DoDrawItems( wxDC& dc,
                     const wxRect* itemsRect ) const;
#endif

    // Draws an expand/collapse (ie. +/-) button.
    virtual void DrawExpanderButton( wxDC& dc, const wxRect& rect,
                                     wxPGProperty* property ) const;

    // Draws items from topItemY to bottomItemY
    void DrawItems( wxDC& dc,
                    unsigned int topItemY,
                    unsigned int bottomItemY,
                    const wxRect* itemsRect = nullptr );

    // Translate wxKeyEvent to wxPGKeyboardActions::XXX
    std::pair<wxPGKeyboardActions, wxPGKeyboardActions> KeyEventToActions(const wxKeyEvent& event) const;
#if WXWIN_COMPATIBILITY_3_2
    wxDEPRECATED_MSG("use single-argument function KeyEventToActions(event)")
    wxPGKeyboardActions KeyEventToActions(wxKeyEvent &event, wxPGKeyboardActions* pSecond) const;
#endif // WXWIN_COMPATIBILITY_3_2

    wxPGKeyboardActions KeyEventToAction(wxKeyEvent& event) const;

    void ImprovedClientToScreen( int* px, int* py ) const;

    // Called by focus event handlers. newFocused is the window that becomes
    // focused.
    void HandleFocusChange( wxWindow* newFocused );

    // Reloads all non-customized colours from system settings.
    void RegainColours();

    virtual bool DoEditorValidate();

    // Similar to DoSelectProperty() but also works on columns
    // other than 1. Does not active editor if column is not
    // editable.
    bool DoSelectAndEdit( wxPGProperty* prop,
                          unsigned int colIndex,
                          wxPGSelectPropertyFlags selFlags );

    void DoSetSelection( const wxArrayPGProperty& newSelection,
                         wxPGSelectPropertyFlags selFlags = wxPGSelectPropertyFlags::Null );

    void DoSetSplitter( int newxpos,
                        int splitterIndex = 0,
                        wxPGSplitterPositionFlags flags = wxPGSplitterPositionFlags::Refresh );

    bool DoAddToSelection( wxPGProperty* prop,
                           wxPGSelectPropertyFlags selFlags = wxPGSelectPropertyFlags::Null );

    bool DoRemoveFromSelection( wxPGProperty* prop,
                                wxPGSelectPropertyFlags selFlags = wxPGSelectPropertyFlags::Null );

    void DoBeginLabelEdit( unsigned int colIndex, wxPGSelectPropertyFlags selFlags = wxPGSelectPropertyFlags::Null );
    void DoEndLabelEdit( bool commit, wxPGSelectPropertyFlags selFlags = wxPGSelectPropertyFlags::Null);
    void OnLabelEditorEnterPress( wxCommandEvent& event );
    void OnLabelEditorKeyPress( wxKeyEvent& event );

    wxPGProperty* DoGetItemAtY( int y ) const;

    void DestroyEditorWnd( wxWindow* wnd );
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

    // Repositions scrollbar and underlying panel according
    // to changed virtual size.
    void RecalculateVirtualSize( int forceXPos = -1 );

    void SetEditorAppearance( const wxPGCell& cell,
                              bool unspecified = false );

    void ResetEditorAppearance()
    {
        wxPGCell cell;
        cell.SetEmptyData();
        SetEditorAppearance(cell, false);
    }

    void PrepareAfterItemsAdded();

    // Send event from the property grid.
    // Omit the wxPGSelectPropertyFlags::NoValidate flag to allow vetoing the event
    bool SendEvent( wxEventType eventType, wxPGProperty* p,
                    wxVariant* pValue = nullptr,
                    wxPGSelectPropertyFlags selFlags = wxPGSelectPropertyFlags::NoValidate,
                    unsigned int column = 1 );

    void SendEvent(wxEventType eventType, int intVal);

    // This function only moves focus to the wxPropertyGrid if it already
    // was on one of its child controls.
    void SetFocusOnCanvas();

    bool DoHideProperty( wxPGProperty* p, bool hide, wxPGPropertyValuesFlags flags );

    void DeletePendingObjects();

private:

    bool ButtonTriggerKeyTest(wxPGKeyboardActions action, wxKeyEvent& event);

    wxDECLARE_EVENT_TABLE();
};

// -----------------------------------------------------------------------

#define wxPG_BASE_EVT_PRE_ID     1775

#ifndef SWIG

wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_SELECTED, wxPropertyGridEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_CHANGING, wxPropertyGridEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_CHANGED, wxPropertyGridEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_HIGHLIGHTED, wxPropertyGridEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_RIGHT_CLICK, wxPropertyGridEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_PAGE_CHANGED, wxPropertyGridEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_ITEM_COLLAPSED, wxPropertyGridEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_ITEM_EXPANDED, wxPropertyGridEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID, wxEVT_PG_DOUBLE_CLICK, wxPropertyGridEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID,
                          wxEVT_PG_LABEL_EDIT_BEGIN, wxPropertyGridEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID,
                          wxEVT_PG_LABEL_EDIT_ENDING, wxPropertyGridEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID,
                          wxEVT_PG_COL_BEGIN_DRAG, wxPropertyGridEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID,
                          wxEVT_PG_COL_DRAGGING, wxPropertyGridEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_PROPGRID,
                          wxEVT_PG_COL_END_DRAG, wxPropertyGridEvent );

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
        wxEVT_PG_DOUBLE_CLICK,
        wxEVT_PG_LABEL_EDIT_BEGIN,
        wxEVT_PG_LABEL_EDIT_ENDING,
        wxEVT_PG_COL_BEGIN_DRAG,
        wxEVT_PG_COL_DRAGGING,
        wxEVT_PG_COL_END_DRAG
    };
#endif


#define wxPG_BASE_EVT_TYPE       wxEVT_PG_SELECTED
#define wxPG_MAX_EVT_TYPE        (wxPG_BASE_EVT_TYPE+30)


#ifndef SWIG
typedef void (wxEvtHandler::*wxPropertyGridEventFunction)(wxPropertyGridEvent&);

#define EVT_PG_SELECTED(id, fn)              wxDECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_SELECTED, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), nullptr ),
#define EVT_PG_CHANGING(id, fn)              wxDECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_CHANGING, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), nullptr ),
#define EVT_PG_CHANGED(id, fn)               wxDECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_CHANGED, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), nullptr ),
#define EVT_PG_HIGHLIGHTED(id, fn)           wxDECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_HIGHLIGHTED, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), nullptr ),
#define EVT_PG_RIGHT_CLICK(id, fn)           wxDECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_RIGHT_CLICK, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), nullptr ),
#define EVT_PG_DOUBLE_CLICK(id, fn)          wxDECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_DOUBLE_CLICK, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), nullptr ),
#define EVT_PG_PAGE_CHANGED(id, fn)          wxDECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_PAGE_CHANGED, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), nullptr ),
#define EVT_PG_ITEM_COLLAPSED(id, fn)        wxDECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_ITEM_COLLAPSED, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), nullptr ),
#define EVT_PG_ITEM_EXPANDED(id, fn)         wxDECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_ITEM_EXPANDED, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), nullptr ),
#define EVT_PG_LABEL_EDIT_BEGIN(id, fn)      wxDECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_LABEL_EDIT_BEGIN, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), nullptr ),
#define EVT_PG_LABEL_EDIT_ENDING(id, fn)     wxDECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_LABEL_EDIT_ENDING, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), nullptr ),
#define EVT_PG_COL_BEGIN_DRAG(id, fn)        wxDECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_COL_BEGIN_DRAG, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), nullptr ),
#define EVT_PG_COL_DRAGGING(id, fn)          wxDECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_COL_DRAGGING, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), nullptr ),
#define EVT_PG_COL_END_DRAG(id, fn)          wxDECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_COL_END_DRAG, id, -1, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn ), nullptr ),

#define wxPropertyGridEventHandler(fn) \
    wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, fn )

#endif


// A propertygrid event holds information about events associated with
// wxPropertyGrid objects.
class WXDLLIMPEXP_PROPGRID wxPropertyGridEvent : public wxCommandEvent
{
public:

    // Constructor.
    wxPropertyGridEvent(wxEventType commandType=0, int id=0);

    // Copy constructor.
    wxPropertyGridEvent(const wxPropertyGridEvent& event);

    // Destructor.
    ~wxPropertyGridEvent();

    // Copyer.
    virtual wxEvent* Clone() const override;

    // Returns the column index associated with this event.
    // For the column dragging events, it is the column to the left
    // of the splitter being dragged
    unsigned int GetColumn() const
    {
        return m_column;
    }

    wxPGProperty* GetMainParent() const
    {
        wxCHECK_MSG(m_property, nullptr, "Property cannot be null");
        return m_property->GetMainParent();
    }

    // Returns property associated with this event.
    wxPGProperty* GetProperty() const
    {
        return m_property;
    }

    wxPGValidationInfo& GetValidationInfo()
    {
        wxASSERT(m_validationInfo);
        return *m_validationInfo;
    }

    // Returns true if you can veto the action that the event is signaling.
    bool CanVeto() const { return m_canVeto; }

    // Call this from your event handler to veto action that the event is
    // signaling.
    // You can only veto a shutdown if wxPropertyGridEvent::CanVeto returns
    // true.
    // Currently only wxEVT_PG_CHANGING supports vetoing.
    void Veto( bool veto = true ) { m_wasVetoed = veto; }

    // Returns name of the associated property.
    // Property name is stored in event, so it remains
    // accessible even after the associated property or
    // the property grid has been deleted.
    wxString GetPropertyName() const
    {
        return m_propertyName;
    }

    // Returns value of the associated property. Works for all event
    // types, but for wxEVT_PG_CHANGING this member function returns
    // the value that is pending, so you can call Veto() if the
    // value is not satisfactory.
    // Property value is stored in event, so it remains
    // accessible even after the associated property or
    // the property grid has been deleted.
    wxVariant GetPropertyValue() const
    {
        return m_validationInfo ? m_validationInfo->GetValue() : m_value;
    }

    // Returns value of the associated property.
    // See GetPropertyValue()
    wxVariant GetValue() const
    {
        return GetPropertyValue();
    }

    // Set override validation failure behaviour.
    // Only effective if Veto was also called, and only allowed if event type
    // is wxEVT_PG_CHANGING.
#if WXWIN_COMPATIBILITY_3_2
    wxDEPRECATED_MSG("use SetValidationFailureBehavior with wxPGVFBFlags argument")
    void SetValidationFailureBehavior(int flags)
    {
        SetValidationFailureBehavior(static_cast<wxPGVFBFlags>(flags));
    }
#endif // WXWIN_COMPATIBILITY_3_2
    void SetValidationFailureBehavior(wxPGVFBFlags flags)
    {
        wxASSERT( GetEventType() == wxEVT_PG_CHANGING );
        m_validationInfo->SetFailureBehavior( flags );
    }

    // Sets custom failure message for this time only. Only applies if
    // wxPGVFBFlags::SHOW_MESSAGE is set in validation failure flags.
    void SetValidationFailureMessage( const wxString& message )
    {
        wxASSERT( GetEventType() == wxEVT_PG_CHANGING );
        m_validationInfo->SetFailureMessage( message );
    }

    wxPGVFBFlags GetValidationFailureBehavior() const
    {
        wxASSERT( GetEventType() == wxEVT_PG_CHANGING );
        return m_validationInfo->GetFailureBehavior();
    }

    void SetColumn( unsigned int column )
    {
        m_column = column;
    }

    void SetCanVeto( bool canVeto ) { m_canVeto = canVeto; }
    bool WasVetoed() const { return m_wasVetoed; }

    // Changes the property associated with this event.
    void SetProperty( wxPGProperty* p )
    {
        m_property = p;
        if ( p )
            m_propertyName = p->GetName();
    }

    void SetPropertyValue( const wxVariant& value )
    {
        m_value = value;
    }

    void SetPropertyGrid( wxPropertyGrid* pg )
    {
        m_pg = pg;
        OnPropertyGridSet();
    }

    void SetupValidationInfo()
    {
        wxASSERT(m_pg);
        wxASSERT( GetEventType() == wxEVT_PG_CHANGING );
        m_validationInfo = &m_pg->GetValidationInfo();
        m_value = m_validationInfo->GetValue();
    }

private:
    void OnPropertyGridSet();
    wxDECLARE_DYNAMIC_CLASS(wxPropertyGridEvent);

    wxPGProperty*       m_property;
    wxPropertyGrid*     m_pg;
    wxPGValidationInfo* m_validationInfo;

    wxString            m_propertyName;
    wxVariant           m_value;

    unsigned int        m_column;

    bool                m_canVeto;
    bool                m_wasVetoed;
};


// -----------------------------------------------------------------------

// Allows populating wxPropertyGrid from arbitrary text source.
class WXDLLIMPEXP_PROPGRID wxPropertyGridPopulator
{
public:
    // Default constructor.
    wxPropertyGridPopulator();

    // Destructor.
    virtual ~wxPropertyGridPopulator();

    void SetState( wxPropertyGridPageState* state );

    void SetGrid( wxPropertyGrid* pg );

    // Appends a new property under bottommost parent.
    // propClass - Property class as string.
    wxPGProperty* Add( const wxString& propClass,
                       const wxString& propLabel,
                       const wxString& propName,
                       const wxString* propValue,
                       wxPGChoices* pChoices = nullptr );

    // Pushes property to the back of parent array (ie it becomes bottommost
    // parent), and starts scanning/adding children for it.
    // When finished, parent array is returned to the original state.
    void AddChildren( wxPGProperty* property );

    // Adds attribute to the bottommost property.
    // type - Allowed values: "string", (same as string), "int", "bool".
    // Empty string mean autodetect.
    bool AddAttribute( const wxString& name,
                       const wxString& type,
                       const wxString& value );

    // Called once in AddChildren.
    virtual void DoScanForChildren() = 0;

    // Returns id of parent property for which children can currently be
    // added.
    wxPGProperty* GetCurParent() const
    {
        return m_propHierarchy.back();
    }

    wxPropertyGridPageState* GetState() { return m_state; }
    const wxPropertyGridPageState* GetState() const { return m_state; }

    // Like wxString::ToLong, except allows N% in addition of N.
    static bool ToLongPCT( const wxString& s, long* pval, long max );

    // Parses strings of format "choice1"[=value1] ... "choiceN"[=valueN] into
    // wxPGChoices. Registers parsed result using idString (if not empty).
    // Also, if choices with given id already registered, then don't parse but
    // return those choices instead.
    wxPGChoices ParseChoices( const wxString& choicesString,
                              const wxString& idString );

    // Implement in derived class to do custom process when an error occurs.
    // Default implementation uses wxLogError.
    virtual void ProcessError( const wxString& msg );

protected:

    // Used property grid.
    wxPropertyGrid*         m_pg;

    // Used property grid state.
    wxPropertyGridPageState*    m_state;

    // Tree-hierarchy of added properties (that can have children).
    std::vector<wxPGProperty*> m_propHierarchy;

    // Hashmap for string-id to wxPGChoicesData mapping.
    std::unordered_map<wxString, wxPGChoicesData*> m_dictIdChoices;
};

// -----------------------------------------------------------------------

#ifndef WXBUILDING
// This really shouldn't be done here, but keep including this header
// for compatibility because user apps can be broken by removing it.
#include "wx/propgrid/props.h"
#endif // !WXBUILDING

#endif // wxUSE_PROPGRID

#endif // _WX_PROPGRID_PROPGRID_H_
