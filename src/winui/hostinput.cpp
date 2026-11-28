/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hostinput.cpp
// Purpose:     Host pointer routing, native capture and dispatch adapters
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "private.h"
#include "hostinput.h"
#ifdef WXWINUI_TEST_SUPPORT
#include "host-input-test-access.h"
#endif
#include "hostnative.h"
#include "hostfocus.h"
#include "hostprofile.h"
#include "hostresize.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/window.h"
#endif
#include "wx/app.h"
#include "wx/toplevel.h"
#include "wx/scopeguard.h"
#include "wx/msw/private.h"

#include <winrt/Microsoft.UI.Input.h>
#include <windowsx.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

// Number of messages this thread has retrieved from its queue. New physical
// input can only arrive through such a retrieval, so a pointer event raised
// while a forwarded message is being sent, with no retrieval in between, is a
// synchronous reflection of that very message. See
// wxWinUIForwardedPointerDispatch::IsEcho().
static thread_local unsigned long long gs_messagePumpSerial = 0;

WXDLLIMPEXP_CORE void wxWinUINoteMessagePumped()
{
    ++gs_messagePumpSerial;
}

unsigned long long wxWinUIGetMessagePumpSerial()
{
    return gs_messagePumpSerial;
}

// True while a slot routes a wheel packet which its XAML control has already
// used: a CalendarView changing month, a list scrolling its rows. The wx
// window still gets its wxEVT_MOUSEWHEEL, but the packet must not go on to
// DefWindowProc(), which hands an unprocessed wheel to the parent window and
// would scroll the page under the control as well.
static thread_local bool gs_slotWheelUsedByPeer = false;

// Step 3: a control shown by XAML is sent the mouse messages of Win32 on its
// HWND, and wxMSW makes the wx events of them as it does for any window,
// where its slot used to make the wx events itself. An override of
// MSWWindowProc() sees them, and USER32 makes WM_CONTEXTMENU of a right
// button. WX_WINUI_INPUT_MESSAGES=0 brings back the events made by the slot.
bool wxWinUIInputMessagesEnabled()
{
    static const bool s_enabled = []()
    {
        wchar_t text[8];
        const DWORD length = ::GetEnvironmentVariableW(
            L"WX_WINUI_INPUT_MESSAGES", text, WXSIZEOF(text));
        return !(length == 1 && text[0] == L'0');
    }();

    return s_enabled;
}

// Step 3: see wxWinUITopLevelHost::QueuedPointer. 2, everything the pointer
// does waits for the message loop; 1 (WX_WINUI_INPUT_QUEUE=1), only what it
// does over a window which is not shown by XAML; 0, nothing.
static int wxWinUIInputQueueMode()
{
    static const int s_mode = []()
    {
        wchar_t text[8];
        const DWORD length = ::GetEnvironmentVariableW(
            L"WX_WINUI_INPUT_QUEUE", text, WXSIZEOF(text));
        if ( length == 1 && text[0] == L'0' )
            return 0;
        if ( length == 1 && text[0] == L'1' )
            return 1;
        return 2;
    }();

    return s_mode;
}


namespace
{

class wxWinUISyntheticMouseDispatch
{
public:
    wxWinUISyntheticMouseDispatch()
    {
        wxWinUIMSWBeginSyntheticMouseDispatch();
    }

    ~wxWinUISyntheticMouseDispatch()
    {
        wxWinUIMSWEndSyntheticMouseDispatch();
    }

private:
    wxDECLARE_NO_COPY_CLASS(wxWinUISyntheticMouseDispatch);
};

// USER32/Microsoft.UI.Input can synchronously turn a forwarded mouse message
// back into the same XAML pointer event (notably WM_NCMOUSEMOVE on a dialog
// border). This is an echo of the dispatch in progress, not another physical
// input transition. Keep an allocation-free stack, as a native handler may
// also run a modal loop admitting genuinely new input before SendMessage
// returns. A depth-only guard would incorrectly discard that new input.
class wxWinUIForwardedPointerDispatch
{
public:
    wxWinUIForwardedPointerDispatch(
        const std::shared_ptr<wxWinUIHostLifetime>& hostState,
        const wxWinUIInputAction& action, unsigned long long epoch)
        : m_hostState(hostState), m_action(action), m_epoch(epoch),
          m_pumpSerial(gs_messagePumpSerial), m_previous(ms_current)
    {
        ms_current = this;
    }

    ~wxWinUIForwardedPointerDispatch()
    {
        ms_current = m_previous;
    }

    static bool IsEcho(
        const std::shared_ptr<wxWinUIHostLifetime>& hostState,
        const wxWinUIPointerSample& sample, unsigned long long epoch)
    {
        // First rule: a pointer event of the same kind raised while the
        // innermost forwarded message is still being sent, with no message
        // retrieved from the queue since, cannot be new physical input: input
        // only arrives through the queue. It is the reflection of the message
        // being sent, whatever became of its coordinates (converted from DIPs
        // on the way back), timestamp (rebuilt by Microsoft.UI.Input) or
        // epoch (advanced by a boundary event the reflection itself raises).
        // Comparing those for exact equality let such reflections through
        // and recursed without bound when the pointer left a dialog.
        if ( const auto innermost = ms_current )
        {
            const auto& action = innermost->m_action;
            const auto kind = action.kind == wxWinUIInputKind::DoubleClick
                ? wxWinUIInputKind::Press : action.kind;
            if ( innermost->m_pumpSerial == gs_messagePumpSerial &&
                 innermost->m_hostState == hostState &&
                 action.device == sample.device &&
                 action.pointerId == sample.pointerId &&
                 kind == sample.kind && action.button == sample.button &&
                 action.modifiers == sample.modifiers &&
                 action.buttonMask == sample.buttonMask )
            {
                return true;
            }
        }

        // Second rule, for a handler which pumped messages: the exact packet
        // of a dispatch still in progress carries no new information either.
        for ( auto dispatch = ms_current; dispatch;
              dispatch = dispatch->m_previous )
        {
            const auto& action = dispatch->m_action;
            // A double click is derived by the wx state machine from the
            // second XAML Press, so normalize it back for packet identity.
            const auto kind = action.kind == wxWinUIInputKind::DoubleClick
                ? wxWinUIInputKind::Press : action.kind;
            if ( dispatch->m_hostState == hostState &&
                 dispatch->m_epoch == epoch &&
                 action.device == sample.device &&
                 action.pointerId == sample.pointerId &&
                 kind == sample.kind && action.button == sample.button &&
                 // Microsoft.UI.Input may reconstruct the Move timestamp
                 // while reflecting WM_NCMOUSEMOVE. An unchanged Move has
                 // no new semantic information during this send; buttons
                 // and wheel packets, by contrast, retain exact timestamps.
                 (kind == wxWinUIInputKind::Move ||
                  action.timestamp == sample.timestamp) &&
                 action.screenX == sample.screenX &&
                 action.screenY == sample.screenY &&
                 action.modifiers == sample.modifiers &&
                 action.buttonMask == sample.buttonMask &&
                 action.wheelDelta == sample.wheelDelta &&
                 action.horizontalWheel == sample.horizontalWheel )
            {
                return true;
            }
        }
        return false;
    }

private:
    static thread_local const wxWinUIForwardedPointerDispatch *ms_current;
    // The lifetime token, not the host address, excludes a replacement host
    // allocated at the same address during an application callback.
    const std::shared_ptr<wxWinUIHostLifetime> m_hostState;
    const wxWinUIInputAction m_action;
    // A genuinely new nested transition invalidates the old qualification.
    // In particular, A -> B -> A inside a modal handler must deliver the
    // return to A instead of confusing it with the first dispatch's echo.
    const unsigned long long m_epoch;
    // Queue retrievals seen when this dispatch started, see IsEcho().
    const unsigned long long m_pumpSerial;
    const wxWinUIForwardedPointerDispatch * const m_previous;
    wxDECLARE_NO_COPY_CLASS(wxWinUIForwardedPointerDispatch);
};

thread_local const wxWinUIForwardedPointerDispatch *
    wxWinUIForwardedPointerDispatch::ms_current = nullptr;

// Hard bound on re-entrant root pointer callbacks. A forwarded Win32 message
// can come back as a XAML pointer event before SendMessage() returns. The
// echo test above is what normally stops that, but no reflection it fails to
// recognise may ever recurse until the stack is exhausted. Genuine nesting,
// i.e. a modal loop started from a mouse handler, is only a few levels deep.
class wxWinUIRootPointerNesting
{
public:
    wxWinUIRootPointerNesting() { ++ms_depth; }
    ~wxWinUIRootPointerNesting() { --ms_depth; }

    static bool TooDeep() { return ms_depth > kMaxDepth; }

private:
    static constexpr unsigned kMaxDepth = 16;
    static thread_local unsigned ms_depth;
    wxDECLARE_NO_COPY_CLASS(wxWinUIRootPointerNesting);
};

thread_local unsigned wxWinUIRootPointerNesting::ms_depth = 0;

void wxWinUISendForwardedPointerMessage(
    const std::shared_ptr<wxWinUIHostLifetime>& hostState,
    const wxWinUIInputAction& action, unsigned long long epoch,
    HWND target, UINT message, WPARAM wParam, LPARAM lParam)
{
    const wxWinUIForwardedPointerDispatch dispatch(hostState, action, epoch);
    ::SendMessage(target, message, wParam, lParam);
}

bool wxWinUIStartsSystemModalLoop(LRESULT hitTest)
{
    switch ( hitTest )
    {
        case HTCAPTION:
        case HTSYSMENU:
        case HTLEFT:
        case HTRIGHT:
        case HTTOP:
        case HTBOTTOM:
        case HTTOPLEFT:
        case HTTOPRIGHT:
        case HTBOTTOMLEFT:
        case HTBOTTOMRIGHT:
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
            return true;
    }

    return false;
}

wxWinUIInputDevice wxWinUIGetInputDevice(
    winrt::Microsoft::UI::Input::PointerDeviceType device)
{
    using Device = winrt::Microsoft::UI::Input::PointerDeviceType;
    switch ( device )
    {
        case Device::Touch:
            return wxWinUIInputDevice::Touch;
        case Device::Pen:
            return wxWinUIInputDevice::Pen;
        case Device::Mouse:
        case Device::Touchpad:
            return wxWinUIInputDevice::Mouse;
    }

    return wxWinUIInputDevice::Mouse;
}

wxWinUIInputUpdateKind wxWinUIGetInputUpdateKind(
    winrt::Microsoft::UI::Input::PointerUpdateKind update)
{
    using Update = winrt::Microsoft::UI::Input::PointerUpdateKind;
    switch ( update )
    {
        case Update::LeftButtonPressed:
            return wxWinUIInputUpdateKind::LeftPressed;
        case Update::LeftButtonReleased:
            return wxWinUIInputUpdateKind::LeftReleased;
        case Update::RightButtonPressed:
            return wxWinUIInputUpdateKind::RightPressed;
        case Update::RightButtonReleased:
            return wxWinUIInputUpdateKind::RightReleased;
        case Update::MiddleButtonPressed:
            return wxWinUIInputUpdateKind::MiddlePressed;
        case Update::MiddleButtonReleased:
            return wxWinUIInputUpdateKind::MiddleReleased;
        case Update::XButton1Pressed:
            return wxWinUIInputUpdateKind::X1Pressed;
        case Update::XButton1Released:
            return wxWinUIInputUpdateKind::X1Released;
        case Update::XButton2Pressed:
            return wxWinUIInputUpdateKind::X2Pressed;
        case Update::XButton2Released:
            return wxWinUIInputUpdateKind::X2Released;
        case Update::Other:
            break;
    }

    return wxWinUIInputUpdateKind::Other;
}

wxWinUIInputTargetKey wxWinUIGetInputTargetKey(
    const wxWinUINativeTarget& target)
{
    wxWinUIInputTargetKey key;
    key.leaf = reinterpret_cast<std::uintptr_t>(target.GetLeafHwnd());
    key.generation = target.GetLeafGeneration();
    key.owner = reinterpret_cast<std::uintptr_t>(target.GetShellHwnd());
    key.ownerGeneration = target.GetShellGeneration();
    return key;
}

WORD wxWinUIGetWParamButtonMask(wxWinUIInputButton button)
{
    switch ( button )
    {
        case wxWinUIInputButton::Left:   return MK_LBUTTON;
        case wxWinUIInputButton::Right:  return MK_RBUTTON;
        case wxWinUIInputButton::Middle: return MK_MBUTTON;
        case wxWinUIInputButton::X1:     return MK_XBUTTON1;
        case wxWinUIInputButton::X2:     return MK_XBUTTON2;
        case wxWinUIInputButton::None:   return 0;
    }

    return 0;
}

// MK_* occupies the low word used by native mouse messages. Keep the Alt
// snapshot outside it: inputstate treats modifiers as opaque and the slot
// adapter consumes this bit when constructing the wxMouseEvent, while a
// native wParam naturally truncates it away.
constexpr unsigned wxWINUI_INPUT_MOD_ALT = 0x00010000u;

wxEventType wxWinUIGetSlotMouseEventType(
    const wxWinUIInputAction& action)
{
    if ( action.action == wxWinUIInputActionKind::HoverEnter )
        return wxEVT_ENTER_WINDOW;
    if ( action.action == wxWinUIInputActionKind::HoverLeave )
        return wxEVT_LEAVE_WINDOW;
    if ( action.action != wxWinUIInputActionKind::Dispatch )
        return wxEVT_NULL;

    if ( action.kind == wxWinUIInputKind::Move )
        return wxEVT_MOTION;

    const bool isDown = action.kind == wxWinUIInputKind::Press;
    const bool isUp = action.kind == wxWinUIInputKind::Release;
    const bool isDouble = action.kind == wxWinUIInputKind::DoubleClick;
    if ( !isDown && !isUp && !isDouble )
        return wxEVT_NULL;

    switch ( action.button )
    {
        case wxWinUIInputButton::Left:
            return isDown ? wxEVT_LEFT_DOWN
                          : isUp ? wxEVT_LEFT_UP : wxEVT_LEFT_DCLICK;
        case wxWinUIInputButton::Right:
            return isDown ? wxEVT_RIGHT_DOWN
                          : isUp ? wxEVT_RIGHT_UP : wxEVT_RIGHT_DCLICK;
        case wxWinUIInputButton::Middle:
            return isDown ? wxEVT_MIDDLE_DOWN
                          : isUp ? wxEVT_MIDDLE_UP : wxEVT_MIDDLE_DCLICK;
        case wxWinUIInputButton::X1:
            return isDown ? wxEVT_AUX1_DOWN
                          : isUp ? wxEVT_AUX1_UP : wxEVT_AUX1_DCLICK;
        case wxWinUIInputButton::X2:
            return isDown ? wxEVT_AUX2_DOWN
                          : isUp ? wxEVT_AUX2_UP : wxEVT_AUX2_DCLICK;
        case wxWinUIInputButton::None:
            break;
    }

    return wxEVT_NULL;
}

UINT wxWinUIGetDoubleClickMessage(UINT message)
{
    switch ( message )
    {
        case WM_LBUTTONDOWN:   return WM_LBUTTONDBLCLK;
        case WM_RBUTTONDOWN:   return WM_RBUTTONDBLCLK;
        case WM_MBUTTONDOWN:   return WM_MBUTTONDBLCLK;
        case WM_XBUTTONDOWN:   return WM_XBUTTONDBLCLK;
        case WM_NCLBUTTONDOWN: return WM_NCLBUTTONDBLCLK;
        case WM_NCRBUTTONDOWN: return WM_NCRBUTTONDBLCLK;
        case WM_NCMBUTTONDOWN: return WM_NCMBUTTONDBLCLK;
        case WM_NCXBUTTONDOWN: return WM_NCXBUTTONDBLCLK;
    }

    return message;
}

bool wxWinUIIsCompatibilityMouseEvent()
{
    constexpr LONG_PTR signature = 0xFF515700;
    constexpr LONG_PTR signatureMask = 0xFFFFFF00;
    return (::GetMessageExtraInfo() & signatureMask) == signature;
}

struct wxWinUIButtonMessages
{
    UINT client = 0;
    UINT nonClient = 0;
    WORD xButton = 0;
    int virtualKey = 0;
};

bool wxWinUIGetButtonMessages(wxWinUIInputButton button,
                              wxWinUIInputKind kind,
                              wxWinUIButtonMessages *messages)
{
    if ( !messages )
        return false;

    wxWinUIButtonMessages result;
    switch ( button )
    {
        case wxWinUIInputButton::Left:
            result.client = WM_LBUTTONDOWN;
            result.nonClient = WM_NCLBUTTONDOWN;
            result.virtualKey = VK_LBUTTON;
            break;
        case wxWinUIInputButton::Right:
            result.client = WM_RBUTTONDOWN;
            result.nonClient = WM_NCRBUTTONDOWN;
            result.virtualKey = VK_RBUTTON;
            break;
        case wxWinUIInputButton::Middle:
            result.client = WM_MBUTTONDOWN;
            result.nonClient = WM_NCMBUTTONDOWN;
            result.virtualKey = VK_MBUTTON;
            break;
        case wxWinUIInputButton::X1:
            result.client = WM_XBUTTONDOWN;
            result.nonClient = WM_NCXBUTTONDOWN;
            result.xButton = XBUTTON1;
            result.virtualKey = VK_XBUTTON1;
            break;
        case wxWinUIInputButton::X2:
            result.client = WM_XBUTTONDOWN;
            result.nonClient = WM_NCXBUTTONDOWN;
            result.xButton = XBUTTON2;
            result.virtualKey = VK_XBUTTON2;
            break;
        case wxWinUIInputButton::None:
            return false;
    }

    if ( kind == wxWinUIInputKind::Release )
    {
        ++result.client;
        ++result.nonClient;
    }
    else if ( kind == wxWinUIInputKind::DoubleClick )
    {
        result.client = wxWinUIGetDoubleClickMessage(result.client);
        result.nonClient = wxWinUIGetDoubleClickMessage(result.nonClient);
    }
    else if ( kind != wxWinUIInputKind::Press )
    {
        return false;
    }

    *messages = result;
    return true;
}

bool wxWinUIIsPointerContactSnapshotActive(
    wxWinUIInputDevice device,
    int virtualKey,
    POINTER_INPUT_TYPE pointerType,
    UINT32 pointerFlags,
    UINT32 penFlags)
{
    if ( (pointerFlags & POINTER_FLAG_UP) != 0 )
        return false;

    if ( device == wxWinUIInputDevice::Touch )
    {
        return pointerType == PT_TOUCH &&
               virtualKey == VK_LBUTTON &&
               (pointerFlags & POINTER_FLAG_INCONTACT) != 0;
    }

    if ( device != wxWinUIInputDevice::Pen ||
         pointerType != PT_PEN )
    {
        return false;
    }

    if ( virtualKey == VK_LBUTTON )
        return (pointerFlags & POINTER_FLAG_INCONTACT) != 0;
    if ( virtualKey == VK_RBUTTON )
        return (penFlags & PEN_FLAG_BARREL) != 0;
    return false;
}

} // anonymous namespace

wxWinUIClickSettings wxWinUIGetClickSettings()
{
    wxWinUIClickSettings settings;
    settings.maxInterval = ::GetDoubleClickTime();
    settings.rectangleWidth = ::GetSystemMetrics(SM_CXDOUBLECLK);
    settings.rectangleHeight = ::GetSystemMetrics(SM_CYDOUBLECLK);
    return settings;
}

bool wxWinUIShouldEmitSetCursor(wxWinUIInputKind kind)
{
    // WM_SETCURSOR is a movement/hit-test protocol. Button, wheel and
    // capture transitions at the current pointer point re-use its verdict.
    return kind == wxWinUIInputKind::Move;
}

UINT wxWinUIInternalCancelMessage()
{
    static const UINT message = ::RegisterWindowMessageW(
        L"wxWidgets.WinUI.InternalCancelMode");
    return message;
}

bool wxWinUIIsPointerContactActive(wxWinUIInputDevice device,
                                   std::uint32_t pointerId,
                                   int virtualKey)
{
    if ( device == wxWinUIInputDevice::Mouse )
    {
        return virtualKey &&
               (::GetAsyncKeyState(virtualKey) & 0x8000) != 0;
    }

    if ( !pointerId )
        return false;

    if ( device == wxWinUIInputDevice::Touch )
    {
        POINTER_INFO info = {};
        return ::GetPointerInfo(pointerId, &info) &&
               wxWinUIIsPointerContactSnapshotActive(
                   device, virtualKey, info.pointerType,
                   info.pointerFlags, 0);
    }

    if ( device == wxWinUIInputDevice::Pen )
    {
        POINTER_PEN_INFO info = {};
        return ::GetPointerPenInfo(pointerId, &info) &&
               wxWinUIIsPointerContactSnapshotActive(
                   device, virtualKey, info.pointerInfo.pointerType,
                   info.pointerInfo.pointerFlags, info.penFlags);
    }

    return false;
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIHostInputTestAccess::PointerContactActiveSnapshot(
    wxWinUIInputDevice device,
    int virtualKey,
    unsigned pointerType,
    unsigned pointerFlags,
    unsigned penFlags)
{
    return wxWinUIIsPointerContactSnapshotActive(
        device, virtualKey,
        static_cast<POINTER_INPUT_TYPE>(pointerType),
        static_cast<UINT32>(pointerFlags),
        static_cast<UINT32>(penFlags));
}
#endif // WXWINUI_TEST_SUPPORT

void wxWinUITopLevelHost::CloseNativeHover()
{
    m_hasUndeliveredNativeHoverEnter = false;
    m_undeliveredNativeHoverEnter = wxWinUIInputAction();

    if ( !m_lastNativeTarget.IsOk() )
        return;

    // Clear our authoritative state before invoking application/native code:
    // a re-entrant move starts a fresh hover instead of closing it twice.
    const wxWinUINativeTarget oldTarget = m_lastNativeTarget;
    const wxWinUINativeArea oldArea = m_lastNativeArea;
    m_lastNativeTarget = wxWinUINativeTarget();
    m_lastNativeArea = wxWinUINativeArea::Client;

    if ( !oldTarget.IsValid() )
        return;

    const HWND hwnd =
        reinterpret_cast<HWND>(oldTarget.GetLeafHwnd());
    ::SendMessage(hwnd,
                  oldArea == wxWinUINativeArea::Client
                      ? WM_MOUSELEAVE
                      : WM_NCMOUSELEAVE,
                  0, 0);
}

bool wxWinUITopLevelHost::AbortUndeliveredNativeHover(
    const wxWinUIInputAction& nextAction)
{
    if ( !m_hasUndeliveredNativeHoverEnter )
        return false;

    const wxWinUIInputAction pending = m_undeliveredNativeHoverEnter;
    if ( pending.target != nextAction.target ||
         pending.area != nextAction.area )
    {
        return false;
    }

    m_hasUndeliveredNativeHoverEnter = false;
    m_undeliveredNativeHoverEnter = wxWinUIInputAction();
    if ( !m_inputState.AbortCommittedHoverEnter(pending) )
        return false;

    if ( m_lastNativeTarget.IsOk() &&
         wxWinUIGetInputTargetKey(m_lastNativeTarget) == pending.target &&
         ((pending.area == wxWinUIInputArea::Client) ==
          (m_lastNativeArea == wxWinUINativeArea::Client)) )
    {
        // No native wndproc observed this hover. Retire only the host-side
        // identity; sending WM_(NC)MOUSELEAVE would invent an unmatched Leave.
        m_lastNativeTarget = wxWinUINativeTarget();
        m_lastNativeArea = wxWinUINativeArea::Client;
    }
    return true;
}

void wxWinUITopLevelHost::MarkNativeHoverDispatchStarted(
    const wxWinUIInputAction& action)
{
    if ( !m_hasUndeliveredNativeHoverEnter ||
         action.kind != wxWinUIInputKind::Move )
        return;

    const wxWinUIInputAction& pending = m_undeliveredNativeHoverEnter;
    if ( pending.target == action.target &&
         pending.area == action.area )
    {
        // Clear before SendMessage: its wndproc can synchronously route newer
        // input, which must observe that this hover is already published.
        m_hasUndeliveredNativeHoverEnter = false;
        m_undeliveredNativeHoverEnter = wxWinUIInputAction();
    }
}

bool wxWinUITopLevelHost::ResolveInputTarget(
    const wxWinUIInputTargetKey& key,
    wxWinUINativeTarget *target) const
{
    if ( !target )
        return false;

    *target = wxWinUINativeTarget();
    if ( !m_tlw || !key.IsOk() ||
         !wxWinUIGetNativeTarget(
             m_tlw, reinterpret_cast<WXHWND>(key.leaf), target) ||
         wxWinUIGetInputTargetKey(*target) != key )
    {
        *target = wxWinUINativeTarget();
        return false;
    }

    return true;
}

bool wxWinUITopLevelHost::SyncNativeCapture(
    const wxWinUIPointerSample& sample,
    bool *hasNativeCapture,
    bool *executedActions,
    bool *interruptedPress,
    bool *superseded)
{
    if ( hasNativeCapture )
        *hasNativeCapture = false;
    if ( executedActions )
        *executedActions = false;
    if ( interruptedPress )
        *interruptedPress = false;
    if ( superseded )
        *superseded = false;

    const auto observeCapture =
        [this](wxWinUIInputTargetKey *owner,
               bool *isNative,
               bool *isForeign)
        {
            *owner = wxWinUIInputTargetKey();
            *isNative = false;
            *isForeign = false;

            const HWND capture = ::GetCapture();
            if ( !capture ||
                 capture == m_bridge ||
                 capture == m_inner ||
                 (m_bridge && ::IsChild(m_bridge, capture)) )
            {
                return;
            }

            wxWinUINativeTarget target;
            if ( wxWinUIGetNativeTarget(
                     m_tlw, reinterpret_cast<WXHWND>(capture), &target) )
            {
                *owner = wxWinUIGetInputTargetKey(target);
                *isNative = true;
            }
            else
            {
                // A capture belonging to another TLW/process is opaque. It
                // invalidates any previous local capture, but the current
                // routed event must fail closed instead of hit-testing below.
                *isForeign = true;
            }
        };

    wxWinUIInputTargetKey owner;
    bool isNative = false;
    bool isForeign = false;
    observeCapture(&owner, &isNative, &isForeign);

    // Cancellation callbacks may themselves release or transfer capture.
    // Converge on the observed USER32 owner, with a hard retry bound.
    for ( unsigned attempt = 0; attempt != 3; ++attempt )
    {
        const wxWinUIInputTransition transition =
            m_inputState.NativeCaptureChanged(owner, sample);
        if ( executedActions && !transition.actions.empty() )
            *executedActions = true;
        if ( interruptedPress &&
             (transition.HasAction(wxWinUIInputActionKind::CancelPress) ||
              transition.HasAction(
                  wxWinUIInputActionKind::ReleaseCapture)) )
        {
            *interruptedPress = true;
        }

        bool transitionSuperseded = false;
        if ( !transition.actions.empty() &&
             !ExecuteInputTransition(
                 transition, nullptr, &transitionSuperseded) )
        {
            if ( superseded )
                *superseded = transitionSuperseded;
            return false;
        }

        wxWinUIInputTargetKey nextOwner;
        bool nextNative = false;
        bool nextForeign = false;
        observeCapture(&nextOwner, &nextNative, &nextForeign);
        if ( nextOwner == owner &&
             nextNative == isNative &&
             nextForeign == isForeign )
        {
            if ( hasNativeCapture )
                *hasNativeCapture = isNative;
            return !isForeign;
        }

        owner = nextOwner;
        isNative = nextNative;
        isForeign = nextForeign;
    }

    // A capture owner that changes three times during its own cancellation
    // callbacks is unstable; stop this source event rather than guessing.
    return false;
}

bool wxWinUITopLevelHost::ExecuteInputTransition(
    const wxWinUIInputTransition& transition,
    const wxWinUINativeHit *currentHit,
    bool *superseded)
{
    if ( superseded )
        *superseded = false;
    if ( transition.actions.empty() )
        return true;

    if ( ++m_inputTransitionEpoch == 0 )
        ++m_inputTransitionEpoch;
    const unsigned long long epoch = m_inputTransitionEpoch;
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });
    const auto cleanupRemaining =
        [this, &transition](std::size_t first)
        {
            for ( std::size_t i = first;
                  i < transition.actions.size(); ++i )
            {
                ExecuteEmergencyInputCleanup(transition.actions[i]);
            }
        };

    for ( std::size_t i = 0; i < transition.actions.size(); ++i )
    {
        const wxWinUIInputAction& action = transition.actions[i];
        if ( action.action == wxWinUIInputActionKind::BreakStorm )
        {
            if ( transition.stormDecision ==
                    wxWinUIInputStormDecision::BreakCycle )
            {
                wxWinUIInputLog("pointer storm cycle period=%u: breaking",
                                action.stormPeriod);
                CancelIslandPointerState();
            }
        }
        else if ( !ExecuteInputAction(action, currentHit, epoch) )
        {
            cleanupRemaining(i + 1);
            if ( superseded )
            {
                *superseded =
                    !hostState ||
                    hostState->GetHost() != this ||
                    m_shuttingDown ||
                    m_inputTransitionEpoch != epoch;
            }
            return false;
        }

        if ( !hostState || hostState->GetHost() != this || m_shuttingDown )
        {
            cleanupRemaining(i + 1);
            if ( superseded )
                *superseded = true;
            return false;
        }
        if ( m_inputTransitionEpoch != epoch )
        {
            // One of the callbacks above synchronously routed a newer input
            // transition. The remainder of this snapshot is obsolete.
            cleanupRemaining(i + 1);
            if ( superseded )
                *superseded = true;
            return false;
        }
    }

    return true;
}

void wxWinUITopLevelHost::ExecuteEmergencyInputCleanup(
    const wxWinUIInputAction& action)
{
    if ( action.action == wxWinUIInputActionKind::HoverLeave )
    {
        // A balancing release is deliberately ordered before hover cleanup.
        // If its callback aborts the outer snapshot, re-prove that the old
        // target is no longer under the sample before retiring it. A nested
        // transition or indeterminate geometry wins; the next real sample
        // will converge without inventing a Leave.
        if ( m_shuttingDown || !m_tlw )
            return;

        const unsigned long long epoch = m_inputTransitionEpoch;
        const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
        const RECT rootScreen = GetClientScreenRect();
        const wxPoint pointClient(action.screenX - rootScreen.left,
                                  action.screenY - rootScreen.top);
        const POINT pointScreen = { action.screenX, action.screenY };
        wxWinUINativeHit currentHit;
        wxWinUIRouteObservation observation;
        bool hasNativeHit = false;
        if ( !ClassifyRootPointer(
                 pointClient, pointScreen, nullptr, false,
                 &currentHit, &observation, &hasNativeHit) ||
             !hostState || hostState->GetHost() != this ||
             m_shuttingDown || m_inputTransitionEpoch != epoch ||
             observation.surface == wxWinUIInputSurface::Indeterminate )
        {
            return;
        }

        const bool stillSameTarget =
            observation.surface == wxWinUIInputSurface::Native &&
            observation.target == action.target &&
            observation.area == action.area;
        if ( !stillSameTarget )
            (void)ExecuteInputAction(action, nullptr, epoch);
        return;
    }

    if ( action.action == wxWinUIInputActionKind::CancelPress )
    {
        if ( !m_inputState.CanDeliverCancellation(action) )
            return;

        wxWinUINativeTarget target;
        if ( !m_shuttingDown &&
             ResolveInputTarget(action.target, &target) )
        {
            ::SendMessage(
                reinterpret_cast<HWND>(target.GetLeafHwnd()),
                WM_CANCELMODE, 0, 0);
        }
        m_inputState.AcknowledgeCancellation(action);
        return;
    }

    // ReleaseCapture is always the first action of a cancellation
    // transition. It can therefore never be an undelivered remainder after
    // another callback; releasing a same-HWND capture here could steal one
    // established by the nested gesture.
}

// Windows delivers WM_PAINT only when nothing else is waiting in the queue,
// which is what lets a window that invalidates itself on every mouse movement
// still appear to follow the pointer: the moves coalesce in the queue and the
// paint gets its turn. Routed pointer input is *sent*, not posted, so it never
// takes that turn and the paint can wait indefinitely -- measured at 300 ms of
// waiting for 1.7 ms of painting, i.e. a canvas frozen for as long as the hand
// keeps moving. Give the target its turn explicitly instead, bounded so that
// painting cannot starve the input either.
void wxWinUITopLevelHost::LetPointerTargetPaint(WXHWND targetHandle)
{
    const HWND target = reinterpret_cast<HWND>(targetHandle);
    if ( !target || !::IsWindow(target) || m_shuttingDown )
        return;

    if ( wxWinUIOptimisationDisabled("paint") )
        return;

    // ~120 Hz: fast enough that no display shows the difference, rare enough
    // that a slow paint cannot consume the whole event loop.
    constexpr unsigned long kMinPaintIntervalMs = 8;
    const unsigned long now = ::GetTickCount();
    if ( now - m_lastPointerPaintTick < kMinPaintIntervalMs )
        return;

    RECT update;
    if ( !::GetUpdateRect(target, &update, FALSE) )
        return;

    m_lastPointerPaintTick = now;
    ::RedrawWindow(target, nullptr, nullptr,
                   RDW_UPDATENOW | RDW_ALLCHILDREN);
}

bool wxWinUITopLevelHost::ExecuteInputAction(
    const wxWinUIInputAction& action,
    const wxWinUINativeHit *currentHit,
    unsigned long long expectedEpoch)
{
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const auto hostIsLive = [this, &hostState]()
    {
        return hostState &&
               hostState->GetHost() == this &&
               !m_shuttingDown;
    };
    const auto makeCaptureSample = [&action]()
    {
        wxWinUIPointerSample sample;
        sample.device = action.device;
        sample.kind = action.kind;
        sample.button = action.button;
        sample.pointerId = action.pointerId;
        sample.screenX = action.screenX;
        sample.screenY = action.screenY;
        sample.timestamp = ::GetTickCount64();
        sample.modifiers = action.modifiers;
        sample.buttonMask = action.buttonMask;
        return sample;
    };
    const auto failUndeliverableRelease =
        [this, &action, &hostIsLive, &makeCaptureSample]()
        {
            if ( AbortUndeliveredNativeHover(action) && hostIsLive() )
                ResetRootCursor();

            wxWinUIInputAction cancellation;
            if ( !m_inputState.AbortUndeliverableRelease(
                     action, &cancellation) )
            {
                return false;
            }

            // Deliver/acknowledge the exact serial first. WM_CANCELMODE is
            // callback-bearing; remember the capture transaction before it
            // runs so a handler can deliberately replace/renew capture
            // without the old cleanup stealing it afterward.
            const HWND captureBeforeCleanup = ::GetCapture();
            const unsigned long long captureGeneration =
                m_nativeCaptureMutationGeneration;
            ExecuteEmergencyInputCleanup(cancellation);
            if ( !hostIsLive() )
                return false;

            const HWND capture = ::GetCapture();
            wxWinUINativeTarget captureTarget;
            if ( capture && capture == captureBeforeCleanup &&
                 m_nativeCaptureMutationGeneration == captureGeneration &&
                 wxWinUIGetNativeTarget(
                     m_tlw, reinterpret_cast<WXHWND>(capture),
                     &captureTarget) &&
                 wxWinUIGetInputTargetKey(captureTarget)
                    .HasSameOwner(cancellation.target) &&
                 !m_inputState.HasActivePressForOwner(
                      cancellation.target) )
            {
                NotifyNativeCaptureMutation();
                (void)::ReleaseCapture();
            }

            if ( hostIsLive() )
                (void)SyncNativeCapture(makeCaptureSample());
            return false;
        };
    wxWinUINativeTarget targetIdentity;

    if ( action.action == wxWinUIInputActionKind::ReleaseCapture )
    {
        wxWinUIPointerSample sample = makeCaptureSample();
        sample.kind = wxWinUIInputKind::Cancel;
        const HWND capture = ::GetCapture();
        wxWinUINativeTarget captureIdentity;
        const bool matches =
            capture &&
            wxWinUIGetNativeTarget(
                  m_tlw, reinterpret_cast<WXHWND>(capture),
                  &captureIdentity) &&
            wxWinUIGetInputTargetKey(captureIdentity) == action.target;
        if ( !matches )
        {
            const bool converged =
                hostIsLive() && SyncNativeCapture(sample);
            return converged &&
                   hostIsLive() &&
                   m_inputTransitionEpoch == expectedEpoch &&
                   ::GetCapture() == nullptr;
        }

        NotifyNativeCaptureMutation();
        if ( !::ReleaseCapture() )
        {
            wxLogDebug("wxWinUI: failed to release native pointer "
                         "capture from %p", static_cast<void *>(capture));
            return false;
        }
        const bool converged =
            hostIsLive() && SyncNativeCapture(sample);
        return converged &&
               m_inputTransitionEpoch == expectedEpoch &&
               ::GetCapture() == nullptr;
    }

    if ( action.action == wxWinUIInputActionKind::CancelPress &&
         !m_inputState.CanDeliverCancellation(action) )
    {
        return true;
    }

    if ( action.action == wxWinUIInputActionKind::HoverLeave &&
         AbortUndeliveredNativeHover(action) )
    {
        if ( !m_shuttingDown )
            ResetRootCursor();
        return !m_shuttingDown;
    }

    if ( action.action == wxWinUIInputActionKind::HoverLeave &&
         !m_inputState.CommitHoverAction(action) )
    {
        return false;
    }

    if ( !ResolveInputTarget(action.target, &targetIdentity) )
    {
        if ( action.action == wxWinUIInputActionKind::CancelPress )
        {
            m_inputState.AcknowledgeCancellation(action);
            return true;
        }
        if ( action.action == wxWinUIInputActionKind::HoverLeave )
        {
            if ( m_lastNativeTarget.IsOk() &&
                 wxWinUIGetInputTargetKey(m_lastNativeTarget) ==
                    action.target &&
                 ((action.area == wxWinUIInputArea::Client) ==
                  (m_lastNativeArea == wxWinUINativeArea::Client)) )
            {
                // CloseNativeHover clears its host-side identity before it
                // checks whether the retired HWND can still receive Leave.
                CloseNativeHover();
            }
            if ( !m_shuttingDown )
                ResetRootCursor();
            return !m_shuttingDown;
        }
        if ( action.action == wxWinUIInputActionKind::Dispatch )
            return failUndeliverableRelease();
        // A destroyed/reparented generation is an expected fail-closed
        // outcome after arbitrary application code ran.
        return false;
    }

    if ( action.action == wxWinUIInputActionKind::CancelPress )
    {
        ::SendMessage(
            reinterpret_cast<HWND>(targetIdentity.GetLeafHwnd()),
            WM_CANCELMODE, 0, 0);
        m_inputState.AcknowledgeCancellation(action);
        return !m_shuttingDown &&
               m_inputTransitionEpoch == expectedEpoch;
    }

    if ( action.action == wxWinUIInputActionKind::HoverLeave )
    {
        if ( targetIdentity.Matches(m_lastNativeTarget) &&
             ((action.area == wxWinUIInputArea::Client) ==
              (m_lastNativeArea == wxWinUINativeArea::Client)) )
        {
            CloseNativeHover();
        }
        else
        {
            ::SendMessage(
                reinterpret_cast<HWND>(targetIdentity.GetLeafHwnd()),
                action.area == wxWinUIInputArea::Client
                    ? WM_MOUSELEAVE
                    : WM_NCMOUSELEAVE,
                0, 0);
        }
        if ( m_inputTransitionEpoch != expectedEpoch )
            return false;
        if ( !m_shuttingDown )
            ResetRootCursor();
        return !m_shuttingDown;
    }

    if ( action.action == wxWinUIInputActionKind::HoverEnter )
    {
        if ( !m_inputState.CommitHoverAction(action) )
            return false;

        m_lastNativeTarget = targetIdentity;
        m_lastNativeArea =
            action.area == wxWinUIInputArea::Client
                ? wxWinUINativeArea::Client
                : wxWinUINativeArea::NonClient;
        m_undeliveredNativeHoverEnter = action;
        m_hasUndeliveredNativeHoverEnter = true;
        return true;
    }

    if ( action.action != wxWinUIInputActionKind::Dispatch )
        return true;

    wxWinUINativeHit refreshedHit;
    const wxWinUINativeHit *verifiedHit = nullptr;
    if ( currentHit &&
         wxWinUIGetInputTargetKey(currentHit->GetTarget()) ==
            action.target &&
         ((currentHit->GetArea() == wxWinUINativeArea::Client) ==
            (action.area == wxWinUIInputArea::Client)) &&
         currentHit->GetHitTest() == action.zone )
    {
        // Hover-leave and cursor callbacks above are re-entrant application
        // boundaries. Prove the same geometric hit again before delivery.
        if ( action.kind == wxWinUIInputKind::Move )
        {
            const long long mirrorStarted = wxWinUIProfileNow();
            const bool mirrored = MirrorNativeCursor(
                     targetIdentity, action.zone, expectedEpoch);
            wxWinUIProfileAdd(wxWinUIProfilePhase::Mirror,
                              mirrorStarted);
            if ( !mirrored || m_shuttingDown )
                return failUndeliverableRelease();
        }

        const long long refreshStarted = wxWinUIProfileNow();
        const bool refreshed = wxWinUIRefreshNativeHit(
            m_tlw, *currentHit, m_bridge, m_inner, &refreshedHit,
            m_hitLayoutGeneration == m_structureGeneration);
        wxWinUIProfileAdd(wxWinUIProfilePhase::Refresh, refreshStarted);
        if ( m_inputTransitionEpoch != expectedEpoch )
            return failUndeliverableRelease();
        if ( !refreshed )
        {
            // The refresh can fail after cursor mirroring ran application
            // code.  Do not emit Leave here: no matching HoverLeave action
            // has been committed, so doing so would leave m_inputState
            // hovering this target and make the next authoritative sample
            // emit a duplicate Leave.  Keep the native and semantic hover
            // prefixes aligned and let that next sample converge them.
            if ( !m_shuttingDown )
                ResetRootCursor();
            return failUndeliverableRelease();
        }

        verifiedHit = &refreshedHit;
        targetIdentity = refreshedHit.GetTarget();
    }

    if ( !targetIdentity.IsValid() )
        return failUndeliverableRelease();

    const HWND target =
        reinterpret_cast<HWND>(targetIdentity.GetLeafHwnd());
    POINT screen = { action.screenX, action.screenY };
    POINT client = screen;
    ::SetLastError(ERROR_SUCCESS);
    if ( !::MapWindowPoints(HWND_DESKTOP, target, &client, 1) &&
         ::GetLastError() != ERROR_SUCCESS )
    {
        return failUndeliverableRelease();
    }

    const LPARAM lpClient =
        MAKELPARAM(static_cast<short>(client.x),
                   static_cast<short>(client.y));
    const LPARAM lpScreen =
        MAKELPARAM(static_cast<short>(screen.x),
                   static_cast<short>(screen.y));
    const WORD keys = static_cast<WORD>(
        action.modifiers | action.buttonMask);

    if ( action.kind == wxWinUIInputKind::Move )
    {
        MarkNativeHoverDispatchStarted(action);
        if ( action.area == wxWinUIInputArea::Client )
        {
            const wxWinUIProfileScope profileSend(
                wxWinUIProfilePhase::Send);
            const wxWinUISyntheticMouseDispatch syntheticDispatch;
            wxWinUISendForwardedPointerMessage(
                hostState, action, expectedEpoch,
                target, WM_MOUSEMOVE, keys, lpClient);
            LetPointerTargetPaint(target);
        }
        else
        {
            wxWinUISendForwardedPointerMessage(
                hostState, action, expectedEpoch, target, WM_NCMOUSEMOVE,
                static_cast<WPARAM>(action.zone), lpScreen);
        }
        return !m_shuttingDown &&
               m_inputTransitionEpoch == expectedEpoch;
    }

    if ( action.kind == wxWinUIInputKind::Wheel )
    {
        if ( action.wheelDelta )
        {
            wxWinUISendForwardedPointerMessage(
                hostState, action, expectedEpoch, target,
                action.horizontalWheel ? WM_MOUSEHWHEEL : WM_MOUSEWHEEL,
                MAKEWPARAM(keys,
                    static_cast<WORD>(
                        static_cast<short>(action.wheelDelta))),
                lpScreen);
        }
        return !m_shuttingDown &&
               m_inputTransitionEpoch == expectedEpoch;
    }

    wxWinUIButtonMessages messages;
    if ( !wxWinUIGetButtonMessages(action.button, action.kind, &messages) )
    {
        return failUndeliverableRelease();
    }

    const bool isDown =
        action.kind == wxWinUIInputKind::Press ||
        action.kind == wxWinUIInputKind::DoubleClick;
    const UINT message =
        action.area == wxWinUIInputArea::Client
            ? messages.client
            : messages.nonClient;
    const WPARAM buttonParams =
        action.area == wxWinUIInputArea::Client
            ? (messages.xButton
                ? MAKEWPARAM(keys, messages.xButton)
                : static_cast<WPARAM>(keys))
            : (messages.xButton
                ? MAKEWPARAM(static_cast<WORD>(action.zone),
                             messages.xButton)
                : static_cast<WPARAM>(action.zone));

    if ( wxWinUIInputLogEnabled() )
    {
        wxWinUIInputLog("fwd %s target=%p ht=%d %s",
                        isDown ? "DOWN" : "UP",
                        static_cast<void *>(target),
                        action.zone,
                        action.area == wxWinUIInputArea::Client
                            ? "client" : "nc");
    }

    // DefWindowProc runs these hit zones inside a system-modal loop. Queue
    // their DOWN outside the XAML callback and retire the bridge gesture
    // first; all other messages remain synchronous.
    if ( isDown &&
         action.button == wxWinUIInputButton::Left &&
         action.area == wxWinUIInputArea::NonClient &&
         wxWinUIStartsSystemModalLoop(action.zone) )
    {
        bool mayQueue =
            verifiedHit &&
            verifiedHit->GetTarget().Matches(targetIdentity);
        const HWND capture = ::GetCapture();
        const bool islandOwnsCapture =
            !capture ||
            capture == m_bridge ||
            capture == m_inner ||
            (m_bridge && ::IsChild(m_bridge, capture));

        if ( !islandOwnsCapture )
        {
            mayQueue = false;
            wxWinUIInputLog(
                "modal-ht %d: foreign capture %p; forward suppressed",
                action.zone, static_cast<void *>(capture));
        }
        else if ( capture )
        {
            NotifyNativeCaptureMutation();
            if ( !::ReleaseCapture() )
            {
                mayQueue = false;
                wxLogDebug("wxWinUI: failed to release island pointer "
                             "capture from %p",
                             static_cast<void *>(capture));
            }
        }

        if ( !hostIsLive() ||
             m_inputTransitionEpoch != expectedEpoch )
        {
            return failUndeliverableRelease();
        }

        wxWinUINativeHit afterCapture;
        if ( mayQueue )
        {
            const bool refreshed = wxWinUIRefreshNativeHit(
                m_tlw, *verifiedHit, m_bridge, m_inner, &afterCapture);
            if ( m_inputTransitionEpoch != expectedEpoch )
                return failUndeliverableRelease();
            if ( !refreshed || ::GetCapture() )
                mayQueue = false;
        }

        // The modal DOWN remains prepared while it is queued and is committed
        // only immediately before the eventual SendMessage. Reset the XAML
        // input site first, then revalidate before publishing the deferred
        // request; the dispatcher will either retire the gesture consumed by
        // the system loop or preserve it for a real routed UP.
        const std::uint32_t resetGeneration =
            m_inputSiteGeneration;
        bool resetTransferredToPending = false;
        const auto finishUnqueuedReset = wxMakeGuard(
            [this, &hostIsLive, &resetTransferredToPending,
             resetGeneration]()
            {
                if ( !resetTransferredToPending && hostIsLive() )
                    PostIslandCancelMode(resetGeneration);
            });

        if ( hostIsLive() )
            CancelIslandPointerState(false);

        if ( !hostIsLive() ||
             m_inputTransitionEpoch != expectedEpoch )
        {
            return failUndeliverableRelease();
        }

        if ( mayQueue )
        {
            wxWinUINativeHit finalHit;
            const bool refreshed = wxWinUIRefreshNativeHit(
                m_tlw, afterCapture, m_bridge, m_inner, &finalHit);
            if ( m_inputTransitionEpoch != expectedEpoch ||
                 !refreshed || ::GetCapture() )
            {
                mayQueue = false;
            }
            else
            {
                mayQueue = QueueModalNativeDispatch(
                    finalHit, action, message, messages.virtualKey,
                    messages.xButton, action.device, action.pointerId);
                resetTransferredToPending = mayQueue;
            }
        }

        if ( !mayQueue )
        {
            if ( AbortUndeliveredNativeHover(action) &&
                 !m_shuttingDown )
            {
                ResetRootCursor();
            }
            wxLogDebug("wxWinUI: suppressed unsafe queued non-client "
                         "input");
        }
        return mayQueue && !m_shuttingDown;
    }

    // This is the transaction's commit point. All HWND/generation, geometry,
    // cursor and modal-loop preflights above are complete, and SendMessage is
    // the very next callback-bearing operation.
    if ( !hostIsLive() ||
         m_inputTransitionEpoch != expectedEpoch ||
         !m_inputState.CommitBeforeDispatch(action) )
    {
        return failUndeliverableRelease();
    }
    MarkNativeHoverDispatchStarted(action);

    const ULONGLONG started = ::GetTickCount64();
    if ( action.area == wxWinUIInputArea::Client )
    {
        const wxWinUISyntheticMouseDispatch syntheticDispatch;
        wxWinUISendForwardedPointerMessage(
            hostState, action, expectedEpoch,
            target, message, buttonParams, lpClient);
    }
    else
    {
        wxWinUISendForwardedPointerMessage(
            hostState, action, expectedEpoch,
            target, message, buttonParams, lpScreen);
    }
    if ( m_shuttingDown )
        return false;
    const wxWinUIPointerSample captureSample = makeCaptureSample();
    const bool captureSynchronized = SyncNativeCapture(captureSample);
    if ( !hostIsLive() )
        return false;
    const bool outerTransitionIsCurrent =
        m_inputTransitionEpoch == expectedEpoch;

    // A synchronous native tracking loop may consume the matching physical
    // release before returning. It then cannot arrive through XAML, so clear
    // the semantic press under the same bounded evidence used by the legacy
    // implementation. Do this before abandoning a re-entered outer
    // transition: MOVE/ENTER/EXIT can advance the epoch without retiring this
    // exact gesture. FinishSynchronousDown() is serial-checked, so a nested UP
    // or replacement DOWN makes this a harmless no-op.
    if ( isDown )
    {
        const ULONGLONG elapsed = ::GetTickCount64() - started;
        const bool contactActive = wxWinUIIsPointerContactActive(
            action.device, action.pointerId, messages.virtualKey);
        const wxWinUIInputTransition cancelTransition =
            m_inputState.FinishSynchronousDown(
                action, captureSample, elapsed, contactActive);
        if ( !cancelTransition.actions.empty() )
        {
            if ( !ExecuteInputTransition(cancelTransition) )
                return false;
            CancelIslandPointerState();
        }
    }

    return captureSynchronized &&
           outerTransitionIsCurrent &&
           !m_shuttingDown;
}

bool wxWinUITopLevelHost::BuildRootPointerSample(
    const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args,
    wxWinUIInputKind kind,
    wxWinUIPointerSample *sample) const
{
    if ( !sample || !m_root )
        return false;

    *sample = wxWinUIPointerSample();
    sample->kind = kind;
    sample->timestamp = ::GetTickCount64();

    bool havePointerIdentity = false;
    try
    {
        const auto pointer = args.Pointer();
        sample->device =
            wxWinUIGetInputDevice(pointer.PointerDeviceType());
        sample->pointerId = pointer.PointerId();
        havePointerIdentity = true;
    }
    catch ( const winrt::hresult_error& )
    {
    }

    try
    {
        const auto point = args.GetCurrentPoint(m_root);
        const auto props = point.Properties();
        const auto position = point.Position();
        const double scale = GetScale();
        const RECT clientScreen = GetClientScreenRect();

        sample->device =
            wxWinUIGetInputDevice(point.PointerDeviceType());
        sample->pointerId = point.PointerId();
        sample->isPrimary = props.IsPrimary();
        sample->isCompatibilityMouse =
            sample->device == wxWinUIInputDevice::Mouse &&
            wxWinUIIsCompatibilityMouseEvent();
        sample->hasRootDips = true;
        sample->rootXDIP = position.X;
        sample->rootYDIP = position.Y;
        sample->screenX =
            clientScreen.left +
            static_cast<int>(std::lround(position.X * scale));
        sample->screenY =
            clientScreen.top +
            static_cast<int>(std::lround(position.Y * scale));
        sample->timestamp = point.Timestamp() / 1000;

        if ( ::GetKeyState(VK_SHIFT) & 0x8000 )
            sample->modifiers |= MK_SHIFT;
        if ( ::GetKeyState(VK_CONTROL) & 0x8000 )
            sample->modifiers |= MK_CONTROL;
        if ( props.IsLeftButtonPressed() )
            sample->buttonMask |= MK_LBUTTON;
        if ( props.IsRightButtonPressed() )
            sample->buttonMask |= MK_RBUTTON;
        if ( props.IsMiddleButtonPressed() )
            sample->buttonMask |= MK_MBUTTON;
        if ( props.IsXButton1Pressed() )
            sample->buttonMask |= MK_XBUTTON1;
        if ( props.IsXButton2Pressed() )
            sample->buttonMask |= MK_XBUTTON2;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        if ( !havePointerIdentity ||
             (kind != wxWinUIInputKind::Cancel &&
              kind != wxWinUIInputKind::CaptureLost) )
        {
            return false;
        }

        // PointerCanceled/CaptureLost may no longer have a visual point.
        // Their stable pointer identity is sufficient for scoped cleanup.
        sample->isPrimary = true;
        sample->isCompatibilityMouse = false;
        POINT screen = { 0, 0 };
        if ( ::GetCursorPos(&screen) )
        {
            sample->screenX = screen.x;
            sample->screenY = screen.y;
        }
        if ( ::GetKeyState(VK_SHIFT) & 0x8000 )
            sample->modifiers |= MK_SHIFT;
        if ( ::GetKeyState(VK_CONTROL) & 0x8000 )
            sample->modifiers |= MK_CONTROL;
        return true;
    }
}

bool wxWinUITopLevelHost::ClassifyRootPointer(
    const wxPoint& pointClientPx,
    const POINT& pointScreen,
    const winrt::Windows::Foundation::Point *exactRootDips,
    bool sourceAlreadyHandled,
    wxWinUINativeHit *nativeHit,
    wxWinUIRouteObservation *observation,
    bool *hasNativeHit)
{
    if ( !nativeHit || !observation || !hasNativeHit ||
         m_shuttingDown || !m_tlw )
    {
        return false;
    }

    *nativeHit = wxWinUINativeHit();
    *observation = wxWinUIRouteObservation();
    *hasNativeHit = false;

    // handledEventsToo lets the root observe events consumed by a
    // Button/TextBox/etc. They remain XAML-owned even if the pointer is over
    // a transparent/native region. Native capture, applied by the state
    // machine after this geometric classification, is the sole override.
    if ( sourceAlreadyHandled )
    {
        observation->surface = wxWinUIInputSurface::Xaml;
        return true;
    }

    const long long xamlStarted = wxWinUIProfileNow();
    wxWindow *xamlWindow = nullptr;
    const XamlHitResolution xamlHit =
        PointOverXamlContent(pointClientPx, exactRootDips,
                             wxWinUIInputNodesEnabled() ? &xamlWindow
                                                        : nullptr);
    wxWinUIProfileAdd(wxWinUIProfilePhase::XamlHit, xamlStarted);
    if ( m_shuttingDown )
        return false;

    if ( xamlHit == XamlHitResolution::Hit )
    {
        observation->surface = wxWinUIInputSurface::Xaml;
        return true;
    }
    if ( xamlHit == XamlHitResolution::Unavailable )
    {
        observation->surface = wxWinUIInputSurface::Indeterminate;
        return true;
    }

    const long long nativeStarted = wxWinUIProfileNow();
    // The window whose canvas is topmost under the pointer is where the
    // target is looked for; the top-level window itself, or no canvas found,
    // leaves the search to the walk through the windows.
    const bool fromXaml = xamlWindow && xamlWindow != m_tlw &&
                          !xamlWindow->IsBeingDeleted();
    const wxWinUIHitResolution nativeResolution = fromXaml
        ? wxWinUIResolveNativeHitFrom(
            m_tlw, xamlWindow->GetHWND(), pointScreen, m_bridge, m_inner,
            nativeHit)
        : wxWinUIResolveNativeHitReusing(
            m_tlw, pointScreen, m_bridge, m_inner,
            m_lastResolvedHit,
            m_lastResolvedLayoutGeneration == m_structureGeneration &&
                !wxWinUIOptimisationDisabled("hittest"),
            nativeHit);
    wxWinUIProfileAdd(wxWinUIProfilePhase::NativeHit, nativeStarted);
    m_hitLayoutGeneration = m_structureGeneration;
    if ( nativeResolution == wxWinUIHitResolution::Hit )
    {
        m_lastResolvedHit = *nativeHit;
        m_lastResolvedLayoutGeneration = m_structureGeneration;
    }
    else
    {
        m_lastResolvedHit = wxWinUINativeHit();
    }
    if ( nativeResolution == wxWinUIHitResolution::Unstable )
    {
        observation->surface = wxWinUIInputSurface::Indeterminate;
        return true;
    }
    if ( nativeResolution == wxWinUIHitResolution::Miss )
    {
        observation->surface = wxWinUIInputSurface::Outside;
        return true;
    }

    if ( !fromXaml && wxWinUIInputNodesEnabled() && wxWinUIInputLogEnabled() )
    {
        // No canvas named a window and the walk found one which is not the
        // top-level window: what keeps the walk from being removed.
        wxWindow * const walked = nativeHit->GetTarget().GetWindow();
        if ( walked && walked != m_tlw && walked != m_inputWalkLogged )
        {
            m_inputWalkLogged = walked;
            wxWinUIInputLog(
                "route WALK found %p %s which no canvas named, pt=(%ld,%ld)",
                static_cast<void *>(walked),
                wxString(walked->GetClassInfo()->GetClassName())
                    .utf8_str().data(),
                pointScreen.x, pointScreen.y);
        }
    }

    if ( wxWinUIInputNodesEnabled() )
    {
        // A window which was never moved since it was created has no canvas
        // yet: the next search starts from it.
        wxWindow * const found = nativeHit->GetTarget().GetWindow();
        if ( found && found != m_tlw && !FindSlot(found) &&
             !m_inputNodes.count(found) )
        {
            EnsureInputNodes(found);
            if ( m_shuttingDown )
                return false;
        }

        if ( wxWinUIInputNodesCompared() )
        {
            // What the search started from the window XAML names found,
            // against the walk through the whole tree.
            wxWinUINativeHit walked;
            const wxWinUIHitResolution walkedResolution =
                wxWinUIResolveNativeHit(m_tlw, pointScreen, m_bridge,
                                        m_inner, &walked);
            if ( m_shuttingDown )
                return false;
            wxWindow * const walkedWindow =
                walkedResolution == wxWinUIHitResolution::Hit
                    ? walked.GetTarget().GetWindow() : nullptr;
            const bool same =
                walkedResolution == wxWinUIHitResolution::Hit &&
                walked.GetTarget().GetLeafHwnd() ==
                    nativeHit->GetTarget().GetLeafHwnd() &&
                walked.GetHitTest() == nativeHit->GetHitTest();
            if ( !same &&
                 (m_inputCompareNative != walkedWindow ||
                  m_inputCompareXaml != found) )
            {
                m_inputCompareNative = walkedWindow;
                m_inputCompareXaml = found;
                const wxString walkedClass = walkedWindow
                    ? wxString(walkedWindow->GetClassInfo()->GetClassName())
                    : wxString("(none)");
                const wxString foundClass = found
                    ? wxString(found->GetClassInfo()->GetClassName())
                    : wxString("(none)");
                const wxString startClass = xamlWindow
                    ? wxString(xamlWindow->GetClassInfo()->GetClassName())
                    : wxString("(none)");
                wxWinUIInputLog(
                    "nodes DISAGREE walk=%p %s ht=%d from-xaml=%p %s ht=%d "
                    "start=%s pt=(%ld,%ld)",
                    static_cast<void *>(walkedWindow),
                    walkedClass.utf8_str().data(),
                    static_cast<int>(walked.GetHitTest()),
                    static_cast<void *>(found),
                    foundClass.utf8_str().data(),
                    static_cast<int>(nativeHit->GetHitTest()),
                    startClass.utf8_str().data(),
                    pointScreen.x, pointScreen.y);
            }
        }
    }

    *hasNativeHit = true;
    observation->surface = wxWinUIInputSurface::Native;
    observation->target =
        wxWinUIGetInputTargetKey(nativeHit->GetTarget());
    observation->area =
        nativeHit->GetArea() == wxWinUINativeArea::Client
            ? wxWinUIInputArea::Client
            : wxWinUIInputArea::NonClient;
    observation->zone =
        static_cast<int>(nativeHit->GetHitTest());
    observation->clientDoubleClicks =
        (::GetClassLongPtr(
            reinterpret_cast<HWND>(
                nativeHit->GetTarget().GetLeafHwnd()),
            GCL_STYLE) & CS_DBLCLKS) != 0;
    return true;
}

void wxWinUITopLevelHost::OnRootPointerBoundary(
    bool entered,
    const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args)
{
    OperationGuard operation(this);
    if ( m_shuttingDown || !m_tlw )
        return;

    const wxWinUIRootPointerNesting nesting;
    if ( wxWinUIRootPointerNesting::TooDeep() )
        return;

    try
    {
        const auto source =
            args.OriginalSource().try_as<
                winrt::Microsoft::UI::Xaml::UIElement>();
        if ( !source || source != m_root )
            return;
    }
    catch ( const winrt::hresult_error& )
    {
        // A descendant boundary is more common than a failed root query.
        // Without positive root ownership, fail closed instead of retiring a
        // native hover because a child XAML element was crossed.
        return;
    }

    wxWinUIPointerSample sample;
    if ( !BuildRootPointerSample(
             args,
             entered ? wxWinUIInputKind::Enter
                     : wxWinUIInputKind::Leave,
             &sample) ||
         (sample.device != wxWinUIInputDevice::Mouse &&
          !sample.isPrimary) ||
         sample.isCompatibilityMouse )
    {
        return;
    }

    // The pointer of a gesture held by a XAML control may leave the island
    // and come back: the control keeps tracking it through its capture. This
    // is not a boundary the router has to act on, and acting on it could
    // cancel that capture. See m_xamlGestureActive.
    if ( m_xamlGestureActive && sample.pointerId == m_xamlGesturePointerId &&
         sample.buttonMask )
    {
        return;
    }

    // What waits in the queue is older than this crossing. It goes first --
    // unless this arrives from inside one of its deliveries, where only
    // the moves, which the crossing makes pointless, are dropped: a move
    // delivered after the pointer has left would leave its window hovered.
    if ( !m_rootPointerQueue.empty() )
    {
        if ( m_rootPointerDelivering )
            DropQueuedRootPointerMoves();
        else
            DrainRootPointerQueue();
        if ( m_shuttingDown || !m_tlw )
            return;
    }

    // Descendant boundaries and unrouteable/duplicate pointer samples are
    // deliberately ignored. Publish a new epoch only after proving this is
    // an authoritative root boundary; otherwise an ignored XAML callback
    // could supersede a valid native transition already in flight.
    if ( ++m_inputTransitionEpoch == 0 )
        ++m_inputTransitionEpoch;
    const unsigned long long eventEpoch = m_inputTransitionEpoch;
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;

    (void)RouteRootPointerBoundaryAtEpoch(
        entered, sample, eventEpoch);
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIHostInputTestAccess::RouteRootPointerBoundary(
    wxWinUITopLevelHost& host,
    bool entered,
    const wxWinUIPointerSample& originalSample)
{
    wxWinUITopLevelHost * const self = &host;
    wxWinUITopLevelHost::OperationGuard operation(self);
    if ( self->m_shuttingDown || !self->m_tlw )
        return false;

    wxWinUIPointerSample sample = originalSample;
    sample.kind = entered
        ? wxWinUIInputKind::Enter
        : wxWinUIInputKind::Leave;
    if ( (sample.device != wxWinUIInputDevice::Mouse &&
          !sample.isPrimary) ||
         sample.isCompatibilityMouse )
    {
        return false;
    }

    if ( ++self->m_inputTransitionEpoch == 0 )
        ++self->m_inputTransitionEpoch;
    return self->RouteRootPointerBoundaryAtEpoch(
        entered, sample, self->m_inputTransitionEpoch);
}
#endif // WXWINUI_TEST_SUPPORT

bool wxWinUITopLevelHost::RouteRootPointerBoundaryAtEpoch(
    bool entered,
    const wxWinUIPointerSample& sample,
    unsigned long long eventEpoch)
{
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;

    if ( m_nativeInputDispatchInFlight &&
         m_nativeInputDispatchCommitted &&
         m_inFlightNativeInput.device == sample.device &&
         m_inFlightNativeInput.pointerId == sample.pointerId )
    {
        // System-modal SendMessage pumps routed boundary events. Preserve the
        // latest boundary state on the published in-flight identity so its
        // return path cannot arm an already-outside release wait.
        m_inFlightNativeInput.pointerInsideRoot = entered;
    }

    if ( !entered )
    {
        CancelPendingNativeDispatch(sample);
        CancelSlotPointerOwners(sample, false);
        if ( !hostState || hostState->GetHost() != this ||
             m_shuttingDown || m_inputTransitionEpoch != eventEpoch )
        {
            return false;
        }

        std::array<wxWinUIInputAction,
                   ModalAwaitingReleaseCapacity> modalOwners;
        std::size_t modalOwnerCount = 0;
        for ( wxWinUIInputAction& current : m_modalAwaitingReleases )
        {
            if ( current.gestureSerial &&
                 current.device == sample.device &&
                 current.pointerId == sample.pointerId )
            {
                modalOwners[modalOwnerCount++] = current;
                // Retire the broker entry before callback-bearing
                // cancellation. A nested modal DOWN then gets a fresh slot
                // and cannot be cleared by this outer boundary snapshot.
                current = wxWinUIInputAction();
            }
        }

        // No XAML capture remains for modal DOWNs. Once their pointer leaves
        // the island, a physical UP outside is not guaranteed to return
        // through the root router. Retire every exact button serial in the
        // semantic state before invoking the first callback; the combined
        // executor will emergency-acknowledge any remaining CancelPress
        // tombstones if the first WM_CANCELMODE re-enters.
        wxWinUIInputTransition captureCleanup;
        wxWinUIInputTransition pressCancellations;
        for ( std::size_t i = 0; i < modalOwnerCount; ++i )
        {
            const wxWinUIInputAction& modalOwner = modalOwners[i];
            if ( m_inputState.HasActiveGesture(modalOwner) )
            {
                wxWinUIPointerSample cancel = sample;
                cancel.kind = wxWinUIInputKind::Cancel;
                cancel.button = modalOwner.button;
                cancel.buttonMask &=
                    ~wxWinUIGetWParamButtonMask(modalOwner.button);
                const wxWinUIInputTransition cancellation =
                    m_inputState.CancelActiveGesture(modalOwner, cancel);
                for ( const wxWinUIInputAction& action :
                      cancellation.actions )
                {
                    if ( action.action ==
                            wxWinUIInputActionKind::ReleaseCapture )
                    {
                        captureCleanup.actions.push_back(action);
                    }
                    else
                    {
                        pressCancellations.actions.push_back(action);
                    }
                }
            }
        }

        // Preserve the executor's cleanup invariant: ReleaseCapture is always
        // first. If the first callback re-enters, it has already run; every
        // remaining CancelPress can then be completed independently.
        wxWinUIInputTransition allCancellations;
        for ( const wxWinUIInputAction& action : captureCleanup.actions )
            allCancellations.actions.push_back(action);
        for ( const wxWinUIInputAction& action : pressCancellations.actions )
            allCancellations.actions.push_back(action);

        unsigned long long expectedEpoch = eventEpoch;
        if ( !allCancellations.actions.empty() )
        {
            if ( ++expectedEpoch == 0 )
                ++expectedEpoch;
        }
        if ( !ExecuteInputTransition(allCancellations) ||
             !hostState || hostState->GetHost() != this ||
             m_shuttingDown ||
             m_inputTransitionEpoch != expectedEpoch )
        {
            return false;
        }
        eventEpoch = m_inputTransitionEpoch;
    }

    const wxWinUIInputTransition transition =
        m_inputState.RouteBoundary(sample);
    if ( !ExecuteInputTransition(transition) )
        return false;
    RetryDeferredIslandCancelMode();

    if ( wxWinUIInputLogEnabled() )
    {
        // Sample transition storms without making healthy traces unreadable.
        static unsigned s_count = 0;
        ++s_count;
        if ( (s_count % 64) == 1 )
        {
            POINT pt = { 0, 0 };
            ::GetCursorPos(&pt);
            const HWND under = ::WindowFromPoint(pt);
            wchar_t cls[64] = L"";
            if ( under )
                ::GetClassNameW(under, cls, WXSIZEOF(cls));
            wxWinUIInputLog("root %s #%u under=%p '%S' pt=(%ld,%ld)",
                            entered ? "ENTER" : "EXIT",
                            s_count, static_cast<void *>(under),
                            cls, pt.x, pt.y);
        }
    }
    return true;
}

void wxWinUITopLevelHost::OnRootPointerInterrupted(
    bool captureLost,
    const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args)
{
    OperationGuard operation(this);
    if ( m_shuttingDown || !m_tlw )
        return;

    const wxWinUIRootPointerNesting nesting;
    if ( wxWinUIRootPointerNesting::TooDeep() )
        return;

    // Losing the capture or the contact ends any gesture XAML was holding.
    m_xamlGestureActive = false;

    if ( ++m_inputTransitionEpoch == 0 )
        ++m_inputTransitionEpoch;
    const unsigned long long epoch = m_inputTransitionEpoch;
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });

    wxWinUIPointerSample sample;
    if ( !BuildRootPointerSample(
             args,
             captureLost ? wxWinUIInputKind::CaptureLost
                         : wxWinUIInputKind::Cancel,
             &sample) ||
         (sample.device != wxWinUIInputDevice::Mouse &&
          !sample.isPrimary) ||
         sample.isCompatibilityMouse )
        return;

    // A pointer-specific interruption can bubble from a different XAML
    // descendant than the slot that received DOWN. Retire host-wide slot
    // ownership before filtering the event out of the independent root
    // native state machine.
    CancelSlotPointerOwners(sample, false);
    if ( !hostState || hostState->GetHost() != this ||
         m_shuttingDown || m_inputTransitionEpoch != epoch )
    {
        return;
    }
    RetryDeferredIslandCancelMode();

    bool sourceIsRoot = false;
    bool sourceKnown = false;
    try
    {
        const auto source =
            args.OriginalSource().try_as<
                winrt::Microsoft::UI::Xaml::UIElement>();
        sourceKnown = static_cast<bool>(source);
        sourceIsRoot = source && source == m_root;
    }
    catch ( const winrt::hresult_error& )
    {
    }

    // CaptureLost/Canceled bubble from every hosted XAML slot. Only the
    // island root may terminate root-native state. If source inspection
    // itself failed, require positive ownership of this pointer as fallback.
    if ( sourceKnown && !sourceIsRoot )
    {
        // XAML may rewrite InputPointerSource as its capture is torn down.
        // This is not a new hit-test: preserve the last Move verdict without
        // re-entering wxEVT_SET_CURSOR while retiring the slot gesture.
        ApplyXamlPointerCursor(args, false, epoch);
        return;
    }
    if ( !sourceKnown &&
         !m_inputState.HasActivePress(
             sample.device, sample.pointerId) &&
         !HasPendingNativeDispatch(sample) )
    {
        return;
    }

    CancelPendingNativeDispatch(sample);
    HandleRootPointerInterrupted(captureLost, sample);
    PruneModalAwaitingReleases();
    RetryDeferredIslandCancelMode();
}

void wxWinUITopLevelHost::OnRootNativeInterrupted(bool captureLost)
{
    OperationGuard operation(this);
    if ( m_shuttingDown || !m_tlw )
        return;
    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });

    if ( ++m_inputTransitionEpoch == 0 )
        ++m_inputTransitionEpoch;
    const unsigned long long epoch = m_inputTransitionEpoch;
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;

    wxWinUIPointerSample sample;
    sample.kind = captureLost
        ? wxWinUIInputKind::CaptureLost
        : wxWinUIInputKind::Cancel;
    sample.interruptAll = true;
    sample.timestamp = ::GetTickCount64();
    POINT screen = { 0, 0 };
    if ( ::GetCursorPos(&screen) )
    {
        sample.screenX = screen.x;
        sample.screenY = screen.y;
    }
    CancelPendingNativeDispatch(sample);
    CancelSlotPointerOwners(sample, true);
    if ( !hostState || hostState->GetHost() != this ||
         m_shuttingDown || m_inputTransitionEpoch != epoch )
    {
        return;
    }
    HandleRootPointerInterrupted(captureLost, sample);
    PruneModalAwaitingReleases();
    RetryDeferredIslandCancelMode();
}

void wxWinUITopLevelHost::HandleRootPointerInterrupted(
    bool captureLost,
    const wxWinUIPointerSample& sample)
{
    OperationGuard operation(this);
    if ( m_shuttingDown )
        return;

    bool hasNativeCapture = false;
    bool captureInterruptedPress = false;
    if ( !SyncNativeCapture(sample, &hasNativeCapture, nullptr,
                            &captureInterruptedPress) )
        return;

    if ( captureInterruptedPress )
        m_inputState.ArmStormGuard(16, 3, 4);

    // PointerCaptureLost is the normal XAML-side consequence of a native
    // target taking USER32 capture during the forwarded DOWN. The explicit
    // capture observation above is authoritative; do not cancel that healthy
    // transfer or arm the emergency breaker.
    if ( captureLost && hasNativeCapture )
    {
        if ( wxWinUIInputLogEnabled() )
            wxWinUIInputLog("root CAPTURE-TRANSFER");
        return;
    }

    wxWinUIRouteObservation observation;
    observation.surface = wxWinUIInputSurface::Indeterminate;
    const wxWinUIInputTransition transition =
        m_inputState.Route(sample, observation);

    // Only a real unfinished gesture is evidence for the bounded guard.
    // Idle capture notifications leave ordinary input completely untouched.
    if ( !captureInterruptedPress &&
         (transition.HasAction(wxWinUIInputActionKind::CancelPress) ||
         transition.HasAction(wxWinUIInputActionKind::ReleaseCapture) )
       )
    {
        m_inputState.ArmStormGuard(16, 3, 4);
    }

    if ( !ExecuteInputTransition(transition) )
        return;

    if ( wxWinUIInputLogEnabled() )
        wxWinUIInputLog("root %s",
                        captureLost ? "CAPTURE-LOST"
                                    : "POINTER-CANCELED");
}

bool wxWinUITopLevelHost::QueueModalNativeDispatch(
    const wxWinUINativeHit& hit,
    const wxWinUIInputAction& action,
    UINT message,
    int virtualKey,
    WORD xButton,
    wxWinUIInputDevice device,
    std::uint32_t pointerId)
{
    if ( m_shuttingDown || m_nativeResize || !m_hostLifetime || !wxTheApp ||
         !hit.IsValid() ||
         hit.GetArea() != wxWinUINativeArea::NonClient ||
         !wxWinUIStartsSystemModalLoop(hit.GetHitTest()) )
    {
        return false;
    }

    PendingNativeDispatch pending;
    pending.hit = hit;
    pending.action = action;
    pending.message = message;
    pending.virtualKey = virtualKey;
    pending.xButton = xButton;
    pending.device = device;
    pending.pointerId = pointerId;
    if ( ++m_nextNativeInputSequence == 0 )
        ++m_nextNativeInputSequence;
    pending.sequence = m_nextNativeInputSequence;
    pending.cancellationGeneration =
        m_nativeInputCancellationGeneration;
    pending.inputSiteGeneration = m_inputSiteGeneration;
    m_pendingNativeInput.push_back(pending);

    if ( m_nativeInputDispatchScheduled )
        return true;

    m_nativeInputDispatchScheduled = true;
    const std::weak_ptr<wxWinUIHostLifetime> weakState = m_hostLifetime;
    wxTheApp->CallAfter(
        [weakState]()
        {
            const std::shared_ptr<wxWinUIHostLifetime> state =
                weakState.lock();
            wxWinUITopLevelHost * const host =
                state ? state->GetHost() : nullptr;
            if ( host )
                host->DispatchPendingNativeInput();
        });
    return true;
}

void wxWinUITopLevelHost::CancelPendingNativeDispatch(
    const wxWinUIPointerSample& sample)
{
    CancelNativeResize(sample);
    const bool cancelsPointer =
        sample.kind == wxWinUIInputKind::Press ||
        sample.kind == wxWinUIInputKind::Release ||
        sample.kind == wxWinUIInputKind::Cancel ||
        sample.kind == wxWinUIInputKind::CaptureLost ||
        sample.kind == wxWinUIInputKind::Leave;
    if ( !cancelsPointer )
        return;

    const auto matches =
        [&sample](const PendingNativeDispatch& pending)
        {
            const bool samePointer =
                sample.interruptAll ||
                (pending.device == sample.device &&
                 pending.pointerId == sample.pointerId);
            const bool sameButton =
                (sample.kind != wxWinUIInputKind::Press &&
                 sample.kind != wxWinUIInputKind::Release) ||
                sample.button == pending.action.button;
            return samePointer && sameButton;
        };

    bool canceled = false;
    std::vector<std::uint32_t> canceledResetGenerations;
    for ( auto it = m_pendingNativeInput.begin();
          it != m_pendingNativeInput.end(); )
    {
        if ( matches(*it) )
        {
            wxWinUIInputLog("modal dispatch #%llu canceled by kind=%d",
                            it->sequence,
                            static_cast<int>(sample.kind));
            canceledResetGenerations.push_back(
                it->inputSiteGeneration);
            it = m_pendingNativeInput.erase(it);
            canceled = true;
        }
        else
        {
            ++it;
        }
    }

    // The deque entry is popped before callback-bearing target refresh. Keep
    // the in-flight identity cancelable until its DOWN is committed. Once the
    // system loop is running, Leave alone is not termination; Release/Cancel
    // and a superseding Press remain authoritative and will update the exact
    // serial in inputstate.
    if ( m_nativeInputDispatchInFlight &&
         matches(m_inFlightNativeInput) &&
         (!m_nativeInputDispatchCommitted ||
          sample.kind != wxWinUIInputKind::Leave) )
    {
        canceled = true;
    }

    if ( canceled )
    {
        if ( ++m_nativeInputCancellationGeneration == 0 )
            ++m_nativeInputCancellationGeneration;
    }

    for ( const std::uint32_t generation :
          canceledResetGenerations )
    {
        PostIslandCancelMode(generation);
    }
}

bool wxWinUITopLevelHost::HasPendingNativeDispatch(
    const wxWinUIPointerSample& sample) const
{
    if ( m_nativeResize &&
         (sample.interruptAll ||
          (m_nativeResize->sample.device == sample.device &&
           m_nativeResize->sample.pointerId == sample.pointerId)) &&
         (sample.button == wxWinUIInputButton::None ||
          sample.button == wxWinUIInputButton::Left) )
    {
        return true;
    }
    const auto matches =
        [&sample](const PendingNativeDispatch& pending)
        {
            return (sample.interruptAll ||
                    (pending.device == sample.device &&
                     pending.pointerId == sample.pointerId)) &&
                   (sample.button == wxWinUIInputButton::None ||
                    pending.action.button == sample.button);
        };

    if ( m_nativeInputDispatchInFlight &&
         matches(m_inFlightNativeInput) )
    {
        return true;
    }

    return std::any_of(
        m_pendingNativeInput.begin(), m_pendingNativeInput.end(),
        matches);
}

bool wxWinUITopLevelHost::RememberModalAwaitingRelease(
    const wxWinUIInputAction& action)
{
    if ( !action.gestureSerial ||
         action.action != wxWinUIInputActionKind::Dispatch ||
         (action.kind != wxWinUIInputKind::Press &&
          action.kind != wxWinUIInputKind::DoubleClick) )
    {
        return false;
    }

    wxWinUIInputAction *freeEntry = nullptr;
    for ( wxWinUIInputAction& current : m_modalAwaitingReleases )
    {
        if ( !current.gestureSerial )
        {
            if ( !freeEntry )
                freeEntry = &current;
            continue;
        }

        if ( current.device == action.device &&
             current.pointerId == action.pointerId &&
             current.button == action.button )
        {
            if ( current.gestureSerial == action.gestureSerial )
            {
                current = action;
                return true;
            }

            // A newer DOWN for this exact physical button must explicitly
            // retire the old serial before it can replace this entry.
            return false;
        }
    }

    if ( !freeEntry )
        return false;

    *freeEntry = action;
    return true;
}

wxWinUIInputAction wxWinUITopLevelHost::TakeModalAwaitingRelease(
    wxWinUIInputDevice device,
    std::uint32_t pointerId,
    wxWinUIInputButton button)
{
    for ( wxWinUIInputAction& current : m_modalAwaitingReleases )
    {
        if ( current.gestureSerial &&
             current.device == device &&
             current.pointerId == pointerId &&
             current.button == button )
        {
            const wxWinUIInputAction result = current;
            current = wxWinUIInputAction();
            return result;
        }
    }
    return wxWinUIInputAction();
}

void wxWinUITopLevelHost::ForgetModalAwaitingRelease(
    std::uint64_t gestureSerial)
{
    if ( !gestureSerial )
        return;

    for ( wxWinUIInputAction& current : m_modalAwaitingReleases )
    {
        if ( current.gestureSerial == gestureSerial )
            current = wxWinUIInputAction();
    }
}

void wxWinUITopLevelHost::PruneModalAwaitingReleases()
{
    for ( wxWinUIInputAction& current : m_modalAwaitingReleases )
    {
        if ( current.gestureSerial &&
             !m_inputState.HasActiveGesture(current) )
        {
            current = wxWinUIInputAction();
        }
    }
}

#ifdef WXWINUI_TEST_SUPPORT
std::size_t wxWinUIHostInputTestAccess::GetModalAwaitingReleaseCount(
    const wxWinUITopLevelHost& host)
{
    const wxWinUITopLevelHost * const self = &host;
    return static_cast<std::size_t>(std::count_if(
        self->m_modalAwaitingReleases.begin(), self->m_modalAwaitingReleases.end(),
        [](const wxWinUIInputAction& action)
        {
            return action.gestureSerial != 0;
        }));
}
#endif // WXWINUI_TEST_SUPPORT

void wxWinUITopLevelHost::DispatchPendingNativeInput()
{
    OperationGuard operation(this);
    m_nativeInputDispatchScheduled = false;

    // A system-modal SendMessage pumps the dispatcher. A CallAfter posted by
    // nested input may therefore re-enter this method; the outer dispatcher
    // owns the single in-flight identity and will drain the newly queued
    // request when the native loop returns.
    if ( m_nativeInputDispatchInFlight )
        return;

    while ( !m_shuttingDown && !m_pendingNativeInput.empty() )
    {
        const PendingNativeDispatch pending =
            m_pendingNativeInput.front();
        m_pendingNativeInput.pop_front();
        m_inFlightNativeInput = pending;
        m_nativeInputDispatchInFlight = true;
        m_nativeInputDispatchCommitted = false;
        const auto clearInFlight = wxMakeGuard(
            [this]()
            {
                m_nativeInputDispatchInFlight = false;
                m_nativeInputDispatchCommitted = false;
                m_inFlightNativeInput = PendingNativeDispatch();
                RetryDeferredIslandCancelMode();
            });
        // CancelIslandPointerState(false) transferred one generation-bound
        // reset obligation to every queued request. Fulfil it on every
        // terminal path: pre-commit drop, cancellation, modal return or
        // normal completion.
        const auto finishIslandReset = wxMakeGuard(
            [this, generation = pending.inputSiteGeneration]()
            {
                if ( !m_shuttingDown )
                    PostIslandCancelMode(generation);
            });
        bool nativeDispatchStarted = false;
        const auto abortUnstartedHover = wxMakeGuard(
            [this, &pending, &nativeDispatchStarted]()
            {
                if ( !nativeDispatchStarted &&
                     AbortUndeliveredNativeHover(pending.action) &&
                     !m_shuttingDown )
                {
                    ResetRootCursor();
                }
            });

        // Starting a system-modal loop after the physical press has already
        // ended would leave it waiting for a release that can never arrive.
        if ( !wxWinUIIsPointerContactActive(
                 pending.device, pending.pointerId, pending.virtualKey) )
        {
            wxWinUIInputLog("modal dispatch #%llu dropped: button released",
                            pending.sequence);
            continue;
        }

        const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
        const unsigned long long epoch = m_inputTransitionEpoch;
        wxWinUINativeHit current;
        const bool refreshed = wxWinUIRefreshNativeHit(
            m_tlw, pending.hit, m_bridge, m_inner, &current);
        if ( !hostState || hostState->GetHost() != this ||
             m_shuttingDown ||
             m_inputTransitionEpoch != epoch ||
             pending.cancellationGeneration !=
                m_nativeInputCancellationGeneration ||
             !refreshed ||
             current.GetArea() != wxWinUINativeArea::NonClient ||
             !wxWinUIStartsSystemModalLoop(current.GetHitTest()) )
        {
            wxWinUIInputLog("modal dispatch #%llu dropped: target changed",
                            pending.sequence);
            continue;
        }

        // Do not steal a capture established while this request was queued.
        if ( ::GetCapture() )
        {
            wxWinUIInputLog("modal dispatch #%llu dropped: capture=%p",
                            pending.sequence,
                            static_cast<void *>(::GetCapture()));
            continue;
        }

        const wxWinUINativeTarget targetIdentity = current.GetTarget();
        if ( !targetIdentity.IsValid() )
            continue;

        const HWND target =
            reinterpret_cast<HWND>(targetIdentity.GetLeafHwnd());
        const POINT pendingScreen = pending.hit.GetScreenPoint();
        const LPARAM screen =
            MAKELPARAM(static_cast<short>(pendingScreen.x),
                       static_cast<short>(pendingScreen.y));

        // This SendMessage is deliberately outside the XAML routed-event
        // callback. DefWindowProc may run the complete move/size/caption
        // system-modal loop before it returns.
        const WPARAM hitAndButton =
            pending.xButton
                ? MAKEWPARAM(static_cast<WORD>(current.GetHitTest()),
                             pending.xButton)
                : static_cast<WPARAM>(current.GetHitTest());

        // The queued action itself is the gesture identity. If another input
        // transition changed this button while CallAfter was pending, the
        // two-phase commit rejects the stale DOWN even if a newer physical
        // press currently makes GetAsyncKeyState() look active.
        if ( !m_inputState.CommitBeforeDispatch(pending.action) )
        {
            wxWinUIInputLog("modal dispatch #%llu dropped: stale gesture",
                            pending.sequence);
            continue;
        }
        m_nativeInputDispatchCommitted = true;
        MarkNativeHoverDispatchStarted(pending.action);
        nativeDispatchStarted = true;

        wxWinUISendForwardedPointerMessage(
            hostState, pending.action, m_inputTransitionEpoch,
            target, pending.message,
            hitAndButton, screen);

        if ( m_shuttingDown )
            return;

        // Not every non-client DOWN consumes its matching UP. In particular,
        // a caption double-click can return immediately after maximize while
        // the second contact is still held. Preserve the exact PressState so
        // the real routed UP can balance it, but do not let an aborted modal
        // loop seed a third click.
        if ( wxWinUIIsPointerContactActive(
                 pending.device, pending.pointerId, pending.virtualKey) )
        {
            const bool hasPublishedBoundaryState =
                m_inFlightNativeInput.sequence == pending.sequence;
            const bool leaveObserved =
                hasPublishedBoundaryState &&
                !m_inFlightNativeInput.pointerInsideRoot;
            bool pointerInsideRoot =
                hasPublishedBoundaryState &&
                m_inFlightNativeInput.pointerInsideRoot;
            bool geometryOutsideWithoutLeave = false;
            if ( pointerInsideRoot &&
                 pending.device == wxWinUIInputDevice::Mouse )
            {
                POINT currentPointer = { 0, 0 };
                const RECT rootScreen = GetClientScreenRect();
                pointerInsideRoot =
                    ::GetCursorPos(&currentPointer) &&
                    ::PtInRect(&rootScreen, currentPointer);
                geometryOutsideWithoutLeave =
                    !pointerInsideRoot && !leaveObserved;
            }

            if ( !pointerInsideRoot &&
                 m_inputState.HasActiveGesture(pending.action) )
            {
                wxWinUIPointerSample cancel;
                cancel.device = pending.action.device;
                cancel.kind = wxWinUIInputKind::Cancel;
                cancel.button = pending.action.button;
                cancel.pointerId = pending.action.pointerId;
                cancel.timestamp = ::GetTickCount64();
                cancel.modifiers = pending.action.modifiers;
                cancel.screenX = pending.action.screenX;
                cancel.screenY = pending.action.screenY;
                cancel.buttonMask =
                    pending.action.buttonMask &
                    ~wxWinUIGetWParamButtonMask(
                        pending.action.button);
                const wxWinUIInputTransition cancellation =
                    m_inputState.CancelActiveGesture(
                        pending.action, cancel);
                unsigned long long expectedEpoch =
                    m_inputTransitionEpoch;
                if ( !cancellation.actions.empty() )
                {
                    if ( ++expectedEpoch == 0 )
                        ++expectedEpoch;
                }
                const bool cancellationDelivered =
                    ExecuteInputTransition(cancellation);

                // A routed Leave already closed hover while SendMessage was
                // pumping. Only synthesize the boundary when geometry proves
                // that no such event reached us.
                if ( cancellationDelivered &&
                     geometryOutsideWithoutLeave &&
                     hostState && hostState->GetHost() == this &&
                     !m_shuttingDown &&
                     m_inputTransitionEpoch == expectedEpoch )
                {
                    wxWinUIPointerSample leave = cancel;
                    leave.kind = wxWinUIInputKind::Leave;
                    const wxWinUIInputTransition boundary =
                        m_inputState.RouteBoundary(leave);
                    (void)ExecuteInputTransition(boundary);
                }
            }
            else if ( m_inputState.SuppressClickForGesture(
                          pending.action) )
            {
                if ( !RememberModalAwaitingRelease(pending.action) )
                {
                    // Never overwrite an unrelated held button. The fixed
                    // table is deliberately larger than the V0 device policy,
                    // but if its invariant is ever exceeded, fail closed by
                    // retiring this exact serial instead of stranding it.
                    wxWinUIPointerSample cancel;
                    cancel.device = pending.action.device;
                    cancel.kind = wxWinUIInputKind::Cancel;
                    cancel.button = pending.action.button;
                    cancel.pointerId = pending.action.pointerId;
                    cancel.timestamp = ::GetTickCount64();
                    cancel.screenX = pending.action.screenX;
                    cancel.screenY = pending.action.screenY;
                    cancel.buttonMask =
                        pending.action.buttonMask &
                        ~wxWinUIGetWParamButtonMask(
                            pending.action.button);
                    const wxWinUIInputTransition cancellation =
                        m_inputState.CancelActiveGesture(
                            pending.action, cancel);
                    (void)ExecuteInputTransition(cancellation);
                    wxLogDebug("wxWinUI: modal release table exhausted; "
                                 "canceled gesture #%llu",
                                 pending.action.gestureSerial);
                }
            }
            wxWinUIInputLog(
                "modal dispatch #%llu returned with contact active (%s)",
                pending.sequence,
                pointerInsideRoot ? "inside" : "outside");
            continue;
        }

        // DefWindowProc consumed the physical release inside its system
        // tracking loop. Retire the committed semantic press without sending
        // a second UP. A click is recorded only if the final geometry still
        // proves the same non-client target/zone, enabling the next queued
        // DOWN to become WM_NC*BUTTONDBLCLK.
        wxWinUIPointerSample release;
        release.device = pending.action.device;
        release.kind = wxWinUIInputKind::Release;
        release.button = pending.action.button;
        release.pointerId = pending.action.pointerId;
        release.timestamp = ::GetTickCount64();
        release.modifiers = pending.action.modifiers;
        release.buttonMask =
            pending.action.buttonMask &
            ~wxWinUIGetWParamButtonMask(pending.action.button);

        POINT finalScreen = { 0, 0 };
        if ( ::GetCursorPos(&finalScreen) )
        {
            release.screenX = finalScreen.x;
            release.screenY = finalScreen.y;
        }

        wxWinUIRouteObservation observation;
        wxWinUINativeHit finalHit;
        const wxWinUIHitResolution resolution =
            wxWinUIResolveNativeHit(
                m_tlw, finalScreen, m_bridge, m_inner, &finalHit);
        if ( resolution == wxWinUIHitResolution::Hit )
        {
            observation.surface = wxWinUIInputSurface::Native;
            observation.target =
                wxWinUIGetInputTargetKey(finalHit.GetTarget());
            observation.area =
                finalHit.GetArea() == wxWinUINativeArea::Client
                    ? wxWinUIInputArea::Client
                    : wxWinUIInputArea::NonClient;
            observation.zone = finalHit.GetHitTest();
            observation.clientDoubleClicks =
                (::GetClassLongPtr(
                    reinterpret_cast<HWND>(
                        finalHit.GetTarget().GetLeafHwnd()),
                    GCL_STYLE) & CS_DBLCLKS) != 0;
        }
        else
        {
            observation.surface =
                resolution == wxWinUIHitResolution::Unstable
                    ? wxWinUIInputSurface::Indeterminate
                    : wxWinUIInputSurface::Outside;
        }

        // The modal loop can dispatch a release/cancel and even a complete
        // newer gesture before SendMessage returns. Route the synthetic
        // balancing release only while this exact committed serial still
        // owns the button; Route() itself mutates hover/click state.
        if ( !m_inputState.HasActiveGesture(pending.action) )
        {
            wxWinUIInputLog(
                "modal dispatch #%llu returned after gesture retired",
                pending.sequence);
            continue;
        }

        const wxWinUIInputTransition completion =
            m_inputState.Route(release, observation);
        const wxWinUIInputAction *balancingRelease = nullptr;
        for ( const wxWinUIInputAction& action : completion.actions )
        {
            if ( action.action == wxWinUIInputActionKind::Dispatch &&
                 action.kind == wxWinUIInputKind::Release &&
                 action.gestureSerial ==
                    pending.action.gestureSerial )
            {
                balancingRelease = &action;
                break;
            }
        }

        wxWinUIInputTransition cleanup;
        if ( balancingRelease )
        {
            if ( !m_inputState.CommitBeforeDispatch(*balancingRelease) )
            {
                wxWinUIInputAction cancellation;
                if ( m_inputState.AbortUndeliverableRelease(
                         *balancingRelease, &cancellation) )
                {
                    ExecuteEmergencyInputCleanup(cancellation);
                }
            }

            for ( const wxWinUIInputAction& action : completion.actions )
            {
                if ( action.action != wxWinUIInputActionKind::Dispatch )
                    cleanup.actions.push_back(action);
            }
        }
        if ( !cleanup.actions.empty() )
            (void)ExecuteInputTransition(cleanup);

        if ( !m_inputState.HasActiveGesture(pending.action) )
            ForgetModalAwaitingRelease(pending.action.gestureSerial);

    }
}

void wxWinUITopLevelHost::CancelIslandPointerState(bool postCancelMode)
{
    OperationGuard operation(this);
    if ( m_cancelingIslandPointerState )
        return;

    m_cancelingIslandPointerState = true;
    const auto resetCancelGuard = wxMakeGuard(
        [this]()
        {
            m_cancelingIslandPointerState = false;
        });

    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const auto root = m_root;

    // This function is reserved for a proven interrupted/native-transferred
    // gesture. Only that evidence arms the otherwise inert state guard.
    m_inputState.ArmStormGuard(4096, 3, 4);

    // Release any XAML pointer capture the root grabbed for this gesture...
    if ( root )
    {
        try
        {
            root.ReleasePointerCaptures();
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    // ReleasePointerCaptures() synchronously raises pointer/capture events.
    // Teardown from an application handler invalidates hostState before any
    // native HWND below may be touched.
    if ( !hostState || hostState->GetHost() != this || m_shuttingDown )
        return;

    // Modal non-client forwarding defers this until its native loop has
    // actually returned; otherwise a posted cancel can race and terminate
    // the very loop being started.
    if ( postCancelMode )
        PostIslandCancelMode();

    if ( wxWinUIInputLogEnabled() )
    {
        wxWinUIInputLog("island reset post-cancel=%d "
                        "(inner=%p bridge=%p)",
                        postCancelMode,
                        static_cast<void *>(m_inner),
                        static_cast<void *>(m_bridge));
    }
}

void wxWinUITopLevelHost::PostIslandCancelMode()
{
    PostIslandCancelMode(m_inputSiteGeneration);
}

bool wxWinUITopLevelHost::CanPostIslandCancelMode(
    std::uint32_t expectedGeneration) const
{
    return expectedGeneration == m_inputSiteGeneration &&
           !m_nativeResize &&
           !m_inputState.HasAnyActivePress() &&
           m_pendingNativeInput.empty() &&
           std::none_of(
               m_slotPointerOwners.begin(), m_slotPointerOwners.end(),
               [](const SlotPointerOwner& owner)
               {
                   return owner.IsActive();
               });
}

void wxWinUITopLevelHost::AcknowledgeIslandCancelMessage(
    std::uint32_t generation)
{
    if ( m_postedIslandCancelMessages &&
         m_postedIslandCancelGeneration == generation )
    {
        --m_postedIslandCancelMessages;
        if ( !m_postedIslandCancelMessages )
            m_postedIslandCancelGeneration = 0;
    }
}

void wxWinUITopLevelHost::RetireIslandCancelTarget(HWND retiredTarget)
{
    if ( !retiredTarget || m_shuttingDown )
        return;

    const bool postedForCurrentSite =
        m_postedIslandCancelMessages &&
        m_postedIslandCancelGeneration == m_inputSiteGeneration;
    const bool deferredForCurrentSite =
        m_islandCancelDeferred &&
        m_deferredIslandCancelGeneration == m_inputSiteGeneration;

    // A stale batch can never coalesce the current generation, but one of its
    // target HWNDs has now disappeared and can never acknowledge it either.
    if ( m_postedIslandCancelMessages && !postedForCurrentSite )
    {
        m_postedIslandCancelMessages = 0;
        m_postedIslandCancelGeneration = 0;
    }

    if ( !postedForCurrentSite && !deferredForCurrentSite )
        return;

    // A posted batch identifies both its generation and the concrete input
    // site HWNDs. If either target retires, invalidate every old message and
    // transfer the still-required reset to the replacement generation. This
    // prevents a lost acknowledgement from coalescing resets forever.
    m_postedIslandCancelMessages = 0;
    m_postedIslandCancelGeneration = 0;
    m_islandCancelDeferred = false;
    m_deferredIslandCancelGeneration = 0;
    if ( ++m_inputSiteGeneration == 0 )
        ++m_inputSiteGeneration;
    m_islandCancelDeferred = true;
    m_deferredIslandCancelGeneration = m_inputSiteGeneration;
}

void wxWinUITopLevelHost::RetryDeferredIslandCancelMode()
{
    if ( !m_islandCancelDeferred )
        return;

    if ( m_shuttingDown ||
         m_deferredIslandCancelGeneration != m_inputSiteGeneration )
    {
        m_islandCancelDeferred = false;
        m_deferredIslandCancelGeneration = 0;
        return;
    }

    if ( !CanPostIslandCancelMode(
             m_deferredIslandCancelGeneration) )
    {
        return;
    }

    const std::uint32_t generation =
        m_deferredIslandCancelGeneration;
    m_islandCancelDeferred = false;
    m_deferredIslandCancelGeneration = 0;
    PostIslandCancelMode(generation);
}

void wxWinUITopLevelHost::AdvanceInputSiteGeneration()
{
    if ( ++m_inputSiteGeneration == 0 )
        ++m_inputSiteGeneration;

    // A newer physical DOWN proves that an older deferred input-site reset
    // must never be applied to the new gesture.
    if ( m_islandCancelDeferred &&
         m_deferredIslandCancelGeneration != m_inputSiteGeneration )
    {
        m_islandCancelDeferred = false;
        m_deferredIslandCancelGeneration = 0;
    }
}

void wxWinUITopLevelHost::NotifyNativeCaptureMutation()
{
    if ( ++m_nativeCaptureMutationGeneration == 0 )
        ++m_nativeCaptureMutationGeneration;

    // A capture callback can run inside wxEVT_SET_CURSOR/WM_SETCURSOR. The
    // in-flight resolution owns the point and will publish its freshly
    // validated verdict after the callback; reasserting the previous
    // surface here would advance the selection generation and incorrectly
    // make that stale cursor look like a newer nested winner.
    if ( m_cursorResolutionDepth )
        return;

    // USER32 capture transitions can cause WinUI to re-evaluate the island
    // cursor. Reapply the already-resolved value without invoking a native
    // WM_SETCURSOR callback in the middle of the capture transaction.
    if ( m_islandPointerCursorApplied )
        (void)SetIslandPointerCursor(m_lastIslandPointerCursor,
                                     m_lastIslandPointerHandle);
}

void wxWinUITopLevelHost::PostIslandCancelMode(
    std::uint32_t expectedGeneration)
{
    // A native modal loop pumps arbitrary application and XAML callbacks.
    // Never let its eventual cleanup cancel a newer physical press, a live
    // slot gesture or a later modal request.
    if ( m_shuttingDown ||
         expectedGeneration != m_inputSiteGeneration )
    {
        if ( m_islandCancelDeferred &&
             m_deferredIslandCancelGeneration == expectedGeneration )
        {
            m_islandCancelDeferred = false;
            m_deferredIslandCancelGeneration = 0;
        }
        return;
    }

    if ( !CanPostIslandCancelMode(expectedGeneration) )
    {
        m_islandCancelDeferred = true;
        m_deferredIslandCancelGeneration = expectedGeneration;
        return;
    }

    if ( m_islandCancelDeferred &&
         m_deferredIslandCancelGeneration == expectedGeneration )
    {
        m_islandCancelDeferred = false;
        m_deferredIslandCancelGeneration = 0;
    }

    // Coalesce repeated terminal paths for the same modal generation while
    // the two distinct host-window resets are still in the USER32 queue.
    if ( m_postedIslandCancelMessages &&
         m_postedIslandCancelGeneration == expectedGeneration )
    {
        return;
    }

    const UINT message = wxWinUIInternalCancelMessage();
    if ( !message )
    {
        m_islandCancelDeferred = true;
        m_deferredIslandCancelGeneration = expectedGeneration;
        return;
    }

    // The subclass translates this private registered message to
    // WM_CANCELMODE only below itself. It therefore reaches the XAML input
    // site without being mistaken for an external USER32 interruption by
    // BridgeSubclassProc.
    unsigned posted = 0;
    if ( m_inner && ::IsWindow(m_inner) )
    {
        if ( ::PostMessage(
                 m_inner, message,
                 static_cast<WPARAM>(expectedGeneration), 0) )
        {
            ++posted;
        }
    }
    if ( m_bridge && m_bridge != m_inner && ::IsWindow(m_bridge) )
    {
        if ( ::PostMessage(
                 m_bridge, message,
                 static_cast<WPARAM>(expectedGeneration), 0) )
        {
            ++posted;
        }
    }

    if ( posted )
    {
        m_postedIslandCancelGeneration = expectedGeneration;
        m_postedIslandCancelMessages = posted;
    }
    else
    {
        // A transient teardown/recreation of the inner window can make both
        // posts fail. Keep the obligation retryable until the host advances
        // to a newer physical press or another terminal path becomes idle.
        m_islandCancelDeferred = true;
        m_deferredIslandCancelGeneration = expectedGeneration;
    }
}

// Whether the window of a slot, or a window inside it, has the mouse capture
// of USER32. Such a window took the mouse itself when a button went down on
// it, as a header does to drag a column or the edge of one. The island then
// loses its own capture, which XAML and the bridge report as an interrupted
// gesture (WM_CAPTURECHANGED, PointerCaptureLost, PointerCanceled, through
// the slot and through the root): nothing was interrupted, the gesture goes
// on for that window, which gets the moves and the release of the button.
// Ending it left the window dragging with no button held: it got nothing
// until the button was released, then followed the pointer until the next
// click. A window which loses the capture is told so by Win32 itself.
static bool wxWinUISlotWindowHoldsCapture(WXHWND slotWindow)
{
    const HWND capture = ::GetCapture();
    const HWND hwnd = reinterpret_cast<HWND>(slotWindow);
    return capture && hwnd &&
           (capture == hwnd || ::IsChild(hwnd, capture));
}

void wxWinUITopLevelHost::OnSlotPointer(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const winrt::Microsoft::UI::Xaml::FrameworkElement& container,
    const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args,
    wxWinUIInputKind kind)
{
    OperationGuard hostOperation(this);
    if ( m_shuttingDown || !state || state->GetHost() != this )
        return;
    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });

    wxWindow * const window = state->GetWindow();
    wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
    if ( !window || !slot || slot->m_lifetime != state ||
         !container || container != slot->GetPresentationRoot() )
    {
        return;
    }

    // Keep the allocation of the slot alive while the event is read.
    wxWinUISlot::OperationGuard slotOperation(slot);

    wxWinUIPointerSample sample;
    sample.kind = kind;
    sample.timestamp = ::GetTickCount64();

    bool wheelUsedByPeer = false;
    if ( kind == wxWinUIInputKind::Wheel )
    {
        try
        {
            wheelUsedByPeer = args.Handled();
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }
    const bool isInterruption =
        kind == wxWinUIInputKind::Cancel ||
        kind == wxWinUIInputKind::CaptureLost;
    if ( isInterruption && wxWinUISlotWindowHoldsCapture(slot->m_hwnd) )
    {
        if ( wxWinUIInputLogEnabled() )
            wxWinUIInputLog("slot INTERRUPT kept: window has capture");
        return;
    }

    bool havePointerIdentity = false;
    try
    {
        const auto pointer = args.Pointer();
        sample.device = wxWinUIGetInputDevice(pointer.PointerDeviceType());
        sample.pointerId = pointer.PointerId();
        havePointerIdentity = true;

        const unsigned keyModifiers =
            static_cast<unsigned>(args.KeyModifiers());
        using VirtualKeyModifiers =
            winrt::Windows::System::VirtualKeyModifiers;
        if ( keyModifiers &
             static_cast<unsigned>(VirtualKeyModifiers::Shift) )
        {
            sample.modifiers |= MK_SHIFT;
        }
        if ( keyModifiers &
             static_cast<unsigned>(VirtualKeyModifiers::Control) )
        {
            sample.modifiers |= MK_CONTROL;
        }
        if ( keyModifiers &
             static_cast<unsigned>(VirtualKeyModifiers::Menu) )
        {
            sample.modifiers |= wxWINUI_INPUT_MOD_ALT;
        }
    }
    catch ( const winrt::hresult_error& )
    {
        // Cancellation must not depend on a visual/input query succeeding:
        // abandoning the cleanup here would leave the slot permanently
        // pressed. Ordinary samples fail closed.
        if ( !isInterruption )
            return;
    }

    bool havePoint = false;
    bool pointInside = true;
    try
    {
        // Ask for the point in the explicitly-LTR root coordinate space.
        // A mirrored slot container has a mirrored local origin, so using it
        // here would require reconstructing the physical X coordinate.
        const auto point = args.GetCurrentPoint(m_root);
        const auto props = point.Properties();
        sample.device = wxWinUIGetInputDevice(point.PointerDeviceType());
        sample.pointerId = point.PointerId();
        sample.isPrimary = props.IsPrimary();
        sample.isCompatibilityMouse =
            sample.device == wxWinUIInputDevice::Mouse &&
            wxWinUIIsCompatibilityMouseEvent();
        sample.timestamp = point.Timestamp() / 1000;

        unsigned buttonMask = 0;
        if ( props.IsLeftButtonPressed() )
            buttonMask |= MK_LBUTTON;
        if ( props.IsRightButtonPressed() )
            buttonMask |= MK_RBUTTON;
        if ( props.IsMiddleButtonPressed() )
            buttonMask |= MK_MBUTTON;
        if ( props.IsXButton1Pressed() )
            buttonMask |= MK_XBUTTON1;
        if ( props.IsXButton2Pressed() )
            buttonMask |= MK_XBUTTON2;

        if ( kind == wxWinUIInputKind::Move ||
             kind == wxWinUIInputKind::Press ||
             kind == wxWinUIInputKind::Release )
        {
            const wxWinUIInputMapping mapping =
                wxWinUIMapPointerUpdate(wxWinUIGetInputUpdateKind(
                    props.PointerUpdateKind()));
            if ( mapping.valid )
            {
                sample.kind = mapping.kind;
                sample.button = mapping.button;
            }
            else if ( kind != wxWinUIInputKind::Move &&
                      sample.device != wxWinUIInputDevice::Mouse )
            {
                // A normal primary touch/pen contact has no mouse update
                // kind. Map that contact to left once; keep an explicit pen
                // barrel/right update from the mapping above unchanged.
                sample.button = wxWinUIInputButton::Left;
            }
            else if ( kind != wxWinUIInputKind::Move )
            {
                return;
            }

            if ( sample.kind == wxWinUIInputKind::Press )
                buttonMask |= wxWinUIGetWParamButtonMask(sample.button);
            else if ( sample.kind == wxWinUIInputKind::Release )
                buttonMask &= ~wxWinUIGetWParamButtonMask(sample.button);
        }
        else if ( kind == wxWinUIInputKind::Wheel )
        {
            sample.wheelDelta = props.MouseWheelDelta();
            sample.horizontalWheel = props.IsHorizontalMouseWheel();
        }
        sample.buttonMask = buttonMask;

        const auto position = point.Position();
        POINT screen;
        if ( !MapRootPointerPointForSlot(
                 *slot, position.X, position.Y, &screen, &pointInside) )
        {
            if ( !isInterruption )
                return;
        }
        else
        {
            sample.screenX = screen.x;
            sample.screenY = screen.y;
            havePoint = true;
        }
    }
    catch ( const winrt::hresult_error& )
    {
        if ( !isInterruption )
            return;
    }

    if ( !havePoint )
    {
        if ( isInterruption )
        {
            // A canceled point may no longer expose PointerPoint properties.
            // Only primary contacts can have entered this slot state; retain
            // the stable id when available, otherwise retire every gesture
            // local to this one slot.
            sample.isPrimary = true;
            sample.isCompatibilityMouse = false;
            sample.interruptAll = !havePointerIdentity;
        }
        POINT screen = { 0, 0 };
        if ( ::GetCursorPos(&screen) )
        {
            sample.screenX = screen.x;
            sample.screenY = screen.y;
        }
    }

    wxWinUIRouteObservation observation;
    if ( isInterruption )
    {
        // inputstate deliberately processes termination independently of
        // hit resolution and ignores this observation.
        observation.surface = wxWinUIInputSurface::Indeterminate;
    }
    else
    {
        const WXHWND hwnd = slot->m_hwnd;
        const unsigned long long generation =
            wxWinUIMSWGetHwndGeneration(window, hwnd);
        if ( !hwnd || !generation )
            return;

        observation.surface = pointInside
            ? wxWinUIInputSurface::Native
            : wxWinUIInputSurface::Outside;
        observation.target.leaf =
            reinterpret_cast<std::uintptr_t>(hwnd);
        observation.target.generation = generation;
        observation.target.owner = observation.target.leaf;
        observation.target.ownerGeneration = generation;
        observation.area = wxWinUIInputArea::Client;
        observation.zone = HTCLIENT;
        observation.clientDoubleClicks =
            (::GetClassLongPtr(reinterpret_cast<HWND>(hwnd),
                               GCL_STYLE) & CS_DBLCLKS) != 0;
    }

    if ( (sample.device != wxWinUIInputDevice::Mouse &&
          !sample.isPrimary) ||
         sample.isCompatibilityMouse )
    {
        return;
    }

    // Step 3: the sample is dealt with from the message loop, as what the
    // pointer does over the other windows is, see QueuedPointer. Two things
    // cannot wait. The end of a gesture (canceled, capture lost) undoes its
    // state now. A right button released is what XAML makes its
    // ContextRequested of, raised next, which must know by then whether the
    // program answered wxEVT_CONTEXT_MENU: see OnRootContextRequested().
    if ( wxWinUIInputQueueMode() == 2 )
    {
        const bool now = isInterruption ||
            (sample.kind == wxWinUIInputKind::Release &&
             sample.button == wxWinUIInputButton::Right);
        if ( !now &&
             QueueSlotPointerSample(state, sample, observation,
                                    wheelUsedByPeer) )
        {
            return;
        }

        // After what was waiting.
        if ( !m_rootPointerQueue.empty() )
        {
            DrainRootPointerQueue();
            if ( m_shuttingDown || state->GetHost() != this ||
                 state->GetWindow() != window || FindSlot(window) != slot )
            {
                return;
            }
        }
    }

    ProcessSlotPointerSample(state, sample, observation, wheelUsedByPeer);
}

void wxWinUITopLevelHost::ProcessSlotPointerSample(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const wxWinUIPointerSample& sample,
    const wxWinUIRouteObservation& observation,
    bool wheelUsedByPeer)
{
    OperationGuard hostOperation(this);
    if ( m_shuttingDown || !state || state->GetHost() != this )
        return;

    wxWindow * const window = state->GetWindow();
    wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
    if ( !window || !slot || slot->m_lifetime != state )
        return;

    // A wx handler invoked below may synchronously unregister the slot. Keep
    // its allocation alive for the state transition, but revalidate the
    // invalidatable lifetime before every subsequent action.
    wxWinUISlot::OperationGuard slotOperation(slot);
    if ( ++slot->m_inputTransitionEpoch == 0 )
        ++slot->m_inputTransitionEpoch;
    const unsigned long long transitionEpoch =
        slot->m_inputTransitionEpoch;

    const bool outerWheelUsedByPeer = gs_slotWheelUsedByPeer;
    gs_slotWheelUsedByPeer = wheelUsedByPeer;
    const auto restoreWheelUsedByPeer = wxMakeGuard(
        [outerWheelUsedByPeer]()
        {
            gs_slotWheelUsedByPeer = outerWheelUsedByPeer;
        });

    // The grip accepts its press before it bubbles through this slot's
    // handledEventsToo observer. The host resize transaction already owns
    // this pointer: routing it again would advance the InputSite generation
    // and create a competing slot press, invalidating the deferred resize.
    // The root still observes release/interruption and cancels the resize.
    if ( m_nativeResize && sample.device == m_nativeResize->sample.device &&
         sample.pointerId == m_nativeResize->sample.pointerId )
    {
        return;
    }

    if ( sample.kind == wxWinUIInputKind::Press &&
         HasSlotPointerOwner(sample) )
    {
        return;
    }

    if ( (sample.kind == wxWinUIInputKind::Cancel ||
          sample.kind == wxWinUIInputKind::CaptureLost) &&
         wxWinUISlotWindowHoldsCapture(slot->m_hwnd) )
    {
        return;
    }

    if ( sample.kind == wxWinUIInputKind::Press )
        AdvanceInputSiteGeneration();

    const wxWinUIInputTransition transition =
        slot->m_inputState.Route(sample, observation);
    const auto cleanupRemaining =
        [this, &transition, &state](std::size_t first)
        {
            for ( std::size_t i = first;
                  i < transition.actions.size(); ++i )
            {
                const wxWinUIInputAction& pending = transition.actions[i];
                if ( pending.action ==
                         wxWinUIInputActionKind::HoverLeave ||
                     pending.action ==
                         wxWinUIInputActionKind::CancelPress )
                {
                    ExecuteSlotInputCleanup(state, pending);
                }
            }
        };

    // Never apply transition.cancelSource here. These events originate in
    // the hosted XAML control itself; setting Handled would prevent its
    // native visual/command behaviour. The state-machine flag is for the
    // root's native pass-through path only.
    for ( std::size_t i = 0; i < transition.actions.size(); ++i )
    {
        const wxWinUIInputAction& action = transition.actions[i];
        if ( state->GetHost() != this ||
             state->GetWindow() != window ||
             FindSlot(window) != slot ||
             slot->m_inputTransitionEpoch != transitionEpoch )
        {
            cleanupRemaining(i);
            return;
        }

        if ( !SendSlotMouseEvent(state, action) )
        {
            cleanupRemaining(i + 1);
            return;
        }

        if ( slot->m_inputTransitionEpoch != transitionEpoch )
        {
            cleanupRemaining(i + 1);
            return;
        }
    }
    RetryDeferredIslandCancelMode();
}

void wxWinUITopLevelHost::OnRootContextRequested(
    const winrt::Microsoft::UI::Xaml::Input::ContextRequestedEventArgs& args)
{
    // A right click released, Shift+F10 or the menu key. Over a window
    // which is not shown by XAML the root router forwards the buttons and
    // USER32 makes WM_CONTEXTMENU of them, as ever; a control shown by XAML
    // gets its mouse events from its slot, which makes none: without this
    // such a control never had wxEVT_CONTEXT_MENU, and a menu bound to it,
    // or to a parent of it, never opened.
    OperationGuard hostOperation(this);
    if ( m_shuttingDown || !args )
        return;

    try
    {
        namespace MUX = winrt::Microsoft::UI::Xaml;

        const wxWinUISlot * const slot = FindSlotForVisual(
            args.OriginalSource().try_as<MUX::DependencyObject>());
        if ( !slot || !slot->m_lifetime )
            return;

        const std::shared_ptr<wxWinUISlotLifetime> state = slot->m_lifetime;
        wxWindow * const window = state->GetWindow();
        if ( !window || !window->IsEnabled() )
            return;

        // Where, in screen coordinates; nowhere when the keyboard asked,
        // which is how wxMSW says so too.
        wxPoint position = wxDefaultPosition;
        winrt::Windows::Foundation::Point point{};
        if ( args.TryGetPosition(m_root, point) )
        {
            // When the control was sent the right button itself (see
            // wxWinUIInputMessagesEnabled()), USER32 has made its
            // WM_CONTEXTMENU of it and wxEVT_CONTEXT_MENU has been sent:
            // not again, but the flyout XAML has for the control still
            // gives way to a menu of the program.
            if ( wxWinUIInputMessagesEnabled() &&
                 m_slotContextMenuWindow == window &&
                 ::GetTickCount64() - m_slotContextMenuTick < 2000 )
            {
                m_slotContextMenuWindow = nullptr;
                if ( m_slotContextMenuProcessed )
                    args.Handled(true);
                return;
            }

            POINT screen{};
            bool inside = false;
            if ( MapRootPointerPointForSlot(*slot, point.X, point.Y,
                                            &screen, &inside) )
            {
                position = wxPoint(screen.x, screen.y);
            }
        }

        wxContextMenuEvent event(wxEVT_CONTEXT_MENU, window->GetId(), position);
        event.SetEventObject(window);

        // The handler may open a menu, which runs a loop of its own, and
        // may destroy the window: nothing of it is used afterwards. A
        // request which was answered must not open the flyout XAML has for
        // some controls (a text box) as well.
        if ( window->HandleWindowEvent(event) )
            args.Handled(true);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("context menu request", e);
    }
}

bool wxWinUITopLevelHost::MapRootPointerPointForSlot(
    const wxWinUISlot& slot,
    double xDIP,
    double yDIP,
    POINT *screen,
    bool *inside) const
{
    if ( !screen || !inside ||
         !std::isfinite(xDIP) || !std::isfinite(yDIP) )
    {
        return false;
    }

    const double scale = GetScale();
    if ( !(scale > 0.0) || !std::isfinite(scale) )
        return false;

    // xDIP/yDIP are already expressed relative to the explicitly-LTR root.
    // Calling window->ClientToScreen() here would mirror X a second time for
    // WS_EX_LAYOUTRTL. SendSlotMouseEvent() deliberately converts the final
    // physical screen point to the native client space only at delivery.
    const RECT rootScreen = GetClientScreenRect();
    const double xPx = xDIP * scale;
    const double yPx = yDIP * scale;
    const wxPoint pointInRoot(
        static_cast<int>(std::lround(xPx)),
        static_cast<int>(std::lround(yPx)));
    screen->x = rootScreen.left + pointInRoot.x;
    screen->y = rootScreen.top + pointInRoot.y;
    // Keep the hit predicate in the continuous physical coordinate space.
    // Rounding is part of the Win32 POINT contract only: using that rounded
    // value for containment shifts every edge by half a pixel at fractional
    // rasterization scales.
    RefreshSlotRects(slot);
    const double left = slot.m_visibleRectPx.GetX();
    const double top = slot.m_visibleRectPx.GetY();
    const double right = left + slot.m_visibleRectPx.GetWidth();
    const double bottom = top + slot.m_visibleRectPx.GetHeight();
    *inside = slot.m_visible &&
              xPx >= left && xPx < right &&
              yPx >= top && yPx < bottom;
    return true;
}

bool wxWinUITopLevelHost::SendSlotMouseEvent(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const wxWinUIInputAction& action)
{
    if ( m_shuttingDown || !state || state->GetHost() != this )
        return false;

    wxWindow * const window = state->GetWindow();
    wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
    if ( !window || !slot || slot->m_lifetime != state )
        return false;

    // CancelPress is semantic bookkeeping for a XAML gesture. There is no
    // wx "button canceled" event and fabricating an UP here would duplicate
    // a later release. Slots never claim USER32 capture in inputstate, so a
    // ReleaseCapture action is likewise only a defensive no-op.
    if ( action.action == wxWinUIInputActionKind::CancelPress )
    {
        UpdateSlotPointerOwner(state, action);
        if ( slot->m_inputState.CanDeliverCancellation(action) )
            slot->m_inputState.AcknowledgeCancellation(action);
        return true;
    }
    if ( action.action == wxWinUIInputActionKind::ReleaseCapture ||
         action.action == wxWinUIInputActionKind::BreakStorm )
    {
        return true;
    }

    const bool isHoverLeave =
        action.action == wxWinUIInputActionKind::HoverLeave;
    if ( isHoverLeave &&
         !slot->m_inputState.CommitHoverAction(action) )
    {
        return false;
    }

    const WXHWND hwnd = slot->m_hwnd;
    wxWinUIInputTargetKey currentTarget;
    currentTarget.leaf = reinterpret_cast<std::uintptr_t>(hwnd);
    currentTarget.generation =
        wxWinUIMSWGetHwndGeneration(window, hwnd);
    currentTarget.owner = currentTarget.leaf;
    currentTarget.ownerGeneration = currentTarget.generation;
    if ( !currentTarget.IsOk() || currentTarget != action.target )
        return isHoverLeave;

    if ( action.action == wxWinUIInputActionKind::HoverEnter &&
         !slot->m_inputState.CommitHoverAction(action) )
    {
        return false;
    }

    if ( action.action == wxWinUIInputActionKind::Dispatch &&
         action.kind == wxWinUIInputKind::Wheel )
    {
        if ( !action.wheelDelta )
            return true;

        const WORD keys = static_cast<WORD>(
            action.modifiers | action.buttonMask);
        if ( !slot->m_inputState.CommitBeforeDispatch(action) )
            return false;
        const WPARAM wheelWParam = MAKEWPARAM(
            keys,
            static_cast<WORD>(static_cast<short>(action.wheelDelta)));
        const LPARAM wheelLParam = MAKELPARAM(
            static_cast<short>(action.screenX),
            static_cast<short>(action.screenY));
        if ( gs_slotWheelUsedByPeer )
        {
            // See gs_slotWheelUsedByPeer: the wx event only.
            window->HandleMouseWheel(
                action.horizontalWheel ? wxMOUSE_WHEEL_HORIZONTAL
                                       : wxMOUSE_WHEEL_VERTICAL,
                wheelWParam, wheelLParam);
        }
        else
        {
            ::SendMessage(
                reinterpret_cast<HWND>(hwnd),
                action.horizontalWheel ? WM_MOUSEHWHEEL : WM_MOUSEWHEEL,
                wheelWParam, wheelLParam);
        }
        return state->GetHost() == this &&
               state->GetWindow() == window &&
               !m_shuttingDown;
    }

    const wxEventType type = wxWinUIGetSlotMouseEventType(action);
    if ( type == wxEVT_NULL )
        return true;

    wxMouseEvent event(type);
    event.SetEventObject(window);
    event.SetId(window->GetId());
    event.SetTimestamp(static_cast<long>(action.timestamp));

    const wxPoint nativeClient = window->ScreenToClient(
        wxPoint(action.screenX, action.screenY));
    // Match wxWindowMSW::InitMouseEvent(): wx logical client coordinates
    // exclude a frame's toolbar/status/menu client-origin offset.
    const wxPoint client =
        nativeClient - window->GetClientAreaOrigin();
    event.m_x = client.x;
    event.m_y = client.y;
    event.m_shiftDown = (action.modifiers & MK_SHIFT) != 0;
    event.m_controlDown = (action.modifiers & MK_CONTROL) != 0;
    event.m_altDown =
        (action.modifiers & wxWINUI_INPUT_MOD_ALT) != 0;
    event.m_leftDown = (action.buttonMask & MK_LBUTTON) != 0;
    event.m_middleDown = (action.buttonMask & MK_MBUTTON) != 0;
    event.m_rightDown = (action.buttonMask & MK_RBUTTON) != 0;
    event.m_aux1Down = (action.buttonMask & MK_XBUTTON1) != 0;
    event.m_aux2Down = (action.buttonMask & MK_XBUTTON2) != 0;
    event.m_synthesized =
        action.device != wxWinUIInputDevice::Mouse;

    if ( action.action == wxWinUIInputActionKind::Dispatch &&
         !slot->m_inputState.CommitBeforeDispatch(action) )
    {
        return false;
    }

    UpdateSlotPointerOwner(state, action);

    if ( wxWinUIInputMessagesEnabled() )
    {
        // Entering is what wxMSW makes of the first move, as on any window.
        if ( type == wxEVT_ENTER_WINDOW )
            return true;

        // While a button is held on a window which is not shown by XAML (a
        // splitter whose sash is dragged), that window alone gets the mouse,
        // as the window which has the capture does in Win32. The control
        // the pointer passes over must not get a message for the same
        // position: wxMSW drops a move at the position of the last mouse
        // message, and the one dropped was that of the window being
        // dragged, which then moved only when the button was released.
        if ( type != wxEVT_LEAVE_WINDOW && m_inputState.HasAnyActivePress() )
            return true;

        const HWND target = reinterpret_cast<HWND>(hwnd);
        UINT message = 0;
        WPARAM wParam = 0;
        LPARAM lParam = 0;
        if ( type == wxEVT_LEAVE_WINDOW )
        {
            message = WM_MOUSELEAVE;
        }
        else
        {
            POINT point = { action.screenX, action.screenY };
            ::SetLastError(ERROR_SUCCESS);
            if ( !::MapWindowPoints(HWND_DESKTOP, target, &point, 1) &&
                 ::GetLastError() != ERROR_SUCCESS )
            {
                return true;
            }

            const WORD keys = static_cast<WORD>(
                (action.modifiers & (MK_SHIFT | MK_CONTROL)) |
                action.buttonMask);
            wParam = keys;
            lParam = MAKELPARAM(static_cast<short>(point.x),
                                static_cast<short>(point.y));
            if ( type == wxEVT_MOTION )
            {
                message = WM_MOUSEMOVE;
            }
            else
            {
                wxWinUIButtonMessages messages;
                if ( !wxWinUIGetButtonMessages(action.button, action.kind,
                                               &messages) )
                {
                    return true;
                }
                message = messages.client;
                if ( messages.xButton )
                    wParam = MAKEWPARAM(keys, messages.xButton);
            }
        }

        bool contextProcessed = false;
        const unsigned long long contextSerial =
            wxWinUIMSWGetContextMenuSerial(nullptr);
        {
            // wxMSW gives the focus to a window whose click nothing has
            // processed. XAML gave it to what was clicked when the button
            // went down, and is the one to say: asking for it again would
            // take it from there to the first element of the control.
            wxWinUIMSWBeginSlotMouseDispatch();
            const auto endSlotDispatch = wxMakeGuard(
                []()
                {
                    wxWinUIMSWEndSlotMouseDispatch();
                });
            const wxWinUISyntheticMouseDispatch syntheticDispatch;
            ::SendMessage(target, message, wParam, lParam);
        }

        // The window may be gone: only its address is kept, to be compared.
        if ( message == WM_RBUTTONUP &&
             wxWinUIMSWGetContextMenuSerial(&contextProcessed) !=
                contextSerial )
        {
            m_slotContextMenuWindow = window;
            m_slotContextMenuProcessed = contextProcessed;
            m_slotContextMenuTick = ::GetTickCount64();
        }

        return state->GetHost() == this &&
               state->GetWindow() == window &&
               !m_shuttingDown;
    }

    window->HandleWindowEvent(event);
    return state->GetHost() == this &&
           state->GetWindow() == window &&
           !m_shuttingDown;
}

void wxWinUITopLevelHost::ExecuteSlotInputCleanup(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const wxWinUIInputAction& action)
{
    if ( action.action == wxWinUIInputActionKind::CancelPress )
    {
        (void)SendSlotMouseEvent(state, action);
        return;
    }

    if ( action.action != wxWinUIInputActionKind::HoverLeave ||
         m_shuttingDown || !state || state->GetHost() != this )
    {
        return;
    }

    wxWindow * const window = state->GetWindow();
    wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
    if ( !window || !slot || slot->m_lifetime != state )
        return;

    // A wx UP/leave handler can route a newer gesture before the prepared
    // balancing HoverLeave runs. Only finish that old cleanup if the current
    // slot geometry still proves the sample is outside; otherwise preserve
    // hover and let the next real sample converge it.
    const RECT rootScreen = GetClientScreenRect();
    const wxPoint pointInRoot(action.screenX - rootScreen.left,
                              action.screenY - rootScreen.top);
    RefreshSlotRects(*slot);
    if ( slot->m_visible &&
         slot->m_visibleRectPx.Contains(pointInRoot) )
    {
        return;
    }

    (void)SendSlotMouseEvent(state, action);
}

void wxWinUITopLevelHost::UpdateSlotPointerOwner(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const wxWinUIInputAction& action)
{
    if ( !state || !action.gestureSerial ||
         action.button == wxWinUIInputButton::None )
    {
        return;
    }

    const bool starts =
        action.action == wxWinUIInputActionKind::Dispatch &&
        (action.kind == wxWinUIInputKind::Press ||
         action.kind == wxWinUIInputKind::DoubleClick);
    const bool ends =
        action.action == wxWinUIInputActionKind::CancelPress ||
        (action.action == wxWinUIInputActionKind::Dispatch &&
         action.kind == wxWinUIInputKind::Release);
    if ( !starts && !ends )
        return;

    SlotPointerOwner *freeOwner = nullptr;
    for ( SlotPointerOwner& owner : m_slotPointerOwners )
    {
        if ( !owner.IsActive() )
        {
            if ( !freeOwner )
                freeOwner = &owner;
            continue;
        }

        const std::shared_ptr<wxWinUISlotLifetime> current =
            owner.state.lock();
        if ( !current )
        {
            owner.Clear();
            if ( !freeOwner )
                freeOwner = &owner;
            continue;
        }

        if ( current == state &&
             owner.device == action.device &&
             owner.pointerId == action.pointerId &&
             owner.button == action.button &&
             owner.gestureSerial == action.gestureSerial )
        {
            if ( ends )
            {
                if ( action.action == wxWinUIInputActionKind::Dispatch &&
                     action.kind == wxWinUIInputKind::Release )
                {
                    RememberSlotRelease(action);
                }
                owner.Clear();
            }
            return;
        }
    }

    if ( starts )
    {
        wxASSERT_MSG(freeOwner,
                     "WinUI slot pointer owner table exhausted");
        if ( freeOwner )
        {
            freeOwner->state = state;
            freeOwner->device = action.device;
            freeOwner->pointerId = action.pointerId;
            freeOwner->button = action.button;
            freeOwner->gestureSerial = action.gestureSerial;
            freeOwner->downTimestamp = action.timestamp;
        }
    }
}

void wxWinUITopLevelHost::RememberSlotRelease(
    const wxWinUIInputAction& action)
{
    SlotReleaseReceipt *freeReceipt = nullptr;
    for ( SlotReleaseReceipt& receipt : m_slotReleaseReceipts )
    {
        if ( !receipt.IsActive() )
        {
            if ( !freeReceipt )
                freeReceipt = &receipt;
            continue;
        }

        if ( receipt.device == action.device &&
             receipt.pointerId == action.pointerId &&
             receipt.button == action.button )
        {
            freeReceipt = &receipt;
            break;
        }
    }

    wxASSERT_MSG(freeReceipt,
                 "WinUI slot release receipt table exhausted");
    if ( freeReceipt )
    {
        freeReceipt->device = action.device;
        freeReceipt->pointerId = action.pointerId;
        freeReceipt->button = action.button;
        freeReceipt->timestamp = action.timestamp;
        freeReceipt->gestureSerial = action.gestureSerial;
    }
}

bool wxWinUITopLevelHost::HasSlotPointerOwner(
    const wxWinUIPointerSample& sample)
{
    if ( sample.button == wxWinUIInputButton::None )
        return false;

    for ( SlotPointerOwner& owner : m_slotPointerOwners )
    {
        if ( !owner.IsActive() )
            continue;
        const std::shared_ptr<wxWinUISlotLifetime> state =
            owner.state.lock();
        wxWindow * const window = state ? state->GetWindow() : nullptr;
        wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
        if ( !state || state->GetHost() != this ||
             !window || !slot || slot->m_lifetime != state )
        {
            owner.Clear();
            continue;
        }
        if ( owner.device == sample.device &&
             owner.pointerId == sample.pointerId &&
             owner.button == sample.button )
        {
            return true;
        }
    }
    return false;
}

bool wxWinUITopLevelHost::ConsumeSlotRelease(
    const wxWinUIPointerSample& sample)
{
    if ( sample.kind != wxWinUIInputKind::Release )
        return false;

    for ( SlotReleaseReceipt& receipt : m_slotReleaseReceipts )
    {
        if ( receipt.IsActive() &&
             receipt.device == sample.device &&
             receipt.pointerId == sample.pointerId &&
             receipt.button == sample.button &&
             receipt.timestamp == sample.timestamp )
        {
            receipt.Clear();
            return true;
        }
    }
    return false;
}

void wxWinUITopLevelHost::ForgetSlotPointerOwners(
    const std::shared_ptr<wxWinUISlotLifetime>& state)
{
    if ( !state )
        return;

    for ( SlotPointerOwner& owner : m_slotPointerOwners )
    {
        if ( owner.IsActive() && owner.state.lock() == state )
            owner.Clear();
    }
    RetryDeferredIslandCancelMode();
}

bool wxWinUITopLevelHost::DispatchSlotOwnedRelease(
    const wxWinUIPointerSample& sample)
{
    OperationGuard hostOperation(this);
    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });

    if ( sample.kind != wxWinUIInputKind::Release ||
         sample.button == wxWinUIInputButton::None ||
         sample.isCompatibilityMouse ||
         (sample.device != wxWinUIInputDevice::Mouse &&
          !sample.isPrimary) )
    {
        return false;
    }

    if ( ConsumeSlotRelease(sample) )
        return true;

    for ( SlotPointerOwner& owner : m_slotPointerOwners )
    {
        if ( !owner.IsActive() ||
             owner.device != sample.device ||
             owner.pointerId != sample.pointerId ||
             owner.button != sample.button ||
             (sample.timestamp && owner.downTimestamp &&
              sample.timestamp < owner.downTimestamp) )
        {
            continue;
        }

        const std::shared_ptr<wxWinUISlotLifetime> state =
            owner.state.lock();
        const std::uint64_t gestureSerial = owner.gestureSerial;
        wxWindow * const window = state ? state->GetWindow() : nullptr;
        wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
        if ( !state || state->GetHost() != this ||
             !window || !slot || slot->m_lifetime != state )
        {
            owner.Clear();
            return true;
        }

        // This path exists only because the release did not traverse the
        // original slot. Route the balancing UP there, but classify the
        // physical release as outside that slot so it cannot manufacture a
        // click/double-click pair.
        (void)RouteSlotPointerSample(
            state, sample, wxWinUIInputSurface::Xaml);
        (void)ConsumeSlotRelease(sample);

        // A matching owner always consumes the physical release. If its
        // state rejected the action, forwarding it below the former slot
        // would create exactly the orphan UP this broker prevents.
        // Clear only the exact old serial: the wx UP handler is allowed to
        // pump a newer DOWN which may reuse this fixed-table entry.
        for ( SlotPointerOwner& current : m_slotPointerOwners )
        {
            if ( current.IsActive() &&
                 current.state.lock() == state &&
                 current.device == sample.device &&
                 current.pointerId == sample.pointerId &&
                 current.button == sample.button &&
                 current.gestureSerial == gestureSerial )
            {
                current.Clear();
                break;
            }
        }
        return true;
    }

    return false;
}

bool wxWinUITopLevelHost::RouteSlotPointerSample(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const wxWinUIPointerSample& sample,
    wxWinUIInputSurface physicalSurface)
{
    OperationGuard hostOperation(this);
    if ( m_shuttingDown || !state || state->GetHost() != this )
        return false;
    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });

    wxWindow * const window = state->GetWindow();
    wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
    if ( !window || !slot || slot->m_lifetime != state )
        return false;

    if ( (sample.device != wxWinUIInputDevice::Mouse &&
          !sample.isPrimary) ||
         sample.isCompatibilityMouse )
    {
        return false;
    }

    // Match the routed OnSlotPointer path: an accepted native-resize
    // pointer must not acquire a second owner through slot dispatch.
    if ( m_nativeResize && sample.device == m_nativeResize->sample.device &&
         sample.pointerId == m_nativeResize->sample.pointerId )
    {
        return true;
    }

    if ( sample.kind == wxWinUIInputKind::Press &&
         HasSlotPointerOwner(sample) )
    {
        return false;
    }

    if ( (sample.kind == wxWinUIInputKind::Cancel ||
          sample.kind == wxWinUIInputKind::CaptureLost) &&
         wxWinUISlotWindowHoldsCapture(slot->m_hwnd) )
    {
        return true;
    }

    if ( sample.kind == wxWinUIInputKind::Press )
        AdvanceInputSiteGeneration();

    wxWinUISlot::OperationGuard slotOperation(slot);
    if ( ++slot->m_inputTransitionEpoch == 0 )
        ++slot->m_inputTransitionEpoch;
    const unsigned long long epoch = slot->m_inputTransitionEpoch;

    wxWinUIRouteObservation observation;
    if ( sample.kind == wxWinUIInputKind::Cancel ||
         sample.kind == wxWinUIInputKind::CaptureLost )
    {
        observation.surface = wxWinUIInputSurface::Indeterminate;
    }
    else
    {
        const WXHWND hwnd = slot->m_hwnd;
        const unsigned long long generation =
            wxWinUIMSWGetHwndGeneration(window, hwnd);
        if ( !hwnd || !generation )
            return false;

        observation.surface = physicalSurface;
        observation.target.leaf =
            reinterpret_cast<std::uintptr_t>(hwnd);
        observation.target.generation = generation;
        observation.target.owner = observation.target.leaf;
        observation.target.ownerGeneration = generation;
        observation.area = wxWinUIInputArea::Client;
        observation.zone = HTCLIENT;
        observation.clientDoubleClicks =
            (::GetClassLongPtr(
                reinterpret_cast<HWND>(hwnd),
                GCL_STYLE) & CS_DBLCLKS) != 0;
    }

    const wxWinUIInputTransition transition =
        slot->m_inputState.Route(sample, observation);
    const auto cleanupRemaining =
        [this, &transition, &state](std::size_t first)
        {
            for ( std::size_t i = first;
                  i < transition.actions.size(); ++i )
            {
                const wxWinUIInputAction& pending = transition.actions[i];
                if ( pending.action ==
                         wxWinUIInputActionKind::HoverLeave ||
                     pending.action ==
                         wxWinUIInputActionKind::CancelPress )
                {
                    ExecuteSlotInputCleanup(state, pending);
                }
            }
        };
    for ( std::size_t i = 0; i < transition.actions.size(); ++i )
    {
        const wxWinUIInputAction& action = transition.actions[i];
        if ( state->GetHost() != this ||
             state->GetWindow() != window ||
             FindSlot(window) != slot ||
             slot->m_inputTransitionEpoch != epoch )
        {
            cleanupRemaining(i);
            return false;
        }
        if ( !SendSlotMouseEvent(state, action) )
        {
            cleanupRemaining(i + 1);
            return false;
        }
        if ( state->GetHost() != this ||
             state->GetWindow() != window ||
             FindSlot(window) != slot ||
             slot->m_inputTransitionEpoch != epoch )
        {
            cleanupRemaining(i + 1);
            // The current wx event was delivered successfully. Re-entrance
            // superseded only the remainder of this prepared snapshot, not
            // the physical sample that the test seam/broker just consumed.
            return true;
        }
    }
    RetryDeferredIslandCancelMode();
    return true;
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIHostInputTestAccess::RouteSlotPointer(
    wxWinUITopLevelHost& host,
    wxWindow *window,
    const wxWinUIPointerSample& sample,
    wxWinUIInputSurface physicalSurface)
{
    wxWinUITopLevelHost * const self = &host;
    wxWinUISlot * const slot = self->FindSlot(window);
    return slot &&
           self->RouteSlotPointerSample(
               slot->m_lifetime, sample, physicalSurface);
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIHostInputTestAccess::MapRootPointerPointForSlot(
    const wxWinUITopLevelHost& host,
    wxWindow *window,
    double xDIP,
    double yDIP,
    wxPoint *screen,
    bool *inside)
{
    const wxWinUITopLevelHost * const self = &host;
    const wxWinUISlot * const slot = self->FindSlot(window);
    if ( !slot || !screen )
        return false;

    POINT nativeScreen;
    if ( !self->MapRootPointerPointForSlot(
             *slot, xDIP, yDIP, &nativeScreen, inside) )
    {
        return false;
    }

    *screen = wxPoint(nativeScreen.x, nativeScreen.y);
    return true;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
std::size_t wxWinUIHostInputTestAccess::GetSlotPointerOwnerCount(
    const wxWinUITopLevelHost& host)
{
    const wxWinUITopLevelHost * const self = &host;
    return static_cast<std::size_t>(std::count_if(
        self->m_slotPointerOwners.begin(), self->m_slotPointerOwners.end(),
        [](const wxWinUITopLevelHost::SlotPointerOwner& owner)
        {
            return owner.IsActive();
          }));
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostInputTestAccess::RetireIslandCancelTarget(
    wxWinUITopLevelHost& host)
{
    wxWinUITopLevelHost * const self = &host;
    self->RetireIslandCancelTarget(self->m_inner ? self->m_inner : self->m_bridge);
    self->RetryDeferredIslandCancelMode();
}
#endif // WXWINUI_TEST_SUPPORT

void wxWinUITopLevelHost::CancelSlotPointerOwners(
    const wxWinUIPointerSample& sample,
    bool allPointers)
{
    OperationGuard hostOperation(this);
    const auto retryIslandReset = wxMakeGuard(
        [this]()
        {
            RetryDeferredIslandCancelMode();
        });

    // Snapshot the entries present at the interruption boundary. wx handlers
    // below may unregister/recreate slots and start a newer gesture; this old
    // cleanup must neither visit nor clear such a reentrant owner.
    const auto owners = m_slotPointerOwners;
    for ( std::size_t ownerIndex = 0;
          ownerIndex < owners.size(); ++ownerIndex )
    {
        const SlotPointerOwner& owner = owners[ownerIndex];
        if ( !owner.IsActive() ||
             (!allPointers &&
              (owner.device != sample.device ||
               owner.pointerId != sample.pointerId)) )
        {
            continue;
        }

        const std::shared_ptr<wxWinUISlotLifetime> state =
            owner.state.lock();
        const auto clearExactOwner =
            [this, ownerIndex, &owner, &state]()
            {
                SlotPointerOwner& current =
                    m_slotPointerOwners[ownerIndex];
                if ( current.IsActive() &&
                     current.state.lock() == state &&
                     current.device == owner.device &&
                     current.pointerId == owner.pointerId &&
                     current.button == owner.button &&
                     current.gestureSerial == owner.gestureSerial )
                {
                    current.Clear();
                }
            };
        wxWindow * const window = state ? state->GetWindow() : nullptr;
        wxWinUISlot * const slot = window ? FindSlot(window) : nullptr;
        if ( !state || state->GetHost() != this ||
             !window || !slot || slot->m_lifetime != state )
        {
            clearExactOwner();
            continue;
        }

        // See wxWinUISlotWindowHoldsCapture().
        if ( wxWinUISlotWindowHoldsCapture(slot->m_hwnd) )
        {
            if ( wxWinUIInputLogEnabled() )
                wxWinUIInputLog("slot owner kept: window has capture");
            continue;
        }

        wxWinUISlot::OperationGuard slotOperation(slot);
        if ( ++slot->m_inputTransitionEpoch == 0 )
            ++slot->m_inputTransitionEpoch;
        const unsigned long long epoch = slot->m_inputTransitionEpoch;

        wxWinUIPointerSample cancel = sample;
        cancel.kind = wxWinUIInputKind::Cancel;
        cancel.device = owner.device;
        cancel.pointerId = owner.pointerId;
        cancel.button = owner.button;
        cancel.isPrimary = true;
        cancel.isCompatibilityMouse = false;
        cancel.interruptAll = false;
        wxWinUIRouteObservation observation;
        observation.surface = wxWinUIInputSurface::Indeterminate;
        const wxWinUIInputTransition transition =
            slot->m_inputState.Route(cancel, observation);
        for ( const wxWinUIInputAction& action : transition.actions )
        {
            if ( state->GetHost() != this ||
                 state->GetWindow() != window ||
                 FindSlot(window) != slot ||
                 slot->m_inputTransitionEpoch != epoch ||
                 !SendSlotMouseEvent(state, action) )
            {
                break;
            }
        }
        clearExactOwner();
    }
    RetryDeferredIslandCancelMode();
}

// ----------------------------------------------------------------------------
// step 3: pointer samples over native space wait for the message loop
// ----------------------------------------------------------------------------

namespace
{

// On unless WX_WINUI_INPUT_QUEUE is 0, which brings back the delivery from
// inside the event of XAML.
bool wxWinUIInputQueueEnabled()
{
    return wxWinUIInputQueueMode() != 0;
}

UINT wxWinUIRootPointerQueueMessageId()
{
    static const UINT message = ::RegisterWindowMessageW(
        L"wxWidgets.WinUI.RootPointerQueue");
    return message;
}

} // anonymous namespace

bool wxWinUITopLevelHost::RootPointerQueueMessage(wxWindowMSW *window,
                                                  WXUINT WXUNUSED(message),
                                                  WXWPARAM WXUNUSED(wParam),
                                                  WXLPARAM WXUNUSED(lParam))
{
    wxWinUITopLevelHost * const host =
        FindForTLW(static_cast<wxWindow *>(window));
    if ( host )
        host->DrainRootPointerQueue();
    return true;
}

bool wxWinUITopLevelHost::QueueRootPointerSample(
    const wxWinUIPointerSample& sample)
{
    QueuedPointer entry;
    entry.kind = QueuedPointer::Kind::Native;
    entry.sample = sample;
    return PushQueuedPointer(std::move(entry));
}

bool wxWinUITopLevelHost::QueueXamlPointerSample(
    const wxWinUIPointerSample& sample,
    const std::shared_ptr<wxWinUISlotLifetime>& cursorSlot,
    bool ownedByXaml)
{
    QueuedPointer entry;
    entry.kind = QueuedPointer::Kind::Xaml;
    entry.sample = sample;
    entry.slot = cursorSlot;
    entry.ownedByXaml = ownedByXaml;
    return PushQueuedPointer(std::move(entry));
}

bool wxWinUITopLevelHost::QueueSlotPointerSample(
    const std::shared_ptr<wxWinUISlotLifetime>& state,
    const wxWinUIPointerSample& sample,
    const wxWinUIRouteObservation& observation,
    bool wheelUsedByPeer)
{
    QueuedPointer entry;
    entry.kind = QueuedPointer::Kind::Slot;
    entry.sample = sample;
    entry.slot = state;
    entry.observation = observation;
    entry.wheelUsedByPeer = wheelUsedByPeer;
    return PushQueuedPointer(std::move(entry));
}

bool wxWinUITopLevelHost::PushQueuedPointer(QueuedPointer&& entry)
{
    const HWND hwnd = m_tlw ? GetHwndOf(m_tlw) : nullptr;
    const UINT message = wxWinUIRootPointerQueueMessageId();
    if ( !hwnd || !message || m_shuttingDown )
        return false;

    static const bool s_registered =
        wxWindow::MSWRegisterMessageHandler(
            static_cast<int>(wxWinUIRootPointerQueueMessageId()),
            &wxWinUITopLevelHost::RootPointerQueueMessage);
    if ( !s_registered )
        return false;

    // A pointer reports hundreds of positions a second and a program which
    // is busy sees none of them until it is done: only the last is of use.
    // A move over a control waits as two, the one for its slot and the one
    // for the root, so the last two are looked at.
    if ( entry.sample.kind == wxWinUIInputKind::Move )
    {
        const std::size_t count = m_rootPointerQueue.size();
        for ( std::size_t back = 1; back <= 2 && back <= count; ++back )
        {
            QueuedPointer& other = m_rootPointerQueue[count - back];
            if ( other.sample.kind != wxWinUIInputKind::Move )
                break;

            if ( other.kind == entry.kind &&
                 other.sample.device == entry.sample.device &&
                 other.sample.pointerId == entry.sample.pointerId &&
                 (entry.kind != QueuedPointer::Kind::Slot ||
                  other.slot == entry.slot) )
            {
                other = std::move(entry);
                return true;
            }
        }
    }

    const bool isMove = entry.sample.kind == wxWinUIInputKind::Move;
    const int kind = static_cast<int>(entry.sample.kind);
    const int queued = static_cast<int>(entry.kind);

    m_rootPointerQueue.push_back(std::move(entry));
    if ( !m_rootPointerDrainPosted )
    {
        // A posted message, not a pending wx event: whichever loop runs
        // gets it, the modal loops of Windows included.
        if ( !::PostMessageW(hwnd, message, 0, 0) )
        {
            m_rootPointerQueue.pop_back();
            return false;
        }
        m_rootPointerDrainPosted = true;
    }

    if ( wxWinUIInputLogEnabled() && !isMove )
    {
        wxWinUIInputLog("queue push for=%d kind=%d waiting=%u",
                        queued, kind,
                        static_cast<unsigned>(m_rootPointerQueue.size()));
    }
    return true;
}

void wxWinUITopLevelHost::DropQueuedRootPointerMoves()
{
    for ( auto it = m_rootPointerQueue.begin();
          it != m_rootPointerQueue.end(); )
    {
        if ( it->sample.kind == wxWinUIInputKind::Move )
            it = m_rootPointerQueue.erase(it);
        else
            ++it;
    }
}

void wxWinUITopLevelHost::DrainRootPointerQueue()
{
    OperationGuard operation(this);
    m_rootPointerDrainPosted = false;

    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const auto live = [this, &hostState]()
    {
        return hostState && hostState->GetHost() == this &&
               !m_shuttingDown && m_tlw;
    };

    // Each one is taken out before it is delivered: a handler which runs a
    // loop of its own has the next ones delivered by that loop, in their
    // order, like posted messages.
    while ( live() && !m_rootPointerQueue.empty() )
    {
        const QueuedPointer entry = m_rootPointerQueue.front();
        m_rootPointerQueue.pop_front();
        const wxWinUIPointerSample& sample = entry.sample;

        if ( wxWinUIInputLogEnabled() &&
             sample.kind != wxWinUIInputKind::Move )
        {
            wxWinUIInputLog("queue deliver for=%d kind=%d left=%u nested=%u",
                            static_cast<int>(entry.kind),
                            static_cast<int>(sample.kind),
                            static_cast<unsigned>(m_rootPointerQueue.size()),
                            m_rootPointerDelivering);
        }

        if ( entry.kind == QueuedPointer::Kind::Slot )
        {
            // The control may be gone since: ProcessSlotPointerSample()
            // checks what it is given.
            ++m_rootPointerDelivering;
            ProcessSlotPointerSample(entry.slot, sample, entry.observation,
                                     entry.wheelUsedByPeer);
            --m_rootPointerDelivering;
            continue;
        }

        const bool xaml = entry.kind == QueuedPointer::Kind::Xaml;

        if ( ++m_inputTransitionEpoch == 0 )
            ++m_inputTransitionEpoch;
        const unsigned long long eventEpoch = m_inputTransitionEpoch;

        if ( xaml && entry.ownedByXaml &&
             sample.kind == wxWinUIInputKind::Move )
        {
            ObserveXamlPointerCursor();
        }

        ++m_rootPointerDelivering;
        const wxWinUIRootPointerOutcome outcome =
            RouteRootPointerSampleAtEpoch(sample, xaml, eventEpoch);
        --m_rootPointerDelivering;

        if ( !live() )
            break;

        if ( xaml && outcome.cancelSource && wxWinUIInputLogEnabled() )
        {
            // Too late to keep the event from XAML: it is not queued when
            // a button is held or the mouse captured, the cases known to
            // ask for it.
            wxWinUIInputLog("queue: an event XAML owns was to be canceled, "
                            "kind=%d", static_cast<int>(sample.kind));
        }

        if ( xaml && entry.ownedByXaml )
        {
            // As in OnRootPointer(), after the route: leaving a window
            // which is not shown by XAML resets its cursor first.
            if ( m_inputTransitionEpoch == eventEpoch )
            {
                ApplyXamlPointerCursorForSlot(
                    entry.slot, wxWinUIShouldEmitSetCursor(sample.kind),
                    eventEpoch);
            }
        }
        else if ( sample.kind != wxWinUIInputKind::Move &&
                  m_inputTransitionEpoch == eventEpoch &&
                  m_activeCursorSurface == ActiveCursorSurface::Native )
        {
            // As in OnRootPointer(): over native space a press, a release
            // or a wheel step does not set the cursor of the window again.
            ReassertIslandPointerCursor();
        }
        RetryDeferredIslandCancelMode();
    }
}

void wxWinUITopLevelHost::OnRootPointer(
    UINT message,
    const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args)
{
    OperationGuard operation(this);
    if ( m_shuttingDown || !m_tlw )
        return;
    const unsigned long long entryEpoch = m_inputTransitionEpoch;
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;

    if ( message == WM_LBUTTONDOWN )
        wxWinUIDismissPopupOnOutsidePress(m_tlw);

    const wxWinUIRootPointerNesting nesting;
    if ( wxWinUIRootPointerNesting::TooDeep() )
    {
        // Swallow the packet: routing it would only deepen the recursion.
        args.Handled(true);
        return;
    }

    try
    {
        const wxWinUIProfileScope profileTotal(wxWinUIProfilePhase::Total);
        const long long pointStarted = wxWinUIProfileNow();
        const auto point = args.GetCurrentPoint(m_root);
        const auto props = point.Properties();
        wxWinUIProfileAdd(wxWinUIProfilePhase::Point, pointStarted);
        bool sourceOwnedByXaml = true;
        try
        {
            const auto source =
                args.OriginalSource().try_as<
                    winrt::Microsoft::UI::Xaml::UIElement>();
            sourceOwnedByXaml = !source || source != m_root;
        }
        catch ( const winrt::hresult_error& )
        {
        }
        const bool sourceAlreadyHandled =
            args.Handled() || sourceOwnedByXaml;

        // See m_xamlGestureActive: while a control holds a gesture which
        // started on it, its moves are not routed.
        {
            const std::uint32_t pointerId = point.PointerId();
            const bool anyButtonDown =
                props.IsLeftButtonPressed() || props.IsRightButtonPressed() ||
                props.IsMiddleButtonPressed() || props.IsXButton1Pressed() ||
                props.IsXButton2Pressed();
            if ( message == WM_LBUTTONDOWN && sourceOwnedByXaml )
            {
                m_xamlGestureActive = true;
                m_xamlGesturePointerId = pointerId;
            }
            else if ( m_xamlGestureActive &&
                      pointerId == m_xamlGesturePointerId )
            {
                if ( !anyButtonDown )
                {
                    m_xamlGestureActive = false;
                }
                else if ( message == WM_MOUSEMOVE )
                {
                    // Unless a window took the mouse when the button went
                    // down on what XAML shows of it (see
                    // CancelSlotPointerOwners()): the window which has the
                    // capture gets the moves wherever the pointer is, as in
                    // Win32, and only this route sees them all.
                    const HWND capture = ::GetCapture();
                    const bool capturedByWindow =
                        capture && capture != m_bridge &&
                        capture != m_inner &&
                        !(m_bridge && ::IsChild(m_bridge, capture));
                    if ( !capturedByWindow )
                        return;
                }
            }
        }

        WORD modifiers = 0;
        WORD buttonMask = 0;
        if ( ::GetKeyState(VK_SHIFT) & 0x8000 )
            modifiers |= MK_SHIFT;
        if ( ::GetKeyState(VK_CONTROL) & 0x8000 )
            modifiers |= MK_CONTROL;
        if ( props.IsLeftButtonPressed() )
            buttonMask |= MK_LBUTTON;
        if ( props.IsRightButtonPressed() )
            buttonMask |= MK_RBUTTON;
        if ( props.IsMiddleButtonPressed() )
            buttonMask |= MK_MBUTTON;
        if ( props.IsXButton1Pressed() )
            buttonMask |= MK_XBUTTON1;
        if ( props.IsXButton2Pressed() )
            buttonMask |= MK_XBUTTON2;

        const double scale = GetScale();
        const auto position = point.Position();
        const wxPoint pointPx(
            static_cast<int>(std::lround(position.X * scale)),
            static_cast<int>(std::lround(position.Y * scale)));
        const RECT clientScreen = GetClientScreenRect();
        const POINT screen =
        {
            clientScreen.left + pointPx.x,
            clientScreen.top + pointPx.y
        };

        wxWinUIPointerSample sample;
        sample.device = wxWinUIGetInputDevice(point.PointerDeviceType());
        sample.pointerId = point.PointerId();
        sample.isPrimary = props.IsPrimary();
        sample.isCompatibilityMouse =
            sample.device == wxWinUIInputDevice::Mouse &&
            wxWinUIIsCompatibilityMouseEvent();
        sample.screenX = screen.x;
        sample.screenY = screen.y;
        // Keep the projection's exact root-space position for XAML hit
        // classification. The integer screen coordinates above are still
        // required by the Win32 dispatch contract, but reconstructing DIPs
        // from them would shift fractional-scale boundaries by up to half a
        // physical pixel.
        sample.hasRootDips = true;
        sample.rootXDIP = position.X;
        sample.rootYDIP = position.Y;
        sample.timestamp = point.Timestamp() / 1000;
        sample.modifiers = modifiers;

        if ( message == WM_MOUSEWHEEL )
        {
            sample.kind = wxWinUIInputKind::Wheel;
            sample.wheelDelta = props.MouseWheelDelta();
            sample.horizontalWheel = props.IsHorizontalMouseWheel();
        }
        else
        {
            const wxWinUIInputMapping mapping =
                wxWinUIMapPointerUpdate(wxWinUIGetInputUpdateKind(
                    props.PointerUpdateKind()));
            if ( mapping.valid )
            {
                sample.kind = mapping.kind;
                sample.button = mapping.button;
            }
            else if ( message == WM_MOUSEMOVE )
            {
                sample.kind = wxWinUIInputKind::Move;
            }
            else if ( sample.device != wxWinUIInputDevice::Mouse )
            {
                // V0 policy: one primary touch/pen contact is represented
                // exactly once as a mouse-compatible left-button gesture.
                sample.kind = message == WM_LBUTTONDOWN
                    ? wxWinUIInputKind::Press
                    : wxWinUIInputKind::Release;
                sample.button = wxWinUIInputButton::Left;
            }
            else
            {
                return;
            }
        }

        if ( sample.kind == wxWinUIInputKind::Press )
            buttonMask |= wxWinUIGetWParamButtonMask(sample.button);
        else if ( sample.kind == wxWinUIInputKind::Release )
            buttonMask &= ~wxWinUIGetWParamButtonMask(sample.button);
        sample.buttonMask = buttonMask;

        if ( !hostState || hostState->GetHost() != this ||
             m_shuttingDown || m_inputTransitionEpoch != entryEpoch )
        {
            return;
        }
        // Do not supersede the original route or touch its capture/hover
        // state for a synchronous reflection of the packet it is delivering.
        if ( wxWinUIForwardedPointerDispatch::IsEcho(
                hostState, sample, entryEpoch) )
        {
            args.Handled(true);
            return;
        }

        if ( wxWinUIInputQueueEnabled() )
        {
            if ( !sourceAlreadyHandled )
            {
                // Over a window which is not shown by XAML: the message
                // loop delivers it, see QueueRootPointerSample().
                if ( QueueRootPointerSample(sample) )
                {
                    wxWinUIProfileSample();
                    return;
                }
            }
            else
            {
                // What XAML owns: the root only takes note of it, telling
                // the window the pointer has left and asking the control
                // under it for its cursor. That waits for the loop as well,
                // unless a button is held on a window which is not shown by
                // XAML or such a window has captured the mouse: the route
                // may then ask for the event to be kept from XAML, which
                // only now can do. The capture XAML takes for its own
                // controls (a button being pressed) is not that.
                const HWND capture = ::GetCapture();
                const bool capturedByWindow =
                    capture && capture != m_bridge && capture != m_inner &&
                    !(m_bridge && ::IsChild(m_bridge, capture));
                if ( wxWinUIInputQueueMode() == 2 &&
                     !m_inputState.HasAnyActivePress() && !capturedByWindow )
                {
                    const std::shared_ptr<wxWinUISlotLifetime> cursorSlot =
                        sourceOwnedByXaml ? FindXamlPointerCursorSlot(args)
                                          : nullptr;
                    if ( QueueXamlPointerSample(sample, cursorSlot,
                                                sourceOwnedByXaml) )
                    {
                        wxWinUIProfileSample();
                        return;
                    }
                }

                if ( !m_rootPointerQueue.empty() )
                {
                    // Handled here and now, but after what was waiting:
                    // the window the pointer has just left gets its last
                    // move before it is told the pointer is gone.
                    DrainRootPointerQueue();
                    if ( !hostState || hostState->GetHost() != this ||
                         m_shuttingDown )
                    {
                        return;
                    }
                }
            }
        }

        if ( ++m_inputTransitionEpoch == 0 )
            ++m_inputTransitionEpoch;
        const unsigned long long eventEpoch = m_inputTransitionEpoch;
        const auto retryIslandReset = wxMakeGuard(
            [this]()
            {
                RetryDeferredIslandCancelMode();
            });
        if ( sourceOwnedByXaml && message == WM_MOUSEMOVE )
        {
            const wxWinUIProfileScope profileObserve(
                wxWinUIProfilePhase::Cursor);
            ObserveXamlPointerCursor();
        }

        const wxWinUIRootPointerOutcome outcome =
            RouteRootPointerSampleAtEpoch(
                sample, sourceAlreadyHandled, eventEpoch);
        if ( sourceOwnedByXaml &&
             hostState && hostState->GetHost() == this &&
             !m_shuttingDown &&
             m_inputTransitionEpoch == eventEpoch )
        {
            // Apply this after the route: a transition from native space
            // first executes its balancing HoverLeave, whose cleanup resets
            // the old native cursor. A Move computes one position-sensitive
            // wxEVT_SET_CURSOR verdict; Press/Release/Wheel only re-assert
            // that generation-bound verdict, because XAML/capture can reset
            // InputPointerSource while the pointer has not moved.
            const wxWinUIProfileScope profileCursor(
                wxWinUIProfilePhase::Cursor);
            ApplyXamlPointerCursor(
                args,
                wxWinUIShouldEmitSetCursor(sample.kind),
                eventEpoch);
        }
        else if ( !sourceOwnedByXaml &&
                  sample.kind != wxWinUIInputKind::Move &&
                  hostState && hostState->GetHost() == this &&
                  !m_shuttingDown &&
                  m_inputTransitionEpoch == eventEpoch &&
                  m_activeCursorSurface == ActiveCursorSurface::Native )
        {
            // Over native space a move sets the cursor of the window again,
            // a press, a release or a wheel step does not: put back what
            // XAML replaced with its own while processing this one.
            ReassertIslandPointerCursor();
        }
        if ( outcome.cancelSource )
            args.Handled(true);
    }
    catch ( const winrt::hresult_error& )
    {
    }

    wxWinUIProfileSample();
}

#ifdef WXWINUI_TEST_SUPPORT
wxWinUIRootPointerOutcome wxWinUIHostInputTestAccess::RouteRootPointerSample(
    wxWinUITopLevelHost& host,
    const wxWinUIPointerSample& sample,
    bool sourceAlreadyHandled)
{
    wxWinUITopLevelHost * const self = &host;
    wxWinUITopLevelHost::OperationGuard operation(self);
    wxWinUIRootPointerOutcome outcome;
    if ( self->m_shuttingDown || !self->m_tlw )
        return outcome;

    if ( wxWinUIForwardedPointerDispatch::IsEcho(
            self->m_hostLifetime, sample, self->m_inputTransitionEpoch) )
    {
        outcome.status = wxWinUIRootPointerStatus::Completed;
        outcome.disposition = wxWinUIInputDisposition::Routed;
        outcome.cancelSource = true;
        return outcome;
    }

    const auto retryIslandReset = wxMakeGuard(
        [self]()
        {
            self->RetryDeferredIslandCancelMode();
        });

    if ( ++self->m_inputTransitionEpoch == 0 )
        ++self->m_inputTransitionEpoch;
    return self->RouteRootPointerSampleAtEpoch(
        sample, sourceAlreadyHandled, self->m_inputTransitionEpoch);
}
#endif // WXWINUI_TEST_SUPPORT

wxWinUIRootPointerOutcome
wxWinUITopLevelHost::RouteRootPointerSampleAtEpoch(
    const wxWinUIPointerSample& sample,
    bool sourceAlreadyHandled,
    unsigned long long eventEpoch)
{
    wxWinUIRootPointerOutcome outcome;
    const auto resize = m_nativeResize;
    if ( resize && sample.device == resize->sample.device &&
         sample.pointerId == resize->sample.pointerId )
    {
        // The grip's accepted press bubbles to the root after its own
        // callback. It is the same gesture, not a superseding DOWN: do not
        // advance the InputSite generation or route a second owner for it.
        if ( (sample.kind == wxWinUIInputKind::Press &&
              sample.button == wxWinUIInputButton::Left &&
              sample.timestamp == resize->sample.timestamp) ||
             sample.kind == wxWinUIInputKind::Move )
        {
            if ( sample.kind == wxWinUIInputKind::Move )
                wxWinUIQueueNativeSizingMove(m_hostLifetime, sample);
            outcome.status = wxWinUIRootPointerStatus::Completed;
            outcome.disposition = wxWinUIInputDisposition::Routed;
            outcome.cancelSource = true;
            return outcome;
        }
        if ( sample.kind == wxWinUIInputKind::Release &&
             sample.button == wxWinUIInputButton::Left )
        {
            CancelNativeResize(sample);
            outcome.status = wxWinUIRootPointerStatus::Completed;
            outcome.disposition = wxWinUIInputDisposition::Routed;
            outcome.cancelSource = true;
            return outcome;
        }
    }
    const std::shared_ptr<wxWinUIHostLifetime> hostState = m_hostLifetime;
    const auto hostIsLive = [this, &hostState]()
    {
        return hostState &&
               hostState->GetHost() == this &&
               !m_shuttingDown;
    };
    const auto abortStatus =
        [this, &hostIsLive](unsigned long long expectedEpoch)
        {
            return !hostIsLive() ||
                   m_inputTransitionEpoch != expectedEpoch
                ? wxWinUIRootPointerStatus::Superseded
                : wxWinUIRootPointerStatus::FailedClosed;
        };

    const POINT screen = { sample.screenX, sample.screenY };
    const RECT clientScreen = GetClientScreenRect();
    m_lastPointerScreen = screen;
    const auto eventRoot = m_root;
    const auto eventXamlRoot = GetXamlRoot();
    const double eventScale = GetScale();

    // Apply the device policy before any host-wide side effect. A promoted
    // compatibility UP must not cancel a queued real-mouse modal request or
    // release a slot owner.
    if ( sample.device != wxWinUIInputDevice::Mouse &&
         !sample.isPrimary )
    {
        outcome.status = wxWinUIRootPointerStatus::PolicyFiltered;
        outcome.disposition =
            wxWinUIInputDisposition::SecondaryPointer;
        return outcome;
    }
    if ( sample.isCompatibilityMouse )
    {
        outcome.status = wxWinUIRootPointerStatus::PolicyFiltered;
        outcome.disposition =
            wxWinUIInputDisposition::CompatibilityDuplicate;
        return outcome;
    }

    // The press of a button over what XAML shows of a window reaches the
    // slot of that window first, then this route, as the event bubbles.
    // If the window took the mouse when it got the press (a header whose
    // column or edge is dragged), the capture is there by now and the
    // route below would give the same press to the window which has the
    // capture, a second time and as its own: the slot was then kept from
    // sending anything while that press was held (see SendSlotMouseEvent())
    // and the release of the button, consumed for the slot which owns it,
    // never reached the window. It went on dragging with no button held,
    // until the release of a second click balanced the press made here.
    // The slot owns this press: nothing to route.
    if ( sample.kind == wxWinUIInputKind::Press && sourceAlreadyHandled &&
         HasSlotPointerOwner(sample) )
    {
        if ( wxWinUIInputLogEnabled() )
            wxWinUIInputLog("root PRESS owned by a slot: not routed");
        outcome.status = wxWinUIRootPointerStatus::Completed;
        outcome.disposition = wxWinUIInputDisposition::Xaml;
        return outcome;
    }

    if ( sample.kind == wxWinUIInputKind::Press )
        AdvanceInputSiteGeneration();

    wxWinUIInputAction superseded;
    if ( sample.kind == wxWinUIInputKind::Press )
    {
        superseded = TakeModalAwaitingRelease(
            sample.device, sample.pointerId, sample.button);
        if ( !superseded.gestureSerial &&
             m_nativeInputDispatchInFlight &&
             m_nativeInputDispatchCommitted &&
             m_inFlightNativeInput.action.gestureSerial &&
             m_inFlightNativeInput.device == sample.device &&
             m_inFlightNativeInput.pointerId == sample.pointerId &&
             m_inFlightNativeInput.action.button == sample.button )
        {
            // A system-modal loop consumed the old UP and pumped the next
            // DOWN before its SendMessage unwound.
            superseded = m_inFlightNativeInput.action;
        }
    }

    if ( superseded.gestureSerial &&
         m_inputState.HasActiveGesture(superseded) )
    {
        // Retire the exact old modal serial before admitting the
        // corresponding new physical DOWN.
        wxWinUIPointerSample cancel = sample;
        cancel.kind = wxWinUIInputKind::Cancel;
        cancel.button = superseded.button;
        cancel.buttonMask &=
            ~wxWinUIGetWParamButtonMask(superseded.button);
        const wxWinUIInputTransition cancellation =
            m_inputState.CancelActiveGesture(superseded, cancel);

        unsigned long long expectedEpoch = m_inputTransitionEpoch;
        if ( !cancellation.actions.empty() )
        {
            if ( ++expectedEpoch == 0 )
                ++expectedEpoch;
        }

        if ( !ExecuteInputTransition(cancellation) ||
             !hostIsLive() ||
             m_inputTransitionEpoch != expectedEpoch )
        {
            outcome.status = abortStatus(expectedEpoch);
            return outcome;
        }
        eventEpoch = m_inputTransitionEpoch;
    }

    CancelPendingNativeDispatch(sample);
    if ( sample.kind == wxWinUIInputKind::Release &&
         DispatchSlotOwnedRelease(sample) )
    {
        outcome.status = abortStatus(eventEpoch);
        if ( outcome.status ==
                wxWinUIRootPointerStatus::FailedClosed )
        {
            outcome.status =
                wxWinUIRootPointerStatus::SlotReleaseConsumed;
        }
        outcome.disposition = wxWinUIInputDisposition::Routed;
        outcome.cancelSource = true;
        RetryDeferredIslandCancelMode();
        return outcome;
    }

    wxWinUINativeHit nativeHit;
    bool hasNativeHit = false;
    wxWinUIRouteObservation observation;
    const winrt::Windows::Foundation::Point exactRootDips{
        static_cast<float>(sample.rootXDIP),
        static_cast<float>(sample.rootYDIP)
    };
    const winrt::Windows::Foundation::Point * const exactRootDipsPtr =
        sample.hasRootDips ? &exactRootDips : nullptr;
    bool exactRootDipsRemainValid = exactRootDipsPtr != nullptr;
    const auto classifySurface =
        [this, &hostIsLive, &screen, &clientScreen, &eventRoot,
         &eventXamlRoot, eventScale, exactRootDipsPtr,
         &exactRootDipsRemainValid,
         sourceAlreadyHandled,
         &nativeHit, &hasNativeHit, &observation]()
        {
            const RECT currentClientScreen = GetClientScreenRect();
            const wxPoint currentPointPx(
                screen.x - currentClientScreen.left,
                screen.y - currentClientScreen.top);
            const bool exactIsCurrent =
                exactRootDipsRemainValid &&
                m_root == eventRoot &&
                GetXamlRoot() == eventXamlRoot &&
                GetScale() == eventScale &&
                currentClientScreen.left == clientScreen.left &&
                currentClientScreen.top == clientScreen.top &&
                currentClientScreen.right == clientScreen.right &&
                currentClientScreen.bottom == clientScreen.bottom;
            return ClassifyRootPointer(
                       currentPointPx, screen,
                       exactIsCurrent ? exactRootDipsPtr : nullptr,
                       sourceAlreadyHandled,
                       &nativeHit, &observation, &hasNativeHit) &&
                   hostIsLive();
        };

    // Visual classification is deliberately first: neither capture
    // bookkeeping nor the breaker may turn an event over real XAML into a
    // native hit by assumption.
    if ( !classifySurface() ||
         m_inputTransitionEpoch != eventEpoch )
    {
        outcome.status = abortStatus(eventEpoch);
        return outcome;
    }

    bool captureActionsRan = false;
    bool captureSuperseded = false;
    if ( !SyncNativeCapture(
             sample, nullptr, &captureActionsRan, nullptr,
             &captureSuperseded) )
    {
        outcome.status = captureSuperseded || !hostIsLive()
            ? wxWinUIRootPointerStatus::Superseded
            : wxWinUIRootPointerStatus::FailedClosed;
        return outcome;
    }
    eventEpoch = m_inputTransitionEpoch;

    // Capture loss/transfer can synchronously invoke application code.
    // Reclassify rather than continuing with pre-callback geometry.
    if ( captureActionsRan )
    {
        // CaptureChanged is an arbitrary application callback boundary. Even
        // when the XamlRoot object survives, layout/DPI may have advanced
        // without preserving the event-local DIP sample. Reproject the
        // authoritative screen point against the current host.
        exactRootDipsRemainValid = false;
        if ( !classifySurface() ||
             m_inputTransitionEpoch != eventEpoch )
        {
            outcome.status = abortStatus(eventEpoch);
            return outcome;
        }
    }

    if ( sample.kind == wxWinUIInputKind::Press )
        m_inputState.DisarmStormGuard();

    const wxWinUIInputTransition transition =
        m_inputState.Route(sample, observation);
    outcome.disposition = transition.disposition;
    outcome.stormDecision = transition.stormDecision;
    outcome.preparedActionCount =
        static_cast<unsigned>(transition.actions.size());
    outcome.cancelSource = transition.cancelSource;
    for ( const wxWinUIInputAction& action : transition.actions )
    {
        if ( action.action == wxWinUIInputActionKind::Dispatch &&
             (action.kind == wxWinUIInputKind::Press ||
              action.kind == wxWinUIInputKind::DoubleClick ||
              action.kind == wxWinUIInputKind::Release) )
        {
            outcome.pointerDispatch = action;
            break;
        }
    }

    unsigned long long expectedEpoch = m_inputTransitionEpoch;
    if ( !transition.actions.empty() )
    {
        if ( ++expectedEpoch == 0 )
            ++expectedEpoch;
    }

    bool transitionSuperseded = false;
    const long long dispatchStarted = wxWinUIProfileNow();
    const bool executed = ExecuteInputTransition(
             transition,
             hasNativeHit ? &nativeHit : nullptr,
             &transitionSuperseded);
    wxWinUIProfileAdd(wxWinUIProfilePhase::Dispatch, dispatchStarted);
    if ( !executed )
    {
        outcome.status = transitionSuperseded
            ? wxWinUIRootPointerStatus::Superseded
            : abortStatus(expectedEpoch);
        return outcome;
    }

    outcome.status = wxWinUIRootPointerStatus::Completed;
    PruneModalAwaitingReleases();
    RetryDeferredIslandCancelMode();

    if ( wxWinUIInputLogEnabled() )
    {
        static unsigned s_moveCount = 0;
        if ( sample.kind != wxWinUIInputKind::Move ||
             (++s_moveCount % 8) == 0 )
        {
            wxWinUIInputLog(
                "root kind=%d surface=%d pt=(%ld,%ld) actions=%u",
                static_cast<int>(sample.kind),
                static_cast<int>(observation.surface),
                screen.x, screen.y,
                static_cast<unsigned>(transition.actions.size()));
        }
    }
    return outcome;
}

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHostInputTestAccess::HandleRootPointerInterrupted(
    wxWinUITopLevelHost& host,
    bool captureLost,
    const wxWinUIPointerSample& sample)
{
    wxWinUITopLevelHost * const self = &host;
    self->HandleRootPointerInterrupted(captureLost, sample);
}

bool wxWinUIHostInputTestAccess::RouteOwnedSlotRelease(
    wxWinUITopLevelHost& host,
    const wxWinUIPointerSample& sample)
{
    wxWinUITopLevelHost * const self = &host;
    return self->DispatchSlotOwnedRelease(sample);
}

bool wxWinUIHostInputTestAccess::RememberModalAwaitingRelease(
    wxWinUITopLevelHost& host,
    const wxWinUIInputAction& action)
{
    wxWinUITopLevelHost * const self = &host;
    return self->RememberModalAwaitingRelease(action);
}

wxWinUIInputAction wxWinUIHostInputTestAccess::TakeModalAwaitingRelease(
    wxWinUITopLevelHost& host,
    wxWinUIInputDevice device,
    std::uint32_t pointerId,
    wxWinUIInputButton button)
{
    wxWinUITopLevelHost * const self = &host;
    return self->TakeModalAwaitingRelease(device, pointerId, button);
}

void wxWinUIHostInputTestAccess::CancelSlotPointerOwners(
    wxWinUITopLevelHost& host,
    const wxWinUIPointerSample& sample,
    bool allPointers)
{
    wxWinUITopLevelHost * const self = &host;
    self->CancelSlotPointerOwners(sample, allPointers);
}

std::uint32_t wxWinUIHostInputTestAccess::GetInputSiteGeneration(
    const wxWinUITopLevelHost& host)
{
    const wxWinUITopLevelHost * const self = &host;
    return self->m_inputSiteGeneration;
}

bool wxWinUIHostInputTestAccess::CanPostIslandCancelMode(
    const wxWinUITopLevelHost& host,
    std::uint32_t expectedGeneration)
{
    const wxWinUITopLevelHost * const self = &host;
    return self->CanPostIslandCancelMode(expectedGeneration);
}

void wxWinUIHostInputTestAccess::PostIslandCancelMode(
    wxWinUITopLevelHost& host,
    std::uint32_t expectedGeneration)
{
    wxWinUITopLevelHost * const self = &host;
    self->PostIslandCancelMode(expectedGeneration);
}

bool wxWinUIHostInputTestAccess::HasDeferredIslandCancel(
    const wxWinUITopLevelHost& host)
{
    const wxWinUITopLevelHost * const self = &host;
    return self->m_islandCancelDeferred;
}

unsigned wxWinUIHostInputTestAccess::GetPostedIslandCancelCount(
    const wxWinUITopLevelHost& host)
{
    const wxWinUITopLevelHost * const self = &host;
    return self->m_postedIslandCancelMessages;
}
#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_WINUI3
