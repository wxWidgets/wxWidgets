/////////////////////////////////////////////////////////////////////////////
// Name:        wx/propgrid/private.h
// Purpose:     Private wxPropertyGrid declarations
// Author:      Artur Wieczorek, Jaakko Salli
// Created:     2023-03-10
// Copyright:   (c) 2023 wxWidgets development team
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROPGRID_PRIVATE_H_
#define _WX_PROPGRID_PRIVATE_H_

#if !defined(WXBUILDING)
#error This header is intended for internal use
#endif // !WXBUILDING

#include "wx/defs.h"
#include "wx/recguard.h"
#include "wx/weakref.h"

#include <cstdint>

class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_PROPGRID wxPropertyGrid;
class WXDLLIMPEXP_FWD_PROPGRID wxPropertyGridManager;
class WXDLLIMPEXP_FWD_PROPGRID wxPropertyGridPageState;

// Callback/gesture state added while hardening PropertyGrid must not alter the
// long-standing public class layouts. These implementation-only sidecars are
// keyed by the wrapper address but also retain its weak identity: reset in the
// constructor and identity-checked erase in the destructor make address reuse
// unable to inherit or remove another window's state.
struct wxPGPropertyGridTransientState
{
    explicit wxPGPropertyGridTransientState(wxPropertyGrid* grid = nullptr);

    wxWeakRef<wxWindow> identity;
    wxPropertyGridPageState* draggedState = nullptr;
    unsigned int draggedColumnCount = 0;
    int dragOffset = 0;
    bool splitterDontCenterBeforeDrag = false;
    bool splitterWasPreSetBeforeDrag = false;
    bool splitterEditorsHidden = false;
    bool splitterBeginDispatching = false;
    bool splitterBeginInvalidated = false;
    int keyboardSplitter = -1;
    bool inPerformValidation = false;
    unsigned int propertyCallbackDepth = 0;
    wxRecursionGuardFlag beginLabelEditGuard = 0;
    bool endingLabelEdit = false;
    std::uint64_t editorModificationRevision = 0;
};

WXDLLIMPEXP_PROPGRID wxPGPropertyGridTransientState&
wxPGGetPropertyGridTransientState(wxPropertyGrid* grid);
WXDLLIMPEXP_PROPGRID const wxPGPropertyGridTransientState&
wxPGGetPropertyGridTransientState(const wxPropertyGrid* grid);
WXDLLIMPEXP_PROPGRID void
wxPGResetPropertyGridTransientState(wxPropertyGrid* grid);
WXDLLIMPEXP_PROPGRID void
wxPGErasePropertyGridTransientState(wxPropertyGrid* grid);

struct wxPGPropertyGridManagerTransientState
{
    explicit wxPGPropertyGridManagerTransientState(
        wxPropertyGridManager* manager = nullptr);

    wxWeakRef<wxWindow> identity;
    int pageSelectionTarget = wxNOT_FOUND;
    bool pageSelectionInProgress = false;
    unsigned int eventDispatchDepth = 0;
};

WXDLLIMPEXP_PROPGRID wxPGPropertyGridManagerTransientState&
wxPGGetPropertyGridManagerTransientState(wxPropertyGridManager* manager);
WXDLLIMPEXP_PROPGRID void
wxPGResetPropertyGridManagerTransientState(wxPropertyGridManager* manager);
WXDLLIMPEXP_PROPGRID void
wxPGErasePropertyGridManagerTransientState(wxPropertyGridManager* manager);

#if wxUSE_TOOLBAR
// Make the Nth toolbar detachment performed by RemovePage() fail before any
// native or common toolbar state is changed. This is an implementation-only
// seam for proving both rollback branches of the page-removal transaction.
WXDLLIMPEXP_PROPGRID void
wxPGManagerFailToolbarRemovalForTesting(unsigned int ordinal);
WXDLLIMPEXP_PROPGRID void
wxPGManagerResetToolbarRemovalFailuresForTesting();
#endif

#if wxUSE_HEADERCTRL
class WXDLLIMPEXP_FWD_CORE wxHeaderCtrlEvent;

WXDLLIMPEXP_PROPGRID bool
wxPGProcessHeaderResizeEventForTesting(wxPropertyGridManager* manager,
                                       wxHeaderCtrlEvent& event);
#endif

#ifdef __WXMSW__
// Keep deferred editor teardown outside the complete property-grid callback
// transaction, even when application code runs a nested event/idle loop.
//
// This guard is deliberately independent of wxPropertyGrid itself: the grid
// is allowed to destroy itself while the guarded callback is on the stack.
class WXDLLIMPEXP_PROPGRID wxPGDeferredEditorCallbackEpoch final
{
public:
    wxPGDeferredEditorCallbackEpoch();
    ~wxPGDeferredEditorCallbackEpoch();

private:
    wxPGDeferredEditorCallbackEpoch(
        const wxPGDeferredEditorCallbackEpoch&) = delete;
    wxPGDeferredEditorCallbackEpoch& operator=(
        const wxPGDeferredEditorCallbackEpoch&) = delete;
};

// One-shot seam for the otherwise exceptional first SetParent(HWND_MESSAGE)
// failure in deferred editor teardown. The fallback still invokes the real
// Win32 APIs and is observable only through the bounded host count.
WXDLLIMPEXP_PROPGRID void
wxPGMSWFailNextDirectEditorParkingForTesting();
WXDLLIMPEXP_PROPGRID void
wxPGMSWFailNextFallbackEditorParkingForTesting();
WXDLLIMPEXP_PROPGRID void
wxPGMSWResetEditorParkingFailuresForTesting();
WXDLLIMPEXP_PROPGRID unsigned int
wxPGMSWGetEditorParkingHostCountForTesting();
WXDLLIMPEXP_PROPGRID unsigned int
wxPGMSWGetDeferredEditorBatchCountForTesting();
WXDLLIMPEXP_PROPGRID bool
wxPGMSWIsEditorParkingHostForTesting(WXWidget hwnd);
#else
class wxPGDeferredEditorCallbackEpoch final
{
public:
    // Keep this non-trivial even on ports where no global callback epoch is
    // needed. The many scoped instances in the common PropertyGrid sources
    // are intentional synchronization markers and must not trigger
    // -Wunused-variable under non-MSW -Werror builds.
    wxPGDeferredEditorCallbackEpoch() {}
    ~wxPGDeferredEditorCallbackEpoch() {}
};
#endif

// -----------------------------------------------------------------------

// space between vertical sides of a custom image
#define wxPG_CUSTOM_IMAGE_SPACINGY      1

// space between caption and selection rectangle,
#define wxPG_CAPRECTXMARGIN             2

// horizontally and vertically
#define wxPG_CAPRECTYMARGIN             1

// -----------------------------------------------------------------------

//
// Here are some platform dependent defines
// NOTE: More in propertygrid.cpp
//
// NB: Only define wxPG_TEXTCTRLXADJUST for platforms that do not
//     (yet) support wxTextEntry::SetMargins() for the left margin.

#if defined(__WXMSW__)

    // space between vertical line and value text
    #define wxPG_XBEFORETEXT            4
    // space between vertical line and value editor control
    #define wxPG_XBEFOREWIDGET          1

    // left margin can be set with wxTextEntry::SetMargins()
    #undef wxPG_TEXTCTRLXADJUST

    // comment to use bitmap buttons
    #define wxPG_ICON_WIDTH             9
    // 1 if wxRendererNative should be employed
    #define wxPG_USE_RENDERER_NATIVE    1

    // width of optional bitmap/image in front of property
    #define wxPG_CUSTOM_IMAGE_WIDTH     20

    // 1 if splitter drag detect margin and control cannot overlap
    #define wxPG_NO_CHILD_EVT_MOTION    0

    #define wxPG_NAT_BUTTON_BORDER_ANY          1
    #define wxPG_NAT_BUTTON_BORDER_X            1
    #define wxPG_NAT_BUTTON_BORDER_Y            1

    // If 1 then controls are refreshed explicitly in a few places
    #define wxPG_REFRESH_CONTROLS 0

#elif defined(__WXGTK__)

    // space between vertical line and value text
    #define wxPG_XBEFORETEXT            5
    // space between vertical line and value editor control
    #define wxPG_XBEFOREWIDGET          1

    // x position adjustment for wxTextCtrl (and like)
    // left margin can be set with wxTextEntry::SetMargins()
    #undef wxPG_TEXTCTRLXADJUST

    // comment to use bitmap buttons
    #define wxPG_ICON_WIDTH             9
    // 1 if wxRendererNative should be employed
    #define wxPG_USE_RENDERER_NATIVE    1

    // width of optional bitmap/image in front of property
    #define wxPG_CUSTOM_IMAGE_WIDTH     20

    // 1 if splitter drag detect margin and control cannot overlap
    #define wxPG_NO_CHILD_EVT_MOTION    1

    #define wxPG_NAT_BUTTON_BORDER_ANY      1
    #define wxPG_NAT_BUTTON_BORDER_X        1
    #define wxPG_NAT_BUTTON_BORDER_Y        1

    // If 1 then controls are refreshed after selected was drawn.
    #define wxPG_REFRESH_CONTROLS 1

#elif defined(__WXMAC__)

    // space between vertical line and value text
    #define wxPG_XBEFORETEXT            4
    // space between vertical line and value editor widget
    #define wxPG_XBEFOREWIDGET          1

    // x position adjustment for wxTextCtrl (and like)
    // left margin cannot be set with wxTextEntry::SetMargins()
    #define wxPG_TEXTCTRLXADJUST        1

    // comment to use bitmap buttons
    #define wxPG_ICON_WIDTH             11
    // 1 if wxRendererNative should be employed
    #define wxPG_USE_RENDERER_NATIVE    1

    // width of optional bitmap/image in front of property
    #define wxPG_CUSTOM_IMAGE_WIDTH     20

    // 1 if splitter drag detect margin and control cannot overlap
    #define wxPG_NO_CHILD_EVT_MOTION    0

    #define wxPG_NAT_BUTTON_BORDER_ANY      0
    #define wxPG_NAT_BUTTON_BORDER_X        0
    #define wxPG_NAT_BUTTON_BORDER_Y        0

    // If 1 then controls are refreshed after selected was drawn.
    #define wxPG_REFRESH_CONTROLS 0

#else // defaults

    // space between vertical line and value text
    #define wxPG_XBEFORETEXT            5
    // space between vertical line and value editor widget
    #define wxPG_XBEFOREWIDGET          1

    // x position adjustment for wxTextCtrl (and like)
    // left margin cannot be set with wxTextEntry::SetMargins()
    #define wxPG_TEXTCTRLXADJUST        3

    // comment to use bitmap buttons
    #define wxPG_ICON_WIDTH             9
    // 1 if wxRendererNative should be employed
    #define wxPG_USE_RENDERER_NATIVE    0

    // width of optional bitmap/image in front of property
    #define wxPG_CUSTOM_IMAGE_WIDTH     20

    // 1 if splitter drag detect margin and control cannot overlap
    #define wxPG_NO_CHILD_EVT_MOTION    1

    #define wxPG_NAT_BUTTON_BORDER_ANY      0
    #define wxPG_NAT_BUTTON_BORDER_X        0
    #define wxPG_NAT_BUTTON_BORDER_Y        0

    // If 1 then controls are refreshed after selected was drawn.
    #define wxPG_REFRESH_CONTROLS 0
#endif // platform


#define wxPG_CONTROL_MARGIN             0 // space between splitter and control

#define wxCC_CUSTOM_IMAGE_MARGIN1       4  // before image
#define wxCC_CUSTOM_IMAGE_MARGIN2       5  // after image

#define DEFAULT_IMAGE_OFFSET_INCREMENT \
    (wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2)

#define wxPG_DRAG_MARGIN                30

#if wxPG_NO_CHILD_EVT_MOTION
    #define wxPG_SPLITTERX_DETECTMARGIN1    3 // this much on left
    #define wxPG_SPLITTERX_DETECTMARGIN2    2 // this much on right
#else
    #define wxPG_SPLITTERX_DETECTMARGIN1    3 // this much on left
    #define wxPG_SPLITTERX_DETECTMARGIN2    2 // this much on right
#endif

// Use this macro to generate standard custom image height from
#define wxPG_STD_CUST_IMAGE_HEIGHT(LINEHEIGHT)  ((LINEHEIGHT)-3)

// Undefine wxPG_ICON_WIDTH to use supplied xpm bitmaps instead
// (for tree buttons)
//#undef wxPG_ICON_WIDTH

// -----------------------------------------------------------------------

// Events used only internally
wxDECLARE_EVENT(wxEVT_PG_HSCROLL, wxPropertyGridEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_PROPGRID,
                         wxEVT_PG_COLS_RESIZED,
                         wxPropertyGridEvent);

// -----------------------------------------------------------------------

// Flags used only internally

// wxBoolProperty, wxFlagsProperty specific flags
constexpr wxPGFlags wxPGPropertyFlags_UseCheckBox = wxPGFlags::Reserved_1;
// DCC = Double Click Cycles
constexpr wxPGFlags wxPGPropertyFlags_UseDCC = wxPGFlags::Reserved_2;

// wxStringProperty flag
constexpr wxPGFlags wxPGPropertyFlags_Password = wxPGFlags::Reserved_2;

// wxColourProperty flag - if set, then match from list is searched for a custom colour.
constexpr wxPGFlags wxPGPropertyFlags_TranslateCustom = wxPGFlags::Reserved_1;

// wxCursorProperty, wxSystemColourProperty - If set, then selection of choices is static
// and should not be changed (i.e. returns nullptr in GetPropertyChoices).
constexpr wxPGFlags wxPGPropertyFlags_StaticChoices = wxPGFlags::Reserved_1;

// wxSystemColourProperty - wxEnumProperty based classes cannot use wxPGFlags::Reserved_1
constexpr wxPGFlags wxPGPropertyFlags_HideCustomColour = wxPGFlags::Reserved_2;
constexpr wxPGFlags wxPGPropertyFlags_ColourHasAlpha = wxPGFlags::Reserved_3;

// wxFileProperty - if set, full path is shown in wxFileProperty.
constexpr wxPGFlags wxPGPropertyFlags_ShowFullFileName = wxPGFlags::ShowFullFileName;

// wxLongStringProperty - flag used to mark that edit button
// should be enabled even in the read-only mode.
constexpr wxPGFlags wxPGPropertyFlags_ActiveButton = wxPGFlags::Reserved_3;

#endif // _WX_PROPGRID_PRIVATE_H_
